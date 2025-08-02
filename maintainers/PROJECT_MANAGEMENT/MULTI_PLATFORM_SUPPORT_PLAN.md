# Multi-Platform Support Plan

**Date:** August 2, 2025  
**Objective:** Support Windows ARM64, Mac ARM64, Mac AMD64, Linux AMD64, Linux ARM64  
**Secondary Objective:** Eliminate AWS-specific builds in favor of standard Linux builds  

## Current State Analysis

### Current Platform Support
**Available Platforms:**
- ✅ Windows AMD64 (`win-amd64`)
- ✅ Mac ARM64 (`mac-arm64`) 
- ✅ Mac AMD64 (`mac-amd64`)
- ✅ Linux AMD64 (`linux-amd64`)
- ✅ Linux ARM64 (`linux-arm64`)
- ❌ Windows ARM64 (`win-arm64`) - **MISSING**
- ❌ AWS AMD64 (`aws-amd64`) - **TO BE ELIMINATED**
- ❌ AWS ARM64 (`aws-arm64`) - **TO BE ELIMINATED**

### Current GitHub Actions Matrix
```yaml
matrix:
  os: [windows-latest, ubuntu-latest, macos-latest]
  python-version: ['3.8', '3.9', '3.10', '3.11', '3.12']
```

**Issues:**
- ❌ No ARM64 support in GitHub Actions
- ❌ No Windows ARM64 support
- ❌ AWS-specific builds still present

## Required Changes

### 1. GitHub Actions Workflow Updates

**Current Matrix:**
```yaml
matrix:
  os: [windows-latest, ubuntu-latest, macos-latest]
  python-version: ['3.8', '3.9', '3.10', '3.11', '3.12']
```

**Required Matrix:**
```yaml
matrix:
  include:
    # Windows AMD64
    - os: windows-latest
      platform: win-amd64
      python-version: '3.8'
    - os: windows-latest
      platform: win-amd64
      python-version: '3.9'
    - os: windows-latest
      platform: win-amd64
      python-version: '3.10'
    - os: windows-latest
      platform: win-amd64
      python-version: '3.11'
    - os: windows-latest
      platform: win-amd64
      python-version: '3.12'
    
    # Windows ARM64 (NEW)
    - os: windows-latest
      platform: win-arm64
      python-version: '3.8'
    - os: windows-latest
      platform: win-arm64
      python-version: '3.9'
    - os: windows-latest
      platform: win-arm64
      python-version: '3.10'
    - os: windows-latest
      platform: win-arm64
      python-version: '3.11'
    - os: windows-latest
      platform: win-arm64
      python-version: '3.12'
    
    # Linux AMD64
    - os: ubuntu-latest
      platform: linux-amd64
      python-version: '3.8'
    - os: ubuntu-latest
      platform: linux-amd64
      python-version: '3.9'
    - os: ubuntu-latest
      platform: linux-amd64
      python-version: '3.10'
    - os: ubuntu-latest
      platform: linux-amd64
      python-version: '3.11'
    - os: ubuntu-latest
      platform: linux-amd64
      python-version: '3.12'
    
    # Linux ARM64 (NEW)
    - os: ubuntu-latest
      platform: linux-arm64
      python-version: '3.8'
    - os: ubuntu-latest
      platform: linux-arm64
      python-version: '3.9'
    - os: ubuntu-latest
      platform: linux-arm64
      python-version: '3.10'
    - os: ubuntu-latest
      platform: linux-arm64
      python-version: '3.11'
    - os: ubuntu-latest
      platform: linux-arm64
      python-version: '3.12'
    
    # macOS AMD64
    - os: macos-latest
      platform: mac-amd64
      python-version: '3.8'
    - os: macos-latest
      platform: mac-amd64
      python-version: '3.9'
    - os: macos-latest
      platform: mac-amd64
      python-version: '3.10'
    - os: macos-latest
      platform: mac-amd64
      python-version: '3.11'
    - os: macos-latest
      platform: mac-amd64
      python-version: '3.12'
    
    # macOS ARM64 (NEW)
    - os: macos-latest
      platform: mac-arm64
      python-version: '3.8'
    - os: macos-latest
      platform: mac-arm64
      python-version: '3.9'
    - os: macos-latest
      platform: mac-arm64
      python-version: '3.10'
    - os: macos-latest
      platform: mac-arm64
      python-version: '3.11'
    - os: macos-latest
      platform: mac-arm64
      python-version: '3.12'
```

