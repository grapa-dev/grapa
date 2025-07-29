// GrapaDBX.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaDBX_
#define _GrapaDBX_

#include "GrapaBtree.h"
#include "GrapaFileCache.h"
#include "GrapaFileTree.h"
#include "GrapaValue.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

// Forward declarations for GrapaDBX data structures
class GrapaDBXField;
class GrapaDBXFieldArray;
class GrapaDBXFieldValue;
class GrapaDBXFieldValueArray;
class GrapaDBXTable;
class GrapaDBXIndex;
class GrapaDBXCursor;

class GrapaDBX : public GrapaBtree
{
public:
	enum { SEARCH_ITEM=LAST_ITEM, DTYPE_ITEM, DRTYPE_ITEM, DITYPE_ITEM, GREC_ITEM, RREC_ITEM, CREC_ITEM, GPTR_ITEM, RPTR_ITEM, CPTR_ITEM, };
	enum { GROUP_TREE=LAST_TREE, RTABLE_TREE, CTABLE_TREE, };
	enum { IPTR_STORE=LAST_STORE, };
	enum { NULL_CMP=0, LT_CMP, LTEQ_CMP, EQ_CMP, GTEQ_CMP, GT_CMP, };
	
	// Caching configuration options
	enum CachingMode { 
		CACHE_DISABLED = 0,    // No caching (current behavior)
		CACHE_ENABLED = 1,     // Always use caching
		CACHE_AUTO = 2         // Auto-detect based on file size/type
	};
	
public:
	GrapaDBX();
	GrapaDBX(GrapaFile* pFile);
	virtual ~GrapaDBX();
	void INIT(GrapaFile* pFile);
	
	// Caching configuration methods
	virtual void SetCachingMode(CachingMode mode);
	virtual CachingMode GetCachingMode() const { return mCachingMode; }
	virtual void SetCacheSize(u64 size);
	virtual u64 GetCacheSize() const { return mCacheSize; }
	virtual bool IsCachingEnabled() const;
	virtual GrapaError EnableCaching();
	virtual GrapaError DisableCaching();
	
	// Core database operations - same interface as GrapaDB
	virtual GrapaError Create(const char *pFileName, u8 treeType, u64& firstTree);
	virtual GrapaError NewTree(u64& treePtr, u8 treeType, u64 parentTree = 0LL, u8 nodeCount = NODE_WIDTH);
	virtual GrapaError CreateRoot(u8 treeType, u64& firstTree);
	virtual GrapaError OpenFile(const char *fileName, char mode);
	virtual u64 RootTree(u8& pRootType);
	virtual u64 RootTree(GrapaCHAR& pRootType);
	virtual GrapaError CloseFile();

	// Table operations - same interface as GrapaDB
	virtual GrapaError LastTableId(u64 firstTree, u64& pTableId);
	virtual GrapaError FirstFreeId(u64 tableRef, u64 minId, u64& resId);
	virtual GrapaError CreateTable(u64 firstTree, u8 pTreeType, u64 pTableId, GrapaDBXTable& pTable);
	virtual GrapaError OpenTable(u64 firstTree, u64 pTableId, GrapaDBXTable& pTable);
	virtual GrapaError DeleteTable(u64 firstTree, u64 pTableId);

