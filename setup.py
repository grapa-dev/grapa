import os
import re
import subprocess
import sys
import sysconfig
import platform

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}

from_os = ''

# 'freebsd'
# 'aix'
# 'cygwin'

if sys.platform.startswith('win32'):
    from_os = 'win'
if sys.platform.startswith('linux'):
    from_os = 'linux'
    temp_result = subprocess.run(["cat", "/etc/os-release"])
    process = subprocess.Popen(['cat', '/etc/os-release'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr.decode()=='':
        stdouts = stdout.decode()
        if stdouts.find("Amazon Linux")>=0:
            is_aws = True
            from_os = 'aws'
elif sys.platform.startswith('darwin'):
    from_os = 'mac-intel'
    if platform.machine()=='arm64':
        from_os = 'mac-apple'

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
        cmake_args += [f"-DEXAMPLE_VERSION_INFO={self.distribution.get_version()}"]
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
            #cmake_args += ['-Wl,-rpath,@loader_path/grapapy']
            #build_args += [
                #'--framework','CoreFoundation',
                #'-framework','AppKit',
                #'-framework','IOKit'
            #    ]
            #cmake_args += [
		    #'-dynamiclib',
		    #'-rpath', get_python_lib(),
		    #'-Wl,-headerpad_max_install_names',
		    #'-Wl,-install_name,%s' % linker_path,
		    #'-Wl,-x']
        #if sys.platform.startswith('linux'):
        #     cmake_args += ['-Wl,-rpath,$ORIGIN/grapapy']
        
        #self.mkpath(os.path.join(extdir, 'grapapy'))

#        if sys.platform.startswith('linux'):
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/grapa-lib/linux')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/grapa-lib/linux',file_name), os.path.join(extdir, 'grapapy',file_name))
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/openssl-lib/linux')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/openssl-lib/linux',file_name), os.path.join(extdir, 'grapapy',file_name))
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/fl-lib/linux')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/fl-lib/linux',file_name), os.path.join(extdir, 'grapapy',file_name))
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/blst-lib/linux')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/blst-lib/linux',file_name), os.path.join(extdir, 'grapapy',file_name))
#        if sys.platform.startswith('darwin'):
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/grapa-lib/mac-intel')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/grapa-lib/mac-intel',file_name), os.path.join(extdir, 'grapapy',file_name))
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/openssl-lib/mac-intel')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/openssl-lib/mac-intel',file_name), os.path.join(extdir, 'grapapy',file_name))
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/fl-lib/mac-intel')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/fl-lib/mac-intel',file_name), os.path.join(extdir, 'grapapy',file_name))
#            for file_name in os.listdir(os.path.join(ext.sourcedir, 'source/blst-lib/mac-intel')):
#                self.copy_file(os.path.join(ext.sourcedir, 'source/blst-lib/mac-intel',file_name), os.path.join(extdir, 'grapapy',file_name))
        
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


# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
setup(
    name="grapapy",
    version="0.0.9",
    author="Chris Matichuk",
    description="grammar parser language",
    long_description="https://github.com/grapa-dev/grapa",
    ext_modules=[CMakeExtension("grapapy")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    python_requires=">=3.6",
)
