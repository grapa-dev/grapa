---
tags:
  - maintainer
  - implementation
  - networking
  - core
  - c++
---

# GrapaNet Implementation

This document provides comprehensive implementation details for `GrapaNet`, the core networking class that provides cross-platform socket operations, SSL/TLS support, and certificate management.

## Overview

**File:** `source/grapa/GrapaNet.cpp` (48KB, 1662 lines)
**Header:** `source/grapa/GrapaNet.h` (85 lines)
**Purpose:** Cross-platform networking with SSL/TLS support
**Status:** Complete implementation

## Architecture

### **Class Hierarchy**
```
GrapaNet : public GrapaCritical
├── Cross-platform socket operations
├── SSL/TLS integration via OpenSSL
├── Certificate management
├── Proxy support
└── Network interface operations
```

### **Key Components**
- **`GrapaNetPrivate`** - Platform-specific socket data
- **OpenSSL Integration** - SSL/TLS operations
- **Cross-platform abstraction** - Windows, macOS, Linux support
- **Certificate management** - X.509, PEM, chain validation

## Implementation Details

### **1. Platform Abstraction**

#### **Private Data Structure**
```cpp
typedef struct _GrapaNetPrivate {
    struct addrinfo mHints;
    struct sockaddr_storage mSockAddr;
    char mServerInfo[46];
#if defined(__MINGW32__) || defined(__GNUC__)
    int mSocket;
#endif
#ifdef _WIN32
    WSADATA wsaData;
    SOCKET mSocket;
#endif
} GrapaNetPrivate;
```

#### **Platform-Specific Headers**
- **Windows**: `winsock2.h`, `iphlpapi.h`, `ws2tcpip.h`
- **Unix/Linux**: `sys/socket.h`, `netdb.h`, `arpa/inet.h`
- **macOS**: `sys/sysctl.h`, `net/if_dl.h`

### **2. Constructor and Initialization**

#### **Constructor Implementation**
```cpp
GrapaNet::GrapaNet()
{
    vInstance = (void*)GrapaMem::Create(sizeof(GrapaNetPrivate));
    
    mStarted = false;
    mConnected = false;
    mBound = false;
    
    // Platform-specific socket initialization
#ifdef _WIN32
    memset(&((GrapaNetPrivate*)vInstance)->wsaData, 0, sizeof(WSADATA));
    memset(&((GrapaNetPrivate*)vInstance)->mHints, 0, sizeof(addrinfo));
#endif
    
    // SSL/TLS initialization
    vIsSSL = false;
    vIsBlocking = false;
    vCriticalError = false;
    
    vDelCTX = NULL;
    vXCTX = NULL;
    vBIO = NULL;
    aBIO = NULL;
    vSSL = NULL;
    
    BIO_sock_init(); // Initialize OpenSSL BIO
}
```

#### **Startup and Shutdown**
```cpp
GrapaError GrapaNet::Startup()
{
#ifdef _WIN32
    if (!mStarted && WSAStartup(MAKEWORD(2, 2), &((GrapaNetPrivate*)vInstance)->wsaData) != 0) 
        return(-1);
#endif
    mStarted = true;
    return(0);
}

void GrapaNet::Shutdown()
{
    if (!mStarted) return;
#ifdef _WIN32
    WSACleanup();
#endif
    mStarted = false;
}
```

### **3. URL Parsing and Service Resolution**

#### **GetService Method**
```cpp
void GrapaNet::GetService(const GrapaCHAR& pURL, GrapaCHAR& pNodeName, 
                         GrapaCHAR& pServiceName, bool& isIPv6)
{
    // Parse URL format: [host]:port or host:port
    // Supports IPv6 addresses in brackets
    // Extracts hostname and port/service name
}
```

**Features:**
- **IPv6 support** with bracket notation `[::1]:8080`
- **IPv4 support** with standard notation `127.0.0.1:8080`
- **Service name resolution** for standard ports
- **Hostname extraction** for DNS resolution

### **4. Connection Management**

#### **Connect Method**
```cpp
GrapaError GrapaNet::Connect(const GrapaCHAR& pURL)
{
    // Parse URL and extract host/port
    GetService(pURL, pNodeName, pServiceName, isIPv6);
    
    // Initialize networking
    if (Startup()) return(-1);
    
    // SSL/TLS setup if enabled
    if (gSystem->mLinkInitialized && vIsSSL) {
        vXCTX = vDelCTX = SSL_CTX_new(TLS_client_method());
        // Certificate and private key setup
    }
    
    // Socket creation and connection
    // Platform-specific connection logic
}
```

#### **Bind Method**
```cpp
GrapaError GrapaNet::Bind(const GrapaCHAR& pURL)
{
    // Parse URL and extract interface/port
    // Create socket and bind to address
    // Support for IPv4 and IPv6
}
```

#### **Listen Method**
```cpp
GrapaError GrapaNet::Listen(GrapaNet& pClient)
{
    // Accept incoming connections
    // Create new socket for client
    // Handle SSL/TLS handshake if enabled
}
```

### **5. Data Transfer Operations**

#### **Send Operations**
```cpp
GrapaError GrapaNet::Send(u8* sendbuf, u64 sendbuflen, u64& sendlen)
{
    if (vIsSSL) {
        // SSL_write for encrypted data
        return SSL_write(vSSL, sendbuf, (int)sendbuflen);
    } else {
        // Standard socket send
        return send(((GrapaNetPrivate*)vInstance)->mSocket, 
                   (char*)sendbuf, (int)sendbuflen, 0);
    }
}

GrapaError GrapaNet::Send(const GrapaBYTE& sendbuf)
{
    u64 sendlen;
    return Send((u8*)sendbuf.mBytes, sendbuf.mLength, sendlen);
}
```

