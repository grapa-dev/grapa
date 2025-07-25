// GrapaBtree.cpp
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

#include "GrapaBtree.h"
#include "GrapaMem.h"

#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaBtree::GrapaBtree()
{
	mFile = NULL;
	mFlags = 0;
	memset(mBuffer, 0, sizeof(mBuffer));
}

void GrapaBtree::SetFile(GrapaFile *pFile)
{
	CloseFile();
	mFile = pFile;
}

GrapaError GrapaBtree::OpenFile(const char *fileName, char mode)
{
	GrapaError err;
	bool createIfErr=false;

	if (!mFile) return(-1);

	if (mode == GrapaReadWriteCreate)
	{
		createIfErr = true;
		mode = GrapaReadWrite;
	}
	err = mFile->Open(fileName,mode);
	if (err)
	{
		if (createIfErr)
		{
			err = Create(fileName);
		}
		if (err) return(err);
	}
	else
	{
		GrapaBlockFileHeader hdr;
		if (!FileOpened()) return(-1);
		err = hdr.Read(mFile, 0);
		if (err) 
		{
			mFile->Close();
			return(err);
		}
		if (((hdr.fileRef1 != 'B') && (hdr.fileRef2 != 'T')))
		{
			mFile->Close();
			return(-1);
		}
		GetFlags(mFlags);
		if (IsLittleEndianS())
		{
			if ((mFlags & 0x80) && !BE_SWAPS()) return(-1);
			if (!(mFlags & 0x80) && BE_SWAPS()) return(-1);
		} else {
			if ((mFlags & 0x80) && BE_SWAPS()) return(-1);
			if (!(mFlags & 0x80) && !BE_SWAPS()) return(-1);
		}
	}
	return(0);
}

u64 GrapaBtree::RootTree(u8& pType)
{
	u64 tree = FirstTree(0);
	pType = 0;
	if (tree == 0) return(0);
	GrapaCursor cursor;
	cursor.Set(tree);
	GetTreeType(cursor, pType);
	return(tree);
}

GrapaError GrapaBtree::CloseFile()
{
	if (!mFile) return(0);
	if (FileOpened()) PurgeFile();
	mFile->Close();
	mFlags = 0;
	return(0);
}

GrapaError GrapaBtree::Create(const char *fileName)
{
	GrapaError err;

	if (!mFile) return(-1);

	if (FileOpened()) return(-1);

	err = mFile->Create(fileName);
	if (err) return(err);

	err = ClearFile();

	if (err)
	{
		mFile->Close();
		mFile->Delete(fileName);
		return(err);
	}

	return(0);
}

GrapaError GrapaBtree::RemoveFile(const char *fileName)
{
	GrapaError err;

	if (!mFile) return(-1);

	err = OpenFile(fileName, GrapaReadOnly);
	if (err) return(err);

	CloseFile();

	err = mFile->Delete(fileName);
	if (err) return(err);

	return(err);
}

GrapaError GrapaBtree::FlushFile()
{
	GrapaError err;
	if (!mFile) return(-1);
	err = mFile->Flush();
	return(err);
}

GrapaError GrapaBtree::ClearFile()
{
	GrapaError err;
	GrapaBlockFileHeader hdr;
	GrapaBlockFirst fst;

	if (!mFile) return(-1);

	if (!FileOpened()) return(-1);

	hdr.Init();
	mFlags &= 0x7F;
	if (IsLittleEndianS()) mFlags |= 0x80;
	hdr.flags = mFlags;
	hdr.firstBlock = hdr.GetBlocks();
	err = hdr.Write(mFile,0);

	fst.Init();
	fst.blockCount = ((u64)hdr.GetBlocks()) + fst.GetBlocks();
	err = fst.Write(mFile,hdr.firstBlock);

	err = hdr.Read(mFile,0);
	if (err) return(err);

	err = fst.Read(mFile,hdr.firstBlock);
	if (err) return(err);

	if (fst.blockCount != (((u64)hdr.GetBlocks()) + fst.GetBlocks()))
		return(-1);

	return(0);
}

GrapaError GrapaBtree::PurgeFile()
{
	GrapaError err;
	if (!mFile) return(-1);
	GrapaBlockFileHeader hdr;
	GrapaBlockFirst fst;
	err = hdr.Read(mFile,0);
	if (err) return(err);
	err = fst.Read(mFile,hdr.firstBlock);
	if (err) return(err);
	return mFile->Purge(fst.blockCount,GrapaBlock::BLOCKSIZE);
}

bool GrapaBtree::FileOpened()
{
	if (!mFile) return(false);
	return mFile->Opened();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError	GrapaBtree::GetFlags(u8& settings)
{
	GrapaError err;

	settings = 0;

	if (!FileOpened()) return(-1);

	GrapaBlockFileHeader hdr;
	err = hdr.Read(mFile,0);
	if (err) return(err);

	settings = hdr.flags;

	return(0);
}

GrapaError	GrapaBtree::SetFlags(u8 settings)
{
	GrapaError err;

	if (!FileOpened()) return(-1);

	GrapaBlockFileHeader hdr;
	err = hdr.Read(mFile,0);
	if (err) return(err);

	hdr.flags = settings;
	err = hdr.Write(mFile,0);
	if (err) return(err);

	GetFlags(mFlags);

	return(0);
}

GrapaError	GrapaBtree::GetCompressSize(u8& size)
{
	GrapaError err;

	size = 0;

	if (!FileOpened()) return(-1);

	GrapaBlockFileHeader hdr;
	err = hdr.Read(mFile, 0);
	if (err) return(err);

	size = hdr.compressSize;

	return(0);
}

GrapaError	GrapaBtree::SetCompressSize(u8 size)
{
	GrapaError err;

	if (!FileOpened()) return(-1);

	GrapaBlockFileHeader hdr;
	err = hdr.Read(mFile, 0);
	if (err) return(err);

	hdr.compressSize = size;
	err = hdr.Write(mFile, 0);
	if (err) return(err);

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaBtree::SetFileTree (u64 firstTree, bool deleteExisting)
{
	GrapaError err;
	GrapaBlockFileHeader hdr;
	err = hdr.Read(mFile,0);
	if (err) return(err);
	if (hdr.firstTree && deleteExisting) 
		DeleteTree(hdr.firstTree);
	hdr.firstTree = firstTree;
	return hdr.Write(mFile,0);
}

u64 GrapaBtree::FirstTree(u64 treePtr)
{
	GrapaError err;
	GrapaBlockFileHeader hdr;
	if (treePtr==0L)
	{
		err = hdr.Read(mFile,0);
		if (err) return(0);
		return(hdr.firstTree);
	}
	else
	{
		GrapaCursor cursor;
		u64 parentTree;
		cursor.Set(treePtr);
		err = GetTreeParent(cursor, parentTree);
		if (err) return(0);
		return(parentTree);
	}
	return(0);
}

GrapaError GrapaBtree::NewTree(u64& treePtr, u8 treeType, u64 parentTree, u8 nodeCount)
{
	GrapaError err=0;
	GrapaBlockTree kb;
	u64 offset;
	
	treePtr = 0;

	offset = NewPage(kb.GetSize(),false);
	if (offset==0) return(-1);

	kb.Init();
	kb.nodeCount = nodeCount;
	kb.parentTree = parentTree;
	kb.treeType = treeType;

	err = kb.Write(mFile,offset);
	if (err) 
	{
		EmptyItem(-1,kb,offset);
		return(err);
	}

	treePtr = offset;

	return(0);
}

GrapaError	GrapaBtree::DeleteTree(u64 treePtr)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	if (treePtr==0L) return(-1);

	err = kb.Read(mFile,treePtr);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	err = EmptyItem(0,kb,treePtr);
	if (err) return(err);

	return(0);
}

GrapaError GrapaBtree::EmptyTree(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	if (cursor.mValue==0L) return(-1);

	err = kb.Read(mFile,cursor.mValue);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	EmptyItem(cursor.mTreeRef, kb, kb.firstItem);

	kb.firstItem = 0L;
	err = kb.Write(mFile,cursor.mValue);

	return(0);
}

GrapaError GrapaBtree::SetTreeIndex(GrapaCursor& cursor, u64 indexTree)
{
	GrapaError err = 0;
	GrapaBlockTree head;

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	head.indexTree = indexTree;

	err = head.Write(mFile,cursor.mTreeRef);
	if (err) return(err);

	return(0);
}

GrapaError GrapaBtree::GetTreeIndex(GrapaCursor& cursor, u64& indexTree)
{
	GrapaError err = 0;
	GrapaBlockTree head;

	indexTree = 0;

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	indexTree = head.indexTree;

	return(0);
}

GrapaError GrapaBtree::SetTreeStore(GrapaCursor& cursor, u64 storeTree, u8 storeType)
{
	GrapaError err = 0;
	GrapaBlockTree head;

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	head.storeTree = storeTree;
	head.storeType = storeType;

	err = head.Write(mFile,cursor.mTreeRef);
	if (err) return(err);

	return(0);
}

GrapaError GrapaBtree::GetTreeStore(GrapaCursor& cursor, u64& storeTree, u8& storeType)
{
	GrapaError err = 0;
	GrapaBlockTree head;

	storeTree = 0;

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	storeTree = head.storeTree;
	storeType = head.storeType;

	return(0);
}

GrapaError	GrapaBtree::GetTreeSize(GrapaCursor& cursor, u64& itemCount)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	itemCount = 0L;

	if (cursor.mTreeRef==0L) return(-1);

	err = kb.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	itemCount = kb.itemCount;

	return(0);
}

GrapaError	GrapaBtree::GetTreeDirty(GrapaCursor& cursor, bool& isDirty)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	isDirty = false;

	if (cursor.mTreeRef==0L) return(-1);

	err = kb.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	isDirty = kb.GetDirty();

	return(0);
}

GrapaError	GrapaBtree::SetTreeDirty(GrapaCursor& cursor, bool isDirty)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	if (cursor.mTreeRef==0L) return(-1);

	err = kb.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	kb.SetDirty(isDirty);

	err = kb.Write(mFile,cursor.mTreeRef);
	if (err) return(err);

	return(0);
}

GrapaError	GrapaBtree::GetTreeType(GrapaCursor& cursor, u8& treeType)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	treeType = 0;

	if (cursor.mTreeRef==0L) return(-1);

	err = kb.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	treeType = kb.treeType;

	return(0);
}

GrapaError	GrapaBtree::SetTreeType(GrapaCursor& cursor, u8 treeType)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	if (cursor.mTreeRef==0L) return(-1);

	err = kb.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	kb.treeType = treeType;

	err = kb.Write(mFile,cursor.mTreeRef);
	if (err) return(err);

	return(0);
}

