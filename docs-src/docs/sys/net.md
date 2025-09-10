# $net

/*
Thread Safety:
Grapa is fully thread safe in all supported environments (command line, Grapa shell, and Python/GrapaPy). All built-in operations—including map, filter, reduce, $thread, and $net—are safe to use concurrently. Users do not need to take any special precautions for thread safety in these environments.

*Note: Only if Grapa is integrated directly into a non-thread-safe C++ host (not anticipated for normal users) would additional thread safety considerations arise.*
*/

Also see [$OBJ document](../obj/document.md)

Provides a socket library, cross functional with all platforms supported. 

## Thread Safety and Concurrency Patterns

### **Critical: Use `$local` Variables in Message Handlers**

Network message handlers run in concurrent contexts and **must** use `$local` variables to avoid race conditions:

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
        
        /* Use $local for all variables to avoid race conditions */
        $local.datasplit = netSession.data.split(" ");
        $local.method = datasplit[0];
        $local.path = datasplit[1];
        $local.response = "";
        
        /* Process request and generate response */
        if (path == "/") {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Server</h1>";
        } else {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        };
        
        netSession.data = "";
        netSession.send(response);
    };
};

/* ❌ INCORRECT - Thread-unsafe (race conditions) */
badMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Global variables - causes race conditions in concurrent execution */
        request = netSession.data;  // ❌ Race condition!
        method = request.left(3);   // ❌ Race condition!
        path = "";                  // ❌ Race condition!
        
        /* Process request... */
    };
};
```

### **Connection Management Patterns**

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

### **Concurrent Client Testing**

Demonstrate true concurrency using `.map()`:

```grapa
/* ✅ CORRECT - True concurrent execution */
concurrent_results = [testHttpClient, testHttpJson, testHttpHeaders, testHttpPost]
    .map(op(x) { @x().len(); });

/* ❌ INCORRECT - Sequential execution */
http_test1 = testHttpClient();  /* Sequential */
http_test2 = testHttpJson();    /* Sequential */
http_test3 = testHttpHeaders(); /* Sequential */
http_test4 = testHttpPost();    /* Sequential */
```

### **HTTP Parsing Best Practices**

Use proper string parsing methods:

```grapa
/* ✅ CORRECT - Proper HTTP parsing */
$local.datasplit = netSession.data.split(" ");
$local.method = datasplit[0];
$local.path = datasplit[1];

/* ❌ INCORRECT - Manual parsing prone to errors */
method = request.left(3);  /* Assumes 3-character method */
path_start = request.find(" ", 4);  /* Complex manual parsing */
```

## type()
Returns $net.

## mac()
Returns information about the primary network interface, including its MAC address and IP address.

**Returns:**
- `name`: Interface name (e.g., "en0", "Ethernet")
- `ip`: Primary IP address of the interface
- `mac`: MAC address in hexadecimal format

**Example:**
```grapa
mac_info = $net().mac();
mac_info.echo();
/* Output example:
{
  "name": "en0",
  "ip": "192.168.4.95", 
  "mac": "0x389B0CFFFEFB1C49"
}
*/
```

**Use Cases:**
- **Network Identification**: Get the primary network interface details
- **MAC Address Retrieval**: Extract the MAC address for network identification
- **IP Address Discovery**: Get the primary IP address of the current machine
- **Network Diagnostics**: Verify network interface configuration

## interfaces()
Returns detailed information about all network interfaces on the system.

**Returns:**
Array of interface objects, each containing:
- `name`: Interface name (e.g., "en0", "bridge100", "awdl0")
- `mac`: MAC address in hexadecimal format
- `family`: Address family ("IPV4" or "IPV6")
- `address`: IP address for this interface/family combination

**Example:**
```grapa
interfaces = $net().interfaces();
interfaces.echo();
/* Output example:
[
  {
    "name": "en0",
    "mac": "389B0CFFFEFB1C49",
    "family": "IPV6", 
    "address": "fe80::10e1:9bf8:150d:9609"
  },
  {
    "name": "en0",
    "mac": "389B0CFFFEFB1C49",
    "family": "IPV4",
    "address": "192.168.4.95"
  },
  {
    "name": "bridge100",
    "mac": "603E5FFFFEA87664", 
    "family": "IPV4",
    "address": "10.211.55.2"
  }
]
*/
```

**Use Cases:**
- **Network Discovery**: List all available network interfaces
- **Multi-Interface Support**: Find interfaces for specific network types (WiFi, Ethernet, VPN)
- **IPv6 Support**: Identify interfaces with IPv6 addresses
- **Virtual Interfaces**: Discover bridge, tunnel, and virtual interfaces
- **Network Configuration**: Verify interface setup and addressing

## connect(url, cert, proxy)
Establishes a network connection with optional SSL certificate and proxy support.

**Parameters:**
- `url`: Connection string in format `hostname:port` (e.g., "httpbin.org:80")
- `cert`: Path to SSL certificate file (optional, for HTTPS)
- `proxy`: Proxy configuration object (optional)

**Proxy Configuration:**
```grapa
// Simple proxy string
n2.connect("example.com:443", null, "http://proxy.example.com:8080");

