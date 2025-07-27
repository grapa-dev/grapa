// GrapaDB2.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaDB2_
#define _GrapaDB2_

#include "GrapaBtree.h"
#include "GrapaFileCache.h"
#include "GrapaFileTree.h"
#include "GrapaValue.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

// Forward declarations for GrapaDB2 data structures
class GrapaDB2Field;
class GrapaDB2FieldArray;
class GrapaDB2FieldValue;
class GrapaDB2FieldValueArray;
class GrapaDB2Table;
class GrapaDB2Index;
class GrapaDB2Cursor;

class GrapaDB2 : public GrapaBtree
{
public:
	enum { SEARCH_ITEM=LAST_ITEM, DTYPE_ITEM, GREC_ITEM, RREC_ITEM, CREC_ITEM, GPTR_ITEM, RPTR_ITEM, CPTR_ITEM, };
	enum { GROUP_TREE=LAST_TREE, RTABLE_TREE, CTABLE_TREE, };
	enum { IPTR_STORE=LAST_STORE, };
	enum { NULL_CMP=0, LT_CMP, LTEQ_CMP, EQ_CMP, GTEQ_CMP, GT_CMP, };
public:
	GrapaDB2();
	GrapaDB2(GrapaFile* pFile);
	virtual ~GrapaDB2();
	void INIT(GrapaFile* pFile);
	
	// Core database operations - same interface as GrapaDB
	virtual GrapaError Create(const char *pFileName, u8 treeType, u64& firstTree);
	virtual GrapaError CreateRoot(u8 treeType, u64& firstTree);
	virtual GrapaError OpenFile(const char *fileName, char mode);
	virtual u64 RootTree(u8& pRootType);
	virtual u64 RootTree(GrapaCHAR& pRootType);
	virtual GrapaError CloseFile();

	// Table operations - same interface as GrapaDB
	virtual GrapaError LastTableId(u64 firstTree, u64& pTableId);
	virtual GrapaError CreateTable(u64 firstTree, u8 pTreeType, u64 pTableId, GrapaDB2Table& pTable);
	virtual GrapaError OpenTable(u64 firstTree, u64 pTableId, GrapaDB2Table& pTable);
	virtual GrapaError DeleteTable(u64 firstTree, u64 pTableId);

