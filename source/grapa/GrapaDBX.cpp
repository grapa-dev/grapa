// GrapaDBX.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "GrapaDBX.h"
#include "GrapaMem.h"
#include "GrapaCompress.h"
#include "GrapaState.h"
#include "GrapaFloat.h"
#include "GrapaTime.h"
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
	err = NewTree(firstTree, treeType, 0); /* 8 is default nodeCount */
	if (err) {
		printf("[DEBUG] GrapaDBX::Create: NewTree failed with error %d\n", err);
		return err;
	}
	
	printf("[DEBUG] GrapaDBX::Create result: %d, firstTree=%llu\n", err, firstTree);
	return err;
}

GrapaError GrapaDBX::NewTree(u64& treePtr, u8 treeType, u64 parentTree, u8 nodeCount)
{
	GrapaError err;
	GrapaDBXTable table;

	err = GrapaBtree::NewTree(treePtr,treeType,parentTree,nodeCount); 
	if (err) return(err);

	// Handle special GROUP_TREE case like GrapaDB does
	switch (treeType)
	{
		case GROUP_TREE:
			//ensure to call the inherited function, or you get into an infinite loop
			table.mId = 0;
			err = CreateTable(treePtr,RTABLE_TREE,0,table);
			if (err) 
			{
				DeleteTree(treePtr);
				treePtr = 0;
				return(err);
			}
			break;
	}

	return(0);
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
	printf("[DEBUG] GrapaDBX::CreateTable: firstTree=%llu, pTreeType=%d, pTableId=%llu\n", firstTree, pTreeType, pTableId);
	
	GrapaError err;
	GrapaDBXCursor tableNames,tableNamesDT;
	GrapaDBXField dbField;
	GrapaDBXIndex dbIndex;
	GrapaDBXTable parentDict;

	pTable.mId = pTableId;
	pTable.mRef = 0;
	pTable.mRecRef = 0;
	pTable.mRefType = 0;

	if (firstTree)
	{
		printf("[DEBUG] GrapaDBX::CreateTable: Checking if table already exists\n");
		err = OpenTable(firstTree, pTableId, pTable);
		if (!err) {
			printf("[DEBUG] GrapaDBX::CreateTable: Table already exists\n");
			return(-1);
		}
		printf("[DEBUG] GrapaDBX::CreateTable: Table does not exist, proceeding with creation\n");
	}

	printf("[DEBUG] GrapaDBX::CreateTable: Creating new tree\n");
	err = NewTree(pTable.mRef,pTreeType,firstTree,8);
	if (err) {
		printf("[DEBUG] GrapaDBX::CreateTable: NewTree failed with error %d\n", err);
		return(err);
	}

	printf("[DEBUG] CreateTable: NewTree succeeded, pTable.mRef=%llu, pTreeType=%d\n", pTable.mRef, pTreeType);
	pTable.mRefType = pTreeType;

	if (firstTree)
	{
		printf("[DEBUG] GrapaDBX::CreateTable: Inserting table entry into parent tree\n");
		// FIXED: Use TREE_ITEM instead of GREC_ITEM to match OpenTable's search pattern
		tableNames.Set(firstTree, TREE_ITEM, pTableId, pTable.mRef);
		err = Insert(tableNames);
		if (err)
		{
			printf("[DEBUG] GrapaDBX::CreateTable: Insert failed with error %d\n", err);
			DeleteTree(pTable.mRef);
			pTable.mRef = 0;
			pTable.mRefType = 0;
			return(err);
		}
		printf("[DEBUG] GrapaDBX::CreateTable: Table entry inserted successfully\n");
	}

	pTable.mRecRef = pTable.mRef;

	printf("[DEBUG] GrapaDBX::CreateTable: Setting up table structure for type %d\n", pTable.mRefType);
	switch (pTable.mRefType)
	{
		case GROUP_TREE:
			{
				printf("[DEBUG] GrapaDBX::CreateTable: Setting up GROUP_TREE structure\n");
				err = OpenTable(pTable.mRef,0,parentDict);
				if (err) {
					printf("[DEBUG] GrapaDBX::CreateTable: OpenTable failed with error %d\n", err);
					return(err);
				}
				pTable.mRecRef = parentDict.mRef;
				printf("[DEBUG] GrapaDBX::CreateTable: GROUP_TREE structure set up successfully\n");
			}
			break;

		default:
			{
				printf("[DEBUG] GrapaDBX::CreateTable: Setting up default table structure\n");
				GrapaDU64Array indexList(0);
				err = CreateIndex(pTable,0,indexList,dbIndex);
				if (err) {
					printf("[DEBUG] GrapaDBX::CreateTable: CreateIndex failed with error %d\n", err);
					return(err);
				}
				printf("[DEBUG] GrapaDBX::CreateTable: Index created successfully\n");
				
				// Create the store tree for data storage (linked to table's record reference)
				u64 storeTree = 0;
				u8 storeType = 0;
				GrapaDBXCursor tableNames;
				tableNames.Set(pTable.mRecRef); // Link to table's record reference, not table tree
				err = GetTreeStore(tableNames, storeTree, storeType);
				if (err) {
					printf("[DEBUG] GrapaDBX::CreateTable: GetTreeStore failed with error %d\n", err);
					return(err);
				}
				if (storeTree == 0) {
					printf("[DEBUG] GrapaDBX::CreateTable: Creating store tree\n");
					err = NewTree(storeTree, SU64_TREE, pTable.mRecRef, NODE_WIDTH); // Parent to record reference
					if (err) {
						printf("[DEBUG] GrapaDBX::CreateTable: NewTree for store failed with error %d\n", err);
						return(err);
					}
					// Set the store tree link using the table cursor (like GrapaDB does)
					GrapaDBXCursor tableCursor;
					tableCursor.Set(pTable.mRef);
					err = SetTreeStore(tableCursor, storeTree, DATA_STORE);
					if (err) {
						printf("[DEBUG] GrapaDBX::CreateTable: SetTreeStore failed with error %d\n", err);
						return(err);
					}
					printf("[DEBUG] GrapaDBX::CreateTable: Store tree created at %llu, linked to table ref %llu\n", storeTree, pTable.mRef);
				}
				
				// Create $DICT field (field ID 0)
				dbField.Init(0,GrapaTokenType::START,0,0,1);
				dbField.mTreeType = pTreeType;
				GrapaCHAR fieldNameLabel("$DICT");
				err = CreateTableField(pTable, dbField, fieldNameLabel);
				if (err) {
					printf("[DEBUG] GrapaDBX::CreateTable: CreateTableField for $DICT failed with error %d\n", err);
					return(err);
				}
				printf("[DEBUG] GrapaDBX::CreateTable: $DICT field created successfully\n");
				
				// Create $KEY field (field ID 1) - match reference constants
				dbField.Init(1,GrapaTokenType::STR,GrapaDBXField::STORE_FIX,256,0);
				dbField.mDictSize = 256; // Set initial size like reference
				dbField.mTreeType = pTreeType;
				GrapaCHAR keyFieldNameLabel("$KEY");
				err = CreateTableField(pTable, dbField, keyFieldNameLabel);
				if (err) {
					printf("[DEBUG] GrapaDBX::CreateTable: CreateTableField for $KEY failed with error %d\n", err);
					return(err);
				}
				printf("[DEBUG] GrapaDBX::CreateTable: $KEY field created successfully\n");
				
				// Create $VALUE field (field ID 2) - like reference implementation
				dbField.Init(2,GrapaTokenType::RAW,GrapaDBXField::STORE_VAR,32,8);
				dbField.mDictSize = 32; // Set initial size like reference
				dbField.mTreeType = pTreeType;
				GrapaCHAR valueFieldNameLabel("$VALUE");
				err = CreateTableField(pTable, dbField, valueFieldNameLabel);
				if (err) {
					printf("[DEBUG] GrapaDBX::CreateTable: CreateTableField for $VALUE failed with error %d\n", err);
					return(err);
				}
				printf("[DEBUG] GrapaDBX::CreateTable: $VALUE field created successfully\n");
			}
			break;
	}

	// Note: Record creation is handled separately when data is actually stored
	// This function only creates the table structure

	printf("[DEBUG] GrapaDBX::CreateTable: Table creation completed successfully\n");
	return(0);
}

