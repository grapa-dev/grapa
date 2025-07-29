# GrapaDB Implementation Reference

## Purpose
This document provides a comprehensive reference for the `GrapaDB` class, its methods, enums, and associated data structures. It is intended as the authoritative source for understanding and maintaining the database layer in Grapa, including table management, indexing, record operations, and the complex pointer dereferencing system that enables efficient data storage and retrieval.

---

## Class: GrapaDB

### Overview
`GrapaDB` extends `GrapaBtree` to provide a complete database management system, supporting tables, indexes, records, and complex data types. It implements a sophisticated pointer dereferencing system that allows indexes to store pointers to actual record data, enabling efficient storage and retrieval while maintaining data integrity.

### Key Data Members
- `GrapaFile *mDumpFile;` — Pointer to file for debug dumping operations.

### Enums
- **Item Types:** `SEARCH_ITEM=LAST_ITEM, DTYPE_ITEM, GREC_ITEM, RREC_ITEM, CREC_ITEM, GPTR_ITEM, RPTR_ITEM, CPTR_ITEM`
- **Tree Types:** `GROUP_TREE=LAST_TREE, RTABLE_TREE, CTABLE_TREE`
- **Store Types:** `IPTR_STORE=LAST_STORE`
- **Comparison Types:** `NULL_CMP=0, LT_CMP, LTEQ_CMP, EQ_CMP, GTEQ_CMP, GT_CMP`
- **Store Types:** `STORE_FIX = 0, STORE_VAR, STORE_PAR`

### Core Methods

#### File and Database Management
- **`Create(const char *pFileName, u8 treeType, u64& firstTree)`** — Create a new database file with specified tree type.
- **`CreateRoot(u8 treeType, u64& firstTree)`** — Create the root tree structure for the database.
- **`OpenFile(const char *fileName, char mode)`** — Open an existing database file.
- **`RootTree(u8& pRootType)` / `RootTree(GrapaCHAR& pRootType)`** — Get the root tree reference and type.
- **`CloseFile()`** — Close the database file.

#### Table Management
- **`LastTableId(u64 firstTree, u64& pTableId)`** — Find the highest table ID in a tree.
- **`CreateTable(u64 firstTree, u8 pTreeType, u64 pTableId, GrapaDBTable& pTable)`** — Create a new table with specified type and ID.
- **`OpenTable(u64 firstTree, u64 pTableId, GrapaDBTable& pTable)`** — Open an existing table by ID.
- **`DeleteTable(u64 firstTree, u64 pTableId)`** — Delete a table and all its contents.
- **`CreateAlias/UpdateAlias`** — Manage table aliases for cross-database references.

#### Field Management
- **`CreateTableField(GrapaDBTable& pTable, GrapaDBField& pField, const GrapaCHAR& pName)`** — Add a field to a table.
- **`OpenTableField(GrapaDBTable& pTable, u64 pFieldId, GrapaDBField& pField)`** — Open a field by ID.
- **`OpenTableFieldList(GrapaDBTable& pTable, GrapaDBFieldArray& pFieldList)`** — Get all fields in a table.
- **`DeleteTableField(GrapaDBTable& pTable, u64 pFieldId)`** — Remove a field from a table.
- **`FlushTableFields(GrapaDBTable& pTable)`** — Persist field changes to disk.

#### Index Management
- **`FindFreeIndexId(GrapaDBIndex& pIndex, u64 pMinId, u64& pIndexId)`** — Find an available index ID.
- **`CreateIndex(GrapaDBTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBIndex& pIndex)`** — Create an index on specified fields.
- **`OpenIndex(GrapaDBTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBIndex& pIndex)`** — Open an existing index.
- **`DeleteIndex(GrapaDBTable& pTable, u64 pIndexId)`** — Remove an index.
- **`RefreshIndex(GrapaDBIndex& pIndex)`** — Rebuild an index after data changes.
- **`CreateIndexField/OpenIndexField`** — Manage index field mappings.

#### Record Operations
- **`FindFreeRecordId(GrapaDBTable& pTable, u64& pRecordId)`** — Find an available record ID.
- **`CreateRecord(GrapaDBTable& pTable, GrapaCursor& pCursor)`** — Create a new record in a table.
- **`DeleteRecord(GrapaDBTable& pTable, GrapaCursor& pCursor)`** — Delete a record from a table.
- **`FindRecordField(GrapaCursor& pCursor, u64 fieldId, GrapaCursor& recCursor, GrapaDBField& pField)`** — Locate a field within a record.
- **`SetRecordField(GrapaCursor& pCursor, GrapaDBFieldValueArray& pFieldList)`** — Update record field values.
- **`GetRecordField(GrapaCursor& pCursor, GrapaDBField& pField, GrapaBYTE& pValue)`** — Retrieve field value from a record.
- **`GetRecordField(GrapaCursor& pCursor, u64 pFieldId, GrapaBYTE& pValue)`** — Retrieve field value by field ID.

