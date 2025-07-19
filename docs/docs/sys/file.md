---
tags:
  - user
  - highlevel
---
# $file()

## Minimal Example
```grapa
f = $file();
f.chd("C:/Users/user/Projects/MyProject");
f.cd("src");
f.ls();  /* List files in /src */
f.get("README.md");  /* Read a file */
f.set("test.txt", "Hello, World!");  /* Write a file */
```

---

References:



Provides the ability to navigate either the file system or a database, querying data and updating data. This class/libraries will be enhanced over time to support navigating data types beyond the file system and the grapa database - such as JSON/XML and unstructured data where a mapping can be defined (maybe with a set of rules). With a few additional enhancements, this class/library will also enable extending the grapa syntax to include SQL with $file for the underlying data.

Each example below assumes the following command has been issued:
```grapa
f = $file();
```

Which assigns f an instance of the $file class. The following are then operations that can be used from the $file class.

The name field for the commands can include a path relative to the "working directory" (see pwd()). If the "working directory" is a OS filesystem directory, then the path must reference a file within the OS filesystem. If the "working directory" is a grapa table, then the path and data item would be within the grapa table. What is not currently supported is referencing a grapa table item when the "working directory" is not within a grapa table.

## type()
Returns the type of the file object.
```grapa
f.type();
$file
```

## table()
The table function creates an in-memory database. 

```grapa
t = f.table();
t.type();
$TABLE
```

**Database Types**: The table can be configured as either:
- **Row Store (RTABLE_TREE)**: Traditional row-oriented storage, optimized for transactional workloads
- **Column Store (CTABLE_TREE)**: Column-oriented storage, optimized for analytical queries and aggregations

**Note**: Column store databases use fragmented data storage (FREC_DATA) for efficient handling of sparse data and dynamic growth.

## Directory Navigation

Grapa provides two levels of directory navigation: **working directory** (relative) and **home directory** (absolute). This dual-level system allows for flexible project management and navigation.

### pwd() - Print Working Directory
Returns the current working directory, relative to the current home directory.

**Purpose**: Shows where you are within the current project context
**Returns**: Relative path (e.g., `/lib`, `/docs`)
**Use Case**: Navigation within a project or database

```grapa
f.pwd();
/

f.cd("lib");
f.pwd();
/lib

f.cd("grapa");
f.pwd();
/lib/grapa
```

### cd([name]) - Change Working Directory
Changes the current working directory, relative to the current home directory.

**Parameters**:
- `name` (optional): Directory name or path to navigate to
  - `".."`: Move up one level
  - `"/"`: Move to root of current home directory
  - `"path"`: Move to specific subdirectory

```grapa
f.cd("lib");
f.pwd();
/lib

f.cd("..");
f.pwd();
/

f.cd("/docs");
f.pwd();
/docs

f.cd("..");
f.pwd();
/
```

### phd() - Print Home Directory
Returns the current home directory (absolute path).

**Purpose**: Shows the base directory that serves as the root for relative navigation
**Returns**: Absolute path (e.g., `C:\Users\matichuk\Documents\GitHub\grapa`)
**Use Case**: Project switching and absolute path reference

```grapa
f.phd();
C:\Users\matichuk\Documents\GitHub\grapa

f.chd("C:/Users/matichuk/Documents/NewProject");
f.phd();
C:\Users\matichuk\Documents\NewProject
```

### chd(filesystempath) - Change Home Directory
Changes the current home directory to a new absolute path.

**Parameters**:
- `filesystempath`: Absolute or relative path to set as new home directory

**Note**: This resets the working directory to the root (`/`) of the new home directory.

```grapa
/* Change to absolute path */
f.chd("C:/Users/matichuk/Documents/NewProject");
f.phd();
C:\Users\matichuk\Documents\NewProject
f.pwd();
/

/* Change to relative path from current home */
f.chd("../sibling_project");
f.phd();
C:\Users\matichuk\Documents\sibling_project
f.pwd();
/
```