	virtual GrapaError CreateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBXTable& pTable);
	virtual GrapaError UpdateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBXTable& pTable);

	// Field operations - same interface as GrapaDB
	virtual GrapaError CreateTableField(GrapaDBXTable& pTable, GrapaDBXField& pField, const GrapaCHAR& pName);
	virtual GrapaError OpenTableField(GrapaDBXTable& pTable, u64 pFieldId, GrapaDBXField& pField);
	virtual GrapaError OpenTableFieldList(GrapaDBXTable& pTable, GrapaDBXFieldArray& pFieldList);
	virtual GrapaError DeleteTableField(GrapaDBXTable& pTable, u64 pFieldId);
	virtual GrapaError ModifyTableField(GrapaDBXTable& pTable, u64 pFieldId, u8 pNewType, u8 pNewStore, u64 pNewSize, u64 pNewGrow);
	virtual GrapaError FlushTableFields(GrapaDBXTable& pTable);

	// Formula field operations - new functionality for GrapaDBX
	virtual GrapaError CreateFormulaField(GrapaDBXTable& pTable, const GrapaCHAR& pFieldName, const GrapaCHAR& pFormulaText, u8 pResultType);
	virtual GrapaError CreateCompiledFormulaField(GrapaDBXTable& pTable, const GrapaCHAR& pFieldName, const GrapaCHAR& pFormulaText, u8 pResultType);
	virtual GrapaError GetFormulaText(u64 pFormulaRef, GrapaCHAR& pFormulaText);
	virtual GrapaError StoreFormulaText(u64 pFormulaRef, const GrapaCHAR& pFormulaText);
	virtual GrapaError ExecuteFormula(u64 pFormulaRef, u8 pFormulaType, const GrapaCHAR& pParams, GrapaCHAR& pResult);
	
	// Context-aware record environment for formula execution
	virtual GrapaRuleEvent* CreateRecordContext(GrapaCursor& cursor, GrapaDBXTable& table);
	virtual GrapaError RecordGetField(GrapaCursor& cursor, const GrapaCHAR& fieldName, GrapaBYTE& result);
	virtual GrapaError RecordGetFieldPartial(GrapaCursor& cursor, const GrapaCHAR& fieldName, u64 offset, u64 length, GrapaCHAR& operation, GrapaBYTE& result);
	virtual GrapaError GetFieldIdByName(GrapaCursor& cursor, const GrapaCHAR& fieldName, u64& fieldId);
	virtual GrapaError StreamingGrep(GrapaCursor& cursor, const GrapaCHAR& fieldName, u64 offset, u64 length, GrapaBYTE& result);
	virtual GrapaError LoadFieldSubstring(GrapaCursor& cursor, const GrapaCHAR& fieldName, u64 offset, u64 length, GrapaBYTE& result);
	
	// Helper methods for formula operations
	virtual GrapaError AllocateFormulaStorage(u64& pFormulaRef);
	virtual u64 GetNextFieldId();
	
	// Formula execution helper methods
	virtual GrapaError ParseFormulaParams(const GrapaCHAR& pParams, GrapaCursor& cursor, GrapaDBXTable& table);
	virtual GrapaError ExecuteFormulaWithContext(const GrapaCHAR& formulaText, GrapaRuleEvent* context, GrapaCHAR& result);
	virtual GrapaError ExecuteCompiledFormula(const GrapaCHAR& compiledFormula, GrapaRuleEvent* context, GrapaCHAR& result);
	virtual GrapaError CompileFormulaToOP(const GrapaCHAR& formulaText, GrapaCHAR& compiledFormula);
	virtual GrapaError StoreCompiledFormula(u64 pFormulaRef, const GrapaCHAR& compiledFormula);

	// Record operations - same interface as GrapaDB
	virtual GrapaError FindFreeRecordId(GrapaDBXTable& pTable, u64& pRecordId);
	virtual GrapaError CreateRecord(GrapaDBXTable& pTable, GrapaCursor& pCursor);
	virtual GrapaError DeleteRecord(GrapaDBXTable& pTable, GrapaCursor& pCursor);

	// Field value operations - same interface as GrapaDB
	virtual GrapaError SetRecordField(GrapaCursor& pCursor, GrapaDBXFieldValueArray& pFieldList);
	virtual GrapaError GetRecordField(GrapaCursor& pCursor, u64 pFieldId, GrapaBYTE& pValue);
	virtual GrapaError GetRecordField(GrapaCursor& pCursor, GrapaDBXField& field, GrapaBYTE& pValue);
	virtual GrapaError GetRecordFieldData(GrapaCursor& recCursor, GrapaDBXField& field, GrapaBYTE& buffer);

	// Search operations - same interface as GrapaDB
	virtual GrapaError SearchDb(GrapaCursor& pCursor, GrapaDBXTable& pTable, GrapaDBXFieldValueArray& pFieldList);
	virtual GrapaError FirstDb(GrapaCursor& pCursor);
	virtual GrapaError LastDb(GrapaCursor& pCursor);
	virtual GrapaError NextDb(GrapaCursor& pCursor);
	virtual GrapaError PrevDb(GrapaCursor& pCursor);

	// Index operations - same interface as GrapaDB
	virtual GrapaError FindFreeIndexId(GrapaDBXIndex& pIndex, u64 pMinId, u64& pIndexId);
	virtual GrapaError CreateIndex(GrapaDBXTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBXIndex& pIndex);
	virtual GrapaError OpenIndex(GrapaDBXTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBXIndex& pIndex);
	virtual GrapaError DeleteIndex(GrapaDBXTable& pTable, u64 pIndexId);
	virtual GrapaError RefreshIndex(GrapaDBXIndex& pIndex);
	virtual GrapaError InsertIntoIndex(u64 tableRef, u8 pValueType, u64 resId, u64 recordRef);

	// Data operations - same interface as GrapaDB
	virtual GrapaError GetData(u64 itemPtr, GrapaCHAR& pValue);
	virtual GrapaError GetDataSize(u64 itemPtr, u64 growBlockSize, u64& dataSize, u64& dataLength, u8& compressType);
	virtual GrapaError GetDataValue(u64 itemPtr, u64 offset, u64 length, char* data, u64* returnSize);
	virtual GrapaError SetDataValue(u64 itemPtr, u64 offset, u64 dataSize, void *buffer, u64 *returnSize = NULL);

	// Override operations - same interface as GrapaBtree - do not change parameters or parameter types
	virtual GrapaError CompareKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult);
	virtual GrapaError DeleteKey(GrapaCursor& pTreeCursor);
	virtual GrapaError Delete(GrapaCursor& cursor);

    
    // Utility operations - same interface as GrapaDB
	virtual GrapaError CompareRecordKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult);
	virtual GrapaError CompareSearchKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult);
	virtual GrapaError DumpFile(GrapaFile *pDumpFile = NULL);
	virtual GrapaError DumpTree(u64 pTreeRef = 0, GrapaFile* pDumpFile = NULL);
	virtual GrapaError DatabaseDump(u64 pTreeRef, GrapaFile& pDumpFile);

	// Additional methods for GrapaDBX-specific functionality
	virtual GrapaError FindRecordField(GrapaCursor& cursor, u64 fieldId, GrapaCursor& recCursor, GrapaDBXField& field);
	virtual GrapaError PtrToRec(GrapaCursor& ptrCursor, GrapaCursor& recCursor);
	
