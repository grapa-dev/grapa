# Grapa Database Architecture Reference

## Overview

This document captures key architectural insights discovered during deep analysis of the Grapa database implementation. It serves as a reference for maintainers working on database-related features, debugging, and performance optimization.

## Core Architecture

### BTree Foundation

Grapa's database is built on a solid BTree foundation with the following characteristics:

- **Node Width**: `NODE_WIDTH=5` provides optimal balance between memory usage and tree depth
- **Weight-Balanced**: Implements weight-based balancing for optimal performance
- **Block-Oriented**: Uses fixed-size blocks (32 bytes) for predictable I/O patterns
- **Unified Interface**: Same BTree API works for both disk and memory storage

### Storage Types

#### BYTE_DATA (Row-Oriented)
- **Purpose**: Contiguous storage for row-oriented data
- **Use Case**: RTABLE_TREE (Row Store)
- **Characteristics**: 
  - Records stored contiguously
  - Fast record retrieval and updates
  - Efficient for point queries on specific records

#### FREC_DATA (Column-Oriented)
- **Purpose**: Fragmented data storage for column-oriented data
- **Use Case**: CTABLE_TREE (Column Store)
- **Characteristics**:
  - Tree-based fragmentation using BDATA_TREE
  - Dynamic growth with configurable block sizes
  - Sparse data efficiency (only allocates when needed)
  - Independent field operations

## Database Types

### Row Store (RTABLE_TREE)
- **Storage Model**: BYTE_DATA for contiguous record storage
- **Access Pattern**: Horizontal (all fields of a record)
- **Best For**: OLTP applications, frequent record updates, point queries
- **Index Structure**: RPTR items point to RREC items

### Column Store (CTABLE_TREE)
- **Storage Model**: FREC_DATA for fragmented field storage
- **Access Pattern**: Vertical (all values of a field)
- **Best For**: Analytical queries, data warehousing, large datasets
- **Index Structure**: CPTR items point to CREC items

### Group Store (GROUP_TREE)
- **Storage Model**: Hierarchical structures
- **Use Case**: File system directories, nested data organization
- **Index Structure**: GPTR items point to GREC items

## Indexing System

### Index Architecture
- **Reference-Based**: Indexes store record references, not actual data
- **String Comparison**: Uses `strcmp()` for string key comparisons
- **Field ID 5**: `$KEY` field is hardcoded as system field for indexing
- **Index Corruption Issue**: Current bug affects first record's index entry when third record is added

### Index Update Process
```cpp
// In SetRecordField (GrapaDB.cpp lines ~1490-1510)
// 1. Delete old index entries for the field being updated
// 2. Insert new index entries with updated values
// 3. Index key = record reference (recCursor.mKey)
// 4. Index value = field value hash or reference
```

## Debug System

### Debug Function Implementation
The `debug()` function provides detailed database structure information:

- **Entry Point**: `DumpTheTree()` in GrapaDB.cpp
- **RREC Output**: `DumpTheRowRec()` shows actual record data
- **RPTR Output**: `DumpThePointer()` shows index entries
- **Field Data**: `DumpTheStructure()` calls `GetRecordField()` to extract field values

### Debug Output Format
```
RREC (63) key=1 node=(66,0) weight=3: 1=1 2=hello1 3=test 4=42 5=test1
RPTR (0) key=1 node=(104,0) weight=3: 1=1 2=hello1 3=test 4=42 5=test1
```

**Key Insight**: Empty RPTR entries (no field data after colon) indicate `GetRecordField()` failures, which explains why `tbl.get()` also fails.

## Unified Storage Architecture

### Single File Design
- **Everything in One File**: Data, indexes, metadata all stored in single file
- **Benefits**: Atomicity, simplicity, portability, easy backup/restore
- **Implementation**: `GrapaFile` handles all storage operations

### Memory + Disk Unification
- **Same API**: BTree interface works identically for memory and disk
- **Code Reuse**: Same logic for both storage types
- **Development Friendly**: Memory storage for fast iteration

### Caching Layer
- **Problem**: Large datasets consume too much RAM
- **Solution**: Caching wrapper handles memory management
- **Features**: LRU/LFU eviction, block-level caching, memory limits, disk spillover

## FREC_DATA Deep Dive