// Detailed proxy object
proxy = {
    type: "HTTP",
    host: "proxy.example.com",
    port: "8080"
};
n2.connect("example.com:443", null, proxy);
```

**SSL Certificate Support:**
```grapa
// HTTPS with client certificate
n2.connect("example.com:443", "keys/client.crt", null);

// HTTPS with certificate and proxy
n2.connect("example.com:443", "keys/client.crt", proxy);
```

## bind(url)
Using bind/listen requires setting up at least 2 net objects. One for bind and the other for listen. See listen().
```
n1 = $net();
n1.bind(':12345');
```

## listen(net)
Pass in a bound net object. Listen is blocking, so a $thread may be needed to manage - or separate threads if more than one listener.

For those familiar with using sockets in C++, the $net class is a representation of sockets, and use bind/listen in the same way.

```
n1 = $net();
n1.bind(':12345');
n2 = $net();
n2.listen('@n1');
```

## onlisten(url,messageHandler [,connectHandler [,count:1]])

Sets up a multi-threaded server that can handle multiple concurrent connections. Each connection runs in its own thread, making it perfect for building scalable network services.

**How it works:**
1. **Background Thread**: The listener runs in a background thread, waiting for incoming connections
2. **Connection Acceptance**: When a client connects, a new network object and thread are automatically created
3. **Handler Execution**: The `connectHandler` is called first to initialize the connection, then `messageHandler` is called for each message
4. **Automatic Restart**: After each connection closes, the listener automatically restarts to accept new connections

**Parameters:**
- `url`: Server URL to bind to (e.g., ":8080", "localhost:12345", "https://:8443")
- `messageHandler`: Lambda function called for each received message
- `connectHandler`: Lambda function called when a new connection is established (optional)
- `count`: Number of concurrent listeners (currently fixed to 1, but the system handles multiple connections per listener)

**Handler Parameters:**
- `connectHandler(netSession)`: Called once per connection
  - `netSession`: Network session object for the new connection
- `messageHandler(netSession, message, hasmore)`: Called for each message
  - `netSession`: Network session object (persists across messages)
  - `message`: Received message data
  - `hasmore`: Flag indicating if more data is available (0 = complete message)

**Thread Safety:**
Grapa is fully thread-safe, so each connection thread can safely:
- Access shared data structures
- Use global variables
- Call other Grapa functions
- Perform database operations

**Example - Multi-Connection HTTP Server:**
```grapa
/* Process incoming HTTP requests */
processRequest = op(requestData) {
    /* Parse HTTP request and generate response */
    if (requestData.left(3) == "GET") {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello from Grapa Server!</h1>";
    } else {
        return "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
    };
};

/* Handle new connections */
connectionHandler = op(netSession) {
    netSession.data = "";
    netSession.connectionId = Math.random().int();
    ("New connection established: " + netSession.connectionId + "\n").echo();
};

/* Handle incoming messages */
messageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Complete message received */
        response = processRequest(netSession.data);
        netSession.send(response);
        
        /* Log the request */
        ("Connection " + netSession.connectionId + " processed request\n").echo();
        
        /* Reset for next request */
        netSession.data = "";
    };
};

/* Start the server */
server = $net();
server.onlisten(':8080', messageHandler, connectionHandler);

/* Server is now running and handling multiple connections */
("HTTP server started on port 8080\n").echo();
("Each connection runs in its own thread\n").echo();

/* IMPORTANT: Always disconnect when done to prevent segmentation faults */
/* server.disconnect(); */
```

**Example - Complete Working Server/Client Test:**
```grapa
/* Complete working example with proper cleanup */
processPost = op(in) {
    {processed: in};
};

