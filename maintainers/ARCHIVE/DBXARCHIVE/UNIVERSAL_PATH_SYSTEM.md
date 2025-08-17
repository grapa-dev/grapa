# Universal Path System Implementation

## Overview

The Universal Path System is a fundamental architectural component that enables seamless navigation across different storage types using a unified path syntax. This system allows users to traverse file systems, databases, and future storage types (network, cloud, etc.) using consistent path operations.

## Vision

A single path syntax that works across all storage types, enabling operations like:
```
$unified().cd("file:///home/user/documents")
$unified().cd("grapadb://database.db/table1")
$unified().cd("network://server.com/path")
$unified().cd("file:///home/user/documents/../database.db/table1")  // Cross-storage navigation
```

## Current State Analysis

### Existing Components

1. **GrapaUnifiedLocalDatabase**: Base class for unified storage operations
2. **ParseStorageUrl**: Basic URL parsing for database storage types
3. **DirectorySwitch**: Context switching within a single storage type
4. **Storage Type Routing**: Basic routing based on storage type

### Current Limitations

1. **Path Parsing**: Limited to database URLs (`grapadb://`, `grapadbx://`)
2. **Cross-Storage Navigation**: No mechanism to navigate between different storage types
3. **Path Normalization**: No unified path normalization across platforms
4. **Context Management**: Directory context doesn't translate across storage boundaries

## Architectural Requirements

### 1. Universal Path Format

**Standard Format**: `[storage://][host/][path]`

**Examples**:
- `file:///home/user/documents` (Unix/Linux file system)
- `file://C:\Users\user\Documents` (Windows file system)
- `grapadb://database.db/table1` (GrapaDB database)
- `grapadbx://database.dbx/table1` (GrapaDBX database)
- `network://server.com/path` (Future network storage)
- `cloud://bucket/path` (Future cloud storage)

### 2. Path Resolution System

**Components**:
- **Path Parser**: Parse any path format and extract components
- **Storage Type Detector**: Determine appropriate storage handler
- **Path Normalizer**: Normalize paths for cross-platform compatibility
- **Context Manager**: Maintain directory context across storage types

### 3. Cross-Storage Navigation

**Requirements**:
- Navigate from file system to database seamlessly
- Handle relative paths (`..`, `.`) across storage boundaries
- Maintain context when switching between storage types
- Support compound paths that span multiple storage types

## Home Directory Concept ("Home"/"Root")

A fundamental feature of the universal path system is the concept of a **home directory** (sometimes called "root"). This is the logical base directory for all navigation and IO operations, and is distinct from the OS root or the shell's current working directory. The home directory provides both usability and security benefits:

### Purpose
- **Navigation Base**: All relative paths are resolved from "home" unless an absolute path is specified. This allows scripts and users to work in a consistent, predictable environment regardless of where Grapa was launched.
- **User/Session Isolation**: Each user, session, or context can have its own home, supporting multi-user and sandboxed environments.

### API and Implementation
- **gwh() / cwh()**: In the `$file` backend, functions like `gwh()` (get working home) and `cwh()` (change working home) manage the home directory. These allow scripts to query and set the home directory at runtime.
- **Universal Requirement**: The universal path system (and `$unified`) must implement equivalent logic, exposing APIs to get/set home for all storage types (file, database, network, etc.).
- **cd and Path Resolution**: The `cd` command and all path resolution must operate within the confines of home. Relative paths are always relative to home, and absolute paths are only allowed if permitted by policy.

### Security Implications
- **Boundary Enforcement**: The home directory acts as a security boundary. IO operations (read, write, delete, list, etc.) are confined to the subtree rooted at home. Attempts to access data outside home (e.g., via `..` or absolute paths) are denied unless explicitly allowed.
- **Configurable and Lockable**: The home directory can be set at startup, per session, or per user. It can be locked down to prevent changes, supporting secure deployments and sandboxing.
- **Audit and Policy**: All navigation and IO operations should be auditable for attempts to escape home. Policies can be enforced to restrict or allow such operations as needed.

### Cross-Storage Consistency
- The home concept applies to all storage types:
  - **File System**: Home is a directory on disk.
  - **Database**: Home is a root table/group or namespace.
  - **Network/Cloud**: Home is a base URI, bucket, or namespace.
- The universal path system must provide a consistent API for home management across all backends.

### Example Usage
- `cd('subdir')` navigates within home.
- `cd('..')` cannot escape home; if at home, remains at home.
- `gwh()` returns the current home directory.
- `cwh('/new/home')` sets a new home (if allowed by policy).
- All IO (`ls`, `rm`, `create`, etc.) is confined to home by default.

