// GrapaFileTree.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaFileTREE_
#define _GrapaFileTREE_

#include "GrapaFile.h"
#include "GrapaBtree.h"
#include "GrapaValue.h"
#include "GrapaFileCache.h"
#include "GrapaHash.h"
#include "GrapaObject.h"
#include "GrapaTinyAES.h"

class GrapaFileTree : public GrapaFile
{
public:
	GrapaFileTree();
	virtual ~GrapaFileTree();
    virtual void    SetFile(GrapaFile *pFile=NULL);
	virtual void SetCode(GrapaCHAR& mCode);
	virtual bool    Opened() { return mBtree.mFile ? mBtree.mFile->Opened() : mOpened; }
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

public:
	virtual GrapaObjectEvent* GetData(u64 blockId);
	virtual GrapaError SetData(u64 blockId, GrapaBYTE& value);
	virtual GrapaError FlushCache();
	virtual GrapaError ClearCache();

protected:
	GrapaBtree mBtree;
	u32 mBlockSize;
	u64 mRoot;
	u8* vBuffer;
	GrapaObjectIndexQueue mCache;
	GrapaTinyAES mCode;
};

#endif //_GrapaFileTREE_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
