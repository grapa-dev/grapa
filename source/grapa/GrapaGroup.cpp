// GrapaGroup.cpp
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

// ADD PTR types (folder pointers)
// Fix the indexes to allow multiple field search.

#include "GrapaGroup.h"
#include "GrapaState.h"
#include "GrapaSystem.h"

extern GrapaSystem* gSystem;

#define NAME_INDEX_ID (1)
#define NAME_INDEX_FIELD_NAME_ID 1

#define NAME_FIELD_TYPE GrapaTokenType::STR
#define NAME_FIELD_STORE GrapaDBField::STORE_FIX
#define NAME_FIELD_SIZE 256
#define NAME_FIELD_GROW 0

#define VALUE_FIELD_TYPE GrapaTokenType::RAW
#define VALUE_FIELD_STORE GrapaDBField::STORE_VAR
#define VALUE_FIELD_SIZE 32
#define VALUE_FIELD_GROW 8

GrapaGroup::GrapaGroup() 
{ 
	mRootTable = 0; 
	mRootType = 0; 
}

GrapaGroup::GrapaGroup(GrapaFile*pFile) 
{ 
	INIT(pFile); 
}

GrapaGroup::~GrapaGroup() 
{ 
	if (mFile) mFile->Flush(); 
	mRootTable = 0; 
	mRootType = 0; 
}

void GrapaGroup::INIT(GrapaFile*pFile) 
{ 
	mTree.SetFile(pFile);
	//mCache.SetFile(&mTree);
	///mCache.SetCache();
	SetFile(&mTree);
}

bool GrapaGroup::Opened()
{ 
	if (mFile) return mFile->Opened(); 
	return false; 
}

GrapaError GrapaGroup::OpenFile(const GrapaCHAR& fileName, char mode)
{
	GrapaError err = 0;
	mCritical.WaitCritical();
	err = GrapaDB::OpenFile((const char*)fileName.mBytes, mode);
	mRootTable = RootTree(mRootType);
	if (mRootType == GROUP_TREE)
	{
		GrapaDBTable parentDict;
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

GrapaError GrapaGroup::OpenFile(GrapaFile*pFile, char mode)
{
	GrapaCHAR s;
	GrapaError err;
	INIT(pFile);
	s.FROM("$");
	err = OpenFile(s, GrapaReadWrite);
	return(err);
}

GrapaError GrapaGroup::CloseFile()
{
	mCritical.WaitCritical();
	GrapaError err = GrapaDB::CloseFile();
	mRootTable = 0;
	mRootType = 0;
	mCritical.LeaveCritical();
	return(err);
}

GrapaError GrapaGroup::GetNameId(u64 parentTree, u8 parentType, u64& pNameId)
{
	pNameId = 0;

	GrapaError err;
	GrapaDBTable parentDict;
	parentDict.mRef = parentTree;
	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err) return(-1);
	}

	u64 indexRef;
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err) return(-1);

	GrapaDBCursor cursor;
	cursor.Set(indexRef);
	err = Search(cursor);
	if (err) return(err);

	GrapaDBField field;
	err = field.Read(this, cursor.mValue);
	if (err) return(err);
	pNameId = field.mNameId;

	return(0);
}

GrapaError GrapaGroup::SetNameId(u64 parentTree, u8 parentType, u64 pNameId)
{
	GrapaError err;
	GrapaDBTable parentDict;
	parentDict.mRef = parentTree;
	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err) return(-1);
	}

	u64 indexRef;
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err) return(-1);

	GrapaDBCursor cursor;
	cursor.Set(indexRef);

	err = Search(cursor);
	if (err) return(err);

	GrapaDBField field;
	err = field.Read(this, cursor.mValue);
	if (err) return(err);
	field.mNameId = pNameId;
	err = field.Write(this, cursor.mValue);
	if (err) return(err);

	return(0);
}

GrapaError GrapaGroup::NextNameId(u64 parentTree, u8 parentType, u64& pNameId)
{
	pNameId = 0;

	GrapaError err;
	GrapaDBTable parentDict;
	parentDict.mRef = parentTree;
	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err) return(-1);
	}

	u64 indexRef;
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err) return(-1);

	err = FirstFreeId(indexRef, 1, pNameId);

	GrapaDBCursor cursor;
	cursor.Set(indexRef, RREC_ITEM, pNameId);
	err = Search(cursor);
	if (!err) return(-1);

	return(0);
}

