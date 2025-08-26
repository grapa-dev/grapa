# OpenSSL 3.5.2 Migration Plan

## Overview

This document outlines the comprehensive plan for migrating Grapa from OpenSSL 1.1.1 to OpenSSL 3.5.2. The migration involves building static libraries for all supported platforms, updating C++ code for API compatibility, and implementing comprehensive regression testing.

## Current OpenSSL Usage Analysis

### C++ Files Using OpenSSL

Based on code analysis, the following C++ files use OpenSSL APIs:

1. **`source/grapa/GrapaTinyAES.cpp`** - AES encryption/decryption
   - Uses: `EVP_CIPHER_CTX_new()`, `EVP_CIPHER_CTX_free()`, `EVP_CipherInit_ex()`, `EVP_CipherUpdate()`, `EVP_CipherFinal_ex()`
   - Includes: `openssl/evp.h`, `openssl/err.h`, `openssl/aes.h`, `openssl/rand.h`

2. **`source/grapa/GrapaValue.cpp`** - Big number operations
   - Uses: `BN_*` functions for mathematical operations
   - Includes: `openssl/bn.h`

3. **`source/grapa/GrapaLink.cpp`** - SSL/TLS networking
   - Uses: SSL context and connection management
   - Includes: `openssl/ssl.h`, `openssl/err.h`

4. **`source/grapa/GrapaNet.cpp`** - Network SSL/TLS implementation
   - Uses: `SSL_CTX_new()`, `SSL_CTX_use_certificate_chain_file()`, `SSL_CTX_use_PrivateKey_file()`, `SSL_new()`, `SSL_set_connect_state()`
   - Includes: `openssl/err.h`

5. **`source/grapa/GrapaSystem.cpp`** - Random number generation
   - Uses: `RAND_*` functions
   - Includes: `openssl/rand.h`

### Regression Test Files

The following `.grc` files will serve as regression tests:

#### Network Tests (Primary OpenSSL Usage)
- `test/network/openssl_compatibility_test.grc` - Direct OpenSSL compatibility testing
- `test/network/curl_function_test.grc` - HTTPS/SSL functionality testing
- `test/network/curl_function_optimized_test.grc` - Optimized curl function testing

#### Core Tests (Mathematical Functions)
- `test/core/test_cosine_comprehensive.grc` - Mathematical operations
- `test/core/test_cosine_detailed.grc` - Detailed cosine calculations
- `test/core/test_cosine_options.grc` - Cosine function options
- `test/core/test_float_comparison.grc` - Float comparison operations
- `test/core/test_float_corner_cases.grc` - Float edge cases
- `test/core/test_float_to_float_comparisons.grc` - Float-to-float comparisons

#### Documentation Examples (SSL/HTTPS)
- `docs-src/docs/examples/https_test.grc` - HTTPS functionality examples
- `docs-src/docs/examples/curl_function_simple.grc` - SSL/TLS certificate handling

## Build Script Requirements

### New Build Script: `scripts/build/build_openssl.py`

The script will replicate the functionality of `build_fltk.py` with the following features:

1. **Platform Support**: All supported platforms (mac-arm64, linux-arm64, linux-amd64, windows-amd64, aws-arm64, aws-amd64)
2. **Static Library Building**: Build OpenSSL static libraries for all Grapa targets
3. **Clean Functionality**: Remove build artifacts and installed libraries
4. **Verify Functionality**: Verify all required libraries are present and properly sized
5. **Cross-Platform**: Support for different build systems (autotools, Visual Studio, etc.)

### Current Build Commands (to be automated)

#### Linux Build Process:
```bash
./config no-shared
make
sudo make install
```

#### Windows Build Process:
```bash
perl Configure VC-WIN64A
nmake
nmake install
```

### Expected OpenSSL Libraries

The build script will create the following static libraries:
- `libcrypto.a` - Core cryptographic functions
- `libssl.a` - SSL/TLS protocol implementation
- `libcrypto.lib` / `libssl.lib` - Windows equivalents

## C++ Code Changes Required

### Minimal Changes Expected

Based on OpenSSL 3.x compatibility analysis, the following changes are expected:

#### 1. **GrapaTinyAES.cpp** - No Changes Required
- All `EVP_*` functions used are compatible with OpenSSL 3.x
- `EVP_CIPHER_CTX_new()` and `EVP_CIPHER_CTX_free()` work identically
- `EVP_CipherInit_ex()`, `EVP_CipherUpdate()`, `EVP_CipherFinal_ex()` are unchanged

