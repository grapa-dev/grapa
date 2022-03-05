// GrapaMem.h
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>

#ifndef _GrapaMem_
#define _GrapaMem_

#if defined(__MINGW32__) || defined(__GNUC__)
#include <pthread.h>
#include <unistd.h>
#else
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#endif

//http://www.ibm.com/developerworks/library/l-port64/index.html

#include "GrapaType.h"

	//http://msdn.microsoft.com/en-us/library/dd302644%28PROT.10%29.aspx
	//http://www.ietf.org/rfc/rfc4122.txt
	//http://support.microsoft.com/kb/36068

//htons()
#define GrapaSwap16(x) ((u16) \
     ((((x) & 0x00FF) << 8) | \
      (((x) & 0xFF00) >> 8)))
#define GrapaSwap32(x) ((u32) \
     ((((x) & 0x000000FF) << 24) | \
      (((x) & 0x0000FF00) <<  8) | \
      (((x) & 0x00FF0000) >>  8) | \
      (((x) & 0xFF000000) >> 24)))
#define GrapaSwap64(x) ((u64) \
     ((((x) & 0x00000000000000FF) << 56) | \
      (((x) & 0x000000000000FF00) << 40) | \
      (((x) & 0x0000000000FF0000) << 24) | \
      (((x) & 0x00000000FF000000) <<  8) | \
      (((x) & 0x000000FF00000000) >>  8) | \
      (((x) & 0x0000FF0000000000) >> 24) | \
      (((x) & 0x00FF000000000000) >> 40) | \
      (((x) & 0xFF00000000000000) >> 56)))
inline static du64 GrapaSwapD64(du64 x) {du64 g=x; g.key = GrapaSwap64(x.key); g.value = GrapaSwap64(x.value); return(g);}

// Store in BigEndian format
static u16 sEndianTest = 0x1234;
static double dEndianTest = -0.0;

#define IsLittleEndianS() (((u8*)&sEndianTest)[0]==0x34)
#define IsLittleEndianD() (((u8*)&dEndianTest)[7]==0x80)
#define BE_SWAPS() (IsLittleEndianS())
#define BE_SWAPD() (IsLittleEndianD())

#define BE_S16(x) BE_SWAPS()?GrapaSwap16((s16)x):(x)
#define BE_S32(x) BE_SWAPS()?GrapaSwap32((s32)x):(x)
#define BE_S64(x) BE_SWAPS()?GrapaSwap64((s64)x):(x)
#define BE_DU64(x) BE_SWAPS()?GrapaSwapD64((du64)x):(x)
#define BE_D64(x) BE_SWAPD()?GrapaSwap64((s64)x):(x)

#if defined(__MINGW32__) || defined(__GNUC__)
#include <sys/time.h>
#include <dirent.h>
#include <termios.h> 
#include <unistd.h> 
#define sscanf_s sscanf
#define sprintf_s(a,b,c,d) sprintf(a,c,d)
#else
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#define mkdir _mkdir
#define rmdir _rmdir
#define chdir _chdir
#endif
#endif

typedef void (*GrapaCB)(void* data);