GrapaDBFieldArray* GrapaGroup::ListFields(u64 parentTree, u8 parentType)
{
	GrapaError err;
	GrapaDBTable parentDict;

	parentDict.mRef = parentTree;
	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err) return(NULL);
	}

	GrapaDBCursor cursor;
	u64 indexRef;
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err) return(NULL);

	u64 fieldCount;
	cursor.Set(indexRef);
	err = GetTreeSize(cursor, fieldCount);

	GrapaDBFieldArray* fieldList = new GrapaDBFieldArray((u32)fieldCount);
	GrapaDBField field;
	err = First(cursor);
	if (!err && cursor.mKey == 0)
	{
		err = field.Read(this, cursor.mValue);
		err = Next(cursor);
	}
	while (!err) // skip the first entry
	{
		err = field.Read(this, cursor.mValue);
		if (!err) fieldList->Append(&field);
		err = Next(cursor);
		if (!err && cursor.mKey == 0)
		{
			err = field.Read(this, cursor.mValue);
			err = Next(cursor);
		}
	}
	return fieldList;
}

GrapaError GrapaGroup::FindField(u64 parentTree, u8 parentType, const GrapaCHAR& pFieldNameX, GrapaDBField& pField, u64& pMaxId)
{
	GrapaError err;
	GrapaDBFieldArray* fieldList = ListFields(parentTree, parentType);
    GrapaCHAR fldName(pFieldNameX);
	pMaxId = 0;
	if (!fieldList) return(-1);
	for (u32 i = 0; i < fieldList->Count(); i++)
	{
		GrapaDBField *field = fieldList->GetFieldAt(i);
		if (!field) continue;
		if (field->mId> pMaxId) pMaxId = field->mId;
		GrapaCHAR fieldName;
		u64 dataSize, dataLength = 0, growBlockSize;
		u64 returnSize;
		u8 compressType=0;
		if (field->mNameRef && !(err = GetDataSize(field->mNameRef, growBlockSize, dataSize, dataLength, compressType)))
		{
			fieldName.SetLength(dataLength, false);
			if (dataLength)
			{
				err = GetDataValue(field->mNameRef, 0, dataLength, fieldName.GetPtr(), &returnSize);
			}
		}
		if (fldName.StrCmp(fieldName) == 0)
		{
			pField = *field;
			delete fieldList;
			return(0);
		}
	}
	delete fieldList;
	return(-1);
}

GrapaError GrapaGroup::DeleteGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName)
{
	GrapaError err;
	u64 newDirId;
	u8 newDirType;
	u64 tableId;
	err = OpenGroup(parentTree, parentType, pTableName, newDirId, newDirType,tableId);
	if (err) return(err);
	mCritical.WaitCritical();
	err = DeleteTable(parentTree, tableId);
	mCritical.LeaveCritical();
	return(err);
}

GrapaError GrapaGroup::CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, GrapaCHAR pTableType, u64 & pNewTree)
{
	u8 listType = GrapaDB::GROUP_TREE;
	if (pTableType.StrCmp("DIR") == 0) listType = GrapaDB::GROUP_TREE;
	else if (pTableType.StrCmp("GROUP") == 0) listType = GrapaDB::GROUP_TREE;
	else if (pTableType.StrCmp("ROW") == 0) listType = GrapaDB::RTABLE_TREE;
	else if (pTableType.StrCmp("COL") == 0) listType = GrapaDB::CTABLE_TREE;
	return CreateGroup(parentTree, parentType, pTableName, listType, pNewTree);
}

