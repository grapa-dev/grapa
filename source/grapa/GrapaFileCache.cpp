// GrapaFileCache.cpp
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

#include "GrapaFileCache.h"
#include "GrapaMem.h"

#include <string.h>

class GrapaCacheBlock
{
public:
	u64 key;
	u64 prev,next;
	u8 dirty,inuse;
	u8 value[GrapaFileCache::BLOCKPAGESIZE];
};

GrapaFileCache::GrapaFileCache()
{
	mLocalyAllocated = false;
	mCacheWidth = 0;
	mCacheHeight = 0;
	mCache = NULL;
	mFirstCache = NULL;
	mFile = NULL;
}

void GrapaFileCache::SetFile(GrapaFile *pFile)
{
	Close();
	mFile = pFile;
}

GrapaError GrapaFileCache::SetCache(u64 pSize, void *pCache)
{
	u64 sizeCalc;

	FreeCache();

	if (pSize < sizeof(GrapaCacheBlock))
		return(0);

	mCacheHeight = DEFAULT_HEIGHT;
	mCacheWidth = DEFAULT_SIZE / (mCacheHeight * sizeof(GrapaCacheBlock) + sizeof(u64));

	if (pCache)
	{
		sizeCalc = pSize;

		mCacheHeight = DEFAULT_HEIGHT;
		mCacheWidth = sizeCalc / (mCacheHeight * sizeof(GrapaCacheBlock) + sizeof(u64));

		if (mCacheWidth <= 4)
		{
			mCacheHeight = sizeCalc / sizeof(GrapaCacheBlock) / 4;
			mCacheWidth = sizeCalc / (mCacheHeight * sizeof(GrapaCacheBlock) + sizeof(u64));
		}

		if (mCacheWidth == 0)
			mCacheWidth = 1;

		sizeCalc = pSize - mCacheWidth * sizeof(u64);
		mCacheHeight = ((sizeCalc / mCacheWidth) - sizeof(u64)) / sizeof(GrapaCacheBlock);

		if (mCacheWidth==0 || mCacheHeight==0)
		{
			mCacheWidth = 0;
			mCacheHeight = 0;
		}
	}

	if (!pCache)
	{
		sizeCalc = (mCacheHeight * sizeof(GrapaCacheBlock) + sizeof(u64)) * mCacheWidth;
		pCache = (GrapaCacheBlock*) GrapaMem::Create(sizeCalc);
		if (pCache == NULL)
		{
			mCacheWidth = 0;
			mCacheHeight = 0;
			return((GrapaError)-1);
		}
		mLocalyAllocated = true;
	}

	mCache = (GrapaCacheBlock*)pCache;
	mFirstCache = (u64*) (((s8*)mCache) + sizeof(GrapaCacheBlock) * mCacheWidth*mCacheHeight);

	ClearCache();

	return(0);
}

GrapaError GrapaFileCache::Open(const char *fileName, char mode)
{
	if (!mFile) return((GrapaError)-1);
	return mFile->Open(fileName,mode);
}

GrapaError GrapaFileCache::Close()
{
	Flush();
	if (!mFile) return((GrapaError)-1);
	return mFile->Close();
}

GrapaError GrapaFileCache::GetSize(u64& pSize)
{
	if (!mFile) return((GrapaError)-1);
	return mFile->GetSize(pSize);
}

GrapaError GrapaFileCache::SetSize(u64 pSize)
{
	if (!mFile) return((GrapaError)-1);
	return mFile->SetSize(pSize);
}

GrapaError GrapaFileCache::Create(const char *fileName)
{
	if (!mFile) return((GrapaError)-1);
	return mFile->Create(fileName);
}

GrapaError GrapaFileCache::Delete(const char *fileName)
{
	if (!mFile) return((GrapaError)-1);
	return mFile->Delete(fileName);
}

GrapaError GrapaFileCache::Flush()
{
	return FlushCache();
}

GrapaError GrapaFileCache::Purge(u64 blockCount, u16 blockSize)
{
	GrapaError err;
	if (!mFile) return((GrapaError)-1);
	err = mFile->Purge(blockCount,blockSize);
	if (err) return(err);
	return(0);
}

GrapaError GrapaFileCache::Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *value)
{
	return SetCache(blockPos, blockSize, offset, value, length, true);
}

GrapaError GrapaFileCache::Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *value)
{
	return GetCache(blockPos, blockSize, offset, value, length);
}

////////////////////////////////////////////////////////////////////////////////

void GrapaFileCache::FreeCache()
{
	Close();
	if (mLocalyAllocated && mCache) GrapaMem::Delete(mCache);
	mLocalyAllocated = false;
	mCacheWidth = 0;
	mCacheHeight = 0;
	mCache = NULL;
	mFirstCache = NULL;
}

