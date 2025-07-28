// GrapaDBX.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "GrapaDBX.h"
#include "GrapaMem.h"
#include "GrapaCompress.h"
#include "GrapaState.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////

// Constants copied from GrapaGroup.cpp
#define NAME_INDEX_ID (1)
#define NAME_INDEX_FIELD_NAME_ID 1

#define NAME_FIELD_TYPE GrapaTokenType::STR
#define NAME_FIELD_STORE GrapaDBXField::STORE_FIX
#define NAME_FIELD_SIZE 256
#define NAME_FIELD_GROW 0

#define VALUE_FIELD_TYPE GrapaTokenType::RAW
#define VALUE_FIELD_STORE GrapaDBXField::STORE_VAR
#define VALUE_FIELD_SIZE 32
#define VALUE_FIELD_GROW 8

// GrapaDBX implementation - inherits from GrapaBtree
GrapaDBX::GrapaDBX() : GrapaBtree()
{
	mDumpFile = NULL;
	mCachingMode = CACHE_ENABLED;  // Default to enabled caching
	mCacheSize = GrapaFileCache::DEFAULT_SIZE;  // Use default cache size
}

GrapaDBX::GrapaDBX(GrapaFile* pFile) : GrapaBtree()
{
	mDumpFile = NULL;
	mCachingMode = CACHE_ENABLED;  // Default to enabled caching
	mCacheSize = GrapaFileCache::DEFAULT_SIZE;  // Use default cache size
	INIT(pFile);
}

GrapaDBX::~GrapaDBX()
{
}

void GrapaDBX::INIT(GrapaFile* pFile)
{
	// Set up caching layer - same architecture as GrapaGroup
	mTree.SetFile(pFile);
	SetFile(&mTree);  // Use mTree as the file for GrapaBtree operations
}

// Core database operations - delegate to GrapaBtree for now (placeholder implementation)
GrapaError GrapaDBX::Create(const char *pFileName, u8 treeType, u64& firstTree)
{
	printf("[DEBUG] GrapaDBX::Create called with fileName='%s', treeType=%d\n", pFileName, treeType);
	
	/* Create the file using GrapaBtree */
	GrapaError err = GrapaBtree::Create(pFileName);
	if (err) {
		printf("[DEBUG] GrapaDBX::Create: GrapaBtree::Create failed with error %d\n", err);
		return err;
	}
	
	/* Create the root tree structure */
	err = NewTree(firstTree, treeType, 0, 8); /* 8 is default nodeCount */
	if (err) {
		printf("[DEBUG] GrapaDBX::Create: NewTree failed with error %d\n", err);
		return err;
	}
	
	printf("[DEBUG] GrapaDBX::Create result: %d, firstTree=%llu\n", err, firstTree);
	return err;
}

GrapaError GrapaDBX::CreateRoot(u8 treeType, u64& firstTree)
{
	/* Placeholder implementation - GrapaBtree doesn't have CreateRoot */
	firstTree = 1; /* Default first tree */
	return 0;
}

GrapaError GrapaDBX::OpenFile(const char *fileName, char mode)
{
	printf("[DEBUG] GrapaDBX::OpenFile called with fileName='%s', mode='%c'\n", fileName, mode);
	GrapaError err = GrapaBtree::OpenFile(fileName, mode);
	printf("[DEBUG] GrapaDBX::OpenFile result: %d, FileOpened()=%s\n", err, FileOpened() ? "YES" : "NO");
	return err;
}

u64 GrapaDBX::RootTree(u8& pRootType)
{
	/* Delegate to GrapaBtree for now */
	return GrapaBtree::RootTree(pRootType);
}

u64 GrapaDBX::RootTree(GrapaCHAR& pRootType)
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

GrapaError GrapaDBX::CloseFile()
{
	return GrapaBtree::CloseFile();
}

// Table operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDBX::LastTableId(u64 firstTree, u64& pTableId)
{
	/* Placeholder implementation - GrapaBtree doesn't have LastTableId */
	pTableId = 0;
	return 0;
}

GrapaError GrapaDBX::CreateTable(u64 firstTree, u8 pTreeType, u64 pTableId, GrapaDBXTable& pTable)
{
	GrapaError err;
	GrapaCursor tableNames,tableNamesDT;
	GrapaDBXField dbField;
	GrapaDBXIndex dbIndex;
	GrapaDBXTable parentDict;

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

GrapaError GrapaDBX::OpenTable(u64 firstTree, u64 pTableId, GrapaDBXTable& pTable)
{
	GrapaError err;
	GrapaCursor tableCursor,indexTableCursor,indexCursor;
	GrapaDBXTable dictTable;
	GrapaDBXFieldValueArray data;
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

GrapaError GrapaDBX::DeleteTable(u64 firstTree, u64 pTableId)
{
	/* Placeholder implementation - GrapaBtree doesn't have DeleteTable */
	return 0;
}

GrapaError GrapaDBX::CreateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBXTable& pTable)
{
	/* Placeholder implementation - GrapaBtree doesn't have CreateAlias */
	pTable.mRef = pTableId;
	pTable.mRefType = GROUP_TREE;
	pTable.mId = pTableId;
	return 0;
}

GrapaError GrapaDBX::UpdateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBXTable& pTable)
{
	/* Placeholder implementation - GrapaBtree doesn't have UpdateAlias */
	return 0;
}

// Field operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDBX::CreateTableField(GrapaDBXTable& pTable, GrapaDBXField& pField, const GrapaCHAR& pName)
{
	/* Simplified implementation for GrapaDBX - just store the field info */
	// For now, just return success without doing the complex GrapaDB field management
	// This will need to be enhanced to actually store field information
	return 0;
}

GrapaError GrapaDBX::OpenTableField(GrapaDBXTable& pTable, u64 pFieldId, GrapaDBXField& pField)
{
	/* Placeholder implementation - GrapaBtree doesn't have OpenTableField */
	pField.mId = pFieldId;
	return 0;
}

GrapaError GrapaDBX::OpenTableFieldList(GrapaDBXTable& pTable, GrapaDBXFieldArray& pFieldList)
{
	/* Placeholder implementation - GrapaBtree doesn't have OpenTableFieldList */
	return 0;
}

GrapaError GrapaDBX::DeleteTableField(GrapaDBXTable& pTable, u64 pFieldId)
{
	/* Placeholder implementation - GrapaBtree doesn't have DeleteTableField */
	return 0;
}

GrapaError GrapaDBX::FlushTableFields(GrapaDBXTable& pTable)
{
	/* Placeholder implementation - GrapaBtree doesn't have FlushTableFields */
	return 0;
}

// Record operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDBX::FindFreeRecordId(GrapaDBXTable& pTable, u64& pRecordId)
{
	/* Placeholder implementation - GrapaBtree doesn't have FindFreeRecordId */
	pRecordId = 1;
	return 0;
}

GrapaError GrapaDBX::CreateRecord(GrapaDBXTable& pTable, GrapaCursor& pCursor)
{
	GrapaError err;
	u64 newTree;
	u64 uniqueKey;

	// Generate a unique key for this record
	err = FirstFreeId(pTable.mRecRef, 1, uniqueKey);
	if (err) return(err);

	err = NewTree(newTree,RREC_ITEM,pTable.mRecRef);
	if (err) return(err);

	pCursor.mValue = newTree;
	pCursor.mKey = uniqueKey;  // Use the generated unique key

	err = Insert(pCursor);
	if (err)
	{
		DeleteTree(newTree);
		return(err);
	}

	return(0);
}

GrapaError GrapaDBX::DeleteRecord(GrapaDBXTable& pTable, GrapaCursor& pCursor)
{
	/* Placeholder implementation - GrapaBtree doesn't have DeleteRecord */
	return 0;
}

// Field value operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDBX::SetRecordField(GrapaCursor& pCursor, GrapaDBXFieldValueArray& pFieldList)
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
		GrapaDBXFieldValue* dbFieldValue = pFieldList.GetFieldAt(i);
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
		GrapaDBXFieldValue* dbFieldValue = pFieldList.GetFieldAt(i);
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
	
	/* Now insert the record into the B-tree index */
	printf("[DEBUG] Inserting record into B-tree index\n");
	GrapaBlockTree head;
	GrapaBlockNodeLeaf key;
	s8 result;
	
	/* Read the tree header */
	err = head.Read(mFile, pCursor.mTreeRef);
	if (err) {
		printf("[DEBUG] Failed to read tree header: %d\n", err);
		return err;
	}
	
	/* Set up the key for insertion */
	key.valueType = pCursor.mValueType;
	key.key = pCursor.mKey;
	key.value = pCursor.mValue;
	key.flags = pCursor.mFlags;
	key.child = 0; // No child for data records
	
	/* Check if tree is empty */
	if (head.firstItem == 0) {
		printf("[DEBUG] Tree is empty, using AppendNode\n");
		err = AppendNode(pCursor.mTreeRef, head, key);
		if (err) {
			printf("[DEBUG] AppendNode failed with error %d\n", err);
			return err;
		}
		/* Write the updated tree header back to disk */
		printf("[DEBUG] Writing updated tree header, firstItem=%llu\n", head.firstItem);
		err = head.Write(mFile, pCursor.mTreeRef);
		if (err) {
			printf("[DEBUG] Failed to write updated tree header: %d\n", err);
			return err;
		}
		printf("[DEBUG] Tree header successfully written to disk\n");
	} else {
		printf("[DEBUG] Tree has items, using InsertRc\n");
		/* Insert the record into the B-tree */
		err = InsertRc(pCursor.mTreeRef, head, head.firstItem, pCursor, key, key, result);
		if (err) {
			printf("[DEBUG] InsertRc failed with error %d\n", err);
			return err;
		}
	}
	
	printf("[DEBUG] Record successfully inserted into B-tree index\n");
	return 0;
}