#### 2. **GrapaValue.cpp** - No Changes Required
- `BN_*` functions for big number operations are unchanged in OpenSSL 3.x

#### 3. **GrapaLink.cpp** - No Changes Required
- Basic SSL context management functions are compatible

#### 4. **GrapaNet.cpp** - Potential Minor Changes
- `TLS_client_method()` is the recommended replacement for `TLSv1_2_client_method()` in OpenSSL 3.x
- All other SSL functions used are compatible

#### 5. **GrapaSystem.cpp** - No Changes Required
- `RAND_*` functions are unchanged in OpenSSL 3.x

### Potential API Changes to Monitor

1. **Deprecated Functions**: Some functions might be deprecated but still available
2. **Provider System**: OpenSSL 3.x introduces a provider system, but legacy providers maintain compatibility
3. **FIPS Mode**: FIPS mode handling might need updates if used

## Implementation Steps

### Phase 1: Pre-Migration Testing
1. **Catalog Current Test Results**: Run all regression tests and document results
2. **Create Test Baseline**: Document expected outputs for all test files
3. **Verify Current OpenSSL Version**: Confirm current version is 1.1.1

### Phase 2: Build Script Development
1. **Create `scripts/build/build_openssl.py`**: Implement cross-platform OpenSSL build script
2. **Test Build Process**: Verify script works on all supported platforms
3. **Implement Clean/Verify**: Add clean and verification functionality

### Phase 3: OpenSSL 3.5.2 Build
1. **Build Static Libraries**: Use new build script to create OpenSSL 3.5.2 static libraries
2. **Update Include Files**: Replace `source/openssl/` headers with OpenSSL 3.5.2 headers
3. **Deploy Libraries**: Copy built libraries to `source/openssl-lib/` by platform

### Phase 4: Code Compatibility Testing
1. **Compile Test**: Verify all C++ code compiles with OpenSSL 3.5.2
2. **API Compatibility**: Test all OpenSSL function calls work correctly
3. **Fix Any Issues**: Address any compilation or runtime issues

### Phase 5: Regression Testing
1. **Run All Tests**: Execute all cataloged regression tests
2. **Compare Results**: Compare with pre-migration baseline
3. **Investigate Differences**: Address any unexpected changes in behavior

### Phase 6: Integration Testing
1. **Build Grapa**: Verify Grapa builds successfully with OpenSSL 3.5.2
2. **Test All Targets**: Test executable, static library, and shared library builds
3. **Platform Testing**: Verify builds work on all supported platforms

## Risk Assessment

### Low Risk
- **API Compatibility**: OpenSSL 3.x maintains backward compatibility for most APIs
- **Static Linking**: Using static libraries reduces runtime dependency issues
- **Comprehensive Testing**: Extensive regression test suite will catch issues

### Medium Risk
- **Platform-Specific Issues**: Different platforms might have different OpenSSL configurations
- **Build System Complexity**: OpenSSL build process is more complex than FLTK

### Mitigation Strategies
1. **Incremental Testing**: Test each component individually before full integration
2. **Platform-Specific Validation**: Test on each supported platform
3. **Rollback Plan**: Maintain ability to revert to OpenSSL 1.1.1 if needed

## Success Criteria

1. **Build Success**: All Grapa targets build successfully with OpenSSL 3.5.2
2. **Test Pass**: All regression tests pass with identical or improved results
3. **Performance**: No significant performance degradation
4. **Compatibility**: All existing functionality works as expected
5. **Documentation**: Updated build and deployment documentation

## Timeline Estimate

- **Phase 1**: 1-2 days (testing and baseline establishment)
- **Phase 2**: 2-3 days (build script development and testing)
- **Phase 3**: 1-2 days (OpenSSL build and deployment)
- **Phase 4**: 1-2 days (code compatibility testing)
- **Phase 5**: 1-2 days (regression testing and validation)
- **Phase 6**: 1-2 days (integration testing and final validation)

**Total Estimated Time**: 7-13 days

## Next Steps

1. Begin Phase 1: Catalog and run all regression tests
2. Document current test results as baseline
3. Start development of `scripts/build/build_openssl.py`
4. Create detailed implementation plan for each phase
