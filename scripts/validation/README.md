# Validation Scripts

This directory contains scripts for testing and validating Grapa builds, syntax, and functionality.

## Scripts

### Platform Validation
- `check_platform_status.sh` - Check status of all platform builds and artifacts
- `validate_grapa.sh` - Basic Grapa validation
- `validate_builds.py` - Validate build artifacts and functionality

### Syntax Validation
- `validate_grapa_syntax.py` - Comprehensive Grapa syntax validation
- `README_VALIDATOR.md` - Documentation for syntax validation

## Usage

### Check Platform Status
```bash
./scripts/validation/check_platform_status.sh
```

### Validate Grapa Syntax
```bash
./scripts/validation/validate_grapa_syntax.py
```

### Validate Builds
```bash
./scripts/validation/validate_builds.py
```

## Dependencies

These scripts require:
- Python 3
- Grapa executable (for validation)
- Build artifacts in `bin/` directory 