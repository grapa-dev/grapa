---
tags:
  - maintainer
  - implementation
  - networking
  - http
  - https
  - server
  - client
---

# GrapaNetConnect Implementation

This document provides comprehensive implementation details for `GrapaNetConnect`, the HTTP/HTTPS client/server implementation that extends `GrapaThread` to provide multi-threaded networking with callback support.

## Overview

**File:** `source/grapa/GrapaNetConnect.cpp` (22KB, 903 lines)
**Header:** `source/grapa/GrapaNetConnect.h` (59 lines)
**Purpose:** HTTP/HTTPS client/server with threading support
**Status:** Complete implementation

## Architecture

### **Class Hierarchy**
```
GrapaNetConnect : public GrapaThread
├── HTTP/HTTPS client operations
├── Multi-threaded server architecture
├── Callback-based message handling
├── Web scraping capabilities
└── Connection management
```

### **Key Components**
- **`GrapaNet`** - Core networking operations
- **Threading support** - Background operations and server handling
- **Callback system** - Message and connection handlers
- **HTTP parsing** - Request/response processing
- **Web scraping** - Content extraction and processing

## Implementation Details

### **1. Class Structure and Initialization**

#### **Class Members**
```cpp
class GrapaNetConnect : public GrapaThread
{
public:
    GrapaNet mNet;                    // Core networking object
    GrapaCHAR mURL;                   // Connection URL
    GrapaRuleEvent* vReceiveLambda;   // Message handler callback
    GrapaRuleEvent* vConnectLambda;   // Connection handler callback
    GrapaScriptState* vScriptState;   // Script execution state
    bool mListenMode;                 // Server mode flag
    bool mListenChild;                // Child connection flag
    GrapaRuleEvent* vObj;             // Associated object
    GrapaRuleQueue mListeners;        // Active listeners
    GrapaNames mNames;                // Namespace management
    GrapaRuleEvent* dPassLambda;      // Password callback
    GrapaRuleEvent* dPassParam;       // Password parameter
};
```

#### **Constructor Implementation**
```cpp
GrapaNetConnect::GrapaNetConnect()
{
    vReceiveLambda = NULL;
    dReceiveLambda = NULL;
    vConnectLambda = NULL;
    dConnectLambda = NULL;
    vScriptState = NULL;
    mListenMode = false;
    mListenChild = false;
    vObj = NULL;
    dPassLambda = NULL;
    dPassParam = NULL;
}
```

### **2. Connection Management**

#### **Connect Method**
```cpp
GrapaError GrapaNetConnect::Connect(GrapaRuleEvent* pUrl)
{
    Disconnect();
    vObj = pUrl;
    mListenMode = false;
    mError = mNet.Connect(mURL);
    return(mError);
}
```

#### **Bind Method**
```cpp
GrapaError GrapaNetConnect::Bind(GrapaRuleEvent* pUrl)
{
    Disconnect();
    vObj = pUrl;
    mError = mNet.Bind(mURL);
    return(mError);
}
```

#### **Listen Method**
```cpp
GrapaError GrapaNetConnect::Listen(GrapaNetConnect* pNet)
{
    Disconnect();
    vObj = pNet->vObj;
    mError = pNet->mNet.Listen(mNet);
    return(mError);
}
```

### **3. Multi-Threaded Server Architecture**

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

#### **Thread Execution (Running Method)**
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

### **4. HTTP Client Operations**

#### **HttpRead Method**
```cpp
GrapaError GrapaNetConnect::HttpRead(GrapaScriptExec* pScriptExec)
{
    // Read HTTP response from connection
    GrapaBYTE response;
    GrapaError result = mNet.Receive(response);
    
    if (result == 0) {
        // Parse HTTP response
        GrapaRuleEvent* httpResponse = HttpMessage(pScriptExec, &mNames);
        
        // Return response object
        GrapaRuleEvent* result = new GrapaRuleEvent();
        result->Set(httpResponse);
        return result;
    }
    
    return result;
}
```

#### **HttpMessage Method**
```cpp
GrapaRuleEvent* GrapaNetConnect::HttpMessage(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace)
{
    // Parse HTTP message (request or response)
    // Extract headers and body
    // Return structured message object
    
    GrapaRuleEvent* message = new GrapaRuleEvent();
    
    // Parse status line
    // Parse headers
    // Extract body
    
    return message;
}
```