postHandler = op(in) {
    $local.data = in.split("\r").join("");
    $local.len = data.len() - data.split("\n\n")[0].len() - 2;
    if (len < 0) len = 0;
    $local.body = data.right(len);
    $local.rstr = processPost(body).str();
    "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\nContent-Length: " + rstr.len().str() + "\r\n\r\n" + rstr;
};

postConnectHandler = op(netSession) {
    netSession.data = "";
};

postMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    if (hasmore == 0) {
        netSession.send(postHandler(netSession.data));
        netSession.data = "";
    };
};

/* Start server */
server = $net();
server.onlisten('0.0.0.0:12345', postMessageHandler, postConnectHandler);
("Server started on 0.0.0.0:12345\n").echo();

/* Wait for server to start */
Sleep(1000);

/* Test client connection */
client = $net();
client.connect('localhost:12345');
client.send('POST / HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{test:123}');
response = client.receive();
response.echo();
client.disconnect();

/* Show connection details */
server.host().echo();
"\n".echo();

/* CRITICAL: Clean up server to prevent segmentation fault */
server.disconnect();
("Test completed successfully\n").echo();

**Example - HTTPS Server with SSL/TLS:**
```grapa
/* HTTPS server with SSL certificate */
httpsServer = $net();

/* Configure SSL server certificate */
serverCert = $file().get("keys/server.crt");
serverKey = $file().get("keys/server.key");
httpsServer.certificate(serverCert);
httpsServer.private(serverKey);

/* Handle HTTPS connections */
httpsMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Process HTTPS request */
        if (netSession.data.left(3) == "GET") {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Secure Hello from Grapa HTTPS Server!</h1>";
        } else {
            response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        };
        
        netSession.send(response);
        netSession.data = "";
    };
};

/* Start HTTPS server */
httpsServer.onlisten(':8443', httpsMessageHandler);

("HTTPS server started on port 8443\n").echo();
("Using SSL certificate: keys/server.crt\n").echo();
("Using private key: keys/server.key\n").echo();

/* Remember to disconnect when done */
/* httpsServer.disconnect(); */
```

**Example - HTTP and HTTPS Dual Server:**
```grapa
/* Universal message handler for both HTTP and HTTPS */
universalHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Detect if this is an HTTPS connection */
        isSecure = netSession.ssl || false;
        protocol = isSecure ? "HTTPS" : "HTTP";
        
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello from Grapa " + protocol + " Server!</h1>";
        netSession.send(response);
        netSession.data = "";
    };
};

/* HTTP Server */
httpServer = $net();
httpServer.onlisten(':8080', universalHandler);

/* HTTPS Server */
httpsServer = $net();
serverCert = $file().get("keys/server.crt");
serverKey = $file().get("keys/server.key");
httpsServer.certificate(serverCert);
httpsServer.private(serverKey);
httpsServer.onlisten(':8443', universalHandler);

("Dual server started:").echo();
("  HTTP:  http://localhost:8080").echo();
("  HTTPS: https://localhost:8443").echo();
```

**Example - Simple Echo Server:**
```grapa
/* Echo server that handles multiple clients */
echoHandler = op(netSession, message, hasmore) {
    if (hasmore == 0) {
        /* Echo the message back to the client */
        netSession.send("Echo: " + message);
    };
};

/* Start echo server */
echoServer = $net();
echoServer.onlisten(':12345', echoHandler);

("Echo server started on port 12345").echo();
("Multiple clients can connect simultaneously").echo();
```

**Example - JSON API Server:**
```grapa
/* API endpoint handlers */
apiHandlers = {
    "/users": op() { return {users: ["alice", "bob", "charlie"]}; },
    "/status": op() { return {status: "running", uptime: 3600}; }
};

/* Process API requests */
processApiRequest = op(requestData) {
    /* Parse JSON request */
    try {
        request = requestData.parse();
        endpoint = request.path || "/";
        
        if (apiHandlers[endpoint]) {
            response = apiHandlers[endpoint]();
            return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" + response.str();
        } else {
            return "HTTP/1.1 404 Not Found\r\n\r\n";
        };
    } catch (e) {
        return "HTTP/1.1 400 Bad Request\r\n\r\n";
    };
};

/* API server message handler */
apiMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        response = processApiRequest(netSession.data);
        netSession.send(response);
        netSession.data = "";
    };
};

/* Start API server */
apiServer = $net();
apiServer.onlisten(':3000', apiMessageHandler);

("JSON API server started on port 3000").echo();
("Handling multiple concurrent API requests").echo();
```

