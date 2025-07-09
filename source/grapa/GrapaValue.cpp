// GrapaValue.cpp 
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

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "GrapaValue.h"
#include "GrapaDB.h"
#include "GrapaInt.h"
#include "GrapaTime.h"
#include "GrapaFloat.h"
#include "GrapaMem.h"
#include "GrapaSystem.h"

extern GrapaSystem* gSystem;

////////////////////////////////////////////////////////////////////////////////

GrapaBYTE::GrapaBYTE() { mBytes = NULL;  INIT(); };
GrapaBYTE::GrapaBYTE(u64 pSize, const void *pValue) { mBytes = NULL; INIT(); if (pValue) FROM(pSize, pValue); else SetSize(pSize); };
GrapaBYTE::GrapaBYTE(const void *pValue, u64 pSize) { mBytes = NULL; INIT(); FROM(pSize, pValue); };
GrapaBYTE::GrapaBYTE(const char *pStr) { mBytes = NULL; INIT(); if (pStr) FROM(strlen(pStr), pStr); };

GrapaBYTE::GrapaBYTE(const GrapaBYTE& pBytes)
{
	mBytes = NULL;
	INIT();
	FROM(pBytes);
}

GrapaBYTE::~GrapaBYTE() 
{ 
	if (mBytes) 
		GrapaMem::Delete(mBytes); 
	mBytes = NULL;
}

GrapaBYTE& GrapaBYTE::operator =(const GrapaBYTE& that)
{
	if (this != &that)
	{
		INIT();
		FROM((GrapaBYTE&)that);
	}
	return *this;
}

GrapaBYTE& GrapaBYTE::operator =(const GrapaCHAR& that)
{
	if (this != &that)
	{
		INIT();
		FROM((GrapaBYTE&)that);
	}
	return *this;
}

void GrapaBYTE::INIT() 
{
	if (mBytes)
	{
		if (mBytes) 
			GrapaMem::Delete(mBytes);
		mBytes = NULL;
	}
	mSize = 0; 
	mLength = 0; 
	mPos = 0; 
	mGrow = 32; 
	mToken = GrapaTokenType::RAW;
}
void* GrapaBYTE::GetPtr() { return(mBytes); }
u64 GrapaBYTE::GetLength() { return(mLength); }
void GrapaBYTE::SetLength(u64 pLength, bool pCopy) { GrowSize(pLength + 1, pCopy); mLength = pLength; mBytes[mLength] = 0; }
u64 GrapaBYTE::GetSize() { return(mSize); }
void GrapaBYTE::GrowSize(u64 pSize, bool pCopy) { if (pSize > mSize) SetSize(((pSize / mGrow) + 1)* mGrow, pCopy); }

void GrapaBYTE::FROM(const char* pStr)
{
	if (pStr) FROM((u64)strlen(pStr), pStr);
	else SetLength(0);
}

void GrapaBYTE::FROM(u64 pSize, const void *pValue)
{
	mToken = GrapaTokenType::RAW;
	SetSize(pSize, false);
	if (mBytes && pValue) GrapaMem::MemCopy(mBytes,pValue,pSize);
	mLength = pSize;
}

void GrapaBYTE::FROM(const GrapaBYTE& pValue)
{
	if (pValue.mLength && pValue.mBytes)
	{
		SetLength(pValue.mLength, false);
		GrapaMem::MemCopy(mBytes, pValue.mBytes, pValue.mLength);
	}
	else
		SetLength(0);
	mToken = pValue.mToken;
}

void GrapaBYTE::ToHex(GrapaBYTE& pValue)
{
	pValue.SetLength(0);
	if (mLength == 0) return;
	u64 i;
	u8 *map = (u8*)"0123456789ABCDEF";
	u8 skipback = 0;
	if ((((u8*)mBytes)[0] >> 4) == 0) skipback = 1;
	switch (mToken)
	{
	case GrapaTokenType::STR:
	case GrapaTokenType::TABLE:
	case GrapaTokenType::RAW:
	case GrapaTokenType::INT:
	case GrapaTokenType::FLOAT:
	case GrapaTokenType::SYSINT:
		pValue.SetLength(mLength * 2 - skipback);
		if (skipback==0) ((u8*)pValue.mBytes)[0] = map[((u8*)mBytes)[0] >> 4];
		((u8*)pValue.mBytes)[1 - skipback] = map[((u8*)mBytes)[0] & 0xF];
		for (i = 1; i < mLength; i++)
		{
			((u8*)pValue.mBytes)[i * 2 - skipback] = map[((u8*)mBytes)[i] >> 4];
			((u8*)pValue.mBytes)[i * 2 + 1 - skipback] = map[((u8*)mBytes)[i]&0xF];
		}
		pValue.mToken = GrapaTokenType::STR;
		break;
    default: break;
	}
}

void GrapaBYTE::FromHex(GrapaBYTE& pValue)
{
	u8 *map1 = (u8*)"0123456789ABCDEF";
	u8 *map2 = (u8*)"0123456789abcdef";
	u8 map[256];
	u64 i, j;
	u64 len;
	len = mLength / 2 + mLength % 2;
	pValue.SetLength(len);
	pValue.mToken = GrapaTokenType::RAW;
	for (i = 0; i < 16; i++) map[map1[i]] = map[map2[i]] = (u8)i;
	i = 0;
	j = 0;
	if (mLength % 2)
	{
		((u8*)pValue.mBytes)[i++] = map[((u8*)mBytes)[j++]];
	}
	for (; j < mLength; j += 2)
	{
		((u8*)pValue.mBytes)[i++] = (map[((u8*)mBytes)[j]] << 4) | map[((u8*)mBytes)[j + 1]];
	}
}

void GrapaBYTE::ToRaw(GrapaBYTE& pValue)
{
	pValue.SetLength(0);
	pValue.FROM(*this);
	pValue.mToken = GrapaTokenType::RAW;
}

GrapaCHAR GrapaBYTE::ToStr()
{
	switch (mToken)
	{
	case GrapaTokenType::SYSINT:
	case GrapaTokenType::INT:
	case GrapaTokenType::BOOL:
	{
		GrapaInt n;
		n.FromBytes(*this);
		return n.ToString(10);
	}
		break;
	case GrapaTokenType::TIME:
	{
		GrapaTime t;
		t.FromBytes(*this);
		return t.getString();
	}
		break;
	case GrapaTokenType::FLOAT:
	{
		GrapaFloat d(false,16,4,0);
		d.FromBytes(*this);
		return d.ToString(10);
	}
		break;
	case GrapaTokenType::SYSID:
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSSTR:
	case GrapaTokenType::STR:
	case GrapaTokenType::TABLE:
	case GrapaTokenType::RAW:
		{
			GrapaCHAR n(*this);
			n.mToken = GrapaTokenType::STR;
			//if (n.mLength && n.mBytes[0] == 0)
			//{
			//	GrapaMem::MemCopy(n.mBytes, &n.mBytes[1], n.mSize - 1);

			//	n.mLength--;
			//	n.mBytes[n.mLength] = 0;
			//}
			return(n);
		}
		break;
	//case GrapaTokenType::HEX:
	//	{
	//		u8 *map1 = (u8*)"0123456789ABCDEF";
	//		u8 *map2 = (u8*)"0123456789abcdef";
	//		u8 map[256];
	//		u64 i, j;
	//		u64 len;
	//		len = mLength / 2 + mLength % 2;
	//		GrapaCHAR pValue;
	//		pValue.SetLength(len);
	//		for (i = 0; i < 16; i++) map[map1[i]] = map[map2[i]] = (u8)i;
	//		i = 0;
	//		j = 0;
	//		if (mLength % 2)
	//		{
	//			if (((u8*)mBytes)[j] != '.')
	//			{
	//				pValue.SetLength(0);
	//				break;
	//			}
	//			((u8*)pValue.mBytes)[i++] = map[((u8*)mBytes)[j++]];
	//		}
	//		for (; j < mLength; j += 2)
	//		{
	//			if (((u8*)mBytes)[j] == '.' || ((u8*)mBytes)[j + 1] == '.')
	//			{
	//				pValue.SetLength(0);
	//				break;
	//			}
	//			((u8*)pValue.mBytes)[i++] = (map[((u8*)mBytes)[j]] << 4) | map[((u8*)mBytes)[j + 1]];
	//		}
	//		return(pValue);
	//	}
	//	break;
    default: break;
	}
	return(GrapaCHAR());
}

