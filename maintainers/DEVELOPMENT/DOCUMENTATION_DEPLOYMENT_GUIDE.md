# Documentation Deployment Guide

## Overview
This guide explains how to deploy documentation changes to the live GitHub Pages site at https://grapa-dev.github.io/grapa/

## GitHub Pages Configuration
The site is configured with the following settings:
- **Source**: Deploy from a branch
- **Branch**: `gh-pages`
- **Folder**: `/docs`

This means GitHub Pages serves content from the `/docs` folder within the `gh-pages` branch.

## Deployment Process

### 1. Build the Documentation
```bash
cd docs
python3 -m mkdocs build --clean
```

This creates the built site in `docs/site/` with all HTML files and assets.

### 2. Switch to gh-pages Branch
```bash
git checkout gh-pages
```

### 3. Update the /docs Folder
The built site needs to be copied to the `/docs` folder where GitHub Pages expects it:

```bash
# Remove old content (be careful with this command)
cd docs
rm -rf USE_CASES  # Remove old uppercase directory if it exists

# Copy new built site to docs folder
cp -r site/use_cases .     # New organized use cases
cp -r site/syntax .        # Syntax section
cp -r site/*.html .        # Main pages
cp -r site/assets .        # CSS/JS assets
cp -r site/sitemap* .      # Sitemap files
```

### 4. Commit and Push
```bash
cd ..
git add docs/
git commit -m "Deploy updated documentation with [description of changes]"
git push origin gh-pages
```

### 5. Wait for Deployment
GitHub Pages typically takes 2-5 minutes to update after a push.

## Link Fixing and Navigation Structure

### Common Link Issues and Fixes
When updating documentation, you may encounter broken links. Here are the most common fixes:

#### Fix Case Sensitivity Issues
```bash
# Fix Python integration links (uppercase to lowercase)
find . -name "*.md" -exec sed -i '' 's/PYTHON_INTEGRATION\.md/python_integration.md/g' {} \;

# Fix grep links (lowercase to uppercase)
find . -name "*.md" -exec sed -i '' 's/grep\.md/GREP.md/g' {} \;

# Fix USE_CASES links to new structure
find . -name "*.md" -exec sed -i '' 's/USE_CASES\.md/use_cases\/index.md/g' {} \;

# Fix PYTHON_use_cases links
find . -name "*.md" -exec sed -i '' 's/PYTHON_use_cases\/index\.md/PYTHON_USE_CASES.md/g' {} \;
```

#### Fix External Test File References
```bash
# Replace missing test file references with examples
find . -name "*.md" -exec sed -i '' 's/test\/test_basic\.grc/examples\/basic_example.grc/g' {} \;
find . -name "*.md" -exec sed -i '' 's/test\/test_basic\.grz/examples\/basic_example.grc/g' {} \;
find . -name "*.md" -exec sed -i '' 's/test\/test_advanced\.grc/examples\/advanced_example.grc/g' {} \;
find . -name "*.md" -exec sed -i '' 's/test\/test_performance\.grz/examples\/performance_example.grc/g' {} \;
```

### Examples Directory Management

#### Purpose
The `docs/docs/examples/` directory contains test examples and sample files that are referenced by the Grapa documentation. This ensures that documentation examples are always available and properly maintained.

#### Dependency System
**⚠️ IMPORTANT**: Files in the examples directory are referenced by documentation. Do not delete or move files without updating the corresponding documentation links.

#### Testing Requirements
**🔬 CRITICAL**: All .grc and .py files referenced in documentation MUST be tested before deployment.

##### Test Commands
```bash
# Test all .grc examples
./grapa -cfile "docs/docs/examples/basic_example.grc" -q
./grapa -cfile "docs/docs/examples/advanced_example.grc" -q
./grapa -cfile "docs/docs/examples/performance_example.grc" -q

# Test Python examples
python3 docs/docs/examples/python_integration_example.py
```

##### Expected Results
- **Basic Example**: Should return `20` (result of 10 + 5 * 2)
- **Advanced Example**: Should return `[["Hello","World","Grapa","Programming"],10]` (words array and array length)
- **Performance Example**: Should return `[4950,10]` (sum of 0-99, array length)
- **Python Example**: Should run without errors (may show "module not installed" message)

#### Current Examples
- `basic_example.grc` - Basic Grapa syntax and operations
- `advanced_example.grc` - Complex Grapa operations and functions
- `performance_example.grc` - Performance testing examples
- `python_integration_example.py` - Python integration examples

#### Maintenance Rules
1. **Search before changes**: Use `grep -r "examples/filename.grc" docs/docs/`
2. **Document dependencies**: Add `<!-- DEPENDENCY: examples/filename.grc -->` comments
3. **Update all references**: When moving/deleting files, update all documentation links
4. **Test examples**: Ensure all examples work correctly before committing
5. **Test before deployment**: Run all example tests before deploying documentation