GrapaError GrapaGroup::CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, u8 listType, u64 & pNewTree)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName,dbFieldValue;
	GrapaDBIndex dbIndexName;
	GrapaDBTable table;

	pNewTree = 0;

	if (parentTree == 0)
	{
		err = Create((char*)pTableName.mBytes, listType, pNewTree);
		mRootTable = RootTree(mRootType);
		if (mRootType == GROUP_TREE)
		{
			GrapaDBTable parentDict;
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

	// for now, we shouldn't create a table in a table. Only create a table in a group.
	// the reason is because the structure for tables vary by implmentation,
	// and so figuring out how to navigate could be hard. And indexing may be broken for the table.

	if (parentType != GROUP_TREE)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	if (nameId)
	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, nameId, pTableName, EQ_CMP);
		err = SearchDb(cursor, parentDict, data);
		if (!err)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}

	u64 tableId;
	err = FirstFreeId(parentDict.mRef, 1, tableId);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	err = CreateTable(parentTree, listType, tableId, table);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	pNewTree = table.mRef;

	// Ensure we have a table name lookup field in the parentTree
	if (nameId == 0 || OpenTableField(parentDict, nameId, dbFieldName) != 0)
	{
		err = NextNameId(parentTree, parentType, nameId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		if (nameId == 0)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
		GrapaCHAR fieldNameLabel("$KEY");
		dbFieldName.Init(nameId, NAME_FIELD_TYPE, NAME_FIELD_STORE, NAME_FIELD_SIZE, NAME_FIELD_GROW);
		err = CreateTableField(parentDict, dbFieldName, fieldNameLabel);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		err = SetNameId(parentTree, parentType, nameId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		{
			GrapaDU64Array catIndexList(1);
			catIndexList.Append(NAME_INDEX_FIELD_NAME_ID, nameId);
			err = CreateIndex(parentDict, NAME_INDEX_ID, catIndexList, dbIndexName);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
		}
	}

	cursor.Set(parentDict.mRecRef, RREC_ITEM, tableId);
	err = Search(cursor);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, nameId, pTableName, EQ_CMP);
		err = SetRecordField(cursor, data);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pEntryName, u64& pNewTree, GrapaCHAR& pNewType, u64& pTableId)
{
	u8 newType;
	GrapaError err = OpenGroup(parentTree, parentType, pEntryName, pNewTree, newType, pTableId);
	pNewType.FROM("");
	switch (newType)
	{
	case GrapaDB::GROUP_TREE:	pNewType.FROM("GROUP"); break;
	case GrapaDB::RTABLE_TREE:	pNewType.FROM("ROW"); break;
	case GrapaDB::CTABLE_TREE:	pNewType.FROM("COL"); break;
	}
	return(err);
}

GrapaError GrapaGroup::OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pEntryName, u64& pNewTree, u8& pNewType, u64& pTableId)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBFieldValueArray data;
	GrapaDBTable parentDict;
	GrapaDBCursor cursor;
	GrapaDBTable table;

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
			mCritical.LeaveCritical();
			return(err);
		}
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	if (nameId == 0)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	data.Append(this, parentDict, nameId, pEntryName, EQ_CMP);
	err = SearchDb(cursor, parentDict, data);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	err = OpenTable(parentTree,cursor.mKey,table);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	pNewTree = table.mRef;
	pNewType = table.mRefType;
	pTableId = table.mId;

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::OpenGroup(u64 parentTree, u8 parentType, u64 pId, u64& pNewTree, u8& pNewType, u64& pTableId, GrapaCHAR& pName)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBFieldValueArray data;
	GrapaDBTable parentDict;
	GrapaDBCursor cursor;
	GrapaDBTable table;

	pNewTree = 0;
	pNewType = 0;
	pTableId = 0;

	if (pId == 0)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = OpenTable(parentTree, pId, table);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	pNewTree = table.mRef;
	pNewType = table.mRefType;
	pTableId = table.mId;

	u64 indexRef;
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);

	cursor.Set(parentDict.mRecRef, RREC_ITEM, pId);
	err = Search(cursor);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	err = GetRecordField(cursor, nameId, pName);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::CreateEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId)
{
	pId = 0;
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	u64 indexRef;
	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 nameId = 0;
	if (pDataName.mLength && pDataName.mBytes)
	{
		err = GetNameId(parentTree, parentType, nameId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}

		if (nameId)
		{
			GrapaDBFieldValueArray data;
			data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
			err = SearchDb(cursor, parentDict, data);
			if (!err)
			{
				mCritical.LeaveCritical();
				return(-1);
			}
		}
	}

	u64 dataId = 0;
	err = FirstFreeId(parentDict.mRef, 1, dataId); // change this to 1
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	cursor.Set(parentDict.mRef, RREC_ITEM, dataId);
	err = CreateRecord(parentDict, cursor);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	pId = dataId;

	if (pDataName.mLength && pDataName.mBytes)
	{
		if (nameId == 0 || OpenTableField(parentDict, nameId, dbFieldName) != 0)
		{
			err = NextNameId(parentTree, parentType, nameId);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
			if (nameId == 0)
			{
				mCritical.LeaveCritical();
				return(-1);
			}
			GrapaCHAR fieldNameLabel("$KEY");
			dbFieldName.Init(nameId, NAME_FIELD_TYPE, NAME_FIELD_STORE, NAME_FIELD_SIZE, NAME_FIELD_GROW);
			err = CreateTableField(parentDict, dbFieldName, fieldNameLabel);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
			err = SetNameId(parentTree, parentType, nameId);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
			{
				GrapaDBIndex dbIndexName;
				GrapaDU64Array catIndexList(1);
				catIndexList.Append(NAME_INDEX_FIELD_NAME_ID, nameId);
				err = CreateIndex(parentDict, NAME_INDEX_ID, catIndexList, dbIndexName);
				if (err)
				{
					mCritical.LeaveCritical();
					return(err);
				}
			}
		}

		cursor.Set(parentDict.mRecRef, RREC_ITEM, dataId);
		err = Search(cursor);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		{
			GrapaDBFieldValueArray data;
			data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
			err = SetRecordField(cursor, data);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
		}
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;
	GrapaDBIndex dbIndexName;
	u64 indexRef;
	
	pId = 0;

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);

	if (!err && nameId == 0)
		err = -1;

	if (err == 0)
	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
		err = SearchDb(cursor, parentDict, data);
		if (err == 0)
			pId = cursor.mKey;
	}

	mCritical.LeaveCritical();
	return(err);
}

