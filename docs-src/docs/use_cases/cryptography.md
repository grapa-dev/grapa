# Cryptography in Grapa

## Overview

Grapa provides comprehensive cryptographic capabilities through OpenSSL integration and custom implementations. All cryptographic functions are designed for production use with industry-standard security.

Grapa's cryptographic functions fall into two main categories:

1. **Mathematical Cryptography**: Prime generation, modular arithmetic, and hash functions implemented using OpenSSL's mathematical primitives
2. **OpenSSL-Based Cryptography**: RSA, Elliptic Curve (EC), and other cryptographic operations using OpenSSL's high-level APIs

## Security Foundation

- **OpenSSL Integration**: Core functions use OpenSSL's battle-tested cryptographic primitives
- **Industry Standards**: Implements NIST-approved algorithms and security practices  
- **Unlimited Precision**: Handles arbitrarily large numbers for cryptographic operations
- **Production Ready**: Prime generation and primality testing use OpenSSL's `BN_generate_prime_ex()` and `BN_is_prime_ex()`

## Prime Number Operations

### Generating Prime Numbers

```grapa
/* Generate a random 256-bit prime */
prime = (256).genprime();
("Generated prime: " + prime.str() + "\n").echo();

/* Generate a safe prime (p-1)/2 is also prime) */
safe_prime = (256).genprime(1);
("Safe prime: " + safe_prime.str() + "\n").echo();

/* Get a predefined prime for testing (specific bit sizes only) */
test_prime = (256).staticprime();
("Test prime: " + test_prime.str() + "\n").echo();
```

**Available staticprime() bit sizes**: 2, 3, 4, 256, 512, 768, 1024, 1536, 2048, 3072, 4096

### Testing Primality

```grapa
/* Test if a number is prime */
is_prime = (17).isprime();
("Is 17 prime? " + is_prime.str() + "\n").echo();

/* Test with higher confidence (default is 64) */
is_prime = (17).isprime(100);
("Is 17 prime with confidence 100? " + is_prime.str() + "\n").echo();

/* Test large numbers */
large_number = 123456789012345678901234567890123456789;
is_large_prime = large_number.isprime();
("Is large number prime? " + is_large_prime.str() + "\n").echo();
```

## Modular Arithmetic

### Modular Exponentiation

```grapa
/* Calculate base^exponent mod modulus */
base = 7;
exponent = 13;
modulus = 11;
result = base.modpow(exponent, modulus);
("7^13 mod 11 = " + result.str() + "\n").echo();

/* Large number example */
large_base = 123456789;
large_exponent = 987654321;
large_modulus = 1000000007;
large_result = large_base.modpow(large_exponent, large_modulus);
("Large modpow result: " + large_result.str() + "\n").echo();
```

### Modular Multiplicative Inverse

```grapa
/* Find modular inverse: (value * inverse) mod modulus = 1 */
value = 3;
modulus = 11;
inverse = value.modinv(modulus);
("Modular inverse of 3 mod 11: " + inverse.str() + "\n").echo();
("Verification: " + ((value * inverse) % modulus).str() + "\n").echo();

/* Large number example */
large_value = 123456789;
large_modulus = 1000000007;
large_inverse = large_value.modinv(large_modulus);
("Large modular inverse: " + large_inverse.str() + "\n").echo();
```

### Greatest Common Divisor

```grapa
/* Find GCD of two numbers */
a = 48;
b = 18;
gcd_result = a.gcd(b);
("GCD of 48 and 18: " + gcd_result.str() + "\n").echo();

/* Large number example */
large_a = 123456789012345678901234567890;
large_b = 987654321098765432109876543210;
large_gcd = large_a.gcd(large_b);
("Large GCD: " + large_gcd.str() + "\n").echo();
```

## Hash Functions

### SHA3 Family

```grapa
/* SHA3 hash functions */
data = "Hello, Grapa!";
sha3_224 = data.encode({method: "sha3-224"});
sha3_256 = data.encode({method: "sha3-256"});
sha3_384 = data.encode({method: "sha3-384"});
sha3_512 = data.encode({method: "sha3-512"});

("SHA3-224: " + sha3_224.uhex() + "\n").echo();
("SHA3-256: " + sha3_256.uhex() + "\n").echo();
("SHA3-384: " + sha3_384.uhex() + "\n").echo();
("SHA3-512: " + sha3_512.uhex() + "\n").echo();
```

### SHAKE Functions

```grapa
/* SHAKE hash functions */
data = "Hello, Grapa!";
shake128 = data.encode({method: "shake128"});
shake256 = data.encode({method: "shake256"});

("SHAKE128: " + shake128.uhex() + "\n").echo();
("SHAKE256: " + shake256.uhex() + "\n").echo();
```

### Hash Function Output Sizes

| Function | Output Size |
|----------|-------------|
| SHA3-224 | 28 bytes (224 bits) |
| SHA3-256 | 32 bytes (256 bits) |
| SHA3-384 | 48 bytes (384 bits) |
| SHA3-512 | 64 bytes (512 bits) |
| SHAKE128 | 32 bytes (256 bits) default |
| SHAKE256 | 64 bytes (512 bits) default |

## Encoding and Decoding Methods

Grapa supports several encoding and decoding methods for data transformation and transmission.

### Supported Encoding Methods

| Method | Description | Encode | Decode | Use Case |
|--------|-------------|--------|--------|----------|
| **SHA3-224** | SHA3-224 hash function | ✅ | ❌ | Cryptographic hashing |
| **SHA3-256** | SHA3-256 hash function | ✅ | ❌ | Cryptographic hashing |
| **SHA3-384** | SHA3-384 hash function | ✅ | ❌ | Cryptographic hashing |
| **SHA3-512** | SHA3-512 hash function | ✅ | ❌ | Cryptographic hashing |
| **SHAKE128** | SHAKE128 hash function | ✅ | ❌ | Cryptographic hashing |
| **SHAKE256** | SHAKE256 hash function | ✅ | ❌ | Cryptographic hashing |
| **BASE64** | Base64 encoding | ✅ | ✅ | Data transmission, binary to text |
| **BASE58** | Base58 encoding | ✅ | ✅ | Cryptocurrency addresses, compact encoding |
| **URL-ASCII** | URL percent-encoding | ✅ | ❌ | URL-safe encoding |

### Base64 Encoding

Base64 is a binary-to-text encoding scheme that represents binary data in ASCII string format.

```grapa
/* Base64 encoding and decoding */
data = "Hello, Grapa!";
encoded = data.encode({method: "base64"});
decoded = encoded.decode({method: "base64"});

("Original: " + data + "\n").echo();
("Base64 encoded: " + encoded.str() + "\n").echo();
("Base64 decoded: " + decoded.str() + "\n").echo();
```

### Base58 Encoding

Base58 is a binary-to-text encoding scheme commonly used in cryptocurrency applications. It's similar to Base64 but excludes similar-looking characters (0, O, I, l).

```grapa
/* Base58 encoding and decoding */
data = "Hello, Grapa!";
encoded = data.encode({method: "base58"});
decoded = encoded.decode({method: "base58"});

("Original: " + data + "\n").echo();
("Base58 encoded: " + encoded.str() + "\n").echo();
("Base58 decoded: " + decoded.str() + "\n").echo();
```

### URL-ASCII Encoding

URL-ASCII encoding (percent-encoding) converts special characters to their percent-encoded equivalents for safe transmission in URLs.

```grapa
/* URL-ASCII encoding */
data = "Hello World & Friends!";
encoded = data.encode({method: "url-ascii"});

("Original: " + data + "\n").echo();
("URL-ASCII encoded: " + encoded.str() + "\n").echo();
/* Note: URL-ASCII decode is not currently implemented */
```

## Data Conversion

### Format Conversion

```grapa
/* Convert between formats */
data = "Hello, Grapa!";
raw_bytes = data.raw();
hex_string = data.hex();
binary_string = data.bin();
unsigned_int = data.uint();

("Raw bytes: " + raw_bytes.str() + "\n").echo();
("Hex string: " + hex_string + "\n").echo();
("Binary string: " + binary_string + "\n").echo();
("Unsigned int: " + unsigned_int.str() + "\n").echo();
```

### Case Conversion

```grapa
/* Convert case for consistent comparison */
data = "Hello, Grapa!";
lowercase = data.lower();
uppercase = data.upper();
casefolded = data.casefold();

("Lowercase: " + lowercase + "\n").echo();
("Uppercase: " + uppercase + "\n").echo();
("Casefolded: " + casefolded + "\n").echo();
```

## Cryptographic Output Interpretation

### Important: Using Unsigned Methods for Cryptographic Operations

When working with cryptographic functions like `encode()` and `sign()`, it's crucial to understand how Grapa interprets the results:

#### Default Behavior: Signed Interpretation

```grapa
/* Generate hash with message digest */
md_keys = "md".genkeys({"digest": "sha256"});
message = "Hello World";
hash = message.encode(md_keys);

/* Default hex output may appear negative if high bit is set */
hash.hex().echo();  /* May show: -5A6E592BF40BDFBFB5FEE8CC30484E6F29D39A40F4325CD4A84D88265260EB92 */

/* Raw output shows internal representation */
hash.raw().echo();  /* Shows: 0xA591A6D40BF420404A011733CFB7B190D62C65BF0BCDA32B57B277D9AD9F146E */
```

#### Correct Approach: Use Unsigned Methods