GrapaError GrapaFileCache::ClearCache()
{
	u64 width,height;
	if (mCache == 0LL) return(-1);
	for (width=0;width<mCacheWidth;width++)
	{
		GrapaCacheBlock *cache = &mCache[width*mCacheHeight];
		for (height=0;height<mCacheHeight;height++)
		{
			cache[height].key = 0;
			cache[height].dirty = false;
			cache[height].inuse = false;
			cache[height].next = height + 1;
			cache[height].prev = height - 1;
		}
		cache[mCacheHeight-1].next = 0;
		cache[0].prev = mCacheHeight-1;
		mFirstCache[width] = 0;
	}
	return(0);
}

u64 GrapaFileCache::FindCache(GrapaCacheBlock *cache, u64 key, u64 first)
{
	u64 ptr = first;
	do
	{
		if (!cache[ptr].inuse)
			break;
		if (cache[ptr].key == key)
			return ptr;
		ptr = cache[ptr].next;
	} while (ptr != first);
	return -1;
}

u64 GrapaFileCache::FindCacheFree(GrapaCacheBlock *cache, u64 first)
{
	u64 ptr = first;
	do
	{
		ptr = cache[ptr].prev;
		if (!cache[ptr].inuse || !cache[ptr].dirty)
			return ptr;
	} while (ptr != first);
	return -1;
}

GrapaError GrapaFileCache::GetCacheBlock(u64 blockPos, void* value)
{
	GrapaError err;

	if (mCacheWidth)
	{
		u64 width = blockPos%mCacheWidth;
		u64 first = mFirstCache[width];

		GrapaCacheBlock *cache = &mCache[width*mCacheHeight];

		u64 ptr = FindCache(cache,blockPos,first);

		if (ptr != -1)
		{
			GrapaMem::MemCopy(value,&cache[ptr].value,GrapaFileCache::BLOCKPAGESIZE);
			if (ptr != first)
			{
				mFirstCache[width] = ptr;
				cache[cache[ptr].prev].next = cache[ptr].next;
				cache[cache[ptr].next].prev = cache[ptr].prev;
				cache[ptr].prev = cache[first].prev;
				cache[ptr].next = first;
				cache[cache[first].prev].next = ptr;
				cache[first].prev = ptr;
			}
			return(0);
		}
	}

	if (!mFile) return((GrapaError)-1);

	err = mFile->Read(blockPos,GrapaFileCache::BLOCKPAGESIZE,0,GrapaFileCache::BLOCKPAGESIZE,value);
	if (err) return(err);

	err = SetCacheBlock(blockPos,value,false);
	//if (err) return(err);

	return(0);
}

GrapaError GrapaFileCache::SetCacheBlock(u64 blockPos, const void* value, bool dirty)
{
	GrapaError err = 0;

	if (!mCacheWidth)
	{
		if (dirty)
		{
			if (mFile == NULL) return((GrapaError)-1);
			err = mFile->Write(blockPos,GrapaFileCache::BLOCKPAGESIZE,0,GrapaFileCache::BLOCKPAGESIZE,value);
			if (err) return(err);
			return(0);
		}
	}

	if (!mCacheWidth) return(0);

	u64 width = blockPos%mCacheWidth;
	u64 first = mFirstCache[width];

	GrapaCacheBlock *cache = &mCache[width*mCacheHeight];

	u64 ptr = FindCache(cache,blockPos,first);

	if (ptr == -1)
	{
		ptr = FindCacheFree(cache,first);
	}

	if (ptr != -1)
	{
		if (ptr != first)
		{
			cache[cache[ptr].prev].next = cache[ptr].next;
			cache[cache[ptr].next].prev = cache[ptr].prev;
			cache[ptr].prev = cache[first].prev;
			cache[ptr].next = first;
			cache[cache[first].prev].next = ptr;
			cache[first].prev = ptr;
			first = mFirstCache[width] = ptr;
		}
		if (cache[first].dirty)
			dirty = true;
	}
	else
	{
		first = mFirstCache[width] = cache[first].prev;
		if (cache[first].inuse && cache[first].dirty)
		{
			if (mFile == NULL) return((GrapaError)-1);
			err = mFile->Write(cache[first].key,GrapaFileCache::BLOCKPAGESIZE,0,GrapaFileCache::BLOCKPAGESIZE,&cache[first].value);
			if (err) return(err);
			cache[first].dirty = false;
		}
	}

	cache[first].key = blockPos;
	GrapaMem::MemCopy(&cache[first].value,value,GrapaFileCache::BLOCKPAGESIZE);
	cache[first].dirty = dirty;
	cache[first].inuse = true;

	return(0);
}

