---
tags:
  - maintainer
  - implementation
  - networking
  - ssl
  - tls
  - security
  - openssl
---

# SSL/TLS Implementation

This document provides comprehensive implementation details for SSL/TLS support in Grapa's networking system, including OpenSSL integration, certificate management, and security features.

## Overview

**Purpose:** Secure communications via SSL/TLS encryption
**Integration:** OpenSSL library for cryptography
**Status:** Complete implementation

## Architecture

### **SSL/TLS Integration**
```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│  HTTP/HTTPS applications, secure web scraping              │
├─────────────────────────────────────────────────────────────┤
│                    SSL/TLS Layer                            │
│  OpenSSL integration, certificate management               │
├─────────────────────────────────────────────────────────────┤
│                    Transport Layer                          │
│  TCP connections, socket operations                        │
├─────────────────────────────────────────────────────────────┤
│                    Platform Layer                           │
│  OS networking stack, security features                    │
└─────────────────────────────────────────────────────────────┘
```

### **OpenSSL Integration**
- **SSL_CTX** - SSL context for connection management
- **SSL** - Individual SSL connection objects
- **BIO** - Basic I/O abstraction for network operations
- **X509** - Certificate handling and validation

## Implementation Details

### **1. SSL/TLS Initialization**

#### **OpenSSL Setup**
```cpp
// Initialize OpenSSL BIO system
BIO_sock_init();

// SSL context creation
vXCTX = vDelCTX = SSL_CTX_new(TLS_client_method());  // Client mode
// or
vXCTX = vDelCTX = SSL_CTX_new(TLS_server_method());  // Server mode
```

#### **SSL Context Configuration**
```cpp
// Set SSL options
SSL_CTX_set_options(vXCTX, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

// Set cipher suites
SSL_CTX_set_cipher_list(vXCTX, "HIGH:!aNULL:!MD5:!RC4");

// Set verification mode
SSL_CTX_set_verify(vXCTX, SSL_VERIFY_PEER, NULL);
```

### **2. Certificate Management**

#### **Certificate Loading**
```cpp
GrapaError GrapaNet::Certificate(GrapaCHAR& certFile)
{
    if (certFile.mLength == 0) {
        // Disable SSL mode
        vIsSSL = false;
        return 0;
    }
    
    // Enable SSL mode
    vIsSSL = true;
    vCertFile.FROM(certFile);
    
    // Load certificate file
    if (vXCTX && SSL_CTX_use_certificate_chain_file(vXCTX, (char*)certFile.mBytes) <= 0) {
        return -1;
    }
    
    return 0;
}
```

#### **Private Key Loading**
```cpp
GrapaError GrapaNet::Private(GrapaCHAR& keyFile, void* passOp, void* passParam)
{
    vKeyFile.FROM(keyFile);
    vPassOp = passOp;
    vPassParam = passParam;
    
    // Load private key with password callback
    if (vXCTX && SSL_CTX_use_PrivateKey_file(vXCTX, (char*)keyFile.mBytes, 
                                            SSL_FILETYPE_PEM) <= 0) {
        return -1;
    }
    
    return 0;
}
```

#### **Trusted CA Management**
```cpp
GrapaError GrapaNet::Trusted(GrapaCHAR& caFile, GrapaCHAR& caPath)
{
    vcaFile.FROM(caFile);
    vcaPath.FROM(caPath);
    
    // Load trusted CA certificates
    if (vXCTX && SSL_CTX_load_verify_locations(vXCTX, 
                                              (char*)caFile.mBytes, 
                                              (char*)caPath.mBytes) <= 0) {
        return -1;
    }
    
    return 0;
}
```

### **3. SSL/TLS Connection Handling**

#### **Client Connection Setup**
```cpp
// In GrapaNet::Connect() for SSL mode
if (gSystem->mLinkInitialized && vIsSSL) {
    // Create SSL context if not exists
    if (!vXCTX) {
        vXCTX = vDelCTX = SSL_CTX_new(TLS_client_method());
    }
    
    // Create SSL connection
    vSSL = SSL_new(vXCTX);
    
    // Attach SSL to socket
    SSL_set_fd(vSSL, ((GrapaNetPrivate*)vInstance)->mSocket);
    
    // Perform SSL handshake
    if (SSL_connect(vSSL) <= 0) {
        return -1;
    }
}
```

#### **Server Connection Setup**
```cpp
// In GrapaNet::Listen() for SSL mode
if (vIsSSL) {
    // Create SSL context if not exists
    if (!vXCTX) {
        vXCTX = vDelCTX = SSL_CTX_new(TLS_server_method());
    }
    
    // Create SSL connection
    vSSL = SSL_new(vXCTX);
    
    // Attach SSL to socket
    SSL_set_fd(vSSL, ((GrapaNetPrivate*)vInstance)->mSocket);
    
    // Perform SSL handshake
    if (SSL_accept(vSSL) <= 0) {
        return -1;
    }
}
```

### **4. Certificate Verification**

