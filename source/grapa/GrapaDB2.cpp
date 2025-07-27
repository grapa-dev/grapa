// GrapaDB2.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "GrapaDB2.h"
#include "GrapaMem.h"
#include "GrapaCompress.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////

// Constants copied from GrapaGroup.cpp
#define NAME_INDEX_ID (1)
#define NAME_INDEX_FIELD_NAME_ID 1

#define NAME_FIELD_TYPE GrapaTokenType::STR
#define NAME_FIELD_STORE GrapaDB2Field::STORE_FIX
#define NAME_FIELD_SIZE 256
#define NAME_FIELD_GROW 0

#define VALUE_FIELD_TYPE GrapaTokenType::RAW
#define VALUE_FIELD_STORE GrapaDB2Field::STORE_VAR
#define VALUE_FIELD_SIZE 32
#define VALUE_FIELD_GROW 8

// GrapaDB2 implementation - inherits from GrapaBtree
GrapaDB2::GrapaDB2() : GrapaBtree()
{
	mDumpFile = NULL;
}

GrapaDB2::GrapaDB2(GrapaFile* pFile) : GrapaBtree()
{
	mDumpFile = NULL;
	INIT(pFile);
}

GrapaDB2::~GrapaDB2()
{
}

void GrapaDB2::INIT(GrapaFile* pFile)
{
	SetFile(pFile);
}

// Core database operations - delegate to GrapaBtree for now (placeholder implementation)
GrapaError GrapaDB2::Create(const char *pFileName, u8 treeType, u64& firstTree)
{
	printf("[DEBUG] GrapaDB2::Create called with fileName='%s', treeType=%d\n", pFileName, treeType);
	/* Placeholder implementation - GrapaBtree::Create only takes fileName */
	firstTree = 1; /* Default first tree */
	GrapaError err = GrapaBtree::Create(pFileName);
	printf("[DEBUG] GrapaDB2::Create result: %d, firstTree=%llu\n", err, firstTree);
	return err;
}

GrapaError GrapaDB2::CreateRoot(u8 treeType, u64& firstTree)
{
	/* Placeholder implementation - GrapaBtree doesn't have CreateRoot */
	firstTree = 1; /* Default first tree */
	return 0;
}

GrapaError GrapaDB2::OpenFile(const char *fileName, char mode)
{
	printf("[DEBUG] GrapaDB2::OpenFile called with fileName='%s', mode='%c'\n", fileName, mode);
	GrapaError err = GrapaBtree::OpenFile(fileName, mode);
	printf("[DEBUG] GrapaDB2::OpenFile result: %d, FileOpened()=%s\n", err, FileOpened() ? "YES" : "NO");
	return err;
}

u64 GrapaDB2::RootTree(u8& pRootType)
{
	/* Delegate to GrapaBtree for now */
	return GrapaBtree::RootTree(pRootType);
}

u64 GrapaDB2::RootTree(GrapaCHAR& pRootType)
{
	/* Delegate to GrapaBtree for now */
	u8 rootType;
	u64 rootTree = GrapaBtree::RootTree(rootType);
	pRootType.FROM("");
	switch (rootType)
	{
	case GROUP_TREE:	pRootType.FROM("GROUP"); break;
	case RTABLE_TREE:	pRootType.FROM("ROW"); break;
	case CTABLE_TREE:	pRootType.FROM("COL"); break;
	}
	return rootTree;
}

GrapaError GrapaDB2::CloseFile()
{
	return GrapaBtree::CloseFile();
}

// Table operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDB2::LastTableId(u64 firstTree, u64& pTableId)
{
	/* Placeholder implementation - GrapaBtree doesn't have LastTableId */
	pTableId = 0;
	return 0;
}

GrapaError GrapaDB2::CreateTable(u64 firstTree, u8 pTreeType, u64 pTableId, GrapaDB2Table& pTable)
{
	GrapaError err;
	GrapaCursor tableNames,tableNamesDT;
	GrapaDB2Field dbField;
	GrapaDB2Index dbIndex;
	GrapaDB2Table parentDict;

	pTable.mId = pTableId;
	pTable.mRef = 0;
	pTable.mRecRef = 0;
	pTable.mRefType = 0;

	if (firstTree)
	{
		err = OpenTable(firstTree, pTableId, pTable);
		if (!err) return(-1);
	}

	err = NewTree(pTable.mRef,pTreeType,firstTree);
	if (err) return(err);

	pTable.mRefType = pTreeType;

	if (firstTree)
	{
		// FIXED: Use TREE_ITEM instead of GREC_ITEM to match OpenTable's search pattern
		tableNames.Set(firstTree, TREE_ITEM, pTableId, pTable.mRef);
		err = Insert(tableNames);
		if (err)
		{
			DeleteTree(pTable.mRef);
			pTable.mRef = 0;
			pTable.mRefType = 0;
			return(err);
		}
	}

	pTable.mRecRef = pTable.mRef;

	switch (pTable.mRefType)
	{
		case GROUP_TREE:
			{
				err = OpenTable(pTable.mRef,0,parentDict);
				if (err) return(err);
				pTable.mRecRef = parentDict.mRef;
			}
			break;

		default:
			{
				GrapaDU64Array indexList(0);
				err = CreateIndex(pTable,0,indexList,dbIndex);
				if (err) return(err);
				dbField.Init(0,GrapaTokenType::START,0,0,1);
				dbField.mTreeType = pTreeType;
				GrapaCHAR fieldNameLabel("$DICT");
				err = CreateTableField(pTable, dbField, fieldNameLabel);
				if (err) return(err);
			}
			break;
	}

	if (firstTree && pTableId)
	{
		err = OpenTable(firstTree,0,parentDict);
		if (err) return(err);
		tableNames.Set(parentDict.mRecRef,RREC_ITEM,pTableId);
		err = CreateRecord(parentDict,tableNames);
		if (err) return(err);
	}

	return(0);
}

GrapaError GrapaDB2::OpenTable(u64 firstTree, u64 pTableId, GrapaDB2Table& pTable)
{
	GrapaError err;
	GrapaCursor tableCursor,indexTableCursor,indexCursor;
	GrapaDB2Table dictTable;
	GrapaDB2FieldValueArray data;
	u64 indexRef;

	pTable.mId = pTableId;
	pTable.mRef = 0;

	tableCursor.Set(firstTree,TREE_ITEM,pTableId);
	err = Search(tableCursor);
	if (err) return(err);

	pTable.mRef = tableCursor.mValue;
	pTable.mRecRef = tableCursor.mValue;

	tableCursor.Set(tableCursor.mValue);
	err = GetTreeType(tableCursor,pTable.mRefType);

	if (pTableId)
	{
		tableCursor.Set(firstTree,RREC_ITEM,pTableId);
		err = Search(tableCursor);
		if (!err)
		{
			pTable.mRecRef = tableCursor.mValue;
		}
	}

	return(0);
}

GrapaError GrapaDB2::DeleteTable(u64 firstTree, u64 pTableId)
{
	/* Placeholder implementation - GrapaBtree doesn't have DeleteTable */
	return 0;
}

GrapaError GrapaDB2::CreateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDB2Table& pTable)
{
	/* Placeholder implementation - GrapaBtree doesn't have CreateAlias */
	pTable.mRef = pTableId;
	pTable.mRefType = GROUP_TREE;
	pTable.mId = pTableId;
	return 0;
}

GrapaError GrapaDB2::UpdateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDB2Table& pTable)
{
	/* Placeholder implementation - GrapaBtree doesn't have UpdateAlias */
	return 0;
}

// Field operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDB2::CreateTableField(GrapaDB2Table& pTable, GrapaDB2Field& pField, const GrapaCHAR& pName)
{
	/* Simplified implementation for GrapaDB2 - just store the field info */
	// For now, just return success without doing the complex GrapaDB field management
	// This will need to be enhanced to actually store field information
	return 0;
}

GrapaError GrapaDB2::OpenTableField(GrapaDB2Table& pTable, u64 pFieldId, GrapaDB2Field& pField)
{
	/* Placeholder implementation - GrapaBtree doesn't have OpenTableField */
	pField.mId = pFieldId;
	return 0;
}

GrapaError GrapaDB2::OpenTableFieldList(GrapaDB2Table& pTable, GrapaDB2FieldArray& pFieldList)
{
	/* Placeholder implementation - GrapaBtree doesn't have OpenTableFieldList */
	return 0;
}

GrapaError GrapaDB2::DeleteTableField(GrapaDB2Table& pTable, u64 pFieldId)
{
	/* Placeholder implementation - GrapaBtree doesn't have DeleteTableField */
	return 0;
}

GrapaError GrapaDB2::FlushTableFields(GrapaDB2Table& pTable)
{
	/* Placeholder implementation - GrapaBtree doesn't have FlushTableFields */
	return 0;
}

// Record operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDB2::FindFreeRecordId(GrapaDB2Table& pTable, u64& pRecordId)
{
	/* Placeholder implementation - GrapaBtree doesn't have FindFreeRecordId */
	pRecordId = 1;
	return 0;
}

