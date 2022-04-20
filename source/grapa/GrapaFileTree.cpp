// GrapaFileTree.cpp
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

#include "GrapaFileTree.h"
#include "GrapaCompress.h"
#include "GrapaHash.h"
#include "GrapaMem.h"

GrapaFileTree::GrapaFileTree()
{
	mRoot = 0;
	mBlockSize = 1024 * 32;
	vBuffer = (u8*)malloc(mBlockSize);
}

GrapaFileTree::~GrapaFileTree()
{ 
	Close();
	if (vBuffer) free(vBuffer);
	vBuffer = NULL;
}

void GrapaFileTree::SetFile(GrapaFile *pFile)
{
	mBtree.SetFile(pFile);
}

void GrapaFileTree::SetCode(GrapaCHAR& pCode)
{ 
	GrapaBYTE h;  
	GrapaHash::SHAKE256(pCode, h, 32);
	mCode.Init(h,GrapaBYTE(),1); 
}

GrapaError GrapaFileTree::Open(const char *fileName, char mode)
{
	u8 treeType;
	GrapaError err;
	err = mBtree.OpenFile(fileName, mode);
	if (err) return(err);
	mRoot = mBtree.RootTree(treeType);
	u8 compressSize = 0;
	err = mBtree.GetCompressSize(compressSize);
	u32 oldBlockSize = mBlockSize;
	mBlockSize = (u32) (1024 * (u64)compressSize);
	if (mBlockSize == 0) mBlockSize = 1024 * 32;
	if (mBlockSize > oldBlockSize)
	{
		if (vBuffer) free(vBuffer);
		vBuffer = (u8*)malloc(mBlockSize);
	}
	return(err);
}

GrapaError GrapaFileTree::Close()
{
	ClearCache();
	return mBtree.CloseFile();
}

GrapaError GrapaFileTree::Create(const char *fileName)
{
	GrapaError err;
	mRoot = 0;

	err = mBtree.Create(fileName);
	err = mBtree.SetCompressSize(mBlockSize/1024);
	if (err) return(err);

	err = mBtree.NewTree(mRoot, GrapaBtree::SDATA_TREE);

	if (!err)
		err = mBtree.SetFileTree(mRoot);
	if (err)
	{
		mBtree.CloseFile();
		mBtree.RemoveFile(fileName);
		return(err);
	}

	return(0);
}

GrapaError GrapaFileTree::Delete(const char *fileName)
{
	return mBtree.RemoveFile(fileName);
}

GrapaError GrapaFileTree::Flush()
{
	FlushCache();
	return mBtree.FlushFile();
}

GrapaError GrapaFileTree::FlushCache()
{
	if (vBuffer == NULL) return(0);
	GrapaObjectEvent* e = mCache.Head();
	while (e)
	{
		if (e->mDirType == 1)
		{
			e->mDirType = 0;
			GrapaError err;
			u64 retSize;
			GrapaCursor c;
			GrapaBYTE value2;
			GrapaCompress::Compress(e->mValue, value2);
			GrapaBYTE* valuePtr = &e->mValue;
			u8 encodeType = (u8)GrapaBtree::ENCODE_NONE;
			if (e->mValue.mLength < value2.mLength)
			{
				if (mCode.mSet)
				{
					encodeType |= (u8)GrapaBtree::ENCODE_AES;
					mCode.Encrypt(e->mValue, value2);
					valuePtr = &value2;
				}
			}
			else
			{
				encodeType = (u8)GrapaBtree::ENCODE_ZIP;
				valuePtr = &value2;
				if (mCode.mSet)
				{
					encodeType |= (u8)GrapaBtree::ENCODE_AES;
					mCode.Encrypt(value2);
				}
			}
			c.Set(mRoot, GrapaBtree::SDATA_ITEM, e->mId);
			err = mBtree.Search(c);
			if (!err)
			{
				u64 growSize, dataSize, dataLen;
				u8 cType;
				err = mBtree.GetDataSize(c.mValue, growSize, dataSize, dataLen, cType);
				if (dataSize < valuePtr->mLength)
				{
					mBtree.Delete(c);
					err = -1;
				}
			}
			if (err)
			{
				u64 item = 0;
				err = mBtree.NewData(GrapaBtree::BYTE_DATA, mRoot, valuePtr->mLength, 0, 0, item, true);
				c.Set(mRoot, GrapaBtree::SDATA_ITEM, e->mId, item);
				err = mBtree.Insert(c);
				if (err) return(err);
			}
			err = mBtree.SetDataSize(c.mValue, valuePtr->mLength, valuePtr->mLength, encodeType);
			if (err) return(err);
			err = mBtree.SetDataValue(c.mValue, 0, valuePtr->mLength, valuePtr->mBytes, &retSize);
		}
		e = e->Next();
	}
	return(0);
}

GrapaError GrapaFileTree::ClearCache()
{
	FlushCache();
	if (vBuffer) mCache.CLEAR();
	return(0);
}

GrapaError GrapaFileTree::Purge(u64 blockCount, u16 blockSize)
{
	return SetSize(((u64)blockCount)*blockSize);
}

GrapaError GrapaFileTree::GetSize(u64& pSize)
{
	FlushCache();
	pSize = 0;
	GrapaError err;
	GrapaCursor c;
	c.Set(mRoot);
	err = mBtree.Last(c);
	if (err) return(err);
	GrapaObjectEvent* e = GetData(c.mKey);
	u64 dataLength = e->mValue.mLength;
	pSize = c.mKey*mBlockSize + dataLength;
	return(0);
}

