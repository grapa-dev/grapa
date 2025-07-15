# Directory Navigation in Grapa

## Syntax Reminders
- Every statement and every block (including after closing braces) must end with a semicolon (`;`).
- Use block comments (`/* ... */`), not line comments (`// ...`).
- To append to arrays, use the `+=` operator (not `.push()` or `.append()`).
- See [Syntax Quick Reference](syntax/basic_syntax.md) for more.

---

## Minimal Example
```grapa
f = $file();
f.chd("C:/Users/user/Projects/MyProject");
f.cd("src");
f.pwd();  /* Returns: /src */
f.phd();  /* Returns: C:\Users\user\Projects\MyProject */
```

---

## Overview

Grapa provides a dual-level directory navigation system that allows for flexible project management and navigation. This system consists of:

1. **Working Directory** (`pwd()`/`cd()`) - Relative navigation within a project
2. **Home Directory** (`phd()`/`chd()`) - Absolute navigation for project switching

## Key Concepts

### Working Directory (Relative Navigation)
- **Purpose**: Navigate within the current project context
- **Scope**: Relative to the current home directory
- **Use Case**: File and database operations within a project
- **Returns**: Relative path (e.g., `/lib`, `/docs/type`)

### Home Directory (Absolute Navigation)
- **Purpose**: Set the base directory for a project
- **Scope**: Absolute system path
- **Use Case**: Project switching and context management
- **Returns**: Absolute path (e.g., `C:\Users\user\Documents\Project`)

## Functions

### `pwd()` - Print Working Directory
Returns the current working directory relative to the home directory.

```grapa
f = $file();
f.pwd()  // Returns: /
f.cd("lib");
f.pwd()  // Returns: /lib
```

### `cd([name])` - Change Working Directory
Changes the current working directory within the home directory context.

**Parameters**:
- `name` (optional): Directory name or path
  - `".."`: Move up one level
  - `"/"`: Move to root of current home directory
  - `"path"`: Move to specific subdirectory

```grapa
f.cd("lib")      // Move to lib subdirectory
f.cd("..")       // Move up one level
f.cd("/")        // Move to root of home directory
f.cd("/docs")    // Move to docs subdirectory
```

### `phd()` - Print Home Directory
Returns the current home directory (absolute path).

```grapa
f.phd()  // Returns: C:\Users\matichuk\Documents\GitHub\grapa
```

### `chd(filesystempath)` - Change Home Directory
Changes the current home directory to a new absolute path.

**Parameters**:
- `filesystempath`: Absolute or relative path to set as new home directory

**Note**: This resets the working directory to the root (`/`) of the new home directory.

```grapa
f.chd("C:/Users/user/NewProject")  // Change to absolute path
f.chd("../sibling_project")        // Change using relative path
```

## Navigation Examples

### Basic Project Navigation
```grapa
f = $file();

// Set project home directory
f.chd("C:/Users/user/Projects/MyProject");
f.phd()  // Returns: C:\Users\user\Projects\MyProject
f.pwd()  // Returns: /

// Navigate within project
f.cd("database");
f.pwd()  // Returns: /database

f.cd("users");
f.pwd()  // Returns: /database/users

f.cd("..");
f.pwd()  // Returns: /database

f.cd("/");
f.pwd()  // Returns: /
```

### Project Switching
```grapa
f = $file();

// Work on first project
f.chd("C:/Users/user/Projects/ProjectA");
f.cd("src");
f.pwd()  // Returns: /src

// Switch to different project
f.chd("C:/Users/user/Projects/ProjectB");
f.pwd()  // Returns: / (reset to root of new project)
f.phd()  // Returns: C:\Users\user\Projects\ProjectB
```

### Database Navigation
```grapa
f = $file();

// Create and navigate database
f.mk("my_database", "ROW");
f.cd("my_database");
f.pwd()  // Returns: /my_database

// Create fields
f.mkfield("id", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");

// Navigate within database
f.cd("users");
f.pwd()  // Returns: /my_database/users
```

## Comparison Table

