#!/usr/bin/env python3
"""
Grapa Build Script

This script automates the build process for the current platform and architecture.
It replaces the manual copy-paste process from BUILD.md.

IMPORTANT: This script builds for the CURRENT platform only. It does NOT support cross-compilation.
You must run this script on each target platform independently.

Usage:
    python3 build.py                    # Build for current platform/arch
    python3 build.py --test             # Run tests after build
    python3 build.py --clean            # Clean build artifacts
    python3 build.py --help             # Show help

Supported Platforms (when run on that platform):
    - windows (amd64)
    - mac (arm64, amd64)
    - linux (arm64, amd64)
    - aws (arm64, amd64)
"""

import os
import sys
import subprocess
import platform
import argparse
import shutil
from pathlib import Path
from typing import Dict, List, Tuple, Optional

class BuildConfig:
    """Configuration for different build targets"""
    
    def __init__(self, platform: str, arch: str):
        self.platform = platform
        self.arch = arch
        # Map platform names to directory names
        if platform == "windows":
            self.target = f"win-{arch}"
        else:
            self.target = f"{platform}-{arch}"
        
        # Platform-specific settings
        self.compiler = self._get_compiler()
        self.flags = self._get_flags()
        self.libs = self._get_libs()
        self.frameworks = self._get_frameworks()
        self.output_name = "grapa.exe" if platform == "windows" else "grapa"
        
    def _get_compiler(self) -> str:
        """Get the appropriate compiler for the platform"""
        if self.platform == "windows":
            return "msbuild"
        elif self.platform == "mac":
            return "clang++"
        else:  # linux, aws
            return "g++"
    
    def _get_flags(self) -> List[str]:
        """Get compiler flags for the platform"""
        base_flags = ["-Isource", "-DUTF8PROC_STATIC", "-std=c++17", "-O3", "-pthread"]
        
        if self.platform == "mac":
            base_flags.extend(["-m64"])
        elif self.platform in ["linux", "aws"]:
            base_flags.extend(["-static-libgcc"])
            
        return base_flags
    
    def _get_libs(self) -> List[str]:
        """Get library paths for the platform"""
        lib_path = f"source/*-lib/{self.target}"
        return [
            f"{lib_path}/*.a",
            f"source/pcre2-lib/{self.target}/libpcre2-8.a"
        ]
    
    def _get_frameworks(self) -> List[str]:
        """Get framework flags for Mac"""
        if self.platform == "mac":
            return [
                "-framework CoreFoundation",
                "-framework AppKit", 
                "-framework IOKit"
            ]
        return []

