---
tags:
  - maintainer
  - research
  - analysis
  - debug-mode
  - implementation
---

# Grapa Enhanced Debug Mode Implementation Plan

**Document Status:** Planning Phase  
**Created:** August 2025  
**Last Updated:** August 2025  
**Priority:** MEDIUM - Development Support  
**Estimated Effort:** Major Release (6-12 months)  
**Dependencies:** CLI Enhancement Phase 2 completion  

## Overview

This document outlines the comprehensive plan for implementing Enhanced Debug Mode in Grapa, which would provide sophisticated debugging capabilities including execution tracing, variable inspection, performance profiling, and AST/bytecode dumping. This is a significant undertaking that requires its own release cycle due to the scope and complexity of changes needed.

## Current State Analysis

### Existing Debug Infrastructure

**Current Implementation:**
```cpp
// GrapaSystem.cpp - Basic debug output
void GrapaSystem::DebugPrint(const char* pStr, bool flush)
{
    if (!mDebugMode) return;
    
    mPrintLock.WaitCritical();
    fprintf(stderr, "[DEBUG] %s", pStr);
    if (flush) fflush(stderr);
    mPrintLock.LeaveCritical();
}
```

**Current Capabilities:**
- ✅ Sets `mDebugMode = true` when `-d` flag is used
- ✅ Basic `[DEBUG]` prefix output to stderr
- ✅ Thread-safe printing with locks
- ❌ **No execution tracing**
- ❌ **No variable inspection**
- ❌ **No AST/bytecode dumping**
- ❌ **No performance profiling**
- ❌ **No step-by-step execution**

### Existing Debug Patterns

The codebase already has sophisticated debug patterns that would need integration:

```cpp
// Example from grapa_grep_unicode.cpp
#ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
printf("DEBUG: split_by_delimiter called with input length %zu, delimiter '%s' (length %zu)\n", 
       input.length(), actual_delim.c_str(), actual_delim.length());
#endif // DEBUG_END
```

**Patterns Found:**
- Conditional compilation with `#ifdef GRAPA_DEBUG_PRINTF`
- Structured debug output with context
- Performance timing with `$TIME().utc()`
- Error tracking and reporting

## Proposed Enhanced Debug Features

### Phase 1: Core Debug Infrastructure (Months 1-3)

#### 1.1 Enhanced Debug System
```cpp
// Proposed enhanced debug system
class GrapaDebugSystem {
public:
    enum DebugLevel {
        DEBUG_NONE = 0,
        DEBUG_BASIC = 1,
        DEBUG_TRACE = 2,
        DEBUG_VERBOSE = 3,
        DEBUG_PROFILE = 4
    };
    
    struct DebugContext {
        std::string operation;
        std::string file;
        int line;
        std::map<std::string, std::string> variables;
        uint64_t start_time;
    };
    
    void TraceEnter(const std::string& operation, const std::string& file, int line);
    void TraceExit(const std::string& operation, uint64_t duration);
    void VariableSnapshot(const std::map<std::string, std::string>& vars);
    void PerformanceMark(const std::string& mark, uint64_t duration);
};
```

#### 1.2 Execution Tracing
```cpp
// Proposed execution trace output
[DEBUG] [TRACE] Entering: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}>
[DEBUG] [TRACE]   Evaluating: @<mul,{@<var,{x}>,2}>
[DEBUG] [TRACE]     Variable 'x' = 5
[DEBUG] [TRACE]     Result: 10
[DEBUG] [TRACE]   Evaluating: @<add,{10,1}>
[DEBUG] [TRACE]     Result: 11
[DEBUG] [TRACE] Exiting: 11
```

#### 1.3 Variable Inspection
```cpp
// Proposed variable inspection
[DEBUG] [VARS] Namespace: {"x": 5, "y": 3}
[DEBUG] [VARS] Local variables: {"result": 11, "temp": 10}
[DEBUG] [VARS] Global variables: {"config": {"debug": true}}
```

### Phase 2: Advanced Debug Features (Months 4-6)

#### 2.1 Performance Profiling
```cpp
// Proposed performance profiling system
class GrapaDebugProfiler {
public:
    struct PerformanceMark {
        std::string name;
        uint64_t start_time;
        uint64_t end_time;
        std::map<std::string, uint64_t> metrics;
    };
    
    void StartMark(const std::string& name);
    void EndMark(const std::string& name);
    void AddMetric(const std::string& mark, const std::string& metric, uint64_t value);
    std::vector<PerformanceMark> GetMarks();
    void GenerateReport();
};
```

