import os
import sys
import platform
import subprocess
import re

from setuptools import Extension, setup, find_packages, Command
from setuptools.command.build_ext import build_ext

from pathlib import Path

def run_diagnostic_cross_compile(cmd: list, log_filename: str = "build-debug.log"):
    """Run the cross-compile command with full output to log"""
    from pathlib import Path
    import subprocess

    log_path = Path(log_filename)
    with log_path.open("w") as f:
        f.write("Running command:\n")
        f.write(" ".join(cmd) + "\n\n")
        try:
            result = subprocess.run(cmd, stdout=f, stderr=subprocess.STDOUT)
            if result.returncode != 0:
                f.write(f"\nCommand failed with exit code {result.returncode}\n")
                raise RuntimeError(f"Build failed with exit code {result.returncode}")
        except Exception as e:
            f.write(f"\nException occurred: {e}\n")
            raise

# Get pybind11 include path
def get_pybind11_include():
    try:
        import pybind11
        return pybind11.get_include()
    except ImportError:
        return None

extra_link_args = []
extra_compile_args = []
runtime_library_dirs = []
grapapy_version = "0.0.277"

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
    extra_compile_args = ['/DUTF8PROC_STATIC', '/DPCRE2_STATIC', '/D_CRT_SECURE_NO_WARNINGS', '/D_LIB']
    extra_link_args = ['/MANIFEST:NO']
    # Add Windows SDK include path to fix io.h dependency issue
    import os
    import glob
    windows_sdk_path = None
    
    # Try multiple methods to find Windows SDK
    sdk_paths = []
    
    # Method 1: Check common Windows SDK locations
    program_files_paths = [
        os.environ.get('ProgramFiles(x86)', 'C:\\Program Files (x86)'),
        os.environ.get('ProgramFiles', 'C:\\Program Files')
    ]
    
    for program_files in program_files_paths:
        if os.path.exists(program_files):
            sdk_base = os.path.join(program_files, 'Windows Kits', '10')
            if os.path.exists(sdk_base):
                # Look for Include directories
                include_dir = os.path.join(sdk_base, 'Include')
                if os.path.exists(include_dir):
                    # Find the highest version number
                    try:
                        versions = [d for d in os.listdir(include_dir) 
                                  if os.path.isdir(os.path.join(include_dir, d)) and d.replace('.', '').isdigit()]
                        if versions:
                            # Sort by version number (highest first)
                            versions.sort(key=lambda x: [int(i) for i in x.split('.')], reverse=True)
                            sdk_paths.append(os.path.join(include_dir, versions[0]))
                    except (OSError, ValueError):
                        pass
    
    # Method 2: Use Windows SDK registry or environment variables
    try:
        import winreg
        # Try to get SDK path from registry
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, 
                           r"SOFTWARE\Microsoft\Windows Kits\Installed Roots") as key:
            sdk_root = winreg.QueryValueEx(key, "KitsRoot10")[0]
            if os.path.exists(sdk_root):
                include_dir = os.path.join(sdk_root, 'Include')
                if os.path.exists(include_dir):
                    # Find the highest version
                    try:
                        versions = [d for d in os.listdir(include_dir) 
                                  if os.path.isdir(os.path.join(include_dir, d)) and d.replace('.', '').isdigit()]
                        if versions:
                            versions.sort(key=lambda x: [int(i) for i in x.split('.')], reverse=True)
                            sdk_paths.append(os.path.join(include_dir, versions[0]))
                    except (OSError, ValueError):
                        pass
    except (ImportError, OSError, FileNotFoundError):
        pass
    
    # Method 3: Use Visual Studio's vcvars to get SDK path
    try:
        import subprocess
        # Try to get SDK path from Visual Studio environment
        result = subprocess.run(['where', 'cl'], capture_output=True, text=True, shell=True)
        if result.returncode == 0:
            # Extract SDK path from cl.exe location
            cl_path = result.stdout.strip().split('\n')[0]
            if 'Microsoft Visual Studio' in cl_path:
                # Navigate up to find SDK
                vs_path = os.path.dirname(os.path.dirname(os.path.dirname(cl_path)))
                sdk_base = os.path.join(vs_path, '..', '..', '..', 'Windows Kits', '10', 'Include')
                if os.path.exists(sdk_base):
                    try:
                        versions = [d for d in os.listdir(sdk_base) 
                                  if os.path.isdir(os.path.join(sdk_base, d)) and d.replace('.', '').isdigit()]
                        if versions:
                            versions.sort(key=lambda x: [int(i) for i in x.split('.')], reverse=True)
                            sdk_paths.append(os.path.join(sdk_base, versions[0]))
                    except (OSError, ValueError):
                        pass
    except (subprocess.SubprocessError, OSError):
        pass
    
    # Use the first found SDK path
    if sdk_paths:
        windows_sdk_path = sdk_paths[0]
        print(f"Found Windows SDK at: {windows_sdk_path}")
    else:
        print("Warning: Windows SDK not found, build may fail")
