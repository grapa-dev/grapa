# Grapa Cross-Platform Objects Reference

> **Note:** GrapaDB2 is being developed as a complete rewrite of GrapaDB, motivated by a persistent, unfixable index bug in the original engine. This new architecture leverages robust cross-platform objects to ensure reliability, extensibility, and maintainability for the next generation of Grapa database systems.

## Overview

This document catalogs all the cross-platform C++ objects available in Grapa for the enhanced LocalDatabase implementation. These objects provide 100% cross-platform functionality and should be used instead of platform-specific implementations.

## Core Cross-Platform Objects

### 1. **Threading & Parallelism**

#### **GrapaThread** (Low-Level)
- **File**: `source/grapa/GrapaThread.h/cpp`
- **Purpose**: Cross-platform thread management
- **Platforms**: Windows (CreateThread), Linux/Mac (pthread)
- **Features**: 
  - Thread creation, suspension, resumption
  - Critical section management
  - Condition variables
  - Cross-platform synchronization primitives

#### **GrapaWorker** (High-Level - Preferred)
- **File**: `source/grapa/GrapaState.h` (GrapaWorker class)
- **Purpose**: Higher-level thread abstraction with Grapa integration
- **Usage**: Used in grep parallelization (`source/grep/`)
- **Features**:
  - Integrated with Grapa script execution
  - Automatic resource management
  - Easier to use than raw GrapaThread
  - Built-in Grapa context and variable management

**Recommendation**: Use `GrapaWorker` for parallelization in GrapaDB2, following the pattern established in `source/grep/`.

### 2. **File System Operations**

#### **GrapaFile** (Abstract Base)
- **File**: `source/grapa/GrapaFile.h`
- **Purpose**: Abstract file interface
- **Methods**: Open, Close, Read, Write, GetSize, SetSize, Create, Delete, Flush

#### **GrapaFileIO** (Cross-Platform Implementation)
- **File**: `source/grapa/GrapaFileIO.h/cpp`
- **Purpose**: Cross-platform file I/O operations
- **Platforms**: Windows (Win32 API), Linux/Mac (POSIX)
- **Features**:
  - Large file support (64-bit offsets)
  - Cross-platform file locking
  - Consistent error handling
  - Memory-mapped file operations

#### **GrapaFileCache** (High-Level - Preferred)
- **File**: `source/grapa/GrapaFileCache.h/cpp`
- **Purpose**: Cached file operations with memory management
- **Features**:
  - In-memory caching for performance
  - Automatic cache management
  - Block-based operations
  - Memory-efficient for large files

#### **GrapaFileTree** (High-Level - Preferred)
- **File**: `source/grapa/GrapaFileTree.h/cpp`
- **Purpose**: BTree-based file storage with encryption
- **Features**:
  - BTree structure for efficient access
  - Optional encryption (AES)
  - Compression support
  - Object-based storage

**Recommendation**: Use `GrapaFileCache` or `GrapaFileTree` for database storage operations.

### 3. **Networking**

#### **GrapaNet** (Cross-Platform)
- **File**: `source/grapa/GrapaNet.h/cpp`
- **Purpose**: Cross-platform networking operations
- **Platforms**: Windows (Winsock), Linux/Mac (BSD sockets)
- **Features**:
  - TCP/UDP socket operations
  - SSL/TLS support (OpenSSL)
  - Cross-platform network interface detection
  - Proxy support
  - Certificate management

#### **GrapaNetConnect** (High-Level - Preferred)
- **File**: `source/grapa/GrapaNetConnect.h/cpp`
- **Purpose**: Higher-level network connection management
- **Features**:
  - Integrated with Grapa threading
  - Event-driven networking
  - HTTP support
  - Automatic connection management

**Recommendation**: Use `GrapaNetConnect` for network storage operations in the unified storage system.

### 4. **Memory Management**

#### **GrapaMem**
- **File**: `source/grapa/GrapaMem.h`
- **Purpose**: Cross-platform memory management
- **Features**:
  - Platform-independent memory allocation
  - Endianness handling
  - Memory alignment utilities
  - Cross-platform memory primitives

### 5. **Unicode Support** (Best-in-Class Implementation)

