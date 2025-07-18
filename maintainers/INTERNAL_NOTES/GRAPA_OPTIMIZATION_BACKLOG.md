# Grapa Optimization Backlog

This document outlines optimization opportunities for Grapa internal functions based on analysis of existing optimizations and function capabilities.

## Current Optimization Analysis

### Existing Optimizations (8 functions)
1. **GrapaLibraryRuleEvent::Optimize** - Base optimization that calls child optimizations
2. **GrapaLibraryRuleNameEvent::Optimize** - Name assignment optimization
3. **GrapaLibraryRuleLitEvent::Optimize** - Literal unwrapping optimization
4. **GrapaLibraryRuleCreateArrayEvent::Optimize** - Constant array folding
5. **GrapaLibraryRuleCreateTupleEvent::Optimize** - Constant tuple folding
6. **GrapaLibraryRuleCreateListEvent::Optimize** - Constant list folding
7. **GrapaLibraryRulePrependEvent::Optimize** - Structure flattening
8. **GrapaLibraryRuleStaticEvent::Optimize** - Static evaluation
9. **GrapaLibraryRuleIncludeEvent::Optimize** - Include file caching

### Optimization Patterns Identified
1. **Constant Folding**: Convert operations with constant parameters to immediate values
2. **Literal Unwrapping**: Remove unnecessary wrapper operations
3. **Structure Flattening**: Combine nested structures when possible
4. **Static Evaluation**: Pre-compute values that don't depend on runtime state
5. **Caching**: Cache expensive operations like file includes

## Critical Order-of-Operations Considerations

### Optimization Execution Order
The optimization system operates in a **bottom-up, recursive manner**:

1. **Base Optimization** (`GrapaLibraryRuleEvent::Optimize`):
   ```cpp
   if (pParam && pParam->vLibraryEvent) 
       pOperation = pParam->vLibraryEvent->Optimize(vScriptExec, pNameSpace, pOperation, pParam->Next());
   ```
   - Calls child optimizations recursively
   - Processes parameters **before** the main operation
   - Ensures child operations are optimized first

2. **Parameter Optimization**: Child parameters are optimized before parent operations
3. **Operation Optimization**: Main operation is optimized after its parameters
4. **Result Propagation**: Optimized results flow back up the call chain

### Order-of-Operations Challenges

#### 1. **Mathematical Expression Optimization**
**Problem**: `add(mul(2, 3), div(10, 2))` must optimize inner operations first
```cpp
// WRONG ORDER: Optimize add before its parameters
add(mul(2, 3), div(10, 2)) → add(6, 5) → 11

// CORRECT ORDER: Optimize parameters first, then add
mul(2, 3) → 6
div(10, 2) → 5  
add(6, 5) → 11
```

#### 2. **Type Conversion Chains**
**Problem**: `int(str(float(5)))` must optimize from innermost to outermost
```cpp
// CORRECT ORDER:
float(5) → 5.0
str(5.0) → "5.0"
int("5.0") → 5
```

#### 3. **String Operation Nesting**
**Problem**: `left(right("hello world", 5), 3)` must optimize right before left
```cpp
// CORRECT ORDER:
right("hello world", 5) → "world"
left("world", 3) → "wor"
```

## High-Priority Optimization Opportunities

### 1. Mathematical Functions (High Impact)
**Functions**: `add`, `sub`, `mul`, `div`, `pow`, `mod`, `abs`, `neg`, `inv`

**Optimization Strategy**: Constant folding for mathematical operations
```cpp
// Example for add optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount == 2) {
    GrapaRuleEvent* v1 = (GrapaRuleEvent*)pParam->vQueue->Head(0);
    GrapaRuleEvent* v2 = (GrapaRuleEvent*)pParam->vQueue->Head(1);
    if (v1->mValue.mToken == GrapaTokenType::INT && v2->mValue.mToken == GrapaTokenType::INT) {
        GrapaInt a1, a2;
        a1.FromBytes(v1->mValue);
        a2.FromBytes(v2->mValue);
        GrapaInt result = a1 + a2;
        pOperation->CLEAR();
        delete pOperation;
        pOperation = new GrapaRuleEvent(0, GrapaCHAR(), result.getBytes());
    }
}
```

