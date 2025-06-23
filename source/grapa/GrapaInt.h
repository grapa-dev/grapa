// GrapaInt.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaInt_
#define _GrapaInt_

#include "GrapaValue.h"
#include "GrapaThread.h"
#include <openssl/rand.h>

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
	void SetLength(u64 pLen = 0, bool pCopy = true);
	void SetSize(u64 pLen = 0, bool pCopy = true);
	void FROM(const GrapaArray32& pData);
	u64 GetCount() const;
	u64 GetByteCount() const;
	void SetCount(u64 pCount);
	void GrowCount(u64 pCount);
	void SetItem(u64 pItem, u32 pValue);
	u32 GetItem(u64 pItem) const;
	void RTrim();
	bool IsNull() const;
	bool IsNeg() const;
	bool IsSignNeg() const;
    bool IsZero() const;
	bool IsItem(u32 pItem) const;
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
	GrapaInt operator +(const GrapaInt& bi2) const;
	GrapaInt operator +(s64 bi) const;
	GrapaInt operator +=(const GrapaInt& bi1);
	GrapaInt& operator ++();
	GrapaInt operator ++(int);
	GrapaInt operator -(const GrapaInt& bi2) const;
	GrapaInt operator -(s64 bi) const;
	GrapaInt operator -=(const GrapaInt& bi1);
	GrapaInt& operator --();
	GrapaInt operator --(int);
	GrapaInt operator *(const GrapaInt& bi2) const;
	GrapaInt operator *(s64 bi) const;
	GrapaInt operator *(u64 bi) const;
	GrapaInt operator <<(u64 shiftVal);
	int shiftLeft(u32* inData, u32 inCount, u64 shiftVal);
	GrapaInt operator >>(u64 shiftVal);
	int shiftRight(u32* inData, u32 inCount, u64 shiftVal);
	GrapaInt operator ~() const;
	GrapaInt operator -() const;
	bool operator ==(const GrapaInt& bi2) const;
	bool operator !=(const GrapaInt& bi2) const;
	bool Equals(const GrapaInt& bi) const;
	bool operator >(const GrapaInt& bi2) const;
	bool operator >(u64 i1) const;
	bool operator <(const GrapaInt& bi2) const;
	bool operator <(s64 bi) const;
	bool operator >=(const GrapaInt& bi2) const;
	bool operator <=(const GrapaInt& bi2) const;
	GrapaInt Abs() const;
	void multiByteDivide(const GrapaInt& bi2, GrapaInt& outQuotient, GrapaInt& outRemainder);
	void singleByteDivide(const GrapaInt& bi2, GrapaInt& outQuotient, GrapaInt& outRemainder);
	GrapaInt Div(const GrapaInt& bi2, GrapaInt& remainder);
	GrapaInt operator /(const GrapaInt& bi2);
	GrapaInt operator /(u64 bi);
	GrapaInt operator %(const GrapaInt& bi2);
	GrapaInt operator %(u64 bi);
	GrapaInt operator %=(const GrapaInt& bi1);
	GrapaInt operator &(const GrapaInt& bi2);
	GrapaInt operator |(const GrapaInt& bi2) const;
	GrapaInt operator ^(const GrapaInt& bi2) const;
	//GrapaInt bxor(const GrapaInt& bi);
	GrapaInt bmax(const GrapaInt& bi) const;
	GrapaInt bmin(const GrapaInt& bi) const;
	GrapaInt babs() const;
	d64 blog();
	d64 blog2() const;
	d64 blog10();
	GrapaInt LargestPrimeFactor();
	static u64 LargestPrimeFactor64(u64 a);
	GrapaCHAR ToString() const;
	GrapaCHAR ToString(u64 radix) const;
	void FromString(const GrapaBYTE& result, u32 radix);
	void FromBytes(const GrapaBYTE& result, bool isUnsigned=false);
	//GrapaInt Factorial();
	GrapaInt Pow(const GrapaInt& exp) const;
	BIGNUM* getBytesOpenSSL() const;
	void setBytesOpenSSL(BIGNUM* b);
	GrapaInt modPow(const GrapaInt& exp, const GrapaInt& n) const;
	GrapaInt modFact(const GrapaInt& p) const;
	static GrapaInt BarrettReduction(const GrapaInt& x, const GrapaInt& n, const GrapaInt& constant);
	GrapaInt gcd(const GrapaInt& bi) const;
	GrapaInt gcd(s64 i1) const;
	void Random(u64 bits);
	u64 bitCount() const;
	u64 bitStart() const;
	s32 IntValue() const;
	s64 LongValue() const;
	GrapaInt modInverse(const GrapaInt& modulus) const;
	GrapaBYTE getBytes(bool pRaw=false) const;
	static GrapaCHAR Base(GrapaBYTE& pInput, u64 pFrom, u64 pTo);
	void setBit(u32 bitNum);
	void unsetBit(u32 bitNum);
	bool testBit(u64 bitNum) const;
	GrapaInt bsqrt() const;
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
