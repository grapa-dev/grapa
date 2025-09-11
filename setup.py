import os
import sys
import platform
import subprocess
import re

# Check setuptools version for compatibility
try:
    import setuptools
    setuptools_version = setuptools.__version__
    print(f"Using setuptools version: {setuptools_version}")
    
    # Check if setuptools version is too old for setuptools.command.build
    try:
        from packaging import version
        if version.parse(setuptools_version) < version.parse("40.0.0"):
            print("Warning: setuptools version is older than 40.0.0, may need fallback to distutils")
    except ImportError:
        # packaging module not available, can't do version comparison
        pass
        
except ImportError:
    setuptools_version = "unknown"
    print("Warning: Could not determine setuptools version")

# Compatibility check for setuptools.command.build
try:
    from setuptools.command.build import build
    print("Using setuptools.command.build")
except ImportError:
    # Fall back to distutils for older setuptools versions
    try:
        from distutils.command.build import build
        print("Using distutils.command.build (fallback for older setuptools)")
    except ImportError:
        raise ImportError(
            "Neither setuptools.command.build nor distutils.command.build is available.\n"
            "This usually indicates an older pip3 version. Please try:\n"
            "  pip3 install --upgrade pip\n"
            "  pip3 install --upgrade setuptools\n"
            "If the issue persists, you may need to install distutils separately."
        )

from setuptools import Extension, setup, find_packages, Command
from setuptools.command.build_ext import build_ext

from pathlib import Path

def check_system_dependencies():
    """Check if required system dependencies are available for compilation"""
    missing_deps = []
    
    if sys.platform.startswith('linux'):
        # Check for required development packages
        try:
            import ctypes
            # Check for development headers - try multiple common library names
            # Support both OpenSSL 1.1 and OpenSSL 3
            ssl_libs = ['libssl.so.1.1', 'libssl.so.3', 'libssl.so', 'libssl3.so']
            crypto_libs = ['libcrypto.so.1.1', 'libcrypto.so.3', 'libcrypto.so']
            x11_libs = ['libX11.so.6', 'libX11.so']
            xext_libs = ['libXext.so.6', 'libXext.so']
            
            found_ssl = False
            found_crypto = False
            found_x11 = False
            found_xext = False
            
            # Check SSL libraries
            for lib in ssl_libs:
                try:
                    ctypes.CDLL(lib)
                    found_ssl = True
                    break
                except OSError:
                    continue
            
            # Check Crypto libraries
            for lib in crypto_libs:
                try:
                    ctypes.CDLL(lib)
                    found_crypto = True
                    break
                except OSError:
                    continue
            
            # Check X11 libraries
            for lib in x11_libs:
                try:
                    ctypes.CDLL(lib)
                    found_x11 = True
                    break
                except OSError:
                    continue
            
            # Check Xext libraries
            for lib in xext_libs:
                try:
                    ctypes.CDLL(lib)
                    found_xext = True
                    break
                except OSError:
                    continue
            
            if not found_ssl or not found_crypto:
                missing_deps.append('OpenSSL development libraries')
            if not found_x11 or not found_xext:
                missing_deps.append('X11 development libraries')
                
        except ImportError:
            pass
            
        # Check for g++ compiler
        try:
            subprocess.run(['g++', '--version'], capture_output=True, check=True)
        except (subprocess.CalledProcessError, FileNotFoundError):
            missing_deps.append('g++ compiler with C++17 support')
            
    elif sys.platform.startswith('darwin'):
        # Check for Xcode command line tools
        try:
            result = subprocess.run(['xcode-select', '--print-path'], 
                                  capture_output=True, text=True, check=True)
            if not result.stdout.strip():
                missing_deps.append('Xcode Command Line Tools')
        except (subprocess.CalledProcessError, FileNotFoundError):
            missing_deps.append('Xcode Command Line Tools')
            
    elif sys.platform.startswith('win32'):
        # Check for Visual Studio build tools
        try:
            # Try to find MSBuild and verify it's from Visual Studio 2022
            result = subprocess.run(['msbuild', '/version'], capture_output=True, text=True, check=True)
            # Check if it's Visual Studio 2022 (version 17.x)
            if '17.' not in result.stdout:
                missing_deps.append('Visual Studio 2022 or Build Tools for Visual Studio 2022 (version 17.x required)')
        except (subprocess.CalledProcessError, FileNotFoundError):
            missing_deps.append('Visual Studio 2022 or Build Tools for Visual Studio 2022')
        
        # Note: 7-Zip is not required for Python package installation
        # It's only needed for building/developing Grapa from source
        
        # Check for Python 3
        if sys.version_info < (3, 6):
            missing_deps.append('Python 3.6 or higher')
        
        # Check for Windows SDK
        try:
            import winreg
            # Check for Windows 10/11 SDK
            sdk_keys = [
                r"SOFTWARE\Microsoft\Windows Kits\Installed Roots",
                r"SOFTWARE\WOW6432Node\Microsoft\Windows Kits\Installed Roots"
            ]
            found_sdk = False
            for key_path in sdk_keys:
                try:
                    key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, key_path)
                    winreg.CloseKey(key)
                    found_sdk = True
                    break
                except FileNotFoundError:
                    continue
            if not found_sdk:
                missing_deps.append('Windows 10/11 SDK (usually included with Visual Studio 2022)')
        except ImportError:
            # winreg not available, skip SDK check
            pass
    
    if missing_deps:
        print("\n" + "="*60)
        print("❌ MISSING SYSTEM DEPENDENCIES")
        print("="*60)
        print("The following system dependencies are required but not found:")
        for dep in set(missing_deps):
            print(f"   • {dep}")
        print("\nPlease install the missing dependencies before installing grapapy.")
        print("\nInstallation instructions:")
        if sys.platform.startswith('linux'):
            print("   Ubuntu/Debian: sudo apt-get install build-essential libssl-dev libx11-dev libxext-dev")
            print("   CentOS/RHEL:   sudo yum install gcc-c++ openssl-devel libX11-devel libXext-devel")
        elif sys.platform.startswith('darwin'):
            print("   macOS:         xcode-select --install")
        elif sys.platform.startswith('win32'):
            print("   Windows:       Install Visual Studio 2022 or Build Tools for Visual Studio 2022")
            print("                 Download from: https://visualstudio.microsoft.com/downloads/")
            print("                 Run from 'x64 Native Tools Command Prompt for VS 2022'")
        print("\nFor detailed instructions, visit: https://grapa-dev.github.io/grapa/")
        print("="*60)
        raise RuntimeError("Missing required system dependencies")