#### Search and Navigation
- **`SearchDb(GrapaDBCursor& pCursor, GrapaDBTable& pTable, GrapaDBFieldValueArray& pFieldList)`** — Search for records matching criteria.
- **`FirstDb/LastDb/NextDb/PrevDb(GrapaDBCursor& pCursor)`** — Navigate through search results.

#### Overridden BTree Methods
- **`NewTree(u64& treePtr, u8 treeType, u64 parentTree = 0LL, u8 nodeCount = NODE_WIDTH)`** — Create a new tree with database-specific logic.
- **`CompareKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult)`** — Compare keys with database-specific logic.
- **`DeleteKey(GrapaCursor& pTreeCursor)`** — Delete a key with index cleanup.
- **`Delete(GrapaCursor& cursor)`** — Delete with index cleanup.

#### Debug and Utility Methods
- **`DumpFile(GrapaFile *pDumpFile = NULL)`** — Dump database structure for debugging.
- **`DumpTree(u64 pTreeRef = 0, GrapaFile *pDumpFile = NULL)`** — Dump a specific tree structure.
- **`GetData(u64 itemPtr, GrapaCHAR& pValue)`** — Retrieve data from a data pointer.

---

## Index Architecture and Search Optimization

### Overview
GrapaDB implements a sophisticated index system that provides efficient query performance through a combination of index-based bounding and full table scanning. The system uses a **two-stage search process** that optimizes for both indexed and non-indexed field searches.

### Index Update Pattern

#### Three-Phase Index Update Process
When a field is updated in a record, GrapaDB follows a **three-phase process** to maintain index consistency:

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

#### Key Characteristics
- **Atomic Updates:** Index updates are performed atomically with record updates
- **All Table Types:** Supports GROUP_TREE, RTABLE_TREE, and CTABLE_TREE
- **All Pointer Types:** Uses appropriate pointer types (GPTR_ITEM, RPTR_ITEM, CPTR_ITEM)
- **Field-Level Granularity:** Only updates indexes that contain the modified fields

### Search Optimization Architecture

#### Two-Stage Search Process
GrapaDB implements a **two-stage search process** that optimizes query performance:

```cpp
GrapaError GrapaDB::SearchDb(GrapaDBCursor& cursor, GrapaDBTable& pTable, GrapaDBFieldValueArray& pFieldList)
{
    // Stage 1: Index Selection (Simple Approach)
    field = pFieldList.GetFieldAt(0);  // Use first field's index
    indexCursor.Set(pTable.mRecRef);
    err = GetTreeIndex(indexCursor, indexRef);
    if (!err) {
        err = LocateIndex(indexCursor, indexRef, field->mId);
        if (!err) {
            usingIndex = true;
            cursor.SetSearch(this, indexCursor.mValue, usingIndex, &pFieldList);
        }
    }
    
    // Stage 2: Search Execution
    if (usingIndex) {
        // Index-based search: Use index to bound search space
        err = Search(cursor);
        err = FirstDb(cursor);
    } else {
        // Full table scan: Scan all records for matches
        cursor.SetSearch(this, pTable.mRecRef, usingIndex, &pFieldList);
        err = First(cursor);
        err = FirstDb(cursor);
        // ... compare and iterate through results
    }
}
```

#### Index Selection Strategy
The reference implementation uses a **simple but effective** index selection strategy:

1. **First Field Priority:** Uses the first field in the search criteria
2. **Index Availability:** Checks if an index exists for that field
3. **Fallback to Full Scan:** If no suitable index is found, performs full table scan

#### Cursor Navigation
Search results are navigated using specialized cursor methods:

```cpp
GrapaError GrapaDB::FirstDb(GrapaDBCursor& cursor)   // Find first matching record
GrapaError GrapaDB::NextDb(GrapaDBCursor& cursor)    // Navigate to next match
GrapaError GrapaDB::LastDb(GrapaDBCursor& cursor)    // Find last matching record
GrapaError GrapaDB::PrevDb(GrapaDBCursor& cursor)    // Navigate to previous match
```

### Index Structure and Management

#### Index BTree Structure
Each index is implemented as a BTree with the following structure:

```
Index BTree:
├── Key=0: DICT field (metadata about index structure)
├── Key=1: Index entry pointing to record 1
├── Key=2: Index entry pointing to record 2
└── ...: Additional index entries
```

#### Index Entry Types
- **GPTR_ITEM:** Group pointer item (for GROUP_TREE tables)
- **RPTR_ITEM:** Row pointer item (for RTABLE_TREE tables)  
- **CPTR_ITEM:** Column pointer item (for CTABLE_TREE tables)

