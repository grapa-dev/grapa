// GrapaFloat.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaFloat_
#define _GrapaFloat_

#include "GrapaInt.h"

class GrapaFloat
{
public:
	bool mSigned, mNaN, mTrunc, mFix;
	s64 mExp, mBits, mMax, mExtra;
	GrapaInt mData;
	GrapaBYTE mTail; //for future proofing...will be maintained in serialization
public:
	GrapaFloat() { INIT(); };
	GrapaFloat(bool pFix, s64 pMax, s64 pExtra, s64 pValue);
	GrapaFloat(bool pFix, s64 pMax, s64 pExtr, const GrapaInt& bi);
	GrapaFloat(bool pFix, s64 pMax, s64 pExtr, const GrapaFloat& bi);
	GrapaFloat(const GrapaFloat& bi);
	GrapaFloat(const GrapaBYTE& inData);
	GrapaFloat(const GrapaInt& bi);
	GrapaFloat(double pValue);
	void INIT() { mSigned = mTrunc = mNaN = false; mExp = mBits = 0; mData = 0; mMax = 4 * 8; mExtra = 10; mFix = false; }
	void FROM(const GrapaFloat& pData) { mSigned = pData.mSigned; mNaN = pData.mNaN; mTrunc = pData.mTrunc; mExp = pData.mExp; mBits = pData.mBits; mMax = pData.mMax; mFix = pData.mFix; mExtra = pData.mExtra; mData = pData.mData; mTail = pData.mTail; }
	void TO(GrapaInt& pInteger, s64& pLenI, GrapaInt& pFraction, s64& pLenF, int lpadbits);
	GrapaFloat& operator=(const GrapaFloat& that);
	GrapaFloat& operator=(const GrapaInt& that);
	static void Add(GrapaFloat& A, GrapaFloat& B, GrapaFloat& result);
	GrapaFloat operator +(const GrapaFloat& bi2);
	GrapaFloat operator +(s64 bi);
	GrapaFloat operator +=(const GrapaFloat& bi1);
	GrapaFloat& operator ++();
	GrapaFloat operator ++(int);
	GrapaFloat operator -(const GrapaFloat& bi2);
	GrapaFloat operator -(s64 bi);
	GrapaFloat operator -=(const GrapaFloat& bi1);
	GrapaFloat& operator --();
	GrapaFloat operator --(int);
	GrapaFloat operator *(const GrapaFloat& bi2);
	GrapaFloat operator *(const GrapaInt& bi2);
	GrapaFloat operator *(s64 bi);
	GrapaFloat operator *(double bi);
	GrapaFloat operator <<(int shiftVal);
	GrapaFloat operator >>(int shiftVal);
	GrapaFloat operator ~();
	GrapaFloat operator -();
	bool operator ==(const GrapaFloat& bi2);
	bool operator !=(const GrapaFloat& bi2);
	bool Equals(const GrapaFloat& bi);
	bool operator >(const GrapaFloat& bi2);
	bool operator >(s64 i1);
	bool operator <(const GrapaFloat& bi2);
	bool operator <(s64 bi);
	bool operator >=(const GrapaFloat& bi2);
	bool operator <=(const GrapaFloat& bi2);
	GrapaInt Div(const GrapaFloat& bi2, GrapaFloat& remainder);
	GrapaFloat operator /(const GrapaFloat& bi2);
	GrapaFloat operator /(const GrapaInt& bi);
	GrapaFloat operator /(s64 bi);
	GrapaFloat operator %(const GrapaFloat& bi2);
	GrapaFloat operator %(const GrapaInt& bi2);
	GrapaFloat operator %(s64 bi);
	GrapaFloat operator %=(const GrapaFloat& bi1);
	GrapaFloat operator &(const GrapaFloat& bi2);
	GrapaFloat operator |(const GrapaFloat& bi2);
	GrapaFloat operator ^(const GrapaFloat& bi2);
	GrapaFloat Pow(const GrapaInt& pexp);
	GrapaFloat Pow2(const GrapaFloat& pexp);
	GrapaFloat Root(const GrapaInt& pexp);
	GrapaFloat Root2(const GrapaFloat& pexp);
	static GrapaFloat Pi(const GrapaFloat& pexp);
	static GrapaFloat E(const GrapaFloat& pexp);
	GrapaFloat Abs();
	GrapaFloat LnBase();
	GrapaFloat Log(const GrapaFloat& b);
	GrapaFloat Ln();
	GrapaFloat Sin();
	GrapaFloat Cos();
	GrapaFloat Tan();
	GrapaFloat Cot();
	GrapaFloat Sec();
	GrapaFloat Csc();
	GrapaFloat ASin();
	GrapaFloat ACos();
	GrapaFloat ATan();
	GrapaFloat ACot();
	GrapaFloat ASec();
	GrapaFloat ACsc();
	GrapaFloat SinH();
	GrapaFloat CosH();
	GrapaFloat TanH();
	GrapaFloat CotH();
	GrapaFloat SecH();
	GrapaFloat CscH();
	GrapaFloat ASinH();
	GrapaFloat ACosH();
	GrapaFloat ATanH();
	GrapaFloat ACotH();
	GrapaFloat ASecH();
	GrapaFloat ACscH();
	static GrapaFloat ATan2(const GrapaFloat& x, const GrapaFloat& y);
	static GrapaFloat Hypot(const GrapaFloat& x, const GrapaFloat& y);
	GrapaCHAR ToString();
	GrapaCHAR ToString(u8 radix);
	bool IsInt();
	bool IsZero();
	GrapaInt ToInt();
	void FromInt(const GrapaInt& bi);
	void FromString(const GrapaBYTE& result, u8 radix, s64 max = 0);
	void FromBytes(const GrapaBYTE& result);
	GrapaBYTE getBytes();
	void Truncate(bool display=false);
};

#endif //_GrapaFloat_

////////////////////////////////////////////////////////////////////////////////