GrapaError GrapaBtree::GetTreeParent(GrapaCursor& cursor, u64& parentTree)
{
	GrapaError err = 0;
	GrapaBlockTree kb;

	parentTree = 0;

	if (cursor.mTreeRef==0L) return(-1);

	err = kb.Read(mFile,cursor.mTreeRef);
	if (err) return(err);

	if (kb.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	parentTree = kb.parentTree;

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError	GrapaBtree::NewData(u8 dataType, u64 parentTree, u64 dataSize, u64 growBlockSize, u64 growBlocks, u64& itemPtr, bool clear)
{
	GrapaError err=-1;
	GrapaBlockDataHeader data;
	u64 offset;

	itemPtr = 0;

	offset = NewPage(data.GetSize(),false);
	if (offset==0) return(-1);

	data.Init();
	data.dataType = dataType;
	data.parentTree = parentTree;
	data.dataSize = dataSize;
	data.growBlockSize = growBlockSize;
	data.growSize = growBlockSize*growBlocks;
	data.dataStart = 0;

	switch (dataType)
	{
	case FREC_DATA:
		err = NewDataFrag(data, clear);
		break;
	case BYTE_DATA:
		err = NewDataByte(data, clear);
		break;
	}

	if (err)
	{
		PurgePage(offset,data.GetSize());
		return(err);
	}

	err = data.Write(mFile,offset);
	if (err) 
	{
		switch(data.dataType)
		{
			case FREC_DATA:
				err = DeleteDataFrag(data);
				break;
			case BYTE_DATA:
				err = DeleteDataByte(data);
				break;
		}
		PurgePage(offset,data.GetSize());
		return(err);
	}

	itemPtr = offset;

	return(err);
}

GrapaError	GrapaBtree::DeleteData(u64 itemPtr)
{
	GrapaError err = 0;
	GrapaBlockDataHeader data;

	if (itemPtr==0) return(0);

	err = data.Read(mFile,itemPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	switch(data.dataType)
	{
		case FREC_DATA:
			err = DeleteDataFrag(data);
			break;
		case BYTE_DATA:
			err = DeleteDataByte(data);
			break;
	}

	err = PurgePage(itemPtr,data.GetSize());
	if (err) return(err);

	return(err);
}

GrapaError	GrapaBtree::SetDataSize(u64 itemPtr, u64 dataSize, u64 dataLength, u8 encodeType)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	if (itemPtr==0) return(-1);

	err = data.Read(mFile,itemPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	data.encodeType = encodeType;

	switch(data.dataType)
	{
	case FREC_DATA:
		err = SetDataSizeFrag(itemPtr,data,dataSize,dataLength);
		break;
	case BYTE_DATA:
		err = SetDataSizeByte(itemPtr, data, dataSize, dataLength);
		break;
	}

	err = data.Write(mFile,itemPtr);
	if (err) return(err);

	return(0);
}

GrapaError	GrapaBtree::GetDataSize(u64 itemPtr, u64& growBlockSize, u64& dataSize, u64& dataLength, u8& encodeType)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	growBlockSize = 0;
	dataSize = 0;
	dataLength = 0;
	encodeType = 0;

	if (itemPtr==0) return(-1);

	err = data.Read(mFile,itemPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	encodeType = data.encodeType;

	switch(data.dataType)
	{
	case FREC_DATA:
		err = GetDataSizeFrag(data,growBlockSize,dataSize,dataLength);
		break;
	case BYTE_DATA:
		err = GetDataSizeByte(data, growBlockSize, dataSize, dataLength);
		break;
	}

	return(err);
}

GrapaError GrapaBtree::SetFieldType(u64 itemPtr, u8 fieldType)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	if (itemPtr == 0) return(-1);

	err = data.Read(mFile, itemPtr);
	if (err) return(err);

	if (data.blockType != GrapaBlock::DATA_BLOCK) return(-1);

	data.fieldType = fieldType;

	err = data.Write(mFile, itemPtr);
	if (err) return(err);

	return(err);
}

GrapaError GrapaBtree::GetFieldType(u64 itemPtr, u8& fieldType)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	fieldType = 0;

	if (itemPtr == 0) return(-1);

	err = data.Read(mFile, itemPtr);
	if (err) return(err);

	if (data.blockType != GrapaBlock::DATA_BLOCK) return(-1);

	fieldType = data.fieldType;

	return(err);
}

GrapaError	GrapaBtree::ClearDataValue(u64 itemPtr, u64 offset, u64 bufferSize)
{
	GrapaError err;
	GrapaBlockDataHeader data;
	u64 returnSize;
	if (itemPtr == 0)
		return(0);

	err = data.Read(mFile, itemPtr);
	if (err) return(err);

	if (data.blockType != GrapaBlock::DATA_BLOCK) return(-1);

	void*buffer = malloc((size_t)bufferSize);
	if (buffer == 0L) return(-1);
	memset(buffer, 0, (size_t)bufferSize);

	switch (data.dataType)
	{
	case FREC_DATA:
		err = SetDataValueFrag(itemPtr, data, offset, bufferSize, buffer, &returnSize);
		break;
	case BYTE_DATA:
		err = SetDataValueByte(itemPtr, data, offset, bufferSize, buffer, &returnSize);
		break;
	}

	free(buffer);

	err = data.Write(mFile, itemPtr);
	if (err) return(err);

	return(err);
}

GrapaError	GrapaBtree::SetDataValue(u64 itemPtr, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	if (returnSize) *returnSize = 0;

	if (bufferSize && buffer == 0L) buffer = (void*)"";

	if ((itemPtr==0)||(buffer==0L))
		return(0);

	err = data.Read(mFile,itemPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	switch(data.dataType)
	{
	case FREC_DATA:
		err = SetDataValueFrag(itemPtr,data,offset,bufferSize,buffer,returnSize);
		break;
	case BYTE_DATA:
		err = SetDataValueByte(itemPtr, data, offset, bufferSize, buffer, returnSize);
		break;
	}

	err = data.Write(mFile,itemPtr);
	if (err) return(err);

	return(err);
}

GrapaError	GrapaBtree::GetDataValue(u64 itemPtr, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	if (returnSize) *returnSize = 0;

	if (itemPtr==0) return(0);

	err = data.Read(mFile,itemPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	switch(data.dataType)
	{
	case FREC_DATA:
		err = GetDataValueFrag(data,offset,bufferSize,buffer,returnSize);
		break;
	case BYTE_DATA:
		err = GetDataValueByte(data, offset, bufferSize, buffer, returnSize);
		break;
	}

	return(err);
}

GrapaError	GrapaBtree::InsertDataValue(u64 itemPtr, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	if (returnSize) *returnSize = 0;

	if (itemPtr==0) return(0);

	err = data.Read(mFile,itemPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	switch(data.dataType)
	{
	case FREC_DATA:
		err = InsertDataValueFrag(itemPtr,data,offset,bufferSize,buffer,returnSize);
		break;
	case BYTE_DATA:
		err = InsertDataValueByte(itemPtr, data, offset, bufferSize, buffer, returnSize);
		break;
	}

	err = data.Write(mFile,itemPtr);
	if (err) return(err);

	return(err);
}

GrapaError	GrapaBtree::DeleteDataValue(u64 itemPtr, u64 offset, u64 dataSize, u64 *returnSize)
{
	GrapaError err;
	GrapaBlockDataHeader data;

	if (returnSize) *returnSize = 0;

	if (itemPtr==0) return(0);

	err = data.Read(mFile,itemPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	switch(data.dataType)
	{
	case FREC_DATA:
		err = DeleteDataValueFrag(itemPtr,data,offset,dataSize,returnSize);
		break;
	case BYTE_DATA:
		err = DeleteDataValueByte(itemPtr, data, offset, dataSize, returnSize);
		break;
	}

	err = data.Write(mFile,itemPtr);
	if (err) return(err);

	return(err);
}

GrapaError GrapaBtree::CopyDataValue(u64 toPtr, u64 toOffset, u64 fromPtr, u64 fromOffset, u64 moveSize, u64 *returnSize)
{
	GrapaError err;
	u64 bytesRead,bytesWriten;

	if (returnSize) *returnSize = 0;

	if (toPtr==0 || fromPtr==0) return(0);

	if (toPtr==fromPtr && fromOffset<toOffset)
	{
		u64 pagePos = moveSize;
		while (pagePos>0)
		{
			u64 moveBytes = (pagePos >= GrapaBlock::COPYSIZE) ? GrapaBlock::COPYSIZE : pagePos;
			pagePos -= moveBytes;
			err = GetDataValue(fromPtr,fromOffset+pagePos,moveBytes,mBuffer,&bytesRead);
			err = SetDataValue(toPtr,toOffset+pagePos,moveBytes,mBuffer,&bytesWriten);
			if (returnSize) (*returnSize) += bytesWriten;
		}
	}
	else
	{
		for (u64 pagePos=0;pagePos<moveSize;pagePos+=GrapaBlock::COPYSIZE)
		{
			u64 moveBytes = (moveSize-pagePos)<GrapaBlock::COPYSIZE?(moveSize-pagePos):GrapaBlock::COPYSIZE;
			err = GetDataValue(fromPtr,fromOffset+pagePos,moveBytes,mBuffer,&bytesRead);
			err = SetDataValue(toPtr,toOffset+pagePos,moveBytes,mBuffer,&bytesWriten);
			if (returnSize) (*returnSize) += bytesWriten;
		}
	}
	return(0);
}

GrapaError GrapaBtree::MoveDataValue(u64 toPtr, u64 toOffset, u64 fromPtr, u64 fromOffset, u64 moveSize, u64 *returnSize)
{
	GrapaError err;
	GrapaBlockDataHeader toData,fromData;

	if (returnSize) *returnSize = 0;

	if (toPtr==0 || fromPtr==0) return(0);

	err = toData.Read(mFile,toPtr);
	if (err) return(err);

	if (toData.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	if (fromPtr!=toPtr)
	{
		err = fromData.Read(mFile,fromPtr);
		if (err) return(err);
		if (fromData.blockType!=GrapaBlock::DATA_BLOCK) return(-1);
		if (toData.dataType!=fromData.dataType) return(-1);
	}

	switch(toData.dataType)
	{
	case FREC_DATA:
		if (fromPtr!=toPtr)
			err = MoveDataValueFrag(toPtr,toData,toOffset,fromPtr,fromData,fromOffset,moveSize,returnSize);
		else
			err = MoveDataValueFrag(toPtr,toData,toOffset,fromPtr,toData,fromOffset,moveSize,returnSize);
		break;
	case BYTE_DATA:
		if (fromPtr != toPtr)
			err = MoveDataValueByte(toPtr,toData,toOffset,fromPtr,fromData,fromOffset,moveSize,returnSize);
		else
			err = MoveDataValueByte(toPtr,toData,toOffset,fromPtr,toData,fromOffset,moveSize,returnSize);
		break;
	}

	if (fromPtr!=toPtr)
	{
		err = toData.Write(mFile,toPtr);
		if (err) return(err);
		err = fromData.Write(mFile,fromPtr);
		if (err) return(err);
	}

	return(err);
}

GrapaError	GrapaBtree::GetDataParent(u64 dataPtr, u64& parentTree)
{
	GrapaError err = 0;
	GrapaBlockDataHeader data;

	parentTree = 0;

	if (dataPtr==0L)
		return(0);

	err = data.Read(mFile,dataPtr);
	if (err) return(err);

	if (data.blockType!=GrapaBlock::DATA_BLOCK) return(-1);

	parentTree = data.parentTree;

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError	GrapaBtree::Search(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockTree head;
	u8 found;
	GrapaBlockNodeLeaf result;

	cursor.mNodeRef = 0;
	cursor.mNodeIndex = 0;
	cursor.mLength = 0;

	if (cursor.mTreeRef==0L) return(-1);

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);
	cursor.mTreeType = head.treeType;

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	found = SearchRc(head.firstItem, cursor, result);
	cursor.mValueType = result.valueType;
	cursor.mKey = result.key;
	cursor.mValue = result.value;
	cursor.mFlags = result.flags;
	if(!found) return(-1);

	return(0);
}

GrapaError GrapaBtree::Insert(GrapaCursor& cursor)
{
	GrapaError err = 0;
	s8 result=1;
	GrapaBlockTree head;
	GrapaBlockNodeLeaf pKey,fKey;

	if (cursor.mTreeRef==0L) return(-1);

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);
	cursor.mTreeType = head.treeType;

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	if (head.firstItem)
	{
		err = InsertRc(cursor.mTreeRef, head, head.firstItem, cursor, pKey, fKey, result);
		if (err) return(err);

		if (result<0) return(-1);

		if (result==1)
		{
			err = AppendNode(cursor.mTreeRef,head,pKey);
			if (err) return(err);
		}
	}
	else
	{
		GrapaBlockNodeLeaf appendLeaf;
		SetKey(head,appendLeaf,cursor);
		err = AppendNode(cursor.mTreeRef,head,appendLeaf);
		if (err) return(err);
	}

	head.itemCount++;

	err = head.Write(mFile,cursor.mTreeRef);
	if (err) return(err);

	return(0);
}

GrapaError GrapaBtree::Update(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockNodeLeaf pKey;

	if (cursor.mTreeRef==0L) return(-1);

	err = pKey.Read(mFile,cursor.mNodeRef+1+cursor.mNodeIndex);
	if (err) return(err);

	pKey.value = cursor.mValue;

	err = pKey.Write(mFile,cursor.mNodeRef+1+cursor.mNodeIndex);
	if (err) return(err);

	return(0);
}

GrapaError	GrapaBtree::Delete(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockTree head;
	GrapaBlockNodeHeader rootTree;
	s8 result;
	u64 firstChild;
	
	if (cursor.mTreeRef==0) return(-1);

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);
	cursor.mTreeType = head.treeType;

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	if (head.firstItem == 0L) return(-1);
	
	err = PurgeRc(cursor.mTreeRef, head, head.firstItem, cursor, rootTree, result);
	if (err) return(err);

	if (rootTree.leafCount==0)
	{
		firstChild = rootTree.firstChild;
		err = EmptyItem(cursor.mTreeRef, head, head.firstItem);
		if (err) return(err);
		head.firstItem = firstChild;
		err = head.Write(mFile,cursor.mTreeRef);
		if (err) return(err);
		err = UpdateChildInfo(firstChild,cursor.mTreeRef,(s8)-1);
		if (err) return(err);
	}

	if (result<0)
		return(result);

	head.itemCount--;
	err = head.Write(mFile,cursor.mTreeRef);
	if (err) return(err);

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError	GrapaBtree::First(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockTree head;

	cursor.mNodeRef = 0;
	cursor.mNodeIndex = 0;
	cursor.mLength = 0;

	if (cursor.mTreeRef==0L) return(-1);

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);
	cursor.mTreeType = head.treeType;

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	err = FindFirstX(head.firstItem, cursor);
	if (err) return(-1);

	return(0);
}

GrapaError	GrapaBtree::Last(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockTree head;
	GrapaBlockNodeHeader node;

	cursor.mNodeRef = 0;
	cursor.mNodeIndex = 0;
	cursor.mLength = 0;

	if (cursor.mTreeRef==0L) return(-1);

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);
	cursor.mTreeType = head.treeType;

	if (head.blockType!=GrapaBlock::TREE_BLOCK) return(-1);

	err = FindLastX(head.firstItem, cursor);
	if (err) return(err);

	err = node.Read(mFile, head.firstItem);
	if (err) return(err);
	cursor.mLength = node.weight;

	return(0);
}

GrapaError	GrapaBtree::Next(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockTree head;
	GrapaBlockNodeHeader xNode;
	GrapaBlockNodeLeaf leaf;
	GrapaCursor leafCursor;
	u64 itemWeight;
	
	u64 sNode = cursor.mNodeRef;
	s8 sIndex = cursor.mNodeIndex; 

	cursor.mNodeRef = 0;
	cursor.mNodeIndex = 0;

	if (cursor.mTreeRef==0L) return(-1);

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);
	cursor.mTreeType = head.treeType;

	if (sIndex<0) return(-1);

	if (sIndex>=head.nodeCount) return(-1);

	if (sNode==0L) return(-1);
	
	err = leaf.Read(mFile,sNode+1+sIndex);
	if (err) return(err);

	leafCursor.Set(cursor.mTreeRef,leaf.valueType,leaf.key,leaf.value,leaf.flags);
	leafCursor.mTreeType = cursor.mTreeType;
	err = GetWeight(leafCursor,itemWeight);
	cursor.mLength += itemWeight;

	if (leaf.child)
	{
		err = FindFirstX(leaf.child,cursor);
		return(err);
	}
	
	err = xNode.Read(mFile,sNode);
	if (err) return(err);

	sIndex++;

	while (xNode.leafCount<=sIndex)
	{
		sNode = xNode.parent;
		sIndex = xNode.parentIndex;
		err = xNode.Read(mFile,sNode);
		if (err) return(err);
		if (xNode.blockType!=GrapaBlock::NODE_BLOCK)
			return(-1);
	}

	err = leaf.Read(mFile,sNode+1+sIndex);
	if (err) return(err);

	cursor.mValueType = leaf.valueType;
	cursor.mKey = leaf.key;
	cursor.mValue = leaf.value;
	cursor.mFlags = leaf.flags;

	cursor.mNodeRef = sNode;
	cursor.mNodeIndex = sIndex;

	return(0);
}

GrapaError	GrapaBtree::Prev(GrapaCursor& cursor)
{
	GrapaError err = 0;
	GrapaBlockTree head;
	GrapaBlockNodeHeader xNode;
	GrapaBlockNodeLeaf leaf;
	GrapaCursor leafCursor;
	u64 itemWeight;
	bool wentUpParrent=false;

	u64 sNode = cursor.mNodeRef;
	s8 sIndex = cursor.mNodeIndex; 

	cursor.mNodeRef = 0;
	cursor.mNodeIndex = 0;

	if (cursor.mTreeRef==0L) return(-1);

	err = head.Read(mFile,cursor.mTreeRef);
	if (err) return(err);
	cursor.mTreeType = head.treeType;

	if (sIndex<0) return(-1);

	if (sIndex>=head.nodeCount) return(-1);

	if (sNode==0L) return(-1);
	
	err = leaf.Read(mFile,sNode+1+sIndex);
	if (err) return(err);

	err = xNode.Read(mFile,sNode);
	if (err) return(err);

	if ((sIndex==0)&&(xNode.firstChild))
	{
		err = FindLastX(xNode.firstChild,cursor);
	}
	else
	{
		while(sIndex==0)
		{
			wentUpParrent = true;
			sNode = xNode.parent;
			sIndex = xNode.parentIndex;
			err = xNode.Read(mFile,sNode);
			if (err) return(err);
			if (xNode.blockType!=GrapaBlock::NODE_BLOCK)
				return(-1);
		}

		sIndex--;

		err = leaf.Read(mFile,sNode+1+sIndex);
		if (err) return(err);

		cursor.mValueType = leaf.valueType;
		cursor.mKey = leaf.key;
		cursor.mValue = leaf.value;
		cursor.mFlags = leaf.flags;

		if ((!wentUpParrent)&&(leaf.child))
		{
			err = FindLastX(leaf.child,cursor);
		}
		else
		{
			cursor.mNodeRef = sNode;
			cursor.mNodeIndex = sIndex;
		}
	}
	
	if (err) return(err);

	leafCursor.Set(cursor.mTreeRef,cursor.mValueType,cursor.mKey,cursor.mValue,cursor.mFlags);
	leafCursor.mTreeType = cursor.mTreeType;
	err = GetWeight(leafCursor,itemWeight);
	cursor.mLength -= itemWeight;

	return(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaBtree::CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
{
	result = GrapaBtree::CompareKey(dataCursor.mKey,treeCursor.mKey);
	return(0);
}

GrapaError GrapaBtree::DeleteKey(GrapaCursor& treeCursor)
{
	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GrapaBtree::SearchNode(u64 rootNode, GrapaCursor& key, s8& pos, u64& child, GrapaBlockNodeLeaf& foundKey, s16 compareType)
{
	GrapaBlockNodeHeader page;
	GrapaCursor leafCursor;
	GrapaBlockNodeLeaf priorKey;
	u64 foundOffset;
	s8 match;
	GrapaError err;
	s8 l,h,last;
	u64 itemWeight;

	if (key.mValueType==BDATA_ITEM)
		return SearchNodeFrag(rootNode,key,pos,child,foundKey,compareType);

	foundOffset = 0;
	child = 0;
	pos = 0;

	err = page.Read(mFile,rootNode);
	if (err) return(false);

	child = page.firstChild;

	l = 0;
	h = page.leafCount - 1;
	match = 1;

	while (h >= l)
	{
		pos = l + (h-l)/2;
		foundOffset = rootNode+1+pos;
		err = foundKey.Read(mFile,foundOffset);
		if (err) return(false);
		switch(key.mValueType)
		{
			case SU64_ITEM:
			case TREE_ITEM:
			case SDATA_ITEM:
				//match = foundKey.key - key.mKey;
				match = GrapaBtree::CompareKey(foundKey.key, key.mKey);
				break;
			default:
				match = -1;
				leafCursor.Set(key.mTreeRef,foundKey.valueType,foundKey.key,foundKey.value);
				leafCursor.mTreeType = key.mTreeType;
				err = CompareKey(compareType,key,leafCursor,match);
				if (err) return(false);
		}
		if (match == 0) break;
		if (match > 0) h = pos - 1; else l = pos + 1;
	}

	if (match < 0) pos++;

	if (pos)
	{
		if (foundOffset == (rootNode+pos))
			child = foundKey.child;
		else
		{
			err = priorKey.Read(mFile,rootNode+pos);
			if (err) return(false);
			child = priorKey.child;
		}
	}

	if (page.leafCount && foundOffset != (rootNode+1+pos))
	{
		foundOffset = rootNode+1+pos;
		err = foundKey.Read(mFile,foundOffset);
		if (err) return(false);
	}

	// Could conditionally do this based on whether the weight feature is enabled for this tree

	if ( (pos || match <= 0) && page.firstChild)
	{
		err = page.Read(mFile,page.firstChild);
		key.mLength += page.weight;
	}
	last = pos - 1;
	if (match==0) last++;
	for (h=0;h<last;h++)
	{
		err = priorKey.Read(mFile,rootNode+1+h);
		if (!err)
		{
			if (priorKey.child)
			{
				err = page.Read(mFile,priorKey.child);
				key.mLength += page.weight;
			}
			leafCursor.Set(key.mTreeRef,priorKey.valueType,priorKey.key,priorKey.value);
			leafCursor.mTreeType = key.mTreeType;
			err = GetWeight(leafCursor,itemWeight);
			key.mLength += itemWeight;
		}
	}
	if (pos && match)
	{
		err = priorKey.Read(mFile,rootNode+1+pos-1);
		leafCursor.Set(key.mTreeRef,priorKey.valueType,priorKey.key,priorKey.value);
		leafCursor.mTreeType = key.mTreeType;
		err = GetWeight(leafCursor,itemWeight);
		key.mLength += itemWeight;
	}

	return(match == 0);
}

bool GrapaBtree::SearchNodeFrag(u64 rootNode, GrapaCursor& key, s8& pos, u64& child, GrapaBlockNodeLeaf& foundKey, s16 compareType)
{
	GrapaBlockNodeHeader page,childPage;
	GrapaCursor leafCursor;
	s64 match;
	GrapaError err;
	s8 searchAdjust = (compareType==GrapaBtree::INSERT_MODE)?0:-1; // Need the -1 to line up the search/delete to the next node

	child = 0;
	pos = 0;

	err = page.Read(mFile,rootNode);
	if (err) return(false);

	if (page.leafCount==0) return(false);

	child = page.firstChild;
	if (child)
	{
		err = childPage.Read(mFile,child);
		if (!err)
		{
			match = key.mLength + childPage.weight - key.mKey + searchAdjust; 
			if (match>=0) return(false); // search child next
			key.mLength += childPage.weight;
		}
	}

	for (pos=0;pos<page.leafCount;pos++)
	{
		foundKey.Read(mFile,rootNode+1+pos);
		match = key.mLength + foundKey.key - key.mKey + searchAdjust;
		if (match>=0) // found
		{
			if (compareType==GrapaBtree::INSERT_MODE && match==0) // Insert after the found node
			{
				pos++;
				key.mLength += foundKey.key;
				child = foundKey.child;
			}
			return(true); 
		}
		key.mLength += foundKey.key;
		child = foundKey.child;
		if (child)
		{
			err = childPage.Read(mFile,child);
			if (!err)
			{
				match = key.mLength + childPage.weight - key.mKey + searchAdjust;
				if (match>=0) {pos++;return(false);} // search child next
				if (pos<(page.leafCount-1))  // Don't add weight for last child...this will happen when that child is searched
					key.mLength += childPage.weight; 
			}
		}
	}

	return(false); // Not found
}

GrapaError GrapaBtree::InsInPage(GrapaBlockTree& head, GrapaCursor& treekey, u64 rootNode, GrapaBlockNodeHeader& page, GrapaBlockNodeLeaf& key, s8 pos)
{
	GrapaError err;
	s8 i;
	GrapaCursor cursor;

	for (i=page.leafCount;i>pos;i--)
	{
		MoveLeaf(treekey.mTreeRef,head,page,rootNode,i-1,page,rootNode,i);
	}

	err = UpdateLeafInfo(&key,rootNode,pos);
	if (err) return(err);

	page.leafCount++;

	cursor.Set(treekey.mTreeRef,key.valueType,key.key,key.value,key.flags);
	cursor.mTreeType = head.treeType;
	err = GainWeightCascading(cursor,0,page,key.child,0);

	err = page.Write(mFile,rootNode);
	if (err) return(err);

	return(0);
}

bool GrapaBtree::SearchRc(u64 rootNode, GrapaCursor& key, GrapaBlockNodeLeaf& foundKey)
{
	GrapaError theErr;
	u64 child;
	GrapaBlockNodeHeader node;
	s8 nodeIndex=0;

	if (rootNode==0) return(false);

	theErr = node.Read(mFile,rootNode);

	if (node.blockType!=GrapaBlock::NODE_BLOCK)
		return(false);

	if (SearchNode(rootNode,key,nodeIndex,child,foundKey,SEARCH_MODE))
	{
		key.mNodeRef = rootNode;
		key.mNodeIndex = nodeIndex;
		return(true);
	}

	return(SearchRc(child,key,foundKey));
}

GrapaError GrapaBtree::SetKey(GrapaBlockTree& head, GrapaBlockNodeLeaf& leaf, GrapaCursor& cursor)
{
	GrapaError err;
	GrapaBlockDataHeader data;
	leaf.Init();
	leaf.valueType = cursor.mValueType;
	leaf.key = cursor.mKey;
	leaf.value = cursor.mValue;
	leaf.flags = cursor.mFlags;
	leaf.child = 0;

	switch(head.treeType)
	{
		case BDATA_TREE:
			err = data.Read(mFile,leaf.value);
			leaf.key = data.dataLength;
			break;
	}
	return(0);
}

GrapaError GrapaBtree::InsertRc(u64 headRef, GrapaBlockTree& head, u64 rootNode,
		GrapaCursor& key, GrapaBlockNodeLeaf& pKey, GrapaBlockNodeLeaf& fKey, s8& result)
{
	GrapaBlockNodeLeaf pbKey,tempLeaf;
	GrapaBlockNodeHeader page;
	GrapaError err;
	s8 pos;
	u64 child=0L;
	GrapaCursor cursor;

	if (rootNode==0L)
	{
		SetKey(head,pKey,key);
		result = 1;
		return(0);
	}

	if (SearchNode(rootNode,key,pos,child,fKey,INSERT_MODE))
	{
		// if exact matching, then return an error
		//return(-1);
		err = 0;
	}

	err = InsertRc(headRef,head,child,key,pbKey,fKey,result);
	if (err) return(err);

	if (result!=1) return(0);

	err = page.Read(mFile,rootNode);
	if (err) return(err);

	if (page.leafCount!=head.nodeCount)
	{
		InsInPage(head,key,rootNode,page,pbKey,pos);
		result = 0;
		return(0);
	}

	if (rootNode == head.firstItem)
	{
		Split(headRef,head,rootNode,page,pbKey,pos,pKey);
		result = 1;
		return(0);
	}

	if (pos==page.leafCount)
	{
		err = tempLeaf.Read(mFile,rootNode+page.leafCount);
		err = UpdateLeafInfo(&pbKey,rootNode,page.leafCount-1);

		cursor.Set(headRef,tempLeaf.valueType,tempLeaf.key,tempLeaf.value,tempLeaf.flags);
		cursor.mTreeType = key.mTreeType;
		err = LooseWeightCascading(cursor,0,page,tempLeaf.child);

		cursor.Set(headRef,pbKey.valueType,pbKey.key,pbKey.value,pbKey.flags);
		cursor.mTreeType = key.mTreeType;
		err = GainWeightCascading(cursor,0,page,pbKey.child,0);

		pbKey = tempLeaf;
		pos--;
	}

	RotateParrentRight(headRef,head,rootNode,page);

	if (page.leafCount!=head.nodeCount)
	{
		InsInPage(head,key,rootNode,page,pbKey,pos);
		result = 0;
		return(0);
	}

	if (pos==0)
	{
		err = tempLeaf.Read(mFile,rootNode+1);
		err = UpdateLeafInfo(&pbKey,rootNode,0);

		cursor.Set(headRef,tempLeaf.valueType,tempLeaf.key,tempLeaf.value,tempLeaf.flags);
		cursor.mTreeType = key.mTreeType;
		err = LooseWeightCascading(cursor,0,page,tempLeaf.child);

		cursor.Set(headRef,pbKey.valueType,pbKey.key,pbKey.value,pbKey.flags);
		cursor.mTreeType = key.mTreeType;
		err = GainWeightCascading(cursor,0,page,pbKey.child,0);

		pbKey = tempLeaf;
		pos++;
	}

	RotateParrentLeft(headRef,head,rootNode,page);

	if (page.leafCount!=head.nodeCount)
	{
		InsInPage(head,key,rootNode,page,pbKey,pos-1);
		result = 0;
		return(0);
	}

	Split(headRef,head,rootNode,page,pbKey,pos,pKey);
	result = 1;
	return(0);
}

void GrapaBtree::RotateParrentRight(u64 headRef, GrapaBlockTree& head, u64 middleOffset, GrapaBlockNodeHeader& middleTree)
{
	GrapaBlockNodeHeader page,rightTree;
	GrapaBlockNodeLeaf leaf;
	u64 rightOffset=0L;
	GrapaError theErr;

	if (middleTree.parentIndex==-1)
		return;

	theErr = page.Read(mFile,middleTree.parent);

	if (middleTree.parentIndex>=page.leafCount)
		return;

	theErr = leaf.Read(mFile,middleTree.parent+1+middleTree.parentIndex);

	rightOffset = leaf.child;

	theErr = rightTree.Read(mFile,rightOffset);

	RotateLeafRight(headRef,head,middleTree.parentIndex,middleTree.parent,page,middleOffset,middleTree,rightOffset,rightTree);
}

void GrapaBtree::RotateParrentLeft(u64 headRef, GrapaBlockTree& head, u64 middleOffset, GrapaBlockNodeHeader& middleTree)
{
	GrapaBlockNodeHeader page,leftTree;
	GrapaBlockNodeLeaf leaf;
	u64 leftOffset=0L;
	GrapaError theErr;

	if (middleTree.parentIndex==0)
		return;

	if (middleTree.parentIndex==-1)
		return;

	theErr = page.Read(mFile,middleTree.parent);

	if (middleTree.parentIndex==1)
	{
		leftOffset = page.firstChild;
	}
	else 
	{
		theErr = leaf.Read(mFile,middleTree.parent+middleTree.parentIndex-1);
		leftOffset = leaf.child;
	}

	theErr = leftTree.Read(mFile,leftOffset);

	RotateLeafLeft(headRef,head,middleTree.parentIndex-1,middleTree.parent,page,leftOffset,leftTree,middleOffset,middleTree);
}

GrapaError GrapaBtree::UpdateChildInfo(u64 childBlock, u64 newBlock, s8 newIndex)
{
	GrapaError err = 0;
	GrapaBlockNodeHeader child;

	if (childBlock)
	{
		err = child.Read(mFile,childBlock);
		if (err) return(err);

		if (child.blockType==GrapaBlock::NODE_BLOCK)
		{
			child.parent = newBlock;
			child.parentIndex = newIndex;
			err = child.Write(mFile,childBlock);
			if (err) return(err);
		}
	}

	return(0);
}

GrapaError GrapaBtree::UpdateLeafInfo(GrapaBlockNodeLeaf* key, u64 newBlock, s8 newIndex)
{
	GrapaError err = 0;
	GrapaBlockNodeLeaf tempKey;

	if (key==0L)
	{
		err = tempKey.Read(mFile,newBlock+1+newIndex);
		if (err) return(err);
		key = &tempKey;
	}

	err = UpdateChildInfo(key->child,newBlock,newIndex+1);
	if (err) return(err);

	err = key->Write(mFile,newBlock+1+newIndex);
	if (err) return(err);

	return(0);
}

GrapaError GrapaBtree::MoveLeaf(u64 headRef, GrapaBlockTree& head, GrapaBlockNodeHeader& oldPage, u64 rootNode, s8 rootIndex, GrapaBlockNodeHeader& newPage, u64 newBlock, s8 newIndex)
{
	GrapaError err;
	GrapaBlockNodeLeaf wrkkey;
	GrapaCursor cursor;

	err = wrkkey.Read(mFile,rootNode+1+rootIndex);
	if (err) return(err);

	err = UpdateLeafInfo(&wrkkey,newBlock,newIndex);
	if (err) return(err);

	if (rootNode!=newBlock)
	{
		cursor.Set(headRef,wrkkey.valueType,wrkkey.key,wrkkey.value,wrkkey.flags);
		cursor.mTreeType = head.treeType;
		err = LooseWeightCascading(cursor,0,oldPage,wrkkey.child);
		err = GainWeightCascading(cursor,0,newPage,wrkkey.child,0);
	}
	return(0);
}

void GrapaBtree::Split(u64 headRef, GrapaBlockTree& head, u64 rootNode, 
		GrapaBlockNodeHeader& oldPage, GrapaBlockNodeLeaf& key, s8 inPos, GrapaBlockNodeLeaf& pKey)
{
	s8 i;
	u64 newBlock;
	GrapaBlockNodeHeader newPage;
	u8 promIndex;
	GrapaError theErr;
	GrapaCursor cursor;

	newPage.Init();
	newPage.parentIndex = -1;
	newPage.parent = headRef;
	newBlock = NewPage((((u64)head.nodeCount)+1)*GrapaBlock::BLOCKSIZE,true);

	promIndex = oldPage.leafCount/2 + 1;

	if (inPos==promIndex)
	{
		pKey = key;
	}
	else
	{
		if (inPos<promIndex)
			theErr = pKey.Read(mFile,rootNode+1+promIndex-1);
		else
			theErr = pKey.Read(mFile,rootNode+1+promIndex);
	}

	newPage.firstChild = pKey.child;

	UpdateChildInfo(newPage.firstChild,newBlock,0);

	pKey.child = newBlock;

	if (inPos<=promIndex)
	{
		cursor.Set(headRef,pKey.valueType,pKey.key,pKey.value,pKey.flags);
		cursor.mTreeType = head.treeType;
		theErr = LooseWeightCascading(cursor,0,oldPage,newPage.firstChild);
		theErr = GainChildWeightCascading(cursor,0,newPage,newPage.firstChild,0);

		cursor.Set(headRef,key.valueType,key.key,key.value,key.flags);
		cursor.mTreeType = head.treeType;
		theErr = GainWeightCascading(cursor,0,oldPage,key.child,0);

		for (i=promIndex;i<oldPage.leafCount;i++)
		{
			MoveLeaf(headRef,head,oldPage,rootNode,i,newPage,newBlock,i-promIndex);
		}

		if (inPos<promIndex)
		{
			for (i=promIndex-1;i>inPos;i--)
			{
				MoveLeaf(headRef,head,oldPage,rootNode,i-1,oldPage,rootNode,i);
			}

			UpdateLeafInfo(&key,rootNode,inPos);
		}
	}
	else 
	{
		cursor.Set(headRef,pKey.valueType,pKey.key,pKey.value,pKey.flags);
		cursor.mTreeType = head.treeType;
		theErr = LooseWeightCascading(cursor,0,oldPage,newPage.firstChild);

		cursor.Set(headRef,key.valueType,key.key,key.value,key.flags);
		cursor.mTreeType = head.treeType;
		theErr = GainWeightCascading(cursor,0,newPage,key.child,newPage.firstChild);

		for (i=promIndex+1;i<inPos;i++)
		{
			MoveLeaf(headRef,head,oldPage,rootNode,i,newPage,newBlock,i-promIndex-1);
		}

		UpdateLeafInfo(&key,newBlock,inPos-promIndex-1);

		for(i=inPos;i<oldPage.leafCount;i++)
		{
			MoveLeaf(headRef,head,oldPage,rootNode,i,newPage,newBlock,i-promIndex);
		}
	}
	
	newPage.leafCount = promIndex - 1;
	oldPage.leafCount = promIndex;

	theErr = oldPage.Write(mFile,rootNode);
	theErr = newPage.Write(mFile,newBlock);
}

GrapaError GrapaBtree::PurgeRc(u64 headRef, GrapaBlockTree& head, u64 rootNode, GrapaCursor& key, GrapaBlockNodeHeader& rootTree, s8& result)
{
	GrapaBlockNodeHeader temp,leftTree,middleTree,rightTree;
	u64 leftOffset,middleOffset,rightOffset;
	GrapaBlockNodeLeaf tempKey,tempKey2,fKey;
	GrapaError err;
	s8 i;
	s8 pos=0;
	u64 child,tempChild;
	GrapaCursor cursor;
	
	if (rootNode==0) return(-1);

	err = rootTree.Read(mFile,rootNode);
	if (err) return(err);

	if (rootTree.blockType!=GrapaBlock::NODE_BLOCK) return(-1);

	if (SearchNode(rootNode,key,pos,child,fKey,DELETE_MODE))
	{
		err = tempKey.Read(mFile,rootNode+1+pos);
		if (err) return(err);

		if (tempKey.child==0)
		{
			cursor.Set(key.mTreeRef,tempKey.valueType,tempKey.key,tempKey.value,tempKey.flags);
			cursor.mTreeType = head.treeType;
			err = LooseWeightCascading(cursor,0,rootTree,0);

			err = EmptyItem(headRef,head,rootNode+1+pos);

			rootTree.leafCount--;

			for(i=pos;i<rootTree.leafCount;i++)
			{
				MoveLeaf(headRef,head,rootTree,rootNode,i+1,rootTree,rootNode,i);
			}

			err = rootTree.Write(mFile,rootNode);
			if (err) return(err);

			if (rootTree.leafCount<(head.nodeCount/2)) result = 1; else result = 0;
			return(0);
		}

		// The following will break any indexes against this key
		// as the indexes need to access the field values.
		// So ensure any indexes referencing this key are first deleted.
		// This was done by over riding Delete in GrapaDB.
		// But maybe there's a better way...

		child = tempKey.child;

		pos++;

		tempChild = child;

		err = temp.Read(mFile,tempChild);
		if (err) return(err);

		while(temp.firstChild)
		{
			tempChild = temp.firstChild;
			err = temp.Read(mFile,tempChild);
			if (err) return(err);
		}

		err = tempKey2.Read(mFile,tempChild+1);
		if (err) return(err);

		// Need to compensate for the key shifting
		// Subtract promoting key, Add deleting key, don't include child
		//   Cascading: start at tempChild, stop prior to rootNode (don't do on rootNode)
		err = rightTree.Read(mFile,tempChild);
		cursor.Set(key.mTreeRef,tempKey.valueType,tempKey.key,tempKey.value,tempKey.flags);
		cursor.mTreeType = head.treeType;
		err = GainWeightCascading(cursor,rootNode,rightTree,0,0); //Stop prior to rootNode
		cursor.Set(key.mTreeRef,tempKey2.valueType,tempKey2.key,tempKey2.value,tempKey2.flags);
		cursor.mTreeType = head.treeType;
		err = LooseWeightCascading(cursor,rootNode,rightTree,0); //Stop prior to rootNode
		err = rightTree.Write(mFile,tempChild);

		tempKey2.child = tempKey.child;

		tempKey.child = 0;  // tempKey2.child should have been 0

		UpdateLeafInfo(&tempKey,tempChild,0);
		//LooseWeight on righttree

		UpdateLeafInfo(&tempKey2,rootNode,pos-1);
		//GainWeight on righttree
	}
	
	err = PurgeRc(headRef,head,child,key,middleTree,result);
	if (err) return(err);

	err = rootTree.Read(mFile,rootNode);
	if (err) return(err);

	if (result==1)
	{
		//if (head.treeType==BDATA_TREE) pos++; // Shouldn't need to do this...what's up?

		middleOffset = child;

		if (pos==0)
		{
			leftOffset = 0;
		} 
		else if (pos==1)
		{
			leftOffset = rootTree.firstChild;
		}
		else
		{
			tempKey.Read(mFile,rootNode+1+pos-2);
			leftOffset = tempKey.child;
		}

		if (pos<rootTree.leafCount)
		{
			err = tempKey.Read(mFile,rootNode+1+pos);
			if (err) return(err);
			rightOffset = tempKey.child;
		}
		else
		{
			rightOffset = 0;
		}

		if (leftOffset)
		{
			err = leftTree.Read(mFile,leftOffset);
			if (err) return(err);
			if (leftTree.leafCount>(head.nodeCount/2))
			{
				RotateLeafRight(headRef,head,pos-1,rootNode,rootTree,leftOffset,leftTree,middleOffset,middleTree);
				return(0);
			}
		}
		if (rightOffset)
		{
			err = rightTree.Read(mFile,rightOffset);
			if (err) return(err);
			if (rightTree.leafCount>(head.nodeCount/2))
			{
				RotateLeafLeft(headRef,head,pos,rootNode,rootTree,middleOffset,middleTree,rightOffset,rightTree);
				return(0);
			}
		}
		if (rightOffset)
		{
			if ((middleTree.leafCount+rightTree.leafCount)<head.nodeCount)
			{
				MirgeLeft(headRef,head,rootNode,pos,rootTree,middleOffset,middleTree,rightOffset,rightTree);
				// rightOffset and rightTree are now invalid
			}
		}
		if (leftOffset)
		{
			if ((leftTree.leafCount+middleTree.leafCount)<head.nodeCount)
			{
				MirgeLeft(headRef,head,rootNode,pos-1,rootTree,leftOffset,leftTree,middleOffset,middleTree);
				// middleOffset and middleTree are now invalid
			}
		}

		// maybe write a MirgeRight routine
		if (rootTree.leafCount<(head.nodeCount/2)) result = 1; else result = 0;
		return(0);
	}
	
	if (child)
	{
		err = temp.Read(mFile,child);
		if (err) return(err);

		if (temp.leafCount==0)
		{
			tempChild = temp.firstChild;

			if (pos==0)
			{
				rootTree.firstChild = tempChild;
				UpdateChildInfo(rootTree.firstChild,rootNode,0);
				err = rootTree.Write(mFile,rootNode);
				if (err) return(err);
			}
			else
			{
				err = tempKey.Read(mFile,rootNode+1+pos);
				if (err) return(err);
				tempKey.child = tempChild;
				err = tempKey.Write(mFile,rootNode+1+pos);
				if (err) return(err);
				UpdateChildInfo(tempChild,rootNode,pos+1);
			}

			EmptyItem(headRef,head,child);
		}
	}
	
	result = 0;
	return(0);
}

GrapaError GrapaBtree::AppendNode(u64 headRef, GrapaBlockTree& head, GrapaBlockNodeLeaf& promKey)
{
	GrapaError err;
	GrapaBlockNodeHeader newNode;
	GrapaCursor cursor;

	newNode.Init();
	newNode.parentIndex = -1;
	newNode.leafCount = 1;
	newNode.parent = headRef;
	newNode.firstChild = head.firstItem;

	head.firstItem = NewPage((((u64)head.nodeCount) + 1)*GrapaBlock::BLOCKSIZE, true);

	err = UpdateLeafInfo(&promKey, head.firstItem, 0);
	if (err) return(err);

	err = UpdateChildInfo(newNode.firstChild, head.firstItem, 0);
	if (err) return(err);

	cursor.Set(headRef,promKey.valueType,promKey.key,promKey.value,promKey.flags);
	cursor.mTreeType = head.treeType;
	err = GainWeightCascading(cursor,0,newNode,promKey.child,newNode.firstChild);

	err = newNode.Write(mFile, head.firstItem);
	if (err) return(err);

	return(0);
}

void GrapaBtree::RotateLeafRight(u64 headRef, GrapaBlockTree& head, s8 pos, u64 rootOffset, GrapaBlockNodeHeader& rootTree, 
			u64 leftOffset, GrapaBlockNodeHeader& leftTree, u64 rightOffset, GrapaBlockNodeHeader& rightTree)
{
	s8 i;
	u64 tempChild;
	GrapaError theErr;
	GrapaBlockNodeLeaf tempKey,tempKey2;
	
	if (pos>=rootTree.leafCount) return;
	if (leftTree.leafCount==0L) return;
	if (rightTree.leafCount==head.nodeCount)
	{
		// The following code may add more overhead than its worth
		// It recursively looks for an empty slot to rotate into.
		//if (pos>=(rootTree.leafCount-1)) return;
		//theErr = tempKey.Read(mFile,rootOffset+1+pos+1);
		//newOffset = tempKey.child;
		//theErr = ReadBlockNode(newOffset,&newTree);
		//BtRotateLeafRight(pos+1,rootOffset,rootTree,rightOffset,rightTree,newOffset,&newTree);
	}

	if (rightTree.leafCount==head.nodeCount) // error condition
		return;
	
	theErr = tempKey.Read(mFile,rootOffset+1+pos);
	theErr = tempKey2.Read(mFile,leftOffset+1+leftTree.leafCount-1);

	theErr = RotateItemWeight(headRef,head,rootTree.parent,tempKey2,leftTree,tempKey,rightTree,tempKey2.child);

	tempChild = tempKey2.child;
	tempKey2.child = tempKey.child;
	UpdateLeafInfo(&tempKey2,rootOffset,pos);

	theErr = rootTree.Write(mFile,rootOffset);
	
	leftTree.leafCount--;

	theErr = leftTree.Write(mFile,leftOffset);
	
	for(i=rightTree.leafCount;i>0;i--)
		MoveLeaf(headRef,head,rightTree,rightOffset,i-1,rightTree,rightOffset,i);

	tempKey.child = rightTree.firstChild;
	UpdateLeafInfo(&tempKey,rightOffset,0);

	rightTree.firstChild = tempChild;
	UpdateChildInfo(rightTree.firstChild,rightOffset,0);

	rightTree.leafCount++;

	theErr = rightTree.Write(mFile,rightOffset);
}

void GrapaBtree::RotateLeafLeft(u64 headRef, GrapaBlockTree& head, s8 pos, u64 rootOffset, GrapaBlockNodeHeader& rootTree, 
			u64 leftOffset, GrapaBlockNodeHeader& leftTree, u64 rightOffset, GrapaBlockNodeHeader& rightTree)
{
	s8 i;
	u64 tempChild;
	GrapaError theErr;
	GrapaBlockNodeLeaf tempKey,tempKey2;

	if (pos<0)
		return;

	if (rightTree.leafCount==0L)
		return;

	if (leftTree.leafCount==head.nodeCount)
	{
		// The following code may add more overhead than its worth
		// It recursively looks for an empty slot to rotate into.
		//if (pos<=0) return;
		//if (pos==1) newOffset = rootTree.firstChild;
		//else {
		//	theErr = tempKey.Read(mFile,rootOffset+1+pos-1);
		//	newOffset = tempKey.child;
		//}
		//theErr = newTree.Read(mFile,newOffset);
		//BtRotateLeafLeft(pos-1,rootOffset,rootTree,newOffset,&newTree,leftOffset,leftTree);
	}

	if (leftTree.leafCount==head.nodeCount) // error condition
		return;
	
	theErr = tempKey.Read(mFile,rootOffset+1+pos);
	theErr = tempKey2.Read(mFile,rightOffset+1);

	theErr = RotateItemWeight(headRef,head,rootTree.parent,tempKey2,rightTree,tempKey,leftTree,rightTree.firstChild);

	tempChild = tempKey2.child;
	tempKey2.child = tempKey.child;
	UpdateLeafInfo(&tempKey2,rootOffset,pos);

	theErr = rootTree.Write(mFile,rootOffset);

	tempKey.child = rightTree.firstChild;

	rightTree.firstChild = tempChild;
	UpdateChildInfo(rightTree.firstChild,rightOffset,0);
	
	rightTree.leafCount--;

	for(i=0;i<rightTree.leafCount;i++)
		MoveLeaf(headRef,head,rightTree,rightOffset,i+1,rightTree,rightOffset,i);
	theErr = rightTree.Write(mFile,rightOffset);
	
	UpdateLeafInfo(&tempKey,leftOffset,leftTree.leafCount);

	leftTree.leafCount++;

	theErr = leftTree.Write(mFile,leftOffset);
}

void GrapaBtree::MirgeLeft(u64 headRef, GrapaBlockTree& head, u64 rootOffset, s8 pos, GrapaBlockNodeHeader& rootTree, 
			u64 leftOffset, GrapaBlockNodeHeader& leftTree, u64 rightOffset, GrapaBlockNodeHeader& rightTree)
{
	GrapaBlockNodeLeaf tempKey;
	s8 i;
	GrapaError theErr;
	GrapaCursor cursor;

	if ((leftTree.leafCount+rightTree.leafCount)>=head.nodeCount)
		return;

	theErr = tempKey.Read(mFile,rootOffset+1+pos);

	tempKey.child = rightTree.firstChild;

	UpdateLeafInfo(&tempKey,leftOffset,leftTree.leafCount);

	cursor.Set(headRef,tempKey.valueType,tempKey.key,tempKey.value,tempKey.flags);
	cursor.mTreeType = head.treeType;

	theErr = GainWeightCascading(cursor,leftTree.parent,leftTree,tempKey.child,0); // stop at head.parentTree (not cascading)

	leftTree.leafCount++;

	for(i=0;i<rightTree.leafCount;i++)
	{
		MoveLeaf(headRef,head,rightTree,rightOffset,i,leftTree,leftOffset,leftTree.leafCount);
		leftTree.leafCount++;
	}

	theErr = leftTree.Write(mFile,leftOffset);
	
	rightTree.leafCount = 0;
	rightTree.firstChild = 0;

	theErr = rightTree.Write(mFile,rightOffset);

	EmptyItem(headRef,head,rightOffset);
	
	rootTree.leafCount--;

	for(i=pos;i<rootTree.leafCount;i++)
	{
		MoveLeaf(headRef,head,rootTree,rootOffset,i+1,rootTree,rootOffset,i);
	}

	theErr = rootTree.Write(mFile,rootOffset);
}

GrapaError GrapaBtree::EmptyItem(u64 headRef, GrapaBlockTree& head, u64 pagePos)
{
	GrapaBlockNodeHeader node;
	GrapaBlockTree tempHead;
	GrapaBlockNodeLeaf leaf;
	GrapaError err;
	s8 i;
	GrapaCursor delCursor;
	GrapaBlockFileHeader hdr;

	if (pagePos==0L)
		return(0);

	err = node.Read(mFile,pagePos); // this is just to get the blockType
	if (err) return(err);

	switch(node.blockType)
	{
		case GrapaBlock::NODE_BLOCK:
			err = node.Read(mFile,pagePos);
			if (err) return(err);

			if (node.leafCount) {
				for (i=node.leafCount;i>0;i--) 
				{
					err = EmptyItem(headRef,head,pagePos+i);
					if (err) return(err);
				}
				err = EmptyItem(headRef,head,node.firstChild);
				if (err) return(err);
			}
			node.Init();
			node.blockType = 0;
			err = PurgePage(pagePos,(((u64)head.nodeCount)+1)*GrapaBlock::BLOCKSIZE);
			if (err) return(err);
			return(0);

		case GrapaBlock::TREE_BLOCK:
			err = tempHead.Read(mFile,pagePos);
			if (err) return(err);
			err = EmptyItem(pagePos, tempHead, tempHead.firstItem);
			if (err) return(err);
			err = EmptyItem(pagePos,tempHead,tempHead.indexTree);
			if (err) return(err);
			if (tempHead.storeType==DATA_STORE)
				err = EmptyItem(pagePos,tempHead,tempHead.storeTree);
			if (err) return(err);
			err = hdr.Read(mFile,0);
			if (err) return(err);
			if (pagePos==hdr.firstTree)
			{
				tempHead.Init();
				tempHead.blockType = 0;
				err = tempHead.Write(mFile,pagePos);
				err = PurgePage(pagePos,tempHead.GetSize());
				if (err) return(err);
			}
			else
			{
				err = PurgePage(pagePos,tempHead.GetSize());
				if (err) return(err);
			}
			return(0);

		case GrapaBlock::LEAF_BLOCK:
			err = leaf.Read(mFile,pagePos);
			if (err) return(err);
			err = EmptyItem(headRef,head,leaf.child);
			if (err) return(err);
			switch(leaf.valueType) {
				case SU64_ITEM:
					break;
				case TREE_ITEM:
				case SDATA_ITEM:
				case BDATA_ITEM:
					err = EmptyItem(headRef, head, leaf.value);
					if (err) return(err);
	 				break;
				default:
					delCursor.Set(headRef,leaf.valueType,leaf.key,leaf.value);
					delCursor.mTreeType = head.treeType;
					err = DeleteKey(delCursor);
					if (err) return(err);
					break;
			}
			break;

		case GrapaBlock::DATA_BLOCK:
			err = DeleteData(pagePos);
			if (err) return(err);
			return(0);

	}

	return(0);
}

GrapaError GrapaBtree::FindFirstX(u64 offset, GrapaCursor& cursor)
{
	GrapaError theErr;
	GrapaBlockNodeHeader node;
	GrapaBlockNodeLeaf result;

	if (offset==0)
		return(-1);

	while(offset)
	{
		theErr = node.Read(mFile,offset);

		if (node.firstChild==0L)
			break;

		offset = node.firstChild;
	}

	theErr = result.Read(mFile,offset+1);

	cursor.mValueType = result.valueType;
	cursor.mKey = result.key;
	cursor.mValue = result.value;
	cursor.mFlags = result.flags;
	cursor.mNodeRef = offset;
	cursor.mNodeIndex = 0;

	return(0);
}

GrapaError GrapaBtree::FindLastX(u64 offset, GrapaCursor& cursor)
{
	GrapaError err;
	GrapaBlockNodeHeader node;
	GrapaBlockNodeLeaf result;
	u64 oldOffset=0;

	if (!offset) return(-1);

	while(offset)
	{
		oldOffset = offset;
		err = node.Read(mFile,offset);
		if (err) return(err);
		err = result.Read(mFile,offset+node.leafCount);
		if (err) return(err);
		offset = result.child;
	}

	cursor.mValueType = result.valueType;
	cursor.mKey = result.key;
	cursor.mValue = result.value;
	cursor.mFlags = result.flags;
	cursor.mNodeRef = oldOffset;
	cursor.mNodeIndex = node.leafCount-1;

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

u64 GrapaBtree::FindFreePage(GrapaBlockPage& newPage, u64 maxBlockCount, u64 minBlockCount, u64* blocksReturned)
{
	GrapaBlockPage lastPage,temp;
	GrapaError err;
	u64 offset,lastOffset=0L;
	GrapaBlockFileHeader hdr;
	GrapaBlockFirst fst;

	if (minBlockCount==0) minBlockCount = maxBlockCount;
	if (blocksReturned) *blocksReturned = 0;

	newPage.blockCount = 0;

	err = hdr.Read(mFile,0);
	if (err) return(0);

	err = fst.Read(mFile,hdr.firstBlock);
	if (err) return(err);

	offset = fst.firstPage;

	while(offset)
	{
		err = newPage.Read(mFile,offset);
		if (err) return(0);

		while (newPage.nextPage==(offset+newPage.blockCount))
		{
			err = temp.Read(mFile,offset+newPage.blockCount);
			if (err) return(0);
			newPage.blockCount += temp.blockCount;
			newPage.nextPage = temp.nextPage;
			err = newPage.Write(mFile,offset);
			if (err) return(0); //could leave DB in a bad state
		}

		if (newPage.blockCount==maxBlockCount)
		{
			if (lastOffset)
			{
				lastPage.nextPage = newPage.nextPage;
				err = lastPage.Write(mFile,lastOffset);
				if (err) return(0); //could leave DB in a bad state
			}
			else
			{
				err = hdr.Read(mFile,0);
				if (err) return(0);
				err = fst.Read(mFile,hdr.firstBlock);
				if (err) return(0);
				fst.firstPage = newPage.nextPage;
				err = fst.Write(mFile,hdr.firstBlock);
				if (err) return(0); //could leave DB in a bad state
			}
			if (blocksReturned) *blocksReturned = maxBlockCount;
			return(offset);
		}
		else if (newPage.blockCount>maxBlockCount)
		{
			temp.Init();
			temp.blockCount = newPage.blockCount-maxBlockCount;
			temp.nextPage = newPage.nextPage;
			err = temp.Write(mFile,offset+maxBlockCount);
			if (err) return(0); //could leave DB in a bad state
			if (lastOffset)
			{
				lastPage.nextPage = offset+maxBlockCount;
				err = lastPage.Write(mFile,lastOffset);
				if (err) return(0); //could leave DB in a bad state
			}
			else
			{
				err = hdr.Read(mFile,0);
				if (err) return(0);
				err = fst.Read(mFile,hdr.firstBlock);
				if (err) return(0);
				fst.firstPage = offset+maxBlockCount;
				err = fst.Write(mFile,hdr.firstBlock);
				if (err) return(0); //could leave DB in a bad state
			}
			if (blocksReturned) *blocksReturned = maxBlockCount;
			return(offset);
		}
		else if ((newPage.blockCount<maxBlockCount) && (newPage.blockCount>=minBlockCount))
		{
			// less than what was requested, but allocate anyway
			// *** verify adding/removing pages does not result in unused/lost space
		}
		lastOffset = offset;
		lastPage = newPage;
		offset = newPage.nextPage;
	}
	return(0);
}

u64 GrapaBtree::NewPage(u64 byteCount, bool clear)
{
	GrapaError err = 0;
	GrapaBlockPage hr;
	u64 offset;
	u8 buffer[GrapaBlock::BLOCKSIZE];
	u64 i;
	GrapaBlockFileHeader hdr;
	GrapaBlockFirst fst;
	u64 blockCount;
	
	if (byteCount==0) return(0);

	blockCount = GrapaMem::Blocks(byteCount,GrapaBlock::BLOCKSIZE);
	offset = FindFreePage(hr,blockCount);

	err = hdr.Read(mFile,0);
	if (err) return(0);

	err = fst.Read(mFile,hdr.firstBlock);
	if (err) return(0);

	if (offset==0)
	{
		offset = fst.blockCount;
		clear = true;
		fst.blockCount += blockCount;
		err = fst.Write(mFile,hdr.firstBlock);
		if (err) return(0);
	}

	if ((offset)&&(clear))
	{
		memset(buffer,0,GrapaBlock::BLOCKSIZE);
		for(i=0;i<blockCount;i++)
		{
			err = mFile->Write( offset, GrapaBlock::BLOCKSIZE, i*GrapaBlock::BLOCKSIZE, GrapaBlock::BLOCKSIZE, buffer);
			if (err)
			{
				fst.blockCount -= blockCount;
				err = fst.Write(mFile,hdr.firstBlock);
				// need to backout any changes made above...
				PurgePage(offset,blockCount*GrapaBlock::BLOCKSIZE);
				return(0);
			}
		}
	}

	return(offset);
}

GrapaError GrapaBtree::PurgePage(u64 pagePos, u64 byteCount)
{
	GrapaError err;
	GrapaBlockPage hr,tempHr,lastHr;
	u64 offset,lastOffset=0L;
	GrapaBlockPage freePage;
	GrapaBlockFileHeader hdr;
	GrapaBlockFirst fst;
	u64 blockCount;

	blockCount = GrapaMem::Blocks(byteCount,GrapaBlock::BLOCKSIZE);

	if ((pagePos==0)||(blockCount<=0))
		return(0);

	freePage.Init();
	freePage.blockCount = blockCount;
	freePage.nextPage = 0;

	err = hdr.Read(mFile,0);
	if (err) return(err);

	err = fst.Read(mFile,hdr.firstBlock);
	if (err) return(err);

	offset = fst.firstPage;

	while (offset)
	{
		err = hr.Read(mFile,offset);
		if (err) return(err);

		if (pagePos==(offset+hr.blockCount))
		{
			if (fst.blockCount==(offset+hr.blockCount+freePage.blockCount))
			{
				fst.blockCount = offset;
				err = fst.Write(mFile,hdr.firstBlock);
				if (err) return(err);

				if (lastOffset)
				{
					lastHr.nextPage = 0L;
					err = lastHr.Write(mFile,lastOffset);
					if(err) return(err);
				}
				else
				{
					fst.firstPage = 0L;
					err = fst.Write(mFile,hdr.firstBlock);
					if (err) return(err);
				}
			}
			else if (pagePos==(offset+hr.blockCount))
			{
				hr.blockCount += freePage.blockCount;
				err = hr.Write(mFile,offset);
				if (err) return(err);
			}

			if (hr.nextPage==(offset+hr.blockCount))
			{
				err = tempHr.Read(mFile,hr.nextPage);
				if (err) return(err);
				hr.nextPage = tempHr.nextPage;
				hr.blockCount += tempHr.blockCount;
				err = hr.Write(mFile,offset);
				if (err) return(err);
			}

			return(0);

		} 
		else if (offset==(pagePos+freePage.blockCount))
		{
			freePage.blockCount += hr.blockCount;
			freePage.nextPage = hr.nextPage;

			if (lastOffset)
			{
				lastHr.nextPage = pagePos;
				err = lastHr.Write(mFile,lastOffset);
				if (err) return(err);
			}
			else 
			{
				fst.firstPage = pagePos;
				err = fst.Write(mFile,hdr.firstBlock);
				if (err) return(err);
			}

			err = freePage.Write(mFile,pagePos);
			if (err) return(err);

			return(0);

		}
		else if (offset>pagePos)
		{
			if (lastOffset)
			{
				freePage.nextPage = lastHr.nextPage;
				lastHr.nextPage = pagePos;
				err = lastHr.Write(mFile,lastOffset);
				if (err) return(err);
			}
			else
			{
				freePage.nextPage = fst.firstPage;
				fst.firstPage = pagePos;
				err = fst.Write(mFile,hdr.firstBlock);
				if (err) return(err);
			}

			err = freePage.Write(mFile,pagePos);
			if (err) return(err);

			return(0);

		}

		lastOffset = offset;
		lastHr = hr;
		offset = hr.nextPage;
	}

	if (fst.blockCount==(pagePos+blockCount))
	{
		err = hdr.Read(mFile,0);
		if (err) return(err);
		fst.blockCount = pagePos;
		err = fst.Write(mFile,hdr.firstBlock);
		if (err) return(err);
		return(0);
	}

	if (fst.firstPage==0L)
	{
		err = fst.Read(mFile,hdr.firstBlock);
		if (err) return(err);
		fst.firstPage = pagePos;
		err = fst.Write(mFile,hdr.firstBlock);
		if (err) return(err);
	}
	else
	{
		hr.nextPage = pagePos;
		err = hr.Write(mFile,lastOffset);
		if (err) return(err);
	}

	err = freePage.Write(mFile,pagePos);
	if (err) return(err);

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaBtree::GetWeight(GrapaCursor& treeCursor, u64& itemWeight)
{
	/*
	GrapaError err;
	GrapaBlockTree tree;
	if (treeCursor.mTreeType==0)   // Need to eliminate this as it's expensive.
	{
		err = tree.Read(mFile,treeCursor.mTreeRef);
		treeCursor.mTreeType = tree.treeType;
	}
	*/
	switch(treeCursor.mTreeType)
	{
		case BDATA_TREE:
			itemWeight = treeCursor.mKey;
			break;
		default:
			itemWeight = 1;
			break;
	}
	return(0);
}

GrapaError GrapaBtree::GainWeight(GrapaCursor& treeCursor, u64& treeWeight, u64 itemWeight, u64 childWeight)
{
	treeWeight += (itemWeight+childWeight);
	return(0);
}

GrapaError GrapaBtree::LooseWeight(GrapaCursor& treeCursor, u64& treeWeight, u64 itemWeight, u64 childWeight)
{
	treeWeight -= (itemWeight+childWeight);
	return(0);
}

GrapaError GrapaBtree::GetChildWeight(u64 childBlock, u64& childWeight)
{
	childWeight = 0;
	if (childBlock)
	{
		GrapaError err;
		GrapaBlockNodeHeader node;
		err = node.Read(mFile,childBlock);
		if(err) return(err);
		childWeight = node.weight;
	}
	return(0);
}

GrapaError GrapaBtree::RotateItemWeight(u64 headRef, GrapaBlockTree& head, u64 rootTree, GrapaBlockNodeLeaf& fromKey, GrapaBlockNodeHeader& fromTree, GrapaBlockNodeLeaf& toKey, GrapaBlockNodeHeader& toTree, u64 child)
{
	GrapaError err;
	GrapaCursor cursor;

	if (fromTree.parent==toTree.parent)
	{
		u64 itemWeight,childWeight;

		err = GetChildWeight(child,childWeight);

		cursor.Set(headRef,fromKey.valueType,fromKey.key,fromKey.value,fromKey.flags);
		cursor.mTreeType = head.treeType;
		err = GetWeight(cursor,itemWeight);
		err = LooseWeight(cursor,fromTree.weight,itemWeight,childWeight);

		cursor.Set(headRef,toKey.valueType,toKey.key,toKey.value,toKey.flags);
		cursor.mTreeType = head.treeType;
		err = GetWeight(cursor,itemWeight);
		err = GainWeight(cursor,toTree.weight,itemWeight,childWeight);
	}
	else
	{
		cursor.Set(headRef,fromKey.valueType,fromKey.key,fromKey.value,fromKey.flags);
		cursor.mTreeType = head.treeType;
		err = LooseWeightCascading(cursor,0,fromTree,child);

		cursor.Set(headRef,toKey.valueType,toKey.key,toKey.value,toKey.flags);
		cursor.mTreeType = head.treeType;
		err = GainWeightCascading(cursor,0,toTree,child,0);
	}
	return(0);
}

GrapaError GrapaBtree::LooseWeightCascading(GrapaCursor& cursor, u64 rootTree, GrapaBlockNodeHeader& oldPage, u64 child)
{
	GrapaError err;
	u64 itemWeight,childWeight;
	s8 parentIndex; 
	u64 parent;
	GrapaBlockNodeHeader pageH;

	err = GetWeight(cursor,itemWeight);
	err = GetChildWeight(child,childWeight);
	err = LooseWeight(cursor,oldPage.weight,itemWeight,childWeight);

	parentIndex = oldPage.parentIndex; 
	parent = oldPage.parent;
	while (parent && parentIndex != -1 && parent != rootTree)
	{
		err = pageH.Read(mFile,parent);
		err = LooseWeight(cursor,pageH.weight,itemWeight,childWeight);
		err = pageH.Write(mFile,parent);
		parentIndex = pageH.parentIndex;
		parent = pageH.parent;
	}

	return(0);
}

GrapaError GrapaBtree::GainWeightCascading(GrapaCursor& cursor, u64 rootTree, GrapaBlockNodeHeader& newPage, u64 child, u64 fistChild)
{
	GrapaError err;
	u64 itemWeight,childWeight,firstChildWeight=0;
	s8 parentIndex; 
	u64 parent;
	GrapaBlockNodeHeader pageH;

	err = GetWeight(cursor,itemWeight);
	err = GetChildWeight(child,childWeight);
	err = GainWeight(cursor,newPage.weight,itemWeight,childWeight);

	if (fistChild) 
	{
		err = GetChildWeight(fistChild,firstChildWeight);
		err = GainWeight(cursor,newPage.weight,0,firstChildWeight);
	}

	parentIndex = newPage.parentIndex; 
	parent = newPage.parent;
	while (parent && parentIndex != -1 && parent != rootTree)
	{
		err = pageH.Read(mFile,parent);
		err = GainWeight(cursor,pageH.weight,itemWeight,childWeight);
		if (fistChild) err = GainWeight(cursor,pageH.weight,0,firstChildWeight);
		err = pageH.Write(mFile,parent);
		parentIndex = pageH.parentIndex;
		parent = pageH.parent;
	}

	return(0);
}

GrapaError GrapaBtree::GainChildWeightCascading(GrapaCursor& cursor, u64 rootTree, GrapaBlockNodeHeader& newPage, u64 child, u64 fistChild)
{
	GrapaError err;
	u64 childWeight,firstChildWeight=0;
	s8 parentIndex; 
	u64 parent;
	GrapaBlockNodeHeader pageH;

	err = GetChildWeight(child,childWeight);
	err = GainWeight(cursor,newPage.weight,0,childWeight);

	if (fistChild)
	{
		err = GetChildWeight(fistChild,firstChildWeight);
		err = GainWeight(cursor,newPage.weight,0,firstChildWeight);
	}

	parentIndex = newPage.parentIndex; 
	parent = newPage.parent;
	while (parent && parentIndex != -1 && parent != rootTree)
	{
		err = pageH.Read(mFile,parent);
		err = GainWeight(cursor,pageH.weight,0,childWeight);
		if (fistChild) err = GainWeight(cursor,pageH.weight,0,firstChildWeight);
		err = pageH.Write(mFile,parent);
		parentIndex = pageH.parentIndex;
		parent = pageH.parent;
	}

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError	GrapaBtree::NewDataByte(GrapaBlockDataHeader& data, bool clear)
{
	data.dataStart = 0;
	if (data.dataSize==0) return(0);
	data.dataStart = NewPage(data.dataSize,clear);
	if (data.dataStart==0) return(-1);
	return(0);
}

GrapaError	GrapaBtree::DeleteDataByte(GrapaBlockDataHeader& data)
{
	return PurgePage(data.dataStart,data.dataSize);
}

GrapaError	GrapaBtree::SetDataSizeByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 dataSize, u64 dataLength)
{
	GrapaError err;
	u64 blockSizeA,blockSizeB;
	u64 i;
	u8 buf[GrapaBlock::BLOCKSIZE];
	u64 offset;

	if (dataLength>dataSize) 
		dataSize = (dataLength>data.dataSize)?dataLength:data.dataSize;
	dataSize = data.growSize?(GrapaMem::Blocks(dataSize,data.growSize)*data.growSize):dataSize;

	// First, null out the space that will be truncated

	blockSizeA = GrapaMem::Blocks(dataLength,GrapaBlock::BLOCKSIZE);
	blockSizeB = GrapaMem::Blocks(data.dataLength,GrapaBlock::BLOCKSIZE);
	
	if (blockSizeA < blockSizeB)
	{
		if (data.dataStart)
		{
			memset(buf,0,GrapaBlock::BLOCKSIZE);
			for (i=blockSizeA;i<blockSizeB;i++)
			{
				err = mFile->Write(data.dataStart, GrapaBlock::BLOCKSIZE, i*GrapaBlock::BLOCKSIZE, GrapaBlock::BLOCKSIZE, buf);
				if (err) return(err);
			}
		}
	}

	// Now resize, either smaller, or bigger

	blockSizeA = GrapaMem::Blocks(dataSize,GrapaBlock::BLOCKSIZE);
	blockSizeB = GrapaMem::Blocks(data.dataSize,GrapaBlock::BLOCKSIZE);

	// no change will happen if the resulting number of blocks is the same as what exists

	if (blockSizeA < blockSizeB)
	{
		if (data.dataStart)
		{
			err = PurgePage(data.dataStart+blockSizeA,(blockSizeB-blockSizeA)*GrapaBlock::BLOCKSIZE);
			if (err) return(err);
			if (blockSizeA==0L)
				data.dataStart = 0;
		}
	}
	else if (blockSizeA > blockSizeB)
	{
		offset = NewPage(dataSize,false);

		memset(buf,0,GrapaBlock::BLOCKSIZE);

		for (i=0;i<blockSizeB;i++)
		{
			if (data.dataStart) err = mFile->Read( data.dataStart, GrapaBlock::BLOCKSIZE, i*GrapaBlock::BLOCKSIZE, GrapaBlock::BLOCKSIZE, buf);
			err = mFile->Write( offset, GrapaBlock::BLOCKSIZE, i*GrapaBlock::BLOCKSIZE, GrapaBlock::BLOCKSIZE, buf);
			if (err) return(err);
		}

		memset(buf,0,GrapaBlock::BLOCKSIZE);
		for (i=blockSizeB;i<blockSizeA;i++)
		{
			err = mFile->Write( offset, GrapaBlock::BLOCKSIZE, i*GrapaBlock::BLOCKSIZE, GrapaBlock::BLOCKSIZE, buf);
			if (err) return(err);
		}

		if (data.dataStart)
		{
			err = PurgePage(data.dataStart,data.dataSize);
			if (err) return(err);
		}

		data.dataStart = offset;
	}

	data.dataSize = dataSize;
	data.dataLength = dataLength;

	return(0);
}

GrapaError	GrapaBtree::GetDataSizeByte(GrapaBlockDataHeader& data, u64& growBlockSize, u64& dataSize, u64& dataLength)
{
	growBlockSize = data.growBlockSize;
	dataSize = data.dataSize;
	dataLength = data.dataLength;
	return(0);
}

GrapaError	GrapaBtree::SetDataValueByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;

	if (returnSize) *returnSize = 0;

	if (offset==-1) offset = data.dataLength;

	if (data.dataSize<(offset+bufferSize))
	{
		err = SetDataSizeByte(itemPtr,data,offset+bufferSize,offset+bufferSize);
		//err = data.Write(mFile,itemPtr);
		if (err) return(err);
	}

	if (data.dataLength<(offset+bufferSize))
	{
		err = SetDataSizeByte(itemPtr,data,data.dataSize,offset+bufferSize);
		//err = data.Write(mFile,itemPtr);
		if (err) return(err);
	}

	if (data.dataStart)
	{
		if (buffer)
		{
			err = mFile->Write( data.dataStart, GrapaBlock::BLOCKSIZE, offset, bufferSize, buffer);
			if (err) return(err);
		}
		else
		{
			// Need to null out
		}
	}

	if (returnSize) *returnSize = bufferSize;

	return(0);
}

GrapaError	GrapaBtree::GetDataValueByte(GrapaBlockDataHeader& data, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;

	if (returnSize) *returnSize = 0;

	if (data.dataSize<(offset+bufferSize))
	{
		if (data.dataSize < offset)
			bufferSize = 0;
		else
			bufferSize = data.dataSize-offset;
	}

	if ((buffer==0L)||(bufferSize<=0)) return(0);

	err = mFile->Read( data.dataStart, GrapaBlock::BLOCKSIZE, offset, bufferSize, buffer);
	if (err) return(err);

	if (returnSize) *returnSize = bufferSize;

	return(0);
}

GrapaError	GrapaBtree::InsertDataValueByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;

	if (!bufferSize) return(0);

	u64 fromLoc = offset;
	u64 toLoc = fromLoc + bufferSize;
	u64 shiftSize = (data.dataLength>fromLoc)?(data.dataLength-fromLoc):0;

	err = SetDataSizeByte(itemPtr,data,data.dataSize,data.dataLength+bufferSize);
	data.Write(mFile,itemPtr);

	if (shiftSize)
	{
		u64 bytesWriten=0;
		err = CopyDataValue(itemPtr,toLoc,itemPtr,fromLoc,shiftSize,&bytesWriten);
	}

	err = SetDataValueByte(itemPtr,data,offset,bufferSize,buffer,returnSize);

	return(0);
}

GrapaError	GrapaBtree::DeleteDataValueByte(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, u64 *returnSize)
{
	GrapaError err;
	u64 bytesWriten;

	u64 toLoc = offset;
	u64 fromLoc = toLoc + dataSize;
	u64 shiftSize = (data.dataLength>fromLoc)?(data.dataLength-fromLoc):0;

	if (shiftSize)
		err = CopyDataValue(itemPtr,toLoc,itemPtr,fromLoc,shiftSize,&bytesWriten);

	if (dataSize)
		//shrink data block to the new size
		err = SetDataSizeByte(itemPtr,data,data.dataSize,data.dataLength-dataSize);

	return(0);
}

GrapaError GrapaBtree::MoveDataValueByte(u64 toPtr, GrapaBlockDataHeader& toData, u64 toOffset, u64 fromPtr, GrapaBlockDataHeader& fromData, u64 fromOffset, u64 moveSize, u64 *returnSize)
{
	// Move fromData(fromOffset,moveSize) to toData(toOffset,moveSize)
	//    Create moveSize space at toData(toOffset)
	//    Copy data over
	//    Delete toData(toOffset,moveSize)

	// If fromPtr==toPtr, then fromData is not valid...use toData

	GrapaError err;
	u64 bytesMoved;
	u64 fromLoc,toLoc,shiftSize;

	if (!moveSize) return(0);
	if (fromPtr==toPtr && (toOffset == fromOffset  || toOffset == (fromOffset+moveSize))) return(0);

	if (fromPtr==toPtr)
	{
		u64 tempPtr=0;
		if (fromOffset > (toData.dataLength-moveSize) || (toOffset+moveSize) > toData.dataLength) return(-1);
		if (fromOffset < toOffset && (fromOffset+moveSize) > toOffset)
		{
			err = MoveDataValueByte(toPtr,toData,fromOffset,toPtr,toData,fromOffset+moveSize,toOffset-fromOffset,&bytesMoved);
			/*
			// Put the smaller part into the temp location and shift the larger part
			shiftSize = toOffset - fromOffset;
			err = NewData(toData.dataType,toData.parentTree,shiftSize,shiftSize,1,tempPtr,false);
			err = CopyDataValue(tempPtr,0,fromPtr,fromOffset+moveSize,shiftSize,&bytesWriten);
			err = CopyDataValue(toPtr,toOffset,toPtr,fromOffset,moveSize,&bytesWriten);
			err = CopyDataValue(toPtr,fromOffset,tempPtr,0,shiftSize,&bytesWriten);
			err = DeleteData(tempPtr);
			*/
		}
		else if ((fromOffset+moveSize) < toOffset && (toOffset-fromOffset-moveSize) < moveSize)
		{
			err = MoveDataValueByte(toPtr,toData,fromOffset,toPtr,toData,fromOffset+moveSize,toOffset-fromOffset-moveSize,&bytesMoved);
		}
		else if (toOffset < fromOffset && (toOffset+moveSize) > fromOffset)
		{
			err = MoveDataValueByte(toPtr,toData,toOffset+moveSize,toPtr,toData,toOffset,fromOffset-toOffset,&bytesMoved);
			/*
			// Put the smaller part into the temp location and shift the larger part
			shiftSize = fromOffset - toOffset;
			err = NewData(toData.dataType,toData.parentTree,shiftSize,shiftSize,1,tempPtr,false);
			err = CopyDataValue(tempPtr,0,fromPtr,toOffset,shiftSize,&bytesWriten);
			err = CopyDataValue(toPtr,toOffset,toPtr,fromOffset,moveSize,&bytesWriten);
			err = CopyDataValue(toPtr,toOffset+moveSize,tempPtr,0,shiftSize,&bytesWriten);
			err = DeleteData(tempPtr);
			*/
		}
		else
		{
			err = NewData(toData.dataType,toData.parentTree,moveSize,moveSize,1,tempPtr,false);
			err = CopyDataValue(tempPtr,0,fromPtr,fromOffset,moveSize,&bytesMoved);
			if (fromOffset < toOffset)
			{
				fromLoc = fromOffset + moveSize;
				toLoc = fromOffset;
				shiftSize = toOffset - fromOffset;
			}
			else
			{
				fromLoc = toOffset;
				toLoc = toOffset + moveSize;
				shiftSize = fromOffset - toOffset;
			}
			err = CopyDataValue(toPtr,toLoc,toPtr,fromLoc,shiftSize,&bytesMoved);
			err = CopyDataValue(toPtr,toOffset,tempPtr,0,moveSize,&bytesMoved);
			err = DeleteData(tempPtr);
		}
	}
	else
	{
		if ((fromOffset+moveSize) > fromData.dataLength || toOffset > toData.dataLength) return(-1);
		fromLoc = toOffset;
		toLoc = toOffset + moveSize;
		shiftSize = (toData.dataLength>fromLoc)?(toData.dataLength-fromLoc):0;
		err = SetDataSizeByte(toPtr,toData,toData.dataSize,toData.dataLength+moveSize);
		toData.Write(mFile,toPtr);
		if (shiftSize)
			err = CopyDataValue(toPtr,toLoc,toPtr,fromLoc,shiftSize,&bytesMoved);
		err = CopyDataValue(toPtr,toOffset,fromPtr,fromOffset,moveSize,&bytesMoved);
		err = DeleteDataValueByte(fromPtr,fromData,fromOffset,moveSize,returnSize);
	}
	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError	GrapaBtree::NewDataFrag(GrapaBlockDataHeader& data, bool clear)
{
	// Don't pre-allocate the data. Grow as needed and rebalance as needed.
	data.dataSize = 0;
	data.dataLength = 0;
	return NewTree(data.dataStart,BDATA_TREE,data.parentTree);
}

GrapaError	GrapaBtree::DeleteDataFrag(GrapaBlockDataHeader& data)
{
	return DeleteTree(data.dataStart);
}

GrapaError	GrapaBtree::SetDataSizeFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 dataSize, u64 dataLength)
{
	GrapaError err;
	GrapaCursor cursor;
	GrapaBlockDataHeader frag;
	u64 size, fragOffset;
	u8 compressType=0;

	// This needs to truncate data.dataLength down to dataLength,
	// or grow to accomodate dataLength or dataSize, whichever is higher.
	// Elmininate resulting 0 length items.

	// Ignore dataSize...it's all about dataLength and growSize

	if (dataLength < data.dataLength)
	{
		u64 truncateBytes = data.dataLength - dataLength;
		// Truncate
		cursor.Set(data.dataStart,BDATA_ITEM,dataLength);
		err = Search(cursor);
		fragOffset = dataLength - cursor.mLength;
		if (!err && fragOffset && truncateBytes)
		{
			err = frag.Read(mFile,cursor.mValue);
			if (err) return(err);
			err = SetDataSize(cursor.mValue, data.dataSize, fragOffset, compressType);
			// Need to think about the error conditions
			if (err) return(err);
			err = UpdateDataKeyFrag(cursor);
			data.dataLength -= (frag.dataLength-fragOffset);
			truncateBytes -= (frag.dataLength-fragOffset);
			if (fragOffset) err = Next(cursor);
			fragOffset = 0;
		}
		// Need to purge
		while (!err)
		{
			cursor.Set(data.dataStart,BDATA_ITEM,dataLength);
			cursor.mTreeType = BDATA_TREE;
			err = Delete(cursor);
		}
		data.dataLength = dataLength;
	} else if (dataLength > data.dataLength)
	{
		// Grow
		u64 nullSize = dataLength - data.dataLength;

		// null out from data.dataLength to end of the last block, grow length if needed
		cursor.Set(data.dataStart,BDATA_ITEM);
		err = Last(cursor);
		if (!err)
		{
			err = frag.Read(mFile,cursor.mValue);
			if (err) return(err);
			size = (frag.dataSize-frag.dataLength)<nullSize?(frag.dataSize-frag.dataLength):nullSize;
			err = SetDataSize(cursor.mValue, data.dataSize, frag.dataLength + size, compressType);
			// Need to think about the error conditions
			if (err) return(err);
			err = UpdateDataKeyFrag(cursor);
			data.dataLength += size;
			nullSize -= size;
		}

		// then add null blocks to get up to offset
		while(nullSize)
		{
			u64 newPtr;
			size = data.growSize<nullSize?data.growSize:nullSize;
			err = NewData(BYTE_DATA,data.parentTree,data.growSize,data.growBlockSize,GrapaMem::Blocks(data.growSize,data.growBlockSize),newPtr,true);
			err = SetDataSize(newPtr, data.growSize, size, compressType);
			cursor.Set(data.dataStart,BDATA_ITEM,data.dataLength,newPtr);
			err = Insert(cursor);
			nullSize -= size;
			data.dataLength += size;
		}
	}

	return(0);
}

GrapaError	GrapaBtree::GetDataSizeFrag(GrapaBlockDataHeader& data, u64& growBlockSize, u64& dataSize, u64& dataLength)
{
	growBlockSize = data.growBlockSize;
	dataSize = data.dataSize;
	dataLength = data.dataLength;
	return(0);
}

GrapaError	GrapaBtree::SetDataValueFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;
	GrapaCursor cursor;
	GrapaBlockDataHeader frag;
	u64 fragOffset,bufferOffset;
	u64 size,rSize;
	u8 compressType = 0;

	if (returnSize) *returnSize = 0;

	if (offset == -1) offset = data.dataLength;

	// Search tree for the offset
	// Overwrite the data...may need to span multiple blocks
	// Update dataSize if additional blocks created
	// Update dataLengh if written past the end

	bufferOffset = 0;

	if (offset < data.dataLength)
	{
		cursor.Set(data.dataStart,BDATA_ITEM,offset);
		err = Search(cursor);
		fragOffset = offset - cursor.mLength;
		while (!err && bufferSize)
		{
			err = frag.Read(mFile,cursor.mValue);
			if (err) return(err);
			size = (frag.dataLength-fragOffset)<bufferSize?(frag.dataLength-fragOffset):bufferSize;
			err = SetDataValue(cursor.mValue,fragOffset,size,&((u8*)buffer)[bufferOffset],&rSize);
			// Need to think about the error conditions
			if (err) return(err);
			bufferOffset += rSize;
			offset += rSize;
			bufferSize -= rSize;
			if (returnSize) *returnSize += rSize;
			err = Next(cursor);
			fragOffset = 0;
		}
	}

	if (offset > data.dataLength && bufferSize)
	{
		u64 nullSize = offset - data.dataLength;

		// null out from data.dataLength to end of the last block, grow length if needed
		cursor.Set(data.dataStart,BDATA_ITEM);
		err = Last(cursor);
		if (!err)
		{
			err = frag.Read(mFile,cursor.mValue);
			if (err) return(err);
			size = (frag.dataSize-frag.dataLength)<nullSize?(frag.dataSize-frag.dataLength):nullSize;
			err = SetDataSize(cursor.mValue, data.dataSize, frag.dataLength + size, compressType);
			// Need to think about the error conditions
			if (err) return(err);
			err = UpdateDataKeyFrag(cursor);
			data.dataLength += size;
			nullSize -= size;
		}

		// then add null blocks to get up to offset
		while(nullSize)
		{
			u64 newPtr;
			size = data.growSize<nullSize?data.growSize:nullSize;
			err = NewData(BYTE_DATA,data.parentTree,data.growSize,data.growBlockSize,GrapaMem::Blocks(data.growSize,data.growBlockSize),newPtr,true);
			err = SetDataSize(newPtr, data.growSize, size, compressType);
			cursor.Set(data.dataStart,BDATA_ITEM,data.dataLength,newPtr);
			err = Insert(cursor);
			nullSize -= size;
			data.dataLength += size;
		}
	}

	if (bufferSize)
	{
		// fill up the last block, grow length to max size if needed
		cursor.Set(data.dataStart,BDATA_ITEM);
		err = Last(cursor);
		if (!err)
		{
			err = frag.Read(mFile,cursor.mValue);
			if (err) return(err);
			size = (frag.dataSize-frag.dataLength)<bufferSize?(frag.dataSize-frag.dataLength):bufferSize;
			err = SetDataSize(cursor.mValue, data.dataSize, frag.dataLength + size, compressType);
			err = SetDataValue(cursor.mValue,frag.dataLength,size,&((u8*)buffer)[bufferOffset],&rSize);
			// Need to think about the error conditions
			if (err) return(err);
			err = UpdateDataKeyFrag(cursor);
			data.dataLength += rSize;
			bufferOffset += rSize;
			offset += rSize;
			bufferSize -= rSize;
			if (returnSize) *returnSize += rSize;
		}

		while (bufferSize)
		{
			u64 newPtr;
			size = data.growSize<bufferSize?data.growSize:bufferSize;
			err = NewData(BYTE_DATA,data.parentTree,data.growSize,data.growBlockSize,GrapaMem::Blocks(data.growSize,data.growBlockSize),newPtr,true);
			err = SetDataValue(newPtr,0,size,&((u8*)buffer)[bufferOffset],&rSize);
			// Need to think about the error conditions
			if (err) return(err);
			cursor.Set(data.dataStart,BDATA_ITEM,data.dataLength,newPtr);
			err = Insert(cursor);
			// Need to think about the error conditions
			if (err) return(err);
			bufferOffset += rSize;
			offset += rSize;
			bufferSize -= rSize;
			if (returnSize) *returnSize += rSize;
			data.dataLength += rSize;
		}
	}

	return(0);
}

GrapaError	GrapaBtree::GetDataValueFrag(GrapaBlockDataHeader& data, u64 offset, u64 bufferSize, void *buffer, u64 *returnSize)
{
	GrapaError err;
	GrapaCursor cursor;
	GrapaBlockDataHeader frag;
	u64 fragOffset,bufferOffset;
	u64 size,rSize;

	if (returnSize) *returnSize = 0;

	if (offset >= data.dataLength) return(-1);

	bufferOffset = 0;

	cursor.Set(data.dataStart,BDATA_ITEM,offset);
	err = Search(cursor);
	fragOffset = offset - cursor.mLength;
	while (!err && bufferSize)
	{
		err = frag.Read(mFile,cursor.mValue);
		if (err) return(err);
		size = (frag.dataLength-fragOffset)<bufferSize?(frag.dataLength-fragOffset):bufferSize;
		err = GetDataValueByte(frag,fragOffset,size,&((u8*)buffer)[bufferOffset],&rSize);
		// Need to think about the error conditions
		if (err) return(err);
		bufferOffset += rSize;
		offset += rSize;
		bufferSize -= rSize;
		if (returnSize) *returnSize += rSize;
		err = Next(cursor);
		fragOffset = 0;
	}

	return(0);
}

GrapaError	GrapaBtree::InsertDataValueFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, void *dataBytes, u64 *returnSize)
{
	GrapaError err;
	GrapaCursor cursor;
	u64 gsize,dsize,dlen;
	u64 toPos,dataOffset=0;
	u64 bytesWriten;
	u64 moveSize;
	u8 compressType=0;

	if (offset >= data.dataLength) offset = data.dataLength;

	cursor.Set(data.dataStart,BDATA_ITEM,offset);
	cursor.mLength = data.dataLength;
	gsize = data.growSize; 
	dsize = dlen = 0;
	if (offset < data.dataLength)
	{
		err = Search(cursor);
		if (err) return(err);
		err = GetDataSize(cursor.mValue, gsize, dsize, dlen, compressType);
	}
	toPos = (offset-cursor.mLength);

	GrapaCursor prevTo=cursor;
	u64 prevToGrowBlockSize;
	u64 prevToLen=0,prevToSize=0;

	if (offset < cursor.mLength)
	{
		if (!Prev(prevTo)) err = GetDataSize(prevTo.mValue, prevToGrowBlockSize, prevToSize, prevToLen, compressType);
	}
	else if (cursor.mLength)
	{
		prevTo.Set(data.dataStart,BDATA_ITEM,cursor.mLength-1);
		err = Search(prevTo);
		err = GetDataSize(prevTo.mValue, prevToGrowBlockSize, prevToSize, prevToLen, compressType);
	}

	if (prevToSize && (prevToSize-prevToLen) >= toPos)
	{
		err = SetDataSize(prevTo.mValue, prevToSize, prevToLen + toPos, compressType);
		err = CopyDataValue(prevTo.mValue,prevToLen,cursor.mValue,0,toPos,&bytesWriten);
		if ((prevToLen+toPos) < prevToSize)
		{
			moveSize = dataSize>(prevToSize-prevToLen-toPos)?(prevToSize-prevToLen-toPos):dataSize;
			err = SetDataSize(prevTo.mValue, prevToSize, prevToLen + toPos + moveSize, compressType);
			if (dataBytes) err = SetDataValue(prevTo.mValue, prevToLen + toPos, moveSize, &((u8*)dataBytes)[dataOffset], &bytesWriten);
			offset += moveSize;
			dataSize -= moveSize;
			dataOffset += moveSize;
			data.dataLength += moveSize;
		}
		err = UpdateDataKeyFrag(prevTo);
		moveSize = dataSize>toPos?toPos:dataSize;
		if (dataBytes) err = SetDataValue(cursor.mValue, 0, moveSize, &((u8*)dataBytes)[dataOffset], &bytesWriten);
		if (moveSize<toPos)
		{
			// Maybe first check if the tail can move to next???
			err = CopyDataValue(cursor.mValue,moveSize,cursor.mValue,toPos,dlen-toPos,&bytesWriten);
			err = SetDataSize(cursor.mValue, dsize, dlen - (toPos - moveSize), compressType);
			err = UpdateDataKeyFrag(cursor);
			dlen -= (toPos-moveSize);
			toPos = moveSize;
		}
		offset += moveSize;
		dataOffset += moveSize;
		dataSize -= moveSize;
		data.dataLength += moveSize;
	}

	if (dataSize)
	{
		GrapaCursor nextTo=cursor;
		u64 nextToGrowBlockSize=0;
		u64 nextToLen=0,nextToSize=0;
		if (!Next(nextTo)) err = GetDataSize(nextTo.mValue, nextToGrowBlockSize, nextToSize, nextToLen, compressType);

		if (nextToSize && (dlen-toPos) <= (nextToSize-nextToLen))
		{
			err = SetDataSize(nextTo.mValue, nextToSize, nextToLen + (dlen - toPos), compressType);
			err = CopyDataValue(nextTo.mValue,(dlen-toPos),nextTo.mValue,0,nextToLen);
			err = CopyDataValue(nextTo.mValue,0,cursor.mValue,toPos,(dlen-toPos));
			err = UpdateDataKeyFrag(nextTo);
			moveSize = dataSize < (dsize-toPos) ? dataSize : (dsize-toPos);
			err = SetDataSize(cursor.mValue, dsize, toPos + moveSize, compressType);
			if (dataBytes) err = SetDataValue(cursor.mValue, toPos, moveSize, &((u8*)dataBytes)[dataOffset], &bytesWriten);
			offset += moveSize;
			dataOffset += moveSize;
			dataSize -= moveSize;
			data.dataLength += moveSize;
			err = UpdateDataKeyFrag(cursor);
			cursor = nextTo;
			gsize = nextToGrowBlockSize;
			dsize = nextToSize;
			dlen = nextToLen + (dlen-toPos);
			toPos = 0;
		}
		else if (dataSize <= (dsize-dlen))
		{
			moveSize = dataSize;
			err = SetDataSize(cursor.mValue, dsize, dlen + moveSize, compressType);
			err = CopyDataValue(cursor.mValue,toPos+moveSize,cursor.mValue,toPos,dlen-toPos,&bytesWriten);
			if (dataBytes) err = SetDataValue(cursor.mValue, toPos, moveSize, &((u8*)dataBytes)[dataOffset], &bytesWriten);
			err = UpdateDataKeyFrag(cursor);
			offset += moveSize;
			dataOffset += moveSize;
			dataSize -= moveSize;
			data.dataLength += moveSize;
			toPos += moveSize;
		}
		if (dataSize && toPos)
		{
			u64 newPtr = 0;
			err = NewData(BYTE_DATA,data.parentTree,data.growSize,data.growBlockSize,GrapaMem::Blocks(data.growSize,data.growBlockSize),newPtr,true);
			if ((data.growSize > (dlen-toPos)))
			{
				// Pull in the tail of the incoming data
				moveSize = (data.growSize-(dlen-toPos)) < dataSize ? (data.growSize-(dlen-toPos)) : dataSize;
				err = SetDataSize(newPtr, data.growSize, moveSize + (dlen - toPos), compressType);
				if (dataBytes) err = SetDataValue(newPtr, 0, moveSize, &((u8*)dataBytes)[dataOffset + (dataSize - moveSize)], &bytesWriten);
				err = CopyDataValue(newPtr,moveSize,cursor.mValue,toPos,(dlen-toPos),&bytesWriten);
				dataSize -= moveSize;
				data.dataLength += moveSize;
			}
			else
			{
				err = SetDataSize(newPtr, data.growSize, dlen - toPos, compressType);
				err = CopyDataValue(newPtr,0,cursor.mValue,toPos,(dlen-toPos),&bytesWriten);
			}
			if (dataSize && (dsize-toPos))
			{
				// Append the head of the incoming data
				moveSize = (dsize-toPos) < dataSize ? (dsize-toPos) : dataSize;
				err = SetDataSize(cursor.mValue, dsize, toPos + moveSize, compressType);
				if (dataBytes) err = SetDataValue(cursor.mValue, toPos, moveSize, &((u8*)dataBytes)[dataOffset], &bytesWriten);
				offset += moveSize;
				dataOffset += moveSize;
				dataSize -= moveSize;
				data.dataLength += moveSize;
			}
			else
			{
				err = SetDataSize(cursor.mValue, dsize, toPos, compressType);
			}
			err = UpdateDataKeyFrag(cursor);
			cursor.Set(data.dataStart,BDATA_ITEM,offset,newPtr);
			err = Insert(cursor);
		}
	}

	while (dataSize)
	{
		u64 newPtr = 0;
		moveSize = data.growSize<dataSize?data.growSize:dataSize;
		err = NewData(BYTE_DATA,data.parentTree,data.growSize,data.growBlockSize,GrapaMem::Blocks(data.growSize,data.growBlockSize),newPtr,true);
		err = SetDataSize(newPtr, data.growSize, moveSize, compressType);
		if (dataBytes) err = SetDataValue(newPtr, 0, moveSize, &((u8*)dataBytes)[dataOffset], &bytesWriten);
		cursor.Set(data.dataStart,BDATA_ITEM,offset,newPtr);
		err = Insert(cursor);
		offset += moveSize;
		dataOffset += moveSize;
		dataSize -= moveSize;
		data.dataLength += moveSize;
	}

	return(0);
}

GrapaError	GrapaBtree::DeleteDataValueFrag(u64 itemPtr, GrapaBlockDataHeader& data, u64 offset, u64 dataSize, u64 *returnSize)
{
	GrapaError err;
	GrapaCursor cursor;
	u64 gsize;
	u64 dsize,dlen;
	u64 doffset;
	u8 compressType=0;

	cursor.Set(data.dataStart,BDATA_ITEM,offset);
	err = Search(cursor);
	if (err) return(err);

	doffset  = offset - cursor.mLength;

	while(!err && dataSize>0)
	{
		err = GetDataSize(cursor.mValue, gsize, dsize, dlen, compressType);
		if (err) return(err);

		u64 delSize = (((dataSize+doffset)<dlen)?dataSize:(dlen-doffset));
		u64 moveSize =  dlen - (doffset + delSize);
		u64 toLoc = doffset;
		u64 fromLoc = doffset + delSize;
		doffset = 0;

		if (moveSize)
		{
			// This should be done in a loop with a fixed buffersize...just in case moveSize is very big.
			u64 bytesWriten=0;
			err = CopyDataValue(cursor.mValue,toLoc,cursor.mValue,fromLoc,moveSize,&bytesWriten);
		}

		if (!delSize) break;

		if (dlen-delSize)
		{
			err = SetDataSize(cursor.mValue, dsize, dlen - delSize, compressType);
			if (err) return(err);
			dataSize -= delSize;
			data.dataLength -= delSize;
			err = UpdateDataKeyFrag(cursor);
			dlen -= delSize;

			if (dlen <= (dsize*5)/5) // The 100% should be configurable
			{
				GrapaCursor prev=cursor,next=cursor;
				u64 prevGrowBlockSize,nextGrowBlockSize;
				u64 prevLen=0,prevSize=0,nextLen=0,nextSize=0;
				if (!Prev(prev)) err = GetDataSize(prev.mValue, prevGrowBlockSize, prevSize, prevLen, compressType);
				if (!Next(next)) err = GetDataSize(next.mValue, nextGrowBlockSize, nextSize, nextLen, compressType);
				if ((prevLen+dlen) <= prevSize)
				{
					// MergeLeft
					u64 bytesWriten;
					err = SetDataSize(prev.mValue, prevSize, prevLen + dlen, compressType);
					err = CopyDataValue(prev.mValue,prevLen,cursor.mValue,0,dlen,&bytesWriten);
					err = UpdateDataKeyFrag(prev);
					cursor.Set(data.dataStart,BDATA_ITEM,cursor.mLength+dlen);
					err = Delete(cursor);
					cursor.Set(data.dataStart,BDATA_ITEM,cursor.mLength);
					err = Search(cursor);
				}
				else if ((dlen+nextLen) <= nextSize)
				{
					// MergeRight
					u64 bytesWriten;
					err = SetDataSize(next.mValue, nextSize, nextLen + dlen, compressType);
					err = CopyDataValue(next.mValue,dlen,next.mValue,0,nextLen,&bytesWriten);
					err = CopyDataValue(next.mValue,0,cursor.mValue,0,dlen,&bytesWriten);
					err = UpdateDataKeyFrag(next);
					cursor.Set(data.dataStart,BDATA_ITEM,cursor.mLength);
					err = Delete(cursor);
					cursor.Set(data.dataStart,BDATA_ITEM,cursor.mLength);
					err = Search(cursor);
				}
			}

			err = Next(cursor);
		}
		else
		{
			cursor.Set(data.dataStart,BDATA_ITEM,offset);
			err = Delete(cursor);
			if (err) return(err);
			dataSize -= delSize;
			data.dataLength -= delSize;
			cursor.Set(data.dataStart,BDATA_ITEM,offset);
			err = Search(cursor);
		}
	}

	return(0);
}

GrapaError GrapaBtree::MoveDataValueFrag(u64 toPtr, GrapaBlockDataHeader& toData, u64 toOffset, u64 fromPtr, GrapaBlockDataHeader& fromData, u64 fromOffset, u64 moveSize, u64 *returnSize)
{
	// Move fromData(fromOffset,moveSize) to toData(toOffset,moveSize)
	//    Create moveSize space at toData(toOffset)
	//    Copy data over
	//    Delete toData(toOffset,moveSize)

	GrapaError err;
	GrapaCursor cursor;
	u64 gsize,dsize,dlen;
	GrapaBlockDataHeader frag;
	u64 bytesMoved;
	GrapaBlockDataHeader moveData;
	GrapaCursor insertCursor;
	u8 compressType=0;

	if (!moveSize) return(0);
	if (fromPtr==toPtr && (toOffset == fromOffset  || toOffset == (fromOffset+moveSize))) return(0);

	if (fromPtr==toPtr)
	{
		if (toOffset > toData.dataLength) toOffset = toData.dataLength;
		if (fromOffset > (toData.dataLength-moveSize) || toOffset > toData.dataLength) return(-1);
		if (fromOffset < toOffset && (fromOffset+moveSize) > toOffset)
		{
			err = MoveDataValueFrag(toPtr,toData,fromOffset,toPtr,toData,fromOffset+moveSize,toOffset-fromOffset,&bytesMoved);
		}
		else if ((fromOffset+moveSize) < toOffset && (toOffset-fromOffset-moveSize) < moveSize)
		{
			err = MoveDataValueFrag(toPtr,toData,fromOffset,toPtr,toData,fromOffset+moveSize,toOffset-fromOffset-moveSize,&bytesMoved);
		}
		else if (toOffset < fromOffset && (toOffset+moveSize) > fromOffset)
		{
			err = MoveDataValueFrag(toPtr,toData,fromOffset+moveSize,toPtr,toData,toOffset,fromOffset-toOffset,&bytesMoved);
		}
		else
		{
			cursor.Set(toData.dataStart,BDATA_ITEM,fromOffset);
			err = Search(cursor);
			if (err) return(err);
			err = GetDataSize(cursor.mValue, gsize, dsize, dlen, compressType);
			if (err) return(err);
			u64 cstart = cursor.mLength;
			u64 cend = cstart + dlen;
			if ((fromOffset+moveSize) <= (cursor.mLength+dlen) && toOffset >= cursor.mLength && (toOffset+moveSize) <= (cursor.mLength+dlen))
			{
				// The move is within the same block...use the byte move directly
				err = frag.Read(mFile,cursor.mValue);
				MoveDataValueByte(cursor.mValue,frag,toOffset-cursor.mLength,cursor.mValue,frag,fromOffset-cursor.mLength,moveSize,&bytesMoved);
			}
			else
			{
				u64 movePtr=0,newPtr;
				GrapaBlockDataHeader newFrag;

				err = NewData(FREC_DATA,toData.parentTree,0,toData.growBlockSize,GrapaMem::Blocks(toData.growSize,toData.growBlockSize),movePtr,false);
				err = moveData.Read(mFile,movePtr);

//what should parentTree be set to for NewData(BYTE_DATA,parentTree)? Probably dataStart...

				if (fromOffset != cursor.mLength)
				{
					err = frag.Read(mFile,cursor.mValue);
					// Create and insert new block
					u64 shiftSize = (fromOffset+moveSize)<=(cursor.mLength+dlen)?moveSize:(cursor.mLength+dlen)-fromOffset;
					
					err = NewData(BYTE_DATA,frag.parentTree,moveData.growSize,moveData.growBlockSize,GrapaMem::Blocks(moveData.growSize,moveData.growBlockSize),newPtr,true);
					err = newFrag.Read(mFile,newPtr);
					err = MoveDataValueByte(newPtr,newFrag,0,cursor.mValue,frag,fromOffset-cursor.mLength,shiftSize,&bytesMoved);

					err = frag.Write(mFile,cursor.mValue);
					err = newFrag.Write(mFile,newPtr);
					err = UpdateDataKeyFrag(cursor);

					// Need to think about the error conditions
					if (err) return(err);
					insertCursor.Set(moveData.dataStart,BDATA_ITEM,moveData.dataLength,newPtr);
					err = Insert(insertCursor);
					// Need to think about the error conditions
					if (err) return(err);

					moveData.dataLength += shiftSize;
					toData.dataLength -= shiftSize;
					moveSize -= shiftSize;
					if (toOffset > fromOffset) toOffset -= shiftSize;

					cursor.Set(toData.dataStart,BDATA_ITEM,fromOffset);
					err = Search(cursor);
					if (err) return(err);
					err = GetDataSize(cursor.mValue, gsize, dsize, dlen, compressType);
					if (err) return(err);
					cstart = cursor.mLength;
					cend = cstart + dlen;
				}

				while(moveSize > dlen)
				{
					// Move entire block over
					err = frag.Read(mFile,cursor.mValue);
					u64 shiftSize = dlen;

					err = NewData(BYTE_DATA,moveData.dataStart,1,1,1,newPtr,false);

					err = newFrag.Read(mFile,newPtr);
					err = SetDataSizeByte(newPtr,newFrag,1,1);
					u64 tempStart = newFrag.dataStart;
					newFrag = frag;
					frag.dataStart = tempStart;
					frag.dataSize = 1;
					frag.growBlockSize = 1;
					frag.dataLength = 1;

					err = frag.Write(mFile,cursor.mValue);
					err = newFrag.Write(mFile,newPtr);
					err = UpdateDataKeyFrag(cursor);
					cursor.Set(toData.dataStart,BDATA_ITEM,fromOffset);
					err = Delete(cursor);

					insertCursor.Set(moveData.dataStart,BDATA_ITEM,moveData.dataLength,newPtr);
					err = Insert(insertCursor);

					moveData.dataLength += shiftSize;
					toData.dataLength -= shiftSize;
					moveSize -= shiftSize;
					if (toOffset > fromOffset) toOffset -= shiftSize;

					if (moveSize)
					{
						cursor.Set(toData.dataStart,BDATA_ITEM,fromOffset);
						err = Search(cursor);
						if (err) return(err);
						err = GetDataSize(cursor.mValue, gsize, dsize, dlen, compressType);
						if (err) return(err);
					}
				}

				if (moveSize)
				{
					// Now move end piece over
					err = frag.Read(mFile,cursor.mValue);
					// Create and insert new block
					u64 shiftSize = moveSize;
					
					err = NewData(BYTE_DATA,frag.parentTree,moveData.growSize,moveData.growBlockSize,GrapaMem::Blocks(moveData.growSize,moveData.growBlockSize),newPtr,true);
					err = newFrag.Read(mFile,newPtr);
					err = MoveDataValueByte(newPtr,newFrag,0,cursor.mValue,frag,0,shiftSize,&bytesMoved);

					err = frag.Write(mFile,cursor.mValue);
					err = newFrag.Write(mFile,newPtr);
					err = UpdateDataKeyFrag(cursor);

					// Need to think about the error conditions
					if (err) return(err);
					insertCursor.Set(moveData.dataStart,BDATA_ITEM,moveData.dataLength,newPtr);
					err = Insert(insertCursor);
					// Need to think about the error conditions
					if (err) return(err);

					moveData.dataLength += shiftSize;
					toData.dataLength -= shiftSize;
					moveSize -= shiftSize;
					if (toOffset > fromOffset) toOffset -= shiftSize;
				}
				
				// merge the boundary blocks if needed

				cursor.Set(toData.dataStart,BDATA_ITEM,fromOffset);
				err = Search(cursor);
				if (err) return(err);
				err = GetDataSize(cursor.mValue, gsize, dsize, dlen, compressType);
				if (err) return(err);

				GrapaCursor prev=cursor;
				u64 prevGrowBlockSize;
				u64 prevLen=0,prevSize=0;
				if (!Prev(prev)) err = GetDataSize(prev.mValue, prevGrowBlockSize, prevSize, prevLen, compressType);

				if ((prevSize-prevLen) >= dlen)
				{
					err = SetDataSize(prev.mValue, prevSize, prevLen + dlen, compressType);
					err = CopyDataValue(prev.mValue,prevLen,cursor.mValue,0,dlen,&bytesMoved);
					err = UpdateDataKeyFrag(prev);
					cursor.Set(toData.dataStart,BDATA_ITEM,fromOffset+dlen);
					err = Delete(cursor);
				}
				else
				{
					GrapaCursor next=cursor;
					u64 nextGrowBlockSize;
					u64 nextLen=0,nextSize=0;
					if (!Next(next)) err = GetDataSize(next.mValue, nextGrowBlockSize, nextSize, nextLen, compressType);

					if ((dsize-dlen) >= nextLen)
					{
						err = SetDataSize(cursor.mValue, dsize, dlen + nextLen, compressType);
						err = CopyDataValue(cursor.mValue,dlen,next.mValue,0,nextLen,&bytesMoved);
						err = UpdateDataKeyFrag(cursor);
						next.Set(toData.dataStart,BDATA_ITEM,fromOffset+dlen);
						err = Delete(next);
					}
				}

				err = MoveDataValueFrag(toPtr,toData,toOffset,movePtr,moveData,0,moveData.dataLength,&bytesMoved);
				err = DeleteData(movePtr);
			}
		}
	}
	else
	{
		GrapaCursor fromCursor,toCursor;
		u64 fgsize,fdsize,fdlen;
		u64 tgsize,tdsize,tdlen;
		u64 fromPos,toPos;

		if (toOffset > toData.dataLength) toOffset = toData.dataLength;
		if ((fromOffset+moveSize) > fromData.dataLength || toOffset > toData.dataLength) return(-1);

		fromCursor.Set(fromData.dataStart,BDATA_ITEM,fromOffset);
		err = Search(fromCursor);
		if (err) return(err);
		err = GetDataSize(fromCursor.mValue, fgsize, fdsize, fdlen, compressType);
		fromPos = (fromOffset-fromCursor.mLength);

		toCursor.Set(toData.dataStart,BDATA_ITEM,toOffset);
		toCursor.mLength = toData.dataLength;
		tgsize = toData.growSize; 
		tdsize = tdlen = 0;
		if (toOffset < toData.dataLength)
		{
			err = Search(toCursor);
			if (err) return(err);
			err = GetDataSize(toCursor.mValue, tgsize, tdsize, tdlen, compressType);
		}
		toPos = (toOffset-toCursor.mLength);

		GrapaCursor prevTo=toCursor;
		u64 prevToGrowBlockSize;
		u64 prevToLen=0,prevToSize=0;

		if (toOffset < toCursor.mLength)
		{
			if (!Prev(prevTo)) err = GetDataSize(prevTo.mValue, prevToGrowBlockSize, prevToSize, prevToLen, compressType);
		}
		else if (toCursor.mLength)
		{
			prevTo.Set(toData.dataStart,BDATA_ITEM,toCursor.mLength-1);
			err = Search(prevTo);
			err = GetDataSize(prevTo.mValue, prevToGrowBlockSize, prevToSize, prevToLen, compressType);
		}

		if (toPos)
		{
			if ((prevToSize-prevToLen) >= toPos)
			{
				err = SetDataSize(prevTo.mValue, prevToSize, prevToLen + toPos, compressType);
				err = CopyDataValue(prevTo.mValue,prevToLen,toCursor.mValue,0,toPos,&bytesMoved);
				err = CopyDataValue(toCursor.mValue,0,toCursor.mValue,toPos,tdlen-toPos,&bytesMoved);
				err = SetDataSize(toCursor.mValue, tdsize, tdlen - toPos, compressType);
				err = UpdateDataKeyFrag(prevTo);
				err = UpdateDataKeyFrag(toCursor);
			}
			else
			{
				u64 newPtr = 0;
				err = NewData(BYTE_DATA,toData.parentTree,toData.growSize,toData.growBlockSize,GrapaMem::Blocks(toData.growSize,toData.growBlockSize),newPtr,true);
				err = SetDataSize(newPtr, toData.growSize, tdlen - toPos, compressType);
				err = CopyDataValue(newPtr,0,toCursor.mValue,toPos,(tdlen-toPos),&bytesMoved);
				err = SetDataSize(toCursor.mValue, tdsize, toPos, compressType);
				err = UpdateDataKeyFrag(toCursor);
				insertCursor.Set(toData.dataStart,BDATA_ITEM,toOffset,newPtr);
				err = Insert(insertCursor);
			}

			toCursor.Set(toData.dataStart,BDATA_ITEM,toOffset);
			err = Search(toCursor);
			if (err) return(err);
			err = GetDataSize(toCursor.mValue, tgsize, tdsize, tdlen, compressType);
			toPos = (toOffset-toCursor.mLength);

			prevTo=toCursor;
			prevToLen=prevToSize=0;
			if (!Prev(prevTo)) err = GetDataSize(prevTo.mValue, prevToGrowBlockSize, prevToSize, prevToLen, compressType);
		}

		u64 shiftSize = (fromPos+moveSize) <= fdlen ? moveSize : (fdlen-fromPos);

		if ((prevToSize-prevToLen) >= shiftSize)
		{
			err = SetDataSize(prevTo.mValue, prevToSize, prevToLen + shiftSize, compressType);
			err = CopyDataValue(prevTo.mValue,prevToLen,fromCursor.mValue,fromPos,shiftSize,&bytesMoved);
			if (shiftSize < (fdlen-fromPos))
				CopyDataValue(fromCursor.mValue,fromPos,fromCursor.mValue,fromPos+shiftSize,fdlen-(fromPos+shiftSize),&bytesMoved);
			err = SetDataSize(fromCursor.mValue, fdsize, fdlen - shiftSize, compressType);
			fromData.dataLength -= shiftSize;
			toData.dataLength += shiftSize;
			moveSize -= shiftSize;
			toOffset += shiftSize;
			err = UpdateDataKeyFrag(prevTo); 
			err = UpdateDataKeyFrag(fromCursor); 

			fromCursor.Set(fromData.dataStart,BDATA_ITEM,fromOffset);
			err = Search(fromCursor);
			if (err) return(err);
			err = GetDataSize(fromCursor.mValue, fgsize, fdsize, fdlen, compressType);
			fromPos = (fromOffset-fromCursor.mLength);
			shiftSize = (fromPos+moveSize) <= fdlen ? moveSize : (fdlen-fromPos);
		}

		if (fromPos)
		{
			u64 newPtr = 0;
			err = NewData(BYTE_DATA,toData.parentTree,toData.growSize,toData.growBlockSize,GrapaMem::Blocks(toData.growSize,toData.growBlockSize),newPtr,true);
			err = SetDataSize(newPtr, toData.growSize, shiftSize, compressType);
			err = CopyDataValue(newPtr,0,fromCursor.mValue,fromPos,shiftSize,&bytesMoved);
			insertCursor.Set(toData.dataStart,BDATA_ITEM,toOffset,newPtr);
			err = Insert(insertCursor);
			toCursor = insertCursor;
			if (shiftSize < (fdlen-fromPos))
				CopyDataValue(fromCursor.mValue,fromPos,fromCursor.mValue,fromPos+shiftSize,fdlen-(fromPos+shiftSize),&bytesMoved);
			err = SetDataSize(fromCursor.mValue, fdsize, fdlen - shiftSize, compressType);
			fromData.dataLength -= shiftSize;
			toData.dataLength += shiftSize;
			moveSize -= shiftSize;
			toOffset += shiftSize;
			err = UpdateDataKeyFrag(fromCursor); 
		}

		fromCursor.Set(fromData.dataStart,BDATA_ITEM,fromOffset);
		err = Search(fromCursor);
		if (err) return(err);
		err = GetDataSize(fromCursor.mValue, fgsize, fdsize, fdlen, compressType);
		fromPos = (fromOffset-fromCursor.mLength);

		while(moveSize >= fdlen)
		{
			u64 newPtr = 0;
			GrapaBlockDataHeader newFrag;

			err = frag.Read(mFile,fromCursor.mValue);
			u64 shiftSize = fdlen;

			err = NewData(BYTE_DATA,toData.dataStart,1,1,1,newPtr,false);

			err = newFrag.Read(mFile,newPtr);
			err = SetDataSizeByte(newPtr,newFrag,1,1);
			u64 tempStart = newFrag.dataStart;
			newFrag = frag;
			frag.dataStart = tempStart;
			frag.dataSize = 1;
			frag.growBlockSize = 1;
			frag.dataLength = 1;

			err = frag.Write(mFile,fromCursor.mValue);
			err = newFrag.Write(mFile,newPtr);
			err = UpdateDataKeyFrag(fromCursor);  // is this really needed given that we are about to delete it anyway?
			fromCursor.Set(fromData.dataStart,BDATA_ITEM,fromOffset);
			err = Delete(fromCursor);

			insertCursor.Set(toData.dataStart,BDATA_ITEM,toOffset,newPtr);
			err = Insert(insertCursor);

			toData.dataLength += shiftSize;
			fromData.dataLength -= shiftSize;
			moveSize -= shiftSize;
			toOffset += shiftSize;

			if (moveSize)
			{
				fromCursor.Set(fromData.dataStart,BDATA_ITEM,fromOffset);
				err = Search(fromCursor);
				if (err) return(err);
				err = GetDataSize(fromCursor.mValue, fgsize, fdsize, fdlen, compressType);
				fromPos = (fromOffset-fromCursor.mLength);
			}
		}

		toCursor.Set(toData.dataStart,BDATA_ITEM,toOffset);
		err = Search(toCursor);
		if (err) return(err);
		err = GetDataSize(toCursor.mValue, tgsize, tdsize, tdlen, compressType);
		toPos = (toOffset-toCursor.mLength);

		if (moveSize)
		{
			if ((tdsize-tdlen) >= moveSize)
			{
				err = SetDataSize(toCursor.mValue, tdsize, tdlen + moveSize, compressType);
				if (tdlen-toPos)
					err = CopyDataValue(toCursor.mValue,toPos+moveSize,toCursor.mValue,toPos,tdlen-toPos,&bytesMoved);
				err = CopyDataValue(toCursor.mValue,toPos,fromCursor.mValue,fromPos,moveSize,&bytesMoved);
				err = UpdateDataKeyFrag(toCursor); 
			}
			else
			{
				u64 newPtr = 0;
				err = NewData(BYTE_DATA,toData.parentTree,toData.growSize,toData.growBlockSize,GrapaMem::Blocks(toData.growSize,toData.growBlockSize),newPtr,true);
				err = SetDataSize(newPtr, toData.growSize, moveSize, compressType);
				err = CopyDataValue(newPtr,0,fromCursor.mValue,fromPos,moveSize,&bytesMoved);
				insertCursor.Set(toData.dataStart,BDATA_ITEM,toOffset,newPtr);
				err = Insert(insertCursor);
				toCursor = insertCursor;
			}
			if (fdlen > (fromPos+moveSize))
			{
				err = CopyDataValue(fromCursor.mValue,fromPos,fromCursor.mValue,fromPos+moveSize,fdlen-(fromPos+moveSize),&bytesMoved);
				err = SetDataSize(fromCursor.mValue, fdsize, fdlen - moveSize, compressType);
				err = UpdateDataKeyFrag(fromCursor); 
			}
			toData.dataLength += moveSize;
			fromData.dataLength -= moveSize;
			moveSize = 0;
			toOffset += moveSize;
		}
		else
		{
			prevTo = toCursor;
			prevToLen=prevToSize=0;
			if (!Prev(prevTo)) err = GetDataSize(prevTo.mValue, prevToGrowBlockSize, prevToSize, prevToLen, compressType);
			if ((prevToLen+tdlen) <= prevToSize)
			{
				err = SetDataSize(prevTo.mValue, prevToSize, prevToLen + tdlen, compressType);
				err = CopyDataValue(prevTo.mValue,prevToLen,toCursor.mValue,0,tdlen,&bytesMoved);
				err = UpdateDataKeyFrag(prevTo);
				toCursor.Set(toData.dataStart,BDATA_ITEM,toOffset+tdlen);
				err = Delete(toCursor);
			}
		}

	}

	return(0);
}

GrapaError GrapaBtree::UpdateDataKeyFrag(GrapaCursor& cursor)
{
	GrapaError err;
	GrapaBlockDataHeader frag;
	GrapaBlockNodeHeader head;
	GrapaBlockNodeLeaf leaf;
	GrapaCursor fcursor;

	err = frag.Read(mFile,cursor.mValue);
	err = head.Read(mFile,cursor.mNodeRef);
	err = leaf.Read(mFile,cursor.mNodeRef,cursor.mNodeIndex);

	fcursor.Set(cursor.mTreeRef,SDATA_ITEM,leaf.key,leaf.value,leaf.flags);
	fcursor.mTreeType = cursor.mTreeType;
	err = LooseWeightCascading(fcursor,0,head,0);

	fcursor.Set(cursor.mTreeRef,SDATA_ITEM,frag.dataLength,leaf.value,leaf.flags);
	fcursor.mTreeType = cursor.mTreeType;
	err = GainWeightCascading(fcursor,0,head,0,0);

	err = head.Write(mFile,cursor.mNodeRef);

	leaf.key = frag.dataLength;
	err = leaf.Write(mFile,cursor.mNodeRef,cursor.mNodeIndex);

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
