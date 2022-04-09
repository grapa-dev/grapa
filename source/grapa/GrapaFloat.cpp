// GrapaFloat.cpp
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

#include "GrapaFloat.h"

#include "GrapaSystem.h"
extern GrapaSystem* gSystem;

// http://en.wikipedia.org/wiki/Newton%27s_method

// http://www.mpia.de/~mathar/progs/jdocs/org/nevec/rjm/BigDecimalMath.html

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaFloat::GrapaFloat(bool pFix, s64 pMax, s64 pExtra, s64 pValue)
{
	INIT();
	mSigned = pValue < 0;
	if (mSigned)
		pValue = -pValue;
	mData = pValue;
	mBits = mData.bitCount();
	mExp = mBits - 1;
	mFix = pFix;
	mMax = pMax;
	mExtra = pExtra;
	mTrunc = false;
	Truncate();
}


GrapaFloat::GrapaFloat(bool pFix, s64 pMax, s64 pExtra, const GrapaInt& bi)
{
	INIT();
	mFix = pFix;
	mMax = pMax;
	mExtra = pExtra;
	FromInt(bi);
}

GrapaFloat::GrapaFloat(bool pFix, s64 pMax, s64 pExtra, const GrapaFloat& bi)
{
	INIT();
	*this = bi;
	mFix = pFix;
	mMax = pMax;
	mExtra = pExtra;
	Truncate();
}

GrapaFloat::GrapaFloat(const GrapaFloat& bi)
{
	INIT();
	*this = bi;
}

GrapaFloat::GrapaFloat(const GrapaInt& bi)
{
	INIT();
	*this = bi;
}

GrapaFloat::GrapaFloat(const GrapaBYTE& inData)
{
	INIT();
	FromBytes(inData);
}

GrapaFloat::GrapaFloat(double value)
{
	INIT();
	mSigned = value < 0;
	if (mSigned)
		value = -value;
	mData = (u64)trunc(value);
	mExp = mData.bitCount() - 1;
	mData = (mData * (u64)0x10000000000000) + (u64)trunc((value - trunc(value)) * (u64)0x10000000000000);
	mBits = mData.bitCount();
	mFix = false;
	mMax = 56;
	mExtra = 4;
	mTrunc = false;
	Truncate();
	if (mBits > 49 && ((mExp + 1) < mBits))
	{
		mTrunc = true;
		mData.SetItem(0, mData.GetItem(0) - 1);	
		mData = mData << 1;
		mData.SetItem(0, mData.GetItem(0) + 1);
		mBits += 1;
	}
}

bool GrapaFloat::IsInt()
{
	if (u64 lead = mData.bitStart())
	{
		mData = mData >> lead;
		mBits -= lead;
	}
	if (mBits < 0) mBits = 0;
	if (mBits <= (mExp + 1))
		return true;
	return false;
}

bool GrapaFloat::IsZero()
{
	if (mData.IsZero())
		return(true);
	if (u64 lead = mData.bitStart())
	{
		mData = mData >> lead;
		mBits -= lead;
	}
	if (mBits < 0) mBits = 0;
	if (mBits <= (mExp + 1) && mData.GetItem(0) == 0)
		return true;
	return false;
}

void GrapaFloat::TO(GrapaInt& pInteger, s64& pLenI, GrapaInt& pFraction, s64& pLenF, int lpadbits)
{
	if (u64 lead = mData.bitStart())
	{
		mData = mData >> lead;
		mBits -= lead;
	}
	if (mBits < 0) mBits = 0;
	if (mBits <= (mExp+1))
	{
		if (u64 lead = (mExp + 1) - mBits)
			pInteger = mData << lead;
		else
			pInteger = mData;
		pLenI = mExp + 1;
		if (pLenI > mMax)
			pLenI = mMax;
		pFraction = 0;
		pLenF = 0;
	}
	else
	{
		pInteger = mData >> (mBits-mExp-1);
		pLenI = mData.bitCount();
		pLenF = (mBits - mExp - 1);
		GrapaInt n = 0;
		for (s64 i = 0; i < pLenF; i++) n.setBit((u32)i);
		pFraction = mData & n;

		// This checks if there are any bits in the last 4 bits besides the high order bit
		// If so, than it adds the into the fraction
		// I don't remember why only the last byte.
		// Need to reproduce the scenario that required this, and see if this should be  function of mExtra rather than just they last 4 bits
		u64 lead = lpadbits - pLenF % lpadbits;
		if (lead != lpadbits)
		{
			pFraction = pFraction << lead;
			pLenF += lead;
		}

		//(1).float(10).pi().sin().hex()

		// for the trailing crud left over...just in case
		// (1).pi().sin() was not working right without removing these trailing bits
		// not sure why...might need more investigation
		// but for now, this seems to do the trick.
		//pFraction = (pFraction >> mExtra) << mExtra;
	}
}

GrapaFloat& GrapaFloat::operator =(const GrapaFloat& that)
{
	if (this != &that)
		FROM(that);
	return *this;
}

GrapaFloat& GrapaFloat::operator =(const GrapaInt& that)
{
	FromInt(that);
	return *this;
}

void GrapaFloat::Add(GrapaFloat& A, GrapaFloat& B, GrapaFloat& result)
{
	s64 bitsDelta = (A.mBits - A.mExp) - (B.mBits - B.mExp);
	if (bitsDelta < 0)
	{
		A.mData = A.mData << (-bitsDelta);
		A.mBits += (-bitsDelta);
	}
	else if (bitsDelta > 0)
	{
		B.mData = B.mData << (bitsDelta);
		B.mBits += (bitsDelta);
	}
	if (A.mSigned == B.mSigned)
	{
		result.mData = A.mData + B.mData;
		result.mSigned = A.mSigned;
	}
	else if (A.mSigned)
	{
		result.mData = B.mData - A.mData;
	}
	else if (B.mSigned)
	{
		result.mData = A.mData - B.mData;
	}
	if (result.mData.dataSigned)
	{
		result.mData = -result.mData;
		result.mSigned = true;
	}
	result.mBits = result.mData.bitCount();
	result.mExp = result.mBits - (A.mBits - A.mExp);
	result.Truncate();
	result.mTrunc = A.mTrunc || B.mTrunc;
}

GrapaFloat GrapaFloat::operator +(const GrapaFloat& bi)
{
	GrapaFloat A(*this), B(bi);
	GrapaFloat result(mFix && bi.mFix ? true : false, mMax > bi.mMax ? mMax : bi.mMax, mExtra > bi.mExtra ? mExtra : bi.mExtra, 0);
	Add(A, B, result);
	return result;
}

GrapaFloat GrapaFloat::operator +(s64 bi) 
{ 
	GrapaFloat bi2(mFix,mMax, mExtra,bi);
	return *this + bi2; 
}

GrapaFloat GrapaFloat::operator +=(const GrapaFloat& bi1)
{
	GrapaFloat that(*this + bi1);
	*this = that;
	return that;
}

GrapaFloat& GrapaFloat::operator ++()
{
	GrapaFloat result(*this);
	*this = result;
	return *this;
}

GrapaFloat GrapaFloat::operator++(int) 
{ 
	GrapaFloat tmp(*this); 
	operator++(); 
	return tmp; 
}

GrapaFloat GrapaFloat::operator -(const GrapaFloat& bi)
{
	GrapaFloat A(*this), B(bi);
	B.mSigned = !B.mSigned;
	GrapaFloat result(mFix&&bi.mFix?true:false, mMax > bi.mMax ? mMax : bi.mMax, mExtra>bi.mExtra?mExtra:bi.mExtra, 0);
	Add(A, B, result);
	return result;
}

GrapaFloat GrapaFloat::operator -(s64 bi) 
{ 
	GrapaFloat that(mFix, mMax, mExtra, bi);
	return *this - that;
}

GrapaFloat GrapaFloat::operator -=(const GrapaFloat& bi1)
{ 
	GrapaFloat that(*this - bi1);
	*this = that;
	return that;
}

GrapaFloat& GrapaFloat::operator --()
{
	GrapaFloat that(*this - (s64)1);
	*this = that;
	return *this;
}

GrapaFloat GrapaFloat::operator--(int x) 
{ 
	GrapaFloat tmp(*this - (s64)x); 
	return tmp; 
}

