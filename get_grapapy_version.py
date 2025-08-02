#!/usr/bin/env python3
"""
GrapaPy Version Checker
Loads GrapaPy and displays the installed version information.
"""

import sys

def get_grapapy_version():
    """Get the version of GrapaPy installed in the current environment."""
    try:
        import grapapy
        version = grapapy.__version__
        print(f"✅ GrapaPy version: {version}")
        return version
    except ImportError as e:
        print(f"❌ Error importing GrapaPy: {e}")
        print("   Make sure GrapaPy is installed: pip3 install grapapy")
        return None
    except AttributeError as e:
        print(f"❌ Error accessing GrapaPy version: {e}")
        print("   The installed GrapaPy package may not have a __version__ attribute")
        return None

def main():
    """Main function to check GrapaPy version."""
    print("GrapaPy Version Checker")
    print("=" * 40)
    
    # Get version
    version = get_grapapy_version()
    
    if version:
        print(f"\n📦 GrapaPy {version} is successfully installed and accessible.")
        print("   Source: mainpy.cpp (C++ extension)")
    else:
        print("\n❌ Failed to get GrapaPy version.")
        sys.exit(1)

if __name__ == "__main__":
    main() 