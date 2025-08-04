#!/usr/bin/env python3
"""
Debug build script for Linux ARM64 cross-compilation
Captures full g++ command and output to build-debug.log for troubleshooting
"""

import subprocess
import sys
import os
from pathlib import Path

def run_cross_compile():
    cmd = [
        "aarch64-linux-gnu-g++",
        "-Isource",
        "-DUTF8PROC_STATIC",
        "source/main.cpp",
        # Reduced set of files for debugging
        "source/grapa/GrapaInt.cpp",
        "source/grapa/GrapaState.cpp", 
        "source/grapa/GrapaDB.cpp",
        "source/utf8proc/utf8proc.c",
        "source/openssl-lib/linux-arm64/libcrypto.a",
        "source/openssl-lib/linux-arm64/libssl.a"
    ]

    print("Writing build command to build-debug.log...")
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

if __name__ == "__main__":
    try:
        run_cross_compile()
        print("Build succeeded.")
    except Exception as e:
        print(f"Build failed: {e}")
        sys.exit(1) 