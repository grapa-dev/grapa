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
    python3 build.py --exe-only         # Build only the executable (skip libraries, Python package)
    python3 build.py --lib-only         # Build only the libraries (skip executable, Python package)
    python3 build.py --python-only      # Build only the Python extension (assumes executable exists)
    python3 build.py --preserve-dist    # Preserve dist/ directory after build
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
        # Check for CI/CD environment variable first
        ci_platform = os.environ.get('CI_PLATFORM')
        if ci_platform:
            # Parse platform like "linux-arm64", "mac-amd64", etc.
            if '-' in ci_platform:
                platform_name, arch = ci_platform.split('-', 1)
                # Map "win" to "windows" for compatibility
                if platform_name == "win":
                    platform_name = "windows"
                return platform_name, arch
            else:
                print(f"Warning: CI_PLATFORM={ci_platform} is not in expected format (platform-arch)")
        
        # Fall back to automatic detection
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
    
    def build_windows(self, config: BuildConfig, exe_only: bool = False, lib_only: bool = False) -> bool:
        """Build for Windows using Visual Studio"""
        print(f"Building for {config.target} using Visual Studio...")
        
        try:
            # Find msbuild in common locations
            msbuild_paths = [
                "msbuild",  # Try PATH first
                "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe",
                "C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\MSBuild\\Current\\Bin\\MSBuild.exe",
                "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\MSBuild\\Current\\Bin\\MSBuild.exe",
                "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe",
                "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\MSBuild\\Current\\Bin\\MSBuild.exe",
                "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Enterprise\\MSBuild\\Current\\Bin\\MSBuild.exe",
            ]
            
            msbuild_exe = None
            for path in msbuild_paths:
                try:
                    if path == "msbuild":
                        # Test if msbuild is in PATH
                        subprocess.run(["msbuild", "/version"], capture_output=True, check=True)
                        msbuild_exe = "msbuild"
                        break
                    elif os.path.exists(path):
                        msbuild_exe = path
                        break
                except (subprocess.CalledProcessError, FileNotFoundError):
                    continue
            
            if not msbuild_exe:
                raise RuntimeError("Could not find msbuild. Please ensure Visual Studio is installed.")
            
            print(f"Using msbuild: {msbuild_exe}")
            
            if not lib_only:
                # Build main executable
                subprocess.run([
                    msbuild_exe, "prj/win-amd64/grapa.sln", "/p:Configuration=Release"
                ], check=True)
                
                # Copy executable
                if os.path.exists("grapa.exe"):
                    os.remove("grapa.exe")
                shutil.copy("prj/win-amd64/x64/Release/grapa.exe", "grapa.exe")
            
            if not exe_only:
                # Build library (utf8proc.c is already included in the MSBuild project)
                subprocess.run([
                    msbuild_exe, "prj/winlib-amd64/grapalib.sln", "/p:Configuration=Release"
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
    
    def build_mac(self, config: BuildConfig, exe_only: bool = False, lib_only: bool = False) -> bool:
        """Build for Mac using clang/clang++"""
        print(f"Building for {config.target} using clang++...")
        
        try:
            if not lib_only:
                # Build main executable
                self._run_mac_build_command(config, is_library=False)
            
            if not exe_only:
                # Build static library
                self._run_mac_build_command(config, is_library=True, is_static=True)
                
                # Build shared library
                self._run_mac_build_command(config, is_library=True, is_static=False)
                
                # Copy library files to top-level directory
                self._copy_libraries_to_top_level(config)
                
                # Create package
                self._create_mac_package(config)
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Mac build failed: {e}")
            return False
    
    def build_linux_aws(self, config: BuildConfig, exe_only: bool = False, lib_only: bool = False) -> bool:
        """Build for Linux/AWS using g++"""
        print(f"Building for {config.target} using g++...")
        
        try:
            if not lib_only:
                # Build main executable
                if config.platform == "aws":
                    self._run_aws_build_command(config, is_library=False)
                else:
                    self._run_linux_build_command(config, is_library=False)
            
            if not exe_only or lib_only:
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
                
                # Copy library files to top-level directory
                self._copy_libraries_to_top_level(config)
                
                # Create package (only if not lib_only)
                if not lib_only:
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
        
        # Check if this is cross-compilation (mac-amd64 on ARM64 runner)
        is_cross_compile = config.target == "mac-amd64" and platform.machine() == "arm64"
        
        # Set up cross-compilation flags if needed
        cross_flags = []
        if is_cross_compile:
            print("Cross-compiling for AMD64 from ARM64...")
            cross_flags = ["-target", "x86_64-apple-macos10.9"]
        
        # Build utf8proc first (C compilation)
        print("Building utf8proc...")
        subprocess.run([
            "clang", "-Isource", "-DUTF8PROC_STATIC", "-c", 
            "source/utf8proc/utf8proc.c", "-m64", "-O3"
        ] + cross_flags, check=True)
        
        if is_library:
            if is_static:
                # Build static library - match BUILD.md exactly
                print("Building static library...")
                cpp_files = glob.glob("source/grapa/*.cpp")
                subprocess.run([
                    "clang++", "-Isource", "-c"
                ] + cpp_files + [
                    "-std=c++17", "-m64", "-O3", "-pthread"
                ] + cross_flags, check=True)
                # Get all .o files
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                subprocess.run(["libtool", "-static", "-o", "libgrapa.a"] + obj_files, check=True)
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
                ] + cross_flags, check=True)
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
            # Step 2: Build executable using utf8proc.o - expand globs properly
            cmd = ["clang++", "-Isource", "source/main.cpp"] + cpp_files + ["utf8proc.o"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                "-framework", "CoreFoundation", "-framework", "AppKit", "-framework", "IOKit",
                "-std=c++17", "-m64", "-O3", "-pthread", "-o", config.output_name
            ] + cross_flags
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            try:
                # Use subprocess.run() to properly handle the command
                subprocess.run(cmd, check=True)
            except subprocess.CalledProcessError as e:
                print(f"❌ Build failed: {e}")
                raise RuntimeError(f"Build failed with exit code {e.returncode}")
            except Exception as e:
                print(f"❌ Build failed: {e}")
                raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"✅ Executable created: {config.output_name}")
            else:
                print(f"❌ Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
    
    def _run_linux_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
        """Run Linux build command (Ubuntu)"""
        import glob
        
        # Remove existing executable only when building executable
        if not is_library and os.path.exists(config.output_name):
            os.remove(config.output_name)
        
        print(f"Building {'library' if is_library else 'executable'} for {config.target}...")
        
        # Check if this is cross-compilation (linux-arm64 on AMD64 runner)
        is_cross_compile = config.target == "linux-arm64" and platform.machine() == "x86_64"
        
        # Set up cross-compilation flags if needed
        cross_flags = []
        cross_compiler_prefix = ""
        if is_cross_compile:
            print("Cross-compiling for ARM64 from AMD64...")
            # Check if cross-compilation toolchain is available
            try:
                subprocess.run(["aarch64-linux-gnu-gcc", "--version"], check=True, capture_output=True)
                cross_compiler_prefix = "aarch64-linux-gnu-"
                print("Using cross-compilation toolchain")
                # Add ARM64 library paths for cross-compilation
                cross_flags = ["-L/usr/lib/aarch64-linux-gnu", "-L/usr/lib/x86_64-linux-gnu"]
                
                # Check if sysroot is available (Option 2 approach)
                sysroot_path = "./arm64-root"
                if os.path.exists(sysroot_path):
                    print(f"Using ARM64 sysroot at: {sysroot_path}")
                    # Use sysroot for cross-compilation
                    cross_flags.extend([
                        f"--sysroot={sysroot_path}",
                        "-I" + os.path.join(sysroot_path, "usr/include"),
                        "-L" + os.path.join(sysroot_path, "usr/lib/aarch64-linux-gnu"),
                        "-L" + os.path.join(sysroot_path, "lib/aarch64-linux-gnu")
                    ])
                    arm64_libs_available = True
                    print("ARM64 sysroot libraries available, using dynamic linking")
                else:
                    # Fallback to static linking if no sysroot
                    print("Warning: ARM64 sysroot not available, will use static linking approach")
                    cross_flags.extend(["-static", "-static-libgcc", "-static-libstdc++"])
                    system_libs = ["-ldl", "-lm", "-static-libgcc"]
                    arm64_libs_available = False
            except (subprocess.CalledProcessError, FileNotFoundError):
                print("Cross-compilation toolchain not available, using native compiler with ARM flags")
                cross_flags = ["-march=armv8-a", "-mtune=cortex-a72"]
        
        # Build utf8proc first (C compilation)
        print("Building utf8proc...")
        # Use -fPIC for shared library builds, regular for executable
        pic_flag = ["-fPIC"] if is_library else []
        gcc_cmd = f"{cross_compiler_prefix}gcc" if cross_compiler_prefix else "gcc"
        gpp_cmd = f"{cross_compiler_prefix}g++" if cross_compiler_prefix else "g++"
        subprocess.run([
            gcc_cmd, "-Isource", "-DUTF8PROC_STATIC", "-c", 
            "source/utf8proc/utf8proc.c", "-O3"
        ] + pic_flag + cross_flags, check=True)
        
        # Get library path based on target
        lib_path = f"source/openssl-lib/{config.target}"
        
        if is_library:
            if is_static:
                # Build static library
                cpp_files = glob.glob("source/grapa/*.cpp")
                
                # Clean any existing object files first
                for obj_file in glob.glob("*.o"):
                    os.remove(obj_file)
                
                subprocess.run([
                    gpp_cmd, "-Isource", "-c"
                ] + cpp_files + [
                    "-std=c++17", "-O3", "-pthread", "-fPIC"
                ] + cross_flags, check=True)
                # Get all .o files (including utf8proc.o if it exists)
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                
                # Remove any existing libgrapa.a to avoid conflicts
                if os.path.exists("libgrapa.a"):
                    os.remove("libgrapa.a")
                
                ar_cmd = f"{cross_compiler_prefix}ar" if cross_compiler_prefix else "ar"
                subprocess.run([ar_cmd, "-crs", "libgrapa.a"] + obj_files, check=True)
                shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa.a")
                os.remove("libgrapa.a")
            else:
                # Build shared library
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                # For ARM64 cross-compilation, use ARM64 X11 libraries
                if is_cross_compile:
                    # Check if we're using static linking (no X11 libraries available)
                    if "-static" in cross_flags:
                        system_libs = ["-ldl", "-lm", "-static-libgcc"]
                    elif arm64_libs_available and "--sysroot" in cross_flags:
                        # Using sysroot - avoid X11 libraries for now, focus on core functionality
                        system_libs = ["-ldl", "-lm", "-static-libgcc"]
                    else:
                        system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                                      "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc"]
                else:
                    system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                                  "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc"]
                
                cmd = [gpp_cmd, "-shared", "-Isource", "-DUTF8PROC_STATIC"] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto"
                ] + system_libs + [
                    "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                ] + cross_flags
                
                # Add -ljpeg for both native and cross-compilation builds (unless doing static linking)
                if "-static" not in cross_flags:
                    cmd.insert(-2, "-ljpeg")
                
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
            
            # For ARM64 cross-compilation, use ARM64 X11 libraries
            if is_cross_compile:
                # Check if we're using static linking (no X11 libraries available)
                if "-static" in cross_flags:
                    system_libs = ["-ldl", "-lm", "-static-libgcc"]
                elif arm64_libs_available and "--sysroot" in cross_flags:
                    # Using sysroot - avoid X11 libraries for now, focus on core functionality
                    system_libs = ["-ldl", "-lm", "-static-libgcc"]
                else:
                    system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                                  "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc"]
            else:
                system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                              "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc"]
            
            cmd = [
                gpp_cmd, "-Isource", "-DUTF8PROC_STATIC", "source/main.cpp"
            ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto"
            ] + system_libs + [
                "-O3", "-pthread", "-o", config.output_name
            ] + cross_flags
            
            # Add -ljpeg for both native and cross-compilation builds (unless doing static linking)
            if "-static" not in cross_flags:
                cmd.insert(-2, "-ljpeg")
            
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            
            # Add debugging logging for Linux ARM64 cross-compilation
            if is_cross_compile:
                print("Writing build command to build-debug.log for Linux ARM64 debugging...")
                build_log = Path("build-debug.log")
                with build_log.open("w") as f:
                    f.write("Running command:\n")
                    f.write(" ".join(cmd) + "\n\n")
                    f.write("Environment:\n")
                    for key, value in os.environ.items():
                        if any(x in key.upper() for x in ['PATH', 'CC', 'CXX', 'ARCH', 'PLATFORM']):
                            f.write(f"{key}={value}\n")
                    f.write("\n")
                    
                    try:
                        process = subprocess.run(cmd, stdout=f, stderr=subprocess.STDOUT, text=True)
                        f.write(f"\nExit code: {process.returncode}\n")
                        if process.returncode != 0:
                            raise RuntimeError(f"Build failed with exit code {process.returncode}")
                    except Exception as e:
                        f.write(f"Exception occurred: {str(e)}\n")
                        raise
            else:
                try:
                    # Use subprocess.run() to properly handle the command
                    subprocess.run(cmd, check=True)
                except subprocess.CalledProcessError as e:
                    print(f"❌ Build failed: {e}")
                    raise RuntimeError(f"Build failed with exit code {e.returncode}")
                except Exception as e:
                    print(f"❌ Build failed: {e}")
                    raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"✅ Executable created: {config.output_name}")
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
                
                # Clean any existing object files first
                for obj_file in glob.glob("*.o"):
                    os.remove(obj_file)
                
                subprocess.run([
                    "g++", "-Isource", "-c"
                ] + cpp_files + [
                    "-std=c++17", "-O3", "-pthread"
                ], check=True)
                # Get all .o files
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                
                # Remove any existing libgrapa.a to avoid conflicts
                if os.path.exists("libgrapa.a"):
                    os.remove("libgrapa.a")
                
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
            ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto", 
                "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama", 
                "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc", 
                "-O3", "-pthread", "-o", config.output_name
            ]
            
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            try:
                # Use subprocess.run() to properly handle the command
                subprocess.run(cmd, check=True)
            except subprocess.CalledProcessError as e:
                print(f"❌ Build failed: {e}")
                raise RuntimeError(f"Build failed with exit code {e.returncode}")
            except Exception as e:
                print(f"❌ Build failed: {e}")
                raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"✅ Executable created: {config.output_name}")
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
        elif preserve_dist and os.path.exists("dist"):
            print("Preserving dist/ directory as requested")
        
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
    
    def build_python_package(self, config: BuildConfig, preserve_dist: bool = False):
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
        
        if preserve_dist:
            print("Preserving dist/ directory as requested")
        else:
            print("Cleaning dist/ directory...")
            shutil.rmtree("dist")
    
    def build_python_only(self, config: BuildConfig, preserve_dist: bool = False) -> bool:
        """Build only the Python extension (assumes executable already exists)"""
        print("Building Python extension only...")
        
        # Check if executable exists
        exe_name = "grapa.exe" if config.platform == "windows" else "grapa"
        if not os.path.exists(exe_name):
            print(f"Warning: {exe_name} not found. Python extension build may fail.")
            print("Consider running full build first to ensure executable exists.")
        
        try:
            # Build Python package
            self.build_python_package(config, preserve_dist=preserve_dist)
            print("Python extension build successful")
            return True
        except Exception as e:
            print(f"Python extension build failed: {e}")
            return False
    
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
    
    def _copy_libraries_to_top_level(self, config: BuildConfig):
        """Copy compiled library files to the top-level directory"""
        print("Copying library files to top-level directory...")
        
        if config.platform == "windows":
            # Windows only has static library
            lib_path = f"source/grapa-lib/{config.target}/grapa.lib"
            if os.path.exists(lib_path):
                shutil.copy(lib_path, "grapa.lib")
                print("✅ Copied grapa.lib to top-level directory")
        else:
            # Mac/Linux/AWS have both static and shared libraries
            static_lib_path = f"source/grapa-lib/{config.target}/libgrapa.a"
            shared_lib_path = f"source/grapa-lib/{config.target}/libgrapa.so"
            
            if os.path.exists(static_lib_path):
                shutil.copy(static_lib_path, "libgrapa.a")
                print("✅ Copied libgrapa.a to top-level directory")
            
            if os.path.exists(shared_lib_path):
                shutil.copy(shared_lib_path, "libgrapa.so")
                print("✅ Copied libgrapa.so to top-level directory")

    def build_libraries_only(self, config: BuildConfig) -> bool:
        """Build only the libraries (skip executable and Python package)"""
        print("Building libraries only...")
        
        try:
            # Build based on platform
            success = False
            if config.platform == "windows":
                success = self.build_windows(config, exe_only=True, lib_only=True)
            elif config.platform == "mac":
                success = self.build_mac(config, exe_only=True, lib_only=True)
            elif config.platform == "linux":
                success = self.build_linux_aws(config, exe_only=True, lib_only=True)
            elif config.platform == "aws":
                success = self.build_linux_aws(config, exe_only=True, lib_only=True)
            else:
                print(f"Unsupported platform: {config.platform}")
                return False
            
            if success:
                print("Libraries build successful")
                return True
            else:
                print("Libraries build failed")
                return False
        except Exception as e:
            print(f"Libraries build failed: {e}")
            return False

    def build(self, run_tests: bool = False, exe_only: bool = False, lib_only: bool = False, python_only: bool = False, preserve_dist: bool = False) -> bool:
        """Build for the current platform and architecture"""
        platform, arch = self.detect_platform()
        config = BuildConfig(platform, arch)
        
        print(f"Building Grapa for {config.target}...")
        
        try:
            if python_only:
                # Build only Python extension
                return self.build_python_only(config, preserve_dist=preserve_dist)
            
            if lib_only:
                # Build only libraries
                return self.build_libraries_only(config)
            
            # Build based on platform
            success = False
            if config.platform == "windows":
                success = self.build_windows(config, exe_only=exe_only, lib_only=False)
            elif config.platform == "mac":
                success = self.build_mac(config, exe_only=exe_only, lib_only=False)
            elif config.platform == "linux":
                success = self.build_linux_aws(config, exe_only=exe_only, lib_only=False)
            elif config.platform == "aws":
                success = self.build_linux_aws(config, exe_only=exe_only, lib_only=False)
            else:
                print(f"Unsupported platform: {config.platform}")
                return False
            
            if success:
                print(f"Build successful for {config.target}")
                
                if not exe_only:
                    # Build Python package
                    self.build_python_package(config, preserve_dist=preserve_dist)
                    # Run tests if requested
                    if run_tests:
                        self.run_tests(config)
                
                return True
            else:
                print(f"Build failed for {config.target}")
                return False
        finally:
            # Always clean up build artifacts, regardless of success or failure
            self._clean_build_artifacts(preserve_dist=preserve_dist)

def main():
    parser = argparse.ArgumentParser(description="Grapa Build Script")
    parser.add_argument("--test", action="store_true", help="Run tests after build")
    parser.add_argument("--clean", action="store_true", help="Clean build artifacts")
    parser.add_argument("--exe-only", action="store_true", help="Build only the main executable (skip libraries, Python package, and packaging steps). Useful for fast iterative development and investigation.")
    parser.add_argument("--lib-only", action="store_true", help="Build only the libraries (skip executable, Python package, and packaging steps). Libraries will be copied to the top-level directory.")
    parser.add_argument("--python-only", action="store_true", help="Build only the Python extension (assumes executable already exists). Useful for debugging Python extension issues without rebuilding the executable.")
    parser.add_argument("--preserve-dist", action="store_true", help="Preserve the dist/ directory after build (useful for debugging or manual installation)")
    
    args = parser.parse_args()
    
    builder = GrapaBuilder()
    
    # Get platform and architecture for reporting
    platform, arch = builder.detect_platform()
    
    if builder.build(args.test, exe_only=args.exe_only, lib_only=args.lib_only, python_only=args.python_only, preserve_dist=args.preserve_dist):
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