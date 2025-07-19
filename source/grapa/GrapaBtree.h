// GrapaBtree.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaBtree_
#define _GrapaBtree_

#include "GrapaFile.h"
#include "GrapaBtreeBlock.h"

class GrapaCursor;

class GrapaBtree
{
public:
	// if another item is added, emums for dependants will also change, so DB version needs to change with it
	enum { SU64_ITEM=0, TREE_ITEM, SDATA_ITEM, BDATA_ITEM, LAST_ITEM, }; 
	enum { SU64_TREE=0, SDATA_TREE, BDATA_TREE, LAST_TREE, };
	enum { BYTE_DATA = 0, FREC_DATA, LAST_DATA, };
	enum { DATA_STORE=0, LAST_STORE, };
	enum { SEARCH_MODE=0, INSERT_MODE, DELETE_MODE, LAST_MODE, };
	enum { ENCODE_NONE = 0, ENCODE_ZIP = 0x01, ENCODE_AES = 0x02, };
	enum { NODE_WIDTH=5 };
	enum { CMP_LT=-1, CMP_EQ=0, CMP_GT=1, };

	u8 mFlags;
	GrapaFile *mFile;

public:
	GrapaBtree();
	virtual ~GrapaBtree() { SetFile(); mFlags = 0; }

    virtual void    SetFile      (GrapaFile *pFile=NULL);

	virtual GrapaError OpenFile(const char *fileName, char mode); 
	virtual u64 RootTree(u8& pType);
    virtual GrapaError CloseFile    ();
	virtual GrapaError Create(const char *fileName);
	virtual GrapaError RemoveFile(const char *fileName);
    virtual GrapaError FlushFile    ();
	virtual GrapaError ClearFile    ();
	virtual GrapaError PurgeFile    ();
    virtual bool    FileOpened   ();

    virtual GrapaError GetFlags (u8& settings);
    virtual GrapaError SetFlags (u8 settings);
	virtual GrapaError	GetCompressSize(u8& size);
	virtual GrapaError	SetCompressSize(u8 size);

	static inline s8 CompareKey(u64 a, u64 b) {return (a > b) ? CMP_GT : ((b > a) ? CMP_LT : CMP_EQ);}
	virtual GrapaError CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result);
    virtual GrapaError DeleteKey(GrapaCursor& treeCursor);

    virtual GrapaError GetWeight(GrapaCursor& treeCursor, u64& itemWeight);
    virtual GrapaError GainWeight(GrapaCursor& treeCursor, u64& treeValue, u64 itemWeight, u64 childWeight);
    virtual GrapaError LooseWeight(GrapaCursor& treeCursor, u64& treeValue, u64 itemWeight, u64 childWeight);

    virtual GrapaError SetFileTree  (u64 firstTree, bool deleteExisting=true);
    virtual u64 FirstTree        (u64 treePtr);

	virtual GrapaError NewTree      (u64& treePtr, u8 treeType, u64 parentTree=0L, u8 nodeCount=NODE_WIDTH);
    virtual GrapaError DeleteTree   (u64 treePtr);
    virtual GrapaError EmptyTree    (GrapaCursor& cursor);
    virtual GrapaError SetTreeIndex (GrapaCursor& cursor, u64 indexTree);
    virtual GrapaError GetTreeIndex (GrapaCursor& cursor, u64& indexTree);
    virtual GrapaError SetTreeStore (GrapaCursor& cursor, u64 storeTree, u8 storeType);
    virtual GrapaError GetTreeStore (GrapaCursor& cursor, u64& storeTree, u8& storeType);
    virtual GrapaError GetTreeSize  (GrapaCursor& cursor, u64& itemCount);
    virtual GrapaError GetTreeDirty (GrapaCursor& cursor, bool& isDirty);
    virtual GrapaError SetTreeDirty (GrapaCursor& cursor, bool isDirty);
	virtual GrapaError GetTreeType  (GrapaCursor& cursor, u8& treeType);
	virtual GrapaError SetTreeType  (GrapaCursor& cursor, u8 treeType);
	virtual GrapaError GetTreeParent(GrapaCursor& cursor, u64& parentTree);

	// need to add a minByteCount and maxByteCount,
	// and then a way to get the first available block within a size range
	// will be used to string data blocks together for very large BLOBs
	// the BLOCKS will be strung together using a TREE for fast lookups and updates
    virtual GrapaError NewData      (u8 dataType, u64 parentTree, u64 dataSize, u64 growBlockSize, u64 growBlocks, u64& itemPtr, bool clear=false);
    virtual GrapaError DeleteData   (u64 itemPtr);
	virtual GrapaError SetDataSize(u64 itemPtr, u64 dataSize, u64 dataLength, u8 encodeType);
	virtual GrapaError GetDataSize(u64 itemPtr, u64& growBlockSize, u64& dataSize, u64& dataLength, u8& encodeType);
	virtual GrapaError SetFieldType(u64 itemPtr, u8 fieldType);
	virtual GrapaError GetFieldType(u64 itemPtr, u8& fieldType);
	virtual GrapaError ClearDataValue(u64 itemPtr, u64 offset, u64 dataSize);
	virtual GrapaError SetDataValue(u64 itemPtr, u64 offset, u64 dataSize, void *buffer, u64 *returnSize = NULL);
    virtual GrapaError GetDataValue (u64 itemPtr, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize=NULL);
    virtual GrapaError InsertDataValue (u64 itemPtr, u64 offset, u64 dataSize, void *buffer, u64 *returnSize=NULL);
	virtual GrapaError DeleteDataValue (u64 itemPtr, u64 offset, u64 dataSize, u64 *returnSize=NULL);
    virtual GrapaError CopyDataValue (u64 toPtr, u64 toOffset, u64 fromPtr, u64 fromOffset, u64 copySize, u64 *returnSize=NULL);
    virtual GrapaError MoveDataValue (u64 toPtr, u64 toOffset, u64 fromPtr, u64 fromOffset, u64 moveSize, u64 *returnSize=NULL);
	virtual GrapaError GetDataParent(u64 dataRef, u64& parentTree);

    virtual GrapaError Search       (GrapaCursor& cursor);
    virtual GrapaError Insert       (GrapaCursor& cursor);
    virtual GrapaError Update       (GrapaCursor& cursor);
    virtual GrapaError Delete       (GrapaCursor& cursor);

    virtual GrapaError First        (GrapaCursor& cursor);
    virtual GrapaError Last         (GrapaCursor& cursor);
    virtual GrapaError Next         (GrapaCursor& cursor);
    virtual GrapaError Prev         (GrapaCursor& cursor);

