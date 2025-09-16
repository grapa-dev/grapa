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
- **Array conversion** (`_toarray`): Converts to `$GOBJ` when headers present
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

**Unicode Support:**
- **Excellent Unicode support** - Vector implementation handles Unicode headers correctly
- **UTF-8 BOM detection** - Automatically detects and removes UTF-8 BOM (3-byte `ï»¿` sequence)
- **Unicode header preservation** - Header names with Unicode characters are fully preserved
- **Multi-script support** - Supports Cyrillic, accented characters, and other Unicode scripts
- **Mixed encoding** - Handles CSV files with mixed ASCII and Unicode content
- **Header extraction** - Unicode headers can be extracted using the `keys()` function with `.getname()` method
- **Encoding limitations** - CSV to vector conversion supports UTF-8 encoding only. UTF-16 BOMs are detected and stripped, but content must be UTF-8 encoded for proper processing
- **Graceful degradation** - Non-UTF-8 content is processed as-is, potentially resulting in garbled text rather than failing

### Header Extraction Methods

**User-Level Header Extraction:**
```grapa
// Define the keys function for header extraction
keys = op(lst){lst.reduce(op(acc,x){if(x.type()==$GOBJ){acc += keys(x);}else{acc += 'x'.getname();}},[]);};

// Extract headers from CSV vector
extract_headers = op(csv_string) {
    keys = op(lst){lst.reduce(op(acc,x){if(x.type()==$GOBJ){acc += keys(x);}else{acc += 'x'.getname();}},[]);};
    vec = csv_string.vector();
    arr = vec.list();
    first = arr[0];
    keys(first);
};

// Usage
headers = extract_headers("Name,Value\nAlice,100\nBob,200");
// Result: ["Name","Value"]
```

**Implementation Details:**
- **`.getname()` method** - Returns the key name from a `$GOBJ` element
- **`keys()` function** - Recursively extracts all key names from a `$GOBJ` object
- **`reduce()` method** - Iterates through list elements to collect key names
- **Array conversion** - Uses `vec.list()` to convert vector to array of `$GOBJ` objects
- **First element access** - Accesses `arr[0]` which contains the header row as a `$GOBJ`

**C++ Implementation:**
```cpp
// In GrapaVector::_toarray (lines 2200-2299)
GrapaRuleEvent* label = mLabels.Head();  // Access stored headers
if (pos == 1 && label) {
    result->mValue.mToken = GrapaTokenType::LIST;  // Set to LIST if labels present
}
// ... (loop to populate result)
if (val) {
    if (label && result->mValue.mToken == GrapaTokenType::LIST)
        val->mName.FROM(label->mName);  // Set property name from label
    result->vQueue->PushTail(val);
}
```

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
- **Header detection** from `$GOBJ` types in first dimension

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
- **Type conversion** with `.vector()` and `.list()`
- **Error handling** with `$ERR` type

## Performance Characteristics and Design Trade-offs

### Current Performance Profile

Grapa's vector implementation prioritizes **flexibility and precision** over raw performance, which creates specific performance characteristics:

#### **Performance Strengths**
- **Unlimited Precision**: No overflow in mathematical operations
- **Heterogeneous Data**: Mixed types handled seamlessly
- **Dynamic Type Safety**: Runtime type checking and conversion
- **CSV Integration**: Built-in header processing with Unicode support
- **Educational Value**: Excellent for prototyping and teaching

#### **Performance Limitations**
- **No SIMD Optimization**: Mixed types prevent vectorized CPU instructions
- **Cache Inefficiency**: Heterogeneous data layout reduces cache locality
- **Memory Overhead**: Dynamic allocation and type checking costs
- **Runtime Type Checking**: Type information not available at compile time

### Performance Analysis

#### **Memory Layout Impact**
```cpp
// Current heterogeneous layout
struct GrapaVectorItem {
    u8 isValue : 1;
    u8 isNull : 1;
    u8 mLen:6;
    u8 mToken;
    u8 d[sizeof(GrapaVectorValue*)];  // Variable size data
};
```

