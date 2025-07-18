# $TABLE

## Thread Safety and Parallelism
$TABLE (and all Grapa datatypes) are fully thread safe by design. All variable and data structure updates are internally synchronized at the C++ level, so you will never encounter crashes or corruption from concurrent access. However, if your program logic allows multiple threads to read and write the same table or field, you may see *logical* race conditions (unexpected values, overwrites, etc.). This is a design consideration, not a stability issue. Minimize shared mutable state between threads unless intentional.

**Only `$thread()` objects provide explicit locking and unlocking via `lock()`, `unlock()`, and `trylock()`.** To protect access to a shared resource, create a `$thread()` lock object and use it to guard access. Calling `.lock()` or `.unlock()` on a regular variable (like a table, array, or scalar) will return an error.

**Canonical Example:**
```grapa
lock_obj = $thread();
lock_obj.lock();
/* ... perform thread-safe operations on shared table or data ... */
lock_obj.unlock();
```

See [Threading and Locking](../sys/thread.md) and [Function Operators: static and const](../operators/function.md) for details and best practices.

References:
- [$TABLE object](../lib/grapa/$TABLE.grc)
- [$file object](../lib/grapa/$file.grc)


A $TABLE is a hierarchical database with columns, rows, supporting both row store and column store architectures.

**Note:**
- The type `GROUP` is used both for hierarchical/grouped databases and for folders/directories in the file system. When navigating a traditional file system, folders/directories will appear as `GROUP` in listings and type queries.

## Database Types

### Row Store (RTABLE_TREE)
- **Optimization**: Transactional workloads, record-based operations
- **Storage**: Contiguous data blocks (BYTE_DATA) for each record
- **Use Cases**: OLTP applications, frequent record updates, small to medium datasets

### Column Store (CTABLE_TREE)  
- **Optimization**: Analytical queries, column-based operations
- **Storage**: Fragmented data storage (FREC_DATA) for efficient sparse data handling
- **Use Cases**: Data warehousing, analytical workloads, large datasets with many columns

### Group Store (GROUP_TREE)
- **Optimization**: Hierarchical/grouped data, or file system folders/directories
- **Storage**: GROUP_TREE
- **Use Cases**: Nested/grouped data, directory navigation, or representing folders in the file system

## Type Table

| Type   | Description/Use Case                                 | Storage Model         |
|--------|-----------------------------------------------------|----------------------|
| GROUP  | Folder/directory in file system, or hierarchical/grouped database | GROUP_TREE           |
| ROW    | Transactional, record-based, OLTP, point queries    | RTABLE_TREE, BYTE_DATA |
| COL    | Analytical, column-based, sparse/large datasets     | CTABLE_TREE, FREC_DATA |

## Key Features

- **In-Memory and Persistent**: Can be stored in memory or on the file system
- **Dynamic Schema**: Fields can be added and removed at runtime
- **Multiple Storage Types**: Support for fixed, variable, and partitioned field storage
- **Efficient Queries**: Optimized for both row-oriented and column-oriented access patterns

## Performance Characteristics

### Row Store
- Fast record retrieval and updates
- Efficient for point queries on specific records
- Good for transactional workloads

### Column Store
- Fast column scans and aggregations
- Efficient for analytical queries across columns
- Better compression for similar data types
- Optimized for sparse data storage

## Basic Usage

```grapa
# Create an in-memory table
f = $file().table()
f.type()
$TABLE

# Basic operations
f.set("test", "value")
f.ls()
[{"$KEY":"test","$TYPE":"ROW","$BYTES":5}]

f.get("test")
value
```

## Field Creation

### Field Creation Syntax

The `mkfield()` method creates fields in the table with the following syntax:

```grapa
table.mkfield(field_name, field_type, storage_mode, size)
```

### Field Types

| Type   | Description           | Default Size | Notes |
|--------|----------------------|--------------|-------|
| STR    | String               | Variable     | Use "VAR" mode for variable length |
| INT    | Integer              | 4 bytes      | **Important**: Use without explicit mode for proper integer types |
| FLOAT  | Floating point       | 8 bytes      | Use "FIX" mode with size 8 |
| TIME   | Timestamp            | 8 bytes      | Unix timestamp format |
| BOOL   | Boolean              | 1 byte       | True/false values |

### Storage Modes

| Mode   | Description                    | Use Case |
|--------|--------------------------------|----------|
| VAR    | Variable length storage        | Strings, variable data |
| FIX    | Fixed length storage           | Numbers, timestamps |
| PAR    | Partitioned storage            | Large data requiring partial updates |

### Field Creation Examples

```grapa
/* String fields */
table.mkfield("name", "STR", "VAR");           /* Variable length string */
table.mkfield("code", "STR", "FIX", 10);       /* Fixed length string (10 chars) */

/* Integer fields - IMPORTANT: Use without explicit mode */
table.mkfield("age", "INT");                   /* ✅ Correct: Creates proper integer field */
table.mkfield("id", "INT", "FIX", 4);          /* ❌ May create string field instead */

/* Float fields */
table.mkfield("salary", "FLOAT", "FIX", 8);    /* 8-byte floating point */
table.mkfield("price", "FLOAT");               /* Default float field */

/* Time fields */
table.mkfield("created", "TIME", "FIX", 8);    /* Unix timestamp */
```

### Field Creation Best Practices

1. **Integer Fields**: Use `table.mkfield("field", "INT")` without explicit mode
2. **String Fields**: Use `"VAR"` mode for variable length, `"FIX"` with size for fixed length
3. **Float Fields**: Use `"FIX"` mode with size 8 for consistency
4. **Time Fields**: Use `"FIX"` mode with size 8 for timestamps

## Storage Architecture

