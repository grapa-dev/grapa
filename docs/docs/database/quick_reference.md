# Database Quick Reference

## Database Creation

### Basic Database Types
```grapa
# Row Store (Transactional)
f.mk("db_name", "ROW")

# Column Store (Analytical)
f.mk("db_name", "COL")

# Group Database (Hierarchical)
f.mk("db_name", "GROUP")

# Regular Directory
f.mk("dir_name", "DIR")
```

### In-Memory Database
```grapa
# Create in-memory table
f = $file().table()
f.type()  # Returns: $TABLE
```

## Field Creation

### Field Types
| Type | Description | Example |
|------|-------------|---------|
| `BOOL` | Boolean values | `f.mkfield("active", "BOOL", "FIX", 1)` |
| `INT` | Integer values | `f.mkfield("age", "INT", "FIX", 4)` |
| `FLOAT` | Floating point | `f.mkfield("price", "FLOAT", "FIX", 8)` |
| `STR` | String data | `f.mkfield("name", "STR", "VAR")` |
| `TIME` | Time values | `f.mkfield("created", "TIME", "FIX", 8)` |
| `TABLE` | Nested tables | `f.mkfield("metadata", "TABLE", "VAR")` |
| `RAW` | Binary data | `f.mkfield("data", "RAW", "VAR")` |

### Storage Types
| Type | Description | Use Case |
|------|-------------|----------|
| `FIX` | Fixed size | Small, frequently accessed fields |
| `VAR` | Variable size | Medium-sized variable data |
| `PAR` | Partitioned | Large data requiring partial updates |

### Field Creation Examples
```grapa
# Fixed field with size
f.mkfield("id", "INT", "FIX", 4)

# Variable field
f.mkfield("name", "STR", "VAR")

# Fixed field with custom growth
f.mkfield("data", "FLOAT", "FIX", 8, 16)
```

## Data Operations

### Setting Data
```grapa
# Set field value
f.set("record_id", "value", "field_name")

# Set multiple fields
f.set("user1", "John Doe", "name")
f.set("user1", 30, "age")
f.set("user1", 75000.50, "salary")
```

### Getting Data
```grapa
# Get field value
value = f.get("record_id", "field_name")

# Get multiple fields
name = f.get("user1", "name")
age = f.get("user1", "age")
```

### Listing Records
```grapa
# List all records
records = f.ls()

# List specific directory
files = f.ls("subdirectory")
```

## Database Navigation

Grapa provides two-level directory navigation: **working directory** (relative) and **home directory** (absolute).

### Directory Operations
```grapa
# Working Directory (Relative Navigation)
f.cd("database_name")        # Change working directory
current = f.pwd()            # Get current working directory (relative)
f.cd("..")                   # Move up one level
f.cd("/")                    # Move to root of current home

# Home Directory (Absolute Navigation)  
home = f.phd()               # Get current home directory (absolute)
f.chd("/new/project/path")   # Change home directory to new project
f.chd("../sibling_project")  # Change home directory using relative path
```

### Navigation Examples
```grapa
# Set project home directory
f.chd("C:/Users/user/Projects/MyProject")
f.phd()  # Returns: C:\Users\user\Projects\MyProject
f.pwd()  # Returns: /

# Navigate within project
f.cd("database")
f.pwd()  # Returns: /database

f.cd("users")
f.pwd()  # Returns: /database/users

f.cd("..")
f.pwd()  # Returns: /database

f.cd("/")
f.pwd()  # Returns: /

# Switch to different project
f.chd("C:/Users/user/Projects/AnotherProject")
f.phd()  # Returns: C:\Users\user\Projects\AnotherProject
f.pwd()  # Returns: /
```

### Database Information
```grapa
# Get database type
db_type = f.type()

# Debug database structure
f.debug()

# Get file info
info = f.info("filename")
```

### File and Directory Information
```grapa
# Get detailed file metadata
file_info = f.info("document.txt")
# Returns: {"$TYPE":"FILE","$BYTES":1024}

# Check if item exists and get type
info = f.info("unknown.txt")
if (info["$TYPE"] == "FILE") {
    "File exists with " + info["$BYTES"] + " bytes\n".echo()
} else if (info["$TYPE"] == "DIR") {
    "Directory exists\n".echo()
} else {
    "Item doesn't exist\n".echo()
}

# Batch file size calculation
files = ["file1.txt", "file2.txt", "file3.txt"]
total_size = 0
i = 0
while (i < files.length()) {
    info = f.info(files[i])
    if (info["$TYPE"] == "FILE") {
        total_size = total_size + info["$BYTES"]
    }
    i = i + 1
}
"Total size: " + total_size + " bytes\n".echo()
```