GrapaError GrapaDB2::CreateRecord(GrapaDB2Table& pTable, GrapaCursor& pCursor)
{
	GrapaError err;
	u64 newTree;

	err = NewTree(newTree,RREC_ITEM,pTable.mRecRef);
	if (err) return(err);

	pCursor.mValue = newTree;
	pCursor.mKey = pCursor.mKey;

	err = Insert(pCursor);
	if (err)
	{
		DeleteTree(newTree);
		return(err);
	}

	return(0);
}

GrapaError GrapaDB2::DeleteRecord(GrapaDB2Table& pTable, GrapaCursor& pCursor)
{
	/* Placeholder implementation - GrapaBtree doesn't have DeleteRecord */
	return 0;
}

// Field value operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDB2::SetRecordField(GrapaCursor& pCursor, GrapaDB2FieldValueArray& pFieldList)
{
	/* Enhanced implementation using GrapaBtree's SetDataValue */
	/* Store all field values in the record */
	
	printf("[DEBUG] SetRecordField called with cursor.mValue=%llu\n", pCursor.mValue);
	
	s32 fieldCount = pFieldList.Count();
	printf("[DEBUG] Field count: %d\n", fieldCount);
	if (fieldCount == 0) return 0;
	
	/* Calculate total size needed for all fields */
	u64 totalSize = 0;
	for (s32 i = 0; i < fieldCount; i++) {
		GrapaDB2FieldValue* dbFieldValue = pFieldList.GetFieldAt(i);
		if (dbFieldValue) {
			totalSize += dbFieldValue->mValue.GetSize() + sizeof(u64); // value + size prefix
		}
	}
	
	printf("[DEBUG] Total size needed: %llu\n", totalSize);
	
	/* If cursor has no data pointer, create one */
	if (pCursor.mValue == 0) {
		printf("[DEBUG] Creating new data block with parentTree=%llu\n", pCursor.mTreeRef);
		printf("[DEBUG] File opened: %s\n", FileOpened() ? "YES" : "NO");
		printf("[DEBUG] File pointer: %p\n", mFile);
		u64 itemPtr;
		GrapaError err = NewData(BYTE_DATA, pCursor.mTreeRef, totalSize, 256, 1, itemPtr, true);
		if (err) {
			printf("[DEBUG] NewData failed with error %d\n", err);
			return err;
		}
		pCursor.mValue = itemPtr;
		printf("[DEBUG] Created data block with ptr=%llu\n", itemPtr);
	}
	
	/* Store all field values */
	u64 currentOffset = 0;
	for (s32 i = 0; i < fieldCount; i++) {
		GrapaDB2FieldValue* dbFieldValue = pFieldList.GetFieldAt(i);
		if (!dbFieldValue) continue;
		
		u64 valueSize = dbFieldValue->mValue.GetSize();
		
		/* Store the size prefix */
		printf("[DEBUG] Storing size prefix at offset %llu: %llu\n", currentOffset, valueSize);
		GrapaError err = SetDataValue(pCursor.mValue, currentOffset, sizeof(u64), (const char*)&valueSize);
		if (err) {
			printf("[DEBUG] SetDataValue (size) failed with error %d\n", err);
			return err;
		}
		currentOffset += sizeof(u64);
		
		/* Store the actual value */
		if (valueSize > 0) {
			printf("[DEBUG] Storing value at offset %llu, size: %llu\n", currentOffset, valueSize);
			err = SetDataValue(pCursor.mValue, currentOffset, valueSize, (const char*)dbFieldValue->mValue.GetPtr());
			if (err) {
				printf("[DEBUG] SetDataValue (value) failed with error %d\n", err);
				return err;
			}
			currentOffset += valueSize;
		}
	}
	
	/* Set the data size */
	printf("[DEBUG] About to call SetDataSize with total size: %llu\n", currentOffset);
	GrapaError err = SetDataSize(pCursor.mValue, currentOffset, currentOffset, 0);
	if (err) {
		printf("[DEBUG] SetDataSize failed with error %d\n", err);
		return err;
	}
	printf("[DEBUG] SetDataSize succeeded\n");
	
	printf("[DEBUG] SetRecordField completed successfully\n");
	return 0;
}

GrapaError GrapaDB2::GetRecordField(GrapaCursor& pCursor, u64 pFieldId, GrapaBYTE& pValue)
{
	/* For now, use a simple placeholder implementation */
	/* In a real implementation, this would retrieve the actual field value */
	printf("[DEBUG] GetRecordField: called with fieldId=%llu, cursor.mValue=%llu\n", pFieldId, pCursor.mValue);
	
	/* Placeholder: return a simple test value */
	pValue.FROM("test_value");
	
	printf("[DEBUG] GetRecordField: returning placeholder value, length=%llu\n", pValue.mLength);
	return(0);
}

GrapaError GrapaDB2::GetDataTypeRecord(u64 tableRef, u64& tableDT)
{
	/* For now, use a simple placeholder implementation */
	/* In a real implementation, this would retrieve the dictionary tree reference */
	printf("[DEBUG] GetDataTypeRecord: called with tableRef=%llu\n", tableRef);
	
	/* Placeholder: return tableRef as tableDT for now */
	tableDT = tableRef;
	
	printf("[DEBUG] GetDataTypeRecord: returning tableDT=%llu\n", tableDT);
	return(0);
}

GrapaError GrapaDB2::FindRecordField(GrapaCursor& cursor, u64 fieldId, GrapaCursor& recCursor, GrapaDB2Field& field)
{
	GrapaError err;
	u64 tableRef;
	
	if (mDumpFile) {
		GrapaCHAR debugMsg;
		debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
			"[DEBUG] FindRecordField: fieldId=%llu, cursor.mValue=%llu\n", fieldId, cursor.mValue);
		mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
	}
	
	/* Convert pointer to record if needed */
	err = PtrToRec(cursor, recCursor);
	if (err) {
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] FindRecordField: PtrToRec failed with error %d\n", err);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(err);
	}
	
	/* Read the tree structure to get tree type */
	GrapaBlockTree tree;
	err = tree.Read(mFile, recCursor.mTreeRef);
	if (err) {
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] FindRecordField: tree.Read failed with error %d\n", err);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(err);
	}
	
	recCursor.mTreeType = tree.treeType;
	
	/* Get field information based on tree type */
	switch (recCursor.mTreeType)
	{
	case GROUP_TREE:
		tableRef = recCursor.mTreeRef;
		err = field.Get(this, tableRef, fieldId);
		break;
	case RTABLE_TREE:
		tableRef = recCursor.mTreeRef;
		err = field.Get(this, tableRef, fieldId);
		if (err) return(err);
		break;
	case CTABLE_TREE:
		tableRef = recCursor.mTreeRef;
		err = field.Get(this, tableRef, fieldId);
		if (err) return(err);
		break;
	default:
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] FindRecordField: unknown tree type %d\n", recCursor.mTreeType);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(-1);
	}
	
	if (mDumpFile) {
		GrapaCHAR debugMsg;
		debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
			"[DEBUG] FindRecordField: success, treeType=%d, tableRef=%llu\n", 
			recCursor.mTreeType, tableRef);
		mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
	}
	
	return(err);
}