**SSL/TLS Server Configuration:**
```grapa
/* Configure SSL server with certificate and private key */
sslServer = $net();

/* Set server certificate (PEM format) */
serverCert = $file().get("keys/server.crt");
sslServer.certificate(serverCert);

/* Set private key (PEM format) */
serverKey = $file().get("keys/server.key");
sslServer.private(serverKey);

/* Optional: Set password callback for encrypted private key */
passwordCallback = op() { return "server_key_password"; };
encryptedKey = $file().get("keys/server_encrypted.key");
sslServer.private(encryptedKey, passwordCallback);

/* Optional: Set trusted CA certificates for client verification */
caCert = $file().get("keys/ca.crt");
sslServer.trusted(caCert, "keys/ca/");

/* Start HTTPS server */
sslServer.onlisten(':8443', messageHandler);
```

**Performance Characteristics:**
- **Concurrent Connections**: Each connection runs in its own thread
- **Scalability**: Can handle hundreds of simultaneous connections
- **Memory Efficiency**: Threads are created on-demand and cleaned up automatically
- **Thread Safety**: Full thread safety for all Grapa operations
- **Automatic Management**: No manual thread management required
- **SSL/TLS Support**: Full HTTPS support with server certificates

This makes Grapa's `onlisten` perfect for building:
- **Web servers** and **API servers** (HTTP and HTTPS)
- **Chat applications** and **real-time services**
- **Game servers** and **multiplayer applications**
- **Data processing services** with concurrent clients
- **Microservices** and **backend APIs**
- **Secure applications** requiring SSL/TLS encryption

**⚠️ Important: Thread Cleanup**

When using `onlisten()`, **always call `disconnect()`** on the network object before your script exits to prevent segmentation faults:

```grapa
/* Start server */
server = $net();
server.onlisten(':8080', messageHandler, connectionHandler);

/* ... your server code ... */

/* CRITICAL: Clean up background threads */
server.disconnect();
```

**Why this is necessary:**
- `onlisten()` creates background threads for handling connections
- Without proper cleanup, these threads can cause segmentation faults when the script exits
- The `disconnect()` method properly terminates the background threads

## disconnect()
Closes the network connection.

## proxy(type, id, ip)
Configures proxy settings for the connection.

**Parameters:**
- `type`: Proxy type (e.g., "HTTP", "SOCKS")
- `id`: Proxy identifier (host:port)
- `ip`: Proxy IP address (host:port)

**Example:**
```grapa
n2 = $net();
n2.proxy("HTTP", "proxy.example.com:8080", "proxy.example.com:8080");
n2.connect("target.com:80", null, null);
```

## certificate(certData)
Sets the SSL certificate data for HTTPS connections. This configures the client certificate to present to the server.

**Parameters:**
- `certData`: Certificate data (PEM or DER format)

**Example:**
```grapa
n2 = $net();
certData = $file().get("keys/client.crt");
n2.certificate(certData);
n2.connect("example.com:443", null, null);
```

**Use Cases:**
- **Client Authentication**: Present a client certificate to the server
- **Mutual TLS**: Enable two-way SSL authentication
- **Server Setup**: Configure server certificates for HTTPS servers

## private(keyData, passOp, passParam)
Sets the private key data with optional password callback for HTTPS connections.

**Parameters:**
- `keyData`: Private key data (PEM or DER format)
- `passOp`: Lambda function for password callback (optional)
- `passParam`: Parameter passed to password callback (optional)

**Example:**
```grapa
/* Simple private key without password */
n2 = $net();
keyData = $file().get("keys/private.key");
n2.private(keyData, null, null);
n2.connect("example.com:443", null, null);
```

**Example with Password:**
```grapa
/* Private key with password callback */
passwordCallback = op() { return "mypassword"; };
n2 = $net();
keyData = $file().get("keys/private.key");
n2.private(keyData, passwordCallback, null);
n2.connect("example.com:443", null, null);
```

**Use Cases:**
- **Client Authentication**: Provide private key for client certificate
- **Server Setup**: Configure server private key for HTTPS servers
- **Password Protection**: Handle encrypted private keys

