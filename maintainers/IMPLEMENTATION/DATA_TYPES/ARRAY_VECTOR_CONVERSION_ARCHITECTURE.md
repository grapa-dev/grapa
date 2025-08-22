# Array-Vector Conversion Architecture

## Overview

Grapa implements a sophisticated bidirectional conversion system between `$ARRAY` and `$VECTOR` types to provide users with seamless interoperability while leveraging the optimal capabilities of each type.

## Core Principle

**Arrays and vectors are conceptually similar but optimized for different use cases:**
- `$ARRAY`: General-purpose collections, flexible indexing, mixed types
- `$VECTOR`: Mathematical operations, linear algebra, homogeneous numeric data

**The conversion system allows:**
- Arrays to access vector mathematical capabilities
- Vectors to integrate with array-based workflows
- Function application using operators on both types

## Implementation Architecture

### Conversion Patterns

#### Pattern 1: Array → Vector → Array (Mathematical Operations)

**Location**: `GrapaLibraryRuleMulEvent::Run` (lines 13525-13554)

```cpp
// Multiplication operator with array operands
else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && ...)
{
    GrapaVector aa;
    aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, 
            vScriptExec->vScriptState->mItemState.mFloatMax, 
            vScriptExec->vScriptState->mItemState.mFloatExtra, 
            r1.vVal, 0);                    // ARRAY → VECTOR conversion
    
    // ... convert second operand to vector ...
    
    err = aa.Mul(vScriptExec, pNameSpace, bb, false);  // Use vector math
    
    if (err)
        result = Error(vScriptExec, pNameSpace, -1);
    else
        result = aa.ToArray();              // VECTOR → ARRAY conversion
}
```

**Used in:**
- `GrapaLibraryRuleMulEvent::Run` (multiplication)
- `GrapaLibraryRuleAddEvent::Run` (addition) 
- `GrapaLibraryRuleSubEvent::Run` (subtraction)
- `GrapaLibraryRuleDivEvent::Run` (division)
- `GrapaLibraryRulePowEvent::Run` (exponentiation)

#### Pattern 2: Array → Vector → Array (Vector Methods)

**Location**: `GrapaLibraryRuleReShapeEvent::Run` (lines 19076-19084)

```cpp
else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || 
                     r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
{
    GrapaVector v;
    v.FROM(vScriptExec, r1.vVal, 0);        // ARRAY → VECTOR conversion
    if (v.ReShape(r2.vVal)==0)
    {
        result = v.ToArray();                // VECTOR → ARRAY conversion
    }
}
```

**Used in:**
- `.reshape()` method
- `.triu()` method (upper triangular)
- `.tril()` method (lower triangular) 
- `.diagonal()` method
- `.dot()` method (when called on arrays)

#### Pattern 3: Vector → Array (Explicit Conversion)

**Location**: `GrapaLibraryRuleArrayEvent::Run` (line 16346)

```cpp
if (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
{
    if (r1.vVal->vVector)
        result = r1.vVal->vVector->ToArray();   // VECTOR → ARRAY conversion
}
```

## Function Application Architecture

### The `Aop` Field System

**Location**: `GrapaVector.cpp`, `GrapaVectorParam` class

The function application system works through the `Aop` (Array Operation) field in `GrapaVectorParam`:

```cpp
// In GrapaVectorParam constructor (lines 1208-1210)
case GrapaTokenType::OP:
    Aop = a->e;     // Store function reference
    break;

// In multiplication method (lines 1097-1098)
GrapaFloat GrapaVectorParam::Mul(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
                                 GrapaVectorParam* fl, bool pDiv)
{
    if (Aop)
        return Op(pScriptExec, pNameSpace, fl);  // Execute custom function
    // ... normal multiplication logic ...
}

// Function execution (line 1070)
GrapaFloat GrapaVectorParam::Op(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, 
                                GrapaVectorParam* fl)
{
    // ... setup parameters ...
    GrapaRuleEvent* temp = pScriptExec->ProcessPlan(pNameSpace, Aop, params.Head());
    // ... process result ...
}
```

### Function Application Flow

1. **Detection**: When an array contains an `$OP` type, it's detected during vector conversion
2. **Storage**: The function is stored in the `Aop` field of `GrapaVectorParam`
3. **Execution**: During operations, if `Aop` is set, the custom function is called instead of default math
4. **Application**: The function is applied element-wise across the data

## All ProcessPlan Invocations from Vector Operations

### 1. **Function Application via Aop Field** (Primary Method)
**Location**: `GrapaVector.cpp`, `GrapaVectorParam::Op()` (line 1069)

```cpp
GrapaRuleEvent* temp = pScriptExec->ProcessPlan(pNameSpace, Aop, params.Head());
```

**Used in**: All arithmetic operations when `Aop` is set:
- `GrapaVectorParam::Mul()` (line 1097)
- `GrapaVectorParam::Pow()` (line 1111) 
- `GrapaVectorParam::Add()` (line 1125)
- `GrapaVectorParam::Cmp()` (line 1139)

### 2. **Vector Sorting with Custom Comparison** (Secondary Method)
**Location**: `GrapaVector.cpp`, `GrapaVectorSort()` (line 2819)

```cpp
GrapaRuleEvent* temp = a->vScriptExec->ProcessPlan(a->vNameSpace, a->vOp, a->vParams->Head());
```

**Used in**: Vector sorting operations when a custom comparison function is provided:
- `GrapaVector::Sort()` method
- Custom sort functions like `[1,2,3].sort(op(a,b){a-b})`