GrapaError GrapaDB2::GetRecordField(GrapaCursor& pCursor, GrapaDB2Field& field, GrapaBYTE& pValue)
{
	/* Proper BTree-based implementation following GrapaDB pattern */
	GrapaError err;
	u64 returnSize = 0;
	u64 dataRef = pCursor.mValue;
	u64 indexTree, storeTree;
	GrapaCursor fieldCursor, recCursor;
	u8 compressType = 0;
	
	/* Debug dump if enabled */
	if (mDumpFile) {
		GrapaCHAR debugMsg;
		debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
			"[DEBUG] GetRecordField(field): fieldId=%llu, cursor.mValue=%llu\n", field.mId, pCursor.mValue);
		mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
	}
	
	pValue.SetSize(0);
	
	/* Read the tree structure to get tree type and storage info */
	GrapaBlockTree tree;
	err = tree.Read(mFile, pCursor.mTreeRef);
	if (err) {
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] GetRecordField(field): tree.Read failed with error %d\n", err);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(err);
	}
	
	pCursor.mTreeType = tree.treeType;
	indexTree = tree.indexTree;
	storeTree = tree.storeTree;
	
	/* Convert pointer to record if needed */
	err = PtrToRec(pCursor, recCursor);
	if (err) {
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] GetRecordField(field): PtrToRec failed with error %d\n", err);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(err);
	}
	
	/* Set up the value buffer based on field type */
	u8 h[2] = { 0, 0 };
	u64 len = 0;
	u64 offset = 0;
	u64 dataPtr, dataSize, dataLength = 0, growBlockSize;
	pValue.FromDbType(field.mType);
	u8 isRaw = (field.mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
	
	/* Handle different tree types */
	switch (recCursor.mTreeType)
	{
	case SU64_TREE:
		/* Simple unsigned 64-bit tree - direct value */
		pValue.SetLength(sizeof(u64), false);
		*(u64*)pValue.GetPtr() = recCursor.mKey;
		break;
		
	case GROUP_TREE:
		/* Group tree - handle based on storage type */
		switch (field.mStore)
		{
		case GrapaDB2Field::STORE_FIX:
			if (field.mDictSize == 1)
			{
				err = GetDataValue(dataRef, field.mDictOffset, 1, (char*)h, &returnSize);
				if ((h[0] & 0x80) == 0)
					pValue.SetSize(0);
				else
				{
					h[0] &= 0x7F;
					pValue.SetLength(1, false);
					((u8*)pValue.GetPtr())[0] = h[0];
				}
			}
			else
			{
				err = GetDataValue(dataRef, field.mDictOffset, field.mDictSize, (char*)pValue.GetPtr(), &returnSize);
				if (!err)
					pValue.SetLength(returnSize, false);
			}
			break;
			
		case GrapaDB2Field::STORE_VAR:
		case GrapaDB2Field::STORE_PAR:
			err = GetDataValue(dataRef, field.mDictOffset, sizeof(dataPtr), (char*)&dataPtr, &returnSize);
			if (!err && dataPtr)
			{
				err = GetDataValue(dataPtr, 0, field.mSize, (char*)pValue.GetPtr(), &returnSize);
				if (!err)
					pValue.SetLength(returnSize, false);
			}
			break;
		}
		break;
		
	case RTABLE_TREE:
		/* Row table tree - similar to group but with row-specific handling */
		switch (field.mStore)
		{
		case GrapaDB2Field::STORE_FIX:
			if (field.mDictSize == 1)
			{
				err = GetDataValue(dataRef, field.mDictOffset, 1, (char*)h, &returnSize);
				if ((h[0] & 0x80) == 0)
					pValue.SetSize(0);
				else
				{
					h[0] &= 0x7F;
					pValue.SetLength(1, false);
					((u8*)pValue.GetPtr())[0] = h[0];
				}
			}
			else
			{
				err = GetDataValue(dataRef, field.mDictOffset, field.mDictSize, (char*)pValue.GetPtr(), &returnSize);
				if (!err)
					pValue.SetLength(returnSize, false);
			}
			break;
			
		case GrapaDB2Field::STORE_VAR:
		case GrapaDB2Field::STORE_PAR:
			err = GetDataValue(dataRef, field.mDictOffset, sizeof(dataPtr), (char*)&dataPtr, &returnSize);
			if (!err && dataPtr)
			{
				err = GetDataValue(dataPtr, 0, field.mSize, (char*)pValue.GetPtr(), &returnSize);
				if (!err)
					pValue.SetLength(returnSize, false);
			}
			break;
		}
		break;
		
	case CTABLE_TREE:
		/* Column table tree - column-specific handling */
		switch (field.mStore)
		{
		case GrapaDB2Field::STORE_FIX:
			if (field.mDictSize == 1)
			{
				err = GetDataValue(dataRef, field.mDictOffset, 1, (char*)h, &returnSize);
				if ((h[0] & 0x80) == 0)
					pValue.SetSize(0);
				else
				{
					h[0] &= 0x7F;
					pValue.SetLength(1, false);
					((u8*)pValue.GetPtr())[0] = h[0];
				}
			}
			else
			{
				err = GetDataValue(dataRef, field.mDictOffset, field.mDictSize, (char*)pValue.GetPtr(), &returnSize);
				if (!err)
					pValue.SetLength(returnSize, false);
			}
			break;
			
		case GrapaDB2Field::STORE_VAR:
		case GrapaDB2Field::STORE_PAR:
			err = GetDataValue(dataRef, field.mDictOffset, sizeof(dataPtr), (char*)&dataPtr, &returnSize);
			if (!err && dataPtr)
			{
				err = GetDataValue(dataPtr, 0, field.mSize, (char*)pValue.GetPtr(), &returnSize);
				if (!err)
					pValue.SetLength(returnSize, false);
			}
			break;
		}
		break;
		
	default:
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] GetRecordField(field): unknown tree type %d\n", recCursor.mTreeType);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(-1);
	}
	
	if (mDumpFile) {
		GrapaCHAR debugMsg;
		debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
			"[DEBUG] GetRecordField(field): success, value length=%llu\n", pValue.mLength);
		mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
	}
	
	return(err);
}

// Placeholder implementations for virtual methods
GrapaError GrapaDB2::CompareKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult)
{
	/* Placeholder implementation */
	pResult = 0;
	return 0;
}

GrapaError GrapaDB2::GetDataValue(u64 itemPtr, u64 offset, u64 length, char* data, u64* returnSize)
{
	/* Delegate to parent GrapaBtree implementation */
	return GrapaBtree::GetDataValue(itemPtr, offset, length, data, returnSize);
}

GrapaError GrapaDB2::SetDataValue(u64 itemPtr, u64 offset, u64 length, const char* data)
{
	/* Delegate to parent GrapaBtree implementation */
	return GrapaBtree::SetDataValue(itemPtr, offset, length, (void*)data);
}

GrapaError GrapaDB2::GetDataSize(u64 itemPtr, u64 growBlockSize, u64& dataSize, u64& dataLength, u8& compressType)
{
	/* Delegate to parent GrapaBtree implementation */
	return GrapaBtree::GetDataSize(itemPtr, growBlockSize, dataSize, dataLength, compressType);
}

GrapaError GrapaDB2::GetData(u64 itemPtr, GrapaCHAR& pValue)
{
	/* Implement using BTree methods */
	u64 dataSize = 0, dataLength = 0;
	u8 compressType = 0;
	
	// Get the data size information
	GrapaError err = GetDataSize(itemPtr, 0, dataSize, dataLength, compressType);
	if (err) return err;
	
	if (dataLength == 0) {
		pValue.FROM("");
		return 0;
	}
	
	// Allocate buffer for the data
	pValue.SetLength(dataLength, false);
	if (!pValue.mBytes) return -1;
	
	// Read the data
	u64 returnSize = 0;
	err = GetDataValue(itemPtr, 0, dataLength, (char*)pValue.mBytes, &returnSize);
	if (err) return err;
	
	pValue.SetLength(returnSize, true);
	return 0;
}

GrapaError GrapaDB2::CreateIndex(GrapaDB2Table& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDB2Index& pIndex)
{
	/* Basic BTree-based index creation */
	// TODO: Implement full index creation with proper BTree operations
	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mTable = pTable;
	return 0;
}

GrapaError GrapaDB2::OpenIndex(GrapaDB2Table& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDB2Index& pIndex)
{
	/* Basic index opening */
	// TODO: Implement full index opening with BTree operations
	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mTable = pTable;
	return 0;
}

GrapaError GrapaDB2::DeleteIndex(GrapaDB2Table& pTable, u64 pIndexId)
{
	/* Basic index deletion */
	// TODO: Implement full index deletion with BTree operations
	return 0;
}

GrapaError GrapaDB2::RefreshIndex(GrapaDB2Index& pIndex)
{
	/* Basic index refresh */
	// TODO: Implement full index refresh with BTree operations
	return 0;
}

GrapaError GrapaDB2::FindFreeIndexId(GrapaDB2Index& pIndex, u64 pMinId, u64& pIndexId)
{
	/* Simple index ID allocation */
	// TODO: Implement proper BTree-based ID tracking
	pIndexId = pMinId;
	return 0;
}

GrapaError GrapaDB2::SearchDb(GrapaCursor& pCursor, GrapaDB2Table& pTable, GrapaDB2FieldValueArray& pFieldList)
{
	/* Basic database search */
	// TODO: Implement full search with BTree operations and field matching
	return 0;
}

GrapaError GrapaDB2::FirstDb(GrapaCursor& pCursor)
{
	/* Get first record in database */
	// TODO: Implement full first record retrieval with BTree operations
	return 0;
}

GrapaError GrapaDB2::LastDb(GrapaCursor& pCursor)
{
	/* Get last record in database */
	// TODO: Implement full last record retrieval with BTree operations
	return 0;
}

GrapaError GrapaDB2::NextDb(GrapaCursor& pCursor)
{
	/* Get next record in database */
	// TODO: Implement full next record retrieval with BTree operations
	return 0;
}

GrapaError GrapaDB2::PrevDb(GrapaCursor& pCursor)
{
	/* Get previous record in database */
	// TODO: Implement full previous record retrieval with BTree operations
	return 0;
}

GrapaError GrapaDB2::DumpTree(u64 pTreeRef, GrapaFile* pDumpFile)
{
	GrapaError err = 0;
	GrapaFile *oldDumpFile = mDumpFile;

	if (pDumpFile)
		mDumpFile = pDumpFile;

	GrapaCHAR dbWrite;
	dbWrite.GrowSize(40000, false);
	dbWrite.SetLength(0);

	if (pTreeRef == 0) pTreeRef = 1; /* Default to first tree for now */
	err = DumpTheTree(dbWrite, "", 0, pTreeRef);

	mDumpFile = oldDumpFile;

	return err;
}

