#!/bin/bash

# Grapa Documentation Build Script (User Docs Only)
# Builds documentation from /docs-src to /docs, with full validation.
# No branch switching or deployment; just build and validate.

set -e  # Exit on any error

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC_DIR="$REPO_ROOT/docs-src"
USER_CONFIG="$SRC_DIR/mkdocs.yml"
SITE_DIR="$SRC_DIR/site"
DEST_DIR="$REPO_ROOT/docs"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

command_exists() { command -v "$1" >/dev/null 2>&1; }

validate_prerequisites() {
    log_info "Validating prerequisites..."
    if ! command_exists python3; then
        log_error "Python 3 is not installed or not in PATH"; exit 1; fi
    if ! python3 -c "import mkdocs" 2>/dev/null; then
        log_error "MkDocs is not installed. Please install it with: pip install mkdocs-material"; exit 1; fi
    if [ ! -d "$SRC_DIR" ]; then
        log_error "Source directory not found: $SRC_DIR"; exit 1; fi
    if [ ! -f "$USER_CONFIG" ]; then
        log_error "User MkDocs configuration not found: $USER_CONFIG"; exit 1; fi
    log_success "Prerequisites validated successfully"
}

build_user_docs() {
    log_info "Building user documentation..."
    cd "$SRC_DIR"
    if [ -d "$SITE_DIR" ]; then
        log_info "Cleaning previous build..."
        rm -rf "$SITE_DIR"
    fi
    log_info "Running MkDocs build..."
    if python3 -m mkdocs build --clean; then
        log_success "User documentation built successfully"
    else
        log_error "Failed to build user documentation"; exit 1; fi
    if [ ! -f "$SITE_DIR/index.html" ]; then
        log_error "Build failed: index.html not found"; exit 1; fi
    validate_case_sensitivity
    log_info "Build verification completed"
    cd "$REPO_ROOT"
}

validate_case_sensitivity() {
    log_info "Validating case sensitivity - checking for uppercase files/folders..."
    local uppercase_items=()
    while IFS= read -r -d '' item; do
        local basename=$(basename "$item")
        if [[ "$basename" =~ [A-Z] ]]; then
            uppercase_items+=("$item")
        fi
    done < <(find "$SITE_DIR" -mindepth 1 -maxdepth 1 -print0)
    if [ ${#uppercase_items[@]} -gt 0 ]; then
        log_error "Found uppercase files/folders in build output:"
        for item in "${uppercase_items[@]}"; do
            local relative_path="${item#$SITE_DIR/}"
            log_error "  - $relative_path"
        done
        log_error "All files and folders must use lowercase naming for case-sensitive compatibility."
        log_error "Please fix the navigation configuration in mkdocs.yml to use lowercase keys."
        exit 1
    fi
    log_success "Case sensitivity validation passed - all files/folders use lowercase naming"
}

copy_site_files() {
    log_info "Copying site files to $DEST_DIR..."
    if [ -d "$DEST_DIR" ]; then
        rm -rf "$DEST_DIR"
    fi
    mkdir -p "$DEST_DIR"
    cp -r "$SITE_DIR"/* "$DEST_DIR"/
    log_success "Site files copied to $DEST_DIR"
}

commit_and_push_docs() {
    log_info "Staging and committing /docs and /docs-src/site deletions..."
    git add docs/
    git add -u docs-src/site
    if git diff --cached --quiet; then
        log_info "No changes to commit."
    else
        git commit -m "docs: Update user documentation and clean up build artifacts"
        log_success "Documentation changes committed."
    fi
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Builds user documentation from /docs-src to /docs with validation."
    echo ""
    echo "OPTIONS:"
    echo "  --push        Push changes to main branch (triggers GitHub Pages deployment)"
    echo "  -h, --help    Show this help message"
    echo ""
    echo "EXAMPLES:"
    echo "  $0              Build and commit documentation (no push)"
    echo "  $0 --push       Build, commit, and push to trigger GitHub Pages deployment"
    echo ""
    echo "Note: Use --push only when you want to deploy documentation to GitHub Pages."
}

main() {
    PUSH_CHANGES=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --push)
                PUSH_CHANGES=true
                shift
                ;;
            -h|--help)
                show_usage
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    log_info "Starting Grapa documentation build (user docs only)..."
    log_info "Source: $SRC_DIR"
    log_info "Destination: $DEST_DIR"
    validate_prerequisites
    build_user_docs
    copy_site_files
    commit_and_push_docs
    
    if [[ "$PUSH_CHANGES" == "true" ]]; then
        log_info "Pushing changes to main branch..."
        git push origin main
        log_success "Changes pushed to main. GitHub Pages deployment triggered."
    else
        log_info "Changes committed locally. Run 'git push origin main' to deploy to GitHub Pages."
    fi
    
    log_success "Documentation build completed!"
}

main "$@" 