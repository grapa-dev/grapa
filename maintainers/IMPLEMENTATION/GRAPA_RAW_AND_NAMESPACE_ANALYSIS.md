---
tags:
  - maintainer
  - lowlevel
  - implementation
  - analysis
---

# Grapa RAW Data and Namespace Stack Analysis

## Overview

This document provides a comprehensive analysis of two critical aspects of Grapa's architecture:
1. **RAW Data Handling**: How GrapaDB and GrapaDBX handle RAW data types, including compression and storage
2. **Namespace Stack Mechanism**: How Grapa's namespace hierarchy works and how variables trace back to the global namespace

## RAW Data Handling Analysis

### RAW Data in GrapaDB

#### Storage Format
RAW data in GrapaDB is handled as a special case with specific storage characteristics:

```cpp
// From GrapaDB.cpp - RAW data handling
u8 isRaw = (dbFieldValue->mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
```

**Key Characteristics:**
- **Type Preservation**: RAW data stores its original type information separately
- **Compression Support**: RAW data can be compressed using GrapaCompress (GRZ format)
- **Size Handling**: Special handling for different size ranges (1-byte, 2-byte, variable)

#### Storage Strategies

**Fixed Storage (STORE_FIX):**
```cpp
// For small RAW data (≤128 bytes + type byte)
if (field.mDictSize <= (((u64)128) + isRaw))
{
    // Store type byte separately for RAW data
    if (isRaw) {
        SetFieldType(dataPtr, fieldType);
    }
    // Store data with length prefix
    err = SetDataValue(dataRef, field.mDictOffset + 1 + isRaw, len, buffer.GetPtr(), &returnSize);
}
```

**Variable Storage (STORE_VAR/STORE_PAR):**
```cpp
// For large RAW data
if (isRaw) {
    GetFieldType(fieldCursor.mValue, h[0]);
    buffer.mToken = h[0];  // Restore original type
}
```

#### Compression Integration

RAW data uses GrapaCompress for compression:

```cpp
// From GrapaCompress.cpp
u8 tk = pData.ToDbType();
if (tk == GrapaTokenType::RAW) tk = 0;  // Special handling for RAW
pEnc.mBytes[3] = tk;  // Store type in GRZ header
```

**GRZ Format for RAW:**
- **Header**: "GRZ" + type byte (0 for RAW)
- **Length**: Compressed data length
- **Compressed Data**: ZIP-compressed RAW content
- **Type Preservation**: Original type stored in header

### RAW Data in GrapaDBX

#### Current Implementation
GrapaDBX inherits the same RAW handling mechanisms from GrapaDB:

```cpp
// From GrapaDBX.cpp - Context-aware environment
virtual GrapaError RecordGetFieldPartial(GrapaCursor& cursor, const GrapaCHAR& fieldName, 
                                        u64 offset, u64 length, GrapaCHAR& operation, GrapaBYTE& result);
```

**Recommendations for GrapaDBX:**
1. **No Changes Needed**: RAW data handling is already optimal
2. **GRZ Integration**: RAW data already uses compressed GRZ format
3. **Streaming Support**: Current `RecordGetFieldPartial` supports streaming operations
4. **Type Safety**: RAW type preservation is already implemented

### Performance Implications

**Large RAW Fields (MB/GB):**
- **Streaming Operations**: `RecordGetFieldPartial` enables partial access
- **Grep Operations**: `StreamingGrep` processes large fields in chunks
- **Memory Efficiency**: No need to load entire field into memory

**Example Usage:**
```grapa
// Access large RAW field partially
result = _record.getFieldPartial('largeData', 0, 1024, 'substring');
grep_result = _record.getFieldPartial('largeData', 0, 0, 'grep');
```

## Namespace Stack Mechanism

### Namespace Hierarchy Structure

#### GrapaNames Class
The core namespace management is handled by the `GrapaNames` class:

```cpp
class GrapaNames {
private:
    GrapaNames* vParentNames;           // Parent namespace
    GrapaConsoleResponse* vConsoleResponse;
    GrapaRuleQueue mNames;              // Current namespace variables
    
public:
    virtual GrapaNames* GetParrent() { return vParentNames; }
    virtual GrapaNames* GetSkipThis() { /* Skip logic */ }
    virtual GrapaNames* GetSkipParrent() { /* Skip logic */ }
    virtual GrapaRuleQueue* GetNameQueue() { return &mNames; }
};
```

#### Namespace Chain
Namespaces form a hierarchical chain:

```
Global Namespace (Root)
    ↓ (vParentNames)
Function Namespace 1
    ↓ (vParentNames)
Function Namespace 2
    ↓ (vParentNames)
Current Namespace
```

### Variable Lookup Process

#### SearchVariable Algorithm
The variable lookup follows a specific hierarchy:

```cpp
GrapaRuleEvent* GrapaScriptState::SearchVariable(GrapaNames* pNameSpace, GrapaRuleEvent *pValue)
{
    GrapaRuleEvent* parameter = NULL;
    GrapaNames *names = pNameSpace->GetSkipThis();
    
    while (!parameter && names)  // Traverse up the namespace chain
    {
        GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
        while (operation)  // Search within current namespace
        {
            // Search logic for different token types
            switch (pValue->mValue.mToken) {
                case GrapaTokenType::ID:
                case GrapaTokenType::STR:
                    parameter = op->vQueue->Search(pValue->mValue, idx);
                    break;
                case GrapaTokenType::SYSID:
                case GrapaTokenType::SYSSTR:
                    name.FROM("$");
                    name.Append(pValue->mValue);
                    parameter = op->vQueue->Search(name, idx);
                    break;
                // ... other cases
            }
            operation = operation->Prev();
        }
        names = names->GetSkipParrent();  // Move to parent namespace
    }
    return parameter;
}
```