// Dump helper methods - placeholder implementations
GrapaError GrapaDB2::DumpTheTree(GrapaCHAR& dbWrite, const char *leader, u64 tableId, u64 firstTree)
{
	GrapaError err=0;
	GrapaCursor cursor;
	char leadbuf[201];
	u8 treeType=0;
	u64 parentTree;
	u64 numItems;
	u64 weight;

	if (firstTree==0)
	{
		return((GrapaError)-1);
	}

	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");

	cursor.Set(firstTree);
	
	/* Get real tree information from BTree */
	err = GetTreeType(cursor, treeType);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		treeType = GROUP_TREE;
	}
	
	err = GetTreeParent(cursor, parentTree);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		parentTree = 0;
	}
	
	err = GetTreeSize(cursor, numItems);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		numItems = 1;
	}
	
	err = GetWeight(cursor, weight);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		weight = 1;
	}

	char* treeTypeStr=(char*)"";

	switch(treeType)
	{
		case SU64_TREE:	treeTypeStr = (char*)"SU64"; break;
		case GROUP_TREE:	treeTypeStr = (char*)"GROUP"; break;
		case RTABLE_TREE:	treeTypeStr = (char*)"RTABLE"; break;
		case CTABLE_TREE:	treeTypeStr = (char*)"CTABLE"; break;
		case SDATA_TREE:	treeTypeStr = (char*)"SDATA"; break;
		case BDATA_TREE:	treeTypeStr = (char*)"BDATA"; break;
		default:		treeTypeStr = (char*)"UNKNOWN"; break;
	}

	cursor.Set(firstTree);
	err = FirstDb(cursor);
	if (!err)
	{
			dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sLIST start (%llu) key=%llu type=%s parent=%llu size=%llu weight=%llu\n",leader,firstTree,tableId,treeTypeStr,parentTree,numItems,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		while(!err)
		{
			DumpTheValue(dbWrite,leadbuf,cursor);
			err = NextDb(cursor);
		}
				dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sLIST ended (%llu) key=%llu type=%s weight=%llu\n",leader,firstTree,tableId,treeTypeStr,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	}

	return(0);
}

GrapaError GrapaDB2::DumpTheValue(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char leadbuf[201];

	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");

	switch(cursor.mValueType)
	{
		case SU64_ITEM:
			DumpTheNumber(dbWrite,leadbuf,cursor);
			break;
		case TREE_ITEM:
			DumpTheTreeItem(dbWrite,leadbuf,cursor);
			break;
		case SDATA_ITEM:
			DumpTheDataType(dbWrite,leadbuf,cursor);
			break;
		case BDATA_ITEM:
			DumpTheDataType(dbWrite,leadbuf,cursor);
			break;

		case SEARCH_ITEM:
			break;
		case DTYPE_ITEM:
			DumpTheDT(dbWrite,leadbuf,cursor);
			break;

		case GREC_ITEM:
			DumpTheGroupRec(dbWrite,leadbuf,cursor);
			break;
		case RREC_ITEM:
			DumpTheRowRec(dbWrite,leadbuf,cursor);
			break;
		case CREC_ITEM:
			DumpTheColRec(dbWrite,leadbuf,cursor);
			break;

		case GPTR_ITEM:
			DumpTheGroupPtr(dbWrite,leadbuf,cursor);
			break;

		case RPTR_ITEM:
		case CPTR_ITEM:
			DumpThePointer(dbWrite,leadbuf,cursor);
			break;

	}
	return(0);
}

GrapaError GrapaDB2::DumpTheNumber(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sSU64 key=%llu value=%llu\n",leader,cursor.mKey,cursor.mValue);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDB2::DumpThePointer(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	u64 weight;
	/* Get real weight from BTree */
	GrapaCursor weightCursor;
	weightCursor.Set(cursor.mValue);
	GrapaError err = GetWeight(weightCursor, weight);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		weight = 1;
	}
	
	char* itemTypeStr=(char*)"PTR";
	switch(cursor.mValueType)
	{
		case GPTR_ITEM:	itemTypeStr = (char*)"GPTR"; break;
		case RPTR_ITEM:	itemTypeStr = (char*)"RPTR"; break;
		case CPTR_ITEM:	itemTypeStr = (char*)"CPTR"; break;
	}

	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%s%s (%llu) key=%llu node=(%llu,%d) weight=%llu: ",leader,itemTypeStr,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	
	/* Try to dereference the pointer to show the record */
	GrapaCursor recCursor = cursor;
	PtrToRec(cursor,recCursor);

	u64 weight2;
	/* Get real weight for the record */
	GrapaCursor weightCursor2;
	weightCursor2.Set(recCursor.mValue);
	err = GetWeight(weightCursor2, weight2);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		weight2 = 1;
	}
	
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "RREC (%llu) key=%llu node=(%llu,%d) weight=%llu: ", recCursor.mValue, recCursor.mKey, recCursor.mNodeRef, recCursor.mNodeIndex, weight2);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);

	switch(cursor.mValueType)
	{
		case GPTR_ITEM:	DumpTheGroupStructure(dbWrite,recCursor); break;
		case RPTR_ITEM:	DumpTheRowStructure(dbWrite,recCursor); break;
		case CPTR_ITEM:	DumpTheColStructure(dbWrite,recCursor); break;
	}
	if (mDumpFile) mDumpFile->Append(1,(void*)"\n");
	return(0);
}

GrapaError GrapaDB2::DumpTheGroupPtr(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char leadbuf[201];
	u64 weight;
	/* Get real weight from BTree */
	GrapaCursor weightCursor;
	weightCursor.Set(cursor.mValue);
	GrapaError err = GetWeight(weightCursor, weight);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		weight = 1;
	}
	
	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sGPTR start (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	DumpTheTree(dbWrite,leadbuf,cursor.mKey,cursor.mValue);
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sGPTR ended (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDB2::DumpTheRowRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	u64 weight;
	/* Get real weight from BTree */
	GrapaCursor weightCursor;
	weightCursor.Set(cursor.mValue);
	GrapaError err = GetWeight(weightCursor, weight);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		weight = 1;
	}
	
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sRREC (%llu) key=%llu node=(%llu,%d) weight=%llu: ",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	DumpTheRowStructure(dbWrite,cursor);
	if (mDumpFile) mDumpFile->Append(1,(void*)"\n");
	return(0);
}

GrapaError GrapaDB2::DumpTheColRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	u64 weight;
	/* Get real weight from BTree */
	GrapaCursor weightCursor;
	weightCursor.Set(cursor.mValue);
	GrapaError err = GetWeight(weightCursor, weight);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		weight = 1;
	}
	
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sCREC (%llu) key=%llu node=(%llu,%d) weight=%llu: ",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	DumpTheColStructure(dbWrite,cursor);
	if (mDumpFile) mDumpFile->Append(1,(void*)"\n");
	return(0);
}

GrapaError GrapaDB2::DumpTheTreeItem(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char leadbuf[201];
	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sTREE start (%llu) key=%llu\n",leader,cursor.mValue,cursor.mKey);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	DumpTheTree(dbWrite,leadbuf,cursor.mKey,cursor.mValue);
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sTREE ended (%llu) key=%llu\n",leader,cursor.mValue,cursor.mKey);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDB2::DumpTheDT(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaDB2Field dbField;
	char nameBlock[201];
	u64 growBlockSize, dataSize, dataLength, returnLen;
	u8 compressType=0;
	u64 weight;
	
	/* Get real weight from BTree */
	GrapaCursor weightCursor;
	weightCursor.Set(cursor.mValue);
	err = GetWeight(weightCursor, weight);
	if (err) {
		/* Fallback to placeholder if BTree method not available */
		weight = 1;
	}

	err = dbField.Read(this,cursor.mValue);
	if (err) 
	{
		/* If we can't read the field, just show basic info */
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDT (%llu) key=%llu node=(%llu,%d) weight=%llu [placeholder]\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		return(0);
	}

	nameBlock[0] = 0;
	if (dbField.mNameRef)
	{
		err = GetDataSize(dbField.mNameRef, growBlockSize, dataSize, dataLength, compressType);
		err = GetDataValue(dbField.mNameRef, 0, 200, (char*)nameBlock, &returnLen);
		if (returnLen>dataLength) returnLen = dataLength;
		nameBlock[returnLen] = 0;
	}
	char *fieldTypeStr = (char*)"?";
	switch(dbField.mType)
	{
		case GrapaTokenType::START: fieldTypeStr = (char*)"DICT"; break;
		case GrapaTokenType::ERR: fieldTypeStr = (char*)"ERR"; break;
		case GrapaTokenType::RAW: fieldTypeStr = (char*)"RAW"; break;
		case GrapaTokenType::BOOL: fieldTypeStr = (char*)"BOOL"; break;
		case GrapaTokenType::INT: fieldTypeStr = (char*)"INT"; break;
		case GrapaTokenType::FLOAT: fieldTypeStr = (char*)"FLOAT"; break;
		case GrapaTokenType::STR: fieldTypeStr = (char*)"STR"; break;
		case GrapaTokenType::TIME: fieldTypeStr = (char*)"TIME"; break;
		case GrapaTokenType::ARRAY: fieldTypeStr = (char*)"ARRAY"; break;
		case GrapaTokenType::TUPLE: fieldTypeStr = (char*)"TUPLE"; break;
		case GrapaTokenType::VECTOR: fieldTypeStr = (char*)"VECTOR"; break;
		case GrapaTokenType::WIDGET: fieldTypeStr = (char*)"WIDGET"; break;
		case GrapaTokenType::LIST: fieldTypeStr = (char*)"LIST"; break;
		case GrapaTokenType::XML: fieldTypeStr = (char*)"XML"; break;
		case GrapaTokenType::CODE: fieldTypeStr = (char*)"CODE"; break;
		case GrapaTokenType::RULE: fieldTypeStr = (char*)"RULE"; break;
		case GrapaTokenType::OP: fieldTypeStr = (char*)"OP"; break;
		case GrapaTokenType::TAG: fieldTypeStr = (char*)"TAG"; break;
		case GrapaTokenType::SYSID: fieldTypeStr = (char*)"SYSID"; break;
		case GrapaTokenType::SYSINT: fieldTypeStr = (char*)"SYSINT"; break;
		case GrapaTokenType::SYSSTR: fieldTypeStr = (char*)"SYSSTR"; break;
		case GrapaTokenType::TABLE: fieldTypeStr = (char*)"TABLE"; break;
		default: fieldTypeStr = (char*)"UNKNOWN"; break;
	}

	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDT (%llu) key=%llu node=(%llu,%d) weight=%llu: id=%llu name=%s type=%s size=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight,dbField.mId,nameBlock,fieldTypeStr,dbField.mSize);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}