GrapaFloat GrapaFloat::operator *(const GrapaFloat& bi)
{
	GrapaFloat result(mFix && bi.mFix ? true : false, mMax > bi.mMax ? mMax : bi.mMax, mExtra > bi.mExtra ? mExtra : bi.mExtra, 0);
	if (mTrunc || bi.mTrunc)
	{
		GrapaInt a(mData);
		if (mTrunc)
			a += 1;
		GrapaInt b(bi.mData);
		if (bi.mTrunc)
			b += 1;
		result.mData = a * b;
	}
	else
	{
		result.mData = mData * bi.mData;
	}
	result.mSigned = mSigned != bi.mSigned;
	result.mTrunc = mTrunc || bi.mTrunc;
	result.mBits = result.mData.bitCount();
	result.mExp = mExp + bi.mExp + (result.mBits - (mBits + bi.mBits - 1));
	result.mFix = mFix;
	result.mMax = mMax;
	result.Truncate();
	return result;
}

GrapaFloat GrapaFloat::operator *(const GrapaInt& bi)
{
	GrapaFloat bi2(mFix,mMax, mExtra,bi);
	return (*this*bi2);
}

GrapaFloat GrapaFloat::operator *(s64 bi) 
{ 
	GrapaFloat bi2(mFix, mMax, mExtra, bi);
	return (*this*bi2); 
}

GrapaFloat GrapaFloat::operator *(double bi) 
{ 
	GrapaFloat bi2(bi); 
	return (*this*bi2); 
}

GrapaFloat GrapaFloat::operator <<(int shiftVal)
{
	GrapaFloat result(*this);
	result.mExp += shiftVal;
	result.Truncate();
	return result;
}

GrapaFloat GrapaFloat::operator >>(int shiftVal)
{
	GrapaFloat result(*this);
	result.mExp -= shiftVal;
	result.Truncate();
	return result;
}

GrapaFloat GrapaFloat::operator ~()
{
	GrapaFloat result(mFix, mMax, mExtra, 0);
	return result;
}

GrapaFloat GrapaFloat::operator -()
{
	GrapaFloat result(*this);
	result.mSigned = !result.mSigned;
	return result;
}

bool GrapaFloat::operator ==(const GrapaFloat& bi2)
{
	return Equals(bi2);
}

bool GrapaFloat::operator !=(const GrapaFloat& bi2)
{
	return !(Equals(bi2));
}

bool GrapaFloat::Equals(const GrapaFloat& bi)
{
	GrapaFloat result(*this - bi);
	result.Truncate(true);
	if (result.mData.IsZero())
		return(true);
	return(false);
}

bool GrapaFloat::operator >(const GrapaFloat& bi)
{
	GrapaFloat result(*this - bi);
	result.Truncate(true);
	if (result.mData.IsZero())
		return(false);
	return (result.mSigned==false && !result.IsZero());
}

bool GrapaFloat::operator >(s64 i1) 
{ 
	GrapaFloat bi1(mFix, mMax, mExtra, i1);
	return (*this > bi1); 
}

bool GrapaFloat::operator <(const GrapaFloat& bi)
{
	GrapaFloat result(*this - bi);
	result.Truncate(true);
	if (result.mData.IsZero())
		return(false);
	return result.mSigned == true && !result.IsZero();
}

bool GrapaFloat::operator <(s64 bi) 
{ 
	GrapaFloat bi2(mFix, mMax, mExtra, bi);
	return (*this < bi2); 
}

bool GrapaFloat::operator >= (const GrapaFloat& bi2)
{
	return (*this == bi2 || *this > bi2);
}

bool GrapaFloat::operator <= (const GrapaFloat& bi2)
{
	return (*this == bi2 || *this < bi2);
}

GrapaInt GrapaFloat::Div(const GrapaFloat& bi2, GrapaFloat& remainder)
{
	GrapaFloat A(*this);
	GrapaFloat B(bi2);
	s64 bitsDelta = (A.mBits - A.mExp) - (B.mBits - B.mExp);
	if (bitsDelta < 0)
	{
		A.mData = A.mData << (-bitsDelta);
		A.mBits += (-bitsDelta);
		A.mExp += (-bitsDelta);
	}
	else if (bitsDelta > 0)
	{
		B.mData = B.mData << (bitsDelta);
		B.mBits += (bitsDelta);
		B.mExp += (bitsDelta);
	}
	GrapaInt result = A.mData.Div(B.mData, remainder.mData);
	result.dataSigned = A.mSigned != B.mSigned;
	remainder.mBits = remainder.mData.bitCount();
	bitsDelta = B.mBits - B.mExp - 1;
	remainder.mExp = remainder.mBits - bitsDelta - 1;
	remainder.mTrunc = mTrunc || bi2.mTrunc;
	remainder.mSigned = A.mSigned;
	remainder.mFix = mFix;
	remainder.mMax = mMax > B.mMax ? B.mMax : mMax;
	remainder.mExtra = mExtra > B.mExtra ? mExtra : B.mExtra;
	remainder.Truncate();
	return result;
}

GrapaFloat GrapaFloat::operator /(const GrapaFloat& bi2)
{
	GrapaFloat result(mFix && bi2.mFix ? true : false, mMax > bi2.mMax ? mMax : bi2.mMax, mExtra > bi2.mExtra ? mExtra : bi2.mExtra, 0);
	GrapaFloat bi(bi2);
	s64 s = 0;
	if (mFix)
	{
		s = mMax - ((mBits - mExp - 1) - (bi.mBits - bi.mExp - 1) - 1) + 3 + result.mExtra + 4;
	}
	else
	{
		s = (mMax - mBits) + bi.mBits + result.mExtra + 4;
	}
	if (s < (result.mExtra + 4))
		s = result.mExtra + 4;
	if (s > 0)
		result.mData = mData << s;
	result.mData = result.mData / bi.mData;
	if (bi.mData.IsZero())
	{
		result.mNaN = true;
	}
	result.mSigned = mSigned != bi.mSigned;
	result.mBits = result.mData.bitCount();
	s64 dbits = mData.bitCount();
	if (s > 0)
		dbits += s;
	s64 bbits = bi.mData.bitCount();
	s64 carry = dbits - result.mBits - bbits + 1;
	result.mExp = mExp - bi.mExp - carry;
	result.mMax = mMax > bi.mMax ? bi.mMax : mMax;
	result.mFix = mFix;
	result.Truncate();
	return result;
}

GrapaFloat GrapaFloat::operator /(const GrapaInt& bi)
{
	GrapaFloat bi2(mFix, mMax, mExtra, bi);
	return((*this) / bi2);
}

GrapaFloat GrapaFloat::operator /(s64 bi)
{
	GrapaFloat bi2(mFix, mMax, mExtra, bi);
	return((*this) / bi2);
}

GrapaFloat GrapaFloat::operator %(const GrapaFloat& bi)
{
	GrapaFloat remainder(mFix, mMax, mExtra > bi.mExtra ? mExtra : bi.mExtra, 0);
	GrapaInt result = Div(bi, remainder);
	return(remainder);
}

GrapaFloat GrapaFloat::operator %(const GrapaInt& bi)
{
	return ((*this) % GrapaFloat(mFix, mMax, mExtra, bi));
}

GrapaFloat GrapaFloat::operator %(s64 bi)
{
	return ((*this) % GrapaFloat(mFix, mMax, mExtra, bi));
}

GrapaFloat GrapaFloat::operator %=(const GrapaFloat& bi1)
{
	*this = *this % bi1;
	return *this;
}

GrapaFloat GrapaFloat::operator |(const GrapaFloat& e)
{
	GrapaFloat exp(e);
	GrapaFloat result(mFix, mMax, mExtra > e.mExtra ? mExtra : e.mExtra, 0);
	return result;
}

GrapaFloat GrapaFloat::operator &(const GrapaFloat& e)
{
	GrapaFloat exp(e);
	GrapaFloat result(mFix, mMax, mExtra > e.mExtra ? mExtra : e.mExtra, 0);
	return result;
}

GrapaFloat GrapaFloat::operator ^(const GrapaFloat& e)
{
	GrapaFloat exp(e);
	GrapaFloat result(mFix, mMax, mExtra > e.mExtra ? mExtra : e.mExtra, 0);
	return result;
}

