#!/bin/bash

# Manual PyPI Deployment Script
# This script manually triggers PyPI deployment by pushing a version tag

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_success() { echo -e "${GREEN}✅ $1${NC}"; }
print_error() { echo -e "${RED}❌ $1${NC}"; }
print_warning() { echo -e "${YELLOW}⚠️  $1${NC}"; }
print_status() { echo -e "${BLUE}📋 $1${NC}"; }

# Function to show help
show_help() {
    echo "Usage: $0 <version>"
    echo ""
    echo "Manually deploy to PyPI by pushing a version tag"
    echo ""
    echo "Examples:"
    echo "  $0 0.0.265    Deploy version 0.0.265 to PyPI"
    echo ""
    echo "This script will:"
    echo "  1. Check if the version tag exists locally"
    echo "  2. Trigger PyPI deployment workflow manually"
    echo "  3. Monitor the deployment process"
}

# Check if version is provided
if [[ $# -eq 0 ]]; then
    show_help
    exit 1
fi

version=$1

# Validate version format
if [[ ! $version =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    print_error "Invalid version format. Use X.Y.Z (e.g., 0.0.265)"
    exit 1
fi

print_status "Starting manual PyPI deployment for version $version..."

# Check if tag exists locally
if git tag -l "v$version" | grep -q "v$version"; then
    print_success "Version tag v$version exists locally"
else
    print_error "Version tag v$version does not exist locally"
    print_status "Create the tag first with: git tag v$version"
    exit 1
fi

# Check if tag exists remotely
if git ls-remote --tags origin "v$version" | grep -q "v$version"; then
    print_warning "Version tag v$version already exists remotely"
    print_status "This will trigger a new PyPI deployment"
fi

       # Trigger PyPI deployment workflow manually
       print_status "Triggering PyPI deployment workflow for version $version..."
       if gh workflow run "Deploy to PyPI" --field version="$version" --field confirm="YES"; then
           print_success "PyPI deployment workflow triggered successfully"
           print_status "Monitor the deployment at: https://github.com/grapa-dev/grapa/actions"
       else
           print_error "Failed to trigger PyPI deployment workflow"
           exit 1
       fi

print_success "Manual PyPI deployment initiated for version $version" 