	virtual GrapaError CreateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDB2Table& pTable);
	virtual GrapaError UpdateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDB2Table& pTable);

	// Field operations - same interface as GrapaDB
	virtual GrapaError CreateTableField(GrapaDB2Table& pTable, GrapaDB2Field& pField, const GrapaCHAR& pName);
	virtual GrapaError OpenTableField(GrapaDB2Table& pTable, u64 pFieldId, GrapaDB2Field& pField);
	virtual GrapaError OpenTableFieldList(GrapaDB2Table& pTable, GrapaDB2FieldArray& pFieldList);
	virtual GrapaError DeleteTableField(GrapaDB2Table& pTable, u64 pFieldId);
	virtual GrapaError FlushTableFields(GrapaDB2Table& pTable);

	// Formula field operations - new functionality for GrapaDB2
	virtual GrapaError CreateFormulaField(GrapaDB2Table& pTable, const GrapaCHAR& pFieldName, const GrapaCHAR& pFormulaText, u8 pResultType);
	virtual GrapaError GetFormulaText(u64 pFormulaRef, GrapaCHAR& pFormulaText);
	virtual GrapaError StoreFormulaText(u64 pFormulaRef, const GrapaCHAR& pFormulaText);
	virtual GrapaError ExecuteFormula(u64 pFormulaRef, u8 pFormulaType, const GrapaCHAR& pParams, GrapaCHAR& pResult);
	
	// Helper methods for formula operations
	virtual GrapaError AllocateFormulaStorage(u64& pFormulaRef);
	virtual u64 GetNextFieldId();

	// Record operations - same interface as GrapaDB
	virtual GrapaError FindFreeRecordId(GrapaDB2Table& pTable, u64& pRecordId);
	virtual GrapaError CreateRecord(GrapaDB2Table& pTable, GrapaCursor& pCursor);
	virtual GrapaError DeleteRecord(GrapaDB2Table& pTable, GrapaCursor& pCursor);

	// Field value operations - same interface as GrapaDB
	virtual GrapaError SetRecordField(GrapaCursor& pCursor, GrapaDB2FieldValueArray& pFieldList);
	virtual GrapaError GetRecordField(GrapaCursor& pCursor, u64 pFieldId, GrapaBYTE& pValue);
	virtual GrapaError GetRecordField(GrapaCursor& pCursor, GrapaDB2Field& field, GrapaBYTE& pValue);

	// Search operations - same interface as GrapaDB
	virtual GrapaError SearchDb(GrapaCursor& pCursor, GrapaDB2Table& pTable, GrapaDB2FieldValueArray& pFieldList);
	virtual GrapaError FirstDb(GrapaCursor& pCursor);
	virtual GrapaError LastDb(GrapaCursor& pCursor);
	virtual GrapaError NextDb(GrapaCursor& pCursor);
	virtual GrapaError PrevDb(GrapaCursor& pCursor);

	// Index operations - same interface as GrapaDB
	virtual GrapaError FindFreeIndexId(GrapaDB2Index& pIndex, u64 pMinId, u64& pIndexId);
	virtual GrapaError CreateIndex(GrapaDB2Table& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDB2Index& pIndex);
	virtual GrapaError OpenIndex(GrapaDB2Table& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDB2Index& pIndex);
	virtual GrapaError DeleteIndex(GrapaDB2Table& pTable, u64 pIndexId);
	virtual GrapaError RefreshIndex(GrapaDB2Index& pIndex);

	// Data operations - same interface as GrapaDB
	virtual GrapaError GetData(u64 itemPtr, GrapaCHAR& pValue);
	virtual GrapaError GetDataSize(u64 itemPtr, u64 growBlockSize, u64& dataSize, u64& dataLength, u8& compressType);
	virtual GrapaError GetDataValue(u64 itemPtr, u64 offset, u64 length, char* data, u64* returnSize);
	virtual GrapaError SetDataValue(u64 itemPtr, u64 offset, u64 length, const char* data);

	// Utility operations - same interface as GrapaDB
	virtual GrapaError CompareKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult);
	virtual GrapaError DumpTree(u64 pTreeRef = 0, GrapaFile* pDumpFile = NULL);

	// Additional methods for GrapaDB2-specific functionality
	virtual GrapaError FindRecordField(GrapaCursor& cursor, u64 fieldId, GrapaCursor& recCursor, GrapaDB2Field& field);
	virtual GrapaError PtrToRec(GrapaCursor& ptrCursor, GrapaCursor& recCursor);
	
