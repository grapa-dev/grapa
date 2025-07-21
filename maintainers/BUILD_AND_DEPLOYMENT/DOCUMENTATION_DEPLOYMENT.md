# Documentation Deployment System

## Overview

The Grapa documentation deployment system has been completely automated to eliminate the problematic manual deployment process. This system provides both automated GitHub Actions deployment and a robust local deployment script.

## Quick Start

### Automated Deployment (Recommended)

The documentation is automatically deployed when changes are pushed to the `main` branch. No manual intervention required.

**What triggers deployment:**
- Push to `main` branch
- Changes to files in `docs/` or `maintainers/` directories
- Manual trigger via GitHub Actions UI

### Manual Deployment

For testing or manual deployment:

```bash
# Basic deployment
./scripts/deploy_docs.sh

# Dry run (build only, no deployment)
./scripts/deploy_docs.sh --dry-run

# Include maintainer documentation
./scripts/deploy_docs.sh --maintainers

# Verbose output for debugging
./scripts/deploy_docs.sh --verbose
```

## System Components

### 1. Deployment Script (`scripts/deploy_docs.sh`)

A comprehensive bash script that handles:
- **Prerequisite validation**: Git repository, branch checks, Python/MkDocs installation
- **Documentation building**: User and maintainer docs with proper symlinks
- **Branch management**: Safe orphan branch creation and cleanup
- **Deployment**: File copying, committing, and pushing
- **Error handling**: Comprehensive error checking and recovery
- **Verification**: Site accessibility testing

### 2. GitHub Actions Workflow (`.github/workflows/deploy-docs.yml`)

Automated deployment pipeline that:
- Triggers on documentation changes
- Runs in isolated environment
- Handles all deployment steps automatically
- Provides detailed logging
- Includes PR comments for documentation updates

### 3. Documentation (`scripts/README.md`)

Comprehensive guide covering:
- Usage instructions
- Troubleshooting
- Best practices
- Configuration options

## Key Features

### Robust Error Handling
- Validates all prerequisites before starting
- Checks for clean working directory
- Verifies build output
- Handles git branch conflicts safely
- Provides clear error messages

### Safe Branch Management
- Creates fresh orphan `gh-pages` branch
- Deletes old branches to prevent conflicts
- Safely clears branch contents
- Returns to main branch after deployment

### Comprehensive Validation
- Git repository and branch checks
- Python and MkDocs installation verification
- Build output validation
- **Case sensitivity validation** - Ensures all files/folders use lowercase naming
- **Deployment fingerprint generation and verification**
- **Content-based verification with retry logic**
- **Site accessibility testing**

### Flexible Options
- Dry run mode for testing
- Maintainer documentation inclusion
- Verbose output for debugging
- Skip verification option

## Deployment Process

### 1. Validation Phase
```bash
# Check prerequisites
- Git repository exists
- On main branch
- Working directory clean
- Python/MkDocs installed
- Required directories exist
```

### 2. Building Phase
```bash
# Build documentation
- Clean previous build
- Build user docs (mkdocs.yml)
- Optionally build maintainer docs (mkdocs-maintainers.yml)
- Verify build output (index.html exists)
- Validate case sensitivity (all files/folders must be lowercase)
```

### 3. Deployment Phase
```bash
# Deploy to GitHub Pages
- Delete existing gh-pages branch
- Create fresh orphan branch
- Copy site files to branch root
- Commit and push changes
- Return to main branch
```

### 4. Verification Phase
```bash
# Verify deployment
- Generate unique deployment fingerprint
- Check site accessibility
- Verify fingerprint is live on site
- Fallback to content verification
- Retry up to 12 times over 2 minutes
- Provide detailed deployment status
```

## Troubleshooting

### Common Issues

#### "Working directory is not clean"
**Solution**: Commit or stash your changes
```bash
git add .
git commit -m "Your changes"
# OR
git stash
./scripts/deploy_docs.sh
git stash pop
```

#### "MkDocs is not installed"
**Solution**: Install required packages
```bash
pip install mkdocs-material pymdown-extensions
```

