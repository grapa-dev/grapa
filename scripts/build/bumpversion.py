#!/usr/bin/env python3
"""
Version Bump and Deploy Script

This script automates the complete process of:
1. Updating version numbers in all required files
2. Creating a Git tag
3. Pushing to trigger CI/CD deployment

Usage: 
  python scripts/build/bumpversion.py <new_version> [--commit-and-push]
  python scripts/build/bumpversion.py --bump-version [--commit-and-push]

Examples:
  python scripts/build/bumpversion.py 0.0.116
  python scripts/build/bumpversion.py 0.0.116 --commit-and-push
  python scripts/build/bumpversion.py --bump-version --commit-and-push
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
    # Check for help flags
    if len(sys.argv) < 2 or sys.argv[1] in ['-h', '--help', '-help']:
        print("Version Bump and Deploy Script")
        print("")
        print("This script automates the complete process of:")
        print("1. Updating version numbers in all required files")
        print("2. Creating a Git tag")
        print("3. Pushing to trigger CI/CD deployment")
        print("")
        print("Usage:")
        print("  ./scripts/build/bumpversion.py <new_version> [--commit-and-push]")
        print("  ./scripts/build/bumpversion.py --bump-version [--commit-and-push]")
        print("")
        print("Arguments:")
        print("  <new_version>     Version in format X.Y.Z (e.g., 0.0.116)")
        print("  --bump-version    Automatically increment the last version number")
        print("  --commit-and-push Commit changes and push to remote repository")
        print("  -h, --help        Show this help message")
        print("")
        print("Examples:")
        print("  ./scripts/build/bumpversion.py 0.0.116")
        print("  ./scripts/build/bumpversion.py 0.0.116 --commit-and-push")
        print("  ./scripts/build/bumpversion.py --bump-version --commit-and-push")
        print("")
        print("Files Updated:")
        print("  - setup.py")
        print("  - source/mainpy.cpp")
        print("  - source/grapa/GrapaLink.h")
        print("")
        if len(sys.argv) < 2:
            sys.exit(1)
        else:
            sys.exit(0)
    
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
        ("setup.py", r'grapapy_version = "\d+\.\d+\.\d+"', f'grapapy_version = "{new_version}"'),
        ("source/mainpy.cpp", r'm\.attr\("__version__"\) = "\d+\.\d+\.\d+"', f'm.attr("__version__") = "{new_version}"'),
        ("source/grapa/GrapaLink.h", r'#define grapa_version "\d+\.\d+\.\d+"', f'#define grapa_version "{new_version}"'),
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
            # Add only the version files that were updated
            version_files = ["setup.py", "source/mainpy.cpp", "source/grapa/GrapaLink.h"]
            for file_path in version_files:
                if os.path.exists(file_path):
                    subprocess.run(["git", "add", file_path], check=True)
                    print(f"SUCCESS: Staged {file_path}")
                else:
                    print(f"WARNING: {file_path} not found - skipping")
            
            # Commit changes
            subprocess.run(["git", "commit", "-m", f"Bump version to {new_version}"], check=True)
            
            # Create tag
            subprocess.run(["git", "tag", tag_name], check=True)
            
            # Push changes and tag
            print(f"Pushing changes and tag...")
            subprocess.run(["git", "push"], check=True)
            subprocess.run(["git", "push", "origin", tag_name], check=True)
            
            print(f"SUCCESS: Successfully bumped version to {new_version}")
            print(f"SUCCESS: Created and pushed tag {tag_name}")
            print(f"INFO: Platform builds will use version {new_version}")
            print(f"INFO: Changes committed and pushed successfully")
            
        except subprocess.CalledProcessError as e:
            print(f"ERROR: Git operation failed: {e}")
            sys.exit(1)
    else:
        print(f"SUCCESS: Successfully bumped version to {new_version}")
        print(f"INFO: Files updated but not committed/pushed")
        print(f"INFO: To commit and push, run: python scripts/build/bumpversion.py {new_version} --commit-and-push")

if __name__ == "__main__":
    main() 