### Purpose
FREC_DATA was specifically designed for column store architecture to solve the fundamental storage paradigm shift:

- **Row Store**: "Store this record with all its fields"
- **Column Store**: "Store this field value for this record position"

### Implementation Details
```cpp
// Tree-based fragmentation
return NewTree(data.dataStart, BDATA_TREE, data.parentTree);

// Dynamic growth - no pre-allocation
data.dataSize = 0;
data.dataLength = 0;

// Intelligent block management
size = data.growSize < nullSize ? data.growSize : nullSize;
err = NewData(BYTE_DATA, data.parentTree, data.growSize, 
              data.growBlockSize, 
              GrapaMem::Blocks(data.growSize, data.growBlockSize), 
              newPtr, true);
```

### Key Features
- **Tree-based indexing** for fast record position lookups
- **Fragmented storage** for efficient sparse data
- **Dynamic growth** as new records are added
- **Independent field management** for optimal performance

## Known Issues

### Index Corruption Bug - CRITICAL
- **Symptoms**: First record's index entry becomes empty when third record is added
- **Affects**: ROW tables only (COL tables work correctly)
- **Root Cause**: Index update logic in `SetRecordField()` 
- **Location**: `source/grapa/GrapaDB.cpp` lines ~1490-1510
- **Status**: Under investigation

**🚨 CRITICAL WORKAROUND: Use COL tables until this bug is fixed**
- **ROW tables**: Currently affected by index corruption bug
- **COL tables**: Work correctly and provide same functionality
- **Recommendation**: Use `table("COL")` instead of `table("ROW")` for all new development
- **Migration**: Existing ROW tables should be migrated to COL tables

### Debug Output Analysis
When debugging index issues, look for:
- **Empty RPTR entries**: `RPTR (0) key=1 node=(104,0) weight=3:` (no field data)
- **Data vs Index mismatch**: RREC has data but RPTR is empty
- **Pattern**: Issue consistently affects first record created

## Performance Characteristics

### Row Store Performance
- **Fast**: Record retrieval and updates
- **Efficient**: Point queries on specific records
- **Good**: Transactional workloads

### Column Store Performance
- **Fast**: Column scans and aggregations
- **Efficient**: Analytical queries across columns
- **Better**: Compression for similar data types
- **Optimized**: Sparse data storage

## Best Practices

### Field Creation
```cpp
// Integer fields
table.mkfield("field", "INT");

// String fields - variable length
table.mkfield("field", "STR", "VAR");

// String fields - fixed length
table.mkfield("field", "STR", "FIX", 256);

// Float fields
table.mkfield("field", "FLOAT", "FIX", 8);

// Time fields
table.mkfield("field", "TIME", "FIX", 8);
```

### Storage Type Selection
- **Use ROW for**: Frequent updates, point queries, small-medium datasets
- **Use COL for**: Analytical queries, large datasets, sparse data
- **Use GROUP for**: Hierarchical data, file system directories

**⚠️ IMPORTANT: Due to current index corruption bug in ROW tables, use COL tables for all new development**

### Debugging Database Issues
1. **Use `tbl.debug()`** to examine structure
2. **Look for empty RPTR entries** as indicators of index corruption
3. **Compare RREC vs RPTR** data to identify mismatches
4. **Test with COL tables** to isolate ROW-specific issues

## Future Enhancements

### Transaction System
```cpp
// TODO: Complete transaction support
virtual GrapaError OpenTransaction();
virtual GrapaError CommitTransaction(); 
virtual GrapaError CancelTransaction();
```

### Index Optimization
```cpp
// TODO: Add index disable/enable for bulk operations
virtual GrapaError DisableIndexes();
virtual GrapaError RefreshIndexes();
```

### Large BLOB Support
- **Status**: Implemented via FREC_DATA
- **Usage**: Column store fixed fields, large data requiring partial updates
- **Features**: Tree-based fragmentation, dynamic growth, efficient sparse data

## Conclusion

Grapa's database architecture is fundamentally sound and well-designed. The unified storage approach with intelligent caching, the sophisticated FREC_DATA implementation for column stores, and the solid BTree foundation provide a robust foundation for enterprise-level applications.

The current index corruption issue is an implementation bug, not an architectural problem. Once resolved, the system will provide excellent performance and reliability for various workloads. 