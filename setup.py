import os
import sys
import platform
import subprocess
import re

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

from pathlib import Path

grapapy_version = "0.1.52"
is_arm = platform.machine().lower() in ["aarch64", "arm64"]

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}

# Determine platform for CMake
if sys.platform.startswith('win32'):
    from_os = 'win-amd64'
elif sys.platform.startswith('linux'):
    from_os = 'linux-amd64'
    if is_arm:
        from_os = 'linux-arm64'
    # Check if it's Amazon Linux or AWS
    try:
        # Check for AWS-specific environment variables
        if 'AWS' in os.environ.get('AWS_EXECUTION_ENV', ''):
            from_os = from_os.replace('linux', 'aws')
        # Check for Amazon Linux in os-release
        elif os.path.exists('/etc/os-release'):
            with open('/etc/os-release', 'r') as f:
                content = f.read().lower()
                if any(identifier in content for identifier in ['amazon linux', 'amazon-linux', 'aws', 'amazon']):
                    from_os = from_os.replace('linux', 'aws')
        # Check for Amazon Linux in system-release
        elif os.path.exists('/etc/system-release'):
            with open('/etc/system-release', 'r') as f:
                content = f.read().lower()
                if any(identifier in content for identifier in ['amazon linux', 'amazon-linux', 'aws', 'amazon']):
                    from_os = from_os.replace('linux', 'aws')
    except:
        pass
elif sys.platform.startswith('darwin'):
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