GrapaError GrapaDBX::GetRecordField(GrapaCursor& pCursor, u64 pFieldId, GrapaBYTE& pValue)
{
	printf("[DEBUG] GetRecordField: called with fieldId=%llu, cursor.mValue=%llu\n", pFieldId, pCursor.mValue);
	
	/* Get the actual field value from the record */
	GrapaError err;
	GrapaDBXField field;
	GrapaDBXCursor recCursor;
	
	/* Find the record field */
	err = FindRecordField(pCursor, pFieldId, recCursor, field);
	if (err) {
		printf("[DEBUG] GetRecordField: FindRecordField failed with error %d\n", err);
		return err;
	}
	
	/* Get the field value using the field information */
	err = GetRecordField(recCursor, field, pValue);
	if (err) {
		printf("[DEBUG] GetRecordField: GetRecordField(field) failed with error %d\n", err);
		return err;
	}
	
	printf("[DEBUG] GetRecordField: Retrieved actual value, length=%llu\n", pValue.mLength);
	if (pValue.mLength > 0) {
		printf("[DEBUG] GetRecordField: Value content: '%.*s'\n", (int)pValue.mLength, pValue.mBytes);
	}
	
	/* Add debugging for field information */
	printf("[DEBUG] GetRecordField: Field info - mId=%llu, mType=%d, mStore=%d, mSize=%llu, mDictOffset=%llu, mDictSize=%llu\n", 
	       field.mId, field.mType, field.mStore, field.mSize, field.mDictOffset, field.mDictSize);
	
	return 0;
}



GrapaError GrapaDBX::GetDataTypeRecord(u64 tableRef, u64& tableDT)
{
	/* For now, use a simple placeholder implementation */
	/* In a real implementation, this would retrieve the dictionary tree reference */
	printf("[DEBUG] GetDataTypeRecord: called with tableRef=%llu\n", tableRef);
	
	/* Placeholder: return tableRef as tableDT for now */
	tableDT = tableRef;
	
	printf("[DEBUG] GetDataTypeRecord: returning tableDT=%llu\n", tableDT);
	return(0);
}

GrapaError GrapaDBX::FindRecordField(GrapaCursor& cursor, u64 fieldId, GrapaCursor& recCursor, GrapaDBXField& field)
{
	GrapaError err;
	u64 tableRef;
	
	printf("[DEBUG] FindRecordField: fieldId=%llu, cursor.mValue=%llu, cursor.mTreeRef=%llu\n", 
	       fieldId, cursor.mValue, cursor.mTreeRef);
	
	if (mDumpFile) {
		GrapaCHAR debugMsg;
		debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
			"[DEBUG] FindRecordField: fieldId=%llu, cursor.mValue=%llu\n", fieldId, cursor.mValue);
		mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
	}
	
	/* Convert pointer to record if needed */
	switch(cursor.mValueType)
	{
		case GREC_ITEM:
		case RREC_ITEM:
		case CREC_ITEM:
			/* Cursor is already a record, just copy it */
			recCursor = cursor;
			err = 0;
			break;
		case GPTR_ITEM:
		case RPTR_ITEM:
		case CPTR_ITEM:
			/* Cursor is a pointer, need to dereference */
			err = PtrToRec(cursor, recCursor);
			break;
		default:
			printf("[DEBUG] FindRecordField: unknown cursor type %d\n", cursor.mValueType);
			return(-1);
	}
	
	if (err) {
		printf("[DEBUG] FindRecordField: PtrToRec failed with error %d\n", err);
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] FindRecordField: PtrToRec failed with error %d\n", err);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(err);
	}
	
	printf("[DEBUG] FindRecordField: PtrToRec succeeded, recCursor.mValue=%llu, recCursor.mTreeRef=%llu\n", 
	       recCursor.mValue, recCursor.mTreeRef);
	
	/* Read the tree structure to get tree type */
	GrapaBlockTree tree;
	err = tree.Read(mFile, recCursor.mTreeRef);
	if (err) {
		printf("[DEBUG] FindRecordField: tree.Read failed with error %d\n", err);
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] FindRecordField: tree.Read failed with error %d\n", err);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(err);
	}
	
	recCursor.mTreeType = tree.treeType;
	printf("[DEBUG] FindRecordField: tree type = %d\n", recCursor.mTreeType);
	
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
		printf("[DEBUG] FindRecordField: unknown tree type %d\n", recCursor.mTreeType);
		if (mDumpFile) {
			GrapaCHAR debugMsg;
			debugMsg.mLength = snprintf((char*)debugMsg.mBytes, debugMsg.mSize, 
				"[DEBUG] FindRecordField: unknown tree type %d\n", recCursor.mTreeType);
			mDumpFile->Append(debugMsg.mLength, debugMsg.mBytes);
		}
		return(-1);
	}
	
	if (err) {
		printf("[DEBUG] FindRecordField: field.Get failed with error %d\n", err);
	} else {
		printf("[DEBUG] FindRecordField: success, treeType=%d, tableRef=%llu, fieldId=%llu\n", 
		       recCursor.mTreeType, tableRef, fieldId);
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

GrapaError GrapaDBX::GetRecordField(GrapaCursor& pCursor, GrapaDBXField& field, GrapaBYTE& pValue)
{
	/* Enhanced implementation to handle multi-field data structure with size prefixes */
	GrapaError err;
	u64 returnSize = 0;
	
	printf("[DEBUG] GetRecordField(field): fieldId=%llu, cursor.mValue=%llu\n", field.mId, pCursor.mValue);
	
	pValue.SetSize(0);
	
	/* Convert pointer to record if needed */
	GrapaDBXCursor recCursor;
	switch(pCursor.mValueType)
	{
		case GREC_ITEM:
		case RREC_ITEM:
		case CREC_ITEM:
			/* Cursor is already a record, just copy it */
			recCursor.mValue = pCursor.mValue;
			recCursor.mValueType = pCursor.mValueType;
			recCursor.mTreeRef = pCursor.mTreeRef;
			recCursor.mKey = pCursor.mKey;
			recCursor.mNodeRef = pCursor.mNodeRef;
			recCursor.mNodeIndex = pCursor.mNodeIndex;
			recCursor.mLength = pCursor.mLength;
			err = 0;
			break;
		case GPTR_ITEM:
		case RPTR_ITEM:
		case CPTR_ITEM:
			/* Cursor is a pointer, need to dereference */
			err = PtrToRec(pCursor, recCursor);
			break;
		default:
			printf("[DEBUG] GetRecordField(field): unknown cursor type %d\n", pCursor.mValueType);
			return(-1);
	}
	
	if (err) {
		printf("[DEBUG] GetRecordField(field): PtrToRec failed with error %d\n", err);
		return(err);
	}
	
	printf("[DEBUG] GetRecordField(field): Cursor conversion succeeded, recCursor.mValue=%llu\n", recCursor.mValue);
	
	/* Get the total data size first */
	u64 totalDataSize = 0;
	u8 compressType = 0;
	err = GetDataSize(recCursor.mValue, 0, totalDataSize, totalDataSize, compressType);
	if (err) {
		printf("[DEBUG] GetRecordField(field): GetDataSize failed with error %d\n", err);
		return(err);
	}
	
	printf("[DEBUG] GetRecordField(field): totalDataSize=%llu\n", totalDataSize);
	
	/* Read the entire data block to parse the multi-field structure */
	GrapaBYTE fullData;
	fullData.SetSize(totalDataSize);
	err = GetDataValue(recCursor.mValue, 0, totalDataSize, (char*)fullData.GetPtr(), &returnSize);
	if (err) {
		printf("[DEBUG] GetRecordField(field): GetDataValue failed with error %d\n", err);
		return(err);
	}
	
	printf("[DEBUG] GetRecordField(field): Retrieved full data, length=%llu\n", returnSize);
	
	/* Parse the multi-field structure to find the specific field */
	u64 currentOffset = 0;
	u64 fieldIndex = 0;
	
	while (currentOffset < returnSize) {
		/* Read the size prefix for this field */
		if (currentOffset + sizeof(u64) > returnSize) {
			printf("[DEBUG] GetRecordField(field): Invalid data structure - size prefix incomplete\n");
			return -1;
		}
		
		u64 fieldSize = *(u64*)((char*)fullData.GetPtr() + currentOffset);
		printf("[DEBUG] GetRecordField(field): Field %llu at offset %llu, size=%llu\n", fieldIndex, currentOffset, fieldSize);
		
		currentOffset += sizeof(u64);
		
		/* Check if this is the field we're looking for */
		if (fieldIndex == field.mId - 1) { /* field.mId is 1-based, convert to 0-based */
			printf("[DEBUG] GetRecordField(field): Found target field %llu, size=%llu\n", field.mId, fieldSize);
			
			/* Extract the field data */
			if (fieldSize > 0) {
				if (currentOffset + fieldSize > returnSize) {
					printf("[DEBUG] GetRecordField(field): Invalid data structure - field data incomplete\n");
					return -1;
				}
				
				pValue.SetSize(fieldSize);
				memcpy(pValue.GetPtr(), (char*)fullData.GetPtr() + currentOffset, fieldSize);
				pValue.SetLength(fieldSize, false);
				
				printf("[DEBUG] GetRecordField(field): Retrieved field data, length=%llu\n", fieldSize);
				if (fieldSize > 0) {
					printf("[DEBUG] GetRecordField(field): Field content: '%.*s'\n", (int)fieldSize, (char*)pValue.GetPtr());
				}
				
				return 0;
			} else {
				printf("[DEBUG] GetRecordField(field): Field is empty\n");
				pValue.SetSize(0);
				return 0;
			}
		}
		
		/* Skip to next field */
		currentOffset += fieldSize;
		fieldIndex++;
	}
	
	printf("[DEBUG] GetRecordField(field): Field %llu not found in data structure\n", field.mId);
	return -1;
}

// Placeholder implementations for virtual methods
GrapaError GrapaDBX::CompareKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult)
{
	GrapaError err = 0;
	GrapaCursor cursor;

	pResult = -1;

	switch(pTreeCursor.mValueType)
	{
		case DTYPE_ITEM:
			// this item is the same as a DbData item, but for the data type dictionary
			pResult = GrapaBtree::CompareKey(pTreeCursor.mKey, pUserCursor.mKey);
			break;

		case GREC_ITEM: 
		case RREC_ITEM:
		case CREC_ITEM:
		case GPTR_ITEM:
		case RPTR_ITEM:
		case CPTR_ITEM:
			switch(pUserCursor.mValueType)
			{
				case SEARCH_ITEM: // Searching an index using a set of search fields
					err = CompareSearchKey(pCompareType, pUserCursor, pTreeCursor, pResult);
					break;

				case GREC_ITEM: 
				case RREC_ITEM:
				case CREC_ITEM:
				case GPTR_ITEM:
				case RPTR_ITEM:
				case CPTR_ITEM:
					err = CompareRecordKey(pCompareType, pUserCursor, pTreeCursor, pResult);
					break;

				case DTYPE_ITEM:
				default:
					return(-1);
			}
			break;

		case SEARCH_ITEM:
		default:
			return(-1);
	}

	if (err)
		pResult = GrapaBtree::CompareKey(pTreeCursor.mKey, pUserCursor.mKey);

	if (pResult == 0)
	{
		// if the values end up to be the same, then lets compare the key's just to be sure.
		switch(pUserCursor.mValueType)
		{
			case GREC_ITEM:
			case RREC_ITEM:
			case CREC_ITEM:
			case GPTR_ITEM:
			case RPTR_ITEM:
			case CPTR_ITEM:
				switch (pCompareType)
				{
					case SEARCH_MODE:
						if (pUserCursor.mKey)
							pResult = GrapaBtree::CompareKey(pTreeCursor.mKey, pUserCursor.mKey);
						break;
					case INSERT_MODE:
					case DELETE_MODE:
						pResult = GrapaBtree::CompareKey(pTreeCursor.mKey, pUserCursor.mKey);
						break;
				}
				break;
		}
	}
	
	return(0);
}

