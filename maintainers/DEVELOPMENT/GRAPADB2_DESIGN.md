# GrapaDB2 Design Document: Database File System

## Vision and Inspiration

GrapaDB2 implements a **Database File System** - a unified storage system that combines file system and database capabilities seamlessly. This vision was inspired by Microsoft's failed Longhorn/WinFS project, which attempted to create a relational file system but failed due to complexity and performance issues.

### The WinFS Problem and Solution

**What Microsoft Attempted (WinFS):**
- Relational file system with complex integration
- Failed due to over-engineering and performance issues
- Never delivered on the promise of unified storage

**What GrapaDB2 Delivers:**
- A working database file system that actually functions
- Hierarchical organization (folders) with database power (tables)
- Seamless navigation between file system and database operations
- Efficient BTree-based storage with hole management

## Core Philosophy

GrapaDB2 treats the database as a file system where:
- **GROUP** = **Folders/Directories** (can contain other folders, tables, and data files)
- **ROW/COL** = **Tables** (structured data storage like spreadsheets)  
- **DATA/FREC** = **Files** (actual data content)
- **BTree** = **Memory Management System** (handles storage, holes, allocation)

## Universal Default Fields

**All database types (ROW, COL, GROUP) have two universal default fields:**

1. **`$KEY`** (if enabled): The primary key field for record identification
   - Type: `STR` (GrapaTokenType::STR)
   - Storage: `FIX` (GrapaDBField::STORE_FIX)
   - Size: 256 bytes
   - **Auto-indexed**: Automatically gets an index (NAME_INDEX_ID) for fast searching
   - **Used for record lookup and indexing**: Primary search mechanism

2. **`$VALUE`** (always available): The default data field
   - Type: `RAW` (GrapaTokenType::RAW) 
   - Storage: `VAR` (GrapaDBField::STORE_VAR)
   - Initial size: 32 bytes, grows by 8 bytes
   - **Auto-created**: This field is automatically created when first accessed
   - **Default behavior**: If no specific field is specified, `$VALUE` is used
   - **Universal**: This applies to ROW, COL, and GROUP databases

**Implementation**: When no field name is provided in `get()` or `set()` operations, the system automatically defaults to `$VALUE`:
```cpp
GrapaCHAR field(pField);
if (field.mLength == 0) field.FROM("$VALUE");
```

## Indexing System

### Automatic `$KEY` Indexing
When a `$KEY` field is created, it automatically gets an index for fast searching:

```cpp
// Automatic index creation for $KEY field
GrapaDBIndex dbIndexName;
GrapaDU64Array catIndexList(1);
catIndexList.Append(NAME_INDEX_FIELD_NAME_ID, nameId);
err = CreateIndex(parentDict, NAME_INDEX_ID, catIndexList, dbIndexName);
```

### Custom Field Indexing
Users can create additional indexes on any field for custom search requirements:

```cpp
// Create custom index on any field
GrapaDBIndex customIndex;
GrapaDU64Array fieldList(1);
fieldList.Append(1, customFieldId);  // Field ID for custom field
err = CreateIndex(table, customIndexId, fieldList, customIndex);
```

### Search Priority
1. **`$KEY` index**: Always available for primary key searches (fastest)
2. **Custom indexes**: Available for specific field searches (user-defined)
3. **Sequential scan**: Fallback when no suitable index exists (slowest)

### Two-Stage Search Algorithm
GrapaDB implements a sophisticated two-stage search process:

#### Stage 1: Index Selection and Range Constraint
1. **Locate Best Index**: Scan through available indexes to find the best match for the search fields
   - Usually defaults to `$KEY` index (NAME_INDEX_ID) for primary key searches
   - For custom field searches, finds the most suitable index that covers the search criteria
   - Uses `LocateIndex()` to find indexes that contain the specified field IDs

2. **Constrain Cursor to Index Range**: 
   - Set cursor to use the selected index (`cursor.SetSearch()` with `usingIndex=true`)
   - This provides a sequential range (start and end) within the index
   - The index narrows down the search space significantly

#### Stage 2: Detailed Field Matching
Within the constrained range from Stage 1:
1. **Individual Field Matching**: Compare each search field against all fields in the candidate records
2. **Multi-Field Comparison**: Use `CompareSearchKey()` for complex multi-field searches
3. **Record Validation**: Ensure all search criteria are satisfied before returning results

#### Implementation Flow
```cpp
// Stage 1: Find best index
field = pFieldList.GetFieldAt(0);  // Get first search field
err = LocateIndex(indexCursor, indexRef, field->mId);  // Find suitable index
if (!err) {
    usingIndex = true;
    cursor.SetSearch(this, indexCursor.mValue, usingIndex, &pFieldList);
}

// Stage 2: Search within index range
if (usingIndex) {
    err = Search(cursor);  // Use index for range constraint
    err = FirstDb(cursor); // Find first matching record
} else {
    // Fallback to sequential scan
    cursor.SetSearch(this, pTable.mRecRef, usingIndex, &pFieldList);
    err = First(cursor);
    err = FirstDb(cursor);
}
```