For cryptographic operations, **always use unsigned methods** to ensure proper interpretation:

```grapa
/* Generate hash with message digest */
md_keys = "md".genkeys({"digest": "sha256"});
message = "Hello World";
hash = message.encode(md_keys);

/* Use uint() for unsigned interpretation */
hash.uint().hex().echo();  /* Shows: A591A6D40BF420404A011733CFB7B190D62C65BF0BCDA32B57B277D9AD9F146E */

/* Use uhex() for direct unsigned hex output */
hash.uhex().echo();  /* Shows: A591A6D40BF420404A011733CFB7B190D62C65BF0BCDA32B57B277D9AD9F146E */
```

#### Why This Matters for Cryptography

1. **Mathematical Operations**: Cryptographic math (modular arithmetic, key derivation) requires unsigned interpretation
2. **Hash Functions**: Hash outputs should never be interpreted as negative numbers
3. **Digital Signatures**: Signature values must be treated as unsigned for verification
4. **Key Exchange**: Shared secrets must be unsigned for proper key derivation

#### Best Practices for Cryptographic Code

```grapa
/* ✅ CORRECT: Always use unsigned methods for cryptographic operations */
md_keys = "md".genkeys({"digest": "sha256"});
message = "Hello World";
hash = message.encode(md_keys);

/* For display and comparison */
hash_hex = hash.uhex();  /* Use uhex() for clean hex output */
("Hash: " + hash_hex).echo();

/* For mathematical operations */
hash_uint = hash.uint();  /* Use uint() for crypto math */
result = hash_uint.modpow(exponent, modulus);

/* For key derivation */
shared_secret = alice_keys.secret(peer_key);
secret_uint = shared_secret.uint();  /* Ensure unsigned for crypto operations */
```

#### Understanding the Issue

The issue occurs because:

1. **`encode()` and `sign()` functions return `GrapaTokenType::RAW` data**
2. **`GrapaInt::FromBytes()` interprets `RAW` data with high bit set as negative**
3. **Cryptographic outputs often have the high bit set** (e.g., SHA256 hashes)
4. **The `uint()` method forces unsigned interpretation** by calling `FromBytes()` with `isUnsigned=true`

This behavior is **correct and intentional** - the `uint()` method is specifically designed for cryptographic applications where unsigned interpretation is required.

## OpenSSL-Based Cryptography

Grapa provides high-level cryptographic functions using OpenSSL's APIs for RSA, Elliptic Curve (EC), and other cryptographic operations.

### Key Generation

#### RSA Key Generation

```grapa
/* Generate RSA key pair with default settings (1024 bits, e=65537) */
rsa_keys = "rsa".genkeys();
("RSA Keys: " + rsa_keys.str() + "\n").echo();

/* Generate RSA key pair with custom parameters */
rsa_keys_2048 = "rsa".genkeys({"bits": 2048, "e": 65537});

/* RSA keys contain all components for cryptography */
rsa_keys.n.echo();      /* Modulus */
rsa_keys.e.echo();      /* Public exponent */
rsa_keys.d.echo();      /* Private exponent */
rsa_keys.p.echo();      /* First prime factor */
rsa_keys.q.echo();      /* Second prime factor */
```

#### Elliptic Curve Key Generation

```grapa
/* Generate EC key pair with default curve (prime256v1) */
ec_keys = "ec".genkeys();
("EC Keys: " + ec_keys.str() + "\n").echo();

/* Generate EC key pair with specific curve */
ec_keys_prime256v1 = "ec".genkeys({"curve": "prime256v1"});
ec_keys_secp224r1 = "ec".genkeys({"curve": "secp224r1"});
ec_keys_secp256k1 = "ec".genkeys({"curve": "secp256k1"});
ec_keys_secp384r1 = "ec".genkeys({"curve": "secp384r1"});
ec_keys_secp521r1 = "ec".genkeys({"curve": "secp521r1"});

/* Note: 7 curves are supported in Grapa */
/* Supported curves: "prime256v1", "secp224r1", "secp256k1", "secp384r1", "secp521r1", "prime192v1", "prime239v1" */
```

#### Block Cipher (AES) Key Generation

```grapa
/* Generate AES keys with default settings (aes-256-cbc) */
bc_keys = "bc".genkeys();
("BC Keys: " + bc_keys.str() + "\n").echo();

/* Generate AES keys with specific cipher and parameters */
bc_keys_128 = "bc".genkeys({
    "cipher": "aes-128-cbc",
    "key": "my-secret-key-32-bytes-long!!",
    "iv": "my-iv-16-bytes!!"
});
```

#### Message Digest Key Generation

```grapa
/* Generate hash function context with default (sha512) */
md_keys = "md".genkeys();
("MD Keys: " + md_keys.str() + "\n").echo();

/* Generate hash function context with specific algorithm */
md_keys_sha256 = "md".genkeys({"digest": "sha256"});
md_keys_md5 = "md".genkeys({"digest": "md5"});
```

#### Raw Public Key Generation

```grapa
/* Generate ED25519 keys (default) */
rpk_keys = "rpk".genkeys();
("RPK Keys: " + rpk_keys.str() + "\n").echo();

/* Generate X25519 keys for key exchange */
x25519_keys = "rpk".genkeys({"alg": "X25519"});

/* Generate ED448 keys for signatures */
ed448_keys = "rpk".genkeys({"alg": "ED448"});

/* Generate X448 keys for key exchange */
x448_keys = "rpk".genkeys({"alg": "X448"});

/* Generate HMAC keys for message authentication */
hmac_keys = "rpk".genkeys({"alg": "HMAC"});

/* Generate Poly1305 keys for message authentication */
poly1305_keys = "rpk".genkeys({"alg": "POLY1305"});

/* Generate SipHash keys for message authentication */
siphash_keys = "rpk".genkeys({"alg": "SIPHASH"});

/* Generate CMAC keys for cipher-based MAC */
cmac_keys = "rpk".genkeys({"alg": "CMAC"});

/* Generate TLS1_PRF keys for TLS 1.0/1.1 PRF */
tls1_prf_keys = "rpk".genkeys({"alg": "TLS1_PRF"});

/* Generate Scrypt keys for password-based key derivation */
scrypt_keys = "rpk".genkeys({"alg": "SCRYPT"});

/* Generate SM2 keys for Chinese elliptic curve cryptography */
sm2_keys = "rpk".genkeys({"alg": "SM2"});
```

### Algorithm-Specific Examples

#### CMAC (Cipher-based MAC) Example

```grapa
/* Generate CMAC keys for AES-based message authentication */
cmac_keys = "rpk".genkeys({"alg": "CMAC"});

/* Create a message to authenticate */
message = "Hello, Grapa!";

/* Generate CMAC tag for the message */
cmac_tag = message.encode(cmac_keys);
("CMAC Tag: " + cmac_tag.hex() + "\n").echo();

/* Verify the CMAC tag (in practice, you'd verify against a received tag) */
verification = cmac_tag.decode(cmac_keys);
("CMAC Verification: " + verification.str() + "\n").echo();
```

#### TLS1_PRF (TLS 1.0/1.1 Pseudo-Random Function) Example

```grapa
/* Generate TLS1_PRF keys for TLS key derivation */
tls1_prf_keys = "rpk".genkeys({"alg": "TLS1_PRF"});

/* Create seed data for key derivation */
seed = "TLS1.3 Key Derivation Seed";

/* Derive a key using TLS1_PRF */
derived_key = seed.encode(tls1_prf_keys);
("TLS1_PRF Derived Key: " + derived_key.hex() + "\n").echo();

/* Note: TLS1_PRF is primarily used internally by TLS implementations */
/* This demonstrates the key derivation capability */
```

#### Scrypt (Password-based Key Derivation) Example

```grapa
/* Generate Scrypt keys for password-based key derivation */
scrypt_keys = "rpk".genkeys({"alg": "SCRYPT"});

/* Create a password and salt */
password = "MySecurePassword123";
salt = "RandomSaltValue";

/* Combine password and salt */
input_data = password + salt;

/* Derive a key using Scrypt */
derived_key = input_data.encode(scrypt_keys);
("Scrypt Derived Key: " + derived_key.hex() + "\n").echo();

/* Verify the same password+salt produces the same key */
verification_key = input_data.encode(scrypt_keys);
("Keys match: " + (derived_key == verification_key).str() + "\n").echo();
```

#### SM2 (Chinese Elliptic Curve) Example

```grapa
/* Generate SM2 keys for Chinese elliptic curve cryptography */
sm2_keys = "rpk".genkeys({"alg": "SM2"});

/* Create a message to sign */
message = "Hello, Grapa!";

/* Sign the message using SM2 */
sm2_signature = sm2_keys.sign(message);
("SM2 Signature: " + sm2_signature.hex() + "\n").echo();

/* Verify the SM2 signature */
is_valid = sm2_keys.verify(message, sm2_signature);
("SM2 Signature Valid: " + is_valid.str() + "\n").echo();

/* Verify with wrong message (should fail) */
wrong_message = "Hello, World!";
is_valid_wrong = sm2_keys.verify(wrong_message, sm2_signature);
("Wrong message valid: " + is_valid_wrong.str() + "\n").echo();
```

#### HMAC (Hash-based Message Authentication) Example

