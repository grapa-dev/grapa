// GrapaInt.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaInt_
#define _GrapaInt_

#include "GrapaValue.h"
#include "GrapaThread.h"

class GrapaArray32
{
public:
	bool dataSigned,NaN;
	u8* mBytes;
	u64 mSize, mLength, mGrow;
public:
	GrapaArray32();
	~GrapaArray32();
	GrapaArray32& operator=(const GrapaArray32& that);
private:
	void INIT();
public:
	virtual void SetLength(u64 pLen = 0, bool pCopy = true);
	virtual void SetSize(u64 pLen = 0, bool pCopy = true);
	virtual void FROM(const GrapaArray32& pData);
	virtual u64 GetCount() const;
	virtual u64 GetByteCount();
	virtual void SetCount(u64 pCount);
	virtual void GrowCount(u64 pCount);
	virtual void SetItem(u64 pItem, u32 pValue);
	virtual u32 GetItem(u64 pItem) const;
	virtual void RTrim();
	virtual bool IsNull();
	virtual bool IsNeg();
	virtual bool IsSignNeg();
    virtual bool IsZero();
	virtual bool IsItem(u32 pItem);
};

class GrapaInt : public GrapaArray32, public GrapaCritical
{
public:
	GrapaInt();
	GrapaInt(s64 value);
	GrapaInt(const GrapaInt& bi);
	GrapaInt(const char* value, s32 radix);
	GrapaInt(const u8* inData, u32 inLen);
	GrapaInt(u32* inData, u32 inCount);
	GrapaInt(const GrapaBYTE& inData);
	~GrapaInt();
	GrapaInt& operator=(const GrapaInt& that);
	void operator =(s64 bi);
	GrapaInt operator +(const GrapaInt& bi2);
	GrapaInt operator +(s64 bi);
	GrapaInt operator +=(const GrapaInt& bi1);
	GrapaInt& operator ++();
	GrapaInt operator ++(int);
	GrapaInt operator -(const GrapaInt& bi2);
	GrapaInt operator -(s64 bi);
	GrapaInt operator -=(const GrapaInt& bi1);
	GrapaInt& operator --();
	GrapaInt operator --(int);
	GrapaInt operator *(const GrapaInt& bi2);
	GrapaInt operator *(s64 bi);
	GrapaInt operator *(u64 bi);
	GrapaInt operator <<(u64 shiftVal);
	int shiftLeft(u32* inData, u32 inCount, u64 shiftVal);
	GrapaInt operator >>(u64 shiftVal);
	int shiftRight(u32* inData, u32 inCount, u64 shiftVal);
	GrapaInt operator ~();
	GrapaInt operator -();
	bool operator ==(const GrapaInt& bi2);
	bool operator !=(const GrapaInt& bi2);
	bool Equals(const GrapaInt& bi);
	bool operator >(const GrapaInt& bi2);
	bool operator >(u64 i1);
	bool operator <(const GrapaInt& bi2);
	bool operator <(s64 bi);
	bool operator >=(const GrapaInt& bi2);
	bool operator <=(const GrapaInt& bi2);
	GrapaInt Abs();
	void multiByteDivide(const GrapaInt& bi2, GrapaInt& outQuotient, GrapaInt& outRemainder);
	void singleByteDivide(const GrapaInt& bi2, GrapaInt& outQuotient, GrapaInt& outRemainder);
	GrapaInt Div(const GrapaInt& bi2, GrapaInt& remainder);
	GrapaInt operator /(const GrapaInt& bi2);
	GrapaInt operator /(u64 bi);
	GrapaInt operator %(const GrapaInt& bi2);
	GrapaInt operator %(u64 bi);
	GrapaInt operator %=(const GrapaInt& bi1);
	GrapaInt operator &(const GrapaInt& bi2);
	GrapaInt operator |(const GrapaInt& bi2);
	GrapaInt operator ^(const GrapaInt& bi2);
	//GrapaInt bxor(const GrapaInt& bi);
	GrapaInt bmax(const GrapaInt& bi);
	GrapaInt bmin(const GrapaInt& bi);
	GrapaInt babs();
	d64 blog();
	d64 blog2();
	d64 blog10();
	GrapaInt LargestPrimeFactor();
	static u64 LargestPrimeFactor64(u64 a);
	GrapaCHAR ToString();
	GrapaCHAR ToString(u64 radix);
	void FromString(const GrapaBYTE& result, u32 radix);
	void FromBytes(const GrapaBYTE& result, bool isUnsigned=false);
	//GrapaInt Factorial();
	GrapaInt Pow(const GrapaInt& exp);
	GrapaInt modPow(const GrapaInt& exp, const GrapaInt& n);
	GrapaInt modFact(const GrapaInt& p);
	static GrapaInt BarrettReduction(const GrapaInt& x, const GrapaInt& n, const GrapaInt& constant);
	GrapaInt gcd(const GrapaInt& bi);
	GrapaInt gcd(s64 i1);
	void Random(u64 bits);
	u64 bitCount();
	u64 bitStart();
	s32 IntValue();
	s64 LongValue();
	GrapaInt modInverse(const GrapaInt& modulus);
	GrapaBYTE getBytes(bool pRaw=false);
	static GrapaCHAR Base(GrapaBYTE& pInput, u64 pFrom, u64 pTo);
	void setBit(u32 bitNum);
	void unsetBit(u32 bitNum);
	bool testBit(u64 bitNum);
	GrapaInt bsqrt();
	static void MulDivTest(u32 rounds);
	static void SqrtTest(u32 rounds);
	static u64 gcd64(u64 a, u64 b);
	static u64 mulinv64(u64 exponent, u64 modulus);
	static u64 Inv64(u64 n, u64 m);
	static u64 modpow64(u64 base, u64 exponent, u64 modulus);
	static u64 Powm64(u64 n, u64 e, u64 m);
	static u64 mul64(u64 a, u64 b, u64 mod);
	static u64 pow64(u64 a, u64 p, u64 mod);
	static u64 sqrt64(u64 ull);
};

class GrapaPolyMod
{
protected:
	// (mod x^r - 1, n)
	u64 m_r, m_deg;
	GrapaInt *mp_a, m_n;
	GrapaInt zero, one, two;
private:
	GrapaPolyMod();
public:
	// default value is x
	GrapaPolyMod(u64 r, const GrapaInt& n);
	GrapaPolyMod(const GrapaPolyMod & p);
	virtual ~GrapaPolyMod();
public:
	GrapaPolyMod & operator = (const GrapaPolyMod & p);
	int operator != (const GrapaPolyMod & p) const;
	GrapaPolyMod & operator += (const GrapaInt& i);
	GrapaPolyMod & operator += (u64 i);
	GrapaPolyMod & operator -= (const GrapaInt& i);
	GrapaPolyMod & operator -= (u64 i);
	GrapaPolyMod Pow(const GrapaInt& e);
private:
	void _polySquare();
	void _polyMul(const GrapaPolyMod & p);
	void _Mod();
	void _Norm();
};

#endif //_GrapaInt_

////////////////////////////////////////////////////////////////////////////////