## trusted(caData, caPath)
Sets trusted CA certificate data and path for SSL verification. This configures which certificate authorities are trusted for verifying server certificates.

**Parameters:**
- `caData`: CA certificate data (PEM or DER format, optional)
- `caPath`: Path to CA certificate directory (optional)

**Example:**
```grapa
n2 = $net();
caData = $file().get("keys/ca.crt");
n2.trusted(caData, "keys/ca/");
n2.connect("example.com:443", null, null);
```

**Example with CA Path Only:**
```grapa
n2 = $net();
n2.trusted("", "/etc/ssl/certs/");  // Use system CA directory
n2.connect("example.com:443", null, null);
```

**Use Cases:**
- **Custom CA**: Trust a specific certificate authority
- **Self-Signed Certificates**: Trust your own CA for internal certificates
- **System Integration**: Use system CA certificates
- **Security**: Control which certificate authorities are trusted

**Complete SSL/TLS Example:**
```grapa
/* Configure SSL/TLS with certificates */
n2 = $net();

/* Set up trusted CA certificates */
caData = $file().get("keys/ca.crt");
n2.trusted(caData, "keys/ca/");

/* Configure client certificate and private key */
certData = $file().get("keys/client.crt");
keyData = $file().get("keys/client.key");
n2.certificate(certData);
n2.private(keyData, null, null);

/* Connect to HTTPS server */
n2.connect("example.com:443", null, null);

/* Verify server certificate */
verify_result = n2.verify();
if (verify_result.X509_error == 0) {
    ("SSL verification successful\n").echo();
    
    /* Decode server certificate */
    if (verify_result.X509_certificate.len() > 0) {
        cert_info = verify_result.X509_certificate.decode("PEM");
        ("Server certificate subject: " + cert_info.subject_name + "\n").echo();
        ("Server certificate issuer: " + cert_info.issuer_name + "\n").echo();
    };
} else {
    ("SSL verification failed: " + verify_result.X509_error + "\n").echo();
};

/* Get certificate chain */
chain_result = n2.chain();
("Certificate chain length: " + chain_result.len() + "\n").echo();

n2.disconnect();
```

## verify()
Returns SSL certificate verification information after connecting to an HTTPS server.

**Returns:**
- `X509_error`: SSL verification error code (0 = success, non-zero = error)
- `X509_certificate`: PEM-encoded server certificate (empty if no certificate)

**Example:**
```grapa
n2 = $net();
n2.connect("https://example.com:443", null, null);
verify_result = n2.verify();
verify_result.X509_error.echo();  // 0 = success
verify_result.X509_certificate.echo();  // Certificate in PEM format

/* Decode certificate details */
if (verify_result.X509_certificate.len() > 0) {
    cert_info = verify_result.X509_certificate.decode("PEM");
    cert_info.echo();  // Shows subject_name, issuer_name, etc.
};
n2.disconnect();
```

**Certificate Decoding:**
Use `.decode("PEM")` on the certificate to extract readable information:
- `subject_name`: Certificate subject (who it was issued to)
- `issuer_name`: Certificate issuer (who issued it)

## chain()
Returns the SSL certificate chain from the server.

**Returns:**
- List of certificates in the chain (PEM format)

**Example:**
```grapa
n2 = $net();
n2.connect("https://example.com:443", null, null);
chain_result = n2.chain();
chain_result.echo();  // Certificate chain

/* Decode individual certificates in the chain */
if (chain_result.len() > 0) {
    i = 0;
    while (i < chain_result.len()) {
        cert_info = chain_result[i].decode("PEM");
        ("Certificate " + i + ":").echo();
        cert_info.echo();
        i += 1;
    };
};
n2.disconnect();
```

**Certificate Chain:**
- Returns all certificates in the server's certificate chain
- Each certificate can be decoded with `.decode("PEM")`
- Useful for verifying the complete trust chain

## host()
Returns detailed connection information for the current network object.

**Returns:**
- `url`: Original connection URL or binding address
- `host`: Local hostname
- `ip`: Local IP address
- `port`: Port number
- `family`: Address family (2 = IPv4, 10 = IPv6)
- `connected`: Connection status (1 = connected, 0 = not connected)
- `bound`: Binding status (1 = bound, 0 = not bound)

