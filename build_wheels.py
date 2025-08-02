#!/usr/bin/env python3
"""
Build wheels for grapapy package.
This script builds wheels for the current platform and Python version.
"""

import os
import sys
import subprocess
import platform
from pathlib import Path

def build_wheel():
    """Build a wheel for the current platform"""
    print(f"Building wheel for {platform.system()} {platform.machine()}")
    print(f"Python version: {sys.version}")
    
    # Clean previous builds
    for path in ['build', 'dist', '*.egg-info']:
        if os.path.exists(path):
            import shutil
            shutil.rmtree(path)
    
    # Install build dependencies
    subprocess.run([sys.executable, '-m', 'pip', 'install', '--upgrade', 'pip', 'wheel', 'setuptools', 'build'], check=True)
    
    # Build wheel
    subprocess.run([sys.executable, '-m', 'build', '--wheel'], check=True)
    
    # List built wheels
    dist_dir = Path('dist')
    if dist_dir.exists():
        wheels = list(dist_dir.glob('*.whl'))
        print(f"\nBuilt wheels:")
        for wheel in wheels:
            print(f"  {wheel.name}")
    else:
        print("No wheels found in dist/ directory")

if __name__ == '__main__':
    try:
        build_wheel()
        print("\n✅ Wheel build completed successfully!")
    except subprocess.CalledProcessError as e:
        print(f"\n❌ Wheel build failed: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Unexpected error: {e}")
        sys.exit(1) 