### Row Store Architecture
- **Data Organization**: Records stored contiguously
- **Access Pattern**: Horizontal (all fields of a record)
- **Storage Type**: BYTE_DATA for fixed fields
- **Best For**: Record-based operations

### Column Store Architecture
- **Data Organization**: Columns stored separately
- **Access Pattern**: Vertical (all values of a field)
- **Storage Type**: FREC_DATA for fixed fields, SDATA_TREE for variable fields
- **Best For**: Column-based operations

## Field Storage Types

### Fixed Fields (STORE_FIX)
- **Row Store**: Contiguous storage within record
- **Column Store**: Fragmented storage (FREC_DATA) for efficient sparse data
- **Use Case**: Small, frequently accessed fields

### Variable Fields (STORE_VAR)
- **Row Store**: Reference to separate data area
- **Column Store**: Tree-based storage (SDATA_TREE)
- **Use Case**: Medium-sized variable data

### Partitioned Fields (STORE_PAR)
- **Row Store**: Reference to partitioned data area
- **Column Store**: Fragmented storage for large data
- **Use Case**: Large data requiring partial updates

## Use Case Examples

### Row Store Example (Transactional)
```grapa
/* Create row store database */
f.mk("orders_db", "ROW");
f.cd("orders_db");

/* Define schema with correct field creation */
f.mkfield("order_id", "INT");                  /* Integer field */
f.mkfield("customer_id", "INT");               /* Integer field */
f.mkfield("order_date", "TIME", "FIX", 8);     /* Timestamp */
f.mkfield("total_amount", "FLOAT", "FIX", 8);  /* Float field */
f.mkfield("status", "STR", "VAR");             /* Variable string */

/* Add records */
f.set("order_001", 1001, "customer_id");
f.set("order_001", 1640995200, "order_date");  /* Unix timestamp */
f.set("order_001", 299.99, "total_amount");
f.set("order_001", "pending", "status");

/* Retrieve complete record */
order = f.get("order_001", "customer_id");     /* Returns integer 1001 */
```

### Column Store Example (Analytical)
```grapa
/* Create column store database */
f.mk("sales_analytics", "COL");
f.cd("sales_analytics");

/* Define schema with correct field creation */
f.mkfield("date", "TIME", "FIX", 8);           /* Timestamp */
f.mkfield("product_id", "INT");                /* Integer field */
f.mkfield("region", "STR", "VAR");             /* Variable string */
f.mkfield("sales_amount", "FLOAT", "FIX", 8);  /* Float field */
f.mkfield("quantity", "INT");                  /* Integer field */

/* Add data (records created automatically) */
f.set("sale_001", 1640995200, "date");
f.set("sale_001", 1001, "product_id");
f.set("sale_001", "North", "region");
f.set("sale_001", 299.99, "sales_amount");
f.set("sale_001", 2, "quantity");

/* Column-oriented queries are efficient */
/* (In a real implementation, you'd have aggregation functions) */
```

## Data Retrieval and Type Conversion

### Retrieving Data with Type Conversion

When retrieving data from tables, use appropriate type conversion methods:

```grapa
/* Retrieve with type conversion */
name = table.get("user1", "name").str();       /* Convert to string */
age = table.get("user1", "age").int();         /* Convert to integer */
salary = table.get("user1", "salary").float(); /* Convert to float */
```

### Type Conversion Methods

| Method | Description | Use Case |
|--------|-------------|----------|
| `.str()` | Convert to string | Text data, display |
| `.int()` | Convert to integer | Numeric calculations |
| `.float()` | Convert to float | Decimal calculations |
| `.type()` | Get data type | Debugging, type checking |

## Performance Optimization

### Row Store Optimization
- Use fixed fields for frequently accessed data
- Minimize variable field usage for better performance
- Design for record-based access patterns

### Column Store Optimization
- Use appropriate field types for data distribution
- Consider data sparsity when designing schemas
- Design for column-oriented access patterns

## Advanced Features

### Dynamic Schema
```grapa
/* Add fields at runtime */
f.mkfield("new_field", "STR", "VAR");

/* Remove fields */
f.rmfield("old_field");
```

### Debug Information
```grapa
/* Inspect database structure */
f.debug();
```

## Best Practices

### Field Creation

1. **Integer Fields**: Always use `table.mkfield("field", "INT")` without explicit mode
2. **String Fields**: Use `"VAR"` for variable length, `"FIX"` with size for fixed length
3. **Float Fields**: Use `"FIX"` mode with size 8 for consistency
4. **Type Conversion**: Always use `.str()`, `.int()`, `.float()` when retrieving data

### Choosing Between Row and Column Store

**Use Row Store When**:
- Frequent record updates
- Point queries on specific records
- Small to medium datasets
- Transactional workloads

**Use Column Store When**:
- Analytical queries and aggregations
- Column scans across many records
- Large datasets with many columns
- Sparse data (many NULL values)

### Schema Design

**Row Store**:
- Group related fields together
- Use fixed fields for primary keys and frequently accessed data
- Consider record size for optimal performance

**Column Store**:
- Design for column-oriented access patterns
- Use appropriate storage types for data distribution
- Consider data sparsity and growth patterns

## Technical Details

### Storage Implementation
- **Row Store**: Uses BYTE_DATA for contiguous storage
- **Column Store**: Uses FREC_DATA for fragmented storage of fixed fields
- **Growth Management**: Automatic growth parameter setting for optimal performance
- **Sparse Data**: Efficient handling of NULL and empty values

### Data Access Patterns
- **Row Store**: Optimized for horizontal data access
- **Column Store**: Optimized for vertical data access
- **Cache Efficiency**: Different cache utilization patterns for each type
