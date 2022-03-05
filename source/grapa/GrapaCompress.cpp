// GrapaCompress.cpp
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
#include "GrapaCompress.h"
#define MINIZ_EXPORT
#include "miniz-master/miniz.c"
#include "miniz-master/miniz_zip.c"
#include "miniz-master/miniz_tdef.c"
#include "miniz-master/miniz_tinfl.c"

#define ZIP_BLOCK_SIZE 4096

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GrapaCompress::Compress(GrapaBYTE& pData)
{
	GrapaBYTE pEnc;
	bool r = Compress(pData, pEnc);
	pData.FROM(pEnc);
	return r;
}

bool GrapaCompress::Expand(GrapaBYTE& pEnc)
{
	GrapaBYTE pData;
	bool r = Expand(pEnc, pData);
	pEnc.FROM(pData);
	return r;
}

bool GrapaCompress::Compress(GrapaBYTE& pData, GrapaBYTE& pEnc)
{
	pEnc.SetLength(0);
	if (pData.mLength == 0 || pData.mBytes == NULL)  return false;
	mz_ulong maxSize = (mz_ulong)MZ_MAX(128 + (pData.mLength * 110) / 100, 128 + pData.mLength + ((pData.mLength / (31 * 1024)) + 1) * 5) + sizeof(u64) + 1;
	pEnc.SetLength(maxSize + sizeof(s64) + 6,false);
	if (pEnc.mBytes == NULL)
	{
		pEnc.SetLength(0);
		return false;
	}
	s64 countBytes2 = GrapaMem::LenToBytes(pData.mLength, (char*)&pEnc.mBytes[4], sizeof(s64) + 2);
	if (countBytes2 < 0) countBytes2 = 0;
	u8 tk = pData.ToDbType();
	if (tk == GrapaTokenType::RAW) tk = 0;
	pEnc.mBytes[0] = (u8)'G';
	pEnc.mBytes[1] = (u8)'R';
	pEnc.mBytes[2] = (u8)'Z';
	pEnc.mBytes[3] = tk;
    int status = mz_compress(&(((u8*)pEnc.mBytes)[countBytes2 + 4]), &maxSize, (u8*)pData.mBytes, (mz_ulong)pData.mLength);
	pEnc.SetLength(maxSize + countBytes2 + 4, true);
	return true;
}

bool GrapaCompress::Expand(GrapaBYTE& pEnc, GrapaBYTE& pData)
{
	pData.SetLength(0);
	if (pEnc.mBytes[0] == 0)
	{
		if (pEnc.mLength < (sizeof(u64) + 1) || pEnc.mBytes == NULL) return false;
		u64 expandSize = BE_S64((((u64*)pEnc.mBytes)[0]));
		pData.FromDbType(((u8*)pEnc.mBytes)[sizeof(u64)]);
		if (expandSize == 0) return false;
		pData.SetLength(expandSize, false);
		mz_ulong pDest_len = (mz_ulong)expandSize;
		int status = 0;
		status = mz_uncompress(pData.mBytes, &pDest_len, &(((u8*)pEnc.mBytes)[sizeof(u64) + 1]), (mz_ulong)pEnc.mLength - sizeof(u64) - 1);
		return true;
	}
	else if (pEnc.mLength > 4 && pEnc.mBytes[0] == 'G' && pEnc.mBytes[1] == 'R' && pEnc.mBytes[2] == 'Z' && (pEnc.mBytes[3] & 0x80) == 0)
	{
		pData.FromDbType(((u8*)pEnc.mBytes)[3]);
		s64 expandSize = 0;
		s64 countBytes = GrapaMem::BytesToLen((char*)&pEnc.mBytes[4], pEnc.mLength, expandSize);
		if (countBytes <= 0) countBytes = 0;
		pData.SetLength(expandSize, false);
		if (expandSize == 0) return true;
		mz_ulong pDest_len = (mz_ulong)expandSize;
		int status = 0;
		status = mz_uncompress(pData.mBytes, &pDest_len, &(((u8*)pEnc.mBytes)[4 + countBytes]), (mz_ulong)pEnc.mLength - 4 - countBytes);
		return true;
	}
	return false;
}