### 2. Build System Updates

**Files to Modify:**
1. `build.py` - Remove AWS-specific logic
2. `setup.py` - Remove AWS detection and paths
3. `.github/workflows/build-wheels.yml` - Update matrix and build logic

**AWS Elimination Strategy:**
- Remove AWS detection logic from `setup.py`
- Remove AWS build paths from `build.py`
- Update platform detection to treat AWS as standard Linux
- Remove AWS-specific library directories

### 3. Platform Detection Updates

**Current Logic (setup.py):**
```python
# AWS detection
is_aws = False
if os.path.exists('/etc/system-release'):
    with open('/etc/system-release', 'r') as f:
        if 'Amazon Linux' in f.read():
            is_aws = True
```

**New Logic:**
```python
# Treat AWS as standard Linux
is_aws = False  # Always False - AWS uses standard Linux builds
```

### 4. Directory Structure Changes

**Current Structure:**
```
source/grapa-lib/
├── win-amd64/
├── win-arm64/          # MISSING
├── mac-amd64/
├── mac-arm64/
├── linux-amd64/
├── linux-arm64/
├── aws-amd64/          # TO BE REMOVED
└── aws-arm64/          # TO BE REMOVED
```

**Target Structure:**
```
source/grapa-lib/
├── win-amd64/
├── win-arm64/          # NEW
├── mac-amd64/
├── mac-arm64/
├── linux-amd64/
└── linux-arm64/
```

## Implementation Plan

### Phase 1: Update GitHub Actions (Priority 1)
1. **Update workflow matrix** to include all required platforms
2. **Add platform-specific build logic** for ARM64 architectures
3. **Test workflow** with version 0.0.51

### Phase 2: Eliminate AWS Builds (Priority 2)
1. **Remove AWS detection** from `setup.py`
2. **Update build.py** to treat AWS as standard Linux
3. **Remove AWS-specific directories** and logic
4. **Update documentation** to reflect standard Linux builds

### Phase 3: Add Windows ARM64 Support (Priority 3)
1. **Create Windows ARM64 build configuration**
2. **Add Windows ARM64 library directories**
3. **Test Windows ARM64 builds** in GitHub Actions

### Phase 4: Comprehensive Testing (Priority 4)
1. **Test all platforms** in GitHub Actions
2. **Verify PyPI uploads** for all platforms
3. **Test installation** from PyPI on all platforms
4. **Update documentation** with platform support matrix

## Success Criteria

### Platform Support Matrix
- ✅ Windows AMD64: Build and install from PyPI
- ✅ Windows ARM64: Build and install from PyPI
- ✅ macOS AMD64: Build and install from PyPI
- ✅ macOS ARM64: Build and install from PyPI
- ✅ Linux AMD64: Build and install from PyPI
- ✅ Linux ARM64: Build and install from PyPI

### AWS Elimination
- ❌ No AWS-specific detection in code
- ❌ No AWS-specific build paths
- ❌ No AWS-specific library directories
- ✅ AWS Lambda uses standard Linux builds

### GitHub Actions Coverage
- ✅ All 6 platforms in workflow matrix
- ✅ All Python versions (3.8-3.12) for each platform
- ✅ Successful builds for all combinations
- ✅ PyPI uploads for all platforms

## Risk Assessment

### High Risk
- **Windows ARM64**: May require additional dependencies or build tools
- **GitHub Actions Matrix**: Complex matrix may cause workflow issues

### Medium Risk
- **AWS Elimination**: May break existing AWS Lambda deployments
- **Platform Detection**: Changes may affect existing builds

### Low Risk
- **Documentation Updates**: Straightforward updates
- **PyPI Uploads**: Standard process

## Next Steps

1. **Immediate**: Update GitHub Actions workflow for version 0.0.51
2. **Short-term**: Eliminate AWS-specific builds
3. **Medium-term**: Add Windows ARM64 support
4. **Long-term**: Comprehensive testing and documentation

**Status**: ⏳ **Planning Complete - Ready for Implementation** 