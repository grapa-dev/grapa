# Secret Method Analysis

**Date**: January 2025  
**Status**: Analysis Complete  
**Purpose**: Investigate the `secret()` method and its dependencies for cryptography operations

---

## Overview

The `secret()` method in Grapa is part of the cryptography system and implements **Diffie-Hellman key exchange** and other key derivation protocols. It's not a standalone method but requires proper setup through the `GrapaEncode` class and key generation.

---

## Method Signature

```grapa
obj.secret(key)
```

**Parameters:**
- `key` - A `GrapaEncode` object containing the peer's public key information

**Returns:**
- Shared secret as bytes (or `{"error":-1}` if operation fails)

---

## Implementation Details

### C++ Implementation Location
- **Class**: `GrapaLibraryRuleSecretEvent` in `source/grapa/GrapaLibRule.cpp` (line 12445)
- **Core Logic**: `GrapaEncode::Secret()` method in `source/grapa/GrapaEncode.cpp` (line 2977)

### How It Works

1. **Parameter Processing**: The method expects two parameters:
   - `r1` - The current object (must be a `GrapaEncode` instance)
   - `r2` - The peer's key information

2. **Key Validation**: The method checks if the current object has a valid cryptography context:
   - `mDH` - Diffie-Hellman context
   - `mEC` - Elliptic Curve context  
   - `mRPK` - Raw Public Key context
   - `mPFC` - Pairing-Friendly Curve context

3. **Key Exchange**: Depending on the context type, it performs the appropriate key derivation:
   - **DH (Diffie-Hellman)**: Uses OpenSSL's `DH_compute_key()`
   - **EC (Elliptic Curve)**: Uses OpenSSL's `EVP_PKEY_derive()`
   - **RPK (Raw Public Key)**: Uses OpenSSL's `EVP_PKEY_derive()`
   - **PFC (Pairing-Friendly Curve)**: Uses custom BLS12-381 implementation

---

## Dependencies and Setup

### Required Setup

The `secret()` method requires proper initialization through the `GrapaEncode` class:

```grapa
/* Step 1: Create a GrapaEncode object */
encode_obj = @<encode,{}>;

/* Step 2: Generate keys using one of the supported methods */
dh_keys = encode_obj.genkeys("dh", {"bits": 1024});
ec_keys = encode_obj.genkeys("ec", {"curve": "prime256v1"});
rsa_keys = encode_obj.genkeys("rsa", {"bits": 2048});

/* Step 3: Use the secret() method with peer's public key */
shared_secret = encode_obj.secret(peer_public_key);
```

### Supported Key Types

#### 1. Diffie-Hellman (DH)
```grapa
/* Generate DH keys */
dh_keys = encode_obj.genkeys("dh", {
    "bits": 1024,  /* Key size in bits */
    "g": 2,        /* Generator (2 or 5) */
    "p": null      /* Prime modulus (auto-generated if null) */
});

/* Use secret() with peer's public key */
peer_key = {
    "method": "dh",
    "pub": peer_public_key_bytes
};
shared_secret = encode_obj.secret(peer_key);
```

#### 2. Elliptic Curve (EC)
```grapa
/* Generate EC keys */
ec_keys = encode_obj.genkeys("ec", {
    "curve": "prime256v1"  /* secp224r1 or prime256v1 */
});

/* Use secret() with peer's public key */
peer_key = {
    "method": "ec",
    "curve": "prime256v1",
    "pub": peer_public_key_bytes
};
shared_secret = encode_obj.secret(peer_key);
```

#### 3. RSA (for encryption/decryption, not key exchange)
```grapa
/* Generate RSA keys */
rsa_keys = encode_obj.genkeys("rsa", {
    "bits": 2048,
    "e": 65537
});

/* Note: RSA doesn't use secret() method */
/* Use encode() and decode() methods instead */
```

---

## Why It Causes Segmentation Faults

### Root Cause Analysis

The `secret()` method causes segmentation faults in simple tests because:

1. **Missing Context**: The method expects the current object to be a properly initialized `GrapaEncode` instance with a valid cryptography context (`mDH`, `mEC`, etc.)

2. **Invalid Parameters**: When called on a simple string or number, the `FROM()` method fails to create a valid `GrapaEncode` context

