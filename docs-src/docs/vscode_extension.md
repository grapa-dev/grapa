# VSCode Extension

The Grapa VSCode extension provides comprehensive language support for Grapa programming, making it easy to write, run, and debug Grapa code directly in Visual Studio Code.

## Installation

### From Visual Studio Marketplace
1. Open VSCode
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "Grapa Programming Language"
4. Click Install

### Prerequisites
- **Grapa executable** must be installed on your system
- See [Installation Guide](../installation.md) for Grapa setup instructions

## Features

### Syntax Highlighting
- **Color-coded Grapa code** for `.grc` files
- **Proper highlighting** for Grapa-specific syntax
- **Support for** strings, numbers, comments, operators, and system types

### IntelliSense & Autocomplete
- **Smart code completion** for Grapa types and methods
- **Context-aware suggestions** based on current code
- **Hover information** with detailed documentation

### Code Snippets
Over 40 pre-built snippets for common Grapa patterns:

#### **Control Flow**
- `if` - If statement
- `ifelse` - If-else statement  
- `while` - While loop
- `for` - For loop

#### **File Operations**
- `read` - File reading
- `write` - File writing
- `exists` - File existence check
- `ls` - Directory listing
- `cd` - Change directory

#### **Cryptography**
- `rsa` - RSA key generation
- `sign` - RSA signing
- `verify` - RSA verification
- `ec` - EC key generation
- `dh` - DH key generation
- `hash` - Hash encoding

#### **Data Structures**
- `array` - Array creation
- `json` - JSON object
- `vector` - Vector creation
- `function` - Function definition

#### **Error Handling**
- `iferr` - Error handling with fallback
- `ifnull` - Null value handling

### Interactive Execution
Two modes for running Grapa code:

#### **Interactive Terminal**
- **Fully interactive** - Type commands directly
- **Real-time execution** - See results immediately
- **Variable persistence** - Variables persist between commands

#### **Code Selection Execution**
- **Run selected code** - Execute highlighted code
- **Variable persistence** - Variables persist between runs
- **Multi-line support** - Execute complex code blocks

### Error Detection
- **Syntax validation** - Real-time error checking
- **Error highlighting** - Visual error indicators
- **Debug information** - Detailed error messages

## Quick Start

### 1. Open a Grapa File
```bash
# Create a new .grc file
touch hello.grc
```

### 2. Write Some Code
```grapa
'Hello, Grapa!'.echo();
10.range().echo();
```

### 3. Run the Code
- **Press Ctrl+Shift+S** to run selected code
- **Press Ctrl+Shift+R** to run the entire file
- **Use Ctrl+Shift+P** → "Grapa: Start Interactive Terminal" for REPL mode

## Commands

### Available Commands
- **Grapa: Run File** - Execute current `.grc` file
- **Grapa: Run Selection** - Run selected code
- **Grapa: Run Command** - Execute custom command
- **Grapa: Start Interactive Terminal** - Open interactive terminal
- **Grapa: Start Interactive Session** - Start session for code selections
- **Grapa: Restart Session** - Restart interactive session
- **Grapa: Interrupt Session** - Stop current execution
- **Grapa: Debug File** - Run with debug mode

### Keyboard Shortcuts
- **Ctrl+Shift+R** - Run current file
- **Ctrl+Shift+S** - Run selected code
- **Ctrl+Shift+I** - Interrupt current command

## Configuration

### Extension Settings
Configure the extension behavior in VSCode settings:

```json
{
  "grapa.executablePath": "./grapa",
  "grapa.debugMode": false,
  "grapa.autoRunOnSave": false,
  "grapa.preferInteractiveMode": true,
  "grapa.sessionTimeout": 30000,
  "grapa.autoEscapeLineFeeds": true,
  "grapa.showInteractiveOutput": true
}
```

### Setting Descriptions
- **`executablePath`** - Path to Grapa executable
- **`debugMode`** - Enable debug output
- **`autoRunOnSave`** - Automatically run files when saved
- **`preferInteractiveMode`** - Use interactive mode for selections
- **`sessionTimeout`** - Timeout for interactive commands
- **`autoEscapeLineFeeds`** - Auto-escape line feeds in multi-line code
- **`showInteractiveOutput`** - Show output in interactive mode

## Usage Examples

### Basic Syntax Highlighting
```grapa
/* This is a comment */
'Hello, Grapa!'.echo();

/* File operations */
$file().set("data.txt", "Hello World");
content = $file().get("data.txt");
content.echo();
```

### Using Snippets
1. **Type snippet prefix** (e.g., `rsa`)
2. **Press Tab** to expand
3. **Fill in placeholders** as needed

Example with RSA snippet:
```grapa
// Type 'rsa' + Tab
rsa = "rsa".genkeys(2048);
signature = 'Hello'.sign(rsa);
'Hello'.verify(rsa, signature).echo();
```

### Interactive Mode
```grapa
// Start interactive terminal
grapa: /> 'Hello, Grapa!'.echo();
Hello, Grapa!
grapa: /> 10.range();
[0,1,2,3,4,5,6,7,8,9]
grapa: /> x = 5;
grapa: /> x * 2;
10
```

### Multi-line Code Execution
```grapa
// Select this block and press Ctrl+Shift+S
if (true) {
    'This is a multi-line block'.echo();
    'It will execute as a single unit'.echo();
}
```

## Troubleshooting

### Common Issues

#### **Extension Not Loading**
- Check VSCode developer console for errors
- Verify `.grc` files are recognized as Grapa files
- Restart VSCode after installation

#### **Grapa Executable Not Found**
- Verify Grapa is installed and in PATH
- Check `grapa.executablePath` setting
- Test `grapa --version` in terminal

#### **Interactive Mode Not Working**
- Use "Grapa: Start Interactive Terminal" for manual typing
- Use "Grapa: Start Interactive Session" for code selections
- Check executable path configuration

#### **Snippets Not Working**
- Type snippet prefix and press Tab
- Check for syntax errors in snippet definitions
- Verify snippet file is properly loaded

### Debug Mode
Enable debug mode for detailed logging:
```json
{
  "grapa.debugMode": true
}
```

Check the Output panel (View → Output → Grapa) for debug information.

## Integration with Other Tools

### Git Integration
- **Version control** for `.grc` files
- **Diff highlighting** for Grapa syntax
- **Blame information** for code history

### Terminal Integration
- **Integrated terminal** with Grapa support
- **Command execution** directly from VSCode
- **Output capture** and display

### Debugging
- **Breakpoint support** for Grapa code
- **Variable inspection** during execution
- **Call stack** and error tracking

## Best Practices

### Code Organization
- **Use `.grc` extension** for Grapa files
- **Organize code** in logical functions
- **Add comments** for complex logic

### Interactive Development
- **Use interactive terminal** for experimentation
- **Use code selection** for testing specific parts
- **Leverage snippets** for common patterns

### Error Handling
- **Use `iferr()`** for error handling
- **Use `ifnull()`** for null value handling
- **Enable debug mode** for troubleshooting

## Support

### Getting Help
- **GitHub Issues** - Report bugs and request features
- **Documentation** - Complete Grapa language reference
- **Community** - Grapa discussions and support

### Resources
- **[Grapa Language Documentation](../index.md)** - Complete language reference
- **[Installation Guide](../installation.md)** - Setup instructions
- **[Examples](../examples.md)** - Code examples and patterns
- **[API Reference](../api_reference.md)** - Built-in functions and types

---

**Extension Version**: 0.1.0  
**Publisher**: Chris Matichuk  
**Marketplace**: [Grapa Programming Language](https://marketplace.visualstudio.com/)
