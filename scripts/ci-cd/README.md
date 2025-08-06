# CI/CD Scripts

This directory contains scripts for Continuous Integration/Continuous Deployment operations, primarily focused on Windows build artifact management.

## Scripts

### Windows Artifact Management
- `monitor_and_download_windows.sh` - Monitor and download Windows build artifacts (Bash)
- `monitor_and_download_windows.ps1` - Monitor and download Windows build artifacts (PowerShell)
- `download_windows_artifacts.sh` - Download Windows artifacts
- `download_windows_artifacts_force.sh` - Force download Windows artifacts

### Workflow Management
- `trigger_workflow.sh` - Trigger GitHub Actions workflows

## Usage

### Monitor Windows Builds
```bash
./scripts/ci-cd/monitor_and_download_windows.sh
```

### Force Download Artifacts
```bash
./scripts/ci-cd/download_windows_artifacts_force.sh
```

### Trigger Workflow
```bash
./scripts/ci-cd/trigger_workflow.sh
```

## Dependencies

These scripts require:
- GitHub CLI (`gh`) for workflow monitoring
- PowerShell (for .ps1 scripts)
- Network connectivity to GitHub 