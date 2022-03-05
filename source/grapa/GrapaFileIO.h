// GrapaFileIO.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaFileIO_
#define _GrapaFileIO_

#include "GrapaFile.h"

class GrapaFileIO : public GrapaFile
{
public:
	int mFp;
public:
	GrapaFileIO() : mFp(-1) {}
	virtual ~GrapaFileIO() { Close(); }
	virtual GrapaError Open(const char *fileName, char mode = GrapaReadOnly);
	virtual GrapaError Close();
	virtual GrapaError GetSize(u64& pSize);
	virtual GrapaError SetSize(u64 pSize);
	virtual GrapaError Create(const char *fileName);
	virtual GrapaError Delete(const char *fileName);
	virtual GrapaError Flush();
	virtual GrapaError Purge(u64 blockCount, u16 blockSize);
	virtual GrapaError Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *b);
	virtual GrapaError Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *b);
	virtual GrapaError Append(u64 length, const void *b);
};

#endif //_GrapaFileIO_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