#### Index Field Mapping
Indexes maintain a mapping between index fields and table fields:

```cpp
// Index field mapping structure
struct IndexFieldMapping {
    u64 indexFieldId;    // Index field identifier
    u64 tableFieldId;    // Corresponding table field identifier
};
```

### Performance Characteristics

#### Index-Based Search Performance
- **Bounding Effect:** Index narrows search space significantly
- **Logarithmic Complexity:** O(log n) for index traversal
- **Pointer Dereferencing:** Additional overhead for record access
- **Memory Efficiency:** Indexes store pointers, not data

#### Full Table Scan Performance
- **Linear Complexity:** O(n) for complete table scan
- **Direct Access:** No pointer dereferencing overhead
- **Memory Intensive:** May require loading large datasets
- **Fallback Strategy:** Used when no suitable index exists

#### Hybrid Search Performance
- **Two-Stage Process:** Index bounding + full scan for remaining fields
- **Optimized for Mixed Queries:** Combines benefits of both approaches
- **Flexible Field Matching:** Supports partial index matches

### Limitations and Enhancement Opportunities

#### Current Limitations
1. **Simple Index Selection:** Only uses first field's index
2. **No Multi-Index Support:** Cannot use multiple indexes simultaneously
3. **No Index Statistics:** No performance monitoring or selectivity analysis
4. **No Compound Index Optimization:** Limited multi-field index usage
5. **No Cost-Based Optimization:** No intelligent index selection

#### Enhancement Opportunities
1. **Multi-Index Support:** Use multiple indexes for complex queries
2. **Index Statistics:** Track cardinality, selectivity, and performance metrics
3. **Compound Index Optimization:** Optimize multi-field index usage
4. **Cost-Based Index Selection:** Intelligent index selection based on statistics
5. **Index Maintenance Optimization:** Efficient index update strategies

### Integration with GrapaDBX

#### Current Status
GrapaDBX is missing the complete index architecture:
- ❌ **Index Update Logic:** No three-phase update pattern
- ❌ **SearchDb Method:** No index selection and two-stage search
- ❌ **Cursor Navigation:** No FirstDb/NextDb/LastDb/PrevDb methods
- ❌ **Index Integration:** No proper integration with field updates

#### Implementation Requirements
To achieve 100% parity with GrapaDB, GrapaDBX needs:
1. **Index Update Logic:** Implement complete three-phase update pattern
2. **SearchDb Method:** Implement index selection and two-stage search
3. **Cursor Navigation:** Implement FirstDb/NextDb/LastDb/PrevDb methods
4. **Index Management:** Implement CreateIndex/OpenIndex/DeleteIndex/RefreshIndex
5. **Helper Methods:** Implement LocateIndex and IndexHasField methods

### Associated Data Structures

#### GrapaDBTable
Represents a database table with metadata:
```cpp
struct {
    u8 mType;           // Table type (GROUP_TREE, RTABLE_TREE, CTABLE_TREE)
    u8 mStore;          // Storage type (STORE_FIX, STORE_VAR, STORE_PAR)
    u8 mTreeType;       // Tree type
    u8 mReserved[5];    // Reserved bytes
    u64 mId;            // Table ID
    u64 mRef;           // Table reference pointer
    u64 mNameId;        // Name ID
    u64 mNameRef;       // Name reference pointer
    u64 mDictOffset;    // Dictionary offset
    u64 mDictSize;      // Dictionary size
    u64 mSize;          // Table size
    u64 mGrow;          // Growth factor
    u64 mTableRef;      // Extended metadata
    u64 mReserved2[2];  // Additional reserved bytes
};
```

#### GrapaDBField
Represents a database field with type and storage information:
- **`Init(u64 pFieldId, u8 pType, u8 pStore, u64 pSize, u64 pGrow)`** — Initialize field metadata.
- **`Write(GrapaDB *pDb, u64 fieldRef)`** — Write field metadata to storage.
- **`Read(GrapaDB *pDb, u64 fieldRef)`** — Read field metadata from storage.
- **`Get(GrapaDB *pDb, u64 tableRef, u64 fieldId)`** — Retrieve field by ID.

#### GrapaDBIndex
Represents a database index with field mappings and metadata.

#### GrapaDBCursor
Extends `GrapaCursor` with database-specific search and navigation capabilities:
- **`SetSearch(GrapaDB* pDb, u64 pTreeRef, bool pUsingIndex, GrapaDBFieldValueArray* pData)`** — Configure cursor for search operations.

#### GrapaDBFieldArray
Array of field definitions for a table.

#### GrapaDBFieldValueArray
Array of field values for record operations and search criteria.

