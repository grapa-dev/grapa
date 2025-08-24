# Network Tests

This directory contains regression tests for Grapa's networking functionality, particularly focused on SSL/TLS and HTTP/HTTPS features.

## Test Files

### **curl_function_test.grc**
- **Purpose**: Tests the comprehensive curl function implementation
- **Scope**: HTTP/HTTPS requests, SSL certificates, proxy support, request bodies
- **Status**: Regression test for curl functionality

### **curl_function_optimized_test.grc**
- **Purpose**: Tests the optimized curl function using maximum C++ capabilities
- **Scope**: Performance testing, C++ integration validation
- **Status**: Performance regression test

### **openssl_compatibility_test.grc** 🔥 **CRITICAL**
- **Purpose**: **OpenSSL 3.0 Compatibility Regression Test**
- **Scope**: Comprehensive SSL/TLS testing for OpenSSL upgrade
- **Critical Tests**:
  - Certificate loading and validation
  - SSL verification and chain validation
  - HTTPS communication
  - Client certificate authentication
  - Proxy with SSL
  - SSL/TLS protocol versions
  - Error handling
  - Performance and memory usage
  - OpenSSL 3.0 specific features

## OpenSSL 3.0 Upgrade Testing

**IMPORTANT**: The `openssl_compatibility_test.grc` file is critical for the OpenSSL 1.1 → 3.0 upgrade.

### **Before OpenSSL 3.0 Upgrade:**
1. Run `./grapa test/network/openssl_compatibility_test.grc`
2. Document all test results and expected behaviors
3. Note any failures or warnings

### **After OpenSSL 3.0 Upgrade:**
1. Run `./grapa test/network/openssl_compatibility_test.grc`
2. Compare results with pre-upgrade baseline
3. Investigate any changes in behavior
4. Fix any regressions before release

### **Key Areas to Monitor:**
- **Certificate Loading**: Client certificates, private keys, trusted CAs
- **SSL Verification**: Certificate chain validation, hostname verification
- **HTTPS Communication**: Request/response handling, content type detection
- **Protocol Support**: TLS 1.2, TLS 1.3 compatibility
- **Error Handling**: Invalid certificates, connection failures
- **Performance**: Memory usage, connection speed

## Running Tests

```bash
# Run all network tests
./grapa test/network/openssl_compatibility_test.grc
./grapa test/network/curl_function_test.grc
./grapa test/network/curl_function_optimized_test.grc

# Run specific test sections
./grapa -c "load('test/network/openssl_compatibility_test.grc'); test_ssl_verification();"
```

## Test Dependencies

These tests require the following files in the `keys/` directory:
- `client.crt` - Client certificate
- `client.key` - Client private key  
- `ca.crt` - Certificate authority certificate
- `server.crt` - Server certificate (for HTTPS server tests)
- `server.key` - Server private key

## Expected Results

### **OpenSSL Compatibility Test Expected Output:**
```
Starting OpenSSL 3.0 Compatibility Regression Tests...

=== Test 1: Certificate Loading ===
Certificate loading result: 0
Private key loading result: 0
Trusted CA loading result: 0
✓ Certificate loading tests completed

=== Test 2: SSL Verification ===
Connection result: 0
SSL verification error: 0
SSL verification certificate: 1234 bytes
Certificate chain length: 2
Host info: {"url":"httpbin.org:443","host":"httpbin.org",...}
✓ SSL verification tests completed

[... additional test results ...]

=== All OpenSSL 3.0 Compatibility Tests Completed ===
```

## Troubleshooting

### **Common Issues:**
1. **Certificate not found**: Ensure `keys/` directory contains required certificates
2. **Connection failures**: Check network connectivity and firewall settings
3. **SSL verification errors**: May indicate OpenSSL 3.0 compatibility issues
4. **Performance degradation**: Monitor for memory leaks or connection pooling issues

### **OpenSSL 3.0 Specific Issues:**
- **Deprecated APIs**: Some OpenSSL 1.1 APIs may be deprecated in 3.0
- **Certificate format changes**: New certificate formats may not be supported
- **Protocol version changes**: TLS 1.3 support may behave differently
- **Memory management**: OpenSSL 3.0 has different memory allocation patterns

## Maintenance

- **Update tests** when new networking features are added
- **Add new test cases** for any SSL/TLS functionality changes
- **Document breaking changes** in OpenSSL compatibility
- **Monitor test performance** to detect regressions
