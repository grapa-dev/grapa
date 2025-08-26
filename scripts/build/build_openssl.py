#!/usr/bin/env python3
"""
OpenSSL 3.5.2 Build Script for Grapa

This script builds OpenSSL 3.5.2 static libraries for all supported platforms.
It replicates the functionality of build_fltk.py but is adapted for OpenSSL's
build process and requirements.

Usage:
    python3 scripts/build/build_openssl.py                    # Build for current platform
    python3 scripts/build/build_openssl.py --target mac-arm64 # Build for specific target
    python3 scripts/build/build_openssl.py --clean            # Clean build artifacts
    python3 scripts/build/build_openssl.py --list-targets     # List supported targets

Supported Platforms:
    - mac-arm64 (Apple Silicon only)
    - linux-arm64, linux-amd64  
    - win-arm64, win-amd64
    - aws-arm64, aws-amd64
"""

import argparse
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Tuple


class OpenSSLBuilder:
    def __init__(self, update_headers=False):
        self.project_root = Path(__file__).parent.parent.parent
        self.openssl_source = self.project_root / "dep" / "openssl-3.5.2"
        self.openssl_lib_dir = self.project_root / "source" / "openssl-lib"
        self.openssl_headers_dir = self.project_root / "source" / "openssl"
        self.update_headers = update_headers

    def detect_platform(self) -> Tuple[str, str]:
        """Detect current platform and architecture"""
        system = platform.system().lower()
        machine = platform.machine().lower()
        
        if system == "windows":
            return "win", "amd64"
        elif system == "darwin":
            # On Mac, only support Apple Silicon (arm64)
            if machine == "arm64":
                return "mac", "arm64"
            else:
                raise RuntimeError(f"mac-amd64 is no longer supported. Please use an Apple Silicon Mac (arm64). Current architecture: {machine}")
        elif system == "linux":
            # Check if this is AWS Linux by looking for Amazon Linux specific files
            if os.path.exists("/etc/system-release") and "Amazon Linux" in open("/etc/system-release").read():
                if machine == "x86_64":
                    return "aws", "amd64"
                elif machine == "aarch64":
                    return "aws", "arm64"
                else:
                    raise RuntimeError(f"Unsupported AWS architecture: {machine}")
            else:
                if machine == "x86_64":
                    return "linux", "amd64"
                elif machine == "aarch64":
                    return "linux", "arm64"
                else:
                    raise RuntimeError(f"Unsupported Linux architecture: {machine}")
        else:
            raise RuntimeError(f"Unsupported operating system: {system}")

    def get_target_dir(self, platform: str, arch: str) -> Path:
        """Get target directory for platform-architecture combination"""
        return self.openssl_lib_dir / f"{platform}-{arch}"

    def clean_build(self, target: str):
        """Clean build artifacts for a specific target"""
        print(f"🧹 Cleaning OpenSSL build artifacts...")
        
        # Clean OpenSSL source
        if self.openssl_source.exists():
            try:
                subprocess.run(["make", "clean"], cwd=self.openssl_source, check=False)
                print("✅ OpenSSL source cleaned")
            except Exception:
                print("⚠️  Could not clean OpenSSL source (make clean failed)")
        
        # Remove target libraries
        platform, arch = target.split("-")
        target_dir = self.get_target_dir(platform, arch)
        
        if target_dir.exists():
            for lib_file in target_dir.glob("*.a"):
                os.remove(lib_file)
                print(f"🗑️  Removed {lib_file}")
            for lib_file in target_dir.glob("*.lib"):
                os.remove(lib_file)
                print(f"🗑️  Removed {lib_file}")
            print(f"✅ Cleaned libraries for {target}")

    def build_mac(self, target: str):
        """Build OpenSSL for macOS"""
        print(f"🍎 Building OpenSSL 3.5.2 for {target}...")
        
        # Configure OpenSSL for macOS
        configure_cmd = [
            "./config",
            "no-shared",
            "no-dso",
            "no-hw",
            "no-engine",
            "no-asm",
            "enable-deprecated",
            "--prefix=/tmp/openssl-3.5.2-mac-arm64"
        ]
        
        # Set environment variables
        env = os.environ.copy()
        env['CFLAGS'] = '-fPIC -DOPENSSL_SUPPRESS_DEPRECATED'
        env['CXXFLAGS'] = '-fPIC -DOPENSSL_SUPPRESS_DEPRECATED'
        
        print(f"🔧 Configuring: {' '.join(configure_cmd)}")
        subprocess.run(configure_cmd, cwd=self.openssl_source, env=env, check=True)
        
        # Build OpenSSL
        print("🔨 Building OpenSSL libraries...")
        subprocess.run(["make", "-j", str(os.cpu_count())], cwd=self.openssl_source, env=env, check=True)
        
        # Copy libraries
        target_dir = self.get_target_dir("mac", target.split("-")[1])
        target_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy static libraries (OpenSSL builds them in the root directory)
        for lib_file in self.openssl_source.glob("*.a"):
            shutil.copy2(lib_file, target_dir)
            print(f"📦 Copied {lib_file.name} to {target_dir}")
        
        # Copy headers (only if --update-headers is specified)
        if self.update_headers:
            if self.openssl_headers_dir.exists():
                shutil.rmtree(self.openssl_headers_dir)
            shutil.copytree(self.openssl_source / "include" / "openssl", self.openssl_headers_dir)
            print(f"📋 Updated OpenSSL headers in {self.openssl_headers_dir}")
        else:
            print("📋 Skipping header update (use --update-headers to update)")
        
        print(f"✅ OpenSSL build complete for {target}")

    def build_linux(self, target: str, platform_override: str = None):
        """Build OpenSSL for Linux"""
        platform, arch = target.split("-")
        if platform_override:
            platform = platform_override
            
        print(f"🐧 Building OpenSSL 3.5.2 for {target}...")
        
        # Configure OpenSSL for Linux
        configure_cmd = [
            "./config",
            "no-shared",
            "no-dso",
            "no-hw",
            "no-engine",
            "enable-deprecated",
            "--prefix=/tmp/openssl-3.5.2-" + platform + "-" + arch
        ]
        
        # Set environment variables for PIC compilation
        env = os.environ.copy()
        env['CFLAGS'] = '-fPIC -DOPENSSL_SUPPRESS_DEPRECATED'
        env['CXXFLAGS'] = '-fPIC -DOPENSSL_SUPPRESS_DEPRECATED'
        
        print(f"🔧 Configuring: {' '.join(configure_cmd)}")
        subprocess.run(configure_cmd, cwd=self.openssl_source, env=env, check=True)
        
        # Build OpenSSL
        print("🔨 Building OpenSSL libraries...")
        subprocess.run(["make", "-j", str(os.cpu_count())], cwd=self.openssl_source, env=env, check=True)
        
        # Copy libraries
        target_dir = self.get_target_dir(platform, arch)
        target_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy static libraries (OpenSSL builds them in the root directory)
        for lib_file in self.openssl_source.glob("*.a"):
            shutil.copy2(lib_file, target_dir)
            print(f"📦 Copied {lib_file.name} to {target_dir}")
        
        # Copy headers (only if --update-headers is specified)
        if self.update_headers:
            if self.openssl_headers_dir.exists():
                shutil.rmtree(self.openssl_headers_dir)
            shutil.copytree(self.openssl_source / "include" / "openssl", self.openssl_headers_dir)
            print(f"📋 Updated OpenSSL headers in {self.openssl_headers_dir}")
        else:
            print("📋 Skipping header update (use --update-headers to update)")
        
        print(f"✅ OpenSSL build complete for {target}")

    def build_windows(self, target: str):
        """Build OpenSSL for Windows"""
        print(f"🪟 Building OpenSSL 3.5.2 for {target}...")
        
        # Check for Visual Studio
        if shutil.which("cl"):
            print("🔧 Using Visual Studio compiler...")
            self._build_windows_msvc(target)
        else:
            raise RuntimeError("Visual Studio compiler not found. Install Visual Studio with C++ tools.")

    def _build_windows_msvc(self, target: str):
        """Build OpenSSL using Visual Studio"""
        print("🔧 Building OpenSSL using Visual Studio...")
        
        # Configure with Perl - try to find Perl in common locations
        print("🔧 Configuring with Perl...")
        
        # Try to find Perl in common installation locations
        perl_paths = [
            "perl",  # Try PATH first
            r"C:\Strawberry\perl\bin\perl.exe",
            r"C:\Perl64\bin\perl.exe",
            r"C:\Perl\bin\perl.exe"
        ]
        
        perl_cmd = None
        for perl_path in perl_paths:
            try:
                result = subprocess.run([perl_path, "--version"], 
                                      capture_output=True, text=True, check=False)
                if result.returncode == 0:
                    perl_cmd = perl_path
                    print(f"✅ Found Perl at: {perl_cmd}")
                    break
            except:
                continue
        
        if not perl_cmd:
            raise RuntimeError("Perl not found. Please install Strawberry Perl or ensure it's in PATH.")
        
        # Set environment variables to suppress deprecation warnings
        env = os.environ.copy()
        env['CFLAGS'] = '-DOPENSSL_SUPPRESS_DEPRECATED'
        env['CXXFLAGS'] = '-DOPENSSL_SUPPRESS_DEPRECATED'
        
        configure_cmd = [
            perl_cmd, "Configure", "VC-WIN64A", "no-shared", "no-dso", "no-hw", "no-engine"
        ]
        
        subprocess.run(configure_cmd, cwd=self.openssl_source, env=env, check=True)
        
        # Build OpenSSL
        print("🔨 Building OpenSSL libraries...")
        subprocess.run(["nmake"], cwd=self.openssl_source, env=env, check=True)
        
        # Copy libraries
        target_dir = self.get_target_dir("win", target.split("-")[1])
        target_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy all library files (OpenSSL builds them in the root directory)
        for lib_file in self.openssl_source.glob("*.lib"):
            shutil.copy2(lib_file, target_dir)
            print(f"📦 Copied {lib_file.name} to {target_dir}")
        
        # Copy PDB files if they exist
        for pdb_file in self.openssl_source.glob("*.pdb"):
            shutil.copy2(pdb_file, target_dir)
            print(f"📦 Copied {pdb_file.name} to {target_dir}")
        
        # Copy headers (only if --update-headers is specified)
        if self.update_headers:
            if self.openssl_headers_dir.exists():
                shutil.rmtree(self.openssl_headers_dir)
            shutil.copytree(self.openssl_source / "include" / "openssl", self.openssl_headers_dir)
            print(f"📋 Updated OpenSSL headers in {self.openssl_headers_dir}")
        else:
            print("📋 Skipping header update (use --update-headers to update)")
        
        print(f"✅ OpenSSL build complete for {target}")

    def build_aws(self, target: str):
        """Build OpenSSL for AWS Linux"""
        print(f"☁️  Building OpenSSL 3.5.2 for {target}...")
        
        # AWS Linux uses the same build process as regular Linux, but with correct platform
        self.build_linux(target, platform_override="aws")

    def build_for_target(self, target: str):
        """Build OpenSSL for a specific target"""
        platform, arch = target.split("-")
        
        # Validate target is supported
        supported_targets = [
            "mac-arm64",
            "linux-arm64", "linux-amd64", 
            "win-arm64", "win-amd64",
            "aws-arm64", "aws-amd64"
        ]
        
        if target not in supported_targets:
            raise ValueError(f"Unsupported target: {target}. Supported targets: {', '.join(supported_targets)}")
        
        if platform == "mac":
            self.build_mac(target)
        elif platform == "linux":
            self.build_linux(target)
        elif platform == "win":
            self.build_windows(target)
        elif platform == "aws":
            self.build_aws(target)
        else:
            raise ValueError(f"Unknown platform: {platform}")

    def list_targets(self):
        """List all supported targets"""
        targets = [
            "mac-arm64",
            "linux-arm64", "linux-amd64", 
            "win-arm64", "win-amd64",
            "aws-arm64", "aws-amd64"
        ]
        
        print("Supported targets:")
        for target in targets:
            print(f"  - {target}")

    def verify_build(self, target: str) -> bool:
        """Verify that the build was successful"""
        platform, arch = target.split("-")
        target_dir = self.get_target_dir(platform, arch)
        
        expected_libs = [
            "libcrypto.a",
            "libssl.a"
        ]
        
        # Add Windows equivalents
        if platform == "win":
            expected_libs = [
                "libcrypto.lib",
                "libssl.lib"
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
        description="Build OpenSSL 3.5.2 static libraries for Grapa",
        formatter_class=argparse.RawDescriptionHelpFormatter,
                       epilog="""
       Examples:
         python3 scripts/build/build_openssl.py                    # Build for current platform
         python3 scripts/build/build_openssl.py --target mac-arm64 # Build for specific target
         python3 scripts/build/build_openssl.py --clean            # Clean build artifacts
         python3 scripts/build/build_openssl.py --list-targets     # List supported targets
         python3 scripts/build/build_openssl.py --update-headers   # Update headers (only needed once per version)
         
         # Windows examples (use 'python' instead of 'python3'):
         python scripts\\build\\build_openssl.py --target win-amd64  # Build for Windows x64
         python scripts\\build\\build_openssl.py --clean             # Clean build artifacts
         python scripts\\build\\build_openssl.py --update-headers    # Update headers (only needed once per version)
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
    parser.add_argument(
        "--update-headers", 
        action="store_true", 
        help="Update OpenSSL headers in source/openssl (only needed once per version)"
    )
    
    args = parser.parse_args()
    
    builder = OpenSSLBuilder(update_headers=args.update_headers)
    
    # Check if OpenSSL source exists
    if not builder.openssl_source.exists():
        print(f"❌ OpenSSL source not found at {builder.openssl_source}")
        print("Please ensure OpenSSL 3.5.2 is extracted to dep/openssl-3.5.2/")
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
                platform, arch = builder.detect_platform()
                target = f"{platform}-{arch}"
                print(f"🔍 Auto-detected platform for building: {platform} {arch}")
                builder.build_for_target(target)
                if args.verify:
                    verification_passed = builder.verify_build(target)
            
            if verification_passed:
                print("🎉 OpenSSL build completed successfully!")
                return 0
            else:
                print("❌ OpenSSL build verification failed!")
                return 1
        
        return 0
        
    except Exception as e:
        print(f"❌ Build failed: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