GrapaError GrapaGroup::DeleteEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor, tableCursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;
	GrapaDBIndex dbIndexName;
	u64 indexRef;
	GrapaDBTable table;

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err) return(err);

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	if (nameId == 0)
	{
		mCritical.LeaveCritical();
		return(-1);
	}
	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
		err = SearchDb(cursor, parentDict, data);
		if (err)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}
	
	tableCursor.Set(parentTree, TREE_ITEM, cursor.mKey);
	err = Search(tableCursor);
	if (!err)
	{
		mCritical.LeaveCritical();
		return DeleteGroup(parentTree, parentType, pDataName);
	}

	err = DeleteRecord(parentDict, cursor);

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::DeleteEntry(u64 parentTree, u8 parentType, u64 pId)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor, tableCursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;
	GrapaDBIndex dbIndexName;
	u64 indexRef;

	if (pId == 0)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	tableCursor.Set(parentTree, TREE_ITEM, pId);
	err = Search(tableCursor);
	if (!err)
	{
		err = DeleteTable(parentTree, pId);
		mCritical.LeaveCritical();
		return(err);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	cursor.mKey = pId;
	err = DeleteRecord(parentDict, cursor);

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::CreateField(u64 parentTree, u8 parentType, const char* pFieldName, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{
	GrapaCHAR s(pFieldName);
	return CreateField(parentTree, parentType, s, pType, pStore, pSize, pGrow);
}

GrapaError GrapaGroup::CreateField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pType, u8 pStore, u64 pSize, u64 pGrow)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName;
	u64 indexRef;

	bool needKeySet = false;
	bool needIndex = false; // need to be able to set this

	if (pFieldName.mLength == 0 || pFieldName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 fieldId = 0;
	if (pFieldName.StrCmp("$KEY") == 0)
	{
		pFieldName.FROM("$KEY");
		err = GetNameId(parentTree, parentType, fieldId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		if (fieldId == 0) 
		{
			err = NextNameId(parentTree, parentType, fieldId); 
			needIndex = true;
			needKeySet = true;
		}
		if (fieldId == 0)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}
	else
	{
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, pFieldName, field, maxId);
		if (!err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		fieldId = maxId + 1;
	}

	err = OpenTableField(parentDict, fieldId, dbFieldName);
	if (err)
	{
		dbFieldName.Init(fieldId, pType, pStore, pSize, pGrow);
		err = CreateTableField(parentDict, dbFieldName, pFieldName);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		if (needKeySet)
		{
			err = SetNameId(parentTree, parentType, fieldId);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
		}
		if (needIndex)
		{
			GrapaDBIndex dbIndexName;
			GrapaDU64Array catIndexList(1);
			catIndexList.Append(NAME_INDEX_FIELD_NAME_ID, fieldId);
			err = CreateIndex(parentDict, NAME_INDEX_ID, catIndexList, dbIndexName);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
		}
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::DeleteField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	u64 indexRef;

	if (pFieldName.mLength == 0 || pFieldName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 fieldId = 0;
	if (pFieldName.StrCmp("$KEY") == 0)
	{ 
		fieldId = nameId;
		if (nameId == 0)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
		pFieldName.FROM("$KEY"); 
	}
	else
	{
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, pFieldName, field, maxId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		fieldId = field.mId;
	}

	// Remove fieldId from all indexes, otherese delete table field will fail

	err = DeleteTableField(parentDict, fieldId);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	if (fieldId == nameId)
		SetNameId(parentTree, parentType, 0);

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, const char* pFieldName, const GrapaBYTE& pDataValue)
{
	GrapaCHAR s(pFieldName);
	return SetField(parentTree, parentType, pDataName, s, pDataValue);
}

GrapaError GrapaGroup::SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, const GrapaCHAR& pFieldNameX, const GrapaBYTE& pDataValue)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;
	GrapaDBIndex dbIndexName;
	u64 indexRef;
	u64 dataId;
    GrapaCHAR fldName(pFieldNameX);

	///////////// can this be optimized? Reuse all the lookup info from CreateEntry...where CreateEntry is simply ensuring an entry exists...
	mCritical.LeaveCritical();
	u64 id2 = 0, nameId2=0;
	err = CreateEntry(parentTree, parentType, pDataName, nameId2);
	mCritical.WaitCritical();
	////////////

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}
	if (fldName.mLength == 0 || fldName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);

	if (nameId)
	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
		err = SearchDb(cursor, parentDict, data);
		//////////don't need to creatre at this point...because that would have been done at the start of the function
		//if (err)
		//{
			//mCritical.LeaveCritical();
			//u64 id2 = 0;
			//err = CreateEntry(parentTree, parentType, pDataName, id2);
			//mCritical.WaitCritical();
			//GrapaDBFieldValueArray data2;
			//data2.Append(this, parentDict, nameId, pDataName, EQ_CMP);
			//err = SearchDb(cursor, parentDict, data2);
			if (err)
			{
				mCritical.LeaveCritical();
				return(-1);
			}
		//}
	}

	dataId = cursor.mKey;

	cursor.Set(parentDict.mRecRef, RREC_ITEM, dataId);
	err = Search(cursor);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 fieldId = 0;
	if (fldName.StrCmp((const char*)"$KEY") == 0)
	{
		if (nameId == 0)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
		fieldId = nameId;
		fldName.FROM("$KEY");
	}
	else if (fldName.StrCmp((const char*)"$VALUE") == 0)
	{
		fldName.FROM("$VALUE");
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			field.mId = maxId + 1;
			dbFieldName.Init(field.mId, VALUE_FIELD_TYPE, VALUE_FIELD_STORE, VALUE_FIELD_SIZE, VALUE_FIELD_GROW);
			err = CreateTableField(parentDict, dbFieldName, fldName);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
		}
		fieldId = field.mId;
	}
	else
	{
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		fieldId = field.mId;
	}

	if (fieldId == nameId && pDataValue.mLength && pDataValue.mBytes)
	{
		GrapaDBFieldValueArray data;
		GrapaDBCursor findCursor = cursor;
		data.Append(this, parentDict, fieldId, pDataValue, EQ_CMP);
		err = SearchDb(findCursor, parentDict, data);
		if (!err)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}

	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, fieldId, pDataValue, EQ_CMP);
		err = SetRecordField(cursor, data);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::GetField(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, const GrapaCHAR& pFieldNameX, GrapaBYTE& pDataValue)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;
	GrapaDBIndex dbIndexName;
	u64 indexRef;
	u64 dataId;
    GrapaCHAR fldName(pFieldNameX);
    
	pDataValue.SetLength(0);

	if (pDataName.mLength == 0 || pDataName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}
	if (fldName.mLength == 0 || fldName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	dataId = 0;

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);

	if (nameId)
	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, nameId, pDataName, EQ_CMP);
		err = SearchDb(cursor, parentDict, data);
		if (err)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}

	dataId = cursor.mKey;

	cursor.Set(parentDict.mRecRef, RREC_ITEM, dataId);
	err = Search(cursor);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 fieldId = 0;
	if (fldName.StrCmp("$KEY") == 0)
	{ 
		if (nameId == 0)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
		fieldId = nameId;
		fldName.FROM("$KEY");
	}
	else
	{
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		fieldId = field.mId;
	}

	err = GetRecordField(cursor, fieldId, pDataValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::SetField(u64 parentTree, u8 parentType, u64 pId, const char* pFieldName, const GrapaBYTE& pDataValue)
{
	GrapaCHAR s(pFieldName);
	return SetField(parentTree, parentType, pId, s, pDataValue);
}

GrapaError GrapaGroup::SetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pFieldNameX, const GrapaBYTE& pDataValue)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;
	GrapaDBIndex dbIndexName;
	u64 indexRef;
    GrapaCHAR fldName(pFieldNameX);
    
	if (pId == 0)
	{
		mCritical.LeaveCritical();
		return(-1);
	}
	if (fldName.mLength == 0 || fldName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 nameId = 0;
	err = GetNameId(parentTree, parentType, nameId);

	cursor.Set(parentDict.mRecRef, RREC_ITEM, pId);
	err = Search(cursor);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 fieldId = 0;
	if (fldName.StrCmp("$KEY") == 0)
	{
		err = GetNameId(parentTree, parentType, fieldId);
		if (fieldId == 0)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}
	else if (fldName.StrCmp("$VALUE") == 0)
	{
		fldName.FROM("$VALUE");
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			field.mId = maxId + 1;
			dbFieldName.Init(field.mId, VALUE_FIELD_TYPE, VALUE_FIELD_STORE, VALUE_FIELD_SIZE, VALUE_FIELD_GROW);
			err = CreateTableField(parentDict, dbFieldName, fldName);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
		}
		fieldId = field.mId;
	}
	else
	{
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		fieldId = field.mId;
	}

	if (fieldId == nameId && pDataValue.mLength && pDataValue.mBytes)
	{
		GrapaDBFieldValueArray data;
		GrapaDBCursor findCursor = cursor;
		data.Append(this, parentDict, fieldId, pDataValue, EQ_CMP);
		err = SearchDb(findCursor, parentDict, data);
		if (!err)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}

	{
		GrapaDBFieldValueArray data;
		data.Append(this, parentDict, fieldId, pDataValue, EQ_CMP);
		err = SetRecordField(cursor, data);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::GetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pFieldNameX, GrapaBYTE& pDataValue)
{
	mCritical.WaitCritical();

	GrapaError err;
	GrapaDBCursor cursor;
	GrapaDBTable parentDict;
	GrapaDBField dbFieldName, dbFieldValue;
	GrapaDBIndex dbIndexName;
	u64 indexRef;
    GrapaCHAR fldName(pFieldNameX);

	pDataValue.SetLength(0);

	if (pId == 0)
	{
		mCritical.LeaveCritical();
		return(-1);
	}
	if (fldName.mLength == 0 || fldName.mBytes == NULL)
	{
		mCritical.LeaveCritical();
		return(-1);
	}

	parentDict.mRef = parentTree;
	parentDict.mRecRef = parentTree;

	if (parentType == GROUP_TREE)
	{
		err = OpenTable(parentTree, 0, parentDict);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
	}

	err = GetDataTypeRecord(parentDict.mRef, indexRef);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	cursor.Set(indexRef);
	err = Search(cursor); // go to 0 item
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}
	err = parentDict.mDictField.Read(this, cursor.mValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	cursor.Set(parentDict.mRecRef, RREC_ITEM, pId);
	err = Search(cursor);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	u64 fieldId = 0;
	if (fldName.StrCmp("$KEY") == 0)
	{
		err = GetNameId(parentTree, parentType, fieldId);
		if (fieldId == 0)
		{
			mCritical.LeaveCritical();
			return(-1);
		}
	}
	else
	{
		GrapaDBField field;
		u64 maxId;
		err = FindField(parentTree, parentType, fldName, field, maxId);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		fieldId = field.mId;
	}

	err = GetRecordField(cursor, fieldId, pDataValue);
	if (err)
	{
		mCritical.LeaveCritical();
		return(err);
	}

	mCritical.LeaveCritical();
	return(0);
}