#### Benefits
- **Self-contained documentation**: No external dependencies on test files
- **Guaranteed availability**: All referenced examples are always present
- **Clear maintenance**: Explicit dependency tracking prevents broken links
- **Easy discovery**: Centralized location for all documentation examples
- **Tested examples**: All examples are verified to work correctly

#### File Organization
- **`.grc` files**: Grapa source code examples
- **`.grz` files**: Compiled Grapa examples  
- **`.py` files**: Python integration examples
- **`.md` files**: Example documentation

#### Adding New Examples
1. Place your example file in `docs/docs/examples/`
2. Update the documentation to reference the new file
3. Add a comment in the documentation indicating the dependency

#### Example Documentation Reference
```markdown
<!-- DEPENDENCY: examples/basic_example.grc -->
```grapa
include "examples/basic_example.grc";
```

#### Moving/Deleting Examples
Before moving or deleting any file in the examples directory:
1. Search the documentation for references to the file
2. Update all documentation links
3. Consider if the example should be preserved elsewhere

#### Search Commands
```bash
# Search for references to a specific example
grep -r "examples/filename.grc" docs/docs/

# Search for all example references
grep -r "examples/" docs/docs/
```

#### Current Examples
- `basic_example.grc` - Basic Grapa syntax and operations
- `advanced_example.grc` - Complex Grapa operations and functions
- `performance_example.grc` - Performance testing examples
- `python_integration_example.py` - Python integration examples

#### Complete Navigation Structure
The navigation in `docs/mkdocs.yml` should include all sections:

```yaml
nav:
  - Home: index.md
  - Getting Started: GETTING_STARTED.md
  - CLI Quickstart: CLI_QUICKSTART.md
  - Python Quickstart: python_integration.md
  - API Reference: API_REFERENCE.md
  - Function Quick Reference: FUNCTION_QUICK_REFERENCE.md
  - Grep Documentation: GREP.md
  - Migration Guides:
      - Python: migrations/PYTHON_TO_GRAPA_MIGRATION.md
      - JavaScript: migrations/JS_TO_GRAPA_MIGRATION.md
      - Rust: migrations/RUST_TO_GRAPA_MIGRATION.md
      - TypeScript: migrations/TYPESCRIPT_TO_GRAPA_MIGRATION.md
      - Go: migrations/GO_TO_GRAPA_MIGRATION.md
      - Ruby: migrations/RUBY_TO_GRAPA_MIGRATION.md
      - Kotlin: migrations/KOTLIN_TO_GRAPA_MIGRATION.md
      - Swift: migrations/SWIFT_TO_GRAPA_MIGRATION.md
  - Use Cases:
      - Overview: use_cases/index.md
      - ETL / Data Engineering: use_cases/etl_data_engineering.md
      - Compiler/BNF Learning: use_cases/compiler_bnf_learning.md
      - High-Precision Math: use_cases/high_precision_math.md
      - Parallel Programming: use_cases/parallel_concurrent_programming.md
      - Web Scraping: use_cases/web_data_scraping.md
      - Database & File System: use_cases/database_file_system.md
      - Education & Prototyping: use_cases/education_prototyping.md
      - Cryptography: use_cases/cryptography.md
  - Examples: EXAMPLES.md
  - Syntax:
      - Basic Syntax: syntax/basic_syntax.md
      - Operators: syntax/operator.md
      - Precedence: syntax/precedence.md
      - Grammar Design: syntax/grammar_design.md
  - System Functions:
      - File Operations: sys/file.md
      - Math Functions: sys/math.md
      - Network Functions: sys/net.md
      - System Functions: sys/sys.md
      - Thread Functions: sys/thread.md
  - Object Types:
      - Document: obj/document.md
      - Encode: obj/encode.md
      - Grep: obj/grep.md
      - Grep Python: obj/grep_python.md
      - Iterate: obj/iterate.md
      - Transform: obj/transform.md
  - Operators:
      - Assignment: operators/assignment.md
      - Bit Operations: operators/bit.md
      - Command: operators/command.md
      - Comparison: operators/compare.md
      - Condition: operators/condition.md
      - Constants: operators/constants.md
      - Function: operators/function.md
      - Loop: operators/loop.md
      - Math: operators/math.md
  - Data Types:
      - Array: type/ARRAY.md
      - Boolean: type/BOOL.md
      - Code: type/CODE.md
      - Error: type/ERR.md
      - Float: type/FLOAT.md
      - ID: type/ID.md
      - Integer: type/INT.md
      - List: type/LIST.md
      - Operation: type/OP.md
      - Raw: type/RAW.md
      - Rule: type/RULE.md
      - String: type/STR.md
      - System ID: type/SYSID.md
      - System Integer: type/SYSINT.md
      - System String: type/SYSSTR.md
      - Table: type/TABLE.md
      - Tag: type/TAG.md
      - Time: type/TIME.md
      - Vector: type/VECTOR.md
      - Widget: type/WIDGET.md
      - XML: type/XML.md
  - Database:
      - Column Store: database/column_store.md
      - Quick Reference: database/quick_reference.md
      - Troubleshooting: database/troubleshooting.md
  - Additional Tools:
      - Directory Navigation: directory_navigation.md
      - GRC Scripts: grc_scripts.md
      - GRZ Format: GRZ_FORMAT.md
      - Python Use Cases: PYTHON_USE_CASES.md
      - Run: RUN.md
      - Testing: TESTING.md
      - Unified Path System: unified_path_system.md
  - Troubleshooting: TROUBLESHOOTING.md