GrapaFloat GrapaFloat::Pow(const GrapaInt& pexp)
{
	GrapaFloat n(*this);
	GrapaFloat result(mFix, mMax, mExtra, 0);
	n.mFix = true;
	n.mMax += 12+mExtra;
	GrapaInt d = pexp;
	bool invert = false;
	if (d.dataSigned)
	{
		invert = true;
		d = -d;
		if (n.mData.IsZero())
		{
			result.mNaN = true;
			return(result);
		}
	}
	s64 e = d.LongValue();
	result.mData = n.mData.Pow(d);
	result.mSigned = n.mSigned && (d.GetItem(0) & 0x1) != 0;
	result.mBits = result.mData.bitCount();
	result.mExp = (n.mExp * e) + (result.mBits - ((n.mBits - 1) * e) - 1);
	result.mFix = mFix;
	result.mMax = mMax;
	result.mTrunc = true;
	result.Truncate();
	if (invert)
		return (GrapaFloat(mFix, mMax, mExtra, 1) / result);
	else
		return result;
}

GrapaFloat GrapaFloat::Pow2(const GrapaFloat& pexp)
{
	GrapaFloat d = pexp;
	if (d.mSigned)
	{
		if (mData.IsZero())
		{
			GrapaFloat result(mFix, mMax, mExtra, 0);
			result.mNaN = true;
			return(result);
		}
	}

	s64 ext = mExtra > pexp.mExtra ? mExtra : pexp.mExtra;
	if (d.mTrunc)
	{
		d = d * GrapaFloat(mFix, mMax, ext, 1);
		d.mTrunc = false;
	}
	s64 mulMax = 1;
	GrapaFloat partA(mFix, mMax, ext, 1);
	if (d.mExp >= 0)
	{
		if (d.mBits <= (d.mExp + 1))
			return Pow(d.mData << ((d.mExp + 1) - d.mBits));
		GrapaInt i = d.mData >> (d.mBits - d.mExp - 1);
		mulMax = i.LongValue();
		partA = Pow(i);
		d = d - GrapaFloat(true,mMax, ext,i);
	}
	GrapaFloat n(*this);
	n.mFix = true;
	n.mMax = n.mMax + mulMax + 8 + mExtra;
	n.mTrunc = false;
	GrapaFloat result = GrapaFloat::E(n.Ln() * d);
	result = result * partA;
	result.mFix = mFix;
	result.mMax = n.mMax - (10+mExtra);
	result.mExtra = ext;
	result.mTrunc = true;
	result.Truncate();
	return(result);
}

GrapaFloat GrapaFloat::Root(const GrapaInt& pexp)
{
	GrapaFloat result(*this);
	s64 e = GrapaInt(pexp).LongValue();
	GrapaFloat em(((double)e) + 0.0625);

	if (e == 1)
		return result;

	if (e == 0)
	{
		result.FROM(0);
		result.mNaN = true;
		return result;
	}

	result.mSigned = false;
	result.mExp /= e;
	result.mMax += mExtra;
	result.mFix = false;
	s64 maxbits = result.mMax;

	GrapaFloat temp(false, maxbits, mExtra, 0);
	GrapaFloat top(false, maxbits, mExtra, 0);
	GrapaFloat lasttemp(false, maxbits, mExtra, 0);
	s64 i = 0;
	s64 oldExp = result.mMax;
	temp.mExp = result.mMax;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		top = ((result.Pow(e)) - *this);

		top.mFix = false;
		top.mMax = top.mData.bitCount();
		if (top.mMax<maxbits)
			top.mMax = maxbits;
		temp = top / (result.Pow(e - 1)*em);
		top.mFix = true;
		top.mMax = maxbits;

		result = result - temp;
		result.mSigned = false;

		//if (temp.mExp >= oldExp)
		//{
		//	if (temp == lasttemp)
		//		break;
		//	lasttemp = temp;
		//}

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mBits && (temp.mExp > 0 || temp.mExp > -mMax));

	// if exp is odd, and *this is signed, then result is signed
	result.mMax = mMax;
	result.Truncate();

	return result;
}

GrapaFloat GrapaFloat::Root2(const GrapaFloat& pexp)
{
	GrapaFloat one(pexp.mFix, pexp.mMax, pexp.mExtra, 1);
	GrapaFloat zero(pexp.mFix, pexp.mMax, pexp.mExtra, 0);
	if (one == pexp)
		return *this;
	if (zero == pexp)
	{
		zero.mNaN = true;
		return zero;
	}
	return (*this).Pow2(one / pexp);
}

GrapaFloat GrapaFloat::Pi(const GrapaFloat& pexp)
{
	//s64 maxbits = pexp.mMax;
	//maxbits += pexp.mExtra + 2;
	//GrapaFloat c13591409(false, maxbits, pexp.mExtra, 13591409);
	//GrapaFloat c545140134(false, maxbits, pexp.mExtra, 545140134);
	//GrapaFloat c640320(false, maxbits, pexp.mExtra, 640320);
	//GrapaFloat c = GrapaFloat(false, maxbits, pexp.mExtra, 12) / ((c640320.Pow(3)).Root(2));

	//c640320.mSigned = true;

	//GrapaFloat f1k(false, maxbits, pexp.mExtra, 1);
	//GrapaFloat f3k(false, maxbits, pexp.mExtra, 1);
	//GrapaFloat f6k(false, maxbits, pexp.mExtra, 1);
	//GrapaFloat c640320_pow(false, maxbits, pexp.mExtra, 0);

	//GrapaFloat result(c13591409);
	//GrapaFloat temp(false, maxbits, pexp.mExtra, 0), temp2(false, maxbits, pexp.mExtra, 0);
	//s64 i = 0;
	//do
	//{
	//	i++;
	//	f1k = f1k * (i);
	//	f3k = f3k * ((i * 3 - 2) * (i * 3 - 1) * (i * 3));
	//	f6k = f6k * ((i * 6 - 5) * (i * 6 - 4) * (i * 6 - 3) * (i * 6 - 2) * (i * 6 - 1) * (i * 6));
	//	c640320_pow = c640320.Pow(3 * i);
	//	temp = (c13591409 + (c545140134 * i)) * f6k;
	//	temp2 = c640320_pow * (f3k * f1k * f1k * f1k);
	//	temp = temp / temp2;
	//	result = result + temp;
	//} while (temp.mBits && temp.mExp > -pexp.mMax);

	//if (pexp.mBits != 1 || pexp.mData.GetItem(0) == 1)
	//	temp = (result * c).Pow2(pexp);
	//else
	//	temp = result * c;
	//temp2 = GrapaFloat(false, maxbits, pexp.mExtra, 1) / temp;


	//temp2.mMax = pexp.mMax;
	//temp2.mFix = true;
	//temp2.Truncate();

	//return (temp2);

	// (1).float(1000,10).pi()
	// 247

	GrapaCHAR nStr(".");
	u64 N = pexp.mMax / 4;
	u64 LEN = (10L * N) / 3 + 1;

	unsigned predigit = 0, nines = 0;
	unsigned *a = (unsigned*)malloc(sizeof(unsigned)*LEN);
	memset(a, 0, sizeof(unsigned) * LEN);
	long x=0, q=0, k=0, i3=0;

	for (unsigned j = N; j; )
	{
		q = 0;
		k = LEN + LEN - 1;

		for (i3 = LEN; i3; --i3)
		{
			x = (j == N ? 20 : 10L * a[i3 - 1]) + q * i3;
			q = x / k;
			a[i3 - 1] = (unsigned)(x - q * k);
			k -= 2;
		}

		k = x % 10;
		if (k == 9)
			++nines;
		else
		{
			if (j)
			{
				--j;
				nStr.Append((char)((predigit + x / 10) + '0'));
			}

			for (; nines; --nines)
			{
				if (j) --j, nStr.Append(x >= 10 ? '0' : '9');
			}

			predigit = (unsigned)k;
		}
	}
	free(a);
	nStr.mBytes[0] = nStr.mBytes[1];
	nStr.mBytes[1] = '.';
	GrapaFloat temp3=pexp;
	temp3.FromString(nStr, 10);
	return (temp3);

	/*
	def pi_digits(n):
    "Generate n digits of Pi."
    k, a, b, a1, b1 = 2, 4, 1, 12, 4
    while n > 0:
        p, q, k = k * k, 2 * k + 1, k + 1
        a, b, a1, b1 = a1, b1, p * a + q * a1, p * b + q * b1
        d, d1 = a / b, a1 / b1
        while d == d1 and n > 0:
            yield int(d)
            n -= 1
            a, a1 = 10 * (a % b), 10 * (a1 % b1)
            d, d1 = a / b, a1 / b1
	*/
}

