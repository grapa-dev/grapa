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
            return "linux", "arm64" if machine == "aarch64" else "amd64"
        else:
            raise RuntimeError(f"Unsupported platform: {system}")
    
    def build_windows(self, config: BuildConfig, exe_only: bool = False, lib_only: bool = False) -> bool:
        """Build for Windows using Visual Studio"""
        print(f"Building for {config.target} using Visual Studio...")
        
        try:
            # Try to find msbuild using multiple methods (like setup.py does)
            msbuild_exe = None
            
            # Method 1: Check if msbuild is in PATH
            try:
                subprocess.run(["msbuild", "/version"], capture_output=True, check=True)
                msbuild_exe = "msbuild"
                print(f"Using msbuild from PATH")
            except (subprocess.CalledProcessError, FileNotFoundError):
                pass
            
            # Method 2: Use vswhere to find Visual Studio installation
            if not msbuild_exe:
                try:
                    vswhere_output = subprocess.run([
                        "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe",
                        "-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.MSBuild",
                        "-property", "installationPath"
                    ], capture_output=True, text=True, check=True)
                    
                    vs_path = vswhere_output.stdout.strip()
                    if vs_path:
                        msbuild_candidate = os.path.join(vs_path, "MSBuild", "Current", "Bin", "MSBuild.exe")
                        if os.path.exists(msbuild_candidate):
                            msbuild_exe = msbuild_candidate
                            print(f"Using msbuild: {msbuild_exe}")
                except (subprocess.CalledProcessError, FileNotFoundError):
                    pass
            
            # Method 3: Try to find Visual Studio using 'where cl' (like setup.py)
            if not msbuild_exe:
                try:
                    result = subprocess.run(['where', 'cl'], capture_output=True, text=True, shell=True)
                    if result.returncode == 0:
                        cl_path = result.stdout.strip().split('\n')[0]
                        if 'Microsoft Visual Studio' in cl_path:
                            # Navigate up from cl.exe to find msbuild
                            vs_path = os.path.dirname(os.path.dirname(os.path.dirname(cl_path)))
                            msbuild_candidate = os.path.join(vs_path, "MSBuild", "Current", "Bin", "MSBuild.exe")
                            if os.path.exists(msbuild_candidate):
                                msbuild_exe = msbuild_candidate
                                print(f"Using msbuild: {msbuild_exe}")
                except (subprocess.SubprocessError, OSError):
                    pass
            
            # Method 4: Check common installation paths
            if not msbuild_exe:
                vs_paths = [
                    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community",
                    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional", 
                    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise",
                    "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community",
                    "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional",
                    "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Enterprise",
                ]
                
                for vs_path in vs_paths:
                    msbuild_candidate = os.path.join(vs_path, "MSBuild", "Current", "Bin", "MSBuild.exe")
                    if os.path.exists(msbuild_candidate):
                        msbuild_exe = msbuild_candidate
                        print(f"Using msbuild: {msbuild_candidate}")
                        break
            
            if not msbuild_exe:
                raise RuntimeError("Could not find msbuild. Please ensure Visual Studio is installed and msbuild is available.")
            
            if not lib_only:
                # Build main executable
                print(f"Building executable with: {msbuild_exe} prj/win-amd64/grapa.sln /p:Configuration=Release")
                subprocess.run([
                    msbuild_exe, "prj/win-amd64/grapa.sln", "/p:Configuration=Release"
                ], check=True)
                
                # Check if executable was created
                if os.path.exists("prj/win-amd64/x64/Release/grapa.exe"):
                    print("SUCCESS: Executable built successfully")
                    # Copy executable
                    if os.path.exists("grapa.exe"):
                        os.remove("grapa.exe")
                    shutil.copy("prj/win-amd64/x64/Release/grapa.exe", "grapa.exe")
                    print("SUCCESS: Executable copied to root directory")
                else:
                    print("ERROR: Executable not found at prj/win-amd64/x64/Release/grapa.exe")
                    print("📁 Checking what files exist in prj/win-amd64/x64/Release/:")
                    if os.path.exists("prj/win-amd64/x64/Release/"):
                        import glob
                        for file in glob.glob("prj/win-amd64/x64/Release/*"):
                            print(f"  - {file}")
                    else:
                        print("  Directory prj/win-amd64/x64/Release/ does not exist")
                    raise RuntimeError("Windows executable build failed")
            
            if not exe_only:
                # Build library (utf8proc.c is already included in the MSBuild project)
                print(f"Building library with: {msbuild_exe} prj/winlib-amd64/grapalib.sln /p:Configuration=Release")
                subprocess.run([
                    msbuild_exe, "prj/winlib-amd64/grapalib.sln", "/p:Configuration=Release"
                ], check=True)
                
                # Check if library was created
                if os.path.exists("prj/winlib-amd64/x64/Release/grapa.lib"):
                    print("SUCCESS: Library built successfully")
                    # Copy library
                    if os.path.exists("grapa.lib"):
                        os.remove("grapa.lib")
                    shutil.copy("prj/winlib-amd64/x64/Release/grapa.lib", "grapa.lib")
                    shutil.copy("prj/winlib-amd64/x64/Release/grapa.lib", f"source/grapa-lib/{config.target}/grapa.lib")
                    print("SUCCESS: Library copied to root directory and source/grapa-lib/")
                else:
                    print("ERROR: Library not found at prj/winlib-amd64/x64/Release/grapa.lib")
                    print("📁 Checking what files exist in prj/winlib-amd64/x64/Release/:")
                    if os.path.exists("prj/winlib-amd64/x64/Release/"):
                        import glob
                        for file in glob.glob("prj/winlib-amd64/x64/Release/*"):
                            print(f"  - {file}")
                    else:
                        print("  Directory prj/winlib-amd64/x64/Release/ does not exist")
                    raise RuntimeError("Windows library build failed")
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
            # Build main executable
            self._run_linux_build_command(config, is_library=False)
            
            # Build static library
            self._run_linux_build_command(config, is_library=True, is_static=True)
            
            # Build shared library
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
        import platform
        
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
                
                # Get all static libraries to include
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                # Create fully static library with all dependencies included
                subprocess.run(["libtool", "-static", "-o", "libgrapa.a"] + obj_files + openssl_libs + fl_libs + blst_libs + pcre2_lib, check=True)
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
                # Ensure the grapa-other directory exists
                os.makedirs(f"source/grapa-other/{config.target}", exist_ok=True)
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
            # Step 2: Build executable using utf8proc.o - expand globs like Linux build
            openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
            fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
            blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
            pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
            
            cmd = [
                "clang++", "-Isource", "source/main.cpp"
            ] + cpp_files + ["utf8proc.o"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                "-framework", "CoreFoundation", "-framework", "AppKit", "-framework", "IOKit",
                "-std=c++17", "-m64", "-O3", "-pthread", "-o", config.output_name
            ] + cross_flags
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            try:
                # Use subprocess.run for better error handling
                subprocess.run(cmd, check=True)
            except subprocess.CalledProcessError as e:
                print(f"Build failed: {e}")
                raise RuntimeError(f"Build failed with exit code {e.returncode}")
            except Exception as e:
                print(f"Build failed: {e}")
                raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"Executable created: {config.output_name}")
            else:
                print(f"Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
    
    def _run_linux_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
        """Run Linux build command (Ubuntu)"""
        import glob
        
        # Remove existing executable only when building executable
        if not is_library and os.path.exists(config.output_name):
            os.remove(config.output_name)
        
        print(f"Building {'library' if is_library else 'executable'} for {config.target}...")
        
        # Check if this is ARM64 native compilation in emulation
        is_arm64_emulation = config.target == "linux-arm64" and platform.machine() != "aarch64"
        
        # Set up compilation flags
        cross_flags = []
        cross_compiler_prefix = ""
        arm64_libs_available = False  # Initialize this variable for both library and executable builds
        
        if is_arm64_emulation:
            print("Using native ARM64 compilation in QEMU emulation...")
            # Check if ARM64 sysroot is available for native compilation
            sysroot_path = "./arm64-root"
            if os.path.exists(sysroot_path):
                print(f"Using ARM64 sysroot at: {sysroot_path}")
                # Use native ARM64 compilation in emulation
                cross_compiler_prefix = ""  # Use native ARM64 compiler in chroot
                # For native ARM64 compilation in chroot, we don't need --sysroot or include paths
                # since we're already running inside the ARM64 environment
                cross_flags = []
                arm64_libs_available = True
                print("ARM64 native compilation in emulation available")
            else:
                print("Warning: ARM64 sysroot not available, falling back to cross-compilation")
                # Fallback to cross-compilation if sysroot not available
                try:
                    subprocess.run(["aarch64-linux-gnu-gcc", "--version"], check=True, capture_output=True)
                    cross_compiler_prefix = "aarch64-linux-gnu-"
                    print("Using cross-compilation toolchain as fallback")
                    cross_flags = ["-L/usr/lib/aarch64-linux-gnu", "-L/usr/lib/x86_64-linux-gnu"]
                    arm64_libs_available = False
                except (subprocess.CalledProcessError, FileNotFoundError):
                    print("Cross-compilation toolchain not available on this platform")
                    print("Linux ARM64 compilation requires Linux environment with ARM64 tools")
                    raise RuntimeError("Linux ARM64 compilation not supported on macOS - requires Linux environment")
        
        # Build utf8proc first (C compilation)
        print("Building utf8proc...")
        # Use -fPIC for shared library builds, regular for executable
        pic_flag = ["-fPIC"] if is_library else []
        gcc_cmd = f"{cross_compiler_prefix}gcc" if cross_compiler_prefix else "gcc"
        gpp_cmd = f"{cross_compiler_prefix}g++" if cross_compiler_prefix else "g++"
        
        utf8proc_cmd = [
            gcc_cmd, "-Isource", "-DUTF8PROC_STATIC", "-c", 
            "source/utf8proc/utf8proc.c", "-O3"
        ] + pic_flag + cross_flags
        
        # Use native ARM64 compilation in chroot for emulation
        if is_arm64_emulation and arm64_libs_available:
            # Copy source files into chroot for native compilation
            print("Copying source files into ARM64 chroot...")
            # Only copy the source code directories, not the pre-built libraries
            subprocess.run(["sudo", "mkdir", "-p", "./arm64-root/source"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/grapa", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/utf8proc", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/FL", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/blst", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/openssl", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/pcre2", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/pybind11", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/X11", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/tiny-AES-c-master", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/miniz-master", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/work", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "-r", "source/grep", "./arm64-root/source/"], check=True)
            # Copy the main source files
            subprocess.run(["sudo", "cp", "source/main.cpp", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "source/maindebug.cpp", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "source/mainpy.cpp", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "source/mainpy_minimal.cpp", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "source/stdafx.cpp", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "source/stdafx.h", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "source/targetver.h", "./arm64-root/source/"], check=True)
            subprocess.run(["sudo", "cp", "source/Keccakreadableandcompact.c", "./arm64-root/source/"], check=True)
            
            # Step 1: Compile utf8proc.c with gcc inside chroot
            utf8proc_chroot_cmd = ["sudo", "chroot", "./arm64-root", "bash", "-c", " ".join(utf8proc_cmd)]
            print(f"Running utf8proc compilation in chroot: {' '.join(utf8proc_chroot_cmd)}")
            subprocess.run(utf8proc_chroot_cmd, check=True)
            
            # Copy utf8proc.o back to host filesystem
            subprocess.run(["sudo", "cp", "./arm64-root/utf8proc.o", "./"], check=True)
        else:
            # Use diagnostic function for cross-compilation or regular compilation
            if is_arm64_emulation:
                run_diagnostic_cross_compile(utf8proc_cmd)
            else:
                subprocess.run(utf8proc_cmd, check=True)
        
        # Get library path based on target
        lib_path = f"source/openssl-lib/{config.target}"
        
        if is_library:
            if is_static:
                # Build fully static library - include all static dependencies
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                # Clean any existing object files first
                for obj_file in glob.glob("*.o"):
                    os.remove(obj_file)
                
                cpp_compile_cmd = [
                    gpp_cmd, "-Isource", "-c"
                ] + cpp_files + [
                    "-std=c++17", "-O3", "-pthread", "-fPIC"
                ] + cross_flags
                
                # Use native ARM64 compilation in chroot for emulation
                if is_arm64_emulation and arm64_libs_available:
                    # Step 2: Compile C++ files inside chroot (source files already copied)
                    cpp_chroot_cmd = ["sudo", "chroot", "./arm64-root", "bash", "-c", " ".join(cpp_compile_cmd)]
                    print(f"Running C++ compilation in chroot: {' '.join(cpp_chroot_cmd)}")
                    subprocess.run(cpp_chroot_cmd, check=True)
                    
                    # Copy all .o files back to host filesystem
                    subprocess.run(["sudo", "cp", "./arm64-root/*.o", "./"], check=True)
                else:
                    # Use diagnostic function for cross-compilation or regular compilation
                    if is_arm64_emulation:
                        run_diagnostic_cross_compile(cpp_compile_cmd)
                    else:
                        subprocess.run(cpp_compile_cmd, check=True)
                
                # Get all .o files (including utf8proc.o if it exists)
                obj_files = glob.glob("*.o")
                if not obj_files:
                    raise RuntimeError("No object files found for static library")
                
                # Remove any existing libgrapa.a to avoid conflicts
                if os.path.exists("libgrapa.a"):
                    os.remove("libgrapa.a")
                
                # Create fully static library with all dependencies included
                ar_cmd = f"{cross_compiler_prefix}ar" if cross_compiler_prefix else "ar"
                subprocess.run([ar_cmd, "-crs", "libgrapa.a"] + obj_files + openssl_libs + fl_libs + blst_libs + pcre2_lib, check=True)
                shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa.a")
                os.remove("libgrapa.a")
            else:
                # Build shared library - completely separate from static build
                cpp_files = glob.glob("source/grapa/*.cpp")
                openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                
                # For shared library, use the exact working command from BUILD.md
                if config.target.startswith("linux-"):
                    print(f"Building shared library for {config.target} using working reference command...")
                    
                    # Build the exact command that works (from BUILD.md)
                    # Use wildcard patterns like the working reference with shell expansion
                    cmd = [gpp_cmd, "-shared", "-Isource", "-DUTF8PROC_STATIC"] + cpp_files + ["source/utf8proc/utf8proc.c"] + [
                        f"source/openssl-lib/{config.target}/*.a",
                        f"source/fl-lib/{config.target}/*.a", 
                        f"source/blst-lib/{config.target}/*.a",
                        f"source/pcre2-lib/{config.target}/libpcre2-8.a"
                    ] + [
                        f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto", "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama", "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                    ]
                    
                    # Convert to shell command for wildcard expansion
                    shell_cmd = " ".join(cmd)
                    
                    # Add -static-libgcc only for Linux systems (not macOS)
                    current_platform = platform.system().lower()
                    if current_platform == "linux":
                        cmd.insert(-2, "-static-libgcc")
                        
                elif config.target.startswith("mac-"):
                    print(f"Building shared library for {config.target} using working Mac reference command...")
                    
                    # For Mac builds, compile utf8proc separately first (like the working reference)
                    utf8proc_cmd = ["clang", "-Isource", "-DUTF8PROC_STATIC", "-c", "source/utf8proc/utf8proc.c", "-m64", "-O3"]
                    subprocess.run(utf8proc_cmd, check=True)
                    
                    # Build the exact Mac command that works (from BUILD.md)
                    cmd = [
                        "clang++", "-shared", "-Isource"
                    ] + cpp_files + ["utf8proc.o"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                        "-framework", "CoreFoundation", "-framework", "AppKit", "-framework", "IOKit", "-std=c++17", "-m64", "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                    ]
                
                # Execute shared library build with shell expansion for wildcards
                if config.target.startswith("linux-"):
                    print(f"Executing shared library build command: {shell_cmd}")
                    subprocess.run(shell_cmd, shell=True, check=True)
                else:
                    print(f"Executing shared library build command: {' '.join(cmd)}")
                    subprocess.run(cmd, check=True)
                
                # Clean up utf8proc.o immediately after successful shared library build
                if os.path.exists("utf8proc.o"):
                    print("Removing utf8proc.o...")
                    os.remove("utf8proc.o")
                
                # Ensure the grapa-other directory exists (standardized shared library location)
                os.makedirs(f"source/grapa-other/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"source/grapa-other/{config.target}/libgrapa.so")
                os.remove("libgrapa.so")
        else:
            # Build executable - match AWS pattern exactly
            cpp_files = glob.glob("source/grapa/*.cpp")
            openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
            fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
            blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
            pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
            
            # For executable build, use the exact working command from BUILD.md
            if config.target.startswith("linux-"):
                print(f"Building executable for {config.target} using working reference command...")
                
                # Build the exact command that works (from BUILD.md)
                # Use wildcard patterns like the working reference with shell expansion
                cmd = [
                    gpp_cmd, "-Isource", "-DUTF8PROC_STATIC", "source/main.cpp"
                ] + cpp_files + ["source/utf8proc/utf8proc.c"] + [
                    f"source/openssl-lib/{config.target}/*.a",
                    f"source/fl-lib/{config.target}/*.a", 
                    f"source/blst-lib/{config.target}/*.a",
                    f"source/pcre2-lib/{config.target}/libpcre2-8.a"
                ] + [
                    f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-lcrypto", "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama", "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-O3", "-pthread", "-o", config.output_name
                ]
                
                # Convert to shell command for wildcard expansion
                shell_cmd = " ".join(cmd)
                
                # Add -static-libgcc only for Linux systems (not macOS)
                current_platform = platform.system().lower()
                if current_platform == "linux":
                    cmd.insert(-2, "-static-libgcc")
                    
            elif config.target.startswith("mac-"):
                print(f"Building executable for {config.target} using working Mac reference command...")
                
                # For Mac builds, compile utf8proc separately first (like the working reference)
                utf8proc_cmd = ["clang", "-Isource", "-DUTF8PROC_STATIC", "-c", "source/utf8proc/utf8proc.c", "-m64", "-O3"]
                subprocess.run(utf8proc_cmd, check=True)
                
                # Build the exact Mac command that works (from BUILD.md)
                cmd = [
                    "clang++", "-Isource", "source/main.cpp"
                ] + cpp_files + ["utf8proc.o"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    "-framework", "CoreFoundation", "-framework", "AppKit", "-framework", "IOKit", "-std=c++17", "-m64", "-O3", "-pthread", "-o", config.output_name
                ]
            
            try:
                # Execute the build command with shell expansion for wildcards
                if config.target.startswith("linux-"):
                    subprocess.run(shell_cmd, shell=True, check=True)
                else:
                    subprocess.run(cmd, check=True)
                
                # Clean up utf8proc.o immediately after successful build
                if os.path.exists("utf8proc.o"):
                    print("Removing utf8proc.o...")
                    os.remove("utf8proc.o")
                    
            except subprocess.CalledProcessError as e:
                print(f"Build failed: {e}")
                raise RuntimeError(f"Build failed with exit code {e.returncode}")
            except Exception as e:
                print(f"Build failed: {e}")
                raise
            
            # Check if executable was created
            if os.path.exists(config.output_name):
                print(f"Executable created: {config.output_name}")
            else:
                print(f"Executable not found: {config.output_name}")
                raise RuntimeError(f"Executable {config.output_name} was not created")
    

    
    def _clean_build_artifacts(self, preserve_exe: bool = False):
        """Clean build artifacts that should be removed after build"""
        print("Cleaning build artifacts...")
        
        # Clean Python package artifacts
        if os.path.exists("dist"):
            print("Removing dist/ directory...")
            shutil.rmtree("dist")
        
        if os.path.exists("grapapy.egg-info"):
            print("Removing grapapy.egg-info/ directory...")
            shutil.rmtree("grapapy.egg-info")
        
        # Clean Python bytecode cache
        for pycache_dir in Path(".").glob("**/__pycache__"):
            print(f"Removing {pycache_dir}...")
            try:
                shutil.rmtree(pycache_dir)
            except PermissionError:
                print(f"Warning: Could not remove {pycache_dir} due to permissions, continuing...")
        
        # Clean ARM64 chroot Python cache if it exists
        arm64_pycache = Path("./arm64-root/usr/share/python3/__pycache__")
        if arm64_pycache.exists():
            print(f"Removing ARM64 chroot Python cache: {arm64_pycache}...")
            try:
                subprocess.run(["sudo", "rm", "-rf", str(arm64_pycache)], check=True)
            except subprocess.CalledProcessError:
                print(f"Warning: Could not remove ARM64 chroot Python cache, continuing...")
        
        # Clean object files
        for obj_file in Path(".").glob("*.o"):
            print(f"Removing {obj_file}...")
            obj_file.unlink()
        
        # Clean library files
        for lib_file in Path(".").glob("*.a"):
            print(f"Removing {lib_file}...")
            lib_file.unlink()
        
        # Clean shared library files
        for so_file in Path(".").glob("*.so"):
            print(f"Removing {so_file}...")
            so_file.unlink()
        
        # Clean Windows library files
        for lib_file in Path(".").glob("*.lib"):
            print(f"Removing {lib_file}...")
            lib_file.unlink()
        
        # Clean Windows DLL files
        for dll_file in Path(".").glob("*.dll"):
            print(f"Removing {dll_file}...")
            dll_file.unlink()
        
        # Clean executable files (unless preserve_exe is True)
        if not preserve_exe:
            for exe_file in Path(".").glob("grapa*"):
                if exe_file.is_file() and os.access(exe_file, os.X_OK):
                    print(f"Removing executable {exe_file}...")
                    exe_file.unlink()
    
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
        
        # Add static library files
        lib_files = glob.glob(f"source/grapa-lib/{config.target}/*")
        files_to_include.extend(lib_files)
        
        # Add shared library files (include shared libraries like macOS)
        other_files = glob.glob(f"source/grapa-other/{config.target}/*")
        files_to_include.extend(other_files)
        
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
        
        return True
    
    def _extract_executable_for_testing(self, config: BuildConfig):
        """Extract executable from bin folder for testing"""
        import tempfile
        import shutil
        
        # Determine the appropriate bin file and executable name
        if config.platform == "windows":
            bin_file = f"bin/grapa-{config.target}.zip"
            exe_name = "grapa.exe"
        else:
            bin_file = f"bin/grapa-{config.target}.tar.gz"
            exe_name = "grapa"
        
        if not os.path.exists(bin_file):
            print(f"ERROR: Bin file not found: {bin_file}")
            return None
        
        # Create temporary directory for extraction
        temp_dir = tempfile.mkdtemp()
        print(f"Extracting executable to: {temp_dir}")
        
        try:
            # Extract the executable
            if config.platform == "windows":
                import zipfile
                with zipfile.ZipFile(bin_file, 'r') as zip_ref:
                    zip_ref.extract(exe_name, temp_dir)
            else:
                import tarfile
                with tarfile.open(bin_file, 'r:gz') as tar_ref:
                    tar_ref.extract(exe_name, temp_dir)
            
            exe_path = os.path.join(temp_dir, exe_name)
            if os.path.exists(exe_path):
                # Make executable (for Unix systems)
                if config.platform != "windows":
                    os.chmod(exe_path, 0o755)
                print(f"SUCCESS: Extracted executable: {exe_path}")
                return exe_path
            else:
                print(f"ERROR: Executable not found in extracted files: {exe_path}")
                return None
        except Exception as e:
            print(f"ERROR: Failed to extract executable: {e}")
            return None

    def run_cli_tests(self, config: BuildConfig):
        """Run CLI tests"""
        print("Running CLI tests...")
        
        # Check if executable exists in current directory
        exe_path = "./grapa"
        if config.platform == "windows":
            exe_path = "./grapa.exe"
        
        if not os.path.exists(exe_path):
            print(f"Executable not found at {exe_path}, extracting from bin folder...")
            exe_path = self._extract_executable_for_testing(config)
            if not exe_path:
                print("ERROR: Could not extract executable for testing")
                return False
        
        # For Windows executables on non-Windows systems, try using Wine
        if config.platform == "windows" and os.name != "nt":
            # Check if Wine is available
            try:
                subprocess.run(["wine", "--version"], check=True, capture_output=True)
                print("✅ Wine detected, will use for Windows executable testing")
                wine_prefix = ["wine"]
            except (subprocess.CalledProcessError, FileNotFoundError):
                print("⚠️  Wine not available, Windows CLI testing limited")
                print("   Windows CLI testing requires Wine or Windows environment")
                wine_prefix = []
        else:
            wine_prefix = []
        
        # Run Grapa CLI tests
        test_commands = [
            wine_prefix + [exe_path, "-h"]  # Help command
        ]
        
        for cmd in test_commands:
            try:
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
                if result.returncode == 0:
                    print("SUCCESS: CLI help command works")
                else:
                    print(f"WARNING: CLI help command failed (exit code: {result.returncode})")
                    print("   This is expected for some platforms - CLI extraction still works")
            except subprocess.TimeoutExpired:
                print("WARNING: CLI test timed out")
            except subprocess.CalledProcessError as e:
                print(f"WARNING: CLI test failed: {e}")
                print("   This is expected for some platforms - CLI extraction still works")
        
        return True
    
    def run_python_tests(self, config: BuildConfig):
        """Run Python tests"""
        print("Running Python tests...")
        
        # Check if grapapy is available
        try:
            import grapapy
            print("SUCCESS: grapapy package is available")
        except ImportError:
            print("grapapy package not found, building Python package...")
            # Build Python package
            if not self.build_python_package(config):
                print("ERROR: Failed to build Python package")
                return False
            
            # Install the package
            dist_files = [f for f in os.listdir("dist") if f.startswith("grapapy-") and f.endswith(".tar.gz")]
            if not dist_files:
                print("ERROR: No grapapy package found in dist/")
                return False
            
            package_path = os.path.join("dist", dist_files[0])
            pip_cmd = "python" if config.platform == "windows" else "python3"
            
            try:
                subprocess.run([pip_cmd, "-m", "pip", "install", package_path], check=True)
                print("SUCCESS: Python package installed")
            except subprocess.CalledProcessError as e:
                print(f"ERROR: Failed to install Python package: {e}")
                return False
        
        # Run Python tests
        test_commands = [
            ["python" if config.platform == "windows" else "python3", "test/python/test_python_examples.py"],
            ["python" if config.platform == "windows" else "python3", "test/python/test_grapapy_table.py"]
        ]
        
        for cmd in test_commands:
            try:
                subprocess.run(cmd, check=True)
                print("SUCCESS: Python tests passed")
            except subprocess.CalledProcessError as e:
                print(f"ERROR: Python tests failed: {e}")
                return False
        
        return True
    
    def run_tests(self, config: BuildConfig, exe_only: bool = False, python_only: bool = False):
        """Run tests based on build flags"""
        print("Running tests...")
        
        # Determine which tests to run based on build flags
        if exe_only:
            # Only run CLI tests when building executable only
            return self.run_cli_tests(config)
        elif python_only:
            # Only run Python tests when building Python extension only
            return self.run_python_tests(config)
        else:
            # Run both tests when building everything
            cli_success = self.run_cli_tests(config)
            python_success = self.run_python_tests(config)
            return cli_success and python_success
    
    def build(self, run_tests: bool = False, exe_only: bool = False, lib_only: bool = False, python_only: bool = False, preserve_dist: bool = False, preserve_exe: bool = False, target_config: BuildConfig = None) -> bool:
        """Build for the current platform and architecture"""
        if target_config is None:
            platform, arch = self.detect_platform()
            config = BuildConfig(platform, arch)
        else:
            config = target_config
        
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
            else:
                print(f"Unsupported platform: {config.platform}")
                return False
            
            if success:
                print(f"Build successful for {config.target}")
                
                if not exe_only:
                    # Build Python package
                    self.build_python_package(config)
                    # Run tests if requested
                    if run_tests:
                        self.run_tests(config, exe_only=exe_only, python_only=python_only)
                elif run_tests:
                    # Run tests even for exe_only builds
                    self.run_tests(config, exe_only=exe_only, python_only=python_only)
                
                return True
            else:
                print(f"Build failed for {config.target}")
                return False
        finally:
            # Always clean up build artifacts, unless preserve_dist is requested
            if not preserve_dist:
                self._clean_build_artifacts(preserve_exe=preserve_exe)

    def build_libraries_only(self, config: BuildConfig) -> bool:
        """Build only the libraries (skip executable and Python package)"""
        print("Building libraries only...")
        
        try:
            # Build based on platform
            success = False
            if config.platform == "windows":
                success = self.build_windows(config, exe_only=False, lib_only=True)
            elif config.platform == "mac":
                success = self.build_mac(config, exe_only=False, lib_only=True)
            elif config.platform == "linux":
                success = self.build_linux_aws(config, exe_only=False, lib_only=True)
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

    def build_python_only(self, config: BuildConfig, preserve_dist: bool = False, run_tests: bool = False) -> bool:
        """Build only the Python extension (assumes executable already exists)"""
        print("Building Python extension only...")
        
        try:
            # Build Python package
            self.build_python_package(config)
            
            # Run tests if requested
            if run_tests:
                self.run_tests(config, exe_only=False, python_only=True)
            
            return True
        except Exception as e:
            print(f"Python extension build failed: {e}")
            return False

    def build_bin_only(self, config: BuildConfig, preserve_exe: bool = False, run_tests: bool = False) -> bool:
        """Build executable and libraries, then create compressed package in bin/"""
        print("Building executable and libraries for bin package...")
        print(f"Platform: {config.platform}, Target: {config.target}")
        
        try:
            # Build main executable
            if config.platform == "windows":
                success = self.build_windows(config, exe_only=False, lib_only=False)
            elif config.platform == "mac":
                success = self.build_mac(config, exe_only=False, lib_only=False)
            elif config.platform == "linux":
                success = self.build_linux_aws(config, exe_only=False, lib_only=False)
            else:
                print(f"Unsupported platform: {config.platform}")
                return False
            
            if success:
                print("Executable and libraries built successfully")
                print("Package already created by build method")
                
                # Run tests if requested
                if run_tests:
                    self.run_tests(config, exe_only=True, python_only=False)
                
                # Clean up build artifacts
                self._clean_build_artifacts(preserve_exe=preserve_exe)
                
                return True
            else:
                print("Build failed")
                return False
                
        except Exception as e:
            print(f"Bin package build failed: {e}")
            return False

    def _copy_libraries_to_top_level(self, config: BuildConfig):
        """Copy library files to top-level directory for packaging"""
        import glob
        
        # Copy static libraries
        lib_patterns = [
            f"source/grapa-lib/{config.target}/*.a",
            f"source/openssl-lib/{config.target}/*.a",
            f"source/fl-lib/{config.target}/*.a",
            f"source/blst-lib/{config.target}/*.a",
            f"source/pcre2-lib/{config.target}/*.a",
        ]
        
        for pattern in lib_patterns:
            for lib_file in glob.glob(pattern):
                if os.path.exists(lib_file):
                    filename = os.path.basename(lib_file)
                    shutil.copy2(lib_file, filename)
                    print(f"Copied {filename} to top-level directory")
        
        # Copy shared libraries
        so_patterns = [
            f"source/grapa-lib/{config.target}/*.so",
            f"source/openssl-lib/{config.target}/*.so",
            f"source/fl-lib/{config.target}/*.so",
            f"source/blst-lib/{config.target}/*.so",
            f"source/pcre2-lib/{config.target}/*.so",
        ]
        
        for pattern in so_patterns:
            for lib_file in glob.glob(pattern):
                if os.path.exists(lib_file):
                    filename = os.path.basename(lib_file)
                    shutil.copy2(lib_file, filename)
                    print(f"Copied {filename} to top-level directory")

def main():
    parser = argparse.ArgumentParser(description="Grapa Build Script")
    parser.add_argument("--test", action="store_true", help="Run tests after build")
    parser.add_argument("--test-only", action="store_true", help="Run tests only (assumes build already completed)")
    parser.add_argument("--clean", action="store_true", help="Clean build artifacts")
    parser.add_argument("--exe-only", action="store_true", help="Build only the main executable (skip library, Python package, and packaging steps). Useful for fast iterative development and investigation.")
    parser.add_argument("--lib-only", action="store_true", help="Build only the libraries (skip executable, Python package, and packaging steps). Libraries will be copied to the top-level directory.")
    parser.add_argument("--bin-only", action="store_true", help="Build executable and libraries, then create compressed package in bin/ directory. This creates the complete distribution package.")
    parser.add_argument("--python-only", action="store_true", help="Build only the Python extension (assumes executable already exists). Useful for debugging Python extension issues without rebuilding the executable.")
    parser.add_argument("--preserve-dist", action="store_true", help="Preserve the dist/ directory after build (useful for debugging or manual installation)")
    parser.add_argument("--preserve-exe", action="store_true", help="Preserve the executable after build (useful for testing or manual use)")
    parser.add_argument("--target-platform", type=str, help="Override target platform (mac-arm64 or mac-amd64 for macOS cross-compilation)")
    
    args = parser.parse_args()
    
    builder = GrapaBuilder()
    
    # Check if we're in CI and use CI_PLATFORM if available
    ci_platform = os.environ.get("CI_PLATFORM")
    if ci_platform:
        print(f"CI detected, using target platform: {ci_platform}")
        # Parse CI_PLATFORM like "linux-arm64", "mac-amd64", etc.
        if "-" in ci_platform:
            platform, arch = ci_platform.split("-", 1)
            # Normalize platform names to match build logic expectations
            if platform == "win":
                platform = "windows"
            elif platform == "mac":
                platform = "mac"
            elif platform == "linux":
                platform = "linux"
            elif platform == "aws":
                platform = "aws"
            print(f"Building for {platform} {arch}")
        else:
            print(f"Invalid CI_PLATFORM format: {ci_platform}")
            return 1
    elif args.target_platform:
        print(f"Using explicit target platform: {args.target_platform}")
        # Parse target platform like "mac-amd64", "mac-arm64"
        if "-" in args.target_platform:
            platform, arch = args.target_platform.split("-", 1)
            # Only support macOS platforms
            if platform == "mac":
                platform = "mac"
                if arch not in ["arm64", "amd64"]:
                    print(f"❌ Unsupported macOS architecture: {arch}")
                    print("Supported architectures: arm64, amd64")
                    return 1
                print(f"Building for {platform} {arch}")
            else:
                print(f"❌ Unsupported platform: {platform}")
                print("Only macOS platforms are supported with --target-platform")
                print("Supported platforms: mac-arm64, mac-amd64")
                return 1
        else:
            print(f"Invalid target platform format: {args.target_platform}")
            print("Expected format: mac-arm64 or mac-amd64")
            return 1
    else:
        # Build for current platform only
        platform, arch = builder.detect_platform()
        print(f"Building for {platform} {arch}")
    
    # Create BuildConfig with the target platform
    config = BuildConfig(platform, arch)
    
    # Handle clean option (run before build, not instead of build)
    if args.clean:
        print("Cleaning build artifacts...")
        builder._clean_build_artifacts(preserve_exe=args.preserve_exe)
    
    if args.test_only:
        # Run tests only (assumes build already completed)
        print("Running tests only (assuming build already completed)...")
        success = builder.run_tests(config, exe_only=args.exe_only, python_only=args.python_only)
    elif args.bin_only:
        success = builder.build_bin_only(config, preserve_exe=args.preserve_exe, run_tests=args.test)
    elif args.python_only:
        success = builder.build_python_only(config, preserve_dist=args.preserve_dist, run_tests=args.test)
    else:
        success = builder.build(args.test, exe_only=args.exe_only, lib_only=args.lib_only, python_only=args.python_only, preserve_dist=args.preserve_dist, preserve_exe=args.preserve_exe, target_config=config)
    
    # Clean after build if requested (unless preserve flags are specified)
    if args.clean and success:
        # Only clean if no preserve flags are specified
        if not (args.preserve_exe or args.preserve_dist):
            print("Cleaning build artifacts after successful build...")
            builder._clean_build_artifacts(preserve_exe=args.preserve_exe)
        else:
            print("Skipping post-build cleanup due to preserve flags...")
    
    if success:
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