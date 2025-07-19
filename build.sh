#!/bin/bash
# Grapa Build Script Wrapper
# This is a simple wrapper around the Python build script

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Change to the project root directory
cd "$SCRIPT_DIR"

# Check if Python 3 is available
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is required but not installed."
    exit 1
fi

# Run the Python build script with all arguments
python3 build.py "$@" 