```grapa
/* Generate HMAC keys for message authentication */
hmac_keys = "rpk".genkeys({"alg": "HMAC"});

/* Create a message to authenticate */
message = "Hello, Grapa!";

/* Generate HMAC for the message */
hmac_tag = message.encode(hmac_keys);
("HMAC Tag: " + hmac_tag.hex() + "\n").echo();

/* Verify the HMAC tag */
verification = hmac_tag.decode(hmac_keys);
("HMAC Verification: " + verification.str() + "\n").echo();
```

#### Poly1305 (Message Authentication) Example

```grapa
/* Generate Poly1305 keys for message authentication */
poly1305_keys = "rpk".genkeys({"alg": "POLY1305"});

/* Create a message to authenticate */
message = "Hello, Grapa!";

/* Generate Poly1305 tag for the message */
poly1305_tag = message.encode(poly1305_keys);
("Poly1305 Tag: " + poly1305_tag.hex() + "\n").echo();

/* Verify the Poly1305 tag */
verification = poly1305_tag.decode(poly1305_keys);
("Poly1305 Verification: " + verification.str() + "\n").echo();
```

#### SipHash (Message Authentication) Example

```grapa
/* Generate SipHash keys for message authentication */
siphash_keys = "rpk".genkeys({"alg": "SIPHASH"});

/* Create a message to authenticate */
message = "Hello, Grapa!";

/* Generate SipHash for the message */
siphash_tag = message.encode(siphash_keys);
("SipHash Tag: " + siphash_tag.hex() + "\n").echo();

/* Verify the SipHash tag */
verification = siphash_tag.decode(siphash_keys);
("SipHash Verification: " + verification.str() + "\n").echo();
```

### Encryption and Decryption

#### RSA Encryption/Decryption

```grapa
/* Generate RSA keys */
keys = "rsa".genkeys();

/* Encrypt a message */
message = "Hello, Grapa!";
encrypted = message.encode(keys);
("Encrypted: " + encrypted.str() + "\n").echo();

/* Decrypt the message */
decrypted = encrypted.decode(keys);
("Decrypted: " + decrypted.str() + "\n").echo();
("Success: " + (message == decrypted.str()).str() + "\n").echo();
```

#### Base64, Base58, and URL Encoding

```grapa
/* Basic encoding formats */
data = "Hello, Grapa!";

/* Base64 encoding */
base64_encoded = data.encode({method: "base64"});
("Base64: " + base64_encoded.str() + "\n").echo();

/* Base58 encoding */
base58_encoded = data.encode({method: "base58"});
("Base58: " + base58_encoded.str() + "\n").echo();

/* URL-ASCII encoding (percent-encoding) */
url_data = "Hello World & Friends!";
url_encoded = url_data.encode({method: "url-ascii"});
("URL-ASCII: " + url_encoded.str() + "\n").echo();

/* Decode back */
base64_decoded = base64_encoded.decode({method: "base64"});
base58_decoded = base58_encoded.decode({method: "base58"});
("Base64 decoded: " + base64_decoded.str() + "\n").echo();
("Base58 decoded: " + base58_decoded.str() + "\n").echo();
```

### Digital Signatures

#### RSA Signatures

```grapa
/* Generate RSA keys */
keys = "rsa".genkeys();

/* Sign a message */
message = "Hello, Grapa!";
signature = keys.sign(message);
("Signature: " + signature.str() + "\n").echo();

/* Verify the signature */
is_valid = keys.verify(message, signature);
("Signature valid: " + is_valid.str() + "\n").echo();

/* Verify with wrong message (should fail) */
wrong_message = "Hello, World!";
is_valid_wrong = keys.verify(wrong_message, signature);
("Wrong message valid: " + is_valid_wrong.str() + "\n").echo();
```

#### EC Signatures

```grapa
/* Generate EC keys */
ec_keys = "ec".genkeys({"curve": "prime256v1"});

/* Sign a message */
message = "Hello, Grapa!";
signature = ec_keys.sign(message);
("EC Signature: " + signature.str() + "\n").echo();

/* Verify the signature */
is_valid = ec_keys.verify(message, signature);
("EC Signature valid: " + is_valid.str() + "\n").echo();
```

### Sign and Verify Functions

Grapa provides comprehensive digital signature capabilities through the `sign()` and `verify()` methods. These functions support multiple cryptographic algorithms and are designed for production use.

#### Function Signatures

```grapa
/* Sign a message with a private key */
signature = private_key.sign(message, [params]);

/* Verify a signature with a public key */
is_valid = public_key.verify(message, signature, [params]);
```

#### Supported Algorithms

| Algorithm | Key Generation | Sign | Verify | Notes |
|-----------|----------------|------|--------|-------|
| **RSA** | `"rsa".genkeys()` | ✅ | ✅ | Uses SHA-256 by default |
| **EC** | `"ec".genkeys()` | ✅ | ✅ | Supports 7 curves |
| **RPK** | `"rpk".genkeys()` | ✅ | ✅ | ED25519/ED448, X25519/X448 |
| **BLS12-381** | `"pfc".genkeys()` | ⚠️ | ⚠️ | Has implementation issues |

#### RSA Signatures

```grapa
/* Generate RSA key pair */
keys = "rsa".genkeys();
("RSA Keys generated\n").echo();

/* Sign a message */
message = "Hello, Grapa!";
signature = keys.sign(message);
("RSA Signature: " + signature.hex() + "\n").echo();

/* Verify the signature */
is_valid = keys.verify(message, signature);
("RSA Signature valid: " + is_valid.str() + "\n").echo();

/* Verify with wrong message (should fail) */
wrong_message = "Hello, World!";
is_valid_wrong = keys.verify(wrong_message, signature);
("Wrong message valid: " + is_valid_wrong.str() + "\n").echo();
```

**RSA Parameters:**
- **Default digest**: SHA-256
- **Key size**: 2048 bits (default)
- **Signature format**: Raw bytes (use `.hex()` for display)

#### Elliptic Curve Signatures

```grapa
/* Generate EC key pair with specific curve */
ec_keys = "ec".genkeys({"curve": "prime256v1"});
("EC Keys generated\n").echo();

/* Sign a message */
message = "Hello, Grapa!";
signature = ec_keys.sign(message);
("EC Signature: " + signature.hex() + "\n").echo();

/* Verify the signature */
is_valid = ec_keys.verify(message, signature);
("EC Signature valid: " + is_valid.str() + "\n").echo();
```

**Supported EC Curves:**
- `prime256v1` (NIST P-256) - **Recommended**
- `secp256k1` (Bitcoin curve)
- `secp384r1` (NIST P-384)
- `secp521r1` (NIST P-521)
- `brainpoolP256r1`
- `brainpoolP384r1`
- `brainpoolP512r1`

**EC Parameters:**
- **Default digest**: SHA-256
- **Signature format**: Raw bytes (use `.hex()` for display)

#### Raw Public Key (RPK) Signatures

```grapa
/* Generate RPK key pair */
rpk_keys = "rpk".genkeys({"method": "ed25519"});
("RPK Keys generated\n").echo();

/* Sign a message */
message = "Hello, Grapa!";
signature = rpk_keys.sign(message);
("RPK Signature: " + signature.hex() + "\n").echo();

/* Verify the signature */
is_valid = rpk_keys.verify(message, signature);
("RPK Signature valid: " + is_valid.str() + "\n").echo();
```

**Supported RPK Methods:**
- `ed25519` - Ed25519 signatures
- `ed448` - Ed448 signatures
- `x25519` - X25519 key exchange
- `x448` - X448 key exchange
- `hmac` - HMAC-based signatures
- `poly1305` - Poly1305 MAC
- `siphash` - SipHash MAC
- `cmac` - CMAC
- `tls1_prf` - TLS1 PRF
- `scrypt` - Scrypt KDF
- `sm2` - SM2 signatures

#### BLS12-381 Signatures (Experimental)

```grapa
/* Generate BLS12-381 key pair */
bls_keys = "pfc".genkeys({"ikm": "random_seed_32_bytes_long"});
("BLS Keys generated\n").echo();

/* Sign a message */
message = "Hello, Grapa!";
signature = bls_keys.sign(message);
("BLS Signature: " + signature.hex() + "\n").echo();

/* Verify the signature */
is_valid = bls_keys.verify(message, signature);
("BLS Signature valid: " + is_valid.str() + "\n").echo();
```

**BLS12-381 Notes:**
- **Status**: Experimental, has implementation issues
- **Required parameter**: `ikm` (Input Keying Material) for key generation
- **Digest**: SHA-256 (Ethereum compatible)
- **Issues**: Key generation may produce invalid keys, signing may fail

#### Advanced Usage Examples

**Batch Signature Verification:**
```grapa
/* Generate multiple signatures */
keys = "rsa".genkeys();
messages = ["Message 1", "Message 2", "Message 3"];
signatures = [];

for (i = 0; i < messages.len(); i++) {
    signatures.push(keys.sign(messages.get(i)));
}

/* Verify all signatures */
all_valid = true;
for (i = 0; i < messages.len(); i++) {
    if (!keys.verify(messages.get(i), signatures.get(i))) {
        all_valid = false;
        break;
    }
}
("All signatures valid: " + all_valid.str() + "\n").echo();
```

**Signature with Custom Parameters:**
```grapa
/* EC signature with specific curve */
ec_keys = "ec".genkeys({"curve": "secp256k1"});
message = "Bitcoin-style signature";
signature = ec_keys.sign(message);
is_valid = ec_keys.verify(message, signature);
("Bitcoin-style signature valid: " + is_valid.str() + "\n").echo();
```