#### **GrapaUnicode** (From source/grep/)
- **Files**: `source/grep/grapa_grep_unicode.hpp/cpp`
- **Purpose**: Comprehensive Unicode processing and text handling
- **Features**:
  - **Normalization Forms**: NFC, NFD, NFKC, NFKD support
  - **Case Folding**: Unicode-aware case-insensitive operations
  - **Diacritic Handling**: Accent-insensitive matching
  - **Grapheme Clusters**: Proper handling of complex Unicode sequences
  - **PCRE2 Integration**: Advanced regex with Unicode support
  - **UTF-8 Processing**: Full UTF-8 validation and manipulation
  - **Caching**: Thread-safe LRU caching for performance
  - **Parallel Processing**: Multi-threaded Unicode operations

#### **Key Unicode Classes**:
- **UnicodeString**: Wrapper for UTF-8 strings with normalization
- **UnicodeRegex**: Advanced regex with Unicode support
- **LRUCache**: Thread-safe caching for normalized text
- **GrapaGrepWorkQueue**: Parallel processing infrastructure

#### **Unicode Features**:
- **Normalization**: Convert text to canonical forms for consistent comparison
- **Case Folding**: Unicode-aware case-insensitive operations
- **Diacritic Stripping**: Remove accents for accent-insensitive matching
- **Grapheme Clusters**: Handle complex Unicode sequences (emojis, combining characters)
- **PCRE2 Integration**: Advanced regex patterns with Unicode properties
- **Performance Optimization**: Caching and parallel processing

**Recommendation**: Use `GrapaUnicode` classes for all text processing in GrapaDB2, following the pattern established in `source/grep/`.

### 6. **Cryptography & Security**

#### **GrapaHash** (SHA3 Implementation)
- **File**: `source/grapa/GrapaHash.h/cpp`
- **Purpose**: Cross-platform cryptographic hashing
- **Features**:
  - SHA3-224, SHA3-256, SHA3-384, SHA3-512
  - SHAKE128, SHAKE256 (extensible output functions)
  - 32-bit and 64-bit hash outputs
  - Cross-platform implementation

#### **GrapaEncode** (Cryptographic Operations)
- **File**: `source/grapa/GrapaEncode.h/cpp`
- **Purpose**: Advanced cryptographic operations
- **Features**:
  - RSA encryption/decryption
  - Digital signatures
  - Diffie-Hellman key exchange
  - Elliptic curve cryptography
  - Message digests
  - Key generation

#### **GrapaPrime** (Cryptographic Primes)
- **File**: `source/grapa/GrapaPrime.h/cpp`
- **Purpose**: Prime number generation for cryptography
- **Features**:
  - Probabilistic prime testing
  - Safe prime generation
  - Multiple primality tests (Fermat, Rabin-Miller, etc.)
  - Co-prime generation
  - RSA key generation support

#### **GrapaTinyAES** (AES Encryption)
- **File**: `source/grapa/GrapaTinyAES.h/cpp`
- **Purpose**: Lightweight AES encryption
- **Features**:
  - AES-128, AES-192, AES-256
  - ECB, CBC, CTR modes
  - Cross-platform implementation
  - Minimal memory footprint

#### **OpenSSL Integration**
- **Purpose**: Cross-platform cryptographic operations
- **Features**:
  - SSL/TLS for secure networking
  - Certificate management
  - Encryption/decryption
  - Hash functions
  - Random number generation

### 7. **Database & Storage**

#### **GrapaBtree** (Core Storage Engine)
- **File**: `source/grapa/GrapaBtree.h/cpp`
- **Purpose**: Cross-platform BTree implementation
- **Features**:
  - Endian-independent storage
  - Cross-platform file operations
  - Memory-efficient design
  - Supports 16-bit, 32-bit, and 64-bit systems

#### **GrapaDB** (Database Engine)
- **File**: `source/grapa/GrapaDB.h/cpp`
- **Purpose**: Full database engine built on GrapaBtree
- **Features**:
  - Tables, indexes, fields, cursors
  - SQL-like operations
  - Transaction support
  - Cross-platform database files

#### **GrapaGroup** (Object Database)
- **File**: `source/grapa/GrapaGroup.h/cpp`
- **Purpose**: Object-oriented database operations
- **Features**:
  - Hierarchical data structures
  - Object relationships
  - Group-based organization
  - Event-driven operations

