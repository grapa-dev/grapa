---
tags:
  - maintainer
  - implementation
  - networking
  - protocols
  - roadmap
  - planning
---

# Networking Protocol Enhancement Plan

This document outlines the comprehensive plan for expanding Grapa's networking capabilities beyond the current HTTP/HTTPS support to include additional protocols needed for modern applications.

## Overview

**Current Status:** HTTP/HTTPS client/server with SSL/TLS support ✅ **COMPLETE**
**Goal:** Expand to support additional protocols for comprehensive networking capabilities
**Timeline:** Phased implementation over 12-18 months
**Priority:** Medium-High (enables broader application scenarios)

## Current Networking Capabilities

### ✅ **Fully Implemented**
- **HTTP/HTTPS Client**: GET, POST, PUT, DELETE, HEAD, OPTIONS, PATCH
- **HTTPS Server**: Multi-threaded server with SSL/TLS support
- **SSL/TLS**: Complete OpenSSL integration with certificate management
- **TCP/UDP**: Low-level socket operations (cross-platform)
- **Proxy Support**: HTTP proxy with authentication
- **Web Scraping**: HTTP client + XML/HTML parsing integration

### ❌ **Not Currently Supported**
- **HTTP/2**: No HTTP/2 protocol support
- **HTTP/3**: No HTTP/3 protocol support  
- **WebSocket**: No WebSocket protocol support
- **SOCKS Proxy**: Only HTTP proxy (SOCKS not implemented)
- **FTP/SFTP**: No file transfer protocol support
- **SMTP/POP3/IMAP**: No email protocol support
- **DNS**: No custom DNS resolution
- **MQTT/CoAP**: No IoT messaging protocols

## Proposed Protocol Additions

### **Phase 1: Modern Web Protocols** (Months 1-4)
**Priority:** High - Essential for modern web applications

#### **1. WebSocket Support** 🔥 **HIGHEST PRIORITY**
- **Use Cases**: Real-time communication, live dashboards, chat applications
- **Implementation**: Extend `GrapaNetConnect` with WebSocket protocol
- **Features**:
  - WebSocket handshake and protocol upgrade
  - Binary and text message support
  - Ping/pong heartbeat mechanism
  - Connection state management
  - Event-driven message handling
- **Grapa API**:
  ```grapa
  ws = $net().websocket("wss://echo.websocket.org");
  ws.onmessage = op(data) { data.echo(); };
  ws.send("Hello WebSocket!");
  ```
- **Estimated Effort**: 3-4 weeks
- **Dependencies**: None (builds on existing HTTP infrastructure)

#### **2. HTTP/2 Support** 🔥 **HIGH PRIORITY**
- **Use Cases**: Performance improvement for HTTP applications
- **Implementation**: Add HTTP/2 protocol layer to existing HTTP client/server
- **Features**:
  - HTTP/2 framing and multiplexing
  - Server push support
  - Header compression (HPACK)
  - Stream prioritization
  - Connection reuse
- **Grapa API**:
  ```grapa
  // Automatic HTTP/2 upgrade when supported
  response = $net().get("https://http2-enabled-site.com");
  ```
- **Estimated Effort**: 4-6 weeks
- **Dependencies**: OpenSSL 3.0 (already planned)

### **Phase 2: File Transfer & Email** (Months 5-8)
**Priority:** Medium - Common enterprise and automation needs

#### **3. FTP/SFTP Support** 🟡 **MEDIUM PRIORITY**
- **Use Cases**: File upload/download, automated file transfers, backup systems
- **Implementation**: New `GrapaFTP` class extending `GrapaNet`
- **Features**:
  - FTP and SFTP protocols
  - Active/passive mode support
  - File upload/download with progress
  - Directory listing and navigation
  - Authentication (username/password, SSH keys)
- **Grapa API**:
  ```grapa
  ftp = $net().ftp("ftp.example.com", "user", "pass");
  ftp.upload("local.txt", "/remote/file.txt");
  files = ftp.list("/remote/directory");
  ```