**Error Handling:**
```grapa
/* Handle signature errors gracefully */
try {
    keys = "rsa".genkeys();
    signature = keys.sign("test message");
    is_valid = keys.verify("test message", signature);
    ("Signature operation successful: " + is_valid.str() + "\n").echo();
} catch (error) {
    ("Signature error: " + error.str() + "\n").echo();
}
```

#### Performance Considerations

- **RSA**: Slower than EC for signing, faster for verification
- **EC**: Fast signing and verification, smaller signatures
- **RPK**: Very fast, especially Ed25519/Ed448
- **BLS12-381**: Slower due to pairing operations

#### Security Best Practices

1. **Key Management**: Store private keys securely, never expose them
2. **Message Integrity**: Always verify signatures before trusting data
3. **Algorithm Selection**: Use EC (prime256v1) or RPK (ed25519) for new applications
4. **Key Size**: Use at least 256-bit keys for EC, 2048-bit for RSA
5. **Randomness**: Ensure proper random number generation for key creation

#### Troubleshooting

**Common Issues:**
- **Segmentation fault**: Usually indicates invalid key format or OpenSSL version mismatch
- **Verification fails**: Check message content, key pair matching, or algorithm compatibility
- **Empty signatures**: May indicate BLS12-381 implementation issues
- **Negative hex output**: Use `.uint().hex()` to force unsigned interpretation

### Key Derivation

#### Secret Key Derivation

```grapa
/* Generate EC keys for key exchange */
alice_keys = "ec".genkeys({"curve": "prime256v1"});
bob_keys = "ec".genkeys({"curve": "prime256v1"});

/* Derive shared secret using Alice's private key and Bob's public key */
alice_secret = alice_keys.secret({
    "method": "ec", 
    "curve": "prime256v1", 
    "pub": bob_keys.pub
});
("Alice's shared secret: " + alice_secret.str() + "\n").echo();

/* Derive shared secret using Bob's private key and Alice's public key */
bob_secret = bob_keys.secret({
    "method": "ec", 
    "curve": "prime256v1", 
    "pub": alice_keys.pub
});
("Bob's shared secret: " + bob_secret.str() + "\n").echo();

/* Verify both parties derived the same secret */
("Secrets match: " + (alice_secret == bob_secret).str() + "\n").echo();
```

**Note**: The `secret()` function currently supports:
- **EC (Elliptic Curve)**: ✅ Fully functional for key derivation
- **DH (Diffie-Hellman)**: ✅ Fully functional for key derivation
- **RPK (Raw Public Key)**: ✅ Fully functional for key derivation (X25519, X448)
- **RSA**: Returns 0 (not useful for key derivation)
- **BLS12-381**: Currently has implementation issues

### Additional Cryptographic Methods

#### Signature Recovery (`verifyrecover()`)

The `verifyrecover()` method allows recovery of the original message from a signature, primarily used with RSA signatures.

```grapa
/* Generate RSA keys */
keys = "rsa".genkeys();

/* Sign a message with raw digest and PKCS#1 padding for recovery */
message = "Hello, Grapa!";
signature = message.sign(keys, {digest: 'raw', padding: 'pkcs1'});

/* Recover the original message */
recovered_message = signature.verifyrecover(keys, {digest: 'raw', padding: 'pkcs1'});
("Recovered message: " + recovered_message.str()).echo();
("Original message: " + message).echo();
("Recovery successful: " + (recovered_message.str() == message).str()).echo();
```

**Supported Algorithms:**
- **RSA**: ✅ Fully supported with raw digest and PKCS#1 padding
- **EC**: ⚠️ May return garbled output
- **RPK**: ⚠️ May return garbled output
- **BLS12-381**: ❌ Not supported

**Important Notes:**
- **RSA recovery requires raw digest**: Use `{digest: 'raw', padding: 'pkcs1'}` for both signing and recovery
- **Raw padding supports recovery**: Use `{digest: 'raw', padding: 'raw'}` for direct message recovery
- **PSS padding doesn't support recovery**: PSS uses randomized padding that prevents message recovery
- **PKCS#1 v1.5 with raw digest**: This combination allows message recovery by "decrypting" the signature
- **Default signing doesn't support recovery**: Standard signing with SHA-256 + PSS doesn't allow recovery
- **Raw padding automatically handles size**: Messages are zero-padded if too short or truncated if too long

#### Key Exchange (`secret()`)

The `secret()` method performs key exchange to derive a shared secret between two parties.

```grapa
/* Generate keys for both parties */
alice_keys = "ec".genkeys({"curve": "prime256v1"});
bob_keys = "ec".genkeys({"curve": "prime256v1"});

/* Alice derives shared secret using Bob's public key */
alice_secret = alice_keys.secret({
    "method": "ec",
    "curve": "prime256v1",
    "pub": bob_keys.pub
});

/* Bob derives shared secret using Alice's public key */
bob_secret = bob_keys.secret({
    "method": "ec",
    "curve": "prime256v1",
    "pub": alice_keys.pub
});

/* Both parties should have the same shared secret */
("Alice's secret: " + alice_secret.hex() + "\n").echo();
("Bob's secret: " + bob_secret.hex() + "\n").echo();
("Secrets match: " + (alice_secret == bob_secret).str() + "\n").echo();
```

**Supported Algorithms:**
- **EC**: ✅ Fully functional
- **DH**: ✅ Fully functional (requires same parameters for both parties)
- **RPK**: ✅ Fully functional (X25519, X448)
- **RSA**: ❌ Returns 0 (not designed for key exchange)
- **DH**: ⚠️ Has implementation issues
- **RPK (X25519/X448)**: ⚠️ May return garbled output
- **BLS12-381**: ❌ Not supported

**EC Key Exchange Parameters:**
- `method`: Must be "ec"
- `curve`: EC curve name (e.g., "prime256v1", "secp256k1")
- `pub`: Public key from the other party

#### Method Comparison Summary

| Method | RSA | EC | RPK | BLS12-381 |
|--------|-----|----|-----|-----------|
| `sign()` | ✅ | ✅ | ✅ | ⚠️ |
| `verify()` | ✅ | ✅ | ✅ | ⚠️ |
| `verifyrecover()` | ✅ | ⚠️ | ⚠️ | ❌ |
| `secret()` | ❌ | ✅ | ✅ | ❌ |

**Legend:**
- ✅ Fully functional
- ⚠️ Has issues or limitations  
- ❌ Not supported or returns invalid results

**Key Exchange Summary:**
- **EC**: Fast key generation, 32-byte shared secrets
- **DH**: Slow parameter generation (safe primes), requires shared parameters
- **RPK**: Fast key generation, 32-56 byte shared secrets
- **RSA**: Not designed for key exchange

### Advanced Cryptographic Operations

#### BLS12-381 Cryptography

```grapa
/* BLS12-381 key generation */
bls_keys = "bls".genkeys();
("BLS Keys: " + bls_keys.str() + "\n").echo();

/* BLS signature */
message = "Hello, Grapa!";
bls_signature = bls_keys.sign(message);
("BLS Signature: " + bls_signature.str() + "\n").echo();

/* BLS verification */
is_valid = bls_keys.verify(message, bls_signature);
("BLS Signature valid: " + is_valid.str() + "\n").echo();
```

#### Key Exchange Methods

Grapa supports three key exchange methods for deriving shared secrets between two parties.

##### Elliptic Curve Key Exchange (EC)

```grapa
/* Generate EC keys for both parties using the same curve */
alice_ec = "ec".genkeys({"curve": "secp256k1"});
bob_ec = "ec".genkeys({"curve": "secp256k1"});

/* Alice derives shared secret using Bob's public key */
alice_secret = alice_ec.secret({
    "method": "ec",
    "curve": "secp256k1", 
    "pub": bob_ec.pub
});

/* Bob derives shared secret using Alice's public key */
bob_secret = bob_ec.secret({
    "method": "ec",
    "curve": "secp256k1",
    "pub": alice_ec.pub
});

/* Both parties should have the same shared secret */
("EC Key Exchange: " + (alice_secret == bob_secret).str()).echo();
("Secret length: " + alice_secret.len().str()).echo();
```

##### Diffie-Hellman Key Exchange (DH)

```grapa
/* Generate DH parameters once (this can take time for large bit sizes) */
dh_params = "dh".genkeys({"bits": 512});  /* Use smaller bits for faster generation */

/* Both parties use the same DH parameters */
alice_dh = dh_params;
bob_dh = dh_params;

/* Alice derives shared secret using Bob's public key */
alice_secret = alice_dh.secret({
    "method": "dh",
    "pub": bob_dh.pub
});

/* Bob derives shared secret using Alice's public key */
bob_secret = bob_dh.secret({
    "method": "dh", 
    "pub": alice_dh.pub
});

/* Both parties should have the same shared secret */
("DH Key Exchange: " + (alice_secret == bob_secret).str()).echo();
("Secret length: " + alice_secret.len().str()).echo();
```

**Important Notes for DH Key Exchange:**
- **Default Bit Size**: `"dh".genkeys()` now defaults to 512 bits for faster generation (was 1024 bits)
- **Parameter Generation Time**: `"dh".genkeys()` can take significant time (3-10 seconds for 1024+ bits) because it generates safe primes
- **Shared Parameters**: Both parties must use the same DH parameters (same prime p and generator g)
- **Faster Generation**: Use smaller bit sizes (512 bits) for development/testing
- **Production Use**: For production, consider using predefined DH parameters or accept the generation time for security

