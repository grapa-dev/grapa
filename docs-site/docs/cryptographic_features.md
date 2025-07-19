# Cryptographic Features

Grapa provides a comprehensive set of cryptographic primitives and mathematical operations essential for security applications, data integrity, and cryptographic protocols.

## Overview

Grapa's cryptographic capabilities are built on well-established standards and algorithms, providing:

- **Hash Functions**: SHA3 family and SHAKE extensible output functions
- **Encryption**: AES-256 symmetric encryption
- **Modular Arithmetic**: Essential for public-key cryptography
- **Prime Number Operations**: Prime testing and generation
- **Large Number Support**: Arbitrary precision integer arithmetic
- **Unsigned Integer Handling**: Critical for cryptographic operations

## Encryption

### AES-256 Symmetric Encryption

Grapa provides AES-256 encryption and decryption using CBC mode with configurable initialization vectors (IV).

#### Encryption
```grapa
data = "Hello, World!"
key = "my_secret_key_32_bytes_long_key!"
iv = "initialization_vector_16_bytes"
encrypted = data.encrypt("AES256", {
    "key": key,
    "iv": iv
})
// Returns encrypted data as raw bytes
```

#### Decryption
```grapa
decrypted = encrypted.decrypt("AES256", {
    "key": key,
    "iv": iv
})
// Returns original data
```

#### Key and IV Requirements
- **Key**: Must be exactly 32 bytes (256 bits)
- **IV**: Must be exactly 16 bytes (128 bits)
- **Mode**: CBC (Cipher Block Chaining)
- **Padding**: PKCS7 padding is automatically applied

#### Security Considerations
- **Key management**: Store keys securely, never in plaintext
- **IV uniqueness**: Use a unique IV for each encryption operation
- **Key derivation**: Use hash functions to derive keys from passwords
- **Random generation**: Use cryptographically secure random number generators

#### Example: Secure File Encryption
```grapa
// Generate a random key and IV
key = random_bytes(32)
iv = random_bytes(16)

// Encrypt file content
file_content = $file("sensitive.txt").read()
encrypted = file_content.encrypt("AES256", {
    "key": key,
    "iv": iv
})

// Save encrypted data
$file("sensitive.txt.enc").write(encrypted)

// Decrypt when needed
decrypted = encrypted.decrypt("AES256", {
    "key": key,
    "iv": iv
})
```

## Hash Functions

### SHA3 Family

Grapa implements the complete SHA3 family of hash functions as standardized in FIPS 202.

#### SHA3-224
```grapa
data = "Hello, World!"
hash = data.hash("SHA3-224")
// Returns 28-byte hash
```

#### SHA3-256
```grapa
data = "Hello, World!"
hash = data.hash("SHA3-256")
// Returns 32-byte hash
```

#### SHA3-384
```grapa
data = "Hello, World!"
hash = data.hash("SHA3-384")
// Returns 48-byte hash
```

#### SHA3-512
```grapa
data = "Hello, World!"
hash = data.hash("SHA3-512")
// Returns 64-byte hash
```

### SHAKE Extensible Output Functions

SHAKE functions allow variable-length output, useful for key derivation and random number generation.

#### SHAKE128
```grapa
data = "Hello, World!"
hash = data.hash("SHAKE128")
// Returns 32-byte hash by default

// Custom length output
hash = data.hash("SHAKE128", 64)  // 64-byte output
```

#### SHAKE256
```grapa
data = "Hello, World!"
hash = data.hash("SHAKE256")
// Returns 64-byte hash by default

// Custom length output
hash = data.hash("SHAKE256", 128)  // 128-byte output
```

### Hash Function Properties

- **Cryptographic strength**: All hash functions provide collision resistance
- **Deterministic**: Same input always produces same output
- **Avalanche effect**: Small input changes cause large output changes
- **Fixed output size**: SHA3 family has fixed output sizes
- **Variable output size**: SHAKE functions support custom output lengths

## Modular Arithmetic

Modular arithmetic is fundamental to public-key cryptography and many cryptographic protocols.

### Modular Exponentiation

```grapa
base = 7
exponent = 13
modulus = 23
result = modpow(base, exponent, modulus)
// result = 7^13 mod 23 = 10
```

**Use cases:**
- RSA encryption/decryption
- Diffie-Hellman key exchange
- Digital signatures

### Modular Multiplicative Inverse

```grapa
value = 7
modulus = 23
inverse = modinv(value, modulus)
// inverse = 7^(-1) mod 23 = 10
// Verification: (7 * 10) mod 23 = 1
```

**Use cases:**
- RSA private key calculation
- Solving linear congruences
- Cryptographic protocol implementations

### Greatest Common Divisor

```grapa
a = 48
b = 18
result = gcd(a, b)
// result = 6
```

**Use cases:**
- Coprime testing
- RSA key generation
- Number theory applications

## Prime Number Operations

### Prime Testing

Grapa provides multiple probabilistic prime testing algorithms:

#### Fermat Test
```grapa
number = 17
is_prime = number.isPrime()  // Uses Fermat test
// Returns true for prime numbers
```

