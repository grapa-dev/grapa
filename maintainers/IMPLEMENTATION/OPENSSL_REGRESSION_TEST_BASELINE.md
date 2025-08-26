# OpenSSL 3.5.2 Migration - Regression Test Baseline

## Overview

This document establishes the baseline test results for OpenSSL functionality before migrating from OpenSSL 1.1.1 to OpenSSL 3.5.2. All tests documented here should be re-run after the migration to ensure no regressions.

## Test Environment

- **Platform**: mac-arm64 (Apple Silicon)
- **Current OpenSSL Version**: 1.1.1 (as used by Grapa)
- **Test Date**: December 2024
- **Grapa Version**: 0.1.49

## Regression Test Categories

### 1. Network Tests (Primary OpenSSL Usage)

#### Test Files:
- `test/network/openssl_compatibility_test.grc`
- `test/network/curl_function_test.grc`
- `test/network/curl_function_optimized_test.grc`

#### Current Test Results:

**openssl_compatibility_test.grc**:
```
Starting OpenSSL 3.0 Compatibility Regression Tests...
=== Test 1: Certificate Loading ===
Certificate file not found, skipping certificate test
Private key file not found, skipping private key test
CA certificate file not found, skipping CA test
✓ Certificate loading tests completed

=== Test 2: SSL Verification ===
Connection result: 
✓ SSL verification tests completed

=== Test 3: HTTPS Communication ===
✓ HTTPS communication tests completed

=== Test 4: Client Certificate Authentication ===
Client certificate files not found, skipping client cert auth test
✓ Client certificate authentication tests completed

=== Test 5: Proxy with SSL ===
Proxy SSL connection result: 
✓ Proxy SSL tests completed

=== Test 6: SSL/TLS Protocol Versions ===
Testing protocol: TLSv1.2
Connection result for TLSv1.2: 
Testing protocol: TLSv1.3
Connection result for TLSv1.3: 
✓ SSL/TLS protocol tests completed

=== Test 7: Certificate Chain Validation ===
CA certificate file not found, skipping chain validation test
✓ Certificate chain validation tests completed

=== Test 8: SSL Error Handling ===
Invalid SSL site connection result: 
Invalid cert connection result: 
Invalid key loading result: 
✓ SSL error handling tests completed

=== Test 9: SSL Performance ===
Performance test connection 0: 
Performance test connection 1: 
Performance test connection 2: 
Performance test connection 3: 
Performance test connection 4: 
✓ SSL performance tests completed

=== Test 10: OpenSSL 3.0 Features ===
Note: These tests will become relevant after OpenSSL 3.0 upgrade
OpenSSL 3.0 compatibility test: 
✓ OpenSSL 3.0 feature tests completed

=== All OpenSSL 3.0 Compatibility Tests Completed ===
```

**Expected Behavior After Migration**:
- All tests should continue to pass
- SSL/TLS connections should work with OpenSSL 3.5.2
- Certificate handling should remain compatible
- Performance should be similar or improved

### 2. Core Mathematical Tests (OpenSSL BN Functions)

#### Test Files:
- `test/core/test_cosine_comprehensive.grc`
- `test/core/test_cosine_detailed.grc`
- `test/core/test_cosine_options.grc`
- `test/core/test_float_comparison.grc`
- `test/core/test_float_corner_cases.grc`
- `test/core/test_float_to_float_comparisons.grc`

#### Current Test Results:
*[To be populated after running these tests]*

**Expected Behavior After Migration**:
- All mathematical operations should produce identical results
- Big number operations (BN_* functions) should work identically
- No performance degradation in mathematical calculations

### 3. Documentation Examples (SSL/HTTPS)

#### Test Files:
- `docs-src/docs/examples/https_test.grc`
- `docs-src/docs/examples/curl_function_simple.grc`

#### Current Test Results:
*[To be populated after running these tests]*

**Expected Behavior After Migration**:
- All examples should continue to work
- SSL/TLS certificate handling should remain functional
- HTTPS connections should work with modern protocols

## C++ Code Compatibility Analysis

### Files Using OpenSSL APIs:

1. **`source/grapa/GrapaTinyAES.cpp`**
   - **Functions Used**: `EVP_CIPHER_CTX_new()`, `EVP_CIPHER_CTX_free()`, `EVP_CipherInit_ex()`, `EVP_CipherUpdate()`, `EVP_CipherFinal_ex()`
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - All EVP functions used are unchanged in OpenSSL 3.x
   - **Migration Impact**: None

2. **`source/grapa/GrapaValue.cpp`**
   - **Functions Used**: `BN_*` functions for mathematical operations
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - BN functions are unchanged in OpenSSL 3.x
   - **Migration Impact**: None

