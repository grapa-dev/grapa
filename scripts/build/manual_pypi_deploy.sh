#!/bin/bash

# Manual PyPI Deployment Script
# This script manually triggers PyPI deployment using the current version from setup.py

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
    echo "Usage: $0"
    echo ""
    echo "Manually deploy to PyPI using the current version from setup.py"
    echo ""
    echo "This script will:"
    echo "  1. Trigger PyPI deployment workflow manually"
    echo "  2. The workflow will auto-detect the version from setup.py"
    echo ""
    echo "Prerequisites:"
    echo "  - build_all_platforms.sh must have completed successfully"
    echo "  - Version must be set in setup.py"
    echo "  - GitHub CLI (gh) must be installed and authenticated"
}

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    print_error "Not in a git repository"
    exit 1
fi

print_status "Starting manual PyPI deployment..."

# Check prerequisites
print_status "Checking prerequisites..."

# Check if GitHub CLI is installed
if ! command -v gh >/dev/null 2>&1; then
    print_error "GitHub CLI (gh) is not installed. Please install it first."
    exit 1
fi

# Check if we're authenticated with GitHub
if ! gh auth status >/dev/null 2>&1; then
    print_error "Not authenticated with GitHub. Please run 'gh auth login' first."
    exit 1
fi

# Trigger PyPI deployment workflow manually
print_status "Triggering PyPI deployment workflow..."
if gh workflow run "Deploy to PyPI" --field confirm="YES"; then
    print_success "PyPI deployment workflow triggered successfully"
    print_status "The workflow will auto-detect the version from setup.py"
    print_status "Monitor the deployment at: https://github.com/grapa-dev/grapa/actions"
else
    print_error "Failed to trigger PyPI deployment workflow"
    exit 1
fi

print_success "Manual PyPI deployment initiated" 