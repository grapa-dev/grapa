# BACKLOG

**Current Date:** August 2025 (use `date` command to verify current date)

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md).

---

## 🎯 **IMMEDIATE NEXT STEPS** (Pull to current_status.md)

**Status:** No immediate items - all active work completed ✅

**Next Priority:** When ready for new development, consider pulling from Major Releases section below.

---

## 🚀 **MAJOR RELEASES** (6 items)

### **1. Grapa Syntax Improvements** - **LANGUAGE ADOPTION FOUNDATION** 🔥 **HIGHEST PRIORITY**
- [ ] **Exception Handling**: try/catch blocks for structured error handling
- [ ] **Control Flow Fixes**: Working return/break/continue statements
- [ ] **Property Access**: Improve attribute-style access for objects
- **Reference**: [`GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (3-6 months)
- **Dependencies**: None (foundational work)
- **Priority**: Highest among major releases - foundational for language adoption

### **2. Enhanced Debug Mode** - **DEVELOPMENT EXPERIENCE** 🔥 **HIGH PRIORITY**
- [ ] **Comprehensive Debug System**: Implement sophisticated debugging capabilities including execution tracing, variable inspection, performance profiling, and AST/bytecode dumping
- [ ] **Debug Infrastructure**: Core debug system with configurable debug levels and thread-safe debug context management
- [ ] **Execution Tracing**: Step-by-step execution tracking with variable state snapshots
- [ ] **Advanced Performance Profiling**: Memory usage tracking and detailed performance analysis (basic timing already available via `$TIME`)
- [ ] **Execution Tree Dumping**: Execution tree visualization and inspection
- [ ] **Advanced Variable Inspection**: Interactive debugging with variable snapshots and scope examination
- [ ] **Debug CLI Integration**: Enhanced debug options and output formats
- **Reference**: [`GRAPA_ENHANCED_DEBUG_MODE_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_ENHANCED_DEBUG_MODE_PLAN.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: None (foundational work)
- **Priority**: High - enables advanced development capabilities

### **3. Distutils Removal and Shared Library Naming** - **BUILD SYSTEM MODERNIZATION** 🔥 **HIGH PRIORITY**
- [ ] **Distutils Dependency Removal**: Replace deprecated `distutils` with `setuptools` for Python 3.13+ compatibility
- [ ] **Shared Library Naming Convention**: Implement consistent `_static` vs `_shared` naming across all platforms
- [ ] **Unified Library Locations**: Move all libraries to `source/grapa-lib/{platform}/` directory
- [ ] **Mac Build Process Update**: Remove separate `grapa-other` directory, use consistent naming
- [ ] **Cross-Platform Consistency**: Ensure Linux, AWS, and Windows use same naming patterns
- **Reference**: [`DISTUTILS_REMOVAL_PLAN.md`](../RESEARCH_AND_ANALYSIS/DISTUTILS_REMOVAL_PLAN.md)
- **Estimated Effort**: Major Release (4-6 weeks)
- **Dependencies**: None (foundational build system work)
- **Priority**: High - critical for Python 3.13+ compatibility and build system consistency
- **Risk Level**: Medium - affects all platforms and build processes

### **4. GrapaDB Enhancements** - **DATABASE SYSTEM** 🟡 **MEDIUM PRIORITY**
- [ ] **Performance Optimization**: Index performance enhancement, memory management optimization, query optimization
- [ ] **Advanced Features**: Transaction support, advanced querying, data validation and constraints
- [ ] **Scalability and Reliability**: Large dataset support, backup and recovery, concurrent access
- [ ] **Integration and Testing**: Language integration, performance monitoring, comprehensive testing
- **Reference**: [`GRAPA_DB_ENHANCEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_DB_ENHANCEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (4-8 months)
- **Dependencies**: Syntax Improvements (for error handling)
- **Priority**: Medium - database-specific enhancements



### **6. Advanced Language Features** - **MODERN LANGUAGE PARITY** 🟡 **MEDIUM PRIORITY**
- [ ] **Enhanced Reflection**: Runtime code inspection and modification
- [ ] **Extension System**: Extending existing types with new methods
- **Estimated Effort**: Major Release (4-6 months)
- **Dependencies**: Syntax Improvements (for error handling)
- **Priority**: Medium - modern language parity features

### **7. Vector Performance Optimization** - **PERFORMANCE** 🟢 **LOWER PRIORITY**
- [ ] **Type-Specialized Paths**: Implement optimized code paths for homogeneous vectors while maintaining backward compatibility
- [ ] **Memory Optimization**: Improve memory layout and allocation strategies for better cache performance
- [ ] **Lazy Evaluation**: Defer expensive operations until results are actually needed
- [ ] **Hybrid Vector Types**: Allow users to specify fixed-type vectors for performance-critical scenarios
- [ ] **SIMD Optimization**: Detect CPU SIMD capabilities and use vectorized instructions for appropriate data types
- **Status**: Comprehensive design plan completed, ready for implementation
- **Reference**: [`GRAPA_VECTOR_PERFORMANCE_OPTIMIZATION_PLAN.md`](../IMPLEMENTATION/TYPE_SYSTEM/GRAPA_VECTOR_PERFORMANCE_OPTIMIZATION_PLAN.md)
- **Estimated Effort**: Major Release (4-6 months across 5 phases)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling)
- **Priority**: Lower - performance optimization after core features are stable
- **Performance Targets**: 2-5x improvement for homogeneous vectors, 20-30% memory reduction, 3-8x SIMD improvement

### **8. General Optimization Implementation** - **PERFORMANCE** 🟢 **LOWER PRIORITY**
- [ ] **Performance improvements** for arithmetic, bitwise, comparison, and assignment operators
- **Status**: Requires extensive testing and its own release cycle
- **Reference**: [`GRAPA_OPTIMIZATION_BACKLOG.md`](../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling)
- **Priority**: Lower - performance optimization after core features are stable

---

## 🆕 **NEW INITIATIVES** (5 items)

### **Package Manager Organization Integration** 🔥 **HIGH PRIORITY**
- **Goal**: Work with package manager organizations to make Grapa available through official repositories
- **Current Status**: Homebrew PR submitted (#235087), other platforms need official integration
- **Target Organizations**:
  - **Homebrew Core**: PR #235087 submitted, awaiting review and merge
  - **Ubuntu/Debian**: Submit to Debian/Ubuntu official repositories
    - **Status**: Team membership applications pending
    - **Requirements**: Join Ubuntu MOTU and Debian Developer teams
    - **Timeline**: 2-8 weeks for review and acceptance
    - **Tracking**: [DEBIAN_OFFICIAL_SUBMISSION.md](../DEVELOPMENT/DEBIAN_OFFICIAL_SUBMISSION.md)
  - **Red Hat/Fedora**: Submit to Fedora/EPEL repositories
  - **Amazon Linux**: Submit to Amazon Linux repositories
  - **Chocolatey**: Submit to Chocolatey community repository
- **Requirements**:
  - Meet each organization's packaging standards and guidelines
  - Provide comprehensive package metadata and documentation
  - Ensure security and quality standards compliance
  - Maintain packages across version updates
  - Join relevant developer teams (Ubuntu MOTU, Debian Developer, etc.)
- **Benefits**:
  - Official distribution channels for better discoverability
  - Automatic updates through system package managers
  - Improved security and trust for enterprise users
  - Reduced installation friction for new users
- **Estimated Effort**: 2-4 weeks per platform
- **Priority**: High - critical for user adoption and discoverability

### **Wikipedia Article Creation** ✅ **READY TO IMPLEMENT**
- **Status**: Article content prepared and ready for submission
- **Goal**: Improve Grapa's discoverability and establish official Wikipedia presence
- **Next Steps**: Submit article to Wikipedia, monitor feedback, maintain article

## ✅ **COMPLETED WORK** (Moved from current_status.md)

### **Core Language Features Complete**
- **Array Comprehension Alternatives**: Documented functional patterns using `.filter()` and `.map()` with threading support
- **Vector Operations**: Complete implementation of all vector comparison operators and matrix operations
- **Exception Handling**: Full try/catch/finally implementation with proper variable binding
- **Control Flow**: Complete break/continue/return/exit implementation with proper propagation through all execution contexts
- **Unicode String Support**: Enhanced all string functions for proper Unicode character handling
- **Advanced Language Features**: Confirmed all 12 planned advanced features are already implemented
- **C++ Backend Validation**: Verified no memory leaks, thread safety issues, or closure problems
- **Automatic File Loading**: Documented Grapa's sophisticated automatic `.grc` file loading mechanism
- **Variable Binding**: Resolved reference/copy issues in throw/catch blocks
- **Error Type Enhancement**: Added `.err()` method to `$LIST` for type conversion to `$ERR`
- **Variable Indirection Documentation**: Enhanced documentation of `@` and `@@` operators and arbitrary variable names using `@this["variable name"]` syntax

### **Vector Operations Performance Analysis Complete**
- **Comprehensive Benchmarking**: Created dedicated test suite in `test/vector/` directory
- **C++ Implementation Review**: Analyzed actual algorithms in `GrapaVector.cpp`
- **Performance Documentation**: Created detailed analysis in `maintainers/IMPLEMENTATION/PERFORMANCE/VECTOR_OPERATIONS_PERFORMANCE_ANALYSIS.md`
- **Key Findings**: Matrix multiplication uses standard O(n³) algorithm, eigenvalue calculation uses Jacobi algorithm, determinant uses Gauss elimination, covariance uses direct computation
- **Optimization Recommendations**: Documented specific improvements for real-time, large dataset, and general use cases

### **Vector Edge Cases Testing Complete**
- **Comprehensive Edge Case Testing**: Created dedicated test suite for edge cases and error conditions
- **Test Coverage**: Empty matrices and vectors, singular matrices and error conditions, boundary conditions for different matrix sizes, error handling and recovery mechanisms, performance with edge case data patterns
- **Key Findings**: All edge cases handled gracefully without crashes, empty vectors return null for operations, singular matrices return determinant 0 as expected, non-square matrices handled appropriately
- **Error Handling Assessment**: Excellent (9/10) - Graceful degradation, null returns, mathematical correctness
- **Documentation**: Created comprehensive analysis in `maintainers/IMPLEMENTATION/PERFORMANCE/VECTOR_EDGE_CASES_ANALYSIS.md`

### **User Documentation Complete**
- **Vector Documentation Updates**: Enhanced `docs-src/docs/type/vector.md` with performance guidelines, edge case behavior, and optimization recommendations
- **Performance Optimization Guide**: Created comprehensive `docs-src/docs/vector_performance_guide.md` with performance characteristics, optimization strategies, memory management best practices, use case-specific recommendations
- **Examples Documentation**: Updated `docs-src/docs/examples.md` to include the new performance guide
- **Key Deliverables**: User-facing performance guides with specific recommendations, best practices documentation for vector operations, edge case behavior documentation for users

### **Machine Learning Documentation Complete**
- **Comprehensive ML Guide**: Created `docs-src/docs/machine_learning.md` covering linear regression implementation, statistical analysis and feature preprocessing, model evaluation and performance metrics, complete class implementation examples, performance considerations and best practices, advanced topics (PCA, cross-validation)
- **Linear Regression Example**: Created `docs-src/docs/examples/linear_regression_example.grc` demonstrating data generation and preprocessing, feature scaling and normalization, model training with multiple algorithms, predictions and model evaluation, performance metrics calculation, ridge regression with regularization
- **Documentation Integration**: Updated navigation and cross-references, added machine learning to main documentation index, updated vector documentation with ML references, added linear regression example to examples list

### **Mac-amd64 Support Removal Complete**
- **Build System Updates**: Successfully removed Intel Mac support from all build scripts
- **Documentation Updates**: Updated all user-facing documentation to specify Apple Silicon only
- **Cleanup**: Removed FLTK 1.3.11 fallback logic and conditional ScreenCaptureKit handling
- **Key Benefits**: Simplified build system, clear user guidance, future-proof alignment with Apple's discontinuation of Intel Mac support in macOS 15.0, consistent messaging

### **OpenSSL 3.5.2 Migration Complete**
- **Cross-Platform Build System**: Created `scripts/build/build_openssl.py` for automated OpenSSL builds
- **C++ Compatibility**: Fixed critical OpenSSL 3.x breaking changes (removed ENGINE functions, added err.h)
- **Functionality Validation**: Core features working correctly (network functions, basic math, vector operations)
- **Key Benefits**: Modern OpenSSL 3.5.2 with improved security and features, cross-platform consistent build process, automated streamlined build process with verification, future-proof alignment with modern OpenSSL standards

### **FLTK 1.4.4 Upgrade Complete**
- **Cross-Platform Build System**: Created `scripts/build/build_fltk.py` for automated FLTK builds
- **Threading System Enhancement**: Fixed critical Cocoa threading violations in `GrapaWidget::New()` and `GrapaWidget::CLEAR()`
- **Memory Management**: Resolved use-after-free bug in destructor with synchronous cleanup
- **Documentation**: Comprehensive threading system documentation in `maintainers/IMPLEMENTATION/GRAPA_WIDGET_THREADING_SYSTEM.md`
- **Key Benefits**: Modern FLTK 1.4.4 with improved performance and features, robust threading system for GUI operations, cross-platform consistent build experience, automated streamlined build process with verification

### **Console Exit Optimization Complete**
- **Problem**: Grapa console would not exit immediately when `.exit()` was called if waiting for input
- **POSIX Solution**: Perfect - Uses `select()` with 100ms timeout, loops internally until input or exit
- **Windows Solution**: Working - Stable console operation with native UTF-8 conversion, exit requires keypress
- **Benefits Achieved**: Mac/Linux immediate exit response, Windows stable console operation with correct character handling and full Unicode support

### **Cryptographic Documentation Enhancement Complete**
- **Status**: Documentation updated to cover all supported digest and encoding methods
- **Methods Documented**: Hash Functions (SHA3-224, SHA3-256, SHA3-384, SHA3-512, SHAKE128, SHAKE256), Encoding Functions (Base64, Base58, URL-ASCII)
- **Impact**: Users now have complete documentation of all available cryptographic methods

### **Command-Line Argument Support Resolved**
- **Status**: Working correctly with both `$ARGV` and `$CLIARGV` variables
- **Implementation**: `$ARGV` provides positional arguments only, `$CLIARGV` provides full command line
- **Evidence**: Testing confirms correct behavior for both variables
- **Impact**: Scripts can now access command-line parameters correctly

### **Memory Management Analysis Complete**
- **Status**: Comprehensive analysis of `std::vector` usage across the codebase
- **Files analyzed**: 10+ source files including GrapaLink.cpp, GrapaEncode.cpp, GrapaValue.cpp, grep implementations, and test files
- **Findings**: No memory leaks identified - all pointer vectors have proper cleanup or store pointers to objects owned elsewhere
- **Best practices observed**: RAII patterns, explicit cleanup methods, proper ownership semantics
- **Documentation**: Created `maintainers/DEVELOPMENT/MEMORY_MANAGEMENT_ANALYSIS.md` with detailed analysis
- **Recommendations**: Continue current patterns, consider adding memory leak detection to CI/CD

### **Grapa curl Function Enhancement** 🔥 **HIGH PRIORITY**
- **Status**: ✅ **POST/PUT/DELETE with Request Bodies implemented**, basic HTTP/HTTPS with SSL certificates working
- **Goal**: Achieve 100% feature parity with CLI curl command
- **✅ Completed Features**:
  - **POST/PUT/DELETE with Request Bodies**: ✅ **WORKING** - Support for `data` (JSON) and `form` (URL-encoded) options
  - **HTTP/HTTPS**: ✅ **WORKING** - Full SSL/TLS support with client certificates
  - **Proxy Support**: ✅ **WORKING** - HTTP proxy with string and object configuration
  - **SSL Certificate Handling**: ✅ **WORKING** - Client certificates, private keys, trusted CAs
  - **Custom Headers**: ✅ **WORKING** - Full header customization support
  - **URL Parsing**: ✅ **WORKING** - Automatic protocol, host, port, and path extraction
  - **Content Type Detection**: ✅ **WORKING** - Automatic JSON/HTML/XML parsing
  - **Error Handling**: ✅ **WORKING** - Robust handling of invalid inputs and edge cases
- **Missing Features**:
  - **Multipart/form-data**: Implement multipart form data encoding/decoding (requires C++ backend support)
  - **HTTP/2 and HTTP/3**: Protocol support (requires C++ backend implementation)
  - **Compression**: Automatic gzip/deflate/brotli decompression (requires C++ backend support)
  - **SOCKS Proxy**: Extend proxy support beyond HTTP to SOCKS protocols
  - **Advanced HTTP Authentication**: Digest, NTLM authentication methods
  - **Rate Limiting**: Built-in rate limiting capabilities
  - **Resume Downloads**: Partial content/resume support for large files
  - **Custom DNS**: DNS override and custom resolution
  - **Certificate Pinning**: HPKP (HTTP Public Key Pinning) support
  - **WebSocket**: WebSocket protocol support
  - **Streaming Responses**: Streaming response handling
  - **Connection Pooling**: Connection reuse/pooling
  - **HTTP Caching**: Cache control and ETag support
  - **Redirect Following**: Automatic redirect handling
  - **Timeout Configuration**: Configurable timeouts
  - **Verbose Output**: Detailed request/response logging
- **Testing**: ✅ **Regression tests created** in `test/network/curl_function_test.grc` and `test/network/curl_function_optimized_test.grc`
- **Documentation**: ✅ **User examples created** in `docs-src/docs/examples/curl_function_simple.grc`
- **Estimated Effort**: Medium Release (2-4 weeks for remaining features, 3-6 months for full parity)
- **Dependencies**: C++ backend enhancements for advanced features
- **Priority**: High - essential for complete HTTP client functionality
- **Reference**: `wikipedia_grapa_article.txt` - Complete article with instructions
- **Priority**: Medium - marketing and visibility

### **Database Engineering Best Practices Review** ⏳ **PENDING REVIEW**
- **Status**: Extracted valuable patterns from database development experience
- **Focus**: Debugging methodologies, testing strategies, implementation patterns
- **Next Steps**: Review extracted content, identify integration opportunities, update documentation
- **Reference**: [`../RESEARCH_AND_ANALYSIS/DATABASE_ENGINEERING_BEST_PRACTICES.md`](../RESEARCH_AND_ANALYSIS/DATABASE_ENGINEERING_BEST_PRACTICES.md)



### **Grapa Rules System Enhancement Roadmap** 🔧 **COMPREHENSIVE PLANNING**
- **Status**: Comprehensive roadmap created with 12 major enhancements across 4 phases

### **`.exec()` Method Enhancement** 🔧 **PARAMETER SUPPORT REVISIT**
- **Status**: `.exec()` method has callback, set, get parameters designed in C++ but not exposed in Grapa syntax
- **Background**: Created before `op()()` was fully implemented, parameter handling designed but never fully exposed
- **Goal**: Revisit whether to enhance `.exec()` with parameter support or keep it simple as `op()(script)()` equivalent
- **Options**: 
  - Keep simple (current behavior) - equivalent to `op()(script)()`
  - Expose callback parameter for custom response handling
  - Expose set/get parameters for custom variable scope management
  - Full parameter support for specialized execution contexts
- **Priority**: Low - current functionality works well, enhancement would be nice-to-have
- **Estimated Effort**: 1-2 weeks (analysis and implementation)
- **Focus**: Modern language features, developer experience, advanced capabilities
- **Timeline**: 16-week phased implementation (4 phases, 4 weeks each)
- **Priority**: High - foundational for language adoption and developer experience
- **Reference**: [`../IMPLEMENTATION/RULES_SYSTEM_ENHANCEMENT_ROADMAP.md`](../IMPLEMENTATION/RULES_SYSTEM_ENHANCEMENT_ROADMAP.md)
- **Next Steps**: Prioritize Phase 1 items, begin exception handling implementation

### **LLM-Powered Dynamic IDE for Grapa** 🤖 **REVOLUTIONARY RESEARCH**
- **Status**: Comprehensive concept document created with detailed implementation strategy
- **Focus**: Revolutionary IDE that uses LLMs to understand Grapa's dynamic syntax in real-time
- **Timeline**: 2-3 year phased implementation (REPL → VSCode Extension → Full IDE)
- **Priority**: Research/Experimental - could revolutionize IDE development for dynamic languages
- **Reference**: [`../IMPLEMENTATION/LLM_POWERED_DYNAMIC_IDE_CONCEPT.md`](../IMPLEMENTATION/LLM_POWERED_DYNAMIC_IDE_CONCEPT.md)
- **Next Steps**: Evaluate feasibility, consider Phase 1 proof of concept
- **Risk Level**: High - ambitious but potentially groundbreaking

---

## 🟡 **MEDIUM PRIORITY** (7 categories)

### **Infrastructure & Dependencies**
- [ ] **OpenSSL 3.0 Migration**: Upgrade from OpenSSL 1.1.1 to OpenSSL 3.0 for security improvements and modern cryptographic standards
- **Priority**: HIGH - Linux compatibility blocker (OpenSSL 1.1.1 is EOL)
- **Security Benefits**: Latest security patches, improved cryptographic algorithms, better TLS support
- **Compatibility**: Ensure all cryptographic functions work with OpenSSL 3.0 APIs
- **Regression Testing**: Comprehensive test suite created in `test/network/openssl_compatibility_test.grc`
- **Test Coverage**: Certificate loading, SSL verification, HTTPS communication, client cert auth, proxy SSL, protocol versions, chain validation, error handling, performance
- **Pre-Upgrade**: Run `./grapa test/network/openssl_compatibility_test.grc` and document baseline results
- **Post-Upgrade**: Run same test and compare for any regressions before release
- **Documentation**: See `test/network/README.md` for detailed testing procedures and troubleshooting
- **Testing**: Comprehensive testing of all crypto operations after migration
- **Dependencies**: Update build scripts, dependency management, and platform-specific configurations
- **Status**: Ready for implementation - testing framework in place

- [ ] **FLTK X11 Dependency Investigation**: Research options for removing X11 dependency from FLTK
- **Priority**: HIGH - Dependency modernization for broader platform compatibility
- **Current State**: FLTK pulls in X11 dependencies
- **Goal**: Investigate if X11 dependency can be removed or minimized
- **Investigation Areas**:
  - FLTK configuration options for X11-less builds
  - Alternative GUI backends or configurations
  - Platform-specific dependency management
  - Impact on current GUI functionality
- **Deliverables**: Research report on X11 dependency removal options
- **Status**: Requires investigation and testing

### **Language Features & Enhancements**
- [ ] **Object Constructors and Destructors**: Implement automatic constructor/destructor support for Grapa objects
- **Current State**: Objects created with `obj Class` are empty and must be manually initialized
- **Proposed Enhancement**: Add automatic constructor execution on object creation and destructor on cleanup
- **Implementation**: Extend class system to support `constructor()` and `destructor()` methods
- **Benefits**: More intuitive object-oriented programming, automatic initialization
- **Priority**: Low - nice to have feature
- **Estimated Effort**: Medium Release (2-4 weeks)

- [ ] **Operator Overloading**: Enable objects/classes to implement custom operator behavior
- **Current State**: Operators work only with built-in types and predefined behaviors
- **Proposed Enhancement**: Allow classes to define methods with operator names (e.g., `+`, `-`, `*`, `/`, `==`, etc.)
- **Implementation**: C++ operator handlers would search for operator-named methods on objects
- **Complexity**: Each operator handler needs runtime method lookup for custom implementations
- **Benefits**: More expressive object-oriented programming, custom arithmetic operations
- **Priority**: Low - nice to have feature  
- **Estimated Effort**: Large Release (1-2 months) - requires extensive C++ changes
  - **Risk Assessment**: Medium - requires careful API compatibility testing
  - **Estimated Effort**: 2-4 weeks
  - **Priority**: High within medium priority - security and compatibility upgrade

- [ ] **Module Loading Path Enhancement**: Add support for specifying file paths in module loading
- **Current State**: Automatic loading only searches predefined paths, no way to specify custom locations
- **Proposed Enhancement**: Allow path specification in module loading calls
- **Options**: 
  - **Path-based**: `x = fld1/fld2/my_class();` (forward slash path)
  - **Dot notation**: `x = fld1.fld2.my_class();` (dot-separated path)
  - **Hybrid approach**: Support both syntaxes
- **Benefits**: Better organization of modules, avoid single large folder structure
- **Trade-offs**: 
  - **Pros**: More flexible module organization, better project structure
  - **Cons**: Adds complexity to loading system, potential for path conflicts
- **Implementation Complexity**: Medium - requires C++ changes to module loading system
- **Priority**: Low - current system works well, enhancement would be nice-to-have
- **Estimated Effort**: Medium Release (2-4 weeks) - requires careful design and testing
- **Status**: Under consideration - not yet decided if this enhancement is needed

- [ ] **$LIST and $file API Unification**: Combine common operations between list and file objects
- **Current State**: `$LIST` and `$file` have separate command sets despite conceptual similarities
- **Common Operations Identified**:
  - **Getting items**: `list.get(key)` vs `file.get(key)` 
  - **Setting items**: `list.set(key, value)` vs `file.set(key, value)`
  - **Listing items**: `list.keys()` vs `file.ls()`
  - **Item existence**: `list.has(key)` vs `file.exists(path)`
  - **Item removal**: `list.remove(key)` vs `file.rm(path)`
- **Proposed Enhancement**: Unify common operations with consistent method names
- **Options**:
  - **Unified interface**: Common base class or trait system
  - **Method aliases**: Keep existing methods, add unified aliases
  - **Protocol-based**: Define common protocols/interfaces
- **Benefits**: 
  - **Consistent API**: Same method names for similar operations
  - **Easier learning**: One set of concepts for list/file operations
  - **Code reuse**: Common patterns work across both types
- **Implementation Complexity**: Medium - requires careful API design and backward compatibility
- **Priority**: Low - current APIs work well, enhancement would improve consistency
- **Estimated Effort**: Medium Release (3-4 weeks) - API design, implementation, testing, documentation
- **Backward Compatibility**: Must maintain existing method names for compatibility
- [ ] **FLTK 1.4.4 Migration**: Upgrade from FLTK 1.3.11 to FLTK 1.4.4 for improved compatibility and support
  - **Official Recommendation**: FLTK team explicitly recommends 1.4.4 over 1.3.11
  - **End of Life**: 1.3.11 is considered "old software" and may not compile on new platforms
  - **Compatibility**: FLTK states "many older apps can use it with little or no change"
  - **Benefits**: Bug fixes, improvements, and future platform compatibility
  - **Testing**: Verify GUI components (My_Text_Console, My_Prompt_Editor) work correctly
  - **Dependencies**: Update dep/fltk-1.3.11-source.tar.gz to fltk-1.4.4-source.tar.gz
  - **Risk Assessment**: Low - FLTK maintains API compatibility between 1.3.x and 1.4.x
  - **Estimated Effort**: 1-2 weeks
  - **Priority**: Medium - recommended upgrade for long-term maintenance

### **Performance & Optimization**
- [ ] **Optimize large array operations**
- [ ] **Improve memory management for long-running scripts**
- [ ] **Add caching for frequently accessed data structures**
- [ ] **Performance Benchmarking**: Implement systematic performance comparison with ripgrep and other tools
- [ ] **Cryptographic Performance and Security Analysis**: Comprehensive evaluation of Grapa's cryptographic capabilities
  - **Performance Analysis**: Benchmark all cryptographic algorithms for speed and memory usage
    - **RSA**: Compare performance across different key sizes (1024, 2048, 3072, 4096 bits)
    - **EC Curves**: Performance comparison of all 7 supported curves (prime256v1, secp224r1, secp256k1, secp384r1, secp521r1, prime192v1, prime239v1)
    - **Block Ciphers**: AES variants, ChaCha20-Poly1305, SM4, ARIA, Camellia performance analysis
    - **Hash Functions**: SHA family, SHA3, SHAKE performance comparison
    - **RPK Algorithms**: ED25519/X25519, ED448/X448, HMAC, Poly1305, SipHash, CMAC, TLS1_PRF, Scrypt, SM2 performance
    - **KDF Functions**: HKDF, PBKDF2, Argon2id performance analysis
  - **Security Considerations**: Evaluate security properties and best practices
    - **Algorithm Security**: Review current algorithm choices against NIST recommendations
    - **Key Size Recommendations**: Document minimum key sizes for different security levels
    - **Random Number Generation**: Audit entropy sources and random number generation quality
    - **Side-Channel Resistance**: Evaluate resistance to timing attacks and other side channels
    - **Post-Quantum Readiness**: Assess current algorithms against quantum computing threats
  - **Memory Usage Analysis**: Profile memory consumption patterns for cryptographic operations
    - **Large Number Operations**: Memory usage for different bit sizes
    - **Key Generation**: Memory requirements for different key types
    - **Encryption/Decryption**: Memory usage during bulk operations
  - **Optimization Opportunities**: Identify performance bottlenecks and improvement areas
    - **OpenSSL Integration**: Leverage OpenSSL's optimized implementations
    - **Algorithm Selection**: Provide guidance on algorithm choice based on use case
    - **Batch Operations**: Optimize for bulk cryptographic operations
  - **Documentation Updates**: Create comprehensive performance and security guidance
    - **Performance Benchmarks**: Document expected performance for each algorithm
    - **Security Recommendations**: Provide algorithm selection guidance based on security requirements
    - **Best Practices**: Document cryptographic best practices and common pitfalls
  - **Estimated Effort**: Medium Release (2-4 weeks)
  - **Dependencies**: None - can be implemented independently
  - **Priority**: Medium - important for production use and security assessment
  - **Deliverables**: Performance benchmark suite, security analysis report, updated documentation

### **Documentation**
- [ ] **Maintain and periodically audit API documentation for completeness and cross-linking**
- [ ] **Continue to add more real-world examples and user recipes to the user guide and examples section**
- [ ] **Create a dedicated performance tuning guide for advanced users**
- [ ] **Review and improve comment handling in Grapa**
- [ ] **Audit and update all code paths for correct field metadata lifecycle handling**
- [ ] **Add a high-level architecture and feature map diagram to the About or Home page**

### **Language Features**
- [ ] **Add support for more mathematical functions**
- [ ] **Implement advanced string manipulation functions**
- [ ] **Consider adding support for custom operators**

### **Database Features**
- [ ] **GrapaDB File Structure Modernization**: Update index storage/retrieval and dictionary structure for future features
- **Priority**: HIGH - Foundation for future database enhancements
- **Current State**: Existing file structure may limit future feature implementation
- **Scope**: 
  - **Index Storage/Retrieval**: Modernize how indexes are stored and accessed
  - **Dictionary Updates**: Update both records and indexes dictionary structures
  - **Future Feature Support**: Ensure structure supports planned enhancements
- **Impact**: 
  - **Breaking Changes**: May require data migration for existing databases
  - **Performance**: Improved index performance and scalability
  - **Features**: Enables advanced database features planned for future
- **Timing**: Should be done before adoption focus, but deferred for now
- **Status**: Deferred - not immediate priority but important for future
- **Dependencies**: None - can be implemented independently
- [ ] **Add support for more database backends**
- [ ] **Implement connection pooling**
- [ ] **Add support for transactions**
- [ ] **Improve query optimization**
- [ ] **Multi-Field Set/Get for Records**: Implement batch .set()/.get() operations for records with many fields
- [ ] **Expose custom index creation and management** to the Grapa language and CLI

### **Test Coverage Expansion**
- [ ] **Multiline + Lookaround Combinations**: Expand testing for complex scenarios
- [ ] **Option Matrix Completion**: Test all possible 4+ flag combinations
- [ ] **Performance Regression Testing**: Implement performance benchmarks vs ripgrep

### **Academic Documentation Gaps**
- [ ] **Grammar Mutation System Documentation**: Create comprehensive documentation for $RULE structure implementation
- [ ] **Weighted BTree Performance Analysis**: Document algorithmic complexity, performance benchmarks
- [ ] **Storage Model Comparison**: Create detailed comparison of COL, ROW, GROUP models

---

## 🟢 **LOW PRIORITY** (4 categories)

### **Development Tools**
- [ ] **Create IDE plugins**
- **Note**: Debugging tools and profiling capabilities are covered in "Enhanced Debug Mode" major release
- **Note**: All tooling enhancements (IDE integration, debugging, profiling, AST dumping) are covered in this section and "Enhanced Debug Mode" major release
- **Note**: All debugging tools (stack traces, profiling, hot reloading, AST dumping) are comprehensively covered in "Enhanced Debug Mode" major release

### **Experimental Features**
- [ ] **Consider adding support for async/await patterns**
- [ ] **Investigate adding support for coroutines**
- [ ] **Explore adding support for metaprogramming features**

### **Future Ecosystem Features** 🔮 **LOW PRIORITY**
- [ ] **C++ Extension Mechanism** - System for developers to create and distribute C++ extensions to Grapa's Event libraries
  - **Scope**: Mechanism for third-party C++ extensions that integrate with Grapa's core Event system
  - **Distribution**: Future PyPI-like system for C++ extension distribution
  - **Timeline**: Only when Grapa gains significant traction and ecosystem
  - **Dependencies**: Requires significant user base and developer community
  - **Complexity**: Major architectural feature requiring distribution infrastructure
  - **Note**: This is a "nice to have" for future ecosystem growth, not a current priority

### **Integration**
- [ ] **Add support for more external libraries**
- [ ] **Create bindings for popular frameworks**
- [ ] **Implement plugin system**

### **Advanced Language Features**
- [ ] **IDE Integration**: Improve error messages and syntax highlighting

---

## 🔧 **INFRASTRUCTURE & PLATFORM** (2 items)

### **OpenSSL Hash Digest Expansion** 🔧 **CRYPTOGRAPHIC ENHANCEMENT**
- **Status**: ✅ **Current hash digest support identified and working**
- **Goal**: Expand hash digest support to include all additional algorithms supported by OpenSSL 3.5.2
- **Current Support**: 
  - ✅ **OpenSSL-based (md.genkeys)**: MD5, SHA1, SHA224, SHA256, SHA384, SHA512
  - ✅ **GrapaHash-based (direct encode)**: SHA3-224, SHA3-256, SHA3-384, SHA3-512, SHAKE128, SHAKE256
  - ✅ **Implementation**: Dual system - OpenSSL's `EVP_DigestInit_ex` functions vs custom `GrapaHash` implementations
  - ✅ **Output handling**: Fixed `.hex()` method to always use unsigned interpretation for `GrapaTokenType::RAW` data
- **Missing Algorithms** (OpenSSL 3.5.2 supports these but Grapa doesn't implement them):
  - **BLAKE2 family**: blake2b256, blake2b384, blake2b512, blake2s128, blake2s256
  - **Other algorithms**: RIPEMD160, WHIRLPOOL, SM3, GOST
- **Dual Hash System Analysis**:
  - **OpenSSL System**: Uses `md.genkeys({"digest": "algorithm"})` → `message.encode(md_keys)` pattern
  - **GrapaHash System**: Uses direct `message.encode("ALGORITHM")` pattern
  - **Current Overlap**: SHA3 and SHAKE families are implemented in both systems
  - **Architectural Decision Needed**: Whether to maintain dual system or consolidate to single approach
- **Implementation Options**:
  - **Option 1**: Add missing algorithms to OpenSSL system (md.genkeys approach)
  - **Option 2**: Add missing algorithms to GrapaHash system (direct encode approach)  
  - **Option 3**: Consolidate to single system (recommended for long-term maintenance)
- **Consolidation Benefits**:
  - **Single API**: Consistent interface for all hash algorithms
  - **Maintenance**: One system to maintain instead of two
  - **Performance**: Leverage OpenSSL's optimized implementations
  - **Standards**: OpenSSL is industry standard, well-tested, and actively maintained
- **Consolidation Approach**:
  - **Phase 1**: Add missing algorithms to OpenSSL system
  - **Phase 2**: Deprecate GrapaHash system (mark as deprecated but keep working)
  - **Phase 3**: Remove GrapaHash system after sufficient migration period
- **Testing Strategy**:
  - Test each new algorithm with `md.genkeys({"digest": "algorithm"})` and `message.encode(md_keys)`
  - Verify hex output is positive (unsigned interpretation working correctly)
  - Compare hash outputs with known test vectors for validation
  - Ensure backward compatibility during transition period
- **Estimated Effort**: Small Release (2-3 days for full consolidation)
- **Dependencies**: None - can be implemented independently
- **Priority**: Medium - enhances cryptographic capabilities and improves architecture
- **Risk Level**: Low - simple addition to existing working infrastructure

### **PFC (Pairing-Friendly Curve) Implementation Fix** 🔧 **CRYPTOGRAPHIC ENHANCEMENT**
- **Status**: ❌ **Partially implemented but has critical issues**
- **Goal**: Fix BLS12-381 implementation for "pfc" (Pairing-Friendly Curve) cryptography
- **Current Implementation**: 
  - ✅ **Key Generation**: Works with `ikm` (Input Keying Material) or `sk` (private key) parameters
  - ❌ **Private Key Generation**: Generated private keys appear to be all zeros
  - ❌ **Signing**: Returns errors when attempting to sign messages
  - ❌ **Signature Aggregation**: `SignAdd` functionality not working properly
  - ❌ **Verification**: Cannot verify signatures due to signing issues
- **Issues Identified**:
  - **Key Generation Bug**: `blst::blst_keygen()` may not be working correctly, producing invalid private keys
  - **Signing Process**: Complex BLS12-381 signing implementation has bugs
  - **BLST Library Integration**: Potential issues with BLST library usage or configuration
  - **Algorithm Support**: Only supports BBS (Boneh-Boyen-Shacham) signatures, not standard BLS signatures
- **Technical Analysis**:
  - **Implementation Location**: `source/grapa/GrapaEncode.cpp` lines 275-1338 (Grapa_bls12_381 class)
  - **Key Generation**: Uses `blst::blst_keygen()` function for private key generation
  - **Signing**: Complex implementation with BBS signature scheme
  - **Dependencies**: Requires BLST library (`prj/blst/`) for BLS12-381 operations
- **Required Fixes**:
  - **Fix Key Generation**: Ensure `blst::blst_keygen()` produces valid private keys
  - **Fix Signing Process**: Debug and fix the BLS12-381 signing implementation
  - **Add Standard BLS Support**: Implement standard BLS signatures in addition to BBS
  - **Fix Signature Aggregation**: Ensure `SignAdd` works correctly for signature aggregation
  - **Add Verification**: Implement proper signature verification
  - **Error Handling**: Improve error reporting for debugging
- **Testing Requirements**:
  - Test key generation with various input keying materials
  - Verify generated private keys are valid (non-zero)
  - Test signing with known test vectors
  - Test signature verification
  - Test signature aggregation functionality
  - Compare with known BLS12-381 implementations
- **Documentation Updates**:
  - Update `docs-src/docs/use_cases/cryptography.md` with working PFC examples
  - Document BLS12-381 algorithm options and parameters
  - Add signature aggregation examples
  - Update status from "Partially Supported" to "Fully Supported"
- **Estimated Effort**: Medium Release (2-4 weeks)
- **Dependencies**: BLST library must be properly configured and working
- **Priority**: Medium - important for advanced cryptographic capabilities
- **Risk Level**: Medium - complex cryptographic implementation requiring careful testing
- **Deliverables**: Working PFC implementation, comprehensive tests, updated documentation

### **Platform-Aware Library Directory Implementation** 🔧 **INFRASTRUCTURE ENHANCEMENT**
- **Status**: Initial implementation complete but reverted due to compilation issues and testing challenges
- **Focus**: Implement platform-specific default library directory paths for better user experience
- **Context**: Currently uses development-focused paths, but production users need platform-appropriate paths
- **Current Search Order** (existing functionality):
  1. **Current Working Directory**: `{current_working_directory}/lib/grapa`
  2. **System Paths**: `/usr/lib/grapa` → `/usr/local/lib/grapa` 
  3. **Binary Directory**: `{binary_directory}/lib/grapa`
  4. **Static Library**: Built-in compressed library files
- **Proposed Platform Requirements**:
  - **Windows**: `%USERPROFILE%\.grapa\lib` → `%PROGRAMFILES%\Grapa\lib` → `{current_directory}\lib\grapa`
  - **macOS**: `~/Library/Application Support/Grapa/lib` → `/usr/local/lib/grapa` → `/opt/grapa/lib` → `{current_directory}/lib/grapa`
  - **Linux/BSD**: `~/.local/lib/grapa` → `/usr/local/lib/grapa` → `/usr/lib/grapa` → `{current_directory}/lib/grapa`
- **Implementation Details**:
  - Modify `gSystem->mLibDir` initialization in `source/grapa/GrapaLink.cpp`
  - Ensure proper ordering after `mHomeDir` initialization
  - Add proper `struct stat sb;` declaration for directory existence checks
  - Test automatic file loading mechanism with platform-aware paths
- **Challenges Identified**:
  - Variable scoping issues with `struct stat sb` declaration
  - Interaction with existing grammar file loading logic (`$grapa.grz`)
  - Testing automatic file loading mechanism complexity
- **Progress**: 
  - ✅ Platform-specific path logic designed and implemented
  - ✅ Build system compilation successful after fixes
  - ✅ Platform-aware directory detection working (`/Users/matichuk/Library/Application Support/Grapa/lib` on macOS)
  - ❌ Automatic file loading testing inconclusive (requires further investigation)
  - ❌ Changes reverted due to testing complexity and scope
- **Next Steps**: 
  - Revisit implementation with proper variable scoping from the start
  - Implement comprehensive testing strategy for automatic file loading
  - Ensure grammar file loading logic compatibility
  - Update documentation for new default paths
- **Documentation Updates Needed**:
  - `docs-src/docs/module_system.md` - Platform-specific search paths
  - `docs-src/docs/getting_started.md` - Platform-aware installation paths
  - `maintainers/IMPLEMENTATION/CORE_SYSTEM/AUTOMATIC_FILE_LOADING_MECHANISM.md` - Updated search paths
- **Reference**: Implementation was in `source/grapa/GrapaLink.cpp` with platform-specific `#ifdef` blocks
- **Priority**: Medium - improves user experience but not critical for core functionality
- **Estimated Effort**: 1-2 weeks (implementation + testing + documentation)
- **Risk Level**: Medium - affects core system initialization and file loading behavior

---

## 🔍 **INVESTIGATION ITEMS** (3 items)

### **SQL Syntax Injection Demo** 🔍 **INVESTIGATION NEEDED**
- **Status**: Initial investigation complete, design concerns identified
- **Focus**: Create working demonstration of custom SQL syntax injection using Grapa's grammar extension capabilities
- **Context**: Educational demonstration showing how `custom_command` and `custom_function` can extend Grapa's syntax
- **Current Issue**: Simple custom grammar rules work (e.g., `select $INT`), but complex SQL rules (e.g., `select $STR from $STR`) fail to parse properly
- **Design Concerns**: 
  - BNF rule design may not be optimal for SQL syntax
  - `findall` function doesn't work on `$TABLE` objects (only XML/LIST/ARRAY/TUPLE)
  - WHERE clause implementation would require manual iteration over `table.ls()`
  - Leverage opportunities limited - would need significant new C++ code for WHERE conditions
- **Progress**: 
  - ✅ Basic custom function demonstration working
  - ✅ Script compilation issues resolved
  - ✅ Core limitation identified (complex rule parsing)
  - ✅ Database operation analysis complete (ls/get/set leverage possible, findall not available)
- **Next Steps**: Reconsider BNF design approach, evaluate if SQL syntax is the right demonstration vehicle
- **Reference**: [`test/use_cases/sql_syntax_injection_table_demo.grc`](../../test/use_cases/sql_syntax_injection_table_demo.grc) - Current test script
- **Priority**: Low - educational/demonstration value, design reconsideration needed

### **ReplaceLocalQueue Optimization** 🔍 **INVESTIGATION NEEDED**
- **Status**: Function currently disabled, basic functionality working via complete copy approach
- **Focus**: Revisit the selective copying optimization for local variable scope exit handling
- **Context**: `ReplaceLocalQueue` function was designed to selectively copy only local variables that are referenced in returned objects, rather than doing complete copies
- **Current State**: 
  - Function is disabled with `return;` at the start (`GrapaState.cpp:5061`)
  - No calls to `ReplaceLocalQueue` exist in the codebase
  - Basic scope exit scenarios work correctly via complete copy approach
- **Original Intent**: 
  - Traverse returned objects to find references to local variables
  - Copy only the referenced local variables before scope exit
  - Avoid unnecessary copying of large objects when only small local variables are referenced
- **Complexity**: Implementation became complex and was abandoned in favor of simpler complete copy approach
- **Progress**: 
  - ✅ mVar field analysis completed
  - ✅ Scope exit behavior verified working correctly
  - ✅ ReplaceLocalQueue function identified as disabled optimization
  - ✅ Current complete copy approach confirmed sufficient for basic cases
- **Next Steps**: Evaluate if selective copying optimization is worth implementing, or if complete copy approach is sufficient
- **Reference**: [`maintainers/IMPLEMENTATION/CORE_SYSTEM/GRAPA_RULE_EVENT_MVAR_IMPLEMENTATION.md`](../../maintainers/IMPLEMENTATION/CORE_SYSTEM/GRAPA_RULE_EVENT_MVAR_IMPLEMENTATION.md) - Complete analysis
- **Priority**: Low - current implementation works correctly, optimization may not be necessary

### **ProcessPlan Control Flow Copy Optimization** 🔍 **INVESTIGATION NEEDED**
- **Status**: Automatic copy in control flow section removed, intelligent copy detection now working correctly
- **Focus**: Evaluate all automatic copies in ProcessPlan for performance optimization opportunities
- **Context**: The automatic copy in `if (result->mControlFlow)` section was interfering with intelligent copy detection system
- **Current State**: 
  - Automatic copy removed from control flow section in `GrapaScriptExec::ProcessPlan`
  - Intelligent copy detection now works properly for local variable references
  - Throw scenarios with local variable references now work correctly
- **Performance Impact**: 
  - Each automatic copy needs evaluation for necessity
  - Local variables are tagged appropriately for recursive inspection
  - Copy should only be made if local variable references are detected
- **Progress**: 
  - ✅ Throw scenario fixed by removing unnecessary automatic copy
  - ✅ Intelligent copy detection system now working as intended
  - ✅ mVar system properly detecting local variable references
- **Next Steps**: 
  - Audit all automatic copies in ProcessPlan for similar optimization opportunities
  - Implement recursive inspection to detect when copies are actually needed
  - Ensure performance optimization without breaking existing functionality
- **Optimization Ideas**: 
  - **mClean flag approach**: Add `mClean` boolean flag that is set to `true` after a copy is made, then checked before making subsequent copies to avoid redundant copying
  - **Recursive inspection**: Use existing `mVar` system to detect when local variable references are present and only copy when necessary
- **Priority**: Medium - performance optimization, but current system works correctly

### **Grep Thread Count Capping Implementation** 🔍 **INVESTIGATION NEEDED**
- **Status**: Parallel processing infrastructure exists but thread capping logic is missing
- **Focus**: Implement proper thread count capping for grep operations to prevent excessive thread creation
- **Context**: Current grep implementation has `num_workers = 0` default but always uses sequential processing regardless of value
- **Current State**: 
  - `grep_extract_matches_unicode_impl_parallel` function exists with proper async processing
  - `split_input_for_parallel` function properly splits input for parallel processing
  - Main functions always call sequential implementation regardless of `num_workers` value
  - No auto-detection of CPU cores or input size-based thread capping
- **Missing Implementation**: 
  - Logic to detect when `num_workers = 0` and auto-determine optimal thread count
  - CPU core detection using `std::thread::hardware_concurrency()`
  - Input size-based thread capping (small inputs use sequential, large inputs use parallel)
  - Decision logic to choose between sequential and parallel processing
- **Progress**: 
  - ✅ Parallel processing infrastructure identified and analyzed
  - ✅ Current implementation behavior documented
  - ✅ Missing thread capping logic identified
- **Next Steps**: 
  - Implement auto-detection logic for optimal thread count
  - Add CPU core detection and input size analysis
  - Update main functions to use parallel processing when appropriate
  - Add thread capping based on input size and system capabilities
- **Implementation Plan**: 
  - Add logic to detect `num_workers = 0` and auto-determine thread count
  - Use `std::thread::hardware_concurrency()` for CPU core detection
  - Implement input size thresholds (e.g., < 1KB sequential, > 1MB use more threads)
  - Update `grep_extract_matches_unicode` to choose between sequential/parallel
- **Priority**: Medium - performance optimization for grep operations

### **Enhanced Error Information System** 🔍 **INVESTIGATION NEEDED**
- **Status**: Current C++ errors return limited information (mostly `{"error":-1}`)
- **Focus**: Enhance error reporting to provide more detailed and useful error information
- **Context**: Most C++ errors currently return `{"error":-1}` which provides minimal debugging information
- **Current State**: 
  - Most C++ errors return `{"error":-1}` regardless of error type
  - `$ERR` type is a `$LIST` (name/value pairs) with potential for rich error information
  - Limited debugging capability due to lack of error context
  - Poor user experience with generic error codes
- **Issues with Current Approach**: 
  - **Limited Debugging**: `-1` doesn't tell developers what went wrong
  - **No Error Context**: Missing information about where/how the error occurred
  - **Difficult Troubleshooting**: Hard to distinguish between different error types
  - **Poor User Experience**: Users can't understand what failed
- **Proposed Enhancement**: 
  - **Error Codes**: Specific numeric codes for different error types
  - **Error Messages**: Human-readable descriptions
  - **Error Context**: Where the error occurred (line, column, operation)
  - **Error Metadata**: Additional debugging information
- **Implementation Plan**: 
  - **Phase 1**: Audit current error codes across C++ codebase
  - **Phase 2**: Define error code constants and error type enumeration
  - **Phase 3**: Enhance error creation to include context and messages
  - **Phase 4**: Update error handling documentation and examples
- **Example Enhanced Error Structure**: 
  ```grapa
  /* Current */
  {"error":-1}
  
  /* Enhanced */
  {
      "error": -1,
      "type": "division_by_zero",
      "message": "Division by zero attempted",
      "location": "line 5, column 10",
      "context": "x = 10 / 0"
  }
  
  /* File operation error */
  {
      "error": -1,
      "type": "file_not_found",
      "message": "File does not exist",
      "path": "/path/to/missing/file",
      "operation": "open"
  }
  ```
- **Benefits**: 
  - **Better Debugging**: Developers know exactly what failed
  - **User-Friendly**: Clear error messages for end users
  - **Error Recovery**: Can implement specific handling for different error types
  - **Logging**: Better error tracking and monitoring
  - **Documentation**: Self-documenting error information
- **Priority**: Medium - improves developer experience and debugging capabilities

---



---

# 📄 **Backlog Summary**

- **Immediate Next Steps**: 0 items - all active work completed ✅
- **Major Releases**: 8 items (prioritized by impact and dependencies)
- **New Initiatives**: 4 items (Wikipedia Article, Database Best Practices, Rules System Enhancement, LLM-Powered Dynamic IDE)
- **Medium Priority**: 7 categories with multiple items each
- **Low Priority**: 5 categories with experimental and future features
- **Investigation Items**: 1 item (SQL Syntax Injection Demo)

**Priority Order Rationale:**
1. **Grapa Syntax Improvements** - Critical for language adoption and developer experience
2. **Enhanced Debug Mode** - Advanced development capabilities (depends on Syntax Improvements)
3. **Distutils Removal and Shared Library Naming** - Critical for Python 3.13+ compatibility and build system consistency
4. **GrapaDB Enhancements** - Database-specific improvements
5. **Advanced Language Features** - Modern language parity features
6. **Optimization Implementation** - Performance optimization after core features are stable 