GrapaError GrapaDB2::DumpTheColStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaCursor itemCursor;

	tableRef = cursor.mTreeRef;
	err = GetDataTypeRecord(tableRef,tableDT);
	if (err) 
	{
		/* If we can't get the data type record, just show basic info */
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "CREC_STRUCT [placeholder]");
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		return(0);
	}
	
	itemCursor.Set(tableRef,CREC_ITEM,cursor.mKey,cursor.mValue);
	itemCursor.mNodeRef = cursor.mNodeRef;
	itemCursor.mNodeIndex = cursor.mNodeIndex;
	itemCursor.mLength = cursor.mLength;
	return DumpTheStructure(dbWrite,itemCursor,tableDT);
}

////////////////////////////////////////////////////////////////////////////////

// GrapaGroup2 implementation - inherits from GrapaDB2
GrapaGroup2::GrapaGroup2() : GrapaDB2()
{
	mRootTable = 0;
	mRootType = 0;
}

GrapaGroup2::GrapaGroup2(GrapaFile* pFile) : GrapaDB2(pFile)
{
	mRootTable = 0;
	mRootType = 0;
	INIT(pFile);
}

GrapaGroup2::~GrapaGroup2()
{
	if (mFile) mFile->Flush();
	mRootTable = 0;
	mRootType = 0;
}

bool GrapaGroup2::Opened()
{
	return mRootTable != 0;
}

void GrapaGroup2::INIT(GrapaFile* pFile)
{
	mTree.SetFile(pFile);
	SetFile(&mTree);
}

GrapaError GrapaGroup2::OpenFile(GrapaFile* pFile, char mode)
{
	GrapaCHAR s;
	GrapaError err;
	INIT(pFile);
	s.FROM("$");
	err = OpenFile(s, GrapaReadWrite);
	return(err);
}

GrapaError GrapaGroup2::OpenFile(const GrapaCHAR& fileName, char mode)
{
	GrapaError err = 0;
	mCritical.WaitCritical();
	err = GrapaDB2::OpenFile((const char*)fileName.mBytes, mode);
	mRootTable = RootTree(mRootType);
	if (mRootType == GROUP_TREE)
	{
		GrapaDB2Table parentDict;
		err = OpenTable(mRootTable, 0, parentDict);
		if (!err)
		{
			mRootTable = parentDict.mRef;
			mRootType = parentDict.mRefType;
		}
	}
	mCritical.LeaveCritical();
	return(err);
}

GrapaError GrapaGroup2::CloseFile()
{
	mCritical.WaitCritical();
	GrapaError err = GrapaDB2::CloseFile();
	mRootTable = 0;
	mRootType = 0;
	mCritical.LeaveCritical();
	return(err);
}

// Hierarchical operations - delegate to GrapaGroup for now (placeholder implementation)
GrapaError GrapaGroup2::CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, u8 pTableType, u64& pNewTree)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2Table newTable;
	GrapaDB2FieldValueArray data;

	pNewTree = 0;

	if (pTableName.mLength == 0 || pTableName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		return(err);
	}
	if (nameId == 0)
	{
		return(-1);
	}

	data.Append(this, parentDict, nameId, pTableName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (!err)
	{
		return(-1);
	}

	u64 tableId = 0;
	err = LastTableId(parentTree, tableId);
	if (err)
	{
		return(err);
	}
	tableId++;

	err = CreateTable(parentTree, pTableType, tableId, newTable);
	if (err)
	{
		return(err);
	}

	pNewTree = newTable.mRef;

	err = CreateRecord(parentDict, cursor);
	if (err)
	{
		return(err);
	}

	{
		GrapaDB2FieldValueArray data2;
		data2.Append(this, parentDict, nameId, pTableName, EQ_CMP);
		err = SetRecordField(cursor, data2);
		if (err)
		{
			return(err);
		}
	}

	return(0);
}

GrapaError GrapaGroup2::CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, GrapaCHAR pTableType, u64& pNewTree)
{
	u8 tableType = GROUP_TREE;
	if (pTableType.StrCmp("ROW") == 0) tableType = RTABLE_TREE;
	else if (pTableType.StrCmp("COL") == 0) tableType = CTABLE_TREE;
	return CreateGroup(parentTree, parentType, pTableName, tableType, pNewTree);
}

GrapaError GrapaGroup2::DeleteGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName)
{
	return -1; // TODO: Implement
}

GrapaError GrapaGroup2::OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pEntryName, u64& pNewTree, u8& pNewType, u64& pTableId)
{

	GrapaError err;
	GrapaDB2FieldValueArray data;
	GrapaDB2Table parentDict;
	GrapaDB2Cursor cursor;
	GrapaDB2Table table;

	pNewTree = 0;
	pNewType = 0;
	pTableId = 0;

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		return(err);
	}
	if (nameId == 0)
	{
		return(-1);
	}

	data.Append(this, parentDict, nameId, pEntryName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (err)
	{
		return(err);
	}

	err = OpenTable(parentTree, cursor.mKey, table);
	if (err)
	{
		return(err);
	}

	pNewTree = table.mRef;
	pNewType = table.mRefType;
	pTableId = table.mId;

	return(0);
}

GrapaError GrapaGroup2::OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pEntryName, u64& pNewTree, GrapaCHAR& pNewType, u64& pTableId)
{
	u8 newType;
	GrapaError err = OpenGroup(parentTree, parentType, pEntryName, pNewTree, newType, pTableId);
	pNewType.FROM("");
	switch (newType)
	{
	case GROUP_TREE:	pNewType.FROM("GROUP"); break;
	case RTABLE_TREE:	pNewType.FROM("ROW"); break;
	case CTABLE_TREE:	pNewType.FROM("COL"); break;
	}
	return(err);
}

GrapaError GrapaGroup2::OpenGroup(u64 parentTree, u8 parentType, u64 pId, u64& pNewTree, u8& pNewType, u64& pTableId, GrapaCHAR& pName)
{

	GrapaError err;
	GrapaDB2Table parentDict;
	GrapaDB2Table table;

	pNewTree = 0;
	pNewType = 0;
	pTableId = 0;

	if (pId == 0)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	err = OpenTable(parentTree, pId, table);
	if (err)
	{
		return(err);
	}

	pNewTree = table.mRef;
	pNewType = table.mRefType;
	pTableId = table.mId;

	// Get the name from the table
	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (!err && nameId)
	{
		GrapaDB2Cursor cursor;
		GrapaDB2FieldValueArray data;
		data.Append(this, parentDict, nameId, "", EQ_CMP);
		err = SearchDb(cursor, parentDict, data);
		if (!err)
		{
			// Get the name from the cursor
			GrapaBYTE nameValue;
			err = GetRecordField(cursor, nameId, nameValue);
			if (!err)
			{
				pName.FROM((char*)nameValue.mBytes);
			}
		}
	}

	return(0);
}

GrapaError GrapaGroup2::CreateEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2FieldValueArray data;

	pId = 0;

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		return(err);
	}
	if (nameId == 0)
	{
		return(-1);
	}

	data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (!err)
	{
		pId = cursor.mKey;
		return(0);
	}

	err = CreateRecord(parentDict, cursor);
	if (err)
	{
		return(err);
	}

	pId = cursor.mKey;

	{
		GrapaDB2FieldValueArray data2;
		data2.Append(this, parentDict, nameId, pDataName, EQ_CMP);
		err = SetRecordField(cursor, data2);
		if (err)
		{
			return(err);
		}
	}

	return(0);
}

GrapaError GrapaGroup2::FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2FieldValueArray data;

	pId = 0;

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		return(err);
	}
	if (nameId == 0)
	{
		return(-1);
	}

	data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (err)
	{
		return(err);
	}

	pId = cursor.mKey;

	return(0);
}