class GrapaMem
{
public:
	inline static void* Create(u64 byteCount)
	{
		return(malloc((size_t)byteCount));
		//{ return ((void*)new s8[(size_t)byteCount]); }
	}
	inline static void Delete(void* ptr)
	{
		if (ptr) free(ptr);
		//{ if (ptr) delete[](s8*)ptr; }
	}
	inline static u64 Blocks(u64 byteCount, u64 blockSize)
	{
		return (blockSize ? ((byteCount%blockSize) ? (byteCount/blockSize+1) : (byteCount/blockSize)) : byteCount);
	}
	inline static void* MemCopy(void* dst, const void* src, u64 size)
	{
		return(memcpy(dst,src,(size_t)size));
	}
	inline static u64 MoveLeft(void*src, u64 i, u64 n)
	{
		if (i) {
			u64 len = n - i;
			n -= i;
			u64 j = 0;
			while (len--) ((char*)src)[j++] = ((char*)src)[i++];
			((char*)src)[n] = 0;
		}
		return(n);
	}
	inline static void MemCat(void* dst, u64 dsize, const void* src, u64 ssize)
	{
		memcpy(((char*)dst)+dsize,src,(size_t)ssize);
	}
	inline static u32 MemCompare(const void* dst, const void* src, u64 size)
	{
		return(memcmp(dst,src,(size_t)size));
	}
	inline static void* MemFind(void* dst, u8 c, u64 size)
	{
		return(memchr(dst, c, (size_t)size));
	}
	inline static void* MemFind(void* dst2, u64 dsize, void* src, u64 ssize)
	{
		void* dst = dst2;
		void* s2 = 0L;
		while (dsize >= ssize)
		{
			s2 = memchr(dst, ((u8*)src)[0], dsize);
			if (!s2)
				return(0L);
			dsize = dsize - (((u8*)s2) - ((u8*)dst));
			dst = s2;
			if (dsize < ssize)
				return(0L);
			if (memcmp(s2, src, ssize) == 0)
				return(s2);
			dsize = dsize - 1;
			dst = ((u8*)dst) + 1;
		}
		return(0L);
	}
	inline static u64 s2au64(const char* src, u64 dsize)
	{
		u64 num=0LL;
		sscanf_s(src, "%llu", &num);
		return(num);
	}
	inline static s64 s2as64(const char* src)
	{
		s64 num=0LL;
		sscanf_s(src, "%lld", &num);
		return(num);
	}
	inline static char* a2s64(u64 num, char* dst, u64 dsize)
	{
		sprintf_s(dst, (size_t)dsize, "%llu", num);
		return(dst);
	}
	inline static char* a2s64(s64 num, char* dst, u64 dsize)
	{
		sprintf_s(dst, (size_t)dsize, "%lld", num);
		return(dst);
	}
	inline static char* a2s32(s32 num, char *sx, u64 ssize)
	{
		sprintf_s(sx, (size_t)ssize, "%d", num);
		return(sx);
	}
	inline static char* StrCpy(char* dst, u64 dsize, const char* src)
	{
#if defined(__MINGW32__) || defined(__GNUC__)
		return strcpy(dst, src);
#else
#ifdef _WIN32
		strcpy_s(dst, (size_t)dsize, src);
		return(dst);
#endif
#endif
	}
	inline static char* StrCat(char* dst, u64 dsize, const char* src)
	{

#if defined(__MINGW32__) || defined(__GNUC__)
		return strcat(dst, src);
#else
#ifdef _WIN32
		strcat_s(dst, (size_t)dsize, src);
		return(dst);
#endif
#endif
	}
    inline static s64 StrCmp(const char*str1, const char*str2)
    {
        return GrapaMem::StrCmp(str1,strlen(str1),str2,strlen(str2));
    }
    inline static s64 StrCmp(const char*str1, u64 len1, const char*str2)
    {
        return GrapaMem::StrCmp(str1,len1,str2,strlen(str2));
    }
	inline static s64 StrCmp(const char*str1, u64 len1, const char*str2, u64 len2)
	{
		u64 cmpLen = len1 > len2 ? len2 : len1;
		s64 cmp = memcmp(str1, str2, (size_t)cmpLen);
		if (len1 == len2) return(cmp);
		if (cmp == 0) return(len1 - len2);
		return(cmp);
	}
	inline static s64 StrUprCmp(const char*str1, u64 len1, const char*str2, u64 len2)
	{
		u64 cmpLen = len1 > len2 ? len2 : len1;
		char *a = (char*)GrapaMem::Create(cmpLen + 1), *b = (char*)GrapaMem::Create(cmpLen + 1);
		{u64 i = cmpLen; while (i--) a[i] = toupper(str1[i]); } a[cmpLen] = 0;
		{u64 i = cmpLen; while (i--) b[i] = toupper(str2[i]); } b[cmpLen] = 0;
		s64 cmp = memcmp(a, b, (size_t)cmpLen);
		GrapaMem::Delete(a);
		GrapaMem::Delete(b);
		if (len1 == len2) return(cmp);
		if (cmp == 0) return(len1 - len2);
		return(cmp);
	}
	inline static s64 StrLwrCmp(const char*str1, u64 len1, const char*str2, u64 len2)
	{
		u64 cmpLen = len1 > len2 ? len2 : len1;
		char *a = (char*)GrapaMem::Create(cmpLen + 1), *b = (char*)GrapaMem::Create(cmpLen + 1);
		{u64 i = cmpLen; while (i--) a[i] = tolower(str1[i]); } a[cmpLen] = 0;
		{u64 i = cmpLen; while (i--) b[i] = tolower(str2[i]); } b[cmpLen] = 0;
		s64 cmp = memcmp(a, b, (size_t)cmpLen);
		GrapaMem::Delete(a);
		GrapaMem::Delete(b);
		if (len1 == len2) return(cmp);
		if (cmp == 0) return(len1 - len2);
		return(cmp);
	}
	inline static char* CharReplace(char*str, char replaceChar, char replaceWith)
	{
		if (!str) return(str);
		for (u64 i = 0; str[i++];) if (str[i] == replaceChar) str[i] = replaceWith;
		return(str);
	}
	inline static void ToUpper(char*str) { 
		s64 i = strlen(str); while (i--) str[i] = toupper(str[i]); 
	}
	inline static void ToLower(char*str) { 
		s64 i = strlen(str); while (i--) str[i] = tolower(str[i]); 
	}
	inline static void Trim(char*str, char c = ' ') { s64 i = strlen(str); while (i--) { if (str[i] != c) break; str[i] = 0; } }
	//inline static s64 StrNCmp(const char*s1, const char*s2, size_t n)
	//{
	//	unsigned char c1 = '\0';
	//	unsigned char c2 = '\0';