#### **HttpSend Method**
```cpp
GrapaError GrapaNetConnect::HttpSend(GrapaScriptExec* pScriptExec, 
                                    GrapaRuleEvent* method, 
                                    GrapaRuleEvent* entity, 
                                    GrapaRuleEvent* header, 
                                    GrapaRuleEvent* body)
{
    // Construct HTTP request
    GrapaCHAR request;
    request.FROM(method->GetString());
    request += " ";
    request += entity->GetString();
    request += " HTTP/1.1\r\n";
    
    // Add headers
    if (header) {
        request += header->GetString();
    }
    
    // Add body
    if (body) {
        request += "\r\n";
        request += body->GetString();
    }
    
    // Send request
    return mNet.Send(request);
}
```

### **5. Web Scraping Implementation**

#### **HTTP GET Request**
```cpp
// Implementation of $net().get(url) functionality
GrapaError GrapaNetConnect::HttpGet(const GrapaCHAR& url)
{
    // Connect to URL
    mError = mNet.Connect(url);
    if (mError) return mError;
    
    // Send HTTP GET request
    GrapaCHAR request = "GET / HTTP/1.1\r\nHost: ";
    request += ExtractHost(url);
    request += "\r\nConnection: close\r\n\r\n";
    
    mError = mNet.Send(request);
    if (mError) return mError;
    
    // Read response
    GrapaBYTE response;
    mError = mNet.Receive(response);
    
    // Parse response and extract content
    return ParseHttpResponse(response);
}
```

#### **Response Parsing**
```cpp
GrapaError GrapaNetConnect::ParseHttpResponse(const GrapaBYTE& response)
{
    // Parse HTTP response headers
    // Extract status code
    // Separate headers from body
    // Handle chunked encoding
    // Handle compression
    
    // Return structured response object
    return 0;
}
```

### **6. SSL/TLS Integration**

#### **Certificate Management**
```cpp
GrapaError GrapaNetConnect::Certificate(GrapaRuleEvent* certFile)
{
    return mNet.Certificate(certFile->GetString());
}
```

#### **Private Key Management**
```cpp
GrapaError GrapaNetConnect::Private(GrapaScriptExec* pScriptExec, 
                                   GrapaRuleEvent* keyFile, 
                                   GrapaRuleEvent* passOp, 
                                   GrapaRuleEvent* passParam)
{
    dPassLambda = passOp;
    dPassParam = passParam;
    return mNet.Private(keyFile->GetString(), dPassLambda, dPassParam);
}
```

#### **Certificate Verification**
```cpp
GrapaError GrapaNetConnect::Verify(GrapaCHAR& cert)
{
    return mNet.Verify(cert);
}
```

### **7. Callback System**

#### **Message Handler Callback**
```cpp
GrapaError GrapaNetConnect::OnReceive(GrapaScriptExec* pScriptExec, 
                                     GrapaRuleEvent* pReceiveLambda)
{
    vReceiveLambda = pReceiveLambda;
    
    // Setup receive callback
    // Handle incoming data
    // Call message handler with data
    
    return 0;
}
```

#### **Connection Handler Callback**
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

### **8. Threading and Concurrency**

#### **Thread Lifecycle**
```cpp
void GrapaNetConnect::Starting()
{
    // Initialize thread-specific resources
    // Setup thread-local storage
}

void GrapaNetConnect::Running()
{
    // Main thread execution loop
    // Handle connections and messages
    // Process callbacks
}

void GrapaNetConnect::Stopping()
{
    // Cleanup thread resources
    // Close connections
    // Free callbacks
}
```

#### **Thread Safety**
- **Thread-safe networking** via `GrapaNet` critical sections
- **Callback protection** with proper locking
- **Resource management** with automatic cleanup
- **Connection isolation** per thread

## Integration with Grapa Language

### **System Function Registration**
```cpp
// HTTP/HTTPS functions
http_read, http_send, http_message

// Connection management functions
net_onlisten, net_onreceive
```

