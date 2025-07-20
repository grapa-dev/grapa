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
	GrapaFloat operator +(const GrapaFloat& bi2) const;
	GrapaFloat operator +(s64 bi) const;
	GrapaFloat operator +=(const GrapaFloat& bi1);
	GrapaFloat& operator ++();
	GrapaFloat operator ++(int);
	GrapaFloat operator -(const GrapaFloat& bi2) const;
	GrapaFloat operator -(s64 bi) const;
	GrapaFloat operator -=(const GrapaFloat& bi1);
	GrapaFloat& operator --();
	GrapaFloat operator --(int);
	GrapaFloat operator *(const GrapaFloat& bi2) const;
	GrapaFloat operator *(const GrapaInt& bi2) const;
	GrapaFloat operator *(s64 bi) const;
	GrapaFloat operator *(double bi) const;
	GrapaFloat operator <<(int shiftVal);
	GrapaFloat operator >>(int shiftVal);
	GrapaFloat operator ~() const;
	GrapaFloat operator -() const;
	bool operator ==(const GrapaFloat& bi2) const;
	bool operator !=(const GrapaFloat& bi2) const;
	bool Equals(const GrapaFloat& bi) const;
	bool operator >(const GrapaFloat& bi2) const;
	bool operator >(s64 i1) const;
	bool operator <(const GrapaFloat& bi2) const;
	bool operator <(s64 bi) const;
	bool operator >=(const GrapaFloat& bi2) const;
	bool operator <=(const GrapaFloat& bi2) const;
	GrapaFloat Comp(const GrapaBYTE& bval) const;
	GrapaFloat Comp(const GrapaFloat& bval) const;
	GrapaFloat Comp(const GrapaInt& bval) const;
	GrapaInt Div(const GrapaFloat& bi2, GrapaFloat& remainder) const;
	GrapaFloat operator /(const GrapaFloat& bi2);
	GrapaFloat operator /(const GrapaInt& bi);
	GrapaFloat operator /(s64 bi);
	GrapaFloat operator %(const GrapaFloat& bi2) const;
	GrapaFloat operator %(const GrapaInt& bi2) const;
	GrapaFloat operator %(s64 bi) const;
	GrapaFloat operator %=(const GrapaFloat& bi1);
	GrapaFloat operator &(const GrapaFloat& bi2) const;
	GrapaFloat operator |(const GrapaFloat& bi2) const;
	GrapaFloat operator ^(const GrapaFloat& bi2) const;
	GrapaFloat Pow(const GrapaInt& pexp) const;
	GrapaFloat Pow2(const GrapaFloat& pexp) const;
	GrapaFloat Root(const GrapaInt& pexp) const;
	GrapaFloat Root2(const GrapaFloat& pexp) const;
	static GrapaFloat Pi(const GrapaFloat& pexp);
	static GrapaFloat E(const GrapaFloat& pexp);
	GrapaFloat Abs() const;
	GrapaFloat LnBase() const;
	GrapaFloat Log(const GrapaFloat& b) const;
	GrapaFloat Ln() const;
	GrapaFloat Sin() const;
	GrapaFloat Cos() const;
	GrapaFloat Tan() const;
	GrapaFloat Cot() const;
	GrapaFloat Sec() const;
	GrapaFloat Csc() const;
	GrapaFloat ASin() const;
	GrapaFloat ACos() const;
	GrapaFloat ATan() const;
	GrapaFloat ACot() const;
	GrapaFloat ASec() const;
	GrapaFloat ACsc() const;
	GrapaFloat SinH() const;
	GrapaFloat CosH() const;
	GrapaFloat TanH() const;
	GrapaFloat CotH() const;
	GrapaFloat SecH() const;
	GrapaFloat CscH() const;
	GrapaFloat ASinH() const;
	GrapaFloat ACosH() const;
	GrapaFloat ATanH() const;
	GrapaFloat ACotH() const;
	GrapaFloat ASecH() const;
	GrapaFloat ACscH() const;
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
	GrapaBYTE getBytes() const;
	void Truncate(bool display=false);
	
	// Helper function for robust numeric string validation
	static bool IsValidNumericString(const GrapaCHAR& str);
};

#endif //_GrapaFloat_

////////////////////////////////////////////////////////////////////////////////
