import os
import sys
import platform
import subprocess
import re

from setuptools import Extension, setup, find_packages, Command
from setuptools.command.build_ext import build_ext

from pathlib import Path

extra_link_args = []
extra_compile_args = []
runtime_library_dirs = []
grapapy_version = "0.1.52"
is_aws = False
is_apple = False
from_os = ''
is_arm = platform.machine().lower() in ["aarch64", "arm64"]

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}

# 'freebsd'
# 'aix'
# 'cygwin'

if sys.platform.startswith('win32'):
    so_ext = '.lib'
    lib_filename = 'grapa_static' + so_ext
    lib_pathfile = 'grapa-lib/win-amd64/' + lib_filename
    extra_compile_args = ['/DUTF8PROC_STATIC', '/DPCRE2_STATIC']
    extra_link_args = ['/MANIFEST:NO']
    
    # Add UCRT include path for io.h and other Windows SDK headers
    import glob
    windows_sdk_paths = glob.glob("C:/Program Files (x86)/Windows Kits/10/Include/*")
    if windows_sdk_paths:
        # Use the latest SDK version
        latest_sdk = max(windows_sdk_paths, key=lambda x: x.split('\\')[-1])
        ucrt_path = os.path.join(latest_sdk, "ucrt")
        shared_path = os.path.join(latest_sdk, "shared")
        um_path = os.path.join(latest_sdk, "um")
        
        # Create a custom include_dirs list to ensure Windows SDK paths come first
        include_dirs = []
        if os.path.exists(ucrt_path):
            include_dirs.append(ucrt_path)
            print(f"Added UCRT include path: {ucrt_path}")
        if os.path.exists(shared_path):
            include_dirs.append(shared_path)
            print(f"Added shared include path: {shared_path}")
        if os.path.exists(um_path):
            include_dirs.append(um_path)
            print(f"Added um include path: {um_path}")
        
        # Also add library paths for linking
        lib_path = latest_sdk.replace("Include", "Lib")
        ucrt_lib_path = os.path.join(lib_path, "ucrt", "x64")
        um_lib_path = os.path.join(lib_path, "um", "x64")
        
        if os.path.exists(ucrt_lib_path):
            extra_link_args.insert(0, f'/LIBPATH:"{ucrt_lib_path}"')
            print(f"Added UCRT library path: {ucrt_lib_path}")
        if os.path.exists(um_lib_path):
            extra_link_args.insert(0, f'/LIBPATH:"{um_lib_path}"')
            print(f"Added UM library path: {um_lib_path}")