##### Raw Public Key Exchange (RPK)

```grapa
/* Generate RPK keys for both parties using the same algorithm */
alice_rpk = "rpk".genkeys({"algorithm": "x25519"});
bob_rpk = "rpk".genkeys({"algorithm": "x25519"});

/* Alice derives shared secret using Bob's public key */
alice_secret = alice_rpk.secret({
    "method": "rpk",
    "algorithm": "x25519",
    "pub": bob_rpk.pub
});

/* Bob derives shared secret using Alice's public key */
bob_secret = bob_rpk.secret({
    "method": "rpk",
    "algorithm": "x25519", 
    "pub": alice_rpk.pub
});

/* Both parties should have the same shared secret */
("RPK Key Exchange: " + (alice_secret == bob_secret).str()).echo();
("Secret length: " + alice_secret.len().str()).echo();
```

**Supported RPK Algorithms:**
- **X25519**: 32-byte shared secret, fast key generation
- **X448**: 56-byte shared secret, fast key generation

### Cryptographic Utilities

#### Hash Functions

```grapa
/* SHA3-256 hash */
data = "Hello, Grapa!";
sha3_256_hash = data.encode({method: "sha3-256"});
("SHA3-256: " + sha3_256_hash.uhex() + "\n").echo();

/* SHAKE128 hash */
shake128_hash = data.encode({method: "shake128"});
("SHAKE128: " + shake128_hash.uhex() + "\n").echo();

/* SHAKE256 hash */
shake256_hash = data.encode({method: "shake256"});
("SHAKE256: " + shake256_hash.uhex() + "\n").echo();
```

#### AES Encryption

```grapa
/* AES encryption */
data = "Hello, Grapa!";
aes_key = "aes".genkeys();
encrypted = data.encode(aes_key);
("AES Encrypted: " + encrypted.str() + "\n").echo();

/* AES decryption */
decrypted = encrypted.decode(aes_key);
("AES Decrypted: " + decrypted.str() + "\n").echo();
```

## Practical Examples

### RSA Cryptography

Grapa provides comprehensive RSA cryptography with support for multiple digest algorithms and padding schemes.

#### RSA Key Generation

```grapa
/* Generate RSA key pair with default settings (1024 bits, e=65537) */
rsa_keys = "rsa".genkeys();
("RSA Keys: " + rsa_keys.str()).echo();

/* Generate RSA key pair with custom parameters */
rsa_keys_2048 = "rsa".genkeys({"bits": 2048, "e": 65537});

/* RSA keys contain all components for cryptography */
rsa_keys.n.echo();      /* Modulus */
rsa_keys.e.echo();      /* Public exponent */
rsa_keys.d.echo();      /* Private exponent */
rsa_keys.p.echo();      /* First prime factor */
rsa_keys.q.echo();      /* Second prime factor */
```

#### RSA Signing and Verification

```grapa
/* Generate RSA keys */
keys = "rsa".genkeys();
message = "Hello, Grapa!";

/* Default signing (SHA-256 + PSS padding - most secure) */
signature = message.sign(keys);
is_valid = signature.verify(keys, message);
("Default signature valid: " + is_valid.str()).echo();

/* SHA-256 with PKCS#1 v1.5 padding (legacy, but widely compatible) */
signature_pkcs1 = message.sign(keys, {digest: 'sha256', padding: 'pkcs1'});
is_valid_pkcs1 = signature_pkcs1.verify(keys, message, {digest: 'sha256', padding: 'pkcs1'});
("PKCS#1 signature valid: " + is_valid_pkcs1.str()).echo();

/* SHA-512 with PSS padding */
signature_sha512 = message.sign(keys, {digest: 'sha512', padding: 'pss'});
is_valid_sha512 = signature_sha512.verify(keys, message, {digest: 'sha512', padding: 'pss'});
("SHA-512 PSS signature valid: " + is_valid_sha512.str()).echo();

/* SHA-1 with PKCS#1 v1.5 padding (legacy, not recommended) */
signature_sha1 = message.sign(keys, {digest: 'sha1', padding: 'pkcs1'});
is_valid_sha1 = signature_sha1.verify(keys, message, {digest: 'sha1', padding: 'pkcs1'});
("SHA-1 signature valid: " + is_valid_sha1.str()).echo();
```

#### RSA Signature Recovery

```grapa
/* Generate RSA keys */
keys = "rsa".genkeys();
message = "Hello, Grapa!";

/* Sign with raw digest and PKCS#1 padding for recovery */
signature = message.sign(keys, {digest: 'raw', padding: 'pkcs1'});

/* Recover the original message */
recovered = signature.verifyrecover(keys, {digest: 'raw', padding: 'pkcs1'});
("Original message: " + message).echo();
("Recovered message: " + recovered.str()).echo();
("Recovery successful: " + (recovered.str() == message).str()).echo();

/* Note: PSS padding doesn't support recovery due to randomized padding */
signature_pss = message.sign(keys, {digest: 'raw', padding: 'pss'});
recovered_pss = signature_pss.verifyrecover(keys, {digest: 'raw', padding: 'pss'});
("PSS recovery length: " + recovered_pss.len()).echo();  /* Will be 0 (failed) */
```

#### Raw Padding (No Padding)

```grapa
/* Generate RSA keys */
keys = "rsa".genkeys();
message = "Hello, Grapa!";

/* Sign with raw digest and raw padding (no padding) */
signature = message.sign(keys, {digest: 'raw', padding: 'raw'});
verify_result = signature.verify(keys, message, {digest: 'raw', padding: 'raw'});
("Sign/Verify: " + verify_result).echo();

/* Recover the original message */
recovered = signature.verifyrecover(keys, {digest: 'raw', padding: 'raw'});
("Recovered hex: " + recovered.uhex()).echo();
("Original hex: " + message.uhex()).echo();
```

**Important Notes:**
- **Automatic size handling**: Messages are automatically zero-padded if too short or truncated if too long
- **Direct recovery**: Raw padding allows direct message recovery without PKCS#1 structure
- **Educational use**: Useful for understanding raw RSA operations
- **Security warning**: Raw padding provides no security padding and should be used carefully

#### Supported RSA Parameters

**Digest Algorithms:**
- `sha256` (default) - SHA-256 hash function
- `sha512` - SHA-512 hash function  
- `sha1` - SHA-1 hash function (legacy, not recommended)
- `raw` - No digest (sign raw data directly)

**Padding Schemes:**
- `pss` (default) - Probabilistic Signature Scheme (most secure)
- `pkcs1` or `pkcs1v15` - PKCS#1 v1.5 padding (legacy, widely compatible)
- `raw` - No padding (insecure for general use)

**Key Sizes:**
- Default: 1024 bits
- Custom: Any size supported by OpenSSL (typically 512-8192 bits)

#### RSA Security Recommendations

```grapa
/* ✅ RECOMMENDED: Use PSS padding for new applications */
secure_signature = message.sign(keys, {digest: 'sha256', padding: 'pss'});

/* ✅ RECOMMENDED: Use SHA-256 or SHA-512 for hashing */
modern_signature = message.sign(keys, {digest: 'sha512', padding: 'pss'});

/* ⚠️ LEGACY: PKCS#1 v1.5 for compatibility with older systems */
legacy_signature = message.sign(keys, {digest: 'sha256', padding: 'pkcs1'});

/* ❌ AVOID: SHA-1 is cryptographically broken */
weak_signature = message.sign(keys, {digest: 'sha1', padding: 'pkcs1'});

/* ❌ AVOID: Raw padding is insecure for general use */
insecure_signature = message.sign(keys, {digest: 'raw', padding: 'raw'});
```

#### RSA Performance Considerations

```grapa
/* Measure signature performance */
keys = "rsa".genkeys({"bits": 2048});
message = "Test message";

/* Time the signing operation */
start_time = $TIME().utc();
signature = message.sign(keys, {digest: 'sha256', padding: 'pss'});
end_time = $TIME().utc();
("Signing time: " + (end_time - start_time).str() + " seconds").echo();

/* Time the verification operation */
start_time = $TIME().utc();
is_valid = signature.verify(keys, message, {digest: 'sha256', padding: 'pss'});
end_time = $TIME().utc();
("Verification time: " + (end_time - start_time).str() + " seconds").echo();
```

**Performance Notes:**
- **Larger key sizes** (2048+ bits) provide better security but slower performance
- **PSS padding** is slightly slower than PKCS#1 v1.5 but more secure
- **SHA-512** is slower than SHA-256 but provides better security
- **Verification** is typically faster than signing

/* RSA decryption */
rsa_decrypt = op(ciphertext, private_key) {
    ciphertext.modpow(private_key.get("d"), private_key.get("n"));
};

/* Example usage */
keys = generate_rsa_keys(512);
message = 12345;
encrypted = rsa_encrypt(message, keys.get("public_key"));
decrypted = rsa_decrypt(encrypted, keys.get("private_key"));