void GrapaBYTE::SetSize(u64 pSize, bool pCopy)
{
	mPos = 0;
	u8* tempStr = 0LL;
	if (pSize) tempStr = (u8*)GrapaMem::Create(pSize+1);
	if (pCopy && mBytes && tempStr) GrapaMem::MemCopy(tempStr, mBytes, (mSize < pSize) ? mSize : pSize);
	if (mBytes) GrapaMem::Delete(mBytes);
	mBytes = tempStr;
	mSize = 0LL;
	if (tempStr)
	{
		mSize = pSize + 1;
		mBytes[pSize] = 0;
	}
	if (pCopy)
	{
		mLength = (pSize < mLength) ? pSize : mLength;
	}
	else
		mLength = 0LL;
}

void GrapaBYTE::SetNull()
{
	if (mBytes) GrapaMem::Delete(mBytes);
	mBytes = NULL;
	mSize = mLength = 0;
	mToken = GrapaTokenType::SYSID;
}

void GrapaBYTE::Append(const GrapaCHAR& pValue)
{
	if (pValue.mLength && pValue.mBytes)
		Append(pValue.mBytes, pValue.mLength);
}

void GrapaBYTE::Append(const GrapaBYTE& pValue)
{
	if (pValue.mLength && pValue.mBytes)
		Append(pValue.mBytes, pValue.mLength);
}

void GrapaBYTE::Append(void *pValue, u64 pLen)
{
	GrowSize(mLength + pLen + 1, true);
	if (mBytes)
	{
		if (pValue && pLen) memcpy(&mBytes[mLength], pValue, (size_t)pLen);
		mLength += pLen;
		mBytes[mLength] = 0;
	}
}

void GrapaBYTE::Append(u8 pValue)
{
	GrowSize(mLength + 2, true);
	if (mBytes)
	{
		((char*)mBytes)[mLength] = pValue;
		mLength += 1;
		mBytes[mLength] = 0;
	}
}

s64 GrapaBYTE::Cmp(const GrapaBYTE& pValue)
{
	if (mBytes == NULL || mLength == 0) { if (pValue.mBytes == NULL || pValue.mLength == 0) return(0); else return(1); }
	else if (pValue.mBytes == NULL) return(-1);
	u64 len = mLength < pValue.mLength ? mLength : pValue.mLength;
	s64 cmp = strncmp((char*)mBytes, (char*)pValue.mBytes, (size_t)len);
	if (cmp) return(cmp);
	return(mLength - pValue.mLength);
}

s64 GrapaBYTE::Cmp(const char* pStr)
{
	if (mBytes == NULL || mLength == 0) { if (pStr == NULL || strlen(pStr) == 0) return(0); else return(1); }
	else if (pStr == NULL) return(-1);
	size_t plen = strlen(pStr);
	u64 len = mLength < plen ? mLength : plen;
	s64 cmp = strncmp((char*)mBytes, (char*)pStr, (size_t)len);
	if (cmp) return(cmp);
	return(mLength - plen);
}

void GrapaBYTE::Pad(u64 pSize, u8 pValue)
{
	GrowSize(pSize, true);
	memset(&((u8*)mBytes)[mLength], pValue, (size_t)(pSize - mLength));
	mLength = pSize;
}

u8 GrapaBYTE::ToDbType()
{
	u8 fieldType = GrapaTokenType::RAW;
	switch (mToken)
	{
	case GrapaTokenType::ERR:
	case GrapaTokenType::RAW:
	case GrapaTokenType::BOOL:
	case GrapaTokenType::INT:
	case GrapaTokenType::FLOAT:
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
	case GrapaTokenType::STR:
	case GrapaTokenType::SYSSTR:
	case GrapaTokenType::TIME:
	case GrapaTokenType::ARRAY:
	case GrapaTokenType::TUPLE:
	case GrapaTokenType::VECTOR:
	case GrapaTokenType::WIDGET:
	case GrapaTokenType::LIST:
	case GrapaTokenType::XML:
	case GrapaTokenType::EL:
	case GrapaTokenType::TAG:
	case GrapaTokenType::OP:
	case GrapaTokenType::CODE:
	case GrapaTokenType::OBJ:
	case GrapaTokenType::TABLE: fieldType = mToken; break;
	default: fieldType = GrapaTokenType::RAW; break;
	}
	return(fieldType);
}

void GrapaBYTE::FromDbType(u8 pType)
{
	mToken = GrapaTokenType::RAW;
	switch (pType)
	{
	case GrapaTokenType::ERR:
	case GrapaTokenType::RAW:
	case GrapaTokenType::BOOL:
	case GrapaTokenType::INT:
	case GrapaTokenType::FLOAT:
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
	case GrapaTokenType::STR:
	case GrapaTokenType::SYSSTR:
	case GrapaTokenType::TIME:
	case GrapaTokenType::ARRAY:
	case GrapaTokenType::TUPLE:
	case GrapaTokenType::VECTOR:
	case GrapaTokenType::WIDGET:
	case GrapaTokenType::LIST:
	case GrapaTokenType::XML:
	case GrapaTokenType::EL:
	case GrapaTokenType::TAG:
	case GrapaTokenType::OP:
	case GrapaTokenType::CODE:
	case GrapaTokenType::OBJ:
	case GrapaTokenType::TABLE: mToken = pType; break;
	default: mToken = GrapaTokenType::RAW; break;
	}
}


////////////////////////////////////////////////////////////////////////////////

GrapaCHAR::GrapaCHAR() { mBytes = NULL; INIT(); };
GrapaCHAR::GrapaCHAR(const char *pStr, u64 pLen) { mBytes = NULL; INIT(); FROM(pStr, pLen); };
GrapaCHAR::GrapaCHAR(const GrapaBYTE& pBlob) { mBytes = NULL; INIT(); FROM((char*)pBlob.mBytes, pBlob.mLength); };
GrapaCHAR::GrapaCHAR(s64 pNum) { mBytes = NULL; INIT(); FROM(pNum); };
GrapaCHAR::GrapaCHAR(u64 pId) { mBytes = NULL; INIT(); FROM(pId); };
GrapaCHAR::GrapaCHAR(du64 pId) { mBytes = NULL; INIT(); FROM(pId); };
void GrapaCHAR::INIT() 
{ 
	mToken = GrapaTokenType::STR;
	if (mBytes)
	{
		if (mBytes) 
			GrapaMem::Delete(mBytes);
		mBytes = NULL;
	}
	mSize = 0; 
	mLength = 0; 
	mPos = 0;  
	mGrow = 34; 
}

