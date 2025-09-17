# C++ Integration with Grapa

## Overview

Grapa provides powerful C++ integration capabilities through its universal development kit, offering a complete C++ API for embedding Grapa in applications. This integration combines Grapa's efficient runtime with C++'s performance and type safety for high-performance applications.

## Key Benefits

### 1. **High-Performance Embedding**
Grapa's C++ API provides direct access to the runtime with minimal overhead:

```cpp
#include "grapa/GrapaLink.h"

int main() {
    // Initialize Grapa runtime
    GrapaLink grapa;
    
    // Execute Grapa code with full performance
    GrapaValue result = grapa.eval("2 + 2");
    std::cout << "Result: " << result << std::endl;
    
    return 0;
}
```

**Advantages:**
- **Direct runtime access** with minimal overhead
- **Type-safe C++ interface** for Grapa values
- **Memory management** handled automatically
- **Exception safety** with proper error handling

### 2. **Complete API Access**

#### **Core Runtime Functions**
Full access to Grapa's core functionality:

```cpp
#include "grapa/GrapaLink.h"

int main() {
    GrapaLink grapa;
    
    // File system operations
    GrapaValue file = grapa.file();
    GrapaValue result = grapa.eval("file().ls('.')");
    
    // Database operations
    GrapaValue db = grapa.eval("{}.table('ROW')");
    grapa.eval("db.mkfield('name', 'STR')");
    
    // Mathematical operations
    GrapaValue math_result = grapa.eval("123456789012345678901234567890 * 987654321098765432109876543210");
    
    return 0;
}
```

#### **Advanced Features**
Access to Grapa's advanced capabilities:

```cpp
// Parallel processing
GrapaValue data = grapa.eval("(1000000).range(0,1)");
GrapaValue processed = grapa.eval("data.map(op(x) { x * x; }, 8)");

// Dynamic code execution
GrapaValue compiled = grapa.eval("$sys().compile('x * 2 + offset')");
GrapaValue result = grapa.eval("$sys().eval(compiled, {'x': 10, 'offset': 5})");

// String processing
GrapaValue text = grapa.eval("'Hello, World!'");
GrapaValue upper = grapa.eval("text.upper()");
```

### 3. **Universal Development Kit**

The universal development kit provides everything needed for C++ development:

```cpp
// Complete API headers
#include "grapa/GrapaLink.h"    // Main API
#include "grapa/GrapaValue.h"   // Value types
#include "grapa/GrapaType.h"    // Type system
#include "grapa/GrapaFile.h"    // File operations
#include "grapa/GrapaDatabase.h" // Database operations
```

**Kit Contents:**
- **Complete C++ API headers** in `include/grapa/`
- **Sample application** (`main.cpp`) demonstrating embedding
- **CMake build system** with cross-platform support
- **All platform libraries** (Windows, macOS, Linux, AWS)
- **Third-party libraries** (FLTK, OpenSSL, BLST, PCRE2)

## Getting Started

### 1. **Download and Extract**

```bash
# Download universal installer (recommended)
curl -O https://github.com/grapa-dev/grapa/releases/download/v0.1.55/install-grapa-0.1.55.py
python3 install-grapa-0.1.55.py

# Or download platform-specific package manually
# curl -L -o grapa-0.1.55-<platform>.zip https://github.com/grapa-dev/grapa/releases/download/v0.1.55/grapa-0.1.55-<platform>.zip
# unzip grapa-0.1.55-<platform>.zip
# cd grapa-0.1.55
```

### 2. **Build the Example**

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the example
./grapa_example  # On Unix/macOS
grapa_example.exe  # On Windows
```

### 3. **Your First C++ Application**

```cpp
#include "grapa/GrapaLink.h"
#include <iostream>