```

### Comprehensive Documentation Check Process
When doing a full documentation review and update:

#### 1. Build and Check for Issues
```bash
cd docs
python3 -m mkdocs build --clean
```

#### 2. Analyze Build Output
Look for:
- **WARNING** messages about broken links
- **INFO** messages about missing anchors
- Files not included in navigation

#### 3. Fix Link Issues
Use the sed commands above to fix common case sensitivity issues.

#### 4. Update Navigation
Ensure all documentation files are included in the navigation structure.

#### 5. Rebuild and Verify
```bash
python3 -m mkdocs build --clean
```

#### 6. Deploy Changes
Follow the standard deployment process.

### Expected Build Warnings
Some warnings are expected and acceptable:
- Migration guide links to syntax files (these are relative links within migration docs)
- Links to maintainer documentation (these are intentionally excluded)
- Some anchor links to specific sections (these may be missing anchors)

### Target Build Quality
A good build should show:
- **0-2 files not in navigation** (minor files like widget attributes)
- **< 20 broken link warnings** (mostly migration guide relative links)
- **All major sections accessible** via navigation

## Important Notes

### File Structure
- **Source files**: Located in `docs/docs/` (main branch)
- **Built site**: Located in `docs/site/` (after mkdocs build)
- **Deployed site**: Located in `docs/` (gh-pages branch)

### Navigation Configuration
The navigation is configured in `docs/mkdocs.yml` and includes:
- Use Cases with organized subsections
- Syntax section with subsections
- All other documentation sections

### Common Issues

#### Issue: Site not updating
- **Cause**: Built site not copied to correct `/docs` folder
- **Solution**: Ensure files are in `docs/` folder in gh-pages branch

#### Issue: Old content still showing
- **Cause**: Browser caching or GitHub Pages caching
- **Solution**: Wait 5-10 minutes, try incognito mode, or hard refresh

#### Issue: Missing sections
- **Cause**: Navigation not updated in mkdocs.yml
- **Solution**: Check navigation configuration and rebuild

#### Issue: Broken links after reorganization
- **Cause**: File names changed or structure reorganized
- **Solution**: Use sed commands to fix case sensitivity and path issues

### Verification
To verify deployment worked:
1. Check that `use_cases/` (lowercase) exists in `docs/` folder
2. Check that `syntax/` directory exists in `docs/` folder
3. Visit https://grapa-dev.github.io/grapa/ and verify navigation
4. Check that all subsections are accessible
5. Verify that broken links are minimized in build output

## Quick Deployment Script
For future automation, here's a complete deployment script:

```bash
#!/bin/bash
# Deploy documentation to GitHub Pages

echo "Building documentation..."
cd docs
python3 -m mkdocs build --clean

echo "Fixing common link issues..."
find . -name "*.md" -exec sed -i '' 's/PYTHON_INTEGRATION\.md/python_integration.md/g' {} \;
find . -name "*.md" -exec sed -i '' 's/grep\.md/GREP.md/g' {} \;
find . -name "*.md" -exec sed -i '' 's/USE_CASES\.md/use_cases\/index.md/g' {} \;
find . -name "*.md" -exec sed -i '' 's/PYTHON_use_cases\/index\.md/PYTHON_USE_CASES.md/g' {} \;

echo "Rebuilding with fixes..."
python3 -m mkdocs build --clean

echo "Switching to gh-pages branch..."
cd ..
git checkout gh-pages

echo "Updating docs folder..."
cd docs
rm -rf USE_CASES 2>/dev/null || true
cp -r site/use_cases .
cp -r site/syntax .
cp -r site/*.html .
cp -r site/assets .
cp -r site/sitemap* .

echo "Committing and pushing..."
cd ..
git add docs/
git commit -m "Deploy documentation updates with link fixes"
git push origin gh-pages

echo "Deployment complete! Site will update in 2-5 minutes."
```

## Related Files
- `docs/mkdocs.yml` - Main configuration file
- `docs/docs/` - Source documentation files
- `docs/site/` - Built site (after mkdocs build)
- `docs/` - Deployed site (in gh-pages branch)

## Troubleshooting
If deployment fails:
1. Check that you're on the gh-pages branch
2. Verify the `/docs` folder contains the built site
3. Ensure all files are committed and pushed
4. Wait for GitHub Pages to update (2-5 minutes)
5. Clear browser cache if needed
6. Check for broken links in build output and fix them 