GrapaCHAR& GrapaCHAR::operator=(const GrapaCHAR& that)
{
	if (this != &that)
	{
		INIT();
		FROM(that);
	}
	return *this;
}

GrapaCHAR& GrapaCHAR::operator=(const GrapaBYTE& that)
{
	if (this != &that)
	{
		INIT();
		FROM(that);
	}
	return *this;
}

void GrapaCHAR::FROM(const char* pStr)
{
	if (pStr) FROM(pStr, (u64)strlen(pStr));
	else SetLength(0);
}

void GrapaCHAR::FROM(const char* pStr, u64 pLen)
{
	if (pStr == NULL)
	{
		SetSize(0);
		mLength = 0;
		return;
	}
	SetSize(pLen + 1,false);
	GrapaMem::MemCopy(mBytes, pStr, pLen);
	mLength = pLen;
	((char*)mBytes)[pLen] = 0;
}

void GrapaCHAR::FROM(s64 pNum)
{
	GrapaSS64 num;
	num.mNum = pNum;
	num.TO(*this);
	mToken = GrapaTokenType::STR;
}

void GrapaCHAR::FROM(u64 pNum)
{
	GrapaSU64 num;
	num.mNum = pNum;
	num.TO(*this);
	mToken = GrapaTokenType::STR;
}

void GrapaCHAR::FROM(du64 pNum)
{
	GrapaDU64 num;
	num.mNum = pNum;
	num.TO(*this);
	mToken = GrapaTokenType::STR;
}

void GrapaCHAR::FROM(const GrapaCHAR& pChar)
{
	if (pChar.mLength && pChar.mBytes)
	{
		SetLength(pChar.mLength, false);
		GrapaMem::MemCopy(mBytes, pChar.mBytes, pChar.mLength);
	}
	else
		SetLength(0);
	mToken = pChar.mToken;
}

void GrapaCHAR::FROM(const GrapaBYTE& pValue)
{
	if (pValue.mLength && pValue.mBytes)
	{
		SetLength(pValue.mLength, false);
		GrapaMem::MemCopy(mBytes, pValue.mBytes, pValue.mLength);
	}
	else
		SetLength(0);
	mToken = pValue.mToken;
}

void GrapaCHAR::SetLength(u64 pLength, bool pCopy)
{
	mPos = 0;
	if ((pLength+1) > mSize)
	{
		if (mBytes == NULL) SetSize(pLength, pCopy);
		else GrowSize(((pLength + 1) <= (mSize + mGrow)) ? mSize + mGrow : pLength + 1, pCopy);
	}
	mLength = pLength;
	if (mBytes) ((char*)mBytes)[mLength] = 0;
}

void GrapaCHAR::SetSize(u64 pSize, bool pCopy)
{
	GrapaBYTE::SetSize(pSize,pCopy);
	if (mLength >= mSize)
	{
		if (mSize)
			mLength = mSize - 1;
		else
			mLength = 0;
	}
}

void GrapaCHAR::Append(const char* pStr, u64 pLen)
{
	if (pStr==NULL || pLen == 0) return;
	SetLength(mLength + pLen);
	GrapaMem::MemCopy(&((char*)mBytes)[mLength - pLen], pStr, pLen);
	((char*)mBytes)[mLength] = 0;
}

void GrapaCHAR::Append(const char* pStr)
{
	if (pStr == NULL) return;
	Append(pStr, strlen(pStr));
}

void GrapaCHAR::Append(const char pChar)
{
	if (pChar == 0) return;
	SetLength(mLength + 1);
	((char*)mBytes)[mLength - 1] = pChar;
	((char*)mBytes)[mLength] = 0;
}

void GrapaCHAR::Append(const GrapaBYTE& pChar)
{
	if (pChar.mLength && pChar.mBytes)
		Append((char*)pChar.mBytes,pChar.mLength);
}

void GrapaCHAR::Append(s64 pNum)
{
	GrapaSS64 num;
	GrapaCHAR str;
	num.mNum = pNum;
	num.TO(str);
	Append(str);
}

void GrapaCHAR::Append(u64 pNum)
{
	GrapaSU64 num;
	GrapaCHAR str;
	num.mNum = pNum;
	num.TO(str);
	Append(str);
}

void GrapaCHAR::AppendQuoted(GrapaCHAR& pStr, bool pIsOptional)
{
	if (pStr.mToken == GrapaTokenType::INT)
	{
		Append(GrapaInt(pStr).ToString());
	}
	else
	{
		if (pStr.mLength == 0 || pStr.mBytes == NULL)
		{
			if (pStr.mToken == GrapaTokenType::STR) Append("\"\"");
		}
		else if (pStr.mBytes[0] == '"' && pStr.mBytes[pStr.mLength - 1] == '"')
			Append(pStr);
		else if (pIsOptional && strpbrk((const char*)pStr.mBytes, (const char*)" ,\"'") == NULL)
			Append(pStr);
		else
		{
			Append("\"");
			GrapaCHAR s;
			pStr.EscapeValue(s, (char*)"\\\"\b\f\n\r\t");
			//pStr.EscapeValue(s, (char*)"\\\"/\b\f\n\r\t");
			Append(s);
			Append("\"");
		}
	}
}

void GrapaCHAR::EscapeValue(GrapaCHAR &pTo, char* e)
{
	char s[12];
	char* str = (char*)mBytes;
	pTo.SetSize(mLength * 2 + 1);
	pTo.SetLength(0);
	// look in mIdentifiers, mEscapeFrom, mEscapeTo, mEscapeHex

	for (u64 i = 0; i<mLength; i++)
	{
		// if in mIdentifiers
		if (char *from = strchr(e, str[i]))
		{
			s[0] = '\\';
			switch (str[i])
			{
			case '\\': s[1] = '\\'; break;
			case '\t': s[1] = 't'; break;
			case '\b': s[1] = 'b'; break;
			case '\n': s[1] = 'n'; break;
			case '\r': s[1] = 'r'; break;
			case '\f': s[1] = 'f'; break;
			//case '/': s[1] = '/'; break;
			case '\0': s[1] = '0'; break;
			default: s[1] = str[i]; break;
			}
			s[2] = 0;
			pTo.Append(s);
		}
		else
			pTo.Append(str[i]);
	}
	pTo.SetSize(pTo.mLength + 1);
}

s64 GrapaCHAR::StrNCmp(const char* pStr, u64 pLen)
{
	if (pStr && *pStr && pLen == 0) pLen = strlen(pStr);
	if (mBytes == NULL || mLength == 0) { if (pStr == NULL || pLen == 0) return(0); else return(-1); }
	else if (pStr == NULL) return(1);
	return strncmp((char*)mBytes, pStr, (size_t)pLen);
}

