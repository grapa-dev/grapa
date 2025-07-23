# Documentation Deployment System (Canonical Guide)

> **Note:** This is the single canonical guide for documentation deployment. The previous `DEVELOPMENT/DOCUMENTATION_DEPLOYMENT_GUIDE.md` is now deprecated. For deployment instructions, always use this file. If you find a reference to the old guide, update it to point here.

## Overview

The Grapa documentation deployment system is fully automated and streamlined. User documentation is built from `/docs-src` and output to `/docs`. Deployment to the live site is handled automatically by GitHub Pages when `/docs` is committed and pushed to the `main` branch. No branch switching or manual deployment is required.

## Quick Start / Step-by-Step Deployment

1. **Build the documentation:**
   ```bash
   ./scripts/deploy_docs.sh
   ```
   - Builds user documentation from `/docs-src` and outputs to `/docs`.
   - Performs validation (case sensitivity, link checks, etc.) automatically.

2. **Commit and push the built docs:**
   ```bash
   git add docs/
   git commit -m "docs: Update user documentation"
   git push origin main
   ```
   - This triggers GitHub Pages to update the live site from `/docs` on the `main` branch.

3. **Check the live site:**
   - Visit: https://grapa-dev.github.io/grapa/
   - Changes may take a few minutes to appear.

### Notes
- Maintainer documentation is not built or deployed by this script.
- No branch switching or manual deployment is required.
- All deployment is handled automatically by GitHub Pages.

## System Components

### 1. Build Script (`scripts/deploy_docs.sh`)
- Builds user documentation from `/docs-src` to `/docs`.
- Performs validation (case sensitivity, etc.).
- Does **not** deploy or push; you must commit and push `/docs` to deploy.

### 2. GitHub Pages
- Automatically serves the site from `/docs` on the `main` branch.
- No need for a `gh-pages` branch or manual deployment steps.

## Troubleshooting
- If the build fails, check for errors in your Markdown or MkDocs config in `/docs-src`.
- If the site does not update, ensure you have committed and pushed `/docs` to `main`.
- For more details, see this file (`BUILD_AND_DEPLOYMENT/DOCUMENTATION_DEPLOYMENT.md`).
- If you find a reference to `DEVELOPMENT/DOCUMENTATION_DEPLOYMENT_GUIDE.md`, update it to point here. 