# GrapaDBX Index Support Design Plan

## Overview

This document outlines the comprehensive plan for implementing full index support in GrapaDBX to achieve 100% functional parity with the reference GrapaDB implementation, plus future enhancements for advanced index capabilities.

## Current Status Analysis

### ✅ What's Working in GrapaDBX
- **Basic Field Operations:** Field creation, storage, and retrieval
- **Record Management:** Record creation, storage, and deletion
- **Data Type Support:** All Grapa data types working correctly
- **Table Types:** ROW, COL, and GROUP table types functional
- **DICT Management:** Field dictionary working correctly

### ❌ What's Missing from GrapaDBX
1. **Index Update Logic:** Complete index update pattern in `SetRecordField`
2. **Index Selection:** `SearchDb` method with index selection
3. **Two-Stage Search:** Index bounding + full scan process
4. **Cursor Navigation:** `FirstDb/NextDb/LastDb` methods
5. **Index Integration:** Proper integration with field update logic

## Reference Implementation Analysis

### Index Update Pattern (SetRecordField)
The reference implements a **three-phase index update pattern**:

```cpp
// Phase 1: Remove from all indexes
indexCursor.Set(indexTree);
err = First(indexCursor);
while (!err) {
    for(i=0; i<fieldCount; i++) {
        dbFieldValue = pFieldList.GetFieldAt(i);
        if (IndexHasField(indexCursor, dbFieldValue->mId)) {
            switch (recCursor.mTreeType) {
                case GROUP_TREE: 
                    tableCursor.Set(indexCursor.mValue, GPTR_ITEM, recCursor.mKey);
                    err = Delete(tableCursor);
                    break;
                case RTABLE_TREE:
                    tableCursor.Set(indexCursor.mValue, RPTR_ITEM, recCursor.mKey);
                    err = Delete(tableCursor);
                    break;
                case CTABLE_TREE:
                    tableCursor.Set(indexCursor.mValue, CPTR_ITEM, recCursor.mKey);
                    err = Delete(tableCursor);
                    break;
            }
            break;
        }
    }
    err = Next(indexCursor);
}

// Phase 2: Update field data
// ... field data is updated in the record ...

// Phase 3: Add back to all indexes
indexCursor.Set(indexTree);
err = First(indexCursor);
while (!err) {
    for (i = 0; i < fieldCount; i++) {
        dbFieldValue = pFieldList.GetFieldAt(i);
        if (IndexHasField(indexCursor, dbFieldValue->mId)) {
            switch (recCursor.mTreeType) {
                case GROUP_TREE:
                    tableCursor.Set(indexCursor.mValue, GPTR_ITEM, recCursor.mKey);
                    err = Insert(tableCursor);
                    break;
                case RTABLE_TREE:
                    tableCursor.Set(indexCursor.mValue, RPTR_ITEM, recCursor.mKey);
                    err = Insert(tableCursor);
                    break;
                case CTABLE_TREE:
                    tableCursor.Set(indexCursor.mValue, CPTR_ITEM, recCursor.mKey);
                    err = Insert(tableCursor);
                    break;
            }
            break;
        }
    }
    err = Next(indexCursor);
}
```

### Search Optimization (SearchDb)
The reference uses a **simple but effective** two-stage search process:

```cpp
GrapaError GrapaDB::SearchDb(GrapaDBCursor& cursor, GrapaDBTable& pTable, GrapaDBFieldValueArray& pFieldList)
{
    // Simple index selection: use first field's index
    field = pFieldList.GetFieldAt(0);
    indexCursor.Set(pTable.mRecRef);
    err = GetTreeIndex(indexCursor, indexRef);
    if (!err) {
        err = LocateIndex(indexCursor, indexRef, field->mId);
        if (!err) {
            usingIndex = true;
            cursor.SetSearch(this, indexCursor.mValue, usingIndex, &pFieldList);
        }
    }
    
    // Two-stage search process
    if (usingIndex) {
        // Stage 1: Use index to narrow down results
        err = Search(cursor);
        err = FirstDb(cursor);
    } else {
        // Stage 2: Full table scan
        cursor.SetSearch(this, pTable.mRecRef, usingIndex, &pFieldList);
        err = First(cursor);
        err = FirstDb(cursor);
        // ... compare and iterate
    }
}
```

### Cursor Navigation
The reference provides navigation methods for search results:

```cpp
GrapaError GrapaDB::FirstDb(GrapaDBCursor& cursor)
GrapaError GrapaDB::NextDb(GrapaDBCursor& cursor)  
GrapaError GrapaDB::LastDb(GrapaDBCursor& cursor)
GrapaError GrapaDB::PrevDb(GrapaDBCursor& cursor)
```

## Implementation Plan

### Phase 1: Basic Index Support (Parity with Reference)

#### 1.1 Index Update Logic in SetRecordField
**File:** `source/grapa/GrapaDBX.cpp`
**Method:** `GrapaDBX::SetRecordField`