protected:
	GrapaFile *mDumpFile;
	
	// Dump system methods for debugging and visualization
	virtual GrapaError DumpTheTree(GrapaCHAR& dbWrite, const char *leader, u64 tableId, u64 firstTree);
	virtual GrapaError DumpTheValue(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheNumber(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheTreeItem(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheDataType(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheDT(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheGroupRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheGroupPtr(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheRowRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheColRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpThePointer(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	virtual GrapaError DumpTheRowStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor);
	virtual GrapaError DumpTheColStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor);
	virtual GrapaError DumpTheStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor, u64 tableDT);
	virtual GrapaError DumpTheGroupStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor);
	
	// Helper functions - same interface as GrapaDB
	virtual GrapaError GetDataTypeRecord(u64 tableRef, u64& tableDT);
};

// GrapaDB2-specific data structures (duplicated from GrapaDB.h with "2" suffix)
class GrapaDB2Field
{
public:
	enum { STORE_FIX = 0, STORE_VAR, STORE_PAR, };
	
	// Formula field types
	enum { FORMULA_TEXT = 1, FORMULA_OP = 2 };

public:
	struct{
		u8 mType;
		u8 mStore;
		u8 mTreeType;
		u8 mReserved[5];
		u64 mId;		// Need to index
		u64 mRef;
		u64 mNameId;
		u64 mNameRef;   // Need to index
		u64 mDictOffset;
		u64 mDictSize;
		u64 mSize;
		u64 mGrow;      // How many items to store in each block
		u64 mTableRef;	// Extended metadata. User configurable.
		u64 mFormulaRef;    // Reference to compressed formula code
		u8 mFormulaType;    // FORMULA_TEXT or FORMULA_OP
		u8 mReserved2[7];   // Reduced from 8 to 7 bytes
	};
public:
	GrapaDB2Field();
	void BigEndian();
	void Init(u64 pFieldId, u8 pType, u8 pStore, u64 pSize, u64 pGrow);
	void* GetPtr();
	inline static u16 GetSize();
	GrapaError Write(GrapaDB2 *pDb, u64 fieldRef);
	GrapaError Read(GrapaDB2 *pDb, u64 fieldRef);
	GrapaError Get(GrapaDB2 *pDb, u64 tableRef, u64 fieldId);
};

class GrapaDB2FieldArray : public GrapaVoidArray
{
public:
	GrapaDB2FieldArray(u32 pCount=0) : GrapaVoidArray(pCount) {};
	~GrapaDB2FieldArray();
public:
	GrapaError Append(GrapaDB2 *pDb, GrapaDB2Table& pTable, u64 pFieldId);
	GrapaError Append(GrapaDB2Field *pField);
	GrapaDB2Field* GetFieldAt(u32 i) { return((GrapaDB2Field*)GetAt(i)); }
};

class GrapaDB2FieldValue : public GrapaDB2Field
{
public:
	GrapaCHAR mValue;
	s16 mCmp;
public:
	GrapaDB2FieldValue() : mCmp(0) {}
	void BigEndian();
};

class GrapaDB2FieldValueArray : public GrapaVoidArray
{
public:
	GrapaDB2FieldValueArray(u32 pCount=0) : GrapaVoidArray(pCount) {};
	~GrapaDB2FieldValueArray();
public:
	GrapaError Append(GrapaDB2 *pDb, GrapaDB2Table& pTable, u64 pFieldId, const GrapaBYTE& pValue, s16 pCmp = 3); // EQ_CMP = 3
	GrapaDB2FieldValue* GetFieldAt(u32 i) {return((GrapaDB2FieldValue*)GetAt(i));}
};

class GrapaDB2Table
{
public:
	GrapaDB2Field mDictField;
	u64 mId;
	u64 mRef;
	u64 mRecRef;
	u8 mRefType;
	// add the name
public:
	GrapaDB2Table() { mId = 0; mRef = 0; mRecRef = 0; mRefType = 0; }
	void BigEndian();
};

class GrapaDB2Index
{
public:
	GrapaDB2Table mTable;
	u64 mId;
	u64 mRef;
	// add the name
public:
	GrapaDB2Index() { mId = 0; mRef = 0; }
	void BigEndian();
};

class GrapaDB2Cursor : public GrapaCursor
{
public:
	GrapaDB2FieldValueArray* mData;
	bool mUsingIndex;
public:
	GrapaDB2Cursor() { mUsingIndex = false; mData = NULL; GrapaCursor(); }
    void SetSearch(GrapaDB2* pDb, u64 pTreeRef, bool pUsingIndex, GrapaDB2FieldValueArray* pData);
};

class GrapaGroup2 : public GrapaDB2
{
public:
	u64 mRootTable;
	u8 mRootType;

public:
	GrapaGroup2();
	GrapaGroup2(GrapaFile*pFile);
	virtual ~GrapaGroup2();
	bool Opened();
	void INIT(GrapaFile*pFile);

public:
	GrapaError OpenFile(GrapaFile*pFile, char mode);
	GrapaError OpenFile(const GrapaCHAR& fileName, char mode);
	GrapaError CloseFile();

	GrapaError CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, u8 pTableType, u64& pNewTree);
	GrapaError CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, GrapaCHAR pTableType, u64& pNewTree);
	GrapaError DeleteGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName);

	GrapaError OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName, u64& pNewTree, u8& pNewType, u64& pTableId);
	GrapaError OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName, u64& pNewTree, GrapaCHAR& pNewType, u64& pTableId);
	GrapaError OpenGroup(u64 parentTree, u8 parentType, u64 pId, u64& pNewTree, u8& pNewType, u64& pTableId, GrapaCHAR& pName);

	GrapaError CreateEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId);
	GrapaError FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId);
	GrapaError DeleteEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName);
	GrapaError DeleteEntry(u64 parentTree, u8 parentType, u64 pId);

	GrapaError CreateField(u64 parentTree, u8 parentType, const char* pFieldName, u8 pType = GrapaTokenType::RAW, u8 pStore = GrapaDB2Field::STORE_VAR, u64 pSize = 32, u64 pGrow = 8);
	GrapaError CreateField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pType = GrapaTokenType::RAW, u8 pStore = GrapaDB2Field::STORE_VAR, u64 pSize = 32, u64 pGrow = 8);
	GrapaError DeleteField(u64 parentTree, u8 parentType, GrapaCHAR& pField);

	GrapaError SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const char* pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, u64 pId, const char* pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pField, const GrapaBYTE& pValue);

	GrapaError GetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaBYTE& pValue);
	GrapaError GetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pField, GrapaBYTE& pValue);

	GrapaError DumpGroup(u64 parentTree, u8 parentType, u64 pId=0, GrapaFile *pDumpFile=NULL);

	GrapaDB2FieldArray* ListFields(u64 parentTree, u8 parentType);
	GrapaError FindField(u64 parentTree, u8 parentType, const GrapaCHAR& pFieldName, GrapaDB2Field& field, u64& pMaxId);
	GrapaError GetNameId(u64 parentTree, u8 parentType, u64& pNameId);
	GrapaError SetNameId(u64 parentTree, u8 parentType, u64 pNameId);
	GrapaError NextNameId(u64 parentTree, u8 parentType, u64& pNameId);

