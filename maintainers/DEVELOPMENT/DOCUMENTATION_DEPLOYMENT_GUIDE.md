## Deployment Troubleshooting

### Common Deployment Issues

#### 1. Git Checkout Conflicts
**Problem**: Cannot switch to gh-pages branch due to untracked files or local changes.

**Symptoms**:
```
error: The following untracked working tree files would be overwritten by checkout
error: Your local changes to the following files would be overwritten by checkout
```

**Solutions**:
```bash
# Option 1: Stash changes and clean
git stash
git clean -fdx
git checkout gh-pages

# Option 2: Force clean everything
git clean -fdx
git checkout gh-pages

# Option 3: Remove specific conflicting directories
rm -rf docs/site
rm -rf *.html
rm -rf assets
rm -rf database
rm -rf directory_navigation
rm -rf examples
git checkout gh-pages
```

#### 2. Case Sensitivity Conflicts
**Problem**: Git reports file alias conflicts due to case sensitivity differences.

**Symptoms**:
```
fatal: will not add file alias 'docs/site/USE_CASES/compiler_bnf_learning/index.html' 
('docs/site/use_cases/compiler_bnf_learning/index.html' already exists in index)
```

**Solutions**:
```bash
# Remove the built site directory completely
rm -rf docs/site

# Then add and commit
git add docs/
git commit -m "Deploy latest documentation"
```

#### 3. Built Site Cleanup
**Problem**: Previous build artifacts interfere with deployment.

**Prevention**:
```bash
# Always clean before building
cd docs && python3 -m mkdocs build --clean

# Remove any existing site directory before copying
rm -rf docs/site
```

#### 4. Branch Switching Workflow
**Recommended Process**:
```bash
# 1. Build the site
cd docs && python3 -m mkdocs build --clean

# 2. Stash any changes
cd .. && git stash

# 3. Switch to gh-pages (DO NOT clean while on main branch!)
git checkout gh-pages

# 4. Clean workspace (ONLY after switching to gh-pages)
git clean -fdx

# 5. Copy built site
cd docs && cp -r site/* . && cp -r site/.nojekyll . 2>/dev/null || true

# 6. Commit and push
cd .. && git add docs/ && git commit -m "Deploy latest documentation" && git push origin gh-pages

# 7. Return to main branch
git checkout main && git stash pop
```

**🚨 CRITICAL WARNING**: Never run `git clean -fdx` while on the main branch! This will delete all source files. Only clean the gh-pages branch after switching to it.

#### 5. Main Branch Deletion Prevention
**🚨 CRITICAL ISSUE**: Running `git clean -fdx` on the main branch will delete all source files.

**What Happened**: During deployment, `git clean -fdx` was run while on the main branch, deleting all source files.

**Prevention**:
```bash
# ❌ NEVER DO THIS on main branch
git clean -fdx  # This deletes all untracked files including source!

# ✅ ONLY do this after switching to gh-pages
git checkout gh-pages
git clean -fdx  # Safe to clean deployment branch
```

**Recovery**: If this happens, immediately run:
```bash
git reset --hard HEAD  # Restores all tracked files
```

**Best Practice**: Always verify current branch before running destructive commands:
```bash
git branch  # Check current branch
git clean -fdx  # Only if on gh-pages branch
```

### Deployment Best Practices

#### Pre-Deployment Checklist
- [ ] All documentation changes committed to main branch
- [ ] All examples tested and syntax-reviewed
- [ ] MkDocs build completes without errors
- [ ] No broken links in build output
- [ ] Workspace is clean (no uncommitted changes)
- [ ] **VERIFY**: Currently on main branch before starting deployment
- [ ] **VERIFY**: All source files present before deployment

#### Post-Deployment Verification
- [ ] GitHub Pages site loads correctly
- [ ] All navigation links work
- [ ] Examples directory is accessible
- [ ] Search functionality works
- [ ] No 404 errors on key pages

### Emergency Rollback
If deployment causes issues:

```bash
# Revert to previous version
git checkout gh-pages
git log --oneline -5  # Find previous good commit
git reset --hard <previous-commit-hash>
git push origin gh-pages --force
```

## Examples Directory Management 