int main() {
    try {
        // Initialize Grapa runtime
        GrapaLink grapa;
        
        // Execute simple Grapa code
        GrapaValue result = grapa.eval("2 + 2");
        std::cout << "2 + 2 = " << result << std::endl;
        
        // Execute more complex code
        GrapaValue complex_result = grapa.eval("'Hello, ' + 'World!'.upper()");
        std::cout << "Complex result: " << complex_result << std::endl;
        
        // File operations
        GrapaValue file_result = grapa.eval("$file().ls('.')");
        std::cout << "Files in current directory: " << file_result << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Use Cases

### 1. **High-Performance Applications**

#### **Scientific Computing**
```cpp
#include "grapa/GrapaLink.h"

class ScientificCalculator {
private:
    GrapaLink grapa;
    
public:
    ScientificCalculator() {
        // Initialize with scientific computing functions
        grapa.eval(R"(
            // Define scientific functions
            factorial = op(n) {
                if (n <= 1) return 1;
                return n * factorial(n - 1);
            };
            
            fibonacci = op(n) {
                if (n <= 1) return n;
                return fibonacci(n - 1) + fibonacci(n - 2);
            };
        )");
    }
    
    double calculateFactorial(int n) {
        GrapaValue result = grapa.eval("factorial(" + std::to_string(n) + ")");
        return result.toDouble();
    }
    
    long long calculateFibonacci(int n) {
        GrapaValue result = grapa.eval("fibonacci(" + std::to_string(n) + ")");
        return result.toLongLong();
    }
};
```

#### **Data Processing Pipeline**
```cpp
#include "grapa/GrapaLink.h"
#include <vector>
#include <string>

class DataProcessor {
private:
    GrapaLink grapa;
    
public:
    DataProcessor() {
        // Initialize data processing functions
        grapa.eval(R"(
            // Data processing functions
            process_data = op(data) {
                // Grapa's parallel processing
                return data.map(op(x) { x * 2; }, 4);
            };
            
            filter_data = op(data, threshold) {
                return data.filter(op(x) { x > threshold; });
            };
        )");
    }
    
    std::vector<double> processVector(const std::vector<double>& input) {
        // Convert C++ vector to Grapa array
        std::string grapa_array = "[";
        for (size_t i = 0; i < input.size(); ++i) {
            if (i > 0) grapa_array += ", ";
            grapa_array += std::to_string(input[i]);
        }
        grapa_array += "]";
        
        // Process with Grapa
        GrapaValue result = grapa.eval("process_data(" + grapa_array + ")");
        
        // Convert back to C++ vector
        std::vector<double> output;
        // Implementation depends on GrapaValue API
        return output;
    }
};
```

### 2. **System Applications**

#### **Configuration Management**
```cpp
#include "grapa/GrapaLink.h"
#include <fstream>
#include <string>

class ConfigManager {
private:
    GrapaLink grapa;
    std::string config_path;
    
public:
    ConfigManager(const std::string& path) : config_path(path) {
        // Load configuration with Grapa
        grapa.eval("config = $file().get('" + config_path + "').json()");
    }
    
    std::string getString(const std::string& key) {
        GrapaValue result = grapa.eval("config." + key);
        return result.toString();
    }
    
    int getInt(const std::string& key) {
        GrapaValue result = grapa.eval("config." + key);
        return result.toInt();
    }
    
    bool getBool(const std::string& key) {
        GrapaValue result = grapa.eval("config." + key);
        return result.toBool();
    }
    
    void setString(const std::string& key, const std::string& value) {
        grapa.eval("config." + key + " = '" + value + "'");
        saveConfig();
    }
    
private:
    void saveConfig() {
        grapa.eval("$file().set('" + config_path + "', config.json())");
    }
};
```

#### **Log Processing**
```cpp
#include "grapa/GrapaLink.h"
#include <iostream>
#include <fstream>

class LogProcessor {
private:
    GrapaLink grapa;
    
public:
    LogProcessor() {
        // Initialize log processing functions
        grapa.eval(R"(
            // Log processing functions
            parse_log_line = op(line) {
                parts = line.split(' ');
                return {
                    'timestamp': parts[0] + ' ' + parts[1],
                    'level': parts[2],
                    'message': parts.slice(3).join(' ')
                };
            };
            
            filter_by_level = op(logs, level) {
                return logs.filter(op(log) { log.level == level; });
            };
        )");
    }
    
    void processLogFile(const std::string& filename) {
        // Read log file
        std::ifstream file(filename);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Process with Grapa
        grapa.eval("log_content = `" + content + "`");
        grapa.eval("log_lines = log_content.split('\\n')");
        grapa.eval("parsed_logs = log_lines.map(parse_log_line)");
        
        // Filter errors
        GrapaValue errors = grapa.eval("filter_by_level(parsed_logs, 'ERROR')");
        std::cout << "Found " << errors << " error entries" << std::endl;
    }
};
```

### 3. **Web Server Integration**

#### **HTTP Server with Grapa Backend**
```cpp
#include "grapa/GrapaLink.h"
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

class GrapaWebServer {
private:
    GrapaLink grapa;
    web::http::experimental::listener::http_listener listener;
    
public:
    GrapaWebServer(const std::string& url) : listener(url) {
        // Initialize Grapa with web functions
        grapa.eval(R"(
            // Web processing functions
            process_request = op(data) {
                // Process request data with Grapa
                return data.map(op(item) { item.upper(); });
            };
            
            validate_input = op(input) {
                return input.len() > 0 && input.len() < 1000;
            };
        )");
        
        // Set up HTTP endpoints
        listener.support(web::http::methods::POST, 
                        std::bind(&GrapaWebServer::handlePost, this, std::placeholders::_1));
    }
    
    void handlePost(web::http::http_request request) {
        request.extract_string().then([this, request](const std::string& body) {
            try {
                // Process with Grapa
                grapa.eval("input_data = `" + body + "`");
                GrapaValue result = grapa.eval("process_request(input_data)");
                
                // Send response
                web::json::value response;
                response["result"] = web::json::value::string(result.toString());
                request.reply(web::http::status_codes::OK, response);
                
            } catch (const std::exception& e) {
                web::json::value error;
                error["error"] = web::json::value::string(e.what());
                request.reply(web::http::status_codes::InternalError, error);
            }
        });
    }
    
    void start() {
        listener.open().wait();
    }
    
    void stop() {
        listener.close().wait();
    }
};
```

## Performance Optimization

### **Memory Management**
```cpp
#include "grapa/GrapaLink.h"

class OptimizedProcessor {
private:
    GrapaLink grapa;
    
public:
    OptimizedProcessor() {
        // Pre-compile frequently used functions
        grapa.eval(R"(
            // Pre-compile for performance
            compiled_processor = $sys().compile('data.map(op(x) { x * 2; })');
            compiled_filter = $sys().compile('data.filter(op(x) { x > threshold; })');
        )");
    }
    
    GrapaValue processData(const GrapaValue& data) {
        // Use pre-compiled function for better performance
        return grapa.eval("$sys().eval(compiled_processor, {'data': " + data.toString() + "})");
    }
    
    GrapaValue filterData(const GrapaValue& data, double threshold) {
        // Use pre-compiled function with parameters
        return grapa.eval("$sys().eval(compiled_filter, {'data': " + data.toString() + 
                         ", 'threshold': " + std::to_string(threshold) + "})");
    }
};
```

### **Error Handling**
```cpp
#include "grapa/GrapaLink.h"
#include <stdexcept>

class SafeGrapaWrapper {
private:
    GrapaLink grapa;
    
public:
    GrapaValue safeEval(const std::string& code) {
        try {
            return grapa.eval(code);
        } catch (const std::exception& e) {
            throw std::runtime_error("Grapa evaluation failed: " + std::string(e.what()));
        }
    }
    
    bool isValidCode(const std::string& code) {
        try {
            grapa.eval(code);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    GrapaValue safeEvalWithDefault(const std::string& code, const GrapaValue& default_value) {
        try {
            return grapa.eval(code);
        } catch (...) {
            return default_value;
        }
    }
};
```

## Build System Integration

### **CMakeLists.txt Example**
```cmake
cmake_minimum_required(VERSION 3.10)
project(grapa_cpp_app)

set(CMAKE_CXX_STANDARD 17)

# Find Grapa
find_path(GRAPA_INCLUDE_DIR grapa/GrapaLink.h
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/include
    NO_DEFAULT_PATH
)

find_library(GRAPA_LIBRARY
    NAMES grapa_static
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/platforms/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}
    NO_DEFAULT_PATH
)

# Include directories
include_directories(${GRAPA_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create executable
add_executable(grapa_app main.cpp)

# Link libraries
target_link_libraries(grapa_app ${GRAPA_LIBRARY})

# Platform-specific libraries
if(WIN32)
    target_link_libraries(grapa_app
        gdiplus
        ComCtl32
        crypt32
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CARBON_LIBRARY Carbon)
    target_link_libraries(grapa_app
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CARBON_LIBRARY}
    )
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(grapa_app ${X11_LIBRARIES})
endif()
```

### **Cross-Platform Build**
```bash
# Build for current platform
mkdir build && cd build
cmake ..
cmake --build .

# Cross-compile (example for different platforms)
# Windows (from Linux with MinGW)
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/windows.cmake ..

# macOS (from Linux with osxcross)
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/macos.cmake ..
```

## Best Practices

### **Initialization and Cleanup**
```cpp
class GrapaApplication {
private:
    std::unique_ptr<GrapaLink> grapa;
    
public:
    GrapaApplication() {
        // Initialize once
        grapa = std::make_unique<GrapaLink>();
        
        // Set up common functions
        grapa->eval(R"(
            // Common utility functions
            format_output = op(data) {
                return 'Processed: ' + data.str();
            };
        )");
    }
    
    ~GrapaApplication() {
        // Automatic cleanup via unique_ptr
    }
    
    // Non-copyable
    GrapaApplication(const GrapaApplication&) = delete;
    GrapaApplication& operator=(const GrapaApplication&) = delete;
};
```

### **Thread Safety**
```cpp
#include "grapa/GrapaLink.h"
#include <mutex>
#include <thread>

class ThreadSafeGrapa {
private:
    GrapaLink grapa;
    std::mutex grapa_mutex;
    
public:
    GrapaValue safeEval(const std::string& code) {
        std::lock_guard<std::mutex> lock(grapa_mutex);
        return grapa.eval(code);
    }
    
    void processInParallel(const std::vector<std::string>& tasks) {
        std::vector<std::thread> threads;
        
        for (const auto& task : tasks) {
            threads.emplace_back([this, task]() {
                GrapaValue result = safeEval(task);
                // Process result...
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
};
```

### **Performance Monitoring**
```cpp
#include "grapa/GrapaLink.h"
#include <chrono>
#include <iostream>

class PerformanceMonitor {
private:
    GrapaLink grapa;
    
public:
    template<typename Func>
    auto measureExecution(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
        
        return result;
    }
    
    void benchmarkGrapaOperations() {
        // Benchmark different operations
        measureExecution([this]() {
            return grapa.eval("(1000000).range(0,1).map(op(x) { x * x; })");
        });
        
        measureExecution([this]() {
            return grapa.eval("'Hello, World!'.upper().split(', ')");
        });
    }
};
```

## Conclusion

Grapa's C++ integration provides:
- **High-performance embedding** with minimal overhead
- **Complete API access** to all Grapa features
- **Type-safe interface** with proper error handling
- **Cross-platform support** through universal development kit
- **Easy integration** with existing C++ applications

The universal development kit makes it simple to get started with C++ integration, while the comprehensive API allows for sophisticated applications that leverage Grapa's powerful runtime capabilities.

---

## See also
- [Installation Guide](installation.md) - Complete C++ development workflow
- [API Reference](api_reference.md) - Detailed API documentation
- [Examples](examples.md) - More code examples
- [Advanced Topics](advanced_topics.md) - Advanced C++ integration patterns
