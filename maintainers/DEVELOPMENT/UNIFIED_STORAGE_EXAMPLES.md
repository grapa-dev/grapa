# Unified Storage Examples

This document provides examples and usage patterns for the unified storage system that allows seamless navigation between file systems, databases, and other storage types.

## Overview

The unified storage system provides a single interface for accessing different storage types:
- **File System**: `file://` URLs for local file system access
- **GrapaDB**: `grapadb://` URLs for database access
- **GrapaDBX**: `grapadbx://` URLs for enhanced database access
- **Network**: `network://` URLs for remote storage (planned)
- **Memory**: `memory://` URLs for in-memory storage (planned)
- **Cloud**: `cloud://` URLs for cloud storage (planned)

## Basic Usage

### Creating and Initializing Storage

```grapa
/* Create unified storage object */
u = $unified();

/* Initialize with file system */
u.create("file:///path/to/files");

/* Initialize with database */
u.create("grapadb://database.gdb");

/* Initialize with enhanced database */
u.create("grapadbx://database2.gdb");
```

### Basic Operations

```grapa
/* Navigation */
u.cd("subdirectory")    /* Change directory */
u.pwd()                 /* Get current path */

/* Listing */
u.ls()                  /* List contents */
u.ls("subdir")          /* List specific directory */

/* Creation and Deletion */
u.mk("newdir", "DIR")   /* Create directory */
u.mk("record", "RECORD") /* Create record */
u.rm("item")            /* Remove item */

/* Data Operations */
u.set("file.txt", "content")           /* Set file content */
u.set("record", "value", "$VALUE")     /* Set field value */
u.get("file.txt")                      /* Get file content */
u.get("record", "$VALUE")              /* Get field value */
```

## Output Formatting Methods

The unified storage system now supports multiple output formats for listing operations:

### Default Output
```grapa
/* Default format - structured array */
result = u.ls();
result.echo();
/* Output: [{"$KEY":"file.txt","$TYPE":"FILE","$BYTES":1024}, ...] */
```

### JSON Output
```grapa
/* JSON format for API integration */
result = u.ls().json();
result.echo();
/* Output: [{"$KEY":"file.txt","$TYPE":"FILE","$BYTES":1024}, ...] */
```

### ASCII Table Output
```grapa
/* ASCII table format for human reading */
result = u.ls().table();
result.echo();
/* Output:
$KEY      $TYPE  $BYTES
file.txt  FILE   1024
dir1      DIR    0
*/
```

### Simple List Output
```grapa
/* Simple list format - just names */
result = u.ls().simple();
result.echo();
/* Output: ["file.txt", "dir1", ...] */
```

## Field Selection

You can specify which fields to include in the output:

```grapa
/* Select specific fields */
u.ls("$KEY,$TYPE")              /* Only name and type */
u.ls("$KEY,$BYTES").json()      /* Only name and size in JSON */
u.ls("$KEY,$TYPE").table()      /* Only name and type in table */
u.ls("$KEY").simple()           /* Only names in simple list */
```

## Cross-Storage Navigation

The unified system allows seamless navigation between different storage types:

```grapa
/* Start with file system */
u = $unified();
u.create("file:///home/user");

/* Navigate to database */
u.cd("database")
u.ls()        /* Lists database contents */

/* Navigate to subdirectory */
u.cd("subdir")
u.ls()        /* Lists contents of subdir */
```

## Storage Type Information

```grapa
/* Get storage type */
storage_type = u.getType();
storage_type.echo();  /* Output: "FILESYSTEM", "GRAPADB", etc. */

/* Get detailed information */
info = u.getInfo();
info.echo();  /* Output: "type:FILESYSTEM,url:file:///path,path:/current" */
```

## Complete Example

```grapa
/* Complete workflow example */
u = $unified();

/* Initialize with file system */
u.create("file:///tmp/test");

/* Create some content */
u.set("readme.txt", "This is a test file");
u.mk("docs", "DIR");
u.cd("docs");
u.set("api.md", "# API Documentation");

/* List with different formats */
"Default format:\n".echo();
u.ls().echo();

"JSON format:\n".echo();
u.ls().json().echo();

"Table format:\n".echo();
u.ls().table().echo();

"Simple format:\n".echo();
u.ls().simple().echo();

/* Navigate back and list parent */
u.cd("..");
"Parent directory:\n".echo();
u.ls().table().echo();
```

## Current Implementation Status

**✅ COMPLETED:**
- All unified handlers (`unified_cd`, `unified_ls`, `unified_mk`, `unified_rm`, `unified_set`, `unified_get`) implemented with full file system logic
- GrapaDB backend fully implemented and integrated
- GrapaDBX backend fully implemented and integrated
- **NEW: Output formatting methods (`.json()`, `.table()`, `.simple()`) implemented**
- **NEW: Field selection support in `.ls()` method**
- Clear TODOs and detailed comments for network, memory, and cloud backends
- Comprehensive test script (`test_unified_handlers.grc`, `test_unified_grapadb.grc`, `test_unified_grapadbx.grc`) created to verify all operations
- **NEW: Comprehensive output formatting test script (`test_unified_output_formats.grc`)**
- Error handling and parameter validation implemented
- Proper Grapa syntax with semicolons in all test scripts
- File system backend fully functional with real file operations

**🚧 IN PROGRESS:**
- Network storage backend integration
- Memory storage backend integration
- Cloud storage backend integration

**📋 TODO:**
- Add network storage support
- Add memory storage support
- Add cloud storage support
- Add comprehensive error handling for all backends
- Performance testing and optimization

### Example: Listing and Navigating the File System

```grapa
/* Create unified storage for file system */
u = $unified();
u.create("file:///tmp");

/* Create some test content */
u.set("file1.txt", "Content 1");
u.set("file2.txt", "Content 2");
u.mk("subdir", "DIR");

/* List with different formats */
"Default format:\n".echo();
u.ls().echo();

"JSON format:\n".echo();
u.ls().json().echo();

"Table format:\n".echo();
u.ls().table().echo();

"Simple format:\n".echo();
u.ls().simple().echo();

/* Navigate to subdirectory */
u.cd("subdir");
u.set("nested.txt", "Nested content");

"Subdirectory contents:\n".echo();
u.ls().table().echo();
```

### Example: Database Operations with Output Formatting

```grapa
/* Create unified storage for database */
u = $unified();
u.create("grapadb://testdb.gdb");

/* Create records */
u.mk("user1", "RECORD");
u.set("user1", "John Doe", "$VALUE");
u.mk("user2", "RECORD");
u.set("user2", "Jane Smith", "$VALUE");

/* List with different formats */
"Database records - default:\n".echo();
u.ls().echo();

"Database records - JSON:\n".echo();
u.ls().json().echo();

"Database records - table:\n".echo();
u.ls().table().echo();

"Database records - simple:\n".echo();
u.ls().simple().echo();
```

## Key Benefits

1. **Unified Interface**: Same methods work across all storage types
2. **Seamless Navigation**: `.cd()` works between file systems and databases
3. **Flexible Output**: Multiple output formats for different use cases
4. **Field Selection**: Choose which fields to include in listings
5. **Backward Compatibility**: Existing code continues to work
6. **Extensible**: Easy to add new storage types and output formats

## Design Philosophy

The unified storage system follows the principle of **"everything is a file"** but extends it to **"everything is accessible through the same interface"**. This allows for:

- **Consistent API**: Same methods regardless of storage type
- **Seamless Migration**: Easy to move between storage types
- **Flexible Output**: Choose the format that best fits your needs
- **Future Extensibility**: New storage types can be added without changing the interface 