s64 GrapaCHAR::StrNCmp(const char* pStr)
{
	if (mBytes == NULL || mLength == 0) { if (pStr == NULL || strlen(pStr) == 0) return(0); else return(-1); }
	else if (pStr == NULL) return(1);
	return strncmp((char*)mBytes, pStr, (size_t)mLength);
}

s64 GrapaCHAR::StrCmp(const char* pStr)
{
	if (mBytes == NULL || mLength == 0) { if (pStr == NULL || strlen(pStr) == 0) return(0); else return(-1); }
	else if (pStr == NULL) return(1);
	return(strcmp((char*)mBytes, pStr));
}

s64 GrapaCHAR::StrCmp(const GrapaBYTE& pStr)
{
    if (mBytes == NULL || mLength == 0) { if (pStr.mBytes == NULL || pStr.mLength == 0) return(0); else return(-1); }
    else if (pStr.mBytes == NULL) return(1);
    u64 len = mLength < pStr.mLength ? mLength : pStr.mLength;
    int cmp = strncmp((char*)mBytes, (char*)pStr.mBytes, (size_t)len);
    if (cmp) return(cmp);
    return(mLength - pStr.mLength);
}

s64 GrapaCHAR::StrCmp(const GrapaCHAR& pStr)
{
    if (mBytes == NULL || mLength == 0) { if (pStr.mBytes == NULL || pStr.mLength == 0) return(0); else return(-1); }
    else if (pStr.mBytes == NULL) return(1);
    u64 len = mLength < pStr.mLength ? mLength : pStr.mLength;
    int cmp = strncmp((char*)mBytes, (char*)pStr.mBytes, (size_t)len);
    if (cmp) return(cmp);
    return(mLength - pStr.mLength);
}

s64 GrapaCHAR::StrUpperCmp(const char* pStr, u64 pLen)
{
	if (pStr && *pStr && pLen == 0) pLen = strlen(pStr);
	if (mBytes == NULL || mLength==0) { if (pStr == NULL || pLen==0) return(0); else return(-1); }
	else if (pStr == NULL) return(1);
	return(GrapaMem::StrUprCmp((char*)mBytes, mLength, (char*)pStr, pLen));
}

s64 GrapaCHAR::StrUpperCmp(const GrapaBYTE& pStr)
{
	if (mBytes == NULL || mLength == 0) { if (pStr.mBytes == NULL || pStr.mLength == 0) return(0); else return(-1); }
	else if (pStr.mBytes == NULL) return(1);
	return(GrapaMem::StrUprCmp((char*)mBytes, mLength, (char*)pStr.mBytes, pStr.mLength));
}

s64 GrapaCHAR::StrLowerCmp(const char* pStr, u64 pLen)
{
	if (pStr && *pStr && pLen == 0) pLen = strlen(pStr);
	if (mBytes == NULL || mLength == 0) { if (pStr == NULL || pLen == 0) return(0); else return(-1); }
	else if (pStr == NULL) return(1);
	return(GrapaMem::StrLwrCmp((char*)mBytes, mLength, (char*)pStr, pLen));
}

s64 GrapaCHAR::StrLowerCmp(const GrapaBYTE& pStr)
{
	if (mBytes == NULL || mLength == 0) { if (pStr.mBytes == NULL || pStr.mLength == 0) return(0); else return(-1); }
	else if (pStr.mBytes == NULL) return(1);
	return(GrapaMem::StrLwrCmp((char*)mBytes, mLength, (char*)pStr.mBytes, pStr.mLength));
}

void GrapaCHAR::Pad(u64 pSize, u8 pValue)
{
	GrowSize(pSize, true);
	memset(&((u8*)mBytes)[mLength], pValue, (size_t)(pSize - mLength));
	mLength = pSize;
}

void GrapaCHAR::ToUpper() { 
	u64 i = mLength; while (i--) ((char*)mBytes)[i] = toupper(((char*)mBytes)[i]); 
}
void GrapaCHAR::ToLower() { 
	u64 i = mLength; while (i--) ((char*)mBytes)[i] = tolower(((char*)mBytes)[i]); 
}
void GrapaCHAR::RTrim(const char c) { 
	if (mBytes == NULL) 
		return;
	u64 i = mLength; 
	while (i-- && ((char*)mBytes)[i] == c) 
		mLength--; 
	((char*)mBytes)[mLength] = 0; 
}
void GrapaCHAR::LTrim(const char c) {
	if (mBytes == NULL) 
		return; 
	u64 i = 0; 
	while (i < mLength && ((char*)mBytes)[i] == c) 
		i++; 
	mLength = GrapaMem::MoveLeft(mBytes, i, mLength);
}
void GrapaCHAR::Trim(const char c) { 
	RTrim(c); 
	LTrim(c); 
}

void GrapaCHAR::RTrim(GrapaCHAR& s) {
	if (s.mLength == 0) return;
	if (s.mLength == 1)
	{
		RTrim((char)*s.mBytes);
	}
	else
	{
		while (mLength >= s.mLength && s.StrCmp((char*)&mBytes[mLength - s.mLength]) == 0)
			SetLength(mLength - s.mLength);
	}
}
void GrapaCHAR::LTrim(GrapaCHAR& s) {
	if (s.mLength == 0) return;
	if (s.mLength == 1)
	{
		LTrim((char)*s.mBytes);
	}
	else
	{
		u64 i = 0;
		while ((mLength-i) >= s.mLength && s.StrNCmp((char*)&mBytes[i], s.mLength) == 0)
			i += s.mLength;
		mLength = GrapaMem::MoveLeft(mBytes, i, mLength);
	}
}
void GrapaCHAR::Trim(GrapaCHAR& s) { RTrim(s); LTrim(s); }

void GrapaCHAR::RTrim2(GrapaRuleEvent* s)
{
	if (s->mValue.mToken == GrapaTokenType::STR || s->mValue.mToken == GrapaTokenType::RAW)
	{
		RTrim(s->mValue);
	}
	else if (s->mValue.mToken == GrapaTokenType::ARRAY || s->mValue.mToken == GrapaTokenType::ARRAY)
	{
		bool found = true;
		while (mLength && found)
		{
			found = false;
			GrapaRuleEvent* v = s->vQueue->Head();
			while (v)
			{
				if (v->mValue.mToken == GrapaTokenType::STR || v->mValue.mToken == GrapaTokenType::RAW)
				{
					if (v->mValue.mLength && mLength >= v->mValue.mLength && v->mValue.StrCmp((char*)&mBytes[mLength - v->mValue.mLength]) == 0)
					{
						found = true;
						SetLength(mLength - v->mValue.mLength);
					}
				}
				v = v->Next();
			}
		}
	}
}
void GrapaCHAR::LTrim2(GrapaRuleEvent* s)
{
	if (s->mValue.mToken == GrapaTokenType::STR || s->mValue.mToken == GrapaTokenType::RAW)
	{
		LTrim(s->mValue);
	}
	else if (s->mValue.mToken == GrapaTokenType::ARRAY || s->mValue.mToken == GrapaTokenType::TUPLE)
	{
		bool found = true;
		u64 i = 0;
		while ((mLength - i) && found)
		{
			found = false;
			GrapaRuleEvent* v = s->vQueue->Head();
			while (v)
			{
				if (v->mValue.mToken == GrapaTokenType::STR || v->mValue.mToken == GrapaTokenType::RAW)
				{
					if (v->mValue.mLength && (mLength - i) >= v->mValue.mLength && v->mValue.StrNCmp((char*)&mBytes[i], v->mValue.mLength) == 0)
					{
						found = true;
						i += v->mValue.mLength;
					}
				}
				v = v->Next();
			}
		}
		mLength = GrapaMem::MoveLeft(mBytes, i, mLength);
	}
}
void GrapaCHAR::Trim2(GrapaRuleEvent* s) { RTrim2(s); LTrim2(s); }