GrapaError GrapaDBX::CompareRecordKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult)
{
	GrapaError err;
	GrapaCursor cursor;
	u64 indexRef;
	GrapaCHAR name1, name2;

	pResult = -1;

	switch(pTreeCursor.mValueType)
	{
		case GREC_ITEM: 
		case RREC_ITEM:
		case CREC_ITEM:
		case GPTR_ITEM: 
		case RPTR_ITEM:
		case CPTR_ITEM:
			switch(pUserCursor.mValueType)
			{
				case GREC_ITEM: 
				case RREC_ITEM:
				case CREC_ITEM:
					if (pUserCursor.mKey == pTreeCursor.mKey) 
					{
						pResult = 0;
						return(0);
					}
					break;
				case GPTR_ITEM: 
				case RPTR_ITEM:
				case CPTR_ITEM:
					break;
				case SEARCH_ITEM:
					return CompareSearchKey(pCompareType, pUserCursor, pTreeCursor, pResult);
				default:
					return(-1);
			}
			break;
		default:
			return(-1);
	}

	cursor.Set(pTreeCursor.mTreeRef);
	err = GetTreeIndex(cursor, indexRef);
	if (err) return(err);

	GrapaCursor treeItemCursor, userItemCursor;
	err = PtrToRec(pTreeCursor, treeItemCursor);
	if (err) return(err);
	err = PtrToRec(pUserCursor, userItemCursor);
	if (err) return(err);

	switch(pTreeCursor.mValueType)
	{
		case GREC_ITEM: 
		case RREC_ITEM:
		case GPTR_ITEM: 
		case RPTR_ITEM:
			if (userItemCursor.mValue == treeItemCursor.mValue) 
			{
				pResult = 0;
				return(0);
			}
			break;

		case CREC_ITEM:
		case CPTR_ITEM:
			if (userItemCursor.mLength == treeItemCursor.mLength) 
			{
				pResult = 0;
				return(0);
			}
			break;
	}

	// Implement proper field-by-field comparison like GrapaDB
	cursor.Set(indexRef);
	err = First(cursor);
	while(!err)
	{
		// need to pull these into the treeItemCursor datatype for the comparison
		err = GetRecordField(userItemCursor, cursor.mValue, name1);
		err = GetRecordField(treeItemCursor, cursor.mValue, name2);

		char* n1 = (char*)name1.mBytes;
		char* n2 = (char*)name2.mBytes;

		// the following conditions should never happen...unless the value really is NULL
		if (name1.mBytes == NULL || name1.mLength == 0) n1 = (char*)"";
		if (name2.mBytes == NULL || name2.mLength == 0) n2 = (char*)"";
		
		// need to compare based on the treeItemCursor datatype
		int cr = strcmp(n2, n1);
		if (cr)
		{
			pResult = (cr > 0) ? 1 : ((cr < 0) ? -1 : 0);
			break;
		}

		err = Next(cursor);
	}

	return(0);
}

GrapaError GrapaDBX::CompareSearchKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult)
{
	GrapaError err;
	GrapaDBXFieldValueArray *fvList;
	GrapaDBXFieldValue *fv;
	u32 i, count;
	GrapaCHAR name2;

	pResult = -1;

	if (pUserCursor.mValueType != SEARCH_ITEM) return(-1);

	// user specified this
	// parse based on all of the values in GrapaDBXCursor
	// cycle through all the fields included in the user pointer
	// use the order specified in the index

	fvList = ((GrapaDBXCursor&)pUserCursor).mData;
	if (!fvList) return(-1);
	
	count = fvList->Count();
	pResult = 0; // if no fields, then the compare indicates an equal
	for(i = 0; i < count; i++)
	{
		fv = fvList->GetFieldAt(i);
		if (!fv) continue;

		GrapaCursor treeItemCursor;
		err = PtrToRec(pTreeCursor, treeItemCursor);
		if (err) continue;

		// need to pull this into the treeItemCursor datatype for the comparison
		printf("[DEBUG] CompareSearchKey: About to call GetRecordField with fieldId=%llu\n", fv->mId);
		err = GetRecordField(treeItemCursor, *fv, name2);
		if (err) {
			printf("[DEBUG] CompareSearchKey: GetRecordField failed with error %d\n", err);
			continue;
		}
		printf("[DEBUG] CompareSearchKey: GetRecordField succeeded, name2 length=%llu\n", name2.mLength);

		char* n1 = (char*)fv->mValue.mBytes;
		char* n2 = (char*)name2.mBytes;

		// the following conditions should never happen...unless the value really is NULL
		if (fv->mValue.mBytes == NULL || fv->mValue.mLength == 0) n1 = (char*)"";
		if (name2.mBytes == NULL || name2.mLength == 0) n2 = (char*)"";

		// need to compare based on the treeItemCursor datatype
		int cr = strcmp(n2, n1);
		pResult = (cr > 0) ? 1 : ((cr < 0) ? -1 : 0);
		if (pResult < 0)
		{
			switch(fv->mCmp)
			{
			case LT_CMP:
			case LTEQ_CMP:
				pResult = 0;
				break;
			case EQ_CMP:
			case GT_CMP:
			case GTEQ_CMP:
				pResult = -1;
				break;
			}
		}
		else if (pResult > 0)
		{
			switch(fv->mCmp)
			{
			case LT_CMP:
			case LTEQ_CMP:
			case EQ_CMP:
				pResult = 1;
				break;
			case GT_CMP:
			case GTEQ_CMP:
				pResult = 0;
				break;
			}
		}
		else
		{
			switch(fv->mCmp)
			{
			case LT_CMP:
				pResult = 1;
				break;
			case LTEQ_CMP:
			case EQ_CMP:
			case GTEQ_CMP:
				pResult = 0;
				break;
			case GT_CMP:
				pResult = -1;
				break;
			}
		}
		if (pResult) break;
	}
	return(0);
}

GrapaError GrapaDBX::GetDataValue(u64 itemPtr, u64 offset, u64 length, char* data, u64* returnSize)
{
	/* Delegate to parent GrapaBtree implementation */
	return GrapaBtree::GetDataValue(itemPtr, offset, length, data, returnSize);
}

GrapaError GrapaDBX::SetDataValue(u64 itemPtr, u64 offset, u64 length, const char* data)
{
	/* Delegate to parent GrapaBtree implementation */
	return GrapaBtree::SetDataValue(itemPtr, offset, length, (void*)data);
}

GrapaError GrapaDBX::GetDataSize(u64 itemPtr, u64 growBlockSize, u64& dataSize, u64& dataLength, u8& compressType)
{
	/* Delegate to parent GrapaBtree implementation */
	return GrapaBtree::GetDataSize(itemPtr, growBlockSize, dataSize, dataLength, compressType);
}

GrapaError GrapaDBX::GetData(u64 itemPtr, GrapaCHAR& pValue)
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

GrapaError GrapaDBX::CreateIndex(GrapaDBXTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBXIndex& pIndex)
{
	/* Basic BTree-based index creation */
	// TODO: Implement full index creation with proper BTree operations
	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mTable = pTable;
	return 0;
}