This design provides optimal performance by leveraging indexes for range selection while ensuring accurate field-level matching within those ranges.

## Field Definition Management

GrapaDB provides comprehensive field definition management with sophisticated propagation handling:

### Field Operations
1. **Add Fields**: `CreateTableField()` - Add new fields to tables
2. **Delete Fields**: `DeleteTableField()` - Remove fields from tables  
3. **Modify Fields**: Update field definitions (type, size, storage)
4. **List Fields**: `OpenTableFieldList()` - Get all fields in a table

### Index Dependency Management
**Critical Constraint**: Fields cannot be deleted if they are used in any indexes
- **Validation**: Before deletion, scan all indexes to check field usage
- **Error Handling**: Return error if field is referenced by any index
- **User Guidance**: Must drop dependent indexes before field deletion

### Data Propagation During Field Changes

#### Field Deletion Process
1. **Index Validation**: Check if field is used in any indexes (fail if used)
2. **Dictionary Update**: Remove field from table dictionary
3. **Offset Adjustment**: Update all remaining field offsets
4. **Data Migration**: Propagate changes through all records:
   - **ROW Tables**: Bit-shift all record data to remove deleted field
   - **COL Tables**: Delete field's column tree/storage
   - **GROUP Tables**: Handle nested field structures

#### Field Addition Process
1. **Dictionary Update**: Add field to table dictionary
2. **Storage Allocation**: Create appropriate storage for field type
3. **Index Integration**: Update indexes if field should be indexed
4. **Data Initialization**: Initialize new field in existing records

#### Field Modification Process
1. **Type Validation**: Ensure new type is compatible
2. **Storage Migration**: Convert data to new storage format
3. **Index Updates**: Rebuild affected indexes
4. **Data Consistency**: Maintain referential integrity

### Implementation Nuances

#### ROW Table Field Management
```cpp
// Bit-shift all records to remove deleted field
err = CopyDataValue(tableCursor.mValue, toOffset, tableCursor.mValue, fromOffset, moveSize, &bytesWriten);
```

#### COL Table Field Management
```cpp
// Delete field's column storage based on type and storage method
switch (fieldStore) {
    case STORE_VAR:
    case STORE_PAR:
        tableCursor.Set(storeTree, TREE_ITEM, pFieldId);
        err = Delete(tableCursor);
        break;
    case STORE_FIX:
        tableCursor.Set(storeTree, SDATA_ITEM, pFieldId);
        err = Delete(tableCursor);
        break;
}
```

#### Index Propagation
```cpp
// Check all indexes for field usage before deletion
while (!err) {
    if (indexFieldCursor.mValue == pFieldId)
        return(-1);  // Field used in index - cannot delete
    err = Next(indexFieldCursor);
}
```

### Transaction Safety
- **Dirty Bit Management**: Set tree dirty during field operations
- **Atomic Operations**: Field changes are atomic across the table
- **Rollback Capability**: Support for transaction rollback on failure
- **Consistency Guarantees**: Maintain data integrity during field changes

This sophisticated field management system ensures that schema changes are handled safely while maintaining data integrity and index consistency across all table types.

## Index Definition Management

GrapaDB uses a **delete-and-recreate approach** for index management, which is simpler and more reliable than complex index modification:

### Index Operations
1. **Create Index**: `CreateIndex()` - Create new index on specified fields
2. **Delete Index**: `DeleteIndex()` - Remove index completely
3. **Refresh Index**: `RefreshIndex()` - Rebuild index from scratch
4. **Add Index Field**: `CreateIndexField()` - Add field to existing index (rebuilds index)

### Index Modification Strategy
**Decision**: **Delete-and-Recreate Approach**
- **Rationale**: Simpler, more reliable, and avoids complex index modification logic
- **Process**: Delete old index → Create new index with updated definition
- **Benefits**: Eliminates complex index modification edge cases and data corruption risks

### Index Field Management
```cpp
// Adding a field to an existing index
GrapaError GrapaDB::CreateIndexField(GrapaDBIndex& pIndex, u64 pIndexFieldId, u64 pFieldId)
{
    // ... validation ...
    
    // Add field to index definition
    indexCursor.Set(indexFieldsRef, SU64_ITEM, pIndexFieldId, pFieldId);
    err = Insert(indexCursor);
    
    // Rebuild entire index with new field
    indexCursor.Set(pIndex.mRef, TREE_ITEM);
    EmptyTree(indexCursor);  // Clear existing index data
    dtCursor.Set(pIndex.mTable.mRef);
    err = First(dtCursor);
    while(!err) {
        // Rebuild index entries for all records
        indexCursor.Set(pIndex.mRef, RPTR_ITEM, dtCursor.mKey);
        Insert(indexCursor);
        err = Next(dtCursor);
    }
    return(0);
}
```

### Index Refresh Process
```cpp
// Complete index rebuild
GrapaError GrapaDB::RefreshIndex(GrapaDBIndex& pIndex)
{
    GrapaCursor indexCursor, dtCursor;
    indexCursor.Set(pIndex.mRef);
    EmptyTree(indexCursor);  // Clear all index data
    
    dtCursor.Set(pIndex.mTable.mRef);
    err = First(dtCursor);
    while(!err) {
        // Rebuild index entries for all records
        indexCursor.Set(pIndex.mRef, RPTR_ITEM, dtCursor.mKey, dtCursor.mValue);
        Insert(indexCursor);
        err = Next(dtCursor);
    }
    return(0);
}
```