### Future Enhancements
- **Security Hooks**: Integrate with authentication/authorization to set and lock home per user/session.
- **Policy Engine**: Allow fine-grained control over what operations are allowed outside home (if any).
- **Audit Logging**: Log all attempts to access data outside home for security review.

---

*This section should be referenced by all maintainers and contributors working on navigation, path resolution, and security in the universal path system.*

## Implementation Strategy

### Phase 1: Enhanced Path Parsing

**File**: `source/grapa/GrapaDatabaseTemplate.cpp`

**New Methods**:
```cpp
class GrapaUnifiedLocalDatabase {
    // Universal path parsing
    GrapaError ParseUniversalPath(const GrapaCHAR& path, 
                                 GrapaCHAR& storageType, 
                                 GrapaCHAR& host, 
                                 GrapaCHAR& path);
    
    // Storage type detection
    GrapaError DetectStorageType(const GrapaCHAR& path, GrapaCHAR& storageType);
    
    // Path normalization
    GrapaError NormalizePath(GrapaCHAR& path);
    
    // Cross-storage context management
    GrapaError SwitchStorageContext(const GrapaCHAR& newPath);
};
```

### Phase 2: Universal Directory Operations

**Enhanced Operations**:
```cpp
// Universal cd - works across storage types
GrapaError UniversalDirectorySwitch(const GrapaCHAR& path);

// Universal ls - lists contents regardless of storage type
GrapaError UniversalDirectoryList(const GrapaCHAR& path, GrapaRuleEvent* pTable);

// Universal rm - deletes across storage types
GrapaError UniversalDataDelete(const GrapaCHAR& path);
```

### Phase 3: Context Management

**Context Structure**:
```cpp
struct UniversalContext {
    GrapaCHAR storageType;    // Current storage type
    GrapaCHAR host;           // Current host/server
    GrapaCHAR path;           // Current path within storage
    u64 dirId;               // Database-specific context
    u8 dirType;              // Database-specific type
    GrapaCHAR filePath;      // File system path
};
```

## Technical Implementation Details

### 1. Path Parser Implementation

**Algorithm**:
1. **Protocol Detection**: Extract storage type from `storage://` prefix
2. **Host Extraction**: Parse host/server component if present
3. **Path Extraction**: Extract the actual path component
4. **Normalization**: Convert path separators and resolve relative paths

**Example**:
```
Input: "grapadb://database.db/table1/field1"
Output: 
  storageType: "grapadb"
  host: ""
  path: "database.db/table1/field1"
```

### 2. Storage Type Router

**Routing Logic**:
```cpp
if (storageType == "file") {
    return FileSystemHandler(path);
} else if (storageType == "grapadb") {
    return GrapaDBHandler(path);
} else if (storageType == "grapadbx") {
    return GrapaDBXHandler(path);
} else if (storageType == "network") {
    return NetworkHandler(path);
}
```

### 3. Cross-Storage Navigation

**Compound Path Handling**:
```
Input: "file:///home/user/../database.db/table1"
Processing:
1. Parse "file:///home/user/" (file system)
2. Navigate to parent directory
3. Switch to "grapadb://database.db/table1" (database)
4. Maintain context for both storage types
```

## File System Integration

### Platform-Specific Path Handling

