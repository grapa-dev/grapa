// GrapaInt.cpp
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

#include "GrapaInt.h"
#include <math.h>

#include <openssl/rand.h>

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
u64 GrapaArray32::GetByteCount() {
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
bool GrapaArray32::IsNull() { return (GetCount() == 0); }
bool GrapaArray32::IsNeg() { return (GetCount() ? (GetItem(GetCount() - 1) & 0x80000000) != 0 : false); }
bool GrapaArray32::IsSignNeg() { return (dataSigned && IsNeg()); }
bool GrapaArray32::IsZero() { return (GetCount()==0 || (GetCount() == 1 && GetItem(0) == 0) ? true : false); }
bool GrapaArray32::IsItem(u32 pItem) { return (GetCount() == 1 && GetItem(0) == pItem ? true : false); }

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

GrapaInt GrapaInt::operator +(const GrapaInt& bi)
{
	GrapaInt result;
	GrapaInt bi2(bi);
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

GrapaInt GrapaInt::operator +(s64 bi) 
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

GrapaInt GrapaInt::operator -(const GrapaInt& bi)
{
	GrapaInt bi2(bi);
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

GrapaInt GrapaInt::operator -(s64 bi) 
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

// See “Numerical Recipes” www.nr.com on how to perform fast multiplication of arbitrary precision numbers

// Actually, FFT multiplication doesn't become competitive until you have many thousands of digits.
  
// A relatively simple method is Karatsuba multiplication.It uses recursion to replace one multiplication of numbers of length N with 3 multiplications of numbers of length N / 2. When N is small(around 10 - 20), you can switch to the classic algorithm.

// FFT Based Schönhage-Strassen algorithm used to be asymptotically fastest algorithm for multiplying large numbers (until slightly faster algorithm was introduced in 2007 (see Fürer algorithm)). Asymptotically slower algorithms such as Karatsuba and Toom–Cook will generally outperform FFT for small numbers. Threshold depends on implementation but it is generally believed that for numbers with more then 10,000 digits using FFT based multiplication is better. 

#include "GrapaFloat.h"

class GrapaComplex {
public:
	GrapaFloat real, img;
	GrapaComplex(const GrapaFloat& r, const GrapaFloat& i) {
		real = r; img = i;
	}
	GrapaComplex add(const GrapaComplex& c);
	GrapaComplex sub(const GrapaComplex& c);
	GrapaComplex mul(const GrapaComplex& c);
	GrapaComplex div(const GrapaComplex& c);
};

GrapaComplex GrapaComplex::add(const GrapaComplex& c) {
	return GrapaComplex(real + c.real, img + c.img);
}
GrapaComplex GrapaComplex::sub(const GrapaComplex& c) {
	return GrapaComplex(real - c.real, img - c.img);
}
GrapaComplex GrapaComplex::mul(const GrapaComplex& c) {
	return GrapaComplex(real * c.real - img * c.img, real * c.img + img * c.real);
}
GrapaComplex GrapaComplex::div(const GrapaComplex& c) {
	GrapaFloat cr = c.real;
	GrapaFloat ci = c.img;
	return GrapaComplex((real * c.real + img * c.img) / (cr * cr + ci * ci), (img * cr - real * ci) / (cr * cr + ci * ci));
}

#include <complex>
#include <vector>

const double PI = acos(-1);

static void fft(std::vector<std::complex<double>>& a, bool invert)
{
	int n = a.size();

	for (int i = 1, j = 0; i < n; i++) {
		int bit = n >> 1;
		for (; j & bit; bit >>= 1)
			j ^= bit;
		j ^= bit;

		if (i < j)
			swap(a[i], a[j]);
	}

	for (int len = 2; len <= n; len <<= 1) {
		double ang = 2 * PI / len * (invert ? -1 : 1);
		std::complex<double> wlen(cos(ang), sin(ang));
		for (int i = 0; i < n; i += len) {
			std::complex<double> w(1);
			for (int j = 0; j < len / 2; j++) {
				std::complex<double> u = a[i + j], v = a[i + j + len / 2] * w;
				a[i + j] = u + v;
				a[i + j + len / 2] = u - v;
				w *= wlen;
			}
		}
	}

	if (invert) {
		for (std::complex<double>& x : a)
			x /= n;
	}
}

static std::vector<s64> fftmult(std::vector<s64> const& a, std::vector<s64> const& b)
{
	std::vector<std::complex<double>> fa(a.begin(), a.end()), fb(b.begin(), b.end());
	int n = 1;
	while (n < a.size() + b.size())
		n <<= 1;
	fa.resize(n);
	fb.resize(n);

	fft(fa, false);
	fft(fb, false);
	for (int i = 0; i < n; i++)
		fa[i] *= fb[i];
	fft(fa, true);

	s64 carry = 0;
	std::vector<s64> result(n);
	for (int i = 0; i < n; i++)
	{
		result[i] = round(fa[i].real());
		result[i] += carry;
		carry = result[i] >> 32;
		result[i] &= 0xFFFFFFFF;
	}
	return result;
}

static GrapaInt FftMultiply(const GrapaInt& bi1, const GrapaInt& bi2)
{
	std::vector<s64> a(bi1.GetCount()),b(bi2.GetCount());
	for (int i = 0; i < bi1.GetCount(); i++)
		a[i] = bi1.GetItem(i);
	for (int i = 0; i < bi2.GetCount(); i++)
		b[i] = bi2.GetItem(i);
	std::vector<s64> c = fftmult(a, b);
	GrapaInt result;
	result.SetCount(c.size());
	for (int i = 0; i < c.size(); i++)
		result.SetItem(i, c[i]);
	result.RTrim();
	return result;
}

static GrapaInt GrapaMultiply(const GrapaInt& bi1, const GrapaInt& bi2)
{
	GrapaInt result;
	result.SetCount(bi1.GetCount() + bi2.GetCount());
	for (s32 i = 0; i < bi1.GetCount(); i++)
	{
		if (bi1.GetItem(i) == 0)
			continue;
		u32 mcarry = 0;
		for (s32 j = 0, k = i; j < bi2.GetCount(); j++, k++)
		{
			u64 val = ((u64)bi1.GetItem(i) * (u64)bi2.GetItem(j)) +
				(u64)result.GetItem(k) + mcarry;
			result.SetItem(k, (u32)(val & 0xFFFFFFFF));
			mcarry = (val >> 32);
		}
		if (mcarry)
			result.SetItem(i + bi2.GetCount(), mcarry);
	}
	result.RTrim();
	return result;
}

static GrapaInt KaratsubaMultiply(const GrapaInt& x, const GrapaInt& y)
{
	s64 xLength = x.GetCount();
	s64 yLength = y.GetCount();
	s64 N = (xLength > yLength) ? xLength : yLength;
	if (N <= 3)
		return GrapaMultiply(x,y);
	N = (N / 2) + (N % 2);
	GrapaInt Xl,Xr;
	Xl.SetCount((xLength-N)>0? (xLength - N): 0);
	Xr.SetCount(N);
	for (s32 j = 0; j < N; j++) Xr.SetItem(j, x.GetItem(j));
	for (s32 j = N; j < xLength; j++) Xl.SetItem(j-N, x.GetItem(j));
	GrapaInt Yl,Yr;
	Yl.SetCount((yLength - N) > 0 ? (yLength - N) : 0);
	Yr.SetCount(N);
	for (s32 j = 0; j < N; j++) Yr.SetItem(j, y.GetItem(j));
	for (s32 j = N; j < yLength; j++) Yl.SetItem(j - N, y.GetItem(j));
	Xl.RTrim();
	Xr.RTrim();
	Yl.RTrim();
	Yr.RTrim();
	GrapaInt P1 = KaratsubaMultiply(Xl, Yl);
	GrapaInt P2 = KaratsubaMultiply(Xr, Yr);
	GrapaInt Xa = Xl + Xr;
	GrapaInt Ya = Yl + Yr;
	GrapaInt P3 = KaratsubaMultiply(Xa, Ya);
	return (P1 << (2 * N * 32)) + ((P3 - P1 - P2) << (N * 32)) + P2;
}

#include <openssl/rsa.h>

static GrapaInt OpenSSLMultiply(const GrapaInt& bi1, const GrapaInt& bi2)
{
	GrapaInt result;
	GrapaBYTE gb;
	int err;
	BIGNUM *n1,*n2;
	u32 sz;
	gb = ((GrapaInt*)&bi1)->getBytes();
	n1 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	gb = ((GrapaInt*)&bi2)->getBytes();
	n2 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	BIGNUM* r = BN_new();
	BN_CTX* ctx = BN_CTX_new();
	err = BN_mul(r, n1, n2, ctx);
	BN_free(n1);
	BN_free(n2);
	BN_CTX_free(ctx);
	if (err == 1)
	{
		gb.SetSize(BN_num_bytes(r) + 1);
		sz = BN_bn2bin(r, (u8*)gb.mBytes);
		gb.SetLength(sz);
		result.FromBytes(gb, true);
		result.RTrim();
	}
	BN_free(r);
	return result;
}

// 123451234512345*123451234512345
// 15240207302622001229957399025

// 123451234512345*123451234512345123451234512345
// 15240207302622016470164701647001229957399025

// 2895083853334686141284026960293263455117935
// 
// (123451234512345 * 123451234512345123451234512345).hex();

GrapaInt GrapaInt::operator *(const GrapaInt& bi)
{
	if (IsZero()) return *this;
	GrapaInt bi2(bi);
	if (bi2.IsZero()) return bi2;
	
	GrapaInt bi1(*this);
	bool bi1Neg = false, bi2Neg = false;

	if (bi1.IsSignNeg())     // bi1 negative
	{
		bi1Neg = true; 
		bi1 = -bi1;
	}
	if (bi2.IsSignNeg())     // bi2 negative
	{
		bi2Neg = true; 
		bi2 = -bi2;
	}

	GrapaInt result = OpenSSLMultiply(bi1, bi2);

	if (bi1Neg != bi2Neg)
		result = -result;

	return result;
}

GrapaInt GrapaInt::operator *(s64 bi) 
{ 
	GrapaInt bi2(bi); 
	return *this*bi2; 
}

GrapaInt GrapaInt::operator *(u64 bi) 
{ 
	GrapaInt bi2(bi); 
	return *this*bi2; 
}

GrapaInt GrapaInt::operator <<(u64 shiftVal)
{
	GrapaInt result(*this);
	u32 oldCount = (u32)result.GetCount();
	result.GrowCount(oldCount + shiftVal/sizeof(32) + 1);
	int newCount = GrapaInt::shiftLeft((u32*)result.mBytes, oldCount, shiftVal);
	result.SetCount(newCount);
	return result;
}

int GrapaInt::shiftLeft(u32* inData, u32 inCount, u64 shiftVal)
{
	u64 shiftAmount = 32;
	u32 bufLen = inCount;

	while (bufLen > 1 && inData[bufLen - 1] == 0)
		bufLen--;

	for (u64 count = shiftVal; count > 0;)
	{
		if (count < shiftAmount)
			shiftAmount = count;

		u64 carry = 0;
		for (u32 i = 0; i < bufLen; i++)
		{
			u64 val = ((u64)inData[i]) << shiftAmount;
			val |= carry;

			inData[i] = (u32)(val & 0xFFFFFFFF);
			carry = val >> 32;
		}

		if (carry != 0)
		{
			if (bufLen + 1 >= inCount)
			{
				inData[bufLen] = (u32)carry;
				bufLen++;
			}
		}
		count -= shiftAmount;
	}
	return bufLen;
}

GrapaInt GrapaInt::operator >>(u64 shiftVal)
{
	GrapaInt result(*this);
	int newCount = shiftRight((u32*)result.mBytes, (u32)result.GetCount(), shiftVal);
	result.SetCount(newCount);
	//if (mData.GetCount() && (GetItem(mData.GetCount() - 1) & 0x80000000) != 0) // negative
	//{
	//	for (s32 i = maxLength - 1; i >= result.dataLength; i--)
	//		result.data[i] = 0xFFFFFFFF;

	//	u32 mask = 0x80000000;
	//	for (int i = 0; i < 32; i++)
	//	{
	//		if ((result.data[result.dataLength - 1] & mask) != 0)
	//			break;

	//		result.data[result.dataLength - 1] |= mask;
	//		mask >>= 1;
	//	}
	//	result.dataLength = maxLength;
	//}

	return result;
}

int GrapaInt::shiftRight(u32* inData, u32 inCount, u64 shiftVal)
{
	u64 shiftAmount = 32;
	u64 invShift = 0;
	s32 bufLen = inCount;

	while (bufLen > 1 && inData[bufLen - 1] == 0)
		bufLen--;

	for (u64 count = shiftVal; count > 0;)
	{
		if (count < shiftAmount)
		{
			shiftAmount = count;
			invShift = 32 - shiftAmount;
		}

		u64 carry = 0;
		for (s32 i = bufLen - 1; i >= 0; i--)
		{
			u64 val = ((u64)inData[i]) >> shiftAmount;
			val |= carry;

			carry = (((u64)inData[i]) << invShift) & 0xFFFFFFFF;
			inData[i] = (u32)(val);
		}

		count -= shiftAmount;

		while (bufLen > 1 && inData[bufLen - 1] == 0)
			bufLen--;
	}

	return bufLen;
}

GrapaInt GrapaInt::operator ~()
{
	GrapaInt result;

	result.SetCount(GetCount());
	for (int i = 0; i < GetCount(); i++)
		result.SetItem(i , (u32)(~(GetItem(i))));

	//result.RTrim();

	result.dataSigned = dataSigned;

	return result;
}

GrapaInt GrapaInt::operator -()
{
	GrapaInt result;
	result.SetItem(0, 0);
	return result - *this;
}

bool GrapaInt::operator ==(const GrapaInt& bi2)
{
	return Equals(bi2);
}

bool GrapaInt::operator !=(const GrapaInt& bi2)
{
	return !(Equals(bi2));
}

bool GrapaInt::Equals(const GrapaInt& bi)
{
	GrapaInt bi2(bi);
	if (GetCount() != bi2.GetCount())
		return false;

	for (s32 i = 0; i < GetCount(); i++)
	{
		if (GetItem(i) != bi2.GetItem(i))
			return false;
	}
	return true;
}

bool GrapaInt::operator >(const GrapaInt& bi)
{
	GrapaInt bi2(bi);

	bool aNeg = IsSignNeg();
	bool bNeg = bi2.IsSignNeg();

	if (aNeg && !bNeg)
		return false;

	if (!aNeg && bNeg)
		return true;

	// same sign
	s64 len = (GetCount() > bi2.GetCount()) ? GetCount() : bi2.GetCount();
	s64 pos = 0;
	for (pos = len - 1; pos >= 0 && GetItem(pos) == bi2.GetItem(pos); pos--);

	if (pos >= 0)
	{
		if (GetItem(pos) > bi2.GetItem(pos))
			return true;
		return false;
	}
	return false;
}

bool GrapaInt::operator >(u64 i1) 
{ 
	GrapaInt bi1(i1); 
	return *this > bi1; 
}

bool GrapaInt::operator <(const GrapaInt& bi)
{
	GrapaInt bi2(bi);

	bool aNeg = IsSignNeg();
	bool bNeg = bi2.IsSignNeg();

	if (aNeg && !bNeg)
		return true;

	if (!aNeg && bNeg)
		return false;

	// same sign
	s64 len = (GetCount() > bi2.GetCount()) ? GetCount() : bi2.GetCount();
	s64 pos = len - 1;
	while (pos >= 0 && GetItem(pos) == bi2.GetItem(pos))
		pos--;

	if (pos >= 0)
	{
		if (GetItem(pos) < bi2.GetItem(pos))
			return true;
		return false;
	}
	return false;
}

bool GrapaInt::operator <(s64 bi) 
{ 
	GrapaInt bi2(bi); 
	return *this < bi2; 
}

bool GrapaInt::operator >= (const GrapaInt& bi2)
{
	return (*this == bi2 || *this > bi2);
}

bool GrapaInt::operator <= (const GrapaInt& bi2)
{
	return (*this == bi2 || *this < bi2);
}

GrapaInt GrapaInt::Abs()
{
	if (dataSigned) return -*this;
	return *this;
}

void GrapaInt::multiByteDivide(const GrapaInt& bi, GrapaInt& outQuotient, GrapaInt& outRemainder)
{
	GrapaInt bi2(bi);
	outQuotient.SetCount(0);
	outRemainder.SetCount(0);

	if (IsZero()) return;
	if (bi2.IsZero()) return;  // it's actually infinity...or undefined.

	GrapaInt result;
	GrapaBYTE gb;
	int err;
	BIGNUM* n1, * n2;
	u32 sz;
	gb = getBytes();
	n1 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	gb = ((GrapaInt*)&bi)->getBytes();
	n2 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);

	int BN_div(BIGNUM * dv, BIGNUM * rem, const BIGNUM * m, const BIGNUM * d,
		BN_CTX * ctx);

	BIGNUM* dv = BN_new();
	BIGNUM* rem = BN_new();
	BN_CTX* ctx = BN_CTX_new();
	err = BN_div(dv,rem, n1, n2, ctx);
	BN_free(n1);
	BN_free(n2);
	BN_CTX_free(ctx);
	if (err == 1)
	{
		gb.SetSize(BN_num_bytes(dv) + 1);
		sz = BN_bn2bin(dv, (u8*)gb.mBytes);
		gb.SetLength(sz);
		outQuotient.FromBytes(gb, true);
		outQuotient.RTrim();
		gb.SetSize(BN_num_bytes(rem) + 1);
		sz = BN_bn2bin(rem, (u8*)gb.mBytes);
		gb.SetLength(sz);
		outRemainder.FromBytes(gb, true);
		outRemainder.RTrim();
	}
	BN_free(dv);
	BN_free(rem);
	return;

	outQuotient = 0;
	outRemainder = *this;
	u64 bits = bi2.bitCount();
	GrapaInt cbits;
	while (outRemainder.mLength >= bi2.mLength && outRemainder >= bi2)
	{
		cbits = outRemainder >> (int)bits;
		if (cbits.GetCount() <= 1 && cbits.GetItem(0) == 0) break;
		outQuotient += cbits;
		outRemainder -= (bi2 * cbits);
	}
	while (outRemainder >= bi2)
	{
		outQuotient++;
		outRemainder -= bi2;
	}
	return;


	//GrapaInt bi1(*this);
	//u32* result = (u32*)malloc(bi2.GetCount()*2*sizeof(u32));

	//int remainderLen = bi1.GetCount() + 1;
	//u32* remainder = (u32*)malloc(remainderLen * 2 * sizeof(u32));

	//u32 mask = 0x80000000;
	//u32 val = bi2.GetItem(bi2.GetCount() - 1);
	//s32 shift = 0, resultPos = 0;

	//while (mask != 0 && (val & mask) == 0)
	//{
	//	shift++; mask >>= 1;
	//}

	//for (s32 i = 0; i < bi1.GetCount(); i++)
	//	remainder[i] = bi1.GetItem(i);
	//shiftLeft(remainder, remainderLen, shift);
	//bi2 = bi2 << shift;

	//s32 j = remainderLen - bi2.GetCount();
	//s32 pos = remainderLen - 1;

	//u64 firstDivisorByte = bi2.GetItem(bi2.GetCount() - 1);
	//u64 secondDivisorByte = bi2.GetItem(bi2.GetCount() - 2);

	//int divisorLen = bi2.GetCount() + 1;
	//u32* dividendPart = (u32*)malloc(divisorLen * 2 * sizeof(u32));

	//while (j > 0)
	//{
	//	u64 dividend = (((u64)remainder[pos]) << 32) | remainder[pos - 1];

	//	u64 q_hat = dividend / firstDivisorByte;
	//	u64 r_hat = dividend % firstDivisorByte;

	//	bool done = false;
	//	while (!done)
	//	{
	//		done = true;

	//		if (q_hat == 0x100000000 ||
	//			(q_hat * secondDivisorByte) > ((r_hat << 32) | remainder[pos - 2]))
	//		{
	//			q_hat--;
	//			r_hat += firstDivisorByte;

	//			if (r_hat < 0x100000000)
	//				done = false;
	//		}
	//	}

	//	for (s32 h = 0; h < divisorLen; h++)
	//		dividendPart[h] = remainder[pos - h];

	//	GrapaInt kk = (u64)dividendPart;
	//	GrapaInt ss = bi2 * (s64)q_hat;

	//	u32 bits = bi2.bitCount();
	//	GrapaInt sd;
	//	while (ss > kk)
	//	{
	//		sd = (ss - kk) >> bits;
	//		if (sd.GetCount() <= 1 && sd.GetItem(0) == 0) break;
	//		q_hat -= (u64)sd.GetItem(0);
	//		ss -= (bi2 * (u64)sd.GetItem(0));
	//	}
	//	while (ss > kk)
	//	{
	//		q_hat--;
	//		ss -= bi2;
	//	}
	//	GrapaInt yy = kk - ss;

	//	for (s32 h = 0; h < divisorLen; h++)
	//		remainder[pos - h] = yy.GetItem(bi2.GetCount() - h);

	//	result[resultPos++] = (u32)q_hat;

	//	pos--;
	//	j--;
	//}

	//outQuotient.SetCount(resultPos);
	//s32 y = 0;
	//for (s32 x = outQuotient.GetCount() - 1; x >= 0; x--, y++)
	//	outQuotient.SetItem(y,result[x]);
	//for (; y < outQuotient.GetCount(); y++)
	//	outQuotient.SetItem(y,0);
	//outQuotient.RTrim();

	//outRemainder.SetCount(shiftRight(remainder, remainderLen, shift));
	//for (y = 0; y < outRemainder.GetCount(); y++)
	//	outRemainder.SetItem(y,remainder[y]);
	//for (; y < outRemainder.GetCount(); y++)
	//	outRemainder.SetItem(y, 0);
	//outQuotient.RTrim();

	//free(result);
	//free(remainder);
	//free(dividendPart);
}

void GrapaInt::singleByteDivide(const GrapaInt& bi, GrapaInt& outQuotient, GrapaInt& outRemainder)
{
	GrapaInt bi2(bi);
	outQuotient.SetCount(0);
	outRemainder.SetCount(0);

	if (IsZero()) return;
	if (bi2.IsZero()) return;  // it's actually infinity...or undefined.

	GrapaInt bi1(*this);

	u64 resultSize = bi1.GetCount() * sizeof(u32);
	if (resultSize < 12) resultSize = 12;
	u32* result = (u32*)malloc(resultSize);
	if (result == 0L) return;
	memset(result, 0, resultSize);
	int resultPos = 0;

	// copy dividend to reminder
	for (u32 i = 0; i < bi1.GetCount(); i++)
		outRemainder.SetItem(i, bi1.GetItem(i));
	outRemainder.SetCount(bi1.GetCount());

	outRemainder.RTrim();

	u64 divisor = (u64)bi2.GetItem(0);
	s64 pos = outRemainder.GetCount() ? outRemainder.GetCount() - 1 : 0;
	u64 dividend = (u64)outRemainder.GetItem(pos);

	if (dividend >= divisor)
	{
		u64 quotient = dividend / divisor;
		result[resultPos++] = (u32)quotient;

		outRemainder.SetItem(pos, (u32)(dividend % divisor));
	}
	pos--;

	while (pos >= 0)
	{
		dividend = (((u64)outRemainder.GetItem(pos + 1)) << 32) | (outRemainder.GetItem(pos) & 0xFFFFFFFF);
		u64 quotient = dividend / divisor;
		result[resultPos++] = (u32)quotient;

		outRemainder.SetItem(pos + 1, 0);
		outRemainder.SetItem(pos--, (u32)(dividend % divisor));
	}

	outQuotient.SetCount(resultPos);
	pos = outQuotient.GetCount() ? outQuotient.GetCount() - 1 : 0;
	s64 j = 0;
	for (s64 i = pos; i >= 0; i--, j++)
		outQuotient.SetItem(j, result[i]);
	for (; j < resultPos; j++)
		outQuotient.SetItem(j, 0);

	outQuotient.RTrim();
	outRemainder.RTrim();

	free(result);
}

GrapaInt GrapaInt::Div(const GrapaInt& bi, GrapaInt& remainder)
{
	GrapaInt bi1(*this);
	GrapaInt bi2(bi);
	GrapaInt quotient(0);
	remainder = *this;

	if (IsZero()) 
		return quotient;
	if (bi2.IsZero())
	{
		quotient.SetCount(0);
		quotient.NaN = true;
		return quotient; // it's actually infinity...or undefined.
	}

	bool divisorNeg = false, dividendNeg = false;

	if (bi1.IsSignNeg())     // bi1 negative
	{
		quotient.dataSigned = true;
		dividendNeg = true;
		bi1 = -bi1;
	}
	if (bi2.IsSignNeg())     // bi2 negative
	{
		quotient.dataSigned = true;
		divisorNeg = true;
		bi2 = -bi2;
	}

	if (bi1 < bi2)
	{
		quotient.dataSigned = false;
		return quotient;
	}

	if (bi2.GetCount() <= 1)
		bi1.singleByteDivide(bi2, quotient, remainder);
	else
		bi1.multiByteDivide(bi2, quotient, remainder);

	if (quotient.IsZero())
		quotient.dataSigned = false;
	if (remainder.IsZero())
		remainder.dataSigned = false;

	if (dividendNeg != divisorNeg)
	{
		if (!remainder.IsZero())
			remainder = -remainder;
		return -quotient;
	}

	return quotient;
}


GrapaInt GrapaInt::operator /(const GrapaInt& bi)
{
	GrapaInt remainder;
	return Div(bi,remainder);
}

GrapaInt GrapaInt::operator /(u64 bi) 
{ 
	GrapaInt bi2(bi); 
	return *this / bi2; 
}

GrapaInt GrapaInt::operator %(const GrapaInt& pbi2)
{
	GrapaInt remainder;
	Div(pbi2, remainder);
	return remainder;

	//GrapaInt bi1(*this);
	//GrapaInt bi2(pbi2);
	//GrapaInt quotient;// = new BigInteger();
	//GrapaInt remainder(*this);// = new BigInteger(bi1);

	//bool dividendNeg = false;

	//if (bi1.IsSignNeg())     // bi1 negative
	//{
	//	bi1 = -bi1;
	//	dividendNeg = true;
	//}
	//if (bi2.IsSignNeg())     // bi2 negative
	//{
	//	bi2 = -bi2;
	//}

	//if (bi1 < bi2)
	//{
	//	return remainder;
	//}

	//else
	//{
	//	if (bi2.GetCount() <= 1)
	//		bi1.singleByteDivide(bi2, quotient, remainder);
	//	else
	//		bi1.multiByteDivide(bi2, quotient, remainder);

	//	if (dividendNeg)
	//		return -remainder;

	//	return remainder;
	//}
}

GrapaInt GrapaInt::operator %(u64 bi) 
{ 
	GrapaInt bi2(bi); 
	return *this % bi2; 
}

GrapaInt GrapaInt::operator %=(const GrapaInt& bi1)
{ 
	*this = *this % bi1;
	return *this;
}

//GrapaInt GrapaInt::operator |(const GrapaInt& bi2)
//{
//	GrapaInt result;// = new BigInteger();
//
//	u32 len = (dataLength > bi2.dataLength) ? dataLength : bi2.dataLength;
//
//	for (u32 i = 0; i < len; i++)
//	{
//		u32 sum = (u32)(data[i] | bi2.data[i]);
//		result.data[i] = sum;
//	}
//
//	result.dataLength = maxLength;
//
//	while (result.dataLength > 1 && result.data[result.dataLength - 1] == 0)
//		result.dataLength--;
//
//	return result;
//}

//GrapaInt GrapaInt::operator ^(const GrapaInt& pexp)
//{
//}

GrapaInt GrapaInt::Pow(const GrapaInt& pexp)
{
	GrapaInt result((u64)1);
	GrapaInt tempNum;

	GrapaInt exp(pexp);
	if (exp.IsSignNeg())
	{
		tempNum.NaN = true;
		return tempNum; // throw an error
	}

	bool thisNegative = false;

	if (IsSignNeg())   // negative this
	{
		result.dataSigned = true;
		tempNum = -*this;
		thisNegative = true;
	}
	else
		tempNum = *this;

	GrapaBYTE gb;
	int err;
	BIGNUM* n1, * n2;
	u32 sz;
	gb = ((GrapaInt*)&tempNum)->getBytes();
	n1 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	gb = ((GrapaInt*)&exp)->getBytes();
	n2 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	BIGNUM* r = BN_new();
	BN_CTX* ctx = BN_CTX_new();
	err = BN_exp(r, n1, n2, ctx);
	BN_free(n1);
	BN_free(n2);
	BN_CTX_free(ctx);
	if (err == 1)
	{
		gb.SetSize(BN_num_bytes(r) + 1);
		sz = BN_bn2bin(r, (u8*)gb.mBytes);
		gb.SetLength(sz);
		result.FromBytes(gb, true);
		result.RTrim();
	}
	BN_free(r);

	//u64 totalBits = exp.bitCount();
	//u64 count = 0;

	//for (u64 pos = 0; pos < exp.GetCount(); pos++)
	//{
	//	u32 mask = 0x01;

	//	for (u32 index = 0; index < 32; index++)
	//	{
	//		if ((exp.GetItem(pos) & mask) != 0)
	//			result = result * tempNum;

	//		mask <<= 1;

	//		tempNum = tempNum * tempNum;

	//		if (tempNum.IsItem(1))
	//		{
	//			if (thisNegative && (exp.GetItem(0) & 0x1) != 0)    //odd exp
	//				return -result;
	//			return result;
	//		}
	//		count++;
	//		if (count >= totalBits)
	//			break;
	//	}
	//}

	if (thisNegative && (exp.GetItem(0) & 0x1) != 0)    //odd exp
		result = -result;

	return result;
}

GrapaInt GrapaInt::operator |(const GrapaInt& e)
{
	GrapaInt exp(e);
	GrapaInt result;
	u64 minLen = (*this).GetCount() > exp.GetCount() ? exp.GetCount() : (*this).GetCount();

	result.dataSigned = dataSigned;

	for (u64 pos = 0; pos < minLen; pos++)
		result.SetItem(pos, (*this).GetItem(pos) | exp.GetItem(pos));

	if ((*this).GetCount() > exp.GetCount())
		for (u64 pos = minLen; pos < (*this).GetCount(); pos++)
			result.SetItem(pos, (*this).GetItem(pos));
	else
		for (u64 pos = minLen; pos < exp.GetCount(); pos++)
			result.SetItem(pos, exp.GetItem(pos));

	result.SetCount((*this).GetCount() < exp.GetCount() ? exp.GetCount() : (*this).GetCount());
	result.RTrim();
	return result;
}

GrapaInt GrapaInt::operator &(const GrapaInt& e)
{
	GrapaInt exp(e);
	GrapaInt result;
	u64 minLen = (*this).GetCount() > exp.GetCount() ? exp.GetCount() : (*this).GetCount();
	result.dataSigned = dataSigned;
	for (u32 pos = 0; pos < minLen; pos++)
		result.SetItem(pos, (*this).GetItem(pos) & exp.GetItem(pos));
	result.SetCount(minLen);
	result.RTrim();
	return result;
}

GrapaInt GrapaInt::operator ^(const GrapaInt& e)
{
	GrapaInt exp(e);
	GrapaInt result;
	u64 minLen = (*this).GetCount() > exp.GetCount() ? exp.GetCount() : (*this).GetCount();
	result.dataSigned = dataSigned;

	for (u32 pos = 0; pos < minLen; pos++)
		result.SetItem(pos, (*this).GetItem(pos) ^ exp.GetItem(pos));

	if ((*this).GetCount() > exp.GetCount())
	{
		for (u64 pos = minLen; pos < (*this).GetCount(); pos++)
			result.SetItem(pos, (*this).GetItem(pos) ^ 0);
	}
	else
	{
		for (u64 pos = minLen; pos < exp.GetCount(); pos++)
			result.SetItem(pos, exp.GetItem(pos) ^ 0);
	}
	result.SetCount((*this).GetCount() < exp.GetCount() ? exp.GetCount() : (*this).GetCount());
	result.RTrim();
	return result;
}


GrapaInt GrapaInt::bmax(const GrapaInt& bi)
{
	if (*this > bi)
		return *this;
	else
		return bi;
}

GrapaInt GrapaInt::bmin(const GrapaInt& bi)
{
	if (*this < bi)
		return *this;
	else
		return bi;

}

GrapaInt GrapaInt::babs()
{
	if (IsSignNeg())
		return (-*this);
	else
		return *this;
}

d64 GrapaInt::blog()
{
	u64 bits = bitCount();
	u64 blex = bits > 64 ? bits - 64 : 0;
	GrapaInt num = *this >> blex;
	return log(((u64)num.GetItem(1) << 32) | ((u64)num.GetItem(0))) + blex * log((d64)2.0);
}

d64 GrapaInt::blog2()
{
	// Get the minimum number of bits necessary to hold this value.
	u64 n = bitCount();

	// Calculate the double-precision fraction of this number; as if the
	// binary point was left of the most significant '1' bit.
	// (Get the most significant 53 bits and divide by 2^53)
	u64 mask = (u64)1 << 62; // mantissa is 53 bits (including hidden bit)
	u64 mantissa = 0;
	s64 j = 0;
	for (u32 i = 1; i < 64; i++)
	{
		j = n - i;
		if (j < 0) break;
		if (testBit((u64)j)) mantissa |= mask;
		mask >>= 1;
	}
	// Round up if next bit is 1.
	if (j > 0 && testBit((u64)(j - 1))) mantissa++;

	d64 f = (d64)mantissa / (d64)((u64)1 << 62);

	// Add the logarithm to the number of bits, and subtract 1 because the
	// number of bits is always higher than necessary for a number
	// (ie. log2(val)<n for every val).
	return ((d64)n - 1.0 + log(f) * 1.44269504088896340735992468100189213742664595415298);
	// Magic number converts from base e to base 2 before adding. For other
	// bases, correct the result, NOT this number!
}

d64 GrapaInt::blog10()
{
	return (blog() * 1000) / (log((d64)10.0) * 1000);
}

GrapaInt GrapaInt::LargestPrimeFactor() // largest prime factor
{
	GrapaInt n(*this);
	if (n.GetCount() <= 1 && n.GetItem(0) < 2) return 1;
	GrapaInt r = n, p;
	if (r % 2 == 0)
	{
		p = 2;
		do { r = r / 2; } while (r % 2 == 0);
	}
	GrapaInt i;
	for (i = 3; i <= r; i += 2)
	{
		if (r % i == 0)
		{
			p = i;
			do { r = r / i; } while (r % i == 0);
		}
	}
	return p;
}

u64 GrapaInt::LargestPrimeFactor64(u64 n)
{
	if (n < 2) return 1;

	u64 r = n, p=1;
	if (r % 2 == 0)
	{
		p = 2;
		do { r /= 2; } while (r % 2 == 0);
	}
	u64 i;
	for (i = 3; i <= r; i += 2)
	{
		if (r % i == 0)
		{
			p = i;
			do { r /= i; } while (r % i == 0);
		}
	}
	return p;
}

GrapaCHAR GrapaInt::ToString()
{
	return ToString(10);
}

//***********************************************************************
// Returns a string representing the BigInteger in sign-and-magnitude
// format in the specified radix.
//
// Example
// -------
// If the value of BigInteger is -255 in base 10, then
// ToString(16) returns "-FF"
//
//***********************************************************************

GrapaCHAR GrapaInt::ToString(u64 radix)
{
	GrapaCHAR result;
	if (radix < 2 || radix > 64)
		return result;// (new ArgumentException("Radix must be >= 2 and <= 36"));

	const char* charSet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
	//http://base64.sourceforge.net/b64.c

	GrapaInt a = *this;

	bool negative = false;

	if (a.IsSignNeg())
	{
		negative = true;
		a = -a;
	}

	GrapaInt quotient;
	GrapaInt remainder;
	GrapaInt biRadix(radix);

	if (a.IsZero())
	{
		result.FROM("0");
		return result;
	}

	while (a.GetCount() > 1 || (a.GetCount() == 1 && a.GetItem(0) != 0))
	{
		a.singleByteDivide(biRadix, quotient, remainder);
		//if (remainder.GetItem(0) < 10)
		//	result.Append((u64)remainder.GetItem(0));
		//else
		//	result.Append(charSet[(u32)remainder.GetItem(0) - 10]);
		result.Append(charSet[(u32)remainder.GetItem(0)]);
		a = quotient;
	}

	if (negative)
		result.Append('-');

	for (u64 i = 0; i < result.mLength / 2; i++)
	{
		u8 c = result.mBytes[i];
		result.mBytes[i] = result.mBytes[result.mLength - i - 1];
		result.mBytes[result.mLength - i - 1] = c;
	}

	return result;
}

void GrapaInt::FromString(const GrapaBYTE& result, u32 radix)
{
	*this = 0;
	if (radix < 2 || radix > 64 || result.mLength == 0) return;
	u64 pos = result.mLength;
	GrapaInt r((u64)radix);
	GrapaInt n;
	bool isNeg = false;
	while (pos--)
	{
		char c = ((char*)result.mBytes)[result.mLength-pos-1];
		n = 0;
		if (c >= '0' && c <= '9')
			n.SetItem(0, c - '0');
		else if (c >= 'A' && c <= 'Z')
			n.SetItem(0, 10 + c - 'A');
		else if (c >= 'a' && c <= 'z')
			n.SetItem(0, 36 + c - 'a');
		else if (c == '+')
			n.SetItem(0, 62);
		else if (c == '/')
			n.SetItem(0, 63);
		else if (c == '-')
		{
			isNeg = true;
			continue;
		}
		else
			c = 0;
		if (c == 0 || n.GetItem(0) > (radix - 1))
		{
			//error
			//*this = 0;
			return;
		}
		if (pos) n = n * (r.Pow(pos));
		*this = *this + n;
	}
	if (isNeg) *this = -*this;
}

//void GrapaInt::ToBytes(GrapaBYTE& result)
//{
//	if (dataLength < 1) return;
//	result.GrowSize(dataLength*sizeof(u32)+1, true);
//	result.SetLength(dataLength*sizeof(u32));
//	for (s32 i = dataLength - 1, j = 0; i >= 0; i--, j++)
//		((u32*)result.mBytes)[i] = BE_S32(data[j]);
//	result.mPos = 0;
//}

void GrapaInt::FromBytes(const GrapaBYTE& result, bool isUnsigned)
{
	u32 processed;
	SetCount(1);
	SetItem(0, 0);
	dataSigned = false;
	bool isNeg = false;
	if (result.mLength == 0 || result.mBytes == NULL) return;
	if ((result.mToken == GrapaTokenType::INT || result.mToken == GrapaTokenType::TIME || result.mToken == GrapaTokenType::RAW) && result.mBytes && (((u8*)result.mBytes)[0] & 0x80) == 0x80)
		isNeg = true;
	if (isUnsigned)
		isNeg = false;
	SetCount(result.mLength / sizeof(u32));
	processed = (u32)GetCount() * sizeof(u32);
	u32 rem = result.mLength % sizeof(u32);
	if (GetCount())
	{
		u8* input = &((u8*)result.mBytes)[rem];
		for (s64 i = GetCount() - 1, j = 0; i >= 0; i--, j++)
			SetItem(i, BE_S32(((u32*)input)[j]));
	}
	if (rem)
	{
		u8* s = ((u8*)result.mBytes);
		u32 r = (isNeg?-1:0) << (rem*8);
		for (u32 i = rem, j = 0; i--; j += 8)
			r |= s[i] << j;
		SetItem(GetCount(), r);
		processed += rem;
	}
	if (isNeg)
	{
		dataSigned = true;
		if (!IsNeg())
			SetItem(GetCount(), -1);
	}
	RTrim();
}

//void GrapaInt::ToBytes(GrapaBYTE& result)
//{
//	if (dataLength < 1) return;
//	u32 rem = dataLength % sizeof(u32);
//	result.GrowSize(dataLength*sizeof(u32)+1, true);
//	result.SetLength(dataLength*sizeof(u32));
//	if (rem)
//	{
//		result.SetLength(result.mLength - sizeof(u32) + rem);
//		u8* s = ((u8*)result.mBytes);
//		for (u32 i = rem, j = 0; i--; j += 8)
//			s[i] = (data[dataLength - 1] >> j) & 0xFF;
//	}
//	for (s32 i = dataLength - 2, j = 0; i >= 0; i--, j++)
//	{
//		u8* s = &((u8*)result.mBytes)[rem];
//		((u32*)s)[i] = BE_S32(data[j]);
//	}
//	result.mPos = 0;
//}
//
//void GrapaInt::FromBytes(const GrapaBYTE& result, u32& processed)
//{
//	dataLength = 1;
//	memset(data, 0, maxLength * sizeof(u32));
//	if (result.mLength == 0 || result.mBytes == NULL) return;
//	processed = result.mLength;
//	processed = processed % ((maxLength - 1) * sizeof(u32));
//	dataLength = processed / sizeof(u32);
//	u32 rem = processed % sizeof(u32);
//	if (rem) dataLength++;
//	for (s32 i = dataLength - 2, j = 0; i >= 0; i--, j++)
//	{
//		u8* s = &((u8*)result.mBytes)[rem];
//		data[i] = BE_S32(((u32*)s)[i]);
//	}
//	if (rem)
//	{
//		u8* s = ((u8*)result.mBytes);
//		for (u32 i = rem, j = 0; i--; j += 8)
//			data[dataLength - 1] |= s[i] << j;
//	}
//	else
//	{
//		data[0] = BE_S32(((u32*)result.mBytes)[dataLength - 1]);
//	}
//}

GrapaInt GrapaInt::modPow(const GrapaInt& pexp, const GrapaInt& pn)
{
	GrapaInt resultNum((u64)1);
	GrapaInt exp(pexp);
	GrapaInt n(pn);

	if (exp.IsSignNeg())
		return resultNum; // throw (new ArithmeticException("Positive exponents only."));

	GrapaInt tempNum;
	bool thisNegative = false;

	if (n.IsSignNeg())   // negative n
		n = -n;

	if (IsSignNeg())   // negative this
	{
		tempNum = -*this % n;
		thisNegative = true;
	}
	else
		tempNum = *this % n;  // ensures (tempNum * tempNum) < b^(2k)

	// calculate constant = b^(2k) / m
	GrapaInt constant;

	constant.SetItem(n.GetCount() << 1, 1);

	constant = constant / n;
	u64 totalBits = exp.bitCount();
	u64 count = 0;

	// perform squaring and multiply exponentiation
	for (s32 pos = 0; pos < exp.GetCount(); pos++)
	{
		u32 mask = 1;

		for (u32 index = 0; index < 32; index++)
		{
			if (exp.GetItem(pos) & mask)
				resultNum = BarrettReduction(resultNum * tempNum, n, constant);

			mask <<= 1;

			tempNum = BarrettReduction(tempNum * tempNum, n, constant);

			if (tempNum.IsItem(1))
			{
				if (thisNegative && (exp.GetItem(0) & 1))    //odd exp
					resultNum = -resultNum;
				if (resultNum.IsZero()) 
					resultNum.SetItem(0, 1);
				return resultNum;
			}
			count++;
			if (count == totalBits)
				break;
		}
	}

	if (thisNegative && (exp.GetItem(0) & 1))    //odd exp
		resultNum = -resultNum;

	if (resultNum.IsZero()) 
		resultNum.SetItem(0, 1);

	resultNum.RTrim();

	return resultNum;
}



//***********************************************************************
// Fast calculation of modular reduction using Barrett's reduction.
// Requires x < b^(2k), where b is the base.  In this case, base is
// 2^32 (uint).
//
// Reference [4]
//***********************************************************************

GrapaInt GrapaInt::BarrettReduction(const GrapaInt& px, const GrapaInt& pn, const GrapaInt& constant)
{
	GrapaInt x(px);
	GrapaInt n(pn);
	u64 k = n.GetCount(),
		kPlusOne = k + 1,
		kMinusOne = k - 1;

	GrapaInt q1;

	// q1 = x / b^(k-1)
	for (u64 i = kMinusOne, j = 0; i < x.GetCount(); i++, j++)
		q1.SetItem(j, x.GetItem(i));
	q1.SetCount(x.GetCount() > kMinusOne ? x.GetCount() - kMinusOne : 1);

	GrapaInt q2(q1 * constant);// = q1 * constant;
	GrapaInt q3;// = new BigInteger();

	// q3 = q2 / b^(k+1)
	for (u64 i = kPlusOne, j = 0; i < q2.GetCount(); i++, j++)
		q3.SetItem(j, q2.GetItem(i));
	q3.SetCount(q2.GetCount() > kPlusOne ? q2.GetCount() - kPlusOne : 1);


	// r1 = x mod b^(k+1)
	// i.e. keep the lowest (k+1) words
	GrapaInt r1;
	u64 lengthToCopy = (x.GetCount() > kPlusOne) ? kPlusOne : x.GetCount();
	for (u64 i = 0; i < lengthToCopy; i++)
		r1.SetItem(i, x.GetItem(i));
	r1.SetCount(lengthToCopy);


	// r2 = (q3 * n) mod b^(k+1)
	// partial multiplication of q3 and n

	GrapaInt r2;
	for (s32 i = 0; i < q3.GetCount(); i++)
	{
		if (q3.GetItem(i) == 0)     continue;

		u64 mcarry = 0;
		s32 t = i;
		for (s32 j = 0; j < n.GetCount() && t < kPlusOne; j++, t++)
		{
			// t = i + j
			u64 val = ((u64)q3.GetItem(i) * (u64)n.GetItem(j)) +
				(u64)r2.GetItem(t) + mcarry;

			r2.SetItem(t, (u32)(val & 0xFFFFFFFF));
			mcarry = (val >> 32);
		}

		if (t < kPlusOne)
			r2.SetItem(t, (u32)mcarry);
	}
	r2.SetCount(kPlusOne);
	r2.RTrim();

	r1 -= r2;
	if (r1.IsSignNeg())        // negative
	{
		GrapaInt val;
		val.SetItem(kPlusOne, 1);
		r1 += val;
	}

	if (r1 >= n) r1 = r1 % n;  // this feels like the same thing...???????????????????
	//while (r1 >= n) r1 -= n;

	return r1;
}

GrapaInt GrapaInt::gcd(const GrapaInt& bi)
{
	GrapaInt x(*this);
	GrapaInt y(bi);

	if (x.IsSignNeg())     // negative
		x = -x;

	if (y.IsSignNeg())     // negative
		y = -y;

	GrapaInt result;
	GrapaBYTE gb;
	int err;
	BIGNUM* n1, * n2;
	u32 sz;
	gb = ((GrapaInt*)&x)->getBytes();
	n1 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	gb = ((GrapaInt*)&y)->getBytes();
	n2 = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	BIGNUM* r = BN_new();
	BN_CTX* ctx = BN_CTX_new();
	err = BN_gcd(r, n1, n2, ctx);
	BN_free(n1);
	BN_free(n2);
	BN_CTX_free(ctx);
	if (err == 1)
	{
		gb.SetSize(BN_num_bytes(r) + 1);
		sz = BN_bn2bin(r, (u8*)gb.mBytes);
		gb.SetLength(sz);
		result.FromBytes(gb, true);
		result.RTrim();
	}
	BN_free(r);
	return result;

	//GrapaInt g = y;

	//while (x.GetCount() > 1 || (x.GetCount() == 1 && x.GetItem(0) != 0))
	//{
	//	g = x;
	//	x = y % x;
	//	y = g;
	//}

	//return g;
}

GrapaInt GrapaInt::gcd(s64 i1) 
{ 
	GrapaInt bi(i1); 
	return gcd(bi); 
}

//***********************************************************************
// Populates "this" with the specified amount of random bits
//***********************************************************************

void GrapaInt::Random(u64 bitsx)
{
	u32 bits = (u32)bitsx;
	*this = 0;

	u32 dwords = bits >> 5;
	u32 remBits = bits & 0x1F;

	if (remBits != 0)
		dwords++;

	SetCount(dwords);
	//if (dwords > (maxLength * 3) / 4) dwords = (maxLength * 3) / 4;

	GrapaBYTE rb;
	rb.SetLength(dwords * 4);
	int err = RAND_priv_bytes((u8*)rb.mBytes, dwords * 4);
	for (u32 i = 0; i < dwords; i++)
		SetItem(i, ((u32*)rb.mBytes)[i]);

	//for (u32 i = 0; i < dwords; i++)
	//	SetItem(i, gSystem->Random32());

	if (remBits != 0)
	{
		u32 mask = (u32)(0x01 << (remBits - 1));
		SetItem(dwords -1, GetItem(dwords - 1) | mask);

		mask = (u32)(0xFFFFFFFF >> (32 - remBits));
		SetItem(dwords - 1, GetItem(dwords - 1) & mask);
	}
	else
		SetItem(dwords - 1, GetItem(dwords - 1) | 0x80000000);

	SetCount(dwords);

	if (GetCount() == 0)
		SetItem(0, 0);
}

//***********************************************************************
// Returns the position of the most significant bit in the BigInteger.
//
// Eg.  The result is 0, if the value of BigInteger is 0...0000 0000
//      The result is 1, if the value of BigInteger is 0...0000 0001
//      The result is 2, if the value of BigInteger is 0...0000 0010
//      The result is 2, if the value of BigInteger is 0...0000 0011
//
//***********************************************************************

u64 GrapaInt::bitCount()
{
	bool isNeg = IsSignNeg();
	u32 skipNum = isNeg?-1:0;
	u64 dlen = GetCount();

	while (dlen > 1 && GetItem(dlen - 1) == skipNum)
		dlen--;

	if (dlen == 0) return(0);

	u32 value = GetItem(dlen - 1);
	u32 mask = 0x80000000;
	u64 bits = 32;
	u32 maskBit = 0;
	if (isNeg) maskBit = -1;

	while (bits > 0 && (value & mask) == (maskBit & mask))
	{
		bits--;
		mask >>= 1;
	}
	bits += ((dlen - 1) << 5);
	
	return bits;
}

u64 GrapaInt::bitStart()
{
	bool isNeg = IsSignNeg();
	u32 skipNum = isNeg ? -1 : 0;
	u64 dlen = 0;

	for (dlen = 0; dlen < GetCount() && (GetItem(dlen) ^ skipNum) == 0; dlen++);

	u32 value = GetItem(dlen);
	if (value == 0) return(0);

	u32 mask = 0x00000001;
	u64 bits = 0;
	u32 maskBit = 0;
	if (isNeg) maskBit = -1;

	while (bits < 32 && (value & mask) == (maskBit & mask))
	{
		bits++;
		mask <<= 1;
	}
	bits += dlen << 5;

	return bits;
}

//***********************************************************************
// Returns the lowest 4 bytes of the BigInteger as an int.
//***********************************************************************

s32 GrapaInt::IntValue()
{
	return (s32)GetItem(0);
}


//***********************************************************************
// Returns the lowest 8 bytes of the BigInteger as a long.
//***********************************************************************

s64 GrapaInt::LongValue()
{
	s64 val = 0;

	val = (s64)GetItem(0);
	try
	{       // exception if maxLength = 1
		val |= (s64)GetItem(1) << 32;
	}
	catch (...)
	{
		if ((GetItem(0) & 0x80000000) != 0) // negative
			val = (s32)GetItem(0);
	}

	return val;
}

//***********************************************************************
// Returns the modulo inverse of this.  Throws ArithmeticException if
// the inverse does not exist.  (i.e. gcd(this, modulus) != 1)
//***********************************************************************

GrapaInt GrapaInt::modInverse(const GrapaInt& modulus)
{
	GrapaInt p0(0), p1(1);
	GrapaInt q0, q1;
	GrapaInt r0(0), r1(0);

	int step = 0;

	GrapaInt a(modulus);
	GrapaInt b(*this);

	while (b.GetCount() > 1 || (b.GetCount() == 1 && b.GetItem(0) != 0))
	{
		GrapaInt quotient;
		GrapaInt remainder;

		if (step > 1)
		{
			GrapaInt pval = (p0 - (p1 * q0)) % modulus;
			//if (gSystem)
			//{
			//	gSystem->Print("step=\t"); gSystem->Print((u64)step); gSystem->Print("\r\n");
			//	gSystem->Print("p0=\t"); gSystem->PrintLine(p0.ToString(16));
			//	gSystem->Print("p1=\t"); gSystem->PrintLine(p1.ToString(16));
			//	gSystem->Print("q0=\t"); gSystem->PrintLine(q0.ToString(16));
			//	gSystem->Print("pval=\t"); gSystem->PrintLine(pval.ToString(16));
			//	gSystem->Print("\r\n");
			//}
			p0 = p1;
			p1 = pval;
		}

		if (b.GetCount() <= 1)
			a.singleByteDivide(b, quotient, remainder);
		else
			a.multiByteDivide(b, quotient, remainder);

		q0 = q1;
		r0 = r1;
		q1 = quotient; r1 = remainder;

		a = b;
		b = remainder;

		step++;
	}

	if (r0.GetCount() > 1 || (r0.GetCount() == 1 && r0.GetItem(0) != 1))
	{
		GrapaInt blank;
		return blank; // (new ArithmeticException("No inverse!"));
	}

	GrapaInt result = ((p0 - (p1 * q0)) % modulus);

	if (result.IsSignNeg())
		result += modulus;  // get the least positive modulus

	result.RTrim();

	return result;
}

GrapaInt GrapaInt::modFact(const GrapaInt& p)
{
	GrapaInt pp(p);
	// n! % p is 0 if n >= p 
	if (pp <= *this)
		return 0;

	// Initialize result as (p-1)! which is -1 or (p-1) 
	GrapaInt res = (pp - 1);

	// Multiply modulo inverse of all numbers from (n+1) 
	// to p 
	for (GrapaInt i = *this + 1; i < pp; i++)
		res = (res * i.modInverse(p)) % p;
	return res;
}

//***********************************************************************
// Returns the value of the BigInteger as a byte array.  The lowest
// index contains the MSB.
//***********************************************************************

GrapaBYTE GrapaInt::getBytes(bool pRaw)
{
	u64 numBits = bitCount() + (pRaw?0:1);
	bool isNeg = IsSignNeg();
	u8 skipByte = isNeg ? 0xFF : 0;

	u64 numBytes = numBits >> 3;
	if ((numBits & 0x7) != 0)
		numBytes++;

	GrapaCHAR result;
	result.mToken = GrapaTokenType::INT;
	if (pRaw)
		result.mToken = GrapaTokenType::RAW;
	result.SetLength(numBytes);
	u8 *value = result.mBytes;

	memset(value, skipByte, (size_t)numBytes);

	s64 dlen = dlen = numBytes / 4;
	if (numBytes % 4) dlen++;

	//START AT A NEGATIVE OFFSET 
	s32 pos = numBytes % 4; if (pos != 0) pos = pos - 4;

	u32 val;
	if (dlen)
	{
		val = GetItem(dlen - 1);
		{//DO NOT WRITE ZEROS
			u8 tempValue;
			tempValue = (u8)(val & 0xFF); if (tempValue != skipByte) value[pos + 3] = tempValue;
			val >>= 8;
			tempValue = (u8)(val & 0xFF); if (tempValue != skipByte) value[pos + 2] = tempValue;
			val >>= 8;
			tempValue = (u8)(val & 0xFF); if (tempValue != skipByte) value[pos + 1] = tempValue;
			val >>= 8;
			tempValue = (u8)(val & 0xFF); if (tempValue != skipByte) value[pos + 0] = tempValue;
			pos += 4;
		}//block
	}

	for (s64 i = dlen - 2; i >= 0; i--, pos += 4) {
		val = GetItem(i);
		value[pos + 3] = (u8)(val & 0xFF);
		val >>= 8;
		value[pos + 2] = (u8)(val & 0xFF);
		val >>= 8;
		value[pos + 1] = (u8)(val & 0xFF);
		val >>= 8;
		value[pos + 0] = (u8)(val & 0xFF);
	}

	return result;
}

GrapaCHAR GrapaInt::Base(GrapaBYTE& pInput, u64 pFrom, u64 pTo)
{
	GrapaCHAR value;
	u32 fromBits = 0;
	switch (pFrom)
	{
	case 2: fromBits = 1; break;	// 0..1
	case 4:	fromBits = 2; break;	// 00..11
	case 8:	fromBits = 3; break;	// 000..111
	case 16: fromBits = 4; break;	// 0000..1111
	case 64: fromBits = 6; break;	// 000000..111111
	case 256: fromBits = 8; break;	// 00000000..11111111
	}
	u64 toBits = 0;
	switch (pTo)
	{
	case 2: toBits = 1; break;		// 0..1
	case 4:	toBits = 2; break;		// 00..11
	case 8:	toBits = 3; break;		// 000..111
	case 16: toBits = 4; break;		// 0000..1111
	case 64: toBits = 6; break;		// 000000..111111
	case 256: toBits = 8; break;	// 00000000..11111111
	}
	if (fromBits == 0 || toBits == 0) return value;
	if (fromBits == toBits) return value;
	//char padChar = '=';
	u8 mapA[256];
	for (u32 i = 0; i < 256; i++)
	{
		if (i >= '0' && i <= '9')		mapA[i] = i - '0';
		else if (i >= 'A' && i <= 'Z')	mapA[i] = 10 + i - 'A';
		else if (i >= 'a' && i <= 'z')	mapA[i] = 36 + i - 'a';
		else if (i == '~')				mapA[i] = 62;
		else if (i == '_')				mapA[i] = 63;
		else							mapA[i] = 0;
	}
	u8 mapB[64];
	for (u32 i = 0; i < 64; i++)
	{
		if (i < 10)						mapB[i] = i;
		else if (i < 36)				mapB[i] = i - 10 + 'A';
		else if (i < 62)				mapB[i] = i - 36 + 'a';
		else if (i == 62)				mapB[i] = '~';
		else if (i == 63)				mapB[i] = '_';
	}
	u8 mask = 0xFF >> (8 - toBits);
	u8 last = (pFrom == 256) ? ((u8*)pInput.mBytes)[pInput.mLength - 1] : mapA[((u8*)pInput.mBytes)[pInput.mLength - 1]];
	u8 lastBits = 0;
	if (last < 0x1) lastBits = 0;
	else if (last < 0x2) lastBits = 1;
	else if (last < 0x4) lastBits = 2;
	else if (last < 0x8) lastBits = 3;
	else if (last < 0x10) lastBits = 4;
	else if (last < 0x20) lastBits = 5;
	else if (last < 0x40) lastBits = 6;
	else if (last < 0x80) lastBits = 7;
	else lastBits = 8;
	u64 inBits = ((pInput.mLength - 1)*fromBits) + lastBits;
	u64 toPos = (inBits / toBits) + ((inBits % toBits) ? 1 : 0) - 1;
	u64 carry = 0;
	u64 carryBits = 0;
	value.GrowSize(toPos + 2, false);
	value.SetLength(toPos + 1, false);
	for (u64 i = value.mLength; i--;)
	{
		u8 c = (pFrom == 256) ? ((u8*)pInput.mBytes)[i] : mapA[((u8*)pInput.mBytes)[i]];
		carry |= ((u64)c) << carryBits;
		carryBits += fromBits;
		while (carryBits > toBits)
		{
			((u8*)value.mBytes)[toPos--] = mapB[carry & mask];
			carry >>= toBits;
			carryBits -= toBits;
		}
	}
	return value;
}

void GrapaInt::setBit(u32 bitNum)
{
	s32 bytePos = bitNum >> 5;             // divide by 32
	u8 bitPos = (u8)(bitNum & 0x1F);    // get the lowest 5 bits

	if (bytePos >= GetCount())
		SetCount(((u64)bytePos) + 1);

	u32 mask = (u32)1 << bitPos;
	SetItem(bytePos, GetItem(bytePos) | mask);
}

void GrapaInt::unsetBit(u32 bitNum)
{
	s32 bytePos = bitNum >> 5;

	if (bytePos < GetCount())
	{
		u8 bitPos = (u8)(bitNum & 0x1F);

		u32 mask = (u32)1 << bitPos;
		u32 mask2 = 0xFFFFFFFF ^ mask;

		SetItem(bytePos, GetItem(bytePos) & mask2);

		RTrim();
	}
}

bool GrapaInt::testBit(u64 bitNum)
{
	u64 bytePos = bitNum >> 5;
	if (bytePos < GetCount())
	{
		u8 bitPos = (u8)(bitNum & 0x1F);
		u32 mask = (u32)1 << bitPos;
		return ((GetItem(bytePos) & mask) != 0);
	}
	return false;
}

GrapaInt GrapaInt::bsqrt()
{
	u32 numBits = (u32)bitCount();

	if ((numBits & 0x1) != 0)        // odd number of bits
		numBits = (numBits >> 1) + 1;
	else
		numBits = (numBits >> 1);

	u32 bytePos = numBits >> 5;
	u8 bitPos = (u8)(numBits & 0x1F);

	u32 mask;

	GrapaInt result;// = new BigInteger();
	if (bitPos == 0)
		mask = 0x80000000;
	else
	{
		mask = (u32)1 << bitPos;
		bytePos++;
	}
	result.SetCount((u32)bytePos);

	for (s32 i = (u32)bytePos - 1; i >= 0; i--)
	{
		while (mask != 0)
		{
			// guess
			result.SetItem(i, result.GetItem(i) ^ mask);

			// undo the guess if its square is larger than this
			if ((result * result) > *this)
				result.SetItem(i, result.GetItem(i) ^ mask);

			mask >>= 1;
		}
		mask = 0x80000000;
	}
	result.RTrim();
	return result;
}

void GrapaInt::MulDivTest(u32 rounds)
{
	u8 val[64];// = new byte[64];
	u8 val2[64];// = new byte[64];

	for (u32 count = 0; count < rounds; count++)
	{
		// generate 2 numbers of random length
		int t1 = 0;
		while (t1 == 0)
			t1 = (int)(gSystem->Random32() % 65);

		int t2 = 0;
		while (t2 == 0)
			t2 = (int)(gSystem->Random32() % 65);

		bool done = false;
		while (!done)
		{
			for (s32 i = 0; i < 64; i++)
			{
				if (i < t1)
					val[i] = (u8)(gSystem->Random32() % 256);
				else
					val[i] = 0;

				if (val[i] != 0)
					done = true;
			}
		}

		done = false;
		while (!done)
		{
			for (s32 i = 0; i < 64; i++)
			{
				if (i < t2)
					val2[i] = (u8)(gSystem->Random32() % 256);
				else
					val2[i] = 0;

				if (val2[i] != 0)
					done = true;
			}
		}

		while (val[0] == 0)
			val[0] = (u8)(gSystem->Random32() % 256);
		while (val2[0] == 0)
			val2[0] = (u8)(gSystem->Random32() % 256);

		GrapaInt bn1(val, t1);
		GrapaInt bn2(val2, t2);

		GrapaInt bn3 = bn1 / bn2;
		GrapaInt bn3b = bn3 * bn2;
		GrapaInt bn4 = bn1 % bn2;

		// Recalculate the number
		GrapaInt bn5 = (bn3 * bn2) + bn4;

		// Make sure they're the same
		if (bn5 != bn1)
		{
			return;
		}
	}
}

void GrapaInt::SqrtTest(u32 rounds)
{
	for (u32 count = 0; count < rounds; count++)
	{
		// generate data of random length
		int t1 = 0;
		while (t1 == 0)
			t1 = (int)(gSystem->Random32() % 1024);

		//Console.Write("Round = " + count);

		GrapaInt a;// = new BigInteger();
		a.Random(t1);

		GrapaInt b = a.bsqrt();
		GrapaInt c = (b + 1)*(b + 1);

		// check that b is the largest integer such that b*b <= a
		if (c <= a)
		{
			return;
		}
	}
}

u64 GrapaInt::gcd64(u64 a, u64 b)
{
	while (b != 0)
	{
		u64 t = b;
		b = a % b;
		a = t;
	}
	return a;
}

u64 GrapaInt::mulinv64(u64 exponent, u64 modulus)
{
	u64 g0 = modulus, g1 = exponent % modulus;
	u64 v0 = 0, v1 = 1;
	u64 y;
	while (g1)
	{
		if (g1 == 1) return(v1);
		y = g0 / g1;
		g0 = g0 % g1;
		if (!g0) return(0);
		v0 += y * v1;
		if (g0 == 1) return(modulus - v0);
		y = g1 / g0;
		g1 = g1 % g0;
		v1 += y * v0;
	}
	return(0);
}

// need to pick from above and the following...
u64 GrapaInt::Inv64(u64 n, u64 m)
{
	u64 a = n, b = m;
	s64 u = 1, v = 0;
	do
	{
		u64 q = a / b;

		u64 t1 = a - q*b;
		a = b;
		b = t1;

		u64 t2 = u - (int)q*v;
		u = v;
		v = t2;
	} while (b != 0);
	if (a != 1) u = 0;
	if (u < 0) u += m;
	return u;
}

u64 GrapaInt::modpow64(u64 base, u64 exponent, u64 modulus)
{
	u64 result = 1;
	base %= modulus;
	while (exponent > 0)
	{
		if (exponent % 2 == 1)
			result = (result * base) % modulus;
		exponent = exponent >> 1;
		base = (base * base) % modulus;
	}
	return(result);
}

// need to verify that (a*b)%c == a*b%c

u64 GrapaInt::Powm64(u64 n, u64 e, u64 m)
{
	u64 r = 1;
	u64 t = n % m;
	u64 i;
	for (i = e; i != 0; i /= 2)
	{
		if (i % 2 != 0)
			r = r * t % m;
		t = t * t % m;
	}
	return r;
}

u64 GrapaInt::mul64(u64 a, u64 b, u64 mod)
{
	s32 i;
	u64 now = 0;
	for (i = 63; i >= 0; i--) if (((a >> i) & 1) == 1) break;
	for (; i >= 0; i--)
	{
		now <<= 1;
		while (now > mod) now -= mod;
		if (((a >> i) & 1) == 1) now += b;
		while (now > mod) now -= mod;
	}
	return now;
}

u64 GrapaInt::pow64(u64 a, u64 p, u64 mod)
{
	if (p == 0) return 1;
	if (p % 2 == 0) return pow64(mul64(a, a, mod), p / 2, mod);
	return mul64(pow64(a, p - 1, mod), a, mod);
}

u64 GrapaInt::sqrt64(u64 x)
{
	/* Computes the (integer truncated) square root of ull, using a
	* Newton-Raphson method. Returns floor(sqrt(ull)).
	*
	* Adapted from a similar routine in the BIGINT ultraprecision
	* integer package, developed (1988-91) and graciously placed in the
	* public domain by Arjen K. Lenstra, Mark Riordan, and Marc Ringuette.
	* The original BIGINT package is available (19 March 2001) at
	* <http://www.funet.fi/pub/crypt/cryptography/rpem/rpem/>
	* (thanks to Charles Doty for this pointer).
	*
	

	u64 ull2, ull3, ull4;

	if (ull == 0)return(0);
	if (ull < 4)return(1);
	if (ull < 9)return(2);
	if (ull >= ((u64)MAXU32)*((u64)MAXU32))return(MAXU32);
	ull2 = ull / 2;
	while (1)
	{
		ull3 = ull / ull2;
		ull4 = (ull3 + ull2) / 2;
		if ((ull4 - ull3 < 2) || (ull3 - ull4 < 2))
		if (ull4*ull4 <= ull)
        {
			return(ull4);
		}
        else
        {
			return(ull3);
        }
		ull2 = ull4;
	}
    return(0);
    */
    
    u64 op, res, one;
    
    op = x;
    res = 0;
    
    /* "one" starts at the highest power of four <= than the argument. */
    one = 1 << 30;  /* second-to-top bit set */
    while (one > op) one >>= 2;
    
    while (one != 0) {
        if (op >= res + one) {
            op -= res + one;
            res += one << 1;  // <-- faster than 2 * one
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaPolyMod::GrapaPolyMod() 
{ 
	m_r = 0; m_n = 0; zero = 0; one = 1; two = 2; 
	m_deg = 0;
	mp_a = NULL;
}

GrapaPolyMod::GrapaPolyMod(u64 r, const GrapaInt& n)
{
	m_r = r; m_n = n; zero = 0; one = 1; two = 2;
	m_deg = 1;
	mp_a = new GrapaInt[m_deg + (u64)1];
	mp_a[0] = 0; mp_a[1] = 1;
}

GrapaPolyMod::GrapaPolyMod(const GrapaPolyMod & p)
{
	m_r = p.m_r; m_n = p.m_n; zero = 0; one = 1; two = 2;
	m_deg = p.m_deg;
	mp_a = new GrapaInt[m_deg + (u64)1];
	for (u64 i = 0; i <= p.m_deg; ++i)
		mp_a[i] = p.mp_a[i];
}

GrapaPolyMod::~GrapaPolyMod()
{
	if (mp_a != NULL)
		delete[] mp_a;
}

void GrapaPolyMod::_polySquare()
{
	u64 degr = m_deg + m_deg;
	GrapaInt * p_ar = new GrapaInt[degr + (u64)1];
	for (u64 k = 0; k <= degr; ++k)
		p_ar[k] = 0;

	for (u64 j = 1; j <= m_deg; ++j)
	{
		if (mp_a[j] != zero)
		{
			for (u64 i = 0; i < j; ++i)
			{
				GrapaInt y = mp_a[i] * two;
				GrapaInt t = p_ar[j + i];
				p_ar[j + i] = ((mp_a[j] * y + t) % m_n);
			}
		}
	}
	for (u64 i = 0; i <= m_deg; ++i)
	{
		GrapaInt x = mp_a[i];
		GrapaInt t = p_ar[2 * i];
		p_ar[2 * i] = ((x * x + t) % m_n);
	}

	m_deg = degr;
	delete[] mp_a;
	mp_a = p_ar;
}

void GrapaPolyMod::_polyMul(const GrapaPolyMod & p)
{
	u64 deg = m_deg;

	u64 degr = deg + p.m_deg;
	GrapaInt * p_ar = new GrapaInt[degr + (u64)1];
	for (u64 k = 0; k <= degr; ++k)
		p_ar[k] = 0;

	for (u64 j = 0; j <= p.m_deg; ++j)
	{
		GrapaInt x = p.mp_a[j];
		if (x != zero)
		{
			for (u64 i = 0; i <= deg; ++i)
			{
				GrapaInt y = mp_a[i];
				GrapaInt t = p_ar[j + i];
				t = ((x * y.LongValue() + t) % m_n);
				p_ar[j + i] = t;
			}
		}
	}

	m_deg = degr;
	delete[] mp_a;
	mp_a = p_ar;
}

void GrapaPolyMod::_Mod()
{
	u64 deg = m_deg;
	while (deg >= m_r)
	{
		mp_a[deg - m_r] += mp_a[deg];
		if (mp_a[deg - m_r] >= m_n) mp_a[deg - m_r] -= m_n;
		--deg;

		while (mp_a[deg] == 0) --deg;
	}
	m_deg = deg;
}

void GrapaPolyMod::_Norm()
{
	if (mp_a[m_deg] != 1)
	{
		GrapaInt y = mp_a[m_deg].modInverse(m_n);
		for (u64 i = 0; i <= m_deg; ++i)
			mp_a[i] = mp_a[i] * y % m_n;
	}
}

GrapaPolyMod & GrapaPolyMod::operator = (const GrapaPolyMod & p)
{
	if (&p == this) return *this;
	m_deg = p.m_deg;
	delete[] mp_a;
	mp_a = new GrapaInt[m_deg + (u64)1];
	for (u64 i = 0; i <= p.m_deg; ++i)
		mp_a[i] = p.mp_a[i];
	return *this;
}

int GrapaPolyMod::operator != (const GrapaPolyMod & p) const
{
	if (m_deg != p.m_deg)
		return true;
	for (u64 i = 0; i <= m_deg; ++i)
		if (mp_a[i] != p.mp_a[i])
			return true;
	return false;
}

GrapaPolyMod & GrapaPolyMod::operator += (const GrapaInt& i)
{
	GrapaInt n = i;
	GrapaInt t = n % m_n;
	mp_a[0] += t;
	if (mp_a[0] >= m_n) mp_a[0] -= m_n;
	return *this;
}

GrapaPolyMod & GrapaPolyMod::operator += (u64 i)
{
	GrapaInt n = i;
	GrapaInt t = n % m_n;
	mp_a[0] += t;
	if (mp_a[0] >= m_n) mp_a[0] -= m_n;
	return *this;
}

GrapaPolyMod & GrapaPolyMod::operator -= (const GrapaInt& i)
{
	GrapaInt n = i;
	GrapaInt t = m_n - n % m_n;
	mp_a[0] += t;
	if (mp_a[0] >= m_n) mp_a[0] -= m_n;
	return *this;
}

GrapaPolyMod & GrapaPolyMod::operator -= (u64 i)
{
	GrapaInt n = i;
	GrapaInt t = m_n - n % m_n;
	mp_a[0] += t;
	if (mp_a[0] >= m_n) mp_a[0] -= m_n;
	return *this;
}

GrapaPolyMod GrapaPolyMod::Pow(const GrapaInt& e)
{
	GrapaInt er = one;
	GrapaInt j;
	for (j = e; j != one; j = j / two)
	{
		er = two * er + (j % two);
	}

	GrapaPolyMod t(*this);
	GrapaInt i;
	for (i = er; i != one; i = i / two)
	{
		t._polySquare();
		t._Mod();
		if ((i % two) != zero)
		{
			t._polyMul(*this);
			t._Mod();
		}
	}
	t._Norm();
	return t;
}

////////////////////////////////////////////////////////////////////////////////
