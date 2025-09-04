// GrapaFile.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaFile_
#define _GrapaFile_

#include "GrapaType.h"

#define GrapaReadOnly 'r'
#define GrapaWriteOnly 'o'
#define GrapaReadWrite 'w'
#define GrapaReadWriteCreate 'c'

class GrapaFile
{
public:
	GrapaFile() : mOpened(false), mMode(GrapaReadOnly) {}
	virtual bool    Opened() { return mOpened; }
	virtual GrapaError Open(const char *fileName, char mode = GrapaReadOnly ) = 0;
	virtual GrapaError Close() = 0;
	virtual GrapaError GetSize(u64& pSize) = 0;
	virtual GrapaError SetSize(u64 pSize) = 0;
	virtual GrapaError Create(const char *fileName) = 0;
	virtual GrapaError Delete(const char *fileName) = 0;
	virtual GrapaError Flush() = 0;
	virtual GrapaError Purge(u64 blockCount, u16 blockSize) = 0;
	virtual GrapaError Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *b) = 0;
	virtual GrapaError Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *b) = 0;
	virtual GrapaError Append(u64 length, const void *b) = 0;
protected:
	bool mOpened;
	char mMode;
};


#endif //_GrapaFile_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