if sys.platform.startswith('linux'):
    from_os = 'linux-amd64'
    if is_arm:
        from_os = 'linux-arm64'
    # For Python extension, we don't need X11 GUI libraries
    extra_link_args = ['-std=c++17','-O3','-pthread','-ldl','-lm']
    so_ext = '.a'  # All platforms use static libraries (.a) for Extension-based approach
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
    so_ext = '.a'  # macOS uses static libraries (.a), not shared libraries (.so)
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
        try:
            subprocess.check_call(["cmake", "--build", ".", "--verbose"] + build_args, cwd=build_temp)
        except subprocess.CalledProcessError as e:
            print(f"CMake build failed with exit code {e.returncode}")
            print("Attempting to get more detailed error information...")
            # Try to get more detailed output
            result = subprocess.run(["cmake", "--build", ".", "--verbose"] + build_args, 
                                  cwd=build_temp, capture_output=True, text=True)
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            raise


class CopySharedLibrary(Command):
    user_options = []

    def initialize_options(self):
        self.build_lib = None
        self.inplace = 0
        self.build_dir = "source"
        self.filename = lib_filename
        self.lib_source_path = os.path.join(self.build_dir, lib_pathfile)
        self.package_name = 'grapapy'

    def finalize_options(self):
        self.set_undefined_options('build', ('build_lib', 'build_lib'), )
        self.set_undefined_options('build_ext', ('inplace', 'inplace'), )

    def run(self) -> None:
        self.inplace = self.get_finalized_command('build_ext').inplace
        if self.inplace:
            lib_target_path = self.package_name
            self.mkpath(lib_target_path)
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
            for file_name in os.listdir(os.path.join(self.build_dir, 'pcre2-lib/'+from_os)):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'pcre2-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
           # for file_name in os.listdir(os.path.join(self.build_dir, 'X11-lib/'+from_os)):
           #    self.copy_file(os.path.join(os.path.join(self.build_dir, 'X11-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
        elif sys.platform.startswith('win32'):
            # Windows: copy all required libraries
            for file_name in os.listdir(os.path.join(self.build_dir, 'grapa-lib/win-amd64')):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'grapa-lib/win-amd64'),file_name), os.path.join(lib_target_path, file_name))
            for file_name in os.listdir(os.path.join(self.build_dir, 'openssl-lib/win-amd64')):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'openssl-lib/win-amd64'),file_name), os.path.join(lib_target_path, file_name))
            for file_name in os.listdir(os.path.join(self.build_dir, 'blst-lib/win-amd64')):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'blst-lib/win-amd64'),file_name), os.path.join(lib_target_path, file_name))
            for file_name in os.listdir(os.path.join(self.build_dir, 'fl-lib/win-amd64')):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'fl-lib/win-amd64'),file_name), os.path.join(lib_target_path, file_name))
            for file_name in os.listdir(os.path.join(self.build_dir, 'pcre2-lib/win-amd64')):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'pcre2-lib/win-amd64'),file_name), os.path.join(lib_target_path, file_name))
            print(f"Copied Windows libraries to distribution")
        if sys.platform.startswith('linux'):
            os.environ["ORIGIN"] = os.path.abspath(lib_target_path)


# CustomBuild class removed - using default build behavior


