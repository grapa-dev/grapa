# Grapa Syntax Validator

A comprehensive tool to validate and auto-fix Grapa syntax issues in `.grc` files.

## Problem Solved

This validator addresses the recurring issue where AI agents (including this one) default to Python/JavaScript syntax patterns when generating or modifying Grapa code, causing working scripts to break. The validator:

1. **Prevents regressions** by catching syntax violations before they cause runtime errors
2. **Auto-fixes common issues** that can be reliably corrected
3. **Reports manual fixes needed** for issues that require human judgment
4. **Saves time** by automating repetitive syntax corrections

## Usage

### Basic Validation
```bash
# Validate all .grc files in test/ directory
python3 scripts/validate_grapa_syntax.py

# Or use the shell wrapper
./scripts/validation/validate_grapa.sh
```

### Auto-fix Issues
```bash
# Auto-fix issues that can be reliably fixed
python3 scripts/validate_grapa_syntax.py --fix
```

### Validate Specific File
```bash
# Validate only one file
python3 scripts/validate_grapa_syntax.py --file test/my_file.grc
```

### Verbose Output
```bash
# Show detailed information about each issue
python3 scripts/validate_grapa_syntax.py --verbose
```

## What It Checks

### Auto-fixable Issues
- **Incorrect semicolons**: `op() {;` → `op() {`
- **Line comments**: `// comment` → `/* comment */`
- **Class definition semicolons**: `class {;` → `class {`
- **If/while/else semicolons**: `if () {;` → `if () {`

### Manual Fix Required
- **Forbidden patterns**: `for` loops (not supported in Grapa)
- **Incorrect echo syntax**: `.echo("string")` → `"string".echo()`
- **Incorrect method calls**: `.len(array)` → `array.len()`
- **Missing quotes**: `.echo(string)` → `.echo("string")`
- **Missing newlines**: `"string".echo()` without `\n`
- **Missing semicolons**: Statements that should end with `;`
- **Complex syntax issues**: Issues requiring human judgment

## Rules Based On

The validator implements syntax rules based on:

1. **`docs-src/docs/syntax/basic_syntax.md`** - User-facing documentation with examples and best practices
2. **`lib/grapa/$grapa.grc`** - **The definitive source for Grapa syntax** (the actual grammar file used by the Grapa parser)

> **Important**: `lib/grapa/$grapa.grc` is the authoritative source for Grapa syntax rules. The validator's patterns are derived from this grammar file and the user documentation. When in doubt about syntax, refer to `$grapa.grc` as the definitive reference.

## Integration

### Pre-commit Hook
Consider adding this to your development workflow:
```bash
# Add to .git/hooks/pre-commit
python3 scripts/validate_grapa_syntax.py --fix
```

### CI/CD Pipeline
```yaml
# Example GitHub Actions step
- name: Validate Grapa Syntax
  run: |
    python3 scripts/validate_grapa_syntax.py
    if [ $? -ne 0 ]; then
      echo "Syntax validation failed. Run with --fix to auto-fix issues."
      exit 1
    fi
```

## Output Example

```
🔍 Grapa Syntax Validator
==================================================
Found 260 .grc files to validate

📁 /Users/matichuk/GitHub/grapa/test/example.grc
------------------------------------------------
🔧 AUTO-FIXED Line 10: Incorrect semicolon after opening brace
    Content: op() {;
⚠️  MANUAL FIX NEEDED Line 15: Forbidden pattern found: \bfor\s*\(
    Content: for (i = 0; i < 10; i++) {
⚠️  MANUAL FIX NEEDED Line 20: Incorrect echo syntax: use "string".echo() not .echo("string")
    Content: .echo("Hello World");

==================================================
📊 VALIDATION SUMMARY
==================================================
Files checked: 260
Files with issues: 140
Total issues found: 2026
Auto-fixed: 32
Manual fix needed: 1994

⚠️  140 files need attention
```

## Exit Codes

- `0`: All files are compliant or all issues were auto-fixed
- `1`: Issues found that require manual attention

## Key Syntax Rules Validated

### Echo Syntax (Most Critical)
```grapa
/* ✅ Correct */
"Hello World\n".echo();
("Result: " + value.str()).echo();

/* ❌ Incorrect */
.echo("Hello World");
echo("Hello World");
```

### Comments
```grapa
/* ✅ Correct - Block comments only */
/* This is a block comment */

/* ❌ Incorrect - Line comments not allowed */
// This is a line comment
```

### Loops
```grapa
/* ✅ Correct - Only while loops supported */
i = 1;
while (i <= 10) {
    ("Count: " + i.str()).echo();
    i = i + 1;
}

/* ❌ Incorrect - for loops not supported */
for (i = 0; i < 10; i++) {
    echo("Count: " + i);
}
```

### Error Handling
```grapa
/* ✅ Correct - Use .iferr() for error handling */
result = risky_operation().iferr(0);  /* Returns 0 if operation fails */
value = (10/0).iferr(55);  /* Returns 55 since division by zero fails */

/* ❌ Incorrect - try/catch not supported */
try {
    result = risky_operation();
} catch (error) {
    ("Error: " + error).echo();
}
```

### Semicolons
```grapa
/* ✅ Correct - Statements end with semicolon */
x = 5;
"Hello".echo();

/* ✅ Correct - Block statements can omit semicolon */
if (x > 0) {
    "Positive".echo();
}  /* No semicolon required */

/* ❌ Incorrect - Opening braces don't need semicolons */
op() {;  /* Should be: op() { */
```

### Database Patterns (GrapaDBX)

#### Correct Patterns
```grapa
/* ✅ File-based GrapaDBX via $unified() */
u = $unified();
u.create("grapadbx:///path/to/database.dbx");

/* ✅ In-memory GrapaDBX via $unified() */
u = $unified();
u.create("memory://temp_database");

/* ✅ In-memory database via $file().table() */
t = $file().table("ROW");    /* Creates in-memory ROW database */
t = $file().table("COL");    /* Creates in-memory COL database */
t = $file().table("GROUP");  /* Creates in-memory GROUP database */
```

#### Incorrect Patterns (Common Mistakes)
```grapa
/* ❌ These patterns do NOT exist */
$file().dbx();              /* No such method */
$file().dbx().table();      /* No such method */
$dbx().table();             /* No such class */
```

#### The `$` Path Discovery
The `$file().table()` method uses a special `"$"` path internally:
- **`"$"` is a symbolic name**, not a file path
- **Creates in-memory database** using `GrapaCHAR` as storage
- **No file system access** - entire database stored in memory
- **`GrapaFileCache` is separate** - caching layer, not in-memory storage

This was discovered by analyzing `GrapaLibraryRuleTableEvent::Run()` in `source/grapa/GrapaLibRule.cpp`:
```cpp
// Creates database with name "$" (symbolic, not file path)
err = g.Create("$", listType, firstTree);
```

## Maintenance

The validator implements syntax patterns based on the definitive `$grapa.grc` grammar file and user documentation in `basic_syntax.md`. When Grapa's syntax evolves, both the grammar file and documentation should be updated, and the validator patterns may need corresponding updates.

## Future Enhancements

Potential improvements:
- More sophisticated auto-fixing (e.g., `for` loop conversion to `while`)
- Integration with IDE plugins
- Performance optimization for large codebases
- Custom rule configuration
- HTML/JSON output formats for CI integration
- Direct parsing using the Grapa grammar for more accurate validation 