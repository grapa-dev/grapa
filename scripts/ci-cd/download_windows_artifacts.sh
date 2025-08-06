#!/bin/bash
# Download Windows Build Artifacts from GitHub Actions
# This script downloads the Windows build artifacts after the workflow completes

echo "🪟 Downloading Windows build artifacts from GitHub Actions..."
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

# Download artifacts
echo "📥 Downloading artifacts..."
gh run download "$RUN_ID" --name="grapa-windows-amd64" --dir="."

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Artifacts downloaded successfully!"
    echo ""
    echo "📁 Downloaded files:"
    ls -la grapa.exe 2>/dev/null || echo "  ❌ grapa.exe (missing)"
    ls -la source/grapa-lib/win-amd64/ 2>/dev/null || echo "  ❌ source/grapa-lib/win-amd64/ (missing)"
    ls -la source/grapa-other/win-amd64/ 2>/dev/null || echo "  ❌ source/grapa-other/win-amd64/ (missing)"
    ls -la bin/grapa-win-amd64.tar.gz 2>/dev/null || echo "  ❌ bin/grapa-win-amd64.tar.gz (missing)"
    echo ""
    echo "🎉 Windows build artifacts are now available!"
    echo "   You can now run: ./scripts/validation/check_platform_status.sh"
else
    echo "❌ Failed to download artifacts"
    echo "   Please check the workflow run page for manual download"
fi 