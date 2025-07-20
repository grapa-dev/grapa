#!/bin/bash

# Grapa Documentation Deployment Script
# This script handles building and deploying the Grapa documentation to GitHub Pages
# It addresses common deployment issues and provides a robust, automated solution

set -e  # Exit on any error

# Configuration
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DOCS_DIR="$REPO_ROOT/docs"
SITE_DIR="$DOCS_DIR/site"
MAINTAINERS_CONFIG="$DOCS_DIR/mkdocs-maintainers.yml"
USER_CONFIG="$DOCS_DIR/mkdocs.yml"
DEPLOY_BRANCH="gh-pages"
MAIN_BRANCH="main"
SITE_URL="https://grapa-dev.github.io/grapa/"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check current git branch
get_current_branch() {
    git branch --show-current
}

# Function to check if working directory is clean
is_working_directory_clean() {
    git diff-index --quiet HEAD -- && git diff-files --quiet
}

# Function to check if remote branch exists
remote_branch_exists() {
    git ls-remote --heads origin "$1" | grep -q "$1"
}

# Function to validate prerequisites
validate_prerequisites() {
    log_info "Validating prerequisites..."
    
    # Check if we're in a git repository
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        log_error "Not in a git repository. Please run this script from the repository root."
        exit 1
    fi
    
    # Check if we're on the main branch
    local current_branch=$(get_current_branch)
    if [ "$current_branch" != "$MAIN_BRANCH" ]; then
        log_error "Not on $MAIN_BRANCH branch. Current branch: $current_branch"
        log_error "Please switch to $MAIN_BRANCH branch before running this script."
        exit 1
    fi
    
    # Check if working directory is clean
    if ! is_working_directory_clean; then
        log_error "Working directory is not clean. Please commit or stash your changes."
        git status --porcelain
        exit 1
    fi
    
    # Check if Python and pip are available
    if ! command_exists python3; then
        log_error "Python 3 is not installed or not in PATH"
        exit 1
    fi
    
    # Check if mkdocs is installed
    if ! python3 -c "import mkdocs" 2>/dev/null; then
        log_error "MkDocs is not installed. Please install it with: pip install mkdocs-material"
        exit 1
    fi
    
    # Check if required directories exist
    if [ ! -d "$DOCS_DIR" ]; then
        log_error "Documentation directory not found: $DOCS_DIR"
        exit 1
    fi
    
    if [ ! -f "$USER_CONFIG" ]; then
        log_error "User MkDocs configuration not found: $USER_CONFIG"
        exit 1
    fi
    
    log_success "Prerequisites validated successfully"
}

# Function to build user documentation
build_user_docs() {
    log_info "Building user documentation..."
    
    cd "$DOCS_DIR"
    
    # Clean previous build
    if [ -d "$SITE_DIR" ]; then
        log_info "Cleaning previous build..."
        rm -rf "$SITE_DIR"
    fi
    
    # Build the site
    log_info "Running MkDocs build..."
    if python3 -m mkdocs build --clean; then
        log_success "User documentation built successfully"
    else
        log_error "Failed to build user documentation"
        exit 1
    fi
    
    # Verify build output
    if [ ! -f "$SITE_DIR/index.html" ]; then
        log_error "Build failed: index.html not found"
        exit 1
    fi
    
    log_info "Build verification completed"
    cd "$REPO_ROOT"
}

