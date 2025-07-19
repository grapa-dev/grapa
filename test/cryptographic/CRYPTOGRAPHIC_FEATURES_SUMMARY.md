# Grapa Cryptographic Features Summary

## Overview
This document summarizes the cryptographic features available in Grapa, including working functions, experimental features, and proper usage patterns.

## Function Call Chain Tracing

### Hash Functions
- **Grapa Call**: `data.encode("SHA3-256")`
- **Class Definition**: `$OBJ.grc` → `encode` method
- **C++ Implementation**: `GrapaLibRule.cpp` → `GrapaLibraryRuleEncodeEvent::Run()` → `GrapaHash::SHA3_256()`

### Modular Arithmetic
- **Grapa Call**: `base.modpow(exponent, modulus)`
- **Class Definition**: `$math.grc` → `modpow` method
- **C++ Implementation**: `GrapaLibRule.cpp` → `GrapaLibraryRuleModPowEvent::Run()` → `GrapaInt::modPow()`

### Prime Number Functions
- **Grapa Call**: `bits.genprime()` or `number.isprime()`
- **Class Definition**: `$math.grc` → `genprime` and `isprime` methods
- **C++ Implementation**: `GrapaLibRule.cpp` → `GrapaLibraryRuleGenPrimeEvent::Run()` → `GrapaPrime::GenPrime()`

### Encryption
- **Grapa Call**: `data.encode("AES256", params)`
- **Class Definition**: `$OBJ.grc` → `encode` method
- **C++ Implementation**: `GrapaLibRule.cpp` → `GrapaLibraryRuleEncodeEvent::Run()` → `GrapaTinyAES::Encrypt()`

## Working Cryptographic Features

### 1. Hash Functions (SHA3 Family)
All hash functions work correctly and return proper byte lengths:

- **SHA3-224**: 28 bytes
- **SHA3-256**: 32 bytes  
- **SHA3-384**: 48 bytes
- **SHA3-512**: 64 bytes
- **SHAKE128**: 32 bytes (default)
- **SHAKE256**: 64 bytes (default)

**Usage**:
```grapa
hash_result = data.encode("SHA3-256");
hash_hex = hash_result.hex();
hash_length = hash_result.len();
```

### 2. Modular Arithmetic
All modular arithmetic functions work correctly:

- **modpow(base, exponent, modulus)**: Modular exponentiation
- **modinv(value, modulus)**: Modular multiplicative inverse
- **gcd(a, b)**: Greatest common divisor

**Usage**:
```grapa
result = base.modpow(exponent, modulus);
inverse = value.modinv(modulus);
gcd_result = a.gcd(b);
```

### 3. Prime Number Functions
Prime number generation and testing work correctly:

- **genprime(bits, safe=0)**: Generate a prime number with specified bit length
- **isprime(number, safe=0)**: Test if a number is prime
- **staticprime(bits)**: Generate a static prime (experimental)
- **isaks(number)**: AKS primality test (experimental)

**Usage**:
```grapa
prime = 256.genprime();        // Generate 256-bit prime
is_prime = 17.isprime();       // Test if 17 is prime
safe_prime = 128.genprime(1);  // Generate 128-bit safe prime
```

**Test Results**:
- 8-bit prime: 211 ✓
- 16-bit prime: 65257 ✓
- 32-bit prime: 3569154599 ✓
- 64-bit prime: 18140857346318911381 ✓

### 4. Data Conversion Methods
Essential for cryptographic operations:

- **`.raw()`**: Get raw bytes
- **`.uraw()`**: Get unsigned raw bytes
- **`.hex()`**: Convert to hexadecimal
- **`.bin()`**: Convert to binary
- **`.ubin()`**: Convert to unsigned binary
- **`.uint()`**: Convert to unsigned integer
- **`.upper()`**: Convert to uppercase
- **`.lower()`**: Convert to lowercase

**Usage**:
```grapa
raw_data = data.raw();
hex_data = data.hex().lower();  // For case-insensitive comparison
binary_data = data.bin();
```

### 5. Bit Operations
Useful for cryptographic algorithms:

- **`.setbit(bit_position)`**: Set a bit to 1
- **`.clearbit(bit_position)`**: Clear a bit to 0

**Usage**:
```grapa
modified = number.setbit(0);
cleared = number.clearbit(1);
```

### 6. Time Measurement
Proper time measurement for performance testing:

- **`$TIME().utc()`**: Get current UTC time in nanoseconds
- **Time arithmetic**: Subtract times to get elapsed duration

**Usage**:
```grapa
start_time = $TIME().utc();
// ... perform operations ...
end_time = $TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
```

## Experimental Features (Use with Caution)

