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

## Open Issues

As of January 2025, there are no known open issues with the BTree implementation itself. All core BTree functionality (insert, delete, search, update, node splitting/merging, traversal, and file I/O) has been validated and is working as intended. Any current database index or corruption issues are tracked in separate investigation documentation and are not attributed to the BTree implementation.

--- 