#### 2.2 AST/Bytecode Dumping
```cpp
// Proposed AST dumping system
class GrapaDebugAST {
public:
    struct ASTNode {
        std::string type;
        std::string value;
        std::vector<ASTNode*> children;
        std::map<std::string, std::string> attributes;
    };
    
    ASTNode* BuildAST(GrapaRuleEvent* execution_tree);
    void DumpAST(ASTNode* root, std::ostream& output);
    void DumpBytecode(GrapaRuleEvent* execution_tree, std::ostream& output);
    std::string ASTToJSON(ASTNode* root);
};
```

#### 2.3 Error Context and Debugging
```cpp
// Proposed error context system
class GrapaDebugErrorContext {
public:
    struct ErrorContext {
        std::string error_type;
        std::string error_message;
        std::string file;
        int line;
        std::string function;
        std::vector<std::string> stack_trace;
        std::map<std::string, std::string> variables;
        uint64_t timestamp;
    };
    
    void CaptureErrorContext(const std::exception& e);
    void CaptureErrorContext(const GrapaCHAR& error);
    ErrorContext GetErrorContext();
    void DumpErrorContext(const ErrorContext& context, std::ostream& output);
    std::vector<ErrorContext> GetErrorHistory();
};
```

**Enhanced Error Debugging Features:**
```grapa
// Error context information in debug mode
try {
    result = risky_operation();
} catch (error) {
    // Enhanced error information available in debug mode
    error_context = error.debug_context();  // Get detailed error context
    stack_trace = error.stack_trace();     // Get execution stack trace
    variables = error.variables_snapshot(); // Get variable state at error
    file_location = error.file_location(); // Get file and line number
    
    // Debug output in enhanced debug mode
    if ($sys().debug_mode() >= 2) {
        ("Error Type: " + error_context.type).echo();
        ("Error Message: " + error_context.message).echo();
        ("File: " + error_context.file + ":" + error_context.line).echo();
        ("Function: " + error_context.function).echo();
        
        "Stack Trace:".echo();
        error_context.stack_trace.map(op(frame) { frame.echo(); });
        
        "Variables at Error:".echo();
        error_context.variables.map(op(name, value) { 
            ("  " + name + " = " + value).echo(); 
        });
    }
}

// Error aggregation and debugging
errors = [];
results = operations.map(op(operation) {
    try {
        operation.execute();
    } catch (error) {
        // Capture full error context for debugging
        error_context = error.debug_context();
        errors.push(error_context);
        return null;
    }
});

if (errors.len() > 0) {
    ("Encountered " + errors.len().str() + " errors during execution").echo();
    
    // Enhanced error reporting in debug mode
    if ($sys().debug_mode() >= 1) {
        errors.map(op(error_ctx) {
            ("Error " + error_ctx.type + ": " + error_ctx.message).echo();
            ("  Location: " + error_ctx.file + ":" + error_ctx.line).echo();
            ("  Function: " + error_ctx.function).echo();
        });
    }
}
```

**Implementation Strategy:**
1. **Error context capture** - Automatic error context capture during exception handling
2. **Stack trace generation** - Execution stack trace generation and formatting
3. **Variable state snapshots** - Variable state capture at error points
4. **Error history tracking** - Comprehensive error history and debugging information
5. **Debug mode integration** - Integration with existing debug mode levels
6. **Error reporting enhancement** - Enhanced error reporting with context information
7. **Cross-platform support** - Platform-specific error context capture
8. **Performance optimization** - Lazy evaluation of error context information

### Phase 3: Integration and Tools (Months 7-9)

#### 3.1 Debug CLI Options
```bash
# Enhanced debug options
grapa -d                    # Basic debug mode
grapa --debug=trace         # Execution tracing
grapa --debug=verbose       # Verbose debug output
grapa --debug=profile       # Performance profiling
grapa --debug=ast           # AST dumping
grapa --debug=memory        # Memory usage tracking
grapa --debug=all           # All debug features
```

#### 3.2 Debug Output Formats
```bash
# JSON format for machine processing
grapa --debug=json --debug=trace script.grc

# Human-readable format
grapa --debug=human --debug=trace script.grc

# Compact format for logging
grapa --debug=compact --debug=trace script.grc
```

## Implementation Requirements

### Core Files Requiring Modification

