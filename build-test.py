#!/usr/bin/env python3
"""
Debug script for Linux ARM64 cross-compilation
Captures full command and output for debugging
"""

import os
import sys
import subprocess
import glob
import platform

def main():
    print("=== Linux ARM64 Cross-Compilation Debug ===")
    
    # Check if we're in the right environment
    if platform.machine() != "x86_64":
        print("Error: This script is designed to run on AMD64 for ARM64 cross-compilation")
        return 1
    
    # Check if sysroot exists
    sysroot_path = "./arm64-root"
    if not os.path.exists(sysroot_path):
        print(f"Error: Sysroot not found at {sysroot_path}")
        return 1
    
    print(f"Using sysroot: {sysroot_path}")
    
    # Reduced set of source files for debugging
    cpp_files = [
        "source/grapa/GrapaLink.cpp",
        "source/grapa/GrapaValue.cpp", 
        "source/grapa/GrapaInt.cpp"
    ]
    
    # Check if source files exist
    for cpp_file in cpp_files:
        if not os.path.exists(cpp_file):
            print(f"Error: Source file not found: {cpp_file}")
            return 1
    
    # Get library files
    openssl_libs = glob.glob("source/openssl-lib/linux-arm64/*.a")
    if not openssl_libs:
        print("Error: OpenSSL libraries not found")
        return 1
    
    print(f"Found OpenSSL libraries: {openssl_libs}")
    
    # Build utf8proc first
    print("Building utf8proc...")
    utf8proc_cmd = [
        "aarch64-linux-gnu-gcc", "-Isource", "-DUTF8PROC_STATIC", "-c",
        "source/utf8proc/utf8proc.c", "-O3"
    ]
    
    # Add sysroot flags
    sysroot_flags = [
        f"--sysroot={sysroot_path}",
        "-I" + os.path.join(sysroot_path, "usr/include"),
        "-L" + os.path.join(sysroot_path, "usr/lib/aarch64-linux-gnu"),
        "-L" + os.path.join(sysroot_path, "lib/aarch64-linux-gnu")
    ]
    utf8proc_cmd.extend(sysroot_flags)
    
    print(f"utf8proc command: {' '.join(utf8proc_cmd)}")
    
    try:
        subprocess.run(utf8proc_cmd, check=True)
        print("utf8proc compilation successful")
    except subprocess.CalledProcessError as e:
        print(f"utf8proc compilation failed: {e}")
        return 1
    
    # Build main executable with reduced set
    print("Building main executable...")
    
    # System libraries (minimal set)
    system_libs = ["-ldl", "-lm", "-static-libgcc"]
    
    # Main build command
    cmd = [
        "aarch64-linux-gnu-g++", "-Isource", "-DUTF8PROC_STATIC", "source/main.cpp"
    ] + cpp_files + ["source/utf8proc/utf8proc.c"] + openssl_libs + [
        "-Lsource/openssl-lib/linux-arm64", "-std=c++17", "-lcrypto"
    ] + system_libs + [
        "-O3", "-pthread", "-o", "grapa-test"
    ] + sysroot_flags
    
    print(f"Full command ({len(cmd)} arguments):")
    print(" ".join(cmd))
    
    # Capture full output
    print("\n=== Executing build command ===")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        print("Build successful!")
        print("stdout:", result.stdout)
        return 0
    except subprocess.CalledProcessError as e:
        print(f"Build failed with exit code {e.returncode}")
        print("stdout:", e.stdout)
        print("stderr:", e.stderr)
        
        # Write debug log
        with open("build-debug.log", "w") as f:
            f.write(f"Command: {' '.join(cmd)}\n")
            f.write(f"Exit code: {e.returncode}\n")
            f.write(f"stdout:\n{e.stdout}\n")
            f.write(f"stderr:\n{e.stderr}\n")
        
        print("Debug log written to build-debug.log")
        return 1

if __name__ == "__main__":
    sys.exit(main())