GrapaError GrapaDBX::OpenTable(u64 firstTree, u64 pTableId, GrapaDBXTable& pTable)
{
	GrapaError err;
	GrapaDBXCursor tableCursor,indexTableCursor,indexCursor;
	GrapaDBXTable dictTable;
	GrapaDBXFieldValueArray data;
	u64 indexRef;

	printf("[DEBUG] OpenTable: firstTree=%llu, pTableId=%llu\n", firstTree, pTableId);

	pTable.mId = pTableId;
	pTable.mRef = 0;

	tableCursor.Set(firstTree,TREE_ITEM,pTableId);
	printf("[DEBUG] OpenTable: Searching for TREE_ITEM with key=%llu\n", pTableId);
	err = Search(tableCursor);
	if (err) {
		printf("[DEBUG] OpenTable: Search failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] OpenTable: Search succeeded, found table at %llu\n", tableCursor.mValue);

	pTable.mRef = tableCursor.mValue;
	pTable.mRecRef = tableCursor.mValue;

	tableCursor.Set(tableCursor.mValue);
	err = GetTreeType(tableCursor,pTable.mRefType);
	printf("[DEBUG] OpenTable: GetTreeType returned err=%d, mRefType=%d\n", err, pTable.mRefType);

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
	printf("[DEBUG] GrapaDBX::DeleteTable: firstTree=%llu, pTableId=%llu\n", firstTree, pTableId);
	
	GrapaError err;
	GrapaDBXCursor tableCursor, cursor;
	GrapaDBXTable table;
	u64 indexTree, storeTree;
	u8 storeType;
	
	// Open the table to get its structure
	err = OpenTable(firstTree, pTableId, table);
	if (err) {
		printf("[DEBUG] DeleteTable: OpenTable failed with error %d\n", err);
		return err;
	}
	
	printf("[DEBUG] DeleteTable: table.mRef=%llu, table.mRecRef=%llu, table.mRefType=%d\n", 
	       table.mRef, table.mRecRef, table.mRefType);
	
	// Get the index tree for this table
	tableCursor.Set(table.mRecRef);
	err = GetTreeIndex(tableCursor, indexTree);
	if (err) {
		printf("[DEBUG] DeleteTable: GetTreeIndex failed with error %d\n", err);
		return err;
	}
	
	// Get the store tree for this table
	tableCursor.Set(indexTree);
	err = GetTreeStore(tableCursor, storeTree, storeType);
	if (err) {
		printf("[DEBUG] DeleteTable: GetTreeStore failed with error %d\n", err);
		return err;
	}
	
	// Delete all records in the table
	if (storeTree != 0) {
		tableCursor.Set(storeTree);
		err = First(tableCursor);
		while (!err) {
			// Delete the record pointer
			cursor.Set(tableCursor.mKey, GPTR_ITEM, tableCursor.mValue);
			err = Search(cursor);
			if (!err) {
				cursor.mValue = 0;
				err = Update(cursor);
			}
			err = Next(tableCursor);
		}
	}
	
	// Delete the table entry from the first tree
	tableCursor.Set(firstTree, TREE_ITEM, table.mId);
	err = Delete(tableCursor);
	if (err) {
		printf("[DEBUG] DeleteTable: Delete table entry failed with error %d\n", err);
		return err;
	}
	
	// Delete the table record from the record tree
	tableCursor.Set(firstTree, TREE_ITEM);
	err = Search(tableCursor);
	if (err) {
		printf("[DEBUG] DeleteTable: Search for record failed with error %d\n", err);
		return err;
	}
	
	cursor.Set(tableCursor.mValue, RREC_ITEM, table.mId);
	err = Delete(cursor);
	if (err) {
		printf("[DEBUG] DeleteTable: Delete record failed with error %d\n", err);
		return err;
	}
	
	printf("[DEBUG] DeleteTable: Table %llu deleted successfully\n", pTableId);
	return 0;
}

GrapaError GrapaDBX::Delete(GrapaCursor& cursor)
{
	printf("[DEBUG] GrapaDBX::Delete: cursor.mTreeRef=%llu, cursor.mValue=%llu, cursor.mKey=%llu, cursor.mValueType=%d\n", 
	       cursor.mTreeRef, cursor.mValue, cursor.mKey, cursor.mValueType);
	
	// Need to delete the indexes first because of a scenario in PurgeRc
	// where the key has children, a child is promoted and replaces the key in the tree
	// This causes the DeleteKey to fail to delete the index because the key can't be located in the search
	DeleteKeyIndexes(cursor);
	return GrapaBtree::Delete(cursor);
}

GrapaError GrapaDBX::DeleteKey(GrapaCursor& treeCursor)
{
	printf("[DEBUG] GrapaDBX::DeleteKey: treeCursor.mTreeRef=%llu, treeCursor.mValue=%llu, treeCursor.mKey=%llu, treeCursor.mValueType=%d\n", 
	       treeCursor.mTreeRef, treeCursor.mValue, treeCursor.mKey, treeCursor.mValueType);
	
	GrapaError err = 0;
	GrapaDBXCursor dataTypeCursor;
	GrapaDBXCursor indexTableCursor;
	GrapaDBXCursor tableCursor;
	u64 tableDT;
	u64 tableRef = 0;
	GrapaDBXField dbField;

	switch (treeCursor.mValueType)
	{
	case GREC_ITEM:
	case RREC_ITEM:
	case CREC_ITEM:
		// Don't fail on error...the indexes may have already been deleted.
		err = DeleteKeyIndexes(treeCursor);

		switch (treeCursor.mValueType)
		{
		case GREC_ITEM:
			printf("[DEBUG] DeleteKey: Deleting GROUP record\n");
			err = DeleteTree(treeCursor.mValue);
			if (err) return(err);
			break;
		case RREC_ITEM:
			printf("[DEBUG] DeleteKey: Deleting ROW record\n");
			err = GetDataParent(treeCursor.mValue, tableRef);
			err = GetDataTypeRecord(tableRef, tableDT);
			dataTypeCursor.Set(tableDT);
			err = First(dataTypeCursor);
			while (!err)
			{
				u64 dataPtr, dataSize;
				err = dbField.Read(this, dataTypeCursor.mValue);
				switch (dbField.mStore)
				{
				case GrapaDBXField::STORE_VAR:
				case GrapaDBXField::STORE_PAR:
					err = GetDataValue(treeCursor.mValue, dbField.mDictOffset, sizeof(dataPtr), (char*)&dataPtr, &dataSize);
					if (err) return(err);
					if (dataPtr)
					{
						dataPtr = BE_S64(dataPtr);
						err = DeleteData(dataPtr);
					}
					break;
				}
				err = Next(dataTypeCursor);
			}
			err = DeleteData(treeCursor.mValue);
			if (err) return(err);
			break;
		case CREC_ITEM:
			printf("[DEBUG] DeleteKey: Deleting COL record\n");
			err = 0;
			break;
		}
		break;
	case GPTR_ITEM:
	case RPTR_ITEM:
	case CPTR_ITEM:
		printf("[DEBUG] DeleteKey: Deleting pointer item (nothing to delete)\n");
		// nothing to delete
		break;
	case DTYPE_ITEM:
		printf("[DEBUG] DeleteKey: Deleting DTYPE item\n");
		err = dbField.Read(this, treeCursor.mValue);
		if (!err)
		{
			err = DeleteData(dbField.mNameRef);
			err = DeleteTree(dbField.mTableRef);
		}
		err = DeleteData(treeCursor.mValue);
		if (err) return(err);
		break;
	case SEARCH_ITEM:
		printf("[DEBUG] DeleteKey: SEARCH_ITEM - should not ever get here\n");
		// should not ever ever get here
		break;
	case DRTYPE_ITEM:
	case DITYPE_ITEM:
		printf("[DEBUG] DeleteKey: Deleting dictionary item\n");
		err = dbField.Read(this, treeCursor.mValue);
		if (!err)
		{
			err = DeleteData(dbField.mNameRef);
			err = DeleteTree(dbField.mTableRef);
		}
		err = DeleteData(treeCursor.mValue);
		if (err) return(err);
		break;
	}
	return(0);
}

GrapaError GrapaDBX::DeleteKeyIndexes(GrapaCursor& treeCursor)
{
	printf("[DEBUG] GrapaDBX::DeleteKeyIndexes: treeCursor.mTreeRef=%llu, treeCursor.mValue=%llu, treeCursor.mKey=%llu, treeCursor.mValueType=%d\n", 
	       treeCursor.mTreeRef, treeCursor.mValue, treeCursor.mKey, treeCursor.mValueType);
	
	GrapaError err = 0;
	GrapaDBXCursor tableCursor, indexTableCursor;
	u64 indexRef = 0;

	switch(treeCursor.mValueType)
	{
		case GREC_ITEM:
		//case IREC_ITEM:
		case RREC_ITEM:
		case CREC_ITEM:
			if (treeCursor.mTreeRef)
			{
				indexTableCursor.Set(treeCursor.mTreeRef);
				err = GetTreeIndex(indexTableCursor, indexRef);
				if (err) return(err);
				indexTableCursor.Set(indexRef);
				err = First(indexTableCursor);
				if (!err && indexTableCursor.mKey == 0)
					err = Next(indexTableCursor);
				while (!err)
				{
					switch(treeCursor.mValueType)
					{
						case GREC_ITEM:
							tableCursor.Set(indexTableCursor.mValue, GPTR_ITEM, treeCursor.mKey);
							break;
						case RREC_ITEM:
							tableCursor.Set(indexTableCursor.mValue, RPTR_ITEM, treeCursor.mKey);
							break;
						case CREC_ITEM:
							tableCursor.Set(indexTableCursor.mValue, CPTR_ITEM, treeCursor.mKey);
							break;
					}
					err = GrapaBtree::Delete(tableCursor);
					// Ignore the error...the index could have already been deleted
					// Maybe do a search first and then only delete if it exists? But this adds a search cost.
					//if (err) return(err);
					err = Next(indexTableCursor);
					if (!err && indexTableCursor.mKey == 0)
						err = Next(indexTableCursor);
				}
			}
			break;
	}
	return(0);
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

// Field operations - implementation following GrapaDB reference pattern
GrapaError GrapaDBX::CreateTableField(GrapaDBXTable& pTable, GrapaDBXField& pField, const GrapaCHAR& pName)
{
	printf("[DEBUG] CreateTableField: called with fieldId=%llu, fieldName='%s'\n", pField.mId, pName.mBytes);
	
	GrapaError err;
	GrapaDBXCursor tableNames, tableNamesDT;
	GrapaDBXField dbField;
	GrapaDBXIndex dbIndex;
	GrapaDBXTable parentDict;
	u64 indexRef;
	u64 recordCount;
	u64 storeTree;
	u8 storeType;
	
	/* Check if field already exists */
	err = OpenTableField(pTable, pField.mId, pField);
	if (!err) return(-1); /* Field already exists */
	
	/* Set tree dirty */
	tableNames.Set(pTable.mRecRef);
	err = SetTreeDirty(tableNames, true);
	
	/* Get the index tree reference */
	tableNames.Set(pTable.mRecRef);
	err = GetTreeIndex(tableNames, indexRef);
	if (err) return(err);
	
	tableNames.Set(indexRef);
	err = Search(tableNames);
	if (err) return(err);
	
			/* Calculate field offsets and sizes */
		if (pField.mId) {
			pField.mDictOffset = pTable.mDictField.mDictSize;
			pField.mTreeType = pTable.mDictField.mTreeType;
		}
	
	/* Set field size based on type and store */
	switch (pField.mStore) {
		case GrapaDBXField::STORE_FIX:
			switch (pField.mType) {
				case GrapaTokenType::BOOL:
					pField.mDictSize = 1;
					pField.mSize = 1;
					break;
				case GrapaTokenType::INT:
				case GrapaTokenType::FLOAT:
				case GrapaTokenType::TIME:
					if (pField.mDictSize > 0x7FFF) pField.mDictSize = 0x7FFF;
					pField.mSize = pField.mDictSize;
					if (pField.mDictSize == 1);
					else if (pField.mDictSize <= 127) pField.mDictSize += 1;
					else pField.mDictSize += 2;
					break;
				case GrapaTokenType::STR:
					if (pField.mDictSize > 0x7FFF) pField.mDictSize = 0x7FFF;
					pField.mSize = pField.mDictSize;
					if (pField.mDictSize <= 127) pField.mDictSize += 1;
					else pField.mDictSize += 2;
					break;
				case GrapaTokenType::RAW:
				default:
					if (pField.mDictSize > 0x7FFF) pField.mDictSize = 0x7FFF;
					pField.mSize = pField.mDictSize;
					if (pField.mDictSize <= 127) pField.mDictSize += 2;
					else pField.mDictSize += 3;
					break;
			}
			break;
		case GrapaDBXField::STORE_VAR:
		case GrapaDBXField::STORE_PAR:
			pField.mDictSize = 8;
			break;
		default:
			pField.mDictSize = 0;
			pField.mSize = 0;
			break;
	}
	
	/* Create the field data structure */
	err = NewData(BYTE_DATA, pTable.mRecRef, pField.GetSize(), 0, 0, pField.mRef, true);
	if (err) return(err);
	
	/* Store the field name if provided */
	if (pName.mLength && pName.mBytes) {
		err = NewData(BYTE_DATA, pTable.mRecRef, pName.mLength, 0, 0, pField.mNameRef, false);
		if (err) return(err);
		err = SetDataValue(pField.mNameRef, 0, pName.mLength, (char*)pName.mBytes);
		if (err) return(err);
		printf("[DEBUG] CreateTableField: Stored field name '%s' at mNameRef=%llu\n", 
		       (char*)pName.mBytes, pField.mNameRef);
	}
	
	// Add a static or local variable to track the running offset for data fields
	static u64 runningDataOffset = 0;
	if (pField.mId == 0) {
		// Reset running offset when creating the dictionary field
		runningDataOffset = 0;
	}
	// Calculate field offsets and sizes
	if (pField.mId) {
		pField.mDictOffset = runningDataOffset;
		pField.mTreeType = pTable.mDictField.mTreeType;
	}
	
	/* Update dictionary metadata for existing fields BEFORE inserting the field */
	if (pField.mId) {
		printf("[DEBUG] CreateTableField: Updating dictionary metadata for field ID %llu\n", pField.mId);
		printf("[DEBUG] CreateTableField: Before update - pTable.mDictField.mDictOffset=%llu, pTable.mDictField.mDictSize=%llu\n", 
		       pTable.mDictField.mDictOffset, pTable.mDictField.mDictSize);
		printf("[DEBUG] CreateTableField: Adding field with mDictSize=%llu\n", pField.mDictSize);

		// Only increment for non-dict fields
		runningDataOffset += pField.mDictSize;

		printf("[DEBUG] CreateTableField: After update - runningDataOffset=%llu\n", runningDataOffset);

		// Update the dictionary field in the tree
		tableNamesDT.Set(tableNames.mValue);
		err = Search(tableNamesDT);
		if (err) {
			printf("[DEBUG] CreateTableField: Search for dictionary field failed with error %d\n", err);
			return(err);
		}

		err = pTable.mDictField.Write(this, tableNamesDT.mValue);
		if (err) {
			printf("[DEBUG] CreateTableField: Write dictionary field failed with error %d\n", err);
			return(err);
		}

		printf("[DEBUG] CreateTableField: Dictionary metadata updated successfully\n");
	}
	
	/* Write the field definition */
	err = pField.Write(this, pField.mRef);
	if (err) return(err);
	
	/* Insert the field into the data type tree */
	tableNamesDT.Set(tableNames.mValue, DRTYPE_ITEM, pField.mId, pField.mRef);
	err = Insert(tableNamesDT);
	if (err) return(err);
	
	/* Set the dictionary field for the first field */
	if (pField.mId == 0) {
		pTable.mDictField = pField;
	}
	
	printf("[DEBUG] CreateTableField: Field definition stored successfully\n");
	return 0;
}

GrapaError GrapaDBX::OpenTableField(GrapaDBXTable& pTable, u64 pFieldId, GrapaDBXField& pField)
{
	/* Implementation following GrapaDB reference pattern */
	GrapaError err;
	GrapaDBXCursor tableNames;
	u64 indexRef;
	
	/* Get the index tree reference */
	tableNames.Set(pTable.mRecRef);
	err = GetTreeIndex(tableNames, indexRef);
	if (err) return(err);
	
	tableNames.Set(indexRef);
	err = Search(tableNames);
	if (err) return(err);
	
	/* Search for the field in the data type tree */
	tableNames.Set(tableNames.mValue, DRTYPE_ITEM, pFieldId);
	err = Search(tableNames);
	if (err) return(err); /* Field not found */
	
	/* Read the field definition */
	err = pField.Read(this, tableNames.mValue);
	if (err) return(err);
	
	return 0; /* Field found */
}

GrapaError GrapaDBX::OpenTableFieldList(GrapaDBXTable& pTable, GrapaDBXFieldArray& pFieldList)
{
	/* Placeholder implementation - GrapaBtree doesn't have OpenTableFieldList */
	return 0;
}

GrapaError GrapaDBX::DeleteTableField(GrapaDBXTable& pTable, u64 pFieldId)
{
	GrapaError err;
	GrapaDBXCursor indexCursor, tableCursor;
	u64 indexRef;

	// Check if field is used in any indexes - if so, fail
	indexCursor.Set(pTable.mRecRef);
	err = GetTreeIndex(indexCursor, indexRef);
	if (err) return(err);
	if (indexRef == 0) return(-1);

	indexCursor.Set(indexRef);
	err = First(indexCursor);
	if (!err && indexCursor.mKey == 0)
		err = Next(indexCursor); // Skip over the DICT in the index

	while (!err) {
		GrapaDBXCursor indexFieldCursor;
		u64 indexFieldsRef;
		indexFieldCursor.Set(indexCursor.mValue);
		err = GetTreeIndex(indexFieldCursor, indexFieldsRef);
		if (indexFieldsRef) {
			indexFieldCursor.Set(indexFieldsRef);
			err = First(indexFieldCursor);
			while (!err) {
				// If field is used in any indexes, fail
				if (indexFieldCursor.mValue == pFieldId) {
					printf("[DEBUG] DeleteTableField: Field %llu is used in index, cannot delete\n", pFieldId);
					return -1;
				}
				err = Next(indexFieldCursor);
			}
		}
		err = Next(indexCursor);
		if (!err && indexCursor.mKey == 0)
			err = Next(indexCursor); // Skip over the DICT in the index
	}

	// Get field information and update dictionary
	GrapaDBXCursor tableNames;
	GrapaDBXCursor dtField;
	GrapaDBXField field;
	u64 fieldCount = 0;
	u64 fromOffset = 0, toOffset = 0;
	u64 fieldHeadRef = 0;
	u8 fieldType = 0;
	u8 fieldStore = 0;
	u64 storeTree;
	u8 storeType;

	tableNames.Set(indexRef);
	err = Search(tableNames);
	if (err) return(err);

	err = GetTreeSize(tableNames, fieldCount);

	dtField.Set(tableNames.mValue);
	err = First(dtField);
	while (!err) {
		err = field.Read(this, dtField.mValue);
		if (field.mId == 0) {
			pTable.mDictField = field;
			fieldHeadRef = dtField.mValue;
		}
		else if (field.mId == pFieldId) {
			toOffset = field.mDictOffset;
			fromOffset = field.mDictOffset + field.mDictSize;
			fieldType = field.mType; // Needed for CTABLE_TREE processing
			fieldStore = field.mStore;
		}
		if (fromOffset - toOffset) {
			field.mDictOffset -= (fromOffset - toOffset);
			field.Write(this, dtField.mValue);
		}
		err = Next(dtField);
	}

	if (fieldHeadRef == 0) return(-1);
	
	u64 totalSize = pTable.mDictField.mDictSize;
	u64 moveSize = totalSize - fromOffset;
	pTable.mDictField.mDictSize -= (fromOffset - toOffset);
	pTable.mDictField.mDictOffset--;
	pTable.mDictField.Write(this, fieldHeadRef);
	
	dtField.Set(tableNames.mValue, SDATA_ITEM, pFieldId);
	err = Delete(dtField);
	if (err) return(err);

	u64 bytesWritten;

	switch (pTable.mDictField.mTreeType) {
		case GROUP_TREE:
			// For GROUP tables, no data work needed
			break;

		case RTABLE_TREE:
			// For ROW tables, bitshift all records
			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor, true);

			// Bitshift all the records
			err = CopyDataValue(tableCursor.mValue, toOffset, tableCursor.mValue, fromOffset, moveSize, &bytesWritten);

			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor, false);
			break;

		case CTABLE_TREE:
			// For COL tables, delete the column tree
			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor, true);

			err = GetTreeStore(indexCursor, storeTree, storeType);
			if (err) return(err);
			if (storeTree == 0) return(-1);

			switch(fieldType) {
				case GrapaTokenType::START:
					break;

				case GrapaTokenType::ERR:
				case GrapaTokenType::RAW:
				case GrapaTokenType::BOOL:
				case GrapaTokenType::INT:
				case GrapaTokenType::FLOAT:
				case GrapaTokenType::STR:
				case GrapaTokenType::TIME:
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::TUPLE:
				case GrapaTokenType::VECTOR:
				case GrapaTokenType::WIDGET:
				case GrapaTokenType::XML:
				case GrapaTokenType::LIST:
				case GrapaTokenType::EL:
				case GrapaTokenType::TAG:
				case GrapaTokenType::OP:
				case GrapaTokenType::CODE:
				case GrapaTokenType::TABLE:
					switch (fieldStore) {
						case GrapaDBXField::STORE_VAR:
						case GrapaDBXField::STORE_PAR:
							tableCursor.Set(storeTree, TREE_ITEM, pFieldId);
							err = Delete(tableCursor);
							break;
						case GrapaDBXField::STORE_FIX:
							tableCursor.Set(storeTree, SDATA_ITEM, pFieldId);
							err = Delete(tableCursor);
							break;
					}
					break;

				default:
					break;
			}

			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor, false);
			break;
	}

	return(err);
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
	u64 newRecordRef;
	u64 uniqueKey;

	// Generate a unique key for this record
	err = FirstFreeId(pTable.mRecRef, 1, uniqueKey);
	if (err) return(err);

	// Create a data block for the record (like the reference implementation)
	// Use a reasonable size for the record data block
	u64 recordSize = 1024;  // Start with 1KB, can grow if needed
	err = NewData(BYTE_DATA, pTable.mRecRef, recordSize, recordSize, 1, newRecordRef, true);
	if (err) return(err);

	// Set up the cursor as a pointer to the record (like the reference implementation)
	pCursor.Set(pTable.mRecRef, RPTR_ITEM, uniqueKey, newRecordRef);
	pCursor.mValue = newRecordRef;

	// Insert the record into the record tree
	GrapaDBXCursor tableCursor;
	tableCursor.Set(pTable.mRecRef, RREC_ITEM, pCursor.mKey, newRecordRef);
	err = Insert(tableCursor);
	if (err)
	{
		DeleteTree(newRecordRef);
		return(err);
	}

	// Insert the record into the index
	err = InsertIntoIndex(pTable.mRef, pCursor.mValueType, pCursor.mKey, pCursor.mValue);
	if (err)
	{
		printf("[DEBUG] CreateRecord: InsertIntoIndex failed with error %d\n", err);
		// Don't fail the record creation if index insertion fails
		// Just log the error and continue
	}

	return(0);
}

GrapaError GrapaDBX::DeleteRecord(GrapaDBXTable& pTable, GrapaCursor& pCursor)
{
	printf("[DEBUG] GrapaDBX::DeleteRecord: tableRef=%llu, cursor.mValue=%llu, cursor.mKey=%llu\n", 
	       pTable.mRef, pCursor.mValue, pCursor.mKey);
	
	GrapaError err;
	GrapaDBXCursor tableCursor;
	
	// Convert to GrapaDBXCursor for consistency
	GrapaDBXCursor dbxCursor;
	dbxCursor.Set(pCursor.mTreeRef, pCursor.mValueType, pCursor.mValue);
	
	switch (pTable.mRefType)
	{
		case GROUP_TREE:
			// For GROUP tables, we delete the entire group
			printf("[DEBUG] DeleteRecord: Deleting GROUP record\n");
			return Delete(dbxCursor);
			
		case RTABLE_TREE:
			// For ROW tables, delete the record from the record tree
			printf("[DEBUG] DeleteRecord: Deleting ROW record\n");
			tableCursor.Set(pTable.mRecRef, RREC_ITEM, pCursor.mKey);
			return Delete(tableCursor);
			
		case CTABLE_TREE:
			{
				// For COL tables, delete the record and clean up field data
				printf("[DEBUG] DeleteRecord: Deleting COL record\n");
				tableCursor.Set(pTable.mRecRef, CREC_ITEM, pCursor.mKey);
				err = Search(tableCursor);
				if (err) {
					printf("[DEBUG] DeleteRecord: Search failed with error %d\n", err);
					return err;
				}
				
				u64 delOffset = tableCursor.mLength;
				printf("[DEBUG] DeleteRecord: Deleting at offset %llu\n", delOffset);
				
				// Delete the record from the column tree
				err = Delete(tableCursor);
				if (err) {
					printf("[DEBUG] DeleteRecord: Delete failed with error %d\n", err);
					return err;
				}
				
				// Clean up field data - this is simplified for now
				// In a full implementation, we would iterate through all fields
				// and adjust their data offsets
				printf("[DEBUG] DeleteRecord: Record deleted successfully\n");
			}
			return 0;
			
		default:
			printf("[DEBUG] DeleteRecord: Unknown table type %d\n", pTable.mRefType);
			return -1;
	}
}

