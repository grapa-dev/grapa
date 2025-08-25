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
   - **macOS (Apple Silicon):** `grapa-mac-arm64.tar.gz`
   - **Linux (Intel):** `grapa-linux-amd64.tar.gz`
   - **Linux (ARM):** `grapa-linux-arm64.tar.gz`
   - **Windows:** `grapa-win-amd64.zip`

> **Note:** Grapa requires macOS 15.0+ (Apple Silicon only). Intel Macs are no longer supported due to Apple's discontinuation of Intel Mac support in macOS 15.0.

2. Extract the archive to a location of your choice:
   ```bash
   # macOS/Linux
   tar -xzf grapa-mac-arm64.tar.gz
   
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
# Direct script execution (preferred)
grapa hello.grc
# Or, using the standard flag
grapa -f hello.grc
```

**Output:**
```
Hello, Grapa!
4 * 3 = 12
```

## Debug Options

Grapa provides comprehensive debugging capabilities that can be controlled from the command line or within scripts.

### Command Line Debug
```bash
# Enable debug mode
grapa -d script.grc

# Debug with command execution
grapa -d -c "'hello world'.echo();"

# Debug with stream redirection (separate normal and debug output)
grapa -d script.grc > normal_output.txt 2> debug_output.txt

# Debug with quiet mode (suppress header, keep debug output)
grapa -d -q script.grc 2> debug_output.txt
```

### Script-Based Debug Control
You can also control debug settings dynamically within your scripts:
```grapa
// Enable debug for database operations
$sys().putenv("GRAPA_DEBUG_MODE", "1");
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database");

// Perform operations with debug output
db = $file().table('ROW');
db.mkfield('name', 'STR');

// Disable debug when done
$sys().putenv("GRAPA_DEBUG_MODE", "0");
```

**For complete debugging documentation, see [Debugging in Grapa](debugging.md).**

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
casefold = text.casefold();
length = text.len();

("Upper: " + upper).echo();
("Lower: " + lower).echo();
("Casefold: " + casefold).echo();
("Length: " + length).echo();

/* String splitting and joining */
words = text.split(", ");
joined = words.join(" - ");
("Joined: " + joined).echo();
```

## Command Line Options

### Complete CLI Options Reference

```bash
# Basic Options
grapa [options] [script_or_command]

# Standard Options
-c, --command <script>     Execute Grapa command/script
-f, --file <file>          Execute Grapa script file (.grc or .grz)
-                          Execute command from stdin (standard Python/Node.js behavior)
-q, --quiet                Suppress version header output
-v, --version              Show version information
-h, --help                 Show help message
--verbose                  Show version header (default: hidden)
-i, --interactive          Run in interactive mode
-d, --debug                Enable debug mode (shows debug output)
```

### Basic Usage
```bash
# Direct script execution (automatic .grc/.grz detection)
grapa script.grc
grapa script.grz

# Standard command execution
grapa -c "'Hello, World!'.echo()"

# Standard script execution
grapa -f script.grc

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

### Advanced Command Line Options
```bash
# Execute command from stdin (standard Python/Node.js behavior)
echo "'Hello from stdin!'.echo()" | grapa -

# Execute command
grapa -c "'Hello'.echo()"

# Execute script file
grapa -f script.grc

# Quiet mode (suppress header)
grapa -q -c "'Hello'.echo()"

# Verbose mode (show version header)
grapa --verbose -c "'Hello'.echo()"

# Debug mode (shows debug output)
grapa -d -c "'Hello'.echo()"

# Debug with stream redirection
grapa -d -c "'Hello'.echo()" > normal.txt 2> debug.txt

# Interactive mode
grapa -i
```

## Using Language Capabilities from CLI

### Performance and Timing
```bash
# Performance profiling using built-in timing
grapa -c "
start_time = \$TIME().utc();
/* ... operations ... */
end_time = \$TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
('Performance: ' + elapsed_ms + ' ms').echo();
"

# Parallel processing with built-in capabilities
grapa -c "
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* 8 worker threads */
('Processed ' + squares.len() + ' items in parallel').echo();
"

# Performance optimization with caching
grapa -c "
compiled = \$sys().compile('x * 2 + 1');
result = \$sys().eval(compiled, {'x': 5});
('Optimized result: ' + result).echo();
"
```

### Environment Management
```bash
# Get environment variables
grapa -c "(\$sys().getenv('USERNAME')).echo();"

# Set environment variables
grapa -c "\$sys().putenv('DEBUG_MODE', 'true'); ('DEBUG_MODE set to: ' + \$sys().getenv('DEBUG_MODE')).echo();"

# Get system information
grapa -c "(\$sys().getenv(\$VERSION)).echo();"

# Environment-based configuration
grapa -c "
env = \$sys().getenv('NODE_ENV');
if (env == 'production') {
    'Running in production mode'.echo();
} else {
    'Running in development mode'.echo();
}
"

# Runtime debug configuration
grapa -c "
/* Set debug mode at runtime */
\$sys().putenv('DEBUG_MODE', 'true');
\$sys().putenv('LOG_LEVEL', 'verbose');

/* Check debug mode and adjust behavior */
if (\$sys().getenv('DEBUG_MODE') == 'true') {
    'Debug mode enabled'.echo();
    log_level = \$sys().getenv('LOG_LEVEL');
    ('Log level: ' + log_level).echo();
    
    /* Enable detailed logging */
    start_time = \$TIME().utc();
    'Performing debug operations...'.echo();
    \$sys().sleep(100);  /* Simulate work */
    end_time = \$TIME().utc();
    elapsed = ((end_time - start_time) / 1000000).int();
    ('Debug operation took: ' + elapsed + ' ms').echo();
} else {
    'Debug mode disabled'.echo();
}

/* Runtime switching - turn debug off */
\$sys().putenv('DEBUG_MODE', 'false');
'Debug mode turned off'.echo();
"
```