**Example - Server Connection Info:**
```grapa
n2 = $net();
n2.bind('0.0.0.0:12345');
host_info = n2.host();
host_info.echo();
/* Output example:
{
  "url": "0.0.0.0:12345",
  "host": "Chris-MacBook-Pro.local",
  "ip": "192.168.4.95",
  "port": 12345,
  "family": 2,
  "connected": 0,
  "bound": 1
}
*/
```

**Example - Client Connection Info:**
```grapa
n2 = $net();
n2.connect('localhost:8080', null, null);
host_info = n2.host();
host_info.echo();
/* Output example:
{
  "url": "localhost:8080",
  "host": "localhost",
  "ip": "127.0.0.1",
  "port": 8080,
  "family": 2,
  "connected": 1,
  "bound": 0
}
*/
```

**Use Cases:**
- **Debugging**: Check connection status and binding information
- **Logging**: Record connection details for troubleshooting
- **Validation**: Verify network object state before operations
- **Monitoring**: Track connection status in server applications

## send(data)
Sends data over the network connection.

**Parameters:**
- `data`: Data to send (string or bytes)

**Example:**
```grapa
n2 = $net();
n2.connect('localhost:12345');
n2.send('Hello, World!');
n2.disconnect();
```

See example in onlisten.

NOTE: If using SSL and the remote system breaks the connection, first send will return no error. The second send will return an error. This is not the case for read - where the first read after a broken connection will return an error and close the connection.

## receive()
**⚠️ Important:** This method **blocks indefinitely** if there is no data pending to receive. Always check `.pending()` first to avoid blocking.

**Returns:**
- Received data as string

**Example:**
```grapa
n2 = $net();
n2.connect('localhost:12345');

/* Check if data is available before receiving */
pending = n2.pending();
if (pending > 0) {
    data = n2.receive();
    data.echo();
} else {
    "No data available to receive".echo();
};

n2.disconnect();
```

**Blocking Behavior:**
- If `.pending()` returns 0, `.receive()` will block indefinitely
- If `.pending()` returns > 0, `.receive()` will return the available data
- Use `.pending()` to check for available data before calling `.receive()`
- Alternative: Use `.onreceive()` for non-blocking asynchronous handling

**When to Use Each Method:**

| Method | Use When | Pros | Cons |
|--------|----------|------|------|
| `.receive()` | Simple, single-shot data retrieval | Simple to use | Blocks if no data |
| `.onreceive()` | Continuous data processing, buffering | Non-blocking, automatic buffering | More complex setup |
| `.pending()` + `.receive()` | Manual control over data retrieval | Full control | Requires polling |

**Best Practices:**
- **Use `.onreceive()`** for servers, real-time applications, or when you need to process data as it arrives
- **Use `.receive()`** only when you know data is available (after checking `.pending()`)
- **Use `.pending()`** to check data availability before calling `.receive()`

See example in onlisten.

## pending()
Returns the number of bytes that can be received without blocking.

**⚠️ Important:** Always check this before calling `.receive()` to avoid blocking indefinitely.

If an SSL connection, the byte count will be what is pending in the SSL encrypted buffer, which is different than the unencrypted data. 

**Returns:**
- Number of bytes available for reading (0 = no data available)

**Example:**
```grapa
n2 = $net();
n2.connect('localhost:12345');

/* Check for available data */
bytes_available = n2.pending();
if (bytes_available > 0) {
    data = n2.receive();
    data.echo();
} else {
    "No data available".echo();
};

n2.disconnect();
```

**Usage Pattern:**
```grapa
/* Safe pattern to avoid blocking */
pending = n2.pending();
if (pending > 0) {
    data = n2.receive();  /* Safe to call - data is available */
} else {
    /* No data available - don't call receive() */
};
```

See example in onlisten.

## onreceive(handler)
Sets up an asynchronous receive handler that will be called when data is received.

**🚀 Key Advantage:** Automatically handles incoming data without blocking, perfect for buffering and processing data as it arrives.

**Parameters:**
- `handler`: Lambda function to handle received data

**Handler Parameters:**
- `netSession`: Network session object
- `message`: Received message data
- `hasmore`: Flag indicating if more data is available (0 = complete message)

**Why Use onreceive() Instead of receive():**
- **Non-blocking**: Never blocks the main thread
- **Automatic buffering**: Automatically collects incoming data
- **Real-time processing**: Processes data as soon as it arrives
- **No manual polling**: No need to check `.pending()` repeatedly

