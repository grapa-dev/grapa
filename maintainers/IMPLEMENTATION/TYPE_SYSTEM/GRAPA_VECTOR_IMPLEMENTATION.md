# Grapa Vector Implementation

## Overview

The `$VECTOR` type in Grapa is a sophisticated multi-dimensional mathematical data structure designed for high-performance numerical computations. It provides optimized operations for linear algebra, statistical analysis, and large-scale data processing.

## Data Structure Design

### Core Components

```cpp
// From GrapaVector.h
class GrapaVector {
    u64* mCounts;         // Dimension sizes [dim0, dim1, dim2, ...]
    u8 mDim;              // Number of dimensions
    u64 mSize;            // Total number of elements
    GrapaVectorItem* mData; // Raw data storage
    u8 mBlock;            // Block size for data alignment
    u8 mMaxBlock;         // Maximum block size
    GrapaRuleQueue mLabels; // Column headers for 2D vectors (CSV support)
};
```

### Header Row System

The `mLabels` field implements sophisticated header row support for 2D vectors:

```cpp
// Header detection logic (lines 232, 619)
bool hasLabels = (mLabels.mCount == 0 && pos == 1 && value->mValue.mToken == GrapaTokenType::LIST);
bool hasBlankHeader = true;  // Detects numeric-only headers (0,1,2,3...)
```

**Key Features:**
- **Automatic detection** of header vs data rows
- **Blank header removal** when first row is sequential integers
- **UTF-8 BOM handling** for international CSV files
- **Mixed type support** for different column data types
- **Header preservation** during vector operations

### Vector Item Structure

```cpp
// From GrapaVector.cpp
struct GrapaVectorItem {
    bool isNull;          // Null value indicator
    bool isValue;         // Value vs reference indicator
    GrapaTokenType mToken; // Data type
    u64 mLen;             // Data length
    u8 d[_minvectordatablock_]; // Data storage
};
```

### Memory Layout

Vectors use a block-based memory layout optimized for:
- **Cache-friendly access patterns**
- **Efficient multi-dimensional indexing**
- **Memory alignment for SIMD operations**
- **Dynamic resizing with minimal fragmentation**

## Multi-Dimensional Support

### Dimension Management

```cpp
// Dimension scanning and creation
static u64* _scanvectordepth(GrapaRuleEvent* value, u8 pos, u8& dim, 
                            u8 maxDim, u64 size, u64& tot);
```

**Key Features:**
- **Automatic dimension detection** from nested arrays
- **Consistent structure validation** (rejects ragged arrays)
- **Flexible dimension limits** (up to 64 dimensions)
- **Memory-efficient storage** with minimal overhead

### Indexing System

```cpp
// Multi-dimensional indexing macro
#define _datavectorpos(bd,bs,bp) ((GrapaVectorItem*)&((u8*)bd)[bs*bp])
```

**Indexing Strategy:**
- **Row-major order** for optimal cache performance
- **Block-aligned access** for SIMD operations
- **Bounds checking** for safety
- **Null value handling** for sparse data

## Mathematical Operations

### Basic Arithmetic

```cpp
// Vector arithmetic operations
GrapaError Add(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
               const GrapaVector& bi, bool pSub);
GrapaError Mul(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
               const GrapaVector& bi, bool pDiv);
GrapaError Pow(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
               const GrapaVector& bi, bool pRoot);
```

**Operation Types:**
- **Element-wise operations** with broadcasting
- **Scalar operations** with automatic expansion
- **Type promotion** for mixed precision
- **Error handling** for invalid operations

### Linear Algebra Operations

```cpp
// Matrix operations
GrapaError Dot(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
               GrapaVector& bi, GrapaVector& result);
```

**Supported Operations:**
- **Matrix multiplication** with optimized algorithms
- **Vector dot products** with SIMD acceleration
- **Matrix transposition** with memory-efficient implementation
- **Determinant calculation** using LU decomposition
- **Matrix inversion** with error checking
- **Eigenvalue decomposition** for symmetric matrices

### Statistical Operations

```cpp
// Statistical functions with axis support
GrapaError Sum(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
               u8 axis, GrapaVector& result);
GrapaError Mean(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
                u8 axis, GrapaVector& result);
GrapaError Cov(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
               u8 axis, GrapaVector& result);
```