GrapaError GrapaGroup::DumpGroup(u64 parentTree, u8 parentType, u64 pId, GrapaFile *pDumpFile)
{
	mCritical.WaitCritical();

	GrapaError err;
	if (pId)
	{
		GrapaDBTable parentDict;
		parentDict.mRef = parentTree;
		parentDict.mRecRef = parentTree;

		if (parentType == GROUP_TREE)
		{
			err = OpenTable(parentTree, 0, parentDict);
			if (err)
			{
				mCritical.LeaveCritical();
				return(err);
			}
		}

		u64 indexRef;
		err = GetDataTypeRecord(parentDict.mRef, indexRef);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		GrapaDBCursor cursor;
		cursor.Set(indexRef);
		err = Search(cursor); // go to 0 item
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}
		err = parentDict.mDictField.Read(this, cursor.mValue);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}

		cursor.Set(parentDict.mRecRef, RREC_ITEM, pId);
		err = Search(cursor);
		if (err)
		{
			mCritical.LeaveCritical();
			return(err);
		}

		parentTree = cursor.mTreeRef;
	}
	err = DumpTree(parentTree, pDumpFile);
	mCritical.LeaveCritical();
	return(err);
}


// Verify digital rights on file. So...open with identity. 
// Make note of the opentype for each connection. For now, use R/W.

