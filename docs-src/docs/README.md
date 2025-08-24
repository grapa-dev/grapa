
# Examples

This directory contains practical examples demonstrating Grapa's capabilities.

## Available Examples

- [basic_example.grc](basic_example.grc)
- [advanced_example.grc](advanced_example.grc)
- [performance_example.grc](performance_example.grc)
- [database_examples.grc](database_examples.grc)
- [python_integration_example.py](python_integration_example.py)
- [curl_function_simple.grc](examples/curl_function_simple.grc) - Complete HTTP/HTTPS client implementation
- [network_server_client_test.grc](examples/network_server_client_test.grc) - Comprehensive server/client networking test

---

## HTTP/HTTPS Networking Example

The `curl_function_simple.grc` demonstrates Grapa's comprehensive HTTP/HTTPS capabilities:

- **Complete HTTP Client**: Full implementation of HTTP methods (GET, POST, PUT, DELETE)
- **SSL/TLS Support**: HTTPS with client certificates and private keys
- **Proxy Support**: HTTP proxy with both string and object configuration
- **Cookie Handling**: Automatic parsing and sending of cookies
- **Chunked Encoding**: RFC 7230 compliant transfer encoding
- **Content Parsing**: Automatic JSON/HTML/XML response parsing
- **Error Handling**: Comprehensive error detection and reporting

This example shows how to build a production-ready HTTP client using Grapa's built-in networking capabilities.

## Comprehensive Network Server/Client Test

The `network_server_client_test.grc` demonstrates Grapa's complete networking capabilities in a single self-contained script:

- **Multi-Threaded Servers**: HTTP and HTTPS servers handling multiple concurrent connections
- **SSL/TLS Server Support**: HTTPS server with server certificates
- **Client Testing**: Comprehensive client tests connecting to the servers
- **Concurrent Connections**: Tests multiple simultaneous connections
- **Network Interface Functions**: Tests MAC address and interface enumeration
- **Complete Validation**: Tests all major networking functions in one script
- **Proper Cleanup**: Demonstrates correct thread cleanup to prevent segmentation faults

This example validates that Grapa's networking capabilities work correctly by:
1. Starting HTTP and HTTPS servers on localhost
2. Running multiple client tests against both servers
3. Testing concurrent connections and network functions
4. Providing a complete validation report
5. Properly shutting down all connections with `disconnect()`

**⚠️ Important**: The example includes proper `disconnect()` calls to prevent segmentation faults that can occur when background threads aren't cleaned up properly.

Perfect for testing and demonstrating Grapa's networking capabilities!

---

## Example Code Viewer

???+ example "basic_example.grc"
    ```grapa
    /* Basic Grapa Example */
    /* This file demonstrates basic Grapa syntax and operations */

    /* Basic variable assignment */
    x = 42;

    /* Basic array operations */
    arr = [1, 2, 3, 4, 5];

    /* Basic string operations */
    str = "Grapa is awesome!";

    /* Basic math operations */
    result = 10 + 5 * 2;

    /* Return the result for testing */
    result 
    ```

???+ example "advanced_example.grc"
    ```grapa
    /* Advanced Grapa Example */
    /* This file demonstrates more complex Grapa operations */

    /* String processing */
    text = "Hello World Grapa Programming";
    words = text.split(" ");

    /* Array operations */
    numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

    /* Return results for testing */
    [words, numbers.len()] 
    ```

???+ example "performance_example.grc"
    ```grapa
    /* Performance Example */
    /* This file demonstrates performance-related operations */

    /* Simple loop using while */
    sum = 0;
    i = 0;
    while (i < 100) {
        sum = sum + i;
        i = i + 1;
    }

    /* Array creation */
    large_array = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

    /* Return results for testing */
    [sum, large_array.len()] 
    ```

???+ example "database_examples.grc"
    ```grapa
    /* Database Examples - Comprehensive Guide */
    /* This file demonstrates all three database types: ROW, COL, and GROUP */
    // ... (file is long, so consider truncating or linking to the full file if needed) ...
    ```
    [View full database_examples.grc](database_examples.grc)

---

## Database Examples

The `database_examples.grc` file demonstrates:
- **ROW databases**: Transactional workloads, user management
- **COL databases**: Analytical workloads, metrics and analytics
- **GROUP databases**: Hierarchical structures, nested databases
- **Cross-database operations**: Data integrity validation
- **Database type selection guidelines**: When to use each type

---

## Related topics
- [API Reference](../api_reference.md)
- [Use Cases](../use_cases/)
- [Language Reference](../syntax/basic_syntax.md)
- [Database Quick Reference](../database/quick_reference.md)
- [Database Storage Types](../database/column_store.md) 