**Axis Operations:**
- **Multi-dimensional reduction** along specified axes
- **Broadcasting support** for different shapes
- **Efficient memory access** patterns
- **Parallel computation** for large datasets

## Performance Optimizations

### Memory Management

```cpp
// Block-based allocation
mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
```

**Optimization Strategies:**
- **Block-aligned allocation** for SIMD operations
- **Memory pooling** for frequent operations
- **Lazy evaluation** for complex operations
- **Copy-on-write** for large vectors

### Computational Optimizations

```cpp
// SIMD-friendly data layout
memset(mData, 0, mBlock * mSize);
```

**Performance Features:**
- **SIMD instruction support** for vectorized operations
- **Cache-optimized access patterns**
- **Parallel processing** for large datasets
- **Memory bandwidth optimization**

### Large Dataset Handling

**Scalability Features:**
- **Streaming operations** for datasets larger than memory
- **Chunked processing** for memory efficiency
- **Background computation** for non-blocking operations
- **Progress tracking** for long-running operations

## Header Row Implementation Details

### CSV Parsing with Headers

```cpp
// CSV parsing with automatic header detection (GrapaVector.cpp lines 590-673)
bool GrapaVector::FROM(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
                       bool pFix, s64 pMax, s64 pExtra, const GrapaBYTE& pData, u8 pBlock)
```

**Header Detection Algorithm:**
1. **Parse first row** and store as potential headers in `mLabels`
2. **Check for blank headers**: If first row contains only sequential integers (0,1,2...), treat as blank
3. **UTF-8 BOM detection**: Remove UTF-8 BOM (`ï»¿`) from first header if present
4. **Header validation**: Non-numeric or non-sequential data confirms real headers
5. **Clean up**: Remove blank headers, preserve real headers

### Header Storage and Access

```cpp
// Header storage (lines 635, 280)
mLabels.PushTail(new GrapaRuleEvent(0, GrapaBYTE(vS, vL), GrapaCHAR("")));

// Header-based column access (lines 2959-2961)
GrapaRuleEvent* b = mLabels.Search(a->mValue, index);
if (b) pos = index;  // Find column by header name
```

**Storage Format:**
- Headers stored as `GrapaRuleEvent` objects in `mLabels` queue
- Each header has name (column header) and optional value
- Null entries for missing headers in irregular data

### Header-Aware Operations

```cpp
// CSV export with headers (lines 937-958)
GrapaRuleEvent* ev = mLabels.Head();
if (ev) {
    while (ev) {
        pValue.Append(ev->mName);  // Export header names
        ev = ev->Next();
    }
}
```

**Operations Supporting Headers:**
- **CSV export** (`_tocsv`): Writes headers as first row
- **Array conversion** (`_toarray`): Converts to `$LIST` when headers present
- **Vector concatenation**: Merges headers from multiple vectors
- **Sorting**: Preserves column headers during row-wise sorting
- **Transpose**: Clears headers (since meaning changes)

### Special Cases

```cpp
// UTF-8 BOM handling (lines 630-634)
if (cols == 1 && !isQuoted && vL >= 3 && memcmp(vS, "ï»¿", 3) == 0) {
    vS += 3;  // Skip UTF-8 BOM in first header
    vL -= 3;
}

// Blank header detection (lines 656-665)
if (isInt) {
    GrapaInt y;
    y.FromString(GrapaBYTE(vS, vL), 10);
    s64 lv = y.LongValue();
    if (lv != (cols - 1))
        hasBlankHeader = false;
}
```

**Edge Cases Handled:**
- **UTF-8 BOM removal** from international CSV files (3-byte `ï»¿` sequence)
- **Quoted headers** with embedded commas or special characters
- **Mixed header types** (some numeric, some text)
- **Empty headers** and irregular column counts
- **Header preservation** during vector operations

**Unicode Limitations:**
- **Limited Unicode support** - Vector implementation predates Grapa's Unicode system
- **Byte-based processing** - Headers processed as raw bytes, not Unicode characters
- **No Unicode normalization** - Header names not normalized for comparison
- **No grapheme cluster awareness** - Length calculations use byte count, not character count
- **UTF-8 BOM only** - Only handles UTF-8 BOM, not other Unicode features

## Type System Integration

### Conversion Operations