---

## Deep Dive: How GrapaDB Operates

### Database Architecture
GrapaDB uses a hierarchical structure:
1. **Database File** — Contains multiple trees (tables)
2. **Tables** — Contain records and indexes
3. **Records** — Store actual data
4. **Indexes** — Store pointers to records for efficient lookup

### Pointer Dereferencing System
The core innovation of GrapaDB is its pointer dereferencing system:

#### Value Types
- **`GREC_ITEM/RREC_ITEM/CREC_ITEM`** — Actual record data
- **`GPTR_ITEM/RPTR_ITEM/CPTR_ITEM`** — Pointers to record data
- **`SEARCH_ITEM`** — Search criteria for multi-field lookups

#### PtrToRec Method
```cpp
GrapaError GrapaDB::PtrToRec(GrapaCursor& ptrCursor, GrapaCursor& recCursor)
```
This method dereferences pointers to actual record data:
1. If input is already a record type, return immediately
2. For pointer types, get the table reference and store type
3. Convert pointer type to corresponding record type:
   - `GPTR_ITEM` → `GREC_ITEM` (GROUP_TREE)
   - `RPTR_ITEM` → `RREC_ITEM` (RTABLE_TREE)  
   - `CPTR_ITEM` → `CREC_ITEM` (CTABLE_TREE)
4. Search for the actual record using the key

### Comparison System
GrapaDB implements a sophisticated comparison system:

#### CompareKey Method
```cpp
GrapaError GrapaDB::CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
```
This method handles different comparison scenarios:
1. **`DTYPE_ITEM`** — Direct key comparison
2. **Record/Pointer Types** — Dispatch to appropriate comparison method:
   - **`SEARCH_ITEM`** → `CompareSearchKey` (multi-field search)
   - **Record/Pointer Types** → `CompareRecordKey` (record comparison)

#### CompareRecordKey Method
```cpp
GrapaError GrapaDB::CompareRecordKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
```
This method compares actual record data:
1. Dereference both cursors using `PtrToRec`
2. Compare record values based on type
3. For complex records, iterate through indexed fields and compare field values
4. Use `strcmp` for field comparisons (note: this is a known limitation)

#### CompareSearchKey Method
```cpp
GrapaError GrapaDB::CompareSearchKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
```
This method handles multi-field index searches:
1. Extract search criteria from `dataCursor`
2. Dereference the tree cursor to get actual record data
3. Compare each search field against the corresponding record field
4. Use `strcmp` for field comparisons (note: this is a known limitation)

### Index Management
Indexes in GrapaDB store pointers to records, not the actual data:

#### Index Creation
1. Create index tree with `RPTR_ITEM` type
2. For each record, insert a pointer (`RPTR_ITEM`) with the record's key
3. The index enables efficient lookup without duplicating data

#### Index Operations
- **Insert** — Add pointer to new record
- **Delete** — Remove pointer when record is deleted
- **Search** — Use `CompareRecordKey` to dereference and compare actual data

### Record Operations
Records are stored with their actual data, and indexes maintain pointers:

#### CreateRecord
1. Allocate record ID
2. Insert record data into table
3. Update all indexes with pointers to the new record

#### DeleteRecord  
1. Delete record data from table
2. Remove all index pointers to the deleted record
3. Clean up any orphaned data

#### SetRecordField
1. Update the field value in the record
2. If the field is indexed, update the index:
   - Delete old index entry
   - Insert new index entry with updated value

### Transaction and Consistency
GrapaDB maintains consistency through:
- **Index Synchronization** — All indexes are updated when records change
- **Pointer Validation** — `PtrToRec` ensures pointers are valid
- **Atomic Operations** — Index updates are performed atomically with record changes

---

## GrapaDB Storage Architecture: Table Layouts and Indexing

GrapaDB organizes all persistent data using BTrees, leveraging them for both record/column storage and for all indexes. The storage architecture varies by table type (ROW, COL, GROUP), but all share a common foundation:
- **Data BTrees**: Store the actual user data (records or columns)
- **Index BTrees**: Store pointers to data BTrees for fast lookup/search
- **Metadata/Dictionary BTrees**: Store field definitions, types, and schema info

### Table Layouts

#### ROW Table
- **Data Organization**: Each record (row) is stored as a single entry in the data BTree; all fields for a record are together.
- **Indexes**: One or more index BTrees point to record locations in the data BTree (using RPTR_ITEM, etc.).
- **Metadata**: Dictionary/field definitions are stored in a DICT field (key==0) in the index BTree.