GrapaFloat GrapaFloat::E(const GrapaFloat& pexp)
{
	GrapaFloat z(pexp);
	if (z.mTrunc)
	{
		z = GrapaFloat(true, pexp.mMax, pexp.mExtra, 1) * z;
		z.mTrunc = false;
	}
	z.mFix = true;
	z.mMax = pexp.mMax + pexp.mExtra;;
	s64 maxbits = z.mMax;
	GrapaFloat result(true, maxbits, pexp.mExtra, 1);
	GrapaFloat pow(true, maxbits, pexp.mExtra, 1);
	GrapaFloat fact(true, maxbits, pexp.mExtra, 1);
	GrapaFloat temp(true, maxbits, pexp.mExtra, 0);
	GrapaFloat lasttemp(true, maxbits, pexp.mExtra, 0);
	s64 i = 0;
	s64 oldExp = result.mExp;
	temp.mExp = result.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		if (z.mExp != 0 || z.mBits>1)
			pow = pow * z;
		fact = fact * i;
		pow.mFix = false;
		pow.mMax = pow.mData.bitCount();
		if (pow.mMax<maxbits)
			pow.mMax = maxbits;
		temp = pow / fact;
		pow.mFix = true;
		pow.mMax = maxbits;
		result = result + temp;
		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -z.mMax);
	result.mMax = pexp.mMax;
	result.Truncate();
	return(result);

	/*
	u64 n = pexp.mMax / 6;
	GrapaCHAR estr("2.");
	u64 prec=10^n;
	*/

	/*
	def espig(n):
		estr = '2.'
		prec = 10 ** n
		ys = [1 for _ in range(2, n + 1)]

		while len(estr) <= n:
			carover, asum, remainders = 0, 0, []

			for x, y in zip(range(n, 1, -1), ys):
				asum = carover + prec * y
				carover, remainder = divmod(asum, x)
				remainders.append(remainder)

			ys = remainders
		estr += str(asum // 2)

	return estr
	*/
}

GrapaFloat GrapaFloat::Abs()
{
	if (mSigned) return -*this;
	return *this;
}

GrapaFloat GrapaFloat::LnBase()
{
	GrapaFloat z(*this);
	if (z.mTrunc)
	{
		z = z * GrapaFloat(mFix, mMax, mExtra, 1);
		z.mTrunc = false;
	}
	z.mFix = true;
	z.mMax += mExtra;
	s64 maxbits = z.mMax;
	z.mSigned = false;
	GrapaFloat one(true, maxbits, mExtra, 1);
	GrapaFloat m((z - one) / (z + one));
	GrapaFloat m2 (m*m);
	GrapaFloat result(m);
	GrapaFloat zpow(m);
	GrapaFloat temp(true, maxbits, mExtra, 0);
	GrapaFloat lasttemp(true, maxbits, mExtra, 0);
	s64 i = 0;
	s64 oldExp = z.mExp;
	temp.mExp = z.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		zpow = zpow * m2;
		zpow.mFix = false;
		zpow.mMax = zpow.mData.bitCount();
		if (zpow.mMax<maxbits)
			zpow.mMax = maxbits;
		temp = zpow / GrapaFloat(true, maxbits, mExtra, 2 * i + 1);
		zpow.mFix = true;
		zpow.mMax = maxbits;
		result = result + temp;

		if (temp.mExp >= oldExp)
		{
			if (temp == lasttemp)
				break;
			lasttemp = temp;
		}

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -z.mMax);

	result.mExp++;
	result.mFix = mFix;
	result.mMax = mMax;
	result.Truncate();
	return(result);
}

GrapaFloat GrapaFloat::Log(const GrapaFloat& bx)
{
	GrapaFloat z(*this);
	GrapaFloat b(bx);
	s64 ext = mExtra > bx.mExtra ? mExtra : bx.mExtra;
	if (z.mTrunc)
	{
		z = z * GrapaFloat(mFix, mMax, ext, 1);
		z.mTrunc = false;
	}
	if (b.mTrunc)
	{
		b = b * GrapaFloat(mFix, mMax, ext, 1);
		b.mTrunc = false;
	}
	z.mFix = true;
	b.mFix = true;
	z.mMax += ext;
	s64 maxbits = z.mMax;
	b.mMax = z.mMax;
	z.mSigned = false;
	GrapaFloat twoln = GrapaFloat(true, maxbits, ext, 2).LnBase();
	GrapaFloat ze(true, maxbits, ext, z.mExp);
	GrapaFloat be(true, maxbits, ext, b.mExp);
	z.mExp = 0;
	b.mExp = 0;
	GrapaFloat result = (z.LnBase() + (twoln*ze)) / (b.LnBase() + (twoln*be));
	result.mFix = mFix;
	result.mMax = mMax;
	result.mExtra = ext;
	result.Truncate();
	return(result);
}

GrapaFloat GrapaFloat::Ln()
{
	GrapaFloat z(*this);
	if (z.mTrunc)
	{
		z = z * GrapaFloat(mFix, mMax, mExtra, 1);
		z.mTrunc = false;
	}
	if (mFix)
	{
		z.mFix = false;
		if ((z.mExp + 1) >= 0)
			z.mMax += (z.mExp + 1);
	}
	z.mMax += mExtra;
	s64 maxbits = z.mMax;
	z.mSigned = false;
	GrapaFloat two(false, maxbits, mExtra, 2);
	GrapaFloat p(false, maxbits, mExtra, z.mExp);
	z.mExp = 0;
	GrapaFloat result = z.LnBase() + (two.LnBase()*p);
	result.mFix = mFix;
	result.mMax = mMax;
	result.Truncate();
	return(result);
}

GrapaFloat GrapaFloat::Sin()
{
	mMax += 4;
	GrapaFloat sin(*this);
	GrapaFloat one(mFix, mMax, mExtra, 1);
	if (sin.mTrunc)
	{
		sin = sin * one;
		sin.mTrunc = false;
	}
	bool oldfix = sin.mFix;
	sin.mFix = true;
	bool sign_sin = sin.mSigned;
	sin.mSigned = false;

	GrapaFloat pi = Pi(one);

	pi.mExp++;
	if (sin > pi)
	{
		sin = sin % pi;
	}
	pi.mExp--;

	if (sin > pi)
	{
		sign_sin = !sign_sin;
		sin = sin % pi;
	}

	pi.mExp--;
	if (sin > pi)
	{
		pi.mExp++;
		sin = sin - pi;
		sin.mSigned = !sin.mSigned;
	}
	else
		pi.mExp++;

	GrapaFloat fact(true, mMax, mExtra, 1);
	GrapaFloat temp(true, mMax, mExtra, 0);
	GrapaFloat lasttemp(true, mMax, mExtra, 0);
	GrapaFloat radpow(sin);
	GrapaFloat rad2(sin*sin);
	u64 i = 0;
	s64 oldExp = sin.mExp;
	temp.mExp = sin.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		radpow = radpow * rad2;
		radpow.mSigned = ((i % 2) == 1) ? true : false;
		fact = fact * GrapaFloat(true, mMax, mExtra, (2 * i) * (2 * i + 1));

		radpow.mFix = false;
		radpow.mMax = radpow.mData.bitCount();
		if (radpow.mMax< mMax)
			radpow.mMax = mMax;
		temp = radpow / fact;
		radpow.mFix = true;
		radpow.mMax = mMax;

		sin = sin + temp;

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -sin.mMax);

	if (sign_sin)
		sin.mSigned = true;

	sin.mFix = true;
	sin.mMax = mMax-4;
	sin.Truncate();

	return(sin);
}