### 3. **Vector String Parsing** (Tertiary Method)
**Location**: `GrapaVector.cpp`, `GrapaVector::FROM()` (line 847)

```cpp
GrapaRuleEvent* temp = pScriptExec->ProcessPlan(pNameSpace, plan);
```

**Used in**: Vector creation from string representations when the parsed plan is an `$OP` type.

### 4. **Vector Library Rule Operations** (Indirect Method)
**Location**: `GrapaLibRule.cpp`, various vector-related operations

**Used in**: Vector operations that are implemented in the library rules rather than directly in `GrapaVector.cpp`:
- Vector creation from arrays/tuples
- Vector conversion operations
- Vector method calls

## ProcessPlan Integration Points

### Arithmetic Operations
When vectors perform arithmetic with function operands:
```cpp
// Triggers ProcessPlan via Aop field
#[1,2,3]# * [op(x){x*2}]  // Uses GrapaVectorParam::Mul()
#[1,2,3]# + [op(x){x+10}] // Uses GrapaVectorParam::Add()
```

### Sorting Operations  
When vectors are sorted with custom comparison functions:
```cpp
// Triggers ProcessPlan via vOp field
#[3,1,2]#.sort(op(a,b){a-b})  // Uses GrapaVectorSort()
```

### String Parsing
When vectors are created from string representations containing operations:
```cpp
// Triggers ProcessPlan during FROM() parsing
$vector("1,2,3")  // May trigger ProcessPlan if string contains operations
```

### Library Rule Integration
When vector operations are handled by library rules:
```cpp
// May trigger ProcessPlan in library rule implementations
[1,2,3].vector()  // Uses GrapaLibraryRuleVectorEvent::Run()
```

## Conversion Methods

### Array to Vector: `GrapaVector::FROM()`

**Multiple overloads in `GrapaVector.cpp`:**

```cpp
// From GrapaRuleEvent (array)
bool FROM(u8 pFloatFix, u8 pFloatMax, u8 pFloatExtra, 
          GrapaRuleEvent* pArray, u8 pDim);

// From GrapaScriptExec context  
bool FROM(GrapaScriptExec* pScriptExec, GrapaRuleEvent* pArray, u8 pDim);
```

### Vector to Array: `GrapaVector::ToArray()`

**Location**: `GrapaVector.cpp` (line 2153)

```cpp
GrapaRuleEvent* GrapaVector::ToArray()
{
    // Creates new GrapaRuleEvent with GrapaTokenType::ARRAY
    // Converts vector data back to array structure
    // Handles multi-dimensional arrays properly
}
```

## Performance Considerations

### Optimization Strategies

1. **Lazy Conversion**: Conversions only happen when needed
2. **Direct Operations**: Some operations work directly without conversion
3. **Memory Management**: Temporary objects are cleaned up promptly
4. **Type Checking**: Early type detection avoids unnecessary conversions

### Conversion Costs

1. **Memory Allocation**: New objects created during conversion
2. **Data Copying**: Vector/array data must be copied
3. **Type Validation**: Ensuring data compatibility
4. **Cleanup Overhead**: Temporary object destruction

## Error Handling

### Conversion Failures

```cpp
// Invalid conversions return errors
if (conversionFailed)
    result = Error(vScriptExec, pNameSpace, -1);
```

**Common failure cases:**
- Non-numeric data in mathematical operations
- Incompatible dimensions for matrix operations
- Memory allocation failures
- Invalid function types in function application

## Extension Points

### Adding New Conversion Patterns

1. **Identify the operation** requiring conversion
2. **Choose conversion pattern** based on use case
3. **Implement in appropriate `GrapaLibraryRule*Event::Run` method**
4. **Add proper error handling** for edge cases
5. **Update documentation** for users and maintainers

### Function Application Extensions

The `Aop` system can be extended for:
- Custom mathematical operations
- Domain-specific functions
- Performance optimizations
- Specialized data processing

## Testing Considerations

### Test Coverage Areas

1. **Conversion Correctness**: Data integrity across conversions
2. **Function Application**: Various function types and patterns
3. **Error Conditions**: Invalid inputs and edge cases
4. **Performance**: Conversion overhead measurement
5. **Memory Management**: No leaks during conversions

### Key Test Scenarios

```cpp
// Conversion accuracy
[1,2,3] * 2 == [2,4,6]

// Function application
[1,2,3] * [op(x){x*2}] == [2,4,6]

// Bidirectional conversion
array.vector().array() == array

// Error handling
["a","b"] * 2  // Should return $ERR
```

## Future Enhancements

### Potential Improvements

1. **Conversion Caching**: Cache converted objects for reuse
2. **Smart Type Selection**: Auto-select optimal type for operations
3. **Parallel Processing**: Leverage multiple cores for large conversions
4. **Memory Pooling**: Reduce allocation overhead
5. **JIT Optimization**: Compile frequently used conversion patterns

## Related Files

- `source/grapa/GrapaLibRule.cpp`: Main conversion implementations
- `source/grapa/GrapaVector.cpp`: Vector operations and `ToArray()`
- `source/grapa/GrapaState.cpp`: Type system integration
- `docs-src/docs/advanced/array_vector_interoperability.md`: User documentation

## See Also

- [Type System Architecture](../CORE/TYPE_SYSTEM_ARCHITECTURE.md)
- [Mathematical Operations Implementation](../LANGUAGE_FEATURES/MATHEMATICAL_OPERATIONS.md)
- [Function Application System](../LANGUAGE_FEATURES/FUNCTION_APPLICATION_SYSTEM.md)
