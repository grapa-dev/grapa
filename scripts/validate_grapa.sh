#!/bin/bash
# Grapa Syntax Validator Wrapper Script

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Change to project root
cd "$PROJECT_ROOT"

# Check if Python 3 is available
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is required but not installed"
    exit 1
fi

# Run the validator with all arguments passed through
python3 "$SCRIPT_DIR/validate_grapa_syntax.py" "$@"

# Exit with the same code as the Python script
exit $? 