### Index Modification Workflow
1. **Delete Index**: Remove existing index completely
2. **Create New Index**: Create index with updated field list
3. **Rebuild Data**: Index is automatically populated with all existing records
4. **Atomic Operation**: Index changes are atomic - either complete or not at all

### Advantages of Delete-and-Recreate
- **Simplicity**: No complex index modification logic to maintain
- **Reliability**: Eliminates edge cases in index modification
- **Consistency**: Ensures index is always in a consistent state
- **Performance**: Rebuilding is often faster than complex modifications
- **Debugging**: Easier to debug and verify index integrity

### When to Use Each Approach
- **Field Addition**: Use `CreateIndexField()` for adding fields to existing indexes
- **Field Removal**: Delete and recreate index with new field list
- **Index Type Changes**: Delete and recreate with new index type
- **Bulk Changes**: Delete and recreate for multiple field changes

### Implementation Considerations
- **Transaction Safety**: Index operations should be wrapped in transactions
- **Performance**: Large tables may require background index rebuilding
- **Availability**: Consider index rebuilding impact on query performance
- **Recovery**: Maintain ability to rollback index changes

This delete-and-recreate approach aligns with the "Database File System" philosophy where operations are simple, reliable, and atomic. It prioritizes data integrity and system reliability over complex optimization, which is appropriate for a database system that needs to be robust and maintainable.

## Future Enhancement: Computed Fields and Formula Indexes

**Design Decision**: Support both computed fields and formula indexes for maximum flexibility
- **Computed Fields**: Fields with formulas that combine other fields
- **Formula Indexes**: Indexes on computed values using Grapa lambda expressions
- **Implementation**: Grapa lambda expressions for both field and index computation
- **Use Cases**: Derived fields, computed values, complex search criteria, business logic
- **Timeline**: Future enhancement, not needed for initial implementation

### Computed Fields vs Formula Indexes

#### Computed Fields
- **Definition**: Fields that store computed values derived from other fields
- **Storage**: Values are computed and stored in the field
- **Updates**: Recalculated when dependent fields change
- **Use Case**: Frequently accessed computed values

#### Formula Indexes
- **Definition**: Indexes on computed values without storing the computed field
- **Storage**: Only the index stores computed values, not a separate field
- **Updates**: Recalculated when dependent fields change
- **Use Case**: Search optimization without storing redundant data

### Unified Formula System Design

#### Extended Field Structure
```cpp
// Extended field structure to support computed fields
struct GrapaDBField {
    // ... existing field properties ...
    
    // Computed field properties
    bool isComputed;                    // Is this a computed field?
    GrapaCHAR formulaCode;              // Grapa lambda expression for computation
    GrapaDU64Array dependentFields;     // Fields this formula depends on
    u64 lastComputedValue;              // Reference to last computed value
};
```

#### Extended Index Structure
```cpp
// Extended index structure to support formula indexes
struct GrapaDBIndexFormula {
    u64 formulaId;                      // Unique formula identifier
    GrapaCHAR formulaCode;              // Grapa lambda expression
    u64 resultFieldId;                  // Field ID for computed result (if stored)
    u8 resultType;                      // Data type of computed result
    u64 resultSize;                     // Size of computed result
    bool storeComputedField;            // Whether to store as computed field
    GrapaDU64Array dependentFields;     // Fields this formula depends on
};
```

#### Formula Evaluation Engine
```cpp
// Unified formula evaluation for both fields and indexes
GrapaError GrapaDB::EvaluateFormula(const GrapaCHAR& formulaCode, GrapaCursor& recordCursor, GrapaValue& result)
{
    // Parse and execute Grapa lambda expression
    GrapaValue lambdaResult;
    err = ExecuteGrapaLambda(formulaCode, recordCursor, lambdaResult);
    if (err) return(err);
    
    // Convert result to appropriate type
    err = ConvertToTargetType(lambdaResult, result);
    return(err);
}

// Evaluate formula for computed field
GrapaError GrapaDB::EvaluateComputedField(GrapaDBField& field, GrapaCursor& recordCursor, GrapaBYTE& result)
{
    GrapaValue computedValue;
    err = EvaluateFormula(field.formulaCode, recordCursor, computedValue);
    if (err) return(err);
    
    // Store computed value in field
    err = StoreFieldValue(field, computedValue, result);
    return(err);
}

// Evaluate formula for index
GrapaError GrapaDB::EvaluateIndexFormula(GrapaDBIndex& pIndex, GrapaCursor& recordCursor, GrapaBYTE& result)
{
    GrapaValue computedValue;
    err = EvaluateFormula(pIndex.mFormula.formulaCode, recordCursor, computedValue);
    if (err) return(err);
    
    // Convert result to index type
    err = ConvertToIndexType(computedValue, pIndex.mFormula.resultType, result);
    return(err);
}
```