if sys.platform.startswith('linux'):
    from_os = 'linux-amd64'
    temp_result = subprocess.run(["cat", "/etc/os-release"])
    process = subprocess.Popen(['cat', '/etc/os-release'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr.decode()=='':
        stdouts = stdout.decode()
        if stdouts.find("Amazon Linux")>=0:
            is_aws = True
    if is_aws:
        from_os = 'aws-amd64'
        if is_arm:
            from_os = 'aws-arm64'
        extra_link_args = ['-std=c++17','-lX11','-lXfixes','-lXft','-lXext','-lXrender','-lXinerama','-lXcursor','-lxcb','-lXau','-lpng','-lfontconfig','-lfreetype','-O3','-pthread','-ldl','-lm']
    else:
        from_os = 'linux-amd64'
        if is_arm:
            from_os = 'linux-arm64'
        extra_link_args = ['-std=c++17','-lX11','-lXfixes','-lXft','-lXext','-lXrender','-lXinerama','-lXcursor','-lxcb','-lXau','-lpng','-lfontconfig','-lfreetype','-O3','-pthread','-ldl','-lm']
    so_ext = '.so'
    lib_filename = 'libgrapa' + so_ext
    lib_pathfile = 'grapa-lib/' + from_os + '/' + lib_filename
    runtime_library_dirs = ['$ORIGIN/grapapy-' + grapapy_version]
elif sys.platform.startswith('darwin'):
    if is_arm:
        is_apple = True
        from_os = 'mac-arm64'
    else:
        raise RuntimeError("mac-amd64 is no longer supported. Please use an Apple Silicon Mac (arm64).")
    extra_link_args = [
        '-Wl,-rpath,@loader_path',
        '-std=c++17','-stdlib=libc++',
        '-O3','-pthread','-fPIC',
        '-framework','CoreFoundation','-framework','AppKit','-framework','IOKit','-framework','ScreenCaptureKit','-O3','-pthread'
        ]
    extra_compile_args = [
        '-std=c++17',
        '-O3','-pthread','-fPIC',
        ]
    so_ext = '.so'
    lib_filename = 'libgrapa' + so_ext
    lib_pathfile = 'grapa-lib/' + from_os + '/' + lib_filename

# Set up include_dirs for Windows SDK paths
if sys.platform.startswith('win32') and 'include_dirs' in locals():
    # Use custom include_dirs that puts Windows SDK paths first
    base_include_dirs = ["source","source/utf8proc",'source/pybind11/include']
    include_dirs = include_dirs + base_include_dirs
else:
    include_dirs = ["source","source/utf8proc",'source/pybind11/include']

# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # required for auto-detection & inclusion of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        cfg = "Debug" if debug else "Release"

        # CMake lets you override the generator - we need to check this.
        # Can be set with Conda-Build, for example.
        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
        ]
        build_args = []
        # Adding CMake arguments set as environment variable
        # (needed e.g. to build for ARM OSx on conda-forge)
        if "CMAKE_ARGS" in os.environ:
            cmake_args += [item for item in os.environ["CMAKE_ARGS"].split(" ") if item]

        # In this example, we pass in the version to C++. You might not need to.
        #cmake_args += [f"-DEXAMPLE_VERSION_INFO={self.distribution.get_version()}"]
        cmake_args += [f"-DGRAPAPY_OS={from_os}"]

        if self.compiler.compiler_type != "msvc":
            # Using Ninja-build since it a) is available as a wheel and b)
            # multithreads automatically. MSVC would require all variables be
            # exported for Ninja to pick it up, which is a little tricky to do.
            # Users can override the generator with CMAKE_GENERATOR in
            # 3.15+.
            if not cmake_generator or cmake_generator == "Ninja":
                try:
                    import ninja  # noqa: F401

                    ninja_executable_path = os.path.join(ninja.BIN_DIR, "ninja")
                    cmake_args += [
                        "-GNinja",
                        f"-DCMAKE_MAKE_PROGRAM:FILEPATH={ninja_executable_path}",
                    ]
                except ImportError:
                    pass

        else:

            # Single config generators are handled "normally"
            single_config = any(x in cmake_generator for x in {"NMake", "Ninja"})

            # CMake allows an arch-in-generator style for backward compatibility
            contains_arch = any(x in cmake_generator for x in {"ARM", "Win64"})

            # Specify the arch if using MSVC generator, but only if it doesn't
            # contain a backward-compatibility arch spec already in the
            # generator name.
            if not single_config and not contains_arch:
                cmake_args += ["-A", PLAT_TO_CMAKE[self.plat_name]]

            # Multi-config generators have a different way to specify configs
            if not single_config:
                cmake_args += [
                    f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={extdir}"
                ]
                build_args += ["--config", cfg]

        if sys.platform.startswith('win32'):
            cmake_args += ['-DUTF8PROC_STATIC=ON', '-DPCRE2_STATIC=ON']
        
        if sys.platform.startswith("darwin"):
            # Cross-compile support for macOS - respect ARCHFLAGS if set
            archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
            if archs:
                cmake_args += ["-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))]
            #cmake_args += ['-Wl,-rpath,@loader/grapapy']

        if sys.platform.startswith('linux') or sys.platform.startswith('aws'):
            #cmake_args += ['-Wl,-rpath,$ORIGIN']
            build_args += ['-Wl,-rpath,${ORIGIN}']
            destPath = os.path.join(extdir, 'grapapy')
            self.mkpath(destPath)
            # sourcePath = os.path.join(ext.sourcedir, 'source','X11-lib', from_os)
                        # for file_name in os.listdir(sourcePath):
                        #     self.copy_file(os.path.join(sourcePath, file_name), os.path.join(destPath, file_name))
        
        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
        # across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            # self.parallel is a Python 3 only way to set parallel jobs by hand
            # using -j in the build_ext call, not supported by pip or PyPA-build.
            if hasattr(self, "parallel") and self.parallel:
                # CMake 3.12+ only.
                build_args += [f"-j{self.parallel}"]

        build_temp = os.path.join(self.build_temp, ext.name)
        if not os.path.exists(build_temp):
            os.makedirs(build_temp)

        subprocess.check_call(["cmake", ext.sourcedir] + cmake_args, cwd=build_temp)
        subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=build_temp)

# All platforms now use CMake build system
setup(
    name="grapapy",
    version=grapapy_version,
    author="Chris Matichuk",
    description="Grammar-based language and parser with unlimited precision, parallelism, and Python integration",
    long_description=(
        "Grapa is a modern grammar-based programming language and parser designed for Python users, educators, "
        "researchers, and data engineers. It features unlimited precision math, runtime-mutable grammars, true parallelism, "
        "and seamless Python integration via GrapaPy. Rapidly prototype languages, process data, and experiment with advanced grammars—all with a unified, high-performance toolset.\n\n"
        "For comprehensive documentation, visit: https://grapa-dev.github.io/grapa/\n\n"
        "See the [Grapa project on GitHub](https://github.com/grapa-dev/grapa) for full documentation, examples, and installation instructions."
    ),
    long_description_content_type="text/markdown",
    url="https://grapa-dev.github.io/grapa/",
    ext_modules=[CMakeExtension("grapapy")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    python_requires=">=3.6",
)