3. **`source/grapa/GrapaLink.cpp`**
   - **Functions Used**: SSL context and connection management
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - Basic SSL functions are unchanged
   - **Migration Impact**: None

4. **`source/grapa/GrapaNet.cpp`**
   - **Functions Used**: `SSL_CTX_new()`, `SSL_CTX_use_certificate_chain_file()`, `SSL_CTX_use_PrivateKey_file()`, `SSL_new()`, `SSL_set_connect_state()`
   - **Compatibility**: ⚠️ **MINOR CHANGES** - `TLS_client_method()` recommended over `TLSv1_2_client_method()`
   - **Migration Impact**: Minimal - may need to update TLS method selection

5. **`source/grapa/GrapaSystem.cpp`**
   - **Functions Used**: `RAND_*` functions
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - RAND functions are unchanged
   - **Migration Impact**: None

## Build System Changes Required

### 1. OpenSSL Headers Update
- **Current**: `source/openssl/` (OpenSSL 1.1.1 headers)
- **Target**: `source/openssl/` (OpenSSL 3.5.2 headers)
- **Action**: Replace entire header directory

### 2. Static Libraries Update
- **Current**: OpenSSL 1.1.1 static libraries
- **Target**: OpenSSL 3.5.2 static libraries
- **Action**: Build and deploy new libraries to `source/openssl-lib/`

### 3. Build Script Updates
- **Current**: Manual OpenSSL builds
- **Target**: Automated builds via `scripts/build/build_openssl.py`
- **Action**: Use new build script for all platforms

## Migration Risk Assessment

### Low Risk (No Changes Expected)
- **AES Encryption/Decryption**: EVP functions are fully compatible
- **Big Number Operations**: BN functions are unchanged
- **Random Number Generation**: RAND functions are unchanged
- **Basic SSL Context Management**: Core SSL functions are compatible

### Medium Risk (Minor Changes Possible)
- **TLS Method Selection**: May need to update from `TLSv1_2_client_method()` to `TLS_client_method()`
- **Certificate Chain Handling**: Minor API changes possible but unlikely
- **Provider System**: OpenSSL 3.x introduces providers but legacy compatibility maintained

### Mitigation Strategies
1. **Incremental Testing**: Test each component individually
2. **Backward Compatibility**: OpenSSL 3.x maintains legacy API support
3. **Static Linking**: Reduces runtime dependency issues
4. **Comprehensive Regression Testing**: All documented tests must pass

## Success Criteria

### Build Success
- [ ] All Grapa targets build successfully with OpenSSL 3.5.2
- [ ] No compilation errors or warnings related to OpenSSL
- [ ] Static libraries link correctly

### Functional Success
- [ ] All network tests pass with identical results
- [ ] All mathematical tests produce identical results
- [ ] All documentation examples work correctly
- [ ] SSL/TLS connections work with modern protocols

### Performance Success
- [ ] No significant performance degradation
- [ ] Memory usage remains similar
- [ ] Startup time remains similar

## Post-Migration Validation Checklist

### Phase 1: Build Validation
- [ ] Run `python3 scripts/build/build_openssl.py --target mac-arm64 --clean --verify`
- [ ] Verify OpenSSL 3.5.2 libraries are built and deployed
- [ ] Build Grapa with new OpenSSL libraries
- [ ] Test all build targets (executable, static lib, shared lib)

### Phase 2: Functional Testing
- [ ] Re-run all regression tests documented above
- [ ] Compare results with baseline
- [ ] Investigate any differences
- [ ] Fix any issues found

### Phase 3: Integration Testing
- [ ] Test on all supported platforms
- [ ] Verify cross-platform compatibility
- [ ] Test with real-world SSL/TLS scenarios
- [ ] Validate certificate handling

### Phase 4: Performance Testing
- [ ] Benchmark mathematical operations
- [ ] Test SSL/TLS connection performance
- [ ] Monitor memory usage
- [ ] Validate startup performance

## Rollback Plan

If issues are encountered during migration:

1. **Immediate Rollback**: Revert to OpenSSL 1.1.1 libraries and headers
2. **Investigation**: Document specific issues encountered
3. **Fix Development**: Address issues in isolation
4. **Re-testing**: Validate fixes before re-attempting migration

## Next Steps

1. **Complete Baseline Testing**: Run all documented tests and record results
2. **Build OpenSSL 3.5.2**: Use new build script to create libraries
3. **Update Headers**: Replace OpenSSL headers with 3.5.2 version
4. **Test Migration**: Build and test Grapa with OpenSSL 3.5.2
5. **Validate Results**: Compare with baseline and address any issues
