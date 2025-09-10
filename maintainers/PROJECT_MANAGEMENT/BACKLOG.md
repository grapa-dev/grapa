# BACKLOG

**Current Date:** August 2025 (use `date` command to verify current date)

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md).

---

## 🎯 **IMMEDIATE NEXT STEPS** (Pull to current_status.md)

**Status:** All 0.1.52 work completed and documented ✅

**Next Priority:** When ready for new development, consider pulling from Major Releases section below.

**⚠️ IMPORTANT:** All work completed in 0.1.52 is documented in the "RECENT COMPLETIONS" section above. The remaining tasks in this backlog are for future releases (0.1.53+). Do not duplicate 0.1.52 work in current_status.md.

---

## 📋 **RECENT COMPLETIONS** (Updated December 2024)

### **Language Features Completed**
- **Grapa Syntax Improvements**: Exception handling, control flow, property access ✅ **COMPLETED IN 0.1.52**
- **`.find()` Method**: Comprehensive implementation for strings, RAW data, arrays, and lists
- **Array/List Slicing**: Fixed `.left()`, `.right()`, and `.mid()` for all data types including strings
- **Boolean String Conversion**: `(true).echo()` and `(true).str()` working correctly
- **$ERR Constructor**: `$ERR()` and `$ERR({error:44, desc:'hi'})` working correctly
- **Switch Statement Comparison**: Fixed `SwitchEvent` to use `DoComparison` for proper string-to-type comparisons
- **Key Exchange Methods**: All three methods (DH, EC, RPK) working and documented
- **Async/Await Documentation**: Documented why async/await is unnecessary in Grapa's concurrency model
- **Advanced Vector Statistical Functions**: Implemented `.norm()`, `.mode()`, `.median()`, `.percentile()`, and `.quantile()` with custom q parameter support

### **Documentation Updates Completed**
- **Comment Support**: Fixed documentation to correctly state that `//` comments are supported
- **Namespace Scoping**: Documented `$global`, `$this`, `$local` usage patterns
- **Network Programming Patterns**: Documented thread safety, disconnection, and concurrency patterns
- **Key Exchange Methods**: Comprehensive documentation of DH, EC, and RPK differences

### **Build System Modernization Completed**
- **Distutils Dependency Removal**: Successfully replaced deprecated `distutils` with `setuptools` for Python 3.13+ compatibility ✅ **COMPLETED IN 0.1.52**
- **Shared Library Naming Convention**: Implemented consistent `_static` vs `_shared` naming across all platforms ✅ **COMPLETED IN 0.1.52**
- **Unified Library Locations**: Moved all libraries to `source/grapa-lib/{platform}/` directory ✅ **COMPLETED IN 0.1.52**
- **Mac Build Process Update**: Removed separate `grapa-other` directory, use consistent naming ✅ **COMPLETED IN 0.1.52**
- **Cross-Platform Consistency**: Ensured Linux, AWS, and Windows use same naming patterns ✅ **COMPLETED IN 0.1.52**

### **Enhanced Debug Mode Completed**
- **Comprehensive Debug System**: Sophisticated debugging capabilities with `-d` flag ✅ **IMPLEMENTED**
- **Level-Based Debugging**: Configurable debug levels for various feature groups ✅ **IMPLEMENTED**
- **Advanced Development Tools**: Execution tracing, variable inspection, performance profiling ✅ **IMPLEMENTED**

### **Infrastructure & Dependencies Completed**
- **OpenSSL 3.0 Migration**: Successfully upgraded from OpenSSL 1.1.1 to 3.0 ✅ **COMPLETED IN 0.1.52**
- **FLTK X11 Dependency Investigation**: Comprehensive research completed and documented ✅ **COMPLETED IN 0.1.52**
- **Security & Compatibility**: Latest security patches, improved cryptographic algorithms, better TLS support ✅ **ACHIEVED**

### **Development Tools Completed**
- **VSCode Extension**: Comprehensive IDE integration with Grapa language support ✅ **COMPLETED IN 0.1.52**
- **IDE Integration**: Full VSCode support with syntax highlighting, IntelliSense, and debugging ✅ **ACHIEVED**

### **Experimental Features Addressed**
- **Async/await patterns**: Documented as unnecessary - Grapa's built-in concurrency is superior ✅ **ADDRESSED**
- **Coroutines support**: Fully implemented via `$thread` system with suspend/resume ✅ **IMPLEMENTED**
- **Metaprogramming features**: Core language feature - `$OP` and `$CODE` are foundation ✅ **IMPLEMENTED**

### **Performance Optimizations Addressed**
- **ProcessPlan Control Flow Copy**: Main control flow copy issue resolved, system now working correctly ✅ **ADDRESSED IN 0.1.52**

### **Windows Installation System Completed** ✅ **COMPLETED IN 0.1.52**
- **Build System**: ✅ `python3 build.py` successfully builds both static and shared libraries on Windows
- **Project Files**: ✅ `prj/winlib-amd64_shared/grapalib.vcxproj` builds a DLL successfully
- **Library Naming**: ✅ Static library named `grapa_static.lib` and shared library named `grapa.dll`
- **Install Script**: ✅ `bin/win-amd64/install-grapa.ps1` handles both library types correctly
- **Library Placement**: ✅ DLL correctly placed in `lib/` folder, executable in `bin/` folder
- **Replacement Logic**: ✅ Script properly removes old installations completely before installing new ones
- **Installation Features**: Smart detection, clean replacement, proper file structure, file validation, error handling, PATH management, uninstall support
- **Documentation**: Installation guide, Windows README, and getting started guide all updated
- **Release System**: Cross-platform archive creation, Windows compatibility, GitHub integration ready
- **Status**: **COMPLETELY FUNCTIONAL and READY FOR PRODUCTION USE** ✅ **COMPLETED IN 0.1.52**

### **Thread System Documentation Completed** ✅ **COMPLETED IN 0.1.52**
- **Thread System Usage**: Comprehensive documentation of threading capabilities and limitations ✅ **COMPLETED**
- **Sleep Limitation**: ~~Documented global process-wide sleep queue behavior with workarounds~~ ✅ **RESOLVED in 0.1.54** - Sleep function now works correctly with proper thread-local behavior
- **Examples**: Four working examples demonstrate proper patterns ✅ **COMPLETED**
- **Status**: Thread system fully functional with documented limitations ✅ **COMPLETED IN 0.1.52**

### **Vector Operations Enhancement Completed** ✅ **COMPLETED IN 0.1.52**
- **Vector Operations**: Comprehensive statistical functions, linear algebra, and creative implementations ✅ **COMPLETED**
- **Mathematical Functions**: Full complement of operations implemented and tested ✅ **COMPLETED**
- **Documentation**: Complete coverage of all capabilities ✅ **COMPLETED**
- **Status**: Vector operations are complete and production-ready ✅ **COMPLETED IN 0.1.52**

---

## 📋 **0.1.52 COMPLETION SUMMARY** ✅

**All major infrastructure and language foundation work completed in 0.1.52:**
- ✅ **Grapa Syntax Improvements** - Core language features (exception handling, control flow, property access)
- ✅ **Enhanced Debug Mode** - Already implemented with `-d` flag and sophisticated debugging
- ✅ **Build System Modernization** - Distutils removal, shared library naming, cross-platform consistency
- ✅ **Infrastructure & Dependencies** - OpenSSL 3.0 migration, FLTK investigation
- ✅ **Development Tools** - VSCode extension with full IDE integration
- ✅ **Experimental Features** - All capabilities implemented or documented as unnecessary
- ✅ **Performance Optimizations** - ProcessPlan control flow copy issue resolved

---

## 🚀 **FUTURE RELEASES (0.1.53+)** (6 items)

