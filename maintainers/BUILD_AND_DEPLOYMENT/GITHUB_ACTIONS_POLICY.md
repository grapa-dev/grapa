# GitHub Actions Workflow Policy

## Overview

This document establishes the policy for GitHub Actions workflows in the Grapa project to ensure **explicit control** and prevent accidental deployments or builds.

## Core Principle

**All GitHub Actions workflows must use explicit triggers only. No workflow should be triggered automatically by commits, pushes, or other operations.**

## Current Workflows

### 1. Build Windows AMD64 (`build-windows.yml`)
- **Trigger**: `workflow_dispatch` (manual only)
- **Purpose**: Build Windows AMD64 artifacts
- **How to trigger**: 
  - GitHub UI: Actions → "Build Windows AMD64" → "Run workflow"
  - Command line: `gh workflow run "Build Windows AMD64"`
  - Script: `build_all_platforms.sh --bump-version` (calls `gh workflow run`)

### 2. Deploy to PyPI (`deploy-pypi.yml`)
- **Trigger**: `workflow_dispatch` with required inputs
- **Purpose**: Deploy Python package to PyPI
- **Required inputs**:
  - `version`: Version to deploy (e.g., "0.0.267")
  - `confirm`: Must type "YES" to confirm deployment
- **How to trigger**:
  - GitHub UI: Actions → "Deploy to PyPI" → "Run workflow" → Enter version and "YES"
  - Script: `./scripts/build/manual_pypi_deploy.sh 0.0.267`

## Policy Requirements

### ✅ Allowed Trigger Types
- `workflow_dispatch` - Manual trigger only
- `workflow_dispatch` with required inputs - Manual trigger with confirmation

### ❌ Prohibited Trigger Types
- `push` - Automatic trigger on commits
- `push: tags: ['v*']` - Automatic trigger on tag pushes
- `pull_request` - Automatic trigger on PRs
- `schedule` - Automatic scheduled triggers
- Any other automatic triggers

### 🔒 Safety Measures
1. **Double Confirmation**: Critical workflows require explicit confirmation
2. **Version Specification**: PyPI deployment requires exact version input
3. **Manual Control**: All workflows must be triggered manually
4. **No Side Effects**: Scripts should not trigger workflows as side effects

## Built-in GitHub Workflows

### Pages Build and Deployment
- **Status**: Built-in GitHub Pages workflow (not in our repository)
- **Trigger**: Automatic on push to main branch
- **Control**: This is a GitHub-provided workflow that we cannot modify
- **Mitigation**: Documentation deployment scripts require explicit `--push` flag

## Script Integration

### Version Bumping (`bump_version_and_deploy.py`)
- ✅ Creates Git tag locally
- ✅ Pushes commit to main (triggers Pages workflow - expected)
- ❌ Does NOT push tag (prevents accidental PyPI deployment)
- ✅ Requires manual tag push for PyPI deployment

### Platform Builds (`build_all_platforms.sh`)
- ✅ Triggers Windows workflow explicitly via `gh workflow run`
- ✅ Does NOT trigger PyPI deployment
- ✅ Does NOT push tags automatically

### Documentation Deployment
- ✅ Requires explicit `--push` flag
- ✅ Does NOT trigger automatically on commits
- ✅ Manual control over Pages deployment

## Compliance Checklist

When creating or modifying GitHub Actions workflows:

- [ ] Uses `workflow_dispatch` trigger only
- [ ] No automatic triggers (`push`, `pull_request`, etc.)
- [ ] Requires explicit confirmation for critical operations
- [ ] Scripts trigger workflows explicitly, not as side effects
- [ ] Documentation updated to reflect manual trigger requirements
- [ ] Tested to ensure no accidental triggers

## Monitoring

### Workflow Audit
Regular audits to ensure:
1. No workflows use automatic triggers
2. All workflows require explicit manual action
3. Scripts don't accidentally trigger workflows
4. Documentation reflects manual trigger requirements

### Current Status
- ✅ **Build Windows AMD64**: Uses `workflow_dispatch` only
- ✅ **Deploy to PyPI**: Uses `workflow_dispatch` with confirmation
- ✅ **Pages Build**: Built-in workflow (expected behavior)
- ✅ **All Scripts**: Require explicit action for workflow triggers

## Best Practices

1. **Always use `workflow_dispatch`** for new workflows
2. **Require confirmation** for deployment workflows
3. **Test trigger behavior** before committing workflow changes
4. **Document manual trigger process** in relevant scripts
5. **Audit existing workflows** regularly for compliance

## Enforcement

- **Code Review**: All workflow changes must be reviewed for trigger compliance
- **Testing**: Verify workflows only trigger when explicitly requested
- **Documentation**: Update relevant docs when workflow triggers change
- **Monitoring**: Regular audits of workflow trigger configurations

---

**Last Updated**: December 2024  
**Status**: ✅ All workflows compliant with explicit trigger policy 