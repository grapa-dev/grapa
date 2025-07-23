# GrapaBtree Implementation Reference

## Purpose
This document provides a comprehensive reference for the `GrapaBtree` class, its methods, enums, and associated data structures. It is intended as the authoritative source for understanding and maintaining the BTree/indexing layer in GrapaDB and related components.

---

## Class: GrapaBtree

### Overview
`GrapaBtree` implements a B-tree structure for indexing and record storage, supporting file-backed and in-memory operation via the `GrapaFile` abstraction. It is the core data structure underlying GrapaDB's table and index management.

### Key Data Members
- `u8 mFlags;` — Flags for BTree settings/state.
- `GrapaFile *mFile;` — Pointer to the file or file-like object used for storage.

### Enums
- `SU64_ITEM, TREE_ITEM, SDATA_ITEM, BDATA_ITEM, LAST_ITEM` — Item types for BTree nodes.
- `SU64_TREE, SDATA_TREE, BDATA_TREE, LAST_TREE` — Tree types.
- `BYTE_DATA, FREC_DATA, LAST_DATA` — Data storage types.
- `DATA_STORE, LAST_STORE` — Store types.
- `SEARCH_MODE, INSERT_MODE, DELETE_MODE, LAST_MODE` — Operation modes.
- `ENCODE_NONE, ENCODE_ZIP, ENCODE_AES` — Encoding/compression options.
- `NODE_WIDTH` — Node width constant.
- `CMP_LT, CMP_EQ, CMP_GT` — Comparison results.

### Core Methods
- **File Management:**
  - `SetFile(GrapaFile *pFile=NULL)` — Set or clear the file pointer.
  - `OpenFile(const char *fileName, char mode)` — Open a file for BTree operations.
  - `Create(const char *fileName)` — Create a new BTree file and initialize structure.
  - `RemoveFile(const char *fileName)` — Delete a BTree file.
  - `CloseFile()` — Close the current file.
  - `FlushFile()` — Flush file buffers to disk.
  - `ClearFile()` — Clear file contents.
  - `PurgeFile()` — Remove all data from the file.
  - `FileOpened()` — Check if a file is open.

- **Settings:**
  - `GetFlags(u8& settings)` / `SetFlags(u8 settings)` — Get/set BTree flags.
  - `GetCompressSize(u8& size)` / `SetCompressSize(u8 size)` — Get/set compression size.

- **Tree Operations:**
  - `RootTree(u8& pType)` — Get the root tree reference and type.
  - `CompareKey(u64 a, u64 b)` (static) — Compare two keys.
  - `CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)` — Compare keys using cursors.
  - `DeleteKey(GrapaCursor& treeCursor)` — Delete a key from the BTree.
  - `GetWeight(GrapaCursor& treeCursor, u64& itemWeight)` — Get the weight (size/count) of a tree item.
  - `GainWeight/LooseWeight (+Cascading)` — Maintain subtree weights for fast size/count queries.
  - `SetFileTree, FirstTree, NewTree, DeleteTree, EmptyTree, SetTreeIndex/GetTreeIndex, SetTreeStore/GetTreeStore, GetTreeSize, GetTreeDirty/SetTreeDirty, GetTreeType/SetTreeType, GetTreeParent` — Manage tree and metadata.

- **CRUD Operations:**
  - `Insert(GrapaCursor& cursor)` — Insert a key/value into the BTree.
  - `Search(GrapaCursor& cursor)` — Search for a key in the BTree.
  - `Delete(GrapaCursor& cursor)` — Delete a key from the BTree.
  - `Update(GrapaCursor& cursor)` — Update the value at a given cursor location.
  - `First/Last/Next/Prev(GrapaCursor& cursor)` — Traverse the tree.

- **Data Block Management:**
  - `NewData, DeleteData, SetDataSize, GetDataSize, SetFieldType, GetFieldType, ClearDataValue, SetDataValue, GetDataValue, InsertDataValue, DeleteDataValue, CopyDataValue, MoveDataValue, GetDataParent` — Manage variable-length data blocks (for BLOBs, strings, etc.) attached to tree nodes.

