# Grapa Documentation Build & Deployment

This directory contains scripts and workflows for building and deploying the Grapa documentation to GitHub Pages.

## Quick Start

### Build and Deploy User Documentation

1. **Build the documentation:**
   ```bash
   ./scripts/deploy_docs.sh
   ```
   - This builds the user documentation from `/docs-src` and outputs to `/docs`.
   - All validation (case sensitivity, link checks, etc.) is performed automatically.

2. **Commit and push the built docs:**
   ```bash
   git add docs/
   git commit -m "docs: Update user documentation"
   git push origin main
   ```
   - This will trigger GitHub Pages to update the live site from `/docs` on the `main` branch.

3. **Check the live site:**
   - Visit: https://grapa-dev.github.io/grapa/
   - Changes may take a few minutes to appear.

### Notes
- Maintainer documentation is not built or deployed by this script.
- No branch switching or manual deployment is required.
- All deployment is handled automatically by GitHub Pages.

## Files

### `deploy_docs.sh`
- Builds user documentation from `/docs-src` to `/docs`.
- Performs validation (case sensitivity, etc.).
- Does **not** deploy or push; you must commit and push `/docs` to deploy.

## Prerequisites
- Python 3.7+
- MkDocs Material theme: `pip install mkdocs-material`
- Pymdown extensions: `pip install pymdown-extensions`

## Troubleshooting
- If the build fails, check for errors in your Markdown or MkDocs config in `/docs-src`.
- If the site does not update, ensure you have committed and pushed `/docs` to `main`.
- For more details, see `maintainers/DEVELOPMENT/DOCUMENTATION_DEPLOYMENT_GUIDE.md`. 