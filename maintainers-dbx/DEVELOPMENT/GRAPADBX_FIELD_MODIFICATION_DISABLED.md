# GrapaDBX Field Modification - DISABLED

**Status**: ❌ **DISABLED** - Feature has been disabled for safety reasons

**Date**: January 2025

## Overview

Field modification (changing field definitions) has been disabled in GrapaDBX due to incomplete implementation that could lead to data corruption. The feature exists in the codebase but lacks critical data migration logic.

## Why It's Disabled

### **Critical Missing Components**

1. **ROW Table Data Migration**: 
   - When field size changes, all existing records need data shifting
   - Requires reading every record and adjusting field offsets
   - Missing: `CopyDataValue()` logic for record data migration

2. **COL Table Data Migration**:
   - Column trees need to be recreated with new structure
   - Variable fields need new storage trees
   - Fixed fields need new FREC_DATA blocks
   - Missing: Complete column tree migration logic

3. **Index Updates**:
   - No logic to update indexes when field definitions change
   - Missing: Index entry removal and re-addition

4. **Data Type Conversions**:
   - No conversion logic between different types (e.g., STR to INT)
   - No handling of storage type changes (FIX to VAR)
   - Missing: Data validation and compatibility checks

### **Current Implementation Status**

The `ModifyTableField()` function exists but contains only:
- ✅ Dictionary field updates (metadata changes)
- ✅ Field definition updates in the dictionary
- ❌ **ROW table data migration** (prints "not yet implemented")
- ❌ **COL table data migration** (prints "not yet implemented")
- ❌ **Index updates**
- ❌ **Data type conversions**

## Reference Implementation Analysis

The reference `GrapaDB::DeleteTableField()` shows the complexity required:

### **ROW Tables**
```cpp
// Offset calculation and data shifting
fromOffset = field.mDictOffset + field.mDictSize;
err = CopyDataValue(tableCursor.mValue, toOffset, tableCursor.mValue, fromOffset, moveSize);

// Field offset updates for all subsequent fields
field.mDictOffset -= (fromOffset - toOffset);
```

### **COL Tables**
```cpp
// Different handling for STORE_FIX vs STORE_VAR
switch (fieldStore) {
    case GrapaDBField::STORE_VAR:
    case GrapaDBField::STORE_PAR:
        tableCursor.Set(storeTree, TREE_ITEM, pFieldId);
        err = Delete(tableCursor);
        break;
    case GrapaDBField::STORE_FIX:
        tableCursor.Set(storeTree, SDATA_ITEM, pFieldId);
        err = Delete(tableCursor);
        break;
}
```

## Implementation Plan

### **Phase 1: Foundation (HIGH PRIORITY)**
1. **ROW Table Migration**
   - Implement `CopyDataValue()` for record data shifting
   - Add field offset recalculation logic
   - Handle all field types (FIX, VAR, PAR)

2. **COL Table Migration**
   - Implement column tree recreation logic
   - Handle storage type changes (FIX ↔ VAR)
   - Add FREC_DATA block management

3. **Data Type Conversions**
   - Implement conversion between Grapa types
   - Add validation for incompatible changes
   - Handle RAW type conversions

### **Phase 2: Index Management**
1. **Index Update Logic**
   - Remove records from affected indexes
   - Re-add records with new field structure
   - Handle index field type changes

2. **Index Validation**
   - Verify index integrity after changes
   - Handle index rebuild scenarios

### **Phase 3: Advanced Features**
1. **Constraint Handling**
   - Handle UNIQUE constraint changes
   - Manage foreign key relationships
   - Update check constraints

2. **Performance Optimization**
   - Batch processing for large tables
   - Progress reporting for long operations
   - Rollback capabilities

## Files to Modify

### **Core Implementation**
- `source/grapa/GrapaDBX.cpp`: `ModifyTableField()` function
- `source/grapa/GrapaDBX.h`: Add helper functions

### **Helper Functions Needed**
```cpp
// ROW table migration
GrapaError MigrateRowTableData(GrapaDBXTable& pTable, u64 pFieldId, 
                               u64 oldDictSize, u64 newDictSize);

// COL table migration  
GrapaError MigrateColTableData(GrapaDBXTable& pTable, u64 pFieldId,
                               u8 oldStore, u8 newStore, u8 oldType, u8 newType);

// Data type conversion
GrapaError ConvertFieldData(GrapaDBXField& oldField, GrapaDBXField& newField,
                           GrapaBYTE& data);

// Index updates
GrapaError UpdateIndexesForFieldChange(GrapaDBXTable& pTable, u64 pFieldId);
```

## Testing Requirements

### **Unit Tests**
1. **ROW Table Tests**
   - Field size increase/decrease
   - Field type changes (STR ↔ INT)
   - Storage type changes (FIX ↔ VAR)

2. **COL Table Tests**
   - Column tree recreation
   - Storage type migrations
   - Data type conversions

3. **Index Tests**
   - Index integrity verification
   - Multi-field index updates
   - Index performance validation

### **Integration Tests**
1. **Large Table Migration**
   - Tables with 1000+ records
   - Multiple field changes
   - Performance benchmarks

2. **Error Handling**
   - Invalid field changes
   - Data corruption scenarios
   - Rollback testing

## Safety Considerations

### **Before Enabling**
1. **Complete Implementation**: All phases must be implemented
2. **Comprehensive Testing**: All test scenarios must pass
3. **Data Validation**: Verify data integrity after changes
4. **Backup Strategy**: Ensure rollback capabilities
5. **Documentation**: Complete user documentation

### **Risk Mitigation**
1. **Transaction Support**: Wrap changes in transactions
2. **Validation Checks**: Verify data before and after changes
3. **Progress Reporting**: Show migration progress
4. **Error Recovery**: Handle partial failures gracefully

## Current Status

- **Feature**: ❌ Disabled
- **Implementation**: 20% complete (dictionary updates only)
- **Testing**: 0% complete
- **Documentation**: This file

## Next Steps

1. **Implement Phase 1** (ROW/COL migration + data conversions)
2. **Add comprehensive testing**
3. **Implement Phase 2** (index management)
4. **Add safety features** (transactions, validation)
5. **Enable feature** with proper documentation

---

**Note**: This feature should not be enabled until all phases are complete and thoroughly tested. Data corruption risks are high with incomplete implementation. 