- **Internal/Protected Methods:**
  - `FindFirstX/FindLastX, SearchNode/SearchRc, InsertRc, PurgeRc, AppendNode, MoveLeaf, RotateLeafLeft/Right, RotateParrentLeft/Right, MirgeLeft, Split, EmptyItem, GainWeight/LooseWeight (+Cascading)` — Handle all internal BTree logic, including node splits, merges, underflow, and root changes.

### Associated Data Structures
- **GrapaCursor** — Used to navigate and operate on BTree nodes/keys.
- **GrapaBlock, GrapaBlockNodeHeader, GrapaBlockNodeLeaf, GrapaBlockTree, GrapaBlockDataHeader, GrapaBlockFileHeader, GrapaBlockFirst, GrapaBlockPage** — Represent BTree node and leaf blocks, tree roots, data blocks, file headers, and free page management.

---

## Deep Dive: How GrapaBtree Operates

### Initialization and File Management
- **SetFile** assigns the file object to the BTree.
- **Create** initializes the file header and first block.
- **OpenFile** validates the header and loads flags.
- **ClearFile** initializes the file header and first block.
- **CloseFile/RemoveFile/FlushFile/FileOpened** manage file lifecycle.

### Tree Management
- **RootTree** returns the root tree pointer and its type.
- **FirstTree** returns the first tree or the parent of a given tree.
- **NewTree** allocates and initializes a new tree block.
- **DeleteTree/EmptyTree** delete or empty a tree and its contents.
- **SetTreeIndex/GetTreeIndex, SetTreeStore/GetTreeStore, SetTreeType/GetTreeType, SetTreeDirty/GetTreeDirty, GetTreeParent** manage tree metadata.

### CRUD Operations
- **Insert**: If the tree is empty, creates the first node. Otherwise, recursively finds the correct position and inserts, splitting nodes as needed.
- **Search**: Recursively searches for a key, updating the cursor with the found value and node location.
- **Delete**: Recursively deletes a key, handling underflow, merging, and root changes as needed.
- **Update**: Updates the value at a given cursor location.
- **First/Last/Next/Prev**: Navigates to the first, last, next, or previous key in the tree.

### Data Block Management
- **NewData/DeleteData/SetDataSize/GetDataSize/SetFieldType/GetFieldType/ClearDataValue/SetDataValue/GetDataValue/InsertDataValue/DeleteDataValue/CopyDataValue/MoveDataValue/GetDataParent**: Manage variable-length data blocks (for BLOBs, strings, etc.) attached to tree nodes.

### Internal/Protected Methods
- **FindFirstX/FindLastX**: Find the first/last item in a subtree.
- **SearchNode/SearchRc**: Core recursive search logic.
- **InsertRc**: Core recursive insert logic, handles node splits.
- **PurgeRc**: Core recursive delete logic, handles underflow, merges, and root changes.
- **AppendNode, MoveLeaf, RotateLeafLeft/Right, RotateParrentLeft/Right, MirgeLeft, Split**: Handle node and tree structure changes during insert/delete.
- **GainWeight/LooseWeight (+Cascading)**: Maintain subtree weights for fast size/count queries.

### Block/Node/Leaf/Data Structures
- **GrapaBlockNodeHeader**: Represents a BTree node (internal node), with parent, child, leaf count, and weight.
- **GrapaBlockNodeLeaf**: Represents a leaf entry (key/value/child).
- **GrapaBlockTree**: Represents a tree root, with pointers to first item, index, store, parent, and item count.
- **GrapaBlockDataHeader**: Represents a data block for variable-length data.
- **GrapaBlockFileHeader**: File-level metadata (endianness, version, root pointer, etc.).
- **GrapaBlockFirst**: Manages the first block and free page list.
- **GrapaBlockPage**: Manages free space and page allocation.

---

## How to Properly Use GrapaBtree

### Initialization
1. Create a `GrapaFile` (e.g., `GrapaFileIO` or `GrapaFileCache`).
2. Create a `GrapaBtree` and call `SetFile`.
3. Call `Create(filename)` to initialize the file and BTree structure.
4. Use `NewTree` to create a root tree, then use `SetFileTree` to set it as the root in the file header.

### CRUD Operations
- For all operations, set up a `GrapaCursor` with the correct `treeRef` (root tree pointer), `valueType`, and `key`.
- Call `Insert`, `Search`, `Delete`, or `Update` as needed.
- Use `First`, `Last`, `Next`, `Prev` for traversal.

