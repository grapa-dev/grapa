---
tags:
  - user
  - highlevel
  - networking
  - concurrency
  - thread-safety
---

# Network Programming Patterns in Grapa

This guide documents the critical patterns for writing thread-safe and concurrent network applications in Grapa, based on working examples and best practices.

## Overview

Grapa's networking capabilities support concurrent server/client operations, but require careful attention to thread safety. This guide covers the essential patterns for building robust network applications.

## Critical Thread Safety Requirements

### **1. Use `$local` Variables in Message Handlers**

**Network message handlers run concurrently and MUST use `$local` variables to avoid race conditions:**

```grapa
/* ✅ CORRECT - Thread-safe message handler */
httpMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Check for connection close */
        if (netSession.data.len() == 0) {
            ("Server:" + netSession.connectionId.str() + ": CLOSE\n").echo();
            netSession.disconnect();
            return(null);
        };
        
        /* ALL variables must be $local to avoid race conditions */
        $local.datasplit = netSession.data.split(" ");
        $local.method = datasplit[0];
        $local.path = datasplit[1];
        $local.response = "";
        
        /* Process request safely */
        if (path == "/") {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Server</h1>";
        } else {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        };
        
        netSession.data = "";
        netSession.send(response);
    };
};

/* ❌ DANGEROUS - Race conditions in concurrent execution */
badMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Global variables cause race conditions! */
        request = netSession.data;  // ❌ Race condition!
        method = request.left(3);   // ❌ Race condition!
        path = "";                  // ❌ Race condition!
        
        /* Multiple concurrent handlers will overwrite each other's variables */
    };
};
```

**Why this is critical:**
- Network message handlers run concurrently
- Global variables are shared across all concurrent executions
- Race conditions can cause data corruption, crashes, or incorrect behavior
- `$local` ensures each concurrent execution has its own variable instances

### **2. Connection Management Patterns**

#### **Proper Connection Disconnection**
Always check for empty data and disconnect appropriately:

```grapa
/* Check for connection close condition */
if (netSession.data.len() == 0) {
    ("Server:" + netSession.connectionId.str() + ": CLOSE\n").echo();
    netSession.disconnect();
    return(null);
};
```

#### **Connection ID Generation**
Use appropriate random generation for connection tracking:

```grapa
connectionHandler = op(netSession) {
    netSession.data = "";
    netSession.connectionId = 32.random();  /* ✅ Appropriate for connection IDs */
    ("New connection: " + netSession.connectionId + "\n").echo();
};
```

### **3. HTTP Parsing Best Practices**

Use proper string parsing methods instead of manual character-by-character parsing:

```grapa
/* ✅ CORRECT - Proper HTTP parsing */
$local.datasplit = netSession.data.split(" ");
$local.method = datasplit[0];
$local.path = datasplit[1];

/* ❌ INCORRECT - Manual parsing prone to errors */
method = request.left(3);  /* Assumes 3-character method */
path_start = request.find(" ", 4);  /* Complex manual parsing */
```

## Server Patterns

### **Basic HTTP Server Setup**

```grapa
/* Message handler with thread safety */
httpMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Check for connection close */
        if (netSession.data.len() == 0) {
            ("Server:" + netSession.connectionId.str() + ": CLOSE\n").echo();
            netSession.disconnect();
            return(null);
        };
        
        /* Parse HTTP request safely */
        $local.datasplit = netSession.data.split(" ");
        $local.method = datasplit[0];
        $local.path = datasplit[1];
        $local.response = "";
        
        /* Generate response based on path */
        if (path == "/") {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Server</h1>";
        } else if (path == "/json") {
            json_data = {"message": "Hello from Grapa", "server": "HTTP"};
            response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" + json_data.str();
        } else {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        };
        
        netSession.data = "";
        netSession.send(response);
    };
};

/* Connection handler */
connectionHandler = op(netSession) {
    netSession.data = "";
    netSession.connectionId = 32.random();
    ("New connection: " + netSession.connectionId + "\n").echo();
};

/* Start server */
server = $net();
server.onlisten(':8080', httpMessageHandler, connectionHandler);
```

### **HTTPS Server Setup**

```grapa
/* HTTPS server with certificate checking */
httpsServer = $net();
https_enabled = false;

/* Check if certificate files exist */
cert_file = $file("keys/server.crt");
key_file = $file("keys/server.key");

if (cert_file.exists() && key_file.exists()) {
    /* Note: HTTPS server setup requires additional implementation */
    ("HTTPS server certificates found but HTTPS server not implemented yet\n").echo();
    https_enabled = false;
} else {
    ("HTTPS server certificates not found\n").echo();
    https_enabled = false;
};
```

## Client Patterns

### **Basic HTTP Client**

```grapa
/* Thread-safe HTTP client function */
testHttpClient = op() {
    $local.n = $net();
    n.connect("localhost:8080", null, null);
    
    /* Send raw HTTP request */
    $local.request = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Grapa-Test-Client/1.0\r\n\r\n";
    n.send(request);
    
    /* Receive response */
    $local.response = n.receive();
    while (n.pending()) {
        response += n.receive();
    };
    
    n.disconnect();
    
    return response;
};
```

### **HTTPS Client with Certificates**

```grapa
/* HTTPS client with SSL certificate */
testHttpsClient = op() {
    $local.n = $net();
    n.connect("localhost:8443", "keys/client.crt", null);
    
    $local.request = "GET / HTTP/1.1\r\nHost: localhost:8443\r\n\r\n";
    n.send(request);
    
    $local.response = n.receive();
    while (n.pending()) {
        response += n.receive();
    };
    
    n.disconnect();
    
    return response;
};
```