### Stream Redirection and Debug Output

Grapa properly separates normal program output from debug output, allowing for flexible stream redirection:

```bash
# Separate normal and debug output into different files
grapa -d script.grc > normal_output.txt 2> debug_output.txt

# Capture only normal output (suppress debug)
grapa -d script.grc > normal_output.txt 2> /dev/null

# Capture only debug output (suppress normal output)
grapa -d script.grc > /dev/null 2> debug_output.txt

# Pipe normal output to another command, debug to file
grapa -d script.grc 2> debug.txt | grep "important"

# Debug with quiet mode (suppress header, keep debug output)
grapa -d -q script.grc 2> debug_output.txt
```

**Stream Separation Benefits:**
- **Clean Output**: Normal program results are separate from debug information
- **Flexible Logging**: Can log debug output while processing normal output
- **Pipeline Integration**: Can pipe normal output to other tools
- **Selective Capture**: Choose which streams to capture or suppress

### Output Control and Redirection
```bash
# Shell piping for output control (already works)
grapa -c "'Hello World'.echo()" > output.txt
grapa -c "'Hello World'.echo()" | grep "Hello"
grapa -c "'Hello World'.echo()" >> log.txt

# Process output in scripts
grapa -c "
result = 'Hello World'.echo();
result = result.upper();
result.echo();
"

# Output formatting and control
grapa -c "
data = ['apple', 'banana', 'cherry'];
for (item in data) {
    ('Item: ' + item.upper()).echo();
}
"
```

### Advanced Language Features
```bash
# Dynamic code execution
grapa -c "
template = 'name + \"! You are \" + age.str() + \" years old.\"';
name = 'Alice';
age = 25;
result = op()(template)();
result.echo();
"

# String templates and dynamic construction
grapa -c "
greeting = op('name'=0, 'time'=0){
    'Good ' + time + ', ' + name + '!'
};
greeting('Alice', 'morning').echo();
"

# Error handling with iferr
grapa -c "
iferr {
    result = 10 / 0;
    'This won\'t print'.echo();
} {
    ('Error occurred: ' + \$sys.error).echo();
}
"
```

### Database and File Operations
```bash
# Database operations
grapa -c "
db = \$file().table('ROW');
db.mkfield('name', 'STR');
db.mkfield('age', 'INT');
db.set('user1', 'Alice', 'name');
db.set('user1', 25, 'age');
result = db.get('user1', 'name');
('User name: ' + result).echo();
"

# File operations
grapa -c "
files = \$file().ls('.');
for (file in files) {
    if (file.type == 'file') {
        ('File: ' + file.name).echo();
    }
}
"
```

### Parallel and Concurrent Processing
```bash
# Parallel data processing
grapa -c "
data = (10000).range(0,1);
processed = data.map(op(x) { x * x + 1; }, 4);  /* 4 workers */
('Processed ' + processed.len() + ' items').echo();
"

# Concurrent operations
grapa -c "
tasks = [];
for (i in (5).range(0,1)) {
    task = op(id) {
        \$sys().sleep(100);  /* Simulate work */
        'Task ' + id.str() + ' completed'.echo();
    };
    tasks.append(task);
}

/* Execute tasks concurrently */
for (task in tasks) {
    task(tasks.index(task));
}
"
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

- **API Reference:** See [Function Quick Reference](function_quick_reference.md) for all available functions
- **Examples:** Check out [Examples](examples.md) for more complex use cases
- **Python Integration:** If you also use Python, see [Python Integration Guide](python_integration.md)
- **Troubleshooting:** For common issues, see [Troubleshooting](troubleshooting.md)
- **Command Line Reference:** For detailed CLI options and examples, see [Run](run.md)

## Tips

- **Direct Script Execution:** You can run script files directly without the -f flag: `grapa script.grc`
- **Clean Output:** By default, Grapa runs without showing version headers (like Python/Node.js)
- **Verbose Mode:** Use `--verbose` to show version headers when needed
- **Parallel Operations:** `map` and `filter` are parallel by default and great for data processing
- **File Extensions:** Use `.grc` extension for Grapa script files, `.grz` for compiled scripts
- **Interactive Mode:** Run `grapa` without arguments for interactive experimentation
- **Error Messages:** Grapa provides detailed error messages to help debug issues
- **Debug Mode:** Use `-d` to see detailed debug information about CLI operations

## Known Limitations

- **Error Handling:** Invalid syntax or missing files are handled silently without error messages
> **Full CLI options:**
> - See [Run](run.md) for all parameters and examples
> - Or run `grapa -h` for the latest help

---

## See also
- [API Reference](api_reference.md)
- [Language Reference](syntax/basic_syntax.md)
- [Examples](examples/)
- [Advanced Topics](advanced_topics.md)