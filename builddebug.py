# builddebug.py - Build script for debug-btree project (BTree investigation)
# This script builds only the debug-btree executable using maindebug.cpp as the entry point.

import os
import platform
import subprocess
import sys

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
PRJ_DIR = os.path.join(PROJECT_ROOT, 'prj', 'debug-btree')
SRC_DIR = os.path.join(PROJECT_ROOT, 'source')

PLATFORM = platform.system().lower()

if PLATFORM == 'windows':
    # Use MSBuild to build the debug-btree solution
    sln_path = os.path.join(PRJ_DIR, 'grapa.sln')
    print(f"Building debug-btree (Windows) with solution: {sln_path}")
    result = subprocess.run(['msbuild', sln_path, '/p:Configuration=Release'])
    if result.returncode != 0:
        print("Build failed.")
        sys.exit(1)
    print("Build succeeded.")
    exe_path = os.path.join(PROJECT_ROOT, 'prj', 'debug-btree', 'x64', 'Release', 'grapa.exe')
    if os.path.exists(exe_path):
        print(f"Executable built: {exe_path}")
    else:
        print("Executable not found after build.")
        sys.exit(1)
else:
    # For Linux/Mac, use g++/clang++ to build maindebug.cpp and all needed sources
    exe_name = 'grapa-debug'
    cpp_files = [os.path.join(SRC_DIR, 'maindebug.cpp')]
    # Add any additional required source files here if needed
    cmd = ['g++', '-std=c++17', '-O2', '-pthread', '-Isource', '-o', exe_name] + cpp_files
    print(f"Building debug-btree ({PLATFORM}) with command: {' '.join(cmd)}")
    result = subprocess.run(cmd)
    if result.returncode != 0:
        print("Build failed.")
        sys.exit(1)
    print(f"Build succeeded. Executable: {exe_name}") 