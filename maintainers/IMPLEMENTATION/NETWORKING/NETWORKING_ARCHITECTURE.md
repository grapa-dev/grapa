---
tags:
  - maintainer
  - implementation
  - networking
  - architecture
  - design
---

# Networking Architecture

This document provides an overview of Grapa's networking architecture, including the design principles, component relationships, and integration patterns.

## Overview

**Purpose:** Cross-platform networking with HTTP/HTTPS client/server capabilities
**Design:** Layered architecture with platform abstraction
**Status:** Complete implementation

## Architecture Overview

### **System Layers**
```
┌─────────────────────────────────────────────────────────────┐
│                    Grapa Language Layer                     │
│  $net class, BNF rules, system function registration       │
├─────────────────────────────────────────────────────────────┤
│                   Application Layer                         │
│  HTTP client/server, web scraping, callback system         │
├─────────────────────────────────────────────────────────────┤
│                   Protocol Layer                            │
│  HTTP/HTTPS, SSL/TLS, certificate management               │
├─────────────────────────────────────────────────────────────┤
│                   Transport Layer                           │
│  TCP/UDP, socket operations, connection management         │
├─────────────────────────────────────────────────────────────┤
│                   Platform Layer                            │
│  Windows (Winsock2), Unix/Linux (POSIX), macOS (BSD)       │
└─────────────────────────────────────────────────────────────┘
```

### **Core Components**

#### **1. GrapaNet (Core Networking)**
- **Purpose:** Cross-platform socket operations and SSL/TLS support
- **Responsibilities:**
  - Platform abstraction for socket operations
  - SSL/TLS integration via OpenSSL
  - Certificate management and validation
  - Network interface operations
  - Proxy support

#### **2. GrapaNetConnect (HTTP/HTTPS)**
- **Purpose:** HTTP/HTTPS client/server with threading support
- **Responsibilities:**
  - HTTP request/response processing
  - Multi-threaded server architecture
  - Callback-based message handling
  - Web scraping capabilities
  - Connection management

#### **3. Threading System Integration**
- **Purpose:** Concurrent operations and server handling
- **Responsibilities:**
  - Background network operations
  - Multi-threaded server with automatic thread management
  - Thread-safe callback execution
  - Resource management and cleanup

## Component Relationships

### **Class Hierarchy**
```
GrapaCritical (Thread Safety)
└── GrapaNet (Core Networking)
    ├── Platform-specific socket operations
    ├── SSL/TLS integration
    └── Certificate management

GrapaThread (Threading)
└── GrapaNetConnect (HTTP/HTTPS)
    ├── GrapaNet (Core networking)
    ├── HTTP processing
    ├── Callback system
    └── Server architecture
```

### **Integration Points**

#### **1. Grapa Language Integration**
```grapa
// System function registration
@global["$net"] = class {
    // Core operations map to GrapaNet methods
    connect = @<[op,@<net_connect,{this,@<var,{url}>}>],{url}>;
    
    // HTTP operations map to GrapaNetConnect methods
    httpread = @<[op,@<http_read,{this}>]>;
    
    // Server operations with threading
    onlisten = @<[op,@<net_onlisten,{this,@<var,{url}>,@<var,{messageHandler}>,@<var,{connectHandler}>}>],{url,messageHandler,connectHandler}>;
};
```

#### **2. Threading Integration**
```cpp
// GrapaNetConnect extends GrapaThread for background operations
class GrapaNetConnect : public GrapaThread {
    GrapaNet mNet;  // Core networking object
    // Threading capabilities for server operations
};
```

#### **3. SSL/TLS Integration**
```cpp
// OpenSSL integration in GrapaNet
class GrapaNet {
private:
    SSL_CTX* vDelCTX;  // SSL context
    SSL* vSSL;         // SSL connection
    // Certificate and private key management
};
```

## Design Principles

### **1. Cross-Platform Abstraction**
- **Unified API** across Windows, macOS, and Linux
- **Platform-specific implementations** hidden behind abstraction layer
- **Conditional compilation** for platform differences
- **Consistent error handling** across platforms

### **2. Thread Safety**
- **Critical sections** for shared resources
- **Thread-local storage** for connection-specific data
- **Atomic operations** for state management
- **Proper resource cleanup** on thread termination

### **3. Layered Security**
- **SSL/TLS encryption** for secure communications
- **Certificate validation** with chain verification
- **Private key protection** with password callbacks
- **Input validation** for URLs and parameters

### **4. Performance Optimization**
- **Connection pooling** for efficient resource usage
- **Non-blocking operations** for high throughput
- **Memory management** with automatic cleanup
- **Efficient parsing** of HTTP messages

## Data Flow

### **HTTP Client Flow**
```
1. User calls $net().get(url)
2. GrapaNetConnect::HttpGet() called
3. GrapaNet::Connect() establishes connection
4. HTTP request sent via GrapaNet::Send()
5. Response received via GrapaNet::Receive()
6. HTTP response parsed in GrapaNetConnect::HttpMessage()
7. Structured response returned to user
```

### **HTTPS Server Flow**
```
1. User calls $net().onlisten(url, messageHandler, connectHandler)
2. GrapaNetConnect::OnListen() sets up server
3. Server thread started via GrapaThread::Start()
4. GrapaNetConnect::Running() accepts connections
5. New client thread created for each connection
6. Connection handler called for initialization
7. Message handler called for each request
8. Response sent back to client
```

