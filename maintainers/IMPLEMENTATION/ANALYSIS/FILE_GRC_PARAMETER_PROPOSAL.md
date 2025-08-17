# $file.grc Parameter Name Proposal

**Date**: January 2025  
**Status**: Proposal Ready  
**Purpose**: Improve parameter names in $file.grc for clarity and consistency

---

## Analysis Summary

Based on C++ implementation analysis and existing documentation, the current parameter names are ambiguous and non-descriptive. This proposal provides intuitive, shorter parameter names that maintain functionality while improving developer experience.

---

## Parameter Rename Proposals

### 1. table(p) → table(type)
- **Current**: `p` (ambiguous)
- **Proposed**: `type` (database type: "DIR", "GROUP", "ROW", "COL")
- **C++ Evidence**: `r2.vVal->mValue.StrCmp("DIR") == 0` etc.
- **Documentation**: Already documented as database type parameter

### 2. cd(path) → cd(path)
- **Current**: `path` (already good)
- **Proposed**: `path` (no change needed)
- **C++ Evidence**: `DirectorySwitch(r2.vVal->mValue)` - clearly a path

### 3. chd(path) → chd(path)
- **Current**: `path` (already good)
- **Proposed**: `path` (no change needed)
- **C++ Evidence**: Home directory change - path parameter

### 4. ls(path, p) → ls(path, format)
- **Current**: `p` (ambiguous)
- **Proposed**: `format` (listing format: "DETAILS", etc.)
- **C++ Evidence**: `DirectoryList(r2.vVal->mValue, result)` - format parameter

### 5. mk(name, p) → mk(name, type)
- **Current**: `p` (ambiguous)
- **Proposed**: `type` (creation type: "GROUP", etc.)
- **C++ Evidence**: `DirectoryCreate(r2.vVal->mValue, gType)` - type parameter

### 6. rm(name) → rm(name)
- **Current**: `name` (already good)
- **Proposed**: `name` (no change needed)
- **C++ Evidence**: `DirectoryDelete(r2.vVal->mValue)` - name parameter

### 7. set(name, p, d) → set(name, field, data)
- **Current**: `p`, `d` (ambiguous)
- **Proposed**: `field`, `data` (field name and data value)
- **C++ Evidence**: `FieldSet(r2.vVal->mValue, setField, fieldValue.vVal->mValue)`
- **Documentation**: Already documented as field and data parameters

### 8. get(name, p) → get(name, field)
- **Current**: `p` (ambiguous)
- **Proposed**: `field` (field name to retrieve)
- **C++ Evidence**: `FieldGet(a.LongValue(), setField, result->mValue)`
- **Documentation**: Already documented as field parameter

### 9. info(name, p) → info(name, format)
- **Current**: `p` (ambiguous)
- **Proposed**: `format` (info format: "DETAILS", etc.)
- **C++ Evidence**: Similar to ls() - format parameter

### 10. split(parts, name, path, delim, option) → split(count, name, path, delim, option)
- **Current**: `parts` (ambiguous)
- **Proposed**: `count` (number of parts to split into)
- **C++ Evidence**: `split(2, "test_file.txt")` - clearly a count
- **Documentation**: Already documented as parts count

### 11. mkfield(o, p, d, e, f) → mkfield(name, type, storage, size, growth)
- **Current**: `o`, `p`, `d`, `e`, `f` (all ambiguous)
- **Proposed**: `name`, `type`, `storage`, `size`, `growth`
- **C++ Evidence**: Based on documentation - field creation parameters
- **Documentation**: Already documented with these semantic meanings

### 12. rmfield(p) → rmfield(name)
- **Current**: `p` (ambiguous)
- **Proposed**: `name` (field name to remove)
- **C++ Evidence**: `DirectoryDelete(r2.vVal->mValue)` - name parameter

### 13. debug(o, p) → debug(level, format)
- **Current**: `o`, `p` (ambiguous)
- **Proposed**: `level`, `format` (debug level and format)
- **C++ Evidence**: Debug parameters - level and format

---

## Complete Before/After Mapping

| Method | Current Parameters | Proposed Parameters | Rationale |
|--------|-------------------|-------------------|-----------|
| `table()` | `(p)` | `(type)` | Database type parameter |
| `cd()` | `(path)` | `(path)` | No change - already clear |
| `chd()` | `(path)` | `(path)` | No change - already clear |
| `ls()` | `(path, p)` | `(path, format)` | Format parameter for listing |
| `mk()` | `(name, p)` | `(name, type)` | Creation type parameter |
| `rm()` | `(name)` | `(name)` | No change - already clear |
| `set()` | `(name, p, d)` | `(name, field, data)` | Field name and data value |
| `get()` | `(name, p)` | `(name, field)` | Field name to retrieve |
| `info()` | `(name, p)` | `(name, format)` | Info format parameter |
| `split()` | `(parts, name, path, delim, option)` | `(count, name, path, delim, option)` | Count of parts |
| `mkfield()` | `(o, p, d, e, f)` | `(name, type, storage, size, growth)` | Field creation parameters |
| `rmfield()` | `(p)` | `(name)` | Field name to remove |
| `debug()` | `(o, p)` | `(level, format)` | Debug level and format |

---

## Implementation Impact

### Files to Modify:
1. `lib/grapa/$file.grc` - Update parameter names in method definitions
2. `docs-src/docs/sys/file.md` - Update documentation to reflect new parameter names

### Test Files:
1. `test/use_cases/file_grc_baseline_test.grc` - Baseline test (already created)
2. `test/use_cases/file_grc_regression_test.grc` - Regression test (to be created after changes)

### Validation:
- All methods currently work with existing parameters
- Baseline test captures current behavior
- Regression test will validate no functional changes
- Documentation already describes correct parameter semantics

---

## Benefits

1. **Clarity**: Parameter names now clearly indicate their purpose
2. **Consistency**: Follows established naming patterns from $OBJ.grc
3. **Maintainability**: Easier for developers to understand method signatures
4. **Documentation**: Aligns parameter names with existing documentation
5. **No Breaking Changes**: All functionality preserved, only names improved

---

## Next Steps

1. ✅ **Baseline Test Created**: `file_grc_baseline_test.grc` captures current behavior
2. 🔄 **Proposal Review**: Await maintainer approval
3. 🔄 **Implementation**: Update `$file.grc` with new parameter names
4. 🔄 **Regression Test**: Create and run regression test
5. 🔄 **Documentation Update**: Update `file.md` to reflect new names
6. 🔄 **Validation**: Ensure all tests pass with identical results