### 1. Bit Generation
- **Function**: `genbits()`
- **Status**: Partially working
- **Issue**: Not actually random - creates number with specified bits set to 1
- **Usage**: `8.genbits()` returns 255 (8 bits set)

### 2. Key Generation
- **Function**: `genkeys()`
- **Status**: Experimental
- **Issue**: Requires specific parameter format
- **Usage**: Not recommended for production

### 3. Digital Signatures
- **Functions**: `sign()`, `verify()`, `signadd()`, `verifyrecover()`
- **Status**: Experimental
- **Issue**: Requires specific key parameter format
- **Usage**: Not recommended for production

### 4. Secret Operations
- **Function**: `secret()`
- **Status**: Experimental
- **Issue**: Causes segmentation fault
- **Usage**: **AVOID** - crashes the interpreter

### 5. Static Prime Generation
- **Function**: `staticprime()`
- **Status**: Experimental
- **Issue**: Returns bit size instead of prime number
- **Usage**: Not recommended for production

### 6. AKS Primality Test
- **Function**: `isaks()`
- **Status**: Experimental
- **Issue**: Returns errors for all inputs
- **Usage**: Not recommended for production

## Proper Grapa Syntax for Cryptographic Tests

### 1. String Literals
```grapa
// Correct
"string".echo();
("string " + variable.str()).echo();

// Incorrect
("string").echo();
```

### 2. Comments
```grapa
// Correct
/* Block comment */

// Incorrect
# Line comment
```

### 3. Method Calls
```grapa
// Correct
result = data.encode("SHA3-256");
hash_hex = result.hex().lower();

// Incorrect
result = encode(data, "SHA3-256");
```

## Test Results Summary

### ✅ Working Features
1. **Hash Functions**: All SHA3 family and SHAKE functions work correctly
2. **Modular Arithmetic**: modpow, modinv, gcd all work correctly
3. **Prime Number Functions**: genprime, isprime work correctly
4. **Data Conversions**: raw, hex, bin, uint, upper, lower all work
5. **Bit Operations**: setbit, clearbit work correctly
6. **Time Measurement**: $TIME().utc() works correctly
7. **RSA Simulation**: Key generation, encryption, decryption work
8. **Diffie-Hellman**: Key exchange simulation works correctly

### ⚠️ Partially Working Features
1. **Bit Generation**: genbits() works but not random
2. **Performance**: All operations complete successfully

### ❌ Non-Working Features
1. **Secret Operations**: secret() causes segmentation fault
2. **Digital Signatures**: sign(), verify() require specific parameters
3. **Key Generation**: genkeys() requires specific parameters
4. **Static Prime**: staticprime() returns bit size instead of prime
5. **AKS Test**: isaks() returns errors for all inputs

## Recommendations

### For Production Use
1. **Use**: Hash functions, modular arithmetic, prime generation, data conversions
2. **Avoid**: Experimental features (sign, verify, secret, genkeys, staticprime, isaks)
3. **Test**: Always test experimental features before use

### For Testing
1. **Use proper time measurement**: `$TIME().utc()`
2. **Handle case sensitivity**: Use `.lower()` for hash comparisons
3. **Use proper data conversions**: `.raw()`, `.hex()`, `.bin()` for crypto operations
4. **Test edge cases**: Zero values, large numbers, coprime numbers
5. **Test prime generation**: Use larger bit sizes (64+ bits) for reliable results

### For Documentation
1. **Document working features** with examples
2. **Mark experimental features** clearly
3. **Provide parameter formats** for complex functions
4. **Include error handling** examples

## File Locations

### Test Files
- `test/cryptographic/test_simple.grc`: Basic syntax verification
- `test/cryptographic/test_hash_functions.grc`: Hash function tests
- `test/cryptographic/test_modular_arithmetic.grc`: Modular arithmetic tests
- `test/cryptographic/test_prime_functions.grc`: Prime number function tests
- `test/cryptographic/test_prime_simple.grc`: Simple prime number debug test
- `test/cryptographic/test_enhanced_crypto_safe.grc`: Advanced features (safe)

### Source Code
- `lib/grapa/$OBJ.grc`: Base class with encode/decode methods
- `lib/grapa/$math.grc`: Mathematical functions including modpow, modinv, gcd, genprime, isprime
- `source/grapa/GrapaLibRule.cpp`: C++ implementation routing
- `source/grapa/GrapaHash.cpp`: Hash function implementations
- `source/grapa/GrapaPrime.cpp`: Prime number implementations

## Conclusion

Grapa provides a solid foundation for cryptographic operations with working hash functions, modular arithmetic, prime number generation, and data conversion methods. The experimental features should be used with caution and proper testing. The function call chain is well-defined and traceable from Grapa scripts to C++ implementations.

**Key Discovery**: The `genprime()` and `isprime()` functions are fully functional and can be used for cryptographic applications like RSA key generation. 