3. **OpenSSL Dependencies**: The method relies on OpenSSL structures that must be properly initialized

4. **Memory Management**: Complex OpenSSL object lifecycle management can cause crashes if not handled correctly

### Safe Usage Pattern

```grapa
/* ✅ CORRECT: Proper setup and usage */
encode_obj = @<encode,{}>;
dh_keys = encode_obj.genkeys("dh", {"bits": 1024});

/* Now secret() can be used safely */
peer_key = {"method": "dh", "pub": some_public_key};
shared_secret = encode_obj.secret(peer_key);

/* ❌ INCORRECT: Direct usage without setup */
"hello".secret("key");  /* This will crash */
42.secret("key");       /* This will crash */
```

---

## Related Methods

### Key Generation Methods
- **`.genkeys(method, params)`** - Generate cryptographic key pairs
  - `"rsa"` - RSA encryption keys
  - `"dh"` - Diffie-Hellman key exchange keys
  - `"ec"` - Elliptic Curve keys

### Encryption/Decryption Methods
- **`.encode(method, data)`** - Encrypt data
- **`.decode(method, data)`** - Decrypt data

### Digital Signature Methods
- **`.sign(method, params)`** - Create digital signatures
- **`.signadd(method, params)`** - Add signatures
- **`.verify(method, data, params)`** - Verify signatures
- **`.verifyrecover(method, params)`** - Recover data from signatures

---

## Testing Strategy

### Safe Test Approach

To test the `secret()` method without crashes:

```grapa
/* Test setup */
encode_obj = @<encode,{}>;

/* Test DH key generation */
dh_keys = encode_obj.genkeys("dh", {"bits": 1024});
dh_keys.echo();  /* Should show DH key structure */

/* Test EC key generation */
ec_keys = encode_obj.genkeys("ec", {"curve": "prime256v1"});
ec_keys.echo();  /* Should show EC key structure */

/* Test RSA key generation */
rsa_keys = encode_obj.genkeys("rsa", {"bits": 2048});
rsa_keys.echo();  /* Should show RSA key structure */
```

### Comprehensive Test File

Create a dedicated test file for cryptography methods:

```grapa
/* test/use_cases/cryptography_test.grc */
("Testing cryptography methods...").echo();

/* Setup */
encode_obj = @<encode,{}>;

/* Test key generation */
("=== Testing Key Generation ===").echo();
dh_keys = encode_obj.genkeys("dh", {"bits": 1024});
ec_keys = encode_obj.genkeys("ec", {"curve": "prime256v1"});
rsa_keys = encode_obj.genkeys("rsa", {"bits": 2048});

("DH keys generated: " + (dh_keys != null)).echo();
("EC keys generated: " + (ec_keys != null)).echo();
("RSA keys generated: " + (rsa_keys != null)).echo();

/* Test secret() method with proper setup */
("=== Testing Secret Method ===").echo();
/* Note: Would need two parties for actual key exchange */
```

---

## Recommendations

### 1. Documentation Updates
- Add comprehensive documentation for cryptography methods
- Include proper usage patterns and examples
- Document the required setup steps

### 2. Error Handling
- Improve error handling in the C++ implementation
- Return meaningful error messages instead of crashing
- Add validation for required context and parameters

### 3. Testing Strategy
- Create dedicated test files for cryptography methods
- Test with proper setup and valid parameters
- Include integration tests for key exchange scenarios

### 4. User Experience
- Consider adding helper functions for common cryptography operations
- Provide clear examples of proper usage patterns
- Add validation to prevent crashes on invalid usage

---

## Conclusion

The `secret()` method is a sophisticated cryptography implementation that requires proper setup and understanding. It's not suitable for simple testing without the correct context and parameters. The segmentation faults occur because the method expects a fully initialized `GrapaEncode` object with valid OpenSSL contexts, not simple data types.

For safe testing and usage, always:
1. Create a `GrapaEncode` object first
2. Generate appropriate keys using `.genkeys()`
3. Use `.secret()` only with properly formatted peer key information
4. Handle the complex key exchange protocols appropriately

This method is part of Grapa's enterprise-grade cryptography capabilities and should be used with proper understanding of the underlying cryptographic protocols.