grapapy_version = "0.1.54"
is_aws = False
is_apple = False
from_os = ''
is_arm = platform.machine().lower() in ["aarch64", "arm64"]

include_dirs = []
extra_link_args = []
extra_compile_args = []
runtime_library_dirs = []

# Determine platform for CMake
if sys.platform.startswith('win32'):
    from_os = 'win-amd64'
    so_ext = '.lib'
    lib_filename = 'grapa_static' + so_ext
    lib_pathfile = 'grapa-lib/win-amd64/' + lib_filename
    extra_compile_args = ['/DUTF8PROC_STATIC', '/DPCRE2_STATIC', '/std:c++17']
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
elif sys.platform.startswith('linux'):
    from_os = 'linux-amd64'
    if is_arm:
        from_os = 'linux-arm64'
    # Check if it's Amazon Linux or AWS
    try:
        # Check for AWS-specific environment variables
        if 'AWS' in os.environ.get('AWS_EXECUTION_ENV', ''):
            from_os = from_os.replace('linux', 'aws')
            is_aws = True;
        # Check for Amazon Linux in os-release
        elif os.path.exists('/etc/os-release'):
            with open('/etc/os-release', 'r') as f:
                content = f.read().lower()
                if any(identifier in content for identifier in ['amazon linux', 'amazon-linux', 'aws', 'amazon']):
                    from_os = from_os.replace('linux', 'aws')
                    is_aws = True;
        # Check for Amazon Linux in system-release
        elif os.path.exists('/etc/system-release'):
            with open('/etc/system-release', 'r') as f:
                content = f.read().lower()
                if any(identifier in content for identifier in ['amazon linux', 'amazon-linux', 'aws', 'amazon']):
                    from_os = from_os.replace('linux', 'aws')
                    is_aws = True;
    except:
        pass
elif sys.platform.startswith('darwin'):
    is_apple = True
    if is_arm:
        from_os = 'mac-arm64'
    else:
        raise RuntimeError("mac-amd64 is no longer supported. Please use an Apple Silicon Mac (arm64).")
else:
    raise RuntimeError(f"Unsupported platform: {sys.platform}")

# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name, sourcedir="."):
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

        # CMake lets you override the generator -- we need to check this.
        # Can be set with Conda-Build, for example.
        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value in the
        # [configuration] section of CMakeLists.txt as a variable; in this
        # case, EXAMPLE_VERSION_INFO tells the CMakeLists.txt file what version
        # of the Python package is being built.
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCMAKE_BUILD_TYPE={cfg}",
            f"-DGRAPAPY_OS={from_os}",
        ]
        build_args = []
        # Adding CMake arguments set as environment variable
        # (needed e.g. to build for ARM OSx on conda-forge)
        if "CMAKE_ARGS" in os.environ:
            cmake_args += [item for item in os.environ["CMAKE_ARGS"].split(" ") if item]

        # In this case, we need to force the generator to use the host compiler.
        # This is necessary because Conda build may pick a cross-compiler
        # based on the target platform.
        if cmake_generator:
            cmake_args += ["-G", cmake_generator]

        if sys.platform.startswith("darwin"):
            # Cross-compile support for macOS - respect ARCHFLAGS if set
            archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
            if archs:
                cmake_args += ["-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))]

        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
        # across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            # self.parallel is a Python 3 only way to set parallel jobs by hand
            # using -j in the build_ext call, not supported by pip or PyPA-build.
            if hasattr(self, "parallel") and self.parallel:
                # CMake 3.12+ only.
                build_args += [f"-j{self.parallel}"]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        # Add rpath flag for Linux to find shared libraries
        if sys.platform.startswith("linux"):
            cmake_args += ["-DCMAKE_INSTALL_RPATH=${ORIGIN}", "-DCMAKE_BUILD_RPATH=${ORIGIN}"]

        subprocess.check_call(["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp)
        subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=self.build_temp)

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
        
    def build_extension(self, ext):
        # Use a local build directory to avoid permission issues
        if sys.platform.startswith('win32'):
            self.build_temp = os.path.join(os.getcwd(), 'build_temp')
            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)
        try:
            super().build_extension(ext)
        except PermissionError as e:
            if "cache" in str(e).lower():
                print("\n" + "="*60)
                print("PERMISSION ERROR: Pip cannot write to its cache directory.")
                print("SOLUTION: Use one of these commands:")
                print("  pip install --no-cache-dir dist/grapapy-0.0.25.tar.gz")
                print("  pip install --no-cache-dir -e .")
                print("="*60)
            raise

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
        return ["source", "source/grapa-lib/win-amd64", "source/fl-lib/win-amd64", "source/blst-lib/win-amd64", "source/pcre2-lib/win-amd64", "source/openssl-lib/win-amd64"]
    raise ValueError("Unknown platform: " + my_system)

def pick_libraries():
    my_system = platform.system()
    if my_system == 'Linux':
        return ['grapa']
    if my_system == 'Darwin':
        #return ['@rpath/grapa']
        return ['source/grapa-lib/libgrapa_static.a']
    if my_system == 'Windows':
        return [
            "grapa_static",
            "fltk",
            "fltk_forms", 
            "fltk_gl",
            "fltk_images",
            "fltk_jpeg",
            "fltk_png",
            "fltk_z",
            "libcrypto",
            "libssl",
            "blst",
            "pcre2-8-static",
            "gdiplus",
            "ComCtl32",
            "Gdi32",
            "Advapi32",
            "User32",
            "Ole32",
            "Shell32",
            "Comdlg32",
            "winspool",
            "crypt32"
        ]
    raise ValueError("Unknown platform: " + my_system)

if sys.platform.startswith('win32') and 'include_dirs' in locals():
    # Use custom include_dirs that puts Windows SDK paths first
    base_include_dirs = ["source","source/utf8proc",'source/pybind11/include']
    include_dirs = include_dirs + base_include_dirs

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

# Check system dependencies before attempting to build
check_system_dependencies()

if sys.platform.startswith('win32'):
    setup(
        name="grapapy",
        version=grapapy_version,
        author="Chris Matichuk",
        author_email="matichuk@hotmail.com",
        description="GrapaPy brings robust, production-ready parallel ETL/data processing to Python. By leveraging Grapa's C++ backend, GrapaPy enables true parallelism for high-throughput data workflows—bypassing Python's GIL and making advanced data processing simple and fast.",
        long_description="""
GrapaPy is a Python extension for the Grapa language, designed for advanced data processing, ETL, and language experimentation. GrapaPy brings robust, production-ready parallel ETL/data processing to Python. By leveraging Grapa's C++ backend, GrapaPy enables true parallelism for high-throughput data workflows—bypassing Python's GIL and making advanced data processing simple and fast.

Features:
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
            'build': CustomBuild,
        },
        zip_safe=False,
        python_requires=">=3.6",
        packages=find_packages(),
        install_requires=[
            "pybind11>=2.6.0",
        ],
        # Add build options to help with Windows permission issues
        options={
            'build_ext': {
                'build_temp': os.path.join(os.getcwd(), 'build_temp'),
            }
        },
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
        install_requires=[
            "pybind11>=2.6.0",
        ],
    )

