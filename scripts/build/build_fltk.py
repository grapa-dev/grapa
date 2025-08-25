#!/usr/bin/env python3
"""
FLTK 1.4.4 Static Library Build Script

This script builds FLTK 1.4.4 static libraries for all supported platforms.
It automatically detects the current platform and builds the appropriate libraries.

Usage:
    python3 scripts/build/build_fltk.py                    # Build for current platform
    python3 scripts/build/build_fltk.py --help             # Show help
    python3 scripts/build/build_fltk.py --clean            # Clean build artifacts
    python3 scripts/build/build_fltk.py --target mac-arm64 # Build for specific target

Supported Platforms:
    - mac-arm64 (Apple Silicon only)
    - linux-arm64, linux-amd64  
    - win-arm64, win-amd64
    - aws-arm64, aws-amd64
"""

import os
import sys
import platform
import subprocess
import argparse
import shutil
from pathlib import Path
from typing import Dict, List, Tuple, Optional

class FLTKBuilder:
    """FLTK 1.4.4 static library builder"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent.parent
        self.fltk_source = self.project_root / "dep" / "fltk-1.4.4"
        self.fl_lib_dir = self.project_root / "source" / "fl-lib"
        self.fl_headers_dir = self.project_root / "source" / "FL"
        

        
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
    
    def get_target_dir(self, platform: str, arch: str) -> Path:
        """Get the target directory for the platform/architecture"""
        # Map platform names to directory names (matching build.py)
        if platform == "windows":
            target_platform = "win"
        else:
            target_platform = platform
        return self.fl_lib_dir / f"{target_platform}-{arch}"
    
    def clean_build(self, target: str = None):
        """Clean FLTK build artifacts for specific target or all targets"""
        print("🧹 Cleaning FLTK build artifacts...")
        
        # Clean FLTK source build
        if self.fltk_source.exists():
            try:
                if platform.system().lower() == "windows":
                    # On Windows, clean CMake build directory if it exists
                    build_dir = self.fltk_source / "build"
                    if build_dir.exists():
                        shutil.rmtree(build_dir)
                        print("✅ FLTK build directory cleaned")
                else:
                    # On Unix-like systems, use make clean
                    subprocess.run(["make", "clean"], cwd=self.fltk_source, check=True)
                    print("✅ FLTK source cleaned")
            except subprocess.CalledProcessError:
                print("⚠️  FLTK clean failed (may not be built yet)")
        
        # Clean target directories
        if target:
            # Clean only the specific target
            platform_name, arch = target.split("-")
            target_dir = self.get_target_dir(platform_name, arch)
            if target_dir.exists():
                # Handle both .a (Unix) and .lib (Windows) files
                for lib_file in target_dir.glob("*.a"):
                    lib_file.unlink()
                    print(f"🗑️  Removed {lib_file}")
                for lib_file in target_dir.glob("*.lib"):
                    lib_file.unlink()
                    print(f"🗑️  Removed {lib_file}")
                print(f"✅ Cleaned libraries for {target}")
            else:
                print(f"⚠️  Target directory {target_dir} does not exist")
        else:
            # Clean all target directories (original behavior)
            for target_dir in self.fl_lib_dir.glob("*"):
                if target_dir.is_dir():
                    for lib_file in target_dir.glob("*.a"):
                        lib_file.unlink()
                        print(f"🗑️  Removed {lib_file}")
                    for lib_file in target_dir.glob("*.lib"):
                        lib_file.unlink()
                        print(f"🗑️  Removed {lib_file}")
            print("✅ All build artifacts cleaned")
    
    def build_mac(self, target: str):
        """Build FLTK for macOS"""
        print(f"🍎 Building FLTK 1.4.4 for {target}...")
        
        # Configure FLTK 1.4.4
        configure_cmd = [
            "./configure",
            "--enable-static",
            "--disable-shared", 
            "--enable-localjpeg",
            "--enable-localpng",
            "--enable-localzlib",
            f"--prefix=/tmp/fltk-1.4.4-{target}"
        ]
        
        # Always include ScreenCaptureKit for mac-arm64 (requires macOS 15.0+)
        configure_cmd.append("LDFLAGS=-framework ScreenCaptureKit")
        print("✅ Including ScreenCaptureKit framework for mac-arm64")
        
        print(f"🔧 Configuring: {' '.join(configure_cmd)}")
        subprocess.run(configure_cmd, cwd=self.fltk_source, check=True)
        
        # Build FLTK
        print("🔨 Building FLTK libraries...")
        subprocess.run(["make", "-j", str(os.cpu_count())], cwd=self.fltk_source, check=True)
        
        # Copy libraries
        target_dir = self.get_target_dir("mac", target.split("-")[1])
        target_dir.mkdir(parents=True, exist_ok=True)
        
        for lib_file in (self.fltk_source / "lib").glob("*.a"):
            shutil.copy2(lib_file, target_dir)
            print(f"📦 Copied {lib_file.name} to {target_dir}")
        
        # Copy headers
        if self.fl_headers_dir.exists():
            shutil.rmtree(self.fl_headers_dir)
        shutil.copytree(self.fltk_source / "FL", self.fl_headers_dir)
        print(f"📋 Copied FLTK headers to {self.fl_headers_dir}")
        
        print(f"✅ FLTK build complete for {target}")
    
    def build_linux(self, target: str):
        """Build FLTK for Linux"""
        print(f"🐧 Building FLTK 1.4.4 for {target}...")
        
        # Run autogen if available (matching the working 1.3 build process)
        if shutil.which("autoconf"):
            print("🔧 Running autogen...")
            env = os.environ.copy()
            env['NOCONFIGURE'] = '1'
            try:
                subprocess.run(["./autogen.sh"], cwd=self.fltk_source, env=env, check=True)
            except subprocess.CalledProcessError:
                print("⚠️  Autogen failed, continuing without it...")
        else:
            print("⚠️  Autoconf not found, skipping autogen step...")
        
        # Configure FLTK (matching the working 1.3 build process)
        configure_cmd = [
            "./configure",
            "--enable-static",
            "--disable-shared",
            "--enable-localjpeg", 
            "--enable-localpng",
            "--enable-localzlib",
            f"--prefix=/tmp/fltk-1.4.4-{target}"
        ]
        
        # Set environment variables for PIC compilation
        env = os.environ.copy()
        env['CFLAGS'] = '-fPIC'
        env['CXXFLAGS'] = '-fPIC'
        
        print(f"🔧 Configuring: {' '.join(configure_cmd)}")
        subprocess.run(configure_cmd, cwd=self.fltk_source, env=env, check=True)
        
        # Build FLTK libraries (skip fluid to avoid linking issues)
        print("🔨 Building FLTK libraries...")
        # Build the builtin libraries first (jpeg, png, zlib)
        for subdir in ["jpeg", "png", "zlib"]:
            print(f"🔨 Building {subdir} library...")
            subprocess.run(["make", "-j", str(os.cpu_count())], cwd=self.fltk_source / subdir, env=env, check=True)
        
        # Build the core libraries from the src directory
        print("🔨 Building core FLTK libraries...")
        subprocess.run(["make", "-j", str(os.cpu_count())], cwd=self.fltk_source / "src", env=env, check=True)
        
        # Copy libraries
        target_dir = self.get_target_dir("linux", target.split("-")[1])
        target_dir.mkdir(parents=True, exist_ok=True)
        
        for lib_file in (self.fltk_source / "lib").glob("*.a"):
            shutil.copy2(lib_file, target_dir)
            print(f"📦 Copied {lib_file.name} to {target_dir}")
        
        # Copy headers (only if not already copied)
        if not self.fl_headers_dir.exists():
            shutil.copytree(self.fltk_source / "FL", self.fl_headers_dir)
            print(f"📋 Copied FLTK headers to {self.fl_headers_dir}")
        
        print(f"✅ FLTK build complete for {target}")
    
    def build_windows(self, target: str):
        """Build FLTK for Windows"""
        print(f"🪟 Building FLTK 1.4.4 for {target}...")
        
        # Check for Visual Studio or MinGW
        if shutil.which("cl"):
            print("🔧 Using Visual Studio compiler...")
            self._build_windows_msvc(target)
        elif shutil.which("gcc"):
            print("🔧 Using MinGW compiler...")
            self._build_windows_mingw(target)
        else:
            raise RuntimeError("No suitable compiler found. Install Visual Studio or MinGW.")
    
    def _build_windows_msvc(self, target: str):
        """Build FLTK using Visual Studio and CMake"""
        print("🔧 Building FLTK using Visual Studio and CMake...")
        
        # Create build directory
        build_dir = self.fltk_source / "build"
        build_dir.mkdir(exist_ok=True)
        
        # Configure with CMake for Visual Studio
        print("🔧 Configuring with CMake for Visual Studio...")
        cmake_cmd = [
            "cmake",
            "-G", "Visual Studio 17 2022",  # Adjust version as needed
            "-A", "x64",  # 64-bit architecture
            "-D", "CMAKE_BUILD_TYPE=Release",
            "-D", "FLTK_BUILD_SHARED_LIBS=OFF",  # Build static libraries
            "-D", "FLTK_BUILD_EXAMPLES=OFF",
            "-D", "FLTK_BUILD_TEST=OFF",
            "-D", "FLTK_BUILD_FLUID=OFF",
            "-D", "FLTK_BUILD_FLTK_OPTIONS=OFF",
            "-D", "FLTK_USE_SYSTEM_LIBJPEG=OFF",
            "-D", "FLTK_USE_SYSTEM_LIBPNG=OFF", 
            "-D", "FLTK_USE_SYSTEM_ZLIB=OFF",
            "-D", "CMAKE_INSTALL_PREFIX=/tmp/fltk-1.4.4-windows-amd64",
            ".."
        ]
        
        print(f"🔧 Running: {' '.join(cmake_cmd)}")
        subprocess.run(cmake_cmd, cwd=build_dir, check=True)
        
        # Build FLTK
        print("🔨 Building FLTK libraries...")
        build_cmd = [
            "cmake", "--build", ".", "--config", "Release"
        ]
        subprocess.run(build_cmd, cwd=build_dir, check=True)
        
        # Copy libraries
        target_dir = self.get_target_dir("win", target.split("-")[1])
        target_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy static libraries from build directory
        lib_dir = build_dir / "lib" / "Release"
        if lib_dir.exists():
            for lib_file in lib_dir.glob("*.lib"):
                shutil.copy2(lib_file, target_dir)
                print(f"📦 Copied {lib_file.name} to {target_dir}")
        else:
            print(f"⚠️  Library directory not found: {lib_dir}")
        
        # Copy headers (only if not already copied)
        if not self.fl_headers_dir.exists():
            shutil.copytree(self.fltk_source / "FL", self.fl_headers_dir)
            print(f"📋 Copied FLTK headers to {self.fl_headers_dir}")
        
        print(f"✅ FLTK build complete for {target}")
    
    def _build_windows_mingw(self, target: str):
        """Build FLTK using MinGW"""
        # Configure FLTK
        configure_cmd = [
            "./configure",
            "--enable-static",
            "--disable-shared",
            "--enable-localjpeg",
            "--enable-localpng", 
            "--enable-localzlib",
            f"--prefix=/tmp/fltk-1.4.4-{target}"
        ]
        
        print(f"🔧 Configuring: {' '.join(configure_cmd)}")
        subprocess.run(configure_cmd, cwd=self.fltk_source, check=True)
        
        # Build FLTK
        print("🔨 Building FLTK libraries...")
        subprocess.run(["make", "-j", str(os.cpu_count())], cwd=self.fltk_source, check=True)
        
        # Copy libraries
        target_dir = self.get_target_dir("win", target.split("-")[1])
        target_dir.mkdir(parents=True, exist_ok=True)
        
        for lib_file in (self.fltk_source / "lib").glob("*.a"):
            shutil.copy2(lib_file, target_dir)
            print(f"📦 Copied {lib_file.name} to {target_dir}")
        
        # Copy headers (only if not already copied)
        if not self.fl_headers_dir.exists():
            shutil.copytree(self.fltk_source / "FL", self.fl_headers_dir)
            print(f"📋 Copied FLTK headers to {self.fl_headers_dir}")
        
        print(f"✅ FLTK build complete for {target}")
    
    def build_aws(self, target: str):
        """Build FLTK for AWS Linux (same as Linux but with AWS-specific notes)"""
        print(f"☁️  Building FLTK 1.4.4 for {target} (AWS Linux)...")
        
        # AWS Linux is essentially the same as regular Linux, but we need to copy to AWS directories
        # First, build using the Linux method (which puts libraries in linux-* directories)
        self.build_linux(target)
        
        # Now copy the libraries from linux-* to aws-* directories
        platform, arch = target.split("-")
        linux_dir = self.get_target_dir("linux", arch)
        aws_dir = self.get_target_dir("aws", arch)
        
        # Create AWS directory if it doesn't exist
        aws_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy all libraries from linux directory to aws directory
        for lib_file in linux_dir.glob("*.a"):
            shutil.copy2(lib_file, aws_dir)
            print(f"📦 Copied {lib_file.name} to {aws_dir}")
        
        print("☁️  AWS Linux build complete")
    

    
    def build_for_target(self, target: str):
        """Build FLTK for a specific target"""
        platform, arch = target.split("-")
        
        # Validate target is supported
        supported_targets = [
            "mac-arm64",
            "linux-arm64", "linux-amd64", 
            "windows-arm64", "windows-amd64",
            "aws-arm64", "aws-amd64"
        ]
        
        if target not in supported_targets:
            raise ValueError(f"Unsupported target: {target}. Supported targets: {', '.join(supported_targets)}")
        
        if platform == "mac":
            self.build_mac(target)
        elif platform == "linux":
            self.build_linux(target)
        elif platform == "windows":
            self.build_windows(target)
        elif platform == "aws":
            self.build_aws(target)
        else:
            raise ValueError(f"Unsupported platform: {platform}")
    
    def build_for_current_platform(self):
        """Build FLTK for the current platform (auto-detected)"""
        platform, arch = self.detect_platform()
        target = f"{platform}-{arch}"
        print(f"🔍 Auto-detected platform: {platform} {arch}")
        print(f"🎯 Building for target: {target}")
        self.build_for_target(target)
    
    def list_targets(self):
        """List all supported targets"""
        targets = [
            "mac-arm64",
            "linux-arm64", "linux-amd64", 
            "windows-arm64", "windows-amd64",
            "aws-arm64", "aws-amd64"
        ]
        print("🎯 Supported targets:")
        for target in targets:
            print(f"  - {target}")
    
    def verify_build(self, target: str):
        """Verify that the build was successful"""
        platform, arch = target.split("-")
        target_dir = self.get_target_dir(platform, arch)
        
        # Different library extensions for different platforms
        if platform == "win" or platform == "windows":
            expected_libs = [
                "fltk.lib",
                "fltk_forms.lib", 
                "fltk_gl.lib",
                "fltk_images.lib",
                "fltk_jpeg.lib",
                "fltk_png.lib",
                "fltk_z.lib"
            ]
        else:
            expected_libs = [
                "libfltk.a",
                "libfltk_forms.a", 
                "libfltk_gl.a",
                "libfltk_images.a",
                "libfltk_jpeg.a",
                "libfltk_png.a",
                "libfltk_z.a"
            ]
        
        print(f"🔍 Verifying build for {target}...")
        missing_libs = []
        
        for lib in expected_libs:
            lib_path = target_dir / lib
            if lib_path.exists():
                size = lib_path.stat().st_size
                print(f"  ✅ {lib} ({size:,} bytes)")
            else:
                print(f"  ❌ {lib} (missing)")
                missing_libs.append(lib)
        
        if missing_libs:
            print(f"⚠️  Missing libraries: {', '.join(missing_libs)}")
            return False
        else:
            print(f"✅ All libraries present for {target}")
            return True

def main():
    parser = argparse.ArgumentParser(
        description="Build FLTK 1.4.4 static libraries for Grapa",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 scripts/build/build_fltk.py                    # Build for current platform
  python3 scripts/build/build_fltk.py --target mac-arm64 # Build for specific target
  python3 scripts/build/build_fltk.py --clean            # Clean build artifacts
  python3 scripts/build/build_fltk.py --list-targets     # List supported targets
  
  # Windows examples (use 'python' instead of 'python3'):
  python scripts\\build\\build_fltk.py --target win-amd64  # Build for Windows x64
  python scripts\\build\\build_fltk.py --clean             # Clean build artifacts
        """
    )
    
    parser.add_argument(
        "--target", 
        help="Target platform-architecture (e.g., mac-arm64, linux-amd64)"
    )
    parser.add_argument(
        "--clean", 
        action="store_true", 
        help="Clean build artifacts"
    )
    parser.add_argument(
        "--list-targets", 
        action="store_true", 
        help="List all supported targets"
    )
    parser.add_argument(
        "--verify", 
        action="store_true", 
        help="Verify build after completion"
    )
    
    args = parser.parse_args()
    
    builder = FLTKBuilder()
    
    # Check if FLTK source exists
    if not builder.fltk_source.exists():
        print(f"❌ FLTK source not found at {builder.fltk_source}")
        print("Please ensure FLTK 1.4.4 is extracted to dep/fltk-1.4.4/")
        return 1
    
    try:
        if args.list_targets:
            builder.list_targets()
            return 0
        
        # Clean if requested
        if args.clean:
            if args.target:
                # Clean only the specific target
                builder.clean_build(args.target)
            else:
                # Clean only the current platform (auto-detected)
                platform, arch = builder.detect_platform()
                target = f"{platform}-{arch}"
                print(f"🔍 Auto-detected platform for cleaning: {platform} {arch}")
                builder.clean_build(target)
        
        # Build if not just listing targets
        if not args.list_targets:
            verification_passed = True  # Default to True if no verification requested
            
            if args.target:
                # Build for specific target
                builder.build_for_target(args.target)
                if args.verify:
                    verification_passed = builder.verify_build(args.target)
            else:
                # Build for current platform (auto-detected)
                builder.build_for_current_platform()
                if args.verify:
                    platform, arch = builder.detect_platform()
                    target = f"{platform}-{arch}"
                    verification_passed = builder.verify_build(target)
            
            if verification_passed:
                print("🎉 FLTK build completed successfully!")
                return 0
            else:
                print("❌ FLTK build completed but verification failed!")
                return 1
        
        print("🎉 FLTK build completed successfully!")
        return 0
        
    except Exception as e:
        print(f"❌ Build failed: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