GrapaError GrapaFileTree::SetSize(u64 pSize)
{
	ClearCache();
	GrapaError err;
	GrapaCursor c;
	c.Set(mRoot);
	err = mBtree.Last(c);
	while (err==0)
	{
		if ((c.mKey+1)*mBlockSize < pSize) return(0);
		if (c.mKey == (pSize / mBlockSize))
		{
			GrapaObjectEvent* e = GetData(c.mKey);
			e->mValue.SetLength(pSize % mBlockSize,true);
			e->mDirType = 1;
			ClearCache();
			mBtree.PurgeFile();
			return(0);
		}
		err = mBtree.Delete(c);
		err = mBtree.Last(c);
	}
	mBtree.PurgeFile();
	return(0);
}

GrapaObjectEvent* GrapaFileTree::GetData(u64 blockId)
{
	GrapaObjectEvent* e = mCache.Search(blockId);
	if (e) return(e);
	e = new GrapaObjectEvent(blockId, GrapaCHAR(), GrapaCHAR());
	e->mDirType = 0;
	mCache.PushTail(e);
	GrapaError err;
	u64 growSize, dataSize, dataLen, retSize;
	u8 encodeType = 0;
	GrapaCursor c;
	c.Set(mRoot, GrapaBtree::SDATA_ITEM, blockId);
	err = mBtree.Search(c);
	if (err) return(e);
	err = mBtree.GetDataSize(c.mValue, growSize, dataSize, dataLen, encodeType);
	e->mValue.SetLength(dataLen);
	err = mBtree.GetDataValue(c.mValue, 0, dataLen, e->mValue.mBytes, &retSize);
	if (err) return(e);
	e->mValue.SetLength(retSize, true);
	if (encodeType & GrapaBtree::ENCODE_AES)
	{
		if (mCode.mSet)
		{
			mCode.Decrypt(e->mValue);
			// if error, then we need to check to see if we need to check the keychain
		}
		else
		{
			// need to aquire the mCode
			// look in the list of accepted ids, see which ones we have access to in our keychain
			// then decrypt using that one to get the mCode
		}
		// if the above doesn't work, then we have an error condition -> don't have access to the file
	}
	if (encodeType & GrapaBtree::ENCODE_ZIP)
	{
		GrapaCompress::Expand(e->mValue);
	}
	return(e);
}

GrapaError GrapaFileTree::SetData(u64 blockId, GrapaBYTE& value)
{
	GrapaObjectEvent* e = mCache.Search(blockId);
	if (e)
	{
		e->mValue.FROM(value);
		e->mDirType = 1;
	}
	else
	{
		e = new GrapaObjectEvent(blockId, GrapaCHAR(), value);
		e->mDirType = 1;
		mCache.PushTail(e);
	}
	return(0);
}

GrapaError GrapaFileTree::Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *value)
{
	u64 startPos = (((u64)blockPos)*blockSize) + offset;
	u8* valuePtr = (u8*)value;
	u64 len;

	if (startPos%mBlockSize)
	{
		GrapaObjectEvent* e = GetData(startPos / mBlockSize);
		len = mBlockSize - startPos%mBlockSize;
		if (len > length) len = length;
		if (e->mValue.mLength < ((startPos%mBlockSize) + len))
			e->mValue.SetLength(((startPos%mBlockSize) + len), true);
		memcpy(&((u8*)e->mValue.mBytes)[startPos%mBlockSize], valuePtr, (size_t)len);
		e->mDirType = 1;
		valuePtr = &valuePtr[len];
		length -= len;
		startPos += len;
	}

	while (length)
	{
		GrapaObjectEvent* e = GetData(startPos / mBlockSize);
		len = mBlockSize;
		if (len > length) len = length;
		if (e->mValue.mLength < len)
			e->mValue.SetLength(len, true);
		memcpy(e->mValue.mBytes, valuePtr, (size_t)len);
		e->mDirType = 1;
		valuePtr = &valuePtr[len];
		length -= len;
		startPos += len;
	}

	return 0;
}

GrapaError GrapaFileTree::Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *value)
{
	u64 startPos = (((u64)blockPos)*blockSize) + offset;
	u8* valuePtr = (u8*)value;
	u64 len;

	memset(value, 0, (size_t)length);

	if (startPos%mBlockSize)
	{
		GrapaObjectEvent* e = GetData(startPos / mBlockSize);
		len = mBlockSize - startPos%mBlockSize;
		if (len > length) len = length;
		if (e->mValue.mLength < ((startPos%mBlockSize) + len))
		{
			e->mValue.SetLength(((startPos%mBlockSize) + len), true);
			e->mDirType = 1;
		}
		memcpy(valuePtr, &((u8*)e->mValue.mBytes)[startPos%mBlockSize], (size_t)len);
		valuePtr = &valuePtr[len];
		length -= len;
		startPos += len;
	}

	while (length)
	{
		GrapaObjectEvent* e = GetData(startPos / mBlockSize);
		len = mBlockSize;
		if (len > length) len = length;
		if (e->mValue.mLength < len)
		{
			e->mValue.SetLength(len, true);
			e->mDirType = 1;
		}
		memcpy(valuePtr, e->mValue.mBytes, (size_t)len);
		valuePtr = &valuePtr[len];
		length -= len;
		startPos += len;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
