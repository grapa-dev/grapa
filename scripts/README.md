# Grapa Documentation Deployment

This directory contains scripts and workflows for building and deploying the Grapa documentation to GitHub Pages.

## Quick Start

### Automated Deployment (Recommended)

The documentation is automatically deployed when changes are pushed to the `main` branch. The GitHub Actions workflow handles everything automatically.

**Trigger conditions:**
- Push to `main` branch
- Changes to files in `docs/` or `maintainers/` directories
- Manual trigger via GitHub Actions UI

### Manual Deployment

For manual deployment or testing, use the deployment script:

```bash
# Basic deployment
./scripts/deploy_docs.sh

# Dry run (build only, no deployment)
./scripts/deploy_docs.sh --dry-run

# Include maintainer documentation
./scripts/deploy_docs.sh --maintainers

# Verbose output
./scripts/deploy_docs.sh --verbose

# Skip verification
./scripts/deploy_docs.sh --no-verify
```

## Files

### `deploy_docs.sh`
The main deployment script that handles:
- Prerequisite validation
- Documentation building
- Branch management
- Deployment to GitHub Pages
- Error handling and recovery

### `.github/workflows/deploy-docs.yml`
GitHub Actions workflow for automated deployment.

## Prerequisites

### For Local Deployment
- Git repository with access to push to `gh-pages` branch
- Python 3.7+ with pip
- MkDocs Material theme: `pip install mkdocs-material`
- Pymdown extensions: `pip install pymdown-extensions`

### For GitHub Actions
- Repository with GitHub Pages enabled
- Proper permissions for the workflow

## Deployment Process

### 1. Validation
- Checks if running from git repository
- Verifies current branch is `main`
- Ensures working directory is clean
- Validates Python and MkDocs installation
- Confirms required directories exist

### 2. Building
- Cleans previous build artifacts
- Builds user documentation using `mkdocs.yml`
- Optionally builds maintainer documentation using `mkdocs-maintainers.yml`
- Verifies build output (index.html exists)

### 3. Deployment
- Deletes existing `gh-pages` branch (local and remote)
- Creates fresh orphan `gh-pages` branch
- Copies built site files to branch root
- Commits and pushes to remote
- Returns to `main` branch

### 4. Verification
- **Fingerprint-based verification**: Creates unique deployment fingerprint and verifies it's live
- **Content verification**: Checks for expected content elements
- **Retry logic**: Up to 12 attempts over 2 minutes to verify deployment
- **Fallback verification**: Multiple verification methods ensure reliability

## Common Issues and Solutions

### Issue: "Not in a git repository"
**Solution:** Run the script from the repository root directory.

### Issue: "Not on main branch"
**Solution:** Switch to the main branch before running the script:
```bash
git checkout main
./scripts/deploy_docs.sh
```

### Issue: "Working directory is not clean"
**Solution:** Commit or stash your changes:
```bash
git add .
git commit -m "Your commit message"
# OR
git stash
./scripts/deploy_docs.sh
git stash pop  # if you stashed
```

### Issue: "MkDocs is not installed"
**Solution:** Install MkDocs and required extensions:
```bash
pip install mkdocs-material pymdown-extensions
```

### Issue: Build fails
**Solution:** Check for syntax errors in markdown files or MkDocs configuration:
```bash
cd docs
python -m mkdocs build --verbose
```

### Issue: Deployment fails
**Solution:** Check git permissions and remote access:
```bash
git remote -v
git push origin main  # Test push access
```

### Issue: Verification fails
**Solution:** The script will retry automatically, but you can:
```bash
# Check if fingerprint file exists for manual verification
cat .deployment-fingerprint

# Manually check the live site
curl -s https://grapa-dev.github.io/grapa/deployment-fingerprint.txt
```

## Troubleshooting

### Manual Recovery from Failed Deployment

If the deployment script fails and leaves you in an inconsistent state:

```bash
# 1. Return to main branch
git checkout main

# 2. Clean up any leftover files
git clean -fdx

# 3. Reset to last good state
git reset --hard HEAD

# 4. Try deployment again
./scripts/deploy_docs.sh
```

### Checking Deployment Status

```bash
# Check if gh-pages branch exists
git branch -a | grep gh-pages

# Check remote branches
git ls-remote --heads origin

# Check site accessibility
curl -I https://grapa-dev.github.io/grapa/
```

### Debugging Build Issues

```bash
# Build with verbose output
cd docs
python -m mkdocs build --verbose

# Check for broken links
python -m mkdocs build --strict

# Validate configuration
python -m mkdocs config
```

## Best Practices

### Before Deployment
1. **Test locally first:**
   ```bash
   ./scripts/deploy_docs.sh --dry-run
   ```

2. **Review changes:**
   ```bash
   git diff HEAD~1 docs/
   ```

3. **Check for broken links:**
   ```bash
   cd docs
   python -m mkdocs build --strict
   ```

### During Development
1. **Use dry runs for testing:**
   ```bash
   ./scripts/deploy_docs.sh --dry-run --verbose
   ```

2. **Keep working directory clean:**
   ```bash
   git status
   git add .
   git commit -m "Your changes"
   ```

3. **Test maintainer docs separately:**
   ```bash
   ./scripts/deploy_docs.sh --maintainers --dry-run
   ```

### After Deployment
1. **Verify the deployment:**
   - Check the live site: https://grapa-dev.github.io/grapa/
   - Test navigation links
   - Verify search functionality

2. **Monitor for issues:**
   - Check GitHub Actions logs
   - Monitor for 404 errors
   - Verify all examples work

## Configuration

### MkDocs Configuration
- **User docs:** `docs/mkdocs.yml`
- **Maintainer docs:** `docs/mkdocs-maintainers.yml`

### Script Configuration
Key variables in `deploy_docs.sh`:
- `DEPLOY_BRANCH`: Branch for deployment (default: `gh-pages`)
- `MAIN_BRANCH`: Source branch (default: `main`)
- `SITE_URL`: Live site URL

### GitHub Pages Configuration
- **Source:** Deploy from a branch
- **Branch:** `gh-pages`
- **Folder:** `/ (root)`

## Security Considerations

### GitHub Actions
- Uses minimal required permissions
- Runs in isolated environment
- Uses GitHub's built-in security features

### Local Deployment
- Requires push access to repository
- Validates prerequisites before execution
- Includes error handling and recovery

## Support

### Getting Help
1. Check this README for common issues
2. Review the deployment guide: `maintainers/DEVELOPMENT/DOCUMENTATION_DEPLOYMENT_GUIDE.md`
3. Check GitHub Actions logs for detailed error messages
4. Use the `--verbose` flag for detailed output

### Contributing
When modifying the deployment system:
1. Test changes locally first
2. Use dry runs to validate
3. Update this README if needed
4. Consider backward compatibility

## Changelog

### Version 1.0.0 (Current)
- Initial deployment script with comprehensive error handling
- GitHub Actions workflow for automated deployment
- Support for both user and maintainer documentation
- Robust branch management and cleanup
- Comprehensive validation and verification 