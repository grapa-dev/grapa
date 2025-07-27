---
tags:
  - user
  - highlevel
  - storage
  - database
---

# Unified Storage System

## Overview

Grapa's **Unified Storage System** provides a single interface (`$unified()`) to interact with various storage backends. This allows you to work with different storage types using the same commands and syntax, making it easy to switch between storage systems or work with multiple storage types simultaneously.

## Supported Storage Backends

The unified storage system supports the following storage types:

### File System Storage
- **Protocol**: `file://` (default)
- **Use Case**: Traditional file system operations
- **Features**: Full file system access with parallel processing support

### GrapaDB Storage
- **Protocol**: `grapadb://`
- **Use Case**: Traditional Grapa database operations
- **Features**: ROW, COL, and GROUP database types with hierarchical navigation

### GrapaDBX Storage
- **Protocol**: `grapadbx://`
- **Use Case**: Enhanced Grapa database operations (under development)
- **Features**: Improved index system and enhanced performance (basic operations working, full hierarchical navigation in progress)

### Network Storage
- **Protocol**: `network://`
- **Use Case**: HTTP/HTTPS web resources
- **Features**: Web scraping, API access, HTML/XML parsing

### Memory Storage
- **Protocol**: `memory://`
- **Use Case**: In-memory data storage
- **Features**: Fast temporary storage, session data

### Cloud Storage
- **Protocol**: `cloud://`
- **Use Case**: Cloud storage services
- **Features**: S3, Google Cloud Storage, Azure support

## Basic Usage

### Creating a Unified Storage Object

```grapa
/* Create unified storage object */
u = $unified();

/* Specify storage type via URL */
u.create("file:///path/to/files");
u.create("grapadb:///path/to/database");
u.create("grapadbx:///path/to/database2");
u.create("network://https://example.com");
u.create("memory://temp_data");
u.create("cloud://bucket-name");
```

### Basic Operations

All storage backends support the same basic operations:

```grapa
/* Create directories/objects */
u.mk("directory_name", "DIR");
u.mk("database_name", "GROUP");
u.mk("table_name", "ROW");

/* Create fields (for database types) */
u.mk("field_name", "STR", "VAR");

/* Set values */
u.set("record_name", "value", "field_name");

/* Get values */
value = u.get("record_name", "field_name");

/* List contents */
contents = u.ls();

/* Change directory/path */
u.cd("subdirectory");

/* Remove items */
u.rm("item_name");
```

## Storage-Specific Features

### File System Storage

```grapa
u = $unified();
u.create("file:///home/user/documents");

/* File system operations */
u.mk("project", "DIR");
u.cd("project");
u.mk("src", "DIR");
u.mk("docs", "DIR");
```

### GrapaDB Storage

```grapa
u = $unified();
u.create("grapadb:///path/to/database");

/* Database operations */
u.mk("users", "GROUP");
u.cd("users");
u.mk("id", "INT", "FIX", 4);
u.mk("name", "STR", "VAR");
u.set("user1", 1001, "id");
u.set("user1", "John Doe", "name");
```

### GrapaDBX Storage

```grapa
u = $unified();
u.create("grapadbx:///path/to/database2");

/* Database operations (basic functionality working) */
u.mk("users", "GROUP");
u.cd("users");
u.mk("id", "INT", "FIX", 4);
u.mk("name", "STR", "VAR");
u.set("user1", 1001, "id");
u.set("user1", "John Doe", "name");
value = u.get("user1", "name");
```

**Note**: GrapaDBX is under active development. Basic operations (mk, set, get) are working, but full hierarchical navigation and advanced features are still in progress.

### Network Storage

```grapa
u = $unified();
u.create("network://https://api.example.com");

/* Web operations */
data = u.get("users", "json");
html_content = u.get("page", "html");
```

### Memory Storage

```grapa
u = $unified();
u.create("memory://session_data");

/* In-memory operations */
u.set("temp_value", "data", "content");
value = u.get("temp_value", "content");
```

## Parallel Processing

The unified storage system supports parallel processing for compatible operations:

```grapa
/* Parallel file system operations */
u.ls("", 4);  /* Use 4 threads for listing */
u.mk("batch_operation", "DIR", 8);  /* Use 8 threads */

/* Database operations use sequential processing for safety */
u.set("record", "value", "field");  /* Always sequential */
```

## Cross-Storage Operations

You can easily transfer data between different storage types:

```grapa
/* Transfer from file system to database */
file_u = $unified();
file_u.create("file:///source/files");

db_u = $unified();
db_u.create("grapadb:///target/database");

/* Copy data */
data = file_u.get("file.txt", "content");
db_u.set("record1", data, "content");
```

## Error Handling

The unified storage system provides consistent error handling across all backends:

```grapa
/* Check for errors */
result = u.set("record", "value", "field");
if (result.error != 0) {
    /* Handle error */
    result.error.echo();
}
```

## Performance Considerations

- **File System**: Best for large files and parallel operations
- **GrapaDB**: Best for structured data with hierarchical relationships
- **GrapaDBX**: Enhanced performance and improved index system (under development)
- **Network**: Best for web data and API access
- **Memory**: Best for temporary data and high-speed access
- **Cloud**: Best for distributed storage and backup

## Best Practices

1. **Choose the Right Storage**: Select storage type based on your data access patterns
2. **Use Parallel Processing**: Leverage parallel processing for file system operations
3. **Plan Your Structure**: Design your data organization before implementation
4. **Handle Errors**: Always check for errors in critical operations
5. **Consider Performance**: Use appropriate storage types for your performance requirements

## Migration Between Storage Types

The unified interface makes it easy to migrate between storage types:

```grapa
/* Migrate from GrapaDB to GrapaDBX */
old_db = $unified();
old_db.create("grapadb:///old/database");

new_db = $unified();
new_db.create("grapadbx:///new/database");

/* Copy data */
records = old_db.ls();
for (record in records) {
    data = old_db.get(record, "all");
    new_db.set(record, data, "all");
}
```

## See Also

- [Unified Path System](unified_path_system.md) - Detailed information about path-based navigation
- [Database Quick Reference](database/quick_reference.md) - Database-specific operations
- [API Reference](api_reference.md) - Complete API documentation
- [Examples](examples.md) - Practical usage examples 