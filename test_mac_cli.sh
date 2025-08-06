#!/bin/bash

echo "🧪 Testing Mac CLI validation..."

# Test macOS ARM64 CLI
echo "🔧 Testing macOS ARM64 CLI..."
verify_cli_executable "mac" "arm64"

echo ""
echo "🔧 Testing macOS AMD64 CLI..."
verify_cli_executable "mac" "amd64"

echo ""
echo "✅ Mac CLI testing completed!" 