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

## Known Issues and Limitations

### CompareSearchKey Field Comparison
- **Issue:** Uses `strcmp` for all field comparisons
- **Impact:** Incorrect results for numeric, date, and other non-string types
- **Priority:** High — affects query correctness
- **Solution:** Refactor to use `DoComparison` for type-aware comparisons

### Index Consistency
- **Issue:** Complex scenarios may lead to index inconsistencies
- **Impact:** Incorrect search results or data corruption
- **Priority:** High — affects data integrity
- **Solution:** Implement index validation and repair mechanisms

### Pointer Dereferencing Performance
- **Issue:** Each index lookup requires pointer dereferencing
- **Impact:** Performance overhead for complex queries
- **Priority:** Medium — affects query performance
- **Solution:** Consider caching strategies for frequently accessed records

### Open Issues
- **ROW and COL Table Index Corruption:**
  - There is a known issue where index entries may become corrupted after field updates, affecting both ROW and COL tables.
  - For full investigation details, root cause analysis, and debugging history, see:
    - [ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md](../DEVELOPMENT/ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md)
- **CompareSearchKey Field Comparison:**
  - Uses `strcmp` for all field comparisons; see investigation doc for impact and future work.
- **Index Consistency:**
  - Complex scenarios may lead to index inconsistencies; see investigation doc for details.

---

## Reference and Ongoing Documentation
- This summary and all future details are maintained in this file.
- As new edge cases, bugs, or usage patterns are discovered, update this doc to keep it as the canonical reference.
- Cross-reference with `GRAPA_BTREE_IMPLEMENTATION.md` for BTree-specific details.
- Update the investigation document (`ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md`) with any findings related to the current bug investigation.

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
```cpp
#define DEBUG_BTREE_INVESTIGATION 1
#define DEBUG_COMPARE_KEY 1
#define DEBUG_INDEX_OPERATIONS 1
```

#### Monitoring Points
1. **Comparison Operations**: Track all key comparison operations
2. **Pointer Dereferencing**: Monitor pointer resolution and record access
3. **Index Operations**: Track index insertions, deletions, and searches
4. **Error Conditions**: Monitor and log error conditions

### Future Architecture Considerations

#### Scalability Improvements
1. **Distributed Indexing**: Support for distributed index operations
2. **Parallel Processing**: Concurrent index operations for improved performance
3. **Memory-Mapped Files**: Efficient large database handling

#### Feature Extensions
1. **Advanced Query Language**: SQL-like query support
2. **Full-Text Search**: Text indexing and search capabilities
3. **Temporal Data**: Support for time-series data and historical queries 