**Example - Simple Data Collection:**
```grapa
receiveHandler = op(netSession, message, hasmore)
{
	netSession.data += message;
	if (hasmore == 0)
	{
		("Received complete message: " + netSession.data).echo();
		netSession.data = "";
	};
};

n2 = $net();
n2.connect('localhost:12345');
n2.onreceive(receiveHandler);
n2.send('Hello, World!');
n2.disconnect();
```

**Example - Advanced Data Processing:**
```grapa
/* Buffer for collecting data */
dataBuffer = "";

receiveHandler = op(netSession, message, hasmore)
{
	/* Automatically collect incoming data */
	dataBuffer += message;
	
	if (hasmore == 0)
	{
		/* Process complete message */
		("Processing complete message of " + dataBuffer.len() + " bytes").echo();
		
		/* Parse and process the data */
		if (dataBuffer.left(3) == "GET") {
			("HTTP GET request received").echo();
		} else if (dataBuffer.left(4) == "POST") {
			("HTTP POST request received").echo();
		};
		
		/* Clear buffer for next message */
		dataBuffer = "";
	};
};

n2 = $net();
n2.connect('localhost:12345');
n2.onreceive(receiveHandler);
n2.send('GET / HTTP/1.1\r\nHost: localhost\r\n\r\n');
n2.disconnect();
```

**Example - JSON Data Processing:**
```grapa
/* Buffer for JSON data */
jsonBuffer = "";

jsonHandler = op(netSession, message, hasmore)
{
	jsonBuffer += message;
	
	if (hasmore == 0)
	{
		/* Try to parse complete JSON */
		try {
			jsonData = jsonBuffer.parse();
			("Received JSON: " + jsonData.str()).echo();
		} catch (e) {
			("Invalid JSON received: " + jsonBuffer).echo();
		};
		
		jsonBuffer = "";
	};
};

n2 = $net();
n2.connect('localhost:12345');
n2.onreceive(jsonHandler);
n2.send('{"name":"test","value":123}');
n2.disconnect();
```

## httpread()
Reads HTTP response data from the connection.

## httpsend(method,entity,header,body)
Sends an HTTP request using the specified method, entity (path), headers, and body.

**Parameters:**
- `method`: HTTP method (GET, POST, PUT, DELETE, etc.)
- `entity`: HTTP path (e.g., "/api/data")
- `header`: Object containing HTTP headers
- `body`: Request body content

**Header Support:**
- **Cookie Headers**: Automatically formatted with `; ` separator
- **Array/Tuple Headers**: Multiple values joined with `, ` separator
- **List Headers**: Name-value pairs properly formatted

**Example:**
```grapa
n2 = $net();
n2.connect("httpbin.org:80", null, null);
header = {
    "Host": "httpbin.org", 
    "User-Agent": "Grapa-HTTP/1.0",
    "Cookie": ["session=abc123", "user=john"],
    "Accept": ["text/html", "application/json"]
};
n2.httpsend("GET", "/get", header, "");
```

## httpmessage([raw])
Parses HTTP response and automatically detects content type. Returns structured response with status, headers, and parsed body.

**Returns:**
- `status`: Object with `code`, `description`, `version`
- `headers`: Object containing response headers
- `body`: Parsed response body (JSON, HTML, XML, or plain text)

**Content Type Detection:**
The C++ implementation automatically detects and parses:
- **JSON** (`application/json`, `text/json`) → Grapa objects
- **HTML** (`text/html`) → Structured HTML objects
- **XML** (`text/xml`, `application/xml`, `application/xhtml+xml`) → Structured XML objects
- **Plain Text** (`text/plain`) → Raw string

**Cookie Parsing:**
- **Set-Cookie Headers**: Automatically parsed into structured format
- **Multiple Cookies**: Properly separated and stored as name-value pairs

**Chunked Transfer Encoding:**
- **Automatic Detection**: Detects `Transfer-Encoding: chunked`
- **RFC 7230 Compliance**: Full chunked encoding parsing
- **Hex Length Parsing**: Correctly parses chunk sizes in hexadecimal
- **Automatic Assembly**: Reassembles chunked response bodies

**Example:**
```grapa
n2 = $net();
n2.connect("httpbin.org:80", null, null);
header = {"Host": "httpbin.org"};
n2.httpsend("GET", "/get", header, "");
response = n2.httpmessage();
response.status.code.echo();  // 200
response.body.echo();         // Parsed JSON object
n2.disconnect();
```