#### **GrapaLocalDatabase** (High-Level - Current Foundation)
- **File**: `source/grapa/GrapaDatabase.h/cpp`
- **Purpose**: Unified file system and database operations
- **Features**:
  - Seamless navigation between file system and database
  - Cross-platform path handling
  - Unified operations (.cd, .ls, .get, .set)
  - Default $KEY/$VALUE for file-like database operations

### 8. **Mathematical Operations**

#### **GrapaInt** (Arbitrary Precision)
- **File**: `source/grapa/GrapaInt.h/cpp`
- **Purpose**: Cross-platform arbitrary precision integers
- **Features**:
  - Unlimited precision arithmetic
  - Cryptographic operations
  - Cross-platform compatibility
  - Memory-efficient design

#### **GrapaFloat** (High Precision Floats)
- **File**: `source/grapa/GrapaFloat.h/cpp`
- **Purpose**: High-precision floating point arithmetic
- **Features**:
  - Configurable precision
  - Trigonometric functions (sin, cos, tan, etc.)
  - Hyperbolic functions (sinh, cosh, tanh, etc.)
  - Logarithmic functions (ln, log)
  - Power and root functions
  - Mathematical constants (π, e)
  - Cross-platform mathematical operations

#### **GrapaVector** (Matrix Operations)
- **File**: `source/grapa/GrapaVector.h/cpp`
- **Purpose**: Multi-dimensional array and matrix operations
- **Features**:
  - N-dimensional arrays
  - Matrix operations (dot product, multiplication, etc.)
  - Linear algebra functions
  - Statistical operations (mean, sum, covariance)
  - Matrix decomposition (LU, QR)
  - Eigenvalue calculations
  - Cross-platform numerical computing

### 9. **Data Compression & Encoding**

#### **GrapaCompress**
- **File**: `source/grapa/GrapaCompress.h/cpp`
- **Purpose**: Cross-platform data compression
- **Features**:
  - Data compression and expansion
  - Cross-platform compression algorithms
  - Memory-efficient compression
  - Database storage optimization

### 10. **Time & Date Operations**

#### **GrapaTime**
- **File**: `source/grapa/GrapaTime.h/cpp`
- **Purpose**: Cross-platform time and date handling
- **Features**:
  - Current time retrieval
  - Date arithmetic
  - Leap year calculations
  - Time zone handling
  - Cross-platform time operations

## Higher-Level Abstractions (Preferred)

### **GrapaWorker** for Parallelization
```cpp
// Example from source/grep/ pattern
class GrapaWorker : public GrapaThread {
    // Integrated with Grapa script execution
    // Automatic resource management
    // Easier to use than raw GrapaThread
};
```

### **GrapaFileCache** for Storage
```cpp
// For database storage operations
GrapaFileCache* cache = new GrapaFileCache();
cache->SetFile(new GrapaFileIO());
cache->Open("database.gdb", GrapaReadWrite);
```

### **GrapaNetConnect** for Networking
```cpp
// For network storage operations
GrapaNetConnect* net = new GrapaNetConnect();
net->Connect("network://remote-server/data");
```

### **GrapaUnicode** for Text Processing
```cpp
// For Unicode-aware text operations in GrapaDB2
GrapaUnicode::UnicodeString text("Hello, 世界! 👋");
GrapaUnicode::UnicodeString normalized = text.normalize(GrapaUnicode::NormalizationForm::NFC);

// Unicode-aware regex for database queries
GrapaUnicode::UnicodeRegex regex("\\p{L}+", true, false, GrapaUnicode::NormalizationForm::NFC);
bool matches = regex.search(normalized);

// Case-insensitive, diacritic-insensitive matching
GrapaUnicode::UnicodeRegex search_pattern("café", true, true, GrapaUnicode::NormalizationForm::NFC);
```

### **GrapaHash** for Cryptographic Operations
```cpp
// For secure hashing in GrapaDB2
GrapaBYTE data("Hello, World!");
GrapaBYTE hash;
GrapaHash::SHA3_256(data, hash);

// For data integrity verification
u32 quick_hash = GrapaHash::SHAKE128_u32(data);
```