GrapaGroupEvent* GrapaGroupQueue::OpenFile(const GrapaCHAR& fileName, GrapaFile* pFile, char mode)
{
	mCritical.WaitCritical();
	GrapaGroupEvent*e = Search(fileName);
	if (e == NULL)
	{
		e = new GrapaGroupEvent(fileName, pFile);
		PushTail(e);
	}
	if (e)
	{
		e->mInstanceCount++;
		if (!e->mValue.Opened())
		{
			GrapaError err = e->mValue.OpenFile(fileName, GrapaReadWrite);
			if (err)
			{
				e->mInstanceCount--;
				if (e->mInstanceCount == 0)
				{
					GrapaQueue::PopEvent((GrapaEvent*)e);
					e->CLEAR();
					delete e;
				}
				e = NULL;
			}
		}
	}
	mCritical.LeaveCritical();
	return(e);
}

GrapaGroupEvent* GrapaGroupQueue::Create(const GrapaCHAR& fileName, GrapaFile* pFile, u8 pType)
{
	mCritical.WaitCritical();
	GrapaGroupEvent*e = Search(fileName);
	if (e == NULL)
	{
		e = new GrapaGroupEvent(fileName, pFile);
		PushTail(e);
	}
	if (e)
	{
		e->mInstanceCount++;
		if (!e->mValue.Opened())
		{
			u64 newTree;
			GrapaError err = e->mValue.CreateGroup(0, 0, fileName, pType, newTree);
			if (err)
			{
				e->mInstanceCount--;
				if (e->mInstanceCount == 0)
				{
					GrapaQueue::PopEvent((GrapaEvent*)e);
					e->CLEAR();
					delete e;
				}
				e = NULL;
			}
		}
	}
	mCritical.LeaveCritical();
	return(e);
}

