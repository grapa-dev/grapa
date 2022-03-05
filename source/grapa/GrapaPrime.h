// GrapaPrime.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaPrime_
#define _GrapaPrime_

#include "GrapaInt.h"

class GrapaPrime : public GrapaInt
{
public:
	u32 mBits;
	bool mFound;
	GrapaCritical* vCond;

public:
	GrapaPrime();
	GrapaPrime(s64 value);
	GrapaPrime(int value);
	GrapaPrime(u64 value);
	GrapaPrime(const GrapaInt& bi);
	GrapaPrime(const u8* inData, u32 inLen);
	virtual ~GrapaPrime();
	GrapaPrime& operator=(const GrapaPrime& that);
	GrapaPrime& operator=(const GrapaInt& that);
	void operator =(s64 bi);
public:
	void GenPrime(u32 bits, u32 safe, const GrapaInt& randP, const GrapaInt& eP);
	bool TestPrime(u32 safe, u32 confidence);
	bool AKS();
public:
	bool FermatLittleTest(int confidence);
	bool RabinMillerTest(int confidence);
	bool SolovayStrassenTest(int confidence);
	bool LucasStrongTest();
	bool LucasStrongTestHelper(const GrapaInt& thisVal);
	bool DivisorTest(u32 safe);
	bool ConfidenceTest(int confidence);
	bool StrongTest();
	bool isProbablePrime();
	static s32 Jacobi(const GrapaInt& a, const GrapaInt& b);
	static s32 Jacobi(s64 a, const GrapaInt& b);
	GrapaInt genCoPrime(u32 bits);
	void GenPrimes16();
	void GenPrimesDiv(u32 *primeList, u32 *numPrimes, u32 *maxPrime);
	bool isPrime32(u32 candidatePrime);
	u32 GenPrime16(u32 min, u32 max);
	static void LucasSequence(GrapaInt& P, GrapaInt& Q, GrapaInt& k, GrapaInt& n,
		GrapaInt& Uk, GrapaInt& Vk, GrapaInt& Qn);
	static void LucasSequenceHelper(GrapaInt& P, GrapaInt& Q, GrapaInt& k, GrapaInt& n,
		GrapaInt& Uk, GrapaInt& Vk, GrapaInt& Qn,
		GrapaInt& constant, s32 s);
	static void LucasSequenceHelper(s64 P, s64 Q, GrapaInt& k, GrapaInt& n,
		GrapaInt& Uk, GrapaInt& Vk, GrapaInt& Qn,
		GrapaInt& constant, s32 s);
	static void RSATest(u32 rounds);
	static void RSATest2(u32 rounds);
	static GrapaInt StaticPrime(int bits);

//private:
//	virtual void Running();
//	virtual void Stopping();
};


#endif //_GrapaPrime_

////////////////////////////////////////////////////////////////////////////////