**Implementation Steps:**
1. **Add Index Traversal Logic:** Traverse all indexes for the table
2. **Add Index Deletion Logic:** Delete old index entries before field updates
3. **Add Index Insertion Logic:** Insert new index entries after field updates
4. **Support All Table Types:** GROUP_TREE, RTABLE_TREE, CTABLE_TREE
5. **Support All Pointer Types:** GPTR_ITEM, RPTR_ITEM, CPTR_ITEM

**Key Methods to Implement:**
```cpp
// Helper methods needed
GrapaError GrapaDBX::LocateIndex(GrapaCursor& cursor, u64 indexRef, u64 fieldId);
bool GrapaDBX::IndexHasField(GrapaCursor& cursor, u64 fieldId);
```

#### 1.2 Search Optimization (SearchDb)
**File:** `source/grapa/GrapaDBX.cpp`
**Method:** `GrapaDBX::SearchDb`

**Implementation Steps:**
1. **Index Selection:** Use first field's index (simple approach like reference)
2. **Two-Stage Search:** Index bounding + full scan for remaining fields
3. **Cursor Setup:** Proper cursor configuration for index-based search
4. **Integration:** Connect with existing field comparison logic

**Key Methods to Implement:**
```cpp
GrapaError GrapaDBX::SearchDb(GrapaCursor& cursor, GrapaDBXTable& pTable, GrapaDBXFieldValueArray& pFieldList);
void GrapaDBXCursor::SetSearch(GrapaDBX* pDb, u64 pTreeRef, bool pUsingIndex, GrapaDBXFieldValueArray* pData);
```

#### 1.3 Cursor Navigation Methods
**File:** `source/grapa/GrapaDBX.cpp`
**Methods:** `FirstDb`, `NextDb`, `LastDb`, `PrevDb`

**Implementation Steps:**
1. **FirstDb:** Find first matching record in search results
2. **NextDb:** Navigate to next matching record
3. **LastDb:** Find last matching record
4. **PrevDb:** Navigate to previous matching record
5. **Comparison Logic:** Use existing `CompareKey` methods

#### 1.4 Index Management Methods
**File:** `source/grapa/GrapaDBX.cpp`
**Methods:** `CreateIndex`, `OpenIndex`, `DeleteIndex`, `RefreshIndex`

**Implementation Steps:**
1. **Index Creation:** Create index BTree with proper field mapping
2. **Index Opening:** Open existing indexes and load field information
3. **Index Deletion:** Remove indexes and clean up references
4. **Index Refresh:** Rebuild index after bulk data changes

### Phase 2: Testing and Validation

#### 2.1 Index Update Testing
**Test Files:** `test/grapadbx/index_update_test.grc`

**Test Scenarios:**
1. **Single Field Update:** Update one field and verify index consistency
2. **Multiple Field Update:** Update multiple fields simultaneously
3. **All Table Types:** Test ROW, COL, and GROUP table types
4. **Index Corruption Recovery:** Test recovery from index corruption
5. **Concurrent Updates:** Test multiple concurrent field updates

#### 2.2 Search Optimization Testing
**Test Files:** `test/grapadbx/search_optimization_test.grc`

**Test Scenarios:**
1. **Index-Based Search:** Search using indexed fields
2. **Full Table Scan:** Search without index (fallback)
3. **Mixed Search:** Search with some indexed and some non-indexed fields
4. **Result Navigation:** Test `FirstDb/NextDb/LastDb/PrevDb`
5. **Performance Comparison:** Compare indexed vs non-indexed search performance

#### 2.3 Integration Testing
**Test Files:** `test/grapadbx/integration_test.grc`

**Test Scenarios:**
1. **End-to-End Workflows:** Complete database operations with indexes
2. **Stress Testing:** Large datasets with multiple indexes
3. **Error Handling:** Invalid index operations and recovery
4. **Reference Compatibility:** Compare with reference implementation results

### Phase 3: Advanced Index Features (Future)

#### 3.1 Multi-Index Support
**Enhancement:** Use multiple indexes for complex queries

**Implementation:**
```cpp
struct IndexSelection {
    u64 indexRef;
    u32 fieldCoverage;      // Number of search fields covered
    u32 selectivity;         // Index selectivity ratio
    u32 cost;               // Estimated query cost
};

GrapaError SelectBestIndexes(GrapaDBXTable& table, GrapaDBXFieldValueArray& searchFields, 
                            std::vector<IndexSelection>& selectedIndexes);
```

#### 3.2 Index Statistics
**Enhancement:** Track index performance and selectivity

**Implementation:**
```cpp
struct IndexStats {
    u64 cardinality;        // Number of unique values
    u64 selectivity;        // Selectivity ratio
    u64 lastUpdateTime;     // When index was last updated
    u64 queryCount;         // How often this index is used
    u64 avgQueryTime;       // Average query time
};

GrapaError UpdateIndexStats(GrapaDBXIndex& index, u64 queryTime);
GrapaError GetIndexStats(GrapaDBXIndex& index, IndexStats& stats);
```

