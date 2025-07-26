# Unified Storage Architecture Examples

## Overview

The Grapa unified storage architecture allows seamless navigation and operations across different storage types:
- **File Systems** - Traditional file/directory structure
- **GrapaDB** - Current database implementation
- **GrapaDB2** - New high-performance database
- **Network Objects** - Remote storage that appears local
- **Memory/Cloud** - Other storage backends

All accessible through the same unified interface with `.cd()`, path navigation, and consistent operations.

## Current Implementation Status

- `unified_cd`, `unified_ls`, `unified_mk`, `unified_rm`, `unified_set`, `unified_get` are implemented for the file system backend.
- TODOs are in place for GrapaDB, GrapaDB2, network, memory, and cloud backends.
- Incremental expansion and documentation are ongoing.

### Example: Listing and Navigating the File System

```grapa
u = $unified()
u.create("file:///tmp/test")
u.ls()        /* Lists directory contents */
u.cd("subdir")
u.ls()        /* Lists contents of subdir */
```

**🚧 IN PROGRESS:**
- `unified_cd` - Directory navigation across storage types
- `unified_ls` - Directory listing (unified format for file system and database)
- `unified_mk` - Create files/directories/tables
- `unified_rm` - Remove files/directories/tables
- `unified_set` - Set values/fields (with default $KEY/$VALUE for file-like operations)
- `unified_get` - Get values/fields (with default $KEY/$VALUE for file-like operations)
- Integration with actual GrapaDB/GrapaDB2 backends

**🎯 KEY DESIGN INSIGHT:**
- **Default $KEY/$VALUE**: GrapaDB2 will support default `$KEY` (mimics filename) and `$VALUE` (mimics file content) for seamless file-like operations within databases
- **Unified .ls() Output**: Same format whether in file system or database - `[name, size, type, date]`
- **Seamless Navigation**: `.cd()` and path operations work identically across storage types

## Enhanced LocalDatabase Pattern

```grapa
/* Different storage types via URLs - extends existing LocalDatabase */
fs = $unified()
fs.create("file:///home/user/documents")
db1 = $unified()
db1.create("grapadb://mydb.gdb")
db2 = $unified()
db2.create("grapadb2://mydb2.gdb") 
net = $unified()
net.create("network://remote-server/data")
mem = $unified()
mem.create("memory://temp-data")
cloud = $unified()
cloud.create("cloud://aws-s3/bucket-name")

/* Or use existing $file pattern with enhanced capabilities */
f = $file()
f.cd("/home/user/documents")     /* File system */
f.cd("mydb://users")             /* Database - seamless transition! */
```

## Seamless Navigation Across Storage Types

```grapa
/* Navigate seamlessly between file system and database */
.cd("/home/user/documents")           /* File system */
.cd("mydb://users")                   /* Database */
.cd("network://remote-server/data")   /* Network object */

/* A path can span multiple storage types */
.cd("/home/user/documents/mydb://users/network://cloud/backups")

/* Unified interface - same operations work everywhere */
storage = $unified()
storage.create("file:///home/user/documents")
files = storage.ls()                  /* File system listing */

storage.create("grapadb2://mydb.gdb")
files = storage.ls()                  /* Database listing - same format! */

/* Default $KEY/$VALUE makes databases feel like file systems */
storage.set("config.json", '{"key": "value"}', "$VALUE")  /* Works in both contexts */
```

## Enhanced LocalDatabase Operations

```grapa
/* Same operations work across all storage types - extends existing pattern */
storage = $unified()
storage.create("grapadb2://mydb.gdb")

/* Navigation - same as existing $file */
storage.cd("users")
storage.ls()
current_path = storage.pwd()

/* Enhanced operations that work across storage types */
storage.mk("config.json", "FILE")
storage.set("data.txt", "content", "$VALUE")
data = storage.get("data.txt", "$VALUE")
storage.rm("temp.txt")

/* Database-specific operations (when supported) */
storage.mk("users", "ROW")
storage.mkfield("users", "name", "STR", "VAR")
storage.set("user1", "John Doe", "name")
storage.get("user1", "name")
```

## Seamless File System / Database Interface

