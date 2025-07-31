---
tags:
  - maintainer
  - implementation
  - grapadbx
  - formula
  - callback
  - database
---

# GrapaDBX Formula Callback Implementation

## Overview

This document proposes the implementation changes needed for GrapaDBX formula fields based on the widget callback pattern. The key insight is that formula callbacks should receive a **database context object reference** as the first parameter, enabling direct access to record fields and database operations via dot notation.

## Current State Analysis

### Current Implementation Issues
The current GrapaDBX formula implementation has these limitations:
1. **No Object Reference**: Formulas don't receive a database context object
2. **Limited Context**: No direct access to record fields or database operations
3. **Missing Function Registration**: `record_get_field` and `record_get_field_partial` functions are not registered
4. **Incomplete Callback Pattern**: Doesn't follow the proven widget callback pattern

### Widget Pattern Reference
The widget system demonstrates the correct pattern:
```grapa
callback: op(o, cbdata, item) {
    // o = widget object reference
    o.get("property");           // Direct property access
    o.set({"property": "value"}); // Direct property setting
    o.parent();                  // Object hierarchy navigation
    o.child("name");             // Child object access
}
```

## Proposed Implementation

### 1. Database Context Object

Create a `GrapaDBXContext` class similar to `GrapaWidget`:

```cpp
// In GrapaDBX.h
class GrapaDBXContext
{
public:
    GrapaDBX* vDBX;                    // Database instance
    GrapaDBXTable* vTable;             // Current table
    GrapaCursor* vCursor;              // Current record cursor
    GrapaRuleEvent* vEvent;            // Associated rule event
    
    GrapaDBXContext(GrapaDBX* dbx, GrapaDBXTable* table, GrapaCursor* cursor);
    ~GrapaDBXContext();
    
    // Field access methods
    GrapaError GetField(const GrapaCHAR& fieldName, GrapaBYTE& result);
    GrapaError GetFieldPartial(const GrapaCHAR& fieldName, u64 offset, u64 length, 
                               const GrapaCHAR& operation, GrapaBYTE& result);
    
    // Context information
    GrapaCHAR GetTableName();
    u64 GetRecordPosition();
    GrapaDBXTable* GetTable();
    GrapaDBX* GetDBX();
};
```

### 2. Function Registration in GrapaLibRule.cpp

Add database function handlers:

```cpp
// In GrapaLibRule.cpp - Add to handlerMap
{ "record_get_field", &GrapaLibraryRuleEvent::HandleRecordGetField },
{ "record_get_field_partial", &GrapaLibraryRuleEvent::HandleRecordGetFieldPartial },

// Add handler classes
class GrapaLibraryRuleRecordGetFieldEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleRecordGetFieldEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, 
                               GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};

class GrapaLibraryRuleRecordGetFieldPartialEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleRecordGetFieldPartialEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, 
                               GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};

// Implementation
GrapaRuleEvent* GrapaLibraryRuleRecordGetFieldEvent::Run(GrapaScriptExec* vScriptExec, 
                                                         GrapaNames* pNameSpace, 
                                                         GrapaRuleEvent* pOperation, 
                                                         GrapaRuleQueue* pInput)
{
    // Extract parameters: dbxContext, fieldName
    GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL); // dbxContext
    GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL); // fieldName
    
    // Get database context object
    GrapaRuleEvent* contextEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
    if (!contextEvent || !contextEvent->vDBXContext) {
        return Error(vScriptExec, pNameSpace, -1);
    }
    
    // Call database method
    GrapaBYTE result;
    GrapaError err = contextEvent->vDBXContext->GetField(r2.vVal->mValue, result);
    
    if (err) {
        return Error(vScriptExec, pNameSpace, err);
    }
    
    // Return result
    GrapaRuleEvent* resultEvent = new GrapaRuleEvent();
    resultEvent->mValue.FROM(result);
    return resultEvent;
}
```

### 3. Formula Execution with Context

Update `GrapaDBX::ExecuteFormula` to use the widget pattern:

```cpp
// In GrapaDBX.cpp
GrapaError GrapaDBX::ExecuteFormula(GrapaCursor& cursor, GrapaDBXTable& table, 
                                   const GrapaCHAR& formulaText, GrapaBYTE& result)
{
    // Create database context object
    GrapaDBXContext* dbxContext = new GrapaDBXContext(this, &table, &cursor);
    
    // Create context event
    GrapaRuleEvent* contextEvent = new GrapaRuleEvent();
    contextEvent->mValue.mToken = GrapaTokenType::DBXCONTEXT;
    contextEvent->vDBXContext = dbxContext;
    contextEvent->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$DBX"));
    
    // Create worker for formula execution
    GrapaWorker worker;
    worker.Set(vScriptExec, pNameSpace);
    
    // Create formula execution parameters
    GrapaRuleQueue params;
    params.PushTail(contextEvent);  // First parameter: database context object
    
    // Execute formula with database context
    worker.Run(formulaOp, &params, doneOp);
    
    // Get result
    if (worker.vResult) {
        result.FROM(worker.vResult->mValue);
    }
    
    return 0;
}
```

### 4. Formula Syntax with Object Reference

Formulas would now be written with direct object access:

```grapa
// Formula examples with database context object
formula1 = "dbx.getField('price') * dbx.getField('quantity')";
formula2 = "dbx.getField('basePrice') * (1 + dbx.getField('taxRate'))";
formula3 = "dbx.getFieldPartial('largeData', 0, 1000, 'grep').len() > 0 ? 'found' : 'not found'";

// Create formula field
db.createFormulaField("total_price", "dbx.getField('price') * dbx.getField('quantity') + dbx.getField('tax')", FLOAT_TYPE);
```

### 5. Database Context Class Definition

Add the database context class to the Grapa type system:

```grapa
// In lib/grapa/$DBX.grc
@global["$DBX"]
= class {
    type = @<"type",{@<this>}>;
    
    // Field access methods
    getField = @<[op,@<"record_get_field",{@<this>,@<var,{fieldName}>}>],{fieldName}>;
    getFieldPartial = @<[op,@<"record_get_field_partial",{@<this>,@<var,{fieldName}>,@<var,{offset}>,@<var,{length}>,@<var,{operation}>}>],{fieldName,offset,length,operation}>;
    
    // Context information
    getTableName = @<"record_get_table_name",{@<this>}>;
    getRecordPosition = @<"record_get_position",{@<this>}>;
    getTable = @<"record_get_table",{@<this>}>;
    getDBX = @<"record_get_dbx",{@<this>}>;
};
```

## Implementation Steps

### Phase 1: Core Infrastructure
1. **Create GrapaDBXContext class** in `GrapaDBX.h` and `GrapaDBX.cpp`
2. **Add DBXCONTEXT token type** to `GrapaTokenType` enum
3. **Register database functions** in `GrapaLibRule.cpp`
4. **Create $DBX.grc** library file

### Phase 2: Formula Execution
1. **Update ExecuteFormula method** to use worker thread pattern
2. **Implement context object injection** into formula parameters
3. **Add database context object** as first parameter to formulas
4. **Test basic field access** functionality

### Phase 3: Advanced Features
1. **Implement partial field access** for large fields
2. **Add streaming operations** (grep, substring, etc.)
3. **Optimize performance** with caching and lazy loading
4. **Add error handling** and validation

### Phase 4: Integration and Testing
1. **Update test scripts** to use new object reference pattern
2. **Document formula syntax** with examples
3. **Performance testing** with large datasets
4. **Integration testing** with existing database operations

## Benefits of This Approach

### 1. Consistency with Existing Patterns
- Follows the proven widget callback pattern
- Uses the same object reference injection mechanism
- Maintains consistency with Grapa's design philosophy

### 2. Rich Context Access
- Direct access to record fields via dot notation
- Access to table and database context
- Support for complex database operations

### 3. Extensibility
- Easy to add new database operations
- Support for custom field access patterns
- Framework for future database features

### 4. Performance
- Efficient object reference passing
- Minimal overhead for field access
- Support for lazy loading and caching

## Migration Strategy

### Backward Compatibility
- Existing formula text storage remains unchanged
- Formulas can be gradually migrated to new syntax
- Support for both old and new execution patterns during transition

### Testing Approach
1. **Unit Tests**: Test individual database functions
2. **Integration Tests**: Test formula execution with real data
3. **Performance Tests**: Test with large datasets and complex formulas
4. **Compatibility Tests**: Ensure existing functionality still works

## Conclusion

The proposed implementation follows the proven widget callback pattern to provide rich, context-aware formula execution in GrapaDBX. By passing a database context object reference as the first parameter, formulas gain direct access to record fields and database operations via dot notation, making them more powerful and easier to use.

This approach maintains consistency with Grapa's existing patterns while providing the functionality needed for advanced database operations and computed fields. 