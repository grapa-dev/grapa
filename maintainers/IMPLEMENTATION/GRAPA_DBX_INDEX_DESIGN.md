# GrapaDBX Index Design

## Overview

GrapaDBX uses a unified index design that follows the same principles as the field dictionary system. Indexes are stored using the same BTree patterns and field structures, ensuring consistency across the entire database system.

## Index Structure

### Core Index Class
```cpp
class GrapaDBXIndex
{
public:
    GrapaDBXTable mTable;  // Contains dictionary field for index metadata
    u64 mId;               // Index ID
    u64 mRef;              // Reference to the index BTree
    // add the name
public:
    GrapaDBXIndex() { mId = 0; mRef = 0; }
    void BigEndian();
};
```

### Index Table Integration
```cpp
class GrapaDBXTable
{
public:
    GrapaDBXField mDictField;  // Dictionary field for table/index metadata
    u64 mId;
    u64 mRef;
    u64 mRecRef;
    u8 mRefType;
};
```

## Index Definition Storage

### Dictionary Field Approach
Index definitions are stored using the same unified field structure as regular fields:

```cpp
// Index definition stored in mTable.mDictField
struct IndexDefinition {
    u64 indexId;           // Unique index identifier
    u64 tableRef;          // Reference to parent table
    u64 indexTreeRef;      // Reference to index BTree
    u8 indexType;          // Index type (single, composite, etc.)
    u8 storageType;        // Storage strategy
    // Field mappings and metadata
};
```

### Field Mapping System
Indexes use a field mapping system to define which table fields are indexed:

```cpp
// Field mapping: (indexFieldId, fieldId) pairs
struct IndexFieldMapping {
    u64 indexFieldId;      // Position in index (0, 1, 2, ...)
    u64 fieldId;           // Reference to table field
    u8 sortOrder;          // ASC/DESC
    u8 dataType;           // Field data type for comparison
};
```

## Index Data Storage

### BTree Integration
Indexes use the same BTree system as the main database:

```cpp
// Index tree structure
- Index Tree (mRef): Contains actual indexed values
  ├── Field Mappings: SU64_ITEM pairs (indexFieldId, fieldId)
  ├── Index Entries: Pointer items (GPTR_ITEM, RPTR_ITEM, CPTR_ITEM)
  └── Metadata: Index configuration and statistics
```

### Index Entry Types
```cpp
enum {
    GPTR_ITEM,  // Group pointer items
    RPTR_ITEM,  // Row pointer items  
    CPTR_ITEM,  // Column pointer items
    // ... other item types
};
```

## Index Operations

### Creation Process
1. **Create Index Tree**: Allocate new BTree for index data
2. **Store Field Mappings**: Save (indexFieldId, fieldId) pairs
3. **Build Index**: Scan table and populate index entries
4. **Update Dictionary**: Store index definition in table dictionary

### Field Mapping
```cpp
GrapaError CreateIndex(GrapaDBXTable& pTable, u64 pIndexId, 
                      GrapaDU64Array& pIndexList, GrapaDBXIndex& pIndex)
{
    // pIndexList contains (indexFieldId, fieldId) pairs
    // Each pair defines which table field is indexed at which position
}
```

### Index Maintenance
- **Insert**: Add new entries to index when records are inserted
- **Update**: Modify index entries when records are updated
- **Delete**: Remove index entries when records are deleted
- **Refresh**: Rebuild entire index (used for bulk operations)

## Design Principles

### 1. Unified Structure
- Same `GrapaDBXField` structure for index definitions
- Same BTree patterns for index storage
- Same endian safety and I/O patterns

### 2. Flexible Mapping
- Can index any combination of fields
- Supports different sort orders per field
- Extensible for future index types

### 3. BTree Integration
- Leverages proven BTree system
- Consistent performance characteristics
- Same memory management and storage patterns

### 4. Dictionary Integration
- Index definitions stored in table dictionary
- Consistent metadata management
- Easy to discover and manage indexes

## Index Types Supported

### 1. Single Field Index
- Index on one table field
- Fastest for simple queries

### 2. Composite Index
- Index on multiple fields
- Supports complex query patterns
- Field order matters for performance

### 3. Unique Index
- Ensures no duplicate values
- Can be single or composite

### 4. Partial Index
- Index on subset of records
- Uses filter conditions

## Performance Characteristics

### Index Selection
- Automatic index selection based on query fields
- Cost-based optimization (future enhancement)
- Fallback to sequential scan when no suitable index

### Index Maintenance
- Incremental updates for insert/update/delete
- Bulk operations use refresh for efficiency
- Background maintenance for large indexes

## Current Implementation Status

### ✅ Implemented
- Index structure definitions
- Basic index creation framework
- Endian safety for index structures
- Dictionary integration pattern

### ❌ Not Implemented
- Actual index creation logic
- Index maintenance operations
- Index-based search optimization
- Index statistics and metadata

## Future Enhancements

### 1. Index Statistics
- Track index usage patterns
- Automatic index recommendations
- Performance monitoring

### 2. Advanced Index Types
- Full-text search indexes
- Spatial indexes
- Hash indexes for equality queries

### 3. Index Optimization
- Automatic index selection
- Query plan optimization
- Index hint system

---

*Last updated: Current session - Documented GrapaDBX index design based on codebase analysis* 