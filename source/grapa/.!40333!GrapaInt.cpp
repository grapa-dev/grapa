// GrapaInt.cpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
/////////////////f///////////////////////////////////////////////////////////////

#include "GrapaInt.h"
#include <math.h>

#include <openssl/rand.h>
#include <openssl/rsa.h>

#include "GrapaSystem.h"

extern GrapaSystem* gSystem;

//struct GrapaBitField8
//{
//	u8 b0 : 1;
//	u8 b1 : 1;
//	u8 b2 : 1;
//	u8 b3 : 1;
//	u8 b4 : 1;
//	u8 b5 : 1;
//	u8 b6 : 1;
//	u8 b7 : 1;
//} _GrapaBitField8;

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaArray32::GrapaArray32() { INIT(); };
GrapaArray32::~GrapaArray32() { if (mBytes) GrapaMem::Delete(mBytes); mBytes = NULL; mSize = mLength = 0; };
GrapaArray32& GrapaArray32::operator=(const GrapaArray32& that) { if (this != &that) { INIT(); SetLength(that.mLength, false); memcpy(mBytes, that.mBytes, (size_t)mLength*sizeof(u32)); } return *this; }
void GrapaArray32::INIT() { mBytes = NULL; mSize = 0; mLength = 0; mGrow = 16; dataSigned = false; NaN = false; }
void GrapaArray32::SetLength(u64 pLen, bool pCopy) {
	if (pLen > mSize)
		SetSize(pLen, pCopy);
	mLength = pLen;
}
void GrapaArray32::SetSize(u64 pLen, bool pCopy) {
	if (mLength > pLen)
		mLength = pLen;
	if (pLen != mSize) {
		u8* b = NULL;
		if (pLen) b = (u8*)GrapaMem::Create(pLen);
		if (pCopy&&mBytes&&b)
			memcpy(b, mBytes, (size_t)mLength);
		GrapaMem::Delete(mBytes);
		mBytes = b;
		mSize = pLen;
	}
}
void GrapaArray32::FROM(const GrapaArray32& pData) { dataSigned = pData.dataSigned; NaN = pData.NaN; SetLength(pData.mLength, false); if (pData.mBytes) memcpy(mBytes, pData.mBytes, (size_t)mLength); }
u64 GrapaArray32::GetCount() const { return(mLength / sizeof(u32)); }
u64 GrapaArray32::GetByteCount() const {
	u64 count = GetCount();
	if (count == 0)
		return(0);
	if (GetItem(count - 1) == 0)
		return((count - 1) * sizeof(u32));
	return(count * sizeof(u32));
}
void GrapaArray32::SetCount(u64 pCount) {
	u64 oldCount = GetCount(), newCount = pCount;
	SetLength(((pCount / mGrow) + 1) * mGrow * sizeof(u32), true);
	mLength = pCount*sizeof(u32);
	if (newCount > oldCount)
		memset(&((u32*)mBytes)[oldCount], 0, (size_t) (newCount - oldCount) * sizeof(u32));
	else if ((newCount + mGrow) < (mSize / sizeof(u32)))
		SetSize(((newCount / mGrow) + 1) * mGrow * sizeof(u32), true);
}
void GrapaArray32::GrowCount(u64 pCount) { if (pCount > GetCount()) SetCount(pCount); }
void GrapaArray32::SetItem(u64 pItem, u32 pValue) { GrowCount(pItem + 1); ((u32*)mBytes)[pItem] = pValue; }
u32 GrapaArray32::GetItem(u64 pItem) const { return((pItem + 1) > GetCount() ? (dataSigned ? -1 : 0) : ((u32*)mBytes)[pItem]); }
void GrapaArray32::RTrim() {
	if (dataSigned && !IsNeg())
		dataSigned = false;
	while (GetCount() > 1 && GetItem(GetCount() - 1) == 0 && (GetItem(GetCount() - 2) & 0x80000000) == 0)
		mLength -= sizeof(u32);
	if (!dataSigned && GetCount() && (GetItem(GetCount() - 1) & 0x80000000) != 0)
		SetItem(GetCount(), 0);
	while (GetCount() > 1 && GetItem(GetCount() - 1) == -1 && (GetItem(GetCount() - 2) & 0x80000000) != 0)
		mLength -= sizeof(u32);
}
bool GrapaArray32::IsNull() const { return (GetCount() == 0); }
bool GrapaArray32::IsNeg() const { return (GetCount() ? (GetItem(GetCount() - 1) & 0x80000000) != 0 : false); }
bool GrapaArray32::IsSignNeg() const { return (dataSigned && IsNeg()); }
bool GrapaArray32::IsZero() const { return (GetCount()==0 || (GetCount() == 1 && GetItem(0) == 0) ? true : false); }
bool GrapaArray32::IsItem(u32 pItem) const { return (GetCount() == 1 && GetItem(0) == pItem ? true : false); }

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaInt::GrapaInt()
{
}