void GrapaCHAR::Replace(const GrapaBYTE& pFrom, const GrapaBYTE& pTo)
{
	u8 *mB = mBytes;
	u8 *s = mBytes;
	u64 len = mLength;
	mBytes = NULL;
	mSize = mLength = 0;
	SetLength(0);
	while (len)
	{
		u8*f = (u8*)strstr((char*)s, (char*)pFrom.mBytes);
		if (f)
		{
			Append((const char*)s, (u64)(f-s));
			len -= (u64)(f - s);
			s = f;
			Append(pTo);
			s = &s[pFrom.mLength];
			len -= pFrom.mLength;

		}
		else
		{
			Append((const char*)s, len);
			len = 0;
		}
	}
	GrapaMem::Delete(mB);
}

static char b64table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char* spc_base64_encode(unsigned char* input, size_t len, int wrap) {
	unsigned char* output, * p;
	size_t        i = 0, mod = len % 3, toalloc;

	toalloc = (len / 3) * 4 + (3 - mod) % 3 + 1;
	if (wrap) {
		toalloc += len / 57;
		if (len % 57) toalloc++;
	}

	p = output = (unsigned char*)malloc(((len / 3) + (mod ? 1 : 0)) * 4 + 1);
	if (!p) return 0;

	while (i < len - mod) {
		*p++ = b64table[input[i++] >> 2];
		*p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
		*p++ = b64table[((input[i] << 2) | (input[i + 1] >> 6)) & 0x3f];
		*p++ = b64table[input[i + 1] & 0x3f];
		i += 2;
		if (wrap && !(i % 57)) *p++ = '\n';
	}
	if (!mod) {
		if (wrap && i % 57) *p++ = '\n';
		*p = 0;
		return output;
	}
	else {
		*p++ = b64table[input[i++] >> 2];
		*p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
		if (mod == 1) {
			*p++ = '=';
			*p++ = '=';
			if (wrap) *p++ = '\n';
			*p = 0;
			return output;
		}
		else {
			*p++ = b64table[(input[i] << 2) & 0x3f];
			*p++ = '=';
			if (wrap) *p++ = '\n';
			*p = 0;
			return output;
		}
	}
}

static const unsigned char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char* base64_decode(const unsigned char* src, size_t len,
	size_t* out_len)
{
	unsigned char dtable[256], * out, * pos, block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

	memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(base64_table) - 1; i++)
		dtable[base64_table[i]] = (unsigned char)i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count == 0 || count % 4)
		return NULL;

	olen = count / 4 * 3;
	pos = out = (unsigned char*)malloc(olen);
	if (out == NULL)
		return NULL;

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		if (tmp == 0x80)
			continue;

		if (src[i] == '=')
			pad++;
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			*pos++ = (block[2] << 6) | (block[3]);
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
					free(out);
					return NULL;
				}
				break;
			}
		}
	}

	*out_len = pos - out;
	return out;
}

#include <string>
#include <vector>
#include <openssl/bn.h>

void encodeb58(const char* priv_hex, GrapaCHAR& endresult)
{
	char table[] = { '1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','J','K','L','M','N','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };

	BIGNUM* base58 = NULL;

	BIGNUM* resultExp = BN_new();
	BIGNUM* resultAdd = BN_new();
	BIGNUM* resultRem = BN_new();
	BN_CTX* bn_ctx = BN_CTX_new();

	BN_dec2bn(&base58, "58");

	std::vector<int> v;

	BN_hex2bn(&resultAdd, priv_hex);

	while (!BN_is_zero(resultAdd))
	{
		BN_div(resultAdd, resultRem, resultAdd, base58, bn_ctx);
		char* asdf = BN_bn2dec(resultRem);
		v.push_back(atoi(asdf));
	}

	for (int i = (int)v.size() - 1; i >= 0; i--)
		endresult.Append(table[v[i]]);

	BN_free(resultAdd);
	BN_free(resultExp);
	BN_free(resultRem);
	BN_CTX_free(bn_ctx);
}

static constexpr const uint8_t Base58Map[] = {
  '1', '2', '3', '4', '5', '6', '7', '8',
  '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
  'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
  'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
  'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
  'y', 'z' };
static constexpr const uint8_t AlphaMap[] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0xff, 0x11, 0x12, 0x13, 0x14, 0x15, 0xff,
  0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0xff, 0x2c, 0x2d, 0x2e,
  0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xff, 0xff, 0xff, 0xff, 0xff };

using CodecMapping = struct _codecmapping
{
	_codecmapping(const uint8_t* amap, const uint8_t* bmap) : AlphaMapping(amap), BaseMapping(bmap) {}
	const uint8_t* AlphaMapping;
	const uint8_t* BaseMapping;
};

std::string Base58Encode(const std::string& str)
{
	CodecMapping mapping(AlphaMap, Base58Map);
	std::vector<uint8_t> data(str.begin(), str.end());
	std::vector<uint8_t> digits((data.size() * 138 / 100) + 1);
	size_t digitslen = 1;
	for (size_t i = 0; i < data.size(); i++)
	{
		uint32_t carry = static_cast<uint32_t>(data[i]);
		for (size_t j = 0; j < digitslen; j++)
		{
			carry = carry + static_cast<uint32_t>(digits[j] << 8);
			digits[j] = static_cast<uint8_t>(carry % 58);
			carry /= 58;
		}
		for (; carry; carry /= 58)
			digits[digitslen++] = static_cast<uint8_t>(carry % 58);
	}
	std::string result;
	for (size_t i = 0; i < (data.size() - 1) && !data[i]; i++)
		result.push_back(mapping.BaseMapping[0]);
	for (size_t i = 0; i < digitslen; i++)
		result.push_back(mapping.BaseMapping[digits[digitslen - 1 - i]]);
	return result;
}

template <class BidirectionalIterator>
void reverseX(BidirectionalIterator first, BidirectionalIterator last)
{
	while ((first != last) && (first != --last)) {
		std::swap(first, last);
		++first;
	}
}

std::string Base58Decode(const std::string& data)
{
	CodecMapping mapping(AlphaMap, Base58Map);
	std::vector<uint8_t> result((data.size() * 138 / 100) + 1);
	size_t resultlen = 1;
	for (size_t i = 0; i < data.size(); i++)
	{
		uint32_t carry = static_cast<uint32_t>(mapping.AlphaMapping[data[i] & 0x7f]);
		for (size_t j = 0; j < resultlen; j++, carry >>= 8)
		{
			carry += static_cast<uint32_t>(result[j] * 58);
			result[j] = static_cast<uint8_t>(carry);
		}
		for (; carry; carry >>= 8)
			result[resultlen++] = static_cast<uint8_t>(carry);
	}
	result.resize(resultlen);
	for (size_t i = 0; i < (data.size() - 1) && data[i] == mapping.BaseMapping[0]; i++)
		result.push_back(0);
	reverseX(result.begin(), result.end());
	return std::string(result.begin(), result.end());
}