class CustomBuildExt(build_ext):

    def run(self):
        self.run_command('copy_grapalib')
        super().run()
        
    def build_extension(self, ext):
        # Use a local build directory to avoid permission issues
        if sys.platform.startswith('win32'):
            self.build_temp = os.path.join(os.getcwd(), 'build_temp')
            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)
        
        # On Windows, utf8proc is already compiled into the library
        # On other platforms, use pre-compiled objects if available
        if not sys.platform.startswith('win32'):
            print("Using pre-compiled utf8proc...")
            try:
                if sys.platform.startswith('darwin'):
                    # macOS: use pre-compiled utf8proc.o
                    utf8proc_obj = 'utf8proc.o'
                    if os.path.exists(utf8proc_obj):
                        ext.extra_objects = [utf8proc_obj]
                        print(f"Using pre-compiled {utf8proc_obj}")
                    else:
                        print(f"Warning: {utf8proc_obj} not found, will compile at runtime")
                        # Fallback to runtime compilation
                        subprocess.run([
                            "clang", "-Isource", "-DUTF8PROC_STATIC", "-c",
                            "source/utf8proc/utf8proc.c", "-O3"
                        ], check=True)
                        if os.path.exists('utf8proc.o'):
                            ext.extra_objects = ['utf8proc.o']
                elif sys.platform.startswith('linux'):
                    # Linux: use pre-compiled utf8proc.o
                    utf8proc_obj = 'utf8proc.o'
                    if os.path.exists(utf8proc_obj):
                        ext.extra_objects = [utf8proc_obj]
                        print(f"Using pre-compiled {utf8proc_obj}")
                    else:
                        print(f"Warning: {utf8proc_obj} not found, will compile at runtime")
                        # Fallback to runtime compilation
                        subprocess.run([
                            "gcc", "-Isource", "-DUTF8PROC_STATIC", "-c",
                            "source/utf8proc/utf8proc.c", "-O3"
                        ], check=True)
                        if os.path.exists('utf8proc.o'):
                            ext.extra_objects = ['utf8proc.o']
                print("utf8proc handling completed")
            except subprocess.CalledProcessError as e:
                print(f"Failed to handle utf8proc: {e}")
                raise
        else:
            # On Windows, try to avoid the io.h issue by using different compiler flags
            print("Windows: Using library-based approach (utf8proc already in library)")
            # Add compiler flags to avoid io.h dependency
            if hasattr(ext, 'extra_compile_args'):
                ext.extra_compile_args.extend(['/D_CRT_SECURE_NO_WARNINGS'])
            else:
                ext.extra_compile_args = ['/D_CRT_SECURE_NO_WARNINGS']
        
        try:
            super().build_extension(ext)
        except PermissionError as e:
            if "cache" in str(e).lower():
                print("\n" + "="*60)
                print("PERMISSION ERROR: Pip cannot write to its cache directory.")
                print("SOLUTION: Use one of these commands:")
                print("  pip install --no-cache-dir dist/grapapy-0.0.100.tar.gz")
                print("  pip install --no-cache-dir -e .")
                print("="*60)
            raise

def pick_library_dirs():
    my_system = platform.system()
    if my_system == 'Linux':
        if is_arm:
            return ["source", "source/grapa-lib/linux-arm64", "source/blst-lib/linux-arm64", "source/fl-lib/linux-arm64", "source/openssl-lib/linux-arm64", "source/pcre2-lib/linux-arm64"]
        else:
            return ["source", "source/grapa-lib/linux-amd64", "source/blst-lib/linux-amd64", "source/fl-lib/linux-amd64", "source/openssl-lib/linux-amd64", "source/pcre2-lib/linux-amd64"]
    if my_system == 'Darwin':
        if is_arm:
            return ["source", "source/grapa-lib/mac-arm64", "source/blst-lib/mac-arm64", "source/fl-lib/mac-arm64", "source/openssl-lib/mac-arm64", "source/pcre2-lib/mac-arm64"]
        else:
            return ["source", "source/grapa-lib/mac-amd64", "source/blst-lib/mac-amd64", "source/fl-lib/mac-amd64", "source/openssl-lib/mac-amd64", "source/pcre2-lib/mac-amd64"]
    if my_system == 'Windows':
        # Add Windows SDK library path if available
        library_dirs = ["source", "source/grapa-lib/win-amd64"]
        if 'windows_sdk_path' in locals() and windows_sdk_path:
            # Add Windows SDK library path for system libraries like Gdi32.lib
            # Convert Include path to Lib path
            lib_path = windows_sdk_path.replace('Include', 'Lib')
            library_dirs.extend([
                f"{lib_path}\\um\\x64",
                f"{lib_path}\\ucrt\\x64"
            ])
        return library_dirs
    raise ValueError("Unknown platform: " + my_system)

def pick_libraries():
    my_system = platform.system()
    if my_system == 'Linux':
        return ['grapa', 'blst', 'fltk', 'fltk_forms', 'fltk_gl', 'fltk_images', 'fltk_jpeg', 'crypto', 'ssl', 'pcre2-8']
    if my_system == 'Darwin':
        #return ['@rpath/grapa']
        return ['grapa', 'blst', 'fltk', 'fltk_forms', 'fltk_gl', 'fltk_images', 'fltk_jpeg', 'fltk_png', 'fltk_z', 'crypto', 'ssl', 'pcre2-8']
    if my_system == 'Windows':
        return ["grapa","Gdi32","Advapi32","User32","Ole32","Shell32","Comdlg32"]
    raise ValueError("Unknown platform: " + my_system)

# Prepare include directories
include_dirs = ["source","source/utf8proc","source/pybind11/include"]