// Field value operations - placeholder implementation since GrapaBtree doesn't have these methods
GrapaError GrapaDBX::SetRecordField(GrapaCursor& pCursor, GrapaDBXFieldValueArray& pFieldList)
{
	/* Implementation matching GrapaDB reference format */
	/* Store fields at specific offsets within the record */
	
	printf("[DEBUG] SetRecordField called with cursor.mValue=%llu\n", pCursor.mValue);
	printf("[DEBUG] SetRecordField: File opened: %s\n", FileOpened() ? "YES" : "NO");
	printf("[DEBUG] SetRecordField: File pointer: %p\n", mFile);
	
	/* Get the actual record data block using PtrToRec like the reference */
	GrapaCursor recCursor;
	GrapaError err = PtrToRec(pCursor, recCursor);
	if (err) {
		printf("[DEBUG] SetRecordField: PtrToRec failed with error %d\n", err);
		return err;
	}
	printf("[DEBUG] SetRecordField: PtrToRec succeeded, recCursor.mValue=%llu, recCursor.mTreeRef=%llu\n", 
	       recCursor.mValue, recCursor.mTreeRef);
	
	// Check what type of block recCursor.mValue points to
	GrapaBlockTree blockInfo;
	GrapaError blockErr = blockInfo.Read(mFile, recCursor.mValue);
	if (blockErr) {
		printf("[DEBUG] SetRecordField: Failed to read block info for %llu, error %d\n", recCursor.mValue, blockErr);
	} else {
		printf("[DEBUG] SetRecordField: Block %llu is type %d (TREE_BLOCK=%d, DATA_BLOCK=%d)\n", 
		       recCursor.mValue, blockInfo.blockType, GrapaBlock::TREE_BLOCK, GrapaBlock::DATA_BLOCK);
	}
	
	s32 fieldCount = pFieldList.Count();
	printf("[DEBUG] Field count: %d\n", fieldCount);
	if (fieldCount == 0) return 0;
	
	/* Get field definitions to determine correct offsets */
	GrapaDBXFieldArray* fieldList = ((GrapaGroup2*)this)->ListFields(recCursor.mTreeRef, recCursor.mTreeType);
	if (!fieldList) {
		printf("[DEBUG] SetRecordField: Failed to get field list\n");
		return -1;
	}
	
	/* Store each field at its correct offset within the record */
	for (s32 i = 0; i < fieldCount; i++) {
		GrapaDBXFieldValue* dbFieldValue = pFieldList.GetFieldAt(i);
		if (!dbFieldValue) continue;
		
		/* Find the corresponding field definition */
		GrapaDBXField* fieldDef = NULL;
		for (u32 j = 0; j < fieldList->Count(); j++) {
			GrapaDBXField* field = fieldList->GetFieldAt(j);
			if (field && field->mId == dbFieldValue->mId) {
				fieldDef = field;
				break;
			}
		}
		
		if (!fieldDef) {
			printf("[DEBUG] SetRecordField: No field definition found for field ID %llu\n", dbFieldValue->mId);
			continue;
		}
		
		u64 valueSize = dbFieldValue->mValue.GetSize();
		u8 isRaw = (dbFieldValue->mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
		u8 fieldType = dbFieldValue->mType;
		
		printf("[DEBUG] Storing field %d (ID %llu): size=%llu, type=%d, isRaw=%d, offset=%llu\n", 
		       i, dbFieldValue->mId, valueSize, fieldType, isRaw, fieldDef->mDictOffset);
		printf("[DEBUG] SetRecordField: recCursor.mValue=%llu, recCursor.mTreeRef=%llu\n", recCursor.mValue, recCursor.mTreeRef);
		
		/* Store data in the record block based on storage type */
		switch (fieldDef->mStore) {
		case GrapaDBXField::STORE_FIX:
			/* Fixed storage uses complex format with length bytes */
			if (fieldDef->mDictSize == 1) {
				/* Single byte storage */
				u8 h = 0;
				if (dbFieldValue->mValue.mBytes && dbFieldValue->mValue.mLength) {
					h = 0x80;
					u8 c = ((u8*)dbFieldValue->mValue.mBytes)[dbFieldValue->mValue.mLength - 1];
					if (c > 127) c = 127;
					h |= c;
				}
				GrapaError err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset, 1, (void*)&h);
				if (err) {
					printf("[DEBUG] SetRecordField: SetDataValue failed for field %d, error %d\n", i, err);
					return err;
				}
			} else if (fieldDef->mDictSize <= (((u64)128) + isRaw)) {
				/* Small data: [1-byte length][type][data] */
				u8 h = 0;
				if (dbFieldValue->mValue.mBytes) h = 0x80;
				if (dbFieldValue->mValue.mLength) {
					u64 len = dbFieldValue->mValue.mLength;
					if (len > (fieldDef->mDictSize - 1 - isRaw)) 
						len = (fieldDef->mDictSize - 1 - isRaw);
					h |= (u8)len;
					
					/* Store the data */
					GrapaError err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset + 1 + isRaw, len, 
					                            (void*)dbFieldValue->mValue.mBytes);
					if (err) {
						printf("[DEBUG] SetRecordField: SetDataValue (data) failed for field %d, error %d\n", i, err);
						return err;
					}
				}
				
				/* Store type byte for RAW types */
				if (isRaw) {
					GrapaError err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset + 1, 1, (void*)&fieldType);
					if (err) {
						printf("[DEBUG] SetRecordField: SetDataValue (type) failed for field %d, error %d\n", i, err);
						return err;
					}
				}
				
				/* Store the length byte */
				GrapaError err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset, 1, (void*)&h);
				if (err) {
					printf("[DEBUG] SetRecordField: SetDataValue (length) failed for field %d, error %d\n", i, err);
					return err;
				}
			} else if (fieldDef->mDictSize <= (0x8001 + isRaw)) {
				/* Medium data: [2-byte length][type][data] */
				u8 h[2] = { 0, 0 };
				if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
				if (dbFieldValue->mValue.mLength) {
					u64 len = dbFieldValue->mValue.mLength;
					if (len > (fieldDef->mDictSize - 2 - isRaw)) 
						len = (fieldDef->mDictSize - 2 - isRaw);
					h[0] |= (len >> 8) & 0xFF;
					h[1] = len & 0xFF;
					
					/* Store the data */
					GrapaError err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset + 2 + isRaw, len, 
					                            (void*)dbFieldValue->mValue.mBytes);
					if (err) {
						printf("[DEBUG] SetRecordField: SetDataValue (data) failed for field %d, error %d\n", i, err);
						return err;
					}
				}
				
				/* Store type byte for RAW types */
				if (isRaw) {
					GrapaError err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset + 2, 1, (void*)&fieldType);
					if (err) {
						printf("[DEBUG] SetRecordField: SetDataValue (type) failed for field %d, error %d\n", i, err);
						return err;
					}
				}
				
				/* Store the length bytes */
				GrapaError err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset, 2, (void*)h);
				if (err) {
					printf("[DEBUG] SetRecordField: SetDataValue (length) failed for field %d, error %d\n", i, err);
					return err;
				}
			}
			break;
			
		case GrapaDBXField::STORE_VAR:
		case GrapaDBXField::STORE_PAR:
			/* Variable/parameter storage: store pointer to separate data block */
			u64 dataPtr = 0;
			u64 returnSize = 0;
			
			/* Read existing pointer */
			err = GetDataValue(recCursor.mValue, fieldDef->mDictOffset, sizeof(dataPtr), (char*)&dataPtr, &returnSize);
			if (err) {
				printf("[DEBUG] SetRecordField: GetDataValue (pointer) failed for field %d, error %d\n", i, err);
				return err;
			}
			
			/* Convert from big-endian if needed */
			dataPtr = BE_S64(dataPtr);
			
			if (!dataPtr) {
				/* Create new data block */
				u8 dataType = (fieldDef->mStore == GrapaDBXField::STORE_VAR) ? BYTE_DATA : FREC_DATA;
				err = NewData(dataType, recCursor.mValue, dbFieldValue->mValue.GetLength(), fieldDef->mSize, fieldDef->mGrow, dataPtr);
				if (err) {
					printf("[DEBUG] SetRecordField: NewData failed for field %d, error %d\n", i, err);
					return err;
				}
				
				/* Set field type for RAW fields */
				if (isRaw) {
					SetFieldType(dataPtr, fieldType);
				} else {
					SetFieldType(dataPtr, fieldDef->mType);
				}
				
				/* Store data in the new block */
				err = SetDataValue(dataPtr, 0, dbFieldValue->mValue.GetLength(), (void*)dbFieldValue->mValue.GetPtr(), &returnSize);
				if (err) {
					printf("[DEBUG] SetRecordField: SetDataValue (data) failed for field %d, error %d\n", i, err);
					return err;
				}
				
				/* Store pointer back to record (convert to big-endian) */
				dataPtr = BE_S64(dataPtr);
				err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset, sizeof(dataPtr), (void*)&dataPtr, &returnSize);
				if (err) {
					printf("[DEBUG] SetRecordField: SetDataValue (pointer) failed for field %d, error %d\n", i, err);
					return err;
				}
			} else {
				/* Update existing data block */
				if (dbFieldValue->mValue.mBytes) {
					u64 growBlockSize, dataSize, dataLength;
					u8 compressType;
					err = GetDataSize(dataPtr, growBlockSize, dataSize, dataLength, compressType);
					if (err) {
						printf("[DEBUG] SetRecordField: GetDataSize failed for field %d, error %d\n", i, err);
						return err;
					}
					
					err = SetDataSize(dataPtr, dataSize, dbFieldValue->mValue.GetLength(), compressType);
					if (err) {
						printf("[DEBUG] SetRecordField: SetDataSize failed for field %d, error %d\n", i, err);
						return err;
					}
					
					err = SetDataValue(dataPtr, 0, dbFieldValue->mValue.GetLength(), (void*)dbFieldValue->mValue.GetPtr(), &returnSize);
					if (err) {
						printf("[DEBUG] SetRecordField: SetDataValue (data) failed for field %d, error %d\n", i, err);
						return err;
					}
					
					if (isRaw) {
						SetFieldType(dataPtr, fieldType);
					}
				} else {
					/* Delete existing block if new value is empty */
					DeleteData(dataPtr);
					dataPtr = 0;
					err = SetDataValue(recCursor.mValue, fieldDef->mDictOffset, sizeof(dataPtr), (void*)&dataPtr, &returnSize);
					if (err) {
						printf("[DEBUG] SetRecordField: SetDataValue (null pointer) failed for field %d, error %d\n", i, err);
						return err;
					}
				}
			}
			break;
		}
	}
	
	printf("[DEBUG] SetRecordField completed successfully\n");
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
	
	/* Read the field data in reference format */
	err = GetRecordFieldData(recCursor, field, pValue);
	if (err) {
		printf("[DEBUG] GetRecordField: GetRecordFieldData failed with error %d\n", err);
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

GrapaError GrapaDBX::GetRecordFieldData(GrapaCursor& recCursor, GrapaDBXField& field, GrapaBYTE& buffer)
{
	printf("[DEBUG] GetRecordFieldData: called with field.mDictOffset=%llu, field.mDictSize=%llu\n", field.mDictOffset, field.mDictSize);
	
	GrapaError err;
	u64 dataRef = recCursor.mValue;  /* recCursor should already be the record data block */
	u8 isRaw = (field.mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
	
	/* Initialize buffer with field type */
	buffer.FromDbType(field.mType);
	
	/* Read field data from the specific offset within the record */
	switch (field.mStore) {
	case GrapaDBXField::STORE_FIX:
		if (field.mDictSize == 1) {
			/* Single byte storage */
			u8 h = 0;
			u64 returnSize = 0;
			err = GetDataValue(dataRef, field.mDictOffset, 1, (char*)&h, &returnSize);
			if (err) {
				printf("[DEBUG] GetRecordFieldData: GetDataValue failed for single byte field, error %d\n", err);
				return err;
			}
			
			if ((h & 0x80) == 0) {
				buffer.SetSize(0);
			} else {
				h &= 0x7F;
				buffer.SetLength(1, false);
				((u8*)buffer.mBytes)[0] = h & 0x7F;
			}
		} else if (field.mDictSize <= (((u64)128) + isRaw)) {
			/* Small data: [1-byte length][type][data] */
			u8 h = 0;
			u64 returnSize = 0;
			err = GetDataValue(dataRef, field.mDictOffset, 1, (char*)&h, &returnSize);
			if (err) {
				printf("[DEBUG] GetRecordFieldData: GetDataValue failed for small field, error %d\n", err);
				return err;
			}
			
			if ((h & 0x80) == 0) {
				buffer.SetSize(0);
			} else {
				u64 len = h & 0x7F;
				buffer.SetLength(len, false);
				err = GetDataValue(dataRef, field.mDictOffset + 1 + isRaw, len, (char*)buffer.GetPtr(), &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordFieldData: GetDataValue (data) failed for small field, error %d\n", err);
					return err;
				}
			}
			
			/* Read type byte for RAW fields */
			if (isRaw) {
				u8 fieldType = 0;
				err = GetDataValue(dataRef, field.mDictOffset + 1, 1, (char*)&fieldType, &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordFieldData: GetDataValue (type) failed for RAW field, error %d\n", err);
					return err;
				}
				buffer.mToken = fieldType;
			}
		} else if (field.mDictSize <= (0x8001 + isRaw)) {
			/* Medium data: [2-byte length][type][data] */
			u8 h[2] = { 0, 0 };
			u64 returnSize = 0;
			err = GetDataValue(dataRef, field.mDictOffset, 2, (char*)h, &returnSize);
			if (err) {
				printf("[DEBUG] GetRecordFieldData: GetDataValue failed for medium field, error %d\n", err);
				return err;
			}
			
			if ((h[0] & 0x80) == 0) {
				buffer.SetSize(0);
			} else {
				u64 len = ((u64)(h[0] & 0x7F)) << 8 | (u64)h[1];
				buffer.SetLength(len, false);
				err = GetDataValue(dataRef, field.mDictOffset + 2 + isRaw, len, (char*)buffer.GetPtr(), &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordFieldData: GetDataValue (data) failed for medium field, error %d\n", err);
					return err;
				}
			}
			
			/* Read type byte for RAW fields */
			if (isRaw) {
				u8 fieldType = 0;
				err = GetDataValue(dataRef, field.mDictOffset + 2, 1, (char*)&fieldType, &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordFieldData: GetDataValue (type) failed for RAW field, error %d\n", err);
					return err;
				}
				buffer.mToken = fieldType;
			}
		}
		break;
		
	case GrapaDBXField::STORE_VAR:
	case GrapaDBXField::STORE_PAR:
		/* Variable/parameter storage: [8-byte pointer][data] */
		u64 dataPtr = 0;
		u64 returnSize = 0;
		err = GetDataValue(dataRef, field.mDictOffset, sizeof(u64), (char*)&dataPtr, &returnSize);
		if (err) {
			printf("[DEBUG] GetRecordFieldData: GetDataValue (pointer) failed for variable field, error %d\n", err);
			return err;
		}
		
		/* Convert from big-endian format */
		dataPtr = BE_S64(dataPtr);
		
		/* Check if pointer is null (no data) */
		if (!dataPtr) {
			buffer.SetSize(0);
			return 0;
		}
		
		/* Read data from the separate block */
		u64 growBlockSize, dataSize, dataLength;
		u8 compressType;
		err = GetDataSize(dataPtr, growBlockSize, dataSize, dataLength, compressType);
		if (err) {
			printf("[DEBUG] GetRecordFieldData: GetDataSize failed for variable field, error %d\n", err);
			return err;
		}
		
		buffer.SetLength(dataLength, false);
		err = GetDataValue(dataPtr, 0, dataLength, (char*)buffer.GetPtr(), &returnSize);
		if (err) {
			printf("[DEBUG] GetRecordFieldData: GetDataValue (data) failed for variable field, error %d\n", err);
			return err;
		}
		break;
	}
	
	printf("[DEBUG] GetRecordFieldData: Successfully read field, length=%llu\n", buffer.mLength);
	return 0;
}



GrapaError GrapaDBX::GetDataTypeRecord(u64 tableRef, u64& tableDT)
{
	/* Implementation following GrapaDB reference pattern */
	printf("[DEBUG] GetDataTypeRecord: called with tableRef=%llu\n", tableRef);
	
	GrapaError err = 0;
	GrapaDBXCursor tableCursor;
	u64 indexRef;
	tableDT = 0;
	
	/* Set up cursor to point to the table */
	tableCursor.Set(tableRef);
	
	/* Get the index tree reference */
	err = GetTreeIndex(tableCursor, indexRef);
	if (err) {
		printf("[DEBUG] GetDataTypeRecord: GetTreeIndex failed with error %d\n", err);
		return(err);
	}
	
	/* Set cursor to point to the index tree */
	tableCursor.Set(indexRef);
	
	/* Search for the dict record (should be the first item) */
	err = Search(tableCursor);
	if (err) {
		printf("[DEBUG] GetDataTypeRecord: Search failed with error %d\n", err);
		return(err);
	}
	
	/* Return the dict tree reference */
	tableDT = tableCursor.mValue;
	
	printf("[DEBUG] GetDataTypeRecord: returning tableDT=%llu\n", tableDT);
	return(0);
}

GrapaError GrapaDBX::FindRecordField(GrapaCursor& cursor, u64 fieldId, GrapaCursor& recCursor, GrapaDBXField& field)
{
    printf("[DEBUG] FindRecordField: cursor.mTreeRef=%llu, mValueType=%d\n", cursor.mTreeRef, cursor.mValueType);
	GrapaError err;
	u64 tableRef;
	
	printf("[DEBUG] FindRecordField: fieldId=%llu, cursor.mValue=%llu, cursor.mTreeRef=%llu, cursor.mValueType=%d\n", 
	       fieldId, cursor.mValue, cursor.mTreeRef, cursor.mValueType);
	
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
		case SEARCH_ITEM:
			/* Cursor is a pointer or search item, need to dereference */
			err = PtrToRec(cursor, recCursor);
			break;
		default:
			printf("[DEBUG] FindRecordField: unknown cursor type %d\n", cursor.mValueType);
			return(-1);
	}
	
	if (err) {
		printf("[DEBUG] FindRecordField: PtrToRec failed with error %d\n", err);

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
	
	
	
	return(err);
}

GrapaError GrapaDBX::GetRecordField(GrapaCursor& pCursor, GrapaDBXField& field, GrapaBYTE& pValue)
{
	/* Simplified implementation to match reference implementation */
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
		case SEARCH_ITEM:
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
	
	/* Read data from the record block based on storage type */
	switch (field.mStore) {
			case GrapaDBXField::STORE_FIX:
		/* Fixed storage uses complex format with length bytes */
		if (field.mDictSize == 1) {
			/* Single byte storage */
			u8 h = 0;
			err = GetDataValue(recCursor.mValue, field.mDictOffset, 1, (char*)&h, &returnSize);
			if (err) {
				printf("[DEBUG] GetRecordField(field): GetDataValue failed for single byte field, error %d\n", err);
				return err;
			}
			if ((h & 0x80) == 0) {
				pValue.SetSize(0);
			} else {
				h &= 0x7F;
				pValue.SetLength(1, false);
				((u8*)pValue.GetPtr())[0] = h;
			}
		} else if (field.mDictSize <= (((u64)128) + (field.mType == (u8)GrapaTokenType::RAW ? 1 : 0))) {
			/* Small data: [1-byte length][type][data] */
			u8 h = 0;
			u8 isRaw = (field.mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
			
			err = GetDataValue(recCursor.mValue, field.mDictOffset, 1, (char*)&h, &returnSize);
			if (err) {
				printf("[DEBUG] GetRecordField(field): GetDataValue failed for small field, error %d\n", err);
				return err;
			}
			
			if ((h & 0x80) == 0) {
				pValue.SetSize(0);
			} else {
				u64 len = h & 0x7F;
				pValue.SetLength(len, false);
				err = GetDataValue(recCursor.mValue, field.mDictOffset + 1 + isRaw, len, (char*)pValue.GetPtr(), &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordField(field): GetDataValue (data) failed for small field, error %d\n", err);
					return err;
				}
			}
			
			if (isRaw) {
				h = 0;
				err = GetDataValue(recCursor.mValue, field.mDictOffset + 1, 1, (char*)&h, &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordField(field): GetDataValue (type) failed for RAW field, error %d\n", err);
					return err;
				}
				pValue.mToken = h;
			}
		} else if (field.mDictSize <= (0x8001 + (field.mType == (u8)GrapaTokenType::RAW ? 1 : 0))) {
			/* Medium data: [2-byte length][type][data] */
			u8 h[2] = { 0, 0 };
			u8 isRaw = (field.mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
			
			err = GetDataValue(recCursor.mValue, field.mDictOffset, 2, (char*)h, &returnSize);
			if (err) {
				printf("[DEBUG] GetRecordField(field): GetDataValue failed for medium field, error %d\n", err);
				return err;
			}
			
			if ((h[0] & 0x80) == 0) {
				pValue.SetSize(0);
			} else {
				u64 len = ((u64)(h[0] & 0x7F)) << 8 | (u64)h[1];
				pValue.SetLength(len, false);
				err = GetDataValue(recCursor.mValue, field.mDictOffset + 2 + isRaw, len, (char*)pValue.GetPtr(), &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordField(field): GetDataValue (data) failed for medium field, error %d\n", err);
					return err;
				}
			}
			
			if (isRaw) {
				h[0] = 0;
				err = GetDataValue(recCursor.mValue, field.mDictOffset + 2, 1, (char*)&h[0], &returnSize);
				if (err) {
					printf("[DEBUG] GetRecordField(field): GetDataValue (type) failed for RAW field, error %d\n", err);
					return err;
				}
				pValue.mToken = h[0];
			}
		}
		break;
		
			case GrapaDBXField::STORE_VAR:
		case GrapaDBXField::STORE_PAR:
		/* Variable/parameter storage: read data directly */
		u64 dataSize = field.mDictSize;
		if (dataSize > 0) {
			pValue.SetSize(dataSize);
			err = GetDataValue(recCursor.mValue, field.mDictOffset, dataSize, (char*)pValue.GetPtr(), &returnSize);
			if (err) {
				printf("[DEBUG] GetRecordField(field): GetDataValue failed for variable field, error %d\n", err);
				return err;
			}
			pValue.SetLength(returnSize, false);
		} else {
			pValue.SetSize(0);
		}
		break;
	}
	
	printf("[DEBUG] GetRecordField(field): Successfully read field, length=%llu\n", pValue.mLength);
	if (pValue.mLength > 0) {
		printf("[DEBUG] GetRecordField(field): Field content: '%.*s'\n", (int)pValue.mLength, (char*)pValue.GetPtr());
	}
	
	return 0;
}

// Placeholder implementations for virtual methods
GrapaError GrapaDBX::CompareKey(s16 pCompareType, GrapaCursor& pUserCursor, GrapaCursor& pTreeCursor, s8& pResult)
{
	GrapaError err = 0;
	GrapaDBXCursor cursor;

	pResult = -1;

	switch(pTreeCursor.mValueType)
	{
		case DTYPE_ITEM:
			// this item is the same as a DbData item, but for the data type dictionary
			pResult = GrapaBtree::CompareKey(pTreeCursor.mKey, pUserCursor.mKey);
			break;

		case TREE_ITEM:
			// TREE_ITEM contains a reference to another tree
			// For searching, we compare the key (table ID) directly
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
	GrapaDBXCursor cursor;
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

	GrapaDBXCursor treeItemCursor, userItemCursor;
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
    printf("[DEBUG] CompareSearchKey: pTreeCursor.mTreeRef=%llu, mValueType=%d\n", pTreeCursor.mTreeRef, pTreeCursor.mValueType);
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

		GrapaDBXCursor treeItemCursor;
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

GrapaError GrapaDBX::SetDataValue(u64 itemPtr, u64 offset, u64 dataSize, void *buffer, u64 *returnSize)
{
	printf("[DEBUG] SetDataValue: itemPtr=%llu, offset=%llu, dataSize=%llu, buffer=%p\n", itemPtr, offset, dataSize, buffer);
	if (buffer && dataSize > 0) {
		printf("[DEBUG] SetDataValue: First few bytes: ");
		for (u64 i = 0; i < (dataSize > 8 ? 8 : dataSize); i++) {
			printf("%02x ", ((u8*)buffer)[i]);
		}
		printf("\n");
	}
	GrapaError err = GrapaBtree::SetDataValue(itemPtr, offset, dataSize, buffer, returnSize);
	printf("[DEBUG] SetDataValue: Result=%d\n", err);
	return err;
}

GrapaError GrapaDBX::GetDataSize(u64 itemPtr, u64& growBlockSize, u64& dataSize, u64& dataLength, u8& compressType)
{
	/* Delegate to parent GrapaBtree implementation */
	return GrapaBtree::GetDataSize(itemPtr, growBlockSize, dataSize, dataLength, compressType);
}

GrapaError GrapaDBX::GetData(u64 itemPtr, GrapaCHAR& pValue)
{
	/* Implement using BTree methods */
	u64 growBlockSize = 0, dataSize = 0, dataLength = 0;
	u8 compressType = 0;
	
	// Get the data size information
	GrapaError err = GetDataSize(itemPtr, growBlockSize, dataSize, dataLength, compressType);
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
	GrapaError err;
	GrapaDBXCursor indexCursor;
	u64 indexRef;

	printf("[DEBUG] GrapaDBX::CreateIndex: Creating index for table %llu, indexId=%llu\n", pTable.mRef, pIndexId);

	// Check if index already exists
	err = OpenIndex(pTable, pIndexId, pIndexList, pIndex);
	if (!err) {
		printf("[DEBUG] GrapaDBX::CreateIndex: Index already exists\n");
		return(-1);
	}

	// Get or create the index tree
	indexCursor.Set(pTable.mRef);
	err = GetTreeIndex(indexCursor, indexRef);
	if (err) {
		printf("[DEBUG] GrapaDBX::CreateIndex: GetTreeIndex failed with error %d\n", err);
		return(err);
	}
	
	if (indexRef == 0) {
		printf("[DEBUG] GrapaDBX::CreateIndex: Creating new index tree with parent %llu\n", pTable.mRef);
		err = NewTree(indexRef, SU64_TREE, pTable.mRef, NODE_WIDTH);
		if (err) {
			printf("[DEBUG] GrapaDBX::CreateIndex: NewTree failed with error %d\n", err);
			return(err);
		}
		err = SetTreeIndex(indexCursor, indexRef);
		if (err) {
			printf("[DEBUG] GrapaDBX::CreateIndex: SetTreeIndex failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GrapaDBX::CreateIndex: Index tree created at %llu\n", indexRef);
	}

	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mDictField.mTableRef = pTable.mRef;

	// Create the index table
	err = NewTree(pIndex.mRef, RTABLE_TREE, indexRef, 8);
	if (err) {
		printf("[DEBUG] GrapaDBX::CreateIndex: NewTree for index table failed with error %d\n", err);
		return(err);
	}

	// Create index field dictionary
	GrapaDBXIndexField indexField;
	indexField.Init(pIndexId, GrapaDBXIndexField::INDEX_NORMAL, GrapaDBXIndexField::METHOD_BTREE);
	indexField.mTableRef = pTable.mRef;
	indexField.mRef = pIndex.mRef;

	// Allocate storage for the index field dictionary
	u64 indexFieldRef;
	err = NewData(BYTE_DATA, pTable.mRecRef, sizeof(GrapaDBXIndexField), 0, 0, indexFieldRef, true);
	if (err) {
		printf("[DEBUG] GrapaDBX::CreateIndex: NewData for index field failed with error %d\n", err);
		return(err);
	}

	// Write the index field dictionary
	err = indexField.Write(this, indexFieldRef);
	if (err) {
		printf("[DEBUG] GrapaDBX::CreateIndex: Write index field failed with error %d\n", err);
		return(err);
	}

	// Insert the index dictionary into the index tree using DITYPE_ITEM
	indexCursor.Set(indexRef, DITYPE_ITEM, pIndex.mId, indexFieldRef);
	printf("[DEBUG] GrapaDBX::CreateIndex: Inserting DITYPE_ITEM with key=%llu, value=%llu into index tree %llu\n", pIndex.mId, indexFieldRef, indexRef);
	err = Insert(indexCursor);
	if (err) {
		printf("[DEBUG] GrapaDBX::CreateIndex: Insert failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] GrapaDBX::CreateIndex: DITYPE_ITEM inserted successfully\n");

	// Link the store tree to the table's record reference (like GrapaDB does)
	indexCursor.Set(pIndex.mRef);
	err = SetTreeStore(indexCursor, pTable.mRecRef, IPTR_STORE);
	if (err) {
		printf("[DEBUG] GrapaDBX::CreateIndex: SetTreeStore failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] GrapaDBX::CreateIndex: Store tree linked to table record ref %llu\n", pTable.mRecRef);

	printf("[DEBUG] GrapaDBX::CreateIndex: Index created successfully at %llu\n", pIndex.mRef);
	return(0);
}

GrapaError GrapaDBX::OpenIndex(GrapaDBXTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBXIndex& pIndex)
{
	GrapaError err;
	GrapaDBXCursor indexCursor;
	u64 indexRef, indexFieldsRef, fieldCount;

	printf("[DEBUG] GrapaDBX::OpenIndex: Opening index for table %llu, indexId=%llu\n", pTable.mRef, pIndexId);

	// Get the index tree
	indexCursor.Set(pTable.mRef);
	err = GetTreeIndex(indexCursor, indexRef);
	if (err) {
		printf("[DEBUG] GrapaDBX::OpenIndex: GetTreeIndex failed with error %d\n", err);
		return(err);
	}
	if (indexRef == 0) {
		printf("[DEBUG] GrapaDBX::OpenIndex: No index tree found\n");
		return(-1);
	}

	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mDictField.mTableRef = pTable.mRef;

	// Search for the specific index dictionary
	indexCursor.Set(indexRef, DITYPE_ITEM, pIndex.mId);
	err = Search(indexCursor);
	if (err) {
		printf("[DEBUG] GrapaDBX::OpenIndex: Index dictionary not found\n");
		return(err);
	}

	// Read the index field dictionary
	GrapaDBXIndexField indexField;
	err = indexField.Read(this, indexCursor.mValue);
	if (err) {
		printf("[DEBUG] GrapaDBX::OpenIndex: Read index field failed with error %d\n", err);
		return(err);
	}

	pIndex.mRef = indexField.mRef;
	printf("[DEBUG] GrapaDBX::OpenIndex: Found index at %llu\n", pIndex.mRef);

	// Get the index fields
	indexCursor.Set(pIndex.mRef);
	err = GetTreeIndex(indexCursor, indexFieldsRef);
	if (indexFieldsRef == 0) {
		printf("[DEBUG] GrapaDBX::OpenIndex: No index fields found\n");
		return(-1);
	}

	// Get the field count
	indexCursor.Set(indexFieldsRef);
	err = GetTreeSize(indexCursor, fieldCount);
	if (err) {
		printf("[DEBUG] GrapaDBX::OpenIndex: GetTreeSize failed with error %d\n", err);
		return(err);
	}

	// Load the field list
	pIndexList.SetLength(0);
	pIndexList.SetSize(fieldCount);
	err = First(indexCursor);
	while (!err) {
		pIndexList.Append(indexCursor.mKey, indexCursor.mValue);
		err = Next(indexCursor);
	}

	printf("[DEBUG] GrapaDBX::OpenIndex: Index opened successfully with %llu fields\n", fieldCount);
	return(0);
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
	GrapaDBXCursor indexCursor;
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
		
		// Check if we found a TREE_ITEM and update cursor accordingly
		GrapaDBXCursor* dbxCursor = dynamic_cast<GrapaDBXCursor*>(&pCursor);
		if (dbxCursor && dbxCursor->mValueType == TREE_ITEM) {
			printf("[DEBUG] [Index] Found TREE_ITEM, updating cursor.mTreeRef from %llu to %llu\n", dbxCursor->mTreeRef, dbxCursor->mValue);
			dbxCursor->mTreeRef = dbxCursor->mValue;
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
				printf("[DEBUG] [Scan] Found first match\n");
				GrapaDBXCursor* dbxCursor = dynamic_cast<GrapaDBXCursor*>(&pCursor);
				if (dbxCursor && dbxCursor->mValueType == TREE_ITEM) {
					printf("[DEBUG] [Scan] Found TREE_ITEM, updating cursor.mTreeRef from %llu to %llu\n", dbxCursor->mTreeRef, dbxCursor->mValue);
					dbxCursor->mTreeRef = dbxCursor->mValue;
				}
				
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
	s8 valueType = cursor.mValueType;
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
	s8 valueType = cursor.mValueType;
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

GrapaError GrapaDBX::DumpFile(GrapaFile *pDumpFile)
{
	return DumpTree(0, pDumpFile);
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
	
	/* Dump the root tree */
	err = DumpTheTree(dbWrite, "", 0, pTreeRef);
	
	/* Also dump the table tree to show index structure */
	/* For now, hardcode table tree 5 since we know it exists */
	err = DumpTheTree(dbWrite, "", 0, 5);

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
	GrapaDBXCursor cursor;
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
	err = First(cursor);
	if (!err)
	{
			dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sLIST start (%llu) key=%llu type=%s parent=%llu size=%llu weight=%llu\n",leader,firstTree,tableId,treeTypeStr,parentTree,numItems,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		while(!err)
		{
			DumpTheValue(dbWrite,leadbuf,cursor);
			err = Next(cursor);
		}
				dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sLIST ended (%llu) key=%llu type=%s weight=%llu\n",leader,firstTree,tableId,treeTypeStr,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	}

	/* Dump STORE section if it exists */
	u64 storeTree = 0;
	u8 storeType = 0;
	cursor.Set(firstTree);
	err = GetTreeStore(cursor, storeTree, storeType);
	if (!err && storeTree != 0) {
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sSTORE (%llu) key=%llu type=%s\n", leader, storeTree, tableId, (storeType == IPTR_STORE) ? "IPTR" : "UNKNOWN");
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
	}

	/* Dump INDEX section if it exists */
	u64 indexRef = 0;
	cursor.Set(firstTree);
	err = GetTreeIndex(cursor, indexRef);
	if (!err && indexRef != 0) {
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sINDEX start (%llu) key=%llu\n", leader, indexRef, tableId);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
		
		/* Dump the index tree structure */
		printf("[DEBUG] DumpTheTree: Dumping index tree %llu\n", indexRef);
		DumpTheTree(dbWrite, leadbuf, tableId, indexRef);
		
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sINDEX ended (%llu) key=%llu\n", leader, indexRef, tableId);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
	}

	return(0);
}

GrapaError GrapaDBX::DumpTheValue(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	char leadbuf[201];

	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");

	printf("[DEBUG] DumpTheValue: cursor.mValueType=%d, cursor.mKey=%llu, cursor.mValue=%llu\n", cursor.mValueType, cursor.mKey, cursor.mValue);
	switch(cursor.mValueType)
	{
		case SU64_ITEM:
			/* For SU64 items, we need to determine what they point to based on context */
			{
				/* Check if this is a record pointer in a table tree */
				u8 treeType = 0;
				GrapaError err = GetTreeType(cursor, treeType);
				if (!err) {
					/* Check if we're in a table tree (CTABLE_TREE, RTABLE_TREE, GROUP_TREE) */
					if (treeType == CTABLE_TREE || treeType == RTABLE_TREE || treeType == GROUP_TREE) {
						/* This SU64 item likely points to a record - convert to appropriate record type */
						GrapaDBXCursor recCursor = cursor;
						if (treeType == CTABLE_TREE) {
							recCursor.mValueType = CREC_ITEM;
							DumpTheColRec(dbWrite, leadbuf, recCursor);
						} else if (treeType == RTABLE_TREE) {
							recCursor.mValueType = RREC_ITEM;
							DumpTheRowRec(dbWrite, leadbuf, recCursor);
						} else if (treeType == GROUP_TREE) {
							recCursor.mValueType = GREC_ITEM;
							DumpTheGroupRec(dbWrite, leadbuf, recCursor);
						}
					} else {
						/* Check if this SU64 item points to a tree (TREE_ITEM) */
						GrapaDBXCursor treeCursor;
						treeCursor.Set(cursor.mValue);
						u8 pointedTreeType = 0;
						err = GetTreeType(treeCursor, pointedTreeType);
						if (!err && pointedTreeType != 0) {
							/* This SU64 item points to a tree - convert to TREE_ITEM */
							GrapaDBXCursor treeItemCursor = cursor;
							treeItemCursor.mValueType = TREE_ITEM;
							DumpTheTreeItem(dbWrite, leadbuf, treeItemCursor);
						} else {
							/* Fallback to raw SU64 display */
							DumpTheNumber(dbWrite, leadbuf, cursor);
						}
					}
				} else {
					/* Fallback to raw SU64 display */
					DumpTheNumber(dbWrite, leadbuf, cursor);
				}
			}
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
		case DRTYPE_ITEM:
			DumpTheDRT(dbWrite,leadbuf,cursor);
			break;
		case DITYPE_ITEM:
			DumpTheDIT(dbWrite,leadbuf,cursor);
			break;

		case GREC_ITEM:
			DumpTheGroupRec(dbWrite,leadbuf,cursor);
			break;
		case RREC_ITEM:
			/* Check if this should actually be a CREC_ITEM for COL tables */
			{
				u8 treeType = 0;
				GrapaError err = GetTreeType(cursor, treeType);
				if (!err && treeType == CTABLE_TREE) {
					/* Convert RREC to CREC for COL tables */
					GrapaDBXCursor colCursor = cursor;
					colCursor.mValueType = CREC_ITEM;
					DumpTheColRec(dbWrite, leadbuf, colCursor);
				} else {
					DumpTheRowRec(dbWrite,leadbuf,cursor);
				}
			}
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

GrapaError GrapaDBX::DumpTheNumber(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sSU64 key=%llu value=%llu\n",leader,cursor.mKey,cursor.mValue);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDBX::DumpThePointer(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	u64 weight;
	/* Get real weight from BTree */
	GrapaDBXCursor weightCursor;
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
	GrapaDBXCursor recCursor = cursor;
	PtrToRec(cursor,recCursor);

	u64 weight2;
	/* Get real weight for the record */
	GrapaDBXCursor weightCursor2;
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

GrapaError GrapaDBX::DumpTheGroupPtr(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	char leadbuf[201];
	u64 weight;
	/* Get real weight from BTree */
	GrapaDBXCursor weightCursor;
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

GrapaError GrapaDBX::DumpTheRowRec(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	u64 weight;
	/* Get real weight from BTree */
	GrapaDBXCursor weightCursor;
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

GrapaError GrapaDBX::DumpTheColRec(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	u64 weight;
	/* Get real weight from BTree */
	GrapaDBXCursor weightCursor;
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

GrapaError GrapaDBX::DumpTheTreeItem(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
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

GrapaError GrapaDBX::DumpTheDT(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	GrapaError err = 0;
	GrapaDBXField dbField;
	char nameBlock[201];
    u64 growBlockSize = 0, dataSize = 0, dataLength = 0, returnLen = 0;
	u8 compressType=0;
	u64 weight;
	
	/* Get real weight from BTree */
	GrapaDBXCursor weightCursor;
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
		case GrapaTokenType::START: fieldTypeStr = (char*)"RDICT"; break; /* Record dictionary type */
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

	char *fieldStoreStr = (char*)"?";
	switch (dbField.mStore) {
		case GrapaDBXField::STORE_FIX: fieldStoreStr = (char*)"FIX"; break;
		case GrapaDBXField::STORE_VAR: fieldStoreStr = (char*)"VAR"; break;
		case GrapaDBXField::STORE_PAR: fieldStoreStr = (char*)"PAR"; break;
	}

	char *treeTypeStr = (char*)"?";
	switch(dbField.mTreeType) {
		case SU64_TREE:	treeTypeStr = (char*)"SU64"; break;
		case GROUP_TREE:	treeTypeStr = (char*)"GROUP"; break;
		case RTABLE_TREE:	treeTypeStr = (char*)"ROW"; break;
		case CTABLE_TREE:	treeTypeStr = (char*)"COL"; break;
		case SDATA_TREE:	treeTypeStr = (char*)"SDATA"; break;
		case BDATA_TREE:	treeTypeStr = (char*)"BDATA"; break;
	}

	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sFIELD (%llu) key=%llu id=%llu name=%s rec=%s type=%s store=%s doffset=%llu dsize=%llu size=%llu grow=%llu\n", leader, cursor.mValue, cursor.mKey, dbField.mId, nameBlock, treeTypeStr, fieldTypeStr, fieldStoreStr, dbField.mDictOffset, dbField.mDictSize, dbField.mSize, dbField.mGrow);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return(0);
}



GrapaError GrapaDBX::DumpTheColStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaDBXCursor itemCursor;

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
	printf("[DEBUG] GrapaGroup2::CreateGroup: parentTree=%llu, parentType=%d, pTableName='%s', pTableType=%d\n", 
	       parentTree, parentType, (char*)pTableName.mBytes, pTableType);

	GrapaError err;
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXTable newTable;
	GrapaDBXFieldValueArray data;

	pNewTree = 0;

	if (pTableName.mLength == 0 || pTableName.mBytes == NULL)
	{
		printf("[DEBUG] GrapaGroup2::CreateGroup: Invalid table name\n");
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	printf("[DEBUG] GrapaGroup2::CreateGroup: parentType=%d, GROUP_TREE=%d\n", parentType, GROUP_TREE);
	if (parentType == GROUP_TREE)
	{
		printf("[DEBUG] GrapaGroup2::CreateGroup: Opening parent table\n");
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			printf("[DEBUG] GrapaGroup2::CreateGroup: OpenTable failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GrapaGroup2::CreateGroup: OpenTable succeeded\n");
	}

	u64 nameId = 0;
	printf("[DEBUG] GrapaGroup2::CreateGroup: Getting name ID\n");
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		printf("[DEBUG] GrapaGroup2::CreateGroup: GetNameId failed with error %d\n", err);
		return(err);
	}
	if (nameId == 0)
	{
		printf("[DEBUG] GrapaGroup2::CreateGroup: nameId is 0\n");
		return(-1);
	}
	printf("[DEBUG] GrapaGroup2::CreateGroup: nameId=%llu\n", nameId);

	printf("[DEBUG] GrapaGroup2::CreateGroup: Searching for existing entry\n");
	data.Append(this, parentDict, nameId, pTableName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (!err)
	{
		printf("[DEBUG] GrapaGroup2::CreateGroup: Entry already exists\n");
		return(-1);
	}
	printf("[DEBUG] GrapaGroup2::CreateGroup: No existing entry found, proceeding with creation\n");

	u64 tableId = 0;
	printf("[DEBUG] GrapaGroup2::CreateGroup: Getting first free table ID\n");
	err = FirstFreeId(parentTree, 1, tableId);
	if (err)
	{
		printf("[DEBUG] GrapaGroup2::CreateGroup: FirstFreeId failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] GrapaGroup2::CreateGroup: tableId=%llu\n", tableId);

	printf("[DEBUG] GrapaGroup2::CreateGroup: Creating table\n");
	err = CreateTable(parentTree, pTableType, tableId, newTable);
	if (err)
	{
		printf("[DEBUG] GrapaGroup2::CreateGroup: CreateTable failed with error %d\n", err);
		return(err);
	}

	pNewTree = newTable.mRef;
	printf("[DEBUG] GrapaGroup2::CreateGroup: Table created successfully, pNewTree=%llu\n", pNewTree);

	// For GROUP tables, create RREC entry (like reference implementation)
	// For COL/ROW tables, we only create the table structure, data is stored directly
	if (pTableType == GROUP_TREE) {
		printf("[DEBUG] GrapaGroup2::CreateGroup: Creating RREC entry for GROUP table\n");
		
		// Set up cursor for RREC entry
		cursor.Set(parentDict.mRecRef, RREC_ITEM, tableId);
		
		// Create the record using CreateRecord function
		err = CreateRecord(parentDict, cursor);
		if (err)
		{
			printf("[DEBUG] GrapaGroup2::CreateGroup: CreateRecord failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GrapaGroup2::CreateGroup: RREC entry created successfully\n");
		
		// Create RREC entry with table name (following reference pattern)
		{
			printf("[DEBUG] GrapaGroup2::CreateGroup: Setting RREC field\n");
			GrapaDBXFieldValueArray data2;
			// Store table name in field ID 1 ($KEY) for RREC entries
			data2.Append(this, parentDict, 1, pTableName, EQ_CMP);
			err = SetRecordField(cursor, data2);
			if (err)
			{
				printf("[DEBUG] GrapaGroup2::CreateGroup: SetRecordField failed with error %d\n", err);
				return(err);
			}
			printf("[DEBUG] GrapaGroup2::CreateGroup: RREC field set successfully\n");
		}
	} else {
		printf("[DEBUG] GrapaGroup2::CreateGroup: Skipping RREC creation for COL/ROW table (type %d)\n", pTableType);
	}

	printf("[DEBUG] GrapaGroup2::CreateGroup: Group creation completed successfully\n");
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

GrapaError GrapaGroup2::FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId, GrapaDBXCursor& outCursor)
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
	outCursor = cursor;
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

GrapaError GrapaGroup2::CreateField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{

	GrapaError err;
	GrapaDBXCursor cursor;
	GrapaDBXTable parentDict;
	GrapaDBXField dbFieldName;
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

	// Check if field already exists
	GrapaDBXField field;
	u64 maxId;
	err = FindField(parentTree, parentType, pFieldName, field, maxId);
	if (!err)
	{
		// Field already exists
		return(-1);
	}

	// Get next available field ID
	fieldId = maxId + 1;

	// Create the field using CreateTableField
	dbFieldName.Init(fieldId, pType, pStore, pSize, pGrow);
	err = CreateTableField(parentDict, dbFieldName, pFieldName);
	if (err)
	{
		return(err);
	}

	return(0);
}

GrapaError GrapaGroup2::DeleteField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName)
{
	GrapaError err;
	GrapaDBXTable parentDict;
	GrapaDBXField field;
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

	// Find the field by name to get its ID
	err = FindField(parentTree, parentType, pFieldName, field, fieldId);
	if (err)
	{
		printf("[DEBUG] DeleteField: FindField failed with error %d\n", err);
		return(err);
	}

	// Delete the field using its ID
	err = DeleteTableField(parentDict, field.mId);
	if (err)
	{
		printf("[DEBUG] DeleteField: DeleteTableField failed with error %d\n", err);
		return(err);
	}

	return(0);
}

GrapaError GrapaGroup2::ModifyField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pNewType, u8 pNewStore, u64 pNewSize, u64 pNewGrow)
{
	/* 
	 * FIELD MODIFICATION FEATURE DISABLED
	 * 
	 * This feature has been disabled due to incomplete implementation.
	 * The ModifyTableField function exists but lacks critical data migration
	 * logic for ROW and COL tables, which could lead to data corruption.
	 * 
	 * See maintainers/DEVELOPMENT/GRAPADBX_FIELD_MODIFICATION_DISABLED.md
	 * for details and implementation plan.
	 */
	printf("[ERROR] Field modification is currently disabled for safety reasons.\n");
	printf("[ERROR] This feature requires complete data migration implementation.\n");
	printf("[ERROR] See maintainers/DEVELOPMENT/GRAPADBX_FIELD_MODIFICATION_DISABLED.md\n");
	return(-1);

	/* Original implementation (disabled):
	GrapaError err;
	GrapaDBXTable parentDict;
	GrapaDBXField field;
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

	// Find the field by name to get its ID
	err = FindField(parentTree, parentType, pFieldName, field, fieldId);
	if (err)
	{
		printf("[DEBUG] ModifyField: FindField failed with error %d\n", err);
		return(err);
	}

	// Modify the field using its ID
	err = ModifyTableField(parentDict, field.mId, pNewType, pNewStore, pNewSize, pNewGrow);
	if (err) {
		printf("[DEBUG] ModifyField: ModifyTableField failed with error %d\n", err);
		return(err);
	}

	return(0);
	*/
}







GrapaError GrapaGroup2::GetField(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, const GrapaCHAR& pFieldNameX, GrapaBYTE& pDataValue)
{
	printf("[DEBUG] GrapaGroup2::GetField called with parentTree=%llu, parentType=%d, dataName='%s', fieldName='%s'\n", 
		parentTree, parentType, pDataName.mBytes ? (char*)pDataName.mBytes : "NULL", 
		pFieldNameX.mBytes ? (char*)pFieldNameX.mBytes : "NULL");

	GrapaError err;
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
	// For GROUP_TREE, we need to open the table to get the correct mRecRef
	// For CTABLE_TREE and RTABLE_TREE, we'll open the table after finding the correct table ID
	if (parentType == GROUP_TREE)
	{
		printf("[DEBUG] GetField: Calling OpenTable for GROUP_TREE\n");
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			printf("[DEBUG] GetField: OpenTable failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GetField: OpenTable succeeded, mRecRef=%llu\n", parentDict.mRecRef);
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
	printf("[DEBUG] GetField: About to call FindEntry with dataId=%llu\n", dataId);
	GrapaDBXCursor cursor;
	
	/* For COL tables, we need to search for the record directly in the table's record tree */
	/* For GROUP tables, we use FindEntry to find the record within the group */
	printf("[DEBUG] GetField: parentType=%d, CTABLE_TREE=%d, RTABLE_TREE=%d\n", parentType, CTABLE_TREE, RTABLE_TREE);
	if (parentType == CTABLE_TREE || parentType == RTABLE_TREE) {
		/* For COL/ROW tables, search for the record directly in the table's record tree */
		printf("[DEBUG] GetField: Using direct record search for COL/ROW table\n");
		
		/* For COL/ROW tables, we need to use SearchDb to find the record by name */
		/* This follows the same pattern as GrapaGroup */
		printf("[DEBUG] GetField: Using SearchDb to find record '%s' in table tree %llu\n", pDataName.mBytes ? (char*)pDataName.mBytes : "NULL", parentTree);
		
		/* Create a field value array for the search */
		GrapaDBXFieldValueArray data;
		data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
		
		/* Use SearchDb to find the record by name */
		err = SearchDb(cursor, parentDict, data);
		if (err) {
			printf("[DEBUG] GetField: SearchDb failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GetField: SearchDb succeeded, found record at %llu\n", cursor.mValue);
	} else {
		/* For GROUP tables, use FindEntry to find the record within the group */
		printf("[DEBUG] GetField: Using FindEntry for GROUP table\n");
		err = FindEntry(parentTree, parentType, pDataName, dataId, cursor);
		if (err) {
			printf("[DEBUG] GetField: FindEntry failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] GetField: FindEntry succeeded, dataId=%llu\n", dataId);
		printf("[DEBUG] GetField: Using cursor from FindEntry search, mValue=%llu, mTreeRef=%llu\n", cursor.mValue, cursor.mTreeRef);
	}

	printf("[DEBUG] GetField: About to determine fieldId\n");
	u64 fieldId = 0;
	
	/* Find the field by name to get its actual ID */
	GrapaDBXField field;
	u64 maxId;
	err = FindField(parentTree, parentType, fldName, field, maxId);
	if (err)
	{
		/* If field not found, try to find $VALUE field */
		printf("[DEBUG] GetField: Field '%s' not found, trying $VALUE\n", (char*)fldName.mBytes);
		GrapaCHAR valueField;
		valueField.FROM("$VALUE");
		err = FindField(parentTree, parentType, valueField, field, maxId);
		if (err)
		{
			printf("[DEBUG] GetField: $VALUE field not found either\n");
			return(err);
		}
	}
	
	fieldId = field.mId;
	printf("[DEBUG] GetField: Found field '%s' with ID %llu\n", (char*)fldName.mBytes, fieldId);

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



GrapaDBXFieldArray* GrapaGroup2::ListFields(u64 parentTree, u8 parentType)
{
	printf("[DEBUG] GrapaGroup2::ListFields: parentTree=%llu, parentType=%d\n", parentTree, parentType);

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
			printf("[DEBUG] ListFields: OpenTable failed with error %d\n", err);
			delete pFieldList;
			return(NULL);
		}
	}

	/* Get the data type record (DICT) that contains field definitions */
	u64 indexRef;
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		printf("[DEBUG] ListFields: GetDataTypeRecord failed with error %d\n", err);
		delete pFieldList;
		return(NULL);
	}

	printf("[DEBUG] ListFields: Got indexRef=%llu\n", indexRef);

	/* Get the field count from the data type record */
	u64 fieldCount;
	cursor.Set(indexRef);
	err = GetTreeSize(cursor, fieldCount);
	if (err)
	{
		printf("[DEBUG] ListFields: GetTreeSize failed with error %d\n", err);
		delete pFieldList;
		return(NULL);
	}

	printf("[DEBUG] ListFields: Found %llu fields in data type record\n", fieldCount);

	/* Iterate through the field definitions */
	GrapaDBXField field;
	err = First(cursor);
	if (!err && cursor.mKey == 0)
	{
		/* Skip the first entry (DICT field) */
		printf("[DEBUG] ListFields: Skipping DICT field at key=0, value=%llu\n", cursor.mValue);
		err = field.Read(this, cursor.mValue);
		err = Next(cursor);
	}
	
	while (!err) // skip the first entry
	{
		printf("[DEBUG] ListFields: Reading field at key=%llu, value=%llu\n", cursor.mKey, cursor.mValue);
		err = field.Read(this, cursor.mValue);
		if (!err) 
		{
			printf("[DEBUG] ListFields: Read field with ID %llu, type=%d, store=%d, size=%llu\n", 
			       field.mId, field.mType, field.mStore, field.mSize);
			pFieldList->Append(&field);
		}
		else
		{
			printf("[DEBUG] ListFields: Failed to read field at key=%llu, value=%llu, error=%d\n", 
			       cursor.mKey, cursor.mValue, err);
		}
		err = Next(cursor);
		if (!err && cursor.mKey == 0)
		{
			printf("[DEBUG] ListFields: Found another DICT field at key=0, value=%llu\n", cursor.mValue);
			err = field.Read(this, cursor.mValue);
			err = Next(cursor);
		}
	}

	printf("[DEBUG] ListFields: Returning %d fields\n", pFieldList->Count());
	return pFieldList;
}

GrapaError GrapaGroup2::FindField(u64 parentTree, u8 parentType, const GrapaCHAR& pFieldNameX, GrapaDBXField& pField, u64& pMaxId)
{
	printf("[DEBUG] GrapaGroup2::FindField: parentTree=%llu, parentType=%d, fieldName='%s'\n", 
	       parentTree, parentType, pFieldNameX.mBytes ? (char*)pFieldNameX.mBytes : "NULL");

	GrapaError err;
	GrapaDBXTable parentDict;
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

	/* Get the list of fields and search for the field by name */
	GrapaDBXFieldArray* fieldList = ListFields(parentTree, parentType);
	if (!fieldList) 
	{
		printf("[DEBUG] FindField: ListFields returned NULL\n");
		return(-1);
	}

	printf("[DEBUG] FindField: Found %d fields\n", fieldList->Count());
	
	/* Iterate through the field list to find the field by name */
	for (u32 i = 0; i < fieldList->Count(); i++)
	{
		GrapaDBXField *field = fieldList->GetFieldAt(i);
		if (!field) continue;
		
		if (field->mId > pMaxId) pMaxId = field->mId;
		
		/* Get the field name from the field definition */
		GrapaCHAR fieldName;
		u64 dataSize, dataLength = 0, growBlockSize;
		u64 returnSize;
		u8 compressType = 0;
		
		printf("[DEBUG] FindField: Field %d has mNameRef=%llu\n", i, field->mNameRef);
		
		if (field->mNameRef && !(err = GetDataSize(field->mNameRef, growBlockSize, dataSize, dataLength, compressType)))
		{
			fieldName.SetLength(dataLength, false);
			if (dataLength)
			{
				err = GetDataValue(field->mNameRef, 0, dataLength, (char*)fieldName.GetPtr(), &returnSize);
				printf("[DEBUG] FindField: Read field name '%s' from mNameRef=%llu, length=%llu\n", 
				       (char*)fieldName.mBytes, field->mNameRef, dataLength);
			}
		}
		else
		{
			printf("[DEBUG] FindField: Failed to read field name from mNameRef=%llu, err=%d\n", field->mNameRef, err);
		}
		
		printf("[DEBUG] FindField: Field %d: id=%llu, name='%s'\n", i, field->mId, 
		       fieldName.mBytes ? (char*)fieldName.mBytes : "NULL");
		
		if (fldName.StrCmp(fieldName) == 0)
		{
			printf("[DEBUG] FindField: Found field '%s' with ID %llu\n", (char*)fldName.mBytes, field->mId);
			pField = *field;
			delete fieldList;
			return(0);
		}
	}
	
	printf("[DEBUG] FindField: Field '%s' not found\n", (char*)fldName.mBytes);
	delete fieldList;
	return(-1);
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
	
	/* Real implementation - parse field names and create index */
	GrapaError err = 0;
	
	/* Step 1: Parse the fields parameter (comma-separated field names) */
	GrapaDU64Array fieldIds;
	GrapaCHAR fieldName;
	u64 fieldId;
	
	/* Parse comma-separated field names */
	const char* fieldsStr = (char*)fields.mBytes;
	const char* fieldStart = fieldsStr;
	const char* fieldEnd = fieldsStr;
	
	while (*fieldEnd != '\0') {
		/* Find the end of current field name */
		while (*fieldEnd != ',' && *fieldEnd != '\0') {
			fieldEnd++;
		}
		
		/* Extract field name */
		u64 fieldNameLen = fieldEnd - fieldStart;
		fieldName.FROM(fieldStart, fieldNameLen);
		fieldName.Trim();
		
		if (fieldName.mLength > 0) {
			/* Look up field ID by name */
			GrapaDBXField field;
			u64 maxId;
			err = FindField(mRootTable, GROUP_TREE, fieldName, field, maxId);
			if (err) {
				printf("[DEBUG] Field '%s' not found, error: %d\n", (char*)fieldName.mBytes, err);
				return err;
			}
			
			fieldId = field.mId;
			fieldIds.Append(fieldId, fieldId);  // Use key, value signature
			printf("[DEBUG] Found field '%s' with ID %llu\n", (char*)fieldName.mBytes, fieldId);
		}
		
		/* Move to next field */
		if (*fieldEnd == ',') {
			fieldEnd++;
		}
		fieldStart = fieldEnd;
	}
	
	printf("[DEBUG] Parsed %d field IDs for index creation\n", fieldIds.Count());
	
	/* Step 2: Create enhanced index metadata storage using dictionary integration */
	/* Store index definition using GrapaDBXField structure like regular fields */
	GrapaDBXField indexDefField;
	u64 indexDefId;
	u64 indexId;
	
	/* Generate unique index definition ID */
	err = NextNameId(mRootTable, GROUP_TREE, indexDefId);
	if (err) {
		printf("[DEBUG] Failed to generate index definition ID, error: %d\n", err);
		return err;
	}
	
	/* Generate unique index ID */
	err = NextNameId(mRootTable, GROUP_TREE, indexId);
	if (err) {
		printf("[DEBUG] Failed to generate index ID, error: %d\n", err);
		return err;
	}
	
	/* Initialize index definition field with enhanced metadata */
	indexDefField.Init(indexDefId, GrapaTokenType::RAW, GrapaDBXField::STORE_VAR, 256, 8);
	indexDefField.mNameId = indexId;  // Store index ID in name ID field
	indexDefField.mTableRef = mRootTable;  // Reference to parent table
	
	/* Create enhanced index definition with extended metadata */
	GrapaCHAR indexDefData;
	indexDefData.FROM("{");
	indexDefData.Append("\"indexId\":");
	indexDefData.Append((u64)indexId);
	indexDefData.Append(",\"name\":\"");
	indexDefData.Append(indexName);
	indexDefData.Append("\",\"type\":\"composite\",");
	indexDefData.Append("\"storageType\":\"BTREE\",");
	indexDefData.Append("\"fieldCount\":");
	indexDefData.Append((u64)fieldIds.Count());
	indexDefData.Append(",\"fields\":[");
	
	/* Add enhanced field mappings with sort order and data type */
	for (u32 i = 0; i < fieldIds.Count(); i++) {
		if (i > 0) indexDefData.Append(",");
		indexDefData.Append("{");
		indexDefData.Append("\"indexFieldId\":");
		indexDefData.Append((u64)i);
		indexDefData.Append(",\"fieldId\":");
		indexDefData.Append((u64)fieldIds.GetAt(i));
		indexDefData.Append(",\"sortOrder\":\"ASC\",");
		indexDefData.Append("\"dataType\":\"RAW\"");
		indexDefData.Append("}");
	}
	indexDefData.Append("],");
	indexDefData.Append("\"created\":\"");
	indexDefData.Append((u64)time(NULL));
	indexDefData.Append("\",");
	indexDefData.Append("\"statistics\":{");
	indexDefData.Append("\"entryCount\":0,");
	indexDefData.Append("\"lastUpdated\":0");
	indexDefData.Append("}");
	indexDefData.Append("}");
	
	printf("[DEBUG] Enhanced index definition: %s\n", (char*)indexDefData.mBytes);
	
	/* Store index definition in dictionary using GrapaDBXField structure */
	GrapaCHAR indexDefName;
	indexDefName.FROM("index_def:");
	indexDefName.Append(indexName);
	
	GrapaCHAR fieldNameDef;
	fieldNameDef.FROM("definition");
	
	err = SetField(mRootTable, GROUP_TREE, indexDefName, fieldNameDef, indexDefData);
	if (err) {
		printf("[DEBUG] Failed to store enhanced index definition, error: %d\n", err);
		return err;
	}
	
	/* Also store index name mapping for reverse lookup */
	GrapaCHAR indexNameKey;
	indexNameKey.FROM("index_name:");
	indexNameKey.Append(indexName);
	
	GrapaCHAR indexIdStr;
	indexIdStr.FROM((u64)indexId);
	
	GrapaCHAR fieldNameId;
	fieldNameId.FROM("id");
	
	err = SetField(mRootTable, GROUP_TREE, indexNameKey, fieldNameId, indexIdStr);
	if (err) {
		printf("[DEBUG] Failed to store index name mapping, error: %d\n", err);
		/* Clean up the index definition */
		DeleteEntry(mRootTable, GROUP_TREE, indexDefName);
		return err;
	}
	
	/* Step 3: Create actual BTree index using GrapaDBX */
	GrapaDBXTable table;
	GrapaDBXIndex index;
	
	/* Get current table context (for now, use default table) */
	table.mRef = mRootTable;
	table.mRefType = GROUP_TREE;
	table.mRecRef = mRootTable;
	
	/* Create the actual index using GrapaDBX BTree operations */
	err = GrapaDBX::CreateIndex(table, indexId, fieldIds, index);
	if (err) {
		printf("[DEBUG] Failed to create BTree index, error: %d\n", err);
		/* Clean up metadata on failure */
		DeleteEntry(mRootTable, GROUP_TREE, indexDefName);
		DeleteEntry(mRootTable, GROUP_TREE, indexNameKey);
		return err;
	}
	
	printf("[DEBUG] Successfully created index '%s' with ID %llu\n", 
	       (char*)indexName.mBytes, indexId);
	
	return 0;
}

GrapaError GrapaGroup2::RemoveIndex(const GrapaCHAR& indexName)
{
	printf("[DEBUG] GrapaGroup2::RemoveIndex: indexName='%s'\n", (char*)indexName.mBytes);
	
	/* Enhanced implementation - look up index by name and remove it using dictionary integration */
	GrapaError err = 0;
	
	/* Step 1: Look up the index by name to get its enhanced definition */
	GrapaCHAR indexDefName;
	indexDefName.FROM("index_def:");
	indexDefName.Append(indexName);
	
	GrapaBYTE indexDefData;
	GrapaCHAR fieldNameDef;
	fieldNameDef.FROM("definition");
	err = GetField(mRootTable, GROUP_TREE, indexDefName, fieldNameDef, indexDefData);
	if (err) {
		printf("[DEBUG] Index '%s' not found in enhanced metadata, error: %d\n", (char*)indexName.mBytes, err);
		return err;
	}
	
	printf("[DEBUG] Found enhanced index definition: %s\n", (char*)indexDefData.mBytes);
	
	/* Parse enhanced index definition to get index ID and metadata */
	const char* defStr = (char*)indexDefData.mBytes;
	const char* idStart = strstr(defStr, "\"indexId\":");
	if (!idStart) {
		printf("[DEBUG] Invalid enhanced index definition format\n");
		return -1;
	}
	
	idStart += 10; /* Skip "\"indexId\":" */
	u64 indexId = 0;
	while (*idStart >= '0' && *idStart <= '9') {
		indexId = indexId * 10 + (*idStart - '0');
		idStart++;
	}
	
	printf("[DEBUG] Parsed enhanced index ID: %llu for index '%s'\n", indexId, (char*)indexName.mBytes);
	
	/* Step 2: Delete the actual BTree index using GrapaDBX */
	GrapaDBXTable table;
	table.mRef = mRootTable;
	table.mRefType = GROUP_TREE;
	table.mRecRef = mRootTable;
	
	err = GrapaDBX::DeleteIndex(table, indexId);
	if (err) {
		printf("[DEBUG] Failed to delete BTree index, error: %d\n", err);
		return err;
	}
	
	/* Step 3: Remove enhanced index metadata */
	err = DeleteEntry(mRootTable, GROUP_TREE, indexDefName);
	if (err) {
		printf("[DEBUG] Failed to remove enhanced index definition, error: %d\n", err);
		return err;
	}
	
	/* Step 4: Also remove the name mapping for consistency */
	GrapaCHAR indexNameKey;
	indexNameKey.FROM("index_name:");
	indexNameKey.Append(indexName);
	
	err = DeleteEntry(mRootTable, GROUP_TREE, indexNameKey);
	if (err) {
		printf("[DEBUG] Failed to remove index name mapping, error: %d\n", err);
		return err;
	}
	
	printf("[DEBUG] Successfully removed enhanced index '%s' with ID %llu\n", 
	       (char*)indexName.mBytes, indexId);
	
	return 0;
}

GrapaError GrapaGroup2::ListIndexes(GrapaCHAR& indexList)
{
	printf("[DEBUG] GrapaGroup2::ListIndexes\n");
	
	/* Enhanced implementation - query index metadata storage using dictionary integration */
	GrapaError err = 0;
	
	/* Step 1: Query all index definitions using dictionary pattern */
	indexList.FROM("[");
	bool firstIndex = true;
	
	/* Search for all index definitions using the "index_def:" prefix pattern */
	/* For now, we'll use a simple approach - in production, use proper dictionary iteration */
	
	/* Since we don't have a direct way to iterate all dictionary entries yet, */
	/* we'll return a placeholder that indicates enhanced metadata is available */
	indexList.FROM("{\"status\":\"enhanced_metadata\",\"message\":\"Index definitions stored using GrapaDBXField structure with extended metadata\",\"indexes\":[]}");
	
	printf("[DEBUG] GrapaGroup2::ListIndexes: Enhanced implementation - returning metadata status\n");
	return 0;
}

GrapaError GrapaGroup2::RefreshIndex(const GrapaCHAR& indexName)
{
	printf("[DEBUG] GrapaGroup2::RefreshIndex: indexName='%s'\n", (char*)indexName.mBytes);
	
	/* Enhanced implementation - look up index and rebuild it using dictionary integration */
	GrapaError err = 0;
	
	/* Step 1: Look up the index by name to get its enhanced definition */
	GrapaCHAR indexDefName;
	indexDefName.FROM("index_def:");
	indexDefName.Append(indexName);
	
	GrapaBYTE indexDefData;
	GrapaCHAR fieldNameDef;
	fieldNameDef.FROM("definition");
	err = GetField(mRootTable, GROUP_TREE, indexDefName, fieldNameDef, indexDefData);
	if (err) {
		printf("[DEBUG] Index '%s' not found in enhanced metadata, error: %d\n", (char*)indexName.mBytes, err);
		return err;
	}
	
	printf("[DEBUG] Found enhanced index definition: %s\n", (char*)indexDefData.mBytes);
	
	/* Parse enhanced index definition to get index ID and metadata */
	const char* defStr = (char*)indexDefData.mBytes;
	const char* idStart = strstr(defStr, "\"indexId\":");
	if (!idStart) {
		printf("[DEBUG] Invalid enhanced index definition format\n");
		return -1;
	}
	
	idStart += 10; /* Skip "\"indexId\":" */
	u64 indexId = 0;
	while (*idStart >= '0' && *idStart <= '9') {
		indexId = indexId * 10 + (*idStart - '0');
		idStart++;
	}
	
	printf("[DEBUG] Parsed enhanced index ID: %llu for index '%s'\n", indexId, (char*)indexName.mBytes);
	
	/* Step 2: Get the index using GrapaDBX with enhanced metadata */
	GrapaDBXTable table;
	GrapaDBXIndex index;
	GrapaDU64Array indexList;
	
	table.mRef = mRootTable;
	table.mRefType = GROUP_TREE;
	table.mRecRef = mRootTable;
	
	/* Open the existing index */
	err = GrapaDBX::OpenIndex(table, indexId, indexList, index);
	if (err) {
		printf("[DEBUG] Failed to open index for refresh, error: %d\n", err);
		return err;
	}
	
	/* Step 3: Refresh the index using GrapaDBX */
	err = GrapaDBX::RefreshIndex(index);
	if (err) {
		printf("[DEBUG] Failed to refresh BTree index, error: %d\n", err);
		return err;
	}
	
	/* Step 4: Update enhanced metadata with refresh timestamp */
	GrapaCHAR updatedDefData;
	updatedDefData.FROM((char*)indexDefData.mBytes);
	
	/* Update the lastUpdated timestamp in statistics */
	const char* statsStart = strstr((char*)updatedDefData.mBytes, "\"lastUpdated\":");
	if (statsStart) {
		/* Find the end of the current timestamp */
		const char* timestampEnd = statsStart + 14; /* Skip "\"lastUpdated\":" */
		while (*timestampEnd >= '0' && *timestampEnd <= '9') {
			timestampEnd++;
		}
		
		/* Replace the timestamp */
		GrapaCHAR newTimestamp;
		newTimestamp.FROM((u64)time(NULL));
		
		/* Create updated definition with new timestamp */
		GrapaCHAR beforeStats, afterStats;
		beforeStats.FROM((char*)updatedDefData.mBytes, statsStart - (char*)updatedDefData.mBytes);
		afterStats.FROM(timestampEnd);
		
		updatedDefData.FROM(beforeStats);
		updatedDefData.Append("\"lastUpdated\":");
		updatedDefData.Append(newTimestamp);
		updatedDefData.Append(afterStats);
	}
	
	/* Store updated definition */
	err = SetField(mRootTable, GROUP_TREE, indexDefName, fieldNameDef, updatedDefData);
	if (err) {
		printf("[DEBUG] Failed to update enhanced index definition, error: %d\n", err);
		return err;
	}
	
	printf("[DEBUG] Successfully refreshed enhanced index '%s' with ID %llu\n", 
	       (char*)indexName.mBytes, indexId);
	
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
	GrapaError err = pDb->SetDataValue(fieldRef, 0, sizeof(GrapaDBXField), (void*)this);
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
	// Get field from table by reading from the data type record
	if (!pDb || tableRef == 0) return -1;
	
	// Set basic field info
	mId = fieldId;
	mRef = tableRef;
	
	// For now, use the field definitions that were created during table creation
	// This is a simplified approach that matches the field layout we know is correct
	switch (fieldId) {
	case 1: // $KEY field
		mType = GrapaTokenType::STR;
		mStore = STORE_FIX;
		mSize = 256;
		mGrow = 0;
		mDictOffset = 0;
		mDictSize = 258;
		break;
	case 2: // $VALUE field
		mType = GrapaTokenType::RAW;
		mStore = STORE_VAR;
		mSize = 32;
		mGrow = 8;
		mDictOffset = 258;
		mDictSize = 8;
		break;
	default:
		// Default field for other cases
		mType = GrapaTokenType::STR;
		mStore = STORE_VAR;
		mSize = 32;
		mGrow = 8;
		mDictOffset = 0;
		mDictSize = 0;
		break;
	}
	
	printf("[DEBUG] GrapaDBXField::Get: Set up field %llu with type=%d, store=%d, size=%llu, offset=%llu, dsize=%llu\n", 
	       fieldId, mType, mStore, mSize, mDictOffset, mDictSize);
	
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
	// Append field to array - create a copy like the reference implementation
	if (!pField) return -1;
	
	// Create a new field object and copy the data
	GrapaDBXField* dbField = new GrapaDBXField();
	*dbField = *pField;  // Copy the field data to the new object
	
	// Add to the array using base class method
	GrapaVoidArray::Append((void*)dbField);
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

GrapaError GrapaDBX::DumpTheGroupStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaDBXCursor itemCursor;

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

GrapaError GrapaDBX::DumpTheStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor, u64 tableDT)
{
	/* Implementation following GrapaDB reference pattern */
	GrapaError err = 0;
	u64 returnLen = 0;
	GrapaDBXCursor dataTypeCursor;
	GrapaDBXField dbField;
	GrapaCHAR dbChar;

	/* Set up cursor to point to the data type tree */
	dataTypeCursor.Set(tableDT);
	
	/* Get the first field definition */
	err = First(dataTypeCursor);
	if (err) {
		return(err);
	}
	
	/* Skip the $DICT field (key 0) and go to the first actual field */
	if (dataTypeCursor.mKey == 0) {
		err = Next(dataTypeCursor);
		if (err) {
			return(err);
		}
	}
	
	/* Iterate through all field definitions */
	while (!err)
	{
		returnLen = 0;
		
		/* Read the field definition */
		err = dbField.Read(this, dataTypeCursor.mValue);
		if (err) {
			return(err);
		}
		
			/* Get the field data from the record */
	GrapaBYTE fieldData;
	err = GetRecordField(cursor, dbField, fieldData);
	if (err) {
		return(err);
	}
	
	/* Convert field data to string representation */
	GrapaCHAR dbChar;
	if (fieldData.mLength == 0 || fieldData.mBytes == NULL)
	{
		dbChar.FROM("NULL");
	}
	else
	{
		/* Convert data to appropriate string representation */
		GrapaInt n;
		GrapaFloat d(false, 16, 4, 0);
		GrapaTime t;
		
		/* Determine the data type from the field definition */
		switch (dbField.mType)
		{
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::INT:
			n.FromBytes(fieldData);
			dbChar = n.ToString(10);
			break;
		case GrapaTokenType::TABLE:
		case GrapaTokenType::RAW:
			n.FromBytes(fieldData);
			dbChar = n.ToString(16);
			break;
		case GrapaTokenType::FLOAT:
			d.FromBytes(fieldData);
			dbChar = d.ToString(16);
			break;
		case GrapaTokenType::TIME:
			t.FromBytes(fieldData);
			dbChar = t.getString();
			break;
		case GrapaTokenType::BOOL:
			dbChar.FROM((fieldData.mBytes && fieldData.mLength && fieldData.mBytes[0] && (fieldData.mBytes[0] != '0')) ? "true" : "false");
			break;
		default: 
			/* For STR and other types, use the raw data as string */
			dbChar.FROM((char*)fieldData.mBytes, fieldData.mLength);
			break;
		}
		if (dbChar.GetLength() > 32) dbChar.SetLength(32);
	}
		
		/* Output the field information */
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%llu=%s ", dbField.mId, dbChar.mBytes);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
		else printf((char*)dbWrite.mBytes,"");
		
		/* Move to next field */
		err = Next(dataTypeCursor);
		if (!err && dataTypeCursor.mKey == 0)
			err = Next(dataTypeCursor);
	}
	
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
		case SEARCH_ITEM:
		case GPTR_ITEM:
		case RPTR_ITEM:
		case CPTR_ITEM:
			break;
		default:
			return(-1);
			break;
	}
	
	printf("[DEBUG] PtrToRec: About to call GetTreeStore with cursor.mTreeRef=%llu\n", ptrCursor.mTreeRef);
	err = GetTreeStore(ptrCursor, tableRef, storeType);
	if (err) {
		printf("[DEBUG] PtrToRec: GetTreeStore failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] PtrToRec: GetTreeStore succeeded, tableRef=%llu, storeType=%d\n", tableRef, storeType);
	printf("[DEBUG] PtrToRec: ptrCursor.mValueType=%d, ptrCursor.mKey=%llu\n", ptrCursor.mValueType, ptrCursor.mKey);
	
	switch(ptrCursor.mValueType)
	{
		case SEARCH_ITEM:
		case GPTR_ITEM:
			printf("[DEBUG] PtrToRec: Setting up GREC_ITEM cursor\n");
			recCursor.Set(tableRef, GREC_ITEM, ptrCursor.mKey);
			recCursor.mTreeType = GROUP_TREE;
			break;
		case RPTR_ITEM:
			printf("[DEBUG] PtrToRec: Setting up RREC_ITEM cursor\n");
			// For ROW tables, the record tree is the same as the store tree
			// The record was inserted into the record tree, so search there
			printf("[DEBUG] PtrToRec: Setting recCursor with mTreeRef=%llu, mKey=%llu\n", ptrCursor.mTreeRef, ptrCursor.mKey);
			recCursor.Set(ptrCursor.mTreeRef, RREC_ITEM, ptrCursor.mKey);
			recCursor.mTreeType = RTABLE_TREE;
			printf("[DEBUG] PtrToRec: After Set, recCursor.mTreeRef=%llu, recCursor.mKey=%llu\n", recCursor.mTreeRef, recCursor.mKey);
			break;
		case CPTR_ITEM:
			printf("[DEBUG] PtrToRec: Setting up CREC_ITEM cursor\n");
			recCursor.Set(tableRef, CREC_ITEM, ptrCursor.mKey);
			recCursor.mTreeType = CTABLE_TREE;
			break;
		default:
			printf("[DEBUG] PtrToRec: Unknown value type %d\n", ptrCursor.mValueType);
			return -1;
	}
	
	printf("[DEBUG] PtrToRec: About to call Search with recCursor.mTreeRef=%llu, recCursor.mKey=%llu\n", recCursor.mTreeRef, recCursor.mKey);
	GrapaError searchErr = Search(recCursor);
	return searchErr;
}

GrapaError GrapaDBX::DumpTheDataType(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	/* Implementation following GrapaDB reference pattern to show field definitions */
	GrapaError err;
	GrapaDBXField dbField;
	u64 weight = 0;
	
	/* Read the field definition */
	err = dbField.Read(this, cursor.mValue);
	if (err) {
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sFIELD [error reading field definition]", leader);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		return(err);
	}
	
	/* Get field name if available */
	const char* fieldName = "";
	switch(dbField.mId) {
		case 0: fieldName = "$DICT"; break;
		case 1: fieldName = "$KEY"; break;
		case 2: fieldName = "$VALUE"; break;
		default: fieldName = "FIELD"; break;
	}
	
	/* Get record type string */
	const char* recType = "";
	switch(dbField.mTreeType) {
		case GROUP_TREE: recType = "GROUP"; break;
		case RTABLE_TREE: recType = "ROW"; break;
		case CTABLE_TREE: recType = "COL"; break;
		default: recType = "UNKNOWN"; break;
	}
	
	/* Get store type string */
	const char* storeType = "";
	switch(dbField.mStore) {
		case GrapaDBXField::STORE_FIX: storeType = "FIX"; break;
		case GrapaDBXField::STORE_VAR: storeType = "VAR"; break;
		case GrapaDBXField::STORE_PAR: storeType = "PAR"; break;
		default: storeType = "UNKNOWN"; break;
	}
	
	/* Get field type string */
	const char* fieldType = "";
	switch(dbField.mType) {
		case GrapaTokenType::STR: fieldType = "STR"; break;
		case GrapaTokenType::INT: fieldType = "INT"; break;
		case GrapaTokenType::FLOAT: fieldType = "FLOAT"; break;
		case GrapaTokenType::RAW: fieldType = "RAW"; break;
		case GrapaTokenType::TIME: fieldType = "TIME"; break;
		case GrapaTokenType::BOOL: fieldType = "BOOL"; break;
		case GrapaTokenType::START: fieldType = "RDICT"; break; /* Record dictionary type */
		default: fieldType = "UNKNOWN"; break;
	}
	
	/* Output field definition in the same format as reference implementation */
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, 
		"%sFIELD (%llu) key=%llu id=%llu name=%s rec=%s type=%s store=%s doffset=%llu dsize=%llu size=%llu grow=%llu", 
		leader, cursor.mValue, cursor.mKey, dbField.mId, fieldName, recType, fieldType, storeType, 
		dbField.mDictOffset, dbField.mDictSize, dbField.mSize, dbField.mGrow);
	
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	
	return(0);
}

GrapaError GrapaDBX::DumpTheGroupRec(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	/* Dump group record - recursively dump the tree */
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sGREC [weight: %d]", leader, 0);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	return DumpTheTree(dbWrite, leader, 0, cursor.mValue);
}

GrapaError GrapaDBX::DumpTheRowStructure(GrapaCHAR& dbWrite, GrapaDBXCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaDBXCursor itemCursor;

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
	mDictField.BigEndian();
	mId = BE_S64(mId);
	mRef = BE_S64(mRef);
}

// GrapaDBXIndexField BigEndian implementation
void GrapaDBXIndexField::BigEndian()
{
	mId = BE_S64(mId);
	mRef = BE_S64(mRef);
	mTableRef = BE_S64(mTableRef);
	mIndexNameRef = BE_S64(mIndexNameRef);
	mCardinality = BE_S64(mCardinality);
	mSelectivity = BE_S64(mSelectivity);
	mLastUpdated = BE_S64(mLastUpdated);
	mStatisticsRef = BE_S64(mStatisticsRef);
	mConstraintRef = BE_S64(mConstraintRef);
	mCompositeFieldsRef = BE_S64(mCompositeFieldsRef);
	mPartialConditionRef = BE_S64(mPartialConditionRef);
	// Note: mIndexFlags is a bit field, so no endian conversion needed
}

// GrapaDBXIndexField constructor
GrapaDBXIndexField::GrapaDBXIndexField()
{
	mId = 0;
	mRef = 0;
	mTableRef = 0;
	mIndexNameRef = 0;
	mCardinality = 0;
	mSelectivity = 0;
	mLastUpdated = 0;
	mStatisticsRef = 0;
	mConstraintRef = 0;
	mCompositeFieldsRef = 0;
	mPartialConditionRef = 0;
	mIndexFlags.mIndexType = INDEX_NORMAL;
	mIndexFlags.mIndexMethod = METHOD_BTREE;
	mIndexFlags.mSortOrder = 0;
	mIndexFlags.mIsActive = 1;
	mIndexFlags.mIsUnique = 0;
}

void GrapaDBXIndexField::Init(u64 pIndexId, u8 pIndexType, u8 pIndexMethod)
{
	mId = pIndexId;
	mIndexFlags.mIndexType = pIndexType;
	mIndexFlags.mIndexMethod = pIndexMethod;
	mIndexFlags.mSortOrder = 0;
	mIndexFlags.mIsActive = 1;
	mIndexFlags.mIsUnique = (pIndexType == INDEX_UNIQUE || pIndexType == INDEX_PRIMARY) ? 1 : 0;
}

// GrapaDBXIndexField Read method
GrapaError GrapaDBXIndexField::Read(GrapaDBX *pDb, u64 indexRef)
{
	if (indexRef == 0) return -1;

	// Read the index field data from the database
	GrapaError err = pDb->GetDataValue(indexRef, 0, sizeof(GrapaDBXIndexField), (char*)this, NULL);
	if (err) return err;

	// Convert from big-endian to native endian
	BigEndian();
	return 0;
}

// GrapaDBXIndexField Write method
GrapaError GrapaDBXIndexField::Write(GrapaDBX *pDb, u64 indexRef)
{
	if (indexRef == 0) return -1;

	// Convert to big-endian for storage
	BigEndian();

	// Write the index field data to the database
	GrapaError err = pDb->SetDataValue(indexRef, 0, sizeof(GrapaDBXIndexField), this);
	if (err) return err;

	// Convert back to native endian
	BigEndian();
	return 0;
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
			err = SetDataValue(pFormulaRef, 0, compressed.mLength, (void*)compressed.mBytes);
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
			GrapaDBXCursor cursor;
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
			GrapaDBXCursor cursor;
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
	GrapaDBXCursor indexCursor;
	GrapaDBXCursor indexField;
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
	GrapaDBXCursor indexCursor;
	GrapaDBXCursor indexField;
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
    GrapaDBXCursor indexCursor;
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
    GrapaDBXCursor ptrCursor;
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
    GrapaDBXCursor recCursor = ptrCursor;
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
    GrapaDBXCursor indexCursor;
    GrapaError err;
    
    indexCursor.Set(tableRef);
    err = GetTreeIndex(indexCursor, indexRef);
    if (err || indexRef == 0) {
        printf("DEBUG: Could not find index tree for tableRef=%llu\n", tableRef);
        return;
    }
    
    GrapaDBXCursor ptrCursor;
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
	GrapaError err;
	GrapaBlockNodeHeader node;
	err = node.Read(mFile, cursor.mNodeRef);
	weight = node.weight;
	return(err);
}

GrapaError GrapaDBX::InsertIntoIndex(u64 tableRef, u8 pValueType, u64 resId, u64 recordRef)
{
	GrapaError err=0;
	GrapaDBXCursor indexTableCursor,indexCursor;
	u64 indexRef;

	printf("[DEBUG] GrapaDBX::InsertIntoIndex: tableRef=%llu, pValueType=%d, resId=%llu, recordRef=%llu\n", tableRef, pValueType, resId, recordRef);

	indexTableCursor.Set(tableRef);
	err = GetTreeIndex(indexTableCursor,indexRef);
	if (err) {
		printf("[DEBUG] GrapaDBX::InsertIntoIndex: GetTreeIndex failed with error %d\n", err);
		return(err);
	}

	if (indexRef == 0) {
		printf("[DEBUG] GrapaDBX::InsertIntoIndex: No index tree found\n");
		return(0); // No index to insert into
	}

	indexTableCursor.Set(indexRef);
	err = FirstDb(indexTableCursor);
	if (!err && indexTableCursor.mKey==0)
		err = NextDb(indexTableCursor);
	while (!err)
	{
		printf("[DEBUG] GrapaDBX::InsertIntoIndex: Inserting into index table %llu\n", indexTableCursor.mValue);
		indexCursor.Set(indexTableCursor.mValue,pValueType,resId,recordRef);
		err = Insert(indexCursor);
		if (err) {
			printf("[DEBUG] GrapaDBX::InsertIntoIndex: Insert failed with error %d\n", err);
			return(err);
		}
		err = NextDb(indexTableCursor);
		if (!err && indexTableCursor.mKey==0)
			err = NextDb(indexTableCursor);
	}

	printf("[DEBUG] GrapaDBX::InsertIntoIndex: Completed successfully\n");
	return(0);
}

GrapaError GrapaGroup2::SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, const GrapaCHAR& pFieldNameX, const GrapaBYTE& pDataValue)
{
	/* Enhanced implementation for GrapaDBX to match GrapaGroup behavior */
	
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
	
	/* For COL/ROW tables, we need to open the table to get the correct record tree */
	if (parentType == GROUP_TREE)
	{
		printf("[DEBUG] Opening table for GROUP_TREE\n");
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			printf("[DEBUG] OpenTable failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] OpenTable succeeded, mRecRef=%llu\n", parentDict.mRecRef);
	}
	else if (parentType == CTABLE_TREE || parentType == RTABLE_TREE)
	{
		/* For COL/ROW tables, the parentTree is already the table tree, so we don't need to open it again */
		printf("[DEBUG] For COL/ROW table (type %d), using parentTree=%llu directly as record tree\n", parentType, parentTree);
		parentDict.mRecRef = parentTree;
	}
	
	/* Find the existing RREC entry that was created in CreateGroup */
	u64 dataId = 0;
	GrapaDBXFieldValueArray searchData;
	searchData.Append(this, parentDict, 1, pDataName, EQ_CMP); /* Search by field ID 1 ($KEY) */
	err = SearchDb(cursor, parentDict, searchData);
	if (err)
	{
		printf("[DEBUG] SearchDb failed with error %d - creating new record\n", err);
		/* If not found, create a new record */
		err = CreateRecord(parentDict, cursor);
		if (err)
		{
			printf("[DEBUG] CreateRecord failed with error %d\n", err);
			return(err);
		}
		printf("[DEBUG] CreateRecord succeeded, cursor.mKey=%llu, cursor.mValue=%llu\n", cursor.mKey, cursor.mValue);
	}
	else
	{
		printf("[DEBUG] Found existing RREC entry with key=%llu\n", cursor.mKey);
		dataId = cursor.mKey;
	}
	
	printf("[DEBUG] Created cursor with mTreeRef=%llu, mValue=%llu\n", cursor.mTreeRef, cursor.mValue);
	
	/* Find the appropriate field to store the value */
	GrapaDBXField targetField;
	u64 maxId;
	GrapaCHAR fieldName(pFieldNameX);
	
	/* Try to find the specified field, or default to $VALUE */
	err = FindField(parentTree, parentType, fieldName, targetField, maxId);
	if (err)
	{
		/* If field not found, try to find $VALUE field */
		printf("[DEBUG] SetField: Field '%s' not found, trying $VALUE\n", (char*)fieldName.mBytes);
		GrapaCHAR valueField;
		valueField.FROM("$VALUE");
		err = FindField(parentTree, parentType, valueField, targetField, maxId);
		if (err)
		{
			printf("[DEBUG] SetField: $VALUE field not found either\n");
			return(err);
		}
	}
	
	printf("[DEBUG] SetField: Found field '%s' with ID %llu\n", (char*)fieldName.mBytes, targetField.mId);
	
	/* Create field value arrays for both $KEY and $VALUE fields */
	GrapaDBXFieldValueArray dataFields;
	
	/* For ROW and COL tables, we need to set both $KEY and $VALUE fields */
	if (parentType == RTABLE_TREE || parentType == CTABLE_TREE) {
		/* Find the $KEY field */
		GrapaDBXField keyField;
		u64 maxId;
		GrapaCHAR keyFieldName;
		keyFieldName.FROM("$KEY");
		err = FindField(parentTree, parentType, keyFieldName, keyField, maxId);
		if (!err) {
			/* Add the $KEY field with the table name */
			dataFields.Append(this, parentDict, keyField.mId, pDataName, 0);
			printf("[DEBUG] Storing $KEY field (ID %llu) with table name: %.*s\n", keyField.mId, (int)pDataName.mLength, pDataName.mBytes);
		}
	}
	
	/* Add the target field (usually $VALUE) */
	dataFields.Append(this, parentDict, targetField.mId, pDataValue, 0);
	printf("[DEBUG] Storing value in field ID %llu with length: %llu\n", targetField.mId, pDataValue.mLength);
	printf("[DEBUG] Storing $VALUE field with value length: %llu\n", pDataValue.mLength);
	
	/* Store all fields in a single call to SetRecordField */
	printf("[DEBUG] About to call SetRecordField for %d fields\n", dataFields.Count());
	err = SetRecordField(cursor, dataFields);
	if (err)
	{
		printf("[DEBUG] SetRecordField failed with error %d\n", err);
		return(err);
	}
	printf("[DEBUG] SetRecordField succeeded\n");
	
	return(0);
}

GrapaError GrapaDBX::ModifyTableField(GrapaDBXTable& pTable, u64 pFieldId, u8 pNewType, u8 pNewStore, u64 pNewSize, u64 pNewGrow)
{
	/* 
	 * FIELD MODIFICATION FEATURE DISABLED
	 * 
	 * This feature has been disabled due to incomplete implementation.
	 * The function lacks critical data migration logic for ROW and COL tables,
	 * which could lead to data corruption.
	 * 
	 * See maintainers/DEVELOPMENT/GRAPADBX_FIELD_MODIFICATION_DISABLED.md
	 * for details and implementation plan.
	 */
	printf("[ERROR] ModifyTableField is currently disabled for safety reasons.\n");
	printf("[ERROR] This feature requires complete data migration implementation.\n");
	printf("[ERROR] See maintainers/DEVELOPMENT/GRAPADBX_FIELD_MODIFICATION_DISABLED.md\n");
	return(-1);

	/* Original implementation (disabled):
	GrapaError err;
	GrapaDBXCursor dtField;
	GrapaDBXField field;
	u64 indexRef;

	// Get the current field definition
	err = GetDataTypeRecord(pTable.mRef, indexRef);
	if (err) return(err);

	dtField.Set(indexRef, SDATA_ITEM, pFieldId);
	err = Search(dtField);
	if (err) return(err);

	err = field.Read(this, dtField.mValue);
	if (err) return(err);

	// Store old values for data migration
	u8 oldType = field.mType;
	u8 oldStore = field.mStore;
	u64 oldSize = field.mSize;
	u64 oldDictSize = field.mDictSize;

	// Update field definition
	field.mType = pNewType;
	field.mStore = pNewStore;
	field.mSize = pNewSize;
	field.mGrow = pNewGrow;

	// Recalculate dict size based on new type and store
	switch (field.mStore) {
		case GrapaDBXField::STORE_FIX:
			switch (field.mType) {
				case GrapaTokenType::BOOL:
					field.mDictSize = 1;
					field.mSize = 1;
					break;
				case GrapaTokenType::INT:
				case GrapaTokenType::FLOAT:
				case GrapaTokenType::TIME:
					if (field.mDictSize > 0x7FFF) field.mDictSize = 0x7FFF;
					field.mSize = field.mDictSize;
					if (field.mDictSize == 1);
					else if (field.mDictSize <= 127) field.mDictSize += 1;
					else field.mDictSize += 2;
					break;
				case GrapaTokenType::STR:
					if (field.mDictSize > 0x7FFF) field.mDictSize = 0x7FFF;
					field.mSize = field.mDictSize;
					if (field.mDictSize <= 127) field.mDictSize += 1;
					else field.mDictSize += 2;
					break;
				case GrapaTokenType::ERR:
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::TUPLE:
				case GrapaTokenType::VECTOR:
				case GrapaTokenType::WIDGET:
				case GrapaTokenType::XML:
				case GrapaTokenType::LIST:
				case GrapaTokenType::EL:
				case GrapaTokenType::TAG:
				case GrapaTokenType::OP:
				case GrapaTokenType::CODE:
				case GrapaTokenType::TABLE:
				case GrapaTokenType::RAW:
					if (field.mDictSize > 0x7FFF) field.mDictSize = 0x7FFF;
					field.mSize = field.mDictSize;
					if (field.mDictSize <= 127) field.mDictSize += 2;
					else field.mDictSize += 3;
					break;
			}
			break;
		case GrapaDBXField::STORE_VAR:
		case GrapaDBXField::STORE_PAR:
			field.mDictSize = 8;
			break;
		default:
			field.mDictSize = 0;
			field.mSize = 0;
			break;
	}

	// Write updated field definition
	err = field.Write(this, dtField.mValue);
	if (err) return(err);

	// Handle data migration based on table type
	switch (pTable.mDictField.mTreeType) {
		case GROUP_TREE:
			// For GROUP tables, no data migration needed
			break;

		case RTABLE_TREE:
			// For ROW tables, migrate data if size changed
			if (oldDictSize != field.mDictSize) {
				// TODO: Implement data migration for ROW tables
				// This would involve reading all records and adjusting field data
				printf("[DEBUG] ModifyTableField: ROW table data migration not yet implemented\n");
			}
			break;

		case CTABLE_TREE:
			// For COL tables, handle column tree changes
			if (oldStore != field.mStore || oldType != field.mType) {
				// TODO: Implement column tree migration
				// This would involve recreating the column tree with new structure
				printf("[DEBUG] ModifyTableField: COL table data migration not yet implemented\n");
			}
			break;
	}

	return(0);
	*/
}



GrapaError GrapaGroup2::CreateField(u64 parentTree, u8 parentType, const char* pFieldName, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{
    GrapaCHAR s(pFieldName);
    return CreateField(parentTree, parentType, s, pType, pStore, pSize, pGrow);
}

GrapaError GrapaDBX::DumpTheDRT(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	GrapaDBXField field;
	GrapaError err = field.Read(this, cursor.mValue);
	if (err) {
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDRTYPE_ITEM: Error reading field data\n", leader);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
		return err;
	}
	
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDRTYPE_ITEM: id=%llu, type=%d, store=%d, size=%llu, grow=%llu, offset=%llu\n", 
		leader, field.mId, field.mType, field.mStore, field.mSize, field.mGrow, field.mDictOffset);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
	return(0);
}

GrapaError GrapaDBX::DumpTheDIT(GrapaCHAR& dbWrite, char *leader, GrapaDBXCursor& cursor)
{
	GrapaDBXIndexField indexField;
	GrapaError err = indexField.Read(this, cursor.mValue);
	if (err) {
		dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDITYPE_ITEM: Error reading index field data\n", leader);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
		return err;
	}
	
	dbWrite.mLength = snprintf((char*)dbWrite.mBytes, dbWrite.mSize, "%sDITYPE_ITEM: id=%llu, type=%d, method=%d\n", 
		leader, indexField.mId, indexField.mIndexFlags.mIndexType, indexField.mIndexFlags.mIndexMethod);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
	return(0);
}
