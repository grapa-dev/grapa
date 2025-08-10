# Grapa Execution Pipeline Overview

## Introduction

This document provides a high-level overview of Grapa's complete execution pipeline, from source text to final results. Understanding this pipeline is essential for debugging, optimization, and extending the Grapa language.

## Complete Pipeline Flow

```
Source Text → Lexer → Parser → Executor → Results
     ↓         ↓        ↓         ↓         ↓
   "x + 5" → Tokens → $OP Tree → Execute → 8
```

### **Stage 1: Lexer (Text → Tokens)**

The lexer converts raw text input into a stream of tokens:

**Input**: `"x + 5"`
**Output**: `[IDENTIFIER:x] [SPACE: ] [OPERATOR:+] [SPACE: ] [NUMBER:5]`

**Key Features**:
- **Token Classification**: Identifies tokens by type (identifier, operator, number, etc.)
- **State Machine**: Handles complex token patterns (strings, comments, etc.)
- **Quote Analysis**: Detects syntax issues with quote characters
- **Performance Tracking**: Measures tokenization speed

**Debug Component**: `lexer`
**Usage**: `GRAPA_SESSION_DEBUG_COMPONENTS=lexer`

### **Stage 2: Parser (Tokens → Execution Trees)**

The parser converts tokens into execution trees using BNF grammar rules:

**Input**: `[IDENTIFIER:x] [OPERATOR:+] [NUMBER:5]`
**Output**: `$OP(add, [$VAR(x), $CONST(5)])`

**Key Features**:
- **Grammar Rules**: Uses BNF grammar for syntax analysis
- **Left Recursion**: Handles complex recursive expressions
- **Error Context**: Provides detailed error information
- **Tree Construction**: Builds optimized execution trees

**Debug Component**: `parser`
**Usage**: `GRAPA_SESSION_DEBUG_COMPONENTS=parser`

### **Stage 3: Executor (Execution Trees → Results)**

The executor runs the execution trees to produce final results:

**Input**: `$OP(add, [$VAR(x), $CONST(5)])`
**Output**: `8` (assuming x = 3)

**Key Features**:
- **Operation Execution**: Executes $OP and $CODE nodes
- **Library Loading**: Dynamically loads required libraries
- **Function Calls**: Invokes Grapa functions and methods
- **Scope Management**: Handles variable scoping and binding
- **Performance Metrics**: Tracks execution timing

**Debug Component**: `executor`
**Usage**: `GRAPA_SESSION_DEBUG_COMPONENTS=executor`

## Debug Component Shorthands

For convenience, Grapa provides shorthand debug components:

### **Compiler Component** (`compiler`)
- **Scope**: Stages 1-2 (Lexer + Parser)
- **Usage**: `GRAPA_SESSION_DEBUG_COMPONENTS=compiler`
- **Equivalent**: `GRAPA_SESSION_DEBUG_COMPONENTS=lexer,parser`

### **Runtime Component** (`runtime`)
- **Scope**: All stages (Lexer + Parser + Executor)
- **Usage**: `GRAPA_SESSION_DEBUG_COMPONENTS=runtime`
- **Equivalent**: `GRAPA_SESSION_DEBUG_COMPONENTS=compiler,executor`

## Complete Example

Here's a complete example showing the full pipeline:

### **Source Code**
```grapa
x = 3;
result = x + 5;
result.echo();
```

### **Pipeline Execution**

#### **Stage 1: Lexer**
```
[DEBUG-SESSION-1-lexer] LEX: Created token type=4 value='x' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value='=' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=8 value='3' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=';' quote=
```

#### **Stage 2: Parser**
```
[DEBUG-SESSION-1-parser] PARSE: Full token stream: [4:'x'] [10:'='] [8:'3'] [10:';']
[DEBUG-SESSION-1-parser] PARSE: Rule matching - assignment statement
[DEBUG-SESSION-1-parser] PARSE: Execution tree built - $OP(assign, [$VAR(x), $CONST(3)])
```

#### **Stage 3: Executor**
```
[DEBUG-SESSION-1-executor] EXEC: Starting execution - operation type=21
[DEBUG-SESSION-1-executor] EXEC: Processing operation type=21 (CODE=22, OP=21)
[DEBUG-SESSION-1-executor] EXEC: Library execution - hasLibrary=true
[DEBUG-SESSION-1-executor] EXEC: Function call - libParam='assign'
[DEBUG-SESSION-1-executor] EXEC: Execution completed - elapsed time=15.00 microseconds, hasResult=true
```

### **Final Output**
```
8
```

## Performance Characteristics

### **Compilation Performance**
- **Lexer**: Typically 1-10 microseconds per token
- **Parser**: Typically 10-100 microseconds per statement
- **Total Compilation**: Usually under 1 millisecond for simple scripts

### **Execution Performance**
- **Simple Operations**: 1-10 microseconds
- **Function Calls**: 10-100 microseconds
- **Library Operations**: 100-1000 microseconds
- **Complex Scripts**: Varies by complexity

### **Debug Overhead**
- **Minimal Impact**: `ShouldDebug()` checks prevent unnecessary output
- **Component Filtering**: Only enabled components generate output
- **Level Filtering**: Granular control over verbosity
- **Session Isolation**: No interference between parallel sessions

## Architecture Insights

### **Async Pipeline Design**
The lexer and parser operate as separate, parallel queues:
```
Raw Input → Lexer Queue → Token Queue → Parser Queue → Execution Tree
```

### **Session Context**
- Each session has isolated debug context
- Session IDs prevent output interference
- Context passed to lexer at creation time
- Multi-processing support for parallel execution

### **Error Handling**
- **Lexer Errors**: Token-level syntax issues
- **Parser Errors**: Grammar and structure issues
- **Executor Errors**: Runtime and execution issues
- **Debug Context**: Detailed error information at each stage

## Best Practices

### **Debug Usage**
1. **Start with `runtime`**: Use `GRAPA_SESSION_DEBUG_COMPONENTS=runtime` for complete visibility
2. **Filter by component**: Use specific components (`lexer`, `parser`, `executor`) for targeted debugging
3. **Adjust levels**: Use debug levels (1-5) for appropriate verbosity
4. **Performance monitoring**: Use Level 5 for performance analysis

### **Development Workflow**
1. **Syntax Issues**: Use `lexer` component to identify token problems
2. **Grammar Issues**: Use `parser` component to debug rule matching
3. **Runtime Issues**: Use `executor` component to trace execution flow
4. **Performance Issues**: Use Level 5 debug for timing analysis

### **Production Considerations**
- Disable debug output in production for maximum performance
- Use session-specific debug flags for targeted debugging
- Monitor debug output volume to prevent performance impact
- Use component filtering to focus on specific areas

## Conclusion

Grapa's execution pipeline provides comprehensive visibility into the entire process from source text to final results. The debug system offers granular control over what information is displayed, ensuring minimal performance impact while providing powerful debugging capabilities.

Understanding this pipeline is essential for:
- **Debugging**: Identifying issues at the correct stage
- **Optimization**: Finding performance bottlenecks
- **Extension**: Adding new language features
- **Troubleshooting**: Resolving complex execution issues

The modular design allows for targeted debugging while maintaining the ability to view the complete execution flow when needed. 