GrapaFloat GrapaFloat::Cos()
{
	GrapaFloat rad(*this);
	GrapaFloat one(mFix, mMax, mExtra, 1);
	if (rad.mTrunc)
	{
		rad = rad * one;
		rad.mTrunc = false;
	}
	bool oldfix = rad.mFix;
	rad.mFix = true;
	rad.mSigned = false;

	GrapaFloat pi = Pi(one);

	pi.mExp++;
	if (rad > pi)
	{
		rad = rad % pi;
	}
	pi.mExp--;

	bool sign_cos = false;
	bool maybesign_cos = false;

	pi.mExp--;
	if (rad > pi)
		maybesign_cos = true;
	pi.mExp++;

	if (rad > pi)
	{
		rad = rad % pi;
		pi.mExp--;
		if (rad < pi && maybesign_cos)
			sign_cos = true;
		pi.mExp++;
	}
	else if (maybesign_cos)
	{
		sign_cos = true;
	}

	pi.mExp--;
	if (rad > pi)
	{
		pi.mExp++;
		rad = rad - pi;
		rad.mSigned = !rad.mSigned;
	}
	else
		pi.mExp++;

	GrapaFloat cos(true, mMax, mExtra,1);
	GrapaFloat fact(true, mMax, mExtra, 1);
	GrapaFloat temp(true, mMax, mExtra, 0);
	GrapaFloat lasttemp(true, mMax, mExtra, 0);
	GrapaFloat radpow(true, mMax, mExtra, 1);
	GrapaFloat rad2(rad*rad);
	u64 i = 0;
	s64 oldExp = rad.mExp;
	temp.mExp = rad.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		radpow = radpow * rad2;
		radpow.mSigned = ((i % 2) == 1) ? true : false;
		fact = fact * (s64)((2 * i - 1) * (2 * i));

		radpow.mFix = false;
		radpow.mMax = radpow.mData.bitCount();
		if (radpow.mMax< mMax)
			radpow.mMax = mMax;
		temp = radpow / fact;
		radpow.mFix = true;
		radpow.mMax = mMax;

		cos = cos + temp;

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -rad.mMax);


	cos.mSigned = sign_cos;

	cos.mFix = true;
	cos.mMax = mMax;
	cos.Truncate();

	return(cos);
}

GrapaFloat GrapaFloat::Tan()
{
	GrapaFloat rad(*this);
	if (rad.mTrunc)
	{
		rad = rad * GrapaFloat(mFix, mMax, mExtra, 1);
		rad.mTrunc = false;
	}
	bool oldfix = rad.mFix;
	rad.mFix = true;
	rad.mSigned = false;

	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat pi = Pi(one);

	bool sign_sin = rad.mSigned;
	bool sign_cos = false;
	bool maybesign_cos = false;
	rad.mSigned = false;

	pi.mExp++;
	if (rad > pi)
	{
		rad = rad % pi;

	}
	pi.mExp--;

	pi.mExp--;
	if (rad > pi)
		maybesign_cos = true;
	pi.mExp++;

	if (rad > pi)
	{
		sign_sin = !sign_sin;
		rad = rad % pi;
		pi.mExp--;
		if (rad < pi && maybesign_cos)
			sign_cos = true;
		pi.mExp++;
	}
	else if (maybesign_cos)
	{
		sign_cos = true;
	}

	pi.mExp--;
	if (rad > pi)
	{
		pi.mExp++;
		rad = rad - pi;
		rad.mSigned = !rad.mSigned;
	}
	else
		pi.mExp++;

	GrapaFloat sin(rad);
	GrapaFloat cos(true, mMax, mExtra, 1);
	GrapaFloat fact(true, mMax, mExtra, 1);
	GrapaFloat temp(true, mMax, mExtra, 0);
	GrapaFloat lasttemp(true, mMax, mExtra, 0);
	GrapaFloat radpow(rad);

	u64 i = 0;
	s64 oldExp = rad.mExp;
	temp.mExp = rad.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		radpow = radpow * rad;
		radpow.mSigned = ((i % 2) == 1) ? true : false;
		fact = fact * (s64)(2 * i);

		radpow.mFix = false;
		radpow.mMax = radpow.mData.bitCount();
		if (radpow.mMax< mMax)
			radpow.mMax = mMax;
		temp = radpow / fact;
		radpow.mFix = true;
		radpow.mMax = mMax;

		cos = cos + temp;

		radpow = radpow * rad;
		radpow.mSigned = ((i % 2) == 1) ? true : false;
		fact = fact * (s64)(2 * i + 1);

		radpow.mFix = false;
		radpow.mMax = radpow.mData.bitCount();
		if (radpow.mMax< mMax)
			radpow.mMax = mMax;
		temp = radpow / fact;
		radpow.mFix = true;
		radpow.mMax = mMax;

		sin = sin + temp;

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -rad.mMax);

	if (sign_sin)
		sin.mSigned = true;

	cos.mSigned = sign_cos;

	GrapaFloat tan = sin / cos;

	tan.mFix = true;
	tan.mMax = mMax;
	tan.Truncate();

	return(tan);
}

GrapaFloat GrapaFloat::Cot()
{
	return (GrapaFloat(mFix,mMax, mExtra,1) / Tan());
}

GrapaFloat GrapaFloat::Sec()
{
	return (GrapaFloat(mFix,mMax, mExtra,1) / Cos());
}

GrapaFloat GrapaFloat::Csc()
{
	return (GrapaFloat(mFix,mMax, mExtra,1) / Sin());
}

GrapaFloat GrapaFloat::ASin()
{
	if (mExp > 0 || (mExp == 0 && mBits > 1)) return(GrapaFloat(mFix, mMax, mExtra, 0));
	GrapaFloat rad(*this);
	if (rad.mTrunc)
	{
		rad = rad * GrapaFloat(mFix, mMax, mExtra, 1);
		rad.mTrunc = false;
	}
	if (rad.mFix)
	{
		rad.mFix = false;
		if ((rad.mExp + 1) >= 0)
			rad.mMax += (rad.mExp + 1);
	}
	rad.mMax += mExtra;
	s64 maxbits = rad.mMax;

	GrapaFloat one(false, maxbits, mExtra, 1);
	GrapaFloat two(false, maxbits, mExtra, 2);

	GrapaFloat result = two * (rad / (one + (one - (rad*rad)).Root(2))).ATan();

	result.mFix = true;
	result.mMax = mMax;
	result.Truncate();

	return(result);

}

GrapaFloat GrapaFloat::ACos()
{
	if (mExp > 0 || (mExp == 0 && mBits > 1)) return(GrapaFloat(mFix, mMax, mExtra, 0));
	GrapaFloat rad(*this);
	if (rad.mTrunc)
	{
		rad = rad * GrapaFloat(mFix, mMax, mExtra, 1);
		rad.mTrunc = false;
	}
	if (rad.mFix)
	{
		rad.mFix = false;
		if ((rad.mExp + 1) >= 0)
			rad.mMax += (rad.mExp + 1);
	}
	rad.mMax += mExtra;
	s64 maxbits = rad.mMax;

	GrapaFloat one(false, maxbits, mExtra, 1);
	GrapaFloat two(false, maxbits, mExtra, 2);

	GrapaFloat result = two * ((one - (rad*rad)).Root(2) / (one+rad)).ATan();

	result.mFix = true;
	result.mMax = mMax;
	result.Truncate();

	return(result);
}

GrapaFloat GrapaFloat::ATan()
{
	if (mExp > 0 || (mExp == 0 && mBits > 1)) return(GrapaFloat(mFix,mMax, mExtra,0));
	GrapaFloat rad(*this);
	if (rad.mTrunc)
	{
		rad = rad * GrapaFloat(mFix, mMax, mExtra, 1);
		rad.mTrunc = false;
	}
	if (rad.mFix)
	{
		rad.mFix = false;
		if ((rad.mExp + 1) >= 0)
			rad.mMax += (rad.mExp + 1);
	}
	rad.mMax += mExtra;
	s64 maxbits = rad.mMax;
	GrapaFloat pow(rad);
	GrapaFloat pow2(rad*rad);
	GrapaFloat pow2plusone(pow2+GrapaFloat(false,maxbits, mExtra,1));
	GrapaFloat two(false, maxbits, mExtra, 2);
	GrapaFloat twopow(false, maxbits, mExtra, 0);
	GrapaFloat nfact(false, maxbits, mExtra, 1);
	GrapaFloat nfactsq(false, maxbits, mExtra, 1);
	GrapaFloat twonfact(false, maxbits, mExtra, 1);
	GrapaFloat zpow(false, maxbits, mExtra, 1);
	GrapaFloat temp(false, maxbits, mExtra, 0);
	GrapaFloat lasttemp(false, maxbits, mExtra, 0);
	GrapaFloat result(rad/(pow2+GrapaFloat(false,maxbits, mExtra,1)));
	s64 i = 0;
	s64 oldExp = rad.mExp;
	temp.mExp = rad.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;

		twopow = two.Pow(2 * i);
		nfact = nfact * GrapaFloat(false, maxbits, mExtra, i);
		nfactsq = nfact * nfact;
		twonfact = twonfact * GrapaFloat(false, maxbits, mExtra, (i*2) * (i * 2 + 1));
		
		pow = pow * pow2;
		zpow = pow2plusone.Pow(i + 1);
		temp = (twopow*nfactsq*pow) / (twonfact*zpow);

		result = result + temp;

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -rad.mMax);

	result.mFix = true;
	result.mMax = mMax;
	result.Truncate();

	return(result);
}