### Directory Navigation Comparison

| Function | Purpose | Scope | Change Method | Example Output |
|----------|---------|-------|---------------|----------------|
| `pwd()` | Show current location | Relative to home | `cd()` | `/lib/grapa` |
| `phd()` | Show base directory | Absolute system | `chd()` | `C:\Users\matichuk\Documents\GitHub\grapa` |

**Typical Workflow**:
1. Use `chd()` to set your project's home directory
2. Use `cd()` to navigate within the project
3. Use `pwd()` to see your current location within the project
4. Use `phd()` to see the absolute project location

## ls([name])
Retrieves a list of files/directories in the current working directory.

**Return Format**: Returns a list of objects with the following properties:
- `$KEY`: File or directory name
- `$TYPE`: Type ("FILE", "GROUP", etc.)
- `$BYTES`: File size in bytes (0 for directories)

**Note:**
- When navigating a traditional file system, folders/directories will be listed as `$TYPE: "GROUP"`.
- In a database context, `GROUP` also refers to hierarchical/grouped database structures.

```grapa
f.ls();
[
  {"$KEY":"docs","$TYPE":"GROUP","$BYTES":0},
  {"$KEY":"README.md","$TYPE":"FILE","$BYTES":4302}
]

/* Check type of a directory */
f.cd("docs");
f.type();
/* Returns: GROUP */
```

## mk(name [,type]) 
Creates a directory at the current working directory within the file system.

**Parameters**:
- `name`: Directory name to create
- `type` (optional): Type of directory/database to create

**Type Options**:
- `""` or `"DIR"`: Creates a regular directory (default)
- `"GROUP"`: Creates a database of GROUP type for hierarchical data, or a folder/directory in the file system
- `"ROW"`: Creates a ROW store database optimized for transactional workloads
- `"COL"`: Creates a COL store database optimized for analytical queries

**Column Store (COL) Characteristics**:
- Uses fragmented data storage for efficient sparse data handling
- Optimized for column-oriented queries and aggregations
- Better performance for analytical workloads
- Efficient storage of wide tables with many optional fields

**Note:**
- When using `mk()` in a file system context, `GROUP` is equivalent to creating a folder/directory.
- In a database context, `GROUP` creates a hierarchical/grouped database structure.

```grapa
/* Create regular directory */
f.mk("test");
f.cd("test");
f.ls();
[]

/* Create database directory */
f.mk("testdb", "GROUP");
f.cd("testdb");
f.ls();
[]

/* Create column store database */
f.mk("analytics_db", "COL");
f.cd("analytics_db");
```

## Type Table

| Type   | Description/Use Case                                 | Storage Model         |
|--------|-----------------------------------------------------|----------------------|
| GROUP  | Folder/directory in file system, or hierarchical/grouped database | GROUP_TREE           |
| ROW    | Transactional, record-based, OLTP, point queries    | RTABLE_TREE, BYTE_DATA |
| COL    | Analytical, column-based, sparse/large datasets     | CTABLE_TREE, FREC_DATA |

## rm(name)
Removes a directory or file.
```grapa
f.rm("test");
```

**Note**: This will recursively remove directories and their contents.

## set(name, value [, field])
Creates or updates a file with the specified content.

**Parameters**:
- `name`: File name
- `value`: Content to write to the file
- `field` (optional): Field name (defaults to `$VALUE`)

```grapa
f.set("test.txt", "Hello, World!");
f.set("config.json", '{"name": "test", "value": 123}');
```

## get(name [, field])
Reads the content of a file.

**Parameters**:
- `name`: File name
- `field` (optional): Field name (defaults to `$VALUE`)

**Return Format**: Returns file content in hexadecimal format.

```grapa
f.set("test.txt", "Hello, World!");
content = f.get("test.txt");
/* Returns: 0x48656C6C6F2C20576F726C6421 */
```

**Note**: File content is returned in hexadecimal format, not plain text. To convert to string, you may need to use additional processing.