**Expected Benefit**: Eliminate runtime computation for constant mathematical expressions

### 2. Type Conversion Functions (Medium Impact)
**Functions**: `int`, `float`, `str`, `bool`, `bits`, `bytes`

**Optimization Strategy**: Constant folding for type conversions
```cpp
// Example for int optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount == 1) {
    GrapaRuleEvent* v = (GrapaRuleEvent*)pParam->vQueue->Head();
    if (v->mValue.mToken == GrapaTokenType::STR || v->mValue.mToken == GrapaTokenType::FLOAT) {
        // Pre-compute conversion at compile time
        GrapaInt result;
        // ... conversion logic ...
        pOperation->CLEAR();
        delete pOperation;
        pOperation = new GrapaRuleEvent(0, GrapaCHAR(), result.getBytes());
    }
}
```

**Expected Benefit**: Reduce runtime type conversion overhead

### 3. String Operations (Medium Impact)
**Functions**: `left`, `right`, `mid`, `trim`, `reverse`, `lower`, `upper`

**Optimization Strategy**: Constant folding for string operations on literal strings
```cpp
// Example for left optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount == 2) {
    GrapaRuleEvent* str = (GrapaRuleEvent*)pParam->vQueue->Head(0);
    GrapaRuleEvent* len = (GrapaRuleEvent*)pParam->vQueue->Head(1);
    if (str->mValue.mToken == GrapaTokenType::STR && len->mValue.mToken == GrapaTokenType::INT) {
        // Pre-compute substring at compile time
        GrapaCHAR result;
        // ... substring logic ...
        pOperation->CLEAR();
        delete pOperation;
        pOperation = new GrapaRuleEvent(0, GrapaCHAR(), result);
    }
}
```

**Expected Benefit**: Eliminate runtime string processing for constant strings

### 4. Bitwise Operations (High Impact)
**Functions**: `bsl`, `bsr`, `bor`, `band`, `xor`, `not`

**Optimization Strategy**: Constant folding for bitwise operations
```cpp
// Example for bsl optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount == 2) {
    GrapaRuleEvent* val = (GrapaRuleEvent*)pParam->vQueue->Head(0);
    GrapaRuleEvent* shift = (GrapaRuleEvent*)pParam->vQueue->Head(1);
    if (val->mValue.mToken == GrapaTokenType::INT && shift->mValue.mToken == GrapaTokenType::INT) {
        GrapaInt v, s;
        v.FromBytes(val->mValue);
        s.FromBytes(shift->mValue);
        GrapaInt result = v << s.LongValue();
        pOperation->CLEAR();
        delete pOperation;
        pOperation = new GrapaRuleEvent(0, GrapaCHAR(), result.getBytes());
    }
}
```

**Expected Benefit**: Significant performance improvement for bit manipulation

### 5. Comparison Operations (Medium Impact)
**Functions**: `eq`, `neq`, `gt`, `lt`, `gteq`, `lteq`, `cmp`

**Optimization Strategy**: Constant folding for comparisons
```cpp
// Example for eq optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount == 2) {
    GrapaRuleEvent* v1 = (GrapaRuleEvent*)pParam->vQueue->Head(0);
    GrapaRuleEvent* v2 = (GrapaRuleEvent*)pParam->vQueue->Head(1);
    if (v1->mValue.mToken == GrapaTokenType::INT && v2->mValue.mToken == GrapaTokenType::INT) {
        GrapaInt a1, a2;
        a1.FromBytes(v1->mValue);
        a2.FromBytes(v2->mValue);
        bool result = (a1 == a2);
        pOperation->CLEAR();
        delete pOperation;
        pOperation = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", result ? "\1" : "");
    }
}
```

**Expected Benefit**: Eliminate runtime comparison overhead