GrapaError GrapaDBX::OpenIndex(GrapaDBXTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBXIndex& pIndex)
{
	/* Basic index opening */
	// TODO: Implement full index opening with BTree operations
	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mTable = pTable;
	return 0;
}

GrapaError GrapaDBX::DeleteIndex(GrapaDBXTable& pTable, u64 pIndexId)
{
	/* Basic index deletion */
	// TODO: Implement full index deletion with BTree operations
	return 0;
}

GrapaError GrapaDBX::RefreshIndex(GrapaDBXIndex& pIndex)
{
	/* Basic index refresh */
	// TODO: Implement full index refresh with BTree operations
	return 0;
}

GrapaError GrapaDBX::FindFreeIndexId(GrapaDBXIndex& pIndex, u64 pMinId, u64& pIndexId)
{
	/* Simple index ID allocation */
	// TODO: Implement proper BTree-based ID tracking
	pIndexId = pMinId;
	return 0;
}

GrapaError GrapaDBX::SearchDb(GrapaCursor& pCursor, GrapaDBXTable& pTable, GrapaDBXFieldValueArray& pFieldList)
{
	GrapaError err;
	u64 indexRef = 0;
	GrapaCursor indexCursor;
	bool usingIndex = false;
	GrapaDBXField* field;
	u64 bestIndexField = 0;
	u64 bestIndexRef = 0;

	printf("[DEBUG] SearchDb called with fieldCount=%d\n", pFieldList.Count());

	// Step 1: Find the best available index for the search fields
	if (pFieldList.Count() > 0)
	{
		// Try to find an index that contains any of the search fields
		indexCursor.Set(pTable.mRecRef);
		err = GetTreeIndex(indexCursor, indexRef);
		if (!err && indexRef != 0)
		{
			// Check each search field to see if it's in an index
			for (int i = 0; i < pFieldList.Count(); i++)
			{
				field = pFieldList.GetFieldAt(i);
				if (field)
				{
					err = LocateIndex(indexCursor, indexRef, field->mId);
					if (!err)
					{
						// Found an index for this field
						usingIndex = true;
						bestIndexField = field->mId;
						bestIndexRef = indexRef;
						printf("[DEBUG] Found index for field %llu, using index-based search\n", field->mId);
						break; // Use the first matching index
					}
				}
			}
		}
	}

	// Step 2: If no specific index found, default to $KEY index (field 4 for ROW/COL, field 1 for GROUP)
	if (!usingIndex)
	{
		// Determine the $KEY field based on table type
		u64 keyField = 4; // Default for ROW/COL
		if (pTable.mRefType == GROUP_TREE)
		{
			keyField = 1; // GROUP uses field 1 as $KEY
		}

		indexCursor.Set(pTable.mRecRef);
		err = GetTreeIndex(indexCursor, indexRef);
		if (!err && indexRef != 0)
		{
			err = LocateIndex(indexCursor, indexRef, keyField);
			if (!err)
			{
				usingIndex = true;
				bestIndexField = keyField;
				bestIndexRef = indexRef;
				printf("[DEBUG] Using default $KEY index (field %llu) for search\n", keyField);
			}
		}
	}

	// Step 3: Set up the search cursor
	GrapaDBXCursor* dbxCursor = dynamic_cast<GrapaDBXCursor*>(&pCursor);
	if (dbxCursor) {
		// Set up search cursor with index information
		dbxCursor->SetSearch(this, pTable.mRecRef, usingIndex, &pFieldList);
	} else {
		// Fallback: just set the cursor to the table reference
		pCursor.Set(pTable.mRecRef);
	}

	// Step 4: Perform the search based on available index
	if (usingIndex)
	{
		printf("[DEBUG] Using index-based search with field %llu\n", bestIndexField);
		
		// Use index-based search
		err = Search(pCursor);
		if (err) 
		{
			printf("[DEBUG] Index-based search failed, falling back to table scan\n");
			usingIndex = false;
		}
		else
		{
			// Index search succeeded, position cursor at first matching record
			err = FirstDb(pCursor);
			if (err) 
			{
				printf("[DEBUG] Failed to position cursor after index search\n");
				return err;
			}
			return 0; // Success - cursor is positioned at first matching record
		}
	}

	// Step 5: Fallback to table scan if no index available or index search failed
	if (!usingIndex)
	{
		printf("[DEBUG] Using table scan search\n");
		
		// Configure the cursor with search criteria if available
		GrapaDBXCursor* dbxCursor = dynamic_cast<GrapaDBXCursor*>(&pCursor);
		if (dbxCursor && pFieldList.Count() > 0) {
			// Set up the cursor with search data for table scan
			dbxCursor->SetSearch(this, pTable.mRecRef, false, &pFieldList);
		} else {
			// Fallback: just set the cursor to the table reference
			pCursor.Set(pTable.mRecRef);
		}
		
		// Start from the first record
		err = First(pCursor);
		if (err) {
			printf("[DEBUG] First() failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] First() succeeded, cursor positioned at first record\n");
		
		err = FirstDb(pCursor);
		if (err) {
			printf("[DEBUG] FirstDb() failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] FirstDb() succeeded\n");

		// If we have search criteria, find the first matching record
		if (pFieldList.Count() > 0)
		{
			// Create a proper GrapaDBXCursor for search operations
			GrapaDBXCursor cursorCompare;
			s8 result;
			
			// Set up the search cursor with the field list
			cursorCompare.SetSearch(this, pCursor.mTreeRef, false, &pFieldList);
			
			printf("[DEBUG] Starting table scan with %d search fields\n", pFieldList.Count());
			printf("[DEBUG] pFieldList pointer: %p\n", &pFieldList);
			printf("[DEBUG] About to set up cursorCompare for search\n");
			
			// Search for the first matching record
			printf("[DEBUG] About to call CompareKey for first record\n");
			err = CompareKey(GrapaBtree::SEARCH_MODE, cursorCompare, pCursor, result);
			printf("[DEBUG] CompareKey result: err=%d, result=%d\n", err, result);
			if (!err && result == 0)
			{
				printf("[DEBUG] Found first match\n");
				return 0; // Found first match
			}
			
			// Continue searching through records
			int recordCount = 0;
			while (!err)
			{
				err = NextDb(pCursor);
				if (err) {
					printf("[DEBUG] NextDb() failed with error %d after %d records\n", err, recordCount);
					break;
				}
				recordCount++;
				
				err = CompareKey(GrapaBtree::SEARCH_MODE, cursorCompare, pCursor, result);
				printf("[DEBUG] CompareKey result: err=%d, result=%d (record %d)\n", err, result, recordCount);
				if (!err && result == 0)
				{
					printf("[DEBUG] Found match after %d records\n", recordCount);
					return 0; // Found match
				}
			}
			printf("[DEBUG] Table scan completed, no match found\n");
		}
		else
		{
			// No search criteria, just position at first record
			printf("[DEBUG] No search criteria, positioned at first record\n");
			return 0;
		}
	}

	return 0;
}

GrapaError GrapaDBX::FirstDb(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaDBXCursor cursorCompare = (GrapaDBXCursor&)cursor;
	while(!err)
	{
		err = PrevDb(cursorCompare);
		if (err) return(0);
		cursor = cursorCompare;
	}
	return(0);
}

GrapaError GrapaDBX::LastDb(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaDBXCursor cursorCompare = (GrapaDBXCursor&)cursor;
	while(!err)
	{
		err = NextDb(cursorCompare);
		if (err) return(0);
		cursor = cursorCompare;
	}
	return(0);
}

GrapaError GrapaDBX::NextDb(GrapaCursor& cursor)
{
	GrapaError err = 0;
	s8 result;
	//s8 valueType = cursor.mValueType;
	while(true)
	{
		GrapaDBXCursor cursorCompare = (GrapaDBXCursor&)cursor;
		err = Next(cursor);
		// do we need to check mKey for 0?
		if (err) return(err);
	
		cursorCompare.mValueType = SEARCH_ITEM;
		//cursor.mValueType = RPTR_ITEM;
		err = CompareKey(SEARCH_MODE, cursorCompare, cursor, result);
		if (err) return(err);
		//cursor.mValueType = valueType;
		if (result==0) return(0);
		
		// Check if using index - cast to GrapaDBXCursor to access mUsingIndex
		GrapaDBXCursor* dbxCursor = dynamic_cast<GrapaDBXCursor*>(&cursor);
		if (dbxCursor && dbxCursor->mUsingIndex)
		{
			return((GrapaError)-1);
		}
	}
	return(0);
}

GrapaError GrapaDBX::PrevDb(GrapaCursor& cursor)
{
	GrapaError err = 0;
	s8 result;
	//s8 valueType = cursor.mValueType;
	while(true)
	{
		GrapaDBXCursor cursorCompare = (GrapaDBXCursor&)cursor;
		err = Prev(cursor);
		if (err) return(err);
		cursorCompare.mValueType = SEARCH_ITEM;
		//cursor.mValueType = RPTR_ITEM;
		err = CompareKey(SEARCH_MODE, cursorCompare, cursor, result);
		if (err) return(err);
		//cursor.mValueType = valueType;
		if (result==0) return(0);
		
		// Check if using index - cast to GrapaDBXCursor to access mUsingIndex
		GrapaDBXCursor* dbxCursor = dynamic_cast<GrapaDBXCursor*>(&cursor);
		if (dbxCursor && dbxCursor->mUsingIndex)
		{
			return((GrapaError)-1);
		}
	}
	return(0);
}

// Add the missing FirstFreeId method
GrapaError GrapaDBX::FirstFreeId(u64 tableRef, u64 minId, u64& resId)
{
	GrapaError err = LastTableId(tableRef, resId);
	if (resId < minId) resId = minId;
	if (err) return(err);
	return(0);
}

GrapaError GrapaDBX::DumpTree(u64 pTreeRef, GrapaFile* pDumpFile)
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

GrapaError GrapaDBX::DatabaseDump(u64 pTreeRef, GrapaFile& pDumpFile)
{
	GrapaError err = 0;
	
	/* Save current dump file and set new one */
	GrapaFile *oldDumpFile = mDumpFile;
	mDumpFile = &pDumpFile;
	
	/* Call the existing DumpTree method */
	err = DumpTree(pTreeRef, &pDumpFile);
	
	/* Restore original dump file */
	mDumpFile = oldDumpFile;
	
	return err;
}

// Dump helper methods - placeholder implementations
GrapaError GrapaDBX::DumpTheTree(GrapaCHAR& dbWrite, const char *leader, u64 tableId, u64 firstTree)
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

GrapaError GrapaDBX::DumpTheValue(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheNumber(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sSU64 key=%llu value=%llu\n",leader,cursor.mKey,cursor.mValue);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDBX::DumpThePointer(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheGroupPtr(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheRowRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheColRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheTreeItem(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheDT(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaDBXField dbField;
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



GrapaError GrapaDBX::DumpTheColStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
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

// GrapaGroup2 implementation - inherits from GrapaDBX
GrapaGroup2::GrapaGroup2() : GrapaDBX()
{
	mRootTable = 0;
	mRootType = 0;
}

GrapaGroup2::GrapaGroup2(GrapaFile* pFile) : GrapaDBX(pFile)
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
	err = GrapaDBX::OpenFile((const char*)fileName.mBytes, mode);
	mRootTable = RootTree(mRootType);
	if (mRootType == GROUP_TREE)
	{
		GrapaDBXTable parentDict;
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
	GrapaError err = GrapaDBX::CloseFile();
	mRootTable = 0;
	mRootType = 0;
	mCritical.LeaveCritical();
	return(err);
}

// Hierarchical operations - delegate to GrapaGroup for now (placeholder implementation)
GrapaError GrapaGroup2::CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, u8 pTableType, u64& pNewTree)
{

	GrapaError err;
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXTable newTable;
	GrapaDBXFieldValueArray data;

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
		GrapaDBXFieldValueArray data2;
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
	GrapaDBXFieldValueArray data;
	GrapaDBXTable parentDict;
	GrapaDBXCursor cursor;
	GrapaDBXTable table;

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
	GrapaDBXTable parentDict;
	GrapaDBXTable table;

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
		GrapaDBXCursor cursor;
		GrapaDBXFieldValueArray data;
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
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXFieldValueArray data;
	GrapaCHAR dataName(pDataName);

	printf("[DEBUG] GrapaGroup2::CreateEntry: parentTree=%llu, parentType=%d, dataName='%s'\n", 
	       parentTree, parentType, (char*)dataName.mBytes);

	pId = 0;

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		printf("[DEBUG] GrapaGroup2::CreateEntry: Invalid data name\n");
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			printf("[DEBUG] GrapaGroup2::CreateEntry: OpenTable failed with error %d\n", err);
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		printf("[DEBUG] GrapaGroup2::CreateEntry: GetNameId failed with error %d\n", err);
		return(err);
	}
	if (nameId == 0)
	{
		printf("[DEBUG] GrapaGroup2::CreateEntry: nameId is 0\n");
		return(-1);
	}

	data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (!err)
	{
		pId = cursor.mKey;
		printf("[DEBUG] GrapaGroup2::CreateEntry: Entry already exists with ID=%llu\n", pId);
		return(0);
	}

	err = CreateRecord(parentDict, cursor);
	if (err)
	{
		printf("[DEBUG] GrapaGroup2::CreateEntry: CreateRecord failed with error %d\n", err);
		return(err);
	}
	
	pId = cursor.mKey;

	{
		GrapaDBXFieldValueArray data2;
		data2.Append(this, parentDict, nameId, pDataName, EQ_CMP);
		err = SetRecordField(cursor, data2);
		if (err)
		{
			printf("[DEBUG] GrapaGroup2::CreateEntry: SetRecordField failed with error %d\n", err);
			return(err);
		}
	}

	printf("[DEBUG] GrapaGroup2::CreateEntry: Successfully created entry with ID=%llu\n", pId);
	return(0);
}

GrapaError GrapaGroup2::FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId)
{
	GrapaError err;
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXFieldValueArray data;
	GrapaCHAR dataName(pDataName);

	printf("[DEBUG] GrapaGroup2::FindEntry: parentTree=%llu, parentType=%d, dataName='%s'\n", 
	       parentTree, parentType, (char*)dataName.mBytes);

	pId = 0;

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		printf("[DEBUG] GrapaGroup2::FindEntry: Invalid data name\n");
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			printf("[DEBUG] GrapaGroup2::FindEntry: OpenTable failed with error %d\n", err);
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		printf("[DEBUG] GrapaGroup2::FindEntry: GetNameId failed with error %d\n", err);
		return(err);
	}
	if (nameId == 0)
	{
		printf("[DEBUG] GrapaGroup2::FindEntry: nameId is 0\n");
		return(-1);
	}

	data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (err)
	{
		printf("[DEBUG] GrapaGroup2::FindEntry: SearchDb failed with error %d\n", err);
		return(err);
	}

	pId = cursor.mKey;
	printf("[DEBUG] GrapaGroup2::FindEntry: Successfully found entry with ID=%llu\n", pId);

	return(0);
}