### File Splitting Operations
```grapa
# Split large file into manageable parts
result = f.split(4, "large_file.txt", "split_output", "\n", "")
# Returns: ["1.large_file.txt","2.large_file.txt","3.large_file.txt","4.large_file.txt"]

# Split CSV file with header preservation
result = f.split(3, "data.csv", "csv_parts", "", "csv")
# Each split file includes the original header

# Split with custom delimiter
result = f.split(2, "custom_data.txt", "output", "|", "")
# Splits on pipe character boundaries

# Smart boundary detection
result = f.split(2, "log_file.txt", "log_parts", "\n", "start")
# Searches backwards for delimiter to avoid breaking content
```

**Split Options**:
- **Basic**: Splits file into equal-sized parts
- **CSV**: Preserves headers in each split file
- **Start**: Searches backwards for delimiter to maintain logical boundaries
- **Custom Delimiters**: Use any character or string as split boundary

## Performance Guidelines

### Row Store Best Practices
- Use for transactional workloads
- Frequent record updates
- Point queries on specific records
- Small to medium datasets

### Column Store Best Practices
- Use for analytical workloads
- Column scans and aggregations
- Large datasets with many columns
- Sparse data (many NULL values)

### Storage Type Selection
- **FIX**: Small, frequently accessed fields
- **VAR**: Medium-sized variable data
- **PAR**: Large data requiring partial updates

## Common Patterns

### Row Store Pattern (Transactional)
```grapa
# Create row store database
f.mk("orders", "ROW")
f.cd("orders")

# Define schema
f.mkfield("order_id", "INT", "FIX", 4)
f.mkfield("customer_id", "INT", "FIX", 4)
f.mkfield("amount", "FLOAT", "FIX", 8)
f.mkfield("status", "STR", "VAR")

# Add records
f.set("order_001", 1001, "customer_id")
f.set("order_001", 299.99, "amount")
f.set("order_001", "pending", "status")
```

### Column Store Pattern (Analytical)
```grapa
# Create column store database
f.mk("sales_analytics", "COL")
f.cd("sales_analytics")

# Define schema
f.mkfield("date", "TIME", "FIX", 8)
f.mkfield("product_id", "INT", "FIX", 4)
f.mkfield("region", "STR", "VAR")
f.mkfield("sales_amount", "FLOAT", "FIX", 8)

# Add data
f.set("sale_001", 1640995200, "date")
f.set("sale_001", 1001, "product_id")
f.set("sale_001", "North", "region")
f.set("sale_001", 299.99, "sales_amount")
```

## Error Handling

### Common Error Patterns
```grapa
# Check for errors
result = f.get("nonexistent")
if (result.error) {
    # Handle error
}

# Safe field creation
try {
    f.mkfield("test", "INT", "FIX", 4)
} catch {
    # Handle field creation error
}
```

### Debug Information
```grapa
# Get detailed database information
f.debug()

# Check database structure
f.ls()

# Verify field creation
f.info("field_name")
```

## Performance Tips

### Optimization Strategies
1. **Choose Right Database Type**: Row vs Column based on workload
2. **Use Appropriate Field Types**: Match data types to storage needs
3. **Optimize Storage Types**: FIX for small data, VAR for variable data
4. **Monitor Performance**: Use debug() function regularly
5. **Plan for Growth**: Consider data growth patterns

### Memory Management
- Column store: Only loads relevant blocks
- Row store: Loads complete records
- Use appropriate storage types for memory efficiency

### Storage Efficiency
- Fixed fields: Predictable storage requirements
- Variable fields: Flexible but with overhead
- Partitioned fields: Efficient for large data updates 

## Python Integration

For Python developers working with Grapa databases, see the [Python Integration Guide](../PYTHON_INTEGRATION.md) for comprehensive examples of:
- Data science workflows with column store databases
- Web application development with row store databases
- System administration and data pipeline integration
- Integration with pandas, NumPy, and machine learning frameworks 