### Formula Examples

#### Computed Fields (Stored Values)
```grapa
// Computed field: Full name (stored and accessible)
group_db.mkcomputedfield("full_name", "STR", "VAR", 
    "lambda(record) { record.get('first_name') + ' ' + record.get('last_name') }");

// Computed field: Age category (stored and accessible)
group_db.mkcomputedfield("age_category", "STR", "VAR", 
    "lambda(record) { 
        age = record.get('age').raw().int(); 
        if (age < 30) 'young' 
        else if (age < 50) 'middle' 
        else 'senior' 
    }");

// Computed field: Annual salary (stored and accessible)
group_db.mkcomputedfield("annual_salary", "INT", "FIX", 4,
    "lambda(record) { record.get('salary').raw().int() * 12 }");
```

#### Formula Indexes (Computed for Search Only)
```grapa
// Index on concatenated name (not stored as field)
index1 = group_db.mkformulaindex("full_name_index", 
    "lambda(record) { record.get('first_name') + ' ' + record.get('last_name') }");

// Index on age category (not stored as field)
index2 = group_db.mkformulaindex("age_category_index", 
    "lambda(record) { 
        age = record.get('age').raw().int(); 
        if (age < 30) 'young' 
        else if (age < 50) 'middle' 
        else 'senior' 
    }");
```

#### Hybrid Approach (Computed Field + Index)
```grapa
// Create computed field and index it
group_db.mkcomputedfield("full_name", "STR", "VAR", 
    "lambda(record) { record.get('first_name') + ' ' + record.get('last_name') }");
group_db.mkindex("full_name_index", ["full_name"]);  // Index the computed field
```

### Index Formula Design

#### Formula Storage
```cpp
// Extended index structure to support formulas
struct GrapaDBIndexFormula {
    u64 formulaId;           // Unique formula identifier
    GrapaCHAR formulaCode;   // Grapa lambda expression
    u64 resultFieldId;       // Field ID for computed result
    u8 resultType;           // Data type of computed result
    u64 resultSize;          // Size of computed result
};
```

#### Formula Evaluation
```cpp
// Index formula evaluation during record insertion/update
GrapaError GrapaDB::EvaluateIndexFormula(GrapaDBIndex& pIndex, GrapaCursor& recordCursor, GrapaBYTE& result)
{
    // Parse and execute Grapa lambda expression
    GrapaValue lambdaResult;
    err = ExecuteGrapaLambda(pIndex.mFormula.formulaCode, recordCursor, lambdaResult);
    if (err) return(err);
    
    // Convert result to appropriate type and store
    err = ConvertToIndexType(lambdaResult, pIndex.mFormula.resultType, result);
    return(err);
}
```

#### Index Creation with Formula
```cpp
// Create index on computed field
GrapaError GrapaDB::CreateFormulaIndex(GrapaDBTable& pTable, u64 pIndexId, const GrapaCHAR& pFormula, GrapaDBIndex& pIndex)
{
    // Parse and validate formula
    GrapaDBIndexFormula formula;
    err = ParseIndexFormula(pFormula, formula);
    if (err) return(err);
    
    // Create index with formula
    pIndex.mFormula = formula;
    err = CreateIndex(pTable, pIndexId, pIndex);
    if (err) return(err);
    
    // Populate index with computed values
    err = PopulateFormulaIndex(pIndex);
    return(err);
}
```

### Formula Examples

#### Simple Computed Fields
```grapa
// Index on concatenated name
index1 = group_db.mkformulaindex("full_name_index", "lambda(record) { record.get('first_name') + ' ' + record.get('last_name') }");

// Index on age category
index2 = group_db.mkformulaindex("age_category_index", "lambda(record) { age = record.get('age').raw().int(); if (age < 30) 'young' else if (age < 50) 'middle' else 'senior' }");
```

#### Complex Computations
```grapa
// Index on salary percentile
index3 = group_db.mkformulaindex("salary_percentile_index", "lambda(record) { salary = record.get('salary').raw().int(); department = record.get('department'); calculatePercentile(salary, department) }");

// Index on location-based calculations
index4 = group_db.mkformulaindex("distance_index", "lambda(record) { lat1 = record.get('lat').raw().float(); lon1 = record.get('lon').raw().float(); calculateDistance(lat1, lon1, 40.7128, -74.0060) }");
```

#### Conditional Indexing
```grapa
// Index only active employees
index5 = group_db.mkformulaindex("active_employee_index", "lambda(record) { if (record.get('status') == 'active') record.get('employee_id') else null }");

// Index on date ranges
index6 = group_db.mkformulaindex("quarter_index", "lambda(record) { date = record.get('hire_date'); extractQuarter(date) }");
```

### Implementation Considerations

#### Performance
- **Formula Caching**: Cache compiled lambda expressions for performance
- **Incremental Updates**: Only recompute formulas when dependent fields change
- **Background Computation**: Compute formulas in background for large datasets
- **Lazy Evaluation**: Evaluate formulas on-demand for read-heavy workloads