protected:
	u8 mBuffer[GrapaBlock::COPYSIZE];

protected:
	GrapaError FindFirstX(u64 offset, GrapaCursor& cursor);
	GrapaError FindLastX(u64 offset, GrapaCursor& cursor);

	bool SearchNode(u64 rootNode, GrapaCursor& key, s8& pos, u64& child, GrapaBlockNodeLeaf& foundKey, s16 compareType);
	bool SearchRc(u64 rootNode, GrapaCursor& key, GrapaBlockNodeLeaf& foundKey);
	GrapaError InsInPage(GrapaBlockTree& head, GrapaCursor& treekey, u64 rootNode, GrapaBlockNodeHeader& page, GrapaBlockNodeLeaf& key, s8 pos);

	void RotateLeafLeft(u64 headRef, GrapaBlockTree& head, s8 pos, u64 rootPage, GrapaBlockNodeHeader& rootTree, 
			u64 leftOffset, GrapaBlockNodeHeader& leftTree, u64 rightOffset, GrapaBlockNodeHeader& rightTree);
	void RotateLeafRight(u64 headRef, GrapaBlockTree& head, s8 pos, u64 rootPage, GrapaBlockNodeHeader& rootTree, 
			u64 leftOffset, GrapaBlockNodeHeader& leftTree, u64 rightOffset, GrapaBlockNodeHeader& rightTree);

	void MirgeLeft(u64 headRef, GrapaBlockTree& head, u64 rootOffset, s8 pos, GrapaBlockNodeHeader& rootTree, 
			u64 leftOffset, GrapaBlockNodeHeader& leftTree, u64 rightOffset, GrapaBlockNodeHeader& rightTree);
	void Split(u64 headRef, GrapaBlockTree& head, u64 rootNode, 
			GrapaBlockNodeHeader& oldPage, GrapaBlockNodeLeaf& key, s8 inPos, GrapaBlockNodeLeaf& pKey);
				
	GrapaError SetKey(GrapaBlockTree& head, GrapaBlockNodeLeaf& leaf, GrapaCursor& cursor);
	GrapaError InsertRc(u64 headRef, GrapaBlockTree& head, u64 rootNode,
			GrapaCursor& key, GrapaBlockNodeLeaf& pKey, GrapaBlockNodeLeaf& fKey, s8& result);
	GrapaError PurgeRc(u64 headRef, GrapaBlockTree& head, u64 rootNode, GrapaCursor& key,
			GrapaBlockNodeHeader& rootTree, s8& result);

	GrapaError UpdateChildInfo(u64 childBlock, u64 newBlock, s8 newIndex);
	GrapaError UpdateLeafInfo(GrapaBlockNodeLeaf* key, u64 newBlock, s8 newIndex);
	GrapaError MoveLeaf(u64 headRef, GrapaBlockTree& head, GrapaBlockNodeHeader& oldPage, u64 rootNode, s8 rootIndex, GrapaBlockNodeHeader& newPage, u64 newBlock, s8 newIndex);

	GrapaError EmptyItem(u64 headRef, GrapaBlockTree& head, u64 pagePos);
	GrapaError AppendNode(u64 headRef, GrapaBlockTree& head, GrapaBlockNodeLeaf& promKey);

	void RotateParrentRight(u64 headRef, GrapaBlockTree& head, u64 middleOffset, GrapaBlockNodeHeader& middleTree);
	void RotateParrentLeft(u64 headRef, GrapaBlockTree& head, u64 middleOffset, GrapaBlockNodeHeader& middleTree);

