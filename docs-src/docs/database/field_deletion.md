# Field Deletion

## Overview

Field deletion allows you to remove fields from database tables at runtime. The complexity and performance impact varies significantly depending on the table type and the number of records.

## Basic Usage

```grapa
/* Delete a field from a table */
table.rmfield("field_name");
```

## Table Type Performance Characteristics

### Group Store (GROUP) - Fastest
```grapa
/* GROUP tables - very fast field deletion */
f.mk("hierarchical_data", "GROUP");
f.mkfield("name", "STR", "VAR");
f.mkfield("description", "STR", "VAR");
f.set("item1", "value1", "name");
f.set("item1", "desc1", "description");

/* Delete field - very fast */
f.rmfield("description");
```

**Performance**: O(1) - Only dictionary updates required
- No record traversal needed
- No data movement required
- Only updates the field dictionary
- **Best for**: Hierarchical data where schema changes are common

### Column Store (COL) - Fast
```grapa
/* COL tables - fast field deletion */
f.mk("analytics_data", "COL");
f.mkfield("date", "TIME", "FIX", 8);
f.mkfield("value", "FLOAT", "FIX", 8);
f.mkfield("category", "STR", "VAR");

/* Delete field - fast */
f.rmfield("category");
```

**Performance**: O(1) - Only column tree deletion required
- No record traversal needed
- Deletes the entire column tree
- No data shifting required
- **Best for**: Analytical workloads with schema evolution

### Row Store (ROW) - Expensive
```grapa
/* ROW tables - expensive field deletion */
f.mk("transactional_data", "ROW");
f.mkfield("id", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");
f.mkfield("email", "STR", "VAR");
f.mkfield("phone", "STR", "VAR");

/* Delete field - expensive for large tables */
f.rmfield("phone");
```

**Performance**: O(n) where n = number of records
- Must traverse all records
- Must bitshift data in each record
- Must update all record pointers
- **Best for**: Small to medium transactional datasets

## Performance Considerations

### When to Use Each Table Type

#### Use GROUP for Field Deletion When:
- You have hierarchical data structures
- Schema changes are frequent
- Performance is critical
- Data is naturally grouped

#### Use COL for Field Deletion When:
- You have analytical workloads
- You have large datasets with many fields
- Schema evolution is common
- You need good performance for field operations

#### Use ROW for Field Deletion When:
- You have small to medium datasets
- You need fast point queries
- Field deletions are rare
- You prefer simple record-based operations

### Performance Guidelines

#### Small Datasets (< 1,000 records)
- **ROW tables**: Field deletion is acceptable
- **COL tables**: Excellent performance
- **GROUP tables**: Excellent performance

#### Medium Datasets (1,000 - 100,000 records)
- **ROW tables**: Consider the performance impact
- **COL tables**: Recommended for frequent schema changes
- **GROUP tables**: Excellent performance

#### Large Datasets (> 100,000 records)
- **ROW tables**: Avoid frequent field deletions
- **COL tables**: Highly recommended
- **GROUP tables**: Excellent performance

## Implementation Details

### Field Deletion Process

1. **Index Validation**: Check if field is used in any indexes
2. **Dictionary Update**: Remove field from field dictionary
3. **Data Cleanup**: Handle data based on table type
   - **GROUP**: No additional work
   - **COL**: Delete column tree
   - **ROW**: Bitshift all records

### Index Considerations

```grapa
/* Fields used in indexes cannot be deleted */
f.mk("data", "ROW");
f.mkfield("id", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");

/* Create index on name field */
f.mkindex("name_index", ["name"]);

/* This will fail - field is used in index */
f.rmfield("name");
/* Error: Field is used in index, cannot delete */
```

**Solution**: Drop the index first, then delete the field
```grapa
/* Drop index first */
f.rmindex("name_index");

/* Now field can be deleted */
f.rmfield("name");
```

### Implementation Details (Update)

- **Field Ordering**: Field ordering is not guaranteed and can change with deletion. All code must use dictionary lookups and never depend on field order.
- **Recent Issue**: Recent failures in field creation, deletion, and navigation were traced to using the root tree reference instead of the actual table reference. The fix is to always navigate to the actual table and use its reference for all field operations.

## Best Practices

### Schema Design

#### For Frequently Changing Schemas
```grapa
/* Use COL tables for flexible schemas */
f.mk("flexible_data", "COL");
f.mkfield("core_field1", "STR", "VAR");
f.mkfield("core_field2", "INT", "FIX", 4);

/* Add fields as needed */
f.mkfield("optional_field1", "STR", "VAR");
f.mkfield("optional_field2", "FLOAT", "FIX", 8);

/* Remove fields easily */
f.rmfield("optional_field1");
```