### **Web Scraping Flow**
```
1. User calls $net().get(url) for web scraping
2. HTTP GET request sent to target server
3. Response received and parsed
4. HTML content extracted from response body
5. $XML().parse() processes HTML content
6. CSS selectors extract specific elements
7. Data returned in structured format
```

## Platform Support

### **Windows Implementation**
- **Winsock2 API** for socket operations
- **IP Helper API** for interface enumeration
- **Windows-specific error handling**
- **Thread-safe operations** with Windows threading

### **Unix/Linux Implementation**
- **POSIX socket API** for cross-platform compatibility
- **getifaddrs** for interface enumeration
- **Standard Unix error handling**
- **pthread** for threading support

### **macOS Implementation**
- **POSIX socket API** with macOS optimizations
- **sysctl** for system information
- **macOS-specific networking features**
- **Grand Central Dispatch** integration

## Security Architecture

### **SSL/TLS Security**
```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│  HTTP/HTTPS applications, web scraping                     │
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

### **Certificate Management**
- **X.509 certificate** loading and validation
- **Certificate chain** verification
- **Private key** protection with password callbacks
- **Trusted CA** management for verification

### **Network Security**
- **Input validation** for URLs and parameters
- **Buffer overflow protection** in data operations
- **Platform-specific security measures**
- **Error handling** for malicious responses

## Performance Architecture

### **Concurrency Model**
```
┌─────────────────────────────────────────────────────────────┐
│                    Main Thread                              │
│  User script execution, callback management                │
├─────────────────────────────────────────────────────────────┤
│                    Server Thread                            │
│  Connection acceptance, thread spawning                    │
├─────────────────────────────────────────────────────────────┤
│                    Client Threads                          │
│  Individual connection handling, message processing        │
└─────────────────────────────────────────────────────────────┘
```

### **Resource Management**
- **Connection pooling** for efficient resource usage
- **Memory management** with automatic cleanup
- **Thread lifecycle** management
- **SSL context** reuse for multiple connections

### **Optimization Strategies**
- **Non-blocking operations** for high throughput
- **Efficient parsing** of HTTP messages
- **Streaming response** handling
- **Compression support** for bandwidth optimization

## Error Handling Architecture

### **Error Propagation**
```
┌─────────────────────────────────────────────────────────────┐
│                    User Layer                               │
│  $ERR objects, error handling in scripts                   │
├─────────────────────────────────────────────────────────────┤
│                    Application Layer                        │
│  HTTP error codes, connection failures                     │
├─────────────────────────────────────────────────────────────┤
│                    Transport Layer                          │
│  Socket errors, SSL/TLS errors                             │
├─────────────────────────────────────────────────────────────┤
│                    Platform Layer                           │
│  OS-specific error codes, system errors                    │
└─────────────────────────────────────────────────────────────┘
```

### **Error Categories**
- **Network errors** - Connection failures, timeouts
- **HTTP errors** - Status codes, parsing failures
- **SSL/TLS errors** - Certificate validation, handshake failures
- **Platform errors** - OS-specific networking errors

## Integration Patterns

### **1. Language Integration**
- **BNF rules** in `lib/grapa/$grapa.grc` resolve to C++ functions
- **System function registration** maps Grapa methods to C++ implementations
- **Type system integration** with `$net` objects
- **Error handling** with `$ERR` objects

### **2. Threading Integration**
- **GrapaNetConnect** extends `GrapaThread` for background operations
- **Callback system** for asynchronous operations
- **Thread-safe networking** via `GrapaNet` critical sections
- **Resource management** with automatic cleanup

### **3. XML/HTML Integration**
- **Web scraping** integrates with `$XML()` for content parsing
- **HTML parsing** for data extraction
- **CSS selectors** for element selection
- **Response processing** with unified data access

## Future Architecture Enhancements

### **Planned Improvements**
- **HTTP/2 and HTTP/3** protocol support
- **WebSocket** implementation for real-time communication
- **Enhanced proxy support** with authentication
- **Connection pooling** for improved performance

### **Performance Enhancements**
- **Zero-copy operations** for high-throughput scenarios
- **Async I/O** with completion callbacks
- **Memory pooling** for connection objects
- **Response caching** for repeated requests

### **Security Enhancements**
- **Enhanced certificate validation** with OCSP
- **Certificate transparency** support
- **Advanced cipher suite** management
- **Security policy** enforcement

## Related Documentation

- **[GRAPA_NET_IMPLEMENTATION.md](GRAPA_NET_IMPLEMENTATION.md)** - Core networking implementation
- **[GRAPA_NET_CONNECT_IMPLEMENTATION.md](GRAPA_NET_CONNECT_IMPLEMENTATION.md)** - HTTP/HTTPS implementation
- **[SSL_TLS_IMPLEMENTATION.md](SSL_TLS_IMPLEMENTATION.md)** - SSL/TLS details
- **[HTTP_SERVER_IMPLEMENTATION.md](HTTP_SERVER_IMPLEMENTATION.md)** - HTTPS server details
- **[WEB_SCRAPING_IMPLEMENTATION.md](WEB_SCRAPING_IMPLEMENTATION.md)** - Web scraping capabilities
- **[../SYSTEM_COMPONENTS/GRAPA_THREADING_ARCHITECTURE.md](../SYSTEM_COMPONENTS/GRAPA_THREADING_ARCHITECTURE.md)** - Threading architecture
