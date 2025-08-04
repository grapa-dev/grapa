
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
        # add a subset of files for quick debug iteration (adjust as needed)
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
        try:
            process = subprocess.run(cmd, stdout=f, stderr=subprocess.STDOUT)
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