**Windows**:
- Path separator: `\`
- Drive letters: `C:\`
- UNC paths: `\\server\share\path`

**Unix/Linux/macOS**:
- Path separator: `/`
- Root: `/`
- Home directory: `~`

**Cross-Platform Normalization**:
```cpp
GrapaError NormalizePath(GrapaCHAR& path) {
    // Convert Windows separators to Unix
    path.Replace("\\", "/");
    
    // Handle drive letters
    if (path.mLength >= 2 && path.mBytes[1] == ':') {
        // Convert C:\path to /C/path
        path.Insert(0, "/");
        path.mBytes[1] = path.mBytes[0];
        path.mBytes[0] = '/';
    }
    
    // Resolve relative paths
    ResolveRelativePaths(path);
    
    return 0;
}
```

## Database Integration

### GrapaDB/GrapaDBX Path Handling

**Database Path Format**:
```
grapadb://database.db/table1/field1
grapadbx://database.dbx/table1/field1
```

**Path Components**:
- Database file: `database.db` or `database.dbx`
- Table: `table1`
- Field/Record: `field1`

**Context Translation**:
```cpp
// Convert database path to internal context
GrapaError DatabasePathToContext(const GrapaCHAR& path, 
                                u64& dirId, u8& dirType) {
    // Parse database path
    GrapaCHAR dbFile, tableName, fieldName;
    ParseDatabasePath(path, dbFile, tableName, fieldName);
    
    // Open database and navigate to table
    GrapaError err = OpenDatabase(dbFile);
    if (err) return err;
    
    err = NavigateToTable(tableName, dirId, dirType);
    return err;
}
```

## Network Storage Integration (Future)

### Network Path Format

**Proposed Format**:
```
network://server.com:port/path
network://user@server.com/path
network://protocol://server.com/path
```

**Implementation Considerations**:
- Authentication handling
- Connection management
- Protocol-specific operations (FTP, SFTP, HTTP, etc.)
- Caching and performance optimization

## Error Handling and Recovery

### Path Resolution Errors

**Common Error Scenarios**:
1. **Invalid Storage Type**: Unknown storage protocol
2. **Connection Failure**: Network storage unavailable
3. **Permission Denied**: Insufficient access rights
4. **Path Not Found**: Resource doesn't exist
5. **Cross-Storage Error**: Cannot navigate between storage types

**Error Recovery Strategies**:
```cpp
GrapaError HandlePathError(GrapaError err, const GrapaCHAR& path) {
    switch (err) {
        case PATH_NOT_FOUND:
            // Try alternative path formats
            return TryAlternativePath(path);
            
        case PERMISSION_DENIED:
            // Prompt for credentials or fallback
            return HandlePermissionError(path);
            
        case STORAGE_UNAVAILABLE:
            // Switch to local storage or cache
            return HandleStorageUnavailable(path);
    }
    return err;
}
```

## Performance Considerations

### Caching Strategy

**Context Caching**:
- Cache frequently accessed storage contexts
- Maintain connection pools for network storage
- Cache parsed path components

**Optimization Techniques**:
- Lazy loading of storage handlers
- Connection pooling for network storage
- Path component caching

### Memory Management

**Context Cleanup**:
- Proper cleanup of storage connections
- Memory deallocation for cached contexts
- Resource cleanup on storage type switches

## Testing Strategy

### Unit Tests

**Path Parsing Tests**:
```cpp
TEST(UniversalPathSystem, ParseFilePath) {
    GrapaCHAR path("file:///home/user/documents");
    GrapaCHAR storageType, host, pathComponent;
    
    GrapaError err = ParseUniversalPath(path, storageType, host, pathComponent);
    
    EXPECT_EQ(err, 0);
    EXPECT_EQ(storageType, "file");
    EXPECT_EQ(host, "");
    EXPECT_EQ(pathComponent, "/home/user/documents");
}
```

### Integration Tests

**Cross-Storage Navigation Tests**:
```cpp
TEST(UniversalPathSystem, CrossStorageNavigation) {
    GrapaUnifiedLocalDatabase db;
    
    // Navigate from file system to database
    GrapaError err = db.UniversalDirectorySwitch("file:///home/user/../database.db/table1");
    
    EXPECT_EQ(err, 0);
    EXPECT_EQ(db.GetCurrentStorageType(), "grapadb");
    EXPECT_EQ(db.GetCurrentPath(), "database.db/table1");
}
```

## Migration Strategy

### Backward Compatibility

**Existing Code Compatibility**:
- Maintain existing `$file()` and `$unified()` interfaces
- Gradual migration to universal path system
- Deprecation warnings for old path formats

**Migration Path**:
1. **Phase 1**: Implement universal path system alongside existing code
2. **Phase 2**: Add migration utilities for existing code
3. **Phase 3**: Deprecate old path formats with warnings
4. **Phase 4**: Remove old path format support

## Documentation Requirements

### User Documentation

**Path Format Guide**:
- Complete path format specification
- Examples for each storage type
- Cross-storage navigation examples
- Error handling and troubleshooting

**API Documentation**:
- Universal path system API reference
- Migration guide from existing code
- Performance optimization tips

### Developer Documentation

**Implementation Guide**:
- Architecture overview
- Extension points for new storage types
- Testing and debugging guidelines
- Performance profiling tools

## Future Enhancements

### Planned Features

1. **Cloud Storage Integration**: AWS S3, Google Cloud Storage, Azure Blob Storage
2. **Network Protocol Support**: FTP, SFTP, WebDAV, REST APIs
3. **Compression and Encryption**: Transparent compression and encryption
4. **Distributed Storage**: Multi-storage synchronization
5. **Version Control Integration**: Git-like versioning across storage types

### Research Areas

1. **Performance Optimization**: Advanced caching and prefetching
2. **Security**: Authentication and authorization across storage types
3. **Scalability**: Handling large-scale distributed storage
4. **Usability**: Enhanced user interface for path navigation

## Conclusion

The Universal Path System represents a fundamental architectural enhancement that will enable seamless navigation across diverse storage types. This system will provide users with a consistent, powerful interface for managing data across file systems, databases, and future storage technologies.

The implementation requires careful consideration of cross-platform compatibility, performance optimization, and extensibility for future storage types. The phased implementation approach ensures backward compatibility while enabling gradual migration to the new universal path system. 