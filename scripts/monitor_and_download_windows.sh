#!/bin/bash
# Monitor Windows Workflow and Download Artifacts
# This script monitors the Windows GitHub Actions workflow and downloads artifacts when it completes

echo "🪟 Monitoring Windows GitHub Actions workflow..."
echo ""

# Check if we have the GitHub CLI installed
if ! command -v gh &> /dev/null; then
    echo "❌ GitHub CLI (gh) is not installed"
    echo "   Please install it from: https://cli.github.com/"
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

# Function to get the latest workflow run
get_latest_run() {
    gh run list --workflow="Build Windows AMD64" --limit=1 --json databaseId,status,conclusion --jq '.[0]'
}

# Function to download artifacts
download_artifacts() {
    local run_id=$1
    echo ""
    echo "📥 Downloading artifacts for run $run_id..."
    
    # Create temporary directory for download
    TEMP_DIR=$(mktemp -d)
    echo "📁 Using temporary directory: $TEMP_DIR"
    
    # Download artifacts to temporary directory
    gh run download "$run_id" --name="grapa-windows-amd64" --dir="$TEMP_DIR"
    
    if [[ $? -eq 0 ]]; then
        echo ""
        echo "✅ Artifacts downloaded successfully!"
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
        elif [[ -f "$TEMP_DIR/bin/grapa-win-amd64.zip" ]]; then
            cp -f "$TEMP_DIR/bin/grapa-win-amd64.zip" bin/
            echo "  ✅ Copied compressed package (zip)"
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
        ls -la bin/grapa-win-amd64.* 2>/dev/null || echo "  ❌ bin/grapa-win-amd64.* (missing)"
        
        return 0
    else
        echo "❌ Failed to download artifacts"
        return 1
    fi
}

# Get the latest run
echo "🔍 Finding latest Windows build workflow run..."
LATEST_RUN=$(get_latest_run)

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

# If already completed successfully, download immediately
if [[ "$STATUS" == "completed" && "$CONCLUSION" == "success" ]]; then
    echo "✅ Workflow already completed successfully!"
    download_artifacts "$RUN_ID"
    exit $?
fi

# If failed or cancelled, exit
if [[ "$STATUS" == "completed" && "$CONCLUSION" != "success" ]]; then
    echo "❌ Workflow failed or was cancelled"
    echo "   Please check the workflow run for errors"
    exit 1
fi

# If still running, monitor it
if [[ "$STATUS" != "completed" ]]; then
    echo "⏳ Workflow is still running... monitoring progress"
    echo "   You can view the live logs at: https://github.com/grapa-dev/grapa/actions/runs/$RUN_ID"
    echo ""
    echo "🔄 Monitoring for completion (checking every 30 seconds)..."
    echo "   Press Ctrl+C to stop monitoring"
    echo ""
    
    # Monitor the workflow
    while true; do
        sleep 30
        
        # Get updated status
        LATEST_RUN=$(get_latest_run)
        STATUS=$(echo "$LATEST_RUN" | jq -r '.status')
        CONCLUSION=$(echo "$LATEST_RUN" | jq -r '.conclusion')
        
        echo "$(date '+%H:%M:%S') - Status: $STATUS, Conclusion: $CONCLUSION"
        
        if [[ "$STATUS" == "completed" ]]; then
            if [[ "$CONCLUSION" == "success" ]]; then
                echo ""
                echo "✅ Workflow completed successfully!"
                download_artifacts "$RUN_ID"
                exit $?
            else
                echo ""
                echo "❌ Workflow failed or was cancelled"
                echo "   Please check the workflow run for errors"
                exit 1
            fi
        fi
    done
fi 