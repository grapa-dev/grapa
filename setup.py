import os
import sys
import platform
import subprocess
import re

from distutils.command.build import build

from setuptools import Extension, setup, find_packages, Command
from setuptools.command.build_ext import build_ext

from pathlib import Path

extra_link_args = []
extra_compile_args = []
runtime_library_dirs = []
grapapy_version = "0.0.25"
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
    lib_filename = 'grapa' + so_ext
    lib_pathfile = 'grapa-lib/win-amd64/' + lib_filename
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
    from_os = 'mac-amd64'
    if is_arm:
        is_apple = True
        from_os = 'mac-arm64'
    extra_link_args = [
        '-Wl,-rpath,@loader_path',
        '-std=c++17','-stdlib=libc++',
        '-O3','-pthread','-fPIC',
        '-framework','CoreFoundation','-framework','AppKit','-framework','IOKit','-O3','-pthread'
        ]
    extra_compile_args = [
        '-std=c++17',
        '-O3','-pthread','-fPIC',
        ]
    so_ext = '.so'
    lib_filename = 'libgrapa' + so_ext
    lib_pathfile = 'grapa-lib/' + from_os + '/' + lib_filename

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
            # Users can override the generator with CMAKE_GENERATOR in CMake
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


class CopySharedLibrary(Command):
    user_options = []

    def initialize_options(self):
        self.build_lib = None
        self.inplace = 0
        self.build_dir = "source"
        self.filename = lib_filename
        self.lib_source_path = os.path.join(self.build_dir, lib_pathfile)
        self.package_name = 'source'

    def finalize_options(self):
        self.set_undefined_options('build', ('build_lib', 'build_lib'), )
        self.set_undefined_options('build_ext', ('inplace', 'inplace'), )

    def run(self) -> None:
        self.inplace = self.get_finalized_command('build_ext').inplace
        if self.inplace:
            lib_target_path = self.package_name
        else:
            lib_target_path = os.path.join(self.build_lib, "grapapy-"+grapapy_version)
            self.mkpath(lib_target_path)
        self.copy_file(self.lib_source_path, os.path.join(lib_target_path, self.filename))
        if sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
            for file_name in os.listdir(os.path.join(self.build_dir, 'grapa-lib/'+from_os)):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'grapa-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
            for file_name in os.listdir(os.path.join(self.build_dir, 'openssl-lib/'+from_os)):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'openssl-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
            for file_name in os.listdir(os.path.join(self.build_dir, 'blst-lib/'+from_os)):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'blst-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
            for file_name in os.listdir(os.path.join(self.build_dir, 'fl-lib/'+from_os)):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'fl-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
                        # for file_name in os.listdir(os.path.join(self.build_dir, 'X11-lib/'+from_os)):
                         #    self.copy_file(os.path.join(os.path.join(self.build_dir, 'X11-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
        if sys.platform.startswith('linux'):
            os.environ["ORIGIN"] = os.path.abspath(lib_target_path)


class CustomBuild(build):
    sub_commands = [
        ('build_clib', build.has_c_libraries),
        ('build_ext', build.has_ext_modules),
        ('build_py', build.has_pure_modules),
        ('build_scripts', build.has_scripts),
    ]


class CustomBuildExt(build_ext):

    def run(self):
        self.run_command('copy_grapalib')
        super().run()

def pick_library_dirs():
    my_system = platform.system()
    if my_system == 'Linux':
        if is_aws:
            if is_arm:
                return ["source", "source/grapa-lib/aws-arm64", "source/X11-lib/aws-arm64"]
            else:
                return ["source", "source/grapa-lib/aws-amd64", "source/X11-lib/aws-amd64"]
        else:
            if is_arm:
                return ["source", "source/grapa-lib/linux-arm64", "source/X11-lib/linux-arm64"]
            else:
                return ["source", "source/grapa-lib/linux-amd64", "source/X11-lib/linux-amd64"]
    if my_system == 'Darwin':
        if is_arm:
            return ["source", "source/grapa-lib/mac-arm64"]
        else:
            return ["source", "source/grapa-lib/mac-amd64"]
    if my_system == 'Windows':
        return ["source", "source/grapa-lib/win-amd64"]
    raise ValueError("Unknown platform: " + my_system)

def pick_libraries():
    my_system = platform.system()
    if my_system == 'Linux':
        return ['grapa']
    if my_system == 'Darwin':
        #return ['@rpath/grapa']
        return ['source/grapa-lib/libgrapa.a']
    if my_system == 'Windows':
        return ["grapa","Gdi32","Advapi32","User32","Ole32","Shell32","Comdlg32"]
    raise ValueError("Unknown platform: " + my_system)

lib_grapa = Extension(
    'grapapy', 
    sources = [
        'source/mainpy.cpp',
    ],
    include_dirs=["source",'source/pybind11/include'],
    library_dirs=pick_library_dirs(),
    libraries=pick_libraries(),
    runtime_library_dirs=runtime_library_dirs,
    extra_link_args=extra_link_args,
    extra_compile_args=extra_compile_args,
)

# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
if sys.platform.startswith('linux') or sys.platform.startswith('win32'):
    setup(
        name="grapapy",
        version=grapapy_version,
        author="Chris Matichuk",
        author_email="matichuk@hotmail.com",
        description="grammar parser language",
        long_description="https://github.com/grapa-dev/grapa/blob/master/docs/PYTHON.md",
        ext_modules=[lib_grapa],
        cmdclass={
            'copy_grapalib': CopySharedLibrary,
            'build_ext': CustomBuildExt,
            'build': CustomBuild,
        },
        zip_safe=False,
        python_requires=">=3.6",
        packages=find_packages(),
    )
else:
    setup(
        name="grapapy",
        version=grapapy_version,
        author="Chris Matichuk",
        description="grammar parser language",
        long_description="https://github.com/grapa-dev/grapa/blob/master/docs/PYTHON.md",
        ext_modules=[CMakeExtension("grapapy")],
        cmdclass={"build_ext": CMakeBuild},
        zip_safe=False,
        python_requires=">=3.6",
    )

