# Grapa Examples Overview

This directory contains practical, ready-to-run examples demonstrating Grapa's core features and capabilities.

## Quick Start

1. **Install Grapa** using the [Universal Installer](../installation.md)
2. **Run any example** with: `./grapa examples/filename.grc`
3. **Copy and modify** examples for your own projects

## Example Categories

### Core Language Examples
- **Basic operations**: Variables, arrays, strings, math
- **Advanced features**: Pattern matching, metaprogramming, concurrency
- **Data structures**: Lists, arrays, vectors, XML
- **Functional programming**: Map, reduce, filter with threading

### Integration Examples
- **Python integration**: Using Grapa from Python
- **Network programming**: HTTP/HTTPS clients and servers
- **Database operations**: Table creation and data manipulation
- **Vector database similarity**: Pinecone-style vector search and similarity operations

### Language Extension Examples
- **Custom syntax**: SQL, PROLOG, LISP implementations
- **Domain-specific languages**: ETL processing, rule systems
- **Grammar extension**: Adding new language features

## Running Examples

### Command Line
```bash
# Run a single example
./grapa examples/basic_example.grc

# Run with debug output
./grapa -d examples/advanced_example.grc

# Run quietly (suppress version header)
./grapa -q examples/performance_example.grc
```

### Python Integration
```bash
# Run Python examples
python3 examples/python_integration_example.py
```

## Vector Database Similarity Examples

### New Examples
- **`vector_database_similarity_example.grc`** - Comprehensive demonstration of Grapa's similarity search capabilities
- **`pinecone_style_vector_search.grc`** - Implementation of typical Pinecone functionality in Grapa

These examples demonstrate:
- **Unified similarity algorithms** (Levenshtein, Damerau, Jaro, Cosine, Jaccard)
- **Advanced options** (TF-IDF, word-based jaccard, configurable parameters)
- **Database integration** with performance optimization using $ID
- **Object similarity** with metadata search
- **Vector mathematical operations** with similarity search
- **Performance benchmarking** and optimization techniques

## Example Structure

Each example file includes:
- **Clear comments** explaining the code
- **Expected output** in comments
- **Best practices** demonstrated
- **Error handling** patterns

## Getting Help

- **Syntax issues**: Check [Basic Syntax](../syntax/basic_syntax.md)
- **Function reference**: See [API Reference](../api_reference.md)
- **Advanced topics**: Review [Advanced Topics](../advanced_topics.md)
- **Troubleshooting**: Check [Troubleshooting Guide](../troubleshooting.md)

## Contributing Examples

When adding new examples:
1. **Use clear, descriptive names**
2. **Include comprehensive comments**
3. **Test thoroughly** before committing
4. **Update this README** with new examples
5. **Follow existing patterns** for consistency

## Related Documentation

- [Getting Started](../getting_started.md) - First steps with Grapa
- [Installation Guide](../installation.md) - How to install Grapa
- [API Reference](../api_reference.md) - Complete function reference
- [Migration Guides](../migrations/) - Coming from other languages