GrapaError GrapaGroup2::DeleteEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2FieldValueArray data;

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		return(err);
	}
	if (nameId == 0)
	{
		return(-1);
	}

	data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (err)
	{
		return(err);
	}

	err = DeleteRecord(parentDict, cursor);
	if (err)
	{
		return(err);
	}

	return(0);
}

GrapaError GrapaGroup2::DeleteEntry(u64 parentTree, u8 parentType, u64 pId)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	cursor.Set(parentDict.mRecRef, RREC_ITEM, pId);
	err = Search(cursor);
	if (err)
	{
		return(err);
	}

	err = DeleteRecord(parentDict, cursor);
	if (err)
	{
		return(err);
	}

	return(0);
}

GrapaError GrapaGroup2::CreateField(u64 parentTree, u8 parentType, const char* pFieldName, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{
	GrapaCHAR s(pFieldName);
	return CreateField(parentTree, parentType, s, pType, pStore, pSize, pGrow);
}

GrapaError GrapaGroup2::CreateField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2Field dbFieldName;
	GrapaDB2Index dbIndexName;
	u64 indexRef;
	u64 fieldId;

	if (pFieldName.mLength == 0 || pFieldName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		return(err);
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		return(err);
	}
	if (nameId == 0)
	{
		return(-1);
	}

	GrapaDB2FieldValueArray data;
	data.Append(this, parentDict, nameId, pFieldName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (!err)
	{
		return(-1);
	}

	err = CreateRecord(parentDict, cursor);
	if (err)
	{
		return(err);
	}

	fieldId = cursor.mKey;

	dbFieldName.Init(fieldId, pType, pStore, pSize, pGrow);
	err = CreateTableField(parentDict, dbFieldName, pFieldName);
	if (err)
	{
		return(err);
	}

	{
		GrapaDB2FieldValueArray data2;
		data2.Append(this, parentDict, nameId, pFieldName, EQ_CMP);
		err = SetRecordField(cursor, data2);
		if (err)
		{
			return(err);
		}
	}

	return(0);
}

GrapaError GrapaGroup2::DeleteField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2FieldValueArray data;

	if (pFieldName.mLength == 0 || pFieldName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		return(err);
	}
	if (nameId == 0)
	{
		return(-1);
	}

	data.Append(this, parentDict, nameId, pFieldName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (err)
	{
		return(err);
	}

	err = DeleteTableField(parentDict, cursor.mKey);
	if (err)
	{
		return(err);
	}

	return(0);
}







GrapaError GrapaGroup2::GetField(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, const GrapaCHAR& pFieldNameX, GrapaBYTE& pDataValue)
{
	printf("[DEBUG] GrapaGroup2::GetField called with parentTree=%llu, parentType=%d, dataName='%s', fieldName='%s'\n", 
		parentTree, parentType, pDataName.mBytes ? (char*)pDataName.mBytes : "NULL", 
		pFieldNameX.mBytes ? (char*)pFieldNameX.mBytes : "NULL");

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2Field dbFieldName, dbFieldValue;
	GrapaDB2Index dbIndexName;
	u64 indexRef;
	u64 dataId;
    GrapaCHAR fldName(pFieldNameX);
    
	pDataValue.SetLength(0);
	
	printf("[DEBUG] GetField: Starting validation\n");

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		return(-1);
	}
	if (fldName.mLength == 0 || fldName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	printf("[DEBUG] GetField: About to open table, parentType=%d\n", parentType);
	if (parentType == GROUP_TREE)
	{
		printf("[DEBUG] GetField: Calling OpenTable\n");
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			printf("[DEBUG] GetField: OpenTable failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GetField: OpenTable succeeded\n");
	}

	printf("[DEBUG] GetField: About to call GetDataTypeRecord\n");
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		printf("[DEBUG] GetField: GetDataTypeRecord failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] GetField: GetDataTypeRecord succeeded\n");
	
	/* For now, since GrapaDB2 is a placeholder implementation, 
	   we'll skip the complex BTree search and just use a simple approach */
	
	/* Initialize the dict field with default values for placeholder */
	parentDict.mDictField.Init(0, GrapaTokenType::START, 0, 0, 1);
	parentDict.mDictField.mTreeType = parentType;

	dataId = 0;

	printf("[DEBUG] GetField: About to call GetNameId\n");
	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	printf("[DEBUG] GetField: GetNameId returned err=%d, nameId=%llu\n", err, nameId);

	if (nameId)
	{
		/* For now, since SearchDb is a placeholder, we'll use a simpler approach */
		/* In a real implementation, this would search for the record by name */
		dataId = 1; /* Placeholder - assume record ID 1 for now */
	}
	else
	{
		dataId = 1; /* Placeholder - assume record ID 1 for now */
	}

	cursor.Set(parentDict.mRecRef, RREC_ITEM, dataId);
	/* For placeholder implementation, we'll skip the complex record search
	   and just proceed to find the field and get the data */
	err = 0; /* Assume success for now */

	printf("[DEBUG] GetField: About to determine fieldId\n");
	u64 fieldId = 0;
	if (fldName.StrCmp("$KEY") == 0)
	{ 
		printf("[DEBUG] GetField: Field is $KEY\n");
		if (nameId == 0)
		{
			return(-1);
		}
		fieldId = nameId;
		fldName.FROM("$KEY");
	}
	else
	{
		printf("[DEBUG] GetField: About to call FindField for field '%s'\n", fldName.mBytes ? (char*)fldName.mBytes : "NULL");
		GrapaDB2Field field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			printf("[DEBUG] GetField: FindField failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GetField: FindField succeeded, fieldId=%llu\n", field.mId);
		fieldId = field.mId;
	}

	printf("[DEBUG] GetField: About to call GetRecordField with fieldId=%llu\n", fieldId);
	err = GetRecordField(cursor, fieldId, pDataValue);
	if (err)
	{
		printf("[DEBUG] GetField: GetRecordField failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] GetField: GetRecordField succeeded, value length=%llu\n", pDataValue.mLength);

	return(0);
}

GrapaError GrapaGroup2::GetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pFieldNameX, GrapaBYTE& pDataValue)
{

	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	GrapaDB2Field dbFieldName, dbFieldValue;
	GrapaDB2Index dbIndexName;
	u64 indexRef;
    GrapaCHAR fldName(pFieldNameX);
    
	pDataValue.SetLength(0);

	if (fldName.mLength == 0 || fldName.mBytes == NULL)
	{
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		return(err);
	}

	cursor.Set(parentDict.mRecRef, RREC_ITEM, pId);
	err = Search(cursor);
	if (err)
	{
		return(err);
	}

	u64 fieldId = 0;
	if (fldName.StrCmp("$KEY") == 0)
	{ 
		u64 nameId = 0;
		err = GetNameId(parentTree, parentType, nameId);
		if (err)
		{
			return(err);
		}
		if (nameId == 0)
		{
			return(-1);
		}
		fieldId = nameId;
		fldName.FROM("$KEY");
	}
	else
	{
		GrapaDB2Field field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			return(err);
		}
		fieldId = field.mId;
	}

	err = GetRecordField(cursor, fieldId, pDataValue);
	if (err)
	{
		return(err);
	}

	return(0);
}

GrapaError GrapaGroup2::DumpGroup(u64 parentTree, u8 parentType, u64 pId, GrapaFile *pDumpFile)
{

	GrapaError err;
	GrapaDB2Table parentDict;
	GrapaDB2Cursor cursor;

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	if (pId == 0)
	{
		// Dump the entire group
		err = DumpTree(parentDict.mRef, pDumpFile);
	}
	else
	{
		// Dump specific record
		cursor.Set(parentDict.mRecRef, RREC_ITEM, pId);
		err = Search(cursor);
		if (!err)
		{
			// Dump the record
			err = DumpTree(cursor.mValue, pDumpFile);
		}
	}

	return(err);
}

GrapaError GrapaGroup2::SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, const GrapaCHAR& pFieldNameX, const GrapaBYTE& pDataValue)
{
	/* Simplified implementation for GrapaDB2 */
	/* This bypasses the complex GrapaGroup logic and directly stores data */
	
	printf("[DEBUG] GrapaGroup2::SetField called with parentTree=%llu, parentType=%d\n", parentTree, parentType);
	printf("[DEBUG] Data name: %s, Field name: %s\n", (char*)pDataName.mBytes, (char*)pFieldNameX.mBytes);
	
	GrapaError err;
	GrapaDB2Cursor cursor;
	GrapaDB2Table parentDict;
	
	/* Basic validation */
	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		printf("[DEBUG] Validation failed: pDataName is empty\n");
		return(-1);
	}
	if (pFieldNameX.mLength == 0 || pFieldNameX.mBytes == NULL)
	{
		printf("[DEBUG] Validation failed: pFieldNameX is empty\n");
		return(-1);
	}
	
	/* Set up the parent dictionary */
	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;
	
	if (parentType == GROUP_TREE)
	{
		printf("[DEBUG] Opening table for GROUP_TREE\n");
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			printf("[DEBUG] OpenTable failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] OpenTable succeeded\n");
	}
	
	/* Create a simple cursor pointing to the record */
	cursor.Set(parentDict.mRecRef, RREC_ITEM, 1); /* Use record ID 1 for simplicity */
	printf("[DEBUG] Created cursor with mTreeRef=%llu, mValue=%llu\n", cursor.mTreeRef, cursor.mValue);
	
	/* Create a field value array with the data */
	GrapaDB2FieldValueArray data;
	GrapaDB2FieldValue fieldValue;
	fieldValue.mValue = pDataValue;
	printf("[DEBUG] About to call data.Append\n");
	data.Append(this, parentDict, 1, fieldValue.mValue, 0); /* Use field ID 1 for simplicity */
	printf("[DEBUG] data.Append completed\n");
	
	/* Store the data using our simplified SetRecordField */
	printf("[DEBUG] About to call SetRecordField\n");
	err = SetRecordField(cursor, data);
	if (err)
	{
		printf("[DEBUG] SetRecordField failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] SetRecordField succeeded\n");
	
	return(0);
}