#### **Certificate Validation**
```cpp
GrapaError GrapaNet::Verify(GrapaCHAR& cert)
{
    if (!vSSL) return -1;
    
    // Get peer certificate
    X509* peerCert = SSL_get_peer_certificate(vSSL);
    if (!peerCert) return -1;
    
    // Verify certificate
    long verifyResult = SSL_get_verify_result(vSSL);
    
    // Convert certificate to PEM format
    GrapaCHAR pemCert;
    X509_to_PEM(peerCert, pemCert);
    
    // Create result object
    GrapaRuleEvent* result = new GrapaRuleEvent();
    result->Set("result", verifyResult);
    result->Set("certificate", pemCert);
    
    X509_free(peerCert);
    return result;
}
```

#### **Certificate Chain Retrieval**
```cpp
GrapaRuleQueue* GrapaNet::Chain()
{
    if (!vSSL) return NULL;
    
    // Get certificate chain
    STACK_OF(X509)* chain = SSL_get_peer_cert_chain(vSSL);
    if (!chain) return NULL;
    
    // Convert chain to PEM format
    GrapaRuleQueue* result = new GrapaRuleQueue();
    for (int i = 0; i < sk_X509_num(chain); i++) {
        X509* cert = sk_X509_value(chain, i);
        GrapaCHAR pemCert;
        X509_to_PEM(cert, pemCert);
        result->Push(pemCert);
    }
    
    return result;
}
```

### **5. Data Transfer with SSL/TLS**

#### **SSL Send Operations**
```cpp
GrapaError GrapaNet::Send(u8* sendbuf, u64 sendbuflen, u64& sendlen)
{
    if (vIsSSL) {
        // SSL_write for encrypted data
        int result = SSL_write(vSSL, sendbuf, (int)sendbuflen);
        sendlen = result;
        return result;
    } else {
        // Standard socket send
        int result = send(((GrapaNetPrivate*)vInstance)->mSocket, 
                         (char*)sendbuf, (int)sendbuflen, 0);
        sendlen = result;
        return result;
    }
}
```

#### **SSL Receive Operations**
```cpp
GrapaError GrapaNet::Receive(u8* recvbuf, u64 recvbuflen, u64& recvlen)
{
    if (vIsSSL) {
        // SSL_read for encrypted data
        int result = SSL_read(vSSL, recvbuf, (int)recvbuflen);
        recvlen = result;
        return result;
    } else {
        // Standard socket recv
        int result = recv(((GrapaNetPrivate*)vInstance)->mSocket, 
                         (char*)recvbuf, (int)recvbuflen, 0);
        recvlen = result;
        return result;
    }
}
```

#### **SSL Pending Data Check**
```cpp
GrapaError GrapaNet::Pending(u64& hasmore)
{
    if (vIsSSL) {
        // Check SSL buffer for pending data
        hasmore = SSL_pending(vSSL);
    } else {
        // Check socket buffer for pending data
        // Platform-specific implementation
    }
    return 0;
}
```

### **6. Certificate Utilities**

#### **PEM Conversion**
```cpp
static void GrapaNet::X509_to_PEM(X509* cert, GrapaCHAR& pem)
{
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_X509(bio, cert);
    
    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio, &bptr);
    
    pem.SetLength(bptr->length);
    memcpy((char*)pem.mBytes, bptr->data, bptr->length);
    
    BIO_free(bio);
}

static X509* GrapaNet::PEM_to_X509(const GrapaCHAR& pem)
{
    BIO* bio = BIO_new_mem_buf((char*)pem.mBytes, (int)pem.mLength);
    X509* cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    BIO_free(bio);
    return cert;
}
```

#### **Certificate Decoding**
```cpp
static GrapaRuleQueue* GrapaNet::Decode(const GrapaCHAR& cert)
{
    X509* x509 = PEM_to_X509(cert);
    if (!x509) return NULL;
    
    // Extract certificate information
    GrapaRuleQueue* result = new GrapaRuleQueue();
    
    // Subject
    X509_NAME* subject = X509_get_subject_name(x509);
    char subjectStr[256];
    X509_NAME_oneline(subject, subjectStr, sizeof(subjectStr));
    result->Push(GrapaCHAR(subjectStr));
    
    // Issuer
    X509_NAME* issuer = X509_get_issuer_name(x509);
    char issuerStr[256];
    X509_NAME_oneline(issuer, issuerStr, sizeof(issuerStr));
    result->Push(GrapaCHAR(issuerStr));
    
    // Serial number
    ASN1_INTEGER* serial = X509_get_serialNumber(x509);
    BIGNUM* bn = ASN1_INTEGER_to_BN(serial, NULL);
    char* serialStr = BN_bn2hex(bn);
    result->Push(GrapaCHAR(serialStr));
    OPENSSL_free(serialStr);
    BN_free(bn);
    
    X509_free(x509);
    return result;
}
```

## Security Features

### **1. Cipher Suite Management**
```cpp
// Set secure cipher suites
SSL_CTX_set_cipher_list(vXCTX, "HIGH:!aNULL:!MD5:!RC4:!DES");

// Disable weak protocols
SSL_CTX_set_options(vXCTX, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);

// Enable perfect forward secrecy
SSL_CTX_set_options(vXCTX, SSL_OP_SINGLE_DH_USE | SSL_OP_SINGLE_ECDH_USE);
```