## info(name)
Returns detailed metadata information about a file or directory.

**Parameters**:
- `name`: File or directory name to inspect

**Return Format**: Returns an object with file metadata containing:
- `$TYPE`: Type of item (`"FILE"`, `"DIR"`, or `"ERR"` for errors/non-existent)
- `$BYTES`: Size in bytes (0 for directories, actual size for files)
- `error`: Error code (-1) if item doesn't exist or is inaccessible

**Examples**:
```grapa
/* File information */
f.set("test.txt", "Hello, World!");
info = f.info("test.txt");
/* Returns: {"$TYPE":"FILE","$BYTES":13} */

/* Directory information */
f.mk("test_dir");
dir_info = f.info("test_dir");
/* Returns: {"$TYPE":"DIR","$BYTES":0} */

/* Non-existent item */
error_info = f.info("nonexistent.txt");
/* Returns: {"error":-1} */
```

**Use Cases**:

**File Type Detection**:
```grapa
info = f.info("document.txt");
if (info["$TYPE"] == "FILE") {
    "This is a file\n".echo();
} else if (info["$TYPE"] == "DIR") {
    "This is a directory\n".echo();
} else {
    "Item doesn't exist\n".echo();
}
```

**File Size Analysis**:
```grapa
info = f.info("large_file.txt");
if (info["$TYPE"] == "FILE") {
    size = info["$BYTES"];
    if (size > 1000000) {
        "File is larger than 1MB\n".echo();
    }
}
```

**Batch File Processing**:
```grapa
files = ["file1.txt", "file2.txt", "file3.txt"];
total_size = 0;
i = 0;
while (i < files.length()) {
    info = f.info(files[i]);
    if (info["$TYPE"] == "FILE") {
        total_size = total_size + info["$BYTES"];
    }
    i = i + 1;
}
"Total size: " + total_size + " bytes\n".echo();
```

**Key Benefits**:
- **Lightweight**: No need to open/read files to get metadata
- **Fast**: Direct OS system calls for file system operations
- **Unified Interface**: Same function works for files and directories
- **Cross-Platform**: Works consistently across different operating systems
- **Error Handling**: Clear error responses for non-existent items

**Implementation Notes**:
- Uses `stat64()` on Unix/Linux systems
- Uses `FindFirstFileA()` on Windows systems
- Works in both file system and database contexts
- Essential for file management, storage monitoring, and data validation

## split(parts, name, path, delim, option)
Splits a large file into multiple smaller, manageable parts for processing, storage, or transfer.

**Parameters**:
- `parts`: Number of files to split into (must be > 0)
- `name`: Input file name to split
- `path`: Output directory path for the split files (created if needed)
- `delim`: Delimiter to use for splitting (default: `"\n"`)
- `option`: Special options
  - `"csv"`: Copy header to each file (preserves CSV headers)
  - `"start"`: Search backwards for delimiter on split (prevents content breaking)

**Return Format**: Returns an array of created file names.
```grapa
result = f.split(4, "large_file.txt", "split_output", "\n", "");
/* Returns: ["1.large_file.txt","2.large_file.txt","3.large_file.txt","4.large_file.txt"] */
```

**Examples**:

**Basic File Splitting**:
```grapa
/* Create a large file */
large_content = "";
i = 1;
while (i <= 100) {
    large_content = large_content + "Line " + i + "\n";
    i = i + 1;
};
f.set("large_file.txt", large_content);

/* Split into 4 parts */
result = f.split(4, "large_file.txt", "split_output", "\n", "");
/* Creates: 1.large_file.txt, 2.large_file.txt, 3.large_file.txt, 4.large_file.txt */
```

**CSV File Splitting with Header Preservation**:
```grapa
/* Split CSV file while preserving headers in each part */
result = f.split(3, "data.csv", "csv_parts", "", "csv");
/* Each split file includes the original header row */
```