class GrapaBuilder:
    """Main build orchestrator"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.bin_dir = self.project_root / "bin"
        self.bin_dir.mkdir(exist_ok=True)
        
    def detect_platform(self) -> Tuple[str, str]:
        """Detect current platform and architecture"""
        system = platform.system().lower()
        machine = platform.machine().lower()
        
        if system == "windows":
            return "windows", "amd64"
        elif system == "darwin":
            # On Mac, machine can be "arm64" or "x86_64"
            if machine == "arm64":
                return "mac", "arm64"
            elif machine == "x86_64":
                return "mac", "amd64"
            else:
                raise RuntimeError(f"Unsupported Mac architecture: {machine}")
        elif system == "linux":
            # Check if this is AWS Linux by looking for Amazon Linux specific files
            if (os.path.exists("/etc/system-release") and 
                ("Amazon Linux" in open("/etc/system-release").read())):
                return "aws", "arm64" if machine == "aarch64" else "amd64"
            else:
                return "linux", "arm64" if machine == "aarch64" else "amd64"
        else:
            raise RuntimeError(f"Unsupported platform: {system}")
    
    def build_windows(self, config: BuildConfig) -> bool:
        """Build for Windows using Visual Studio"""
        print(f"Building for {config.target} using Visual Studio...")
        
        try:
            # Build main executable
            subprocess.run([
                "msbuild", "prj/win-amd64/grapa.sln", "/p:Configuration=Release"
            ], check=True)
            
            # Copy executable
            if os.path.exists("grapa.exe"):
                os.remove("grapa.exe")
            shutil.copy("prj/win-amd64/x64/Release/grapa.exe", "grapa.exe")
            
            # Build library
            subprocess.run([
                "msbuild", "prj/winlib-amd64/grapalib.sln", "/p:Configuration=Release"
            ], check=True)
            
            # Copy library
            if os.path.exists("grapa.lib"):
                os.remove("grapa.lib")
            shutil.copy("prj/winlib-amd64/x64/Release/grapa.lib", "grapa.lib")
            shutil.copy("prj/winlib-amd64/x64/Release/grapa.lib", f"source/grapa-lib/{config.target}/grapa.lib")
            
            # Clean build artifacts
            self._clean_windows_build()
            
            # Create package
            self._create_windows_package(config)
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Windows build failed: {e}")
            return False
    
    def build_mac(self, config: BuildConfig) -> bool:
        """Build for Mac using clang/clang++"""
        print(f"Building for {config.target} using clang++...")
        
        try:
            # Build main executable
            self._run_mac_build_command(config, is_library=False)
            
            # Build static library
            self._run_mac_build_command(config, is_library=True, is_static=True)
            
            # Build shared library
            self._run_mac_build_command(config, is_library=True, is_static=False)
            
            # Create package
            self._create_mac_package(config)
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Mac build failed: {e}")
            return False
    
    def build_linux_aws(self, config: BuildConfig) -> bool:
        """Build for Linux/AWS using g++"""
        print(f"Building for {config.target} using g++...")
        
        try:
            # Build main executable
            if config.platform == "aws":
                self._run_aws_build_command(config, is_library=False)
            else:
                self._run_linux_build_command(config, is_library=False)
            
            # Build static library
            if config.platform == "aws":
                self._run_aws_build_command(config, is_library=True, is_static=True)
            else:
                self._run_linux_build_command(config, is_library=True, is_static=True)
            
            # Build shared library
            if config.platform == "aws":
                self._run_aws_build_command(config, is_library=True, is_static=False)
            else:
                self._run_linux_build_command(config, is_library=True, is_static=False)
            
            # Create package
            self._create_linux_package(config)
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"{config.platform.capitalize()} build failed: {e}")
            return False
    
    def _run_mac_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
        """Run Mac build command"""
        import glob
        
        # Remove existing executable only when building executable
        if not is_library and os.path.exists(config.output_name):
            os.remove(config.output_name)
        
        print(f"Building {'library' if is_library else 'executable'} for {config.target}...")
        
        # Build utf8proc first (C compilation)
        print("Building utf8proc...")
        subprocess.run([
            "clang", "-Isource", "-DUTF8PROC_STATIC", "-c", 
            "source/utf8proc/utf8proc.c", "-m64", "-O3"
        ], check=True)
        
        if is_library:
            if is_static:
                # Build static library - match BUILD.md exactly
                print("Building static library...")
                cpp_files = glob.glob("source/grapa/*.cpp")
                subprocess.run([
                    "clang++", "-Isource", "-c"
                ] + cpp_files + [
                    "-std=c++17", "-m64", "-O3", "-pthread"
                ], check=True)
                # Get all .o files
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                subprocess.run(["ar", "-crs", "libgrapa.a"] + obj_files, check=True)
                shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa.a")
                os.remove("libgrapa.a")
            else:
                # Build shared library - match BUILD.md exactly
                print("Building shared library...")
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                subprocess.run([
                    "clang++", "-shared", "-Isource"
                ] + cpp_files + ["utf8proc.o"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    "-framework", "CoreFoundation", "-framework", "AppKit", "-framework", "IOKit",
                    "-std=c++17", "-m64", "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                ], check=True)
                shutil.copy("libgrapa.so", f"source/grapa-other/{config.target}/libgrapa.so")
                os.remove("libgrapa.so")
        else:
            # Build executable - match BUILD.md exactly (two separate steps)
            print("Building executable...")
            cpp_files = glob.glob("source/grapa/*.cpp")
            openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
            fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
            blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
            pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
            
            # Step 1: utf8proc.o is already built above
            # Step 2: Build executable using utf8proc.o - use shell globs like manual command
            cmd = [
                "clang++", "-Isource", "source/main.cpp", "source/grapa/*.cpp", "utf8proc.o",
                f"source/openssl-lib/{config.target}/*.a", f"source/fl-lib/{config.target}/*.a", 
                f"source/blst-lib/{config.target}/*.a", f"source/pcre2-lib/{config.target}/libpcre2-8.a",
                "-framework", "CoreFoundation", "-framework", "AppKit", "-framework", "IOKit",
                "-std=c++17", "-m64", "-O3", "-pthread", "-o", config.output_name
            ]
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            try:
                # Try os.system() first - it might be faster than subprocess
                result = os.system(" ".join(cmd))
                if result != 0:
                    raise RuntimeError(f"Build failed with exit code {result}")
            except Exception as e:
                print(f"❌ Build failed: {e}")
                raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"✅ Executable created: {config.output_name}")
            else:
                print(f"❌ Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
        
        # Clean object files (but preserve executable)
        for obj_file in Path(".").glob("*.o"):
            obj_file.unlink()
    
    def _run_linux_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
        """Run Linux build command (Ubuntu)"""
        import glob
        
        # Remove existing executable only when building executable
        if not is_library and os.path.exists(config.output_name):
            os.remove(config.output_name)
        
        print(f"Building {'library' if is_library else 'executable'} for {config.target}...")
        
        # Build utf8proc first (C compilation)
        print("Building utf8proc...")
        # Use -fPIC for shared library builds, regular for executable
        pic_flag = ["-fPIC"] if is_library else []
        subprocess.run([
            "gcc", "-Isource", "-DUTF8PROC_STATIC", "-c", 
            "source/utf8proc/utf8proc.c", "-O3"
        ] + pic_flag, check=True)
        
        # Get library path based on target
        lib_path = f"source/openssl-lib/{config.target}"
        
        if is_library:
            if is_static:
                # Build static library
                cpp_files = glob.glob("source/grapa/*.cpp")
                subprocess.run([
                    "g++", "-Isource", "-c"
                ] + cpp_files + [
                    "-std=c++17", "-O3", "-pthread", "-fPIC"
                ], check=True)
                # Get all .o files (including utf8proc.o if it exists)
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                subprocess.run(["ar", "-crs", "libgrapa.a"] + obj_files, check=True)
                shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa.a")
                os.remove("libgrapa.a")
            else:
                # Build shared library
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                cmd = ["g++", "-shared", "-Isource", "-DUTF8PROC_STATIC"] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto",
                    "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                    "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc",
                    "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                ]
                
                subprocess.run(cmd, check=True)
                shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa.so")
                os.remove("libgrapa.so")
        else:
            # Build executable - match AWS pattern exactly
            cpp_files = glob.glob("source/grapa/*.cpp")
            openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
            fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
            blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
            pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
            
            cmd = [
                "g++", "-Isource", "-DUTF8PROC_STATIC", "source/main.cpp"
            ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + [
                f"source/pcre2-lib/{config.target}/libpcre2-8.a", f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto", 
                "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama", 
                "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc", 
                "-O3", "-pthread", "-o", config.output_name
            ]
            
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            try:
                # Try os.system() first - it might be faster than subprocess
                result = os.system(" ".join(cmd))
                if result != 0:
                    raise RuntimeError(f"Build failed with exit code {result}")
            except Exception as e:
                print(f"❌ Build failed: {e}")
                raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"✅ Executable created: {config.output_name}")
            else:
                print(f"❌ Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
        
        # Clean object files (but preserve executable)
        for obj_file in Path(".").glob("*.o"):
            obj_file.unlink()
    
    def _run_aws_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
        """Run AWS build command (Amazon Linux)"""
        import glob
        
        # Remove existing executable only when building executable
        if not is_library and os.path.exists(config.output_name):
            os.remove(config.output_name)
        
        print(f"Building {'library' if is_library else 'executable'} for {config.target}...")
        
        # Build utf8proc first (C compilation) - only for static libraries and executables
        if is_library and is_static:
            print("Building utf8proc...")
            subprocess.run([
                "gcc", "-Isource", "-DUTF8PROC_STATIC", "-c", 
                "source/utf8proc/utf8proc.c", "-O3", "-fPIC"
            ], check=True)
        elif not is_library:
            print("Building utf8proc...")
            subprocess.run([
                "gcc", "-Isource", "-DUTF8PROC_STATIC", "-c", 
                "source/utf8proc/utf8proc.c", "-O3"
            ], check=True)
        
        if is_library:
            if is_static:
                # Build static library
                cpp_files = glob.glob("source/grapa/*.cpp")
                subprocess.run([
                    "g++", "-Isource", "-c"
                ] + cpp_files + [
                    "-std=c++17", "-O3", "-pthread"
                ], check=True)
                # Get all .o files
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                subprocess.run(["ar", "-crs", "libgrapa.a"] + obj_files, check=True)
                shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa.a")
                os.remove("libgrapa.a")
            else:
                # Build shared library - match original working script exactly
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                cmd = ["g++", "-shared", "-Isource", "-DUTF8PROC_STATIC"] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto",
                    "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                    "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc",
                    "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                ]
                
                print(f"Executing shared library build command: {' '.join(cmd)}")
                subprocess.run(cmd, check=True)
                shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa.so")
                os.remove("libgrapa.so")
        else:
            # Build executable
            cpp_files = glob.glob("source/grapa/*.cpp")
            openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
            fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
            blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
            pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
            
            cmd = [
                "g++", "-Isource", "-DUTF8PROC_STATIC", "source/main.cpp"
            ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + [
                f"source/pcre2-lib/{config.target}/libpcre2-8.a", f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto", 
                "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama", 
                "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc", 
                "-O3", "-pthread", "-o", config.output_name
            ]
            
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            try:
                # Try os.system() first - it might be faster than subprocess
                result = os.system(" ".join(cmd))
                if result != 0:
                    raise RuntimeError(f"Build failed with exit code {result}")
            except Exception as e:
                print(f"❌ Build failed: {e}")
                raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"✅ Executable created: {config.output_name}")
            else:
                print(f"❌ Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
        
        # Clean object files (but preserve executable)
        for obj_file in Path(".").glob("*.o"):
            obj_file.unlink()
    

    
    def _clean_windows_build(self):
        """Clean Windows build artifacts"""
        build_dirs = [
            "prj/win-amd64/x64",
            "prj/win-amd64/grapa",
            "prj/winlib-amd64/x64",
            "prj/winlib-amd64/grapalib"
        ]
        for dir_path in build_dirs:
            if os.path.exists(dir_path):
                shutil.rmtree(dir_path)
    
    def _create_windows_package(self, config: BuildConfig):
        """Create Windows package"""
        subprocess.run([
            "7z", "a", f"bin/grapa-{config.target}.zip", "grapa.exe", "grapa.lib"
        ], check=True)
    
    def _create_mac_package(self, config: BuildConfig):
        """Create Mac package"""
        import glob
        
        # Get the actual files to include
        files_to_include = []
        
        # Add executable if it exists
        if os.path.exists(config.output_name):
            files_to_include.append(config.output_name)
        
        # Add library files
        lib_files = glob.glob(f"source/grapa-lib/{config.target}/*")
        files_to_include.extend(lib_files)
        
        # Add other files
        other_files = glob.glob(f"source/grapa-other/{config.target}/*")
        files_to_include.extend(other_files)
        
        if not files_to_include:
            raise RuntimeError(f"No files found to include in package for {config.target}")
        
        # Create tar command with actual files
        tar_cmd = ["tar", "-czvf", f"bin/grapa-{config.target}.tar.gz"] + files_to_include
        print(f"Creating package with files: {files_to_include}")
        subprocess.run(tar_cmd, check=True)
    
    def _create_linux_package(self, config: BuildConfig):
        """Create Linux/AWS package"""
        import glob
        
        # Get the actual files to include
        files_to_include = []
        
        # Add executable if it exists
        if os.path.exists(config.output_name):
            files_to_include.append(config.output_name)
        
        # Add library files (only grapa-lib, matching original script)
        lib_files = glob.glob(f"source/grapa-lib/{config.target}/*")
        files_to_include.extend(lib_files)
        
        if not files_to_include:
            raise RuntimeError(f"No files found to include in package for {config.target}")
        
        # Create tar command with actual files
        tar_cmd = ["tar", "-czvf", f"bin/grapa-{config.target}.tar.gz"] + files_to_include
        print(f"Creating package with files: {files_to_include}")
        subprocess.run(tar_cmd, check=True)
    
    def build_python_package(self, config: BuildConfig):
        """Build Python package"""
        print("Building Python package...")
        
        # Clean previous builds
        if os.path.exists("dist"):
            shutil.rmtree("dist")
        if os.path.exists("grapapy.egg-info"):
            shutil.rmtree("grapapy.egg-info")
        
        # Build package
        subprocess.run(["python3", "setup.py", "sdist"], check=True)
        
        # Find the built package file
        dist_files = list(Path("dist").glob("*.tar.gz"))
        if not dist_files:
            raise RuntimeError("No Python package found in dist/ directory")
        
        package_file = dist_files[0].name
        print(f"Found package: {package_file}")
        
        # Install package - handle systems that don't support wildcards
        if config.platform == "windows":
            # On Windows, use grapa to find the filename
            subprocess.run([
                "grapa.exe", "-q", "-ccmd", 
                f"f=$file().ls('dist')[0].$KEY;$sys().shell('pip install dist/'+f);"
            ], check=True)
        else:
            # On Unix systems, try direct filename first, fallback to wildcard
            try:
                # Try with explicit filename
                subprocess.run([
                    "pip3", "install", f"dist/{package_file}"
                ], check=True)
            except subprocess.CalledProcessError:
                # Fallback to using grapa to find the filename
                subprocess.run([
                    "./grapa", "-q", "-ccmd",
                    f"f=$file().ls('dist')[0].$KEY;$sys().shell('pip3 install dist/'+f);"
                ], check=True)
    
    def run_tests(self, config: BuildConfig):
        """Run tests"""
        print("Running tests...")
        
        # Run Grapa tests
        test_commands = [
            ["./grapa" if config.platform != "windows" else "grapa.exe", "-cfile", "test/run_tests.grc"],
            ["python3", "test/run_tests.py"]
        ]
        
        for cmd in test_commands:
            try:
                subprocess.run(cmd, check=True)
            except subprocess.CalledProcessError as e:
                print(f"Test failed: {e}")
                return False
        
        return True
    
    def build(self, run_tests: bool = False) -> bool:
        """Build for the current platform and architecture"""
        platform, arch = self.detect_platform()
        config = BuildConfig(platform, arch)
        
        print(f"Building Grapa for {config.target}...")
        
        # Build based on platform
        success = False
        if config.platform == "windows":
            success = self.build_windows(config)
        elif config.platform == "mac":
            success = self.build_mac(config)
        elif config.platform == "linux":
            success = self.build_linux_aws(config)
        elif config.platform == "aws":
            success = self.build_linux_aws(config)
        else:
            print(f"Unsupported platform: {config.platform}")
            return False
        
        if success:
            print(f"Build successful for {config.target}")
            
            # Build Python package
            self.build_python_package(config)
            
            # Run tests if requested
            if run_tests:
                self.run_tests(config)
            
            return True
        else:
            print(f"Build failed for {config.target}")
            return False

def main():
    parser = argparse.ArgumentParser(description="Grapa Build Script")
    parser.add_argument("--test", action="store_true", help="Run tests after build")
    parser.add_argument("--clean", action="store_true", help="Clean build artifacts")
    
    args = parser.parse_args()
    
    builder = GrapaBuilder()
    
    # Build for current platform only
    platform, arch = builder.detect_platform()
    print(f"Building for {platform} {arch}")
    
    if builder.build(args.test):
        print(f"\n{'='*50}")
        print(f"Build successful for {platform} {arch}")
        print(f"{'='*50}")
        return 0
    else:
        print(f"\n{'='*50}")
        print(f"Build failed for {platform} {arch}")
        print(f"{'='*50}")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 