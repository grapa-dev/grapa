# Grapa Language Support for Visual Studio Code

Language support for the Grapa programming language in Visual Studio Code.

## About Grapa

Grapa is a modern programming language that combines the simplicity of scripting languages with the power of systems programming. It features:

- **Unified syntax** for file system, database, and network operations
- **Native support** for JSON, XML, and HTML
- **Built-in cryptography** with RSA, EC, and DH key generation
- **Vector operations** for high-performance computing
- **Cross-platform** support for Windows, macOS, and Linux

## Prerequisites

**This extension requires the Grapa executable to be installed on your system.**

### Installing Grapa

1. **Download Grapa**: Visit the [Grapa releases page](https://github.com/grapa-dev/grapa/releases)
2. **Install for your platform**:
   - **macOS**: Download the `.tar.gz` file and extract to `/usr/local/bin/`
   - **Windows**: Download the `.zip` file and extract to a directory in your PATH
   - **Linux**: Download the `.tar.gz` file and extract to `/usr/local/bin/`
3. **Verify installation**: Open a terminal and run `grapa --version`

For detailed installation instructions, see the [Grapa Installation Guide](https://grapa-dev.github.io/grapa/getting_started/).

## Features

- **Syntax Highlighting**: Color-coded Grapa code for `.grc` files
- **IntelliSense**: Smart autocomplete for Grapa types and methods
- **Code Snippets**: Quick templates for common patterns (40+ snippets)
- **Interactive Terminal**: Run Grapa commands directly in VSCode
- **Code Execution**: Run files, selections, or individual commands
- **Error Detection**: Syntax validation and error highlighting

## Quick Start

1. **Install the extension** from the VSCode marketplace
2. **Open a `.grc` file** - Syntax highlighting activates automatically
3. **Start interactive mode**: `Ctrl+Shift+P` → "Grapa: Start Interactive Terminal"
4. **Try a snippet**: Type `hello` + `Tab` for a Hello World example
5. **Run code**: Select code and press `Ctrl+Shift+S`

## Commands

- **Grapa: Run File** - Execute the current `.grc` file
- **Grapa: Run Selection** - Run selected code
- **Grapa: Start Interactive Terminal** - Open interactive Grapa terminal
- **Grapa: Start Interactive Session** - Start session for code selections

## Documentation

- **[Grapa Language Documentation](https://grapa-dev.github.io/grapa/)** - Complete language reference
- **[Getting Started Guide](https://grapa-dev.github.io/grapa/getting_started/)** - Installation and first steps
- **[Syntax Reference](https://grapa-dev.github.io/grapa/syntax/basic_syntax/)** - Language syntax and examples
- **[API Reference](https://grapa-dev.github.io/grapa/api_reference/)** - Built-in functions and types

## Examples

### Basic Syntax
```grapa
'Hello, Grapa!'.echo();
10.range();  // [0,1,2,3,4,5,6,7,8,9]
```

### File Operations
```grapa
/* Write to file */
$file().set("data.txt", "Hello World");

/* Read from file (returns hex format) */
content = $file().get("data.txt");
content.echo();

/* Read and convert to string */
text = $file().get("data.txt").str();
text.echo();
```

### Cryptography
```grapa
rsa = "rsa".genkeys(2048);
signature = 'Hello'.sign(rsa);
'Hello'.verify(rsa, signature).echo();
```

## Support

- **GitHub Issues**: [Report bugs or request features](https://github.com/grapa-dev/grapa/issues)
- **Documentation**: [Complete Grapa documentation](https://grapa-dev.github.io/grapa/)
- **Community**: [Grapa discussions](https://github.com/grapa-dev/grapa/discussions)

## Requirements

- Visual Studio Code 1.85.0 or higher
- Grapa executable installed on your system

---

**Note**: This extension provides language support for Grapa. The Grapa programming language and runtime are developed separately. Please ensure you have the Grapa executable installed before using this extension.
