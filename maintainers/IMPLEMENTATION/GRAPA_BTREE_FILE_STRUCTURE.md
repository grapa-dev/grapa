# Grapa BTree File System Structure and Manual Traversal

## Purpose
A deep technical reference for the on-disk (and in-memory) structure of Grapa BTree files. This guide enables maintainers and tool authors to dissect, debug, and recover any Grapa BTree or GrapaDB file by hand.

---

## Block Types and Roles

| Block Type                | Description                                                      |
|--------------------------|------------------------------------------------------------------|
| File Header              | Global file metadata: endianness, version, root pointer, etc.     |
| Tree Root (GrapaBlockTree) | Root of a BTree: pointers to first item, index, store, parent, etc. |
| Node Header (GrapaBlockNodeHeader) | Internal BTree node: parent, child pointers, leaf count, weight   |
| Node Leaf (GrapaBlockNodeLeaf)     | Leaf entry: key, value, child pointer (if internal)              |
| Data Block (GrapaBlockDataHeader)  | Variable-length data storage for BLOBs, strings, etc.            |
| Free Page (GrapaBlockPage)         | Manages free space and page allocation                           |
| First Block (GrapaBlockFirst)      | Manages the first block and free page list                       |

---

## Block Layout and Fields

### File Header (GrapaBlockFileHeader)
- Endianness (u8)
- Version (u8)
- Root pointer (u64)
- ... (other metadata fields)

### Tree Root (GrapaBlockTree)
- Pointers to:
  - First item
  - Index
  - Store
  - Parent
- Item count
- Tree type

### Node Header (GrapaBlockNodeHeader)
- Parent pointer
- Child pointers (array)
- Leaf count
- Weight (for weighted BTree)

### Node Leaf (GrapaBlockNodeLeaf)
- Key
- Value
- Child pointer (if internal)

### Data Block (GrapaBlockDataHeader)
- Data size
- Data pointer(s)

### Free Page (GrapaBlockPage)
- Next/prev free page pointers
- Free space info

### First Block (GrapaBlockFirst)
- Pointer to first block
- Free page list

---

## Linkage and Navigation
- Each block contains pointers (u64 offsets) to related blocks (parent, child, next, prev).
- The file header/root block gives you the entry point to the tree.
- Internal nodes point to children; leaves may point to data blocks.
- Free pages are linked in a free list for efficient allocation.

---

## File Layout Overview

```
+-------------------+
| File Header       |
+-------------------+
| First Block       |
+-------------------+
| Tree Root Block   |
+-------------------+
| Node Headers/Leafs|
+-------------------+
| Data Blocks       |
+-------------------+
| Free Pages        |
+-------------------+
```
- The file starts with the header, followed by the first block, then tree root, nodes, data, and free pages.
- All pointers are file offsets (u64), so the structure can be traversed from any entry point.

---

## Manual Traversal Guide

1. **Start at the File Header:**
   - Read the root pointer and version info.
2. **Find the Tree Root Block:**
   - Use the root pointer to locate the GrapaBlockTree.
3. **Traverse Internal Nodes:**
   - For each node, read its header, then follow child pointers to descend.
   - Use leaf count and weight to guide traversal (especially for FREC_DATA).
4. **Visit Leaves:**
   - Read key/value pairs; if value is a data pointer, follow to data block.
5. **Handle Data Blocks:**
   - For variable-length or fragmented data, follow data block pointers/pages.
6. **Free Pages:**
   - Use the free page list to find available space for new allocations.

---

## Debugging and Forensics Tips
- Use a hex editor to inspect file offsets and block headers.
- Check invariants: parent/child pointers must be consistent, weights must add up.
- If corruption is suspected, walk the tree manually from the root and verify all links.
- Free page list should not overlap with allocated blocks.
- Data blocks should be referenced by at least one leaf or node.

---

## Example Walkthrough

Suppose you have a Grapa BTree file and want to find a specific key:
1. Open the file and read the file header to get the root pointer.
2. Seek to the root block and read its fields.
3. For each internal node, compare the search key to the keys in the node to decide which child to follow.
4. When you reach a leaf node, scan for the key; if found, follow the value pointer to the data block.
5. If the data is fragmented (FREC_DATA), use the weighted BTree to locate the correct fragment/page.

---

## See Also
- [GRAPA_BTREE_IMPLEMENTATION.md](./GRAPA_BTREE_IMPLEMENTATION.md) — Main BTree implementation reference
- [GRAPA_DB_IMPLEMENTATION.md](./GRAPA_DB_IMPLEMENTATION.md) — Database layer implementation 