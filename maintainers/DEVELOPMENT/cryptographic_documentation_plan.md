# Cryptographic Documentation Plan

## Overview

This document outlines where and how to document Grapa's working cryptographic features. Based on the analysis, Grapa has several production-ready cryptographic functions that should be properly documented for users.

## Working Features to Document

### ✅ **Core Cryptographic Functions**

#### 1. Prime Number Operations
- **`genprime(bits, safe=0)`**: Generate random primes using OpenSSL
- **`isprime(number, safe=0)`**: Test primality using OpenSSL
- **`staticprime(bits)`**: Get predefined primes for testing (specific bit sizes)

#### 2. Modular Arithmetic
- **`modpow(base, exponent, modulus)`**: Modular exponentiation
- **`modinv(value, modulus)`**: Modular multiplicative inverse
- **`gcd(a, b)`**: Greatest common divisor

#### 3. Hash Functions
- **`data.encode("SHA3-224")`**: SHA3-224 hash (28 bytes)
- **`data.encode("SHA3-256")`**: SHA3-256 hash (32 bytes)
- **`data.encode("SHA3-384")`**: SHA3-384 hash (48 bytes)
- **`data.encode("SHA3-512")`**: SHA3-512 hash (64 bytes)
- **`data.encode("SHAKE128")`**: SHAKE128 hash (32 bytes default)
- **`data.encode("SHAKE256")`**: SHAKE256 hash (64 bytes default)

#### 4. Data Conversion Methods
- **`.raw()`**: Get raw bytes
- **`.hex()`**: Convert to hexadecimal
- **`.bin()`**: Convert to binary
- **`.uint()`**: Convert to unsigned integer
- **`.upper()`**, **`.lower()`**: Case conversion

## Documentation Locations

### 1. **USE_CASES.md Section 3: High-Precision Math & Scientific Computing**

**Current Status**: Has basic cryptographic examples but needs enhancement

**Proposed Additions**:

```markdown
### Cryptographic Applications

#### RSA Key Generation and Encryption
```grapa
/* Generate RSA keys using Grapa's prime functions */
generate_rsa_keys = op(bits) {
    p = bits.genprime();
    q = bits.genprime();
    n = p * q;
    phi = (p - 1) * (q - 1);
    e = 65537;  /* Common public exponent */
    d = e.modinv(phi);
    {
        "public_key": {"n": n, "e": e},
        "private_key": {"n": n, "d": d}
    };
};

/* RSA encryption/decryption */
rsa_encrypt = op(message, public_key) {
    message.modpow(public_key.get("e"), public_key.get("n"));
};

rsa_decrypt = op(ciphertext, private_key) {
    ciphertext.modpow(private_key.get("d"), private_key.get("n"));
};
```

#### Diffie-Hellman Key Exchange
```grapa
/* Diffie-Hellman key exchange */
diffie_hellman = op(field_size, generator) {
    /* Alice's private key */
    alice_private = $random().genbits(256);
    alice_public = generator.modpow(alice_private, field_size);
    
    /* Bob's private key */
    bob_private = $random().genbits(256);
    bob_public = generator.modpow(bob_private, field_size);
    
    /* Shared secret */
    shared_secret_alice = bob_public.modpow(alice_private, field_size);
    shared_secret_bob = alice_public.modpow(bob_private, field_size);
    
    {
        "alice_public": alice_public,
        "bob_public": bob_public,
        "shared_secret": shared_secret_alice
    };
};
```

#### Hash Functions and Data Integrity
```grapa
/* Hash data for integrity verification */
hash_data = op(data) {
    {
        "sha3_256": data.encode("SHA3-256").hex(),
        "sha3_512": data.encode("SHA3-512").hex(),
        "shake128": data.encode("SHAKE128").hex(),
        "shake256": data.encode("SHAKE256").hex()
    };
};

/* Verify data integrity */
verify_integrity = op(data, expected_hash) {
    actual_hash = data.encode("SHA3-256").hex().lower();
    expected_hash.lower() == actual_hash;
};
```
```

### 2. **New Dedicated Page: `docs/docs/cryptography.md`**

**Purpose**: Comprehensive cryptographic reference

**Content Structure**:

```markdown
# Cryptography in Grapa

## Overview
Grapa provides comprehensive cryptographic capabilities through OpenSSL integration and custom implementations. All cryptographic functions are designed for production use with industry-standard security.

## Security Foundation
- **OpenSSL Integration**: Core functions use OpenSSL's battle-tested cryptographic primitives
- **Industry Standards**: Implements NIST-approved algorithms and security practices
- **Unlimited Precision**: Handles arbitrarily large numbers for cryptographic operations

## Prime Number Operations

### Generating Prime Numbers
```grapa
/* Generate a random 256-bit prime */
prime = 256.genprime();

/* Generate a safe prime (p-1)/2 is also prime */
safe_prime = 256.genprime(1);

/* Get a predefined prime for testing */
test_prime = 256.staticprime();
```

### Testing Primality
```grapa
/* Test if a number is prime */
is_prime = 17.isprime();