### 6. Array/Vector Operations (High Impact)
**Functions**: `len`, `shape`, `sum`, `mean`, `dot`

**Optimization Strategy**: Constant folding for operations on constant arrays
```cpp
// Example for len optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount == 1) {
    GrapaRuleEvent* v = (GrapaRuleEvent*)pParam->vQueue->Head();
    if (v->mValue.mToken == GrapaTokenType::ARRAY && v->vQueue) {
        GrapaInt result(v->vQueue->mCount);
        pOperation->CLEAR();
        delete pOperation;
        pOperation = new GrapaRuleEvent(0, GrapaCHAR(), result.getBytes());
    }
}
```

**Expected Benefit**: Reduce array processing overhead

### 7. Control Flow Functions (Low-Medium Impact)
**Functions**: `if`, `while`, `switch`, `case`

**Optimization Strategy**: Dead code elimination and constant condition evaluation
```cpp
// Example for if optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount >= 1) {
    GrapaRuleEvent* condition = (GrapaRuleEvent*)pParam->vQueue->Head();
    if (condition->mValue.mToken == GrapaTokenType::BOOL) {
        bool isTrue = (condition->mValue.mBytes && condition->mValue.mBytes[0] && condition->mValue.mBytes[0] != '0');
        // Select appropriate branch at compile time
        GrapaRuleEvent* branch = isTrue ? pParam->vQueue->Head(1) : pParam->vQueue->Head(2);
        if (branch) {
            pOperation->CLEAR();
            delete pOperation;
            pOperation = branch;
        }
    }
}
```

**Expected Benefit**: Eliminate dead code paths

### 8. File System Functions (Low Impact)
**Functions**: `file_pwd`, `file_phd`, `file_info`

**Optimization Strategy**: Caching of file system information
```cpp
// Example for file_pwd optimization
if (pParam && pParam->vQueue && pParam->vQueue->mCount == 1) {
    GrapaRuleEvent* obj = (GrapaRuleEvent*)pParam->vQueue->Head();
    if (obj && obj->vDatabase) {
        // Cache the PWD result if the database hasn't changed
        GrapaCHAR cachedPwd;
        // ... caching logic ...
        pOperation->CLEAR();
        delete pOperation;
        pOperation = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", cachedPwd);
    }
}
```

**Expected Benefit**: Reduce file system calls

## Implementation Priority

### Phase 1 (High Impact, Easy Implementation)
1. Mathematical functions (`add`, `sub`, `mul`, `div`)
2. Bitwise operations (`bsl`, `bsr`, `bor`, `band`)
3. Type conversions (`int`, `float`, `str`)

### Phase 2 (Medium Impact, Moderate Implementation)
1. String operations (`left`, `right`, `trim`)
2. Comparison operations (`eq`, `neq`, `gt`, `lt`)
3. Array operations (`len`, `sum`)

### Phase 3 (Lower Impact, Complex Implementation)
1. Control flow optimization
2. File system caching
3. Advanced mathematical functions

## Implementation Guidelines

### Template for New Optimizations
```cpp
GrapaRuleEvent* GrapaLibraryRule[Function]Event::Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam)
{
    // CRITICAL: Let base optimization handle parameter optimization first
    // This ensures child operations are optimized before this operation
    
    // 1. Check if parameters are constants (after they've been optimized)
    if (pParam && pParam->vQueue && pParam->vQueue->mCount == [expected_count]) {
        // 2. Extract and validate parameters
        GrapaRuleEvent* param1 = (GrapaRuleEvent*)pParam->vQueue->Head(0);
        // ... more parameters ...
        
        // 3. Check if all parameters are constant types
        if (param1->mValue.mToken == GrapaTokenType::[TYPE] && 
            param2->mValue.mToken == GrapaTokenType::[TYPE]) {
            
            // 4. Perform constant folding
            // ... computation logic ...
            
            // 5. Create optimized result
            pOperation->CLEAR();
            delete pOperation;
            pOperation = new GrapaRuleEvent(0, GrapaCHAR(), result.getBytes());
        }
    }
    return(pOperation);
}
```