# Add Windows SDK include path if on Windows
if sys.platform.startswith('win32') and 'windows_sdk_path' in locals() and windows_sdk_path:
    include_dirs.extend([windows_sdk_path, f"{windows_sdk_path}\\ucrt", f"{windows_sdk_path}\\um", f"{windows_sdk_path}\\shared"])
    # Also set environment variables for the compiler
    import os
    os.environ['INCLUDE'] = os.environ.get('INCLUDE', '') + f";{windows_sdk_path}\\um;{windows_sdk_path}\\ucrt;{windows_sdk_path}\\shared"
    # Fix the LIB path to use the correct Windows SDK Lib directory
    lib_path = windows_sdk_path.replace('Include', 'Lib')
    os.environ['LIB'] = os.environ.get('LIB', '') + f";{lib_path}\\um\\x64;{lib_path}\\ucrt\\x64"

lib_grapa = Extension(
    'grapapy', 
    sources = [
        'source/mainpy.cpp',
    ],
    include_dirs=include_dirs,
    library_dirs=pick_library_dirs(),
    libraries=pick_libraries(),
    runtime_library_dirs=runtime_library_dirs,
    extra_link_args=extra_link_args,
    extra_compile_args=extra_compile_args,
)

# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
if sys.platform.startswith('linux') or sys.platform.startswith('win32') or sys.platform.startswith('darwin'):
    setup(
        name="grapapy",
        version=grapapy_version,
        author="Chris Matichuk",
        author_email="matichuk@hotmail.com",
        description="GrapaPy brings robust, production-ready parallel ETL/data processing to Python. By leveraging Grapa's C++ backend, GrapaPy enables true parallelism for high-throughput data workflows—bypassing Python's GIL and making advanced data processing simple and fast. **Platform Support**: Windows (AMD64), macOS (AMD64/ARM64), Linux (AMD64/ARM64).",
        long_description="""
GrapaPy is a Python extension for the Grapa language, designed for advanced data processing, ETL, and language experimentation. GrapaPy brings robust, production-ready parallel ETL/data processing to Python. By leveraging Grapa's C++ backend, GrapaPy enables true parallelism for high-throughput data workflows—bypassing Python's GIL and making advanced data processing simple and fast.

## Platform Support

GrapaPy supports the following platforms:
- **Windows**: AMD64 (x86_64) - Requires Visual Studio Build Tools or Visual Studio
- **macOS**: AMD64 (x86_64) and ARM64 (Apple Silicon) - Requires Xcode Command Line Tools
- **Linux**: AMD64 (x86_64) and ARM64 - Requires GCC and development libraries

## Installation

```bash
# Windows
pip install grapapy

# macOS
pip3 install grapapy

# Linux (requires X11 libraries)
# Ubuntu/Debian:
sudo apt-get install libx11-dev libxfixes-dev libxft-dev libxext-dev libxrender-dev libxinerama-dev libfontconfig1-dev libxcursor-dev
pip3 install grapapy

# CentOS/RHEL:
sudo yum install libX11-devel libXfixes-devel libXft-devel libXext-devel libXrender-devel libXinerama-devel fontconfig-devel libXcursor-devel
pip3 install grapapy

# Fedora:
sudo dnf install libX11-devel libXfixes-devel libXft-devel libXext-devel libXrender-devel libXinerama-devel fontconfig-devel libXcursor-devel
pip3 install grapapy
```

## Features
- True parallel ETL/data processing from Python
- Hardened, production-ready parallelism
- High performance for large file processing, data transformation, analytics, and more
- Simple, functional API

For comprehensive documentation, visit: https://grapa-dev.github.io/grapa/
""",
        long_description_content_type="text/markdown",
        url="https://grapa-dev.github.io/grapa/",
        ext_modules=[lib_grapa],
        cmdclass={
            'copy_grapalib': CopySharedLibrary,
            'build_ext': CustomBuildExt,
        },
        # Add post-install script to check X11 dependencies
        scripts=['scripts/check_x11_dependencies.py'],
        entry_points={
            'console_scripts': [
                'grapapy-check-x11=scripts.check_x11_dependencies:main',
            ],
        },
        zip_safe=False,
        python_requires=">=3.6",
        packages=find_packages(),
        # Add build options to help with Windows permission issues
        options={
            'build_ext': {
                'build_temp': os.path.join(os.getcwd(), 'build_temp'),
            }
        },
        # System dependencies for Linux X11 support
        install_requires=[],
        # Note: X11 libraries are system dependencies that need to be installed separately
        # For Ubuntu/Debian: sudo apt-get install libx11-dev libxfixes-dev libxft-dev libxext-dev libxrender-dev libxinerama-dev libfontconfig1-dev libxcursor-dev
        # For CentOS/RHEL: sudo yum install libX11-devel libXfixes-devel libXft-devel libXext-devel libXrender-devel libXinerama-devel fontconfig-devel libXcursor-devel
        # For Fedora: sudo dnf install libX11-devel libXfixes-devel libXft-devel libXext-devel libXrender-devel libXinerama-devel fontconfig-devel libXcursor-devel
    )
else:
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

