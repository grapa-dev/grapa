# Grapa Cross-Platform Objects

## **Grapa's Parallel-by-Design Architecture**

Grapa was designed from the ground up to be **parallel by design**. This fundamental architectural decision manifests in several key ways:

### **Universal Locking System**
Many Grapa C++ objects inherit from `GrapaCritical`, providing built-in thread safety:
- `GrapaInt` - Arbitrary precision integers with locking
- `GrapaQueue` - Queue data structures with locking  
- `GrapaNet` - Network operations with locking
- `GrapaThread` - Thread management with locking
- `GrapaConsoleResponse` - Console I/O with locking
- `GrapaWidgetLock` - UI widget locking

### **Automatic Variable Locking**
**CRITICAL IMPLEMENTATION DETAIL**: Every access to a Grapa variable automatically locks/unlocks the underlying object. This was extensively stress-tested with hundreds of threads to ensure:
- No crashes under extreme concurrent access
- Cross-platform thread safety
- Worker object reliability
- Predictable behavior under load

### **Built-in Parallel Processing**
**`.map()`, `.filter()`, and `.reduce()` are parallel by default** and will create one thread per item. For large arrays, always specify a thread count to avoid resource exhaustion:

```grapa
/* Parallel by default - creates one thread per item */
small_data = [1, 2, 3, 4, 5];
squares = small_data.map(op(x) { x * x; });

/* For large datasets, limit threads to avoid resource exhaustion */
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### **Thread Safety Methods**
All `GrapaCritical` objects provide:
- `TryCritical()` - Try to acquire lock
- `WaitCritical()` - Wait for lock
- `LeaveCritical()` - Release lock
- `WaitCondition()` - Wait for condition
- `SendCondition()` - Signal condition

### **Worker Pattern Integration**
The `GrapaWorker`/`GrapaWorkEvent`/`GrapaWorkQueue` pattern is deeply integrated throughout the system, enabling parallel processing across various subsystems (grep, unified storage, etc.).

### **Documentation References**
- **Threading Documentation**: `docs-src/docs/sys/thread.md` - Comprehensive guide to `$thread` objects and locking
- **Parallel Processing**: `docs-src/docs/use_cases/parallel_concurrent_programming.md` - Examples and best practices
- **Functional Methods**: `docs-src/docs/syntax/basic_syntax.md` - `.map()`, `.filter()`, `.reduce()` with threading

### **Implications**
This design means that Grapa objects are inherently thread-safe at the C++ level, even if the language syntax doesn't currently expose locking primitives. Future language enhancements could leverage this built-in capability.

**The automatic locking system makes Grapa one of the most thread-safe languages ever designed, with built-in protection against race conditions at the variable access level.**

---

## Core C++ Objects

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

**Recommendation**: Use `GrapaWorker` for parallelization in GrapaDBX, following the pattern established in `source/grep/`.

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

**Recommendation**: Use `GrapaUnicode` classes for all text processing in GrapaDBX, following the pattern established in `source/grep/`.

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
// For Unicode-aware text operations in GrapaDBX
GrapaUnicode::UnicodeString text("Hello, 世界! 👋");
GrapaUnicode::UnicodeString normalized = text.normalize(GrapaUnicode::NormalizationForm::NFC);

// Unicode-aware regex for database queries
GrapaUnicode::UnicodeRegex regex("\\p{L}+", true, false, GrapaUnicode::NormalizationForm::NFC);
bool matches = regex.search(normalized);

// Case-insensitive, diacritic-insensitive matching
GrapaUnicode::UnicodeRegex search_pattern("café", true, true, GrapaUnicode::NormalizationForm::NFC);
```