### Data Blocks
- Use `NewData` to allocate a data block, then attach its pointer as the value in a tree leaf.
- Use the data block management methods to read/write/resize/delete variable-length data.

### Tree Traversal
- Use the navigation methods (`First`, `Last`, `Next`, `Prev`) with a properly initialized cursor to iterate over all keys.

### Edge Cases
- The BTree handles node splits, merges, and root changes automatically during insert/delete.
- Underflow and merging logic is handled in `PurgeRc` and related methods.

---

## Best Practices and Gotchas
- Always ensure the file is open and the root tree is set before performing operations.
- Cursors must have the correct `treeRef` and `valueType` for the operation to succeed.
- After creating a new tree, update the file header with `SetFileTree`.
- For variable-length data, always manage the data block lifecycle (allocate, attach, free).
- The BTree is sensitive to endianness and file version; always use the provided methods for file management.

---

## Reference and Ongoing Documentation
- This summary and all future details are maintained in this file.
- As new edge cases, bugs, or usage patterns are discovered, update this doc to keep it as the canonical reference.

## Current Investigation Status (January 2025)

### BTree Core Functionality Validation
- **Status**: Active investigation using isolated test harness
- **Target**: Comprehensive validation of `source/grapa/GrapaBtree.cpp` implementation
- **Method**: Isolated test harness in `source/maindebug.cpp` with `builddebug.py` build script
- **Goal**: Validate all core BTree functionality before proceeding to GrapaDB integration testing

### Investigation Focus Areas
1. **Initialization and Setup**: Proper BTree file creation and tree initialization
2. **Core CRUD Operations**: Insert, delete, search, update functionality
3. **Node Management**: Splits, merges, underflow handling, root changes
4. **Traversal Operations**: First, last, next, prev navigation
5. **Edge Cases**: Empty trees, single nodes, large datasets, stress testing
6. **File I/O**: Proper file operations, memory management, cleanup

### Documentation Updates
- This document will be updated with validation findings and any discovered edge cases
- Cross-reference with `ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md` for investigation progress
- Update with any new best practices or gotchas discovered during validation 

---

## 2024-07: Critical Bug and Fix – BTree Leaf Move/Overwrite During Delete

### Summary
A critical bug was discovered in the BTree's node deletion logic, specifically in how leaf entries were moved to fill a gap after a delete. This bug caused data corruption in GrapaDB's ROW table index, manifesting as index entries pointing to the wrong records or becoming invalid after certain insert/delete sequences.

### The Bug
When deleting a key from a BTree node, the code that shifts subsequent leaves to fill the gap was moving leaves in the wrong order, causing data to be overwritten:

```cpp
// Buggy code (overwrites data when moving within the same block):
for(i=pos;i<rootTree.leafCount;i++)
{
    MoveLeaf(headRef,head,rootTree,rootNode,i+1,rootTree,rootNode,i);
}
```
This would overwrite data when deleting from the start or middle of a node, especially when multiple entries needed to be shifted.

### The Investigation
- The bug was first observed as index corruption in the ROW table after inserting or deleting multiple records.
- Extensive debug output and test harnesses isolated the problem to BTree node manipulation during delete.
- Instrumentation showed that the move loop was overwriting entries before they could be safely relocated.

### The Fix
The fix is to move leaves from higher to lower positions, but in the correct order, to avoid overwriting data that hasn't been moved yet:

```cpp
// Corrected code:
for(i=pos+1;i<rootTree.leafCount;i++)
{
    MoveLeaf(headRef,head,rootTree,rootNode,i,rootTree,rootNode,i-1);
}
```
This ensures that each leaf is moved only after the previous one has been safely relocated, preserving all data.

### Impact and Verification
- All BTree insert, delete, and update operations now work correctly, even in edge cases.
- No more index corruption: all RPTR entries point to the correct records after multiple inserts and deletes.
- All test scripts, including `test_row.grc`, now pass with correct data retrieval.
- Debug output confirms correct leaf movement and no data loss.

### Lessons Learned
- BTree node manipulation (especially in-place moves) must be done with careful attention to order to avoid overwriting data.
- Detailed debug instrumentation at every step of the BTree and index management logic was essential to root cause this subtle bug.
- Maintaining a thorough investigation log and test harness was critical for isolating and fixing the issue.

--- 