GrapaFloat GrapaFloat::ACot()
{
	if (mExp > 0 || (mExp == 0 && mBits > 1)) return(GrapaFloat(mFix,mMax, mExtra,0));
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat pi = Pi(one);
	pi.mExp--;
	GrapaFloat result = pi - ATan();
	result.mFix = mFix;
	result.mMax = mMax;
	result.Truncate();
	return (result);
}

GrapaFloat GrapaFloat::ASec()
{
	if (mExp < 0) return(GrapaFloat(mFix,mMax, mExtra,0));
	GrapaFloat z(GrapaFloat(mFix,mMax, mExtra,1) / (*this));
	return z.ACos();
}

GrapaFloat GrapaFloat::ACsc()
{
	if (mExp < 0) return(GrapaFloat(mFix, mMax, mExtra, 0));
	GrapaFloat z = GrapaFloat(mFix, mMax, mExtra, 1) / (*this);
	return z.ASin();
}

GrapaFloat GrapaFloat::SinH()
{
	GrapaFloat sin(*this);
	if (sin.mTrunc)
	{
		sin = sin * GrapaFloat(mFix, mMax, mExtra, 1);
		sin.mTrunc = false;
	}
	sin.mFix = true;
	bool sign_sin = sin.mSigned;
	sin.mSigned = false;

	GrapaFloat fact(true, mMax, mExtra, 1);
	GrapaFloat temp(true, mMax, mExtra, 0);
	GrapaFloat lasttemp(true, mMax, mExtra, 0);
	GrapaFloat radpow(sin);
	GrapaFloat rad2(sin*sin);
	u64 i = 0;
	s64 oldExp = sin.mExp;
	temp.mExp = sin.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		radpow = radpow * rad2;
		fact = fact * GrapaFloat(true, mMax, mExtra, (2 * i) * (2 * i + 1));

		radpow.mFix = false;
		radpow.mMax = radpow.mData.bitCount();
		if (radpow.mMax< mMax)
			radpow.mMax = mMax;
		temp = radpow / fact;
		radpow.mFix = true;
		radpow.mMax = mMax;

		sin = sin + temp;

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -sin.mMax);

	if (sign_sin)
		sin.mSigned = true;

	sin.mFix = true;
	sin.mMax = mMax;
	sin.Truncate();

	return(sin);
}

GrapaFloat GrapaFloat::CosH()
{
	GrapaFloat rad(*this);
	if (rad.mTrunc)
	{
		rad = rad * GrapaFloat(mFix, mMax, mExtra, 1);
		rad.mTrunc = false;
	}
	rad.mFix = true;

	GrapaFloat cos(true, mMax, mExtra, 1);
	GrapaFloat fact(true, mMax, mExtra, 1);
	GrapaFloat temp(true, mMax, mExtra, 0);
	GrapaFloat lasttemp(true, mMax, mExtra, 0);
	GrapaFloat radpow(true, mMax, mExtra, 1);
	GrapaFloat rad2(rad*rad);
	u64 i = 0;
	s64 oldExp = rad.mExp;
	temp.mExp = rad.mExp;
	bool checklasttemp = true;
	do
	{
		i++;
		oldExp = temp.mExp;
		radpow = radpow * rad2;
		fact = fact * (s64)((2 * i - 1) * (2 * i));

		radpow.mFix = false;
		radpow.mMax = radpow.mData.bitCount();
		if (radpow.mMax< mMax)
			radpow.mMax = mMax;
		temp = radpow / fact;
		radpow.mFix = true;
		radpow.mMax = mMax;

		cos = cos + temp;

		if (checklasttemp && (temp.mExp > lasttemp.mExp || temp > lasttemp))
		{
			lasttemp = temp;
			continue;
		}
		checklasttemp = false;

	} while (temp.mExp > 0 || temp.mExp > -cos.mMax);

	cos.mFix = true;
	cos.mMax = mMax;
	cos.Truncate();

	return(cos);
}

GrapaFloat GrapaFloat::TanH()
{
	GrapaFloat e1 = GrapaFloat(mFix, mMax, mExtra, 2) * (*this);
	e1.mSigned = !e1.mSigned;
	GrapaFloat e2(GrapaFloat::E(e1));
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat result = (one - e2) / (one + e2);
	return(result);
}

GrapaFloat GrapaFloat::CotH()
{
	GrapaFloat e1 = GrapaFloat(mFix, mMax, mExtra, 2) * (*this);
	e1.mSigned = !e1.mSigned;
	GrapaFloat e2(GrapaFloat::E(e1));
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat result = (one + e2) / (one - e2);
	return(result);
}

GrapaFloat GrapaFloat::SecH()
{
	return (GrapaFloat(mFix, mMax, mExtra, 1) / CosH());
}

GrapaFloat GrapaFloat::CscH()
{
	return (GrapaFloat(mFix, mMax, mExtra, 1) / SinH());
}

GrapaFloat GrapaFloat::ASinH()
{
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat x2((*this)*(*this));
	return ((*this)+(x2+one).Root(2)).Ln();
}

GrapaFloat GrapaFloat::ACosH()
{
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat x2((*this)*(*this));
	return ((*this) + (x2 - one).Root(2)).Ln();
}

GrapaFloat GrapaFloat::ATanH()
{
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat two(mFix, mMax, mExtra, 2);
	return ((one + (*this)) / (one - (*this))).Ln() / two;
}

GrapaFloat GrapaFloat::ACotH()
{
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat two(mFix, mMax, mExtra, 2);
	return (((*this) + one) / ((*this) - one)).Ln() / two;
}

GrapaFloat GrapaFloat::ASecH()
{
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat x2((*this)*(*this));
	return ((one / (*this)) + (one - x2).Root(2) / (*this)).Ln();
}

GrapaFloat GrapaFloat::ACscH()
{
	GrapaFloat one(mFix, mMax, mExtra, 1);
	GrapaFloat x2((*this)*(*this));
	GrapaFloat ab(*this);
	ab.mSigned = false;
	return ((one / (*this)) + (one + x2).Root(2) / ab).Ln();
}

GrapaFloat GrapaFloat::ATan2(const GrapaFloat& xp, const GrapaFloat& yp)
{
	s64 max = xp.mMax > yp.mMax ? xp.mMax : yp.mMax;
	s64 ext = xp.mExtra > yp.mExtra ? xp.mExtra : yp.mExtra;
	GrapaFloat one(false, max, ext, 1);
	GrapaFloat pi(GrapaFloat::Pi(one));
	GrapaFloat x(xp);
	GrapaFloat y(yp);

	GrapaFloat zero(false, max, ext, 0);
	GrapaFloat two(false, max, ext, 2);

	if (y == zero && x == zero)
	{
		return zero; // undefined
	}
	if (x > zero)
	{
		return (y / x).ATan();
	}
	if (y >= zero && x < zero)
	{
		return (y / x).ATan() + pi;
	}
	if (y < zero && x < zero)
	{
		return (y / x).ATan() - pi;
	}
	if (x == zero)
	{
		GrapaFloat result = pi / two;
		if (y < zero)
			result.mSigned = !result.mSigned;
		return result;
	}
	return zero;
}

GrapaFloat GrapaFloat::Hypot(const GrapaFloat& xp, const GrapaFloat& yp)
{
	s64 max = xp.mMax > yp.mMax ? xp.mMax : yp.mMax;
	s64 ext = xp.mExtra > yp.mExtra ? xp.mExtra : yp.mExtra;
	GrapaFloat x(xp);
	GrapaFloat y(yp);
	x.mSigned = false;
	y.mSigned = false;
	if (x < y)
	{
		GrapaFloat t = x;
		x = y;
		y = t;
	}
	y = y / x;
	return x * (GrapaFloat(false,max, ext,1) + y * y).Root(2);
}

