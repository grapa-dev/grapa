# Documentation Examples

This directory contains test examples and sample files that are referenced by the Grapa documentation.

## Dependency System

**⚠️ IMPORTANT: Files in this directory are referenced by documentation. Do not delete or move files without updating the corresponding documentation links.**

### File Organization

- **`.grc` files**: Grapa source code examples
- **`.grz` files**: Compiled Grapa examples  
- **`.py` files**: Python integration examples
- **`.md` files**: Example documentation

### Adding New Examples

1. Place your example file in the appropriate subdirectory
2. Update the documentation to reference the new file
3. Add a comment in the documentation indicating the dependency

### Example Documentation Reference

```markdown
<!-- DEPENDENCY: examples/basic_example.grc -->
```grapa
include "examples/basic_example.grc";
```

### Moving/Deleting Examples

Before moving or deleting any file in this directory:

1. Search the documentation for references to the file
2. Update all documentation links
3. Consider if the example should be preserved elsewhere

### Search Commands

To find documentation references to a specific example:

```bash
# Search for references to a specific file
grep -r "examples/filename.grc" docs/docs/

# Search for all example references
grep -r "examples/" docs/docs/
```

## Current Examples

This section will be updated as examples are added to the directory. 