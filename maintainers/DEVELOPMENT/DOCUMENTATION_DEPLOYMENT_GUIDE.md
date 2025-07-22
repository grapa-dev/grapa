## Documentation Deployment Guide

### Overview

The Grapa documentation deployment process is now streamlined and automated. User documentation is built from `/docs-src` and output to `/docs`. Deployment to the live site is handled automatically by GitHub Pages when `/docs` is committed and pushed to the `main` branch. No branch switching or manual deployment is required.

### Step-by-Step Deployment

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

### Troubleshooting
- If the build fails, check for errors in your Markdown or MkDocs config in `/docs-src`.
- If the site does not update, ensure you have committed and pushed `/docs` to `main`.
- For more details, see `maintainers/BUILD_AND_DEPLOYMENT/DOCUMENTATION_DEPLOYMENT.md`. 