GrapaDB2FieldArray* GrapaGroup2::ListFields(u64 parentTree, u8 parentType)
{

	GrapaError err;
	GrapaDB2Table parentDict;
	GrapaDB2Cursor cursor;
	GrapaDB2FieldArray* pFieldList = new GrapaDB2FieldArray();

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			delete pFieldList;
			return(NULL);
		}
	}

	err = OpenTableFieldList(parentDict, *pFieldList);
	if (err)
	{
		delete pFieldList;
		return(NULL);
	}

	return(pFieldList);
}

GrapaError GrapaGroup2::FindField(u64 parentTree, u8 parentType, const GrapaCHAR& pFieldNameX, GrapaDB2Field& pField, u64& pMaxId)
{

	GrapaError err;
	GrapaDB2Table parentDict;
	GrapaDB2Cursor cursor;
	GrapaDB2FieldValueArray data;
    GrapaCHAR fldName(pFieldNameX);

	pMaxId = 0;

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			return(err);
		}
	}

	u64 fieldId = 0;
	err = GetNameId(parentTree, parentType, fieldId);
	if (err)
	{
		return(err);
	}
	if (fieldId == 0)
	{
		return(-1);
	}

	data.Append(this, parentDict, fieldId, fldName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (err)
	{
		return(err);
	}

	err = OpenTableField(parentDict, cursor.mKey, pField);
	if (err)
	{
		return(err);
	}

	pMaxId = cursor.mKey;

	return(0);
}

GrapaError GrapaGroup2::GetNameId(u64 parentTree, u8 parentType, u64& pNameId)
{
	/* Placeholder implementation - GrapaBtree doesn't have GetDataTypeRecord */
	pNameId = 1; /* Default name ID */
	return 0;
}

GrapaError GrapaGroup2::SetNameId(u64 parentTree, u8 parentType, u64 pNameId)
{
	/* Placeholder implementation - GrapaBtree doesn't have SetDataTypeRecord */
	return 0;
}

GrapaError GrapaGroup2::NextNameId(u64 parentTree, u8 parentType, u64& pNameId)
{
	/* Placeholder implementation - GrapaBtree doesn't have NextDataTypeRecord */
	pNameId++;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// GrapaGroup2Queue implementation
GrapaGroup2Event* GrapaGroup2Queue::OpenFile(const GrapaCHAR& fileName, GrapaFile* pFile, char mode)
{
	GrapaGroup2Event* e = Search(fileName);
	if (e) {
		e->mInstanceCount++;
		return(e);
	}
	e = new GrapaGroup2Event(fileName, pFile);
	if (e) {
		e->mValue.OpenFile(fileName, mode);
		PushTail((GrapaEvent*)e);
	}
	return(e);
}

GrapaGroup2Event* GrapaGroup2Queue::Create(const GrapaCHAR& fileName, GrapaFile* pFile, u8 pType)
{
	GrapaGroup2Event* e = Search(fileName);
	if (e) {
		e->mInstanceCount++;
		return(e);
	}
	e = new GrapaGroup2Event(fileName, pFile);
	if (e) {
		u64 firstTree;
		e->mValue.Create((char*)fileName.mBytes, pType, firstTree);
		PushTail((GrapaEvent*)e);
	}
	return(e);
}

GrapaGroup2Event* GrapaGroup2Queue::Create(const GrapaCHAR& fileName, GrapaFile* pFile, GrapaCHAR& pType)
{
	GrapaGroup2Event* e = Search(fileName);
	if (e) {
		e->mInstanceCount++;
		return(e);
	}
	e = new GrapaGroup2Event(fileName, pFile);
	if (e) {
		u64 firstTree;
		u8 type = GrapaDB2::GROUP_TREE;
		if (pType.StrCmp("ROW") == 0) type = GrapaDB2::RTABLE_TREE;
		else if (pType.StrCmp("COL") == 0) type = GrapaDB2::CTABLE_TREE;
		e->mValue.Create((char*)fileName.mBytes, type, firstTree);
		PushTail((GrapaEvent*)e);
	}
	return(e);
}

void GrapaGroup2Queue::CloseFile(GrapaGroup2Event* pEvent)
{
	if (pEvent) {
		pEvent->mInstanceCount--;
		if (pEvent->mInstanceCount == 0) {
			pEvent->mValue.CloseFile();
			PopEvent(pEvent);
			delete pEvent;
		}
	}
} 

////////////////////////////////////////////////////////////////////////////////
// GrapaDB2 Data Structure Method Implementations
////////////////////////////////////////////////////////////////////////////////

// GrapaDB2Field implementations
GrapaDB2Field::GrapaDB2Field()
{
	memset(this, 0, sizeof(GrapaDB2Field));
}

void GrapaDB2Field::Init(u64 pFieldId, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{
	mId = pFieldId;
	mType = pType;
	mStore = pStore;
	mSize = pSize;
	mGrow = pGrow;
	mRef = 0;
	mNameId = 0;
	mNameRef = 0;
	mDictOffset = 0;
	mDictSize = 0;
	mTableRef = 0;
	mTreeType = 0;
	memset(mReserved, 0, sizeof(mReserved));
	memset(mReserved2, 0, sizeof(mReserved2));
}

void GrapaDB2Field::BigEndian()
{
	mId = BE_S64(mId);
	mRef = BE_S64(mRef);
	mNameId = BE_S64(mNameId);
	mNameRef = BE_S64(mNameRef);
	mDictOffset = BE_S64(mDictOffset);
	mDictSize = BE_S64(mDictSize);
	mSize = BE_S64(mSize);
	mGrow = BE_S64(mGrow);
	mTableRef = BE_S64(mTableRef);
	mFormulaRef = BE_S64(mFormulaRef);
	// mFormulaType is u8, no endian conversion needed
}

void* GrapaDB2Field::GetPtr()
{
	return (void*)this;
}

u16 GrapaDB2Field::GetSize()
{
	return sizeof(GrapaDB2Field);
}

GrapaError GrapaDB2Field::Write(GrapaDB2 *pDb, u64 fieldRef)
{
	// Write field data to database using GrapaBtree operations
	if (!pDb || fieldRef == 0) return -1;
	
	BigEndian();  // Convert to big-endian for storage
	GrapaError err = pDb->SetDataValue(fieldRef, 0, sizeof(GrapaDB2Field), (const char*)this);
	BigEndian();  // Convert back to native endian
	return err;
}

GrapaError GrapaDB2Field::Read(GrapaDB2 *pDb, u64 fieldRef)
{
	// Read field data from database using GrapaBtree operations
	if (!pDb || fieldRef == 0) return -1;
	
	// Read the field structure from the database
	u64 returnSize = 0;
	GrapaError err = pDb->GetDataValue(fieldRef, 0, sizeof(GrapaDB2Field), (char*)this, &returnSize);
	BigEndian();  // Convert from big-endian to native endian
	return err;
}

GrapaError GrapaDB2Field::Get(GrapaDB2 *pDb, u64 tableRef, u64 fieldId)
{
	// Get field from table - simplified implementation
	if (!pDb || tableRef == 0) return -1;
	
	// For now, just set basic field info
	mId = fieldId;
	mRef = tableRef;
	return 0;
}

// GrapaDB2FieldArray implementations
GrapaDB2FieldArray::~GrapaDB2FieldArray()
{
	// Clean up any allocated resources
	// GrapaVoidArray handles most cleanup automatically
}

GrapaError GrapaDB2FieldArray::Append(GrapaDB2 *pDb, GrapaDB2Table& pTable, u64 pFieldId)
{
	// Append field to array - simplified implementation
	if (!pDb) return -1;
	
	// Create a new field and add it to the array
	GrapaDB2Field* newField = new GrapaDB2Field();
	if (!newField) return -1;
	
	newField->Init(pFieldId, GrapaTokenType::STR, GrapaDB2Field::STORE_VAR, 32, 8);
	newField->mRef = pTable.mRef;
	
	// Add to the array using base class method
	GrapaVoidArray::Append((void*)newField);
	return 0;
}

GrapaError GrapaDB2FieldArray::Append(GrapaDB2Field *pField)
{
	// Append field to array
	if (!pField) return -1;
	
	// Add to the array using base class method
	GrapaVoidArray::Append((void*)pField);
	return 0;
}

// GrapaDB2FieldValue implementations
void GrapaDB2FieldValue::BigEndian()
{
	GrapaDB2Field::BigEndian();
	mValue.mLength = BE_S64(mValue.mLength);
	mValue.mSize = BE_S64(mValue.mSize);
	mCmp = BE_S16(mCmp);
}

// GrapaDB2FieldValueArray implementations
GrapaDB2FieldValueArray::~GrapaDB2FieldValueArray()
{
	// Clean up any allocated resources
	// GrapaVoidArray handles most cleanup automatically
}

GrapaError GrapaDB2FieldValueArray::Append(GrapaDB2 *pDb, GrapaDB2Table& pTable, u64 pFieldId, const GrapaBYTE& pValue, s16 pCmp)
{
	// Append field value to array - simplified implementation
	if (!pDb) return -1;
	
	// Create a new field value and add it to the array
	GrapaDB2FieldValue* newFieldValue = new GrapaDB2FieldValue();
	if (!newFieldValue) return -1;
	
	// Initialize the field value
	newFieldValue->Init(pFieldId, GrapaTokenType::STR, GrapaDB2Field::STORE_VAR, 32, 8);
	newFieldValue->mValue = pValue;
	newFieldValue->mCmp = pCmp;
	newFieldValue->mRef = pTable.mRef;
	
	// Add to the array using base class method
	GrapaVoidArray::Append((void*)newFieldValue);
	return 0;
} 

GrapaError GrapaDB2::DumpTheGroupStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaCursor itemCursor;

	tableRef = cursor.mTreeRef;
	err = GetDataTypeRecord(tableRef,tableDT);
	if (err) 
	{
		/* If we can't get the data type record, just show basic info */
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "GREC_STRUCT [placeholder]");
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		return(0);
	}
	
	itemCursor.Set(tableRef,GREC_ITEM,cursor.mKey,cursor.mValue);
	return DumpTheStructure(dbWrite,itemCursor,tableDT);
}