GrapaInt::GrapaInt(s64 value)
{
	dataSigned = value < 0;
	u64 v = (u64)value;
	while (v)
	{
		SetItem(GetCount(), v & 0xFFFFFFFF);
		v >>= 32;
	}
	RTrim();
}

GrapaInt::GrapaInt(const GrapaInt& bi)
{
	FROM(bi);
}

GrapaInt::GrapaInt(const char* inData, s32 radix)
{
	dataSigned = false;

	if (inData == NULL || radix == 0) return;

	GrapaInt multiplier((u64)1);
	GrapaInt result;

	s64 len = strlen(inData);
	s64 valueSize = (len + 1) * sizeof(char);
	char* value = (char*)malloc(valueSize);
	memcpy(value, inData, (size_t)len + 1);
	GrapaMem::ToUpper(value);
	GrapaMem::Trim(value);
	len = strlen(value);
	s32 limit = 0;

	if (value[0] == '-')
		limit = 1;

	for (s64 i = len - 1; i >= limit; i--)
	{
		s32 posVal = (s32)value[i];

		if (posVal >= '0' && posVal <= '9')
			posVal -= '0';
		else if (posVal >= 'A' && posVal <= 'Z')
			posVal = (posVal - 'A') + 10;
		else
			posVal = 9999999;       // arbitrary large


		if (posVal >= radix)
			throw;// (new ArithmeticException("Invalid string in constructor."));
		else
		{
			if (value[0] == '-')
				posVal = -posVal;

			result = result + (multiplier * (s64)posVal);

			if ((i - 1) >= limit)
				multiplier = multiplier * (u64)radix;
		}
	}

	if (!result.IsNull() && value[0] == '-')     // negative values
		dataSigned = true;

	free(value);
	*this = result;
}

GrapaInt::GrapaInt(const u8* inData, u32 inLen)
{
	dataSigned = false;

	if (inData == NULL || inLen == 0) return;

	SetCount(inLen >> 2);

	int leftOver = inLen & 0x3;
	if (leftOver != 0)         // length not multiples of 4
		SetCount(GetCount()+1);

	if (IsNull())
		return;

	for (int i = inLen - 1, j = 0; i >= 3; i -= 4, j++)
	{
		SetItem(j,
			(u32)((u32)(((u8*)inData)[i - 3] << 24) | ((u32)((u8*)inData)[i - 2] << 16) |
			((u32)((u8*)inData)[i - 1] << 8) | (u32)((u8*)inData)[i])
			);
	}

	if (leftOver == 1)
		SetItem(GetCount() - 1, (u32)((u8*)inData)[0]);
	else if (leftOver == 2)
		SetItem(GetCount() - 1, (u32)((((u8*)inData)[0] << 8) + ((u8*)inData)[1]));
	else if (leftOver == 3)
		SetItem(GetCount() - 1, (u32)((((u8*)inData)[0] << 16) + (((u8*)inData)[1] << 8) + ((u8*)inData)[2]));

	RTrim();
}

GrapaInt::GrapaInt(u32* inData, u32 inCount)
{
	if (inCount == 0) return;

	dataSigned = false;

	SetCount(inCount);

	for (s64 i = GetCount() - 1, j = 0; i >= 0; i--, j++)
		SetItem(j,inData[i]);

	RTrim();
}

GrapaInt::GrapaInt(const GrapaBYTE& inData)
{
	dataSigned = false;
	FromBytes(inData);
}

GrapaInt::~GrapaInt()
{
}

GrapaInt& GrapaInt::operator =(const GrapaInt& that)
{
	if (this != &that)
		FROM(that);
	return *this;
}

void GrapaInt::operator =(s64 bi) 
{ 
	GrapaInt bi2(bi); 
	*this = bi2; 
}

