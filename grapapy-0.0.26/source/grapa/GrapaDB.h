// GrapaDB.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaDB_
#define _GrapaDB_

#include "GrapaBtree.h"
#include "GrapaValue.h"
#include <string.h>

class GrapaDBTable;
class GrapaDBIndex;
class GrapaDBField;
class GrapaDBFieldArray;
class GrapaDBFieldValueArray;
class GrapaDBCursor;

class GrapaDB : public GrapaBtree
{
public:
	enum { SEARCH_ITEM=LAST_ITEM, DTYPE_ITEM, GREC_ITEM, RREC_ITEM, CREC_ITEM, GPTR_ITEM, RPTR_ITEM, CPTR_ITEM, };
	enum { GROUP_TREE=LAST_TREE, RTABLE_TREE, CTABLE_TREE, };
	enum { IPTR_STORE=LAST_STORE, };
	enum { NULL_CMP=0, LT_CMP, LTEQ_CMP, EQ_CMP, GTEQ_CMP, GT_CMP, };
public:
    GrapaDB() : mDumpFile(NULL) {}
public:

	virtual GrapaError Create(const char *pFileName, u8 treeType, u64& firstTree);
	virtual GrapaError CreateRoot(u8 treeType, u64& firstTree);
	virtual GrapaError OpenFile(const char *fileName, char mode);
	virtual u64 RootTree(u8& pRootType);
	virtual u64 RootTree(GrapaCHAR& pRootType);
	virtual GrapaError CloseFile();

	// Need a way to lock a table for writing...use buffer while locked and then move data on unlock
	virtual GrapaError LastTableId(u64 firstTree, u64& pTableId);
	virtual GrapaError CreateTable(u64 firstTree, u8 pTreeType, u64 pTableId, GrapaDBTable& pTable);
	virtual GrapaError OpenTable(u64 firstTree, u64 pTableId, GrapaDBTable& pTable);
	virtual GrapaError DeleteTable(u64 firstTree, u64 pTableId);

	virtual GrapaError CreateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBTable& pTable);
	virtual GrapaError UpdateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBTable& pTable);

	// Need to CreateTableField with all the fields
	virtual GrapaError CreateTableField(GrapaDBTable& pTable, GrapaDBField& pField, const GrapaCHAR& pName);
	virtual GrapaError OpenTableField(GrapaDBTable& pTable, u64 pFieldId, GrapaDBField& pField);
	virtual GrapaError OpenTableFieldList(GrapaDBTable& pTable, GrapaDBFieldArray& pFieldList);
	virtual GrapaError DeleteTableField(GrapaDBTable& pTable, u64 pFieldId);
	virtual GrapaError FlushTableFields(GrapaDBTable& pTable);

	virtual GrapaError GetData(u64 itemPtr, GrapaCHAR& pValue);

	// Need a way to disable the indexes, and then refresh after all the work is done
	virtual GrapaError FindFreeIndexId(GrapaDBIndex& pIndex, u64 pMinId, u64& pIndexId);
	// Need a unique capability. By default, the ID is unique.
	virtual GrapaError CreateIndex(GrapaDBTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBIndex& pIndex);
	virtual GrapaError OpenIndex(GrapaDBTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBIndex& pIndex);
	virtual GrapaError DeleteIndex(GrapaDBTable& pTable, u64 pIndexId);
	virtual GrapaError RefreshIndex(GrapaDBIndex& pIndex);

	virtual GrapaError CreateIndexField(GrapaDBIndex& pIndex, u64 pIndexFieldId, u64 pFieldId);
	virtual GrapaError OpenIndexField(GrapaDBIndex& pIndex, u64 pIndexFieldId, u64& pFieldId);

	virtual GrapaError FindFreeRecordId(GrapaDBTable& pTable, u64& pRecordId);
	// Need to CreateRecord with all the fields filled in
	// Don't update the index on create record. Set the dirty bit in the record added to trigger having indexes updated after.
	// Need and OpenTransaction, CommitTransaction, and CancelTransaction.
	virtual GrapaError CreateRecord(GrapaDBTable& pTable, GrapaCursor& pCursor);
	virtual GrapaError DeleteRecord(GrapaDBTable& pTable, GrapaCursor& pCursor);

	virtual GrapaError FindRecordField(GrapaCursor& pCursor, u64 fieldId, GrapaCursor& recCursor, GrapaDBField& pField);
	virtual GrapaError SetRecordField(GrapaCursor& pCursor, GrapaDBFieldValueArray& pFieldList);
	virtual GrapaError GetRecordField(GrapaCursor& pCursor, GrapaDBField& pField, GrapaBYTE& pValue);
	virtual GrapaError GetRecordField(GrapaCursor& pCursor, u64 pFieldId, GrapaBYTE& pValue);

	virtual GrapaError SearchDb(GrapaDBCursor& pCursor, GrapaDBTable& pTable, GrapaDBFieldValueArray& pFieldList);
	virtual GrapaError FirstDb(GrapaDBCursor& pCursor);
	virtual GrapaError LastDb(GrapaDBCursor& pCursor);
	virtual GrapaError NextDb(GrapaDBCursor& pCursor);
	virtual GrapaError PrevDb(GrapaDBCursor& pCursor);

	// override...don't change parameter list or it will break the override
	virtual GrapaError NewTree(u64& treePtr, u8 treeType, u64 parentTree = 0LL, u8 nodeCount = NODE_WIDTH);
	virtual GrapaError CompareKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult);
	virtual GrapaError DeleteKey(GrapaCursor& pTreeCursor);
	virtual GrapaError Delete(GrapaCursor& cursor);

	// other helper functions
	virtual GrapaError DeleteKeyIndexes(GrapaCursor& pTreeCursor);
	virtual GrapaError DumpFile(GrapaFile *pDumpFile = NULL);
	virtual GrapaError DumpTree(u64 pTreeRef = 0, GrapaFile *pDumpFile = NULL);

