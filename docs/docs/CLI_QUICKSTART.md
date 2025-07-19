---
tags:
  - user
  - highlevel
  - cli
  - quickstart
---

# Grapa CLI Quickstart Guide

> **For High-Level Grapa Programmers:** This guide is for users who want to write and run Grapa scripts (`.grc` files) from the command line.

## Installation

### Step 1: Download and Extract
1. Download the appropriate binary for your system from the `bin/` directory:
   - **macOS (Intel):** `grapa-mac-amd64.tar.gz`
   - **macOS (Apple Silicon):** `grapa-mac-arm64.tar.gz`
   - **Linux (Intel):** `grapa-linux-amd64.tar.gz`
   - **Linux (ARM):** `grapa-linux-arm64.tar.gz`
   - **Windows:** `grapa-win-amd64.zip`

2. Extract the archive to a location of your choice:
   ```bash
   # macOS/Linux
   tar -xzf grapa-mac-amd64.tar.gz
   
   # Windows
   # Extract the .zip file using your preferred tool
   ```

### Step 2: Make Executable (macOS/Linux)
```bash
chmod +x grapa
```

### Step 3: Add to PATH (Optional)
Move the `grapa` binary to a directory in your PATH, or add its location to your PATH:
```bash
# Option 1: Move to /usr/local/bin
sudo mv grapa /usr/local/bin/

# Option 2: Add to PATH in your shell profile
export PATH="$PATH:/path/to/grapa/directory"
```

## Your First Grapa Script

### Step 1: Create a Script File
Create a file named `hello.grc`:
```grapa
/* Simple hello world */
"Hello, Grapa!".echo();

/* Basic arithmetic */
result = 4 * 3;
("4 * 3 = " + result).echo();
```

### Step 2: Run the Script
```bash
grapa -cfile hello.grc
```

**Output:**
```
Hello, Grapa!
4 * 3 = 12
```

## Basic Syntax and Patterns

### Variables and Assignment
```grapa
/* Simple assignment */
name = "Grapa";
count = 42;

/* Array creation */
numbers = [1, 2, 3, 4, 5];

/* List (JSON object) */
config = { "host": "localhost", "port": 8080 };
```

### Control Flow
```grapa
/* If statements */
if (count > 10) {
    "Count is high".echo();
} else {
    "Count is low".echo();
}

/* While loops */
i = 0;
while (i < 5) {
    ("Iteration " + i).echo();
    i = i + 1;
}

/* For loops (using range) */
for (i in range(5)) {
    ("For loop iteration " + i).echo();
}
```

### Functions
```grapa
/* Define a function */
function = op(a, b) {
    return a + b;
};

/* Call the function */
result = function(10, 20);
("Sum: " + result).echo();
```

## File Operations

### Reading Files
```grapa
/* Read entire file */
content = $file().get("data.txt").str();
("File content: " + content).echo();

/* Read file as lines */
lines = $file().get("data.txt").str().split("\n");
for (line in lines) {
    ("Line: " + line).echo();
}
```

### Writing Files
```grapa
/* Write to file */
$file().set("output.txt", "Hello from Grapa!");

/* Append to file */
$file().set("log.txt", "New entry\n", "a");
```

## Data Processing

### Working with Arrays
```grapa
/* Create array */
data = [1, 2, 3, 4, 5];

/* Map operation (parallel by default) */
doubled = data.map(op(x) { return x * 2; });
("Doubled: " + doubled).echo();

/* Filter operation */
evens = data.filter(op(x) { return x % 2 == 0; });
("Evens: " + evens).echo();

/* Reduce operation */
sum = data.reduce(op(acc, x) { return acc + x; }, 0);
("Sum: " + sum).echo();
```

### String Operations
```grapa
text = "Hello, World!";

/* String methods */
upper = text.upper();
lower = text.lower();
length = text.len();

("Upper: " + upper).echo();
("Lower: " + lower).echo();
("Length: " + length).echo();

/* String splitting and joining */
words = text.split(", ");
joined = words.join(" - ");
("Joined: " + joined).echo();
```

## Command Line Options

### Basic Usage
```bash
# Run a script file
grapa -cfile script.grc

# Run with arguments
grapa -cfile script.grc arg1 arg2

# Interactive mode
grapa

# Help
grapa -h
```

### Script Arguments
Access command line arguments in your script:
```grapa
/* Get all arguments */
args = $sys.argv;

/* Get specific argument */
first_arg = args[1];
("First argument: " + first_arg).echo();
```

## Common Patterns

### Configuration Files
```grapa
/* Read JSON config */
config_text = $file().get("config.json").str();
config = config_text.json();

/* Use configuration */
host = config.host;
port = config.port;
("Connecting to " + host + ":" + port).echo();
```

### Error Handling
```grapa
/* Try-catch equivalent */
iferr {
    result = 10 / 0;
    "This won't print".echo();
} {
    ("Error occurred: " + $sys.error).echo();
}
```

### Working with Directories
```grapa
/* List files in directory */
files = $file().ls(".");
for (file in files) {
    if (file.type == "file") {
        ("File: " + file.name).echo();
    } else {
        ("Directory: " + file.name).echo();
    }
}
```

## Next Steps

- **API Reference:** See [Function Quick Reference](FUNCTION_QUICK_REFERENCE.md) for all available functions
- **Examples:** Check out [Examples](EXAMPLES.md) for more complex use cases
- **Python Integration:** If you also use Python, see [Python Integration Guide](PYTHON_INTEGRATION.md)
- **Troubleshooting:** For common issues, see [Troubleshooting](TROUBLESHOOTING.md)

## Tips

- **Parallel Operations:** `map` and `filter` are parallel by default and great for data processing
- **File Extensions:** Use `.grc` extension for Grapa script files
- **Interactive Mode:** Run `grapa` without arguments for interactive experimentation
- **Error Messages:** Grapa provides detailed error messages to help debug issues