GrapaError GrapaGroup2::DeleteEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName)
{

	GrapaError err;
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXFieldValueArray data;

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
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;

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
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXField dbFieldName;
	GrapaDBXIndex dbIndexName;
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

	GrapaDBXFieldValueArray data;
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
		GrapaDBXFieldValueArray data2;
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
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXFieldValueArray data;

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
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXField dbFieldName, dbFieldValue;
	GrapaDBXIndex dbIndexName;
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
	
	/* For now, since GrapaDBX is a placeholder implementation, 
	   we'll skip the complex BTree search and just use a simple approach */
	
	/* Initialize the dict field with default values for placeholder */
	parentDict.mDictField.Init(0, GrapaTokenType::START, 0, 0, 1);
	parentDict.mDictField.mTreeType = parentType;

	dataId = 0;

	printf("[DEBUG] GetField: About to call GetNameId\n");
	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	printf("[DEBUG] GetField: GetNameId returned err=%d, nameId=%llu\n", err, nameId);

	/* Find the actual record by name */
	err = FindEntry(parentTree, parentType, pDataName, dataId);
	if (err) {
		printf("[DEBUG] GetField: FindEntry failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] GetField: FindEntry succeeded, dataId=%llu\n", dataId);

	cursor.Set(parentDict.mRecRef, RREC_ITEM, dataId);
	err = Search(cursor);
	if (err) {
		printf("[DEBUG] GetField: Search(cursor) failed with error %d\n", err);
		return(err);
	}

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
		GrapaDBXField field;
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
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXField dbFieldName, dbFieldValue;
	GrapaDBXIndex dbIndexName;
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
		GrapaDBXField field;
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
	GrapaDBXTable parentDict;
	GrapaDBXCursor cursor;

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
	/* Simplified implementation for GrapaDBX */
	/* This bypasses the complex GrapaGroup logic and directly stores data */
	
	printf("[DEBUG] GrapaGroup2::SetField called with parentTree=%llu, parentType=%d\n", parentTree, parentType);
	printf("[DEBUG] Data name: %s, Field name: %s\n", (char*)pDataName.mBytes, (char*)pFieldNameX.mBytes);
	
	GrapaError err;
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	
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
	GrapaDBXFieldValueArray data;
	GrapaDBXFieldValue fieldValue;
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

GrapaDBXFieldArray* GrapaGroup2::ListFields(u64 parentTree, u8 parentType)
{

	GrapaError err;
	GrapaDBXTable parentDict;
	GrapaDBXCursor cursor;
	GrapaDBXFieldArray* pFieldList = new GrapaDBXFieldArray();

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

GrapaError GrapaGroup2::FindField(u64 parentTree, u8 parentType, const GrapaCHAR& pFieldNameX, GrapaDBXField& pField, u64& pMaxId)
{

	GrapaError err;
	GrapaDBXTable parentDict;
	GrapaDBXCursor cursor;
	GrapaDBXFieldValueArray data;
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

/* Index Management Methods */
GrapaError GrapaGroup2::CreateIndex(const GrapaCHAR& indexName, const GrapaCHAR& fields)
{
	printf("[DEBUG] GrapaGroup2::CreateIndex: indexName='%s', fields='%s'\n", 
	       (char*)indexName.mBytes, (char*)fields.mBytes);
	
	/* Basic implementation - parse field names and create index */
	GrapaError err = 0;
	
	/* Parse the fields parameter (comma-separated field names) */
	GrapaDU64Array fieldIds;
	GrapaCHAR fieldName;
	u64 fieldId;
	
	/* For now, assume simple field names and map them to IDs */
	/* In a real implementation, this would parse the fields string and look up field IDs */
	
	/* Create a dummy index for testing */
	GrapaDBXIndex index;
	GrapaDBXTable table;
	
	/* Get the current table context */
	/* This would need to be passed in or retrieved from the current context */
	
	/* For now, just return success */
	printf("[DEBUG] GrapaGroup2::CreateIndex: Basic implementation - returning success\n");
	return 0;
}

GrapaError GrapaGroup2::RemoveIndex(const GrapaCHAR& indexName)
{
	printf("[DEBUG] GrapaGroup2::RemoveIndex: indexName='%s'\n", (char*)indexName.mBytes);
	
	/* Basic implementation - look up index by name and remove it */
	GrapaError err = 0;
	
	/* Look up the index by name to get its ID */
	/* In a real implementation, this would query the index metadata storage */
	
	/* For now, just return success */
	printf("[DEBUG] GrapaGroup2::RemoveIndex: Basic implementation - returning success\n");
	return 0;
}

GrapaError GrapaGroup2::ListIndexes(GrapaCHAR& indexList)
{
	printf("[DEBUG] GrapaGroup2::ListIndexes\n");
	
	/* Basic implementation - query index metadata storage */
	GrapaError err = 0;
	
	/* Query the index metadata storage */
	/* In a real implementation, this would build a list of all available indexes */
	
	/* For now, return empty list as placeholder */
	indexList.FROM("[]");
	printf("[DEBUG] GrapaGroup2::ListIndexes: Basic implementation - returning empty list\n");
	return 0;
}

GrapaError GrapaGroup2::RefreshIndex(const GrapaCHAR& indexName)
{
	printf("[DEBUG] GrapaGroup2::RefreshIndex: indexName='%s'\n", (char*)indexName.mBytes);
	
	/* Basic implementation - look up index and rebuild it */
	GrapaError err = 0;
	
	/* Look up the index by name to get its ID */
	/* In a real implementation, this would call the underlying GrapaDBX::RefreshIndex method */
	
	/* For now, just return success */
	printf("[DEBUG] GrapaGroup2::RefreshIndex: Basic implementation - returning success\n");
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
		u8 type = GrapaDBX::GROUP_TREE;
		if (pType.StrCmp("ROW") == 0) type = GrapaDBX::RTABLE_TREE;
		else if (pType.StrCmp("COL") == 0) type = GrapaDBX::CTABLE_TREE;
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
// GrapaDBX Data Structure Method Implementations
////////////////////////////////////////////////////////////////////////////////

// GrapaDBXField implementations
GrapaDBXField::GrapaDBXField()
{
	memset(this, 0, sizeof(GrapaDBXField));
}

void GrapaDBXField::Init(u64 pFieldId, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
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

void GrapaDBXField::BigEndian()
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

void* GrapaDBXField::GetPtr()
{
	return (void*)this;
}

u16 GrapaDBXField::GetSize()
{
	return sizeof(GrapaDBXField);
}

GrapaError GrapaDBXField::Write(GrapaDBX *pDb, u64 fieldRef)
{
	// Write field data to database using GrapaBtree operations
	if (!pDb || fieldRef == 0) return -1;
	
	BigEndian();  // Convert to big-endian for storage
	GrapaError err = pDb->SetDataValue(fieldRef, 0, sizeof(GrapaDBXField), (const char*)this);
	BigEndian();  // Convert back to native endian
	return err;
}

GrapaError GrapaDBXField::Read(GrapaDBX *pDb, u64 fieldRef)
{
	// Read field data from database using GrapaBtree operations
	if (!pDb || fieldRef == 0) return -1;
	
	// Read the field structure from the database
	u64 returnSize = 0;
	GrapaError err = pDb->GetDataValue(fieldRef, 0, sizeof(GrapaDBXField), (char*)this, &returnSize);
	BigEndian();  // Convert from big-endian to native endian
	return err;
}

GrapaError GrapaDBXField::Get(GrapaDBX *pDb, u64 tableRef, u64 fieldId)
{
	// Get field from table - simplified implementation
	if (!pDb || tableRef == 0) return -1;
	
	// Set basic field info
	mId = fieldId;
	mRef = tableRef;
	
	// Set up field information for string fields (most common case)
	mType = GrapaTokenType::STR;
	mStore = STORE_VAR;
	mSize = 32;
	mGrow = 8;
	mDictOffset = 0;
	mDictSize = 0;
	
	printf("[DEBUG] GrapaDBXField::Get: Set up field %llu with type=%d, store=%d, size=%llu\n", 
	       fieldId, mType, mStore, mSize);
	
	return 0;
}

// GrapaDBXFieldArray implementations
GrapaDBXFieldArray::~GrapaDBXFieldArray()
{
	// Clean up any allocated resources
	// GrapaVoidArray handles most cleanup automatically
}

GrapaError GrapaDBXFieldArray::Append(GrapaDBX *pDb, GrapaDBXTable& pTable, u64 pFieldId)
{
	// Append field to array - simplified implementation
	if (!pDb) return -1;
	
	// Create a new field and add it to the array
	GrapaDBXField* newField = new GrapaDBXField();
	if (!newField) return -1;
	
	newField->Init(pFieldId, GrapaTokenType::STR, GrapaDBXField::STORE_VAR, 32, 8);
	newField->mRef = pTable.mRef;
	
	// Add to the array using base class method
	GrapaVoidArray::Append((void*)newField);
	return 0;
}

GrapaError GrapaDBXFieldArray::Append(GrapaDBXField *pField)
{
	// Append field to array
	if (!pField) return -1;
	
	// Add to the array using base class method
	GrapaVoidArray::Append((void*)pField);
	return 0;
}

// GrapaDBXFieldValue implementations
void GrapaDBXFieldValue::BigEndian()
{
	GrapaDBXField::BigEndian();
	mValue.mLength = BE_S64(mValue.mLength);
	mValue.mSize = BE_S64(mValue.mSize);
	mCmp = BE_S16(mCmp);
}

// GrapaDBXFieldValueArray implementations
GrapaDBXFieldValueArray::~GrapaDBXFieldValueArray()
{
	// Clean up any allocated resources
	// GrapaVoidArray handles most cleanup automatically
}

GrapaError GrapaDBXFieldValueArray::Append(GrapaDBX *pDb, GrapaDBXTable& pTable, u64 pFieldId, const GrapaBYTE& pValue, s16 pCmp)
{
	// Append field value to array - simplified implementation
	if (!pDb) return -1;
	
	// Create a new field value and add it to the array
	GrapaDBXFieldValue* newFieldValue = new GrapaDBXFieldValue();
	if (!newFieldValue) return -1;
	
	// Initialize the field value
	newFieldValue->Init(pFieldId, GrapaTokenType::STR, GrapaDBXField::STORE_VAR, 32, 8);
	newFieldValue->mValue = pValue;
	newFieldValue->mCmp = pCmp;
	newFieldValue->mRef = pTable.mRef;
	
	// Add to the array using base class method
	GrapaVoidArray::Append((void*)newFieldValue);
	return 0;
} 

GrapaError GrapaDBX::DumpTheGroupStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor, u64 tableDT)
{
	GrapaError err;
	GrapaCursor dataTypeCursor;
	GrapaDBXField dbField;
	GrapaBYTE dbChar;

	/* For now, use placeholder implementation since we don't have full BTree implementation */
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "STRUCT [placeholder]");
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDBX::PtrToRec(GrapaCursor& ptrCursor, GrapaCursor& recCursor)
{
	GrapaError err = 0;
	u64 tableRef;
	u8 storeType;
	
	recCursor = ptrCursor;
	
	switch(ptrCursor.mValueType)
	{
		case GREC_ITEM:
		case RREC_ITEM:
		case CREC_ITEM:
			return(0);
		case GPTR_ITEM:
		case RPTR_ITEM:
		case CPTR_ITEM:
			break;
		default:
			return(-1);
			break;
	}
	
	err = GetTreeStore(ptrCursor, tableRef, storeType);
	if (err) return(err);
	
	switch(ptrCursor.mValueType)
	{
		case GPTR_ITEM:
			recCursor.Set(tableRef, GREC_ITEM, ptrCursor.mKey);
			recCursor.mTreeType = GROUP_TREE;
			break;
		case RPTR_ITEM:
			recCursor.Set(tableRef, RREC_ITEM, ptrCursor.mKey);
			recCursor.mTreeType = RTABLE_TREE;
			break;
		case CPTR_ITEM:
			recCursor.Set(tableRef, CREC_ITEM, ptrCursor.mKey);
			recCursor.mTreeType = CTABLE_TREE;
			break;
	}
	
	GrapaError searchErr = Search(recCursor);
	return searchErr;
}

GrapaError GrapaDBX::DumpTheDataType(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDBX::DumpTheGroupRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	/* Dump group record - recursively dump the tree */
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sGREC [weight: %d]", leader, 0);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return DumpTheTree(dbWrite, leader, 0, cursor.mValue);
}

GrapaError GrapaDBX::DumpTheRowStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
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

// GrapaDBXTable BigEndian implementation
void GrapaDBXTable::BigEndian()
{
	mDictField.BigEndian();
	mId = BE_S64(mId);
	mRef = BE_S64(mRef);
	mRecRef = BE_S64(mRecRef);
}

// GrapaDBXIndex BigEndian implementation
void GrapaDBXIndex::BigEndian()
{
	mTable.BigEndian();
	mId = BE_S64(mId);
	mRef = BE_S64(mRef);
}

// Formula field operations - new functionality for GrapaDBX
GrapaError GrapaDBX::CreateFormulaField(GrapaDBXTable& pTable, const GrapaCHAR& pFieldName, const GrapaCHAR& pFormulaText, u8 pResultType)
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
	GrapaDBXField field;
	field.Init(GetNextFieldId(), pResultType, GrapaDBXField::STORE_VAR, 32, 8);
	field.mFormulaRef = formulaRef;
	field.mFormulaType = GrapaDBXField::FORMULA_TEXT;
	field.mTableRef = pTable.mRef;
	
	// 4. Store the field in the table
	err = CreateTableField(pTable, field, pFieldName);
	return err;
}

GrapaError GrapaDBX::CreateCompiledFormulaField(GrapaDBXTable& pTable, const GrapaCHAR& pFieldName, const GrapaCHAR& pFormulaText, u8 pResultType)
{
	GrapaError err;
	
	// 1. Allocate storage for the compiled formula
	u64 formulaRef = 0;
	err = AllocateFormulaStorage(formulaRef);
	if (err) return err;
	
	// 2. Compile the formula text to $OP format
	GrapaCHAR compiledFormula;
	err = CompileFormulaToOP(pFormulaText, compiledFormula);
	if (err) return err;
	
	// 3. Store the compiled formula
	err = StoreCompiledFormula(formulaRef, compiledFormula);
	if (err) return err;
	
	// 4. Create the field with compiled formula reference
	GrapaDBXField field;
	field.Init(GetNextFieldId(), pResultType, GrapaDBXField::STORE_VAR, 32, 8);
	field.mFormulaRef = formulaRef;
	field.mFormulaType = GrapaDBXField::FORMULA_OP;  // Use compiled $OP type
	field.mTableRef = pTable.mRef;
	
	// 5. Store the field in the table
	err = CreateTableField(pTable, field, pFieldName);
	return err;
}

GrapaError GrapaDBX::GetFormulaText(u64 pFormulaRef, GrapaCHAR& pFormulaText)
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

GrapaError GrapaDBX::StoreFormulaText(u64 pFormulaRef, const GrapaCHAR& pFormulaText)
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

GrapaError GrapaDBX::ExecuteFormula(u64 pFormulaRef, u8 pFormulaType, const GrapaCHAR& pParams, GrapaCHAR& pResult)
{
	if (pFormulaRef == 0) return -1;
	
	GrapaError err = 0;
	
	switch (pFormulaType) {
		case GrapaDBXField::FORMULA_TEXT: {
			// 1. Get the formula text
			GrapaCHAR formulaText;
			err = GetFormulaText(pFormulaRef, formulaText);
			if (err) return err;
			
			// 2. Parse parameters to get cursor and table info
			GrapaCursor cursor;
			GrapaDBXTable table;
			err = ParseFormulaParams(pParams, cursor, table);
			if (err) return err;
			
			// 3. Create context-aware record environment
			GrapaRuleEvent* context = CreateRecordContext(cursor, table);
			if (!context) return -1;
			
			// 4. Execute the formula with context
			err = ExecuteFormulaWithContext(formulaText, context, pResult);
			
			// Cleanup context
			if (context) {
				context->CLEAR();
				delete context;
			}
			
			return err;
		}
		
		case GrapaDBXField::FORMULA_OP: {
			// 1. Get the compiled $OP formula
			GrapaCHAR compiledFormula;
			err = GetFormulaText(pFormulaRef, compiledFormula);
			if (err) return err;
			
			// 2. Parse parameters to get cursor and table info
			GrapaCursor cursor;
			GrapaDBXTable table;
			err = ParseFormulaParams(pParams, cursor, table);
			if (err) return err;
			
			// 3. Create context-aware record environment
			GrapaRuleEvent* context = CreateRecordContext(cursor, table);
			if (!context) return -1;
			
			// 4. Execute the compiled $OP formula with context
			err = ExecuteCompiledFormula(compiledFormula, context, pResult);
			
			// Cleanup context
			if (context) {
				context->CLEAR();
				delete context;
			}
			
			return err;
		}
		
		default:
			return -1; // Unsupported formula type
	}
}

// Helper method to allocate formula storage
GrapaError GrapaDBX::AllocateFormulaStorage(u64& pFormulaRef)
{
	// For now, use a simple allocation strategy
	// TODO: Implement proper BTree-based allocation
	static u64 nextFormulaRef = 1000000; // Start at a high number to avoid conflicts
	pFormulaRef = nextFormulaRef++;
	return 0;
}

// Helper method to get next field ID
u64 GrapaDBX::GetNextFieldId()
{
	/* BTree-based field ID allocation */
	// For now, use a simple counter approach
	// TODO: Implement proper BTree-based ID tracking
	static u64 nextFieldId = 1;
	return nextFieldId++;
}

// Context-aware record environment for formula execution
GrapaRuleEvent* GrapaDBX::CreateRecordContext(GrapaCursor& cursor, GrapaDBXTable& table)
{
	GrapaRuleEvent* context = new GrapaRuleEvent();
	context->mValue.mToken = GrapaTokenType::LIST;
	context->vQueue = new GrapaRuleQueue();
	
	// Add record context information
	GrapaRuleEvent* recordContext = new GrapaRuleEvent();
	recordContext->mName.FROM("_record");
	recordContext->mValue.mToken = GrapaTokenType::LIST;
	recordContext->vQueue = new GrapaRuleQueue();
	
	// Add table information
	GrapaRuleEvent* tableInfo = new GrapaRuleEvent();
	tableInfo->mName.FROM("table");
	GrapaCHAR tableName;
	tableName.FROM("table_id_");
	tableName.Append(table.mId);
	tableInfo->mValue.FROM(tableName);
	recordContext->vQueue->PushTail(tableInfo);
	
	// Add record position
	GrapaRuleEvent* recordPos = new GrapaRuleEvent();
	recordPos->mName.FROM("position");
	recordPos->mValue.FROM(cursor.mValue);
	recordContext->vQueue->PushTail(recordPos);
	
	// Add dynamic field access functions
	GrapaRuleEvent* getFieldFunc = new GrapaRuleEvent();
	getFieldFunc->mName.FROM("getField");
	getFieldFunc->mValue.mToken = GrapaTokenType::OP;
	getFieldFunc->mValue.FROM("@<[op,@<record_get_field,{this,@<var,{fieldName}>}>],{fieldName}>");
	recordContext->vQueue->PushTail(getFieldFunc);
	
	// Add partial field access for large fields
	GrapaRuleEvent* getFieldPartialFunc = new GrapaRuleEvent();
	getFieldPartialFunc->mName.FROM("getFieldPartial");
	getFieldPartialFunc->mValue.mToken = GrapaTokenType::OP;
	getFieldPartialFunc->mValue.FROM("@<[op,@<record_get_field_partial,{this,@<var,{fieldName}>,@<var,{offset}>,@<var,{length}>,@<var,{operation}>}>],{fieldName,offset,length,operation}>");
	recordContext->vQueue->PushTail(getFieldPartialFunc);
	
	context->vQueue->PushTail(recordContext);
	
	return context;
}

// Formula execution helper methods implementation

GrapaError GrapaDBX::ParseFormulaParams(const GrapaCHAR& pParams, GrapaCursor& cursor, GrapaDBXTable& table)
{
	// Parse parameters to extract cursor and table information
	// For now, use simple parsing - this can be enhanced later
	
	if (pParams.mLength == 0) {
		// Use default values if no parameters provided
		cursor.mValue = 1; // Default record position
		table.mId = 1;     // Default table ID
		return 0;
	}
	
	// Simple parameter parsing: "cursor=X,table=Y"
	// This is a basic implementation - can be enhanced for more complex parameter formats
	
	GrapaCHAR params(pParams);
	GrapaRuleQueue paramList;
	paramList.AppendNames((char*)params.mBytes, ",");
	
	GrapaObjectEvent* param = paramList.Head();
	while (param) {
		GrapaRuleQueue keyValue;
		keyValue.AppendNames((char*)param->mName.mBytes, "=");
		
		if (keyValue.mCount >= 2) {
			GrapaObjectEvent* key = keyValue.Head();
			GrapaObjectEvent* value = key->Next();
			
			if (key->mName.StrCmp("cursor") == 0) {
				// Parse cursor value
				GrapaInt cursorValue((char*)value->mName.mBytes, 10);
				cursor.mValue = cursorValue.LongValue();
			}
			else if (key->mName.StrCmp("table") == 0) {
				// Parse table ID
				GrapaInt tableId((char*)value->mName.mBytes, 10);
				table.mId = tableId.LongValue();
			}
		}
		
		param = param->Next();
	}
	
	return 0;
}

GrapaError GrapaDBX::ExecuteFormulaWithContext(const GrapaCHAR& formulaText, GrapaRuleEvent* context, GrapaCHAR& result)
{
	// Execute formula text with context-aware record environment
	// This uses Grapa's dynamic code execution capabilities
	
	// For now, implement a basic execution that uses the context
	// In a full implementation, this would integrate with Grapa's script execution engine
	
	result.FROM("Formula executed with context: ");
	result.Append(formulaText);
	result.Append(" (context provides _record.getField() and _record.getFieldPartial())");
	
	// TODO: Integrate with Grapa's script execution engine
	// This would involve:
	// 1. Creating a GrapaScriptExec instance
	// 2. Setting up the namespace with the context
	// 3. Compiling and executing the formula text
	// 4. Capturing the result
	
	return 0;
}

GrapaError GrapaDBX::ExecuteCompiledFormula(const GrapaCHAR& compiledFormula, GrapaRuleEvent* context, GrapaCHAR& result)
{
	// Execute compiled $OP formula with context-aware record environment
	// This is more efficient than text-based execution
	
	// For now, implement a basic execution that recognizes compiled formulas
	// In a full implementation, this would integrate with Grapa's $OP execution engine
	
	result.FROM("Compiled formula executed with context: ");
	result.Append(compiledFormula);
	result.Append(" (using pre-compiled $OP for better performance)");
	
	// TODO: Integrate with Grapa's $OP execution engine
	// This would involve:
	// 1. Parsing the compiled $OP structure
	// 2. Setting up the namespace with the context
	// 3. Executing the compiled $OP directly
	// 4. Capturing the result
	
	return 0;
}

GrapaError GrapaDBX::CompileFormulaToOP(const GrapaCHAR& formulaText, GrapaCHAR& compiledFormula)
{
	// Compile formula text to $OP format for efficient execution
	// This uses Grapa's compilation capabilities
	
	// For now, create a simple $OP structure as placeholder
	// In a full implementation, this would use Grapa's actual compiler
	
	compiledFormula.FROM("@<[op,@<formula_execution,{");
	compiledFormula.Append(formulaText);
	compiledFormula.Append("}>],{}>");
	
	// TODO: Integrate with Grapa's actual compiler
	// This would involve:
	// 1. Using Grapa's parsing and compilation engine
	// 2. Creating proper $OP execution trees
	// 3. Optimizing the compiled code for formula execution
	
	return 0;
}

GrapaError GrapaDBX::StoreCompiledFormula(u64 pFormulaRef, const GrapaCHAR& compiledFormula)
{
	// Store compiled formula in the database
	// This uses the same storage mechanism as text formulas
	
	return StoreFormulaText(pFormulaRef, compiledFormula);
}

GrapaError GrapaDBX::RecordGetField(GrapaCursor& cursor, const GrapaCHAR& fieldName, GrapaBYTE& result)
{
	// Look up field by name
	u64 fieldId;
	GrapaError err = GetFieldIdByName(cursor, fieldName, fieldId);
	if (err) return err;
	
	// Load field value on-demand
	return GetRecordField(cursor, fieldId, result);
}

GrapaError GrapaDBX::RecordGetFieldPartial(GrapaCursor& cursor, const GrapaCHAR& fieldName, 
                                          u64 offset, u64 length, GrapaCHAR& operation, GrapaBYTE& result)
{
	// Handle large fields with streaming operations
	if (operation.StrCmp("grep") == 0) {
		return StreamingGrep(cursor, fieldName, offset, length, result);
	} else if (operation.StrCmp("substring") == 0) {
		return LoadFieldSubstring(cursor, fieldName, offset, length, result);
	}
	
	// Fallback to normal field access
	return RecordGetField(cursor, fieldName, result);
}

GrapaError GrapaDBX::GetFieldIdByName(GrapaCursor& cursor, const GrapaCHAR& fieldName, u64& fieldId)
{
	// For now, use a simplified approach since we don't have GetTable method
	// TODO: Implement proper table lookup from cursor
	// This is a placeholder implementation
	
	// Search for field by name in the current table context
	// For now, return a default field ID
	fieldId = 1; // Placeholder
	return 0;
}

GrapaError GrapaDBX::StreamingGrep(GrapaCursor& cursor, const GrapaCHAR& fieldName, 
                                  u64 offset, u64 length, GrapaBYTE& result)
{
	const u64 BUFFER_SIZE = 8192;  // 8KB chunks
	GrapaBYTE buffer;
	buffer.SetSize(BUFFER_SIZE);
	
	// Get field information
	u64 fieldId;
	GrapaError err = GetFieldIdByName(cursor, fieldName, fieldId);
	if (err) return err;
	
	// For now, use a simplified approach
	// TODO: Implement proper field lookup and streaming
	result.FROM("Streaming grep not yet implemented for field: ");
	result.Append(fieldName);
	result.Append(" with offset: ");
	result.Append(offset);
	result.Append(" length: ");
	result.Append(length);
	
	return 0;
}

GrapaError GrapaDBX::LoadFieldSubstring(GrapaCursor& cursor, const GrapaCHAR& fieldName, 
                                       u64 offset, u64 length, GrapaBYTE& result)
{
	// Get field information
	u64 fieldId;
	GrapaError err = GetFieldIdByName(cursor, fieldName, fieldId);
	if (err) return err;
	
	// For now, use a simplified approach
	// TODO: Implement proper field lookup and substring loading
	result.FROM("Substring loading not yet implemented for field: ");
	result.Append(fieldName);
	result.Append(" offset: ");
	result.Append(offset);
	result.Append(" length: ");
	result.Append(length);
	
	return 0;
}

// Caching configuration methods
void GrapaDBX::SetCachingMode(CachingMode mode)
{
	mCachingMode = mode;
	if (mode == CACHE_DISABLED) {
		DisableCaching();
	} else if (mode == CACHE_ENABLED) {
		EnableCaching();
	}
}

void GrapaDBX::SetCacheSize(u64 size)
{
	mCacheSize = size;
	if (IsCachingEnabled()) {
		mTree.SetCache(size);
	}
}

bool GrapaDBX::IsCachingEnabled() const
{
	return mCachingMode != CACHE_DISABLED;
}

GrapaError GrapaDBX::EnableCaching()
{
	mCachingMode = CACHE_ENABLED;
	return mTree.SetCache(mCacheSize);
}

GrapaError GrapaDBX::DisableCaching()
{
	mCachingMode = CACHE_DISABLED;
	return mTree.SetCache(0);  // Set cache size to 0 to disable
}

// Implement SetSearch for GrapaDBXCursor
void GrapaDBXCursor::SetSearch(GrapaDBX* pDb, u64 pTreeRef, bool pUsingIndex, GrapaDBXFieldValueArray* pData)
{
	mTreeRef = pTreeRef;
	mUsingIndex = pUsingIndex;
	mData = pData;
	mValueType = GrapaDBX::SEARCH_ITEM; // Fixed scope
}

// Index helper methods - same interface as GrapaDB
GrapaError GrapaDBX::LocateIndex(GrapaCursor& cursor, u64 indexRef, u64 fieldId)
{
	GrapaError err;
	GrapaCursor indexCursor;
	GrapaCursor indexField;
	u64 indexFieldsRef;

	cursor.Set(indexRef);
	err = First(cursor);
	if (!err && cursor.mKey==0)
		err = Next(cursor);
	while (!err)
	{
		indexCursor.Set(cursor.mValue);
		err = GetTreeIndex(indexCursor,indexFieldsRef);
		if (err) return(err);
		if (indexFieldsRef==0) return(-1);
		indexField.Set(indexFieldsRef);
		err = First(indexField);
		if ((!err) && (indexField.mValue == fieldId))
		{
			return(0);
		}
		err = Next(cursor);
		if (!err && cursor.mKey==0)
			err = Next(cursor);
	}
	return((GrapaError)-1);
}

bool GrapaDBX::IndexHasField(GrapaCursor& cursor, u64 fieldId)
{
	GrapaError err;
	GrapaCursor indexCursor;
	GrapaCursor indexField;
	u64 indexFieldsRef;

	indexCursor.Set(cursor.mValue);
	err = GetTreeIndex(indexCursor,indexFieldsRef);
	if (err) return(false);
	indexField.Set(indexFieldsRef);
	err = First(indexField);
	while(!err)
	{
		if (indexField.mValue == fieldId)
			return(true);
		err = Next(indexField);
	}
	return(false);
}

// Debug functions for index validation and testing
void GrapaDBX::DebugPrintIndexPointerAndRecord(u64 tableRef, u64 key)
{
    u64 indexRef = 0;
    GrapaCursor indexCursor;
    GrapaError err;
    u64 weight, recWeight;
    
    // Get index tree reference
    indexCursor.Set(tableRef);
    err = GetTreeIndex(indexCursor, indexRef);
    if (err || indexRef == 0) {
        printf("DEBUG: Could not find index tree for tableRef=%llu\n", tableRef);
        return;
    }
    
    // Search for the RPTR_ITEM with the given key
    GrapaCursor ptrCursor;
    ptrCursor.Set(indexRef, RPTR_ITEM, key);
    err = Search(ptrCursor);
    if (err) {
        printf("DEBUG: Could not find RPTR_ITEM for key=%llu in indexRef=%llu\n", key, indexRef);
        return;
    }
    
    DumpGetItemWeight(ptrCursor, weight);
    printf("DEBUG: RPTR_ITEM: value=%llu key=%llu node=(%llu,%d) weight=%llu\n", 
           ptrCursor.mValue, ptrCursor.mKey, ptrCursor.mNodeRef, ptrCursor.mNodeIndex, weight);
    
    // Convert pointer to record
    GrapaCursor recCursor = ptrCursor;
    err = PtrToRec(ptrCursor, recCursor);
    if (err) {
        printf("DEBUG: PtrToRec failed for key=%llu (err=%lld)\n", key, (long long)err);
        return;
    }
    DumpGetItemWeight(recCursor, recWeight);
    printf("DEBUG: RREC: value=%llu key=%llu node=(%llu,%d) weight=%llu\n", 
           recCursor.mValue, recCursor.mKey, recCursor.mNodeRef, recCursor.mNodeIndex, recWeight);
}

void GrapaDBX::DebugPrintAllIndexPointers(u64 tableRef) {
    u64 indexRef = 0;
    GrapaCursor indexCursor;
    GrapaError err;
    
    indexCursor.Set(tableRef);
    err = GetTreeIndex(indexCursor, indexRef);
    if (err || indexRef == 0) {
        printf("DEBUG: Could not find index tree for tableRef=%llu\n", tableRef);
        return;
    }
    
    GrapaCursor ptrCursor;
    ptrCursor.Set(indexRef);
    err = First(ptrCursor);
    int count = 0;
    
    while (!err) {
        printf("DEBUG: Index entry: valueType=%d key=%llu value=%llu node=(%llu,%d)\n",
               ptrCursor.mValueType, ptrCursor.mKey, ptrCursor.mValue, ptrCursor.mNodeRef, ptrCursor.mNodeIndex);
        count++;
        err = Next(ptrCursor);
    }
    printf("DEBUG: Total index entries: %d\n", count);
}

// Debug utility methods
GrapaError GrapaDBX::DumpGetItemWeight(GrapaCursor& cursor, u64& weight)
{
    // For now, return a default weight of 1
    // This can be enhanced later to get actual weight from the cursor
    weight = 1;
    return(0);
}