#### **Receive Operations**
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

#### **Pending Data Check**
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

### **6. SSL/TLS Integration**

#### **Certificate Management**
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

#### **Private Key Management**
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

#### **Certificate Verification**
```cpp
GrapaError GrapaNet::Verify(GrapaCHAR& cert)
{
    // Verify SSL certificate
    // Return verification result and certificate chain
    // Uses OpenSSL X509 verification functions
}
```

### **7. Network Interface Operations**

#### **MAC Address Operations**
```cpp
static GrapaError GrapaNet::GetMac(char* pName, char* pIp, u64& pMAC)
{
    // Platform-specific MAC address retrieval
    // Windows: GetAdaptersInfo
    // Unix: getifaddrs
    // macOS: sysctl
}
```

#### **Interface Enumeration**
```cpp
static GrapaRuleEvent* GrapaNet::GetInterfaces()
{
    // Enumerate all network interfaces
    // Return interface information (name, MAC, IP, family)
    // Platform-specific implementation
}
```

### **8. Proxy Support**

#### **Proxy Configuration**
```cpp
GrapaError GrapaNet::Proxy(GrapaCHAR& ptype, GrapaCHAR& pid, GrapaCHAR& ph)
{
    vIsProxy = true;
    vpType.FROM(ptype);
    vpId.FROM(pid);
    vPh.FROM(ph);
    
    // Configure proxy settings
    // Support for HTTP and SOCKS proxies
}
```

## Integration with Grapa Language

### **System Function Registration**
The following C++ functions are registered with the Grapa system:

```cpp
// Core networking functions
net_mac, net_interfaces, net_connect, net_bind, net_listen
net_disconnect, net_send, net_receive, net_pending

// SSL/TLS functions
net_certificate, net_private, net_trusted, net_verify, net_chain

// Proxy functions
net_proxy, net_host

// HTTP functions
http_read, http_send, http_message
```

### **$net Class Methods**
```grapa
@global["$net"] = class {
    // Core operations
    connect = @<[op,@<net_connect,{this,@<var,{url}>,@<var,{cert}>,@<var,{proxy}>}>],{url,cert,proxy}>;
    bind = @<[op,@<net_bind,{this,@<var,{url}>}>],{url}>;
    listen = @<[op,@<net_listen,{this,@<var,{net}>}>],{net}>;
    
    // SSL/TLS operations
    certificate = @<[op,@<net_certificate,{this,@<var,{cert}>}>],{cert}>;
    private = @<[op,@<net_private,{this,@<var,{key}>,@<var,{pass}>,@<var,{param}>}>],{key,pass,param}>;
    verify = @<[op,@<net_verify,{this}>]>;
    
    // Data operations
    send = @<[op,@<net_send,{this,@<var,{data}>}>],{data}>;
    receive = @<[op,@<net_receive,{this}>]>;
    pending = @<[op,@<net_pending,{this}>]>;
};
```

## Performance Characteristics

### **Cross-Platform Performance**
- **Windows**: Winsock2 optimized for Windows networking
- **Unix/Linux**: Standard POSIX socket operations
- **macOS**: Optimized for macOS networking stack

### **SSL/TLS Performance**
- **OpenSSL integration** for high-performance cryptography
- **Session reuse** for improved connection performance
- **Certificate caching** to reduce validation overhead

### **Memory Management**
- **Automatic cleanup** of network resources
- **Platform-specific memory allocation**
- **SSL context reuse** for multiple connections

## Error Handling

### **Network Errors**
- **Connection failures** with detailed error codes
- **SSL/TLS errors** with OpenSSL error reporting
- **Platform-specific error handling**

### **Resource Management**
- **Automatic socket cleanup** in destructor
- **SSL context cleanup** on disconnect
- **Memory leak prevention** with proper deallocation

## Security Considerations

### **SSL/TLS Security**
- **Certificate validation** with chain verification
- **Private key protection** with password callbacks
- **Secure random number generation** via OpenSSL

### **Network Security**
- **Input validation** for URLs and parameters
- **Buffer overflow protection** in data operations
- **Platform-specific security measures**

## Platform-Specific Details

### **Windows Implementation**
- **Winsock2 API** for socket operations
- **IP Helper API** for interface enumeration
- **Windows-specific error handling**

### **Unix/Linux Implementation**
- **POSIX socket API** for cross-platform compatibility
- **getifaddrs** for interface enumeration
- **Standard Unix error handling**

### **macOS Implementation**
- **POSIX socket API** with macOS optimizations
- **sysctl** for system information
- **macOS-specific networking features**

## Future Enhancements

### **Planned Improvements**
- **HTTP/2 and HTTP/3** protocol support
- **WebSocket** implementation
- **Enhanced proxy support** with authentication
- **Connection pooling** for improved performance

### **Performance Optimizations**
- **Zero-copy operations** for high-throughput scenarios
- **Async I/O** with completion callbacks
- **Memory pooling** for connection objects

## Related Documentation

- **[GRAPA_NET_CONNECT_IMPLEMENTATION.md](GRAPA_NET_CONNECT_IMPLEMENTATION.md)** - HTTP/HTTPS implementation
- **[SSL_TLS_IMPLEMENTATION.md](SSL_TLS_IMPLEMENTATION.md)** - SSL/TLS details
- **[NETWORKING_ARCHITECTURE.md](NETWORKING_ARCHITECTURE.md)** - Overall architecture
- **[docs-src/docs/sys/net.md](../../../docs-src/docs/sys/net.md)** - User API documentation