## Concurrency Patterns

### **Grapa's Powerful Parallel Execution Model**

Grapa's `.map()` function provides a sophisticated parallel execution model that automatically handles completion and result collection:

```grapa
/* Execute 4 thread-safe functions in parallel */
concurrent_results = [testHttpClient, testHttpJson, testHttpHeaders, testHttpPost]
    .map(op(x) { @x().len(); });

/* What happens:
 * 1. All 4 functions start executing simultaneously
 * 2. Each function runs independently with its own $local variables
 * 3. Command waits for ALL functions to complete
 * 4. Returns array of results: [95, 97, 189, 172]
 * 5. Total execution time = slowest function, not sum of all
 */
```

**Real-world example from your test:**
```grapa
/* From network_server_client_test.grc */
concurrent_results = [testHttpClient,testHttpJson,testHttpHeaders,testHttpPost]
    .map(op(x){@x().len();});
("Concurrent Test - All response lengths: " + concurrent_results.str() + "\n").echo();
/* Output: Concurrent Test - All response lengths: [95,97,189,172] */
```

### **Concurrent Client Testing**

```grapa
/* ✅ CORRECT - True concurrent execution with automatic completion */
concurrent_results = [testHttpClient, testHttpJson, testHttpHeaders, testHttpPost]
    .map(op(x) { @x().len(); });

/* The command completes when ALL 4 functions complete */
/* Returns: [95, 97, 189, 172] - array of results from each function */

/* ❌ INCORRECT - Sequential execution */
http_test1 = testHttpClient();  /* Sequential - waits for each to complete */
http_test2 = testHttpJson();    /* Sequential - waits for each to complete */
http_test3 = testHttpHeaders(); /* Sequential - waits for each to complete */
http_test4 = testHttpPost();    /* Sequential - waits for each to complete */
```

**Key Benefits:**
- **Parallel Execution**: All 4 functions run simultaneously
- **Automatic Completion**: Command waits for ALL functions to finish
- **Result Collection**: Returns array of results in same order as input
- **Thread Safety**: Each function uses `$local` variables (no race conditions)
- **Performance**: Total time = slowest function, not sum of all functions

### **Parallel Processing**

```grapa
/* Process multiple URLs concurrently */
urls = ["http://example1.com", "http://example2.com", "http://example3.com"];
results = urls.map(op(url) {
    $local.client = $net();
    client.connect(url, null, null);
    
    $local.request = "GET / HTTP/1.1\r\nHost: " + url + "\r\n\r\n";
    client.send(request);
    
    $local.response = client.receive();
    while (client.pending()) {
        response += client.receive();
    };
    
    client.disconnect();
    return response.len();
});

/* All 3 URLs are processed in parallel */
/* Command completes when ALL 3 complete */
/* Returns: [response_length_1, response_length_2, response_length_3] */
```

## Network Information Patterns

### **Interface Discovery**

```grapa
/* Get MAC address */
mac_info = $net().mac();
("MAC Address: " + mac_info.mac.hex() + "\n").echo();

/* Get all interfaces */
interfaces = $net().interfaces();
("Network Interfaces: " + interfaces.len() + " found\n").echo();
```

### **Connection Information**

```grapa
/* Get connection details */
test_conn = $net();
test_conn.connect("localhost:8080", null, null);
conn_info = test_conn.host();
("Connection Info: " + conn_info.str() + "\n").echo();
test_conn.disconnect();
```

## Error Handling Patterns

### **Connection Error Handling**

```grapa
/* Handle connection failures gracefully */
testConnection = op(url) {
    $local.client = $net();
    $local.err = client.connect(url, null, null);
    
    if (err.type() == $ERR) {
        client.disconnect();
        return {"error": -1, "message": "Connection failed"};
    };
    
    /* Continue with request... */
    client.disconnect();
    return {"success": true};
};
```

### **Response Validation**

```grapa
/* Validate HTTP responses */
validateResponse = op(response) {
    if (response.len() == 0) {
        return {"error": -1, "message": "Empty response"};
    };
    
    if (response.left(9) != "HTTP/1.1 ") {
        return {"error": -1, "message": "Invalid HTTP response"};
    };
    
    return {"success": true, "response": response};
};
```

## Best Practices Summary

### **Thread Safety**
1. **Always use `$local` variables** in message handlers and concurrent functions
2. **Never use global variables** in concurrent contexts
3. **Declare all local variables upfront** for clarity

### **Connection Management**
1. **Check for empty data** and disconnect appropriately
2. **Use proper connection IDs** for tracking
3. **Always disconnect** connections when done

### **HTTP Processing**
1. **Use `.split()` for parsing** instead of manual character operations
2. **Validate responses** before processing
3. **Handle errors gracefully** with proper cleanup

### **Concurrency**
1. **Use `.map()` for concurrent execution** instead of sequential calls
2. **Automatic completion** - command waits for ALL parallel functions to finish
3. **Result collection** - returns array of results in same order as input functions
4. **Performance optimization** - total time equals slowest function, not sum of all
5. **Test with multiple concurrent connections** to verify thread safety
6. **Monitor for race conditions** in development

## Working Examples

See the following examples for complete implementations:

- **[Network Server/Client Test](../examples/network_server_client_test.grc)** - Comprehensive server/client with concurrency
- **[Simple Network Test](../examples/simple_network_test.grc)** - Basic server/client operations
- **[Curl Function](../examples/curl_function_simple.grc)** - HTTP/HTTPS client implementation

These examples demonstrate all the patterns described in this guide and serve as reference implementations for network programming in Grapa.