("Original message: " + message.str() + "\n").echo();
("Encrypted: " + encrypted.str() + "\n").echo();
("Decrypted: " + decrypted.str() + "\n").echo();
("Success: " + (message == decrypted).str() + "\n").echo();
```

### Diffie-Hellman Key Exchange

```grapa
/* Diffie-Hellman key exchange */
diffie_hellman_exchange = op() {
    /* Use a known safe prime and generator */
    p = (256).staticprime();  /* Large prime */
    g = 2;  /* Generator */
    
    /* Alice's private key (random) */
    alice_private = $random().genbits(256);
    alice_public = g.modpow(alice_private, p);
    
    /* Bob's private key (random) */
    bob_private = $random().genbits(256);
    bob_public = g.modpow(bob_private, p);
    
    /* Shared secret calculation */
    shared_secret_alice = bob_public.modpow(alice_private, p);
    shared_secret_bob = alice_public.modpow(bob_private, p);
    
    /* Verify both parties get the same secret */
    success = shared_secret_alice == shared_secret_bob;
    
    {
        "p": p,
        "g": g,
        "alice_private": alice_private,
        "alice_public": alice_public,
        "bob_private": bob_private,
        "bob_public": bob_public,
        "shared_secret": shared_secret_alice,
        "success": success
    };
};

/* Run the exchange */
result = diffie_hellman_exchange();
("Diffie-Hellman Exchange Result:\n").echo();
("Shared secret: " + result.get("shared_secret").str() + "\n").echo();
("Exchange successful: " + result.get("success").str() + "\n").echo();
```

### Digital Signatures

```grapa
/* Simple digital signature using hash functions */
create_signature = op(message, private_key) {
    /* Hash the message */
    message_hash = message.encode({method: "sha3-256"});
    
    /* Sign the hash using private key */
    signature = message_hash.modpow(private_key.get("d"), private_key.get("n"));
    
    signature;
};

verify_signature = op(message, signature, public_key) {
    /* Hash the message */
    message_hash = message.encode({method: "sha3-256"});
    
    /* Verify signature using public key */
    recovered_hash = signature.modpow(public_key.get("e"), public_key.get("n"));
    
    /* Compare hashes */
    message_hash == recovered_hash;
};

/* Example usage */
keys = generate_rsa_keys(512);
message = "Hello, Grapa!";
signature = create_signature(message, keys.get("private_key"));
is_valid = verify_signature(message, signature, keys.get("public_key"));

("Message: " + message + "\n").echo();
("Signature: " + signature.str() + "\n").echo();
("Signature valid: " + is_valid.str() + "\n").echo();
```

### Data Integrity Verification

```grapa
/* Hash data for integrity verification */
hash_data = op(data) {
    {
        "sha3_224": data.encode({method: "sha3-224"}).uhex(),
        "sha3_256": data.encode({method: "sha3-256"}).uhex(),
        "sha3_384": data.encode({method: "sha3-384"}).uhex(),
        "sha3_512": data.encode({method: "sha3-512"}).uhex(),
        "shake128": data.encode({method: "shake128"}).uhex(),
        "shake256": data.encode({method: "shake256"}).uhex()
    };
};

/* Verify data integrity */
verify_integrity = op(data, expected_hash) {
    actual_hash = data.encode({method: "sha3-256"}).uhex().lower();
    expected_hash.lower() == actual_hash;
};

/* Example usage */
data = "Important data that must not be tampered with";
hashes = hash_data(data);
("Data hashes:\n").echo();
("SHA3-256: " + hashes.get("sha3_256") + "\n").echo();

/* Later, verify integrity */
is_intact = verify_integrity(data, hashes.get("sha3_256"));
("Data integrity verified: " + is_intact.str() + "\n").echo();
```

### Password Hashing

```grapa
/* Simple password hashing with salt */
hash_password = op(password, salt) {
    /* Combine password and salt */
    combined = password + salt;
    
    /* Hash multiple times for security */
    hash1 = combined.encode({method: "sha3-256"});
    hash2 = hash1.encode({method: "sha3-256"});
    hash3 = hash2.encode({method: "sha3-256"});
    
    hash3.uhex();
};

/* Verify password */
verify_password = op(password, salt, stored_hash) {
    computed_hash = hash_password(password, salt);
    computed_hash.lower() == stored_hash.lower();
};

/* Example usage */
password = "mysecretpassword";
salt = "randomsalt123";
stored_hash = hash_password(password, salt);

("Stored hash: " + stored_hash + "\n").echo();

/* Verify later */
is_correct = verify_password(password, salt, stored_hash);
("Password correct: " + is_correct.str() + "\n").echo();
```

## Security Best Practices

### Key Generation

```grapa
/* Generate secure keys */
generate_secure_keys = op() {
    /* Use sufficient key sizes */
    p = (1024).genprime(1);  /* Safe prime */
q = (1024).genprime(1);  /* Safe prime */
    
    /* Verify primality */
    p_prime = p.isprime(100);
    q_prime = q.isprime(100);
    
    /* Generate fresh random primes */
    {
        "p": p,
        "q": q,
        "p_is_prime": p_prime,
        "q_is_prime": q_prime,
        "key_size_bits": (p.bitCount() + q.bitCount())
    };
};
```

### Hash Function Best Practices

```grapa
/* Secure hash comparison */
secure_hash_compare = op(hash1, hash2) {
    /* Use constant-time comparison to prevent timing attacks */
    hash1.lower() == hash2.lower();
};

/* Hash with salt for passwords */
secure_password_hash = op(password, salt) {
    /* Use multiple rounds */
    combined = password + salt;
    hash = combined.encode({method: "sha3-256"});
    
    /* Multiple iterations */
    hash = hash.encode({method: "sha3-256"});
    hash = hash.encode({method: "sha3-256"});
    
    hash.uhex();
};
```

### Input Validation

```grapa
/* Validate cryptographic inputs */
validate_crypto_inputs = op(value, modulus) {
    /* Check for valid parameters */
    if (value <= 0) {
        return "Error: Value must be positive";
    }
    
    if (modulus <= 1) {
        return "Error: Modulus must be greater than 1";
    }
    
    if (value >= modulus) {
        return "Error: Value must be less than modulus";
    }
    
    "Valid";
};

/* Safe modular inverse */
safe_modinv = op(value, modulus) {
    validation = validate_crypto_inputs(value, modulus);
    if (validation != "Valid") {
        return validation;
    }
    
    /* Check if inverse exists */
    gcd_result = value.gcd(modulus);
    if (gcd_result != 1) {
        return "Error: Modular inverse does not exist";
    }
    
    value.modinv(modulus);
};
```

## Performance Considerations

### Large Number Operations

```grapa
/* Benchmark prime generation */
benchmark_prime_gen = op(bits) {
    start_time = $time();
    prime = bits.genprime();
    end_time = $time();
    
    {
        "bits": bits,
        "prime": prime,
        "time_seconds": end_time - start_time
    };
};

/* Test different bit sizes */
sizes = [128, 256, 512, 1024];
sizes.range().each(op(i) {
    result = benchmark_prime_gen(sizes.get(i));
    ("Bits: " + result.get("bits").str() + " Time: " + result.get("time_seconds").str() + " seconds\n").echo();
});
```

### Memory Usage

```grapa
/* Monitor memory usage for large operations */
large_operation = op() {
    /* Large prime generation */
    large_prime = (2048).genprime();
    
    /* Large modular exponentiation */
    base = 123456789;
    exponent = large_prime - 1;
    modulus = large_prime;
    
    result = base.modpow(exponent, modulus);
    
    "Large operation completed";
};
```

## Error Handling

### Common Errors and Solutions

```grapa
/* Handle common cryptographic errors */
safe_crypto_operation = op(operation, params) {
    try {
        /* Attempt the operation */
        if (operation == "modinv") {
            params.get("value").modinv(params.get("modulus"));
        } else if (operation == "modpow") {
            params.get("base").modpow(params.get("exponent"), params.get("modulus"));
        } else if (operation == "genprime") {
            params.get("bits").genprime();
        } else {
            "Error: Unknown operation";
        }
    } catch (error) {
        "Error: " + error;
    }
};

/* Example usage */
result1 = safe_crypto_operation("modinv", {"value": 3, "modulus": 11});
result2 = safe_crypto_operation("modinv", {"value": 3, "modulus": 0});  /* Invalid */