GrapaCHAR GrapaCHAR::ToBase(u32 radix)
{
	GrapaCHAR result;
	if (mLength == 0) return(result);
	unsigned char* in = NULL;
	std::string str;
	switch (radix)
	{
	case 64:
		in = spc_base64_encode((unsigned char*)mBytes, mLength, false);
		if (in)
		{
			result.FROM((char*)in);
			free(in);
			return(result);
		}
		break;
	case 58:
		str = Base58Encode(std::string((char*)mBytes,mLength));
		result.FROM(str.c_str(), str.size());
		break;
	}
	return(result);
}

GrapaCHAR GrapaCHAR::FromBase(u32 radix)
{
	GrapaCHAR result;
	if (mLength==0) return(result);
	unsigned char* in = NULL;
	std::string str;

	size_t outlen = 0;

	switch (radix)
	{
	case 64:
		in = base64_decode((unsigned char*)mBytes, mLength, &outlen);
	if (in)
	{
		result.FROM((char*)in, outlen);
		free(in);
		return(result);
	}
	case 58:
		str = Base58Decode(std::string((char*)mBytes, mLength));
		result.FROM(str.c_str(), str.size());
		break;
	}
	return(result);
}

////////////////////////////////////////////////////////////////////////////////

GrapaSS64::GrapaSS64() { mNum = 0; };

void GrapaSS64::TO(GrapaCHAR& pChar)
{
	pChar.GrowSize(65,false);
	        pChar.mLength = snprintf((char*)pChar.mBytes, pChar.mSize, "%lld", mNum);
	pChar.SetSize(pChar.mLength);
}

////////////////////////////////////////////////////////////////////////////////

GrapaSU64::GrapaSU64() : mNum(0) {};

void GrapaSU64::TO(GrapaCHAR& pChar)
{
	pChar.GrowSize(65,false);
	        pChar.mLength = snprintf((char*)pChar.mBytes, pChar.mSize, "%llu", mNum);
	pChar.SetSize(pChar.mLength);
}

////////////////////////////////////////////////////////////////////////////////

GrapaDU64::GrapaDU64() { mNum.key = 0; mNum.value = 0; };
GrapaDU64::GrapaDU64(du64 pNum) : mNum(pNum) {};

void GrapaDU64::TO(GrapaCHAR& pChar)
{
	pChar.GrowSize(120,false);
	        pChar.mLength = snprintf((char*)pChar.mBytes, pChar.mSize, "{%llu,%llu}", mNum.key, mNum.value);
}

////////////////////////////////////////////////////////////////////////////////

//void BtDouble::ToBytes(GrapaCHAR& pChar) 
//{
//	pChar.SetLength(8);
//	double n = mNum;
//	s64 u = *(s64*)((u8*)&n);
//	(*(s64*)pChar.mBytes) = BE_D64(u);
//	pChar.mToken = GrapaBYTE::DEC;
//}
//
//void BtDouble::FromBytes(const GrapaBYTE& pValue) 
//{
//	if (pValue.mLength == 8) 
//	{
//		s64 u = *(s64*)pValue.mBytes;
//		u = BE_D64(u);
//		mNum = *(double*)((u8*)&u);
//	}
//}
//void BtDouble::ToString(GrapaCHAR& pChar, u32 radix) 
//{
//	switch (radix)
//	{
//	case 2:
//	case 10:
//	case 16:
//		{
//			char *map = (char*)"0123456789ABCDEF";
//			bool isNeg = (mNum < 0);
//			long double d = isNeg ? -mNum : mNum;
//			pChar.SetLength(0);
//			if (isNeg) pChar.Append('-');
//			GrapaInt a = (u64)trunc(d);
//			pChar.Append(a.ToString(radix));
//			pChar.Append('.');
//			u64 pow = 0, powlen = 0;
//			switch (radix)
//			{
//			case 2: pow = 0x80000000000000; powlen = 56; break;
//			case 10: pow = 10000000000000000; powlen = 17;  break;
//			case 16: pow = 0x1000000000000000; powlen = 16; break;
//			}
//			for (int i = isNeg ? 1 : 0; powlen && i < pChar.mLength; i++)
//			{
//				pow /= radix;
//				powlen--;
//			}
//			d -= trunc(d);
//			d *= pow;
//			GrapaInt b = (u64)trunc(d);
//			GrapaInt r = (u64)radix;
//			b += trunc((d-trunc(d))*radix) > (radix / 2) ? 1 : 0;
//			GrapaCHAR mant = b.ToString(radix);
//			if (powlen > mant.mLength)
//			{
//				GrapaCHAR pad;
//				pad.Pad(powlen - mant.mLength - 1, '0');
//				pChar.Append(pad);
//			}
//			pChar.Append(mant);
//			pChar.RTrim('0');
//			pChar.RTrim('.');
//		}
//		break;
//	}
//}
//void BtDouble::FromString(const GrapaCHAR& pChar, u32 radix)
//{
//	mNum = 0;
//	if (pChar.mLength == 0) return;
//	long double d = 0;
//	u64 powlen = 0;
//	switch (radix)
//	{
//	case 2: powlen = 56; break;
//	case 10: powlen = 17;  break;
//	case 16: powlen = 16; break;
//	default: return;
//	}
//	GrapaCHAR b1, b2;
//	GrapaInt a, b;
//	GrapaCHAR in(pChar);
//	b1.SetSize(128);
//	b2.SetSize(128);
//	if (char* ps = strchr((char*)in.mBytes, '.'))
//		ps[0] = ' ';
//	sscanf((char*)in.mBytes, "%s %s", b1.mBytes, b2.mBytes);
//	b1.SetLength(strlen((char*)b1.mBytes));
//	b2.SetLength(strlen((char*)b2.mBytes));
//	if (b1.mLength > powlen)
//		return;
//	if ((b1.mLength + b2.mLength) > powlen)
//		b2.mLength = powlen - b1.mLength;
//	a.FromString(b1, radix);
//	b.FromString(b2, radix);
//	mNum = (u64)b.LongValue();
//	for (int i = 0; i < b2.mLength; i++)
//		mNum /= radix;
//	mNum += (u64)a.LongValue();
//}

////////////////////////////////////////////////////////////////////////////////

//#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
//#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
//#else
//#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
//#endif
//
//struct timezonewindows
//{
//	int  tz_minuteswest; /* minutes W of Greenwich */
//	int  tz_dsttime;     /* type of dst correction */
//};
//
//int gettimeofdaywindows(struct timeval *tv, struct timezonewindows *tz)
//{
//	FILETIME ft;
//	unsigned __int64 tmpres = 0;
//	static int tzflag;
//
//	if (NULL != tv)
//	{
//		GetSystemTimeAsFileTime(&ft);
//
//		tmpres |= ft.dwHighDateTime;
//		tmpres <<= 32;
//		tmpres |= ft.dwLowDateTime;
//
//		/*converting file time to unix epoch*/
//		tmpres -= DELTA_EPOCH_IN_MICROSECS;
//		tmpres /= 10;  /*convert into microseconds*/
//		tv->tv_sec = (long)(tmpres / 1000000UL);
//		tv->tv_usec = (long)(tmpres % 1000000UL);
//	}
//
//	if (NULL != tz)
//	{
//		if (!tzflag)
//		{
//			_tzset();
//			tzflag++;
//		}
//		tz->tz_minuteswest = _timezone / 60;
//		tz->tz_dsttime = _daylight;
//	}
//
//	return 0;
//}

