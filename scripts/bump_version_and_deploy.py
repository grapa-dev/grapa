#!/usr/bin/env python3
"""
Version Bump and Deploy Script

This script automates the complete process of:
1. Updating version numbers in all required files
2. Creating a Git tag
3. Pushing to trigger CI/CD deployment

Usage: python scripts/bump_version_and_deploy.py [new_version]
Example: python scripts/bump_version_and_deploy.py 0.0.116
"""

import sys
import re
import subprocess
import os
from pathlib import Path

def update_version_in_file(file_path, old_version_pattern, new_version):
    """Update version in a specific file"""
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Replace version pattern
    new_content = re.sub(old_version_pattern, new_version, content)
    
    with open(file_path, 'w') as f:
        f.write(new_content)
    
    print(f"✅ Updated {file_path}")

def main():
    if len(sys.argv) != 2:
        print("Usage: python scripts/bump_version_and_deploy.py <new_version>")
        print("Example: python scripts/bump_version_and_deploy.py 0.0.116")
        sys.exit(1)
    
    new_version = sys.argv[1]
    
    # Validate version format
    if not re.match(r'^\d+\.\d+\.\d+$', new_version):
        print("❌ Error: Version must be in format X.Y.Z (e.g., 0.0.116)")
        sys.exit(1)
    
    print(f"🚀 Starting version bump to {new_version}...")
    
    # Files to update with their version patterns
    files_to_update = [
        ("setup.py", r'grapapy_version = "0\.0\.\d+"', f'grapapy_version = "{new_version}"'),
        ("source/mainpy.cpp", r'm\.attr\("__version__"\) = "0\.0\.\d+"', f'm.attr("__version__") = "{new_version}"'),
        ("source/grapa/GrapaLink.h", r'#define grapa_version "0\.0\.\d+"', f'#define grapa_version "{new_version}"'),
    ]
    
    # Update all version files
    for file_path, pattern, replacement in files_to_update:
        if os.path.exists(file_path):
            update_version_in_file(file_path, pattern, replacement)
        else:
            print(f"❌ Warning: {file_path} not found")
    
    # Create Git tag
    tag_name = f"v{new_version}"
    print(f"🏷️  Creating Git tag: {tag_name}")
    
    try:
        # Add all changes
        subprocess.run(["git", "add", "."], check=True)
        
        # Commit changes
        subprocess.run(["git", "commit", "-m", f"Bump version to {new_version}"], check=True)
        
        # Create tag
        subprocess.run(["git", "tag", tag_name], check=True)
        
        # Push changes and tag
        subprocess.run(["git", "push"], check=True)
        subprocess.run(["git", "push", "origin", tag_name], check=True)
        
        print(f"✅ Successfully bumped version to {new_version}")
        print(f"✅ Created and pushed tag {tag_name}")
        print(f"🚀 CI/CD workflow should now trigger automatically")
        print(f"📦 PyPI deployment will begin shortly...")
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Git operation failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main() 