| Function | Purpose | Scope | Change Method | Example Output |
|----------|---------|-------|---------------|----------------|
| `pwd()` | Show current location | Relative to home | `cd()` | `/lib/grapa` |
| `phd()` | Show base directory | Absolute system | `chd()` | `C:\Users\user\Documents\Project` |

## Best Practices

### 1. Project Organization
```grapa
// Set project home directory
f.chd("C:/Users/user/Projects/MyProject");

// Organize project structure
f.mk("src", "GROUP");
f.mk("docs", "GROUP");
f.mk("data", "COL");
f.mk("config", "GROUP");
```

### 2. Database Management
```grapa
// Create project database
f.mk("project_db", "ROW");
f.cd("project_db");

// Define schema
f.mkfield("id", "INT", "FIX", 4);
f.mkfield("name", "STR", "VAR");
f.mkfield("created", "TIME", "FIX", 8);
```

### 3. File System Operations
```grapa
// Set project directory
f.chd("C:/Users/user/Projects/MyProject");

// Create project files
f.set("README.md", "# My Project\n\nProject description...");
f.set("config.json", '{"version": "1.0", "debug": false}');

// Navigate and list files
f.cd("src");
files = f.ls();
```

## Common Patterns

### Development Workflow
1. **Set Project Context**: Use `chd()` to set project home directory
2. **Navigate Within Project**: Use `cd()` for relative navigation
3. **Monitor Location**: Use `pwd()` to see current position
4. **Switch Projects**: Use `chd()` to change project context

### Database Workflow
1. **Create Database**: Use `mk()` with appropriate type
2. **Navigate Database**: Use `cd()` to move within database structure
3. **Define Schema**: Use `mkfield()` to create fields
4. **Manage Data**: Use `set()`/`get()` for data operations

## Error Handling

### Common Issues
```grapa
// Directory doesn't exist
f.cd("nonexistent")  // May return error

// Invalid home directory
f.chd("invalid/path")  // May return error

// Check current state
f.pwd()  // Verify current working directory
f.phd()  // Verify current home directory
```

### Safe Navigation
```grapa
// Check if directory exists before navigating
files = f.ls();
if (files.length > 0) {
    f.cd("subdirectory");
} else {
    // Handle missing directory
}
```

## Using info() for Safe Navigation

The `info` function returns metadata about a file or directory, such as its type (file, directory, database, etc.), size, and other attributes. This is useful for:
- Checking if a directory exists before navigating
- Determining if an entry is a file or directory
- Making navigation and file operations more robust

### Example: Check Before Navigating
```grapa
f = $file();
dir_info = f.info("my_subdir");
if (dir_info["$TYPE"] == "DIR") {
    f.cd("my_subdir");
};
```

### Example: List and Navigate Only to Directories
```grapa
f = $file();
files = f.ls();
for (i = 0; i < files.len(); i += 1;) {
    entry = files[i];
    entry_info = f.info(entry);
    if (entry_info["$TYPE"] == "DIR") {
        f.cd(entry);
        // Do something in the directory
    };
};
```

Using `info` helps prevent errors and makes your navigation scripts more robust.

## Integration with Other Systems

### File System Integration
- Works with traditional file system directories
- Supports both Windows and Unix-style paths
- Handles path separators automatically

### Database Integration
- Supports GROUP, ROW, and COL database types
- Maintains navigation context within databases
- Allows hierarchical database structures

### Cross-Platform Compatibility
- Windows: Uses backslashes in display, forward slashes in code
- Unix/Linux: Uses forward slashes
- Automatic path normalization

## Summary

The dual-level directory navigation system in Grapa provides:

1. **Flexibility**: Easy project switching and context management
2. **Clarity**: Clear separation between project context and internal navigation
3. **Efficiency**: Relative navigation within projects, absolute navigation between projects
4. **Consistency**: Unified interface for file system and database navigation

This system enables efficient project management and database operations while maintaining clear navigation context. 

---

## Related Topics
- [Syntax Quick Reference](syntax/basic_syntax.md)
- [System Functions](SYSTEM_FUNCTIONS.md)
- [Testing](TESTING.md)
- [Examples](EXAMPLES.md) 