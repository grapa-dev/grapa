# Documentation Deployment Guide

## Overview
This guide explains how to deploy documentation changes to the live GitHub Pages site at https://grapa-dev.github.io/grapa/

## GitHub Pages Configuration
The site is configured with the following settings:
- **Source**: Deploy from a branch
- **Branch**: `gh-pages`
- **Folder**: `/docs`

This means GitHub Pages serves content from the `/docs` folder within the `gh-pages` branch.

## Deployment Process

### 1. Build the Documentation
```bash
cd docs
python3 -m mkdocs build --clean
```

This creates the built site in `docs/site/` with all HTML files and assets.

### 2. Switch to gh-pages Branch
```bash
git checkout gh-pages
```

### 3. Update the /docs Folder
The built site needs to be copied to the `/docs` folder where GitHub Pages expects it:

```bash
# Remove old content (be careful with this command)
cd docs
rm -rf USE_CASES  # Remove old uppercase directory if it exists

# Copy new built site to docs folder
cp -r site/use_cases .     # New organized use cases
cp -r site/syntax .        # Syntax section
cp -r site/*.html .        # Main pages
cp -r site/assets .        # CSS/JS assets
cp -r site/sitemap* .      # Sitemap files
```

### 4. Commit and Push
```bash
cd ..
git add docs/
git commit -m "Deploy updated documentation with [description of changes]"
git push origin gh-pages
```

### 5. Wait for Deployment
GitHub Pages typically takes 2-5 minutes to update after a push.

## Important Notes

### File Structure
- **Source files**: Located in `docs/docs/` (main branch)
- **Built site**: Located in `docs/site/` (after mkdocs build)
- **Deployed site**: Located in `docs/` (gh-pages branch)

### Navigation Configuration
The navigation is configured in `docs/mkdocs.yml` and includes:
- Use Cases with organized subsections
- Syntax section with subsections
- All other documentation sections

### Common Issues

#### Issue: Site not updating
- **Cause**: Built site not copied to correct `/docs` folder
- **Solution**: Ensure files are in `docs/` folder in gh-pages branch

#### Issue: Old content still showing
- **Cause**: Browser caching or GitHub Pages caching
- **Solution**: Wait 5-10 minutes, try incognito mode, or hard refresh

#### Issue: Missing sections
- **Cause**: Navigation not updated in mkdocs.yml
- **Solution**: Check navigation configuration and rebuild

### Verification
To verify deployment worked:
1. Check that `use_cases/` (lowercase) exists in `docs/` folder
2. Check that `syntax/` directory exists in `docs/` folder
3. Visit https://grapa-dev.github.io/grapa/ and verify navigation
4. Check that all subsections are accessible

## Quick Deployment Script
For future automation, here's a complete deployment script:

```bash
#!/bin/bash
# Deploy documentation to GitHub Pages

echo "Building documentation..."
cd docs
python3 -m mkdocs build --clean

echo "Switching to gh-pages branch..."
cd ..
git checkout gh-pages

echo "Updating docs folder..."
cd docs
rm -rf USE_CASES 2>/dev/null || true
cp -r site/use_cases .
cp -r site/syntax .
cp -r site/*.html .
cp -r site/assets .
cp -r site/sitemap* .

echo "Committing and pushing..."
cd ..
git add docs/
git commit -m "Deploy documentation updates"
git push origin gh-pages

echo "Deployment complete! Site will update in 2-5 minutes."
```

## Related Files
- `docs/mkdocs.yml` - Main configuration file
- `docs/docs/` - Source documentation files
- `docs/site/` - Built site (after mkdocs build)
- `docs/` - Deployed site (in gh-pages branch)

## Troubleshooting
If deployment fails:
1. Check that you're on the gh-pages branch
2. Verify the `/docs` folder contains the built site
3. Ensure all files are committed and pushed
4. Wait for GitHub Pages to update (2-5 minutes)
5. Clear browser cache if needed 