#### For Stable Schemas
```grapa
/* Use ROW tables for stable schemas */
f.mk("stable_data", "ROW");
f.mkfield("id", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");
f.mkfield("email", "STR", "VAR");

/* Field deletions are rare but possible */
f.rmfield("email");
```

### Performance Optimization

#### Batch Operations
```grapa
/* For ROW tables with many fields to delete */
fields_to_delete = ["field1", "field2", "field3"];

/* Consider recreating the table instead */
f.mk("new_table", "ROW");
/* Copy data without unwanted fields */
```

#### Alternative Approaches
```grapa
/* Instead of deleting fields, mark them as unused */
f.mkfield("deprecated_field", "STR", "VAR");
/* Set to NULL or empty values */
f.set("record1", "", "deprecated_field");
```

## Error Handling

### Common Errors

#### Field Used in Index
```grapa
/* Error: Field is used in index */
f.rmfield("indexed_field");
/* Solution: Drop index first */
```

#### Field Not Found
```grapa
/* Error: Field doesn't exist */
f.rmfield("nonexistent_field");
/* Solution: Check field exists before deletion */
```

#### Large ROW Table
```grapa
/* Warning: Large ROW table field deletion */
f.mk("large_table", "ROW");
/* Add many records... */
f.rmfield("field_name");
/* May take significant time */
```

## Migration Strategies

### From ROW to COL
```grapa
/* If you need frequent field deletions */
/* Consider migrating from ROW to COL */

/* Create new COL table */
new_table = f.mk("migrated_data", "COL");

/* Copy data without unwanted fields */
/* This avoids the expensive ROW field deletion */
```

### Schema Evolution Planning
```grapa
/* Plan your schema evolution */
/* Use COL tables for experimental schemas */
/* Use ROW tables for stable, production schemas */
/* Use GROUP tables for hierarchical data */
```

## Technical Details

### Storage Type Impact

#### Fixed Fields (STORE_FIX)
- **ROW**: Requires bitshifting in all records
- **COL**: Simple column tree deletion
- **GROUP**: No impact

#### Variable Fields (STORE_VAR)
- **ROW**: Requires bitshifting in all records
- **COL**: Simple column tree deletion
- **GROUP**: No impact

#### Partitioned Fields (STORE_PAR)
- **ROW**: Requires bitshifting in all records
- **COL**: Simple column tree deletion
- **GROUP**: No impact

### Memory and Disk Usage

#### After Field Deletion
- **GROUP**: No change in data size
- **COL**: Immediate space recovery
- **ROW**: Space recovery after potential fragmentation

#### Fragmentation Considerations
- **ROW tables**: May experience fragmentation after field deletion
- **COL tables**: No fragmentation issues
- **GROUP tables**: No fragmentation issues

## Examples

### Complete Field Lifecycle
```grapa
/* Create table */
f.mk("user_data", "COL");
f.mkfield("id", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");
f.mkfield("email", "STR", "VAR");
f.mkfield("phone", "STR", "VAR");

/* Add data */
f.set("user1", 1, "id");
f.set("user1", "John", "name");
f.set("user1", "john@example.com", "email");
f.set("user1", "555-1234", "phone");

/* Delete field */
f.rmfield("phone");

/* Verify deletion */
f.debug();
/* phone field is no longer present */
```

### Performance Comparison
```grapa
/* Test field deletion performance */

/* GROUP table - very fast */
group_table = f.mk("group_test", "GROUP");
group_table.mkfield("field1", "STR", "VAR");
group_table.mkfield("field2", "STR", "VAR");
/* Add data... */
group_table.rmfield("field2"); /* Very fast */

/* COL table - fast */
col_table = f.mk("col_test", "COL");
col_table.mkfield("field1", "STR", "VAR");
col_table.mkfield("field2", "STR", "VAR");
/* Add data... */
col_table.rmfield("field2"); /* Fast */

/* ROW table - expensive for large datasets */
row_table = f.mk("row_test", "ROW");
row_table.mkfield("field1", "STR", "VAR");
row_table.mkfield("field2", "STR", "VAR");
/* Add data... */
row_table.rmfield("field2"); /* Expensive for large datasets */
```

## Summary

Field deletion is fully supported across all table types, but with different performance characteristics:

- **GROUP**: O(1) - Very fast, dictionary updates only
- **COL**: O(1) - Fast, column tree deletion
- **ROW**: O(n) - Expensive, requires traversing all records

Choose your table type based on your schema evolution needs and performance requirements. For large datasets with frequent schema changes, prefer COL tables. For small, stable datasets, ROW tables are acceptable. 