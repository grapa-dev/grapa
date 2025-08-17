# GrapaBtree Implementation Reference

## Purpose
This document provides a comprehensive reference for the `GrapaBtree` class, its methods, enums, and associated data structures. It is intended as the authoritative source for understanding and maintaining the BTree/indexing layer in GrapaDB and related components.

> For a deep dive into the on-disk file/block structure and manual traversal of Grapa BTree files, see [GRAPA_BTREE_FILE_STRUCTURE.md](./GRAPA_BTREE_FILE_STRUCTURE.md).

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

### Performance Optimization Details
- **Node Width Tuning**: Configurable `NODE_WIDTH` for performance vs. memory trade-offs
- **Block Size Optimization**: 32-byte blocks for consistent allocation patterns
- **Memory Management**: Efficient allocation and deallocation with hole management
- **Cache Hit Optimization**: LRU eviction for optimal cache performance

### Thread Safety Features
- **Built-in Synchronization**: All operations internally synchronized at C++ level
- **No Crashes**: Never encounter crashes or corruption from concurrent access
- **Logical Race Conditions**: May see logical race conditions but no stability issues
- **Cache Thread Safety**: Thread-safe caching mechanisms for concurrent access

### Performance Monitoring and Debugging
- **Weight Tracking**: Maintain subtree weights for fast size/count queries
- **Debug Dumping**: Comprehensive dump system for performance analysis
- **Block-level Debugging**: Inspect individual blocks and nodes for optimization
- **Performance Profiling**: Tools for identifying performance bottlenecks

---

## BYTE_DATA and FREC_DATA: Standard and Fragmented Data Support

### BYTE_DATA
- **BYTE_DATA** is the standard data type for BTree values in GrapaBtree.
- It is used for storing contiguous, non-fragmented data blocks—suitable for most small-to-medium-sized values.
- Operations on BYTE_DATA are straightforward: insert, search, update, and delete work on a single, contiguous data block.

### FREC_DATA (Fragmented Data)
- **FREC_DATA** is a specialized, fragmented data type designed for efficient handling of large data items.
- Inspired by file system page-linking, FREC_DATA uses a *weighted BTree* to index and manage data fragments (pages).
- Each fragment/page is a node in the BTree, and the tree maintains a "weight" for each node, representing the size or count of data items beneath it.
- This design allows for:
  - **Rapid locate**: Quickly find the position of any fragment or byte offset in a large data item.
  - **Efficient inserts/deletes**: Insert or delete data at any point without reading or writing the entire data item—only the affected fragments/pages are modified.
  - **Scalability**: Supports very large data items by breaking them into manageable pieces, avoiding memory and I/O bottlenecks.

### Weighted BTree and FREC_DATA
- The *weighted BTree* is central to FREC_DATA:
  - Each node's weight is updated as fragments are inserted or deleted.
  - The BTree can efficiently traverse to the correct fragment/page for any given offset by comparing weights, similar to how file systems locate blocks.
  - This enables O(log n) access, insert, and delete operations for large, fragmented data.
- The weight system is also used to maintain balance and optimize performance during BTree operations.

### Relationship to COL Store
- FREC_DATA was specifically created to support the COL store (columnar storage) model in GrapaDB.
- COL store requires efficient, file-system-based storage of large columns, which may be updated or accessed in fragments.
- FREC_DATA, with its weighted BTree, enables COL store to:
  - Store columns as fragmented data efficiently on disk
  - Support rapid updates, appends, and partial reads
  - Avoid the need to rewrite entire columns for small changes

### Best Practices and Notes
- Use BYTE_DATA for small, simple values where fragmentation is unnecessary.
- Use FREC_DATA for large, growing, or frequently updated data items—especially in COL store scenarios.
- When working with FREC_DATA, always update weights correctly during insert/delete to maintain BTree efficiency.
- The weighted BTree approach is extensible and can be adapted for other use cases requiring efficient fragmented data management.

---

## Custom Node Width (NODE_WIDTH) per BTree

A unique feature of GrapaBtree is the ability to specify a custom node width (number of leafs per node) for each BTree instance at creation time.

### How to Specify Node Width
- The `NewTree` function allows you to set a custom node width via the `nodeCount` parameter:
  ```cpp
  GrapaError NewTree(u64& treePtr, u8 treeType, u64 parentTree = 0LL, u8 nodeCount = NODE_WIDTH);
  ```
- If `nodeCount` is not specified, the global `NODE_WIDTH` constant is used (default is 5).
- This lets you tune the BTree structure for specific workloads or storage scenarios.

### Example: Creating a BTree with Custom Node Width
```cpp
u64 myTree;
GrapaBtree btree;
btree.NewTree(myTree, SU64_TREE, 0, 9); // Creates a BTree with node width 9
```

### Trade-offs: Small vs. Large Node Width
- **Smaller NODE_WIDTH (e.g., 3–5):**
  - Shallower nodes, more frequent splits/merges
  - Less memory per node
  - Can be better for small, random updates
  - Deeper trees (more pointer traversals)
- **Larger NODE_WIDTH (e.g., 8–64+):**
  - Fewer levels in the tree (shallower)
  - Fewer splits/merges, better for large, sequential workloads
  - More memory per node
  - Can be more cache/disk block friendly

### When to Use
- For most general use, the default (5) is a safe, balanced choice.
- For large, disk-backed, or bulk-insert workloads, a larger node width may improve performance.
- You can experiment with different values to optimize for your specific data and access patterns.

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

## Related Documentation
- [GRAPA_BTREE_FILE_STRUCTURE.md](./GRAPA_BTREE_FILE_STRUCTURE.md) — Canonical reference for the on-disk BTree file/block structure and manual traversal
- [GRAPA_DB_IMPLEMENTATION.md](./GRAPA_DB_IMPLEMENTATION.md) — Database layer implementation 

## [2025-07-22] Note on Index Entry Value Correctness

- The BTree implementation faithfully inserts whatever value is provided for each key.
- It is the responsibility of higher-level logic (such as GrapaDB) to ensure that index entries (RPTR_ITEM, CPTR_ITEM, GPTR_ITEM) use the correct record reference as the value.
- The BTree implementation is robust and handles all edge cases correctly. 