#### COL Table
- **Data Organization**: Each column is stored separately, often as its own BTree (using FREC_DATA for fragmentation).
- **Indexes**: Indexes may be per-column or global, depending on configuration; pointers link index BTrees to column BTrees.
- **Fragmentation**: FREC_DATA and weighted BTree are used for efficient column storage and updates.
- **Metadata**: Dictionary/field definitions as in ROW.

#### GROUP Table
- **Data Organization**: Hierarchical or nested records (like a directory tree or JSON object); may use a BTree per group or nested structure.
- **Indexes**: Can be hierarchical or flat; pointers link parent/child groups and their indexes.
- **Metadata**: As above, with possible extensions for nested/grouped fields.

### Diagram: High-Level Storage Layout

```
+-------------------+         +-------------------+
|   Data BTree(s)   | <------ |   Index BTree(s)  |
| (records/columns) |         | (pointers to data)|
+-------------------+         +-------------------+
         ^                           ^
         |                           |
         +-----------+   +-----------+
                     |   |
             +-------------------+
             |  Metadata/DICT    |
             |  (field defs, etc)|
             +-------------------+
```
- For COL tables, there may be multiple data BTrees (one per column), each with its own index or shared indexes.
- GROUP tables may have a tree of data/index BTrees linked by parent/child pointers.

### Comparison Table: Table Layouts

| Table Type | Data Storage         | Indexing                | Metadata/DICT         |
|------------|---------------------|-------------------------|-----------------------|
| ROW        | One BTree for rows  | Index BTrees point to records | DICT field in index BTree |
| COL        | BTree per column (FREC_DATA) | Index per column or global | DICT field in index BTree |
| GROUP      | BTree per group/nested structure | Hierarchical or flat indexes | DICT, possibly nested    |

### Why This Matters
- **For maintainers**: Understanding the mapping from high-level tables to BTrees is essential for debugging, extending, or recovering GrapaDB files.
- **For debugging/forensics**: Knowing how data and indexes are linked allows for manual recovery and low-level analysis.
- **For extensibility**: Enables future contributors to add new table types, optimize storage, or build tools for migration and analysis.

---

## How to Properly Use GrapaDB

### Database Initialization
1. Create a `GrapaDB` instance
2. Call `Create(fileName, treeType, firstTree)` to initialize
3. Use `OpenFile(fileName, mode)` for existing databases

### Table Operations
1. **Create Table:** `CreateTable(firstTree, treeType, tableId, table)`
2. **Open Table:** `OpenTable(firstTree, tableId, table)`
3. **Add Fields:** `CreateTableField(table, field, name)`
4. **Create Indexes:** `CreateIndex(table, indexId, fieldList, index)`

### Record Operations
1. **Create Record:** `CreateRecord(table, cursor)`
2. **Update Fields:** `SetRecordField(cursor, fieldValueArray)`
3. **Retrieve Data:** `GetRecordField(cursor, fieldId, value)`
4. **Delete Record:** `DeleteRecord(table, cursor)`

### Search Operations
1. **Setup Search:** Create `GrapaDBFieldValueArray` with search criteria
2. **Execute Search:** `SearchDb(cursor, table, fieldValueArray)`
3. **Navigate Results:** Use `FirstDb/NextDb/LastDb/PrevDb`

### Index Usage
1. **Create Indexes** on frequently searched fields
2. **Use Search** with indexed fields for optimal performance
3. **Refresh Indexes** after bulk data changes

---

## Best Practices and Gotchas

### Pointer Management
- Always use `PtrToRec` when working with pointer types
- Validate pointer validity before dereferencing
- Handle pointer-to-record conversion errors gracefully

### Index Considerations
- Indexes store pointers, not data — they're space-efficient
- Multi-field indexes require `CompareSearchKey` logic
- Index updates are automatic but can be expensive for bulk operations

### Comparison Limitations
- **Current Issue:** `CompareSearchKey` uses `strcmp` for all field comparisons
- **Impact:** Incorrect results for numeric, date, and other non-string types
- **Priority:** High — affects query correctness
- **Solution:** Refactor to use `DoComparison` for type-aware comparisons

### Performance Considerations
- Index lookups require pointer dereferencing overhead
- Multi-field searches are more expensive than single-field
- Bulk operations should disable indexes temporarily
- Use appropriate node widths for BTree performance

### Error Handling
- Always check return values from `PtrToRec` operations
- Handle index update failures gracefully
- Validate cursor state before operations
- Use debug methods (`DumpFile`, `DumpTree`) for troubleshooting

---

## Performance Architecture

### Caching System
- **GrapaFileCache**: Multi-level caching with LRU eviction
  - Configurable cache sizes (`DEFAULT_SIZE=(1024*8*4)`)
  - Block-based caching (`BLOCKSIZE=32`, `BLOCKSPERPAGE=8`)
  - Thread-safe cache management