### **GrapaFloat** for High-Precision Math
```cpp
// For mathematical operations in GrapaDB2
GrapaFloat pi = GrapaFloat::Pi(GrapaFloat(3.14159));
GrapaFloat result = pi.Sin() + pi.Cos();

// For statistical calculations
GrapaFloat mean = GrapaFloat(0);
// ... statistical operations
```

## Implementation Guidelines

### 1. **Always Use Cross-Platform Objects**
- Never use platform-specific APIs directly
- Use Grapa's abstraction layers
- Leverage existing cross-platform implementations

### 2. **Prefer Higher-Level Abstractions**
- Use `GrapaWorker` over `GrapaThread`
- Use `GrapaFileCache` over `GrapaFileIO` for database operations
- Use `GrapaNetConnect` over `GrapaNet` for network operations
- Use `GrapaUnicode::UnicodeString` and `GrapaUnicode::UnicodeRegex` for text processing

### 3. **Follow Established Patterns**
- Study `source/grep/` for parallelization patterns
- Study `source/grapa/GrapaDatabase.cpp` for LocalDatabase patterns
- Follow existing error handling and resource management patterns

### 4. **Leverage Existing Infrastructure**
- Use `GrapaLocalDatabase` as the foundation for unified storage
- Extend existing patterns rather than creating new ones
- Maintain backward compatibility with existing APIs

## Future Considerations

### **Parallelization in GrapaDB2**
- Follow the `source/grep/` pattern using `GrapaWorker`
- Consider parallel index building
- Parallel query execution
- Background maintenance operations

### **Unicode Support in GrapaDB2**
- Use `GrapaUnicode::UnicodeString` for all text field storage
- Use `GrapaUnicode::UnicodeRegex` for advanced text searching
- Implement Unicode-aware indexing for better performance
- Support normalization forms for consistent text comparison
- Enable diacritic-insensitive matching for user-friendly searches

### **Cryptographic Support in GrapaDB2**
- Use `GrapaHash` for data integrity verification
- Use `GrapaEncode` for secure data encryption
- Use `GrapaPrime` for cryptographic key generation
- Use `GrapaTinyAES` for lightweight encryption

### **Mathematical Support in GrapaDB2**
- Use `GrapaFloat` for high-precision calculations
- Use `GrapaVector` for statistical operations
- Use `GrapaInt` for arbitrary precision arithmetic
- Support mathematical functions in database queries

### **Network Storage Integration**
- Use `GrapaNetConnect` for remote storage operations
- SSL/TLS for secure network storage
- Proxy support for enterprise environments

### **Memory Storage**
- Use `GrapaFileCache` for in-memory database operations
- Leverage existing caching infrastructure
- Consider memory-mapped files for large datasets

## References

- **Threading**: `source/grep/grapa_grep_unicode.cpp` - Parallel grep implementation
- **Unicode**: `source/grep/grapa_grep_unicode.hpp/cpp` - Best-in-class Unicode implementation
- **File Operations**: `source/grapa/GrapaDatabase.cpp` - LocalDatabase file system integration
- **Networking**: `source/grapa/GrapaNetConnect.cpp` - Network connection management
- **Storage**: `source/grapa/GrapaBtree.cpp` - Core storage engine
- **Cryptography**: `source/grapa/GrapaHash.cpp` - SHA3 hashing implementation
- **Mathematics**: `source/grapa/GrapaFloat.cpp` - High-precision arithmetic
- **Compression**: `source/grapa/GrapaCompress.cpp` - Data compression utilities
- **Time**: `source/grapa/GrapaTime.cpp` - Cross-platform time operations

## Conclusion

Grapa provides a comprehensive set of cross-platform objects that should be used for all enhanced LocalDatabase implementation work. The existing patterns in `source/grep/` and `source/grapa/GrapaDatabase.cpp` provide excellent examples of how to leverage these objects effectively while maintaining 100% cross-platform compatibility.

The Unicode implementation in `source/grep/` represents a best-in-class example that should be leveraged for all text processing in GrapaDB2 and other areas requiring Unicode support. This includes proper normalization, case folding, diacritic handling, and grapheme cluster support. 