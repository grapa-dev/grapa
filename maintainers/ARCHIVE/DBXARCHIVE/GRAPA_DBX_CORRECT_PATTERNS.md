# GrapaDBX Correct Patterns

## Overview
This document captures the correct and incorrect patterns for using GrapaDBX, based on empirical testing and implementation analysis.

## ✅ Working Patterns

### In-Memory Database via $unified()
```grapa
/* ✅ WORKING - In-memory GrapaDBX via $unified() */
u = $unified();
result = u.create("memory://test_db");
if (result.type() != $ERR) {
    u.mk("test_table", "GROUP");
    u.cd("test_table");
    u.mkfield("data", "STR", "VAR");
    u.set("key1", "value1", "data");
    retrieved = u.get("key1", "data");
    ("Retrieved value: " + retrieved + "\n").echo();
}
```

### File-Based Database via $unified()
```grapa
/* ✅ WORKING - File-based GrapaDBX via $unified() */
u = $unified();
result = u.create("grapadbx://test_file.dbx");
if (result.type() != $ERR) {
    u.mk("test_table", "GROUP");
    u.cd("test_table");
    u.mkfield("data", "STR", "VAR");
    u.set("key1", "value1", "data");
    retrieved = u.get("key1", "data");
    ("Retrieved value: " + retrieved + "\n").echo();
}
```

### Direct GrapaDBX Usage
```grapa
/* ✅ WORKING - Direct GrapaDBX usage */
dbx = $file().dbx();
dbx.create("test_file.dbx");
dbx.mk("test_table", "GROUP");
dbx.cd("test_table");
dbx.mkfield("data", "STR", "VAR");
dbx.set("key1", "value1", "data");
retrieved = dbx.get("key1", "data");
```

## ❌ Incorrect Patterns

### Wrong: Direct $file().table() for DBX
```grapa
/* ❌ WRONG - $file().table() doesn't work for DBX */
table = $file().table("test.dbx");  /* This creates GrapaDB, not GrapaDBX */
```

### Wrong: Incorrect URL Schemes
```grapa
/* ❌ WRONG - These URL schemes don't work */
u.create("dbx://test");        /* Should be grapadbx:// */
u.create("memory://test");     /* This works now! */
u.create("file://test");       /* Should be grapadbx:// */
```

## Current Implementation Status

### ✅ Working Components
1. **Storage Type Detection**: `memory://` and `grapadbx://` URLs correctly detected
2. **Database Creation**: Both in-memory and file-based databases can be created
3. **Field Operations**: `mkfield()`, `set()`, and basic `get()` operations work
4. **Routing**: `$unified()` correctly routes to `GrapaDBX` for appropriate storage types
5. **Basic Data Storage**: Data is being stored (confirmed by debug output)
6. **Index Discovery Methods**: `LocateIndex()` and `IndexHasField()` methods implemented

### ❌ Current Issues
1. **Record Retrieval**: `FindRecordField` failing with error -1, cursor `mValue=0`
2. **Performance**: Very high times for file-based operations (140+ seconds for 100 records)
3. **Error Handling**: Invalid URL handling needs improvement
4. **🔥 CURRENT: Index-Based Searching**: `SearchDb()` method needs proper implementation

## 🔥 Current Task: Implement Proper Index-Based Searching

### **Task Overview**
The current `SearchDb()` implementation in `GrapaDBX.cpp` is a placeholder that doesn't properly utilize indexes. This needs to be replaced with a proper index-based searching implementation according to the planned architecture.

### **Requirements (from user)**
1. **Index Selection**: Match up to the best available index for the table
2. **Index-Based Traversal**: Use index pointers to traverse records when good index found
3. **$KEY Default**: If no field specified, use $KEY index (guaranteed to exist)
4. **Fallback to Table Scan**: Only scan records directly if no suitable index found
5. **Field-Specific Search**: Use appropriate index for specified fields, fallback to table scan if no index available

### **Current Implementation Status**
- ✅ **`LocateIndex()` method**: Implemented to find appropriate index for field
- ✅ **`IndexHasField()` method**: Implemented to check if index contains field
- ❌ **`SearchDb()` method**: Current implementation is incomplete placeholder
- ❌ **Index selection logic**: Not implemented
- ❌ **Two-stage search process**: Not implemented
- ❌ **$KEY field handling**: Not implemented

### **Implementation Plan**