### **1. Universal .get/.set Implementation** - **LANGUAGE ADOPTION FOUNDATION** ✅ **COMPLETED in 0.1.54**
- [x] **Universal .get/.set Implementation**: Implemented consistent .get/.set methods across all data types
- **Reference**: [`GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md)
- **Status**: ✅ **COMPLETED** - Universal .get/.set methods now work across all major data types
- **Implementation**: 
  - **Universal Coverage**: `.get()/.set()` work across `$ARRAY`, `$LIST`, `$OBJ`, `$file`, `$TABLE`, and `$WIDGET`
  - **Flexible Field Access**: `.getfield()/.setfield()` with mix/match of names and indices for `$ARRAY`, `$LIST`, and `$OBJ` (non-system classes)
  - **Index Support**: 0-based indexing with negative indexing support for arrays and lists
  - **Boundary Operations**: Append/prepend functionality using length-based indexing
  - **System Class Handling**: `$file` and `$TABLE` use specialized C++ implementations for named keys only
- **Documentation**: Comprehensive coverage in `basic_syntax.md`, `api_reference.md`, and migration guides
- **Testing**: Extensive validation across 1D/2D structures, named/indexed access, and edge cases
- **Result**: Consistent API across all data types with flexible access patterns

### **2. GrapaDB Enhancements** - **DATABASE SYSTEM** 🟡 **MEDIUM PRIORITY**
- [ ] **Performance Optimization**: Index performance enhancement, memory management optimization, query optimization
- [ ] **Advanced Features**: Transaction support, advanced querying, data validation and constraints
- [ ] **Scalability and Reliability**: Large dataset support, backup and recovery, concurrent access
- [ ] **Integration and Testing**: Language integration, performance monitoring, comprehensive testing
- **Reference**: [`GRAPA_DB_ENHANCEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_DB_ENHANCEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (4-8 months)
- **Dependencies**: Syntax Improvements (for error handling) ✅ **COMPLETED IN 0.1.52**
- **Priority**: Medium - database-specific enhancements

### **3. Vector ++= Operator Fix** - **BUG FIX** ✅ **RESOLVED in 0.1.54**
- [x] **Fix GrapaVector::Extend Method**: Replace `memcpy` with proper deep copying for `GrapaVectorItem` structures containing pointers
- [x] **Support 1D Vectors**: Extend functionality to work with 1D vectors, not just 2D matrices
- [x] **Memory Management**: Ensure proper memory allocation and deallocation during vector extension
- [x] **Testing**: Comprehensive testing for various vector types and sizes
- **Issue**: `++=` operator produces `null` values when extending 2D vectors due to `memcpy` copying pointer addresses instead of underlying data
- **Resolution**: Operator now works correctly for extending 2D vectors with mixed data types
- **Validation**: Tested with `x = #[[1,'text A'],[3,4]]#; x ++= #[[555933303.9495578668.float(30),'text B']]#; x;`
- **Result**: Successfully produces `#[[1,"text A"],[3,4],[555933303.9495578,"text B"]]#`
- **Impact**: Limited - only use case for `GrapaVector::Extend` in entire codebase is the `++=` operator
- **Estimated Effort**: Bug Fix (1-2 weeks) - **COMPLETED**
- **Dependencies**: None
- **Priority**: High - affects vector concatenation functionality
- **Risk Level**: Low - isolated to single operator, has working alternative

### **4. Sleep Limitation Fix** - **THREADING SYSTEM** ✅ **RESOLVED in 0.1.54**
- [x] **Fix Global Sleep Queue**: Resolved global process-wide sleep queue limitation in multi-threaded applications
- [x] **Thread-Local Sleep**: Sleep function now works correctly with proper thread-local behavior
- [x] **Documentation Updates**: Updated all documentation to reflect that sleep works correctly in multi-threaded applications
- [x] **Testing**: Validated with `thread_example4.grc` showing multiple threads sleeping concurrently
- **Issue**: Previously documented limitation where sleep calls were process-wide, not thread-local, causing first sleep call to block all subsequent ones
- **Impact**: Limited multi-threaded application design and required complex workarounds for timing coordination
- **Resolution**: Sleep function now works correctly with proper thread-local behavior
- **Validation**: Tested with multiple threads sleeping concurrently with different durations
- **Result**: Each thread can sleep independently without blocking others, enabling proper concurrent timing
- **Documentation**: Updated all documentation to reflect that sleep works correctly in multi-threaded applications
- **Estimated Effort**: Bug Fix (1-2 weeks) - **COMPLETED**
- **Dependencies**: None
- **Priority**: High - affects multi-threaded application design
- **Risk Level**: Low - isolated to sleep functionality





### **5. Advanced Language Features** - **DEBUGGING & EXTENSIONS** 🟡 **MEDIUM PRIORITY**
- [ ] **Enhanced Debugging System**: Step-by-step debugger, variable inspection, call stack analysis
- [ ] **Extension System**: Extending existing types with new methods
- [ ] **Enhanced Error Information**: Detailed error context and recovery mechanisms
- [ ] **Namespace Chaining Support**: Implement member functions in `$OBJ.grc` for `oplocal()`, `local()`, `parent()`, and `this()` to enable chaining like `$oplocal.$parent.$oplocal`
- **Current State**: Grapa already has extensive reflection and dynamic modification capabilities
  - **Code Inspection**: ✅ `55.random` shows function source code
  - **Dynamic Modification**: ✅ Language syntax modification, mutable rules engine, metaprogramming foundation
  - **Metadata Access**: ✅ Comprehensive type information and object inspection
- **Gap Identified**: Limited debugging capabilities (step-by-step debugger, variable inspection)
- **Modern Development**: Focus on debugging tools rather than reflection (already comprehensive)
- **Estimated Effort**: Major Release (4-6 months)
- **Dependencies**: Syntax Improvements (for error handling) ✅ **COMPLETED IN 0.1.52**
- **Priority**: Medium - debugging and extension capabilities

### **6. Stderr/Stdout File Support** - **SYSTEM INTEGRATION** 🟡 **MEDIUM PRIORITY**
- [ ] **System Stream Support**: Enable `$file().set($stderr, "data")` and `$file().set($stdout, "data")` for direct system stream writing
- [ ] **Windows Stderr Redirection Fix**: Resolve stderr redirection issues on Windows when redirected to files
- [ ] **GrapaFileIO Enhancement**: Update `Open()` method to accept `GrapaCHAR` and handle system stream constants
- [ ] **Cross-Platform Compatibility**: Ensure system stream support works on Windows, Linux, and macOS
- [ ] **Backward Compatibility**: Maintain existing file operations while adding system stream support
- **Reference**: [Stack Overflow Issue](https://stackoverflow.com/questions/73512538/software-written-in-grapa-can-write-to-stderr-when-redirected-to-a-file-but-not/79712508#79712508)
- **Implementation Guide**: [STDERR_STDOUT_FILE_SUPPORT.md](DEVELOPMENT/STDERR_STDOUT_FILE_SUPPORT.md)
- **Estimated Effort**: Low-Medium (2-3 hours)
- **Priority**: Medium - addresses community-reported issue

### **7. Certificate Management Enhancements** - **NETWORKING & SECURITY** 🟡 **MEDIUM PRIORITY**
- [ ] **Certificate Generation & Management**: CSR creation, self-signed certificate generation, certificate renewal workflows
- [ ] **Certificate Lifecycle Management**: Expiration monitoring, certificate rotation, backup/restore
- [ ] **Certificate Operations**: Format conversion (PEM ↔ DER), fingerprint generation, detailed certificate field extraction
- [ ] **Advanced SSL/TLS Configuration**: Cipher suite selection, TLS version specification, SNI support
- [ ] **Certificate Security**: Revocation checking (CRL/OCSP), certificate pinning, hostname validation
- [ ] **Certificate Storage**: Certificate store management, import/export functionality
- **Current State**: `$net` class supports basic SSL/TLS operations but lacks comprehensive certificate management
  - **Basic SSL/TLS**: ✅ Client/server certificate configuration, certificate verification, certificate chains
  - **CA Trust**: ✅ Custom CA certificate and path support, private key support
  - **Missing**: Certificate generation, lifecycle management, advanced SSL/TLS configuration
- **Reference**: [`CERTIFICATE_MANAGEMENT_ENHANCEMENTS.md`](../DEVELOPMENT/CERTIFICATE_MANAGEMENT_ENHANCEMENTS.md)
- **Implementation**: Extend `$net` class with new methods for comprehensive certificate management
- **Benefits**: Enterprise-grade certificate management, improved security capabilities, better SSL/TLS control
- **Estimated Effort**: Major Release (3-4 months)
- **Dependencies**: OpenSSL 3.5.2 integration (already complete)
- **Priority**: Medium - important for enterprise applications and advanced security use cases

### **8. Vector Performance Optimization** - **PERFORMANCE** 🟢 **LOWER PRIORITY**
- [ ] **Type-Specialized Paths**: Implement optimized code paths for homogeneous vectors while maintaining backward compatibility
- [ ] **Memory Optimization**: Improve memory layout and allocation strategies for better cache performance
- [ ] **Lazy Evaluation**: Defer expensive operations until results are actually needed
- [ ] **Hybrid Vector Types**: Allow users to specify fixed-type vectors for performance-critical scenarios
- [ ] **SIMD Optimization**: Detect CPU SIMD capabilities and use vectorized instructions for appropriate data types
- **Status**: Comprehensive design plan completed, ready for implementation
- **Reference**: [`GRAPA_VECTOR_PERFORMANCE_OPTIMIZATION_PLAN.md`](../IMPLEMENTATION/TYPE_SYSTEM/GRAPA_VECTOR_PERFORMANCE_OPTIMIZATION_PLAN.md)
- **Estimated Effort**: Major Release (4-6 months across 5 phases)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling) ✅ **COMPLETED IN 0.1.52**

### **9. General Optimization Implementation** - **PERFORMANCE** 🟢 **LOWER PRIORITY**
- [ ] **Performance improvements** for arithmetic, bitwise, comparison, and assignment operators
- **Status**: Requires extensive testing and its own release cycle
- **Reference**: [`GRAPA_OPTIMIZATION_BACKLOG.md`](../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling) ✅ **COMPLETED IN 0.1.52**
- **Priority**: Lower - performance optimization after core features are stable

---

## 🆕 **NEW INITIATIVES** (5 items)

### **Package Manager Organization Integration** ⚠️ **NOT FEASIBLE**
- **Goal**: Work with package manager organizations to make Grapa available through official repositories
- **Status**: **BLOCKED** - Package managers require full source code inclusion, which is not feasible for Grapa
- **Feedback**: Received confirmation that without full source code, official repository inclusion is unlikely
- **Decision**: Current distribution methods (GitHub Releases, VSCode Extension) are sufficient
- **Current Distribution Channels**:
  - **GitHub Releases**: Direct binary downloads for all platforms
  - **VSCode Extension**: Visual Studio Marketplace for IDE integration
  - **Homebrew**: Community formula (not official core)
  - **Chocolatey**: Community package (not official)
- **Benefits Achieved**:
  - Effective distribution through existing channels
  - No source code disclosure requirements
  - Maintains project's intellectual property protection
  - Sufficient user adoption and discoverability
- **Priority**: **LOW** - Current distribution channels meet user needs effectively

### **Wikipedia Article Creation** ✅ **READY TO IMPLEMENT**
- **Status**: Article content prepared and ready for submission
- **Goal**: Improve Grapa's discoverability and establish official Wikipedia presence
- **Next Steps**: Submit article to Wikipedia, monitor feedback, maintain article

### **Release System Improvements** 🔥 **HIGH PRIORITY** - **USER FEEDBACK DRIVEN**
- **Status**: Critical issues identified from user feedback on v0.1.53 release
- **Goal**: Address high-impact issues affecting user experience and installer reliability
- **Source**: User feedback on GitHub release page and installer functionality

#### **Critical Issues (Immediate Fixes Needed)**
- [x] **Repository Mismatch Fix**: ✅ **COMPLETED** - Fixed installer URLs pointing to `matichuk/grapa` instead of `grapa-dev/grapa`
  - **Issue**: Installer download URLs caused 404 errors for users
  - **Resolution**: Updated all repository references in `smart_release_manager.py` and `install-grapa-0.1.53.py`
  - **Impact**: Users can now successfully download and install Grapa

- [ ] **Dynamic Version Discovery**: Make installer discover latest version instead of hardcoding v0.1.53
  - **Current State**: Installer hardcodes version in filename and URLs
  - **Enhancement**: Use GitHub Releases API to fetch latest version or support `--version` flag
  - **Implementation**: Modify installer to call GitHub API for version discovery
  - **Benefits**: Users get latest version automatically, easier maintenance
  - **Example**: `python3 -c "$(curl -fsSL https://grapa.dev/install.py)"`
  - **Priority**: High (affects user experience)
  - **Estimated Effort**: Small Release (1-2 weeks)

- [ ] **Release Page Formatting Fix**: Fix bullets showing literal `\n` instead of proper list formatting
  - **Issue**: Release notes display raw text instead of formatted lists
  - **Impact**: Poor readability and professional appearance
  - **Priority**: Medium (cosmetic but affects perception)
  - **Estimated Effort**: Bug Fix (1-2 days)

#### **Installer Hardening (Security & Reliability)**
- [ ] **Checksums & Signature Verification**: Add integrity verification to installer
  - **Current State**: No verification of downloaded archives
  - **Enhancement**: Publish `SHA256SUMS.txt` and verify checksums before extraction
  - **Security**: Optional detached signatures for additional verification
  - **Implementation**: Add checksum verification step in installer
  - **Priority**: High (security and reliability)
  - **Estimated Effort**: Small Release (1-2 weeks)

- [ ] **Idempotency & Rollback**: Make installer safe for re-runs and failures
  - **Current State**: No rollback mechanism if installation fails
  - **Enhancement**: Revert partial changes if any step fails, make re-runs no-ops
  - **Features**: 
    - Check `grapa --version` to detect current installation
    - Backup existing installation before changes
    - Restore backup if installation fails
    - Skip installation if already current version
  - **Priority**: High (reliability)
  - **Estimated Effort**: Medium Release (2-3 weeks)

- [ ] **Proper PATH Management**: Fix PATH updates that actually persist
  - **Current State**: PATH updates may not persist across shell sessions
  - **Platform-Specific Requirements**:
    - **macOS**: Update `~/.zprofile` or `~/.zshrc`, system-wide via `/etc/paths.d/grapa`
    - **Linux**: Update `~/.bashrc` or `~/.profile`, prefer `~/.local/bin` for unprivileged
    - **Windows**: Registry updates (User scope for non-admin, Machine scope for admin)
  - **Priority**: High (affects usability)
  - **Estimated Effort**: Medium Release (2-3 weeks)

- [ ] **Installation Layout Validation**: Enforce expected directory structure
  - **Current State**: No validation of archive contents
  - **Enhancement**: Verify required files exist before installation
  - **Expected Layout**: `bin/grapa`, `include/...`, `lib/...`, `LICENSE`
  - **Implementation**: Fail fast if required files missing
  - **Priority**: Medium (reliability)
  - **Estimated Effort**: Small Release (1 week)

#### **User Experience Improvements**
- [ ] **Cross-Platform Quick Start**: Single copy-paste command for all platforms
  - **Current State**: Different commands for different platforms
  - **Enhancement**: Universal bootstrap script that works on macOS/Linux/Windows
  - **Implementation**: Host bootstrap at `grapa.dev` with platform detection
  - **Example**: 
    ```bash
    # macOS/Linux
    /usr/bin/env python3 - <<'PY'
    # bootstrap code
    PY
    
    # Windows PowerShell
    Set-ExecutionPolicy Bypass -Scope Process -Force
    iwr https://grapa.dev/install.ps1 -UseBasicParsing | iex
    ```
  - **Priority**: High (user experience)
  - **Estimated Effort**: Medium Release (2-3 weeks)

- [ ] **Architecture Detection**: Robust platform and architecture detection
  - **Current State**: Basic platform detection
  - **Enhancement**: Map `uname -m` values properly (x86_64|amd64 → x64, arm64|aarch64 → arm64)
  - **macOS**: Prefer universal binary when available
  - **Priority**: Medium (compatibility)
  - **Estimated Effort**: Small Release (1 week)

- [ ] **Uninstall Support**: Add `--uninstall` flag to remove installation
  - **Current State**: No uninstall mechanism
  - **Enhancement**: Remove files and revert PATH changes
  - **Implementation**: Track installation changes for clean removal
  - **Priority**: Medium (completeness)
  - **Estimated Effort**: Small Release (1-2 weeks)

#### **Distribution Enhancements (Future)**
- [ ] **Package Manager Integration**: Windows MSI, macOS PKG, Linux packages
  - **Windows**: Signed MSI or EXE with winget manifest
  - **macOS**: Signed + notarized .pkg with proper system integration
  - **Linux**: AppImage for "just run it" installs
  - **Priority**: Low (nice to have)
  - **Estimated Effort**: Major Release (1-2 months)

- **Priority**: High - Critical user experience issues
- **Estimated Effort**: Medium Release (4-6 weeks) for critical issues
- **Dependencies**: None - can be implemented independently
- **Status**: **ACTIVE** - Repository mismatch fixed, other issues pending

## Previous Release Work Summary

The following work was completed in version 0.1.52 and is now stable:

### **Core Language Features**
- Array Comprehension Alternatives, Vector Operations, Exception Handling, Control Flow, Unicode String Support, Advanced Language Features, C++ Backend Validation, Automatic File Loading, Variable Binding, Error Type Enhancement, Variable Indirection Documentation

### **Vector Operations Performance Analysis**
- Comprehensive benchmarking, C++ implementation review, performance documentation, optimization recommendations

### **Vector Edge Cases Testing**
- Edge case testing, error handling assessment, comprehensive analysis

### **User Documentation**
- Vector documentation updates, performance optimization guide, examples documentation

### **Machine Learning Documentation**
- Comprehensive ML guide, linear regression examples, documentation integration

### **Build System Modernization**
- Mac-amd64 support removal, OpenSSL 3.5.2 migration, FLTK 1.4.4 upgrade

### **System Optimizations**
- Console exit optimization, cryptographic documentation enhancement, command-line argument support, memory management analysis

### **Networking Protocol Enhancement Plan**
- HTTP/HTTPS client/server with SSL/TLS support, comprehensive protocol addition plan

### **Infrastructure**
- Database engineering best practices review, Grapa rules system enhancement roadmap, .exec() method enhancement, LLM-powered dynamic IDE concept

---

## 🟡 **MEDIUM PRIORITY** (7 categories)

### **Infrastructure & Dependencies** ✅ **COMPLETED IN 0.1.52**
- [x] **OpenSSL 3.0 Migration**: Upgrade from OpenSSL 1.1.1 to OpenSSL 3.0 for security improvements and modern cryptographic standards ✅ **COMPLETED**
- **Priority**: HIGH - Linux compatibility blocker (OpenSSL 1.1.1 is EOL) ✅ **RESOLVED**
- **Security Benefits**: Latest security patches, improved cryptographic algorithms, better TLS support ✅ **ACHIEVED**
- **Compatibility**: Ensure all cryptographic functions work with OpenSSL 3.0 APIs ✅ **VERIFIED**
- **Regression Testing**: Comprehensive test suite created in `test/network/openssl_compatibility_test.grc` ✅ **IMPLEMENTED**
- **Test Coverage**: Certificate loading, SSL verification, HTTPS communication, client cert auth, proxy SSL, protocol versions, chain validation, error handling, performance ✅ **COMPLETE**
- **Pre-Upgrade**: Run `./grapa test/network/openssl_compatibility_test.grc` and document baseline results ✅ **COMPLETED**
- **Post-Upgrade**: Run same test and compare for any regressions before release ✅ **COMPLETED**
- **Documentation**: See `test/network/README.md` for detailed testing procedures and troubleshooting ✅ **COMPLETED**
- **Testing**: Comprehensive testing of all crypto operations after migration ✅ **COMPLETED**
- **Dependencies**: Update build scripts, dependency management, and platform-specific configurations ✅ **COMPLETED**
- **Status**: OpenSSL 3.0 migration successfully implemented ✅ **COMPLETED IN 0.1.52**

- [x] **FLTK X11 Dependency Investigation**: Research options for removing X11 dependency from FLTK ✅ **COMPLETED**
- **Priority**: HIGH - Dependency modernization for broader platform compatibility ✅ **RESOLVED**
- **Current State**: FLTK pulls in X11 dependencies ✅ **INVESTIGATED**
- **Goal**: Investigate if X11 dependency can be removed or minimized ✅ **ACHIEVED**
- **Investigation Areas**:
  - FLTK configuration options for X11-less builds ✅ **RESEARCHED**
  - Alternative GUI backends or configurations ✅ **EVALUATED**
  - Platform-specific dependency management ✅ **ANALYZED**
  - Impact on current GUI functionality ✅ **ASSESSED**
- **Deliverables**: Research report on X11 dependency removal options ✅ **COMPLETED**
- **Status**: Investigation completed, findings documented ✅ **COMPLETED IN 0.1.52**

### **Language Features & Enhancements**
- [x] **Universal .get/.set Implementation**: ✅ **COMPLETED in 0.1.54** - Implemented consistent .get/.set methods across all data types
  - **Status**: ✅ **COMPLETED** - Universal .get/.set methods now work across all major data types
  - **Implementation**: Universal coverage across `$ARRAY`, `$LIST`, `$OBJ`, `$file`, `$TABLE`, and `$WIDGET`
  - **Features**: Flexible field access, index support, boundary operations, system class handling
  - **Documentation**: Comprehensive coverage in basic_syntax.md, api_reference.md, and migration guides
  - **Result**: Consistent API across all data types with flexible access patterns
- [ ] **Object Constructors and Destructors**: Implement automatic constructor/destructor support for Grapa objects
- **Current State**: Objects created with `obj Class` are empty and must be manually initialized
- **Proposed Enhancement**: Add automatic constructor execution on object creation and destructor on cleanup
- **Implementation**: Extend class system to support `constructor()` and `destructor()` methods
- **Benefits**: More intuitive object-oriented programming, automatic initialization
- **Priority**: Low - nice to have feature
- **Estimated Effort**: Medium Release (2-4 weeks)

- [x] **Array and List `.left()` and `.right()` Method Fix**: Fixed data type handling in array and list slicing methods
- **Status**: Array and list slicing now properly handles all data types including strings
- **Implementation**: 
  - Fixed `GrapaVector::Left()` and `GrapaVector::Right()` methods to properly handle string data
  - Added support for `$LIST` type in `GrapaLibraryRuleLeftEvent` and `GrapaLibraryRuleRightEvent`
  - Ensured proper data type preservation for all array elements (strings, numbers, objects)
- **Testing**: Verified with string arrays, mixed type arrays, and list types
- **Benefits**: Proper array slicing for all data types, consistent behavior across `$ARRAY` and `$LIST`
- **Documentation**: Updated `docs-src/docs/type/str.md`, `docs-src/docs/type/obj_methods.md`, `docs-src/docs/type/array.md`, and `docs-src/docs/type/list.md` to reflect full capabilities

- [x] **Vector Functions for Single Dimension Vectors**: Add support for vector operations like sum, mean, min, max on single dimension vectors
- **Current State**: Vector operations now support both 1D and 2D vectors with comprehensive statistical functions
- **Implementation**: 
  - Added vector function handlers in `source/grapa/GrapaLibRule.cpp`
  - Implemented statistical calculations for both 1D and 2D vectors
  - Enhanced `GrapaVector.cpp` with `Min`, `Max`, `Std`, and `Var` methods
  - Updated `GrapaLibRule.cpp` with `GrapaLibraryRuleMinEvent`, `GrapaLibraryRuleMaxEvent`, `GrapaLibraryRuleStdEvent`, and `GrapaLibraryRuleVarianceEvent`
  - Registered all methods in handler map: `min`, `max`, `std`, `variance`
- **Features**:
  - **Sum**: `vector.sum()` - sum all elements in 1D and 2D vectors
  - **Mean**: `vector.mean()` - calculate arithmetic mean of vector elements
  - **Min/Max**: `vector.min()`, `vector.max()` - find minimum and maximum values
  - **Standard Deviation**: `vector.std()` - calculate standard deviation
  - **Variance**: `vector.variance()` - calculate variance
- **Benefits**: Enhanced data analysis capabilities, easier statistical operations on arrays
- **Status**: All statistical functions now work with both 1D and 2D vectors
- **Testing**: Verified with various numeric arrays and edge cases
- **Documentation**: Updated `docs-src/docs/type/vector.md` to reflect full capabilities

- [x] **Add `.find()` Method**: Implemented comprehensive `.find()` method for strings, RAW data, arrays, and lists
- **Status**: `.find()` method now works with strings, RAW data, arrays, and lists
- **Implementation**: 
  - Added string `.find()` method in `GrapaLibraryRuleFindEvent::Run`
  - Added RAW data support with binary-safe search using `mLength`
  - Added array and list support for searching subarrays and sublists
  - Implemented optional `start` and `count` parameters for large data blocks
- **Features**:
  - **String Search**: `"hello world".find("world")` returns `6`
  - **RAW Data Search**: `0xFE0021.raw().find(0x0021.raw())` returns `1`
  - **Array Search**: `[1,2,3,4,5].find([3,4])` returns `2`
  - **List Search**: `{a:1,b:2,c:3}.find([2,3])` returns `1`
- **Benefits**: Comprehensive search capabilities across all data types, better language parity
- **Documentation**: Updated `docs-src/docs/type/str.md` and `docs-src/docs/type/obj_methods.md` with examples and usage

- [x] **Boolean String Conversion**: Fixed boolean string conversion for `.echo()` and `.str()` methods
- **Resolution**: Using `(true).echo()` and `(true).str()` syntax instead of implementing dot notation for boolean literals
- **Current State**: Boolean values now work properly with string conversion
  - `(false).echo()` returns "false"
  - `(true).echo()` returns "true"
  - `(false).str()` returns "false"
  - `(true).str()` returns "true"
- **Design Decision**: 
  - **No dot notation for literals**: Boolean literals don't support `true.echo()` syntax
  - **Parentheses approach**: Use `(true).echo()` and `(true).str()` for boolean operations
  - **Consistency**: Follows same pattern as `(42).echo()`, `("hello").str()`, etc.
- **Benefits**: Clean, consistent approach that maintains language design principles
- **Status**: Boolean string conversion now works correctly

- [x] **$ERR Class Constructor Support**: Already working correctly
- **Current State**: `$ERR` type is a `$LIST` (name/value pairs) with proper constructor support
- **Working Examples**: 
  - `x = $ERR()` creates empty error object `{}`
  - `x = $ERR({error:44, desc:"hi"})` creates structured error object `{"error":44,"desc":"hi"}`
  - Both return proper `$ERR` type when `.type()` is called
- **Implementation**: Already implemented in C++ backend
- **Benefits**: Structured error creation, proper error handling patterns, type safety
- **Status**: `$ERR` constructor works correctly

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
- [x] **FLTK 1.4.4 Migration**: Upgrade from FLTK 1.3.11 to FLTK 1.4.4 for improved compatibility and support
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
- [ ] **Namespace Chaining Support**: Enable chaining of namespace variables like `$oplocal.$parent.$oplocal`
- [x] **Vector Data Loss Bug**: ✅ **COMPLETED** - Fixed multiple methods in GrapaVector.cpp that lose non-numeric data
- [ ] **Dual-Dimension Vector Label Support**: Evaluate supporting labels on both dimensions for 2D vectors
  - **Current State**: GrapaVector supports column labels only (spreadsheet-like functionality)
  - **Enhancement**: Add support for both row and column labels in 2D vectors
  - **Technical Requirements**: 
    - Add `mRowLabels` and `mColLabels` queues to GrapaVector
    - Update 2D access methods to support both row and column label resolution
    - Modify vector creation/initialization to populate both label queues
  - **Use Cases**: Enhanced spreadsheet functionality, matrix operations with named dimensions
  - **Priority**: Medium (current column-only labels meet most spreadsheet needs)

- [ ] **Extend .replace() to Collections**: Add support for `.replace()` method on `$LIST` and `$ARRAY` types
  - **Current State**: `.replace()` only supports `$STR` type for string replacement
  - **Enhancement**: Extend to support element replacement in collections
  - **Technical Requirements**:
    - Update `GrapaLibraryRuleReplaceEvent::Run` to handle `GrapaTokenType::LIST` and `GrapaTokenType::ARRAY`
    - Implement element-by-element replacement logic for collections
    - Maintain type consistency (replace elements with same type)
  - **Use Cases**: Data cleaning, element substitution in arrays/lists, batch data processing
  - **Example**: `[1,2,3,2,4].replace(2, 99)` → `[1,99,3,99,4]`
  - **Priority**: High (common data processing operation)
  - **Estimated Effort**: Small Release (1-2 weeks)

- [ ] **Extend .trim() to Collections**: Add support for `.trim()` method on `$LIST` and `$ARRAY` types
  - **Current State**: `.trim()` only supports `$STR` and `$RAW` types for character trimming
  - **Enhancement**: Extend to support element trimming in collections
  - **Technical Requirements**:
    - Update `GrapaLibraryRuleTrimEvent::Run` to handle `GrapaTokenType::LIST` and `GrapaTokenType::ARRAY`
    - Implement element removal from start/end based on criteria
    - Support trimming by value, type, or custom function
  - **Use Cases**: Data cleaning, removing null/empty elements, array/list sanitization
  - **Example**: `[null,1,2,3,null].trim(null)` → `[1,2,3]`
  - **Priority**: Medium (useful for data processing)
  - **Estimated Effort**: Small Release (1-2 weeks)

- [ ] **Consider Splitting .get/.set Functions for Extension System**: Architectural consideration for future extension system
  - **Current State**: `FileGetEvent` and `FileSetEvent` handle `.get()/.set()/.getfield()/.setfield()` for all data types (ARRAY/LIST, FILE/TABLE, VECTOR)
  - **Proposed Enhancement**: Split into separate functions for each data type category and move methods to respective class objects
  - **Technical Requirements**:
    - Create separate event handlers for ARRAY/LIST, FILE/TABLE, and VECTOR
    - Move `.get()/.set()/.getfield()/.setfield()` methods to each class object (`$ARRAY.grc`, `$file.grc`, `$VECTOR.grc`, etc.)
    - Maintain backward compatibility during transition
  - **Motivation**: Future extension system may have different mechanisms for get/set operations (similar to how widgets and vectors are different)
  - **Benefits**: 
    - **Extension Flexibility**: Extensions can implement custom get/set mechanisms
    - **Type-Specific Optimization**: Each data type can optimize its access patterns
    - **Cleaner Architecture**: Methods live with their respective types
    - **Future-Proofing**: Enables custom data types with unique access patterns
  - **Use Cases**: Custom data types, specialized access patterns, extension system integration
  - **Example**: Extension could implement `$CUSTOM_TYPE` with specialized `.get()/.set()` behavior
  - **Priority**: Low (architectural consideration for future)
  - **Estimated Effort**: Major Release (2-3 months) - requires careful API design and migration
  - **Status**: **CONSIDERATION ONLY** - Not planned for immediate implementation
  - **Dependencies**: Extension system development (future)
  - **Issue**: ~~Vector methods return `[0]` or `[[0,0],[0,0]]` instead of preserving string/mixed-type data~~
  - **Root Cause**: ~~`result.Set()` method not properly handling non-numeric data types in multiple functions~~
  - **Solution Implemented**: 
    - ✅ Fixed `GrapaVectorParam` constructor to handle `GrapaTokenType::STR` properly using `Aop->mValue.FROM(a->b->Get())`
    - ✅ Updated all affected functions to use `if (p1.Aop) { result.Set(i, p1.Aop); } else { result.Set(i, *p1.aa); }` logic
    - ✅ Added `Aop = NULL;` to `GrapaVectorParam::~GrapaVectorParam()` to prevent dangling pointers
  - **Affected Functions (Now Fixed)**: 
    - **`.left()`**: ✅ `["a","b"].vector().left(1)` now returns `["a"]` correctly
    - **`.right()`**: ✅ `["a","b"].vector().right(1)` now returns `["b"]` correctly
    - **`.diag()`**: ✅ `["a","b"].vector().diag()` now returns `[["a",0],[0,"b"]]` correctly
    - **`.triu()`**: ✅ `[["a","b"],["c","d"]].vector().triu()` now returns `[["a","b"],[0,"d"]]` correctly
    - **`.tril()`**: ✅ `[["a","b"],["c","d"]].vector().tril()` now returns `[["a",0],["c","d"]]` correctly
  - **Result**: All vector operations now work correctly with both numeric and non-numeric data types
- [ ] **Namespace Chaining Support**: Enable chaining of namespace variables like `$oplocal.$parent.$oplocal`
  - **Current Limitation**: Namespace variables (`$oplocal`, `$local`, `$parent`, `$this`) cannot be chained
  - **Required Implementation**: Add member functions to `$OBJ.grc`:
    ```grapa
    oplocal = op(context) { /* locate context in namespace and return its $oplocal */ };
    local = op(context) { /* locate context in namespace and return its $local */ };
    parent = op(context) { /* locate context in namespace and return its $parent */ };
    this = op(context) { /* locate context in namespace and return its $this */ };
    ```
  - **Use Case**: Complex namespace navigation without storing intermediate references
  - **Priority**: Low - current workaround (storing references) is sufficient
  - **Status**: Documented limitation, implementation deferred

- [ ] **C++ Event Glue Function Error Handling Audit**: Verify consistent error handling across all C++ Event glue functions
  - **Current State**: Error handling patterns vary across different Event functions in `GrapaLibRule.cpp`
  - **Recent Example**: Vector `.get()/.set()` error handling was recently standardized with proper `NULL` checks, bounds checking, and `$ERR` returns
  - **Scope**: Audit all `GrapaLibraryRule*Event::Run` functions for consistent error handling patterns
  - **Technical Requirements**:
    - **NULL Result Handling**: Ensure all function calls that can return `NULL` are properly checked
    - **Bounds Checking**: Verify consistent bounds checking for array/list/vector access operations
    - **Error Propagation**: Ensure `$ERR` objects are properly returned instead of generic `{"error":-1}`
    - **Input Validation**: Standardize parameter validation across all Event functions
    - **Memory Safety**: Verify proper memory management and pointer validation
  - **Affected Areas**:
    - **File Operations**: `FileGetEvent`, `FileSetEvent`, `FileListEvent`, etc.
    - **Array/List Operations**: `LeftEvent`, `RightEvent`, `MidEvent`, `ReplaceEvent`, `TrimEvent`
    - **String Operations**: `FindEvent`, `GrepEvent`, `MatchEvent`, `SplitEvent`
    - **Mathematical Operations**: `MinEvent`, `MaxEvent`, `SumEvent`, `MeanEvent`
    - **Database Operations**: `TableGetEvent`, `TableSetEvent`, `TableListEvent`
  - **Benefits**:
    - **Consistent User Experience**: All functions return meaningful errors instead of generic codes
    - **Better Debugging**: Specific error messages help developers identify issues
    - **Memory Safety**: Prevents crashes from invalid pointer access
    - **API Reliability**: Predictable error handling across all language functions
  - **Use Cases**: Improved error reporting, better debugging experience, consistent API behavior
  - **Example**: Standardize error handling like the recent vector implementation:
    ```cpp
    if (result == NULL) {
        result = Error(vScriptExec, pNameSpace, -1);
        return(result);
    }
    if (index >= (s64)objEvent->vVector->mSize) {
        result = Error(vScriptExec, pNameSpace, -1);
        return(result);
    }
    ```
  - **Priority**: High (affects all language functions and user experience)
  - **Estimated Effort**: Medium Release (2-4 weeks) - requires systematic audit and testing
  - **Dependencies**: None - can be implemented independently
  - **Status**: **AUDIT NEEDED** - Systematic review of all Event functions required

- [ ] **C++ to Grapa Exception Bridge**: Implement mechanism for C++ code to throw exceptions that can be caught by Grapa scripts
  - **Current State**: Grapa has `try/catch/throw/finally` for scripts, but C++ code cannot throw exceptions that scripts can catch
  - **Issue Identified**: C++ code uses `Error()` function to return `$ERR` objects, but these are not thrown as exceptions
  - **Current C++ Error Handling**: 
    - `GrapaLibraryEvent::Error()` creates `$ERR` objects with error codes
    - C++ `throw` statements exist but are not integrated with Grapa exception system
    - Scripts can only catch exceptions thrown by other Grapa scripts, not C++ code
  - **Key Insight**: Only throw exceptions when there's an active `try/catch` block to handle them
  - **Technical Requirements**:
    - **Exception Bridge**: Create mechanism for C++ code to throw exceptions that propagate to Grapa scripts
    - **Try Block Detection**: Detect if there's an active `try/catch` block in the current execution context
    - **Conditional Throwing**: Only set `mControlFlow = GrapaControlFlowType::THROW` when try block is active
    - **Backward Compatibility**: Maintain existing behavior when no try block is present
    - **Control Flow Integration**: Integrate with existing `GrapaControlFlowType::THROW` system
  - **Implementation Approach**:
    - **Option 1**: Add try block detection to `Error()` function
      ```cpp
      GrapaRuleEvent* Error(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaError err, bool forceThrow = false) {
          GrapaRuleEvent *result = new GrapaRuleEvent();
          result->mValue.mToken = GrapaTokenType::ERR;
          result->vQueue = new GrapaRuleQueue();
          result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaInt(err).getBytes()));
          
          // Only throw if there's an active try block or forceThrow is true
          if (forceThrow || HasActiveTryBlock(pNameSpace)) {
              result->mControlFlow = GrapaControlFlowType::THROW;
          }
          return result;
      }
      ```
    - **Option 2**: Create new `ThrowError()` function for explicit exception throwing
    - **Option 3**: Add try block detection to existing Event functions
  - **Try Block Detection Strategy**:
    - **Namespace Stack Traversal**: Walk up the `GrapaNames` chain to find active try blocks
    - **Control Flow Tracking**: Check if current execution context is within a try block
    - **Switch Event Analysis**: Since try/catch uses switch mechanism, detect switch events with try semantics
    - **Implementation Approach**:
      ```cpp
      bool HasActiveTryBlock(GrapaNames* pNameSpace) {
          GrapaNames* current = pNameSpace;
          while (current) {
              // Check if current namespace has a try block
              GrapaRuleQueue* nameQueue = current->GetNameQueue();
              if (nameQueue) {
                  GrapaRuleEvent* item = (GrapaRuleEvent*)nameQueue->Head();
                  while (item) {
                      // Look for switch events with try semantics
                      if (item->vLibraryEvent && 
                          item->vLibraryEvent->mName.Cmp("try") == 0) {
                          return true;
                      }
                      item = (GrapaRuleEvent*)item->Next();
                  }
              }
              current = current->GetParrent();
          }
          return false;
      }
      ```
  - **Affected Areas**:
    - **All Event Functions**: Any C++ code that currently calls `Error()` should be able to throw
    - **Mathematical Operations**: Division by zero, invalid operations
    - **File Operations**: File not found, permission errors, I/O failures
    - **Network Operations**: Connection failures, timeout errors
    - **Database Operations**: Query failures, constraint violations
  - **Benefits**:
    - **Unified Error Handling**: Scripts can catch and handle all errors consistently
    - **Better Error Recovery**: Scripts can implement proper error handling strategies
    - **Improved Debugging**: Exceptions provide better error context and stack traces
    - **Language Completeness**: Makes `try/catch/finally` actually useful for real-world scenarios
    - **Backward Compatibility**: Existing code continues to work without try blocks
  - **Use Cases**:
    ```grapa
    // Without try block - returns error object (current behavior)
    result = file.get("nonexistent.txt");  // Returns $ERR object
    if (result.type() == $ERR) {
        echo("File error: " + result.error);
    }
    
    // With try block - throws exception (new behavior)
    try {
        result = file.get("nonexistent.txt");  // C++ throws exception
        data = result.parse();                 // Never reached if file missing
    } catch (error) {
        echo("File error: " + error.error);    // Handle C++ exception
    }
    ```
  - **Example Implementation**:
    ```cpp
    // Current approach
    if (fileNotFound) {
        result = Error(vScriptExec, pNameSpace, -1);
        return result;
    }
    
    // Enhanced approach with try block detection
    if (fileNotFound) {
        result = Error(vScriptExec, pNameSpace, -1);  // Auto-detects try blocks
        return result;
    }
    
    // Explicit throwing (for cases where you always want to throw)
    if (criticalError) {
        result = Error(vScriptExec, pNameSpace, -1, true);  // Force throw
        return result;
    }
    ```
  - **Priority**: High (completes the exception handling system)
  - **Estimated Effort**: Medium Release (2-3 weeks) - requires careful integration testing
  - **Dependencies**: None - can be implemented independently
  - **Status**: **DESIGN NEEDED** - Requires analysis of try block detection mechanism

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

### **Development Tools** ✅ **COMPLETED IN 0.1.52**
- [x] **VSCode Extension**: Comprehensive IDE integration with Grapa language support ✅ **COMPLETED IN 0.1.52**
- **Features**: Syntax highlighting, IntelliSense, debugging integration, language server
- **Status**: Fully functional VSCode extension available on Visual Studio Marketplace
- **Note**: Debugging tools and profiling capabilities are covered in "Enhanced Debug Mode" major release
- **Note**: All tooling enhancements (IDE integration, debugging, profiling, AST dumping) are covered in this section and "Enhanced Debug Mode" major release
- **Note**: All debugging tools (stack traces, profiling, hot reloading, AST dumping) are comprehensively covered in "Enhanced Debug Mode" major release

### **Experimental Features** ✅ **CAPABILITIES ALREADY EXIST**
- [x] **Async/await patterns**: Documented as unnecessary - Grapa's built-in concurrency is superior ✅ **ADDRESSED**
- [x] **Coroutines support**: Fully implemented via `$thread` system with suspend/resume ✅ **IMPLEMENTED**
- [x] **Metaprogramming features**: Core language feature - `$OP` and `$CODE` are foundation ✅ **IMPLEMENTED**
- **Status**: All experimental features either implemented or documented as unnecessary
- **Note**: Threading capabilities documented in `thread.md` with existing coroutine support

### **Future Ecosystem Features** 🔮 **LOW PRIORITY**
- [ ] **C++ Extension Mechanism** - System for developers to create and distribute C++ extensions to Grapa's Event libraries
  - **Scope**: Mechanism for third-party C++ extensions that integrate with Grapa's core Event system
  - **Distribution**: Future PyPI-like system for C++ extension distribution
  - **Timeline**: Only when Grapa gains significant traction and ecosystem
  - **Dependencies**: Requires significant user base and developer community
  - **Complexity**: Major architectural feature requiring distribution infrastructure
  - **Note**: This is a "nice to have" for future ecosystem growth, not a current priority
  - **Note**: Superseded by Plugin Architecture System (see above) - provides more comprehensive extension capabilities

### **Integration**
- [ ] **Add support for more external libraries**
- [ ] **Create bindings for popular frameworks**

### **Plugin Architecture System** 🔥 **HIGH PRIORITY**
- **Status**: Comprehensive plan created, ready for implementation
- **Goal**: Modular plugin architecture for optional features and third-party extensions without bloating core distribution
- **Motivation**: 
  - **Distribution Size Problem**: Current binary ~15-20MB, with embedded features could grow to 100MB+
  - **Python Extension Impact**: Would become 400MB+ with all platforms
  - **Ecosystem Growth**: Enable third-party extensions and community contributions
- **Implementation Phases**:
  - **Phase 1 (Months 1-3)**: Core Plugin Infrastructure - Dynamic library loading, plugin discovery, lifecycle management
  - **Phase 2 (Months 4-6)**: JavaScript Engine Plugin - QuickJS integration (~5MB) as first practical plugin
  - **Phase 3 (Months 7-9)**: Plugin Distribution System - Repository, management tools, development kit
  - **Phase 4 (Months 10-12)**: Advanced Features - Dependencies, performance, security
- **Plugin Categories**:
  - **JavaScript Engine Plugins**: QuickJS (~5MB), V8 (~50MB), SpiderMonkey (~30MB)
  - **Database Plugins**: PostgreSQL, MySQL, MongoDB integrations
  - **Network Protocol Plugins**: WebSocket, MQTT, FTP support
  - **Language Integration Plugins**: Enhanced Python, Rust, Go integrations
  - **Development Tool Plugins**: Debug visualizers, profilers, test runners
- **Distribution Strategy**:
  - **Core Distribution**: ~15MB (no size increase)
  - **Plugin Distribution**: Optional separate packages (5-50MB each)
  - **Python Extension**: Core (~60MB) + optional plugin packages
- **Technical Implementation**:
  - **Plugin Interface**: Standard C++ interface with Grapa integration
  - **Metadata System**: JSON-based plugin descriptions and commands
  - **Grapa Integration**: `$sys().plugins()` management system
  - **Cross-Platform**: Support for Linux, macOS, Windows
- **Success Metrics**:
  - **Core Binary Size**: <20MB (no increase from current)
  - **Plugin Loading Time**: <1 second for typical plugins
  - **Memory Overhead**: <10MB per loaded plugin
  - **Ecosystem Growth**: 5+ plugins within 6 months
- **Estimated Effort**: Major Release (6-12 months across 4 phases)
- **Dependencies**: None - foundational architecture work
- **Priority**: High - enables ecosystem growth without distribution bloat
- **Risk Level**: Medium - new architectural feature requiring careful design
- **Reference**: [`../IMPLEMENTATION/PLUGIN_ARCHITECTURE_PLAN.md`](../IMPLEMENTATION/PLUGIN_ARCHITECTURE_PLAN.md)
- **Next Steps**: Begin Phase 1 implementation (Core Plugin Infrastructure), design plugin loading system, create QuickJS JavaScript engine plugin as first example

### **Advanced Language Features**
- [ ] **IDE Integration**: Improve error messages and syntax highlighting

---

## 🔧 **INFRASTRUCTURE & PLATFORM** (2 items)

### **OpenSSL Hash Digest Expansion** 🔧 **CRYPTOGRAPHIC ENHANCEMENT** 🔍 **EVALUATION NEEDED**
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
- **Current Assessment**: **NEEDS EVALUATION** - Determine if missing algorithms are actually needed for production use

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

### **ReplaceLocalQueue Optimization** 🔍 **EVALUATION: MAY NOT BE NEEDED**
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
- **Current Assessment**: **MAY NOT BE NEEDED** - Complete copy approach works well and may be sufficient for production use

### **ProcessPlan Control Flow Copy Optimization** 🔍 **ADDRESSED IN 0.1.52**
- **Status**: Automatic copy in control flow section removed, intelligent copy detection now working correctly ✅ **RESOLVED**
- **Focus**: Evaluate all automatic copies in ProcessPlan for performance optimization opportunities
- **Context**: The automatic copy in `if (result->mControlFlow)` section was interfering with intelligent copy detection system
- **Current State**: 
  - Automatic copy removed from control flow section in `GrapaScriptExec::ProcessPlan` ✅ **COMPLETED**
  - Intelligent copy detection now works properly for local variable references ✅ **WORKING**
  - Throw scenarios with local variable references now work correctly ✅ **FIXED**
- **Performance Impact**: 
  - Each automatic copy needs evaluation for necessity
  - Local variables are tagged appropriately for recursive inspection
  - Copy should only be made if local variable references are detected
- **Progress**: 
  - ✅ Throw scenario fixed by removing unnecessary automatic copy ✅ **COMPLETED**
  - ✅ Intelligent copy detection system now working as intended ✅ **WORKING**
  - ✅ mVar system properly detecting local variable references ✅ **VERIFIED**
- **Next Steps**: 
  - Audit all automatic copies in ProcessPlan for similar optimization opportunities
  - Implement recursive inspection to detect when copies are actually needed
  - Ensure performance optimization without breaking existing functionality
- **Optimization Ideas**: 
  - **mClean flag approach**: Add `mClean` boolean flag that is set to `true` after a copy is made, then checked before making subsequent copies to avoid redundant copying
  - **Recursive inspection**: Use existing `mVar` system to detect when local variable references are present and only copy when necessary
- **Priority**: Medium - performance optimization, but current system works correctly ✅ **CORE ISSUE RESOLVED**
- **Status**: **ADDRESSED IN 0.1.52** - Main control flow copy issue resolved, system now working correctly

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

## 🔍 **LANGUAGE & SYNTAX BACKLOG UPDATE** (December 2024)

### **Remaining Language Features by Priority**

#### **🔥 HIGHEST PRIORITY - Grapa Syntax Improvements**
**Status**: Exception Handling ✅ **COMPLETED**, Control Flow ✅ **COMPLETED**, Property Access ✅ **COMPLETED**

**All Items Completed:**
1. **Control Flow Fixes**: Working return/break/continue statements ✅ **COMPLETED**
   - **Status**: All control flow statements working correctly
   - **Verified**: `return`, `break`, `continue` all function properly
   - **Priority**: Highest - foundational for language adoption ✅ **DONE**

2. **Property Access**: Attribute-style access for objects ✅ **COMPLETED**
   - **Status**: Basic property access works correctly
   - **Verified**: `obj.property` syntax works for `$LIST` objects
   - **Priority**: High - improves developer experience ✅ **DONE**

#### **🟡 MEDIUM PRIORITY - Language Features**

3. **Vector Functions for Single Dimension Vectors** ✅ **COMPLETED**
   - **Status**: ✅ **COMPLETED** - All statistical functions now support both 1D and 2D vectors
   - **Scope**: `.sum()`, `.mean()`, `.min()`, `.max()`, `.std()`, `.variance()` for 1D and 2D vectors
   - **Verified**: All functions working correctly with both 1D and 2D vectors
   - **Priority**: Medium - useful for data analysis ✅ **DONE**
   - **Estimated Effort**: 1-2 weeks ✅ **COMPLETED**

4. **C++ Exception Handling Expansion** 🔥 **HIGH PRIORITY** - **USER EXPERIENCE**
   - **Status**: Foundation complete, expansion needed
   - **Goal**: Expand C++ to Grapa exception bridge to high-expectation areas where users most expect exceptions
   - **Foundation**: ✅ **COMPLETED** - Basic exception bridge implemented with `mInTryBlock` flag and `HasActiveTryBlock()` detection
   - **Current State**: Mathematical errors (division by zero, etc.) properly throw exceptions when try block is active
   - **Scope**: Add exception throwing to high-impact, high-expectation areas using existing architecture
   
   **Priority Implementation Areas** (in order):
   - [ ] **File Operations** (Highest Priority)
     - `file.get("nonexistent.txt")` should throw `FILE_NOT_FOUND`
     - `file.set()` operations should throw on permission/IO errors
     - **Why**: Most expected by users, classic exception use case
     - **Implementation**: Use existing `Error()` function in file operation paths
   
   - [ ] **Array/List/Vector Bounds Checking** (High Priority)
     - Out-of-bounds access should throw `INDEX_OUT_OF_BOUNDS`
     - Applies to `.get()`, `.set()`, bracket notation access
     - **Why**: Classic bounds checking exception, users expect this
     - **Implementation**: Add bounds checking to `GrapaLibraryRuleFileGetEvent::Run`
   
   - [ ] **Database/Table Operations** (High Priority)
     - Query failures, record not found, constraint violations
     - **Why**: Database operations benefit greatly from exception handling
     - **Implementation**: Add to database query execution paths
   
   - [ ] **Type Conversion Errors** (Medium Priority)
     - `string.toInt()` with invalid input should throw `INVALID_CONVERSION`
     - JSON parsing failures, invalid format conversions
     - **Why**: Common error case that users expect to handle
   
   - [ ] **Network Operations** (Medium Priority)
     - Connection failures, timeouts, DNS resolution errors
     - **Why**: Network operations are inherently error-prone
   
   **Implementation Strategy**:
   - **No Architecture Changes**: Use existing `Error()` function and try block detection
   - **Incremental Rollout**: Implement one area at a time, starting with file operations
   - **Backward Compatibility**: Maintains existing behavior when no try block is active
   - **Testing**: Each area gets comprehensive exception handling tests
   
   **Benefits**:
   - **User Experience**: Exceptions where users most expect them
   - **Error Handling**: Cleaner error handling in user scripts
   - **Language Completeness**: Makes try/catch actually useful for real-world scenarios
   - **Debugging**: Better error context and stack traces
   
   - **Priority**: High - significantly improves user experience and language usability
   - **Estimated Effort**: Medium Release (4-6 weeks) - incremental implementation
   - **Dependencies**: Exception bridge foundation ✅ **COMPLETED**
   - **Risk Level**: Low - uses existing architecture, incremental rollout

5. **Operator Overloading**
   - **Status**: Not started
   - **Scope**: Allow objects/classes to implement custom operator behavior
   - **Priority**: Low - nice to have feature
   - **Estimated Effort**: 1-2 months (large release)

5. **Object Constructors and Destructors**
   - **Status**: ✅ **PARTIALLY IMPLEMENTED**
   - **Scope**: Automatic constructor/destructor support for Grapa objects
   - **Verified**: `$new` constructors work correctly in `$ERR` and `$WIDGET` classes
   - **Implementation**: Classes can define `$new = op(...)` method that is automatically called
   - **Priority**: Low - nice to have feature
   - **Estimated Effort**: 2-4 weeks (destructors still needed)

6. **Module Loading Path Enhancement**
   - **Status**: Under consideration
   - **Scope**: Support for specifying file paths in module loading
   - **Priority**: Low - current system works well
   - **Estimated Effort**: 2-4 weeks

7. **$LIST and $file API Unification**
   - **Status**: Partially working
   - **Scope**: Combine common operations between list and file objects
   - **Verified**: `$LIST` property access works (`list.a`), but `$file` property access doesn't work
   - **Priority**: Low - improves API consistency
   - **Estimated Effort**: 3-4 weeks

#### **🟢 LOW PRIORITY - Experimental Features**

8. **Async/await patterns**
    - **Status**: ✅ **DOCUMENTED AS UNNECESSARY**
    - **Decision**: Grapa's built-in concurrency (`.map()`, `$thread()`, `$net` callbacks) provides superior capabilities
    - **Priority**: None - not needed

9. **Coroutines support**
    - **Status**: ✅ **FULLY IMPLEMENTED**
    - **Implementation**: `$thread` system provides complete coroutine support with `suspend()`, `resume()`, `wait()`, `signal()`, `trylock()`, `lock()`, `unlock()`, `waiting()`, `suspended()` methods
    - **Priority**: None - already complete
    - **Note**: Used extensively in lexer→compiler→executor pipeline

10. **Metaprogramming features**
    - **Status**: ✅ **CORE LANGUAGE FEATURE**
    - **Implementation**: `$OP` and `$CODE` are the foundation of the entire language. Rules engine is fully mutable at runtime. Everything is metaprogramming.
    - **Priority**: None - this is what makes Grapa unique
    - **Note**: Probably more metaprogramming-focused than any other language including LISP

### **Infrastructure & Dependencies**

#### **🔥 HIGH PRIORITY**
11. **OpenSSL 3.0 Migration**
    - **Status**: Ready for implementation
    - **Priority**: High - Linux compatibility blocker
    - **Estimated Effort**: 2-4 weeks

12. **FLTK X11 Dependency Investigation**
    - **Status**: Requires investigation
    - **Priority**: High - dependency modernization
    - **Estimated Effort**: 1-2 weeks

#### **🟡 MEDIUM PRIORITY**
13. **FLTK 1.4.4 Migration**
    - **Status**: Not started
    - **Priority**: Medium - recommended upgrade
    - **Estimated Effort**: 1-2 weeks

### **Performance & Optimization**

14. **Vector Performance Optimization**
    - **Status**: Comprehensive design plan completed
    - **Priority**: Lower - after core features are stable
    - **Estimated Effort**: 4-6 months across 5 phases

15. **General Optimization Implementation**
    - **Status**: Requires extensive testing
    - **Priority**: Lower - after core features are stable
    - **Estimated Effort**: 6-12 months

### **Summary of Language & Syntax Priorities**

**Immediate Focus (Next 2-3 months):**
1. **FLTK X11 Investigation** - Dependency modernization
2. **$file Property Access** - Fix property access for $file objects

**Medium-term Focus (3-6 months):**
5. **FLTK 1.4.4 Migration** - Recommended upgrade
6. **Object Destructors** - Add destructor support (constructors already work)
7. **Module Loading Path Enhancement** - Support for custom paths

**Long-term Focus (6+ months):**
7. **Operator Overloading** - Advanced language feature
8. **Performance Optimization** - After core features are stable

**Completed in 0.1.52 (2024):**
- Exception handling (try/catch/finally)
- Control flow (return/break/continue)
- Property access for $LIST objects
- `.find()` method for all data types
- Array/list slicing fixes
- Boolean string conversion
- $ERR constructor support
- Object constructors ($new method)
- Switch statement comparison fixes
- Key exchange methods (DH, EC, RPK)
- Async/await documentation (why unnecessary)
- Vector functions (sum, mean, min, max, std, variance) for 1D and 2D vectors
- Comprehensive documentation updates

---

## 📝 **IMPORTANT NOTES FOR 0.1.53 DEVELOPMENT**

### **✅ 0.1.52 COMPLETED WORK (DO NOT DUPLICATE)**
All major infrastructure, language foundation, and development tools were completed in 0.1.52. These items should **NOT** appear in `current_status.md` as pending work for 0.1.53.

### **🚀 0.1.53+ FUTURE WORK**
The remaining tasks in this backlog are for future releases (0.1.53+) and represent new development work beyond what was accomplished in 0.1.52.

### **📋 TRACKING SEPARATION**
- **`current_status.md`**: Tracks active development for 0.1.53
- **`backlog.md`**: Documents 0.1.52 completions and future work for 0.1.53+
- **No overlap**: Ensure completed 0.1.52 work is not duplicated as pending work 