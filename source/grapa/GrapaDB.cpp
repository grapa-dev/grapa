// GrapaDB.cpp 
/*
Copyright 2022 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "GrapaDB.h"
#include "GrapaFileIO.h"
#include "GrapaInt.h"
#include "GrapaFloat.h"
#include "GrapaMem.h"

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::Create(const char *fileName, u8 treeType, u64& firstTree)
{
	firstTree = 0;
	GrapaError err = GrapaCore::Create(fileName);
	if (err) return(err);
	err = CreateRoot(treeType, firstTree);
	if (err) RemoveFile(fileName);
	return(err);
}

GrapaError GrapaDB::OpenFile(const char *fileName, char mode)
{
	return GrapaCore::OpenFile(fileName, mode);
}

u64 GrapaDB::RootTree(u8& pNewType)
{
	return GrapaCore::RootTree(pNewType);
}

GrapaError GrapaDB::CreateRoot(u8 treeType, u64& firstTree)
{
	GrapaError err;
	firstTree = 0;

	//err = NewTree(firstTree, treeType);
	GrapaDBTable table;
	err = CreateTable(0, treeType, 0, table);
	firstTree = table.mRef;

	if (!err)
		err = SetFileTree(firstTree);
	if (err)
	{
		CloseFile();
		return(err);
	}

	return(0);
}

u64 GrapaDB::RootTree(GrapaCHAR& pNewType)
{
	u8 rootType = 0;
	u64 rootTree = GrapaCore::RootTree(rootType);
	switch (rootType)
	{
	case GrapaDB::GROUP_TREE:	pNewType.FROM("GROUP"); break;
	case GrapaDB::RTABLE_TREE:	pNewType.FROM("ROWS"); break;
	case GrapaDB::CTABLE_TREE:	pNewType.FROM("COLS"); break;
	case GrapaDB::SDATA_TREE: pNewType.FROM("TREE"); break;
	default:  pNewType.FROM("OLDTREE"); break;
	}
	return(rootTree);
}

GrapaError GrapaDB::CloseFile()
{
	return GrapaCore::CloseFile();
}

GrapaError GrapaDB::NewTree(u64& treePtr, u8 treeType, u64 parentTree, u8 nodeCount)
{
	GrapaError err;
	GrapaDBTable table;

	err = GrapaCore::NewTree(treePtr,treeType,parentTree,nodeCount); 
	if (err) return(err);

	switch (treeType)
	{
		case GROUP_TREE:
			//ensure to call the inhereted function, or you get into an infinate loop
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

GrapaError GrapaDB::DumpFile(GrapaFile *pDumpFile)
{
	return DumpTree(0, pDumpFile);
}

GrapaError GrapaDB::DumpTree(u64 pTreeRef, GrapaFile *pDumpFile)
{
	GrapaError err = 0;
	GrapaFile *oldDumpFile = mDumpFile;

	if (pDumpFile)
		mDumpFile = pDumpFile;

	GrapaCHAR dbWrite;
	dbWrite.GrowSize(40000, false);
	dbWrite.SetLength(0);

	if (pTreeRef == 0) pTreeRef = FirstTree(0);
	err = DumpTheTree(dbWrite, "", 0, pTreeRef);

	mDumpFile = oldDumpFile;

	return err;
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::LastTableId(u64 firstTree, u64& pTableId)
{
	GrapaError err;
	GrapaCursor dtCursor;
	u64 itemCount;
	pTableId = 1;
	dtCursor.Set(firstTree);
	err = GetTreeSize(dtCursor, itemCount);
	if (err) return(err);
	if (itemCount == 0) return(0);
	err = Last(dtCursor);
	if (err) return(err);
	pTableId = dtCursor.mKey + 1;
	return(0);
}

GrapaError GrapaDB::CreateTable(u64 firstTree, u8 pTreeType, u64 pTableId, GrapaDBTable& pTable)
{
	GrapaError err;
	GrapaCursor tableNames,tableNamesDT;
	GrapaDBField dbField;
	GrapaDBIndex dbIndex;
	GrapaDBTable parentDict;

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
		tableNames.Set(firstTree, GREC_ITEM, pTableId, pTable.mRef);
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

GrapaError GrapaDB::OpenTable(u64 firstTree, u64 pTableId, GrapaDBTable& pTable)
{
	GrapaError err;
	GrapaCursor tableCursor,indexTableCursor,indexCursor;
	GrapaDBTable dictTable;
	GrapaDBFieldValueArray data;
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
		switch (pTable.mRefType)
		{
			case GROUP_TREE:
				tableCursor.Set(pTable.mRef);
				err = Search(tableCursor); // go to 0 item
				if (err) return(err);
				pTable.mRecRef = tableCursor.mValue;
				break;
		}
	}

	err = GetDataTypeRecord(pTable.mRecRef, indexRef);
	if (err) return(err);
	tableCursor.Set(indexRef);
	err = Search(tableCursor); // go to 0 item
	if (err) return(err);
	err = pTable.mDictField.Read(this, tableCursor.mValue);
	if (err) return(err);

	return(0);
}

GrapaError GrapaDB::DeleteTable(u64 firstTree, u64 pTableId)
{
	GrapaError err;
	GrapaCursor tableCursor;
	GrapaCursor cursor;
	GrapaDBTable table;
	u64 indexTree,storeTree;
	u8 storeType;

	err = OpenTable(firstTree,pTableId,table);
	if (err) return(err);

	tableCursor.Set(table.mRecRef);
	err = GetTreeIndex(tableCursor,indexTree);
	if (err) return(err); // the index tree should alread exist for all tables

	tableCursor.Set(indexTree);
	err = GetTreeStore(tableCursor,storeTree,storeType);
	if (err) return(err);
	tableCursor.Set(storeTree);
	err = First(tableCursor);
	while (!err)
	{
		cursor.Set(tableCursor.mKey,GPTR_ITEM,tableCursor.mValue); // mValue is the mId in this case
		err = Search(cursor);
		if (!err)
		{
			cursor.mValue = 0;
			err = Update(cursor);
		}
		err = Next(tableCursor);
	}

	tableCursor.Set(firstTree,TREE_ITEM,table.mId);
	err = Delete(tableCursor);
	if (err) return(err);

	tableCursor.Set(firstTree,TREE_ITEM);
	err = Search(tableCursor);
	if (err) return(err);
	cursor.Set(tableCursor.mValue,RREC_ITEM,table.mId);
	err = Delete(cursor);
	if (err) return(err);

	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::CreateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBTable& pTable)
{
	GrapaError err;
	GrapaCursor tableNames;
	GrapaDBTable parentDict;
	u64 indexTree,storeTree;
	u8 storeType;

	pTable.mId = pTableId;
	pTable.mRef = 0;
	pTable.mRecRef = 0;
	pTable.mRefType = 0;

	err = OpenTable(pAliasFirstTree,pAliasTableId,pTable);
	if (!err) return(-1);

	err = OpenTable(pFirstTree,pTableId,pTable);
	if (err) return(err);

	pTable.mId = pAliasTableId;
	pTable.mRecRef = pTable.mRef;

	tableNames.Set(pAliasFirstTree,GPTR_ITEM,pAliasTableId,pTable.mRef);
	err = Insert(tableNames);
	if (err)
	{
		pTable.mRef = 0;
		pTable.mRecRef = 0;
		pTable.mRefType = 0;
		return(err);
	}

	if (pAliasTableId)
	{
		err = OpenTable(pAliasFirstTree,0,parentDict);
		if (err) return(err);
		tableNames.Set(parentDict.mRecRef,RREC_ITEM,pAliasTableId);
		err = CreateRecord(parentDict,tableNames);
		if (err) return(err);
	}

	tableNames.Set(pTable.mRecRef);
	err = GetTreeIndex(tableNames,indexTree);
	if (err) return(err); // the index tree should alread exist for all tables

	tableNames.Set(indexTree);
	err = GetTreeStore(tableNames,storeTree,storeType);
	if (err) return(err);
	if (storeTree==0)
	{
		err = NewTree(storeTree,SU64_TREE,indexTree);
		if (err) return(err);
		err = SetTreeStore(tableNames,storeTree,DATA_STORE);
		if (err) return(err);
	}
	tableNames.Set(storeTree,SU64_ITEM,pAliasFirstTree,pAliasTableId);  //need to put fistTree in the key
	err = Insert(tableNames);
	if (err) return(err);

	return(0);
}

GrapaError GrapaDB::UpdateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBTable& pTable)
{
	GrapaError err;
	GrapaCursor tableNames;
	GrapaDBTable parentDict;
	u64 indexTree,storeTree;
	u8 storeType;

	// First lookup the alias reference and go to the reference
	tableNames.Set(pAliasFirstTree,GPTR_ITEM,pAliasTableId);
	err = Search(tableNames);
	if (err) return(err);

	// Go to the reference and remove the dependancy
	tableNames.Set(tableNames.mValue);
	err = GetTreeIndex(tableNames,indexTree);
	if (err) return(err); // the index tree should alread exist for all tables

	tableNames.Set(indexTree);
	err = GetTreeStore(tableNames,storeTree,storeType);
	if (err) return(err);
	if (storeTree)
	{
		tableNames.Set(storeTree,SU64_ITEM,pAliasFirstTree);
		err = Delete(tableNames);
		if (err) return(err);
	}

	// Then add the dependancy on the new target

	err = OpenTable(pFirstTree,pTableId,pTable);
	if (err) return(err);

	pTable.mId = pAliasTableId;
	pTable.mRecRef = pTable.mRef;

	tableNames.Set(pAliasFirstTree,GPTR_ITEM,pAliasTableId);
	err = Search(tableNames);
	if (err)
	{
		pTable.mRef = 0;
		pTable.mRecRef = 0;
		pTable.mRefType = 0;
		return(err);
	}
	tableNames.mValue = pTable.mRef;
	err = Update(tableNames);

	tableNames.Set(pTable.mRecRef);
	err = GetTreeIndex(tableNames,indexTree);
	if (err) return(err); // the index tree should alread exist for all tables

	tableNames.Set(indexTree);
	err = GetTreeStore(tableNames,storeTree,storeType);
	if (err) return(err);
	if (storeTree==0)
	{
		err = NewTree(storeTree,SU64_TREE,indexTree);
		if (err) return(err);
		err = SetTreeStore(tableNames,storeTree,DATA_STORE);
		if (err) return(err);
	}
	tableNames.Set(storeTree,SU64_ITEM,pAliasFirstTree,pAliasTableId);  //need to put fistTree in the key
	err = Insert(tableNames);
	if (err) return(err);

	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::FirstFreeId(u64 tableRef, u64 minId, u64& resId)
{
	GrapaError err = LastTableId(tableRef, resId);
	if (resId < minId) resId = minId;
	if (err) return(err);
	return(0);
}

GrapaError GrapaDB::CreateTableField(GrapaDBTable& pTable, GrapaDBField& pField, const GrapaCHAR& pName)
{
	GrapaError err;
	GrapaCursor tableNames;
	u64 indexRef;
	GrapaCursor dtCursor;
	u64 recordCount;
	u64 storeTree;
	u8 storeType;

	err = OpenTableField(pTable,pField.mId,pField);
	// if replace, then delete and continue
	if (!err) return(-1);

	tableNames.Set(pTable.mRecRef);
	err = SetTreeDirty(tableNames,true);

	tableNames.Set(pTable.mRecRef);
	err = GetTreeIndex(tableNames,indexRef);
	if (err) return(err);

	tableNames.Set(indexRef);
	err = Search(tableNames);
	if (err) return(err);

	if (pField.mId)
	{
		pField.mDictOffset = pTable.mDictField.mDictSize;
		pField.mTreeType = pTable.mDictField.mTreeType;
	}

	switch (pField.mStore)
	{
	case GrapaDBField::STORE_FIX:
		switch (pField.mType)
		{
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
			else if (pField.mDictSize <= 127) pField.mDictSize += 1; // 1 byte for header, 1 bit isNull, 7 bits for length (0..127)
			else pField.mDictSize += 2;
			break;
		case GrapaTokenType::STR: // str will store 2x bytes for unicode16
			if (pField.mDictSize > 0x7FFF) pField.mDictSize = 0x7FFF;
			pField.mSize = pField.mDictSize;
			if (pField.mDictSize <= 127) pField.mDictSize += 1; // 1 byte for header, 1 bit isNull, 7 bits for length (0..127)
			else pField.mDictSize += 2; // 2 bytes for header, 1 bit isNull, 15 bits for length (0..32767)
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::ARRAY:
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
			if (pField.mDictSize > 0x7FFF) pField.mDictSize = 0x7FFF;
			pField.mSize = pField.mDictSize;
			if (pField.mDictSize <= 127) pField.mDictSize += 2; // 1 byte for header, 1 bit isNull, 7 bits for length (0..127)
			else pField.mDictSize += 3; // 2 bytes for header, 1 bit isNull, 15 bits for length (0..32767)
			break;
		};
		break;
	case GrapaDBField::STORE_VAR:
	case GrapaDBField::STORE_PAR:
		pField.mDictSize = 8;
		break;
	default:
		pField.mDictSize = 0;
		pField.mSize = 0;
		break;
	};

	err = NewData(BYTE_DATA,pTable.mRecRef,pField.GetSize(),0,0,pField.mRef,true);
	if (err) return(err);
	if (pName.mLength && pName.mBytes)
	{
		err = NewData(BYTE_DATA, pTable.mRecRef, pName.mLength, 0, 0, pField.mNameRef, false);
		if (err) return(err);
		err = SetDataValue(pField.mNameRef, 0, pName.mLength, (char*)pName.mBytes);
		if (err) return(err);
	}
	err = pField.Write(this,pField.mRef);

	dtCursor.Set(tableNames.mValue,DTYPE_ITEM,pField.mId,pField.mRef);
	err = Insert(dtCursor);
	if (err) return(err);

	if (pField.mId)
	{
		dtCursor.Set(tableNames.mValue);
		err = Search(dtCursor);
		if (err) return(err);
		err = pTable.mDictField.Read(this,dtCursor.mValue);
		if (err) return(err);
		pTable.mDictField.mDictOffset += 1;
		pTable.mDictField.mDictSize += pField.mDictSize;
		err = pTable.mDictField.Write(this,dtCursor.mValue);
		if (err) return(err);
	}

	if (pField.mId==0)
		pTable.mDictField = pField;

	//indexes should be off while the fields are being filled in
	//should really be setting the correct field values here instead of just nulling.
	//unfilled data should be nulled out to help the compress (future) function out

	// Cycle through all the records ensuring the structure is setup

	switch (pTable.mDictField.mTreeType)
	{
		case SU64_TREE:
		case GROUP_TREE:
		//case ITABLE_TREE:
		case RTABLE_TREE:
			{
				//null out the field, create the space
				GrapaDBFieldValueArray fieldList(1);
				// Need a NULL object, and use this to set all the items
				GrapaBYTE blankByte;
				fieldList.Append(this,pTable,pField.mId,blankByte);
				dtCursor.Set(pTable.mRecRef);
				err = First(dtCursor);
				while(!err)
				{
					SetRecordField(dtCursor,fieldList);
					err = Next(dtCursor);
				}
			}
			break;
		case CTABLE_TREE:
			// Create the field item (DATA OR TREE)
			switch(pField.mType)
			{
				case GrapaTokenType::START:
					// For now, DICT fields are only stored in RTABLE_TREE.
					// In any case, I don't think the DICT field creating makes it to this routine...
					// Should verify.
					break;

				case GrapaTokenType::ERR:
				case GrapaTokenType::RAW:
				case GrapaTokenType::BOOL:
				case GrapaTokenType::INT:
				case GrapaTokenType::FLOAT:
				case GrapaTokenType::STR:
				case GrapaTokenType::TIME:
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::VECTOR:
				case GrapaTokenType::WIDGET:
				case GrapaTokenType::XML:
				case GrapaTokenType::LIST:
				case GrapaTokenType::EL:
				case GrapaTokenType::TAG:
				case GrapaTokenType::OP:
				case GrapaTokenType::CODE:
				case GrapaTokenType::TABLE:

					// create the pField.mId SDATA_ITEM in dataTree
					tableNames.Set(pTable.mRecRef);
					err = GetTreeStore(tableNames,storeTree,storeType);
					if (err) return(err);

					if (storeTree==0)
					{
						err = NewTree(storeTree,SU64_TREE,pTable.mRecRef);
						if (err) return(err);
						err = SetTreeStore(tableNames,storeTree,DATA_STORE);
						if (err) return(err);
					}

					tableNames.Set(storeTree);
					err = GetTreeSize(tableNames,recordCount);
					if (err) return(err);

					switch(pField.mStore)
					{
						case GrapaDBField::STORE_VAR:
						case GrapaDBField::STORE_PAR:
							err = NewTree(indexRef, SDATA_TREE, storeTree);
							if (err) return(err);
							tableNames.Set(storeTree,TREE_ITEM,pField.mId,indexRef); 
							err = Insert(tableNames);
							if (err) return(err);
							break;
						case GrapaDBField::STORE_FIX:
							err = NewData(FREC_DATA,storeTree,0,pField.mDictSize,pField.mGrow,indexRef,true);
							if (err) return(err);
							tableNames.Set(storeTree,SDATA_ITEM,pField.mId,indexRef); 
							err = Insert(tableNames);
							if (err) return(err);
							break;
					}

					break;

				default:
					return(-1);
			}
			break;
	}

	tableNames.Set(pTable.mRecRef);
	err = SetTreeDirty(tableNames,false);

	return 0;
}

GrapaError GrapaDB::OpenTableField(GrapaDBTable& pTable, u64 pFieldId, GrapaDBField& pField)
{
	GrapaError err = 0;
	GrapaCursor tableNames;
	u64 indexRef;
	GrapaCursor dtField;

	tableNames.Set(pTable.mRecRef);
	err = GetTreeIndex(tableNames,indexRef);
	if (err) return(err);

	tableNames.Set(indexRef,TREE_ITEM);
	err = Search(tableNames);
	if (err) return(err);

	dtField.Set(tableNames.mValue,SDATA_ITEM,pFieldId);
	err = Search(dtField);
	if (err) return(err);
	err = pField.Read(this,dtField.mValue);
	return(err);
}

GrapaError GrapaDB::OpenTableFieldList(GrapaDBTable& pTable, GrapaDBFieldArray& pFieldList)
{
	GrapaError err = 0;
	GrapaCursor tableNames;
	u64 indexRef;
	GrapaCursor dtField;
	GrapaDBField field;
	u64 fieldCount=0;

	tableNames.Set(pTable.mRecRef);
	err = GetTreeIndex(tableNames,indexRef);
	if (err) return(err);

	tableNames.Set(indexRef);
	err = Search(tableNames);
	if (err) return(err);

	err = GetTreeSize(tableNames,fieldCount);
	pFieldList.SetLength(0);
	pFieldList.SetSize(fieldCount);
	dtField.Set(tableNames.mValue);
	err = First(dtField);
	while (!err)
	{
		pFieldList.Append(this,pTable,dtField.mKey);
		err = Next(dtField);
	}

	return(0);
}

GrapaError GrapaDB::DeleteTableField(GrapaDBTable& pTable, u64 pFieldId)
{
	GrapaError err;
	GrapaCursor indexCursor, tableCursor;
	u64 indexRef;

	//if used in any indexes, fail...need to drop indexes first
	indexCursor.Set(pTable.mRecRef);
	err = GetTreeIndex(indexCursor,indexRef);
	if (err) return(err);
	if (indexRef==0) return(-1);

	indexCursor.Set(indexRef);
	err = First(indexCursor);
	if (!err && indexCursor.mKey==0)
		err = Next(indexCursor); //skip over the DICT in the index
	while (!err)
	{
		GrapaCursor indexFieldCursor;
		u64 indexFieldsRef;
		indexFieldCursor.Set(indexCursor.mValue);
		err = GetTreeIndex(indexFieldCursor,indexFieldsRef);
		if (indexFieldsRef)
		{
			indexFieldCursor.Set(indexFieldsRef);
			err = First(indexFieldCursor);
			while (!err)
			{
				//if used in any indexes, fail...need to drop indexes first
				if (indexFieldCursor.mValue == pFieldId)
					return(-1);
				err = Next(indexFieldCursor);
			}
		}
		err = Next(indexCursor);
		if (!err && indexCursor.mKey==0)
			err = Next(indexCursor); //skip over the DICT in the index
	}

	GrapaCursor tableNames;
	GrapaCursor dtField;
	GrapaDBField field;
	u64 fieldCount=0;
	u64 fromOffset=0,toOffset=0;
	u64 fieldHeadRef = 0;
	u8 fieldType = 0;
	u8 fieldStore = 0;
	u64 storeTree;
	u8 storeType;

	tableNames.Set(indexRef);
	err = Search(tableNames);
	if (err) return(err);

	err = GetTreeSize(tableNames,fieldCount);

	dtField.Set(tableNames.mValue);
	err = First(dtField);
	while (!err)
	{
		err = field.Read(this,dtField.mValue);
		if (field.mId == 0)
		{
			pTable.mDictField = field;
			fieldHeadRef = dtField.mValue;
		}
		else if (field.mId == pFieldId)
		{
			toOffset = field.mDictOffset;
			fromOffset = field.mDictOffset + field.mDictSize;
			fieldType = field.mType; // needed for CTABLE_TREE processing
			fieldStore = field.mStore;
		}
		if (fromOffset-toOffset)
		{
			field.mDictOffset -= (fromOffset - toOffset);
			field.Write(this,dtField.mValue);
		}
		err = Next(dtField);
	}

	if (fieldHeadRef==0) return(-1);
	
	u64 totalSize = pTable.mDictField.mDictSize;
	u64 moveSize = totalSize-fromOffset;
	pTable.mDictField.mDictSize -= (fromOffset - toOffset);
	pTable.mDictField.mDictOffset--;
	pTable.mDictField.Write(this,fieldHeadRef);
	
	dtField.Set(tableNames.mValue,SDATA_ITEM,pFieldId);
	err = Delete(dtField);
	if (err) return(err);

	u64 bytesWriten;

	switch (pTable.mDictField.mTreeType)
	{
		case GROUP_TREE:
			break;

		case RTABLE_TREE:

			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor,true);

			//bitshift all the records
			err = CopyDataValue(tableCursor.mValue,toOffset,tableCursor.mValue,fromOffset,moveSize,&bytesWriten);

			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor,false);

			err = 0;
			break;

		case CTABLE_TREE:
			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor,true);

			err = GetTreeStore(indexCursor,storeTree,storeType);
			if (err) return(err);
			if (storeTree==0) return(-1);

			switch(fieldType)
			{
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
				case GrapaTokenType::VECTOR:
				case GrapaTokenType::WIDGET:
				case GrapaTokenType::XML:
				case GrapaTokenType::LIST:
				case GrapaTokenType::EL:
				case GrapaTokenType::TAG:
				case GrapaTokenType::OP:
				case GrapaTokenType::CODE:
				case GrapaTokenType::TABLE:
					switch (fieldStore)
					{
					case GrapaDBField::STORE_VAR:
					case GrapaDBField::STORE_PAR:
						tableCursor.Set(storeTree, TREE_ITEM, pFieldId);
						err = Delete(tableCursor);
						break;
					case GrapaDBField::STORE_FIX:
						tableCursor.Set(storeTree, SDATA_ITEM, pFieldId);
						err = Delete(tableCursor);
						break;
					}
					break;

				default:
					break;
			}

			indexCursor.Set(pTable.mRecRef);
			err = SetTreeDirty(indexCursor,false);

			err = 0;
			break;

	}

	return(err);
}

GrapaError GrapaDB::FlushTableFields(GrapaDBTable& pTable)
{
	GrapaError err;
	GrapaCursor tableCursor;
	bool isDirty=false;

	tableCursor.Set(pTable.mRecRef);
	err = GetTreeDirty(tableCursor,isDirty);
	if(err) return(-1);
	
	if (!isDirty) return(0);

	//Need to scan through the DICT to see what has changed and make the updates

	err = SetTreeDirty(tableCursor,false);

	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::GetData(u64 itemPtr, GrapaCHAR& pValue)
{
	GrapaError err = -1;
	u64 dataSize, dataLength = 0, growBlockSize;
	u64 returnSize;
	u8 compressType = 0;
	pValue.SetLength(0, false);
	if (itemPtr && !(err = GetDataSize(itemPtr, growBlockSize, dataSize, dataLength, compressType)))
	{
		pValue.SetLength(dataLength, false);
		err = GetDataValue(itemPtr, 0, dataLength, pValue.GetPtr(), &returnSize);
		pValue.SetLength(returnSize, true);
	}
	return(err);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::FindFreeIndexId(GrapaDBIndex& pIndex, u64 minId, u64& pIndexId)
{
	GrapaError err = LastTableId(pIndex.mRef, pIndexId);
	if (pIndexId < minId) pIndexId = minId;
	if (err) return(err);
	return(0);
}

GrapaError GrapaDB::CreateIndex(GrapaDBTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBIndex& pIndex)
{
	GrapaError err;
	GrapaCursor indexCursor;
	u64 indexRef;

	err = OpenIndex(pTable,pIndexId,pIndexList,pIndex);
	// if replace, then delete and continue
	if (!err) return(-1);

	indexCursor.Set(pTable.mRecRef);
	err = GetTreeIndex(indexCursor,indexRef);
	if (err) return(err);
	if (indexRef==0)
	{
		err = NewTree(indexRef,SU64_TREE,pTable.mRecRef);
		if (err) return(err);
		err = SetTreeIndex(indexCursor,indexRef);
		if (err) return(err);
	}

	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mTable = pTable;

	err = NewTree(pIndex.mRef,RTABLE_TREE,indexRef);
	if (err) return(err);

	indexCursor.Set(indexRef,TREE_ITEM,pIndex.mId,pIndex.mRef);
	err = Insert(indexCursor);
	if (err) return(err);

	u64 indexFieldsRef;

	indexCursor.Set(indexRef);
	err = GetTreeIndex(indexCursor,indexFieldsRef);
	if (indexFieldsRef == 0)
	{
		err = NewTree(indexFieldsRef,RTABLE_TREE,indexRef);
		if (err) return(err);
		indexCursor.Set(indexRef);
		err = SetTreeIndex(indexCursor,indexFieldsRef);
		if (err) return(err);
	}

	u32 i,indexCount = pIndexList.Count();
	for (i=0;i<indexCount;i++)
	{
		GrapaDU64 *indexValue = (GrapaDU64*)pIndexList.GetAt(i);
		indexCursor.Set(pIndex.mRef);
		err = GetTreeIndex(indexCursor,indexFieldsRef);
		if (indexFieldsRef == 0)
		{
			err = NewTree(indexFieldsRef,SU64_TREE,pIndex.mRef);
			if (err) return(err);
			indexCursor.Set(pIndex.mRef);
			err = SetTreeIndex(indexCursor,indexFieldsRef);
			if (err) return(err);
			err = SetTreeStore(indexCursor,pTable.mRecRef,IPTR_STORE);
		}
		indexCursor.Set(indexFieldsRef,SU64_ITEM,indexValue->mNum.key,indexValue->mNum.value);
		err = Insert(indexCursor);
		if (err) return(err);
	}

	return(0);
}

GrapaError GrapaDB::OpenIndex(GrapaDBTable& pTable, u64 pIndexId, GrapaDU64Array& pIndexList, GrapaDBIndex& pIndex)
{
	GrapaError err;
	GrapaCursor indexCursor;
	u64 indexRef,indexFieldsRef,fieldCount;

	indexCursor.Set(pTable.mRecRef);
	err = GetTreeIndex(indexCursor,indexRef);
	if (err) return(err);
	if (indexRef==0) return(-1);

	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mTable = pTable;

	indexCursor.Set(indexRef,TREE_ITEM,pIndex.mId);
	err = Search(indexCursor);
	if (err) return(err);

	pIndex.mRef = indexCursor.mValue;

	indexCursor.Set(pIndex.mRef);
	err = GetTreeIndex(indexCursor,indexFieldsRef);
	if (indexFieldsRef == 0) return(-1); //maybe there are no fields defined????

	indexCursor.Set(indexFieldsRef);
	err = GetTreeSize(indexCursor,fieldCount);
	if (err) return(err);

	pIndexList.SetLength(0);
	pIndexList.SetSize(fieldCount);
	err = First(indexCursor);
	while (!err)
	{
		pIndexList.Append(indexCursor.mKey,indexCursor.mValue);
		err = Next(indexCursor);
	}

	return(0);
}

GrapaError GrapaDB::DeleteIndex(GrapaDBTable& pTable, u64 pIndexId)
{
	GrapaError err;
	GrapaCursor indexCursor;
	u64 indexRef;
	GrapaDBIndex pIndex;

	indexCursor.Set(pTable.mRecRef);
	err = GetTreeIndex(indexCursor,indexRef);
	if (err) return(err);
	if (indexRef==0) return(-1);

	pIndex.mId = pIndexId;
	pIndex.mRef = 0;
	pIndex.mTable = pTable;

	indexCursor.Set(indexRef,TREE_ITEM,pIndex.mId);
	err = Delete(indexCursor);
	if (err) return(err);

	return(0);
}

GrapaError GrapaDB::RefreshIndex(GrapaDBIndex& pIndex)
{
	GrapaError err;
	GrapaCursor indexCursor,dtCursor;
	indexCursor.Set(pIndex.mRef);
	EmptyTree(indexCursor);
	dtCursor.Set(pIndex.mTable.mRef);
	err = First(dtCursor);
	while(!err)
	{
		indexCursor.Set(pIndex.mRef,RPTR_ITEM,dtCursor.mKey,dtCursor.mValue);
		Insert(indexCursor);
		err = Next(dtCursor);
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::CreateIndexField(GrapaDBIndex& pIndex, u64 pIndexFieldId, u64 pFieldId)
{
	GrapaError err;
	GrapaCursor indexCursor,dtCursor;
	u64 indexFieldsRef;
	u64 fieldId;

	err = OpenIndexField(pIndex,pIndexFieldId,fieldId);
	// if replace, then delete and continue
	if (!err) return(-1);

	indexCursor.Set(pIndex.mRef,TREE_ITEM);
	err = GetTreeIndex(indexCursor,indexFieldsRef);
	if (indexFieldsRef == 0)
	{
		err = NewTree(indexFieldsRef,SU64_TREE);
		if (err) return(err);
		indexCursor.Set(pIndex.mRef,TREE_ITEM);
		err = SetTreeIndex(indexCursor,indexFieldsRef);
		if (err) return(err);
	}

	indexCursor.Set(indexFieldsRef,SU64_ITEM,pIndexFieldId,pFieldId);
	err = Insert(indexCursor);
	if (err) return(err);

	//recaculate the index
	indexCursor.Set(pIndex.mRef,TREE_ITEM);
	EmptyTree(indexCursor);
	dtCursor.Set(pIndex.mTable.mRef);
	err = First(dtCursor);
	while(!err)
	{
		GrapaBlockTree tree;
		err = tree.Read(mFile,dtCursor.mTreeRef);
		switch (tree.treeType)
		{
			case GROUP_TREE: 
				indexCursor.Set(pIndex.mRef,GPTR_ITEM,dtCursor.mKey);
				Insert(indexCursor);
				break;
			case RTABLE_TREE: 
				indexCursor.Set(pIndex.mRef,RPTR_ITEM,dtCursor.mKey);
				Insert(indexCursor);
				break;
			case CTABLE_TREE: 
				indexCursor.Set(pIndex.mRef,CPTR_ITEM,dtCursor.mKey);
				Insert(indexCursor);
				break;
		}
		err = Next(dtCursor);
	}

	return(0);
}

GrapaError GrapaDB::OpenIndexField(GrapaDBIndex& pIndex, u64 pIndexFieldId, u64& pFieldId)
{
	GrapaError err;
	GrapaCursor indexCursor;
	u64 indexFieldsRef;

	pFieldId = 0;
	indexCursor.Set(pIndex.mRef,TREE_ITEM);
	err = GetTreeIndex(indexCursor,indexFieldsRef);
	if (err) return(err);
	if (indexFieldsRef == 0) return(-1);
	indexCursor.Set(indexFieldsRef,SU64_ITEM,pIndexFieldId);
	err = Search(indexCursor);
	if (err) return(err);
	pFieldId = indexCursor.mValue;
	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::FindFreeRecordId(GrapaDBTable& pTable, u64& pRecordId)
{
	return LastTableId(pTable.mRef, pRecordId);
}

GrapaError GrapaDB::CreateRecord(GrapaDBTable& pTable, GrapaCursor& pCursor)
{
	GrapaError err = 0;
	u64 indexTree=0;
	GrapaCursor tableCursor,indexCursor,dtField;
	u64 newRecordRef = 0;
	GrapaDBField dbField;
	GrapaCursor searchCursor=pCursor;

	pCursor.mValue = 0;

	err = Search(searchCursor);
	if (!err) return(-1);

	switch (pTable.mRefType)
	{
		case GROUP_TREE:
			{
				GrapaDBTable recTable;
				switch (pCursor.mValueType)
				{
					case GREC_ITEM:
						err = CreateTable(pTable.mRef,GROUP_TREE,pCursor.mKey,recTable);
						if (err) return(err);
						break;
					case RREC_ITEM:
						err = CreateTable(pTable.mRef,RTABLE_TREE,pCursor.mKey,recTable);
						if (err) return(err);
						break;
					case CREC_ITEM:
						err = CreateTable(pTable.mRef,CTABLE_TREE,pCursor.mKey,recTable);
						if (err) return(err);
						break;
					default:
						return(-1);
				}
				pCursor.Set(pTable.mRecRef,pCursor.mValueType,pCursor.mKey);
				err = Search(pCursor);
				if (err) return(err);
				return(0);
			}
			break;
	}

	tableCursor.Set(pTable.mRecRef);
	err = GetTreeIndex(tableCursor,indexTree);
	if (err) return(err);

	switch (pTable.mDictField.mTreeType)
	{
		case SU64_TREE:
			tableCursor.Set(pTable.mRecRef,SU64_ITEM,pCursor.mKey,0);
			err = Insert(tableCursor);
			break;

		case GROUP_TREE:
			return(-1); // dict should not be a group tree

		case RTABLE_TREE:
			{
				indexCursor.Set(indexTree);
				err = First(indexCursor);
				if (err) return(err);

				dtField.Set(indexCursor.mValue);
				err = First(dtField); //This is the DICT field
				if (err) return(err);

				err = dbField.Read(this,dtField.mValue);
				if (err) return(err);

				err = NewData(BYTE_DATA, pTable.mRecRef, dbField.mDictSize, dbField.mDictSize, 1, newRecordRef, true);
				if (err) return(err);
				tableCursor.Set(pTable.mRecRef,RREC_ITEM,pCursor.mKey,newRecordRef);
				err = Insert(tableCursor);
				if (err) return(err);

				pCursor.mValue = newRecordRef;

				indexCursor.Set(indexTree);
				err = First(indexCursor);
				if (!err && indexCursor.mKey==0)
					err = Next(indexCursor);
				while (!err)
				{
					tableCursor.Set(indexCursor.mValue,RPTR_ITEM,pCursor.mKey);
					err = Insert(tableCursor);
					err = Next(indexCursor);
					if (!err && indexCursor.mKey==0)
						err = Next(indexCursor);
				}
			}
			break;

		case CTABLE_TREE:
			{
				tableCursor.Set(pTable.mRecRef,CREC_ITEM,pCursor.mKey,0);  // mValue is available for some use...
				err = Insert(tableCursor);
				if (err) return(err);
				pCursor.mLength = tableCursor.mLength;

				GrapaCursor itemCursor;
				u64 tableDT;
				itemCursor.Set(pTable.mRecRef);
				err = GetTreeIndex(itemCursor,tableDT);
				if (err) return(err);
				itemCursor.Set(tableDT);
				err = Search(itemCursor);
				if (err) return(err);
				tableDT = itemCursor.mValue;

				u64 storeTree;
				u8 storeType;
				err = GetTreeStore(tableCursor,storeTree,storeType);
				if (err) return(err);
				if (storeTree == 0) return(-1);
				tableCursor.Set(storeTree);
				err = First(tableCursor);
				while(!err)
				{
					switch(tableCursor.mValueType)
					{
						case SDATA_ITEM:
							{
								GrapaDBField dbField;
								itemCursor.Set(tableDT,0,tableCursor.mKey);
								err = Search(itemCursor);
								if (err) return(err);
								err = dbField.Read(this,itemCursor.mValue);
								if (err) return(err);
								err = InsertDataValue(tableCursor.mValue, pCursor.mLength*dbField.mDictSize, dbField.mDictSize, NULL);
							}
							break;

						case TREE_ITEM:
							break;
					}
					err = Next(tableCursor);
				}
				
				indexCursor.Set(indexTree);
				err = First(indexCursor);
				if (!err && indexCursor.mKey==0)
					err = Next(indexCursor);
				while (!err)
				{
					tableCursor.Set(indexCursor.mValue,CPTR_ITEM,pCursor.mKey,0);
					err = Insert(tableCursor);
					err = Next(indexCursor);
					if (!err && indexCursor.mKey==0)
						err = Next(indexCursor);
				}
			}

			break;

		default:
			return(-1);
			break;
	}

	return(0);
}

GrapaError GrapaDB::DeleteRecord(GrapaDBTable& pTable, GrapaCursor& pCursor)
{
	GrapaError err;
	GrapaCursor tableCursor;
	switch (pTable.mDictField.mTreeType)
	{
		case SU64_TREE:
			break;
		case GROUP_TREE:
			return(-1);
		case RTABLE_TREE:
			tableCursor.Set(pTable.mRecRef,RREC_ITEM,pCursor.mKey);
			return Delete(tableCursor);
		case CTABLE_TREE:
			{
				tableCursor.Set(pTable.mRecRef,CREC_ITEM,pCursor.mKey);
				err = Search(tableCursor);
				if (err) return(err);

				u64 delOffset = tableCursor.mLength;

				err = Delete(tableCursor);
				if (err) return(err);

				// then cycle through the fields deleting the item at the offset location at delOffset

				GrapaCursor itemCursor;
				u64 tableDT;
				itemCursor.Set(pTable.mRecRef);
				err = GetTreeIndex(itemCursor,tableDT);
				if (err) return(err);
				itemCursor.Set(tableDT);
				err = Search(itemCursor);
				if (err) return(err);
				tableDT = itemCursor.mValue;

				u64 storeTree;
				u8 storeType;
				GrapaDBField dbField;
				u64 rsize;
				err = GetTreeStore(tableCursor,storeTree,storeType);
				if (err) return(err);
				if (storeTree == 0) return(-1);
				tableCursor.Set(storeTree);
				err = First(tableCursor);
				while(!err)
				{
					switch(tableCursor.mValueType)
					{
						case SDATA_ITEM:
							//Move from (delOffset+1)*dbField.mSize to delOffset*dbField.mSize
							itemCursor.Set(tableDT,0,tableCursor.mKey);
							err = Search(itemCursor);
							if (err) return(err);
							err = dbField.Read(this,itemCursor.mValue);
							if (err) return(err);
							err = DeleteDataValue(tableCursor.mValue, delOffset*dbField.mDictSize, dbField.mDictSize, &rsize);
							break;

						case TREE_ITEM:
							itemCursor.Set(tableCursor.mValue, 0, pCursor.mKey);
							err = Delete(itemCursor);
							break;
					}
					err = Next(tableCursor);
				}
			}
			return(0);
	}
	return(-1);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::SetRecordField(GrapaCursor& cursor, GrapaDBFieldValueArray& pFieldList)
{
	GrapaError err;
	GrapaCursor recCursor,tableCursor,indexCursor;
	u64 indexTree,storeTree;
	u64 bytesWriten;
	s32 fieldCount = pFieldList.Count();
	GrapaDBFieldValue* dbFieldValue;
	s32 i;
	u8 compressType=0;

	err = PtrToRec(cursor,recCursor);
	if (err) return(err);

	GrapaBlockTree tree;
	err = tree.Read(mFile,recCursor.mTreeRef);
	if (err) return(err);

	recCursor.mTreeType = tree.treeType;
	indexTree = tree.indexTree;
	storeTree = tree.storeTree;

	indexCursor.Set(indexTree);
	err = First(indexCursor);
	if (!err && indexCursor.mKey==0)
		err = Next(indexCursor);
	while (!err)
	{
		for(i=0;i<fieldCount;i++)
		{
			dbFieldValue = pFieldList.GetFieldAt(i);
			if (IndexHasField(indexCursor,dbFieldValue->mId))
			{
				switch (recCursor.mTreeType)
				{
					case GROUP_TREE: 
						tableCursor.Set(indexCursor.mValue,GPTR_ITEM,recCursor.mKey);
						err = Delete(tableCursor);
						break;
					case RTABLE_TREE: 
						tableCursor.Set(indexCursor.mValue,RPTR_ITEM,recCursor.mKey);
						err = Delete(tableCursor);
						break;
					case CTABLE_TREE:
						tableCursor.Set(indexCursor.mValue,CPTR_ITEM,recCursor.mKey);
						err = Delete(tableCursor);
						break;
				}
				break;
			}
		}
		err = Next(indexCursor);
		if (!err && indexCursor.mKey==0)
			err = Next(indexCursor);
	}

	GrapaDBTable parentDict;
	GrapaDBField dbField;
	GrapaCursor dbCursor;

	switch (recCursor.mTreeType)
	{
	case SU64_TREE:
		break;

	case GROUP_TREE:
		err = OpenTable(recCursor.mTreeRef, 0, parentDict);
		if (err) return(err);
		dbCursor.Set(parentDict.mRef, RTABLE_TREE, recCursor.mKey);
		err = Search(dbCursor);
		if (err) return(err);
		err = SetRecordField(dbCursor, pFieldList);
		if (err) return(err);
		break;

	case RTABLE_TREE:

		//unfilled data should be nulled out to help the compress (future) function out
		// this is the delta [(oldLen<newLen)?0:(oldLen-newLen)]

		for (i = 0; i < fieldCount; i++)
		{
			dbFieldValue = pFieldList.GetFieldAt(i);
			u8 h[2] = { 0, 0 };
			u64 len = 0;
			u64 offset = 0;
			u64 dataPtr = 0, dataSize, dataLength, growBlockSize;
			u8 fieldType = dbFieldValue->mValue.ToDbType();
			u8 isRaw = (dbFieldValue->mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
			switch (dbFieldValue->mStore)
			{
			case GrapaDBField::STORE_FIX:
				if (dbFieldValue->mDictSize == 1)
				{
					switch (dbFieldValue->mType)
					{
					case GrapaTokenType::BOOL:
					case GrapaTokenType::INT:
					case GrapaTokenType::TIME:
						if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
						if (dbFieldValue->mValue.mLength)
						{
							u8 c = ((u8*)dbFieldValue->mValue.mBytes)[dbFieldValue->mValue.mLength - 1];
							if (c > 127) c = 127;
							h[0] |= c;
						}
						err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, 1, h, &bytesWriten);
						if (err) return(err);
						break;
					}
				}
				//else if (dbFieldValue->mDictSize == 9 && (dbFieldValue->mType == (u8)GrapaTokenType::TIME))
				//{
				//	if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
				//	if (dbFieldValue->mValue.mLength == 8)
				//	{
				//		h[0] |= 8;
				//		err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset + 1, 8, dbFieldValue->mValue.mBytes, &bytesWriten);
				//	}
				//	err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, 1, h, &bytesWriten);
				//	if (err) return(err);
				//}
				else if (dbFieldValue->mDictSize <= (((u64)128) + isRaw))
				{
					err = GetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, 1, h, &bytesWriten);
					len = h[0] & 0x7F;
					h[0] = 0;
					if (len > dbFieldValue->mValue.mLength)
					{
						err = ClearDataValue(recCursor.mValue, dbFieldValue->mDictOffset + 1 + isRaw + dbFieldValue->mValue.mLength, len - dbFieldValue->mValue.mLength);
					}
					if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
					if (dbFieldValue->mValue.mLength)
					{
						if (dbFieldValue->mValue.mLength > (dbFieldValue->mDictSize - 1 - isRaw)) len = (dbFieldValue->mDictSize - 1 - isRaw); else len = dbFieldValue->mValue.mLength;
						h[0] |= (u8)len;
						switch (dbFieldValue->mType)
						{
						case GrapaTokenType::INT:
						case GrapaTokenType::TIME:
							offset = dbFieldValue->mValue.mLength - len;
							break;
						}
						err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset + 1 + isRaw, len, &((u8*)dbFieldValue->mValue.mBytes)[offset], &bytesWriten);
						if (err) return(err);
					}
					if (isRaw)
						err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset + 1, 1, &fieldType, &bytesWriten);
					err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, 1, h, &bytesWriten);
					if (err) return(err);
				}
				else if (dbFieldValue->mDictSize <= (0x8001 + isRaw))
				{
					err = GetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, 2, h, &bytesWriten);
					len = ((u64)(h[0] & 0x7F)) << 8 | (u64)h[1];
					h[0] = 0;
					h[1] = 0;
					if (len > dbFieldValue->mValue.mLength)
					{
						err = ClearDataValue(recCursor.mValue, dbFieldValue->mDictOffset + 2 + isRaw + dbFieldValue->mValue.mLength, len - dbFieldValue->mValue.mLength);
					}
					if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
					if (dbFieldValue->mValue.mBytes && dbFieldValue->mValue.mLength)
					{
						if (dbFieldValue->mValue.mLength > (dbFieldValue->mDictSize - 2 - isRaw)) len = (dbFieldValue->mDictSize - 2 - isRaw); else len = dbFieldValue->mValue.mLength;
						h[0] |= (len >> 8) & 0xFF; h[1] = len & 0xFF;
						switch (dbFieldValue->mType)
						{
						case GrapaTokenType::INT:
						case GrapaTokenType::TIME:
							offset = dbFieldValue->mValue.mLength - len;
							break;
						}
						err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset + 2 + isRaw, len, &((u8*)dbFieldValue->mValue.mBytes)[offset], &bytesWriten);
						if (err) return(err);
					}
					if (isRaw)
						err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset + 2, 1, &fieldType, &bytesWriten);
					err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, 2, h, &bytesWriten);
					if (err) return(err);
				}
				else
					return((GrapaError)-1);
				break;
			case GrapaDBField::STORE_VAR:
			case GrapaDBField::STORE_PAR:
				err = GetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, sizeof(dataPtr), &dataPtr, &bytesWriten);
				if (err) return(err);
				dataPtr = BE_S64(dataPtr);
				if (!dataPtr)
				{
					switch (dbFieldValue->mStore)
					{
					case GrapaDBField::STORE_VAR:
						err = NewData(BYTE_DATA, recCursor.mValue, dbFieldValue->mValue.GetLength(), dbFieldValue->mSize, dbFieldValue->mGrow, dataPtr);
						break;
					case GrapaDBField::STORE_PAR:
						err = NewData(FREC_DATA, recCursor.mValue, dbFieldValue->mValue.GetLength(), dbFieldValue->mSize, dbFieldValue->mGrow, dataPtr);
						break;
					}
					if (err) return(err);
					if (isRaw) SetFieldType(dataPtr, fieldType); 
					else SetFieldType(dataPtr, dbFieldValue->mType);
					err = SetDataValue(dataPtr, 0, dbFieldValue->mValue.GetLength(), dbFieldValue->mValue.GetPtr(), &bytesWriten);
					if (err) return(err);
					dataPtr = BE_S64(dataPtr);
					err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, sizeof(dataPtr), &dataPtr, &bytesWriten);
				}
				else
				{
					if (dbFieldValue->mValue.mBytes)
					{
						err = GetDataSize(dataPtr, growBlockSize, dataSize, dataLength, compressType);
						if (err) return(err);
						err = SetDataSize(dataPtr, dataSize, dbFieldValue->mValue.GetLength(), compressType);
						if (err) return(err);
						err = SetDataValue(dataPtr, 0, dbFieldValue->mValue.GetLength(), dbFieldValue->mValue.GetPtr(), &bytesWriten);
						if (err) return(err);
						if (isRaw) SetFieldType(dataPtr, fieldType);
					}
					else
					{
						DeleteData(dataPtr); // need to test/verify this
						dataPtr = 0;
						err = SetDataValue(recCursor.mValue, dbFieldValue->mDictOffset, sizeof(dataPtr), &dataPtr, &bytesWriten);
					}
				}
				break;
			default:
				return((GrapaError)-1);
			}
		}
		break;

	case CTABLE_TREE:
		for (i = 0; i < fieldCount; i++)
		{
			dbFieldValue = pFieldList.GetFieldAt(i);
			u8 h[2] = { 0, 0 };
			u64 len = 0;
			u64 offset = 0;
			u64 dataPtr = 0, dataSize, dataLength, growBlockSize;
			GrapaCursor fieldCursor;
			u8 fieldType = dbFieldValue->mValue.ToDbType();
			u8 isRaw = (dbFieldValue->mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
			switch (dbFieldValue->mStore)
			{
			case GrapaDBField::STORE_FIX:
				tableCursor.Set(storeTree, SDATA_ITEM, dbFieldValue->mId);
				err = Search(tableCursor);
				if (err) return(err);
				if (dbFieldValue->mDictSize == 1)
				{
					switch (dbFieldValue->mType)
					{
					case GrapaTokenType::INT:
					case GrapaTokenType::TIME:
						if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
						if (dbFieldValue->mValue.mLength && dbFieldValue->mValue.mBytes)
						{
							u8 c = ((u8*)dbFieldValue->mValue.mBytes)[dbFieldValue->mValue.mLength - 1];
							if (c > 127) c = 127;
							h[0] |= c;
						}
						err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength, 1, h, &bytesWriten);
						if (err) return(err);
						break;
					}
				}
				//else if (dbFieldValue->mDictSize == 9 && (dbFieldValue->mType == (u8)GrapaTokenType::TIME))
				//{
				//	if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
				//	if (dbFieldValue->mValue.mLength == 8)
				//	{
				//		h[0] |= 8;
				//		SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength + 1, 8, dbFieldValue->mValue.mBytes, &bytesWriten);
				//	}
				//	err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength, 1, h, &bytesWriten);
				//	if (err) return(err);
				//}
				else if (dbFieldValue->mDictSize <= (128 + isRaw))
				{
					err = GetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength, 1, h, &bytesWriten);
					len = h[0] & 0x7F;
					h[0] = 0;
					if (len > dbFieldValue->mValue.mLength)
					{
						err = ClearDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength + 1 + isRaw + dbFieldValue->mValue.mLength, len - dbFieldValue->mValue.mLength);
					}
					if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
					if (dbFieldValue->mValue.mLength)
					{
						if (dbFieldValue->mValue.mLength > (dbFieldValue->mDictSize - 1 - isRaw)) len = (dbFieldValue->mDictSize - 1 - isRaw); else len = dbFieldValue->mValue.mLength;
						h[0] |= (u8)len;
						switch (dbFieldValue->mType)
						{
						case GrapaTokenType::INT:
						case GrapaTokenType::TIME:
							offset = dbFieldValue->mValue.mLength - len;
							break;
						}
						err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength + 1 + isRaw, len, &((u8*)dbFieldValue->mValue.mBytes)[offset], &bytesWriten);
						if (err) return(err);
					}
					if (isRaw)
						err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength + 1, 1, &fieldType, &bytesWriten);
					err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength, 1, h, &bytesWriten);
					if (err) return(err);
				}
				else if (dbFieldValue->mDictSize <= (0x8001 + isRaw))
				{
					err = GetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength, 2, h, &bytesWriten);
					len = ((u64)(h[0] & 0x7F)) << 8 | (u64)h[1];
					h[0] = 0;
					h[1] = 0;
					if (len > dbFieldValue->mValue.mLength)
					{
						err = ClearDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength + 2 + isRaw + dbFieldValue->mValue.mLength, len - dbFieldValue->mValue.mLength);
					}
					if (dbFieldValue->mValue.mBytes) h[0] = 0x80;
					if (dbFieldValue->mValue.mLength)
					{
						if (dbFieldValue->mValue.mLength > (dbFieldValue->mDictSize - 2 - isRaw)) len = (dbFieldValue->mDictSize - 2 - isRaw); else len = dbFieldValue->mValue.mLength;
						h[0] |= (len >> 8) & 0xFF; h[1] = len & 0xFF;
						switch (dbFieldValue->mType)
						{
						case GrapaTokenType::INT:
						case GrapaTokenType::TIME:
							offset = dbFieldValue->mValue.mLength - len;
							break;
						}
						err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength + 2 + isRaw, len, &((u8*)dbFieldValue->mValue.mBytes)[offset], &bytesWriten);
						if (err) return(err);
					}
					if (isRaw)
						err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength + 2, 1, &fieldType, &bytesWriten);
					err = SetDataValue(tableCursor.mValue, dbFieldValue->mDictSize*recCursor.mLength, 2, h, &bytesWriten);
					if (err) return(err);
				}
				else
					return((GrapaError)-1);
				break;
			case GrapaDBField::STORE_VAR:
			case GrapaDBField::STORE_PAR:
				tableCursor.Set(storeTree, TREE_ITEM, dbFieldValue->mId);
				err = Search(tableCursor);
				if (err) return(err);
				// stored in a tree by recordId, cursor.mValue is the tree
				fieldCursor.Set(tableCursor.mValue, SDATA_ITEM, cursor.mKey);
				err = Search(fieldCursor);
				if (err)
				{
					if (dbFieldValue->mValue.mBytes == NULL) break;
					switch (dbFieldValue->mStore)
					{
					case GrapaDBField::STORE_VAR:
						err = NewData(BYTE_DATA, tableCursor.mValue, dbFieldValue->mValue.GetLength(), dbFieldValue->mSize, dbFieldValue->mGrow, dataPtr);
						break;
					case GrapaDBField::STORE_PAR:
						err = NewData(FREC_DATA, tableCursor.mValue, dbFieldValue->mValue.GetLength(), dbFieldValue->mSize, dbFieldValue->mGrow, dataPtr);
						break;
					}
					if (err) return(err);
					SetFieldType(dataPtr, dbFieldValue->mType);
					fieldCursor.Set(tableCursor.mValue, SDATA_ITEM, cursor.mKey, dataPtr);
					err = Insert(fieldCursor);
					if (err)
					{
						DeleteData(dataPtr);
						return(err);
					}
				}
				if (dbFieldValue->mValue.mBytes == NULL)
				{
					err = Delete(fieldCursor);
					break;
				}
				err = GetDataSize(fieldCursor.mValue, growBlockSize, dataSize, dataLength, compressType);
				if (err) return(err);
				err = SetDataSize(fieldCursor.mValue, dataSize, dbFieldValue->mValue.mLength, compressType);
				if (err) return(err);
				err = SetDataValue(fieldCursor.mValue, 0, dbFieldValue->mValue.mLength, dbFieldValue->mValue.GetPtr(), &bytesWriten);
				if (err) return(err);
				if (isRaw) SetFieldType(fieldCursor.mValue, fieldType);
				break;
			default:
				return((GrapaError)-1);
			}
		}
		break;
	}

	indexCursor.Set(indexTree);
	err = First(indexCursor);
	if (!err && indexCursor.mKey==0)
		err = Next(indexCursor);
	while (!err)
	{
		for(i=0;i<fieldCount;i++)
		{
			dbFieldValue = pFieldList.GetFieldAt(i);
			if (IndexHasField(indexCursor,dbFieldValue->mId))
			{
				switch (recCursor.mTreeType)
				{
					case GROUP_TREE: 
						tableCursor.Set(indexCursor.mValue,GPTR_ITEM,recCursor.mKey);
						err = Insert(tableCursor);
						break;
					case RTABLE_TREE: 
						tableCursor.Set(indexCursor.mValue,RPTR_ITEM,recCursor.mKey);
						err = Insert(tableCursor);
						break;
					case CTABLE_TREE: 
						tableCursor.Set(indexCursor.mValue,CPTR_ITEM,recCursor.mKey);
						err = Insert(tableCursor);
						break;
				}
				break;
			}
		}
		err = Next(indexCursor);
		if (!err && indexCursor.mKey==0)
			err = Next(indexCursor);
	}

	return(0);
}

GrapaError GrapaDB::FindRecordField(GrapaCursor& cursor, u64 fieldId, GrapaCursor& recCursor, GrapaDBField& field)
{
	GrapaError err;
	u64 tableRef;

	err = PtrToRec(cursor, recCursor);
	if (err) return(err);

	GrapaBlockTree tree;
	err = tree.Read(mFile, recCursor.mTreeRef);
	if (err) return(err);

	recCursor.mTreeType = tree.treeType;

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
		return(-1);  // do we every get to this location?
		//err = dbField.Get(this, recCursor.mValue, fieldId);
		//if (err) return(err);
		break;
	}
	return(err);
}

GrapaError GrapaDB::GetRecordField(GrapaCursor& cursor, u64 fieldId, GrapaBYTE& buffer)
{
	GrapaError err;
	GrapaDBField field;
	GrapaCursor recCursor;
	err = FindRecordField(cursor, fieldId, recCursor, field);
	if (err) return(err);
	err = GetRecordField(recCursor, field, buffer);
	if (err) return(err);
	return 0;
}

GrapaError GrapaDB::GetRecordField(GrapaCursor& cursor, GrapaDBField& field, GrapaBYTE& buffer)
{
	GrapaError err;
	u64 returnSize=0;
	u64 dataRef = cursor.mValue;
	u64 indexTree,storeTree;
	GrapaCursor fieldCursor,recCursor;
	u8 compressType=0;

	buffer.SetSize(0);

	GrapaBlockTree tree;
	err = tree.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	cursor.mTreeType = tree.treeType;
	indexTree = tree.indexTree;
	storeTree = tree.storeTree;

	err = PtrToRec(cursor,recCursor);
	if (err) return(err);

	u8 h[2] = { 0, 0 };
	u64 len = 0;
	u64 dataPtr, dataSize, dataLength = 0, growBlockSize;
	buffer.FromDbType(field.mType);
	u8 isRaw = (field.mType == (u8)GrapaTokenType::RAW) ? 1 : 0;
	switch (recCursor.mTreeType)
	{
	case SU64_TREE:
		break;

	case GROUP_TREE:
		break;

	case RTABLE_TREE:
		switch (field.mStore)
		{
		case GrapaDBField::STORE_FIX:
			if (field.mDictSize == 1)
			{
				err = GetDataValue(dataRef, field.mDictOffset, 1, h, &returnSize);
				if ((h[0] & 0x80) == 0)
					buffer.SetSize(0);
				else
				{
					h[0] &= 0x7F;
					buffer.SetLength(1, false);
					((u8*)buffer.mBytes)[0] = h[0] & 0x7F;
				}
			}
			//else if (field.mDictSize == 9 && (field.mType == (u8)GrapaTokenType::TIME))
			//{
			//	err = GetDataValue(dataRef, field.mDictOffset, 1, h, &returnSize);
			//	if ((h[0] & 0x80) == 0)
			//		buffer.SetSize(0);
			//	else
			//	{
			//		len = h[0] & 0x7F;
			//		buffer.SetLength(len, false);
			//		err = GetDataValue(dataRef, field.mDictOffset + 1, len, buffer.GetPtr(), &returnSize);
			//		if (err) return(err);
			//	}
			//}
			else if (field.mDictSize <= (((u64)128) + isRaw))
			{
				err = GetDataValue(dataRef, field.mDictOffset, 1, h, &returnSize);
				if ((h[0] & 0x80) == 0)
					buffer.SetSize(0);
				else
				{
					len = h[0] & 0x7F;
					buffer.SetLength(len, false);
					err = GetDataValue(dataRef, field.mDictOffset + 1 + isRaw, len, buffer.GetPtr(), &returnSize);
					if (err) return(err);
				}
				if (isRaw)
				{
					h[0] = 0;
					err = GetDataValue(dataRef, field.mDictOffset + 1, 1, h, &returnSize);
					buffer.mToken = h[0];
				}
			}
			else if (field.mDictSize <= (0x8001 + isRaw))
			{
				err = GetDataValue(dataRef, field.mDictOffset, 2, h, &returnSize);
				if ((h[0] & 0x80) == 0)
					buffer.SetSize(0);
				else
				{
					len = ((u64)(h[0] & 0x7F)) << 8 | (u64)h[1];
					buffer.SetLength(len, false);
					err = GetDataValue(dataRef, field.mDictOffset + 2 + isRaw, len, buffer.GetPtr(), &returnSize);
					if (err) return(err);
				}
				if (isRaw)
				{
					h[0] = 0;
					err = GetDataValue(dataRef, field.mDictOffset + 2, 1, h, &returnSize);
					buffer.mToken = h[0];
				}
			}
			else
				return((GrapaError)-1);
			break;

		case GrapaDBField::STORE_VAR:
		case GrapaDBField::STORE_PAR:
			err = GetDataValue(dataRef, field.mDictOffset, sizeof(dataPtr), &dataPtr, &returnSize);
			if (err) return(err);
			if (dataPtr)
			{
				dataPtr = BE_S64(dataPtr);
				err = GetDataSize(dataPtr, growBlockSize, dataSize, dataLength, compressType);
				if (err) return(err);
				buffer.SetLength(dataLength, false);
				err = GetDataValue(dataPtr, 0, dataLength, buffer.GetPtr(), &returnSize);
				if (err) return(err);
			}
			else
			{
				buffer.SetSize(0);
			}
			if (isRaw)
			{
				GetFieldType(dataPtr, h[0]);
				buffer.mToken = h[0];
			}
			break;
		}
		break;

	case CTABLE_TREE:
		//if (recCursor.mNodeRef==0)
		//err = Search(recCursor); // shouldn't need to do this for every field...do 1'ce for the record.

		switch (field.mStore)
		{
		case GrapaDBField::STORE_FIX:
			fieldCursor.Set(storeTree, SDATA_ITEM, field.mId);
			err = Search(fieldCursor);
			if (err) return(err);

			if (field.mDictSize == 1)
			{
				err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength, 1, h, &returnSize);
				if ((h[0] & 0x80) == 0)
					buffer.SetSize(0);
				else
				{
					h[0] &= 0x7F;
					buffer.SetLength(1, false);
					((u8*)buffer.mBytes)[0] = h[0] & 0x7F;
				}
			}
			//else if (field.mDictSize == 9 && (field.mType == (u8)GrapaTokenType::TIME))
			//{
			//	err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength, 1, h, &returnSize);
			//	if ((h[0] & 0x80) == 0)
			//		buffer.SetSize(0);
			//	else
			//	{
			//		len = h[0] & 0x7F;
			//		buffer.SetLength(len, false);
			//		err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength + 1, len, buffer.GetPtr(), &returnSize);
			//		if (err) return(err);
			//	}
			//}
			else if (field.mDictSize <= (((u64)128) + isRaw))
			{
				err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength, 1, h, &returnSize);
				if ((h[0] & 0x80) == 0)
					buffer.SetSize(0);
				else
				{
					len = h[0] & 0x7F;
					buffer.SetLength(len, false);
					err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength + 1 + isRaw, len, buffer.GetPtr(), &returnSize);
					if (err) return(err);
				}
				if (isRaw)
				{
					h[0] = 0;
					err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength + 1, 1, h, &returnSize);
					buffer.mToken = h[0];
				}
			}
			else if (field.mDictSize <= (0x8001 + isRaw))
			{
				err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength, 2, h, &returnSize);
				if ((h[0] & 0x80) == 0)
					buffer.SetSize(0);
				else
				{
					len = ((u64)(h[0] & 0x7F)) << 8 | (u64)h[1];
					buffer.SetLength(len, false);
					err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength + 2 + isRaw, len, buffer.GetPtr(), &returnSize);
					if (err) return(err);
				}
				if (isRaw)
				{
					h[0] = 0;
					err = GetDataValue(fieldCursor.mValue, field.mDictSize*recCursor.mLength + 2, 1, h, &returnSize);
					buffer.mToken = h[0];
				}
			}
			else
				return((GrapaError)-1);
			break;

		case GrapaDBField::STORE_VAR:
		case GrapaDBField::STORE_PAR:
			fieldCursor.Set(storeTree, TREE_ITEM, field.mId);
			err = Search(fieldCursor);
			if (err) return(err);
			// stored in a tree by recordId, cursor.mValue is the tree
			fieldCursor.Set(fieldCursor.mValue, SDATA_ITEM, cursor.mKey);
			err = Search(fieldCursor);
			if (err)
			{
				buffer.SetSize(0);
				break;
			}
			err = GetDataSize(fieldCursor.mValue, growBlockSize, dataSize, dataLength, compressType);
			buffer.SetLength(dataLength, false);
			returnSize = 0;
			err = GetDataValue(fieldCursor.mValue, 0, dataLength, buffer.GetPtr(), &returnSize);
			if (err) return(err);
			if (isRaw)
			{
				GetFieldType(fieldCursor.mValue, h[0]);
				buffer.mToken = h[0];
			}
			break;

		}
		break;

	}

	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::SearchDb(GrapaDBCursor& cursor, GrapaDBTable& pTable, GrapaDBFieldValueArray& pFieldList)
{
	GrapaError err;
	u64 indexRef=0;
	GrapaCursor indexCursor;
	bool usingIndex=false;
	GrapaDBField* field;

	if (pFieldList.Count())
	{
		// here we try to find the best index to use
		// we may also re-arange pFieldList to accomodate things
		field = pFieldList.GetFieldAt(0);
		indexCursor.Set(pTable.mRecRef);
		err = GetTreeIndex(indexCursor,indexRef);
		if (!err)
		{
			err = LocateIndex(indexCursor,indexRef,field->mId);
			if (!err) 
			{
				usingIndex = true;
				cursor.SetSearch(this,indexCursor.mValue,usingIndex,&pFieldList);
			}
		}
	}
	
	// This needs to be a 2 stage process
	// First use the index to narrow down for the fields that are in the index
	// Then do the second scan without the index for the remaining search fields
	if (usingIndex)
	{
		err = Search(cursor);
		if (err) return(err);
		err = FirstDb(cursor);
		if (err) return(err);
	}
	else
	{
		// need to verify this logic. Locate the first record given the search term
		cursor.SetSearch(this,pTable.mRecRef,usingIndex,&pFieldList);
		err = First(cursor);
		if (err) return(err);
		err = FirstDb(cursor);
		if (err) return(err);
		GrapaDBCursor cursorCompare = cursor;
		s8 result;
		cursorCompare.mValueType = SEARCH_ITEM;
		err = CompareKey(SEARCH_MODE,cursorCompare,cursor,result);
		if (!err && result==0)
			return(0);
		err = NextDb(cursor);
		if (err) return(err);
	}

	return(0);
}

GrapaError GrapaDB::FirstDb(GrapaDBCursor& cursor)
{
	GrapaError err = 0;
	GrapaDBCursor cursorCompare = cursor;
	while(!err)
	{
		err = PrevDb(cursorCompare);
		if (err) return(0);
		cursor = cursorCompare;
	}
	return(0);
}

GrapaError GrapaDB::LastDb(GrapaDBCursor& cursor)
{
	GrapaError err = 0;
	GrapaDBCursor cursorCompare = cursor;
	while(!err)
	{
		err = NextDb(cursorCompare);
		if (err) return(0);
		cursor = cursorCompare;
	}
	return(0);
}

GrapaError GrapaDB::NextDb(GrapaDBCursor& cursor)
{
	GrapaError err = 0;
	s8 result;
	//s8 valueType = cursor.mValueType;
	while(true)
	{
		GrapaDBCursor cursorCompare = cursor;
		err = Next(cursor);
		// do we need to check mKey for 0?
		if (err) return(err);
		cursorCompare.mValueType = SEARCH_ITEM;
		//cursor.mValueType = RPTR_ITEM;
		err = CompareKey(SEARCH_MODE,cursorCompare,cursor,result);
		if (err) return(err);
		//cursor.mValueType = valueType;
		if (result==0) return(0);
		if (cursor.mUsingIndex)
		{
			return((GrapaError)-1);
		}
	}
	return(0);
}

GrapaError GrapaDB::PrevDb(GrapaDBCursor& cursor)
{
	GrapaError err = 0;
	s8 result;
	//s8 valueType = cursor.mValueType;
	while(true)
	{
		GrapaDBCursor cursorCompare = cursor;
		err = Prev(cursor);
		if (err) return(err);
		cursorCompare.mValueType = SEARCH_ITEM;
		//cursor.mValueType = RPTR_ITEM;
		err = CompareKey(SEARCH_MODE,cursorCompare,cursor,result);
		if (err) return(err);
		//cursor.mValueType = valueType;
		if (result==0) return(0);
		if (cursor.mUsingIndex)
		{
			return((GrapaError)-1);
		}
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////////////

// need to update to use a field list for indexes that have multiple fields
// and would need to match up the compar fields to the index fields
GrapaError GrapaDB::LocateIndex(GrapaCursor& cursor, u64 indexRef, u64 fieldId)
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

bool GrapaDB::IndexHasField(GrapaCursor& cursor, u64 fieldId)
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

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::GetDataTypeRecord(u64 tableRef, u64& tableDT)
{
	GrapaError err=0;
	GrapaCursor tableCursor;
	u64 indexTree=0;
	tableDT = 0;
	tableCursor.Set(tableRef);
	err = GetTreeIndex(tableCursor,indexTree);
	if (err) return(err);
	tableCursor.Set(indexTree);
	err = Search(tableCursor);
	if (err) return(err);
	tableDT = tableCursor.mValue;
	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::InsertIntoIndex(u64 tableRef, u8 pValueType, u64 resId, u64 recordRef)
{
	GrapaError err=0;
	GrapaCursor indexTableCursor,indexCursor;
	u64 indexRef;

	indexTableCursor.Set(tableRef);
	err = GetTreeIndex(indexTableCursor,indexRef);
	if (err) return(err);

	indexTableCursor.Set(indexRef);
	err = First(indexTableCursor);
	if (!err && indexTableCursor.mKey==0)
		err = Next(indexTableCursor);
	while (!err)
	{
		indexCursor.Set(indexTableCursor.mValue,pValueType,resId,recordRef);
		err = Insert(indexCursor);
		if (err) return(err);
		err = Next(indexTableCursor);
		if (!err && indexTableCursor.mKey==0)
			err = Next(indexTableCursor);
	}

	return(0);
}

////////////////////////////////////////////////////////////////////////////////

// Need to fix the comparisons. Numbers are being compared as strings...which isn't right.
// Compare needs to be based on the field type

GrapaError GrapaDB::CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
{
	GrapaError err=0;
	GrapaCursor cursor;

	result = -1;

	switch(treeCursor.mValueType)
	{
		case DTYPE_ITEM:
			// this item is the same as a DbData item, but for the data type dictionary
			//result = treeCursor.mKey - dataCursor.mKey;
			result = GrapaCore::CompareKey(treeCursor.mKey, dataCursor.mKey);
			break;

		case GREC_ITEM: 
		case RREC_ITEM:
		case CREC_ITEM:
		case GPTR_ITEM:
		case RPTR_ITEM:
		case CPTR_ITEM:
			switch(dataCursor.mValueType)
			{
				case SEARCH_ITEM: // Searching an index using a set of search fields
					err = CompareSearchKey(compareType, dataCursor, treeCursor, result);
					//if (err) return(err);
					break;

				case GREC_ITEM: 
				case RREC_ITEM:
				case CREC_ITEM:
				case GPTR_ITEM:
				case RPTR_ITEM:
				case CPTR_ITEM:
					err = CompareRecordKey(compareType, dataCursor, treeCursor, result);
					//if (err) return(err);
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
		result = GrapaCore::CompareKey(treeCursor.mKey, dataCursor.mKey);

	if (result==0)
	{
		// if the values end up to be the same, then lets compare the key's just to be sure.
		switch(dataCursor.mValueType)
		{
			case GREC_ITEM:
			case RREC_ITEM:
			case CREC_ITEM:
			case GPTR_ITEM:
			case RPTR_ITEM:
			case CPTR_ITEM:
				switch (compareType)
				{
					case SEARCH_MODE:
						if (dataCursor.mKey)
							result = GrapaCore::CompareKey(treeCursor.mKey, dataCursor.mKey);
						break;
					case INSERT_MODE:
					case DELETE_MODE:
						result = GrapaCore::CompareKey(treeCursor.mKey, dataCursor.mKey);
						break;
				}
				break;
		}
	}
	
	return(0);
}

GrapaError GrapaDB::CompareRecordKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
{
	GrapaError err;
	GrapaCursor cursor;
	u64 indexRef;
	GrapaCHAR name1,name2;

	result = -1;

	switch(treeCursor.mValueType)
	{
		case GREC_ITEM: 
		case RREC_ITEM:
		case CREC_ITEM:
		case GPTR_ITEM: 
		case RPTR_ITEM:
		case CPTR_ITEM:
			switch(dataCursor.mValueType)
			{
				case GREC_ITEM: 
				case RREC_ITEM:
				case CREC_ITEM:
					if (dataCursor.mKey==treeCursor.mKey) 
					{
						result = 0;
						return(0);
					}
					break;
				case GPTR_ITEM: 
				case RPTR_ITEM:
				case CPTR_ITEM:
					break;
				case SEARCH_ITEM:
					return CompareSearchKey(compareType, dataCursor, treeCursor, result);
				default:
					return(-1);
			}
			break;
		default:
			return(-1);
	}

	cursor.Set(treeCursor.mTreeRef);
	err = GetTreeIndex(cursor,indexRef);
	if (err) return(err);

	GrapaCursor treeItemCursor,dataItemCursor;
	err = PtrToRec(treeCursor, treeItemCursor);
	if (err) return(err);
	err = PtrToRec(dataCursor, dataItemCursor);
	if (err) return(err);

	switch(treeCursor.mValueType)
	{
		case GREC_ITEM: 
		case RREC_ITEM:
		case GPTR_ITEM: 
		case RPTR_ITEM:
			if (dataItemCursor.mValue==treeItemCursor.mValue) 
			{
				result = 0;
				return(0);
			}
			break;

		case CREC_ITEM:
		case CPTR_ITEM:
			if (dataItemCursor.mLength==treeItemCursor.mLength) 
			{
				result = 0;
				return(0);
			}
			break;
	}

	cursor.Set(indexRef);
	err = First(cursor);
	while(!err)
	{
		// need to pull these into the treeItemCursor datatype for the comparison
		err = GetRecordField(dataItemCursor,cursor.mValue,name1);
		err = GetRecordField(treeItemCursor,cursor.mValue,name2);

		char* n1 = (char*)name1.mBytes;
		char* n2 = (char*)name2.mBytes;

		// the following conditions should never happen...unless the value really is NULL
		if (name1.mBytes==NULL  || name1.mLength==0) n1 = (char*)"";
		if (name2.mBytes==NULL  || name2.mLength==0) n2 = (char*)"";
		
		// need to compare based on the treeItemCursor datatype
		int cr = strcmp(n2, n1);
		if (cr)
		{
			result = (cr > 0) ? 1 : ((cr < 0) ? -1 : 0);
			break;
		}

		err = Next(cursor);
	}

	return(0);
}

GrapaError GrapaDB::CompareSearchKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
{
	GrapaError err;
	GrapaDBFieldValueArray *fvList;
	GrapaDBFieldValue *fv;
	u32 i,count;
	GrapaCHAR name2;

	result = -1;

	if (dataCursor.mValueType!=SEARCH_ITEM) return(-1);

	// user specified this
	// parse based on all of the values in GrapaDBCursor
	// cycle through all the fields included in the user pointer
	// use the order specified in the index

	fvList = ((GrapaDBCursor&)dataCursor).mData;
	count = fvList->Count();
	result = 0; // if no fields, then the compare indicates an equal
	for(i=0;i<count;i++)
	{
		fv = fvList->GetFieldAt(i);
		GrapaCursor treeItemCursor;
		err = PtrToRec(treeCursor, treeItemCursor);

		// need to pull this into the treeItemCursor datatype for the comparison
		err = GetRecordField(treeItemCursor,*fv,name2);

		char* n1 = (char*)fv->mValue.mBytes;
		char* n2 = (char*)name2.mBytes;

		// the following conditions should never happen...unless the value really is NULL
		if (fv->mValue.mBytes==NULL  || fv->mValue.mLength==0) n1 = (char*)"";
		if (name2.mBytes==NULL  || name2.mLength==0) n2 = (char*)"";

		// need to compare based on the treeItemCursor datatype
		int cr = strcmp(n2, n1);
		result = (cr > 0) ? 1 : ((cr < 0) ? -1 : 0);
		if (result<0)
		{
			switch(fv->mCmp)
			{
			case LT_CMP:
			case LTEQ_CMP:
				result = 0;
				break;
			case EQ_CMP:
			case GT_CMP:
			case GTEQ_CMP:
				result = -1;
				break;
			}
		}
		else if (result>0)
		{
			switch(fv->mCmp)
			{
			case LT_CMP:
			case LTEQ_CMP:
			case EQ_CMP:
				result = 1;
				break;
			case GT_CMP:
			case GTEQ_CMP:
				result = 0;
				break;
			}
		}
		else
		{
			switch(fv->mCmp)
			{
			case LT_CMP:
				result = 1;
				break;
			case LTEQ_CMP:
			case EQ_CMP:
			case GTEQ_CMP:
				result = 0;
				break;
			case GT_CMP:
				result = -1;
				break;
			}
		}
		if (result) break;
	}
	return(0);
}

GrapaError GrapaDB::PtrToRec(GrapaCursor& ptrCursor, GrapaCursor& recCursor)
{
	GrapaError err=0;
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
	err = GetTreeStore(ptrCursor,tableRef,storeType);
	if (err) return(err);
	switch(ptrCursor.mValueType)
	{
		case GPTR_ITEM:
			recCursor.Set(tableRef,GREC_ITEM,ptrCursor.mKey);
			recCursor.mTreeType = GROUP_TREE;
			break;
		case RPTR_ITEM:
			recCursor.Set(tableRef,RREC_ITEM,ptrCursor.mKey);
			recCursor.mTreeType = RTABLE_TREE;
			break;
		case CPTR_ITEM:
			recCursor.Set(tableRef,CREC_ITEM,ptrCursor.mKey);
			recCursor.mTreeType = CTABLE_TREE;
			break;
	}
	return Search(recCursor);
}

GrapaError GrapaDB::Delete(GrapaCursor& treeCursor)
{
	// Need to delete the indexes first because of a scenario in PurgeRc
	// where the key has children, a child is promoted and replaces the key in the tree
	// This causes the DeleteKey to fail to delete the index because the key can't be located in the search
	DeleteKeyIndexes(treeCursor);
	return GrapaCore::Delete(treeCursor);
}

GrapaError GrapaDB::DeleteKeyIndexes(GrapaCursor& treeCursor)
{
	GrapaError err=0;
	GrapaCursor tableCursor, indexTableCursor;
	u64 indexRef=0;

	switch(treeCursor.mValueType)
	{
		case GREC_ITEM:
		//case IREC_ITEM:
		case RREC_ITEM:
		case CREC_ITEM:
			if (treeCursor.mTreeRef)
			{
				indexTableCursor.Set(treeCursor.mTreeRef);
				err = GetTreeIndex(indexTableCursor,indexRef);
				if (err) return(err);
				indexTableCursor.Set(indexRef);
				err = First(indexTableCursor);
				if (!err && indexTableCursor.mKey==0)
					err = Next(indexTableCursor);
				while (!err)
				{
					switch(treeCursor.mValueType)
					{
						case GREC_ITEM:
							tableCursor.Set(indexTableCursor.mValue,GPTR_ITEM,treeCursor.mKey);
							break;
						case RREC_ITEM:
							tableCursor.Set(indexTableCursor.mValue,RPTR_ITEM,treeCursor.mKey);
							break;
						case CREC_ITEM:
							tableCursor.Set(indexTableCursor.mValue,CPTR_ITEM,treeCursor.mKey);
							break;
					}
					err = GrapaCore::Delete(tableCursor);
					// Ignore the error...the index could have already been deleted
					// Maybe do a search first and then only delete if it exists? But this adds a search cost.
					//if (err) return(err);
					err = Next(indexTableCursor);
					if (!err && indexTableCursor.mKey==0)
						err = Next(indexTableCursor);
				}
			}
			break;
	}
	return(0);
}

GrapaError GrapaDB::DeleteKey(GrapaCursor& treeCursor)
{
	GrapaError err=0;
	GrapaCursor dataTypeCursor;
	GrapaCursor indexTableCursor;
	GrapaCursor tableCursor;
	u64 tableDT;
	u64 tableRef=0;
	GrapaDBField dbField;

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
			err = DeleteTree(treeCursor.mValue);
			if (err) return(err);
			break;
		case RREC_ITEM:
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
				case GrapaDBField::STORE_VAR:
				case GrapaDBField::STORE_PAR:
					err = GetDataValue(treeCursor.mValue, dbField.mDictOffset, sizeof(dataPtr), &dataPtr, &dataSize);
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
			err = 0;
			break;
		}
		break;
	case GPTR_ITEM:
	case RPTR_ITEM:
	case CPTR_ITEM:
		// nothing to delete
		break;
	case DTYPE_ITEM:
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
		// should not ever ever get here
		break;
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaDB::DumpTheStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor, u64 tableDT)
{
	GrapaError err = 0;
	u64 returnLen=0;
	GrapaCursor dataTypeCursor;
	GrapaDBField dbField;
	GrapaCHAR dbChar;

	dataTypeCursor.Set(tableDT);
	err = First(dataTypeCursor);
	if (!err && dataTypeCursor.mKey==0)
		err = Next(dataTypeCursor);
	while (!err)
	{
		returnLen = 0;
		err = dbField.Read(this, dataTypeCursor.mValue);
		if (err) return(err);
		err = GetRecordField(cursor, dbField, dbChar);
		if (err) return(err);
		if (dbChar.mBytes == NULL)
		{
			dbChar.FROM("NULL");
			dbChar.mToken = GrapaTokenType::STR;
		}
		else
		{
			GrapaInt n;
			GrapaFloat d(false, 16, 4, 0);
			GrapaTIME t;
			switch (dbChar.mToken)
			{
			case GrapaTokenType::SYSINT:
			case GrapaTokenType::INT:
				n.FromBytes(dbChar);
				dbChar = n.ToString(10);
				break;
			case GrapaTokenType::TABLE:
			case GrapaTokenType::RAW:
				n.FromBytes(dbChar);
				dbChar = n.ToString(16);
				break;
			case GrapaTokenType::FLOAT:
				d.FromBytes(dbChar);
				dbChar = d.ToString(16);
				break;
			case GrapaTokenType::TIME:
				t.FromBytes(dbChar);
				dbChar = t.getString();
				break;
			case GrapaTokenType::BOOL:
				dbChar.FROM((dbChar.mBytes && dbChar.mLength && dbChar.mBytes[0] && (dbChar.mBytes[0] != '0')) ? "true" : "false");
				break;
			default: break;
			}
			if (dbChar.GetLength() > 32) dbChar.SetLength(32);
		}
		dbWrite.mLength = sprintf((char*)dbWrite.mBytes, "%llu=%s ", dbField.mId, dbChar.mBytes);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength, dbWrite.mBytes);
		else printf((char*)dbWrite.mBytes,"");
		err = Next(dataTypeCursor);
		if (!err && dataTypeCursor.mKey == 0)
			err = Next(dataTypeCursor);
	}
	return(0);
}

GrapaError GrapaDB::DumpTheGroupStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaCursor itemCursor;

	tableRef = cursor.mTreeRef;
	err = GetDataTypeRecord(tableRef,tableDT);
	if (err) return(err);
	itemCursor.Set(tableRef,GREC_ITEM,cursor.mKey,cursor.mValue);
	return DumpTheStructure(dbWrite,itemCursor,tableDT);
}

GrapaError GrapaDB::DumpTheRowStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaCursor itemCursor;

	tableRef = cursor.mTreeRef;
	err = GetDataTypeRecord(tableRef,tableDT);
	if (err) return(err);
	itemCursor.Set(tableRef,RREC_ITEM,cursor.mKey,cursor.mValue);
	return DumpTheStructure(dbWrite,itemCursor,tableDT);
}

GrapaError GrapaDB::DumpTheColStructure(GrapaCHAR& dbWrite, GrapaCursor& cursor)
{
	GrapaError err;
	u64 tableRef,tableDT;
	GrapaCursor itemCursor;

	tableRef = cursor.mTreeRef;
	err = GetDataTypeRecord(tableRef,tableDT);
	if (err) return(err);
	itemCursor.Set(tableRef,CREC_ITEM,cursor.mKey,cursor.mValue);
	itemCursor.mNodeRef = cursor.mNodeRef;
	itemCursor.mNodeIndex = cursor.mNodeIndex;
	itemCursor.mLength = cursor.mLength;
	return DumpTheStructure(dbWrite,itemCursor,tableDT);
}

GrapaError GrapaDB::DumpTheNumber(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sSU64 key=%llu value=%llu\n",leader,cursor.mKey,cursor.mValue);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	return(0);
}

GrapaError GrapaDB::DumpGetItemWeight(GrapaCursor& cursor, u64& weight)
{
	GrapaError err;
	GrapaBlockNodeHeader node;
	err = node.Read(mFile,cursor.mNodeRef);
	weight = node.weight;
	return(err);
}

GrapaError GrapaDB::DumpTheDataType(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	GrapaError err;
	char dataBlock[201];
	u64 returnLen=0;
	u64 growBlockSize;
	u64 dataSize,dataLength=0;
	u64 weight;
	GrapaBlockDataHeader data;
	u64 parent=0;
	u8 compressType=0;
	err = data.Read(mFile,cursor.mValue);
	if (err) return(err);
	err = GetDataParent(cursor.mValue,parent);
	DumpGetItemWeight(cursor,weight);
	switch(data.dataType)
	{
		case BYTE_DATA:
			err = GetDataSize(cursor.mValue, growBlockSize, dataSize, dataLength, compressType);
			err = GetDataValue(cursor.mValue,0,200,&dataBlock,&returnLen);
			if (err) return(err);
			if (returnLen>dataLength) returnLen = dataLength;
			dataBlock[returnLen] = 0;
			dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sBYTE_DATA (%llu) parent=%llu key=(%llu/%llu,%llu/%llu) value=%s node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,parent,cursor.mKey,dataSize,GrapaMem::Blocks(cursor.mKey,growBlockSize),GrapaMem::Blocks(dataSize,growBlockSize),dataBlock,cursor.mNodeRef,cursor.mNodeIndex,weight);
			if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
			else printf((char*)dbWrite.mBytes,"");
			break;
		case FREC_DATA:
			err = data.Read(mFile,cursor.mValue);
			if (err) return(err);
			DumpTheTree(dbWrite,leader,cursor.mKey,data.dataStart);
			break;
		default:
			dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sUNKNOWN_DATA (%llu) parent=%llu key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,parent,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
			if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
			else printf((char*)dbWrite.mBytes,"");
			break;
	}
	return(0);
}

GrapaError GrapaDB::DumpTheGroupRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char leadbuf[201];
	u64 weight;
	DumpGetItemWeight(cursor,weight);
	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sGREC start (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	DumpTheTree(dbWrite,leadbuf,cursor.mKey,cursor.mValue);
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sGREC ended (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	return(0);
}

GrapaError GrapaDB::DumpTheGroupPtr(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char leadbuf[201];
	u64 weight;
	DumpGetItemWeight(cursor,weight);
	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sGPTR start (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	DumpTheTree(dbWrite,leadbuf,cursor.mKey,cursor.mValue);
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sGPTR ended (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	return(0);
}

GrapaError GrapaDB::DumpTheItemRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char leadbuf[201];
	u64 weight;
	DumpGetItemWeight(cursor,weight);
	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sIREC start (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	DumpTheTree(dbWrite,leadbuf,cursor.mKey,cursor.mValue);
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sIREC ended (%llu) key=%llu node=(%llu,%d) weight=%llu\n",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	return(0);
}

GrapaError GrapaDB::DumpTheRowRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	u64 weight;
	DumpGetItemWeight(cursor,weight);
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sRREC (%llu) key=%llu node=(%llu,%d) weight=%llu: ",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	DumpTheRowStructure(dbWrite,cursor);
	if (mDumpFile) mDumpFile->Append(1,(void*)"\n");
	else printf("\n");
	return(0);
}

GrapaError GrapaDB::DumpTheColRec(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	u64 weight;
	DumpGetItemWeight(cursor,weight);
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sCREC (%llu) key=%llu node=(%llu,%d) weight=%llu: ",leader,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	DumpTheColStructure(dbWrite,cursor);
	if (mDumpFile) mDumpFile->Append(1,(void*)"\n");
	else printf("\n");
	return(0);
}

GrapaError GrapaDB::DumpTheDT(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaDBField dbField;
	char nameBlock[201];
	u64 growBlockSize, dataSize, dataLength, returnLen;
	u8 compressType=0;

	err = dbField.Read(this,cursor.mValue);
	if (err) return(err);

	nameBlock[0] = 0;
	if (dbField.mNameRef)
	{
		err = GetDataSize(dbField.mNameRef, growBlockSize, dataSize, dataLength, compressType);
		err = GetDataValue(dbField.mNameRef, 0, 200, &nameBlock, &returnLen);
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
		case GrapaTokenType::VECTOR: fieldTypeStr = (char*)"VECTOR"; break;
		case GrapaTokenType::WIDGET: fieldTypeStr = (char*)"WIDGET"; break;
		case GrapaTokenType::LIST: fieldTypeStr = (char*)"LIST"; break;
		case GrapaTokenType::XML: fieldTypeStr = (char*)"XML"; break;
		case GrapaTokenType::EL: fieldTypeStr = (char*)"EL"; break;
		case GrapaTokenType::TAG: fieldTypeStr = (char*)"TAG"; break;
		case GrapaTokenType::OP: fieldTypeStr = (char*)"OP"; break;
		case GrapaTokenType::CODE: fieldTypeStr = (char*)"CODE"; break;
		case GrapaTokenType::TABLE: fieldTypeStr = (char*)"TABLE"; break;
	}

	char *fieldStoreStr = (char*)"?";
	switch (dbField.mStore)
	{
	case GrapaDBField::STORE_FIX: fieldStoreStr = (char*)"FIX"; break;
	case GrapaDBField::STORE_VAR: fieldStoreStr = (char*)"VAR"; break;
	case GrapaDBField::STORE_PAR: fieldStoreStr = (char*)"PAR"; break;
	}

	char *treeTypeStr = (char*)"?";
	switch(dbField.mTreeType)
	{
		case SU64_TREE:	treeTypeStr = (char*)"SU64"; break;
		case GROUP_TREE:	treeTypeStr = (char*)"GROUP"; break;
		case RTABLE_TREE:	treeTypeStr = (char*)"ROW"; break;
		case CTABLE_TREE:	treeTypeStr = (char*)"COL"; break;
		case SDATA_TREE:	treeTypeStr = (char*)"SDATA"; break;
		case BDATA_TREE:	treeTypeStr = (char*)"BDATA"; break;
	}

	dbWrite.mLength = sprintf((char*)dbWrite.mBytes, "%sFIELD (%llu) key=%llu id=%llu name=%s rec=%s type=%s store=%s doffset=%llu dsize=%llu size=%llu grow=%llu\n", leader, cursor.mValue, cursor.mKey, dbField.mId, nameBlock, treeTypeStr, fieldTypeStr, fieldStoreStr, dbField.mDictOffset, dbField.mDictSize, dbField.mSize, dbField.mGrow);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	return(0);
}

GrapaError GrapaDB::DumpThePointer(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char* itemTypeStr=(char*)"";
	u64 weight;

	DumpGetItemWeight(cursor,weight);

	switch(cursor.mValueType)
	{
		case GPTR_ITEM:	itemTypeStr = (char*)"GPTR"; break;
		case RPTR_ITEM:	itemTypeStr = (char*)"RPTR"; break;
		case CPTR_ITEM:	itemTypeStr = (char*)"CPTR"; break;
	}

	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%s%s (%llu) key=%llu node=(%llu,%d) weight=%llu: ",leader,itemTypeStr,cursor.mValue,cursor.mKey,cursor.mNodeRef,cursor.mNodeIndex,weight);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	GrapaCursor recCursor = cursor;
	PtrToRec(cursor,recCursor);
	switch(cursor.mValueType)
	{
		case GPTR_ITEM:	DumpTheGroupStructure(dbWrite,recCursor); break;
		case RPTR_ITEM:	DumpTheRowStructure(dbWrite,recCursor); break;
		case CPTR_ITEM:	DumpTheColStructure(dbWrite,recCursor); break;
	}
	if (mDumpFile) mDumpFile->Append(1,(void*)"\n");
	else printf("\n");
	return(0);
}

GrapaError GrapaDB::DumpTheTreeItem(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
{
	char leadbuf[201];
	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sTREE start (%llu) key=%llu\n",leader,cursor.mValue,cursor.mKey);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	DumpTheTree(dbWrite,leadbuf,cursor.mKey,cursor.mValue);
	dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sTREE ended (%llu) key=%llu\n",leader,cursor.mValue,cursor.mKey);
	if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
	else printf((char*)dbWrite.mBytes,"");
	return(0);
}

GrapaError GrapaDB::DumpTheValue(GrapaCHAR& dbWrite, char *leader, GrapaCursor& cursor)
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

GrapaError GrapaDB::DumpTheTree(GrapaCHAR& dbWrite, const char *leader, u64 tableId, u64 firstTree)
{
	GrapaError err=0;
	GrapaCursor cursor;
	u64 indexRef=0;
	u64 storeTree=0;
	u8 storeType;
	char leadbuf[201];
	u8 treeType=0;
	u64 parentTree;
	u64 numItems;

	if (firstTree==0)
	{
		return((GrapaError)-1);
	}

	strcpy(leadbuf,leader);
	strcat(leadbuf,"| ");

	cursor.Set(firstTree);
	err = GetTreeIndex(cursor,indexRef);
	err = GetTreeStore(cursor,storeTree,storeType);
	err = GetTreeType(cursor,treeType);
	err = GetTreeParent(cursor,parentTree);
	err = GetTreeSize(cursor,numItems);

	char* treeTypeStr=(char*)"";

	switch(treeType)
	{
		case SU64_TREE:	treeTypeStr = (char*)"SU64"; break;
		case GROUP_TREE:	treeTypeStr = (char*)"GROUP"; break;
		case RTABLE_TREE:	treeTypeStr = (char*)"RTABLE"; break;
		case CTABLE_TREE:	treeTypeStr = (char*)"CTABLE"; break;
		case SDATA_TREE:	treeTypeStr = (char*)"SDATA"; break;
		case BDATA_TREE:	treeTypeStr = (char*)"BDATA"; break;
	}

	cursor.Set(firstTree);
	err = First(cursor);
	if (!err)
	{
		dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sLIST start (%llu) key=%llu type=%s parent=%llu size=%llu\n",leader,firstTree,tableId,treeTypeStr,parentTree,numItems);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		else printf((char*)dbWrite.mBytes,"");
		while(!err)
		{
			DumpTheValue(dbWrite,leadbuf,cursor);
			err = Next(cursor);
		}
		dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sLIST ended (%llu) key=%llu type=%s\n",leader,firstTree,tableId,treeTypeStr);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		else printf((char*)dbWrite.mBytes,"");
	}

	if (storeTree)
	{
		switch (storeType)
		{
			case IPTR_STORE:
				dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sSTORE (%llu) key=%llu type=IPTR\n",leader,storeTree,tableId);
				if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
				else printf((char*)dbWrite.mBytes,"");
				break;
			case DATA_STORE:
				dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sSTORE start (%llu) key=%llu type=DATA\n",leader,storeTree,tableId);
				if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
				else printf((char*)dbWrite.mBytes,"");
				DumpTheTree(dbWrite,leadbuf,tableId,storeTree);
				dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sSTORE ended (%llu) key=%llu type=DATA\n",leader,storeTree,tableId);
				if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
				else printf((char*)dbWrite.mBytes,"");
				break;
		}
	}

	if (indexRef)
	{
		dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sINDEX start (%llu) key=%llu\n",leader,indexRef,tableId);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		else printf((char*)dbWrite.mBytes,"");
		DumpTheTree(dbWrite,leadbuf,tableId,indexRef);
		dbWrite.mLength = sprintf((char*)dbWrite.mBytes,"%sINDEX ended (%llu) key=%llu\n",leader,indexRef,tableId);
		if (mDumpFile) mDumpFile->Append(dbWrite.mLength,dbWrite.mBytes);
		else printf((char*)dbWrite.mBytes,"");
	}

	return(0);
}

////////////////////////////////////////////////////////////////////////////////

void GrapaDBCursor::SetSearch(GrapaDB* pDb, u64 pTreeRef, bool pUsingIndex, GrapaDBFieldValueArray* pData)
{
	mData = pData;
	mUsingIndex = pUsingIndex;
	Set(pTreeRef,GrapaDB::SEARCH_ITEM);
}

////////////////////////////////////////////////////////////////////////////////

GrapaDBField::GrapaDBField() { Init(1, 0, 0, 0, 1); }
void GrapaDBField::BigEndian()
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
}
void GrapaDBField::Init(u64 pFieldId, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{
	memset(GetPtr(), 0, GetSize());
	mId = pFieldId;
	mType = pType;
	mStore = pStore;
	mDictSize = pSize;
	mSize = pSize;
	mGrow = pGrow;
}
void* GrapaDBField::GetPtr() { return(&mType); }
u16 GrapaDBField::GetSize() { return(96); }

GrapaError GrapaDBField::Write(GrapaDB *pDb, u64 fieldRef)
{
	BigEndian();
	GrapaError err = pDb->SetDataValue(fieldRef,0,GetSize(),GetPtr());
	BigEndian();
	return(err);
}

GrapaError GrapaDBField::Read(GrapaDB *pDb, u64 fieldRef)
{
	GrapaError err = pDb->GetDataValue(fieldRef,0,GetSize(),GetPtr());
	BigEndian();
	return(err);
}

GrapaError GrapaDBField::Get(GrapaDB *pDb, u64 tableRef, u64 fieldId)
{
	GrapaError err=0;
	GrapaDBCursor dataTypeCursor;
	u64 tableDT;

	err = pDb->GetDataTypeRecord(tableRef,tableDT);
	if (err) return(err);

	dataTypeCursor.Set(tableDT,GrapaDB::SU64_ITEM,fieldId,0);
	err = pDb->Search(dataTypeCursor);
	if (err) return(err);

	err = Read(pDb,dataTypeCursor.mValue);
	if (err) return(err);

	return(0);
}

void GrapaDBFieldValue::BigEndian()
{
	GrapaDBField::BigEndian();
	mValue.mLength = BE_S64(mValue.mLength);
	mValue.mSize = BE_S64(mValue.mSize);
	mCmp = BE_S16(mCmp);
}

////////////////////////////////////////////////////////////////////////////////

GrapaDBFieldArray::~GrapaDBFieldArray()
{
	u32 count,i;
	count = Count();
	for (i=0;i<count;i++)
	{
		GrapaDBField* dbField = GetFieldAt(i);
		if (dbField)
		{
			delete dbField;
		}
	}
	SetSize(0);
}

GrapaError GrapaDBFieldArray::Append(GrapaDB *pDb, GrapaDBTable& pTable, u64 pFieldId)
{
	GrapaError err;
	GrapaDBField* dbField = new GrapaDBField();
	err = pDb->OpenTableField(pTable,pFieldId,*dbField);
	if (err)
	{
		delete dbField;
		return(err);
	}
	GrapaVoidArray::Append((void*)dbField);
	return(0);
}

GrapaError GrapaDBFieldArray::Append(GrapaDBField *pField)
{
	GrapaDBField* dbField = new GrapaDBField();
	*dbField = *pField;
	GrapaVoidArray::Append((void*)dbField);
	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaDBFieldValueArray::~GrapaDBFieldValueArray()
{
	u32 count,i;
	count = Count();
	for (i=0;i<count;i++)
	{
		GrapaDBFieldValue *dbFieldValue = GetFieldAt(i);
		if (dbFieldValue)
		{
			delete dbFieldValue;
		}
	}
	SetSize(0);
}

GrapaError GrapaDBFieldValueArray::Append(GrapaDB *pDb, GrapaDBTable& pTable, u64 pFieldId, const GrapaBYTE& pValue, s16 pCmp)
{
	GrapaError err;
	GrapaDBFieldValue* dbFieldValue = new GrapaDBFieldValue();
	err = pDb->OpenTableField(pTable,pFieldId,*dbFieldValue);
	if (err)
	{
		delete dbFieldValue;
		return(err);
	}
	//pValue.TO(dbFieldValue->mValue);
	dbFieldValue->mValue.FROM(pValue);
	dbFieldValue->mCmp = pCmp;
	GrapaVoidArray::Append((void*)dbFieldValue);
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created