protected:
	GrapaError GetChildWeight(u64 childBlock, u64& childWeight);
	GrapaError RotateItemWeight(u64 headRef, GrapaBlockTree& head, u64 rootTree, GrapaBlockNodeLeaf& fromKey, GrapaBlockNodeHeader& fromTree, GrapaBlockNodeLeaf& toKey, GrapaBlockNodeHeader& toTree, u64 child);
	GrapaError LooseWeightCascading(GrapaCursor& cursor, u64 rootTree, GrapaBlockNodeHeader& oldPage, u64 child);
	GrapaError GainWeightCascading(GrapaCursor& cursor, u64 rootTree, GrapaBlockNodeHeader& newPage, u64 child, u64 fistChild);
	GrapaError GainChildWeightCascading(GrapaCursor& cursor, u64 rootTree, GrapaBlockNodeHeader& newPage, u64 child, u64 fistChild);

protected:
	bool SearchNodeFrag(u64 rootNode, GrapaCursor& key, s8& pos, u64& child, GrapaBlockNodeLeaf& foundKey, s16 compareType);

	GrapaError NewDataByte(GrapaBlockDataHeader& data, bool clear=false);
    GrapaError DeleteDataByte(GrapaBlockDataHeader& data);
	GrapaError	SetDataSizeByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 dataSize, u64 dataLength);
	GrapaError	GetDataSizeByte(GrapaBlockDataHeader& data, u64& growBlockSize, u64& dataSize, u64& dataLength);
	GrapaError	SetDataValueByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, void *buffer, u64 *returnSize);
	GrapaError	GetDataValueByte(GrapaBlockDataHeader& data, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize);
    GrapaError InsertDataValueByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, void *buffer, u64 *returnSize=NULL);
	GrapaError DeleteDataValueByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, u64 *returnSize=NULL);
    GrapaError MoveDataValueByte(u64 toPtr, GrapaBlockDataHeader& toData, u64 toOffset, u64 fromPtr, GrapaBlockDataHeader& fromData, u64 fromOffset, u64 moveSize, u64 *returnSize=NULL);

    GrapaError NewDataFrag(GrapaBlockDataHeader& data, bool clear=false);
    GrapaError DeleteDataFrag(GrapaBlockDataHeader& data);
	GrapaError	SetDataSizeFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 dataSize, u64 dataLength);
	GrapaError	GetDataSizeFrag(GrapaBlockDataHeader& data, u64& growBlockSize, u64& dataSize, u64& dataLength);
	GrapaError	SetDataValueFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, void *buffer, u64 *returnSize);
	GrapaError	GetDataValueFrag(GrapaBlockDataHeader& data, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize);
    GrapaError InsertDataValueFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, void *buffer, u64 *returnSize=NULL);
	GrapaError DeleteDataValueFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, u64 *returnSize=NULL);
    GrapaError MoveDataValueFrag(u64 toPtr, GrapaBlockDataHeader& toData, u64 toOffset, u64 fromPtr, GrapaBlockDataHeader& fromData, u64 fromOffset, u64 moveSize, u64 *returnSize=NULL);
    GrapaError UpdateDataKeyFrag(GrapaCursor& cursor);

protected:
	u64 FindFreePage(GrapaBlockPage& newHole, u64 maxBlockCount, u64 minBlockCount=0, u64* blocksReturned=NULL);
	u64 NewPage(u64 byteCount, bool clear);
	GrapaError PurgePage(u64 pagePos, u64 byteCount);

};

class GrapaCursor
{
public:
	GrapaCursor() {Set(0);}
    virtual void Set(u64 pTreeRef, s8 pValueType=0, u64 pKey=0, u64 pValue=0, u8 pFlags=0)
	{
		mValueType = pValueType;
		mNodeIndex = 0;
		mFlags = pFlags;
		mTreeType = 0;
		reserved = 0;
		mTreeRef = pTreeRef;
		mKey = pKey;
		mValue = pValue;
		mNodeRef = 0;
		mLength = 0;
	}
public:
	s8 mValueType;
	s8 mNodeIndex;
	u8 mFlags;
	u8 mTreeType;
	u32 reserved;
	u64 mTreeRef;
	u64 mKey;
	u64 mValue;
	u64 mNodeRef;
	u64 mLength;
};

#endif //_GrapaBtree_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