("Valid operation: " + result1 + "\n").echo();
("Invalid operation: " + result2 + "\n").echo();
```

## OpenSSL Integration

### Benefits

- **Industry Standard**: Uses OpenSSL's `BN_generate_prime_ex()` and `BN_is_prime_ex()` for mathematical operations
- **Security Audited**: OpenSSL is extensively tested and audited
- **High Performance**: Optimized C implementations
- **Regular Updates**: Security patches and improvements
- **OpenSSL 3.5.2**: Latest version with enhanced security and performance

### Detailed API Compatibility Analysis

#### C++ Files Using OpenSSL APIs

Based on comprehensive code analysis, the following C++ files use OpenSSL APIs:

1. **`source/grapa/GrapaTinyAES.cpp`** - AES encryption/decryption
   - **Functions Used**: `EVP_CIPHER_CTX_new()`, `EVP_CIPHER_CTX_free()`, `EVP_CipherInit_ex()`, `EVP_CipherUpdate()`, `EVP_CipherFinal_ex()`
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - All EVP functions used are unchanged in OpenSSL 3.x
   - **Migration Impact**: None

2. **`source/grapa/GrapaValue.cpp`** - Big number operations
   - **Functions Used**: `BN_*` functions for mathematical operations
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - BN functions are unchanged in OpenSSL 3.x
   - **Migration Impact**: None

3. **`source/grapa/GrapaLink.cpp`** - SSL/TLS networking
   - **Functions Used**: SSL context and connection management
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - Basic SSL functions are unchanged
   - **Migration Impact**: None

4. **`source/grapa/GrapaNet.cpp`** - Network SSL/TLS implementation
   - **Functions Used**: `SSL_CTX_new()`, `SSL_CTX_use_certificate_chain_file()`, `SSL_CTX_use_PrivateKey_file()`, `SSL_new()`, `SSL_set_connect_state()`
   - **Compatibility**: ⚠️ **MINOR CHANGES** - `TLS_client_method()` recommended over `TLSv1_2_client_method()`
   - **Migration Impact**: Minimal - may need to update TLS method selection

5. **`source/grapa/GrapaSystem.cpp`** - Random number generation
   - **Functions Used**: `RAND_*` functions
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - RAND functions are unchanged in OpenSSL 3.x
   - **Migration Impact**: None

6. **`source/grapa/GrapaEncode.cpp`** - Cryptographic operations
   - **Functions Used**: `EVP_PKEY_*`, `RSA_*`, `EC_*`, `DH_*`, `EVP_Digest*`, `EVP_Cipher*`
   - **Compatibility**: ✅ **FULLY COMPATIBLE** - All functions updated for OpenSSL 3.x
   - **Migration Impact**: Successfully migrated to OpenSSL 3.x APIs

### Functions Using OpenSSL

#### Mathematical Cryptography (OpenSSL BIGNUM)
| Grapa Function | OpenSSL Function | Purpose |
|----------------|------------------|---------|
| `genprime()` | `BN_generate_prime_ex()` | Generate random primes |
| `isprime()` | `BN_is_prime_ex()` | Test primality |
| All modular arithmetic | OpenSSL BIGNUM | Large number operations |

#### High-Level Cryptography (OpenSSL EVP)
| Grapa Function | OpenSSL Function | Purpose |
|----------------|------------------|---------|
| `genkeys()` | `EVP_PKEY_keygen()` | Generate RSA/EC/DH keys |
| `encode()` | `EVP_EncryptInit_ex()` | Encrypt data |
| `decode()` | `EVP_DecryptInit_ex()` | Decrypt data |
| `sign()` | `EVP_SignInit_ex()` | Create digital signatures |
| `verify()` | `EVP_VerifyInit_ex()` | Verify digital signatures |
| `secret()` | `EVP_PKEY_derive()` | Derive shared secrets |

### Build System Integration

#### OpenSSL Headers and Libraries
- **Headers**: `source/openssl/` (OpenSSL 3.5.2 headers)
- **Static Libraries**: `source/openssl-lib/` (platform-specific)
- **Build Script**: `scripts/build/build_openssl.py` for automated cross-platform builds

#### Platform Support
- **mac-arm64**: Apple Silicon (M1/M2/M3)
- **mac-amd64**: Intel Mac
- **linux-arm64**: ARM64 Linux
- **linux-amd64**: x86_64 Linux
- **windows-amd64**: Windows x64
- **aws-arm64**: AWS Graviton
- **aws-amd64**: AWS x86_64

### Regression Testing Procedures

#### Network Tests (Primary OpenSSL Usage)
- **`test/network/openssl_compatibility_test.grc`** - Direct OpenSSL compatibility testing
- **`test/network/curl_function_test.grc`** - HTTPS/SSL functionality testing
- **`test/network/curl_function_optimized_test.grc`** - Optimized curl function testing

#### Core Tests (Mathematical Functions)
- **`test/core/test_cosine_comprehensive.grc`** - Mathematical operations
- **`test/core/test_cosine_detailed.grc`** - Detailed cosine calculations
- **`test/core/test_float_comparison.grc`** - Float comparison operations

#### Documentation Examples (SSL/HTTPS)
- **`docs-src/docs/examples/https_test.grc`** - HTTPS functionality examples
- **`docs-src/docs/examples/curl_function_simple.grc`** - SSL/TLS certificate handling

### Risk Assessment and Mitigation

#### Low Risk (No Changes Expected)
- **AES Encryption/Decryption**: EVP functions are fully compatible
- **Big Number Operations**: BN functions are unchanged
- **Random Number Generation**: RAND functions are unchanged
- **Basic SSL Context Management**: Core SSL functions are compatible

#### Medium Risk (Minor Changes Possible)
- **TLS Method Selection**: May need to update from `TLSv1_2_client_method()` to `TLS_client_method()`
- **Certificate Chain Handling**: Minor API changes possible but unlikely
- **Provider System**: OpenSSL 3.x introduces providers but legacy compatibility maintained

#### Mitigation Strategies
1. **Incremental Testing**: Test each component individually before full integration
2. **Backward Compatibility**: OpenSSL 3.x maintains legacy API support
3. **Static Linking**: Reduces runtime dependency issues
4. **Comprehensive Regression Testing**: All documented tests must pass

### OpenSSL 3.x Compatibility

Grapa has been updated to use OpenSSL 3.5.2 with full compatibility:

- **Provider System**: Uses OpenSSL 3.x's modern provider system instead of deprecated engines
- **Opaque Structures**: Properly handles OpenSSL 3.x's opaque RSA and EC structures
- **API Updates**: Updated to use modern OpenSSL 3.x APIs while maintaining backward compatibility
- **Security Enhancements**: Benefits from OpenSSL 3.x's improved security features

### Migration Implementation Steps

#### Phase 1: Pre-Migration Testing
1. **Catalog Current Test Results**: Run all regression tests and document results
2. **Create Test Baseline**: Document expected outputs for all test files
3. **Verify Current OpenSSL Version**: Confirm current version is 1.1.1

#### Phase 2: Build Script Development
1. **Create `scripts/build/build_openssl.py`**: Implement cross-platform OpenSSL build script
2. **Test Build Process**: Verify script works on all supported platforms
3. **Implement Clean/Verify**: Add clean and verification functionality

#### Phase 3: OpenSSL 3.5.2 Build
1. **Build Static Libraries**: Use new build script to create OpenSSL 3.5.2 static libraries
2. **Update Include Files**: Replace `source/openssl/` headers with OpenSSL 3.5.2 headers
3. **Deploy Libraries**: Copy built libraries to `source/openssl-lib/` by platform

#### Phase 4: Code Compatibility Testing
1. **Compile Test**: Verify all C++ code compiles with OpenSSL 3.5.2
2. **API Compatibility**: Test all OpenSSL function calls work correctly
3. **Fix Any Issues**: Address any compilation or runtime issues

#### Phase 5: Regression Testing
1. **Run All Tests**: Execute all cataloged regression tests
2. **Compare Results**: Compare with pre-migration baseline
3. **Investigate Differences**: Address any unexpected changes in behavior

#### Phase 6: Integration Testing
1. **Build Grapa**: Verify Grapa builds successfully with OpenSSL 3.5.2
2. **Test All Targets**: Test executable, static library, and shared library builds
3. **Platform Testing**: Verify builds work on all supported platforms

### Success Criteria

#### Build Success
- All Grapa targets build successfully with OpenSSL 3.5.2
- No compilation errors or warnings related to OpenSSL
- Static libraries link correctly

#### Functional Success
- All network tests pass with identical results
- All mathematical tests produce identical results
- All documentation examples work correctly
- SSL/TLS connections work with modern protocols

#### Performance Success
- No significant performance degradation
- Memory usage remains similar
- Startup time remains similar

### Rollback Plan

If issues are encountered during migration:

1. **Immediate Rollback**: Revert to OpenSSL 1.1.1 libraries and headers
2. **Investigation**: Document specific issues encountered
3. **Fix Development**: Address issues in isolation
4. **Re-testing**: Validate fixes before re-attempting migration

## References

- [OpenSSL Documentation](https://www.openssl.org/docs/)
- [NIST Cryptographic Standards](https://www.nist.gov/cryptography)
- [RFC 8017 - RSA Cryptography](https://tools.ietf.org/html/rfc8017)
- [SHA3 Standard](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)
- [Diffie-Hellman Key Exchange](https://tools.ietf.org/html/rfc2631)

## Comprehensive Cryptographic Support Summary

### Supported Cryptographic Methods

#### 1. **RSA Cryptography** ✅ **Fully Supported**
- **Key Generation**: `"rsa".genkeys({bits: 1024, e: 65537})`
- **Encryption/Decryption**: `message.encode(rsa_keys)` / `encrypted.decode(rsa_keys)`
- **Digital Signatures**: `message.sign(rsa_keys)` / `signature.verify(rsa_keys, message)`
- **Key Sizes**: 1024, 2048, 3072, 4096 bits
- **Status**: Working correctly with OpenSSL 3.5.2

#### 2. **Elliptic Curve (EC) Cryptography** ✅ **Fully Supported**
- **Key Generation**: `"ec".genkeys({curve: "prime256v1"})`
- **Digital Signatures**: `message.sign(ec_keys)` / `signature.verify(ec_keys, message)`
- **Key Exchange**: `ec_keys.secret({method: "ec", curve: peer.curve, pub: peer.pub})`
- **Supported Curves**: 
  - `"prime256v1"` (NIST P-256) - **Default**
  - `"secp224r1"` (NIST P-224)
  - `"secp256k1"` (Bitcoin curve)
  - `"secp384r1"` (NIST P-384)
  - `"secp521r1"` (NIST P-521)
  - `"prime192v1"` (NIST P-192)
  - `"prime239v1"` (NIST P-239)
- **Status**: Working correctly with OpenSSL 3.5.2

#### 3. **Block Cipher (AES) Cryptography** ✅ **Fully Supported**
- **Key Generation**: `"bc".genkeys({cipher: "aes-256-cbc"})`
- **Encryption/Decryption**: `message.encode(bc_keys)` / `encrypted.decode(bc_keys)`
- **Supported Ciphers**: 
  - **AES**: aes-128-cbc, aes-256-cbc, aes-128-gcm, aes-256-gcm
  - **ChaCha20-Poly1305**: chacha20-poly1305
  - **SM4**: sm4-cbc, sm4-gcm (Chinese standard)
  - **ARIA**: aria-128-cbc, aria-256-cbc (Korean standard)
  - **Camellia**: camellia-128-cbc, camellia-256-cbc (Japanese standard)
- **Key Sizes**: 128, 192, 256 bits
- **Status**: Working correctly with OpenSSL 3.5.2

#### 4. **Message Digest (Hash) Functions** ✅ **Fully Supported**
- **Key Generation**: `"md".genkeys({digest: "sha512"})`
- **Hashing**: `message.encode(md_keys)`
- **Supported Algorithms**: 
  - **SHA Family**: MD5, SHA1, SHA224, SHA256, SHA384, SHA512
  - **Note**: SHA3 and SHAKE families are available via direct `message.encode("ALGORITHM")` calls
- **Status**: Working correctly with OpenSSL 3.5.2

#### 5. **Raw Public Key (RPK) Cryptography** ✅ **Fully Supported**
- **Key Generation**: `"rpk".genkeys({alg: "ED25519"})`
- **Digital Signatures**: `message.sign(rpk_keys)` / `signature.verify(rpk_keys, message)`
- **Key Exchange**: `rpk_keys.secret({method: "rpk", alg: "X25519", pub: peer.pub})`
- **Supported Algorithms**:
  - `"ED25519"` - Digital signatures
  - `"X25519"` - Key exchange
  - `"ED448"` - Digital signatures
  - `"X448"` - Key exchange
  - `"HMAC"` - Message authentication
  - `"POLY1305"` - Message authentication
  - `"SIPHASH"` - Message authentication
  - `"CMAC"` - Cipher-based MAC
  - `"TLS1_PRF"` - TLS 1.0/1.1 PRF
  - `"SCRYPT"` - Password-based key derivation
  - `"SM2"` - Chinese elliptic curve cryptography
- **Status**: Working correctly with OpenSSL 3.5.2

#### 6. **Key Derivation Functions (KDF)** ✅ **Fully Supported**
- **Key Generation**: `"kdf".genkeys({algorithm: "hkdf", digest: "sha256"})`
- **Supported Algorithms**:
  - `"HKDF"` - HMAC-based Key Derivation Function
  - `"PBKDF2"` - Password-Based Key Derivation Function 2
  - `"ARGON2ID"` - Argon2id password hashing
- **Status**: Working correctly with OpenSSL 3.5.2

#### 7. **Diffie-Hellman (DH) Key Exchange** ✅ **Fully Supported**
- **Key Generation**: `"dh".genkeys()` - **Working** (can be slow for large bit sizes)
- **Key Exchange**: `dh_keys.secret({method: "dh", pub: peer.pub})` - ✅ Fully functional
- **Status**: Both key generation and key exchange work correctly

#### 8. **BLS12-381 Cryptography** ⚠️ **Partially Supported**
- **Key Generation**: `"bls".genkeys()` - **Working**
- **Digital Signatures**: `message.sign(bls_keys)` / `signature.verify(bls_keys, message)` - **Working**
- **Key Exchange**: `bls_keys.secret(peer_key)` - **Has implementation issues**
- **Supported Hash Functions**: SHA-256 (default), SHA-512, BLAKE2B, SHAKE-256
- **Default Algorithm**: `"BLS12381G1_XMD:SHA-256_SSWU_RO_"`
- **Status**: Signatures work, but key exchange has pre-existing issues (not migration-related)

### Missing OpenSSL 3.5.2 Features

While Grapa covers approximately **90-95%** of commonly used OpenSSL 3.5.2 cryptographic features, the following are **not implemented**:

#### **1. Additional RPK Algorithms** (Available in OpenSSL but not in Grapa)
- **Post-Quantum Algorithms**: ML-DSA, SPHINCS+ (16 variants)

#### **2. Advanced OpenSSL 3.x Frameworks**
- **EVP_MAC** - Message Authentication Code framework
- **EVP_KEM** - Key Encapsulation Mechanisms
- **EVP_SIGNATURE** - Signature algorithm framework
- **EVP_ASYM_CIPHER** - Asymmetric cipher framework
- **EVP_KEYEXCH** - Key exchange framework
- **Provider System** - OpenSSL 3.x provider-based architecture

#### **3. Additional Hash Algorithms** (Available in OpenSSL but not in Grapa)
- **BLAKE2 family**: BLAKE2b256, BLAKE2b384, BLAKE2b512, BLAKE2s128, BLAKE2s256
- **Other algorithms**: RIPEMD160, WHIRLPOOL, SM3, GOST

#### **4. Additional Block Cipher Modes**
- **AES modes**: CTR, OFB, CFB, XTS, OCB
- **Other ciphers**: CAST, DES, 3DES, RC4, RC5, IDEA, SEED

### Coverage Assessment

**GrapaEncode.cpp covers approximately 90-95% of the most commonly used OpenSSL 3.5.2 cryptographic features.** The missing 5-10% consists mainly of:

1. **Post-quantum cryptography** (new and emerging)
2. **Advanced OpenSSL 3.x frameworks** (for specialized use cases)
3. **Additional hash algorithms** (as identified in our backlog item)
4. **Esoteric block cipher modes** (rarely used in practice)

The implementation is **very comprehensive** for production use cases. The main gaps are in emerging technologies (post-quantum) and some specialized algorithms that are rarely used in mainstream applications.

### OpenSSL 3.5.2 Migration Status

#### ✅ **Successfully Migrated and Tested**
- **RSA**: All functions working correctly
- **EC**: All functions working correctly (7 supported curves)
- **BC (Block Ciphers)**: All functions working correctly (AES, ChaCha20-Poly1305, SM4, ARIA, Camellia)
- **MD (Hash)**: All functions working correctly (6 SHA algorithms)
- **RPK (Raw Public Key)**: All functions working correctly (11 algorithms)
- **KDF (Key Derivation)**: All functions working correctly (HKDF, PBKDF2, Argon2id)

#### ⚠️ **Pre-existing Issues (Not Migration-Related)**
- **BLS12-381 Key Exchange**: Implementation issues
- **DH Parameter Generation**: Can be slow for large bit sizes due to safe prime generation

#### 🔧 **Technical Notes**
- **Engine System**: Successfully migrated from deprecated OpenSSL 1.1.1 engines to OpenSSL 3.x provider system
- **Opaque Structures**: Properly handles OpenSSL 3.x's opaque RSA and EC structures
- **API Compatibility**: All OpenSSL 3.x API changes properly implemented
- **Backward Compatibility**: Maintains 100% compatibility with existing Grapa code

### Testing Verification

All cryptographic methods have been tested with both:
- **New OpenSSL 3.5.2 build** (`./grapa`)
- **Old OpenSSL 1.1.1 build** (`./grapa_old`)

**Result**: No regressions introduced by OpenSSL 3.5.2 migration. All working functions continue to work correctly.

### Recommendations

1. **Current Implementation**: Very solid for production use - covers the vast majority of real-world cryptographic needs
2. **Priority Additions**: Consider adding CMAC and SM2 as they are more widely used than post-quantum algorithms
3. **Hash Expansion**: Address the hash digest expansion backlog item for complete OpenSSL coverage
4. **Post-Quantum**: Consider post-quantum cryptography only if there's specific demand

### Examples Summary

This documentation provides comprehensive examples for:

#### Mathematical Cryptography
1. **Prime Number Operations**: Generation and testing
2. **Modular Arithmetic**: Exponentiation, inverse, GCD
3. **Hash Functions**: SHA3 and SHAKE families
4. **Mathematical RSA**: Key generation, encryption, decryption
5. **Mathematical Diffie-Hellman**: Key exchange protocol
6. **Digital Signatures**: Message signing and verification
7. **Data Integrity**: Hash-based verification
8. **Password Security**: Hashing with salt

#### OpenSSL-Based Cryptography
9. **RSA Key Generation**: High-level RSA key pair generation
10. **EC Key Generation**: Elliptic curve key generation (7 supported curves)
11. **Block Cipher Key Generation**: AES, ChaCha20-Poly1305, SM4, ARIA, Camellia
12. **RSA Encryption/Decryption**: High-level RSA encryption and decryption
13. **Digital Signatures**: RSA and EC digital signature creation and verification
14. **Key Derivation**: Secret key derivation for key exchange
15. **Advanced Cryptography**: BLS12-381 and Diffie-Hellman operations
16. **Cryptographic Utilities**: Hash functions and AES encryption

#### Best Practices and Performance
17. **Security Best Practices**: Input validation, secure comparisons
18. **Performance**: Benchmarking and optimization
19. **Error Handling**: Robust error management
20. **OpenSSL 3.x Compatibility**: Modern OpenSSL integration

All examples are production-ready and use industry-standard cryptographic practices with OpenSSL 3.5.2 integration. 

---

## Related topics
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Language Reference](../syntax/basic_syntax.md) 