#### Primary Files (High Impact)
1. **`source/grapa/GrapaSystem.cpp`** - Core debug infrastructure
2. **`source/grapa/GrapaSystem.h`** - Debug mode definitions
3. **`source/grapa/GrapaScriptExec.cpp`** - Execution tracing
4. **`source/grapa/GrapaScriptExec.h`** - Debug interfaces
5. **`source/grapa/GrapaState.cpp`** - Lexical analysis debugging

#### Secondary Files (Medium Impact)
6. **`source/grapa/GrapaLibRule.cpp`** - Library call tracing
7. **`source/grapa/GrapaLink.cpp`** - CLI debug options
8. **`source/grapa/GrapaConsole.cpp`** - Debug output handling
9. **`source/grapa/GrapaRuleEvent.cpp`** - Rule execution tracing
10. **`source/grapa/GrapaNames.cpp`** - Variable inspection

#### Supporting Files (Low Impact)
11. **`source/grep/grapa_grep_unicode.cpp`** - Grep debug integration
12. **`source/grep/grep_unicode.cpp`** - Grep debug integration
13. **`source/grapa/GrapaDB.cpp`** - Database debug integration
14. **`source/grapa/GrapaBtree.cpp`** - BTree debug integration
15. **`source/grapa/GrapaFileIO.cpp`** - File I/O debug integration

### New Files to Create

#### Debug Infrastructure
1. **`source/grapa/GrapaDebugSystem.cpp`** - Core debug system
2. **`source/grapa/GrapaDebugSystem.h`** - Debug system interface
3. **`source/grapa/GrapaDebugContext.cpp`** - Debug context management
4. **`source/grapa/GrapaDebugContext.h`** - Debug context interface
5. **`source/grapa/GrapaDebugOutput.cpp`** - Debug output formatting
6. **`source/grapa/GrapaDebugOutput.h`** - Debug output interface

#### Debug Tools
7. **`source/grapa/GrapaDebugTracer.cpp`** - Execution tracing
8. **`source/grapa/GrapaDebugTracer.h`** - Tracing interface
9. **`source/grapa/GrapaDebugProfiler.cpp`** - Performance profiling
10. **`source/grapa/GrapaDebugProfiler.h`** - Profiling interface
11. **`source/grapa/GrapaDebugAST.cpp`** - AST dumping
12. **`source/grapa/GrapaDebugAST.h`** - AST interface

## Technical Challenges

### 1. Performance Impact

**Current debug mode is lightweight:**
```cpp
if (!mDebugMode) return;  // Early exit if not enabled
```

**Enhanced debug mode would have significant overhead:**
- Execution tree traversal tracking
- Variable state snapshots
- Performance timing measurements
- Memory usage monitoring
- AST construction and dumping

**Mitigation Strategies:**
- Conditional compilation for debug features
- Lazy evaluation of debug information
- Configurable debug levels
- Performance impact measurement tools

### 2. Thread Safety

**Current implementation:**
```cpp
mPrintLock.WaitCritical();
fprintf(stderr, "[DEBUG] %s", pStr);
mPrintLock.LeaveCritical();
```

**Enhanced requirements:**
- Thread-safe debug context management
- Concurrent execution tracing
- Thread-local debug state
- Cross-thread debug coordination

### 3. Memory Management

**Challenges:**
- Debug context memory allocation
- Variable state snapshots
- AST tree construction
- Memory leak prevention

**Solutions:**
- RAII-based debug context management
- Smart pointer usage for debug objects
- Memory pool for debug allocations
- Automatic cleanup mechanisms

### 4. Cross-Platform Compatibility

**Platform-specific considerations:**
- Windows: Debug output to debugger
- macOS: Console logging integration
- Linux: syslog integration
- Cross-platform timing accuracy

## Testing Strategy

### 1. Unit Testing

**Debug Infrastructure Tests:**
- Debug system initialization
- Debug level configuration
- Debug context management
- Debug output formatting

**Execution Tracing Tests:**
- Function entry/exit tracing
- Variable state tracking
- Performance timing accuracy
- Memory usage tracking

### 2. Integration Testing

**End-to-End Tests:**
- Complete script execution with debug
- Debug output accuracy
- Performance impact measurement
- Memory leak detection

**Cross-Platform Tests:**
- Windows debug output
- macOS debug output
- Linux debug output
- Cross-platform consistency

### 3. Performance Testing

**Benchmark Tests:**
- Debug mode performance impact
- Memory usage overhead
- Debug output generation speed
- Large script debugging performance

### 4. Regression Testing

**Compatibility Tests:**
- Existing debug patterns
- Current debug mode functionality
- Third-party integration
- Backward compatibility