#### 3.3 Compound Index Optimization
**Enhancement:** Optimize multi-field index usage

**Implementation:**
```cpp
GrapaError UseCompoundIndex(GrapaCursor& cursor, GrapaDBXIndex& index, 
                           GrapaDBXFieldValueArray& searchFields);
GrapaError PartialIndexMatch(GrapaCursor& cursor, GrapaDBXIndex& index, 
                            GrapaDBXFieldValueArray& searchFields, u32 fieldCount);
```

#### 3.4 Index Cost Analysis
**Enhancement:** Intelligent index selection based on statistics

**Implementation:**
```cpp
struct IndexCost {
    u64 indexRef;
    u64 estimatedRows;      // Estimated rows to scan
    u64 indexSize;          // Index size in bytes
    u64 selectivity;        // Index selectivity
    u64 cost;              // Total estimated cost
};

GrapaError AnalyzeIndexCosts(GrapaDBXTable& table, GrapaDBXFieldValueArray& searchFields,
                            std::vector<IndexCost>& costs);
GrapaError SelectOptimalIndex(std::vector<IndexCost>& costs, u64& selectedIndex);
```

## Implementation Priority

### High Priority (Phase 1)
1. **Index Update Logic:** Critical for data consistency
2. **SearchDb Method:** Essential for query performance
3. **Cursor Navigation:** Required for result iteration
4. **Basic Testing:** Ensure functionality works correctly

### Medium Priority (Phase 2)
1. **Comprehensive Testing:** Full validation suite
2. **Performance Testing:** Large dataset testing
3. **Error Handling:** Robust error recovery
4. **Documentation:** Complete implementation docs

### Low Priority (Phase 3)
1. **Multi-Index Support:** Advanced query optimization
2. **Index Statistics:** Performance monitoring
3. **Compound Index Optimization:** Complex query support
4. **Index Cost Analysis:** Intelligent index selection

## Success Criteria

### Phase 1 Success Criteria
- ✅ **Index Updates:** All field updates properly update all affected indexes
- ✅ **Search Performance:** Index-based searches are faster than full table scans
- ✅ **Result Accuracy:** Search results match reference implementation exactly
- ✅ **Cursor Navigation:** All navigation methods work correctly
- ✅ **Table Type Support:** All three table types (ROW, COL, GROUP) work correctly

### Phase 2 Success Criteria
- ✅ **Comprehensive Testing:** All test scenarios pass
- ✅ **Performance Validation:** Index performance meets expectations
- ✅ **Error Recovery:** Robust handling of index corruption scenarios
- ✅ **Documentation:** Complete implementation documentation

### Phase 3 Success Criteria
- ✅ **Multi-Index Queries:** Complex queries use multiple indexes effectively
- ✅ **Performance Monitoring:** Index statistics provide useful insights
- ✅ **Intelligent Selection:** Index selection optimizes query performance
- ✅ **Advanced Features:** Compound indexes and cost analysis work correctly

## Risk Assessment

### Technical Risks
1. **Index Corruption:** Complex index update logic may introduce bugs
2. **Performance Degradation:** Index overhead may slow down simple operations
3. **Memory Usage:** Index storage may increase memory requirements
4. **Complexity:** Advanced features may make codebase harder to maintain

### Mitigation Strategies
1. **Incremental Implementation:** Implement features one at a time with thorough testing
2. **Comprehensive Testing:** Extensive test coverage for all scenarios
3. **Performance Monitoring:** Regular performance testing and optimization
4. **Documentation:** Maintain detailed documentation for complex features

## Timeline Estimate

### Phase 1: Basic Index Support
- **Index Update Logic:** 2-3 weeks
- **SearchDb Implementation:** 1-2 weeks
- **Cursor Navigation:** 1 week
- **Basic Testing:** 1 week
- **Total:** 5-7 weeks

### Phase 2: Testing and Validation
- **Comprehensive Testing:** 2-3 weeks
- **Performance Testing:** 1-2 weeks
- **Documentation:** 1 week
- **Total:** 4-6 weeks

### Phase 3: Advanced Features (Future)
- **Multi-Index Support:** 3-4 weeks
- **Index Statistics:** 2-3 weeks
- **Compound Index Optimization:** 2-3 weeks
- **Index Cost Analysis:** 3-4 weeks
- **Total:** 10-14 weeks

## Conclusion

This design plan provides a comprehensive roadmap for implementing full index support in GrapaDBX. The phased approach ensures that basic functionality is implemented first, followed by thorough testing and validation, with advanced features as future enhancements.

The plan prioritizes achieving 100% functional parity with the reference implementation while laying the groundwork for advanced index capabilities that could significantly improve query performance for complex workloads. 