// GrapaSHA3.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaHash_
#define _GrapaHash_

#include "GrapaValue.h"
#include "GrapaInt.h"

class GrapaHash
{
public:
	//GrapaHash();
	//virtual ~GrapaHash(); 
	//static GrapaError Hash(const u8* pData, u64 pLen, u8* pEnc, u64 pBytes);
	//static GrapaError Hash(const u8* pData, u64 pLen, GrapaBYTE& pEnc, u64 pBytes);
	static void SHAKE256(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes = 0);
	static u32 SHAKE128_u32(const GrapaBYTE& pData);
	static u64 SHAKE128_u64(const GrapaBYTE& pData);
	static void SHAKE128(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes = 0);
	static void SHA3_224(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes = 0);
	static void SHA3_256(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes = 0);
	static void SHA3_384(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes = 0);
	static void SHA3_512(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes = 0);
	static GrapaError Hash(const GrapaBYTE& pData, GrapaCHAR& pType, GrapaBYTE& pEnc, u64 pBytes = 16);
	//GrapaError Start();
	//GrapaError Append(const GrapaBYTE& pData);
	//GrapaError Append(const u8* pData, u64 pLen);
	//GrapaError Append(u8 pData);
	//GrapaError End(GrapaBYTE& pEnc, u64 pBytes);
//private:
	//void* vInstance;
};


#endif //_GrapaHash_

////////////////////////////////////////////////////////////////////////////////
