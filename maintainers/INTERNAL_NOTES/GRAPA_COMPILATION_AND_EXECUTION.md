# Grapa Compilation and Execution Process

## Overview

This document explains the core compilation and execution process in Grapa, focusing on how order of operations is handled and how the left recursion problem is solved. This understanding is critical for the operator documentation audit and optimization work.

## Compilation Process

### 1. **Tokenization (GrapaState.cpp)**

The compilation process starts with tokenization in `GrapaScriptExec::Plan()`:

```cpp
GrapaRuleEvent *GrapaScriptExec::Plan(GrapaNames* pNameSpace, GrapaCHAR& pInput, GrapaRuleEvent* rulexx, u64 pRuleId, GrapaCHAR pProfile)
```

**Process:**
1. **Input Processing**: Raw input is converted to a token stream via `GrapaItemState`
2. **Token Queue**: Tokens are placed in `tokenQueue` for processing
3. **State Machine**: Uses a state machine (START, ESCAPE, BLOCK) to handle different token types
4. **Rule Matching**: Tokens are matched against BNF grammar rules

### 2. **Rule Matching (PlanRule Method)**

The core compilation happens in `GrapaScriptExec::PlanRule()`:

```cpp
GrapaRuleEvent* GrapaScriptExec::PlanRule(GrapaNames* pNameSpace, GrapaRuleEvent* pTokenEvent, GrapaRuleEvent* pRuleEvent, GrapaRuleQueue* pOperationQueue, bool& pMatched, GrapaRuleEvent* pLastNext, bool& pAddLastNext, s8 pLeftRecursion, GrapaKeyValue& pCache, bool pTouched)
```

**Key Features:**
- **Right Recursion**: Default parsing uses right recursion
- **Left Recursion Detection**: Special handling for left recursive rules
- **Operation Building**: Creates execution trees with `$OP` and `$CODE` nodes

### 3. **Left Recursion Solution**

The left recursion problem is solved using a sophisticated approach inspired by Python's solution:

#### **Left Recursion Detection**
```cpp
// Check if first token of rule is a self reference
if (parameter->Queue()->Head() == parameter && parameter->mName.StrCmp(pRuleEvent->mName) == 0)
    isFirstisRule = 1;
```

#### **Two-Phase Parsing**
1. **Phase 1 (pLeftRecursion == 1)**: Parse rules that start with self-reference
2. **Phase 2 (pLeftRecursion == 2)**: Parse remaining rules

#### **Recursive Building**
```cpp
if (pLeftRecursion == 0)
{
    // First attempt: try non-left-recursive rules
    evalEvent = PlanRule(pNameSpace, nextEvent, rulexx, &operationQueue, matched, lastNext, pushExtraToken, 1, pCache, true);
    
    // Then recursively build left-recursive structure
    while (true)
    {
        GrapaRuleEvent* oldEvalEvent = evalEvent;
        evalEvent = PlanRule(pNameSpace, evalEvent, rulexx, &operationQueue, matched, lastNext, pushExtraToken, 2, pCache, true);
        if (evalEvent == oldEvalEvent && !matched)
        {
            evalEvent = oldEvalEvent;
            break;
        }
    }
}
```

## Operator Precedence and Order of Operations

### 1. **BNF Grammar Structure**

The operator precedence is defined in the BNF grammar (`lib/grapa/$grapa.grc`):

```grapa
@global["$compPow"]     // Highest precedence: **, */
@global["$compMul"]     // *, /, %, .*
@global["$compAdd"]     // +, -
@global["$compsetBitShift"] // <<, >>
@global["$compsetTW"]   // <=>
@global["$compsetGtLt"] // <, <=, >, >=
@global["$compsetEqNEq"] // ==, !=
@global["$compsetBitAnd"] // &
@global["$compsetBitXOr"] // ^
@global["$compsetBitOr"] // |
@global["$compsetAnd"]  // &&
@global["$compsetOr"]   // ||
```

### 2. **Precedence Implementation**

Each precedence level is implemented as a separate rule that:
- **Left-associates** operators at the same precedence level
- **Right-recurses** to higher precedence levels
- **Creates operation nodes** with `$OP` tokens

**Example:**
```grapa
@global["$compAdd"]
    = rule <$compAdd> '+' <$compMul> {@<add,{$1,$3}>}
    | <$compAdd> '-' <$compMul> {@<sub,{$1,$3}>}
    | <$compMul>
    ;
```

This creates left-associative parsing: `a + b + c` becomes `((a + b) + c)`

### 3. **Operation Tree Building**

When a rule matches, it creates an operation node:

```cpp
operationEvent = new GrapaRuleEvent(operation->mValue.mToken, operation->mId, (char*)operation->mName.mBytes, (char*)operation->mValue.mBytes);
parameterQueue = new GrapaRuleQueue();
operationEvent->vQueue = parameterQueue;
```

**Node Types:**
- **$OP**: Operator nodes (add, sub, mul, etc.)
- **$CODE**: Code execution nodes
- **$RULE**: Rule reference nodes

## Execution Process

### 1. **ProcessPlan Method**

Execution happens in `GrapaScriptExec::ProcessPlan()`:

```cpp
GrapaRuleEvent* GrapaScriptExec::ProcessPlan(GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam, u64 pCount)
```

### 2. **Execution Flow**

#### **$OP Node Execution**
```cpp
else if (pOperation->mValue.mToken == GrapaTokenType::OP)
{
    // Extract library name and parameters
    GrapaRuleEvent *libName = &gSystem->mLib;
    GrapaRuleEvent *libParam = (GrapaRuleEvent*)pOperation->vQueue->Head();
    
    // Load library and execute
    LoadLib(libName);
    if (libName->vLibraryEvent)
    {
        result = libName->vLibraryEvent->Run(this, pNameSpace, libParam, input);
    }
}
```

#### **$CODE Node Execution**
```cpp
else if (pOperation->mValue.mToken == GrapaTokenType::CODE)
{
    // Execute each child operation
    GrapaRuleEvent *item = (GrapaRuleEvent*)pOperation->vQueue->Head();
    while (item)
    {
        result = ProcessPlan(pNameSpace, item, pParam, pCount);
        item = item->Next();
    }
}
```

### 3. **Bottom-Up Optimization**

The optimization system operates bottom-up:

```cpp
// Base optimization calls child optimizations first
if (pParam && pParam->vLibraryEvent) 
    pOperation = pParam->vLibraryEvent->Optimize(vScriptExec, pNameSpace, pOperation, pParam->Next());
```

**Order:**
1. **Child parameters** are optimized first
2. **Parent operation** is optimized after its parameters
3. **Results flow back up** the call chain

## Key Insights for Operator Documentation

### 1. **Operator Precedence is Grammar-Driven**

The operator precedence is **not hardcoded** but defined in the BNF grammar. This means:
- **Adding new operators** requires grammar changes
- **Changing precedence** requires grammar restructuring
- **Documentation must match** the grammar exactly

### 2. **Left Recursion Enables Natural Expression Parsing**

The left recursion solution enables:
- **Natural mathematical expressions**: `a + b + c`
- **Left-associative operators**: `a - b - c` becomes `(a - b) - c`
- **Complex nested expressions**: `a + b * c + d`

### 3. **Execution Trees are Built During Compilation**

The compilation process:
- **Creates execution trees** with operator nodes
- **Preserves precedence** through tree structure
- **Enables optimization** at compile time
- **Supports runtime execution** via ProcessPlan

### 4. **Type Support is Runtime-Determined**

While precedence is compile-time, type support is determined at runtime:
- **Operator functions** check operand types
- **Type conversion** happens during execution
- **Error handling** occurs at runtime

## Implications for Documentation

### 1. **Operator Precedence Documentation**

Documentation must reflect the BNF grammar exactly:
- **Precedence levels** match grammar rules
- **Associativity** is left-associative for most operators
- **Grouping** follows grammar structure

### 2. **Type Support Documentation**

Type support documentation should:
- **List all supported combinations** for each operator
- **Explain type conversion rules**
- **Document error conditions**
- **Provide examples** for each combination

### 3. **Examples and Testing**

Examples should:
- **Test precedence** with complex expressions
- **Verify associativity** with chained operations
- **Demonstrate type conversions**
- **Show error conditions**

## Technical Notes

### **Recursion Management**
- **Right recursion** is the default parsing strategy
- **Left recursion** is detected and handled specially
- **Recursion depth** is managed to prevent stack overflow
- **Caching** is used to improve performance

### **Memory Management**
- **Execution trees** are built dynamically
- **Memory cleanup** happens after execution
- **Copy operations** are used for parameter passing
- **Reference counting** manages object lifetimes

### **Performance Considerations**
- **Compilation** happens once per expression
- **Execution trees** can be cached
- **Optimization** reduces runtime overhead
- **Type checking** happens at runtime

## Conclusion

Understanding the compilation and execution process is essential for:
1. **Accurate operator documentation**
2. **Correct precedence documentation**
3. **Proper type support documentation**
4. **Effective optimization strategies**
5. **Debugging and troubleshooting**

The left recursion solution is particularly important as it enables natural mathematical expression parsing while maintaining correct precedence and associativity. 