```grapa
/* GrapaDB2 supports default $KEY and $VALUE for file-like operations */
db = $unified()
db.create("grapadb2://mydb.gdb")

/* File-like operations within database */
db.set("document.txt", "This is file content", "$VALUE")     /* Mimics file creation */
content = db.get("document.txt", "$VALUE")                   /* Mimics file reading */
db.rm("document.txt")                                         /* Mimics file deletion */

/* Directory-like operations */
db.set("folder/", "", "$VALUE")                              /* Mimics directory creation */
db.cd("folder")
db.set("nested.txt", "Nested content", "$VALUE")

/* Unified .ls() output - same format as file system */
files = db.ls()  /* Returns same format whether in file system or database */
/* Output format: [name, size, type, date] - consistent across storage types */
```

## Performance Comparison

```grapa
/* Easy performance comparison between storage types */
db1 = $unified()
db1.create("grapadb://test1.gdb")
db2 = $unified()
db2.create("grapadb2://test2.gdb")

/* Both work identically */
db1.createTable("users", "ROW")
db2.createTable("users", "ROW")

/* Performance testing */
start1 = time()
db1.search("name = 'John'")
end1 = time()

start2 = time()
db2.search("name = 'John'")
end2 = time()

"GrapaDB: " + (end1 - start1).str() + "ms\n".echo();
"GrapaDB2: " + (end2 - start2).str() + "ms\n".echo();
```

## Cross-Storage Data Operations

```grapa
/* Read from file system, process, store in database */
fs = $unified()
fs.create("file:///home/user/data")
db = $unified()
db.create("grapadb2://processed.gdb")

/* Read CSV from file system */
csv_data = fs.read("users.csv")
lines = csv_data.split("\n")

/* Process and store in database */
db.createTable("users", "ROW")
db.addField("users", "name", "STR", "ROW", 100)
db.addField("users", "email", "STR", "ROW", 200)

/* Import data */
lines.range().each(line => {
    if (line.len() > 0) {
        fields = line.split(",")
        record = db.createRecord("users")
        db.updateRecord("users", record, "name", fields[0])
        db.updateRecord("users", record, "email", fields[1])
    }
})
```

## Network Storage Integration

```grapa
/* Network storage appears as local */
remote = $storage("network://server.example.com/data")

/* Same operations as local storage */
remote.cd("users")
remote.ls()
remote.read("config.json")
remote.write("log.txt", "Remote log entry")

/* Seamless integration with local operations */
local = $storage("file:///home/user")
local.write("remote_data.txt", remote.read("data.json"))
```

## Storage Type Detection and Fallback

```grapa
/* Automatic storage type detection */
storage = $storage("auto://path/to/resource")

/* Storage type information */
type = storage.getType()
info = storage.getInfo()

/* Fallback mechanisms */
if (type == "NETWORK" && !network_available()) {
    storage = $storage("file:///local/fallback")
}
```

## Advanced Path Navigation

```grapa
/* Complex path spanning multiple storage types */
storage = $storage("file:///home/user")

/* Navigate through file system */
storage.cd("documents")

/* Jump to database */
storage.cd("grapadb://mydb/users")

/* Jump to network */
storage.cd("network://cloud/backups")

/* Current path shows the full journey */
full_path = storage.pwd()
/* Result: "/home/user/documents/grapadb://mydb/users/network://cloud/backups" */
```

## Benefits of This Architecture

1. **Unified Interface** - Same operations work across all storage types
2. **Seamless Navigation** - `.cd()` works across file systems, databases, and networks
3. **Performance Flexibility** - Easy swapping between storage implementations
4. **Backward Compatibility** - Existing code continues to work
5. **Future Extensibility** - Easy to add new storage types
6. **Cross-Storage Operations** - Data can flow between different storage types
7. **Network Transparency** - Remote storage appears local
8. **Path Spanning** - Single path can reference multiple storage types

## Implementation Notes

- All storage types use the GrapaFile pattern for cross-platform compatibility
- URL parsing determines storage type and connection parameters
- Template-based implementation allows easy addition of new storage types
- Unified error handling across all storage types
- Performance monitoring and comparison built-in
- Network storage includes caching and offline capabilities 