**Custom Delimiter Splitting**:
```grapa
/* Split on pipe character instead of newlines */
result = f.split(2, "custom_data.txt", "output", "|", "");
/* Splits content at pipe boundaries */
```

**Smart Boundary Detection**:
```grapa
/* Use start option to avoid breaking content arbitrarily */
result = f.split(2, "log_file.txt", "log_parts", "\n", "start");
/* Searches backwards for delimiter to maintain logical boundaries */
```

**Use Cases**:

**Large File Management**:
```grapa
/* Split large database export for processing */
f.split(10, "database_export.csv", "exports", "", "csv");
```

**Log File Processing**:
```grapa
/* Split large log files for parallel analysis */
f.split(5, "server.log", "log_chunks", "\n", "");
```

**Data Pipeline Preparation**:
```grapa
/* Prepare data for distributed processing */
f.split(8, "dataset.txt", "chunks", "\n", "start");
```

**Key Features**:
- **Automatic Naming**: Files named as `1.filename`, `2.filename`, etc.
- **Size Distribution**: Calculates optimal part sizes based on total file size
- **Memory Efficient**: Processes files in chunks, not all at once
- **Flexible Delimiters**: Supports any character or string as delimiter
- **Error Handling**: Returns `{"error":-1}` for non-existent files, `null` for invalid parameters
- **Cross-Platform**: Works consistently across operating systems

**Implementation Notes**:
- Uses efficient block-based file I/O for memory management
- Automatically creates output directory if it doesn't exist
- Handles remainder content appropriately when file size doesn't divide evenly
- Supports both file system and database contexts
- Zero-padded numbering ensures proper file sorting

## mkfield(name [, fieldType [, storeType [, storeSize [, storeGrow]]]])
Creates a field within the current working directory (database context).

**Parameters**:
- `name`: Field name
- `fieldType` (optional): Type of field (default: `"STR"`)
- `storeType` (optional): Storage type (default: `"VAR"`)
- `storeSize` (optional): Size for fixed fields
- `storeGrow` (optional): Growth size for variable fields

**Field Types**:
| Type | Description |
|------|-------------|
| `BOOL` | Fixed size for $BOOL |
| `TIME` | Stores an $INT. Size depends on storeType and storeSize |
| `INT` | Stores an $INT. Size depends on storeType and storeSize |
| `FLOAT` | Stores a $FLOAT. Size depends on storeType and storeSize |
| `STR` | Stores a $STR. Size depends on storeType and storeSize |
| `TABLE` | Stores a $TABLE. Size depends on storeType and storeSize |
| `RAW` | Stores a $RAW. Size depends on storeType and storeSize |

**Storage Types**:
| Type | Description | Use Case |
|------|-------------|----------|
| `FIX` | Fixed field size, data embedded in row/col | Small, frequently accessed fields |
| `VAR` | Variable field size, uses extra reference | Medium-sized variable data |
| `PAR` | Partitioned field for large data updates | Large data requiring partial updates, COL store $TABLE types |

**Important Notes**:
- **Column Store Fixed Fields**: Use fragmented data storage (FREC_DATA) for efficient sparse data handling
- **Growth Parameters**: The `storeGrow` parameter is automatically set to `storeSize` for fixed fields if not specified
- **Performance**: Column store is optimized for analytical queries across columns

```grapa
f.mkfield("test");
f.mkfield("age", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");
```

## rmfield(name)
Deletes a field within the current working directory (database context).

```grapa
f.rmfield("test");
```

## debug()
Used for debugging the database during development. Displays the BTree structure of the data dictionary and fields and indexes for the current working directory when in a database (either in memory or on the file system).

```grapa
f.debug();
```

## Performance Considerations

### Row Store vs Column Store

**Row Store (ROW)**
- **Best for**: Transactional workloads, frequent record updates, point queries
- **Storage**: Contiguous data blocks per record
- **Performance**: Fast record retrieval and updates