GrapaCHAR GrapaFloat::ToString()
{
	return ToString(10);
}

GrapaCHAR GrapaFloat::ToString(u8 radix)
{
	GrapaCHAR result, left, right;
	result.mToken = GrapaTokenType::STR;
	GrapaInt a, b;
	s64 lena=0, lenb=0;
	GrapaFloat out(*this);
	out.Truncate(true);
	s64 startbits = out.mBits - out.mExp - 1;
	int lpadbits = 4;
	switch (radix) 
	{
	case 2: lpadbits = 1; break; 
	case 4: lpadbits = 2; break;
	case 8: lpadbits = 3; break;
	case 16: lpadbits = 4; break;
	case 32: lpadbits = 5; break;
	case 64: lpadbits = 6; break;
	};
	switch (radix)
	{
	case 2:
	case 4:
	case 8:
	case 16:
	case 32:
	case 64:
		TO(a, lena, b, lenb, lpadbits);
		left = a.ToString(radix);
		break;
	case 10:
		if (out.mTrunc)
		{
			GrapaFloat bi(out);
			u64 shift = (out.mBits <= (out.mExp + 1)) ? (bi.mBits % 4) : ((bi.mBits - (bi.mExp + 1)) % 4);
			if (shift)
			{
				bi.mData = bi.mData >> shift;
				bi.mBits -= shift;
				if (bi.mBits < 0)
				{
					bi.mBits = 0;
					bi.mExp = 0;
				}
			}
			if (u64 lead = bi.mData.bitStart())
			{
				bi.mData = bi.mData >> lead;
				bi.mBits -= lead;
				if (bi.mBits < 0)
				{
					bi.mBits = 0;
					bi.mExp = 0;
				}
			}
			if (bi.mBits && bi.mBits > (out.mExp + 1))
			{
				u64 bits = bi.mBits;
				bi.mData = bi.mData + 1;
				bi.mBits = bi.mData.bitCount();
				if (bits != bi.mBits)
					bi.mExp++;
			}
			bi.TO(a, lena, b, lenb, lpadbits);
			left = a.ToString(radix);
			if (!out.mFix)
			{
				// This is the more acturate formulat than the 3/10 ratio, but takes more time to compute
				// n = (GrapaFloat(lena) * (GrapaFloat(1) / (GrapaFloat(10).Log(16.0) * 4.0))).ToInt().LongValue();
				for (u64 i = ((lena * 3) / 10) + 1; i < left.mLength; i++)
					left.mBytes[i] = '0';
			}
		}
		else
		{
			out.TO(a, lena, b, lenb, lpadbits);
			left = a.ToString(radix);
		}
		break;
	}
	if (out.mSigned) result.Append('-');
	result.Append(left);
	if (lenb)
	{
		GrapaInt c;
		GrapaCHAR pad;
		u64 p = lpadbits;
		u64 bits;
		switch (radix)
		{
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
			//switch (radix)
			//{
			//case 2: p = 1; break;
			//case 4: p = 2; break;
			//case 8: p = 3; break;
			//case 16: p = 4; break;
			//case 32: p = 5; break;
			//case 64: p = 6; break;
			//}
			right = b.ToString(radix);
			right.RTrim('0');
			if (((lenb * p) / radix) > right.mLength)
				pad.Pad(((lenb * p) / radix) - right.mLength, '0');
			break;
		case 10:
			bits = lenb - b.bitStart();
			c = radix;
			b = b * c.Pow(lenb);
			b = b >> lenb;
			right = b.ToString(radix);
			right.RTrim('0');
			if (bits > right.mLength)
				pad.Pad(bits - right.mLength, '0');
			if (out.mTrunc)
			{
				//s64 max = mMax;
				//if (mFix)
				//	max += a.bitCount();
				//if (right.mLength > ((max - a.bitCount()) / 4))
				//{
				//	right.mLength = ((max - a.bitCount()) / 4);
				//}
				if (right.mLength > (u64)startbits / 4)
					right.mLength = (u64)startbits / 4;
				right.RTrim('0');
			}
			break;
		}
		if (right.mLength)
		{
			result.Append('.');
			result.Append(pad);
			result.Append(right);
		}
		else
		{
			result.Append(".0");
		}
	}
	else
	{
		result.Append(".0");
	}
	return result;
}

void GrapaFloat::FromString(const GrapaBYTE& pIn, u8 radix, s64 max)
{
	mData = 0;
	mExp = 0;
	if (pIn.mLength == 0 || pIn.mBytes == NULL) return;
	GrapaCHAR s1, s2;
	if (char* ps = strchr((char*)pIn.mBytes, '.'))
	{
		s1.FROM((char*)pIn.mBytes, ps - (char*)pIn.mBytes);
		s2.FROM((char*)&pIn.mBytes[ps - (char*)pIn.mBytes + 1], pIn.mLength - 1 - (ps - (char*)pIn.mBytes));
	}
	else
		s1.FROM(pIn);
	s1.Trim(' ');
	s2.Trim(' ');
	s2.RTrim('0');
	mData.FromString(s1, radix);
	if (mData.dataSigned)
	{
		mSigned = true;
		mData = -mData;
	}
	mBits = 0;
	mExp = mData.bitCount() - 1;
	GrapaInt n2;
	s64 bits = 0;
	u64 lead = 0;
	GrapaCHAR bucket;
	//bool roundUp = false;
	if (s2.mLength)
	{
		switch (radix)
		{
		case 2:
		case 4:
		case 8:
		case 16:
			lead = ((radix == 2) ? 1 : (radix == 4) ? 2 : (radix == 8) ? 3 : 4);
			lead -= s2.mLength % lead;
			bucket.Pad(lead, 0);
			s2.Append(bucket);
			n2.FromString(s2, radix);
			bits = ((radix == 2) ? 1 : (radix == 4) ? 2 : (radix == 8) ? 3 : 4) * s2.mLength;
			break;
		case 10:
			for (lead = 0; lead < s2.mLength; lead++)
			{
				s2.mBytes[lead] -= '0';
				if (s2.mBytes[lead]) break;
			}
			if (max == 0)
			{
				max = mMax+mExtra;
				if (!mFix)
					max -= (((s64)s1.mLength) - 1) * 4;
				if (max < 0)
					max = 0;
				if (max < ((s64)s2.mLength * 8))
					max = (s64)s2.mLength * 8;
			}
			max = max / 6;
			for (lead = lead + 1; lead < s2.mLength; lead++)
				s2.mBytes[lead] -= '0';
			bucket.SetSize(max + 1);
			bucket.SetLength(1);
			bucket.mBytes[0] = 0;

			for (s64 k = 1; k < max + 1; k++)
			{
				bucket.mBytes[k] = 0;
				for (s64 j = 7; j >= 0; j--)
				{
					u8 c = 0, r = 0;
					for (s64 i = s2.mLength - 1; i >= 0; i--)
					{
						c = s2.mBytes[i];
						if (c || r)
						{
							c = (c << 1) + r;
							s2.mBytes[i] = c % 10;
							r = c / 10;
						}
					}
					if (r)
						bucket.mBytes[k] |= (1 << j);
					while (s2.mLength && s2.mBytes[s2.mLength - 1] == 0) s2.mLength--;
					if (s2.mLength == 0) break;
				}
				bucket.mLength = k + 1;
				if (s2.mLength == 0) break;
			}
			if (bucket.mLength == max + 1)
			{
				mTrunc = true;
			}
			while (bucket.mLength && bucket.mBytes[bucket.mLength - 1] == 0) bucket.mLength--;
			if (bucket.mLength == 1)
			{
				bits = 0;
				break;
			}
			n2.FromBytes(bucket);
			bits = (bucket.mLength - 1) * 8;
			break;
		}
	}
	if (bits)
	{
		lead = n2.bitStart();
		if (lead)
		{
			n2 = n2 >> lead;
			bits -= lead;
		}
		mData = (mData << bits) + n2;
		if (mExp < 0)
		{
			lead = n2.bitCount();
			mExp = lead - bits - 1;
		}
	}
	mBits = mData.bitCount();
	if (mBits > mMax) mMax = mBits;
	//mMax = mBits;
	//s64 tmax = mFix ? (mBits - mExp - 1) : mBits;
	//mMax = (mMax < tmax) ? mMax : tmax;
	Truncate();
}