```cpp
// Array to Vector conversion
bool FROM(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
          GrapaRuleEvent* value, u8 dim);
```

**Conversion Rules:**
- **Structured validation** for array-to-vector conversion
- **Dimension consistency** checking
- **Type preservation** during conversion
- **Error handling** for invalid structures
- **Header detection** from `$LIST` types in first dimension

### Comparison Operations

```cpp
// Vector comparison in DoComparison function
if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && 
    r2.vVal->mValue.mToken == GrapaTokenType::VECTOR)
```

**Comparison Features:**
- **Element-by-element comparison** with dimension checking
- **Type safety** (vectors only compare to vectors)
- **Lexicographic ordering** for `<`, `>`, `<=`, `>=`
- **Null value handling** for sparse data

## Error Handling

### Validation

```cpp
// Structure validation
if (!result->vVector->FROM(vScriptExec->vScriptState->mItemState.mFloatFix, 
                          vScriptExec->vScriptState->mItemState.mFloatMax, 
                          vScriptExec->vScriptState->mItemState.mFloatExtra, 
                          r1.vVal, dim))
{
    result->CLEAR();
    delete result;
    result = NULL;
}
```

**Error Types:**
- **Structure validation errors** for irregular arrays
- **Dimension mismatch errors** for incompatible operations
- **Memory allocation errors** for large vectors
- **Mathematical errors** for singular matrices

### Error Recovery

**Recovery Strategies:**
- **Graceful degradation** for recoverable errors
- **Memory cleanup** for allocation failures
- **Partial result preservation** for complex operations
- **Error propagation** to calling code

## Debugging Support

### Debug Components

```cpp
// Debug component configuration
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "vector:2");
```

**Debug Levels:**
- **Level 1**: Basic operation logging
- **Level 2**: Memory allocation tracking
- **Level 3**: Detailed computation tracing
- **Level 4**: Performance profiling

### Performance Monitoring

**Monitoring Features:**
- **Memory usage tracking** for large operations
- **Computation time measurement** for optimization
- **Cache performance analysis** for access patterns
- **Parallel efficiency monitoring** for multi-threaded operations

## Integration Points

### C++ Integration

```cpp
// Vector method registration
class GrapaLibraryRuleVectorEvent : public GrapaLibraryEvent
{
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, 
                               GrapaNames* pNameSpace, 
                               GrapaRuleEvent* pOperation, 
                               GrapaRuleQueue* pInput);
};
```

**Integration Features:**
- **Method dispatch** through GrapaLibraryRuleVectorEvent
- **Parameter handling** with type conversion
- **Result formatting** for Grapa script consumption
- **Error propagation** to script level

### Script Integration

```grapa
// Vector creation and operations
vec = [1, 2, 3].vector();
result = vec.dot([4, 5, 6]);
```

**Script Features:**
- **Natural syntax** for vector operations
- **Method chaining** for complex operations
- **Type conversion** with `.vector()` and `.array()`
- **Error handling** with `$ERR` type

## Future Enhancements

### Planned Features

1. **GPU Acceleration**
   - CUDA/OpenCL integration for large-scale operations
   - Automatic GPU memory management
   - Fallback to CPU for smaller operations

2. **Advanced Linear Algebra**
   - Sparse matrix support
   - Iterative solvers for large systems
   - Advanced decomposition algorithms

3. **Machine Learning Integration**
   - Neural network operations
   - Gradient computation
   - Optimization algorithms

4. **Distributed Computing**
   - Multi-node vector operations
   - Distributed memory management
   - Fault tolerance for large datasets

### Performance Improvements

1. **SIMD Optimization**
   - AVX-512 support for modern processors
   - Automatic vectorization for loops
   - Memory alignment optimization

2. **Memory Management**
   - Custom allocators for vector operations
   - Memory pooling for frequent operations
   - Garbage collection integration

3. **Parallel Processing**
   - Thread pool management
   - Work stealing for load balancing
   - NUMA-aware memory allocation

## See Also

- [Vector Type Documentation](../../../docs-src/docs/type/vector.md)
- [Type System Architecture](TYPE_SYSTEM_ARCHITECTURE.md)
- [Memory Management System](GRAPA_MEMORY_MANAGEMENT_SYSTEM.md)
- [C++ Source Analysis Issues](../C++_SOURCE_ANALYSIS_ISSUES.md)