GrapaVoidArray::GrapaVoidArray(u32 pCount)
{
	mBytes = NULL;  
	GrapaBYTE::INIT();
	Init(pCount);
}

GrapaVoidArray::~GrapaVoidArray()
{
	u32 count,i;
	count = Count();
	for (i=0;i<count;i++)
	{
		//void *ptr = GetAt(i);
		//if (ptr) delete ptr;
	}
	SetSize(0);
}

void GrapaVoidArray::Init(u32 pCount)
{
	SetSize(pCount*sizeof(void*),false);
	memset(mBytes,0,pCount*sizeof(void*));
}

void GrapaVoidArray::Append(void *pItem)
{
	SetLength(GetLength()+sizeof(void*),true);
	GrapaMem::MemCopy(&((char*)mBytes)[GetLength() - sizeof(void*)], &pItem, sizeof(void*));
}

u32 GrapaVoidArray::Count()
{
	return((u32)(GetLength()/sizeof(void*)));
}

void* GrapaVoidArray::GetAt(u32 i)
{
	return(((void**)mBytes)[i]);
}

////////////////////////////////////////////////////////////////////////////////

GrapaCHARArray::GrapaCHARArray(u32 pCount) : GrapaVoidArray(pCount) {};

GrapaCHARArray::~GrapaCHARArray()
{
	s32 count,i;
	count = Count();
	for (i=0;i<count;i++)
	{
		GrapaCHAR *ptr = (GrapaCHAR*)GetAt(i);
		if (ptr)
		{
			delete ptr;
		}
	}
	SetSize(0);
}

void GrapaCHARArray::Append(char *pStr)
{
	GrapaVoidArray::Append((void*)new GrapaCHAR(pStr));
}

////////////////////////////////////////////////////////////////////////////////

GrapaDU64Array::GrapaDU64Array(u32 pCount) : GrapaVoidArray(pCount) {};

GrapaDU64Array::~GrapaDU64Array()
{
	s32 count,i;
	count = Count();
	for (i=0;i<count;i++)
	{
		GrapaDU64 *ptr = (GrapaDU64*)GetAt(i);
		if (ptr)
		{
			delete ptr;
		}
	}
	SetSize(0);
}

void GrapaDU64Array::Append(du64 pNum)
{
	GrapaVoidArray::Append((void*)new GrapaDU64(pNum));
}

void GrapaDU64Array::Append(u64 key, u64 value)
{
	du64 num;
	num.key = key;
	num.value = value;
	GrapaVoidArray::Append((void*)new GrapaDU64(num));
}

GrapaArray64::GrapaArray64() { INIT(); };
GrapaArray64& GrapaArray64::operator=(const GrapaArray64& that) { if (this != &that) { INIT(); SetLength(that.mLength, false); memcpy(mBytes, that.mBytes, (size_t)(mLength*sizeof(u64))); } return *this; }
void GrapaArray64::INIT() { mBytes = NULL; mSize = 0; mLength = 0; mPos = 0; mGrow = 4; mToken = GrapaTokenType::START; }
u64 GrapaArray64::GetCount() { return(mLength / sizeof(u64)); }
void GrapaArray64::SetCount(u64 pCount) { u64 oldCount = GetCount(), newCount = pCount; SetLength(((pCount / mGrow) + 1) * mGrow * sizeof(u64), true); mLength = pCount*sizeof(u64); if (newCount > oldCount) memset(&((u64*)mBytes)[oldCount], 0, (size_t)((newCount - oldCount) * sizeof(u64))); else if ((newCount + mGrow) < (mSize / sizeof(u64))) SetSize(newCount*sizeof(u64), true); }
void GrapaArray64::GrowCount(u64 pCount) { if (pCount > GetCount()) SetCount(pCount); }
void GrapaArray64::SetItem(u64 pItem, u64 pValue) { GrowCount(pItem + 1); ((u64*)mBytes)[pItem] = pValue; }
u64 GrapaArray64::GetItem(u64 pItem) { return((pItem + 1) > GetCount() ? 0 : ((u64*)mBytes)[pItem]); }

////////////////////////////////////////////////////////////////////////////////

void GrapaQueue::CLEAR()
{
	WaitCritical();
	GrapaEvent* e = mHead;
	while (e)
	{
		GrapaEvent* en = e->Next();
		e->CLEAR();
		delete e;
		e = en;
		mCount--;
	}
	mHead = NULL;
	mTail = NULL;
	LeaveCritical();
}

void GrapaQueue::PushHead(GrapaEvent* pEvent)
{
	WaitCritical();
	if (pEvent->Queue()) 
		pEvent->Queue()->PopEvent(pEvent);
	if (mHead == NULL || mTail == NULL)
		mHead = mTail = pEvent;
	else
	{
		mHead->SetPrev(pEvent);
		pEvent->SetNext(mHead);
		mHead = pEvent;
	}
	pEvent->SetQueue(this);
	mCount++;
	if (mState) mState->Resume();
	LeaveCritical();
}

void GrapaQueue::PushTail(GrapaEvent* pEvent)
{
	WaitCritical();
	if (pEvent->Queue()) 
		pEvent->Queue()->PopEvent(pEvent);
	if (mHead == NULL || mTail == NULL)
		mTail = mHead = pEvent;
	else
	{
		mTail->SetNext(pEvent);
		pEvent->SetPrev(mTail);
		mTail = pEvent;
	}
	pEvent->SetQueue(this);
	mCount++;
	if (mState) mState->Resume();
	LeaveCritical();
}

void GrapaQueue::PushNext(GrapaEvent* pLoc, GrapaEvent* pEvent)
{
	if (pLoc == NULL) 
		PushTail(pEvent);
	else
	{
		WaitCritical();
		if (mHead == NULL || mTail == NULL)
			mTail = mHead = pEvent;
		else
		{
			if (pEvent->Queue()) pEvent->Queue()->PopEvent(pEvent);
			pEvent->SetNext(pLoc->Next());
			pEvent->SetPrev(pLoc);
			pLoc->SetNext(pEvent);
			if (pEvent->Next()) pEvent->Next()->SetPrev(pEvent);
			if (mTail == pLoc) mTail = pEvent;
		}
		pEvent->SetQueue(this);
		mCount++;
		if (mState) mState->Resume();
		LeaveCritical();
	}
}

void GrapaQueue::Push(GrapaEvent* pEvent, GrapaEvent* pLoc)
{
	if (pLoc == NULL) 
		PushTail(pEvent);
	else
	{
		WaitCritical();
		if (mHead == NULL || mTail == NULL)
			mTail = mHead = pEvent;
		else
		{
			if (pEvent->Queue()) pEvent->Queue()->PopEvent(pEvent);
			pEvent->SetNext(pLoc);
			pEvent->SetPrev(pLoc->Prev());
			if (pLoc->Prev()) pLoc->Prev()->SetNext(pEvent);
			pLoc->SetPrev(pEvent);
			if (mHead == pLoc) mHead = pEvent;
		}
		pEvent->SetQueue(this);
		mCount++;
		if (mState) mState->Resume();
		LeaveCritical();
	}
}

