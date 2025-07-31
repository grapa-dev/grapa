# Field Metadata Lifecycle and Requirements in GrapaDB

## Overview

Correct operation of GrapaDB's field access, comparison, and storage logic depends on each `GrapaDBField` and `GrapaDBFieldValue` having up-to-date schema metadata (type, storage, size, etc.) before use. This document explains:
- Why this is necessary
- The `EnsureFieldMeta` pattern
- Which operations depend on it
- Risks of not following this pattern
- Example code
- Open items for future work

---

## Why Field Metadata Must Be Set

Many core operations (get/set, comparison, serialization, etc.) require the field's type, storage, and other schema info to be correct. If this metadata is missing or stale, bugs such as type mismatches, data corruption, or incorrect comparisons can occur.

---

## The `EnsureFieldMeta` Pattern

Add a boolean flag (e.g., `mFieldMetaSet`) to `GrapaDBFieldValue`. Before any operation that depends on schema info, call `EnsureFieldMeta(db, table)`. If the flag is false, fetch the schema info (e.g., via `OpenTableField`), update the field, and set the flag true.

**Example:**
```cpp
class GrapaDBFieldValue : public GrapaDBField {
public:
    GrapaCHAR mValue;
    s16 mCmp;
    bool mFieldMetaSet;
    GrapaDBFieldValue() : mCmp(0), mFieldMetaSet(false) {}
    void EnsureFieldMeta(GrapaDB* db, GrapaDBTable& table) {
        if (!mFieldMetaSet) {
            db->OpenTableField(table, mId, *this);
            mFieldMetaSet = true;
        }
    }
};
```

---

## Operations That Depend on Field Metadata

| Operation/Function         | Needs Valid Metadata? | Why?                                 |
|----------------------------|----------------------|--------------------------------------|
| `GetRecordField`           | Yes                  | To read value correctly              |
| `SetRecordField`           | Yes                  | To write value correctly             |
| `CompareSearchKey`         | Yes                  | For type-aware comparison            |
| `CompareRecordKey`         | Yes                  | For type-aware comparison            |
| Serialization/Deserialization | Yes               | For correct size/type                |
| Value Conversion           | Yes                  | For correct conversion logic         |
| Index/Query Planning       | Yes                  | For optimization decisions           |
| Dynamic Schema Ops         | Yes                  | To update/refresh affected fields    |

---

## Risks of Not Following This Pattern
- Type mismatches (e.g., $STR instead of $INT)
- Data corruption or loss
- Incorrect query results
- Subtle, hard-to-debug bugs

---

## Example Usage

```cpp
// Before using fv in a type-dependent operation:
fv->EnsureFieldMeta(db, table);
// Now safe to use fv->mType, fv->mStore, etc.
```

---

## Open Items / Future Work
- Audit all code paths (not just get) that use `GrapaDBField`/`GrapaDBFieldValue` for operations listed above.
- Refactor as needed to ensure `EnsureFieldMeta` is called everywhere required.
- Consider adding debug assertions to catch uninitialized metadata in development builds.
- Review and update this doc as new field-dependent features are added.

---

## See Also
- [Backlog entry: Field Metadata Lifecycle Enforcement]
- [GrapaDB Internal Architecture]

---

*Last updated: 2024-06-XX* 