# Function to build maintainer documentation (optional)
build_maintainer_docs() {
    if [ ! -f "$MAINTAINERS_CONFIG" ]; then
        log_warning "Maintainer config not found, skipping maintainer docs build"
        return 0
    fi
    
    log_info "Building maintainer documentation..."
    
    cd "$DOCS_DIR"
    
    # Create symlink for maintainer docs if it doesn't exist
    if [ ! -L "docs/maintainers" ]; then
        log_info "Creating maintainer docs symlink..."
        ln -sf "../../maintainers" "docs/maintainers"
    fi
    
    # Build maintainer site
    local maintainer_site_dir="$DOCS_DIR/site-maintainers"
    if [ -d "$maintainer_site_dir" ]; then
        rm -rf "$maintainer_site_dir"
    fi
    
    if python3 -m mkdocs build -f "$MAINTAINERS_CONFIG" --clean -d "$maintainer_site_dir"; then
        log_success "Maintainer documentation built successfully"
        # Copy maintainer docs to main site if needed
        if [ -d "$maintainer_site_dir" ]; then
            cp -r "$maintainer_site_dir"/* "$SITE_DIR/" 2>/dev/null || true
        fi
    else
        log_warning "Failed to build maintainer documentation (continuing with user docs only)"
    fi
    
    cd "$REPO_ROOT"
}

# Function to prepare deployment branch
prepare_deployment_branch() {
    log_info "Preparing deployment branch..."
    
    # Check if remote branch exists
    if remote_branch_exists "$DEPLOY_BRANCH"; then
        log_info "Remote $DEPLOY_BRANCH branch exists, deleting..."
        git push origin --delete "$DEPLOY_BRANCH" 2>/dev/null || true
    fi
    
    # Delete local branch if it exists
    if git show-ref --verify --quiet refs/heads/"$DEPLOY_BRANCH"; then
        log_info "Local $DEPLOY_BRANCH branch exists, deleting..."
        git branch -D "$DEPLOY_BRANCH" 2>/dev/null || true
    fi
    
    # Create fresh orphan branch
    log_info "Creating fresh orphan $DEPLOY_BRANCH branch..."
    git checkout --orphan "$DEPLOY_BRANCH"
    
    # Clear the branch completely (safe on orphan branch)
    log_info "Clearing branch contents..."
    git rm -rf . 2>/dev/null || true
    
    log_success "Deployment branch prepared"
}

# Function to copy site files
copy_site_files() {
    log_info "Copying site files to deployment branch..."
    
    # Copy site files from main branch
    git checkout "$MAIN_BRANCH" -- "$SITE_DIR/"
    
    # Move files to root (where GitHub Pages expects them)
    if [ -d "$SITE_DIR" ]; then
        log_info "Moving site files to root..."
        mv "$SITE_DIR"/* . 2>/dev/null || true
        rmdir "$SITE_DIR" 2>/dev/null || true
        rmdir "$DOCS_DIR" 2>/dev/null || true
    else
        log_error "Site directory not found after checkout"
        exit 1
    fi
    
    # Verify key files exist
    if [ ! -f "index.html" ]; then
        log_error "index.html not found in deployment directory"
        exit 1
    fi
    
    log_success "Site files copied successfully"
}

# Function to commit and push
commit_and_push() {
    log_info "Committing and pushing deployment..."
    
    # Add all files
    git add .
    
    # Check if there are changes to commit
    if git diff-index --quiet HEAD --; then
        log_warning "No changes to commit"
        return 0
    fi
    
    # Commit with timestamp
    local commit_message="Deploy documentation site - $(date '+%Y-%m-%d %H:%M:%S')"
    git commit -m "$commit_message"
    
    # Push to remote
    log_info "Pushing to remote $DEPLOY_BRANCH branch..."
    if git push origin "$DEPLOY_BRANCH"; then
        log_success "Deployment pushed successfully"
    else
        log_error "Failed to push deployment"
        exit 1
    fi
}

# Function to return to main branch
return_to_main() {
    log_info "Returning to $MAIN_BRANCH branch..."
    git checkout "$MAIN_BRANCH"
    log_success "Returned to $MAIN_BRANCH branch"
}

# Function to verify deployment
verify_deployment() {
    log_info "Verifying deployment..."
    
    # Wait a moment for GitHub Pages to process
    sleep 5
    
    # Check if site is accessible
    if command_exists curl; then
        log_info "Checking site accessibility..."
        if curl -s -f "$SITE_URL" > /dev/null; then
            log_success "Site is accessible at $SITE_URL"
        else
            log_warning "Site may not be accessible yet (GitHub Pages can take 5-10 minutes to update)"
        fi
    fi
    
    log_info "Deployment verification completed"
    log_info "Note: GitHub Pages updates can take 5-10 minutes to appear"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -v, --verbose       Enable verbose output"
    echo "  -d, --dry-run       Build docs but don't deploy"
    echo "  -m, --maintainers   Include maintainer documentation"
    echo "  --no-verify         Skip deployment verification"
    echo ""
    echo "This script builds and deploys the Grapa documentation to GitHub Pages."
    echo "It handles common deployment issues and provides robust error handling."
}

# Main function
main() {
    local verbose=false
    local dry_run=false
    local include_maintainers=false
    local skip_verify=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            -d|--dry-run)
                dry_run=true
                shift
                ;;
            -m|--maintainers)
                include_maintainers=true
                shift
                ;;
            --no-verify)
                skip_verify=true
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Enable verbose output if requested
    if [ "$verbose" = true ]; then
        set -x
    fi
    
    log_info "Starting Grapa documentation deployment..."
    log_info "Repository root: $REPO_ROOT"
    log_info "Documentation directory: $DOCS_DIR"
    
    # Validate prerequisites
    validate_prerequisites
    
    # Build documentation
    build_user_docs
    
    if [ "$include_maintainers" = true ]; then
        build_maintainer_docs
    fi
    
    # Check if this is a dry run
    if [ "$dry_run" = true ]; then
        log_success "Dry run completed successfully"
        log_info "Documentation built at: $SITE_DIR"
        exit 0
    fi
    
    # Deploy to GitHub Pages
    prepare_deployment_branch
    copy_site_files
    commit_and_push
    return_to_main
    
    # Verify deployment
    if [ "$skip_verify" != true ]; then
        verify_deployment
    fi
    
    log_success "Documentation deployment completed successfully!"
    log_info "Site URL: $SITE_URL"
    log_info "Note: Changes may take 5-10 minutes to appear on GitHub Pages"
}

# Run main function with all arguments
main "$@" 