void GrapaFloat::FromBytes(const GrapaBYTE& result)
{
	if (result.mToken == GrapaTokenType::INT || result.mToken == GrapaTokenType::TIME)
	{
		GrapaInt a;
		a.FromBytes(result);
		FromInt(a);
		return;
	}

	mBits = 0;
	mExp = 0;
	mData = 0;
	mTrunc = false;
	mSigned = false;
	bool isExtended = false;

	if (result.mLength < 3) return;

	u8 header = result.mBytes[0];

	mSigned = (header & 0x80) == 0x80;
	isExtended = (header & 0x40) == 0x40;
	mTrunc = (header & 0x20) == 0x20;
	mFix = (header & 0x10) == 0x10;
	u8 expB = (header & 0x07);

	u64 n = 0;
	if (result.mBytes[1] & 0x80) n = -1;
	memcpy(&((u8*)&n)[sizeof(u64) - (((u64)expB) + 1)], &result.mBytes[1], ((u64)expB) + 1);
	mExp = BE_S64(n);

	s64 maxBits = 0;
	s64 countBytes = GrapaMem::BytesToLen((char*)&result.mBytes[1 + (((u64)expB) + 1)], result.mLength - (1 + (((u64)expB) + 1)), maxBits);
	if (countBytes <= 0) countBytes = 0;

	s64 extraBits = 0;
	s64 countBytes2 = GrapaMem::BytesToLen((char*)&result.mBytes[1 + (((u64)expB) + 1) + countBytes], result.mLength - (1 + (((u64)expB) + 1) + countBytes), extraBits);
	if (countBytes2 <= 0) countBytes2 = 0;

	s64 countBytes3 = 0;
	s64 dataBytes = 0;
	if (isExtended)
	{
		countBytes3 = GrapaMem::BytesToLen((char*)&result.mBytes[1 + (((u64)expB) + 1) + countBytes + countBytes2], result.mLength - (1 + (((u64)expB) + 1) + countBytes + countBytes2), dataBytes);
		if (countBytes3 <= 0) countBytes3 = 0;
	}

	GrapaBYTE b;
	if (dataBytes) b.SetLength(dataBytes);
	else b.SetLength(result.mLength - (1 + (((u64)expB) + 1) + countBytes + countBytes2 + countBytes3));
	memcpy(b.mBytes, &result.mBytes[1 + (((u64)expB) + 1) + countBytes + countBytes2 + countBytes3], (size_t)b.mLength);
	mData.FromBytes(b);

	mTail.SetLength(0);
	if (dataBytes)
	{
		mTail.SetLength(result.mLength - (1 + (((u64)expB) + 1) + countBytes + countBytes2 + countBytes3 + dataBytes));
		memcpy(mTail.mBytes, &result.mBytes[1 + (((u64)expB) + 1) + countBytes + countBytes2 + countBytes3 + dataBytes], (size_t)mTail.mLength);
	}

	if (extraBits >= 0)
		mExtra = extraBits;

	mBits = mData.bitCount();
	if (maxBits >= 0)
		mMax = maxBits;
	else
	{
		s64 max = mFix ? (mBits - mExp - 1) : mBits;
		if (mMax < max) mMax = max;
	}
	bool oldtrunc = mTrunc;
	Truncate();
	mTrunc = mTrunc || oldtrunc;
}

GrapaBYTE GrapaFloat::getBytes()
{
	// if special case
	// mSigned=true; mExp = 0; mBits = 0; mData = code;
	u64 mask1 = 0xFF00000000000000;
	u64 mask2 = 0x8000000000000000;
	u8 expB = 8;
	bool isNeg = (mExp < 0);
	while (expB && (mExp & mask1) == (isNeg ? mask1 : 0))
	{
		mask1 >>= 8;
		mask2 >>= 8;
		expB--;
	}
	if ((mExp & mask2) == (isNeg ? 0 : mask2))
		expB++;
	if (expB) expB--;

	// bits 0x20, 0x10, 0x80 -> 8 special states
	u8 header = expB;
	bool isExtended = mTail.mLength!=0;
	if (mSigned) header |= 0x80;
	if (isExtended) header |= 0x40;
	if (mTrunc) header |= 0x20;
	if (mFix) header |= 0x10;

	GrapaBYTE b = mData.getBytes();
	GrapaCHAR result;
	result.SetLength(1 + (((u64)expB) + 1) + b.mLength + 3*(sizeof(s64)*4/3)); //max number of bits for mMax that could be added
	result.mBytes[0] = header;

	u64 n = BE_S64(mExp);
	memcpy(&result.mBytes[1], &((u8*)&n)[sizeof(u64) - (((u64)expB) + 1)], (((u64)expB) + 1));

	s64 countBytes = GrapaMem::LenToBytes(mMax, (char*)&result.mBytes[1 + (((u64)expB) + 1)],  (sizeof(s64)*4/3));
	if (countBytes < 0) countBytes = 0;

	s64 countBytes2 = GrapaMem::LenToBytes(mExtra, (char*)&result.mBytes[1 + (((u64)expB) + 1) + countBytes], (sizeof(s64) * 4 / 3));
	if (countBytes2 < 0) countBytes2 = 0;

	s64 tailBytes = 0;
	s64 countBytes3 = 0;
	if (isExtended)
	{
		tailBytes = mTail.mLength;
		countBytes3 = GrapaMem::LenToBytes(b.mLength, (char*)&result.mBytes[1 + (((u64)expB) + 1) + countBytes + countBytes2], (sizeof(s64) * 4 / 3));
		if (countBytes3 < 0) countBytes3 = 0;
	}

	result.SetLength     (1 + (((u64)expB) + 1) + countBytes + countBytes2 + countBytes3 + b.mLength + tailBytes); //reset the length now that we know how may bits for mMax
	memcpy(&result.mBytes[1 + (((u64)expB) + 1) + countBytes + countBytes2 + countBytes3], b.mBytes, (size_t)b.mLength);
	if (isExtended)
	{
		memcpy(&result.mBytes[1 + (((u64)expB) + 1) + countBytes + countBytes2 + countBytes3 + b.mLength], mTail.mBytes, (size_t)tailBytes);
	}

	result.mToken = GrapaTokenType::FLOAT;
	return result;
}

void GrapaFloat::Truncate(bool display)
{
	bool hasfraction = ((mExp + 1) <= 0);
	if (display && mExtra > 0)
		mExtra = 0;
	if (mFix)
	{
		s64 bits = mBits - mExp;
		if (bits > (mMax + mExtra))
		{
			mData = mData >> (bits - (mMax + mExtra));
			mBits -= (bits - (mMax + mExtra));
			if (mBits < 0) mBits = 0;
			mTrunc = true;
		}
		s64 lead = mData.bitStart();
		if (lead)
		{
			mData = mData >> lead;
			mBits -= lead;
			if (mBits < 0) mBits = 0;
			if (mTrunc && (mBits - mExp + lead) < (mMax + mExtra))  // Not sure if this line ever is true
				mTrunc = false;
		}
	}
	else
	{
		s64 bits = hasfraction ? mBits : mBits - mExp;
		if (bits > (mMax + mExtra))
		{
			mData = mData >> (bits - (mMax + mExtra));
			mBits -= (bits - (mMax + mExtra));
			if (mBits < 0) mBits = 0;
			mTrunc = true;
		}
		s64 lead = mData.bitStart();
		if (lead)
		{
			mData = mData >> lead;
			mBits -= lead;
			if (mBits < 0) mBits = 0;
			if (mTrunc && (mBits + 8) < (mMax + mExtra))  // Not sure if this line ever is true
				mTrunc = false;
		}
	}
	if (hasfraction && !((mExp + 1) <= 0))
		mTrunc = false;
	if (mData.IsZero())
	{
		//mExp = -1;
		mTrunc = false;
		mSigned = false;
	}
}

GrapaInt GrapaFloat::ToInt()
{
	GrapaInt a, b;
	s64 lena = 0, lenb = 0;
	TO(a, lena, b, lenb, 4);
	return mSigned ? -a : a;
}

void GrapaFloat::FromInt(const GrapaInt& bi)
{
	if (bi.dataSigned)
	{
		mSigned = true;
		GrapaInt bi2(bi);
		mData = -bi2;
	}
	else
	{
		mData = bi;
	}
	mBits = mData.bitCount();
	mExp = mBits - 1;
	mTrunc = false;
	Truncate();

}