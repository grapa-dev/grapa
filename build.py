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
    - mac (arm64 only - Apple Silicon required)
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
        
        # Define FLTK_USE_X11 for Linux/AWS builds since FLTK was built with X11 support
        if self.platform in ["linux", "aws"]:
            base_flags.append("-DFLTK_USE_X11")
        
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
            frameworks = [
                "-framework CoreFoundation",
                "-framework AppKit", 
                "-framework IOKit"
            ]
            
            # Always include ScreenCaptureKit for mac-arm64 (requires macOS 15.0+)
            frameworks.append("-framework ScreenCaptureKit")
            print("✅ Including ScreenCaptureKit framework for mac-arm64")
            
            return frameworks
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
            # On Mac, only support Apple Silicon (arm64)
            if machine == "arm64":
                return "mac", "arm64"
            else:
                raise RuntimeError(f"mac-amd64 is no longer supported. Please use an Apple Silicon Mac (arm64). Current architecture: {machine}")
        elif system == "linux":
            # Check if this is AWS Linux by looking for Amazon Linux specific files
            if (os.path.exists("/etc/system-release") and 
                ("Amazon Linux" in open("/etc/system-release").read())):
                return "aws", "arm64" if machine == "aarch64" else "amd64"
            else:
                return "linux", "arm64" if machine == "aarch64" else "amd64"
        else:
            raise RuntimeError(f"Unsupported platform: {system}")
    
    def build_windows(self, config: BuildConfig, exe_only: bool = False) -> bool:
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
            
            # Copy executable to bin directory
            os.makedirs(f"bin/{config.target}", exist_ok=True)
            shutil.copy("grapa.exe", f"bin/{config.target}/grapa.exe")
            
            if not exe_only:
                # Build static library
                subprocess.run([
                    "msbuild", "prj/winlib-amd64_static/grapalib.sln", "/p:Configuration=Release"
                ], check=True)
                # Copy static library
                if os.path.exists("grapa_static.lib"):
                    os.remove("grapa_static.lib")
                shutil.copy("prj/winlib-amd64_static/x64/Release/grapa.lib", "grapa_static.lib")
                
                # Build shared library (DLL)
                try:
                    subprocess.run([
                        "msbuild", "prj/winlib-amd64_shared/grapalib.sln", "/p:Configuration=Release"
                    ], check=True)
                    # Copy shared library
                    if os.path.exists("grapa.dll"):
                        os.remove("grapa.dll")
                    shutil.copy("prj/winlib-amd64_shared/x64/Release/grapa.dll", "grapa.dll")
                    print("✅ Built and copied grapa.dll")
                except subprocess.CalledProcessError as e:
                    print(f"⚠️  Shared library build failed: {e}")
                    print("Continuing with static library only...")
                
                # Ensure target directories exist
                target_lib_dir = f"bin/{config.target}"
                source_lib_dir = f"source/grapa-lib/{config.target}"
                os.makedirs(target_lib_dir, exist_ok=True)
                os.makedirs(source_lib_dir, exist_ok=True)
                
                # Copy static library to both locations
                shutil.copy("prj/winlib-amd64_static/x64/Release/grapa.lib", f"{target_lib_dir}/grapa_static.lib")
                shutil.copy("prj/winlib-amd64_static/x64/Release/grapa.lib", f"{source_lib_dir}/grapa_static.lib")
                
                # Copy shared library to both locations if it exists
                if os.path.exists("grapa.dll"):
                    shutil.copy("grapa.dll", f"{target_lib_dir}/grapa.dll")
                    shutil.copy("grapa.dll", f"{source_lib_dir}/grapa.dll")
                elif os.path.exists("prj/winlib-amd64_shared/x64/Release/grapa.dll"):
                    # If the DLL exists in the project directory, copy it directly
                    shutil.copy("prj/winlib-amd64_shared/x64/Release/grapa.dll", f"{target_lib_dir}/grapa.dll")
                    shutil.copy("prj/winlib-amd64_shared/x64/Release/grapa.dll", f"{source_lib_dir}/grapa.dll")
                # Clean build artifacts
                self._clean_windows_build()
                # Create package
                self._create_windows_package(config)
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Windows build failed: {e}")
            return False
    
    def build_mac(self, config: BuildConfig, exe_only: bool = False) -> bool:
        """Build for Mac using clang/clang++"""
        print(f"Building for {config.target} using clang++...")
        
        try:
            # Build main executable
            self._run_mac_build_command(config, is_library=False)
            
            # Build static library
            self._run_mac_build_command(config, is_library=True, is_static=True)
            
            # Build shared library (skip if exe_only)
            if not exe_only:
                self._run_mac_build_command(config, is_library=True, is_static=False)
            
            if not exe_only:
                # Create package
                self._create_mac_package(config)
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Mac build failed: {e}")
            return False
    
    def build_linux_aws(self, config: BuildConfig, exe_only: bool = False) -> bool:
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
            
            # Build shared library (skip if exe_only)
            if not exe_only:
                if config.platform == "aws":
                    self._run_aws_build_command(config, is_library=True, is_static=False)
                else:
                    self._run_linux_build_command(config, is_library=True, is_static=False)
            
            if not exe_only:
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
                subprocess.run(["ar", "-crs", "libgrapa_static.a"] + obj_files, check=True)
                
                # Copy to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy("libgrapa_static.a", f"bin/{config.target}/libgrapa_static.a")
                
                # Copy to source/grapa-lib directory
                os.makedirs(f"source/grapa-lib/{config.target}", exist_ok=True)
                shutil.copy("libgrapa_static.a", f"source/grapa-lib/{config.target}/libgrapa_static.a")
                
                os.remove("libgrapa_static.a")
            else:
                # Build shared library - match BUILD.md exactly
                print("Building shared library...")
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                cmd = [
                    "clang++", "-shared", "-Isource"
                ] + cpp_files + ["utf8proc.o"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + config.frameworks + [
                    "-std=c++17", "-m64", "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                ]
                print(f"Executing shared library build command: {' '.join(cmd)}")
                try:
                    result = os.system(" ".join(cmd))
                    if result != 0:
                        raise RuntimeError(f"Shared library build failed with exit code {result}")
                except Exception as e:
                    print(f"❌ Shared library build failed: {e}")
                    raise
                
                # Copy to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"bin/{config.target}/libgrapa.so")
                
                # Copy to source/grapa-lib directory
                os.makedirs(f"source/grapa-lib/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa.so")
                
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
                f"source/blst-lib/{config.target}/*.a", f"source/pcre2-lib/{config.target}/libpcre2-8.a"
            ] + config.frameworks + [
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
                
                # Copy executable to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy(config.output_name, f"bin/{config.target}/{config.output_name}")
            else:
                print(f"❌ Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
    
    def _get_x11_libs(self):
        """Get X11 libraries based on FLTK configuration"""
        try:
            # Check if FLTK_USE_X11 is defined in the configuration
            with open("source/FL/fl_config.h", "r") as f:
                config_content = f.read()
                if "define FLTK_USE_X11" in config_content:
                    return ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama", "-lfontconfig", "-lXcursor"]
                else:
                    # Even if FLTK_USE_X11 is not defined, the library might still contain X11 code
                    # Check if the FLTK library contains X11 symbols
                    import subprocess
                    try:
                        result = subprocess.run(["nm", "-D", "source/fl-lib/linux-arm64/libfltk.a"], 
                                              capture_output=True, text=True, timeout=10)
                        if "X11" in result.stdout or "Xft" in result.stdout:
                            print("⚠️  FLTK library contains X11 symbols, including X11 libraries")
                            return ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama", "-lfontconfig", "-lXcursor"]
                    except:
                        pass
                    print("⚠️  FLTK_USE_X11 not defined, skipping X11 libraries")
                    return []
        except FileNotFoundError:
            print("⚠️  FLTK config not found, skipping X11 libraries")
            return []

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
                    "g++"
                ] + config.flags + [
                    "-c"
                ] + cpp_files + [
                    "-fPIC"
                ], check=True)
                # Get all .o files (including utf8proc.o if it exists)
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                subprocess.run(["ar", "-crs", "libgrapa_static.a"] + obj_files, check=True)
                
                # Copy to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy("libgrapa_static.a", f"bin/{config.target}/libgrapa_static.a")
                
                # Copy to source/grapa-lib directory
                os.makedirs(f"source/grapa-lib/{config.target}", exist_ok=True)
                shutil.copy("libgrapa_static.a", f"source/grapa-lib/{config.target}/libgrapa_static.a")
                
                os.remove("libgrapa_static.a")
            else:
                # Build shared library
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                # Get X11 libraries conditionally
                x11_libs = self._get_x11_libs()
                
                cmd = ["g++", "-shared"] + config.flags + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    f"-Lsource/openssl-lib/{config.target}", "-lcrypto"
                ] + x11_libs + [
                    "-ldl", "-lm", "-static-libgcc",
                    "-fPIC", "-o", "libgrapa.so"
                ]
                
                subprocess.run(cmd, check=True)
                
                # Copy to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"bin/{config.target}/libgrapa.so")
                
                # Copy to source/grapa-lib directory
                os.makedirs(f"source/grapa-lib/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa.so")
                
                os.remove("libgrapa.so")
        else:
            # Build executable - match AWS pattern exactly
            cpp_files = glob.glob("source/grapa/*.cpp")
            openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
            fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
            blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
            pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
            
            # Get X11 libraries conditionally
            x11_libs = self._get_x11_libs()
            
            cmd = [
                "g++"
            ] + config.flags + [
                "source/main.cpp"
            ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + [
                f"source/pcre2-lib/{config.target}/libpcre2-8.a", f"-Lsource/openssl-lib/{config.target}", "-lcrypto"
            ] + x11_libs + [
                "-ldl", "-lm", "-static-libgcc", 
                "-o", config.output_name
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
                
                # Copy executable to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy(config.output_name, f"bin/{config.target}/{config.output_name}")
            else:
                print(f"❌ Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
    
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
                    "g++"
                ] + config.flags + [
                    "-c"
                ] + cpp_files + [
                    "-fPIC"
                ], check=True)
                # Get all .o files
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                subprocess.run(["ar", "-crs", "libgrapa_static.a"] + obj_files, check=True)
                
                # Copy to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy("libgrapa_static.a", f"bin/{config.target}/libgrapa_static.a")
                
                # Copy to source/grapa-lib directory
                os.makedirs(f"source/grapa-lib/{config.target}", exist_ok=True)
                shutil.copy("libgrapa_static.a", f"source/grapa-lib/{config.target}/libgrapa_static.a")
                
                os.remove("libgrapa_static.a")
            else:
                # Build shared library - match original working script exactly
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                # Get X11 libraries conditionally
                x11_libs = self._get_x11_libs()
                
                cmd = ["g++", "-shared"] + config.flags + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    f"-Lsource/openssl-lib/{config.target}", "-lcrypto"
                ] + x11_libs + [
                    "-ldl", "-lm", "-static-libgcc",
                    "-fPIC", "-o", "libgrapa.so"
                ]
                
                print(f"Executing shared library build command: {' '.join(cmd)}")
                subprocess.run(cmd, check=True)
                
                # Copy to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"bin/{config.target}/libgrapa.so")
                
                # Copy to source/grapa-lib directory
                os.makedirs(f"source/grapa-lib/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa.so")
                
                os.remove("libgrapa.so")
        else:
            # Build executable
            cpp_files = glob.glob("source/grapa/*.cpp")
            openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
            fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
            blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
            pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
            
            # Get X11 libraries conditionally
            x11_libs = self._get_x11_libs()
            
            cmd = [
                "g++"
            ] + config.flags + [
                "source/main.cpp"
            ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + [
                f"source/pcre2-lib/{config.target}/libpcre2-8.a", f"-Lsource/openssl-lib/{config.target}", "-lcrypto"
            ] + x11_libs + [
                "-ldl", "-lm", "-static-libgcc", 
                "-o", config.output_name
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
                
                # Copy executable to bin directory
                os.makedirs(f"bin/{config.target}", exist_ok=True)
                shutil.copy(config.output_name, f"bin/{config.target}/{config.output_name}")
            else:
                print(f"❌ Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
    
    def _clean_build_artifacts(self, preserve_dist: bool = False):
        """Clean build artifacts that should be removed after build"""
        print("Cleaning build artifacts...")
        
        # Clean Python package artifacts
        if os.path.exists("dist") and not preserve_dist:
            print("Removing dist/ directory...")
            shutil.rmtree("dist")
        
        if os.path.exists("grapapy.egg-info"):
            print("Removing grapapy.egg-info/ directory...")
            shutil.rmtree("grapapy.egg-info")
        
        # Clean Python bytecode cache
        for pycache_dir in Path(".").glob("**/__pycache__"):
            print(f"Removing {pycache_dir}...")
            shutil.rmtree(pycache_dir)
        
        # Clean object files (but preserve executable)
        for obj_file in Path(".").glob("*.o"):
            obj_file.unlink()
    
    def _clean_windows_build(self):
        """Clean Windows build artifacts"""
        build_dirs = [
            "prj/win-amd64/x64",
            "prj/win-amd64/grapa",
            "prj/winlib-amd64_static/x64",
            "prj/winlib-amd64_static/grapalib",
            "prj/winlib-amd64_shared/x64",
            "prj/winlib-amd64_shared/grapalib"
        ]
        for dir_path in build_dirs:
            if os.path.exists(dir_path):
                shutil.rmtree(dir_path)
    
    def _create_windows_package(self, config: BuildConfig):
        """Create Windows package - replace files in bin/grapa-win-amd64 and create zip"""
        # Create the target directory if it doesn't exist
        target_dir = f"bin/grapa-{config.target}"
        os.makedirs(target_dir, exist_ok=True)
        
        # Copy the newly built files to the target directory
        if os.path.exists("grapa.exe"):
            shutil.copy("grapa.exe", f"{target_dir}/grapa.exe")
            print(f"✅ Copied grapa.exe to {target_dir}/")
        
        if os.path.exists("grapa_static.lib"):
            shutil.copy("grapa_static.lib", f"{target_dir}/grapa_static.lib")
            print(f"✅ Copied grapa_static.lib to {target_dir}/")
            
        # Copy DLL if it exists
        if os.path.exists("grapa.dll"):
            shutil.copy("grapa.dll", f"{target_dir}/grapa.dll")
            print(f"✅ Copied grapa.dll to {target_dir}/")
        
        # Copy installation files
        installer_dir = "packaging/windows-installer"
        if os.path.exists(f"{installer_dir}/install-grapa.ps1"):
            shutil.copy(f"{installer_dir}/install-grapa.ps1", f"{target_dir}/install-grapa.ps1")
            print(f"✅ Copied install-grapa.ps1 to {target_dir}/")
        
        if os.path.exists(f"{installer_dir}/README-Windows-Installation.md"):
            shutil.copy(f"{installer_dir}/README-Windows-Installation.md", f"{target_dir}/README-Windows-Installation.md")
            print(f"✅ Copied README-Windows-Installation.md to {target_dir}/")
        
        # Create zip file
        zip_path = f"bin/grapa-{config.target}.zip"
        if os.path.exists(zip_path):
            os.remove(zip_path)
        
        # Create zip with all files in the directory
        import zipfile
        with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
            for root, dirs, files in os.walk(target_dir):
                for file in files:
                    file_path = os.path.join(root, file)
                    arcname = os.path.relpath(file_path, target_dir)
                    zipf.write(file_path, arcname)
        
        print(f"✅ Windows package files updated in {target_dir}/")
        print(f"✅ Created zip file: {zip_path}")
    
    def _create_mac_package(self, config: BuildConfig):
        """Create Mac package"""
        import glob
        
        # Get the actual files to include
        files_to_include = []
        
        # Add executable if it exists
        if os.path.exists(config.output_name):
            files_to_include.append(config.output_name)
        
                # Add library files
        lib_files = glob.glob(f"bin/{config.target}/*")
        files_to_include.extend(lib_files)
        
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
        lib_files = glob.glob(f"bin/{config.target}/*")
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

        # Build package
        python_cmd = "python" if config.platform == "windows" else "python3"
        pip_cmd = "pip" if config.platform == "windows" else "pip3"
        subprocess.run([python_cmd, "setup.py", "sdist"], check=True)

        # Find the built package file
        dist_files = list(Path("dist").glob("*.tar.gz"))
        if not dist_files:
            raise RuntimeError("No Python package found in dist/ directory")

        package_file = dist_files[0].name
        print(f"Found package: {package_file}")

        # Install package
        package_path = os.path.join("dist", package_file)
        subprocess.run([pip_cmd, "install", package_path], check=True)
    
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
    
    def build(self, run_tests: bool = False, exe_only: bool = False, build_python: bool = False, python_only: bool = False) -> bool:
        """Build for the current platform and architecture"""
        platform, arch = self.detect_platform()
        config = BuildConfig(platform, arch)
        
        print(f"Building Grapa for {config.target}...")
        
        try:
            # If python_only is True, skip the main build and just build Python package
            if python_only:
                print("Skipping main build, building Python package only...")
                if self._check_libraries_exist(config):
                    print("✅ Required libraries found, building Python package...")
                    self.build_python_package(config)
                    return True
                else:
                    print("❌ Required libraries not found. Please run full build first.")
                    return False
            
            # Build based on platform
            success = False
            if config.platform == "windows":
                success = self.build_windows(config, exe_only=exe_only)
            elif config.platform == "mac":
                success = self.build_mac(config, exe_only=exe_only)
            elif config.platform == "linux":
                success = self.build_linux_aws(config, exe_only=exe_only)
            elif config.platform == "aws":
                success = self.build_linux_aws(config, exe_only=exe_only)
            else:
                print(f"Unsupported platform: {config.platform}")
                return False
            
            if success:
                print(f"Build successful for {config.target}")

                if not exe_only:
                    # Build Python package if requested
                    if build_python:
                        self.build_python_package(config)
                    # Run tests if requested
                    if run_tests:
                        self.run_tests(config)

                return True
            else:
                print(f"Build failed for {config.target}")
                return False
        finally:
            # Always clean up build artifacts, regardless of success or failure
            # But preserve dist/ directory if we built Python packages
            if not build_python and not python_only:
                self._clean_build_artifacts()
            else:
                # Clean everything except dist/ for Python builds
                self._clean_build_artifacts(preserve_dist=True)
    
    def _check_libraries_exist(self, config: BuildConfig) -> bool:
        """Check if required libraries exist for Python package build"""
        if config.platform == "windows":
            # Windows uses .lib and .dll files
            required_files = [
                f"bin/{config.target}/grapa_static.lib",
                f"bin/{config.target}/grapa.dll",
                f"bin/{config.target}/grapa.exe"
            ]
        else:
            # Non-Windows uses .a and .so files
            required_files = [
                f"bin/{config.target}/libgrapa_static.a",
                f"bin/{config.target}/libgrapa.so",
                f"bin/{config.target}/grapa"
            ]
        
        for file_path in required_files:
            if not os.path.exists(file_path):
                print(f"❌ Missing required file: {file_path}")
                return False
        
        print("✅ All required libraries found")
        return True

def main():
    parser = argparse.ArgumentParser(description="Grapa Build Script")
    parser.add_argument("--test", action="store_true", help="Run tests after build")
    parser.add_argument("--clean", action="store_true", help="Clean build artifacts")
    parser.add_argument("--exe-only", action="store_true", help="Build only the main executable (skip library, Python package, and packaging steps). Useful for fast iterative development and investigation.")
    parser.add_argument("--python", action="store_true", help="Build Python extension package (after building executable and libraries)")
    parser.add_argument("--python-only", action="store_true", help="Only build the Python package, skipping main executable and library builds. Requires libraries to already exist.")
    
    args = parser.parse_args()
    
    # Validate argument combinations
    if args.python_only and (args.exe_only or args.test or args.python):
        print("❌ Error: --python-only cannot be used with other build flags")
        print("   Use --python-only by itself to build only the Python package")
        return 1
    
    builder = GrapaBuilder()
    
    # Build for current platform only
    platform, arch = builder.detect_platform()
    print(f"Building for {platform} {arch}")
    
    if builder.build(args.test, exe_only=args.exe_only, build_python=args.python, python_only=args.python_only):
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