GrapaEvent* GrapaQueue::PopHead(u64 mSkip)
{
	if (mConst) return(NULL);
	WaitCritical();
	GrapaEvent* e = mHead;
	while (mSkip--&&e) 
		e = e->Next();
	if (e)
	{
		GrapaEvent *prev = e->Prev();
		GrapaEvent *next = e->Next();
		if (prev) prev->SetNext(next);
		if (next) next->SetPrev(prev);
		if (mHead == e) mHead = next;
		if (mTail == e) mTail = prev;
		e->SetQueue(NULL);
		mCount--;
		e->SetNext(NULL);
		e->SetPrev(NULL);
	}
	LeaveCritical();
	return(e);
}

GrapaEvent* GrapaQueue::PopTail(u64 mSkip)
{
	if (mConst) return(NULL);
	WaitCritical();
	GrapaEvent* e = mTail;
	while (mSkip--&&e) 
		e = e->Prev();
	if (e)
	{
		GrapaEvent *prev = e->Prev();
		GrapaEvent *next = e->Next();
		if (prev) prev->SetNext(next);
		if (next) next->SetPrev(prev);
		if (mHead == e) mHead = next;
		if (mTail == e) mTail = prev;
		e->SetQueue(NULL);
		mCount--;
		e->SetNext(NULL);
		e->SetPrev(NULL);
	}
	LeaveCritical();
	return(e);
}

GrapaEvent* GrapaQueue::PopEvent(GrapaEvent* pEvent)
{
	if (mConst) return(NULL);
	WaitCritical();
	GrapaEvent *e = mHead;
	while (e && e != pEvent) 
		e = e->Next();
	if (e)
	{
		GrapaEvent *prev = e->Prev();
		GrapaEvent *next = e->Next();
		if (prev) prev->SetNext(next);
		if (next) next->SetPrev(prev);
		if (mHead == e) mHead = next;
		if (mTail == e) mTail = prev;
		e->SetQueue(NULL);
		mCount--;
		e->SetNext(NULL);
		e->SetPrev(NULL);
	}
	LeaveCritical();
	return(e);
}

GrapaEvent* GrapaQueue::Head(u64 mSkip)
{
	WaitCritical();
	GrapaEvent* e = mHead;
	while (mSkip--&&e)
		e = e->Next();
	LeaveCritical();
	return(e);
}

GrapaEvent* GrapaQueue::Tail(u64 mSkip)
{
	WaitCritical();
	GrapaEvent* e = mTail;
	while (mSkip--&&e) 
		e = e->Prev();
	LeaveCritical();
	return(e);
}

bool GrapaQueue::HasEvent(GrapaEvent* pEvent) 
{ 
	WaitCritical();
	bool foundEvent = false;
	GrapaEvent* item = Head(); 
	while (item)
	{ 
		if (item == pEvent)
		{
			foundEvent = true;
			break;
		}
		item = item->Next(); 
	}
	LeaveCritical();
	return(foundEvent);
}

void GrapaQueue::ReplaceEvent(GrapaEvent* pLoc, GrapaEvent* pEvent)
{
	if (mConst) return;
	WaitCritical();
	GrapaEvent* prev = pLoc->Prev();
	GrapaEvent* next = pLoc->Next();
	if (prev) prev->SetNext(pEvent);
	if (next) next->SetPrev(pEvent);
	if (mHead == pLoc) mHead = pEvent;
	if (mTail == pLoc) mTail = pEvent;
	pEvent->SetPrev(prev);
	pEvent->SetNext(next);
	pEvent->SetQueue(this);
	pLoc->SetPrev(NULL);
	pLoc->SetNext(NULL);
	pLoc->SetQueue(NULL);
	LeaveCritical();
};

void GrapaWorkEvent::Set(GrapaWorkQueue*pQueue)
{
	WaitCritical();
	vQueue = pQueue;
	vQueue->mLock.WaitCritical();
	vQueue->mLock.mWaitCount++;
	vQueue->PushTail(this);
	vQueue->mLock.LeaveCritical();
	LeaveCritical();
}
void GrapaWorkEvent::Starting() {}
void GrapaWorkEvent::Stopping()
{
	WaitCritical();
	vQueue->mLock.WaitCritical();
	vQueue->mLock.SendCondition();
	vQueue->mLock.LeaveCritical();
	LeaveCritical();
}
GrapaError GrapaWorkQueue::Start()
{
	mLock.WaitCritical();
	if (mLock.mWaitCount)
	{
		mLock.WaitCondition(true);
		mLock.WaitCritical();
	}
	mLock.LeaveCritical();
	mLock.WaitCritical();
	mLock.LeaveCritical();
	return(0);
}

////////////////////////////////////////////////////////////////////////////////

GrapaCHARFile::GrapaCHARFile() { mOpened = false; mMode = GrapaReadOnly; SetLength(0); }
bool    GrapaCHARFile::Opened() { return mOpened; }
GrapaError GrapaCHARFile::Open(const char *fileName, char mode) { mOpened = true; mMode = mode;  return(0); }
GrapaError GrapaCHARFile::Close() { mOpened = false; return(0); }
GrapaError GrapaCHARFile::GetSize(u64& pSize) { pSize = mLength; return(0); }
GrapaError GrapaCHARFile::SetSize(u64 pSize) { SetLength(pSize, true); return(0); }
GrapaError GrapaCHARFile::Create(const char *fileName) { mOpened = true;  return(0); }
GrapaError GrapaCHARFile::Delete(const char *fileName) { return(0); }
GrapaError GrapaCHARFile::Flush() { return(0); }
GrapaError GrapaCHARFile::Purge(u64 blockCount, u16 blockSize) { GrapaCHAR::SetSize(blockCount*blockSize, true); return(0); }
GrapaError GrapaCHARFile::Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *b)
{
	if (mLength < (blockPos*blockSize + offset + length))
		SetLength((blockPos*blockSize + offset + length), true);
	GrapaMem::MemCopy(&((char*)mBytes)[blockPos*blockSize + offset], b, length);
	//char*p = (char*)malloc((size_t)length);
	//Read(blockPos, blockSize, offset, length, p);
	//free(p);
	return(0);
}
GrapaError GrapaCHARFile::Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *b)
{
	if (mLength < (blockPos*blockSize + offset)) return(-1);
	if (mLength == (blockPos*blockSize + offset)) return(0);
	if (mLength < (blockPos*blockSize + offset + length))
		length = mLength - (blockPos*blockSize + offset);
	GrapaMem::MemCopy(b, &((char*)mBytes)[blockPos*blockSize + offset], length);
	return(0);
}
GrapaError GrapaCHARFile::Append(u64 length, const void *b)
{
	SetLength(mLength + length);
	GrapaMem::MemCopy(&((char*)mBytes)[mLength - length], b, length);
	return(0);
}

GrapaError GrapaCHARFile::Append(const GrapaCHARFile& pValue)
{
	SetLength(mLength + pValue.mLength);
	GrapaMem::MemCopy(&((char*)mBytes)[mLength - pValue.mLength], pValue.mBytes, pValue.mLength);
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