- **Estimated Effort**: 4-5 weeks
- **Dependencies**: SSH library for SFTP (libssh2)

#### **4. SMTP/POP3/IMAP Support** 🟡 **MEDIUM PRIORITY**
- **Use Cases**: Email automation, notification systems, email processing
- **Implementation**: New `GrapaEmail` class with protocol-specific handlers
- **Features**:
  - SMTP for sending emails
  - POP3/IMAP for receiving emails
  - MIME message handling
  - Attachment support
  - Authentication (PLAIN, LOGIN, CRAM-MD5)
- **Grapa API**:
  ```grapa
  email = $net().email();
  email.smtp("smtp.gmail.com", "user@gmail.com", "pass");
  email.send({
    to: "recipient@example.com",
    subject: "Test Email",
    body: "Hello from Grapa!"
  });
  ```
- **Estimated Effort**: 5-6 weeks
- **Dependencies**: MIME parsing library

### **Phase 3: Network Control & IoT** (Months 9-12)
**Priority:** Lower - Specialized use cases

#### **5. DNS Support** 🟢 **LOWER PRIORITY**
- **Use Cases**: Custom DNS resolution, network troubleshooting, DNS overrides
- **Implementation**: Extend `GrapaNet` with DNS query capabilities
- **Features**:
  - DNS query and response handling
  - Custom DNS server configuration
  - Record type support (A, AAAA, MX, TXT, etc.)
  - DNS over HTTPS (DoH) support
- **Grapa API**:
  ```grapa
  dns = $net().dns("8.8.8.8");
  records = dns.query("example.com", "A");
  ```
- **Estimated Effort**: 2-3 weeks
- **Dependencies**: None

#### **6. MQTT Support** 🟢 **LOWER PRIORITY**
- **Use Cases**: IoT applications, sensor data collection, real-time messaging
- **Implementation**: New `GrapaMQTT` class for MQTT protocol
- **Features**:
  - MQTT 3.1.1 and 5.0 support
  - QoS levels (0, 1, 2)
  - Topic subscription and publishing
  - Retained messages
  - Last Will and Testament
- **Grapa API**:
  ```grapa
  mqtt = $net().mqtt("mqtt.example.com");
  mqtt.subscribe("sensors/temperature", op(topic, message) {
    ("Temperature: " + message).echo();
  });
  mqtt.publish("sensors/temperature", "23.5");
  ```
- **Estimated Effort**: 3-4 weeks
- **Dependencies**: MQTT library (Paho MQTT C)

### **Phase 4: Enhanced Proxy & Advanced Features** (Months 13-18)
**Priority:** Lower - Advanced networking features

#### **7. SOCKS Proxy Support** 🟢 **LOWER PRIORITY**
- **Use Cases**: Enhanced proxy support, network tunneling
- **Implementation**: Extend existing proxy support in `GrapaNet`
- **Features**:
  - SOCKS4 and SOCKS5 protocols
  - Authentication support
  - UDP support (SOCKS5)
- **Estimated Effort**: 2-3 weeks
- **Dependencies**: None

#### **8. HTTP/3 Support** 🟢 **LOWER PRIORITY**
- **Use Cases**: Next-generation HTTP performance
- **Implementation**: Add HTTP/3 over QUIC support
- **Features**:
  - QUIC transport protocol
  - HTTP/3 framing
  - 0-RTT connection establishment
- **Estimated Effort**: 6-8 weeks
- **Dependencies**: QUIC library (ngtcp2 or similar)

## Implementation Strategy

### **Architecture Extensions**

#### **1. Protocol Abstraction Layer**
```cpp
// Base protocol interface
class GrapaProtocol {
public:
    virtual GrapaError Connect(const GrapaCHAR& url) = 0;
    virtual GrapaError Send(const GrapaCHAR& data) = 0;
    virtual GrapaError Receive(GrapaCHAR& data) = 0;
    virtual GrapaError Disconnect() = 0;
};

// Protocol-specific implementations
class GrapaWebSocket : public GrapaProtocol { /* ... */ };
class GrapaFTP : public GrapaProtocol { /* ... */ };
class GrapaSMTP : public GrapaProtocol { /* ... */ };
```