### Order-of-Operations Rules

1. **Never Skip Base Optimization**: Always let the base `Optimize` method handle parameter optimization first
2. **Check Optimized Parameters**: After base optimization, check if parameters are now constants
3. **Respect Operator Precedence**: Mathematical operations must follow proper precedence
4. **Handle Nested Operations**: String and array operations must optimize from innermost to outermost
5. **Preserve Side Effects**: Don't optimize operations that have side effects (file I/O, network calls, etc.)

### Common Pitfalls

#### 1. **Premature Optimization**
```cpp
// WRONG: Optimizing before parameters are ready
if (pParam->vQueue->mCount == 2) {
    // This might optimize before child operations are processed
}

// RIGHT: Let base optimization handle parameters first
// Then check if parameters are now constants
```

#### 2. **Ignoring Recursion**
```cpp
// WRONG: Not calling base optimization
return pOperation; // Skips parameter optimization

// RIGHT: Let base optimization handle recursion
// Then perform this operation's optimization
```

#### 3. **Type Mismatch After Optimization**
```cpp
// WRONG: Assuming parameter types before optimization
if (param1->mValue.mToken == GrapaTokenType::INT) {
    // Parameter might be optimized to a different type
}

// RIGHT: Check types after optimization
if (param1->mValue.mToken == GrapaTokenType::INT) {
    // Now safe to assume type
}
```

### Testing Strategy
1. **Unit Tests**: Test each optimization with constant and non-constant inputs
2. **Performance Tests**: Measure runtime improvement for optimized vs non-optimized code
3. **Integration Tests**: Ensure optimizations don't break existing functionality
4. **Edge Cases**: Test with boundary values and error conditions

### Testing Order-of-Operations

#### Test Cases for Mathematical Operations
```grapa
// Test nested mathematical operations
add(mul(2, 3), div(10, 2))  // Should optimize to 11
pow(2, add(1, 2))           // Should optimize to 8
mod(pow(3, 2), add(1, 1))   // Should optimize to 1
```

#### Test Cases for String Operations
```grapa
// Test nested string operations
left(right("hello world", 5), 3)  // Should optimize to "wor"
upper(left("test string", 4))     // Should optimize to "TEST"
```

#### Test Cases for Type Conversions
```grapa
// Test type conversion chains
int(str(float(5)))           // Should optimize to 5
str(int(float(3.7)))         // Should optimize to "3"
```

### Validation Requirements
1. **Correctness**: Optimized result must match runtime result exactly
2. **Order Preservation**: Operations must be optimized in correct order
3. **Type Safety**: Optimizations must preserve type correctness
4. **Side Effect Safety**: Operations with side effects must not be optimized
5. **Recursion Safety**: Deeply nested operations must optimize correctly

## Success Metrics

### Performance Targets
- **Mathematical Operations**: 50-80% runtime reduction for constant expressions
- **String Operations**: 30-60% runtime reduction for constant strings
- **Bitwise Operations**: 70-90% runtime reduction for constant operations
- **Array Operations**: 40-70% runtime reduction for constant arrays

### Code Quality Targets
- **Coverage**: 90%+ test coverage for new optimizations
- **Documentation**: Complete documentation of optimization behavior
- **Maintainability**: Clean, readable code following existing patterns

## Future Considerations

### Advanced Optimizations
1. **Loop Unrolling**: Optimize `while` loops with constant bounds
2. **Function Inlining**: Inline small functions at compile time
3. **Expression Simplification**: Simplify complex mathematical expressions
4. **Memory Access Optimization**: Optimize array and vector access patterns

### Compiler Integration
1. **Optimization Levels**: Add optimization level controls
2. **Profile-Guided Optimization**: Use runtime profiles to guide optimizations
3. **Cross-Module Optimization**: Optimize across multiple files/modules

This backlog provides a roadmap for systematically improving Grapa's performance through compile-time optimizations while maintaining code quality and correctness. 