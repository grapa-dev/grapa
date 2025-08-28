# Run from shell
From Windows
```
grapa.exe [parameters]
```

From Linux
```
./grapa [parameters]
```

To exit while in the console, enter a '.' character.

## Interactive Mode

When running Grapa in interactive mode (`-i` flag), you can enter commands line by line. The interactive console provides several features for multi-line input:

### Line Continuation with Backslash
Use `\` (backslash) at the end of a line to continue the command on the next line:

```grapa
grapa: />a = \
5

grapa: />a
5
```

This allows you to enter multi-line commands without executing each line separately.

### Multi-line Code Blocks
If using paste to enter a block of code to run, ensure each line is not larger than the maximum line length for the console - break up the code into shorter lines.

If using paste to enter multiple lines that need to be run at the same time (such as when a single command spans multiple lines), enter `$[` sequence first and `$]` sequence at the end. This will trigger the lexical engine to signal to the grammar engine to pause grammar analysis until after all the tokens have been generated for the input stream. 

# Command line options

Once copied into a location (and PATH updated if needed), you'll be able to run the tool using "grapa" from a terminal or command shell from any location. Running the tool with "grapa --env" will display a few of the directories where the tool will look for information.

Other command line options:
```
        -h,--help       :Show this help message
        -v,--version    :Show version
        -q,--quiet      :Suppress header
        --verbose       :Show version header (default: hidden)
        -i,--interactive:Run in interactive mode
        -d,--debug      :Enable debug mode (shows debug output)
        -c <script>     :Execute command/script
        -f <file>       :Execute file
        -               :Execute from stdin (standard Python/Node.js behavior)
```

Example: Performs a SHAKE256 hash of the string "thisisatest".
```
grapa -c "'thisisatest'.encode('SHAKE256')" -q
0x94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```

Example: Same, but takes input from stdin. 
```
echo "thisisatest".encode('SHAKE256') | grapa - -q
0x94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```

Example: Debug mode with stream redirection (separate normal and debug output)
```
grapa -d -c "'hello world'.echo()" > normal.txt 2> debug.txt
```

**Note:** The `-` option requires pipe input. Using `grapa -` without pipe input will show an error message.

**Note:** By default, Grapa runs without showing version headers (like Python/Node.js). Use `--verbose` to show version headers when needed.

# Testing

Grapa includes a comprehensive test suite for validating functionality and performance. All tests are organized in the `test/` directory.

## Running Tests

### Complete Test Suite
Run all tests in one command:
```bash
# Windows
.\grapa.exe -f "test/infrastructure/run_tests.grc"

# Linux/Mac
./grapa -f "test/infrastructure/run_tests.grc"
```

### Individual Test Categories
```bash
# Basic examples
.\grapa.exe -f "examples/basic_example.grc"

# Advanced examples
.\grapa.exe -f "examples/advanced_example.grc"

# Performance examples
.\grapa.exe -f "examples/performance_example.grc"
```

### Regression Testing
For development and CI/CD:
```bash
# Quick regression test
.\grapa.exe -f "test/infrastructure/run_tests.grc"
```

For detailed testing information, see [Testing Documentation](testing.md).

# Editor
Grapa includes a widget library that can be used to create GUIs. The widget system is available through the `$WIDGET` type in Grapa scripts.

**Note:** The `-w` and `-wfile` CLI options have been removed from the main Grapa executable. GUI functionality is now available through the widget system in scripts.

For editor functionality, you can create custom GUI applications using the `$WIDGET` type. See the [Widget Documentation](type/widget.md) for details.
