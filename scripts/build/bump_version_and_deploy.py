#!/usr/bin/env python3
"""
Version Bump and Deploy Script

This script automates the complete process of:
1. Updating version numbers in all required files
2. Creating a Git tag
3. Pushing to trigger CI/CD deployment

Usage: 
  python scripts/bump_version_and_deploy.py <new_version> [--commit-and-push]
  python scripts/bump_version_and_deploy.py --bump-version [--commit-and-push]

Examples:
  python scripts/bump_version_and_deploy.py 0.0.116
  python scripts/bump_version_and_deploy.py 0.0.116 --commit-and-push
  python scripts/bump_version_and_deploy.py --bump-version --commit-and-push
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
    
    print(f"SUCCESS: Updated {file_path}")

def get_current_version():
    """Get current version from setup.py"""
    try:
        with open("setup.py", 'r') as f:
            content = f.read()
            match = re.search(r'grapapy_version = "([^"]+)"', content)
            if match:
                return match.group(1)
    except FileNotFoundError:
        pass
    return None

def calculate_next_version(current_version):
    """Calculate next version by incrementing last number"""
    parts = current_version.split('.')
    if len(parts) == 3:
        parts[2] = str(int(parts[2]) + 1)
        return '.'.join(parts)
    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: python scripts/bump_version_and_deploy.py <new_version|--bump-version> [--commit-and-push]")
        print("Example: python scripts/bump_version_and_deploy.py 0.0.116")
        print("Example: python scripts/bump_version_and_deploy.py 0.0.116 --commit-and-push")
        print("Example: python scripts/bump_version_and_deploy.py --bump-version --commit-and-push")
        sys.exit(1)
    
    # Check if --bump-version is requested
    if sys.argv[1] == "--bump-version":
        current_version = get_current_version()
        if not current_version:
            print("ERROR: Could not read current version from setup.py")
            sys.exit(1)
        
        new_version = calculate_next_version(current_version)
        if not new_version:
            print("ERROR: Could not calculate next version")
            sys.exit(1)
        
        print(f"INFO: Current version: {current_version}")
        print(f"INFO: Bumping to version: {new_version}")
        
        # Remove --bump-version from argv and replace with calculated version
        sys.argv[1] = new_version
    else:
        new_version = sys.argv[1]
    
    commit_and_push = len(sys.argv) == 3 and sys.argv[2] == "--commit-and-push"
    
    # Validate version format
    if not re.match(r'^\d+\.\d+\.\d+$', new_version):
        print("ERROR: Version must be in format X.Y.Z (e.g., 0.0.116)")
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
            print(f"WARNING: {file_path} not found")
    
    # Create Git tag
    tag_name = f"v{new_version}"
    
    if commit_and_push:
        print(f"🏷️  Creating Git tag and pushing: {tag_name}")
        
        try:
            # Add all changes
            subprocess.run(["git", "add", "."], check=True)
            
            # Commit changes
            subprocess.run(["git", "commit", "-m", f"Bump version to {new_version}"], check=True)
            
            # Create tag
            subprocess.run(["git", "tag", tag_name], check=True)
            
            # Push changes only (no tag push to avoid triggering PyPI deployment)
            subprocess.run(["git", "push"], check=True)
            
            print(f"SUCCESS: Successfully bumped version to {new_version}")
            print(f"SUCCESS: Created tag {tag_name} (not pushed)")
            print(f"INFO: Platform builds will use version {new_version}")
            print(f"INFO: PyPI deployment should be triggered manually after platform builds complete")
            print(f"INFO: Note: GitHub Pages workflow may trigger (this is expected)")
            
        except subprocess.CalledProcessError as e:
            print(f"ERROR: Git operation failed: {e}")
            sys.exit(1)
    else:
        print(f"SUCCESS: Successfully bumped version to {new_version}")
        print(f"INFO: Files updated but not committed/pushed")
        print(f"INFO: To commit and push, run: python scripts/bump_version_and_deploy.py {new_version} --commit-and-push")

if __name__ == "__main__":
    main() 