#### **Step 1: Replace SearchDb Implementation**
- Replace current placeholder in `source/grapa/GrapaDBX.cpp`
- Add proper index selection using `LocateIndex()`
- Implement two-stage search: index-based first, then table scan if needed
- Add proper `$KEY` field handling as default

#### **Step 2: Implement Index Selection Logic**
- Check if search fields have available indexes using `LocateIndex()`
- If no specific index found, check if `$KEY` index is appropriate
- Only fallback to table scan if no suitable index available
- Use `GrapaBtree::SEARCH_MODE` for proper comparison

#### **Step 3: Implement Two-Stage Search Process**
- **Stage 1**: Use index to narrow down records (if good index found)
- **Stage 2**: Scan remaining records for additional field criteria
- Implement proper cursor navigation using `FirstDb`, `NextDb`, `PrevDb`
- Use `CompareKey()` with `SEARCH_MODE` for field comparisons

#### **Step 4: Test and Validate**
- Test with existing `test_unified_database_working.grc`
- Verify index-based searches work correctly
- Verify fallback to table scan works when no index available
- Benchmark performance improvements

### **Technical Implementation Details**
- **File**: `source/grapa/GrapaDBX.cpp` - `SearchDb()` method
- **Dependencies**: `LocateIndex()`, `IndexHasField()`, `CompareKey()`
- **Constants**: Use `GrapaBtree::SEARCH_MODE` (value 0) for comparisons
- **Integration**: Works with existing `$unified()` system

### **Reference Documents**
- `maintainers/DEVELOPMENT/GRAPA_DBX_ENHANCED_INDEXING_SYSTEM.md` - Planned architecture
- `maintainers/IMPLEMENTATION/GRAPA_DBX_INDEX_DESIGN.md` - Technical design
- `maintainers/DEVELOPMENT/CURRENT_STATUS.md` - Current task status

## Technical Details

### Storage Type Detection
The `ParseStorageUrl` method in `GrapaUnifiedLocalDatabase` correctly identifies:
- `memory://` → `MEMORY` storage type
- `grapadbx://` → `GRAPADBX` storage type
- `grapadb://` → `GRAPADB` storage type
- Default → `FILESYSTEM` storage type

### Field Operations Routing
The `FieldGet` and `FieldSet` methods in `GrapaUnifiedLocalDatabase` route to:
- `MEMORY` → `GrapaDBX` with in-memory file
- `GRAPADBX` → `GrapaDBX` with file-based storage
- `FILESYSTEM` → Parent `GrapaLocalDatabase` implementation

### Debug Output Analysis
Successful operations show:
```
[DEBUG] ParseStorageUrl: memory:// comparison returned 0, setting MEMORY type
[DEBUG] ParseStorageUrl: mStorageType='MEMORY', mStoragePath='test_db'
[DEBUG] GrapaLibRule Set: UnifiedDB found, storage type = 'MEMORY'
[DEBUG] GrapaLibRule Set: Final err = 0, result = 0x13e21c180
```

Failed operations show:
```
[DEBUG] GetRecordField: called with fieldId=0, cursor.mValue=0
[DEBUG] GetRecordField: FindRecordField failed with error -1
```

## Performance Considerations

### Current Performance Issues
- **File-based operations**: 140+ seconds for 100 records
- **Likely cause**: BTree file expansion on every new record
- **Potential solution**: Pre-allocate larger BTree files and compact later

### Caching Status
- **BTree caching**: Implemented at C++ level
- **Script-level caching**: Needs testing and optimization
- **Performance optimization**: High priority for file-based operations

## Error Handling

### Current Error Patterns
```grapa
/* Error handling for $unified() operations */
result = u.create("memory://test_db");
if (result.type() == $ERR) {
    ("Creation failed: " + result + "\n").echo();
} else {
    ("Creation succeeded\n").echo();
}
```

### Error Types
- `$ERR` type: Contains error details as `$LIST`
- Storage type detection errors: Invalid URLs fall back to `FILESYSTEM`
- Field operation errors: `FindRecordField` failures need investigation

## Testing Patterns

### Basic Functionality Test
```grapa
/* test/grapadbx/test_simple_unified_fix.grc */
"=== Testing Unified Database Fix ===\n".echo();

u = $unified();
result = u.create("memory://test_db");
("Creation result: " + result + "\n").echo();

if (result.type() != $ERR) {
    u.mk("test_table", "GROUP");
    u.cd("test_table");
    u.mkfield("data", "STR", "VAR");
    u.set("key1", "value1", "data");
    retrieved = u.get("key1", "data");
    ("Retrieved value: " + retrieved + "\n").echo();
} else {
    ("Creation failed: " + result + "\n").echo();
}
```

