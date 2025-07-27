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
./scripts/validate_grapa.sh
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

### Manual Fix Required
- **Forbidden patterns**: `for` loops (not supported in Grapa)
- **Missing semicolons**: Statements that should end with `;`
- **Missing newlines**: `.echo()` calls without explicit `\n`
- **Complex syntax issues**: Issues requiring human judgment

## Rules Based On

The validator reads rules from `docs-src/docs/syntax/basic_syntax.md` to ensure it stays current with Grapa's syntax requirements.

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
🔧 Line 10: Incorrect semicolon after opening brace
   Content: op() {;
⚠️ Line 15: Forbidden pattern found: \bfor\s*\(
   Content: for (i = 0; i < 10; i++) {

==================================================
📊 VALIDATION SUMMARY
==================================================
Files checked: 260
Files with issues: 140
Total issues found: 2026
Auto-fixed: 32
Manual fix needed: 1994

⚠️  1994 issues require manual attention
   Review the output above and fix manually

✅ 32 issues were auto-fixed
```

## Exit Codes

- `0`: All files are compliant or all issues were auto-fixed
- `1`: Issues found that require manual attention

## Maintenance

The validator automatically reads syntax rules from `basic_syntax.md`, so it stays current as Grapa's syntax evolves. If new syntax rules are added to the documentation, the validator will automatically pick them up.

## Future Enhancements

Potential improvements:
- More sophisticated auto-fixing (e.g., `for` loop conversion)
- Integration with IDE plugins
- Performance optimization for large codebases
- Custom rule configuration
- HTML/JSON output formats for CI integration 