#!/bin/bash
# Download Windows Build Artifacts from GitHub Actions (Force Overwrite)
# This script downloads the Windows build artifacts after the workflow completes

echo "🪟 Downloading Windows build artifacts from GitHub Actions (force overwrite)..."
echo ""

# Check if we have the GitHub CLI installed
if ! command -v gh &> /dev/null; then
    echo "❌ GitHub CLI (gh) is not installed"
    echo "   Please install it from: https://cli.github.com/"
    echo ""
    echo "📋 MANUAL DOWNLOAD INSTRUCTIONS:"
    echo "   1. Go to: https://github.com/grapa-dev/grapa/actions"
    echo "   2. Find the latest 'Build Windows AMD64' workflow run"
    echo "   3. Click on the workflow run"
    echo "   4. Scroll down to 'Artifacts' section"
    echo "   5. Download 'grapa-windows-amd64' artifact"
    echo "   6. Extract the files to the project root"
    echo ""
    exit 1
fi

# Check if we're authenticated
if ! gh auth status &> /dev/null; then
    echo "❌ Not authenticated with GitHub CLI"
    echo "   Please run: gh auth login"
    exit 1
fi

echo "✅ GitHub CLI is available and authenticated"
echo ""

# Get the latest workflow run
echo "🔍 Finding latest Windows build workflow run..."
LATEST_RUN=$(gh run list --workflow="Build Windows AMD64" --limit=1 --json databaseId,status,conclusion --jq '.[0]')

if [[ "$LATEST_RUN" == "null" ]]; then
    echo "❌ No Windows build workflow runs found"
    echo "   Please trigger the workflow first:"
    echo "   1. Go to: https://github.com/grapa-dev/grapa/actions"
    echo "   2. Click 'Build Windows AMD64'"
    echo "   3. Click 'Run workflow'"
    exit 1
fi

RUN_ID=$(echo "$LATEST_RUN" | jq -r '.databaseId')
STATUS=$(echo "$LATEST_RUN" | jq -r '.status')
CONCLUSION=$(echo "$LATEST_RUN" | jq -r '.conclusion')

echo "📋 Latest run ID: $RUN_ID"
echo "📋 Status: $STATUS"
echo "📋 Conclusion: $CONCLUSION"
echo ""

if [[ "$STATUS" != "completed" ]]; then
    echo "⏳ Workflow is still running..."
    echo "   Please wait for it to complete and try again"
    exit 1
fi

if [[ "$CONCLUSION" != "success" ]]; then
    echo "❌ Workflow failed or was cancelled"
    echo "   Please check the workflow run for errors"
    exit 1
fi

echo "✅ Workflow completed successfully!"
echo ""

# Create temporary directory for download
TEMP_DIR=$(mktemp -d)
echo "📥 Downloading artifacts to temporary directory: $TEMP_DIR"

# Download artifacts to temporary directory
gh run download "$RUN_ID" --name="grapa-windows-amd64" --dir="$TEMP_DIR"

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Artifacts downloaded successfully to temporary directory!"
    echo ""
    
    # Create necessary directories
    mkdir -p source/grapa-lib/win-amd64
    mkdir -p source/grapa-other/win-amd64
    mkdir -p bin
    
    # Copy files with force overwrite
    echo "📁 Copying files with force overwrite..."
    
    # Copy executable
    if [[ -f "$TEMP_DIR/grapa.exe" ]]; then
        cp -f "$TEMP_DIR/grapa.exe" .
        echo "  ✅ Copied grapa.exe"
    else
        echo "  ❌ grapa.exe not found in artifacts"
    fi
    
    # Copy static libraries
    if [[ -d "$TEMP_DIR/source/grapa-lib/win-amd64" ]]; then
        cp -rf "$TEMP_DIR/source/grapa-lib/win-amd64/"* source/grapa-lib/win-amd64/
        echo "  ✅ Copied static libraries"
    else
        echo "  ❌ Static libraries not found in artifacts"
    fi
    
    # Copy shared libraries
    if [[ -d "$TEMP_DIR/source/grapa-other/win-amd64" ]]; then
        cp -rf "$TEMP_DIR/source/grapa-other/win-amd64/"* source/grapa-other/win-amd64/
        echo "  ✅ Copied shared libraries"
    else
        echo "  ❌ Shared libraries not found in artifacts"
    fi
    
    # Copy compressed package
    if [[ -f "$TEMP_DIR/bin/grapa-win-amd64.tar.gz" ]]; then
        cp -f "$TEMP_DIR/bin/grapa-win-amd64.tar.gz" bin/
        echo "  ✅ Copied compressed package"
    else
        echo "  ❌ Compressed package not found in artifacts"
    fi
    
    # Clean up temporary directory
    rm -rf "$TEMP_DIR"
    echo ""
    echo "🎉 Windows build artifacts are now available!"
    echo "   You can now run: ./scripts/check_platform_status.sh"
    
    # Show what was downloaded
    echo ""
    echo "📁 Downloaded files:"
    ls -la grapa.exe 2>/dev/null || echo "  ❌ grapa.exe (missing)"
    ls -la source/grapa-lib/win-amd64/ 2>/dev/null || echo "  ❌ source/grapa-lib/win-amd64/ (missing)"
    ls -la source/grapa-other/win-amd64/ 2>/dev/null || echo "  ❌ source/grapa-other/win-amd64/ (missing)"
    ls -la bin/grapa-win-amd64.tar.gz 2>/dev/null || echo "  ❌ bin/grapa-win-amd64.tar.gz (missing)"
    
else
    echo "❌ Failed to download artifacts"
    echo "   Please check the workflow run page for manual download"
fi 