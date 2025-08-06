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

# Function to wait for the specific workflow run that was just triggered
wait_for_specific_run() {
    local max_wait_time=600  # 10 minutes
    local wait_interval=10   # Check every 10 seconds
    local elapsed=0
    
    while [[ $elapsed -lt $max_wait_time ]]; do
        # Get the latest runs
        local runs=$(gh run list --workflow="Build Windows AMD64" --limit=3 --json databaseId,status,conclusion,headSha)
        
        # Get the most recent run that's completed
        local target_run=$(echo "$runs" | jq -r '.[] | select(.status == "completed" and .conclusion == "success") | .databaseId' | head -1)
        
        if [[ -n "$target_run" ]]; then
            echo "$target_run"
            return 0
        fi
        
        sleep $wait_interval
        elapsed=$((elapsed + wait_interval))
    done
    
    return 1
}

# Function to download artifacts
download_artifacts() {
    local run_id=$1
    echo ""
    echo "📥 Downloading artifacts for run $run_id..."
    
    # Create temporary directory for download
    TEMP_DIR=$(mktemp -d)
    echo "📁 Using temporary directory: $TEMP_DIR"
    
    # Download artifacts to temporary directory with timeout
    echo "⏳ Downloading artifacts (timeout: 5 minutes)..."
    if command -v timeout >/dev/null 2>&1; then
        timeout 300 gh run download "$run_id" --name="grapa-windows-amd64" --dir="$TEMP_DIR"
    else
        # On macOS, run without timeout (should be fast enough)
        gh run download "$run_id" --name="grapa-windows-amd64" --dir="$TEMP_DIR"
    fi
    
    local download_exit_code=$?
    if [[ $download_exit_code -eq 0 ]]; then
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
        echo "   You can now run: ./scripts/validation/check_platform_status.sh"
        
        # Show what was downloaded
        echo ""
        echo "📁 Downloaded files:"
        ls -la grapa.exe 2>/dev/null || echo "  ❌ grapa.exe (missing)"
        
        return 0
    elif [[ $download_exit_code -eq 124 ]]; then
        echo ""
        echo "❌ Download timed out after 5 minutes"
        echo "   The download may be taking longer than expected."
        echo "   You can try running this script again later."
        rm -rf "$TEMP_DIR"
        return 1
    else
        echo ""
        echo "❌ Download failed with exit code $download_exit_code"
        echo "   Please check your network connection and GitHub CLI permissions."
        rm -rf "$TEMP_DIR"
        return 1
    fi
}

# Get the most recent completed workflow run
echo "🔍 Finding most recent completed Windows build workflow run..."
TARGET_RUN_ID=$(wait_for_specific_run)

if [[ -z "$TARGET_RUN_ID" ]]; then
    echo "❌ Could not find completed workflow run"
    echo "   Please check if the workflow was triggered correctly"
    exit 1
fi

# Get the status of the target run
TARGET_RUN=$(gh run view "$TARGET_RUN_ID" --json databaseId,status,conclusion)
RUN_ID=$(echo "$TARGET_RUN" | jq -r '.databaseId')
STATUS=$(echo "$TARGET_RUN" | jq -r '.status')
CONCLUSION=$(echo "$TARGET_RUN" | jq -r '.conclusion')

echo "📋 Target run ID: $RUN_ID"
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