	//	if (n >= 4)
	//	{
	//		size_t n4 = n >> 2;
	//		do
	//		{
	//			c1 = (unsigned char)*s1++;
	//			c2 = (unsigned char)*s2++;
	//			if (c1 == '\0' || c1 != c2)
	//				return c1 - c2;
	//			c1 = (unsigned char)*s1++;
	//			c2 = (unsigned char)*s2++;
	//			if (c1 == '\0' || c1 != c2)
	//				return c1 - c2;
	//			c1 = (unsigned char)*s1++;
	//			c2 = (unsigned char)*s2++;
	//			if (c1 == '\0' || c1 != c2)
	//				return c1 - c2;
	//			c1 = (unsigned char)*s1++;
	//			c2 = (unsigned char)*s2++;
	//			if (c1 == '\0' || c1 != c2)
	//				return c1 - c2;
	//		} while (--n4 > 0);
	//		n &= 3;
	//	}

	//	while (n > 0)
	//	{
	//		c1 = (unsigned char)*s1++;
	//		c2 = (unsigned char)*s2++;
	//		if (c1 == '\0' || c1 != c2)
	//			return c1 - c2;
	//		n--;
	//	}

	//	return c1 - c2;

	//}

	inline static s64 LenToBytes(u64 num, char* str, s64 max)
	{
		s64 countBytes = 0;
		u64 tmax = num;
		u64 trev = 0;
		u32 tcount = 0;
		if (tmax == 0)
		{
			tcount++;
		}
		while (tmax)
		{
			tcount++;
			trev = (trev << 8) | ((tmax & 0x7F) | 0x80);
			tmax = tmax >> 7;
		}
		while (tcount)
		{
			tcount--;
			u8 tc = trev & 0xFF;
			trev = trev >> 8;
			if (tcount == 0) tc = tc & 0x7F;
			str[countBytes] = tc;
			countBytes++;
			if (!tcount && countBytes == max)
				return(-countBytes);
		}
		return(countBytes);
	}

	inline static s64 BytesToLen(char* str, s64 max, s64& len)
	{
		if (max <= 0) return(0);
		s64 countBytes = 0;
		u8 chb = str[countBytes];
		countBytes++;
		len = chb & 0x7F;
		while ((chb & 0x80) == 0x80)
		{
			if (countBytes == max)
			{
				len = -len;
				return(-countBytes);
			}
			chb = str[countBytes];
			countBytes++;
			len = (len << 7) | (chb & 0x7F);
		}
		return(countBytes);
	}
};

#endif //_GrapaMem_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
