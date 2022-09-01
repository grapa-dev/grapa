// GrapaFileIO.cpp
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

#define _LARGEFILE64_SOURCE 1
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#ifndef O_LARGEFILE
#define O_LARGEFILE 0400000
#endif

#if defined(__MINGW32__) || defined(__GNUC__)
#include <unistd.h>
#define oflaginit 0
#define ftruncate64 ftruncate
#else
#ifdef _WIN32
#include <io.h>
#define lseek _lseeki64
#define ftruncate64 _chsize_s
#define oflaginit (O_BINARY)
#define close _close
#define write _write
#define read _read
#define sched_yield _Thrd_yield
#endif
#endif

#include "GrapaFileIO.h"

//#include <share.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#if defined(__MINGW32__) || defined(__GNUC__)
#include <sys/ioctl.h>
#endif

//http://stackoverflow.com/questions/22372316/how-to-get-usb-connected-hard-disk-serial-in-linux
//http://stackoverflow.com/questions/20291022/ioctl-and-hdreg-to-get-information-on-harddrives

GrapaError GrapaFileIO::Open(const char *fileName, char mode)
{
	if (mode == GrapaReadWriteCreate) return Create(fileName);
	int oflag = oflaginit;
	int pmode = 0;
	int fRef = -1;
#ifdef _WIN32
	int shr = _SH_SECURE;
	if (mode == GrapaReadOnly)
		shr = _SH_DENYWR;
#endif
	if (mode == GrapaReadWrite)
	{
		oflag |= O_RDWR;
		pmode = S_IREAD | S_IWRITE;
	}
	else if (mode == GrapaReadWriteCreate)
	{
		oflag |= O_RDWR;
		pmode = S_IREAD | S_IWRITE;
	}
	else if (mode == GrapaWriteOnly)
	{
		oflag |= O_WRONLY;
		pmode = S_IWRITE;
	}
	else
	{
		oflag |= O_RDONLY;
		pmode = S_IREAD;	
	}
	
	Close();

#if defined(__MINGW32__) || defined(__GNUC__)
	fRef = open(fileName, oflag, pmode);
#else
#ifdef _WIN32
	GrapaError err = _sopen_s(&fRef, fileName, oflag, shr, pmode);
	if (err != 0) return((GrapaError)-1);
#endif
#endif
	if (fRef == -1) return((GrapaError)-1);
	mFp = fRef;
	mOpened = true;
	mMode = mode;
	return(0);
}

GrapaError GrapaFileIO::Close()
{
	if (!Opened()) return(0);
	close(mFp);
	mFp = -1;
	mOpened = false;
	mMode = GrapaReadOnly;
	return(0);
}

GrapaError GrapaFileIO::GetSize(u64& pSize)
{
	if (!Opened()) return(-1);
	pSize = lseek(mFp, 0LL, SEEK_END);
#if defined(__MINGW32__) || defined(__GNUC__)
	if (pSize == -1)
	{
		int backUpSize = 0;
		int err = ioctl(mFp, FIONREAD, &backUpSize);
		pSize = backUpSize;
		if (err != 0) return(-1);
	}
#endif
	return(0);
}

GrapaError GrapaFileIO::SetSize(u64 pSize)
{
	GrapaError err;
	u64 fileSize;
	if (!Opened()) return((GrapaError)-1);
	err = ftruncate64(mFp, pSize);
	if (err) return((GrapaError)-1);
	err = GetSize(fileSize);
	if (err)
		return((GrapaError)-1);
	if (fileSize != pSize)
		return((GrapaError)-1);
	return(0);
}

GrapaError GrapaFileIO::Create(const char *fileName)
{
	//mFp = fopen(fileName, "wb+");
	//if (mFp == NULL) return(-1);
	int oflag = oflaginit | O_RDWR | O_TRUNC | O_CREAT;
	//int shflag = _SH_DENYRW;
	int pmode = S_IREAD | S_IWRITE;
	int fRef = -1;
	Close();
#if defined(__MINGW32__) || defined(__GNUC__)
	fRef = open(fileName, oflag, pmode);
#else
#ifdef _WIN32
	GrapaError err = _sopen_s(&fRef, fileName, oflag, _SH_DENYRW, pmode);
	if (err != 0) return((GrapaError)-1);
#endif
#endif
	if (fRef == -1) return((GrapaError)-1);
	mFp = fRef;
	mOpened = true;
	mMode = GrapaReadWriteCreate;
	return(0);
}

GrapaError GrapaFileIO::Delete(const char *fileName)
{
	GrapaError err;
	err = remove(fileName);
	if (err != 0)
		return((GrapaError)-1);
	return(0);
}

GrapaError GrapaFileIO::Flush()
{
	//errno_t err;
	if (!Opened()) return(0);
	if (mMode == GrapaReadOnly) return(0);
	//err = _commit(mFp);
	//if (err) return((GrapaError)-1);
	return(0);
}

GrapaError GrapaFileIO::Purge(u64 blockCount, u16 blockSize)
{
	GrapaError err;
	if (!Opened()) return((GrapaError)-1);
	err = SetSize(((u64)blockCount)*blockSize);
	if (err)
		return((GrapaError)-1);
	return(0);
}

GrapaError GrapaFileIO::Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *b)
{
	GrapaError err;
	u64 len = 0, len2 = 0;
	u64 endPos = 0, total = 0;
	u64 fileSize = 0L;

	if (!Opened()) return((GrapaError)-1);
	if (length == 0) return(0);
	err = GetSize(fileSize);
	if (err)
		return((GrapaError)-1);
	if (fileSize<(blockPos*blockSize + offset + length))
	{
		err = SetSize(blockPos*blockSize + offset + length);
		if (err)
			return((GrapaError)-1);
	}
	endPos = lseek(mFp, (blockPos*blockSize + offset), SEEK_SET);
	if (endPos != (blockPos*blockSize + offset))
		return((GrapaError)-1);

	while (length)
	{
		len = length < 131072 ? length : 131072;
		len2 = write(mFp, &(((char*)b)[total]), (u32)len);
		if (len2 < 0)
			return((GrapaError)-1);
		if (len != len2)
			return((GrapaError)-1);
		length -= len2;
		total += len2;
		//std::this_thread::sleep_for(std::chrono::milliseconds(0));
		sched_yield();
	}
	if (length != 0)
		return((GrapaError)-1);
	return 0;
}

GrapaError GrapaFileIO::Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *b)
{
	u64 len=0,len2=0;
	u64 endPos = 0, total = 0;
	if (!Opened()) return((GrapaError)-1);
	endPos = lseek(mFp, (blockPos * blockSize + offset), SEEK_SET);
	if (endPos != (blockPos * blockSize + offset))
		return((GrapaError)-1);
	while (length)
	{
		len = length < 131072 ? length : 131072;
		len2 = read(mFp, &(((char*)b)[total]), (u32)len);
		if (len2 < 0)
			return((GrapaError)-1);
		if (len != len2)
			return((GrapaError)-1);
		length -= len2;
		total += len2;
		//std::this_thread::sleep_for(std::chrono::milliseconds(0));
		sched_yield();
	}
	if (length!=0)
		return((GrapaError)-1);
	return(0);
}

GrapaError GrapaFileIO::Append(u64 length, const void *b)
{
	u64 endPos;
	GrapaError err;
	if (!Opened()) return((GrapaError)-1);
	err = GetSize(endPos);
	if (err) return(err);
	err = SetSize(endPos + length);
	if (err) return(err);
	err = Write(0, 0, endPos, length, b);
	if (err) return(err);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
//	08-Aug-12	cmatichuk	Updated to use _sopen_s and _chsize_s