### **2. Certificate Validation**
```cpp
// Set verification mode
SSL_CTX_set_verify(vXCTX, SSL_VERIFY_PEER, NULL);

// Set verification depth
SSL_CTX_set_verify_depth(vXCTX, 4);

// Custom verification callback
SSL_CTX_set_verify(vXCTX, SSL_VERIFY_PEER, custom_verify_callback);
```

### **3. Session Management**
```cpp
// Enable session reuse
SSL_CTX_set_session_cache_mode(vXCTX, SSL_SESS_CACHE_SERVER);

// Set session timeout
SSL_CTX_set_timeout(vXCTX, 300);  // 5 minutes

// Session ID context
SSL_CTX_set_session_id_context(vXCTX, (unsigned char*)"grapa", 5);
```

## Integration with Grapa Language

### **System Function Registration**
```cpp
// SSL/TLS functions
net_certificate, net_private, net_trusted, net_verify, net_chain
```

### **$net Class SSL/TLS Methods**
```grapa
@global["$net"] = class {
    // SSL/TLS operations
    certificate = @<[op,@<net_certificate,{this,@<var,{cert}>}>],{cert}>;
    private = @<[op,@<net_private,{this,@<var,{key}>,@<var,{pass}>,@<var,{param}>}>],{key,pass,param}>;
    trusted = @<[op,@<net_trusted,{this,@<var,{file}>,@<var,{path}>}>],{file,path}>;
    verify = @<[op,@<net_verify,{this}>]>;
    chain = @<[op,@<net_chain,{this}>]>;
};
```

## Usage Patterns

### **HTTPS Client Setup**
```grapa
/* Setup HTTPS client with certificate verification */
n = $net();
n.trusted("ca-bundle.crt", "/etc/ssl/certs");
n.connect("https://api.example.com:443");
verifyResult = n.verify();
if (verifyResult.get("result") == 0) {
    // Certificate is valid
    response = n.httpread();
};
```

### **HTTPS Server Setup**
```grapa
/* Setup HTTPS server with certificates */
n = $net();
n.certificate("server.crt");
n.private("server.key", passwordCallback, passwordParam);
n.onlisten(':443', messageHandler, connectHandler);
```

### **Certificate Management**
```grapa
/* Certificate verification and chain inspection */
n = $net();
n.connect("https://example.com");
verifyResult = n.verify();
certChain = n.chain();

// Decode certificate information
certInfo = certChain[0].decode("PEM");
("Subject: " + certInfo[0]).echo();
("Issuer: " + certInfo[1]).echo();
("Serial: " + certInfo[2]).echo();
```

## Performance Characteristics

### **SSL/TLS Performance**
- **Session reuse** for improved connection performance
- **Certificate caching** to reduce validation overhead
- **Efficient cipher suite** selection for optimal performance
- **Connection pooling** for SSL context reuse

### **Memory Management**
- **Automatic cleanup** of SSL resources
- **Certificate memory** management with proper deallocation
- **Context reuse** for multiple connections
- **Buffer management** for encrypted data

## Error Handling

### **SSL/TLS Errors**
- **Handshake failures** with detailed error reporting
- **Certificate validation** errors with verification results
- **Cipher suite** negotiation failures
- **Protocol version** compatibility issues

### **Certificate Errors**
- **Certificate loading** failures
- **Private key** password errors
- **CA certificate** validation failures
- **Chain verification** errors

## Security Considerations

### **Best Practices**
- **Strong cipher suites** with perfect forward secrecy
- **Certificate validation** with proper CA verification
- **Private key protection** with secure password handling
- **Protocol version** enforcement for security compliance

### **Vulnerability Mitigation**
- **BEAST attack** prevention with TLS 1.1+
- **Heartbleed** mitigation with updated OpenSSL
- **POODLE attack** prevention with TLS 1.2+
- **Bleichenbacher attack** mitigation with proper padding

## Future Enhancements

### **Planned Features**
- **TLS 1.3** support for improved security and performance
- **Certificate transparency** support for enhanced validation
- **OCSP stapling** for real-time certificate validation
- **Client certificate** authentication support

### **Security Enhancements**
- **Enhanced certificate validation** with OCSP
- **Certificate pinning** for additional security
- **Advanced cipher suite** management
- **Security policy** enforcement

## Related Documentation

- **[GRAPA_NET_IMPLEMENTATION.md](GRAPA_NET_IMPLEMENTATION.md)** - Core networking implementation
- **[GRAPA_NET_CONNECT_IMPLEMENTATION.md](GRAPA_NET_CONNECT_IMPLEMENTATION.md)** - HTTP/HTTPS implementation
- **[NETWORKING_ARCHITECTURE.md](NETWORKING_ARCHITECTURE.md)** - Overall architecture
- **[docs-src/docs/sys/net.md](../../../docs-src/docs/sys/net.md)** - User API documentation