protected:
	// Caching system - same architecture as GrapaGroup
	GrapaFileCache mTree;           // Caching layer for BTree operations
	CachingMode mCachingMode;       // Current caching mode
	u64 mCacheSize;                 // Cache size in bytes
	
	GrapaFile *mDumpFile;
	
	// Dump system methods for debugging and visualization
	virtual GrapaError DumpTheTree(GrapaCHAR& dbWrite, const char *leader, u64 tableId, u64 firstTree);
	virtual GrapaError DumpTheValue(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheNumber(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheTreeItem(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheDataType(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheDT(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheDRT(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheDIT(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheGroupRec(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheGroupPtr(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheRowRec(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheColRec(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpThePointer(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheRowStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheColStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor);
	virtual GrapaError DumpTheStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor, u64 tableDT);
	virtual GrapaError DumpTheGroupStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor);
	
	// Index helper methods - same interface as GrapaDB
	virtual GrapaError LocateIndex(GrapaCursor& cursor, u64 indexRef, u64 fieldId);
	virtual bool IndexHasField(GrapaCursor& cursor, u64 fieldId);
	
	// Debug functions for index validation and testing
	virtual void DebugPrintIndexPointerAndRecord(u64 tableRef, u64 key);
	virtual void DebugPrintAllIndexPointers(u64 tableRef);
	
	// Debug utility methods
	virtual GrapaError DumpGetItemWeight(GrapaCursor& cursor, u64& weight);
	
	// Helper functions - same interface as GrapaDB
	virtual GrapaError GetDataTypeRecord(u64 tableRef, u64& tableDT);
	virtual GrapaError DeleteKeyIndexes(GrapaCursor& treeCursor);
};

// Replace the class GrapaDBXField definition with a plain struct, matching the reference field order and types
struct GrapaDBXField
{
    enum { STORE_FIX = 0, STORE_VAR, STORE_PAR };
    enum { FORMULA_TEXT = 1, FORMULA_OP = 2 };
    
    // Core field metadata (needs endian conversion)
    u8 mType;
    u8 mStore;
    u8 mTreeType;
    
    // Bit field for constraints and flags (NO endian conversion needed)
    struct {
        u8 mConstraints : 4;    // UNIQUE=1, NOT_NULL=2, CHECK=4, DEFAULT=8
        u8 mIndexed : 1;        // 0/1
        u8 mAutoIncrement : 1;  // 0/1
        u8 mReserved : 2;       // 2 bits reserved
    } mFlags;
    
    u8 mReserved[4];        // Reduced from 5 to 4 bytes
    
    // Field references (needs endian conversion)
    u64 mId;
    u64 mRef;
    u64 mNameId;
    u64 mNameRef;
    u64 mDictOffset;
    u64 mDictSize;
    u64 mSize;
    u64 mGrow;
    u64 mTableRef;
    u64 mFormulaRef;
    u8 mFormulaType;
    
    // Additional bit field (NO endian conversion needed)
    struct {
        u8 mDefaultValueType : 4;  // Type of default value
        u8 mSortOrder : 1;         // ASC=0, DESC=1
        u8 mReserved : 3;          // 3 bits reserved
    } mExtraFlags;
    
    // Constraint references (needs endian conversion)
    u64 mDefaultValueRef;
    u64 mCheckConstraintRef;
    u64 mForeignKeyRef;
    u64 mSequenceRef;
    u64 mStatisticsRef;
    
    u8 mReserved2[3];       // Reduced from 7 to 3 bytes

    GrapaDBXField();
    void BigEndian();
    void Init(u64 pFieldId, u8 pType, u8 pStore, u64 pSize, u64 pGrow);
    void* GetPtr();
    inline static u16 GetSize();
    GrapaError Write(GrapaDBX *pDb, u64 fieldRef);
    GrapaError Read(GrapaDBX *pDb, u64 fieldRef);
    GrapaError Get(GrapaDBX *pDb, u64 tableRef, u64 fieldId);
};

// Index field dictionary - optimized for index metadata
struct GrapaDBXIndexField
{
    enum { INDEX_PRIMARY = 1, INDEX_UNIQUE = 2, INDEX_NORMAL = 3, INDEX_FULLTEXT = 4 };
    enum { METHOD_BTREE = 1, METHOD_HASH = 2, METHOD_RTREE = 3 };
    
    // Core index metadata (needs endian conversion)
    u64 mId;
    u64 mRef;
    u64 mTableRef;
    u64 mIndexNameRef;        // Reference to index name
    
    // Bit field for index properties (NO endian conversion needed)
    struct {
        u8 mIndexType : 3;     // PRIMARY=1, UNIQUE=2, NORMAL=3, FULLTEXT=4
        u8 mIndexMethod : 2;   // BTREE=1, HASH=2, RTREE=3
        u8 mSortOrder : 1;     // ASC=0, DESC=1
        u8 mIsActive : 1;      // 0/1
        u8 mIsUnique : 1;      // 0/1
    } mIndexFlags;
    
    // Index statistics (needs endian conversion)
    u64 mCardinality;         // Number of unique values
    u64 mSelectivity;         // Selectivity ratio
    u64 mLastUpdated;         // Timestamp of last update
    u64 mStatisticsRef;       // Reference to index statistics
    
    // Index references (needs endian conversion)
    u64 mConstraintRef;       // Reference to constraint definition
    u64 mCompositeFieldsRef;  // Reference to composite field list
    u64 mPartialConditionRef; // Reference to partial index condition
    
    // Methods
    GrapaDBXIndexField();
    void BigEndian();
    void Init(u64 pIndexId, u8 pIndexType, u8 pIndexMethod);
    void* GetPtr();
    inline static u16 GetSize();
    GrapaError Write(GrapaDBX *pDb, u64 indexRef);
    GrapaError Read(GrapaDBX *pDb, u64 indexRef);
    GrapaError Get(GrapaDBX *pDb, u64 tableRef, u64 indexId);
};

class GrapaDBXFieldArray : public GrapaVoidArray
{
public:
	GrapaDBXFieldArray(u32 pCount=0) : GrapaVoidArray(pCount) {};
	~GrapaDBXFieldArray();
public:
	GrapaError Append(GrapaDBX *pDb, GrapaDBXTable& pTable, u64 pFieldId);
	GrapaError Append(GrapaDBXField *pField);
	GrapaDBXField* GetFieldAt(u32 i) { return((GrapaDBXField*)GetAt(i)); }
};

class GrapaDBXIndexFieldArray : public GrapaVoidArray
{
public:
	GrapaDBXIndexFieldArray(u32 pCount=0) : GrapaVoidArray(pCount) {};
	~GrapaDBXIndexFieldArray();
public:
	GrapaError Append(GrapaDBX *pDb, GrapaDBXTable& pTable, u64 pIndexId);
	GrapaError Append(GrapaDBXIndexField *pIndexField);
	GrapaDBXIndexField* GetIndexFieldAt(u32 i) { return((GrapaDBXIndexField*)GetAt(i)); }
};

class GrapaDBXFieldValue : public GrapaDBXField
{
public:
	GrapaCHAR mValue;
	s16 mCmp;
public:
	GrapaDBXFieldValue() : mCmp(0) {}
	void BigEndian();
};

class GrapaDBXFieldValueArray : public GrapaVoidArray
{
public:
	GrapaDBXFieldValueArray(u32 pCount=0) : GrapaVoidArray(pCount) {};
	~GrapaDBXFieldValueArray();
public:
	GrapaError Append(GrapaDBX *pDb, GrapaDBXTable& pTable, u64 pFieldId, const GrapaBYTE& pValue, s16 pCmp = 3); // EQ_CMP = 3
	GrapaDBXFieldValue* GetFieldAt(u32 i) {return((GrapaDBXFieldValue*)GetAt(i));}
};

class GrapaDBXTable
{
public:
	GrapaDBXField mDictField;    // Record field dictionary
    u64 mId;
    u64 mRef;
    u64 mRecRef;
    u8 mRefType;
    
    // Bit field for table properties (NO endian conversion needed)
    struct {
        u8 mTableType : 2;     // TABLE=1, VIEW=2, TEMPORARY=3
        u8 mAccessMode : 1;    // READ_ONLY=0, READ_WRITE=1
        u8 mReserved : 5;      // 5 bits reserved
    } mTableFlags;
    
    // Table metadata (needs endian conversion)
    u64 mTableNameRef;        // Reference to table name
    u64 mSchemaRef;           // Reference to schema definition
    u64 mConstraintsRef;      // Reference to table constraints
    u64 mIndexesRef;          // Reference to index list
    u64 mTriggersRef;         // Reference to trigger definitions
    u64 mLastModified;        // Timestamp of last modification
    u64 mRowCount;            // Approximate row count
    u64 mTableSize;           // Approximate table size in bytes
    
public:
	GrapaDBXTable() { mId = 0; mRef = 0; mRecRef = 0; mRefType = 0; }
	void BigEndian();
};

class GrapaDBXIndex
{
public:
	GrapaDBXIndexField mDictField;  // Index field dictionary
    u64 mId;
    u64 mRef;
    
public:
	GrapaDBXIndex() { mId = 0; mRef = 0; }
	void BigEndian();
};

// Helper macros for bit field access
#define IS_UNIQUE(field) ((field.mFlags.mConstraints & 1) != 0)
#define IS_NOT_NULL(field) ((field.mFlags.mConstraints & 2) != 0)
#define IS_CHECK(field) ((field.mFlags.mConstraints & 4) != 0)
#define IS_DEFAULT(field) ((field.mFlags.mConstraints & 8) != 0)
#define IS_INDEXED(field) (field.mFlags.mIndexed != 0)
#define IS_AUTO_INCREMENT(field) (field.mFlags.mAutoIncrement != 0)

#define IS_PRIMARY_INDEX(index) (index.mDictField.mIndexFlags.mIndexType == 1)
#define IS_UNIQUE_INDEX(index) (index.mDictField.mIndexFlags.mIndexType == 2)
#define IS_BTREE_INDEX(index) (index.mDictField.mIndexFlags.mIndexMethod == 1)
#define IS_ACTIVE_INDEX(index) (index.mDictField.mIndexFlags.mIsActive != 0)

#define IS_TABLE(table) (table.mTableFlags.mTableType == 1)
#define IS_VIEW(table) (table.mTableFlags.mTableType == 2)
#define IS_READ_WRITE(table) (table.mTableFlags.mAccessMode == 1)

class GrapaDBXCursor : public GrapaCursor
{
public:
	GrapaDBXFieldValueArray* mData;
	bool mUsingIndex;
public:
	GrapaDBXCursor() { mUsingIndex = false; mData = NULL; }
    void SetSearch(GrapaDBX* pDb, u64 pTreeRef, bool pUsingIndex, GrapaDBXFieldValueArray* pData);
};

class GrapaGroup2 : public GrapaDBX
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
	GrapaError FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId, GrapaDBXCursor& outCursor);
	GrapaError DeleteEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName);
	GrapaError DeleteEntry(u64 parentTree, u8 parentType, u64 pId);

	GrapaError CreateField(u64 parentTree, u8 parentType, const char* pFieldName, u8 pType = GrapaTokenType::RAW, u8 pStore = GrapaDBXField::STORE_VAR, u64 pSize = 32, u64 pGrow = 8);
	GrapaError CreateField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pType = GrapaTokenType::RAW, u8 pStore = GrapaDBXField::STORE_VAR, u64 pSize = 32, u64 pGrow = 8);
	GrapaError DeleteField(u64 parentTree, u8 parentType, GrapaCHAR& pField);
	GrapaError ModifyField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pNewType, u8 pNewStore, u64 pNewSize, u64 pNewGrow);

	GrapaError SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const char* pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, u64 pId, const char* pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pField, const GrapaBYTE& pValue);

	GrapaError GetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaBYTE& pValue);
	GrapaError GetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pField, GrapaBYTE& pValue);

	GrapaError DumpGroup(u64 parentTree, u8 parentType, u64 pId=0, GrapaFile *pDumpFile=NULL);

	/* Index Management Methods */
	GrapaError CreateIndex(const GrapaCHAR& indexName, const GrapaCHAR& fields);
	GrapaError RemoveIndex(const GrapaCHAR& indexName);
	GrapaError ListIndexes(GrapaCHAR& indexList);
	GrapaError RefreshIndex(const GrapaCHAR& indexName);

	GrapaDBXFieldArray* ListFields(u64 parentTree, u8 parentType);
	GrapaError FindField(u64 parentTree, u8 parentType, const GrapaCHAR& pFieldName, GrapaDBXField& field, u64& pMaxId);
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

#endif // _GrapaDBX_ 
