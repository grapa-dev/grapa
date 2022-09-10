import os
import sys
import platform
import subprocess

from distutils.command.build import build

from setuptools import Extension, setup, find_packages, Command
from setuptools.command.build_ext import build_ext

from pathlib import Path

extra_link_args = []
runtime_library_dirs = []
grapapy_version = "0.0.5"
is_aws = False
from_os = ''

if sys.platform.startswith('win32'):
    so_ext = '.lib'
    lib_filename = 'grapa' + so_ext
    lib_pathfile = 'grapa-lib/win/' + lib_filename
if sys.platform.startswith('linux'):
    from_os = 'linux'
    temp_result = subprocess.run(["cat", "/etc/os-release"]);
    process = subprocess.Popen(['cat', '/etc/os-release'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr.decode()=='':
        stdouts = stdout.decode();
        if stdouts[stdouts.find("NAME")+6:stdouts.find("VERSION")-2]=="Amazon Linux":
            is_aws = True
            from_os = 'aws';
    so_ext = '.so'
    lib_filename = 'libgrapa' + so_ext
    lib_pathfile = 'grapa-lib/' + from_os + '/' + lib_filename
    runtime_library_dirs = ['$ORIGIN/grapapy-' + grapapy_version]
elif sys.platform.startswith('darwin'):
    so_ext = '.dylib'
    lib_filename = 'libgrapa' + so_ext
    lib_pathfile = 'grapa-lib/mac-intel/' + lib_filename
    extra_link_args = ['-Wl,-rpath,@loader_path']
            

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
        if sys.platform.startswith('linux'):
            for file_name in os.listdir(os.path.join(self.build_dir, 'grapa-lib/'+from_os)):
                self.copy_file(os.path.join(os.path.join(self.build_dir, 'grapa-lib/'+from_os),file_name), os.path.join(lib_target_path, file_name))
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
            return ["source", "source/grapa-lib/aws"]
        else:
            return ["source", "source/grapa-lib/linux"]
    if my_system == 'Darwin':
        return ["source", "source/grapa-lib/mac-intel"]
    if my_system == 'Windows':
        return ["source", "source/grapa-lib/win"]
    raise ValueError("Unknown platform: " + my_system)

def pick_libraries():
    my_system = platform.system()
    if my_system == 'Linux':
        return ["grapa"]
    if my_system == 'Darwin':
        return ["grapa"]
    if my_system == 'Windows':
        return ["grapa","Gdi32","Advapi32","User32","Ole32","Shell32","Comdlg32"]
    raise ValueError("Unknown platform: " + my_system)

lib_grapa = Extension(
    'grapapy', 
    sources = ['source/mainpy.cpp'],
    include_dirs=["source",'source/pybind11/include'],
    library_dirs=pick_library_dirs(),
    libraries=pick_libraries(),
    runtime_library_dirs=runtime_library_dirs,
    extra_link_args=extra_link_args
)

setup(
    name="grapapy",
    version="0.0.5",
    author="Chris Matichuk",
    author_email="matichuk@hotmail.com",
    description="grammar parser language",
    long_description="",
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
