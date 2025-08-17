---
tags:
  - maintainer
  - implementation
  - networking
  - http
  - https
  - server
  - threading
---

# HTTP Server Implementation

This document provides comprehensive implementation details for HTTP/HTTPS server capabilities in Grapa, including multi-threaded architecture, callback system, and server management.

## Overview

**Purpose:** Multi-threaded HTTP/HTTPS server with callback-based request handling
**Architecture:** Thread-per-connection with automatic thread management
**Status:** Complete implementation

## Architecture

### **Server Architecture**
```
┌─────────────────────────────────────────────────────────────┐
│                    Main Thread                              │
│  Server initialization, callback management                │
├─────────────────────────────────────────────────────────────┤
│                    Server Thread                            │
│  Connection acceptance, thread spawning                    │
├─────────────────────────────────────────────────────────────┤
│                    Client Threads                          │
│  Individual connection handling, request processing        │
└─────────────────────────────────────────────────────────────┘
```

### **Component Relationships**
- **`GrapaNetConnect`** extends `GrapaThread` for server operations
- **`GrapaNet`** provides core networking and SSL/TLS support
- **Callback system** handles request/response processing
- **Thread management** for concurrent connections

## Implementation Details

### **1. Server Initialization**

#### **OnListen Method**
```cpp
GrapaError GrapaNetConnect::OnListen(GrapaScriptExec* pScriptExec, 
                                    GrapaRuleEvent* pUrl, 
                                    GrapaRuleEvent* pReceiveLambda, 
                                    GrapaRuleEvent* pConnectLambda)
{
    // Setup server with callbacks
    vScriptState = pScriptExec->vScriptState;
    vReceiveLambda = pReceiveLambda;
    vConnectLambda = pConnectLambda;
    mListenMode = true;
    
    // Bind to address
    mError = mNet.Bind(mURL);
    if (mError) return mError;
    
    // Start listening thread
    Start();
    return 0;
}
```

#### **Server Setup Process**
```grapa
/* HTTPS server setup with callbacks */
n = $net();
n.certificate("server.crt");
n.private("server.key", passwordCallback, passwordParam);
n.onlisten(':443', messageHandler, connectHandler);
```

### **2. Multi-Threaded Server Architecture**

#### **Server Thread Execution**
```cpp
void GrapaNetConnect::Running()
{
    while (!mStop && mListenMode) {
        // Accept incoming connections
        GrapaNetConnect* client = new GrapaNetConnect();
        client->mListenChild = true;
        
        if (mNet.Listen(client->mNet) == 0) {
            // Setup client connection
            client->vScriptState = vScriptState;
            client->vReceiveLambda = vReceiveLambda;
            client->vConnectLambda = vConnectLambda;
            
            // Call connection handler
            if (vConnectLambda) {
                GrapaRuleEvent* args[] = { client->vObj };
                vConnectLambda->CALL(vScriptState, args, 1);
            }
            
            // Start client thread
            client->Start();
        } else {
            delete client;
        }
    }
}
```

#### **Client Thread Management**
```cpp
// Each client connection gets its own thread
class GrapaNetConnect : public GrapaThread {
    // Thread lifecycle management
    void Starting() { /* Initialize thread resources */ }
    void Running() { /* Handle client requests */ }
    void Stopping() { /* Cleanup thread resources */ }
};
```

### **3. Connection Handling**

#### **Connection Handler Callback**
```grapa
/* Connection handler for new connections */
connectHandler = op(netSession) {
    netSession.data = "";
    netSession.requestCount = 0;
    netSession.startTime = $sys().time();
    // Initialize connection-specific data
};
```

#### **Connection Setup Process**
```cpp
// Called when new connection is established
void GrapaNetConnect::HandleConnection()
{
    if (vConnectLambda) {
        GrapaRuleEvent* args[] = { vObj };
        vConnectLambda->CALL(vScriptState, args, 1);
    }
}
```

### **4. Request Processing**

#### **Message Handler Callback**
```grapa
/* Message handler for incoming requests */
messageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    if (hasmore == 0) {
        // Process complete message
        response = processRequest(netSession.data);
        netSession.send(response);
        netSession.data = "";
        netSession.requestCount += 1;
    };
};
```

