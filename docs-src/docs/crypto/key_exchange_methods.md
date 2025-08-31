# Key Exchange Methods in Grapa

Grapa supports three different key exchange protocols through the `.secret()` method. Each has different requirements and use cases.

## Overview

The `.secret()` method enables secure key exchange between two parties, allowing them to generate a shared secret without directly sharing it over the network. This shared secret can then be used for symmetric encryption, key derivation, or other cryptographic purposes.

## 1. Traditional Diffie-Hellman (DH)

**Key Requirement**: Both parties must use the same prime modulus (`p`) and generator (`g`).

### How It Works

1. **Party A** generates DH keys: `dh_alice = "dh".genkeys()`
2. **Party A** sends `p` and `g` to **Party B**
3. **Party B** generates DH keys using the same parameters: `dh_bob = "dh".genkeys({p: dh_alice.p, g: dh_alice.g})`
4. Both parties compute the shared secret using each other's public keys

### Example

```grapa
/* Party A generates keys and sends p, g to Party B */
dh_alice = "dh".genkeys();

/* Party B generates keys using the same p, g from Party A */
dh_bob = "dh".genkeys({p: dh_alice.p, g: dh_alice.g});

/* Both parties compute shared secret */
alice_secret = dh_alice.secret({method: "dh", pub: dh_bob.pub});
bob_secret = dh_bob.secret({method: "dh", pub: dh_alice.pub});

/* Secrets should match */
(alice_secret == bob_secret).echo();  /* true */
```

### Use Cases

- Legacy systems requiring traditional DH
- When you need to control the prime modulus and generator
- Systems that must use specific DH parameters

## 2. Elliptic Curve (EC) Key Exchange

**Key Requirement**: Both parties must use the same curve.

### How It Works

1. **Party A** generates EC keys on a specific curve: `ec_alice = "ec".genkeys({curve: "prime256v1"})`
2. **Party B** generates EC keys on the same curve: `ec_bob = "ec".genkeys({curve: "prime256v1"})`
3. Both parties compute the shared secret using each other's public keys

### Example

```grapa
/* Both parties generate independent keys on the same curve */
ec_alice = "ec".genkeys({curve: "prime256v1"});
ec_bob = "ec".genkeys({curve: "prime256v1"});

/* Both parties compute shared secret */
alice_secret = ec_alice.secret({
    method: "ec",
    curve: "prime256v1",
    pub: ec_bob.pub
});
bob_secret = ec_bob.secret({
    method: "ec",
    curve: "prime256v1",
    pub: ec_alice.pub
});

/* Secrets should match */
(alice_secret == bob_secret).echo();  /* true */
```

### Use Cases

- Modern cryptographic applications
- When you want shorter key lengths for the same security level
- TLS/SSL connections
- Most contemporary secure communications

## 3. Raw Public Key (RPK) Key Exchange

**Key Requirement**: Both parties must use the same algorithm.

### How It Works

1. **Party A** generates RPK keys using a specific algorithm: `rpk_alice = "rpk".genkeys({alg: "X25519"})`
2. **Party B** generates RPK keys using the same algorithm: `rpk_bob = "rpk".genkeys({alg: "X25519"})`
3. Both parties compute the shared secret using each other's public keys

### Example

```grapa
/* Both parties generate independent keys using the same algorithm */
rpk_alice = "rpk".genkeys({alg: "X25519"});
rpk_bob = "rpk".genkeys({alg: "X25519"});

/* Both parties compute shared secret */
alice_secret = rpk_alice.secret({
    method: "rpk",
    alg: "X25519",
    pub: rpk_bob.pub
});
bob_secret = rpk_bob.secret({
    method: "rpk",
    alg: "X25519",
    pub: rpk_alice.pub
});

/* Secrets should match */
(alice_secret == bob_secret).echo();  /* true */
```

### Use Cases

- Modern applications requiring high performance
- When you need algorithm-specific optimizations
- Systems using X25519, Ed25519, or other raw public key algorithms
- IoT and embedded systems

## Comparison Table

| Method | Parameter Sharing | Key Generation | Security Level | Performance |
|--------|------------------|----------------|----------------|-------------|
| **DH** | Must share `p` and `g` | Dependent | Variable (depends on key size) | Slower |
| **EC** | Must agree on curve | Independent | High (256-bit equivalent) | Fast |
| **RPK** | Must agree on algorithm | Independent | High (algorithm-dependent) | Fastest |

## Security Considerations

- **DH**: Security depends on the size of the prime modulus. 2048-bit or larger recommended.
- **EC**: Security depends on the curve. `prime256v1` provides 128-bit security.
- **RPK**: Security depends on the algorithm. X25519 provides 128-bit security.

## Recommendations

- **Use EC** for most modern applications
- **Use RPK** when you need maximum performance or specific algorithms
- **Use DH** only for legacy compatibility

## Testing

You can test all three methods using the comprehensive test file:

```bash
./grapa test/crypto/key_exchange_comprehensive.grc
```

This will verify that all three key exchange methods work correctly and generate matching shared secrets.