- **Block-based Caching**: Configurable cache sizes and block management
- **Thread-safe Caching**: Concurrent access support with proper synchronization

### Compression and Encryption
- **ZIP Compression**: Built-in compression for data storage (`ENCODE_ZIP`)
- **AES Encryption**: Optional encryption for sensitive data (`ENCODE_AES`)
- **Compression Size**: Configurable compression block sizes
- **Combined Encoding**: Support for both compression and encryption simultaneously

### Optimization Framework
- **Constant Folding**: Pre-compute constant expressions during compilation
- **Structure Flattening**: Combine nested structures when possible
- **Pattern Caching**: Cache compiled patterns and expressions for reuse
- **Background Computation**: Compute expensive operations in background threads

### Performance Optimization Details
- **Index Compilation Caching**: Compiled patterns cached for reuse
- **PCRE2 JIT Compilation**: Just-In-Time compilation for fast pattern matching
- **Fast Path Expansions**: Optimized paths for simple literal, word, and digit patterns
- **LRU Cache Management**: Thread-safe LRU cache for text normalization

---

## Storage Architecture

### Data Types and Storage Methods
- **BYTE_DATA**: Contiguous data storage for small/fixed data
  - Single data block per value
  - Direct storage without fragmentation
  - Suitable for most small-to-medium values
- **FREC_DATA**: Fragmented data storage with weighted BTree
  - Uses weighted BTree for efficient fragment management
  - Supports very large data items by breaking into manageable pieces
  - Enables O(log n) access, insert, and delete operations for large data
  - Inspired by file system page-linking for efficient handling of sparse data

### Storage Types
- **STORE_FIX**: Fixed-size storage for predictable data sizes
- **STORE_VAR**: Variable-size storage for dynamic data
- **STORE_PAR**: Partial storage for large data requiring incremental updates

### Data Block Management
- **Dynamic Allocation**: Allocate storage with configurable growth parameters
- **Fragmentation Handling**: Efficient handling of large/fragmented data
- **Automatic Cleanup**: Clean up orphaned data blocks automatically
- **Weight-based Navigation**: Use weights for efficient fragment location in FREC_DATA

### Column Store Optimization
- **Fragmented Data Storage**: Uses FREC_DATA for efficient handling of sparse data
- **Column-Oriented**: Each field stored as separate column
- **Dynamic Growth**: Columns grow incrementally as new records added
- **Sparse Data Support**: Only stores data where it actually exists

---

## Transaction Safety and Consistency

### Dirty Bit Management
- **SetTreeDirty/GetTreeDirty**: Track transaction state across tree operations
- **Atomic Operations**: Multi-step operations with rollback capability
- **Consistency Guarantees**: Maintain data integrity during complex operations

### Index Synchronization
- **Automatic Updates**: Indexes automatically updated when records change
- **Pointer Validation**: `PtrToRec` ensures pointer validity before dereferencing
- **Transaction Wrapping**: Index operations wrapped in atomic transactions
- **Rollback Capability**: Support for transaction rollback on failure

### Field Change Propagation
- **Index Dependency Checks**: Validate field usage before deletion
- **Data Migration**: Sophisticated handling of schema changes across all table types
- **Atomic Field Operations**: Field changes are atomic across the entire table
- **Consistency Maintenance**: Ensure all indexes remain consistent during field changes

### Implementation Details
```cpp
// Example: Dirty bit management during field operations
indexCursor.Set(pTable.mRecRef);
err = SetTreeDirty(indexCursor, true);  // Mark as dirty during operation

// Perform field operations...

indexCursor.Set(pTable.mRecRef);
err = SetTreeDirty(indexCursor, false); // Clear dirty bit after completion
```

---

## Thread Safety and Concurrency

### Built-in Thread Safety
- **Internal Synchronization**: All variable and data structure updates internally synchronized at C++ level
- **No Crashes**: Never encounter crashes or corruption from concurrent access
- **Logical Race Conditions**: May see logical race conditions (unexpected values, overwrites) but no stability issues
- **Design Consideration**: Minimize shared mutable state between threads unless intentional

### Lock Objects
- **`$thread()` Objects**: Only `$thread()` objects provide explicit locking via `lock()`, `unlock()`, and `trylock()`
- **Resource Protection**: Use `$thread()` lock objects to guard access to shared resources
- **Error Prevention**: Calling `.lock()` or `.unlock()` on regular variables returns an error

### Cache Thread Safety
- **Thread-safe Caching**: All caching mechanisms are thread-safe
- **Concurrent Access**: Multiple threads can safely access cached data
- **LRU Cache Management**: Thread-safe LRU cache for text normalization and pattern compilation