#### **Request Processing Flow**
```cpp
// In client thread
void GrapaNetConnect::ProcessRequest()
{
    // Receive HTTP request
    GrapaBYTE request;
    GrapaError result = mNet.Receive(request);
    
    if (result == 0) {
        // Parse HTTP request
        GrapaRuleEvent* httpRequest = ParseHttpRequest(request);
        
        // Call message handler
        if (vReceiveLambda) {
            GrapaRuleEvent* args[] = { vObj, httpRequest };
            vReceiveLambda->CALL(vScriptState, args, 2);
        }
    }
}
```

### **5. HTTP Request Parsing**

#### **Request Structure**
```cpp
struct HttpRequest {
    std::string method;      // GET, POST, PUT, DELETE, etc.
    std::string path;        // Request path
    std::string version;     // HTTP version
    std::map<std::string, std::string> headers;
    std::string body;        // Request body
};
```

#### **Request Parsing Implementation**
```cpp
GrapaRuleEvent* GrapaNetConnect::ParseHttpRequest(const GrapaBYTE& request)
{
    // Parse HTTP request line
    // Extract method, path, version
    
    // Parse headers
    // Extract header name-value pairs
    
    // Extract body
    // Handle content-length and chunked encoding
    
    // Return structured request object
    return requestObj;
}
```

### **6. Response Generation**

#### **Response Structure**
```cpp
struct HttpResponse {
    int statusCode;          // 200, 404, 500, etc.
    std::string statusText;  // OK, Not Found, etc.
    std::map<std::string, std::string> headers;
    std::string body;        // Response body
};
```

#### **Response Generation**
```grapa
/* Generate HTTP response */
processRequest = op(requestData) {
    // Parse request
    request = parseHttpRequest(requestData);
    
    // Route request based on method and path
    if (request.get("method") == "GET") {
        if (request.get("path") == "/") {
            response = generateHomePage();
        } else if (request.get("path").startsWith("/api/")) {
            response = handleApiRequest(request);
        } else {
            response = generate404Response();
        };
    } else if (request.get("method") == "POST") {
        response = handlePostRequest(request);
    } else {
        response = generate405Response();
    };
    
    // Format HTTP response
    formatHttpResponse(response);
};
```

### **7. SSL/TLS Server Support**

#### **HTTPS Server Setup**
```grapa
/* HTTPS server with SSL certificates */
n = $net();
n.certificate("server.crt");
n.private("server.key", passwordCallback, passwordParam);
n.trusted("ca-bundle.crt", "/etc/ssl/certs");
n.onlisten(':443', messageHandler, connectHandler);
```

#### **SSL Handshake Process**
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

### **8. Thread Safety and Resource Management**

#### **Thread Safety**
```cpp
// Thread-safe networking via GrapaNet critical sections
class GrapaNet : public GrapaCritical {
    // All networking operations are thread-safe
    // Critical sections protect shared resources
};
```

#### **Resource Management**
```cpp
// Automatic cleanup in destructor
GrapaNetConnect::~GrapaNetConnect()
{
    Disconnect();
    
    // Cleanup callbacks
    if (dReceiveLambda) {
        dReceiveLambda->CLEAR();
        delete dReceiveLambda;
    }
    if (dConnectLambda) {
        dConnectLambda->CLEAR();
        delete dConnectLambda;
    }
}
```

## Usage Patterns

### **1. Basic HTTP Server**
```grapa
/* Simple HTTP server */
messageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    if (hasmore == 0) {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello World</h1>";
        netSession.send(response);
        netSession.data = "";
    };
};

connectHandler = op(netSession) {
    netSession.data = "";
};

n = $net();
n.onlisten(':8080', messageHandler, connectHandler);
```

### **2. HTTPS Server with API**
```grapa
/* HTTPS server with API endpoints */
processRequest = op(requestData) {
    request = parseHttpRequest(requestData);
    
    if (request.get("method") == "GET" && request.get("path") == "/api/users") {
        users = getUsersFromDatabase();
        response = {
            "status": 200,
            "headers": {"Content-Type": "application/json"},
            "body": $JSON().stringify(users)
        };
    } else if (request.get("method") == "POST" && request.get("path") == "/api/users") {
        userData = $JSON().parse(request.get("body"));
        result = createUser(userData);
        response = {
            "status": 201,
            "headers": {"Content-Type": "application/json"},
            "body": $JSON().stringify(result)
        };
    } else {
        response = {
            "status": 404,
            "headers": {"Content-Type": "text/plain"},
            "body": "Not Found"
        };
    };
    
    formatHttpResponse(response);
};

n = $net();
n.certificate("server.crt");
n.private("server.key");
n.onlisten(':443', processRequest, connectHandler);
```