#### Storage
- **Formula Storage**: Store formulas in index metadata
- **Result Caching**: Cache computed results in index structure
- **Dependency Tracking**: Track field dependencies for formula updates
- **Version Management**: Version formulas for schema evolution

#### Validation
- **Formula Syntax**: Validate Grapa lambda syntax
- **Type Safety**: Ensure formula result types match index requirements
- **Dependency Analysis**: Validate field dependencies exist
- **Circular Reference**: Prevent circular formula dependencies

#### Integration with Existing System
- **Two-Stage Search**: Extend search algorithm to handle formula-based indexes
- **Index Selection**: Include formula indexes in index selection logic
- **Field Dependencies**: Update field modification to handle formula dependencies
- **Transaction Safety**: Ensure formula updates are atomic

### Formula Index Lifecycle

#### Creation
1. **Parse Formula**: Validate and parse Grapa lambda expression
2. **Analyze Dependencies**: Identify fields used in formula
3. **Create Index**: Create index structure with formula metadata
4. **Populate Index**: Compute formula for all existing records
5. **Register Dependencies**: Track field dependencies for updates

#### Updates
1. **Detect Changes**: Monitor dependent field changes
2. **Recompute Formula**: Evaluate formula for changed records
3. **Update Index**: Update index entries with new computed values
4. **Maintain Consistency**: Ensure index remains consistent

#### Deletion
1. **Remove Dependencies**: Clear field dependency tracking
2. **Delete Index**: Remove index structure and metadata
3. **Cleanup Cache**: Clear formula and result caches

### Benefits of Formula Indexes

#### Query Performance
- **Complex Searches**: Index complex computed values
- **Aggregation Support**: Pre-compute aggregation results
- **Conditional Indexing**: Index only relevant records
- **Multi-Field Computations**: Index derived from multiple fields

#### Application Flexibility
- **Business Logic**: Encode business rules in indexes
- **Data Transformation**: Index transformed data views
- **Conditional Logic**: Index based on complex conditions
- **Dynamic Computations**: Index computed at query time

#### System Integration
- **Grapa Language**: Leverage full Grapa language capabilities
- **Type Safety**: Maintain Grapa's type system
- **Error Handling**: Use Grapa's error handling mechanisms
- **Extensibility**: Support future Grapa language features

This formula index design extends GrapaDB's indexing capabilities while maintaining the simple, reliable philosophy of the delete-and-recreate approach. It leverages Grapa's powerful lambda expressions to provide sophisticated indexing capabilities without compromising system reliability.

This design reflects the "Database File System" philosophy where every record can have both structured data (custom fields) and unstructured data (the default `$VALUE` field), just like files in a file system can have both metadata and content. The `$KEY` field provides the primary indexing mechanism, while custom fields can be indexed as needed for specific search requirements.

## Overview

GrapaDB2 is a complete rewrite of GrapaDB that maintains the same tree-of-trees architecture but with improved implementation. This document outlines the complete data structure design before any implementation begins.

## Tree-of-Trees Architecture

GrapaDB2 uses a hierarchical BTree structure where each tree serves a specific purpose in the database system. The architecture is designed to support ROW, COL, and GROUP storage types with proper indexing and data management.

### Core Tree Structure

```
Database File
├── Root Tree (Tree 1) - Database metadata and table registry
│   ├── Table Registry (TREE_ITEM entries)
│   └── Database Type (GROUP_TREE, RTABLE_TREE, CTABLE_TREE)
│
├── Table Trees (Tree 2, 3, 4, ...) - Individual tables
│   ├── Records Tree (RREC_ITEM, CREC_ITEM, GREC_ITEM)
│   ├── Index Tree (RPTR_ITEM, CPTR_ITEM, GPTR_ITEM)
│   ├── Dictionary Tree (DTYPE_ITEM) - Field definitions
│   └── Data Store Tree (BYTE_DATA, FREC_DATA)
│
└── Index Trees (Tree N+1, N+2, ...) - Search indexes
    ├── Index Entries (pointers to records)
    └── Index Metadata
```

## Detailed Tree Design

### 1. Root Tree (Tree 1) - Database Registry

**Purpose**: Contains database metadata and table registry
**Tree Type**: Based on database type (GROUP_TREE, RTABLE_TREE, CTABLE_TREE)

**Structure**:
```
Key=0: Database metadata (reserved for future use)
Key=1: Table 1 reference (TREE_ITEM)
Key=2: Table 2 reference (TREE_ITEM)
...
Key=N: Table N reference (TREE_ITEM)
```

**TREE_ITEM Structure**:
- `mKey`: Table ID
- `mValue`: Reference to table's root tree
- `mValueType`: TREE_ITEM

### 2. Table Trees - Data Storage

Each table has its own tree structure that varies by table type:

#### 2.1 ROW Table Structure

**Main Tree**: Contains records and indexes
**Tree Type**: RTABLE_TREE

**Structure**:
```
Key=0: Dictionary (DTYPE_ITEM) - Field definitions
Key=1: Record 1 (RREC_ITEM) - Complete record data
Key=2: Record 2 (RREC_ITEM) - Complete record data
...
Key=N: Record N (RREC_ITEM) - Complete record data
```

