# Database & File System Integration

Unified APIs for files and databases boost productivity for backend developers, sysadmins, and data engineers.

## Key Features for File/DB Work:
- **Unified Interface**: Same APIs for files and databases
- **Query Language**: SQL-like syntax for file operations
- **Transaction Support**: ACID compliance for data integrity
- **Parallel Access**: Concurrent read/write operations

## Example: File System Operations
```grapa
/* Recursive file processing */
process_files = op(directory, file_pattern) {
    files = $file().find(directory, file_pattern);
    
    results = files.map(op(file) {
        try {
            content = $file().read(file);
            stats = $file().stat(file);
            
            {
                "file": file,
                "size": stats.get("size"),
                "modified": stats.get("modified"),
                "lines": content.split("\n").len(),
                "words": content.split(" ").len()
            };
        } catch (error) {
            {"file": file, "error": error.get("message")};
        };
    });
    
    results;
};

/* Database-like file queries */
query_files = op(directory) {
    /* Find all Python files larger than 1KB */
    large_python_files = $file().select("* from " + directory + " where name like '*.py' and size > 1024");
    
    /* Group by directory */
    grouped = large_python_files.group(op(file) { $file().dirname(file); });
    
    grouped.map(op(dir, files) {
        {
            "directory": dir,
            "file_count": files.len(),
            "total_size": files.reduce(op(sum, f) { sum + f.get("size"); }, 0)
        };
    });
};
```

## Example: Database Operations
```grapa
/* Create and populate a database table */
create_user_table = op() {
    /* Create table with schema */
    $TABLE().create("users", {
        "id": "INT PRIMARY KEY",
        "name": "STR",
        "email": "STR UNIQUE",
        "created_at": "TIME"
    });
    
    /* Insert sample data */
    users_data = [
        {1, "Alice Johnson", "alice@example.com", $TIME().now()},
        {2, "Bob Smith", "bob@example.com", $TIME().now()},
        {3, "Carol Davis", "carol@example.com", $TIME().now()}
    ];
    
    users_data.map(op(user) {
        $TABLE().insert("users", user);
    });
    
    "User table created and populated".echo();
};

/* Query and process database data */
query_users = op() {
    /* Find users by email domain */
    gmail_users = $TABLE().select("SELECT * FROM users WHERE email LIKE '%@gmail.com'");
    
    /* Aggregate user statistics */
    total_users = $TABLE().select("SELECT COUNT(*) as count FROM users");
    avg_name_length = $TABLE().select("SELECT AVG(LENGTH(name)) as avg_length FROM users");
    
    {
        "gmail_users": gmail_users,
        "total_users": total_users.get(0).get("count"),
        "avg_name_length": avg_name_length.get(0).get("avg_length")
    };
};
```

## Example: File System Monitoring
```grapa
/* Monitor file system changes */
monitor_directory = op(directory, interval_ms) {
    initial_files = $file().find(directory, "*");
    initial_state = {};
    
    /* Create initial file state */
    initial_files.map(op(file) {
        stats = $file().stat(file);
        initial_state.set(file, {
            "size": stats.get("size"),
            "modified": stats.get("modified")
        });
    });
    
    /* Monitor for changes */
    while (true) {
        current_files = $file().find(directory, "*");
        current_state = {};
        
        current_files.map(op(file) {
            stats = $file().stat(file);
            current_state.set(file, {
                "size": stats.get("size"),
                "modified": stats.get("modified")
            });
        });
        
        /* Detect new files */
        new_files = current_files.filter(op(file) { !initial_state.has(file); });
        if (new_files.len() > 0) {
            ("New files detected: " + new_files.join(", ")).echo();
        };
        
        /* Detect modified files */
        modified_files = current_files.filter(op(file) {
            if (initial_state.has(file)) {
                initial = initial_state.get(file);
                current = current_state.get(file);
                initial.get("size") != current.get("size") || 
                initial.get("modified") != current.get("modified");
            } else {
                false;
            };
        });
        
        if (modified_files.len() > 0) {
            ("Modified files: " + modified_files.join(", ")).echo();
        };
        
        /* Update state */
        initial_state = current_state;
        
        /* Wait before next check */
        $sys().sleep(interval_ms);
    };
};
```

## Example: Data Backup and Sync
```grapa
/* Backup database to file system */
backup_database = op(database_name, backup_dir) {
    /* Get all tables */
    tables = $TABLE().tables();
    
    backup_data = {};
    tables.map(op(table) {
        /* Export table data */
        table_data = $TABLE().select("SELECT * FROM " + table);
        backup_data.set(table, table_data);
    });
    
    /* Save backup to file */
    backup_file = backup_dir + "/" + database_name + "_" + $TIME().now().str() + ".json";
    $file().write(backup_file, $JSON().stringify(backup_data));
    
    ("Database backup saved to: " + backup_file).echo();
    backup_file;
};

/* Restore database from backup */
restore_database = op(backup_file) {
    /* Read backup data */
    backup_content = $file().read(backup_file);
    backup_data = $JSON().parse(backup_content);
    
    /* Restore each table */
    backup_data.keys().map(op(table) {
        table_data = backup_data.get(table);
        
        /* Drop existing table if it exists */
        try {
            $TABLE().drop(table);
        } catch (error) {
            /* Table doesn't exist, continue */
        };
        
        /* Recreate table and insert data */
        if (table_data.len() > 0) {
            /* Create table with inferred schema */
            sample_row = table_data.get(0);
            schema = {};
            sample_row.keys().map(op(key) {
                value = sample_row.get(key);
                if (value.type() == "$INT") {
                    schema.set(key, "INT");
                } else if (value.type() == "$FLOAT") {
                    schema.set(key, "FLOAT");
                } else {
                    schema.set(key, "STR");
                };
            });
            
            $TABLE().create(table, schema);
            
            /* Insert data */
            table_data.map(op(row) {
                $TABLE().insert(table, row);
            });
        };
        
        ("Restored table: " + table).echo();
    });
    
    "Database restoration completed".echo();
};
```

- **See also:** [Python File/DB Examples](../PYTHON_USE_CASES.md#6-database-file-system-integration) 