### Best Practices for Parallelism
- **Prefer Value Passing**: Pass data by value or use thread-local variables
- **Minimize Shared State**: Minimize shared mutable state between threads
- **Use Lock Objects**: If sharing is necessary, use `$thread()` lock objects to guard access
- **Design for Concurrency**: Design applications with thread safety in mind from the start

---

## Debugging and Forensics

### Comprehensive Dump System
- **DumpFile**: Dump entire database structure for debugging
- **DumpTree**: Dump specific tree structures with detailed information
- **Structure Visualization**: Visual representation of BTree structure and relationships
- **Debug Output**: Detailed debug information for troubleshooting

### Manual File Traversal
- **Low-level Analysis**: Tools for inspecting individual blocks and nodes
- **File Structure Analysis**: Manual traversal of BTree file structure
- **Block-level Debugging**: Ability to inspect individual blocks and nodes
- **Forensic Capabilities**: Tools for data recovery and corruption analysis

### Debug Information Available
- **Tree Structure**: Complete tree hierarchy and relationships
- **Data Distribution**: Information about data distribution and storage efficiency
- **Index Information**: Details about index structure and performance
- **Storage Statistics**: Information about storage usage and efficiency

### Debugging Examples
```cpp
// Dump entire database structure
GrapaFile dumpFile;
dumpFile.Create("debug_dump.txt");
db.DumpFile(&dumpFile);

// Dump specific tree
db.DumpTree(treeRef, &dumpFile);
```

---

## Known Issues and Limitations

### CompareSearchKey Field Comparison
- **Issue**: Uses `strcmp` for all field comparisons
- **Impact**: Incorrect results for numeric, date, and other non-string types
- **Priority**: High — affects query correctness
- **Solution**: Refactor to use `DoComparison` for type-aware comparisons

### Index Consistency
- **Issue**: Complex scenarios may lead to index inconsistencies
- **Impact**: Incorrect search results or data corruption
- **Priority**: High — affects data integrity
- **Solution**: Implement index validation and repair mechanisms

### File Fragmentation Limitation
- **Issue**: File size may not reduce even when all data is deleted
- **Root Cause**: Hole management system limitations with end-of-file blocks
- **Impact**: Storage waste when files are frequently created and deleted
- **Current Status**: No built-in defragmentation or file transfer utilities
- **Workaround**: Manual file recreation or external file system tools
- **Future Enhancement**: Implement `DefragmentFile()` and `TransferToNewFile()` utilities

### Pointer Dereferencing Performance
- **Issue**: Each index lookup requires pointer dereferencing overhead
- **Impact**: Performance overhead for complex queries
- **Priority**: Medium — affects query performance
- **Solution**: Consider caching strategies for frequently accessed records

### ROW Table Index Corruption (Historical)
- **Issue**: Historical bug where index entries became corrupted after field updates
- **Status**: Investigation completed, root cause identified
- **Impact**: Affected both ROW and COL tables
- **Resolution**: Fixes implemented, but monitoring recommended
- **Documentation**: Full investigation details in `ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md`

### Open Issues for Future Work
- **Type-Aware Comparisons**: Implement proper type-aware field comparisons
- **Index Validation**: Add automatic index validation and repair mechanisms
- **Defragmentation**: Implement file defragmentation utilities
- **Performance Monitoring**: Add performance monitoring and optimization tools
- **Advanced Caching**: Implement more sophisticated caching strategies

---

## Reference and Ongoing Documentation
- This summary and all future details are maintained in this file.
- As new edge cases, bugs, or usage patterns are discovered, update this doc to keep it as the canonical reference.
- Cross-reference with `GRAPA_BTREE_IMPLEMENTATION.md` for BTree-specific details.
- Update the investigation document (`archive/ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md`) with any findings related to the historical bug investigation (now closed).

---

## Database Architecture Overview

### Core Design Philosophy
GrapaDB extends GrapaBtree to provide a complete database management system with the following key architectural principles:

1. **Extensible Comparison System**: The core BTree comparison logic is extensible through virtual method overrides, allowing specialized comparison behavior for different data types and pointer dereferencing scenarios.

2. **Pointer-Based Indexing**: Indexes store pointers to actual record data rather than duplicating the data, enabling efficient storage and complex multi-field indexing.

3. **Type-Aware Operations**: All database operations are type-aware, with proper handling of different data types (CHAR, INT, FLOAT, etc.) during comparisons and storage.

4. **Layered Architecture**: Clear separation between BTree operations, database management, and record handling layers.

### Component Architecture

#### 1. Core BTree Layer (GrapaBtree)
- **Purpose**: Provides the fundamental BTree data structure for efficient key-value storage and retrieval
- **Key Features**:
  - Configurable node width for performance tuning
  - Automatic node splitting and merging
  - Extensible comparison system via virtual methods
  - Support for different value types (SU64_ITEM, TREE_ITEM, etc.)

