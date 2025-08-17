---
tags:
  - maintainer
  - implementation
  - core
---

# Core Implementation Components

This directory contains the fundamental implementation details for Grapa's core system components.

## Overview

Core components provide the foundation for Grapa's execution model, type system, and system integration.

## Components

### System Functions
- [System Functions](SYSTEM_FUNCTIONS.md) - Low-level system APIs and functions

### Type System
- [Base Types](BASETYPES.md) - Core type system implementation

### Execution Model
- [Execution Trees](EXECUTION_TREES.md) - Execution model and compilation
- [Compilation and Execution](GRAPA_COMPILATION_AND_EXECUTION.md) - Core compilation process, left recursion solution, and operator precedence

### Module System
- [Include System](INCLUDE_SYSTEM.md) - Module system and code organization
- [Module System Implementation](MODULE_SYSTEM_IMPLEMENTATION.md) - Complete module system with $PATH, $LIB, and class loading

### Binary Format
- [GRZ Format](GRZ_FORMAT.md) - Binary format specification

### Command Line Interface
- [CLI Implementation](GRAPA_CLI_IMPLEMENTATION.md) - Command line interface implementation and working functionality

## Architecture

These components work together to provide:
- **Type Safety**: Robust type system with compile-time checking
- **Execution Efficiency**: Optimized execution trees and compilation
- **Modularity**: Flexible module system for code organization
- **Binary Distribution**: GRZ format for pre-compiled code distribution
- **User Interface**: Command-line interface for script execution

## Development Guidelines

When working with core components:
1. **Type System Changes**: Update BASETYPES.md and related documentation
2. **Execution Model**: Modify EXECUTION_TREES.md and compilation docs
3. **Module System**: Update include and module system documentation
4. **Binary Format**: Ensure GRZ format compatibility
5. **CLI Changes**: Update CLI implementation documentation

## Related Documentation

- [Database Implementation](../DATABASE/) - Database and storage systems
- [Language Features](../LANGUAGE_FEATURES/) - Language extensibility features
- [System Components](../SYSTEM_COMPONENTS/) - System-level components
