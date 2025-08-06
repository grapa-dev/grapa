#!/bin/bash
# Manual Workflow Trigger Script
#
# This script triggers the GitHub workflow manually without using the version bump script.
# This allows testing the workflow without committing local investigation files.
#
# Usage: ./scripts/ci-cd/trigger_workflow.sh

echo "🚀 Triggering GitHub workflow manually..."

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo "❌ Error: Not in a git repository"
    exit 1
fi

# Check if we have uncommitted changes
if [ -n "$(git status --porcelain)" ]; then
    echo "⚠️  Warning: You have uncommitted changes"
    echo "Current git status:"
    git status --porcelain
    echo ""
    echo "This script will only commit build.py changes, not local investigation files."
    echo "Continue? (y/N)"
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        echo "❌ Aborted"
        exit 1
    fi
fi

# Add only the build.py file (not local investigation files)
echo "📝 Adding build.py changes..."
git add build.py

# Check if there are any changes to commit
if [ -z "$(git diff --cached)" ]; then
    echo "ℹ️  No changes to build.py to commit"
else
    # Commit the build.py changes
    echo "💾 Committing build.py changes..."
    git commit -m "Fix Linux ARM64 QEMU emulation build - apply proven solution to workflow"
fi

# Create a version tag to trigger the workflow
NEW_VERSION="0.0.220"
TAG_NAME="v$NEW_VERSION"
echo "🏷️  Creating version tag: $TAG_NAME"
git tag "$TAG_NAME"

# Push the tag to trigger the workflow
echo "🚀 Pushing tag to trigger workflow..."
git push origin "$TAG_NAME"

echo "✅ Workflow triggered with tag: $TAG_NAME"
echo "📊 Check the workflow status at: https://github.com/$(git config --get remote.origin.url | sed 's/.*github.com[:/]\([^/]*\/[^/]*\).*/\1/')/actions"
echo ""
echo "🎯 This will test the Linux ARM64 QEMU emulation fixes in the GitHub workflow"
echo "📦 If successful, this will also trigger PyPI deployment" 