### **$net Class HTTP Methods**
```grapa
@global["$net"] = class {
    // HTTP operations
    httpread = @<[op,@<http_read,{this}>]>;
    httpsend = @<[op,@<http_send,{this,@<var,{method}>,@<var,{entity}>,@<var,{header}>,@<var,{body}>}>],{method,entity,header,body}>;
    httpmessage = @<[op,@<http_message,{this,@<var,{raw}>}>],{raw:null}>;
    
    // Server operations
    onlisten = @<[op,@<net_onlisten,{this,@<var,{url}>,@<var,{messageHandler}>,@<var,{connectHandler}>,@<var,{count}>}>],{url,messageHandler,connectHandler,count:1}>;
    onreceive = @<[op,@<net_onreceive,{this,@<var,{handler}>}>],{handler}>;
};
```

## Web Scraping Capabilities

### **HTTP Client for Web Requests**
```grapa
/* Basic web scraping */
response = $net().get("https://example.com");
if (response.get("status") == 200) {
    content = response.get("body");
    html = $XML().parse(content);
    titles = html.select("h1, h2, h3");
};
```

### **Response Processing**
- **Status code extraction** for error handling
- **Header parsing** for content type detection
- **Body extraction** for content processing
- **Encoding detection** for proper text handling

### **Integration with XML/HTML Processing**
- **HTML parsing** via `$XML().parse()`
- **Element selection** with CSS selectors
- **Attribute extraction** for data mining
- **Content transformation** for data processing

## HTTPS Server Implementation

### **Server Setup**
```grapa
/* HTTPS server with callbacks */
n = $net();
n.certificate("server.crt");
n.private("server.key");
n.onlisten(':443', messageHandler, connectHandler);
```

### **Message Handler**
```grapa
messageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    if (hasmore == 0) {
        // Process complete message
        response = processRequest(netSession.data);
        netSession.send(response);
        netSession.data = "";
    };
};
```

### **Connection Handler**
```grapa
connectHandler = op(netSession) {
    netSession.data = "";
    // Initialize connection-specific data
};
```

## Performance Characteristics

### **Multi-threaded Performance**
- **Concurrent connections** handled by separate threads
- **Non-blocking operations** for high throughput
- **Connection pooling** for efficient resource usage
- **Callback-based processing** for low latency

### **HTTP Performance**
- **Efficient parsing** of HTTP messages
- **Streaming response** handling
- **Compression support** for bandwidth optimization
- **Connection reuse** for improved performance

### **Web Scraping Performance**
- **Parallel requests** with rate limiting
- **Content caching** for repeated requests
- **Efficient parsing** of HTML/XML content
- **Memory management** for large responses

## Error Handling

### **Network Errors**
- **Connection failures** with retry logic
- **HTTP errors** with status code handling
- **SSL/TLS errors** with certificate validation
- **Timeout handling** for long-running operations

### **Thread Safety**
- **Thread-safe callbacks** with proper locking
- **Resource cleanup** on thread termination
- **Error propagation** across thread boundaries
- **Graceful shutdown** of server threads

## Security Considerations

### **HTTPS Security**
- **Certificate validation** with chain verification
- **Private key protection** with password callbacks
- **Secure cipher suites** via OpenSSL
- **TLS version enforcement** for security compliance

### **Web Scraping Security**
- **Input validation** for URLs and parameters
- **Rate limiting** to avoid server overload
- **User agent management** for proper identification
- **Error handling** for malicious responses

## Future Enhancements

### **Planned Features**
- **HTTP/2 and HTTP/3** protocol support
- **WebSocket** implementation for real-time communication
- **Enhanced compression** support (gzip, brotli)
- **Connection pooling** for improved performance

### **Performance Optimizations**
- **Zero-copy operations** for high-throughput scenarios
- **Async I/O** with completion callbacks
- **Memory pooling** for connection objects
- **Response caching** for repeated requests

## Related Documentation

- **[GRAPA_NET_IMPLEMENTATION.md](GRAPA_NET_IMPLEMENTATION.md)** - Core networking implementation
- **[HTTP_SERVER_IMPLEMENTATION.md](HTTP_SERVER_IMPLEMENTATION.md)** - HTTPS server details
- **[WEB_SCRAPING_IMPLEMENTATION.md](WEB_SCRAPING_IMPLEMENTATION.md)** - Web scraping capabilities
- **[docs-src/docs/sys/net.md](../../../docs-src/docs/sys/net.md)** - User API documentation
- **[docs-src/docs/use_cases/web_data_scraping.md](../../../docs-src/docs/use_cases/web_data_scraping.md)** - Web scraping examples