**RREC_ITEM Structure**:
- `mKey`: Record ID
- `mValue`: Data block reference containing all fields
- `mValueType`: RREC_ITEM

**Index Tree** (separate tree):
```
Key=0: Index metadata (reserved)
Key=1: Index entry 1 (RPTR_ITEM) - Points to record
Key=2: Index entry 2 (RPTR_ITEM) - Points to record
...
```

**RPTR_ITEM Structure**:
- `mKey`: Index key value
- `mValue`: Reference to record tree
- `mValueType`: RPTR_ITEM

#### 2.2 COL Table Structure

**Main Tree**: Contains column metadata and pointers
**Tree Type**: CTABLE_TREE

**Structure**:
```
Key=0: Dictionary (DTYPE_ITEM) - Field definitions
Key=1: Column 1 pointer (CPTR_ITEM) - Points to column data
Key=2: Column 2 pointer (CPTR_ITEM) - Points to column data
...
Key=N: Column N pointer (CPTR_ITEM) - Points to column data
```

**CPTR_ITEM Structure**:
- `mKey`: Column ID
- `mValue`: Reference to column data tree
- `mValueType`: CPTR_ITEM

**Column Data Trees** (separate trees per column):
```
Key=1: Row 1 data (BYTE_DATA or FREC_DATA)
Key=2: Row 2 data (BYTE_DATA or FREC_DATA)
...
Key=N: Row N data (BYTE_DATA or FREC_DATA)
```

**Index Tree** (separate tree):
```
Key=0: Index metadata (reserved)
Key=1: Index entry 1 (CPTR_ITEM) - Points to column data
Key=2: Index entry 2 (CPTR_ITEM) - Points to column data
...
```

#### 2.3 GROUP Table Structure

**Main Tree**: Contains group hierarchy and navigation
**Tree Type**: GROUP_TREE

**Structure**:
```
Key=0: Dictionary (DTYPE_ITEM) - Field definitions and group schema
Key=1: Group 1 (GREC_ITEM) - Root group or first level group
Key=2: Group 2 (GREC_ITEM) - Sibling group or next level group
...
Key=N: Group N (GREC_ITEM) - Additional groups at same level
```

**GREC_ITEM Structure**:
- `mKey`: Group ID (hierarchical ID for navigation)
- `mValue`: Reference to group's data tree
- `mValueType`: GREC_ITEM

**Default Fields in GROUP**:
Every GROUP database has two special default fields:

1. **`$KEY`** (if enabled): The primary key field for record identification
   - Type: `STR` (GrapaTokenType::STR)
   - Storage: `FIX` (GrapaDBField::STORE_FIX)
   - Size: 256 bytes
   - Used for record lookup and indexing

2. **`$VALUE`** (always available): The default data field
   - Type: `RAW` (GrapaTokenType::RAW) 
   - Storage: `VAR` (GrapaDBField::STORE_VAR)
   - Initial size: 32 bytes, grows by 8 bytes
   - **Auto-created**: This field is automatically created when first accessed
   - **Default behavior**: If no specific field is specified, `$VALUE` is used

**Field Support**: GROUP databases can support **any field type that ROW or COL tables support**:
- `BOOL`, `INT`, `FLOAT`, `STR`, `TIME`, `TABLE`, `RAW`
- `FIX`, `VAR`, `PAR` storage types
- Custom field sizes and growth parameters

**Group Data Trees** (separate trees per group):
```
Key=0: Group metadata (parent, children, group type, etc.)
Key=1: $VALUE field data (auto-created RAW data)
Key=2: $KEY field data (if enabled, STR data)
Key=3: Custom field 1 data (any data type)
Key=4: Custom field 2 data (any data type)
...
Key=N: Custom field N data (any data type)
Key=N+1: Child Group 1 reference (GPTR_ITEM) - Points to child group
Key=N+2: Child Group 2 reference (GPTR_ITEM) - Points to child group
...
Key=N+M: Child Group M reference (GPTR_ITEM) - Points to child group
```

**Group Metadata Structure** (Key=0 in group data tree):
```
- mParentGroup: Reference to parent group (0 for root)
- mGroupType: Type of group (folder, object, array, etc.)
- mFieldCount: Number of fields in this group
- mChildCount: Number of child groups
- mGroupName: Name of the group
- mGroupFlags: Various flags (read-only, hidden, etc.)
```

**Index Tree** (separate tree):
```
Key=0: Index metadata (reserved)
Key=1: Index entry 1 (GPTR_ITEM) - Points to group data
Key=2: Index entry 2 (GPTR_ITEM) - Points to group data
...
```

**GPTR_ITEM Structure**:
- `mKey`: Index key value or child group ID
- `mValue`: Reference to group data tree
- `mValueType`: GPTR_ITEM

**Hierarchical Navigation**:
```
Root Group (ID=1)
├── Child Group A (ID=1.1)
│   ├── Grandchild A1 (ID=1.1.1)
│   └── Grandchild A2 (ID=1.1.2)
├── Child Group B (ID=1.2)
│   └── Grandchild B1 (ID=1.2.1)
└── Child Group C (ID=1.3)
```