#### 2. Database Management Layer (GrapaDB)
- **Purpose**: Extends BTree functionality with database-specific operations
- **Key Features**:
  - Table and index management
  - Record creation and deletion
  - Field management with type awareness
  - Pointer dereferencing for complex data types

#### 3. Record Management Layer
- **Purpose**: Handles individual record operations and field management
- **Components**:
  - `GrapaDBTable`: Represents database tables
  - `GrapaDBField`: Defines field structure and metadata
  - `GrapaDBIndex`: Manages indexes on table fields
  - `GrapaCursor`: Provides navigation and operation context

### Data Flow Architecture

#### 1. Record Insertion Flow
```
User Request → CreateRecord() → Insert() → CompareKey() → BTree Insert
                                    ↓
                              InsertIntoIndex() → Index BTree Insert
```

#### 2. Record Retrieval Flow
```
User Query → SearchDb() → CompareKey() → BTree Search → PtrToRec() → Record Data
```

#### 3. Index Operations Flow
```
Index Query → CompareSearchKey() → Multi-field Comparison → BTree Search → Results
```

### Performance Architecture

#### Optimization Strategies
1. **Node Width Tuning**: Configurable BTree node width for performance vs. memory trade-offs
2. **Lazy Loading**: Pointers are dereferenced only when needed
3. **Batch Operations**: Multiple operations can be batched for efficiency
4. **Index Caching**: Frequently accessed indexes can be cached

#### Memory Management
- **Dynamic Allocation**: Records and fields are allocated dynamically
- **Garbage Collection**: Unused records and indexes are cleaned up
- **Memory Pooling**: Efficient memory reuse for similar operations

### Error Handling Architecture

#### Error Propagation
Errors are propagated through the call stack using return codes:
```
Operation → Error Check → Return Code → Caller → Error Handling
```

#### Common Error Patterns
- **File I/O Errors**: Database file creation, opening, or writing issues
- **Memory Errors**: Insufficient memory for operations
- **Data Errors**: Invalid data types or corrupted records
- **Index Errors**: Index corruption or invalid pointer references

### Security Architecture

#### Data Integrity
1. **Transaction Support**: Atomic operations for data consistency
2. **Index Validation**: Automatic index integrity checking
3. **Corruption Detection**: Detection and handling of corrupted data

#### Access Control
1. **File-Level Security**: Database file access control
2. **Record-Level Security**: Individual record access permissions
3. **Index Security**: Index access control and validation

### Extension Architecture

#### Plugin System
The architecture supports extensions through:
1. **Virtual Method Overrides**: Custom comparison and operation behavior
2. **Custom Field Types**: User-defined data types with custom comparison logic
3. **Custom Index Types**: Specialized indexing for specific use cases

#### Integration Points
- **BTree Extension**: Custom BTree behavior through inheritance
- **Comparison Extension**: Custom comparison logic through virtual methods
- **Storage Extension**: Custom storage backends through file system abstraction

### Debugging and Monitoring Architecture

#### Debug Output System
The system includes comprehensive debug output for investigation:
```
```

## [2025-07-22] Index Entry Value Handling and ROW Table Bug Resolution

- When inserting index entries (RPTR_ITEM, CPTR_ITEM, GPTR_ITEM) during record creation or field update, always set the value to the actual record reference (recCursor.mValue).
- Never use 0 or the key as the value for these index entries, as this leads to index corruption and data loss after multiple inserts.
- This rule was established and validated during the investigation and resolution of the ROW table index corruption bug (see investigation docs for details).
- The implementation now comments out the old logic and uses the correct value assignment in both CreateRecord and SetRecordField.

## [2025-07-24] Index Creation and Exposure: Default $KEY Index and Custom Indexes

- **Default Index:** Every GrapaDB table automatically receives a default index on the $KEY field (fieldId=4) at creation. This index is required for correct table operation and is always present.
- **Custom Indexes:** Additional (user-defined) indexes, including multi-field indexes, can be created via the C++ API using `CreateIndex` (to create the index structure) and `CreateIndexField` (to associate one or more fields with the index). The index field list is stored in the index's field mapping tree, and multi-field indexes are supported by adding multiple fields to the same index.
- **Language/CLI Exposure:** As of this update, there are no Grapa language or CLI commands to create or manage custom indexes; this functionality is only available via the C++ API. This is now a documented backlog item (see BACKLOG.md) to expose user-defined index creation and management to the Grapa language and CLI.
- **Index Structure:** Each index maintains a mapping of index field IDs, and the comparison/search logic supports multi-field indexes via the `CompareSearchKey` method. Indexes store pointers (not data) and are updated automatically on record changes.