**Column Store (COL)**
- **Best for**: Analytical queries, column scans, aggregations, sparse data
- **Storage**: Fragmented data storage for efficient sparse data handling
- **Performance**: Fast column-oriented operations, better compression

### Storage Type Performance

**FIX (Fixed)**
- Fastest access for small, frequently used fields
- Predictable storage requirements
- Best for primary keys, status flags, small integers

**VAR (Variable)**
- Flexible storage for variable-length data
- Good for medium-sized text fields
- Slight overhead for reference management

**PAR (Partitioned)**
- Best for large data requiring partial updates
- Efficient for very large fields
- Used automatically for COL store $TABLE types

## Troubleshooting

### Common Issues

**Column Store Performance**
- **Issue**: Slow performance on small datasets
- **Solution**: Consider row store for small, transactional workloads

**Field Creation Errors**
- **Issue**: Fields not created properly
- **Solution**: Ensure proper field type and storage parameters are specified

**Storage Efficiency**
- **Issue**: High storage overhead
- **Solution**: Use appropriate storage types and monitor growth parameters

### Debug Information

Use the `debug()` function to inspect database structure:
```grapa
f.debug();
```

This provides detailed information about:
- Database type and structure
- Field definitions and storage types
- Data distribution and storage efficiency

## Error Handling

When operations fail, the system returns error objects:
```grapa
/* Non-existent file */
result = f.get("non_existent.txt");
/* Returns: {"error":-1} */

/* Non-existent directory */
result = f.cd("non_existent_dir");
/* Returns: {"error":-1} */
```

## Usage Examples

### Basic File Operations
```grapa
f = $file();

/* Create and write to a file */
f.set("test.txt", "Hello, World!");

/* Read file content */
content = f.get("test.txt");

/* List directory contents */
files = f.ls();

/* Navigate directories */
f.cd("docs");
f.pwd();  /* Returns: /docs */
```

### Database Operations
```grapa
/* Create a column store database */
f.mk("analytics_db", "COL");
f.cd("analytics_db");

/* Create fields */
f.mkfield("id", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");
f.mkfield("age", "INT", "FIX", 4);

/* Add data */
f.set("user1", "John Doe", "name");
f.set("user1", 30, "age");

/* Retrieve data */
name = f.get("user1", "name");
age = f.get("user1", "age");
```

### Row Store vs Column Store Example
```grapa
/* Row store for transactional data */
f.mk("transaction_db", "ROW");
f.cd("transaction_db");
f.mkfield("order_id", "INT", "FIX", 4);
f.mkfield("customer_id", "INT", "FIX", 4);
f.mkfield("amount", "FLOAT", "FIX", 8);

/* Column store for analytical data */
f.mk("analytics_db", "COL");
f.cd("analytics_db");
f.mkfield("date", "TIME", "FIX", 8);
f.mkfield("product_id", "INT", "FIX", 4);
f.mkfield("sales_amount", "FLOAT", "FIX", 8);
f.mkfield("region", "STR", "VAR");
```

## Python Integration

For Python developers, Grapa's file system and database capabilities can be leveraged through the Python integration. The unified API provides seamless access to both file systems and databases, making it ideal for data science, web development, and system administration workflows.

### Key Python Use Cases

**Data Science and Analytics**:
- **Column store databases** for analytical workloads
- **Large file management** with built-in splitting capabilities
- **Unified data access** across different storage types

**Web Application Development**:
- **Backend data management** with row store for transactional data
- **Content management** with flexible field types
- **API development** with consistent data access patterns

**System Administration**:
- **Log file management** with automatic splitting for large files
- **Configuration management** with unified path navigation
- **Data pipeline integration** for ETL workflows

For detailed examples and best practices, see the [Python Integration Guide](../python_integration.md).

## Related Topics
- [Directory Navigation](../directory_navigation.md)
- [Database Quick Reference](../database/quick_reference.md)
- [Syntax Quick Reference](../syntax/basic_syntax.md)