## Documentation Requirements

### 1. User Documentation

**Debug Mode Guide:**
- Debug mode usage examples
- Debug output interpretation
- Performance impact guidelines
- Troubleshooting guide

**CLI Reference:**
- Debug command-line options
- Debug output formats
- Debug level configuration
- Debug tool integration

### 2. Developer Documentation

**Implementation Guide:**
- Debug system architecture
- Debug integration patterns
- Debug API reference
- Debug extension development

**Maintenance Guide:**
- Debug system maintenance
- Debug performance tuning
- Debug troubleshooting
- Debug system updates

### 3. API Documentation

**Debug System API:**
- GrapaDebugSystem class reference
- Debug context management
- Debug output formatting
- Debug tool integration

## Release Planning

### Phase 1: Foundation (Months 1-3)

**Deliverables:**
- Enhanced debug system infrastructure
- Basic execution tracing
- Variable inspection framework
- Debug output formatting

**Success Criteria:**
- Debug system compiles and runs
- Basic execution tracing works
- Variable inspection functional
- Debug output readable

### Phase 2: Advanced Features (Months 4-6)

**Deliverables:**
- Performance profiling
- AST/bytecode dumping
- Memory usage tracking
- Advanced debug tools

**Success Criteria:**
- Performance profiling accurate
- AST dumping functional
- Memory tracking working
- Advanced tools integrated

### Phase 3: Integration (Months 7-9)

**Deliverables:**
- CLI integration
- Cross-platform compatibility
- Documentation complete
- Testing comprehensive

**Success Criteria:**
- CLI options working
- Cross-platform compatibility
- Documentation complete
- Testing passing

### Phase 4: Release (Months 10-12)

**Deliverables:**
- Production-ready debug mode
- Performance optimized
- Documentation published
- User training materials

**Success Criteria:**
- Production stability
- Performance acceptable
- Documentation complete
- User adoption

## Risk Assessment

### High Risk

1. **Performance Impact**: Debug mode could significantly slow down execution
2. **Memory Usage**: Debug tracking could consume excessive memory
3. **Thread Safety**: Concurrent debug operations could cause issues
4. **Cross-Platform Compatibility**: Debug output could vary across platforms

### Medium Risk

1. **Code Complexity**: Debug system could make codebase more complex
2. **Maintenance Overhead**: Debug system could require ongoing maintenance
3. **User Adoption**: Users might not adopt new debug features
4. **Integration Issues**: Debug system could conflict with existing code

### Low Risk

1. **Documentation**: Documentation could be incomplete or unclear
2. **Testing**: Testing could miss edge cases
3. **User Training**: Users might need training on new features
4. **Support**: Support burden could increase

## Success Metrics

### Technical Metrics

1. **Performance Impact**: < 5% performance degradation with debug disabled
2. **Memory Usage**: < 10% memory overhead with debug enabled
3. **Thread Safety**: No race conditions in concurrent execution
4. **Cross-Platform Compatibility**: Consistent behavior across platforms

### User Metrics

1. **User Adoption**: > 50% of users adopt debug features within 6 months
2. **User Satisfaction**: > 80% user satisfaction with debug features
3. **Support Reduction**: > 20% reduction in debug-related support requests
4. **Documentation Usage**: > 70% of users reference debug documentation

### Quality Metrics

1. **Test Coverage**: > 90% test coverage for debug features
2. **Code Quality**: < 5% code complexity increase
3. **Documentation Quality**: > 95% documentation completeness
4. **Bug Rate**: < 1% bug rate for debug features

## Conclusion

Enhanced Debug Mode is a significant undertaking that requires careful planning, implementation, and testing. The proposed phased approach allows for incremental development and validation while minimizing risk. The comprehensive debug capabilities would significantly improve the developer experience and make Grapa more suitable for complex applications and debugging scenarios.

**Next Steps:**
1. Review and approve this plan
2. Allocate resources for Phase 1 implementation
3. Begin detailed design for debug system architecture
4. Create implementation timeline and milestones
5. Set up testing infrastructure for debug features

---

## References

- [Grapa System Architecture](IMPLEMENTATION/GRAPA_SYSTEM_ARCHITECTURE.md)
- [Execution Trees](IMPLEMENTATION/EXECUTION_TREES.md)
- [CLI Implementation](IMPLEMENTATION/GRAPA_CLI_IMPLEMENTATION.md)
- [Testing Strategy](TESTING_AND_QUALITY/TESTING.md)
- [Performance Optimization](RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md) 