protected:
	GrapaFile *mDumpFile;

public: // should be protected, and explosed to GrapaDBField with friend...but doesn't work on g++
	GrapaError GetDataTypeRecord(u64 tableRef, u64& tableDT);

protected:
	//friend GrapaDBField;

	GrapaError CompareSearchKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result);
	GrapaError CompareRecordKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result);

	GrapaError PtrToRec(GrapaCursor& ptrCursor, GrapaCursor& recCursor);

	GrapaError FirstFreeId(u64 tableRef, u64 pMinId, u64& resId);

	//GrapaError SetupDataTypeField(u64 indexRef, u64 fieldId, char *fieldName, u8 fieldType, u64 fieldSize);

	GrapaError LocateIndex(GrapaCursor& cursor, u64 indexRef, u64 fieldId);
	GrapaError InsertIntoIndex(u64 tableRef, u8 pValueType, u64 resId, u64 recordRef);

	bool IndexHasField(GrapaCursor& cursor, u64 fieldId);

	GrapaError DumpTheStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor, u64 tableDT);
	GrapaError DumpTheGroupStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor);
	GrapaError DumpTheRowStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor);
	GrapaError DumpTheColStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor);
	GrapaError DumpTheNumber(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpGetItemWeight(GrapaCursor& cursor, u64& weight);
	GrapaError DumpTheDataType(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheGroupRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheGroupPtr(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheItemRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheRowRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheColRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheDT(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpThePointer(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheValue(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheTreeItem(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor);
	GrapaError DumpTheTree(GrapaCHAR& dbWrite, const char *leader, u64 tableId, u64 firstTree);
};

// need a way for user to extend the dict field definition
// need to be aware of upgrading to newer DB versions where the dict changes...how?
class GrapaDBField
{
public:
	enum { STORE_FIX = 0, STORE_VAR, STORE_PAR, };

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
		u64 mReserved2[2];
	};
public:
	GrapaDBField();
	void BigEndian();
	void Init(u64 pFieldId, u8 pType, u8 pStore, u64 pSize, u64 pGrow);
	void* GetPtr();
	inline static u16 GetSize();
	GrapaError Write(GrapaDB *pDb, u64 fieldRef);
	GrapaError Read(GrapaDB *pDb, u64 fieldRef);
	GrapaError Get(GrapaDB *pDb, u64 tableRef, u64 fieldId);
};

class GrapaDBFieldArray : public GrapaVoidArray
{
public:
	GrapaDBFieldArray(u32 pCount=0) : GrapaVoidArray(pCount) {};
	~GrapaDBFieldArray();
public:
	GrapaError Append(GrapaDB *pDb, GrapaDBTable& pTable, u64 pFieldId);
	GrapaError Append(GrapaDBField *pField);
	GrapaDBField* GetFieldAt(u32 i) { return((GrapaDBField*)GetAt(i)); }
};

class GrapaDBFieldValue : public GrapaDBField
{
public:
	GrapaCHAR mValue;
	s16 mCmp;
public:
	GrapaDBFieldValue() : mCmp(0) {}
	void BigEndian();
};

class GrapaDBFieldValueArray : public GrapaVoidArray
{
public:
	GrapaDBFieldValueArray(u32 pCount=0) : GrapaVoidArray(pCount) {};
	~GrapaDBFieldValueArray();
public:
	GrapaError Append(GrapaDB *pDb, GrapaDBTable& pTable, u64 pFieldId, const GrapaBYTE& pValue, s16 pCmp = GrapaDB::EQ_CMP);
	GrapaDBFieldValue* GetFieldAt(u32 i) {return((GrapaDBFieldValue*)GetAt(i));}
};

class GrapaDBTable
{
public:
	GrapaDBField mDictField;
	u64 mId;
	u64 mRef;
	u64 mRecRef;
	u8 mRefType;
	// add the name
public:
	GrapaDBTable() { mId = 0; mRef = 0; mRecRef = 0; mRefType = 0; }
};

class GrapaDBIndex
{
public:
	GrapaDBTable mTable;
	u64 mId;
	u64 mRef;
	// add the name
public:
	GrapaDBIndex() { mId = 0; mRef = 0; }
};

class GrapaDBCursor : public GrapaCursor
{
public:
	GrapaDBFieldValueArray* mData;
	bool mUsingIndex;
public:
	GrapaDBCursor() { mUsingIndex = false; mData = NULL; GrapaCursor(); }
    void SetSearch(GrapaDB* pDb, u64 pTreeRef, bool pUsingIndex, GrapaDBFieldValueArray* pData);
};

#endif // _GrapaDB_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
