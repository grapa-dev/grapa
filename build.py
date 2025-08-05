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
        
        # Check if this is ARM64 native compilation in emulation
        is_arm64_emulation = config.target == "linux-arm64" and platform.machine() != "aarch64"
        
        # Set up compilation flags
        cross_flags = []
        cross_compiler_prefix = ""
        if is_arm64_emulation:
            print("Using native ARM64 compilation in QEMU emulation...")
            # Check if ARM64 sysroot is available for native compilation
            sysroot_path = "./arm64-root"
            if os.path.exists(sysroot_path):
                print(f"Using ARM64 sysroot at: {sysroot_path}")
                # Use native ARM64 compilation in emulation
                cross_compiler_prefix = ""  # Use native ARM64 compiler in chroot
                cross_flags = [
                    f"--sysroot={sysroot_path}",
                    "-I" + os.path.join(sysroot_path, "usr/include"),
                    "-L" + os.path.join(sysroot_path, "usr/lib/aarch64-linux-gnu"),
                    "-L" + os.path.join(sysroot_path, "lib/aarch64-linux-gnu")
                ]
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
            subprocess.run(["sudo", "cp", "-r", "source", "./arm64-root/"], check=True)
            
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
                
                # For shared library, use dynamic linking (no -static-libgcc)
                if is_arm64_emulation:
                    # For native ARM64 compilation in emulation, use X11 libraries from sysroot with libbsd
                    system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                                  "-lfontconfig", "-lXcursor", "-lbsd", "-ldl", "-lm"]
                else:
                    # For native builds, use dynamic linking without libbsd (not needed for AMD64)
                    system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                                  "-lfontconfig", "-lXcursor", "-ldl", "-lm"]
                
                cmd = [gpp_cmd, "-shared", "-Isource", "-DUTF8PROC_STATIC"] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                    f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                ] + cross_flags + [
                    "-lcrypto"
                ] + system_libs
                
                # Add -static-libgcc for Linux builds (matching working command)
                if config.platform == "linux":
                    cmd.extend(["-static-libgcc"])
                
                # Add -ljpeg for shared library builds
                cmd.insert(-2, "-ljpeg")
                
                # Execute shared library build
                print(f"Executing shared library build command: {' '.join(cmd)}")
                if is_arm64_emulation and arm64_libs_available:
                    # Use native ARM64 compilation in chroot for emulation
                    chroot_cmd = ["sudo", "chroot", "./arm64-root", "bash", "-c", " ".join(cmd)]
                    print(f"Running native ARM64 compilation: {' '.join(chroot_cmd)}")
                    subprocess.run(chroot_cmd, check=True)
                else:
                    # Use diagnostic function for cross-compilation or regular compilation
                    if is_arm64_emulation:
                        run_diagnostic_cross_compile(cmd)
                    else:
                        subprocess.run(cmd, check=True)
                
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
            
            # For executable build, use static linking for cross-compilation, dynamic for native
            if is_arm64_emulation:
                # For native ARM64 compilation in emulation, use dynamic linking with libbsd
                system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                              "-lfontconfig", "-lXcursor", "-lbsd", "-ldl", "-lm"]
            else:
                # For native builds, use dynamic linking without libbsd (not needed for AMD64)
                system_libs = ["-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                              "-lfontconfig", "-lXcursor", "-ldl", "-lm"]
            
            cmd = [
                gpp_cmd, "-Isource", "-DUTF8PROC_STATIC", "source/main.cpp"
            ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + fl_libs + blst_libs + pcre2_lib + [
                f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-O3", "-pthread", "-o", config.output_name
            ] + cross_flags + [
                "-lcrypto"
            ] + system_libs
            
            # Add -static-libgcc for Linux builds (matching working command)
            if config.platform == "linux":
                cmd.extend(["-static-libgcc"])
            
            # Add -ljpeg for executable builds
            cmd.insert(-2, "-ljpeg")
            
            print(f"Current working directory: {os.getcwd()}")
            print(f"Executing executable build command: {' '.join(cmd)}")
            try:
                # Use native ARM64 compilation in chroot for emulation
                if is_arm64_emulation and arm64_libs_available:
                    # Copy library files into chroot environment
                    print("Copying library files into ARM64 chroot...")
                    subprocess.run(["sudo", "cp", "-r", "source/openssl-lib", "./arm64-root/source/"], check=True)
                    subprocess.run(["sudo", "cp", "-r", "source/fl-lib", "./arm64-root/source/"], check=True)
                    subprocess.run(["sudo", "cp", "-r", "source/blst-lib", "./arm64-root/source/"], check=True)
                    subprocess.run(["sudo", "cp", "-r", "source/pcre2-lib", "./arm64-root/source/"], check=True)
                    
                    # Update library paths to use chroot-relative paths
                    openssl_libs = glob.glob(f"source/openssl-lib/{config.target}/*.a")
                    fl_libs = glob.glob(f"source/fl-lib/{config.target}/*.a")
                    blst_libs = glob.glob(f"source/blst-lib/{config.target}/*.a")
                    pcre2_lib = glob.glob(f"source/pcre2-lib/{config.target}/libpcre2-8.a")
                    
                    # Convert to chroot-relative paths
                    chroot_openssl_libs = [lib.replace("source/", "/source/") for lib in openssl_libs]
                    chroot_fl_libs = [lib.replace("source/", "/source/") for lib in fl_libs]
                    chroot_blst_libs = [lib.replace("source/", "/source/") for lib in blst_libs]
                    chroot_pcre2_lib = [lib.replace("source/", "/source/") for lib in pcre2_lib]
                    
                    # Build command with chroot-relative paths
                    chroot_cmd = [
                        gpp_cmd, "-Isource", "-DUTF8PROC_STATIC", "source/main.cpp"
                    ] + cpp_files + ["source/utf8proc/utf8proc.c"] + chroot_openssl_libs + chroot_fl_libs + chroot_blst_libs + chroot_pcre2_lib + [
                        f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-O3", "-pthread", "-o", config.output_name
                    ] + cross_flags + [
                        "-lcrypto"
                    ] + system_libs
                    
                    # Add -static-libgcc for Linux builds (matching working command)
                    if config.platform == "linux":
                        chroot_cmd.extend(["-static-libgcc"])
                    
                    # Add -ljpeg for executable builds
                    chroot_cmd.insert(-2, "-ljpeg")
                    
                    # Run native ARM64 compilation in chroot
                    chroot_bash_cmd = ["sudo", "chroot", "./arm64-root", "bash", "-c", " ".join(chroot_cmd)]
                    print(f"Running native ARM64 compilation: {' '.join(chroot_bash_cmd)}")
                    subprocess.run(chroot_bash_cmd, check=True)
                else:
                    # Use diagnostic function for cross-compilation or regular compilation
                    if is_arm64_emulation:
                        run_diagnostic_cross_compile(cmd)
                    else:
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
                    f"-Lsource/openssl-lib/{config.target}", "-std=c++17", "-O3", "-pthread", "-fPIC", "-o", "libgrapa.so"
                ] + [
                    "-lcrypto", "-lX11", "-lXfixes", "-lXft", "-lXext", "-lXrender", "-lXinerama",
                    "-lfontconfig", "-lXcursor", "-ldl", "-lm", "-static-libgcc"
                ]
                
                print(f"Executing shared library build command: {' '.join(cmd)}")
                subprocess.run(cmd, check=True)
                # Ensure the grapa-other directory exists
                os.makedirs(f"source/grapa-other/{config.target}", exist_ok=True)
                shutil.copy("libgrapa.so", f"source/grapa-other/{config.target}/libgrapa.so")
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
    
    def _clean_build_artifacts(self):
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
    
    def build(self, run_tests: bool = False, exe_only: bool = False, lib_only: bool = False, python_only: bool = False, preserve_dist: bool = False, target_config: BuildConfig = None) -> bool:
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
            elif config.platform == "aws":
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
                        self.run_tests(config)
                
                return True
            else:
                print(f"Build failed for {config.target}")
                return False
        finally:
            # Always clean up build artifacts, unless preserve_dist is requested
            if not preserve_dist:
                self._clean_build_artifacts()

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
            elif config.platform == "aws":
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

    def build_python_only(self, config: BuildConfig, preserve_dist: bool = False) -> bool:
        """Build only the Python extension (assumes executable already exists)"""
        print("Building Python extension only...")
        
        try:
            # Build Python package
            self.build_python_package(config)
            return True
        except Exception as e:
            print(f"Python extension build failed: {e}")
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
    parser.add_argument("--clean", action="store_true", help="Clean build artifacts")
    parser.add_argument("--exe-only", action="store_true", help="Build only the main executable (skip library, Python package, and packaging steps). Useful for fast iterative development and investigation.")
    parser.add_argument("--lib-only", action="store_true", help="Build only the libraries (skip executable, Python package, and packaging steps). Libraries will be copied to the top-level directory.")
    parser.add_argument("--python-only", action="store_true", help="Build only the Python extension (assumes executable already exists). Useful for debugging Python extension issues without rebuilding the executable.")
    parser.add_argument("--preserve-dist", action="store_true", help="Preserve the dist/ directory after build (useful for debugging or manual installation)")
    parser.add_argument("--target-platform", type=str, help="Override target platform (e.g., 'mac-amd64' for cross-compilation from ARM64)")
    
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
        # Parse target platform like "mac-amd64", "linux-arm64", etc.
        if "-" in args.target_platform:
            platform, arch = args.target_platform.split("-", 1)
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
            print(f"Invalid target platform format: {args.target_platform}")
            return 1
    else:
        # Build for current platform only
        platform, arch = builder.detect_platform()
        print(f"Building for {platform} {arch}")
    
    # Create BuildConfig with the target platform
    config = BuildConfig(platform, arch)
    
    if builder.build(args.test, exe_only=args.exe_only, lib_only=args.lib_only, python_only=args.python_only, preserve_dist=args.preserve_dist, target_config=config):
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