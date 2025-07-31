# Index-Based Search Implementation Guide

## Overview
This document provides a comprehensive guide for implementing proper index-based searching in Grapa databases. The current `SearchDb()` method needs to be implemented with proper index utilization according to the planned architecture.

## 🔥 Implementation Requirements

### **Requirements**:
1. **Index Selection**: Match up to the best available index for the table
2. **Index-Based Traversal**: Use index pointers to traverse records when good index found
3. **$KEY Default**: If no field specified, use $KEY index (guaranteed to exist)
4. **Fallback to Table Scan**: Only scan records directly if no suitable index found
5. **Field-Specific Search**: Use appropriate index for specified fields, fallback to table scan if no index available

## Implementation Plan

### **Step 1: Replace SearchDb Implementation**
**File**: `source/grapa/GrapaDB.cpp` - `SearchDb()` method

**Target Implementation** (proper index-based):
```cpp
GrapaError GrapaDB::SearchDb(GrapaCursor& pCursor, GrapaDBTable& pTable, GrapaDBFieldValueArray& pFieldList)
{
    // 1. Find best available index for search fields
    // 2. If no specific index, check if $KEY index is appropriate
    // 3. Use index-based search if good index found
    // 4. Fallback to table scan only if no suitable index available
    // 5. Implement two-stage search process
}
```

### **Step 2: Implement Index Selection Logic**
**Key Methods to Use**:
- `LocateIndex(cursor, indexRef, fieldId)` - Find index for specific field
- `IndexHasField(cursor, fieldId)` - Check if index contains field
- `GetTreeIndex(cursor, indexRef)` - Get index tree reference

**Logic Flow**:
1. Check if search fields have available indexes using `LocateIndex()`
2. If no specific index found, check if `$KEY` index is appropriate
3. Only fallback to table scan if no suitable index available
4. Use `GrapaBtree::SEARCH_MODE` for proper comparison

### **Step 3: Implement Two-Stage Search Process**
**Stage 1**: Use index to narrow down records (if good index found)
- Use index pointers to traverse records
- Use `PtrToRec()` to dereference pointers to actual records
- Use `CompareKey()` with `SEARCH_MODE` for field comparisons

**Stage 2**: Scan remaining records for additional field criteria
- Implement proper cursor navigation using `FirstDb`, `NextDb`, `PrevDb`
- Apply additional field filters that weren't covered by the index

## Index Structure Patterns

### **Correct Index Structure**:
```
RPTR (0) key=1 node=(84,0) weight=2: RREC (55) key=1 node=(58,0) weight=2: data
```

### **Index Components**:
- **RPTR/CPTR**: Record/Column pointers in index tree
- **RREC/CREC**: Record/Column entries with actual data
- **key**: The search key value
- **node**: BTree node coordinates (block, offset)
- **weight**: Number of records in subtree

### **$KEY Index Usage**:
- **ROW/COL Tables**: Field 4 typically used as $KEY
- **GROUP Tables**: Field 1 typically used as $KEY
- **Guaranteed Existence**: Every table has a $KEY index

## Debug and Validation

### **Debug Output Format**:
Use the debug functions to verify correct index structure:
```cpp
// Debug output should show proper index structure
// RPTR → RREC → actual data
```

### **Testing Strategy**:
1. **Test with 2 records**: Verify index works correctly
2. **Test with 3+ records**: Verify no corruption occurs
3. **Test index-based searches**: Verify proper index utilization
4. **Test fallback scenarios**: Verify table scan when no index available

## Performance Considerations

### **Index Selection Strategy**:
- **Primary**: Use field-specific indexes when available
- **Secondary**: Fallback to $KEY index for general searches
- **Last Resort**: Use table scan only when no suitable index exists

### **Search Optimization**:
- **Index-based first**: Use indexes to narrow down candidate records
- **Field filtering**: Apply additional field criteria to index results
- **Cursor efficiency**: Minimize record scanning through proper index usage

## Implementation Notes

### **Critical Considerations**:
- **Avoid corruption-prone patterns**: Don't copy problematic index update logic
- **Use reference patterns**: Reference GrapaDB for correct structure patterns
- **Focus on read operations**: Prioritize search functionality over complex updates
- **Test extensively**: Verify no corruption occurs in any scenario

### **Key Methods to Implement**:
- `SearchDb()`: Main search method with index selection
- `LocateIndex()`: Find appropriate index for search fields
- `IndexHasField()`: Check if index contains specific field
- `CompareKey()`: Compare search keys with proper mode

This guide provides the foundation for implementing robust, index-based searching while avoiding the corruption issues that plagued the original implementation. 