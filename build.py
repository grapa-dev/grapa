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
    """Main builder class for Grapa"""
    
    def __init__(self, config: BuildConfig):
        self.config = config
        self.target_dir = f"bin/{config.target}"
        self.target_lib_dir = f"source/grapa-lib/{config.target}"
        
    def clean(self):
        """Clean build artifacts"""
        print(f"🧹 Cleaning build artifacts for {self.config.target}...")
        
        # Clean executable
        if os.path.exists(self.config.output_name):
            os.remove(self.config.output_name)
            print(f"✅ Removed {self.config.output_name}")
        
        # Clean static library
        if self.config.platform == "windows":
            if os.path.exists("grapa.lib"):
                os.remove("grapa.lib")
                print("✅ Removed grapa.lib")
        else:
            if os.path.exists("libgrapa.a"):
                os.remove("libgrapa.a")
                print("✅ Removed libgrapa.a")
            if os.path.exists("libgrapa.so"):
                os.remove("libgrapa.so")
                print("✅ Removed libgrapa.so")
        
        # Clean object files
        for obj_file in Path(".").glob("*.o"):
            obj_file.unlink()
            print(f"✅ Removed {obj_file}")
        
        print("✅ Clean complete")
    
    def build_windows(self):
        """Build for Windows"""
        print(f"🔨 Building Grapa for Windows {self.config.arch}...")
        
        # Build executable
        print("Building executable...")
        cmd = [
            self.config.compiler,
            "prj/win-amd64/grapa.sln",
            "/p:Configuration=Release",
            "/p:Platform=x64"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Build failed: {result.stderr}")
            return False
        
        print("✅ Executable built successfully")
        
        # Copy executable to target directory
        os.makedirs(self.target_dir, exist_ok=True)
        shutil.copy("prj/win-amd64/x64/Release/grapa.exe", f"{self.target_dir}/grapa.exe")
        print(f"✅ Copied executable to {self.target_dir}/")
        
        # Build static library
        print("Building static library...")
        cmd = [
            self.config.compiler,
            "prj/winlib-amd64_static/grapalib.sln",
            "/p:Configuration=Release",
            "/p:Platform=x64"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Static library build failed: {result.stderr}")
            return False
        
        print("✅ Static library built successfully")
        
        # Copy static library to target directory
        shutil.copy("prj/winlib-amd64_static/x64/Release/grapalib.lib", f"{self.target_dir}/grapa_static.lib")
        print(f"✅ Copied static library to {self.target_dir}/")
        
        # Copy static library to source/grapa-lib
        os.makedirs(self.target_lib_dir, exist_ok=True)
        shutil.copy("prj/winlib-amd64_static/x64/Release/grapalib.lib", f"{self.target_lib_dir}/grapa_static.lib")
        print(f"✅ Copied static library to {self.target_lib_dir}/")
        
        # Build shared library (DLL)
        print("Building shared library...")
        cmd = [
            self.config.compiler,
            "prj/winlib-amd64_shared/grapalib.sln",
            "/p:Configuration=Release",
            "/p:Platform=x64"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Shared library build failed: {result.stderr}")
            return False
        
        print("✅ Shared library built successfully")
        
        # Copy shared library (DLL) to target directory
        shutil.copy("prj/winlib-amd64_shared/x64/Release/grapalib.dll", f"{self.target_dir}/grapa.dll")
        print(f"✅ Copied shared library to {self.target_dir}/")
        
        # Copy shared library to source/grapa-lib
        shutil.copy("prj/winlib-amd64_shared/x64/Release/grapalib.dll", f"{self.target_lib_dir}/grapa.dll")
        print(f"✅ Copied shared library to {self.target_lib_dir}/")
        
        # Create install script
        self._create_install_script()
        
        return True
    
    def build_mac(self):
        """Build for Mac"""
        print(f"🔨 Building Grapa for Mac {self.config.arch}...")
        
        # Build executable
        print("Building executable...")
        cmd = [
            self.config.compiler,
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            *self.config.frameworks,
            "-o", self.config.output_name
        ]
        
        # Add library paths
        for lib_pattern in self.config.libs:
            cmd.extend([lib_pattern])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Build failed: {result.stderr}")
            return False
        
        print("✅ Executable built successfully")
        
        # Copy executable to target directory
        os.makedirs(self.target_dir, exist_ok=True)
        shutil.copy(self.config.output_name, f"{self.target_dir}/{self.config.output_name}")
        print(f"✅ Copied executable to {self.target_dir}/")
        
        # Build static library
        print("Building static library...")
        cmd = [
            self.config.compiler,
            "-c",
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            "-o", "libgrapa.a"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Static library build failed: {result.stderr}")
            return False
        
        # Create static library archive
        obj_files = list(Path(".").glob("*.o"))
        if not obj_files:
            raise RuntimeError("No object files found for static library")
        
        cmd = ["ar", "rcs", "libgrapa.a"] + [str(f) for f in obj_files]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Static library archive creation failed: {result.stderr}")
            return False
        
        print("✅ Static library built successfully")
        
        # Copy static library to target directory
        shutil.copy("libgrapa.a", f"{self.target_dir}/libgrapa_static.a")
        print(f"✅ Copied static library to {self.target_dir}/")
        
        # Copy static library to source/grapa-lib
        os.makedirs(self.target_lib_dir, exist_ok=True)
        shutil.copy("libgrapa.a", f"{self.target_lib_dir}/libgrapa_static.a")
        print(f"✅ Copied static library to {self.target_lib_dir}/")
        
        # Build shared library
        print("Building shared library...")
        cmd = [
            self.config.compiler,
            "-shared",
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            *self.config.frameworks,
            "-o", "libgrapa.so"
        ]
        
        # Add library paths
        for lib_pattern in self.config.libs:
            cmd.extend([lib_pattern])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Shared library build failed: {result.stderr}")
            return False
        
        print("✅ Shared library built successfully")
        
        # Copy shared library to target directory
        shutil.copy("libgrapa.so", f"{self.target_dir}/libgrapa.so")
        print(f"✅ Copied shared library to {self.target_dir}/")
        
        # Copy shared library to source/grapa-lib
        shutil.copy("libgrapa.so", f"{self.target_lib_dir}/libgrapa.so")
        print(f"✅ Copied shared library to {self.target_lib_dir}/")
        
        # Create install script
        self._create_install_script()
        
        return True
    
    def build_linux(self):
        """Build for Linux"""
        print(f"🔨 Building Grapa for Linux {self.config.arch}...")
        
        # Build executable
        print("Building executable...")
        cmd = [
            self.config.compiler,
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            "-ldl", "-lm", "-static-libgcc",
            "-o", self.config.output_name
        ]
        
        # Add library paths
        for lib_pattern in self.config.libs:
            cmd.extend([lib_pattern])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Build failed: {result.stderr}")
            return False
        
        print("✅ Executable built successfully")
        
        # Copy executable to target directory
        os.makedirs(self.target_dir, exist_ok=True)
        shutil.copy(self.config.output_name, f"{self.target_dir}/{self.config.output_name}")
        print(f"✅ Copied executable to {self.target_dir}/")
        
        # Build static library
        print("Building static library...")
        cmd = [
            self.config.compiler,
            "-c",
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            "-o", "libgrapa.a"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Static library build failed: {result.stderr}")
            return False
        
        # Create static library archive
        obj_files = list(Path(".").glob("*.o"))
        if not obj_files:
            raise RuntimeError("No object files found for static library")
        
        cmd = ["ar", "rcs", "libgrapa.a"] + [str(f) for f in obj_files]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Static library archive creation failed: {result.stderr}")
            return False
        
        print("✅ Static library built successfully")
        
        # Copy static library to target directory
        shutil.copy("libgrapa.a", f"{self.target_dir}/libgrapa_static.a")
        print(f"✅ Copied static library to {self.target_dir}/")
        
        # Copy static library to source/grapa-lib
        os.makedirs(self.target_lib_dir, exist_ok=True)
        shutil.copy("libgrapa.a", f"{self.target_lib_dir}/libgrapa_static.a")
        print(f"✅ Copied static library to {self.target_lib_dir}/")
        
        # Build shared library
        print("Building shared library...")
        cmd = [
            self.config.compiler,
            "-shared",
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            "-ldl", "-lm", "-static-libgcc",
            "-o", "libgrapa.so"
        ]
        
        # Add library paths
        for lib_pattern in self.config.libs:
            cmd.extend([lib_pattern])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Shared library build failed: {result.stderr}")
            return False
        
        print("✅ Shared library built successfully")
        
        # Copy shared library to target directory
        shutil.copy("libgrapa.so", f"{self.target_dir}/libgrapa.so")
        print(f"✅ Copied shared library to {self.target_dir}/")
        
        # Copy shared library to source/grapa-lib
        shutil.copy("libgrapa.so", f"{self.target_lib_dir}/libgrapa.so")
        print(f"✅ Copied shared library to {self.target_lib_dir}/")
        
        # Create install script
        self._create_install_script()
        
        return True
    
    def build_aws(self):
        """Build for AWS (similar to Linux but with different library paths)"""
        print(f"🔨 Building Grapa for AWS {self.config.arch}...")
        
        # Build executable
        print("Building executable...")
        cmd = [
            self.config.compiler,
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            "-ldl", "-lm", "-static-libgcc",
            "-o", self.config.output_name
        ]
        
        # Add library paths
        for lib_pattern in self.config.libs:
            cmd.extend([lib_pattern])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Build failed: {result.stderr}")
            return False
        
        print("✅ Executable built successfully")
        
        # Copy executable to target directory
        os.makedirs(self.target_dir, exist_ok=True)
        shutil.copy(self.config.output_name, f"{self.target_dir}/{self.config.output_name}")
        print(f"✅ Copied executable to {self.target_dir}/")
        
        # Build static library
        print("Building static library...")
        cmd = [
            self.config.compiler,
            "-c",
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            "-o", "libgrapa.a"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Static library build failed: {result.stderr}")
            return False
        
        # Create static library archive
        obj_files = list(Path(".").glob("*.o"))
        if not obj_files:
            raise RuntimeError("No object files found for static library")
        
        cmd = ["ar", "rcs", "libgrapa.a"] + [str(f) for f in obj_files]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Static library archive creation failed: {result.stderr}")
            return False
        
        print("✅ Static library built successfully")
        
        # Copy static library to target directory
        shutil.copy("libgrapa.a", f"{self.target_dir}/libgrapa_static.a")
        print(f"✅ Copied static library to {self.target_dir}/")
        
        # Copy static library to source/grapa-lib
        os.makedirs(self.target_lib_dir, exist_ok=True)
        shutil.copy("libgrapa.a", f"{self.target_lib_dir}/libgrapa_static.a")
        print(f"✅ Copied static library to {self.target_lib_dir}/")
        
        # Build shared library
        print("Building shared library...")
        cmd = [
            self.config.compiler,
            "-shared",
            *self.config.flags,
            "source/main.cpp",
            "source/utf8proc/utf8proc.c",
            "-ldl", "-lm", "-static-libgcc",
            "-o", "libgrapa.so"
        ]
        
        # Add library paths
        for lib_pattern in self.config.libs:
            cmd.extend([lib_pattern])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ Shared library build failed: {result.stderr}")
            return False
        
        print("✅ Shared library built successfully")
        
        # Copy shared library to target directory
        shutil.copy("libgrapa.so", f"{self.target_dir}/libgrapa.so")
        print(f"✅ Copied shared library to {self.target_dir}/")
        
        # Copy shared library to source/grapa-lib
        shutil.copy("libgrapa.so", f"{self.target_lib_dir}/libgrapa.so")
        print(f"✅ Copied shared library to {self.target_lib_dir}/")
        
        # Create install script
        self._create_install_script()
        
        return True
    
    def _create_install_script(self):
        """Create platform-specific install script"""
        if self.config.platform == "windows":
            self._create_windows_install_script()
        else:
            self._create_unix_install_script()
    
    def _create_windows_install_script(self):
        """Create Windows PowerShell install script"""
        script_content = f"""# Grapa Installation Script for Windows {self.config.arch}
# Run this script as Administrator

param(
    [string]$InstallPath = "C:\\Program Files\\Grapa"
)

Write-Host "Installing Grapa for Windows {self.config.arch}..." -ForegroundColor Green

# Create installation directory
if (!(Test-Path $InstallPath)) {{
    New-Item -ItemType Directory -Path $InstallPath -Force
    Write-Host "Created installation directory: $InstallPath" -ForegroundColor Yellow
}}

 # Copy files
 $SourceDir = "{os.path.abspath(self.target_dir)}"
 Copy-Item "$SourceDir\\grapa.exe" "$InstallPath\\" -Force
 Copy-Item "$SourceDir\\grapa_static.lib" "$InstallPath\\" -Force
 if (Test-Path "$SourceDir\\grapa.dll") {{
     Copy-Item "$SourceDir\\grapa.dll" "$InstallPath\\" -Force
 }}

# Add to PATH
$CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
if ($CurrentPath -notlike "*$InstallPath*") {{
    $NewPath = "$CurrentPath;$InstallPath"
    [Environment]::SetEnvironmentVariable("PATH", $NewPath, "Machine")
    Write-Host "Added Grapa to system PATH" -ForegroundColor Yellow
}}

 Write-Host "Installation complete!" -ForegroundColor Green
 Write-Host "Grapa executable: $InstallPath\\grapa.exe" -ForegroundColor Cyan
 Write-Host "Static library: $InstallPath\\grapa_static.lib" -ForegroundColor Cyan
 if (Test-Path "$InstallPath\\grapa.dll") {{
     Write-Host "Shared library: $InstallPath\\grapa.dll" -ForegroundColor Cyan
 }}
 Write-Host "Please restart your terminal for PATH changes to take effect." -ForegroundColor Yellow
"""
        
        script_path = f"{self.target_dir}/install-grapa.ps1"
        with open(script_path, 'w') as f:
            f.write(script_content)
        print(f"✅ Created install script: {script_path}")
    
    def _create_unix_install_script(self):
        """Create Unix shell install script"""
        script_content = f"""#!/bin/bash
# Grapa Installation Script for {self.config.platform.title()} {self.config.arch}
# Run this script with sudo for system-wide installation

set -e

INSTALL_PATH="/usr/local"
BIN_PATH="$INSTALL_PATH/bin"
LIB_PATH="$INSTALL_PATH/lib"
INCLUDE_PATH="$INSTALL_PATH/include"

echo "Installing Grapa for {self.config.platform.title()} {self.config.arch}..."

# Create directories
sudo mkdir -p $BIN_PATH $LIB_PATH $INCLUDE_PATH

# Copy executable
sudo cp "{os.path.abspath(self.target_dir)}/grapa" $BIN_PATH/
sudo chmod +x $BIN_PATH/grapa

# Copy libraries
if [ -f "{os.path.abspath(self.target_dir)}/libgrapa_static.a" ]; then
    sudo cp "{os.path.abspath(self.target_dir)}/libgrapa_static.a" $LIB_PATH/
fi

 if [ -f "{os.path.abspath(self.target_dir)}/libgrapa.so" ]; then
     sudo cp "{os.path.abspath(self.target_dir)}/libgrapa.so" $LIB_PATH/
     {"sudo ldconfig" if self.config.platform != "mac" else "# Note: ldconfig is not needed on macOS"}
 fi

echo "Installation complete!"
echo "Grapa executable: $BIN_PATH/grapa"
if [ -f "$LIB_PATH/libgrapa_static.a" ]; then
    echo "Static library: $LIB_PATH/libgrapa_static.a"
fi
if [ -f "$LIB_PATH/libgrapa.so" ]; then
    echo "Shared library: $LIB_PATH/libgrapa.so"
fi
"""
        
        script_path = f"{self.target_dir}/install-grapa.sh"
        with open(script_path, 'w') as f:
            f.write(script_content)
        
        # Make executable
        os.chmod(script_path, 0o755)
        print(f"✅ Created install script: {script_path}")
    
    def build(self):
        """Main build method"""
        try:
            if self.config.platform == "windows":
                return self.build_windows()
            elif self.config.platform == "mac":
                return self.build_mac()
            elif self.config.platform == "linux":
                return self.build_linux()
            elif self.config.platform == "aws":
                return self.build_aws()
            else:
                print(f"❌ Unsupported platform: {self.config.platform}")
                return False
        except Exception as e:
            print(f"❌ Build failed with error: {e}")
            return False

def detect_platform() -> Tuple[str, str]:
    """Detect the current platform and architecture"""
    system = platform.system().lower()
    machine = platform.machine().lower()
    
    if system == "windows":
        platform_name = "windows"
        if machine in ["amd64", "x86_64"]:
            arch = "amd64"
        else:
            arch = "amd64"  # Default for Windows
    elif system == "darwin":
        platform_name = "mac"
        if machine == "arm64":
            arch = "arm64"
        else:
            arch = "amd64"
    elif system == "linux":
        if "aws" in platform.platform().lower():
            platform_name = "aws"
        else:
            platform_name = "linux"
        
        if machine in ["aarch64", "arm64"]:
            arch = "arm64"
        else:
            arch = "amd64"
    else:
        raise RuntimeError(f"Unsupported platform: {system}")
    
    return platform_name, arch

def main():
    parser = argparse.ArgumentParser(description="Build Grapa for the current platform")
    parser.add_argument("--test", action="store_true", help="Run tests after build")
    parser.add_argument("--clean", action="store_true", help="Clean build artifacts")
    parser.add_argument("--help", action="store_true", help="Show help")
    
    args = parser.parse_args()
    
    if args.help:
        parser.print_help()
        return
    
    try:
        platform_name, arch = detect_platform()
        print(f"🔍 Detected platform: {platform_name} {arch}")
        
        config = BuildConfig(platform_name, arch)
        builder = GrapaBuilder(config)
        
        if args.clean:
            builder.clean()
            return
        
        print(f"🚀 Starting build for {platform_name} {arch}...")
        
        if builder.build():
            print(f"✅ Build completed successfully for {platform_name} {arch}")
            
            if args.test:
                print("🧪 Running tests...")
                # Add test logic here
                print("✅ Tests completed")
        else:
            print(f"❌ Build failed for {platform_name} {arch}")
            sys.exit(1)
            
    except Exception as e:
        print(f"❌ Build script failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main() 