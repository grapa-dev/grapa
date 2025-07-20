// GrapaFileCache.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaFileCACHE_
#define _GrapaFileCACHE_

#include "GrapaFile.h"
#include "GrapaBtree.h"

class GrapaCacheBlock;

class GrapaFileCache : public GrapaFile
{
public:
	enum { BLOCKSIZE=GrapaBlock::BLOCKSIZE, BLOCKSPERPAGE=8, BLOCKPAGESIZE=BLOCKSIZE*BLOCKSPERPAGE, };
	enum { DEFAULT_HEIGHT=4, DEFAULT_SIZE=(1024*8*4)};
public:
    GrapaFileCache();
	virtual ~GrapaFileCache() { Close(); FreeCache();}
    virtual void    SetFile(GrapaFile *pFile=NULL);
    virtual GrapaError SetCache(u64 pSize=DEFAULT_SIZE, void *pCache=NULL);
	virtual bool    Opened() { return mFile?mFile->Opened():mOpened; }
	virtual GrapaError Open(const char *fileName, char mode = GrapaReadOnly);
	virtual GrapaError Close();
	virtual GrapaError	GetSize(u64& pSize);
	virtual GrapaError	SetSize(u64 pSize);
	virtual GrapaError Create(const char *fileName);
	virtual GrapaError Delete(const char *fileName);
	virtual GrapaError Flush();
	virtual GrapaError Purge(u64 blockCount, u16 blockSize);
	virtual GrapaError Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *b);
	virtual GrapaError Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *b);
	virtual GrapaError Append(u64 length, const void *b) {return(0);}

protected:
	virtual GrapaError ClearCache();
	virtual GrapaError GetCacheBlock(u64 blockPos, void* value);
	virtual GrapaError SetCacheBlock(u64 blockPos, const void* value, bool dirty);
	virtual GrapaError FlushCache();

protected:
	virtual void    FreeCache();
	virtual GrapaError GetCache(u64 blockPos, u16 blockSize, u64 offset, void* value, u64 length);
	virtual GrapaError SetCache(u64 blockPos, u16 blockSize, u64 offset, const void* value, u64 length, bool dirty);

private:
	u64 FindCache(GrapaCacheBlock *cache, u64 key, u64 first);
	u64 FindCacheFree(GrapaCacheBlock *cache, u64 first);

protected:
	GrapaFile *mFile;

protected:
	u64 mCacheWidth,mCacheHeight;
	GrapaCacheBlock *mCache;
	u64* mFirstCache;
	bool mLocalyAllocated;

};

#endif //_GrapaFileCACHE_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