GrapaError GrapaDB2::DumpTheStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor, u64 tableDT)
{
	GrapaError err;
	GrapaCursor dataTypeCursor;
	GrapaDB2Field dbField;
	GrapaBYTE dbChar;

	/* For now, use placeholder implementation since we don't have full BTree implementation */
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "STRUCT [placeholder]");
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDB2::PtrToRec(GrapaCursor& ptrCursor, GrapaCursor& recCursor)
{
	/* Convert pointer cursor to record cursor */
	/* For now, use placeholder implementation since we don't have full BTree implementation */
	recCursor.Set(ptrCursor.mTreeRef, ptrCursor.mValueType, ptrCursor.mKey, ptrCursor.mValue);
	return(0);
}

GrapaError GrapaDB2::DumpTheDataType(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	GrapaError err;
	GrapaBlockDataHeader dataHeader;
	u64 returnLen;
	char dataBlock[201];

	/* Read the data type record */
	err = GetDataValue(cursor.mValue, 0, sizeof(GrapaBlockDataHeader), (char*)&dataHeader, &returnLen);
	if (err) 
	{
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDTYPE [error reading header]", leader);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		return(err);
	}

	if (dataHeader.dataType == BYTE_DATA)
	{
		/* Read the actual data */
		err = GetDataValue(cursor.mValue, sizeof(GrapaBlockDataHeader), 200, dataBlock, &returnLen);
		if (err) 
		{
			dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDTYPE [error reading data]", leader);
			if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
			return(err);
		}
		dataBlock[returnLen] = 0;
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDTYPE [%s] [weight: %d]", leader, dataBlock, 0);
	}
	else if (dataHeader.dataType == FREC_DATA)
	{
		/* Recursively dump the tree */
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDTYPE [tree] [weight: %d]", leader, 0);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		return DumpTheTree(dbWrite, leader, 0, cursor.mValue);
	}
	else
	{
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDTYPE [unknown type %d] [weight: %d]", leader, dataHeader.dataType, 0);
	}

	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDB2::DumpTheGroupRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	/* Dump group record - recursively dump the tree */
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sGREC [weight: %d]", leader, 0);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return DumpTheTree(dbWrite, leader, 0, cursor.mValue);
}

GrapaError GrapaDB2::DumpTheRowStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaCursor itemCursor;

	tableRef = cursor.mTreeRef;
	err = GetDataTypeRecord(tableRef,tableDT);
	if (err) 
	{
		/* If we can't get the data type record, just show basic info */
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "RROW_STRUCT [placeholder]");
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		return(0);
	}
	
	itemCursor.Set(tableRef,RREC_ITEM,cursor.mKey,cursor.mValue);
	return DumpTheStructure(dbWrite,itemCursor,tableDT);
}

// GrapaDB2Table BigEndian implementation
void GrapaDB2Table::BigEndian()
{
	mDictField.BigEndian();
	mId = BE_S64(mId);
	mRef = BE_S64(mRef);
	mRecRef = BE_S64(mRecRef);
}

// GrapaDB2Index BigEndian implementation
void GrapaDB2Index::BigEndian()
{
	mTable.BigEndian();
	mId = BE_S64(mId);
	mRef = BE_S64(mRef);
}

// Formula field operations - new functionality for GrapaDB2
GrapaError GrapaDB2::CreateFormulaField(GrapaDB2Table& pTable, const GrapaCHAR& pFieldName, const GrapaCHAR& pFormulaText, u8 pResultType)
{
	GrapaError err;
	
	// 1. Allocate storage for the compressed formula
	u64 formulaRef = 0;
	err = AllocateFormulaStorage(formulaRef);
	if (err) return err;
	
	// 2. Store the compressed formula text
	err = StoreFormulaText(formulaRef, pFormulaText);
	if (err) return err;
	
	// 3. Create the field with formula reference
	GrapaDB2Field field;
	field.Init(GetNextFieldId(), pResultType, GrapaDB2Field::STORE_VAR, 32, 8);
	field.mFormulaRef = formulaRef;
	field.mFormulaType = GrapaDB2Field::FORMULA_TEXT;
	field.mTableRef = pTable.mRef;
	
	// 4. Store the field in the table
	err = CreateTableField(pTable, field, pFieldName);
	return err;
}

GrapaError GrapaDB2::GetFormulaText(u64 pFormulaRef, GrapaCHAR& pFormulaText)
{
	if (pFormulaRef == 0) return -1;
	
	// 1. Get the compressed formula data
	u64 returnSize = 0;
	GrapaError err = GetDataValue(pFormulaRef, 0, 0, NULL, &returnSize);
	if (err) return err;
	
	// 2. Allocate buffer for compressed data
	GrapaBYTE compressed;
	compressed.SetLength(returnSize, false);
	if (!compressed.mBytes) return -1;
	
	// 3. Read the compressed data
	err = GetDataValue(pFormulaRef, 0, returnSize, (char*)compressed.mBytes, &returnSize);
	if (err) return err;
	
	// 4. Decompress using GrapaCompress
	err = GrapaCompress::Expand(compressed);
	if (err) return err;
	
	// 5. Extract the formula text
	pFormulaText.FROM((char*)compressed.mBytes, compressed.mLength);
	return 0;
}

GrapaError GrapaDB2::StoreFormulaText(u64 pFormulaRef, const GrapaCHAR& pFormulaText)
{
	if (pFormulaRef == 0) return -1;
	
	// 1. Prepare the formula data for compression
	GrapaBYTE compressed;
	compressed.FROM(pFormulaText);
	compressed.mToken = GrapaTokenType::STR;  // Mark as string data
	
	// 2. Compress using GrapaCompress
	GrapaError err = GrapaCompress::Compress(compressed);
	if (err) return err;
	
	// 3. Store the compressed data
	err = SetDataValue(pFormulaRef, 0, compressed.mLength, (const char*)compressed.mBytes);
	return err;
}

GrapaError GrapaDB2::ExecuteFormula(u64 pFormulaRef, u8 pFormulaType, const GrapaCHAR& pParams, GrapaCHAR& pResult)
{
	if (pFormulaRef == 0) return -1;
	
	if (pFormulaType == GrapaDB2Field::FORMULA_TEXT) {
		// 1. Get the formula text
		GrapaCHAR formulaText;
		GrapaError err = GetFormulaText(pFormulaRef, formulaText);
		if (err) return err;
		
		// 2. For now, just return the formula text as a placeholder
		// TODO: Implement actual compilation and execution
		pResult.FROM("Formula execution not yet implemented: ");
		pResult.Append(formulaText);
		return 0;
	}
	
	return -1; // Unsupported formula type
}

// Helper method to allocate formula storage
GrapaError GrapaDB2::AllocateFormulaStorage(u64& pFormulaRef)
{
	// For now, use a simple allocation strategy
	// TODO: Implement proper BTree-based allocation
	static u64 nextFormulaRef = 1000000; // Start at a high number to avoid conflicts
	pFormulaRef = nextFormulaRef++;
	return 0;
}

// Helper method to get next field ID
u64 GrapaDB2::GetNextFieldId()
{
	/* BTree-based field ID allocation */
	// For now, use a simple counter approach
	// TODO: Implement proper BTree-based ID tracking
	static u64 nextFieldId = 1;
	return nextFieldId++;
}

