// GrapaHash.cpp
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

#include "GrapaMem.h"
#include "GrapaHash.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Keccakreadableandcompact.c"

///////////////////////////////////////////////////////////////////////////////////////////////////

//GrapaHash::GrapaHash()
//{
//	//vInstance = NULL;
//}
//
//GrapaHash::~GrapaHash()
//{
//	//if (vInstance)
//	//	GrapaMem::Delete((void*)vInstance);
//	//vInstance = NULL;
//}

//GrapaError GrapaHash::Hash(const u8* pData, u64 pLen, u8* pEnc, u64 pBytes)
//{
//	FIPS202_SHAKE256(pData, pLen, pEnc, pBytes);
//	return(0);
//}
//
//GrapaError GrapaHash::Hash(const u8* pData, u64 pLen, GrapaBYTE& pEnc, u64 pBytes)
//{
//	pEnc.SetLength(pBytes);
//	FIPS202_SHAKE256(pData, pLen, pEnc.mBytes, pBytes);
//	return(0);
//}

void GrapaHash::SHAKE256(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes)
{
	if (pBytes == 0) pBytes = 64;
	pEnc.SetLength(pBytes);
	FIPS202_SHAKE256(pData.mBytes, (unsigned int)pData.mLength, pEnc.mBytes, (int)pBytes);
}

u32 GrapaHash::SHAKE128_u32(const GrapaBYTE& pData)
{
	u32 pBytes;
	FIPS202_SHAKE128(pData.mBytes, (unsigned int)pData.mLength, (u8*)& pBytes, (int)sizeof(u32));
	return(pBytes);
}

u64 GrapaHash::SHAKE128_u64(const GrapaBYTE& pData)
{
	u64 pBytes;
	FIPS202_SHAKE128(pData.mBytes, (unsigned int)pData.mLength, (u8*)& pBytes, (int)sizeof(u64));
	return(pBytes);
}

void GrapaHash::SHAKE128(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes)
{
	if (pBytes == 0) pBytes = 32;
	pEnc.SetLength(pBytes);
	FIPS202_SHAKE128(pData.mBytes, (unsigned int)pData.mLength, pEnc.mBytes, (int)pBytes);
}

void GrapaHash::SHA3_224(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes)
{
	pEnc.SetLength(28);
	FIPS202_SHA3_224(pData.mBytes, (unsigned int)pData.mLength, pEnc.mBytes);
}

void GrapaHash::SHA3_256(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes)
{
	pEnc.SetLength(32);
	FIPS202_SHA3_256(pData.mBytes, (unsigned int)pData.mLength, pEnc.mBytes);
}

void GrapaHash::SHA3_384(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes)
{
	pEnc.SetLength(48);
	FIPS202_SHA3_384(pData.mBytes, (unsigned int)pData.mLength, pEnc.mBytes);
}

void GrapaHash::SHA3_512(const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pBytes)
{
	pEnc.SetLength(64);
	FIPS202_SHA3_512(pData.mBytes, (unsigned int)pData.mLength, pEnc.mBytes);
}

GrapaError GrapaHash::Hash(const GrapaBYTE& pData, GrapaCHAR& pType, GrapaBYTE& pEnc, u64 pBytes)
{
	if (pType.mLength==0 || pType.StrCmp("SHAKE256") == 0)
	{
		SHAKE256(pData, pEnc, pBytes);
	}
	else if (pType.StrCmp("SHAKE128") == 0)
	{
		SHAKE128(pData, pEnc, pBytes);
	}
	else if (pType.StrCmp("SHA3-224") == 0)
	{
		SHA3_224(pData, pEnc, pBytes);
	}
	else if (pType.StrCmp("SHA3-256") == 0)
	{
		SHA3_256(pData, pEnc, pBytes);
	}
	else if(pType.StrCmp("SHA3-384") == 0)
	{
		SHA3_384(pData, pEnc, pBytes);
	}
	else if (pType.StrCmp("SHA3-512") == 0)
	{
		SHA3_512(pData, pEnc, pBytes);
	}
	return(0);
}

//GrapaError GrapaHash::Start()
//{
//	if (vInstance == NULL)
//		vInstance = (void*)GrapaMem::Create(sizeof(Keccak_SpongeInstance));
//	int result;
//	result = Keccak_SpongeInitialize((Keccak_SpongeInstance*)vInstance, 1024, 576); // these are the recomended defaults
//	if (result != SUCCESS) return(-1);
//	return(0);
//}
//
//GrapaError GrapaHash::Append(const GrapaBYTE& pData)
//{
//	if (vInstance == NULL)
//		return -1;
//	int result;
//	result = Keccak_SpongeAbsorb((Keccak_SpongeInstance*)vInstance, (u8*)pData.mBytes, pData.mLength);
//	if (result != SUCCESS) return(-1);
//	return(0);
//}
//
//GrapaError GrapaHash::Append(const u8* pData, u64 pLen)
//{
//	if (vInstance == NULL)
//		return -1;
//	int result;
//	result = Keccak_SpongeAbsorb((Keccak_SpongeInstance*)vInstance, pData, pLen);
//	if (result != SUCCESS) return(-1);
//	return(0);
//}
//
//GrapaError GrapaHash::Append(u8 pData)
//{
//	if (vInstance == NULL)
//		return -1;
//	int result;
//	result = Keccak_SpongeAbsorbLastFewBits((Keccak_SpongeInstance*)vInstance, pData);
//	if (result != SUCCESS) return(-1);
//	return(0);
//}
//
//GrapaError GrapaHash::End(GrapaBYTE& pEnc, u64 pBytes)
//{
//	if (vInstance == NULL)
//		return -1;
//	int result;
//	pEnc.SetLength(pBytes);
//	result = Keccak_SpongeSqueeze((Keccak_SpongeInstance*)vInstance, (u8*)pEnc.mBytes, pBytes);
//	if (result != SUCCESS) { pEnc.SetLength(0); return(-1); }
//	return(0);
//}