#### Rabin-Miller Test
```grapa
number = 17
is_prime = number.isPrime()  // Uses Rabin-Miller test
// More reliable than Fermat test
```

#### Solovay-Strassen Test
```grapa
number = 17
is_prime = number.isPrime()  // Uses Solovay-Strassen test
// Alternative probabilistic test
```

### Prime Generation

```grapa
// Generate a 256-bit prime
prime = generatePrime(256)
// Returns a probable prime of specified bit length
```

### Coprime Generation

```grapa
modulus = 60
coprime = generateCoprime(modulus)
// Returns a number coprime to modulus
```

## Large Number Support

Grapa provides arbitrary precision integer arithmetic, essential for cryptographic operations.

### Basic Operations

```grapa
// Large number arithmetic
a = 123456789012345678901234567890
b = 987654321098765432109876543210
sum = a + b
product = a * b
power = a ^ 3
```

### Bit Operations

```grapa
number = 42
bit_count = number.bitCount()
// Returns number of bits needed to represent the number

// Test specific bits
is_set = number.testBit(5)
// Returns true if bit 5 is set

// Set/unset bits
number.setBit(10)
number.unsetBit(5)
```

### Base Conversion

```grapa
number = 255
hex_str = number.hex()
// Returns "ff"

bin_str = number.bin()
// Returns "11111111"

oct_str = number.oct()
// Returns "377"
```

## Unsigned Integer Handling

Cryptographic operations often require unsigned integer arithmetic to avoid sign-related issues.

### Unsigned Methods

```grapa
number = -42

// Convert to unsigned representations
unsigned_raw = number.uraw()
unsigned_int = number.uint()
unsigned_hex = number.uhex()
unsigned_bin = number.ubin()
```

### Raw Byte Manipulation

```grapa
number = 255
raw_bytes = number.raw(1)
// Returns unsigned byte representation

// For cryptographic operations
hash_input = number.raw(1).hash("SHA3-256")
```

## Performance Considerations

### Hash Functions
- **SHA3 family**: Fixed output size, optimized for security
- **SHAKE functions**: Variable output, useful for key derivation
- **Performance**: SHAKE128 is fastest, SHA3-512 is slowest

### Modular Arithmetic
- **modPow**: Uses efficient algorithms (square-and-multiply)
- **modInverse**: Uses extended Euclidean algorithm
- **Large numbers**: Arbitrary precision with optimized algorithms

### Prime Operations
- **Testing**: Probabilistic tests provide high confidence
- **Generation**: Uses efficient sieving and testing algorithms
- **Confidence levels**: Configurable for different security requirements

## Security Best Practices

### Hash Functions
```grapa
// Use appropriate hash function for your use case
password_hash = password.hash("SHA3-256")
key_derivation = salt.hash("SHAKE256", 32)
```

### Modular Arithmetic
```grapa
// Use large, secure parameters for cryptographic operations
secure_modulus = generatePrime(2048)
secure_exponent = generateCoprime(secure_modulus - 1)
```

### Random Number Generation
```grapa
// Use cryptographically secure random numbers
random_prime = generatePrime(256)
random_coprime = generateCoprime(modulus)
```

## Examples

### RSA Key Generation (Simplified)
```grapa
// Generate two large primes
p = generatePrime(1024)
q = generatePrime(1024)
n = p * q

// Choose public exponent
e = 65537

// Calculate private exponent
phi = (p - 1) * (q - 1)
d = modinv(e, phi)

// Public key: (n, e)
// Private key: (n, d)
```

### Digital Signature Verification
```grapa
message = "Hello, World!"
signature = message_hash.modPow(private_key, modulus)
verification = signature.modPow(public_exponent, modulus)
is_valid = verification == message_hash
```

### Key Derivation
```grapa
password = "my_password"
salt = random_bytes(32)
derived_key = (password + salt).hash("SHAKE256", 32)
```

## Error Handling

Cryptographic operations can fail for various reasons:

```grapa
// Handle modular inverse failures
try {
    inverse = modinv(value, modulus)
} catch {
    // Inverse doesn't exist (gcd(value, modulus) != 1)
    print("Modular inverse does not exist")
}

// Handle prime generation timeouts
try {
    prime = generatePrime(2048)
} catch {
    print("Prime generation took too long")
}
```

## Integration with Other Features

### Database Operations
```grapa
// Store hashed passwords in database
user_table = $TABLE("users")
user_table.insert({
    "username": "alice",
    "password_hash": password.hash("SHA3-256")
})
```

### File System Operations
```grapa
// Verify file integrity
file_content = $file("document.txt").read()
file_hash = file_content.hash("SHA3-256")
expected_hash = "expected_hash_value"
is_valid = file_hash == expected_hash
```

### Network Operations
```grapa
// Secure communication
message = "sensitive_data"
encrypted = message.modPow(public_key, modulus)
// Send encrypted message over network
```

This documentation covers the fundamental cryptographic capabilities in Grapa, providing the building blocks for secure applications while avoiding experimental features that are still in development. 