protected:
	GrapaFileTree mTree;
	GrapaCritical mCritical;
};

class GrapaGroup2Event : public GrapaEvent
{
public:
	GrapaCHAR mName;
	GrapaGroup2 mValue;
	u64 mInstanceCount;
public:
	GrapaGroup2Event(const GrapaCHAR& pName, GrapaFile* pFile) { mName.FROM(pName); mInstanceCount = 0; mValue.INIT(pFile); }
	virtual ~GrapaGroup2Event() {}
public:
	virtual inline GrapaEvent* Next() { return(mNext); }
	virtual inline GrapaEvent* Prev() { return(mPrev); }
};

class GrapaGroup2Queue : public GrapaQueue
{
public:
	GrapaGroup2Event* OpenFile(const GrapaCHAR& fileName, GrapaFile* pFile, char mode);
	GrapaGroup2Event* Create(const GrapaCHAR& fileName, GrapaFile* pFile, u8 pType);
	GrapaGroup2Event* Create(const GrapaCHAR& fileName, GrapaFile* pFile, GrapaCHAR& pType);
	void CloseFile(GrapaGroup2Event* pEvent);
protected:
	virtual GrapaGroup2Event* Search(const GrapaCHAR& pName) { GrapaGroup2Event* item = (GrapaGroup2Event*)Head(); while (item) { if (item->mName.StrCmp(pName) == 0) break; item = (GrapaGroup2Event*)item->Next(); } return(item); }
	virtual GrapaEvent* Head(u64 mSkip = 0) { return GrapaQueue::Head(mSkip); }
	virtual GrapaEvent* Tail(u64 mSkip = 0) { return GrapaQueue::Tail(mSkip); }
	virtual GrapaGroup2Event* PopHead() { GrapaGroup2Event*e = (GrapaGroup2Event*)GrapaQueue::PopHead();return(e); }
	virtual GrapaGroup2Event* PopTail() { GrapaGroup2Event*e = (GrapaGroup2Event*)GrapaQueue::PopTail(); return(e); }
	virtual GrapaGroup2Event* PopEvent(GrapaGroup2Event* pEvent) { GrapaGroup2Event*e = (GrapaGroup2Event*)GrapaQueue::PopEvent((GrapaEvent*)pEvent); return(e); }
protected:
	GrapaCritical mCritical;
};

#endif // _GrapaDB2_ 