### Performance Test
```grapa
/* test/grapadbx/test_unified_database_working.grc */
/* Comprehensive test with performance benchmarking */
```

## Next Steps

### Immediate Priority
1. **Fix FindRecordField**: Investigate cursor initialization issue
2. **Performance Optimization**: Address BTree file expansion issue
3. **Error Handling**: Improve invalid URL handling

### Medium Priority
1. **Caching Integration**: Test and optimize script-level caching
2. **Comprehensive Testing**: Create test suite for all storage types
3. **Documentation Updates**: Update user documentation with working examples

## References

- `maintainers/DEVELOPMENT/CURRENT_STATUS.md` - Current implementation status
- `maintainers/IMPLEMENTATION/GRAPA_UNIFIED_SYSTEM_ANALYSIS.md` - Technical analysis
- `test/grapadbx/test_simple_unified_fix.grc` - Basic working test
- `test/grapadbx/test_unified_database_working.grc` - Comprehensive test 

## **Complete GrapaDB Baseline Analysis** (from `.debug()` output)

### **ROW Database Pattern**:
```
LIST start (3) key=0 type=RTABLE parent=0 size=1
| | RREC (55) key=1 node=(58,0) weight=1: 1=Alice 2=25 3=New York 4=user1 
INDEX start (5) key=0
| | | TREE start (74) key=1
| | | | | | RPTR (0) key=1 node=(84,0) weight=1: RREC (55) key=1 node=(58,0) weight=1: 1=Alice 2=25 3=New York 4=user1
| | | | | | | SU64 key=1 value=4  // $KEY index (field 4)
```

### **COL Database Pattern**:
```
LIST start (3) key=0 type=CTABLE parent=0 size=1
| | CREC (0) key=1 node=(71,0) weight=1: 1=Alice 2=25 3=New York 4=user1 
STORE start (39) key=0 type=DATA  // Separate data storage
| | | BYTE_DATA (252) parent=41 key=(1/256,1/8) value=Alice
INDEX start (5) key=0
| | | TREE start (99) key=1
| | | | | | CPTR (0) key=1 node=(246,0) weight=1: RREC (0) key=1 node=(71,0) weight=1: 1=Alice 2=25 3=New York 4=user1
| | | | | | | SU64 key=1 value=4  // $KEY index (field 4)
```

### **GROUP Database Pattern**:
```
LIST start (108) key=0 type=GROUP parent=3 size=2
| | TREE start (110) key=0
| | | LIST start (110) key=0 type=RTABLE parent=108 size=1
| | | | | RREC (260) key=1 node=(262,0) weight=1: 1=bob 
| | | INDEX start (118) key=0
| | | | | | TREE start (277) key=1
| | | | | | | | | RPTR (0) key=1 node=(287,0) weight=1: RREC (260) key=1 node=(262,0) weight=1: 1=bob 
| | | | | | | | | | SU64 key=1 value=1  // $KEY index (field 1)
```

### **Key Differences by Database Type**:
- **ROW**: Uses `RTABLE` type with `RREC` records, `RPTR` pointers
- **COL**: Uses `CTABLE` type with `CREC` records, `CPTR` pointers, separate `STORE` section
- **GROUP**: Uses `GROUP` type but contains `RTABLE` with `RREC` records and `RPTR` pointers
- **Index Pattern**: All use `SU64 key=1 value=X` where X is the $KEY field number (4 for ROW/COL, 1 for GROUP)

### **Common Index-Based Search Pattern**:
1. **$KEY Index**: Always present, field 4 (ROW/COL) or field 1 (GROUP)
2. **Pointer Traversal**: `RPTR/CPTR` → `RREC/CREC` → actual data
3. **Index Lookup**: Use `SU64 value=X` to find the indexed field
4. **Record Retrieval**: Follow pointer chain to get actual record data

### **Implementation Requirements for GrapaDBX**:
- **Support All Types**: ROW, COL, and GROUP patterns
- **$KEY Indexing**: Default to appropriate $KEY field for each type
- **Pointer Navigation**: Use `PtrToRec()` for all database types
- **Type Detection**: Determine database type and use appropriate index pattern
- **Debug Validation**: Show same debug output structure as GrapaDB 