**Group Types**:
- **FOLDER**: Directory-like structure (like file system folders)
- **OBJECT**: JSON-like object with named fields
- **ARRAY**: Array-like structure with indexed elements
- **TABLE**: Database table within group structure
- **LINK**: Reference to another group or external resource

### 3. Dictionary Tree (DTYPE_ITEM)

**Purpose**: Store field definitions and metadata
**Location**: Key=0 in each table tree

**Structure**:
```
Field 1: {id, name, type, store, size, grow}
Field 2: {id, name, type, store, size, grow}
...
Field N: {id, name, type, store, size, grow}
```

**Field Structure**:
- `mId`: Field ID
- `mName`: Field name
- `mType`: Data type (STR, INT, FLOAT, etc.)
- `mStore`: Storage type (FIX, VAR, PAR)
- `mSize`: Field size
- `mGrow`: Growth factor

### 4. Data Storage Trees

#### 4.1 BYTE_DATA Storage
**Purpose**: Store fixed-size or small variable data
**Structure**: Single data block per value

#### 4.2 FREC_DATA Storage
**Purpose**: Store large or fragmented data
**Structure**: Weighted BTree for efficient fragment management

## Key Design Principles

### 1. Tree Separation
- **Records Tree**: Stores actual data
- **Index Tree**: Stores pointers for fast lookup
- **Dictionary Tree**: Stores schema information
- **Data Store Tree**: Stores large/fragmented data

### 2. Key=0 Reserved Pattern
- **Key=0**: Always reserved for metadata/dictionary
- **Key=1+**: Used for actual data/index entries
- **Consistent**: Same pattern across all tree types

### 3. Pointer System
- **RPTR_ITEM**: Row table pointers
- **CPTR_ITEM**: Column table pointers  
- **GPTR_ITEM**: Group table pointers
- **TREE_ITEM**: Tree references

### 4. Index Management
- **Separate Index Trees**: Each index has its own tree
- **Pointer-Based**: Indexes store pointers, not data
- **Multi-Field Support**: Composite indexes supported
- **Automatic Updates**: Indexes updated when records change

## Implementation Strategy

### Phase 1: Core Tree Structure
1. Implement basic tree creation and management
2. Implement dictionary storage (DTYPE_ITEM)
3. Implement basic record storage (RREC_ITEM, CREC_ITEM, GREC_ITEM)
4. Implement GROUP hierarchical navigation and metadata

### Phase 2: Data Storage
1. Implement BYTE_DATA storage
2. Implement FREC_DATA storage with weighted BTree
3. Implement data block management

### Phase 3: Index System
1. Implement pointer types (RPTR_ITEM, CPTR_ITEM, GPTR_ITEM)
2. Implement index tree creation and management
3. Implement index-based search
4. Implement GROUP hierarchical indexing and path-based search

### Phase 4: Advanced Features
1. Implement multi-field indexes
2. Implement query optimization
3. Implement transaction support
4. Implement GROUP advanced features:
   - Path-based queries and wildcards
   - Group inheritance and templates
   - Cross-group references and links
   - Group-level permissions and access control

## Tree Type Mapping

| Database Type | Tree Type | Record Type | Pointer Type | Storage Type |
|---------------|-----------|-------------|--------------|--------------|
| ROW           | RTABLE_TREE | RREC_ITEM   | RPTR_ITEM    | BYTE_DATA    |
| COL           | CTABLE_TREE | CREC_ITEM   | CPTR_ITEM    | FREC_DATA    |
| GROUP         | GROUP_TREE  | GREC_ITEM   | GPTR_ITEM    | BYTE_DATA    |

## Reserved Keys and Special Values

### Key=0 Reserved Items
- **Root Tree**: Database metadata (future use)
- **Table Trees**: Dictionary (DTYPE_ITEM)
- **Index Trees**: Index metadata (future use)

### Special Value Types
- **TREE_ITEM**: Tree references
- **DTYPE_ITEM**: Dictionary/field definitions
- **RREC_ITEM/CREC_ITEM/GREC_ITEM**: Record data
- **RPTR_ITEM/CPTR_ITEM/GPTR_ITEM**: Index pointers

## Data Flow Examples

### ROW Table Insert
1. Create RREC_ITEM in records tree
2. Store record data in data block
3. Update all index trees with RPTR_ITEM entries

### COL Table Insert
1. Create CPTR_ITEM in main tree
2. Store column data in column-specific tree
3. Update index trees with CPTR_ITEM entries

### GROUP Table Insert
1. Create GREC_ITEM in main tree
2. Store group data in group-specific tree
3. Update parent group's child references
4. Update index trees with GPTR_ITEM entries
5. Handle hierarchical ID assignment

### GROUP Table Navigation
1. **Path-based Access**: Navigate using hierarchical paths (e.g., "/users/admins/user1")
2. **Parent/Child Traversal**: Move up/down the hierarchy using parent/child references
3. **Sibling Navigation**: Move between groups at the same level
4. **Field Access**: Access fields within groups using field IDs or names
5. **Group Creation**: Create new groups at any level in the hierarchy

## GROUP-Specific Operations