GrapaInt GrapaInt::operator +(const GrapaInt& bi2) const
{
	GrapaInt result;
	//GrapaInt bi2(bi);
	result.SetCount(((GetCount() > bi2.GetCount()) ? GetCount() : bi2.GetCount()) + ((dataSigned || bi2.dataSigned) ? 1 : 0));
	result.dataSigned = (dataSigned || bi2.dataSigned);
	s64 carry = 0;
	for (s32 i = 0; i < result.GetCount(); i++)
	{
		s64 sum = (s64)GetItem(i) + (s64)bi2.GetItem(i) + carry;
		carry = sum >> 32;
		result.SetItem(i,(u32)(sum & 0xFFFFFFFF));
	}
	if ((carry && dataSigned == bi2.dataSigned) || (dataSigned && bi2.dataSigned))
	{
		if (dataSigned)
		{
			if (!result.IsNeg())
				result.SetItem(result.GetCount(), -1);
		}
		else
			result.SetItem(result.GetCount(), (u32)(carry));
	}
	result.RTrim();
	return result;
}

GrapaInt GrapaInt::operator +(s64 bi) const
{ 
	GrapaInt bi2(bi); 
	return *this + bi2; 
}

GrapaInt GrapaInt::operator +=(const GrapaInt& bi1)
{
	GrapaInt that(*this + bi1);
	*this = that;
	return that;
}

GrapaInt& GrapaInt::operator ++()
{
	GrapaInt result(*this);

	s64 carry = 1;
	s32 index = 0;

	while (carry)
	{
		s64 val = (s64)(result.GetItem(index));
		val++;

		result.SetItem(index, (u32)(val & 0xFFFFFFFF));
		carry = val >> 32;

		index++;
	}

	result.RTrim();

	*this = result;
	return *this;
}

GrapaInt GrapaInt::operator++(int) 
{ 
	GrapaInt tmp(*this); 
	operator++(); 
	return tmp; 
}

GrapaInt GrapaInt::operator -(const GrapaInt& bi2) const
{
	//GrapaInt bi2(bi);
	GrapaInt result;
	result.SetCount(((GetCount() > bi2.GetCount()) ? GetCount() : bi2.GetCount()) + ((dataSigned || bi2.dataSigned)?1:0));
	result.dataSigned = dataSigned;
	s64 carryIn = 0;
	for (s32 i = 0; i < result.GetCount(); i++)
	{
		s64 diff = (s64)GetItem(i) - (s64)bi2.GetItem(i) - carryIn;
		result.SetItem(i, (u32)(diff & 0xFFFFFFFF));
		if (diff < 0)
			carryIn = 1;
		else
			carryIn = 0;
	}
	if ((carryIn && !bi2.dataSigned)) // || (dataSigned && !bi2.dataSigned))
	{
		result.dataSigned = true;
		if (!result.IsNeg())
			result.SetItem(result.GetCount(), 0xFFFFFFFF);
	}
	result.RTrim();
	return result;
}

GrapaInt GrapaInt::operator -(s64 bi) const
{ 
	GrapaInt bi2(bi); 
	return *this - bi2; 
}

GrapaInt GrapaInt::operator -=(const GrapaInt& bi1)
{ 
	GrapaInt that(*this - bi1);
	*this = that;
	return that;
}

GrapaInt& GrapaInt::operator --()
{
	GrapaInt result(*this);

	s64 val;
	bool carryIn = true;
	s32 index = 0;

	while (carryIn)
	{
		val = (s64)(result.GetItem(index));
		val--;

		result.SetItem(index, (u32)(val & 0xFFFFFFFF));

		if (val >= 0)
			carryIn = false;

		index++;
	}

	result.RTrim();

	*this = result;
	return *this;
}

GrapaInt GrapaInt::operator--(int) 
{ 
	GrapaInt tmp(*this); 
	operator--(); 
	return tmp; 
}

// Maybe it's better to use faster multiplication algorithm like multiplication 
// using FFT (or FHT)? Take a look at my library 
// IntX: http://contest2005.gotdotnet.ru/Request/Tools/UtilitiesLib/Downloads_GetFile.aspx?id=169728[^] (there is also a description 
// in Russian: http://contest2005.gotdotnet.ru/Request/Tools/UtilitiesLib/169728.aspx[^]) 
// - it multiplies using FFT which takes O(N*log N) instead of classic O(N^2). 

// uses FFT and Newton's reciprocal convergence algorithms for multiplication and division. 