#### Build fails
**Solution**: Check for syntax errors
```bash
cd docs
python -m mkdocs build --verbose
```

#### Case sensitivity validation fails
**Solution**: Fix navigation configuration in mkdocs.yml
```bash
# Check for uppercase directories
find docs/site -mindepth 1 -maxdepth 1 -print0 | while IFS= read -r -d '' item; do basename "$item"; done | grep -E '[A-Z]'

# Fix navigation keys to use lowercase
# Change: - API_Reference: to: - apireference:
# Change: - Use_Cases: to: - usecases:
```

#### Deployment fails
**Solution**: Check git permissions
```bash
git remote -v
git push origin main  # Test push access
```

### Manual Recovery

If deployment fails and leaves inconsistent state:

```bash
# Return to main branch
git checkout main

# Clean up
git clean -fdx
git reset --hard HEAD

# Try again
./scripts/deploy_docs.sh
```

## Configuration

### Script Configuration
Key variables in `deploy_docs.sh`:
- `DEPLOY_BRANCH`: Branch for deployment (default: `gh-pages`)
- `MAIN_BRANCH`: Source branch (default: `main`)
- `SITE_URL`: Live site URL

### GitHub Pages Configuration
- **Source**: Deploy from a branch
- **Branch**: `gh-pages`
- **Folder**: `/ (root)`

### MkDocs Configuration
- **User docs**: `docs/mkdocs.yml`
- **Maintainer docs**: `docs/mkdocs-maintainers.yml`

## Best Practices

### Before Deployment
1. **Test locally first**:
   ```bash
   ./scripts/deploy_docs.sh --dry-run
   ```

2. **Review changes**:
   ```bash
   git diff HEAD~1 docs/
   ```

3. **Check for broken links**:
   ```bash
   cd docs
   python -m mkdocs build --strict
   ```

### During Development
1. **Use dry runs for testing**:
   ```bash
   ./scripts/deploy_docs.sh --dry-run --verbose
   ```

2. **Keep working directory clean**:
   ```bash
   git status
   git add .
   git commit -m "Your changes"
   ```

3. **Test maintainer docs separately**:
   ```bash
   ./scripts/deploy_docs.sh --maintainers --dry-run
   ```

### After Deployment
1. **Verify the deployment**:
   - Check: https://grapa-dev.github.io/grapa/
   - Test navigation links
   - Verify search functionality

2. **Monitor for issues**:
   - Check GitHub Actions logs
   - Monitor for 404 errors
   - Verify all examples work

## Migration from Manual Process

### What Changed
- **Before**: Manual git commands, error-prone branch management
- **After**: Automated script with comprehensive error handling

### Benefits
- **Reliability**: Consistent deployment process
- **Safety**: Prevents accidental file deletion
- **Speed**: Automated workflow saves time
- **Debugging**: Clear error messages and logging
- **Recovery**: Built-in recovery mechanisms

### Backward Compatibility
- Old manual process still works if needed
- Script can be used alongside existing workflows
- No breaking changes to documentation structure

## Case Sensitivity Validation

### Overview

The deployment system includes automated case sensitivity validation to prevent 404 errors caused by uppercase file/folder names on GitHub Pages. This validation ensures all generated directories use lowercase naming for cross-platform compatibility.

### How Validation Works

The validation runs after the build step and before deployment:

1. **Scans build output**: Checks all files and directories in the site directory
2. **Detects uppercase**: Identifies any items with uppercase letters in their names
3. **Fails deployment**: Stops the process if uppercase items are found
4. **Provides guidance**: Lists problematic files and suggests fixes

### Validation Example

```bash
[INFO] Validating case sensitivity - checking for uppercase files/folders...
[ERROR] Found uppercase files/folders in build output:
[ERROR]   - API_REFERENCE
[ERROR]   - USE_CASES
[ERROR] All files and folders must use lowercase naming for case-sensitive compatibility.
[ERROR] Please fix the navigation configuration in mkdocs.yml to use lowercase keys.
```

### Fixing Case Sensitivity Issues

When validation fails, the issue is typically in the MkDocs navigation configuration:

#### Problematic Configuration
```yaml
nav:
  - API_Reference:
      - Overview: api_reference.md
  - Use_Cases:
      - Overview: use_cases/index.md
```

#### Correct Configuration
```yaml
nav:
  - apireference:
      - Overview: api_reference.md
  - usecases:
      - Overview: use_cases/index.md
```

### Manual Validation

You can manually check for case sensitivity issues:

```bash
# Check for uppercase directories
find docs/site -mindepth 1 -maxdepth 1 -print0 | while IFS= read -r -d '' item; do basename "$item"; done | grep -E '[A-Z]'

# If no output, validation passes
# If output shows directories, fix navigation configuration
```

### Best Practices

1. **Use lowercase navigation keys**: Always use lowercase for navigation section names
2. **Avoid spaces and special characters**: Use underscores or hyphens instead
3. **Test before deployment**: Run validation manually if unsure
4. **Rebuild after changes**: Always rebuild after fixing navigation configuration

## Deployment Verification System

### How Verification Works

The deployment system includes a sophisticated verification mechanism that ensures the live site actually contains the expected deployment:

#### 1. Deployment Fingerprint Generation
- **Unique identifier**: Creates a timestamp and git commit-based fingerprint
- **Deployment file**: Stores fingerprint in `deployment-fingerprint.txt` on the live site
- **Local tracking**: Maintains fingerprint locally for verification

#### 2. Multi-Stage Verification
- **Primary verification**: Checks for deployment fingerprint on live site
- **Fallback verification**: Content-based verification if fingerprint unavailable
- **Retry logic**: Up to 12 attempts over 2 minutes to verify deployment

#### 3. Verification Checks
- **Site accessibility**: Ensures site is reachable
- **Fingerprint match**: Verifies expected deployment is live
- **Content validation**: Checks for expected page elements
- **Search functionality**: Validates search features are working

#### 4. Verification Output
- **Success indicators**: Clear ✅/❌ status with detailed messages
- **Timing information**: Shows verification attempts and timing
- **Manual fallback**: Preserves fingerprint for manual verification if needed

### Verification Example

```bash
[INFO] Verifying deployment...
[INFO] Expected deployment fingerprint: GRAPA_DEPLOYMENT_1753048937_3029aec
[INFO] Verification attempt 1/12...
[INFO] Deployment fingerprint not found yet, checking site accessibility...
[INFO] Verification attempt 2/12...
[SUCCESS] ✅ Deployment fingerprint verified!
[SUCCESS] ✅ Site contains the expected deployment at https://grapa-dev.github.io/grapa/
```

### Manual Verification

If automated verification fails, you can manually verify:

```bash
# Check expected fingerprint
cat .deployment-fingerprint

# Check live site fingerprint
curl -s https://grapa-dev.github.io/grapa/deployment-fingerprint.txt

# Compare the two to verify deployment
```

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
1. Check `scripts/README.md` for detailed usage
2. Review this deployment guide
3. Check GitHub Actions logs for detailed error messages
4. Use `--verbose` flag for debugging

### Contributing
When modifying the deployment system:
1. Test changes locally first
2. Use dry runs to validate
3. Update documentation if needed
4. Consider backward compatibility

## Changelog

### Version 1.0.0 (Current)
- **Initial deployment script** with comprehensive error handling
- **GitHub Actions workflow** for automated deployment
- **Support for both user and maintainer documentation**
- **Robust branch management** and cleanup
- **Comprehensive validation** and verification
- **Detailed documentation** and troubleshooting guides

### Previous Manual Process Issues Resolved
- ✅ **Branch conflicts**: Fresh orphan branch creation
- ✅ **File deletion**: Safe cleanup procedures
- ✅ **Case sensitivity**: Automated validation prevents uppercase files/folders
- ✅ **Build failures**: Comprehensive validation
- ✅ **Deployment errors**: Clear error messages and recovery
- ✅ **Manual steps**: Fully automated process
- ✅ **Verification uncertainty**: Fingerprint-based deployment verification
- ✅ **Manual checking**: Automated verification with retry logic 