**Performance Implications:**
- **Cache Misses**: Non-contiguous data access patterns
- **Memory Bandwidth**: Inefficient memory utilization
- **Allocation Overhead**: Dynamic memory allocation for each element
- **Type Checking**: Runtime type validation on every access

#### **Arithmetic Operation Overhead**
```cpp
// Current arithmetic requires type checking and conversion
GrapaError Add(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
               const GrapaVector& bi, bool pSub) {
    // Type checking for each element
    // Dynamic allocation for results
    // Runtime conversion between types
}
```

**Performance Impact:**
- **Type Checking**: O(n) runtime cost for n elements
- **Memory Allocation**: Dynamic allocation for result vectors
- **Conversion Overhead**: Runtime type conversion between operations

### Strategic Performance Considerations

#### **When Grapa Vectors Excel**
1. **Scientific Computing**: Unlimited precision calculations
2. **Data Processing**: Mixed-type CSV file handling
3. **Educational/Prototyping**: Dynamic type flexibility
4. **Research Applications**: Exact arithmetic without overflow

#### **When Alternatives Are Better**
1. **High-Frequency Computing**: Fixed-type numerical operations
2. **Real-Time Processing**: Signal processing with known data types
3. **Large-Scale Matrix Operations**: >1000x1000 matrices
4. **GPU Computing**: Deep learning and parallel processing

### Future Enhancement Roadmap

#### **Phase 1: Type-Specialized Paths (High Impact)**
- **Homogeneous Vector Detection**: Identify vectors with uniform types
- **Optimized Arithmetic**: Specialized code paths for common types
- **SIMD Integration**: Vectorized operations for supported types

#### **Phase 2: Memory Optimization (Medium Impact)**
- **Contiguous Storage**: Optimize memory layout for homogeneous data
- **Memory Pooling**: Reduce allocation overhead
- **Cache Optimization**: Improve data locality

#### **Phase 3: Lazy Evaluation (Medium Impact)**
- **Expression Trees**: Defer computation until needed
- **View-Based Operations**: Avoid unnecessary data copying
- **Optimization Passes**: Compile-time optimization where possible

#### **Phase 4: Hybrid Vector Types (High Impact)**
- **Type Annotations**: Allow users to specify fixed types
- **Performance Hints**: User-controlled optimization strategies
- **Specialized Classes**: Type-specific vector implementations

### Performance Monitoring and Debugging

#### **Debug Features**
```grapa
// Enable vector performance debugging
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "vector:3");
$sys().putenv("GRAPA_DEBUG_LEVEL", "3");
```

**Debug Levels:**
- **Level 1**: Basic operation logging
- **Level 2**: Memory allocation tracking
- **Level 3**: Performance profiling and timing
- **Level 4**: Detailed cache and memory analysis

#### **Performance Metrics**
- **Memory Usage**: Track allocation patterns and memory efficiency
- **Computation Time**: Measure operation performance
- **Cache Performance**: Monitor cache hit/miss ratios
- **Type Distribution**: Analyze vector type homogeneity

### Implementation Considerations

#### **Backward Compatibility**
- **100% Compatibility**: All existing code must continue to work
- **Progressive Enhancement**: Performance improves automatically
- **Graceful Degradation**: Fallback to current implementation

#### **Educational Value Preservation**
- **Teaching Capabilities**: Maintain prototyping and educational features
- **Debugging Support**: Keep comprehensive debugging capabilities
- **Type Flexibility**: Preserve dynamic type system benefits

#### **Strategic Positioning**
- **Unique Capabilities**: Focus on strengths (precision, flexibility)
- **Complementary Role**: Work alongside specialized numerical libraries
- **User Choice**: Provide clear guidance on when to use alternatives

## See Also

- [Vector Type Documentation](../../../docs-src/docs/type/vector.md)
- [Type System Architecture](TYPE_SYSTEM_ARCHITECTURE.md)
- [Memory Management System](GRAPA_MEMORY_MANAGEMENT_SYSTEM.md)
- [C++ Source Analysis Issues](../C++_SOURCE_ANALYSIS_ISSUES.md)