#### Variable Resolution Order
1. **Current Context**: Variables within current `{ }` block
2. **Function Scope**: Variables in current function namespace
3. **Local Namespace**: Variables in current execution context
4. **Call Stack Namespaces**: Variables in calling function namespaces
5. **Global Namespace**: Variables in root namespace

### Namespace Injection Patterns

#### $WIDGET Pattern
The `$WIDGET` system demonstrates namespace injection:

```cpp
// From GrapaWidget.cpp
if (data->mValue.StrLowerCmp("global") == 0)
{
    GrapaNames* parentSpace = vNameSpace;
    while (parentSpace->GetParrent()) 
        parentSpace = parentSpace->GetParrent();
    
    if (mThreadSet)
        mNameSpace.UpdateParrent(parentSpace);
    else
        mNameSpace.SetParrent(parentSpace);
}
```

#### $net Pattern
Network callbacks use namespace injection:

```cpp
// From GrapaLibRule.cpp
GrapaNames* vNames = pNameSpace;
while (vNames->GetParrent()) vNames = vNames->GetParrent();
objEvent->vNetConnect->mNames.SetParrent(vNames);
```

### Context-Aware Record Environment

#### Implementation in GrapaDBX
The context-aware environment leverages namespace injection:

```cpp
GrapaRuleEvent* GrapaDBX::CreateRecordContext(GrapaCursor& cursor, GrapaDBXTable& table)
{
    GrapaRuleEvent* context = new GrapaRuleEvent();
    context->mValue.mToken = GrapaTokenType::LIST;
    context->vQueue = new GrapaRuleQueue();
    
    // Create _record object with table context
    GrapaRuleEvent* recordContext = new GrapaRuleEvent();
    recordContext->mName.FROM("_record");
    recordContext->mValue.mToken = GrapaTokenType::LIST;
    recordContext->vQueue = new GrapaRuleQueue();
    
    // Add table information
    GrapaRuleEvent* tableInfo = new GrapaRuleEvent();
    tableInfo->mName.FROM("table");
    tableInfo->mValue.FROM(tableName);
    recordContext->vQueue->PushTail(tableInfo);
    
    // Add dynamic field access functions
    GrapaRuleEvent* getFieldFunc = new GrapaRuleEvent();
    getFieldFunc->mName.FROM("getField");
    getFieldFunc->mValue.mToken = GrapaTokenType::OP;
    getFieldFunc->mValue.FROM("@<[op,@<record_get_field,{this,@<var,{fieldName}>}>],{fieldName}>");
    recordContext->vQueue->PushTail(getFieldFunc);
    
    context->vQueue->PushTail(recordContext);
    return context;
}
```

## Implications for GrapaDBX Formula Fields

### Formula Execution Context
Formulas execute within a rich context that provides:

1. **Record Access**: `_record.getField(fieldName)`
2. **Partial Access**: `_record.getFieldPartial(fieldName, offset, length, operation)`
3. **Table Context**: `_record.table` information
4. **Position Context**: `_record.position` for record location

### Namespace Integration
The formula execution environment:

1. **Injects Context**: Provides `_record` object in formula namespace
2. **Maintains Hierarchy**: Preserves access to global variables
3. **Supports Callbacks**: Enables dynamic field access through namespace injection
4. **Ensures Isolation**: Formula variables don't pollute global namespace

### Performance Optimization
The namespace system enables:

1. **Selective Loading**: Only load required fields
2. **Lazy Access**: On-demand field loading through callbacks
3. **Streaming Operations**: Process large fields without full memory load
4. **Context Caching**: Reuse context for multiple formula evaluations

## Recommendations

### For RAW Data Handling
1. **No Changes Required**: Current RAW handling is optimal
2. **GRZ Integration**: Already properly implemented
3. **Streaming Support**: Current implementation supports large field operations
4. **Type Safety**: RAW type preservation is correctly implemented

### For Namespace Stack
1. **Leverage Existing Patterns**: Use $WIDGET and $net patterns for context injection
2. **Maintain Hierarchy**: Preserve variable lookup order
3. **Context Isolation**: Ensure formula variables don't leak
4. **Performance Optimization**: Use selective loading and streaming operations

### For GrapaDBX Implementation
1. **Continue Current Approach**: The context-aware record environment is well-designed
2. **Extend Streaming**: Implement full `StreamingGrep` and `LoadFieldSubstring` functionality
3. **Optimize Performance**: Add field name indexing for faster lookups
4. **Maintain Compatibility**: Ensure backward compatibility with GrapaDB

## Conclusion

The investigation reveals that:

1. **RAW Data Handling**: GrapaDB already has excellent RAW data support with GRZ compression and streaming capabilities. No changes needed for GrapaDBX.

2. **Namespace Stack**: Grapa's namespace hierarchy is sophisticated and well-designed, providing automatic variable lookup through parent namespaces while maintaining proper isolation.

3. **Formula Fields**: The current context-aware record environment approach in GrapaDBX is well-aligned with existing Grapa patterns and provides the necessary functionality for efficient formula execution.

The existing architecture provides a solid foundation for GrapaDBX's formula field implementation, with the namespace system enabling rich context provision and the RAW data handling supporting efficient large field operations. 