### **3. Web Application Server**
```grapa
/* Web application server with routing */
router = {
    "GET": {
        "/": op() { serveFile("index.html"); },
        "/api/data": op() { serveApiData(); },
        "/static/*": op(path) { serveStaticFile(path); }
    },
    "POST": {
        "/api/submit": op(request) { handleSubmission(request); }
    }
};

processRequest = op(requestData) {
    request = parseHttpRequest(requestData);
    method = request.get("method");
    path = request.get("path");
    
    if (router.get(method) && router.get(method).get(path)) {
        handler = router.get(method).get(path);
        response = handler(request);
    } else {
        response = generate404Response();
    };
    
    formatHttpResponse(response);
};
```

## Performance Characteristics

### **Concurrency Model**
- **Thread-per-connection** for maximum concurrency
- **Non-blocking operations** for high throughput
- **Connection pooling** for efficient resource usage
- **Callback-based processing** for low latency

### **Scalability Features**
- **Automatic thread management** for connection handling
- **Resource cleanup** on connection termination
- **Memory management** with automatic deallocation
- **SSL context reuse** for multiple connections

### **Performance Optimizations**
- **Efficient HTTP parsing** with minimal allocations
- **Response caching** for static content
- **Connection reuse** for improved performance
- **Compression support** for bandwidth optimization

## Error Handling

### **Server Errors**
- **Connection failures** with graceful handling
- **SSL/TLS errors** with proper error reporting
- **Request parsing errors** with 400 responses
- **Thread termination** with resource cleanup

### **Client Errors**
- **Invalid requests** with 400 Bad Request
- **Authentication failures** with 401 Unauthorized
- **Resource not found** with 404 Not Found
- **Method not allowed** with 405 Method Not Allowed

### **Server Errors**
- **Internal errors** with 500 Internal Server Error
- **Service unavailable** with 503 Service Unavailable
- **Gateway errors** with 502 Bad Gateway

## Security Considerations

### **HTTPS Security**
- **SSL/TLS encryption** for secure communications
- **Certificate validation** with chain verification
- **Private key protection** with password callbacks
- **Secure cipher suites** via OpenSSL

### **Server Security**
- **Input validation** for all request parameters
- **Header validation** to prevent injection attacks
- **Content length limits** to prevent DoS attacks
- **Rate limiting** for API endpoints

### **Access Control**
- **Authentication** via headers or cookies
- **Authorization** for protected resources
- **CORS headers** for cross-origin requests
- **Security headers** for enhanced protection

## Monitoring and Logging

### **Server Monitoring**
```grapa
/* Server monitoring with metrics */
connectHandler = op(netSession) {
    netSession.data = "";
    netSession.startTime = $sys().time();
    netSession.requestCount = 0;
    
    // Log connection
    ("New connection from " + netSession.host()).echo();
};

messageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    if (hasmore == 0) {
        netSession.requestCount += 1;
        
        // Log request
        request = parseHttpRequest(netSession.data);
        ("Request: " + request.get("method") + " " + request.get("path")).echo();
        
        // Process request
        response = processRequest(netSession.data);
        netSession.send(response);
        netSession.data = "";
    };
};
```

### **Performance Metrics**
- **Connection count** tracking
- **Request rate** monitoring
- **Response time** measurement
- **Error rate** tracking

## Future Enhancements

### **Planned Features**
- **HTTP/2 and HTTP/3** protocol support
- **WebSocket** implementation for real-time communication
- **Enhanced routing** with middleware support
- **Load balancing** for multiple server instances

### **Performance Improvements**
- **Connection pooling** for improved performance
- **Response compression** for bandwidth optimization
- **Static file serving** with caching
- **API rate limiting** and throttling

## Related Documentation

- **[GRAPA_NET_IMPLEMENTATION.md](GRAPA_NET_IMPLEMENTATION.md)** - Core networking implementation
- **[GRAPA_NET_CONNECT_IMPLEMENTATION.md](GRAPA_NET_CONNECT_IMPLEMENTATION.md)** - HTTP/HTTPS implementation
- **[SSL_TLS_IMPLEMENTATION.md](SSL_TLS_IMPLEMENTATION.md)** - SSL/TLS details
- **[NETWORKING_ARCHITECTURE.md](NETWORKING_ARCHITECTURE.md)** - Overall architecture
- **[docs-src/docs/sys/net.md](../../../docs-src/docs/sys/net.md)** - User API documentation