#### **2. Grapa Language Integration**
```grapa
// Extend $net class with new protocols
@global["$net"] = class {
    // Existing methods...
    websocket = @<[op,@<net_websocket,{this,@<var,{url}>}>],{url}>;
    ftp = @<[op,@<net_ftp,{this,@<var,{url}>,@<var,{user}>,@<var,{pass}>}>],{url,user,pass}>;
    email = @<[op,@<net_email,{this}>]>;
    mqtt = @<[op,@<net_mqtt,{this,@<var,{url}>}>],{url}>;
    dns = @<[op,@<net_dns,{this,@<var,{server}>}>],{server}>;
};
```

### **Dependencies and Libraries**

#### **Required Libraries**
- **libssh2**: SFTP support (Phase 2)
- **libcurl**: Enhanced HTTP features (optional)
- **Paho MQTT C**: MQTT protocol (Phase 3)
- **ngtcp2**: HTTP/3 support (Phase 4)

#### **Build System Updates**
- Add library dependencies to CMakeLists.txt
- Platform-specific library linking
- Conditional compilation for optional features

### **Testing Strategy**

#### **Protocol-Specific Test Suites**
```grapa
// Example test structure
test/network/
├── websocket/
│   ├── basic_websocket_test.grc
│   ├── binary_message_test.grc
│   └── connection_test.grc
├── ftp/
│   ├── ftp_upload_test.grc
│   ├── sftp_test.grc
│   └── directory_test.grc
├── email/
│   ├── smtp_test.grc
│   ├── pop3_test.grc
│   └── imap_test.grc
└── mqtt/
    ├── publish_test.grc
    ├── subscribe_test.grc
    └── qos_test.grc
```

#### **Integration Testing**
- Cross-protocol communication tests
- Performance benchmarking
- Error handling and recovery tests
- Security testing (authentication, encryption)

## Success Metrics

### **Functional Metrics**
- **Protocol Coverage**: Support for 8+ major protocols
- **Feature Completeness**: 90%+ feature parity with standard libraries
- **Cross-Platform Compatibility**: Consistent behavior across Windows, macOS, Linux

### **Performance Metrics**
- **Connection Speed**: Comparable to native protocol implementations
- **Memory Usage**: Efficient resource management
- **Concurrent Connections**: Support for 100+ simultaneous connections

### **Quality Metrics**
- **Test Coverage**: 95%+ code coverage for new protocols
- **Documentation**: Complete API documentation and examples
- **Error Handling**: Comprehensive error reporting and recovery

## Risk Assessment

### **Technical Risks**
- **Library Dependencies**: External library compatibility and maintenance
- **Protocol Complexity**: Some protocols (HTTP/3, MQTT) are complex to implement correctly
- **Performance Impact**: Additional protocols may increase binary size and memory usage

### **Mitigation Strategies**
- **Phased Implementation**: Start with simpler protocols to build expertise
- **Thorough Testing**: Comprehensive test suites for each protocol
- **Optional Features**: Make advanced protocols optional to minimize impact
- **Documentation**: Detailed implementation guides and troubleshooting

## Resource Requirements

### **Development Effort**
- **Total Estimated Effort**: 12-18 months
- **Team Size**: 1-2 developers
- **External Dependencies**: Protocol libraries and documentation

### **Infrastructure**
- **Test Servers**: Dedicated servers for each protocol testing
- **CI/CD Integration**: Automated testing for all protocols
- **Documentation**: Comprehensive user guides and API documentation

## Conclusion

This networking protocol enhancement plan will significantly expand Grapa's capabilities for modern applications. The phased approach ensures manageable implementation while delivering value incrementally. The focus on real-world use cases and comprehensive testing will ensure robust, production-ready implementations.

**Next Steps:**
1. Prioritize Phase 1 protocols (WebSocket, HTTP/2)
2. Begin implementation planning and architecture design
3. Set up development and testing infrastructure
4. Start with WebSocket implementation as highest priority