GrapaError GrapaFileCache::GetCache(u64 blockPosX, u16 blockSizeX, u64 offsetX, void* value, u64 length)
{
	u8 valueBlock[GrapaFileCache::BLOCKPAGESIZE];
	u64 valueLength;
	u64 blockPos = (blockPosX * blockSizeX) / GrapaFileCache::BLOCKPAGESIZE;
	u64 offset = offsetX + (blockPosX * blockSizeX) % GrapaFileCache::BLOCKPAGESIZE;

	if (offset > GrapaFileCache::BLOCKPAGESIZE)
	{
		u64 jumpBlock = offset/GrapaFileCache::BLOCKPAGESIZE;
		blockPos += jumpBlock;
		offset -= jumpBlock*GrapaFileCache::BLOCKPAGESIZE;
	}

	if (offset)
	{
		GetCacheBlock(blockPos,&valueBlock);
		//above will error if reading past the end of the file
		valueLength = (length < (GrapaFileCache::BLOCKPAGESIZE-offset)) ? length : GrapaFileCache::BLOCKPAGESIZE-offset;
		GrapaMem::MemCopy(value,&valueBlock[offset],valueLength);
		value = &((s8*)value)[valueLength];
		length -= valueLength;
		blockPos++;
	}

	while (length)
	{
		if (length >= GrapaFileCache::BLOCKPAGESIZE)
		{
			GetCacheBlock(blockPos,value);
			value = &((s8*)value)[GrapaFileCache::BLOCKPAGESIZE];
			length -= GrapaFileCache::BLOCKPAGESIZE;
		}
		else
		{
			GetCacheBlock(blockPos,&valueBlock);
			GrapaMem::MemCopy(value,&valueBlock,length);
			value = &((s8*)value)[length];
			length = 0;
		}
		blockPos++;
	}

	return(0);
}

GrapaError GrapaFileCache::SetCache(u64 blockPosX, u16 blockSizeX, u64 offsetX, const void* value, u64 length, bool dirty)
{
	u8 valueBlock[GrapaFileCache::BLOCKPAGESIZE];
	u64 valueLength;
	u64 blockPos = (blockPosX * blockSizeX) / GrapaFileCache::BLOCKPAGESIZE;
	u64 offset = offsetX + (blockPosX * blockSizeX) % GrapaFileCache::BLOCKPAGESIZE;

	if (offset > GrapaFileCache::BLOCKPAGESIZE)
	{
		u64 jumpBlock = offset/GrapaFileCache::BLOCKPAGESIZE;
		blockPos += jumpBlock;
		offset -= jumpBlock*GrapaFileCache::BLOCKPAGESIZE;
	}

	if (offset)
	{
		GetCacheBlock(blockPos,&valueBlock);
		//above will error if reading past the end of the file
		valueLength = (length < (GrapaFileCache::BLOCKPAGESIZE-offset)) ? length : GrapaFileCache::BLOCKPAGESIZE-offset;
		GrapaMem::MemCopy(&valueBlock[offset],value,valueLength);
		value = &((s8*)value)[valueLength];
		length -= valueLength;
		SetCacheBlock(blockPos,&valueBlock,dirty);
		blockPos++;
	}

	while (length)
	{
		if (length >= GrapaFileCache::BLOCKPAGESIZE)
		{
			SetCacheBlock(blockPos,value,dirty);
			value = &((s8*)value)[GrapaFileCache::BLOCKPAGESIZE];
			length -= GrapaFileCache::BLOCKPAGESIZE;
		}
		else
		{
			GetCacheBlock(blockPos,&valueBlock);
			GrapaMem::MemCopy(&valueBlock,value,length);
			value = &((s8*)value)[length];
			length = 0;
			SetCacheBlock(blockPos,&valueBlock,dirty);
		}
		blockPos++;
	}

	return(0);
}

GrapaError GrapaFileCache::FlushCache()
{
	GrapaError err = 0;
	u64 width;

	if (!mFile)
		return((GrapaError)-1);

	for (width=0;width<mCacheWidth;width++)
	{
		u64 first = mFirstCache[width];
		GrapaCacheBlock *cache = &mCache[width*mCacheHeight];
		u64 ptr = first;
		do
		{
			if (!cache[ptr].inuse)
				break;
			if (cache[ptr].dirty)
			{
				err = mFile->Write(cache[ptr].key,GrapaFileCache::BLOCKPAGESIZE,0,GrapaFileCache::BLOCKPAGESIZE,&cache[ptr].value);
				if (err) return(err);
				cache[ptr].dirty = false;
			}
			ptr = cache[ptr].next;
		} while (ptr != first);
	}
	ClearCache();
	return mFile->Flush();
}

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
