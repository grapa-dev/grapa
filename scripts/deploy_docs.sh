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
    
    # Generate deployment fingerprint for verification
    generate_deployment_fingerprint
    
    log_info "Build verification completed"
    cd "$REPO_ROOT"
}

# Function to generate deployment fingerprint
generate_deployment_fingerprint() {
    log_info "Generating deployment fingerprint..."
    
    # Create a unique fingerprint based on current deployment
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    local fingerprint="GRAPA_DEPLOYMENT_$(date +%s)_$(git rev-parse --short HEAD)"
    
    # Store fingerprint in a file that will be deployed
    echo "$fingerprint" > "$SITE_DIR/deployment-fingerprint.txt"
    echo "Deployment timestamp: $timestamp" >> "$SITE_DIR/deployment-fingerprint.txt"
    echo "Git commit: $(git rev-parse HEAD)" >> "$SITE_DIR/deployment-fingerprint.txt"
    echo "Build completed: $(date)" >> "$SITE_DIR/deployment-fingerprint.txt"
    
    # Store fingerprint for verification
    echo "$fingerprint" > "$REPO_ROOT/.deployment-fingerprint"
    
    log_info "Deployment fingerprint: $fingerprint"
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
    
    # Move files to root of gh-pages branch (where GitHub Pages expects them)
    if [ -d "$SITE_DIR" ]; then
        log_info "Moving site files to root of deployment branch..."
        mv "$SITE_DIR"/* . 2>/dev/null || true
        rmdir "$SITE_DIR" 2>/dev/null || true
        rmdir "$DOCS_DIR" 2>/dev/null || true
    else
        log_error "Site directory not found after checkout"
        exit 1
    fi
    
    # Verify key files exist
    if [ ! -f "index.html" ]; then
        log_error "index.html not found in root directory"
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
    
    # Wait for GitHub Pages to process
    log_info "Waiting for GitHub Pages to process deployment..."
    sleep 10
    
    # Check if site is accessible
    if ! command_exists curl; then
        log_warning "curl not available, skipping verification"
        return 0
    fi
    
    # Function to check if content matches expected deployment
    check_content_verification() {
        local max_attempts=12  # 2 minutes total (12 * 10 seconds)
        local attempt=1
        
        # Get expected fingerprint
        local expected_fingerprint=""
        if [ -f "$REPO_ROOT/.deployment-fingerprint" ]; then
            expected_fingerprint=$(head -n 1 "$REPO_ROOT/.deployment-fingerprint")
            log_info "Expected deployment fingerprint: $expected_fingerprint"
        else
            log_warning "No deployment fingerprint found, falling back to content verification"
        fi
        
        while [ $attempt -le $max_attempts ]; do
            log_info "Verification attempt $attempt/$max_attempts..."
            
            # First, try to verify using deployment fingerprint
            if [ -n "$expected_fingerprint" ]; then
                local fingerprint_url="$SITE_URL/deployment-fingerprint.txt"
                local fingerprint_content=$(curl -s -f "$fingerprint_url" 2>/dev/null)
                
                if [ $? -eq 0 ] && echo "$fingerprint_content" | grep -q "$expected_fingerprint"; then
                    log_success "✅ Deployment fingerprint verified!"
                    log_success "✅ Site contains the expected deployment at $SITE_URL"
                    return 0
                else
                    log_info "Deployment fingerprint not found yet, checking site accessibility..."
                fi
            fi
            
            # Fallback: Get the current site content
            local site_content=$(curl -s -f "$SITE_URL" 2>/dev/null)
            if [ $? -ne 0 ]; then
                log_warning "Site not accessible yet, waiting 10 seconds..."
                sleep 10
                attempt=$((attempt + 1))
                continue
            fi
            
            # Check for key content that should be present
            local verification_passed=true
            
            # Check for main page content
            if ! echo "$site_content" | grep -q "Grapa Documentation"; then
                log_warning "Main page content not found, waiting 10 seconds..."
                verification_passed=false
            fi
            
            # Check for navigation elements
            if ! echo "$site_content" | grep -q "Getting Started"; then
                log_warning "Navigation elements not found, waiting 10 seconds..."
                verification_passed=false
            fi
            
            # Check for search functionality
            if ! echo "$site_content" | grep -q "search"; then
                log_warning "Search functionality not found, waiting 10 seconds..."
                verification_passed=false
            fi
            
            # If all checks pass, deployment is verified
            if [ "$verification_passed" = true ]; then
                log_success "✅ Deployment verification successful!"
                log_success "✅ Site is live and contains expected content at $SITE_URL"
                return 0
            fi
            
            sleep 10
            attempt=$((attempt + 1))
        done
        
        log_error "❌ Deployment verification failed after $max_attempts attempts"
        log_error "❌ Site may not be fully updated yet"
        return 1
    }
    
    # Run content verification
    if check_content_verification; then
        log_success "Deployment verification completed successfully"
        
        # Clean up fingerprint file
        if [ -f "$REPO_ROOT/.deployment-fingerprint" ]; then
            rm "$REPO_ROOT/.deployment-fingerprint"
            log_info "Cleaned up deployment fingerprint"
        fi
    else
        log_warning "Deployment verification incomplete - site may still be updating"
        log_info "Note: GitHub Pages can take up to 10 minutes to fully update"
        log_info "You can manually check: $SITE_URL"
        
        # Keep fingerprint for manual verification
        if [ -f "$REPO_ROOT/.deployment-fingerprint" ]; then
            log_info "Deployment fingerprint preserved for manual verification: $REPO_ROOT/.deployment-fingerprint"
        fi
    fi
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