### Group Management Methods
- **CreateGroup(parentGroup, groupName, groupType)**: Create new group
- **DeleteGroup(groupId)**: Delete group and all children
- **MoveGroup(groupId, newParentId)**: Move group to different parent
- **CopyGroup(sourceId, targetParentId)**: Copy group structure and data
- **RenameGroup(groupId, newName)**: Change group name

### Navigation Methods
- **OpenGroup(groupId)**: Open group for access
- **GetParentGroup(groupId)**: Get parent group reference
- **GetChildGroups(groupId)**: Get list of child groups
- **GetSiblingGroups(groupId)**: Get groups at same level
- **FindGroupByPath(path)**: Find group using path string
- **GetGroupPath(groupId)**: Get full path to group

### Field Operations
- **CreateField(groupId, fieldName, fieldType)**: Add field to group
- **DeleteField(groupId, fieldId)**: Remove field from group
- **SetField(groupId, fieldId, value)**: Set field value
- **GetField(groupId, fieldId)**: Get field value
- **ListFields(groupId)**: Get all fields in group

### Advanced GROUP Features
- **Group Templates**: Predefined group structures for reuse
- **Group Inheritance**: Child groups inherit parent structure
- **Cross-Group References**: Groups can reference other groups
- **Group Permissions**: Access control at group level
- **Group Versioning**: Track changes to group structure
- **Group Backup/Restore**: Save and restore group hierarchies

## Performance Considerations

### Tree Depth Optimization
- Use appropriate NODE_WIDTH for different tree types
- Balance between memory usage and tree depth

### Index Efficiency
- Separate index trees for different search patterns
- Pointer-based indexing for fast lookups

### Data Storage Optimization
- BYTE_DATA for small/fixed data
- FREC_DATA for large/variable data
- Weighted BTree for efficient fragment management

### GROUP-Specific Optimization
- **Hierarchical Caching**: Cache frequently accessed parent/child relationships
- **Path Indexing**: Index group paths for fast path-based lookups
- **Lazy Loading**: Load group children only when accessed
- **Bulk Operations**: Optimize operations on entire group hierarchies
- **Memory Management**: Efficient memory usage for large group structures

## Compatibility with GrapaDB

This design maintains full compatibility with GrapaDB's tree structure while providing a cleaner, more maintainable implementation. The key differences are:

1. **Cleaner Implementation**: Better separation of concerns
2. **Improved Error Handling**: More robust error detection and recovery
3. **Enhanced Performance**: Optimized tree operations and data storage
4. **Better Maintainability**: Clearer code structure and documentation

## Next Steps

1. **Review and Approve Design**: Ensure this design meets all requirements
2. **Create Implementation Plan**: Break down implementation into phases
3. **Begin Phase 1**: Start with core tree structure implementation
4. **Test Each Phase**: Validate implementation against design
5. **Document Progress**: Update documentation as implementation progresses

## GROUP Use Cases and Examples

### File System Emulation
```
/users (GROUP)
├── /users/admins (GROUP)
│   ├── /users/admins/user1 (GROUP)
│   │   ├── name: "Alice"
│   │   ├── email: "alice@example.com"
│   │   └── permissions: ["read", "write", "admin"]
│   └── /users/admins/user2 (GROUP)
│       ├── name: "Bob"
│       └── email: "bob@example.com"
└── /users/guests (GROUP)
    └── /users/guests/visitor1 (GROUP)
        ├── name: "Visitor"
        └── permissions: ["read"]
```

### Configuration Management
```
/config (GROUP)
├── /config/database (GROUP)
│   ├── host: "localhost"
│   ├── port: 5432
│   └── credentials (GROUP)
│       ├── username: "admin"
│       └── password: "secret"
├── /config/application (GROUP)
│   ├── debug: true
│   └── log_level: "INFO"
└── /config/network (GROUP)
    ├── timeout: 30
    └── retry_count: 3
```

### Document Storage
```
/documents (GROUP)
├── /documents/reports (GROUP)
│   ├── /documents/reports/2024 (GROUP)
│   │   ├── /documents/reports/2024/Q1 (GROUP)
│   │   │   ├── sales_report.pdf
│   │   │   └── financial_summary.xlsx
│   │   └── /documents/reports/2024/Q2 (GROUP)
│   └── /documents/reports/2023 (GROUP)
└── /documents/templates (GROUP)
    ├── invoice_template.docx
    └── contract_template.pdf
```

### Application State
```
/state (GROUP)
├── /state/session (GROUP)
│   ├── user_id: 12345
│   ├── login_time: "2024-01-15T10:30:00Z"
│   └── preferences (GROUP)
│       ├── theme: "dark"
│       └── language: "en"
├── /state/cache (GROUP)
│   ├── /state/cache/users (GROUP)
│   └── /state/cache/products (GROUP)
└── /state/queue (GROUP)
    ├── pending_tasks: 5
    └── completed_tasks: 150
```

This design provides a solid foundation for implementing GrapaDB2 with full compatibility to GrapaDB while improving maintainability and performance. The GROUP table design is now comprehensive and covers all the hierarchical data management needs that make GrapaDB2 a powerful database system. 