/* Test with higher confidence */
is_prime = 17.isprime(100);
```

## Modular Arithmetic

### Modular Exponentiation
```grapa
/* Calculate base^exponent mod modulus */
result = base.modpow(exponent, modulus);
```

### Modular Multiplicative Inverse
```grapa
/* Find modular inverse: (value * inverse) mod modulus = 1 */
inverse = value.modinv(modulus);
```

### Greatest Common Divisor
```grapa
/* Find GCD of two numbers */
gcd_result = a.gcd(b);
```

## Hash Functions

### SHA3 Family
```grapa
/* SHA3 hash functions */
sha3_224 = data.encode("SHA3-224");
sha3_256 = data.encode("SHA3-256");
sha3_384 = data.encode("SHA3-384");
sha3_512 = data.encode("SHA3-512");
```

### SHAKE Functions
```grapa
/* SHAKE hash functions */
shake128 = data.encode("SHAKE128");
shake256 = data.encode("SHAKE256");
```

## Data Conversion

### Format Conversion
```grapa
/* Convert between formats */
raw_bytes = data.raw();
hex_string = data.hex();
binary_string = data.bin();
unsigned_int = data.uint();
```

### Case Conversion
```grapa
/* Convert case for consistent comparison */
lowercase = data.lower();
uppercase = data.upper();
```

## Practical Examples

### RSA Cryptography
[Complete RSA implementation example]

### Diffie-Hellman Key Exchange
[Complete Diffie-Hellman example]

### Digital Signatures
[Digital signature example using hash functions]

### Data Integrity Verification
[Hash-based integrity checking example]

## Security Best Practices

### Key Generation
- Use sufficient key sizes (2048+ bits for RSA)
- Generate fresh random primes for each key pair
- Verify primality of generated numbers

### Hash Functions
- Use SHA3-256 or SHA3-512 for general hashing
- Use SHAKE for variable-length output
- Always verify hash comparisons case-insensitively

### Modular Arithmetic
- Ensure modulus is prime for multiplicative inverse
- Use appropriate confidence levels for primality testing
- Validate input parameters

## Performance Considerations

### Large Number Operations
- Prime generation scales with bit size
- Modular arithmetic is optimized for large numbers
- Hash functions have consistent performance

### Memory Usage
- Unlimited precision arithmetic uses dynamic memory
- Large prime generation may be memory-intensive
- Consider memory constraints for very large operations

## OpenSSL Integration

### Benefits
- Industry-standard cryptographic primitives
- Regular security updates and patches
- Extensive testing and validation
- High performance optimized implementations

### Functions Using OpenSSL
- `genprime()`: Uses `BN_generate_prime_ex()`
- `isprime()`: Uses `BN_is_prime_ex()`
- All modular arithmetic operations

## Error Handling

### Common Errors
- Invalid parameters (negative numbers, zero modulus)
- Memory allocation failures for large numbers
- Timeout for very large prime generation

### Best Practices
- Always check return values
- Use try-catch blocks for error handling
- Validate input parameters before operations

## References

- [OpenSSL Documentation](https://www.openssl.org/docs/)
- [NIST Cryptographic Standards](https://www.nist.gov/cryptography)
- [RFC 8017 - RSA Cryptography](https://tools.ietf.org/html/rfc8017)
- [SHA3 Standard](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)
```

### 3. **API Reference Updates**

**Location**: `docs/docs/API_REFERENCE.md`

**Additions**:
- Complete function signatures for all cryptographic functions
- Parameter descriptions and return types
- Usage examples for each function
- Error conditions and handling

### 4. **Examples Page Updates**

**Location**: `docs/docs/EXAMPLES.md`

**Additions**:
- Cryptographic examples section
- Complete working examples
- Step-by-step explanations
- Best practices demonstrations

## Implementation Priority

### **Phase 1: Core Documentation (High Priority)**
1. Update USE_CASES.md Section 3 with cryptographic examples
2. Create `docs/docs/cryptography.md` comprehensive reference
3. Add cryptographic functions to API reference

### **Phase 2: Enhanced Examples (Medium Priority)**
1. Add comprehensive examples to EXAMPLES.md
2. Create interactive tutorials
3. Add performance benchmarks

### **Phase 3: Advanced Topics (Low Priority)**
1. Add advanced cryptographic protocols
2. Include security analysis and recommendations
3. Create educational content

## Success Metrics

### **Documentation Quality**
- Complete function coverage
- Clear examples and explanations
- Proper error handling documentation
- Security best practices included

### **User Experience**
- Easy-to-find cryptographic functions
- Working examples that users can copy-paste
- Clear guidance on when to use each function
- Proper security recommendations

### **Technical Accuracy**
- Correct function signatures
- Accurate parameter descriptions
- Proper OpenSSL integration documentation
- Security considerations addressed

## Conclusion

Grapa's cryptographic features provide a solid foundation for cryptographic applications. The OpenSSL integration ensures industry-standard security, while the unlimited precision arithmetic enables complex cryptographic operations. Proper documentation will make these features accessible to users and demonstrate Grapa's capabilities in the cryptographic domain.

The documentation should emphasize the production-ready nature of the core functions while clearly marking experimental features that need additional work. This will help users understand what they can safely use in production applications versus what is still under development. 