## HTTP/HTTPS Complete Example
See [curl function example](../examples/curl_function_simple.grc) for a complete HTTP/HTTPS client implementation.

**HTTPS with SSL Certificates:**
```grapa
n2 = $net();
n2.connect("https://example.com:443", "keys/client.crt", null);
header = {"Host": "example.com"};
n2.httpsend("GET", "/api/data", header, "");
response = n2.httpmessage();
n2.disconnect();
```

## Supported Features

### ✅ HTTP/HTTPS Capabilities
- **HTTP Methods**: GET, POST, PUT, DELETE, HEAD, OPTIONS, PATCH
- **HTTPS/SSL**: Full SSL/TLS support with client certificates
- **Proxy Support**: HTTP proxy with authentication
- **Cookie Handling**: Automatic parsing and sending of cookies
- **Chunked Transfer Encoding**: RFC 7230 compliant
- **Content Type Detection**: Automatic JSON/HTML/XML parsing
- **Custom Headers**: Full header customization support
- **Request Bodies**: Support for any content type
- **Certificate Passwords**: Dynamic password callback support
- **CA Certificate Verification**: Trusted CA certificate support

### ✅ SSL/TLS Features
- **Client Certificates**: PEM format certificate files
- **Private Keys**: Password-protected private key support
- **Certificate Chains**: Full certificate chain validation
- **CA Verification**: Custom CA certificate and path support
- **SSL Modes**: Automatic SSL retry and error handling

### ✅ Proxy Features
- **HTTP Proxy**: Full HTTP proxy support
- **Proxy Authentication**: Basic authentication support
- **Proxy Configuration**: Both string and object formats
- **Multiple Proxy Types**: Extensible proxy type system

## Not Supported Features

### ❌ HTTP/HTTPS Limitations
- **HTTP/2**: No HTTP/2 protocol support
- **HTTP/3**: No HTTP/3 protocol support
- **Multipart/Form-Data**: No multipart form data encoding/decoding
- **Compression**: No automatic gzip/deflate/brotli decompression
- **Rate Limiting**: No built-in rate limiting
- **Resume Downloads**: No partial content/resume support
- **SOCKS Proxy**: Only HTTP proxy supported (SOCKS not implemented)
- **Custom DNS**: No DNS override or custom resolution
- **Certificate Pinning**: No HPKP (HTTP Public Key Pinning) support
- **HTTP Authentication**: Only Basic auth supported (Digest, NTLM not implemented)

### ❌ Advanced Features
- **WebSocket**: No WebSocket protocol support
- **HTTP/2 Server Push**: Not applicable (no HTTP/2)
- **HTTP/3 QUIC**: Not applicable (no HTTP/3)
- **Streaming Responses**: No streaming response handling
- **Connection Pooling**: No connection reuse/pooling
- **Keep-Alive Management**: Basic support only
- **HTTP Caching**: No cache control or ETag support

### ❌ Certificate Management Limitations
- **Certificate Generation**: No CSR (Certificate Signing Request) creation
- **Self-Signed Certificates**: No self-signed certificate generation
- **Certificate Renewal**: No certificate renewal workflows
- **Certificate Lifecycle**: No expiration monitoring or rotation
- **Certificate Format Conversion**: No PEM ↔ DER conversion
- **Certificate Revocation**: No CRL (Certificate Revocation List) or OCSP checking
- **Certificate Fingerprints**: No fingerprint generation
- **Certificate Storage**: No certificate store management
- **Advanced SSL/TLS**: No cipher suite selection or TLS version specification

### ✅ Certificate Data Support
- **Memory-Based Loading**: All certificate methods now accept certificate data directly instead of file paths
- **PEM Format Support**: Automatic detection and parsing of PEM format certificates
- **DER Format Support**: Support for binary DER format certificates
- **Certificate Chains**: Full certificate chain support in PEM format
- **Private Key Support**: Memory-based private key loading with password callback support
- **CA Certificate Support**: Memory-based CA certificate loading for custom trust stores
- **Certificate Pinning**: No certificate pinning support
- **SNI Support**: No Server Name Indication configuration

**Developer Responsibility:**
For certificate management tasks not covered by Grapa's `$net` class, developers should use external tools such as:
- **OpenSSL CLI**: For certificate generation, conversion, and validation
- **Certificate Management Tools**: For enterprise certificate lifecycle management
- **Custom Scripts**: For automated certificate monitoring and renewal workflows
