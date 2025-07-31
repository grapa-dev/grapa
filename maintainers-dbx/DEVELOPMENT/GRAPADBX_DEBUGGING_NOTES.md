# GrapaDBX Debugging Notes

## Current Debugging Session

### Get Operation Failure Analysis

#### Problem Description
The `get` operation is failing at the `GetDataSize` call. Debug output shows that the block being accessed (`cursor.mValue=40`) was of type `TREE_BLOCK` (type 2) instead of the expected `DATA_BLOCK` (type 5).

#### Debugging Flow
1. `GrapaUnifiedLocalDatabase::FieldGet` calls `GrapaGroup2::GetField`
2. `GrapaGroup2::GetField` calls `GrapaGroup2::FindEntry` which successfully finds the record and returns a `SEARCH_ITEM` cursor (e.g., `mValue=40`, `mTreeRef=3`)
3. `GrapaGroup2::GetField` then calls `GrapaDBX::FindField` to get the field ID
4. `GrapaGroup2::GetField` then calls `GrapaDBX::GetRecordField` (the overload taking `fieldId`)
5. `GrapaDBX::GetRecordField` calls `GrapaDBX::FindRecordField`
6. `GrapaDBX::FindRecordField` receives the `SEARCH_ITEM` cursor (`mValue=40`, `mTreeRef=3`, `mValueType=4`)
7. `GrapaDBX::FindRecordField` then calls `PtrToRec(cursor, recCursor)`
8. Inside `PtrToRec`, for `SEARCH_ITEM`, it attempts to call `GetTreeStore(ptrCursor, tableRef, storeType)`
9. `GrapaBtree::GetTreeStore` is called with `cursor.mTreeRef=3`
10. The latest debug output shows `[DEBUG] GetTreeStore: head.Read failed with error -1`

#### Root Cause Analysis
The core problem is that `head.Read(mFile, 3)` is failing, indicating an issue with reading the root tree block itself, or an incorrect `mTreeRef` being passed to `GetTreeStore` in the context of `SEARCH_ITEM` within `PtrToRec`.

#### Cursor State Analysis
- `cursor.mValue=40` (the data block)
- `cursor.mTreeRef=3` (the table tree)
- `cursor.mValueType=4` (SEARCH_ITEM)

The current debug output suggests `PtrToRec` is being called with `cursor.mValue=40` (the data block) and `cursor.mTreeRef=3` (the table tree). `GetTreeStore` is then called on `cursor.mTreeRef` (3), which is the table tree, not the data block. This might be an incorrect usage of `GetTreeStore` for `SEARCH_ITEM`s.

### Cursor Handling Fixes Applied

#### 1. FindEntry Signature Update
**Problem**: `FindEntry` was not returning the positioned cursor, leading to re-searches and incorrect cursor states.

**Fix**: Modified `GrapaGroup2::FindEntry` signature to include an output `GrapaDBXCursor& outCursor` parameter.

```cpp
// Before
GrapaError FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId);

// After
GrapaError FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId, GrapaDBXCursor& outCursor);
```

#### 2. SEARCH_ITEM Handling
**Problem**: `SEARCH_ITEM` (type 4) was not handled in multiple switch statements.

**Fixes Applied**:
- Added `SEARCH_ITEM` to `GrapaDBX::FindRecordField` case that calls `PtrToRec`
- Added `SEARCH_ITEM` to `GrapaDBX::GetRecordField` (overload with `GrapaDBXField& field`) case that copies cursor directly
- Added `SEARCH_ITEM` to `GrapaDBX::PtrToRec` case that returns 0, with specific logic for `SEARCH_ITEM`

#### 3. Cursor Variable Redefinition
**Problem**: Redundant cursor declarations in `GrapaGroup2::GetField`.

**Fix**: Removed redundant `GrapaDBXCursor& cursor = foundCursor;` and initial `GrapaDBXCursor cursor;` declarations, ensuring only one `GrapaDBXCursor cursor;` is used after the `FindEntry` call.

### Debug Output Enhancement

#### INDEX Section Implementation
- ✅ INDEX and STORE sections added to `DumpTheTree` method
- ✅ Table tree (5) now being dumped in addition to root tree (3)
- ✅ INDEX section showing up in debug output
- ✅ SU64 to database-level conversion implemented in `DumpTheValue`
- ✅ Main list sections now show `CREC` instead of `SU64`
- ✅ Index creation and TREE_ITEM insertion working correctly
- ⚠️ INDEX tree not displaying TREE_ITEM entries in debug output

#### Debug Print Additions
Added debug prints to trace execution:
- `GrapaDBX::FindRecordField`: Shows `cursor.mValueType`
- `GrapaBtree::GetTreeStore`: Traces execution and parameters
- `GrapaDBX::PtrToRec`: Shows `mTreeRef` and error conditions

### Next Steps for Get Operation Fix

1. **Verify File Integrity**
   - Check if block ID 3 exists and is readable
   - Verify `mFile` is correctly opened and accessible
   - Confirm file structure is intact

2. **Re-evaluate PtrToRec Logic**
   - Determine if `GetTreeStore` is necessary for `SEARCH_ITEM`s
   - Check if `mTreeRef=3` is the correct parameter for `GetTreeStore`
   - Consider alternative approaches for `SEARCH_ITEM` handling

3. **Compare with GrapaDB/GrapaGroup**
   - Examine how `SEARCH_ITEM` is handled in reference implementation
   - Check if `GetTreeStore` is used differently in legacy code
   - Verify cursor state expectations

### Technical Insights

#### Cursor Type Handling
- `SEARCH_ITEM` (type 4) represents a search result that needs dereferencing
- `PtrToRec` is responsible for converting pointer/search cursors to record cursors
- `GetTreeStore` retrieves the store tree reference from a tree block header

#### Block Type Analysis
- `TREE_BLOCK` (type 2): Contains tree structure information
- `DATA_BLOCK` (type 5): Contains actual data
- The issue suggests data is being stored as `TREE_BLOCK` instead of `DATA_BLOCK`

#### File Structure
- Block ID 3: Root tree block (should be readable)
- Block ID 40: Data block (currently type 2 instead of 5)
- The failure to read block 3 suggests fundamental file structure issues

### Lessons Learned

1. **Cursor State Preservation**: Critical to maintain cursor state through function calls
2. **SEARCH_ITEM Handling**: Must be handled consistently across all cursor operations
3. **Debug Output**: Essential for tracing complex cursor state issues
4. **File Integrity**: Block reading failures often indicate deeper structural problems

Last Updated: Current debugging session - Investigating `GetTreeStore` failure with root tree block (ID 3) 