GrapaGroupEvent* GrapaGroupQueue::Create(const GrapaCHAR& fileName, GrapaFile* pFile, GrapaCHAR& pType)
{
	mCritical.WaitCritical();
	GrapaGroupEvent*e = Search(fileName);
	if (e == NULL)
	{
		e = new GrapaGroupEvent(fileName, pFile);
		PushTail(e);
	}
	if (e)
	{
		e->mInstanceCount++;
		if (!e->mValue.Opened())
		{
			u64 newTree;
			GrapaError err = e->mValue.CreateGroup(0, 0, fileName, pType, newTree);
			if (err)
			{
				e->mInstanceCount--;
				if (e->mInstanceCount == 0)
				{
					GrapaQueue::PopEvent((GrapaEvent*)e);
					e->CLEAR();
					delete e;
				}
				e = NULL;
			}
		}
	}
	mCritical.LeaveCritical();
	return(e);
}

void GrapaGroupQueue::CloseFile(GrapaGroupEvent* pEvent)
{
	if (pEvent == NULL) return;
	mCritical.WaitCritical();
	pEvent->mInstanceCount--;
	if (pEvent->mInstanceCount == 0)
	{
		GrapaQueue::PopEvent((GrapaEvent*)pEvent);
		pEvent->CLEAR();
		delete pEvent;
	}
	mCritical.LeaveCritical();
}


////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
