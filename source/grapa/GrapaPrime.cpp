// GrapaPrime.cpp
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

#include "GrapaPrime.h"
#include <math.h>
#include <limits.h>
#include <time.h>

#include <openssl/rand.h>

#include "GrapaSystem.h"
extern GrapaSystem* gSystem;

///////////////////////////////////////////////////////////////////////////////////////////////////

#define MAXU32  4294967295UL
#define NUMU32PRIMES 203280221UL
u32 primeCount = 6543;
u32 maxPrime = 65537UL;
u32 primeList16[6545];
bool primeList16Initialized = false;

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaPrime::GrapaPrime()
{
	GenPrimes16();
	mBits = 48 * 8;
	mFound = false;
	vCond = NULL;
}

GrapaPrime::GrapaPrime(s64 value)
{
	GrapaInt that(value);
	*this = that;
	mBits = 48 * 8;
	mFound = false;
	vCond = NULL;
}

GrapaPrime::GrapaPrime(int value)
{
	GrapaInt that(value);
	*this = that;
	mBits = 48 * 8;
	mFound = false;
	vCond = NULL;
}

GrapaPrime::GrapaPrime(u64 value)
{
	GrapaInt that(value);
	*this = that;
	mBits = 48 * 8;
	mFound = false;
	vCond = NULL;
}

GrapaPrime::GrapaPrime(const GrapaInt& bi)
{
	FROM(bi);
	mBits = 48 * 8;
	mFound = false;
	vCond = NULL;
}

GrapaPrime::GrapaPrime(const u8* inData, u32 inLen)
{
	GrapaInt that(inData, inLen);
	*this = that;
	mBits = 48 * 8;
	mFound = false;
	vCond = NULL;
}

GrapaPrime::~GrapaPrime()
{
}

GrapaPrime& GrapaPrime::operator =(const GrapaPrime& that)
{
	if (this != &that)
	{
		dataSigned = that.dataSigned;
		FROM(that);
	}
	return *this;
}

GrapaPrime& GrapaPrime::operator =(const GrapaInt& that)
{
	if (this != &that)
	{
		dataSigned = that.dataSigned;
		FROM(that);
	}
	return *this;
}

void GrapaPrime::operator =(s64 bi)
{
	GrapaInt bi2(bi);
	*this = bi2;
}

//***********************************************************************
// Probabilistic prime test based on Fermat's little theorem
//
// for any a < p (p does not divide a) if
//      a^(p-1) mod p != 1 then p is not prime.
//
// Otherwise, p is probably prime (pseudoprime to the chosen base).
//
// Returns
// -------
// True if "this" is a pseudoprime to randomly chosen
// bases.  The number of chosen bases is given by the "confidence"
// parameter.
//
// False if "this" is definitely NOT prime.
//
// Note - this method is fast but fails for Carmichael numbers except
// when the randomly chosen base is a factor of the number.
//***********************************************************************

bool GrapaPrime::FermatLittleTest(int confidence) const
{
	GrapaInt thisVal;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;

	if (thisVal.GetCount() <= 1)
	{
		// test small numbers
		if (thisVal.GetItem(0) == 0 || thisVal.GetItem(0) == 1)
			return false;
		else if (thisVal.GetItem(0) == 2 || thisVal.GetItem(0) == 3)
			return true;
	}

	if ((thisVal.GetItem(0) & 0x1) == 0)     // even numbers
		return false;

	u64 bits = thisVal.bitCount();
	GrapaInt a;// = new BigInteger();
	GrapaInt p_sub1 = thisVal - 1;// thisVal - (new BigInteger(1));

	for (int round = 0; round < confidence; round++)
	{
		bool done = false;

		while (!done)		// generate a < n
		{
			u32 testBits = 0;

			// make sure "a" has at least 2 bits
			while (testBits < 2)
				testBits = gSystem->Random32() % bits;

			a.Random(testBits);

			u64 byteLen = a.GetCount();

			// make sure "a" is not 0
			if (byteLen > 1 || (byteLen == 1 && a.GetItem(0) != 1))
				done = true;
		}

		// check whether a factor exists (fix for version 1.03)
		GrapaInt gcdTest = a.gcd(thisVal);
		if (gcdTest.GetCount() == 1 && gcdTest.GetItem(0) != 1)
			return false;

		// calculate a^(p-1) mod p
		GrapaInt expResult = a.modPow(p_sub1, thisVal);

		u64 resultLen = expResult.GetCount();

		// is NOT prime is a^(p-1) mod p != 1

		if (resultLen > 1 || (resultLen == 1 && expResult.GetItem(0) != 1))
		{
			//Console.WriteLine("a = " + a.ToString());
			return false;
		}

	}

	return true;
}


//***********************************************************************
// Probabilistic prime test based on Rabin-Miller's
//
// for any p > 0 with p - 1 = 2^s * t
//
// p is probably prime (strong pseudoprime) if for any a < p,
// 1) a^t mod p = 1 or
// 2) a^((2^j)*t) mod p = p-1 for some 0 <= j <= s-1
//
// Otherwise, p is composite.
//
// Returns
// -------
// True if "this" is a strong pseudoprime to randomly chosen
// bases.  The number of chosen bases is given by the "confidence"
// parameter.
//
// False if "this" is definitely NOT prime.
//
//***********************************************************************

bool GrapaPrime::RabinMillerTest(int confidence) const
{
	GrapaInt thisVal;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;

	if (thisVal.GetCount() <= 1)
	{
		// test small numbers
		if (thisVal.GetItem(0) == 0 || thisVal.GetItem(0) == 1)
			return false;
		else if (thisVal.GetItem(0) == 2 || thisVal.GetItem(0) == 3)
			return true;
	}

	if ((thisVal.GetItem(0) & 0x1) == 0)     // even numbers
		return false;


	// calculate values of s and t
	GrapaInt p_sub1 = thisVal - 1;// (new BigInteger(1));
	int s = 0;

	for (u64 index = 0; index < p_sub1.GetCount(); index++)
	{
		u32 mask = 0x01;

		for (u64 i = 0; i < 32; i++)
		{
			if ((p_sub1.GetItem(index) & mask) != 0)
			{
				index = p_sub1.GetCount();      // to break the outer loop
				break;
			}
			mask <<= 1;
			s++;
		}
	}

	GrapaInt t = p_sub1 >> s;

	u64 bits = thisVal.bitCount();
	GrapaInt a;// = new BigInteger();

	for (int round = 0; round < confidence; round++)
	{
		bool done = false;

		while (!done)		// generate a < n
		{
			u32 testBits = 0;

			// make sure "a" has at least 2 bits
			while (testBits < 2)
				testBits = (u32)(gSystem->Random32() % bits);

			a.Random(testBits);

			u64 byteLen = a.GetCount();

			// make sure "a" is not 0
			if (byteLen > 1 || (byteLen == 1 && a.GetItem(0) != 1))
				done = true;
		}

		// check whether a factor exists (fix for version 1.03)
		GrapaInt gcdTest = a.gcd(thisVal);
		if (gcdTest.GetCount() == 1 && gcdTest.GetItem(0) != 1)
			return false;

		GrapaInt b = a.modPow(t, thisVal);

		bool result = false;

		if (b.IsItem(1))         // a^t mod p = 1
			result = true;

		for (int j = 0; result == false && j < s; j++)
		{
			if (b == p_sub1)         // a^((2^j)*t) mod p = p-1 for some 0 <= j <= s-1
			{
				result = true;
				break;
			}

			b = (b * b) % thisVal;
		}

		if (result == false)
			return false;
	}
	return true;
}

//***********************************************************************
// Probabilistic prime test based on Solovay-Strassen (Euler Criterion)
//
// p is probably prime if for any a < p (a is not multiple of p),
// a^((p-1)/2) mod p = J(a, p)
//
// where J is the Jacobi symbol.
//
// Otherwise, p is composite.
//
// Returns
// -------
// True if "this" is a Euler pseudoprime to randomly chosen
// bases.  The number of chosen bases is given by the "confidence"
// parameter.
//
// False if "this" is definitely NOT prime.
//
//***********************************************************************

bool GrapaPrime::SolovayStrassenTest(int confidence) const
{
	GrapaInt thisVal;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;

	if (thisVal.GetCount() <= 1)
	{
		// test small numbers
		if (thisVal.GetItem(0) == 0 || thisVal.GetItem(0) == 1)
			return false;
		else if (thisVal.GetItem(0) == 2 || thisVal.GetItem(0) == 3)
			return true;
	}

	if ((thisVal.GetItem(0) & 0x1) == 0)     // even numbers
		return false;


	u64 bits = thisVal.bitCount();
	GrapaInt a;// = new BigInteger();
	GrapaInt p_sub1 = thisVal - 1;
	GrapaInt p_sub1_shift = p_sub1 >> 1;

	for (int round = 0; round < confidence; round++)
	{
		bool done = false;

		while (!done)		// generate a < n
		{
			u32 testBits = 0;

			// make sure "a" has at least 2 bits
			while (testBits < 2)
				testBits = gSystem->Random32() % bits;

			a.Random(testBits);

			u64 byteLen = a.GetCount();

			// make sure "a" is not 0
			if (byteLen > 1 || (byteLen == 1 && a.GetItem(0) != 1))
				done = true;
		}

		// check whether a factor exists (fix for version 1.03)
		GrapaInt gcdTest = a.gcd(thisVal);
		if (gcdTest.GetCount() == 1 && gcdTest.GetItem(0) != 1)
			return false;

		// calculate a^((p-1)/2) mod p

		GrapaInt expResult = a.modPow(p_sub1_shift, thisVal);
		if (expResult == p_sub1)
			expResult = (s64)-1;

		// calculate Jacobi symbol
		GrapaInt jacob = (s64)Jacobi(a, thisVal);

		// if they are different then it is not prime
		if (expResult != jacob)
			return false;
	}

	return true;
}

//***********************************************************************
// Implementation of the Lucas Strong Pseudo Prime test.
//
// Let n be an odd number with gcd(n,D) = 1, and n - J(D, n) = 2^s * d
// with d odd and s >= 0.
//
// If Ud mod n = 0 or V2^r*d mod n = 0 for some 0 <= r < s, then n
// is a strong Lucas pseudoprime with parameters (P, Q).  We select
// P and Q based on Selfridge.
//
// Returns True if number is a strong Lucus pseudo prime.
// Otherwise, returns False indicating that number is composite.
//***********************************************************************

bool GrapaPrime::LucasStrongTest() const
{
	GrapaInt thisVal;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;

	if (thisVal.GetCount() <= 1)
	{
		// test small numbers
		if (thisVal.GetItem(0) == 0 || thisVal.GetItem(0) == 1)
			return false;
		else if (thisVal.GetItem(0) == 2 || thisVal.GetItem(0) == 3)
			return true;
	}

	if ((thisVal.GetItem(0) & 0x1) == 0)     // even numbers
		return false;

	return LucasStrongTestHelper(thisVal);
}

bool GrapaPrime::LucasStrongTestHelper(const GrapaInt& pthisVal) const
{
	// Do the test (selects D based on Selfridge)
	// Let D be the first element of the sequence
	// 5, -7, 9, -11, 13, ... for which J(D,n) = -1
	// Let P = 1, Q = (1-D) / 4

	GrapaInt thisVal(pthisVal);
	s32 D = 5, sign = -1, dCount = 0;
	bool done = false;

	while (!done)
	{
		s32 Jresult = Jacobi(D, thisVal);

		if (Jresult == -1)
			done = true;    // J(D, this) = 1
		else
		{
			if (Jresult == 0 && thisVal > (u64)abs(D))       // divisor found
				return false;

			if (dCount == 20)
			{
				// check for square
				GrapaInt root = thisVal.bsqrt();
				if (root * root == thisVal)
					return false;
			}

			//Console.WriteLine(D);
			D = (abs(D) + 2) * sign;
			sign = -sign;
		}
		dCount++;
	}

	s64 Q = (1 - D) >> 2;

	GrapaInt p_add1 = thisVal + 1;
	s32 s = 0;

	for (u64 index = 0; index < p_add1.GetCount(); index++)
	{
		u32 mask = 0x01;

		for (u64 i = 0; i < 32; i++)
		{
			if ((p_add1.GetItem(index) & mask) != 0)
			{
				index = p_add1.GetCount();      // to break the outer loop
				break;
			}
			mask <<= 1;
			s++;
		}
	}

	GrapaInt t = p_add1 >> s;

	// calculate constant = b^(2k) / m
	// for Barrett Reduction
	GrapaInt constant;// = new BigInteger();

	u64 nLen = thisVal.GetCount() << 1;
	constant.SetItem(nLen, 0x00000001);

	constant = constant / thisVal;

	GrapaInt Uk, Vk, Qn;
	LucasSequenceHelper(1, Q, t, thisVal, Uk, Vk, Qn, constant, 0);
	bool isPrime = false;

	if (Uk.IsZero() || Vk.IsZero())
	{
		// u(t) = 0 or V(t) = 0
		isPrime = true;
	}

	for (s32 i = 1; i < s; i++)
	{
		if (!isPrime)
		{
			// doubling of index
			Vk = thisVal.BarrettReduction(Vk * Vk, thisVal, constant);
			Vk = (Vk - (Qn << 1)) % thisVal;

			//lucas[1] = ((lucas[1] * lucas[1]) - (lucas[2] << 1)) % thisVal;

			if (Vk.IsZero())
				isPrime = true;
		}

		Qn = thisVal.BarrettReduction(Qn * Qn, thisVal, constant);     //Q^k
	}


	if (isPrime)     // additional checks for composite numbers
	{
		// If n is prime and gcd(n, Q) == 1, then
		// Q^((n+1)/2) = Q * Q^((n-1)/2) is congruent to (Q * J(Q, n)) mod n

		GrapaInt g = thisVal.gcd(Q);
		if (g.IsItem(1))         // gcd(this, Q) == 1
		{
			//Qn = Q; // not sure if this is right???????????????????????????????????????????????????
			if (Qn.IsSignNeg())
				Qn += thisVal;

			GrapaInt temp = GrapaInt(Q * Jacobi(Q, thisVal)) % thisVal;
			GrapaCHAR s = temp.ToString(10);
			if (temp.IsSignNeg())
				temp += thisVal;

			if (Qn != temp)
				isPrime = false;
		}
	}

	return isPrime;
}

//http://www.cplusplus.com/forum/general/72028/
//http://www.sanfoundry.com/cpp-program-implement-solovay-strassen-primality-test/

//***********************************************************************
// Determines whether a number is probably prime, using the Rabin-Miller's
// test.  Before applying the test, the number is tested for divisibility
// by primes < 2000
//
// Returns true if number is probably prime.
//***********************************************************************

bool GrapaPrime::DivisorTest(u32 safe) const
{
	GrapaPrime thisVal;
	GrapaPrime m1d2;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;
	if (safe)
		m1d2 = (thisVal - 1) >> 1;
	for (u32 p = 1; p <= primeCount; p++)
	{
		GrapaInt divisor((u64)primeList16[p]);
		if (divisor >= thisVal)
			return true;
		GrapaInt resultNum = thisVal % divisor;
		if (resultNum.IntValue() == 0)
			return false;
		if (safe)
		{
			resultNum = m1d2 % divisor;
			if (resultNum.IntValue() == 0)
				return false;
		}
	}
	return true;
}

bool GrapaPrime::ConfidenceTest(int confidence) const
{
	GrapaPrime thisVal;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;
	if (thisVal.RabinMillerTest(confidence))
		if (thisVal.FermatLittleTest(confidence))
			return true;
	return false;
}

bool GrapaPrime::StrongTest() const
{
	GrapaPrime thisVal;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;
	if (thisVal.LucasStrongTest()) 
		return true;
	return false;
}

//***********************************************************************
// Determines whether this BigInteger is probably prime using a
// combination of base 2 strong pseudoprime test and Lucas strong
// pseudoprime test.
//
// The sequence of the primality test is as follows,
//
// 1) Trial divisions are carried out using prime numbers below 2000.
//    if any of the primes divides this BigInteger, then it is not prime.
//
// 2) Perform base 2 strong pseudoprime test.  If this BigInteger is a
//    base 2 strong pseudoprime, proceed on to the next step.
//
// 3) Perform strong Lucas pseudoprime test.
//
// Returns True if this BigInteger is both a base 2 strong pseudoprime
// and a strong Lucas pseudoprimeisProbablePrime
//
// For a detailed discussion of this primality test, see [6].
//
//***********************************************************************

bool GrapaPrime::isProbablePrime() const
{
	GrapaInt thisVal;
	if (IsSignNeg())        // negative
		thisVal = -*this;
	else
		thisVal = *this;

	if (thisVal.GetCount() <= 1)
	{
		// test small numbers
		if (thisVal.GetItem(0) == 0 || thisVal.GetItem(0) == 1)
			return false;
		else if (thisVal.GetItem(0) == 2 || thisVal.GetItem(0) == 3)
			return true;
	}

	if ((thisVal.GetItem(0) & 0x1) == 0)     // even numbers
		return false;


	// test for divisibility by primes < 2000
	//for (int p = 0; p < primesBelow2000len; p++)
	for (u32 p = 1; p <= primeCount; p++)
	{
		//GrapaInt divisor((u64)primesBelow2000[p]);
		GrapaInt divisor((u64)primeList16[p]);

		if (divisor >= thisVal)
			break;

		GrapaInt resultNum = thisVal % divisor;
		if (resultNum.IntValue() == 0)
		{
			//Console.WriteLine("Not prime!  Divisible by {0}\n",
			//                  primesBelow2000[p]);

			return false;
		}
	}

	// Perform BASE 2 Rabin-Miller Test

	// calculate values of s and t
	GrapaInt p_sub1 = thisVal - 1;// (new BigInteger(1));
	int s = 0;

	for (u64 index = 0; index < p_sub1.GetCount(); index++)
	{
		u32 mask = 0x01;

		for (u64 i = 0; i < 32; i++)
		{
			if ((p_sub1.GetItem(index) & mask) != 0)
			{
				index = p_sub1.GetCount();      // to break the outer loop
				break;
			}
			mask <<= 1;
			s++;
		}
	}

	GrapaInt t = p_sub1 >> s;

	GrapaInt a((u64)2);

	// b = a^t mod p
	GrapaInt b = a.modPow(t, thisVal);
	bool result = false;

	if (b.IsItem(1))         // a^t mod p = 1
		result = true;

	for (int j = 0; result == false && j < s; j++)
	{
		if (b == p_sub1)         // a^((2^j)*t) mod p = p-1 for some 0 <= j <= s-1
		{
			result = true;
			break;
		}

		b = (b * b) % thisVal;
	}

	// if number is strong pseudoprime to base 2, then do a strong lucas test
	if (result)
		result = LucasStrongTestHelper(thisVal);

	return result;
}

s32 GrapaPrime::Jacobi(const GrapaInt& pa, const GrapaInt& pb)
{
	GrapaInt a(pa);
	GrapaInt b(pb);

	// Jacobi defined only for odd integers
	if ((b.GetItem(0) & 0x1) == 0)
		return 0;
	//throw;// (new ArgumentException("Jacobi defined only for odd integers."));

	bool retNeg = false;
	s32 retMul = 1;

	while (true)
	{
		if (a >= b) a %= b;
		if (a.IsZero())      return 0;  // a == 0
		if (a.IsItem(1))      return 1;  // a == 1

		if (a < 0)
		{
			if ((((b - 1).GetItem(0)) & 0x2) != 0)       //if( (((b-1) >> 1).data[0] & 0x1) == 0)
				retNeg = !retNeg;
			a = -a;
			continue;
		}

		s32 e = 0;
		for (u32 index = 0; index < a.GetCount(); index++)
		{
			u32 mask = 0x01;

			for (u32 i = 0; i < 32; i++)
			{
				if ((a.GetItem(index) & mask) != 0)
				{
					index = (u32)a.GetCount();      // to break the outer loop
					break;
				}
				mask <<= 1;
				e++;
			}
		}

		GrapaInt a1 = a >> e;

		s32 s = 1;
		if ((e & 0x1) != 0 && ((b.GetItem(0) & 0x7) == 3 || (b.GetItem(0) & 0x7) == 5))
			s = -1;

		if ((b.GetItem(0) & 0x3) == 3 && (a1.GetItem(0) & 0x3) == 3)
			s = -s;

		retMul *= s;

		if (a1.IsItem(1))
			break;

		a = b % a1;
		b = a1;
	}

	if (retNeg) retMul = -retMul;
	return retMul;
}

s32 GrapaPrime::Jacobi(s64 a, const GrapaInt& b)
{
	GrapaInt bi1(a);
	return Jacobi(bi1, b);
}

// a, p = prime
// AKS test (x-a)^p = (x^p-a) (mod p) 
// x^n = (x - a)^n + a

#include <future>
#include <atomic>
#include <vector>

bool GrapaPrime::AKS() const
{
	GrapaInt n = *this;
	if (n < 2)
		return false;

	// Step 1: Check if n is a perfect power
	for (u64 b = 2; b <= n.bitCount(); ++b)
	{
		GrapaInt low(2), high(n), mid, pow;
		while (low <= high)
		{
			mid = (low + high) / 2;
			pow = mid.Pow(GrapaInt((s64)b));
			if (pow == n)
				return false; // n is a perfect power
			if (pow < n)
				low = mid + 1;
			else
				high = mid - 1;
		}
	}

	// Step 2: Find the smallest r such that ord_r(n) > log^2 n
	u64 logn = (u64)ceil(n.blog());
	u64 maxk = logn * logn;
	u64 r = 2;
	bool found_r = false;
	while (!found_r)
	{
		found_r = true;
		for (u64 k = 1; k <= maxk; ++k)
		{
			if (n.modPow(k, r) == 1 || n.gcd(r) != 1)
			{
				found_r = false;
				break;
			}
		}
		if (!found_r)
			++r;
	}

	// Step 3: Check for gcd(a, n) > 1 for 2 <= a <= r
	for (u64 a = 2; a <= r; ++a)
	{
		GrapaInt g = n.gcd(GrapaInt((s64)a));
		if (g > 1 && g < n)
			return false;
	}

	// Step 4: If n <= r, n is prime
	if (n.LongValue() <= (s64)r)
		return true;

	// Step 5: Polynomial congruence check
	u64 phi_r = r - 1; // For prime r, phi(r) = r-1; for composite, this is an overestimate
	u64 limit = (u64)(sqrt(phi_r) * logn);

	/*
	for (u64 a = 1; a <= limit; ++a)
	{
		GrapaPolyMod left(r, n);  // x
		left -= a;                // x - a
		left = left.Pow(n);       // (x - a)^n

		GrapaPolyMod right(r, n); // x
		right = right.Pow(n);     // x^n
		right -= a;               // x^n - a

		if (left != right)
			return false;
	}
	*/

	std::atomic<bool> found_composite(false);
	std::vector<std::future<void>> futures;
	for (u64 a = 1; a <= limit; ++a) {
		futures.push_back(std::async(std::launch::async, [&, a]() {
			if (found_composite.load()) return;
			GrapaPolyMod left(r, n);  left -= a;  left = left.Pow(n);
			GrapaPolyMod right(r, n); right = right.Pow(n); right -= a;
			if (left != right) found_composite.store(true);
			}));
	}
	// Wait for all threads to finish
	for (auto& f : futures) f.get();
	if (found_composite) return false;

	return true;
}

//***********************************************************************
// Generates a positive BigInteger that is probably prime.
//***********************************************************************

bool GrapaPrime::TestPrime(u32 safe, u32 confidence) const
{
	if ((GetItem(0) & 0x01) == 0) 
		return(false);
	GrapaInt four(4);
	GrapaInt one(1);
	GrapaPrime m1(*this - 1);
	GrapaInt max = (((u64)m1.bitCount() * 8)) / 4;
	u64 maxMod = (max.bitCount() / 2) + 1;
	for (u32 modCount = 1; modCount <= maxMod; modCount++)
	{
		GrapaInt exp((u64)2 << (u64)modCount);
		GrapaInt isZero((four.Pow(exp)) + one);  // 1 = 257, 2 = 65537
		isZero = m1 % isZero;
		if (isZero.IsZero())
			return(false);
	}
	if (!DivisorTest(safe))
		return(false);
	GrapaBYTE gb = getBytes();
	BIGNUM* r = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
	int err = BN_is_prime_ex(r, confidence, NULL, NULL);
	BN_free(r);
	if (err == 0) return false;
	//if (!ConfidenceTest(confidence))
	//	return(false);
	if (safe)
	{
		m1 = m1 >> 1;
		return m1.TestPrime(0, confidence);
	}
	return(true);
}

void GrapaPrime::GenPrime(u32 bits, u32 safe, const GrapaInt& randP, const GrapaInt& eP)
{
	GrapaInt one(1);
	GrapaInt pm1(GrapaInt(randP) - 1);
	GrapaInt pe(eP);
	int err = 0;
	BIGNUM* r = BN_new();
	err = BN_generate_prime_ex(r, bits, safe, NULL, NULL, NULL);
	if (err == 1)
	{
		GrapaBYTE gb;
		gb.SetSize(BN_num_bytes(r) + 1);
		u32 sz = BN_bn2bin(r, (u8*)gb.mBytes);
		gb.SetLength(sz);
		FromBytes(gb, true);
	}
	BN_free(r);
	if (err == 0)
	{
		bool isSecond = true;
		if (((GrapaInt*)&randP)->IsZero())
			isSecond = false;
		while (true)
		{
			Random(bits);
			GrapaInt thm1(*this - 1);
			//if (thm1.modFact(*this) != thm1)
			//	continue;
			SetItem(0, GetItem(0) | 0x01);
			if (isSecond && *this == randP)
				continue;
			if (isSecond && pe.gcd(GrapaInt((*this - one) * pm1)) != one)
				continue;
			if (TestPrime(safe,64))
				break;
		}
	}
}

//***********************************************************************
// Generates a random number with the specified number of bits such
// that gcd(number, this) = 1
//***********************************************************************

GrapaInt GrapaPrime::genCoPrime(u32 bits) const
{
	bool done = false;
	GrapaInt result;

	while (!done)
	{
		result.Random(bits);
		GrapaInt g = result.gcd(*this);
		if (g.IsItem(1))
			done = true;
	}

	return result;
}

void GrapaPrime::GenPrimes16()
{
	WaitCritical();
	if (!primeList16Initialized)
	{
		primeCount = 6543;
		maxPrime = 65537UL;
		GenPrimesDiv(primeList16, &primeCount, &maxPrime);
		if (primeList16[6543] == maxPrime)
			primeList16Initialized = 1;
	}
	LeaveCritical();
}

void GrapaPrime::GenPrimesDiv(u32 *ulPrime, u32 *nPrimes, u32 *ulUB)
{
	unsigned long ul2, ulSqrt, ulDivisor;
    u32 ulN,ul;

	ulPrime[0] = 0;  /* 0th prime---just a marker */
	ulPrime[1] = 2;  /* 1st prime */
	ulPrime[2] = 3;  /* 2nd prime */
	ulPrime[3] = 5;  /* 3rd prime */
	if (*ulUB < 7)*ulUB = 5;
	if (*nPrimes < 3)*nPrimes = 3;
	if (*ulUB > MAXU32 - 4)*ulUB = MAXU32 - 4;  /* Largest 32-bit prime */
	if (*nPrimes > NUMU32PRIMES)*nPrimes = NUMU32PRIMES;
	ul = 4;
	ulN = 7;
	while (1)
	{
		ul2 = 2;
		ulSqrt = (unsigned long) ceil(sqrt(ulN + 0.5));  /* faster than integer sqrt */
		while (1)
		{
			ulDivisor = ulPrime[ul2++];
			if (ulDivisor >= ulSqrt)
			{
				ulPrime[ul++] = ulN;
				break;
			}
			if (ulN % ulDivisor == 0)break;
		}
		ulN += 2;
		if ((ulN > *ulUB) && (ul > *nPrimes))break;  /* post-increments */
	}
	ulPrime[ul] = 0;  /* end marker */
	*nPrimes = ul - 1;
	*ulUB = ulPrime[ul - 1];

	return;
}

bool GrapaPrime::isPrime32(u32 ulN)
{
	u64 ulSqrtN, ul = 2, ulDiv;
	if ((ulN < 3) || ((ulN & 1) == 0))return(ulN == 2 ? true : false);
	GenPrimes16();
	ulSqrtN = sqrt64(ulN);
	while (1)
	{
		ulDiv = primeList16[ul++];
		if (ulDiv > ulSqrtN)return(true);
		if (ulN%ulDiv == 0)return(false);
	}
}

u32 GrapaPrime::GenPrime16(u32 min, u32 max)
{
	GenPrimes16();
	return primeList16[min + (gSystem->Random32() % (max - min))-1];
}

//***********************************************************************
// Returns the k_th number in the Lucas Sequence reduced modulo n.
//
// Uses index doubling to speed up the process.  For example, to calculate V(k),
// we maintain two numbers in the sequence V(n) and V(n+1).
//
// To obtain V(2n), we use the identity
//      V(2n) = (V(n) * V(n)) - (2 * Q^n)
// To obtain V(2n+1), we first write it as
//      V(2n+1) = V((n+1) + n)
// and use the identity
//      V(m+n) = V(m) * V(n) - Q * V(m-n)
// Hence,
//      V((n+1) + n) = V(n+1) * V(n) - Q^n * V((n+1) - n)
//                   = V(n+1) * V(n) - Q^n * V(1)
//                   = V(n+1) * V(n) - Q^n * P
//
// We use k in its binary expansion and perform index doubling for each
// bit position.  For each bit position that is set, we perform an
// index doubling followed by an index addition.  This means that for V(n),
// we need to update it to V(2n+1).  For V(n+1), we need to update it to
// V((2n+1)+1) = V(2*(n+1))
//
// This function returns
// [0] = U(k)
// [1] = V(k)
// [2] = Q^n
//
// Where U(0) = 0 % n, U(1) = 1 % n
//       V(0) = 2 % n, V(1) = P % n
//***********************************************************************

// replacing Barett's reduction methode with Montgomery's approach could 
// speed up execution for integers larger than 1024 bits.

void GrapaPrime::LucasSequence(GrapaInt& P, GrapaInt& Q,
	GrapaInt& k, GrapaInt& n,
	GrapaInt& Uk, GrapaInt& Vk, GrapaInt& Qn)
{
	if (k.IsZero())
	{
		Uk = 0; Vk = GrapaInt((s64)2) % n; Qn = GrapaInt((s64)1) % n;
		return;
	}

	// calculate constant = b^(2k) / m
	// for Barrett Reduction
	GrapaInt constant;// = new BigInteger();

	u32 nLen = (u32)n.GetCount() << 1;
	constant.SetItem(nLen, 0x00000001);

	constant = constant / n;

	// calculate values of s and t
	int s = 0;

	for (u32 index = 0; index < k.GetCount(); index++)
	{
		u32 mask = 0x01;

		for (u32 i = 0; i < 32; i++)
		{
			if ((k.GetItem(index) & mask) != 0)
			{
				index = (u32)k.GetCount();      // to break the outer loop
				break;
			}
			mask <<= 1;
			s++;
		}
	}

	GrapaInt t = k >> s;

	LucasSequenceHelper(P, Q, t, n, Uk, Vk, Qn, constant, s);
}


//***********************************************************************
// Performs the calculation of the kth term in the Lucas Sequence.
// For details of the algorithm, see reference [9].
//
// k must be odd.  i.e LSB == 1
//***********************************************************************

void GrapaPrime::LucasSequenceHelper(GrapaInt& P, GrapaInt& Q,
	GrapaInt& k, GrapaInt& n,
	GrapaInt& Uk, GrapaInt& Vk, GrapaInt& Qn,
	GrapaInt& constant, s32 s)
{
	if ((k.GetItem(0) & 0x00000001) == 0)
		return;
		//throw;// (new ArgumentException("Argument k must be odd."));

	u64 numbits = k.bitCount();
	u32 mask = (u32)0x1 << ((numbits & 0x1F) - 1);

	// v = v0, v1 = v1, u1 = u1, Q_k = Q^0

	GrapaInt v = GrapaInt((s64)2) % n, Q_k = GrapaInt((s64)1) % n,
		v1 = P % n, u1 = Q_k;
	bool flag = true;

	if (k.GetCount())
	{
		for (s32 i = (s32)k.GetCount() - 1; i >= 0; i--)     // iterate on the binary expansion of k
		{
			//Console.WriteLine("round");
			while (mask != 0)
			{
				if (i == 0 && mask == 0x00000001)        // last bit
					break;

				if ((k.GetItem(i) & mask) != 0)             // bit is set
				{
					// index doubling with addition

					u1 = (u1 * v1) % n;

					v = ((v * v1) - (P * Q_k)) % n;
					v1 = n.BarrettReduction(v1 * v1, n, constant);
					v1 = (v1 - ((Q_k * Q) << 1)) % n;

					if (flag)
						flag = false;
					else
						Q_k = n.BarrettReduction(Q_k * Q_k, n, constant);

					Q_k = (Q_k * Q) % n;
				}
				else
				{
					// index doubling
					u1 = ((u1 * v) - Q_k) % n;

					v1 = ((v * v1) - (P * Q_k)) % n;
					v = n.BarrettReduction(v * v, n, constant);
					v = (v - (Q_k << 1)) % n;

					if (flag)
					{
						Q_k = Q % n;
						flag = false;
					}
					else
						Q_k = n.BarrettReduction(Q_k * Q_k, n, constant);
				}

				mask >>= 1;
			}
			mask = 0x80000000;
		}
	}

	// at this point u1 = u(n+1) and v = v(n)
	// since the last bit always 1, we need to transform u1 to u(2n+1) and v to v(2n+1)

	u1 = ((u1 * v) - Q_k) % n;
	v = ((v * v1) - (P * Q_k)) % n;
	if (flag)
		flag = false;
	else
		Q_k = n.BarrettReduction(Q_k * Q_k, n, constant);

	Q_k = (Q_k * Q) % n;


	for (s32 i = 0; i < s; i++)
	{
		// index doubling
		u1 = (u1 * v) % n;
		v = ((v * v) - (Q_k << 1)) % n;

		if (flag)
		{
			Q_k = Q % n;
			flag = false;
		}
		else
			Q_k = n.BarrettReduction(Q_k * Q_k, n, constant);
	}

	Uk = u1; Vk = v; Qn = Q_k;
}

void GrapaPrime::LucasSequenceHelper(s64 P, s64 Q, GrapaInt& k, GrapaInt& n,
	GrapaInt& Uk, GrapaInt& Vk, GrapaInt& Qn,
	GrapaInt& constant, s32 s)
{
	GrapaInt BP(P), BQ(Q);
	LucasSequenceHelper(BP, BQ, k, n, Uk, Vk, Qn, constant, s);
}

//***********************************************************************
// Tests the correct implementation of the modulo exponential function
// using RSA encryption and decryption (using pre-computed encryption and
// decryption keys).
//***********************************************************************

void GrapaPrime::RSATest(u32 rounds)
{
	u8 val[64];// = new byte[64];

	// private and public key
	GrapaInt bi_e("a932b948feed4fb2b692609bd22164fc9edb59fae7880cc1eaff7b3c9626b7e5b241c27a974833b2622ebe09beb451917663d47232488f23a117fc97720f1e7", 16);
	GrapaInt bi_d("4adf2f7a89da93248509347d2ae506d683dd3a16357e859a980c4f77a4e2f7a01fae289f13a851df6e9db5adaa60bfd2b162bbbe31f7c8f828261a6839311929d2cef4f864dde65e556ce43c89bbbf9f1ac5511315847ce9cc8dc92470a747b8792d6a83b0092d2e5ebaf852c85cacf34278efa99160f2f8aa7ee7214de07b7", 16);
	GrapaInt bi_n("e8e77781f36a7b3188d711c2190b560f205a52391b3479cdb99fa010745cbeba5f2adc08e1de6bf38398a0487c4a73610d94ec36f17f3f46ad75e17bc1adfec99839589f45f95ccc94cb2a5c500b477eb3323d8cfab0c8458c96f0147a45d27e45a4d11d54d77684f65d48f15fafcc1ba208e71e921b9bd9017c16a5231af7f", 16);

	for (u32 count = 0; count < rounds; count++)
	{
		// generate data of random length
		int t1 = 0;
		while (t1 == 0)
			t1 = (int)(gSystem->Random32() % 65);

		bool done = false;
		while (!done)
		{
			for (int i = 0; i < 64; i++)
			{
				if (i < t1)
					val[i] = (u8)(gSystem->Random32() % 256);
				else
					val[i] = 0;

				if (val[i] != 0)
					done = true;
			}
		}

		while (val[0] == 0)
			val[0] = (u8)(gSystem->Random32() % 256);

		// encrypt and decrypt data
		GrapaInt bi_data(val, t1);
		GrapaInt bi_encrypted = bi_data.modPow(bi_e, bi_n);
		GrapaInt bi_decrypted = bi_encrypted.modPow(bi_d, bi_n);

		// compare
		if (bi_decrypted != bi_data)
		{
			return;
		}
	}

}

//***********************************************************************
// Tests the correct implementation of the modulo exponential and
// inverse modulo functions using RSA encryption and decryption.  The two
// pseudoprimes p and q are fixed, but the two RSA keys are generated
// for each round of testing.
//***********************************************************************

void GrapaPrime::RSATest2(u32 rounds)
{
	u8 val[64];// = new byte[64];

	u8 pseudoPrime1[64] = {
		0x85, 0x84, 0x64, 0xFD, 0x70, 0x6A,
		0x9F, 0xF0, 0x94, 0x0C, 0x3E, 0x2C,
		0x74, 0x34, 0x05, 0xC9, 0x55, 0xB3,
		0x85, 0x32, 0x98, 0x71, 0xF9, 0x41,
		0x21, 0x5F, 0x02, 0x9E, 0xEA, 0x56,
		0x8D, 0x8C, 0x44, 0xCC, 0xEE, 0xEE,
		0x3D, 0x2C, 0x9D, 0x2C, 0x12, 0x41,
		0x1E, 0xF1, 0xC5, 0x32, 0xC3, 0xAA,
		0x31, 0x4A, 0x52, 0xD8, 0xE8, 0xAF,
		0x42, 0xF4, 0x72, 0xA1, 0x2A, 0x0D,
		0x97, 0xB1, 0x31, 0xB3,
	};

	u8 pseudoPrime2[64] = {
		0x99, 0x98, 0xCA, 0xB8, 0x5E, 0xD7,
		0xE5, 0xDC, 0x28, 0x5C, 0x6F, 0x0E,
		0x15, 0x09, 0x59, 0x6E, 0x84, 0xF3,
		0x81, 0xCD, 0xDE, 0x42, 0xDC, 0x93,
		0xC2, 0x7A, 0x62, 0xAC, 0x6C, 0xAF,
		0xDE, 0x74, 0xE3, 0xCB, 0x60, 0x20,
		0x38, 0x9C, 0x21, 0xC3, 0xDC, 0xC8,
		0xA2, 0x4D, 0xC6, 0x2A, 0x35, 0x7F,
		0xF3, 0xA9, 0xE8, 0x1D, 0x7B, 0x2C,
		0x78, 0xFA, 0xB8, 0x02, 0x55, 0x80,
		0x9B, 0xC2, 0xA5, 0xCB,
	};

	GrapaInt bi_p(pseudoPrime1, 64);
	GrapaInt bi_q(pseudoPrime2, 64);
	GrapaPrime bi_pq = (bi_p - 1)*(bi_q - 1);
	GrapaInt bi_n = bi_p * bi_q;

	for (u32 count = 0; count < rounds; count++)
	{
		// generate private and public key
		GrapaInt bi_e = bi_pq.genCoPrime(512);
		GrapaInt bi_d = bi_e.modInverse(bi_pq);

		// generate data of random length
		int t1 = 0;
		while (t1 == 0)
			t1 = (int)(gSystem->Random32() % 65);

		bool done = false;
		while (!done)
		{
			for (int i = 0; i < 64; i++)
			{
				if (i < t1)
					val[i] = (u8)(gSystem->Random32() % 256);
				else
					val[i] = 0;

				if (val[i] != 0)
					done = true;
			}
		}

		while (val[0] == 0)
			val[0] = (u8)(gSystem->Random32() % 256);

		// encrypt and decrypt data
		GrapaInt bi_data(val, t1);
		GrapaInt bi_encrypted = bi_data.modPow(bi_e, bi_n);
		GrapaInt bi_decrypted = bi_encrypted.modPow(bi_d, bi_n);

		// compare
		if (bi_decrypted != bi_data)
		{
			return;
		}
	}
}

GrapaInt GrapaPrime::StaticPrime(int bits)
{
	if (bits == 2)
	{
		GrapaInt bi_e("a932b948feed4fb2b692609bd22164fc9edb59fae7880cc1eaff7b3c9626b7e5b241c27a974833b2622ebe09beb451917663d47232488f23a117fc97720f1e7", 16);
		return bi_e;
	}
	if (bits == 3)
	{
		GrapaInt bi_d("4adf2f7a89da93248509347d2ae506d683dd3a16357e859a980c4f77a4e2f7a01fae289f13a851df6e9db5adaa60bfd2b162bbbe31f7c8f828261a6839311929d2cef4f864dde65e556ce43c89bbbf9f1ac5511315847ce9cc8dc92470a747b8792d6a83b0092d2e5ebaf852c85cacf34278efa99160f2f8aa7ee7214de07b7", 16);
		return bi_d;
	}
	if (bits == 4)
	{
		GrapaInt bi_n("e8e77781f36a7b3188d711c2190b560f205a52391b3479cdb99fa010745cbeba5f2adc08e1de6bf38398a0487c4a73610d94ec36f17f3f46ad75e17bc1adfec99839589f45f95ccc94cb2a5c500b477eb3323d8cfab0c8458c96f0147a45d27e45a4d11d54d77684f65d48f15fafcc1ba208e71e921b9bd9017c16a5231af7f", 16);
		return bi_n;
	}
	if (bits == 256)
	{
		u8 rp[32] = {
			0xCA, 0x3A, 0xD4, 0xCE, 0x1D, 0x03, 0x34, 0x38, 0x95, 0x1C, 0xCA, 0x37, 0x34, 0xE2, 0xE2, 0x3E, 
			0x8D, 0x33, 0xD7, 0xDA, 0xE1, 0xC5, 0x20, 0x33, 0x86, 0x57, 0x4A, 0x96, 0x6F, 0x5C, 0xD5, 0x95,
		};
		GrapaInt bi(rp, 32);
		return(bi);
	}
	else if (bits == 512)
	{
		u8 rp[64] = {
			0x84, 0xB7, 0x2E, 0xE4, 0x3D, 0x7F, 0x47, 0xBA, 0x58, 0x7F, 0x0A, 0xA8, 0x1C, 0xE1, 0x5B, 0xFB, 
			0x7B, 0x44, 0x2A, 0xE6, 0x62, 0x54, 0x6D, 0x4E, 0x51, 0x2B, 0x76, 0x17, 0x4C, 0x27, 0x18, 0x35, 
			0x69, 0xFC, 0x54, 0xDC, 0x06, 0x2D, 0x4D, 0xA9, 0x13, 0x33, 0x6B, 0xA7, 0x59, 0xF7, 0x32, 0x67, 
			0x7D, 0x66, 0x1A, 0xAF, 0x28, 0x52, 0x22, 0x76, 0x30, 0xD8, 0x10, 0x14, 0x08, 0x02, 0x44, 0xFB,
		};
		GrapaInt bi(rp, 64);
		return(bi);
	}
	else if (bits == 768)
	{
		u8 rp[96] = {
			0xF0, 0x46, 0x7F, 0xD1, 0x17, 0x52, 0x21, 0x20, 0x75, 0x39, 0x00, 0xD3, 0x0F, 0x42, 0x48, 0xB0, 
			0x39, 0x62, 0x13, 0x39, 0x17, 0x2F, 0x16, 0x66, 0x5C, 0xF5, 0x01, 0xD5, 0x08, 0xDB, 0x48, 0xEB, 
			0x13, 0x19, 0x3B, 0xA1, 0x71, 0xB1, 0x38, 0xEE, 0x0F, 0x26, 0x7C, 0x0B, 0x4D, 0x23, 0x1A, 0x0C, 
			0x2B, 0x1C, 0x47, 0x7C, 0x7E, 0x42, 0x7D, 0xFE, 0x23, 0x66, 0x28, 0x28, 0x04, 0x9E, 0x26, 0x09, 
			0x03, 0xCE, 0x57, 0x1C, 0x28, 0x5F, 0x6E, 0xA1, 0x21, 0xC2, 0x3D, 0x83, 0x19, 0xA2, 0x61, 0x5D, 
			0x3F, 0xC8, 0x7A, 0x3A, 0x3F, 0x18, 0x39, 0x21, 0x36, 0x3E, 0x57, 0x96, 0x2F, 0xDB, 0x64, 0x89,
		};
		GrapaInt bi(rp, 96);
		return(bi);
	}
	else if (bits == 1024)
	{
		u8 rp[128] = {
			0xEE, 0x8A, 0x4E, 0xF7, 0x63, 0x7B, 0x27, 0xF2, 0x71, 0x10, 0x7E, 0x4F, 0x2B, 0x01, 0x20, 0x32, 
			0x2C, 0xC4, 0x2F, 0xCB, 0x64, 0x1E, 0x1D, 0x34, 0x37, 0xB9, 0x20, 0x22, 0x4C, 0x73, 0x49, 0x80, 
			0x4D, 0x2E, 0x61, 0x84, 0x00, 0x37, 0x10, 0xE7, 0x72, 0xAD, 0x5C, 0x44, 0x65, 0x6D, 0x4E, 0xCB, 
			0x40, 0x66, 0x65, 0xC7, 0x70, 0x66, 0x70, 0xAC, 0x08, 0xC7, 0x39, 0xAB, 0x1D, 0x79, 0x78, 0xF6, 
			0x5B, 0x5E, 0x61, 0x3A, 0x3B, 0xFF, 0x0E, 0x08, 0x1E, 0xF2, 0x55, 0x10, 0x66, 0x93, 0x01, 0x86, 
			0x3E, 0xDF, 0x65, 0x0A, 0x44, 0x8A, 0x7B, 0xE2, 0x27, 0xB1, 0x32, 0x71, 0x70, 0xAF, 0x38, 0x27, 
			0x0F, 0x02, 0x48, 0x66, 0x21, 0x43, 0x7C, 0x07, 0x42, 0x97, 0x1E, 0x8E, 0x6D, 0x32, 0x38, 0x27, 
			0x7A, 0xB8, 0x12, 0x00, 0x4A, 0x99, 0x7C, 0xA8, 0x6B, 0xCE, 0x20, 0x6E, 0x22, 0x76, 0x2D, 0xF9,
		};
		GrapaInt bi(rp, 128);
		return(bi);
	}
	else if (bits == 1536)
	{
		u8 rp[192] = {
			0xEF, 0x3F, 0x0B, 0x7B, 0x25, 0x73, 0x38, 0xD6, 0x3E, 0x84, 0x10, 0xC2, 0x07, 0xBC, 0x74, 0xD1, 
			0x44, 0x28, 0x17, 0x8A, 0x02, 0xEC, 0x5B, 0x0E, 0x30, 0x0B, 0x54, 0x24, 0x6A, 0xD8, 0x30, 0x7B, 
			0x2A, 0x56, 0x47, 0x01, 0x1B, 0x25, 0x6F, 0x9B, 0x3B, 0xD8, 0x73, 0xA7, 0x5B, 0xC3, 0x0B, 0x0E, 
			0x15, 0xA1, 0x79, 0xD4, 0x7A, 0xA8, 0x10, 0x60, 0x33, 0xDE, 0x1C, 0xF3, 0x36, 0xCF, 0x57, 0x10, 
			0x17, 0xF6, 0x05, 0xBE, 0x58, 0x72, 0x27, 0xE2, 0x15, 0xE4, 0x4A, 0x33, 0x12, 0x85, 0x08, 0x68, 
			0x70, 0xD6, 0x07, 0xBA, 0x09, 0x75, 0x16, 0xCE, 0x43, 0x05, 0x1B, 0x96, 0x1B, 0x21, 0x21, 0xE4, 
			0x6C, 0xD5, 0x45, 0x8C, 0x44, 0x18, 0x69, 0x70, 0x07, 0x2F, 0x40, 0xD1, 0x3E, 0x12, 0x32, 0xB3, 
			0x15, 0x1D, 0x1F, 0x38, 0x33, 0xB3, 0x1D, 0x3B, 0x70, 0xA6, 0x72, 0x9A, 0x25, 0x7D, 0x63, 0xEA, 
			0x2E, 0xEB, 0x10, 0x87, 0x5C, 0x15, 0x76, 0x45, 0x77, 0x81, 0x7C, 0x2D, 0x03, 0xB9, 0x16, 0x01, 
			0x0B, 0x0F, 0x42, 0x87, 0x4D, 0x00, 0x64, 0xCA, 0x75, 0x29, 0x54, 0xC9, 0x2E, 0xD2, 0x0E, 0x54, 
			0x25, 0x6E, 0x2D, 0x0A, 0x25, 0xA9, 0x7A, 0xA7, 0x6B, 0xDF, 0x49, 0x31, 0x0C, 0x08, 0x34, 0xA5, 
			0x14, 0x81, 0x48, 0x24, 0x68, 0x3B, 0x70, 0xDE, 0x1E, 0x35, 0x35, 0x2D, 0x4B, 0x4F, 0x60, 0x97,
		};
		GrapaInt bi(rp, 192);
		return(bi);
	}
	else if (bits == 2048)
	{
		u8 rp[256] = {
			0xC7, 0x9D, 0x74, 0x38, 0x01, 0x4D, 0x1F, 0x33, 0x0A, 0x65, 0x64, 0xD8, 0x56, 0xA2, 0x2A, 0xB2,
			0x1E, 0x7F, 0x17, 0x6D, 0x76, 0x2F, 0x07, 0x50, 0x4A, 0xF9, 0x61, 0xBE, 0x47, 0x78, 0x2D, 0xEE,
			0x50, 0xA0, 0x31, 0x58, 0x44, 0xA6, 0x7C, 0x84, 0x32, 0x17, 0x23, 0x3E, 0x30, 0xBF, 0x03, 0x69,
			0x2F, 0x37, 0x01, 0xD4, 0x37, 0x28, 0x16, 0xE6, 0x2F, 0xAD, 0x75, 0x07, 0x50, 0x5A, 0x10, 0xFD,
			0x18, 0xD0, 0x70, 0xC0, 0x07, 0xE3, 0x6F, 0x74, 0x4A, 0xC2, 0x2D, 0x89, 0x7F, 0x9F, 0x3A, 0x28,
			0x40, 0xC9, 0x37, 0x7F, 0x4C, 0x36, 0x5D, 0x8F, 0x10, 0xED, 0x5F, 0x79, 0x06, 0xD7, 0x65, 0x8C,
			0x46, 0xD5, 0x1A, 0x79, 0x30, 0x35, 0x44, 0x79, 0x55, 0xDD, 0x1B, 0x52, 0x40, 0xC1, 0x12, 0x71,
			0x1E, 0x7B, 0x32, 0x99, 0x02, 0x27, 0x70, 0xDC, 0x42, 0xD2, 0x40, 0xD0, 0x0E, 0x0D, 0x7E, 0x40,
			0x46, 0x95, 0x46, 0xCF, 0x0E, 0x0A, 0x06, 0x45, 0x36, 0x20, 0x60, 0x75, 0x18, 0xE3, 0x5A, 0x07,
			0x50, 0xD4, 0x35, 0x8E, 0x49, 0x0B, 0x7C, 0xA2, 0x72, 0xB1, 0x2D, 0x05, 0x68, 0x5C, 0x0F, 0xFA,
			0x39, 0x38, 0x6E, 0x4D, 0x4D, 0x11, 0x2D, 0xCA, 0x1F, 0x82, 0x7D, 0x0C, 0x44, 0x05, 0x18, 0xEE,
			0x0D, 0x9B, 0x7B, 0x08, 0x24, 0x31, 0x72, 0xF3, 0x17, 0x23, 0x5C, 0x55, 0x67, 0x62, 0x61, 0xDF,
			0x65, 0x25, 0x19, 0xBE, 0x64, 0x38, 0x52, 0x3C, 0x07, 0x39, 0x2D, 0x73, 0x05, 0x66, 0x41, 0x69,
			0x27, 0xD7, 0x65, 0xF3, 0x1A, 0x27, 0x3C, 0x23, 0x5F, 0xFD, 0x5B, 0x3A, 0x1B, 0xFE, 0x7D, 0x50,
			0x26, 0x03, 0x12, 0x2D, 0x05, 0xB0, 0x59, 0x0D, 0x13, 0xBC, 0x1A, 0x44, 0x17, 0x85, 0x5F, 0xFA,
			0x7F, 0xD0, 0x31, 0x79, 0x24, 0xBD, 0x66, 0xC4, 0x26, 0x57, 0x3A, 0x6F, 0x46, 0x4F, 0x5D, 0xF1,
		};
		GrapaInt bi(rp, 256);
		return(bi);
	}
	else if (bits == 3072)
	{
		u8 rp[384] = {
			0x9D, 0x93, 0x69, 0xB7, 0x40, 0x34, 0x35, 0xCF, 0x21, 0xC5, 0x5D, 0x97, 0x32, 0x40, 0x64, 0x01, 
			0x05, 0x1F, 0x27, 0xD8, 0x05, 0x3B, 0x09, 0x99, 0x6C, 0x72, 0x19, 0xFC, 0x10, 0x31, 0x7D, 0xE5, 
			0x61, 0x26, 0x4C, 0xBA, 0x2F, 0xCE, 0x14, 0x51, 0x30, 0x99, 0x71, 0xE9, 0x29, 0xD9, 0x61, 0xF8, 
			0x7A, 0xBA, 0x18, 0xAC, 0x15, 0xF0, 0x73, 0x70, 0x6B, 0xE4, 0x6F, 0xDD, 0x67, 0x84, 0x1E, 0xD5, 
			0x5D, 0xFE, 0x52, 0xBE, 0x75, 0x16, 0x5E, 0x16, 0x5C, 0xD3, 0x2B, 0xDC, 0x18, 0xB2, 0x5D, 0x7E, 
			0x49, 0xAD, 0x7A, 0x45, 0x45, 0xAB, 0x2A, 0x84, 0x1A, 0x37, 0x04, 0xED, 0x2F, 0x97, 0x2E, 0xD5, 
			0x6E, 0x93, 0x08, 0x5B, 0x5E, 0x8C, 0x63, 0xA1, 0x7A, 0xB9, 0x6A, 0x9A, 0x4C, 0x9B, 0x69, 0x25, 
			0x7F, 0x11, 0x41, 0x5E, 0x68, 0x8C, 0x6E, 0x78, 0x4C, 0x52, 0x36, 0x75, 0x19, 0xD9, 0x15, 0x98, 
			0x0E, 0x9B, 0x16, 0x6F, 0x21, 0xEF, 0x3F, 0xB6, 0x5B, 0xD2, 0x15, 0x8F, 0x76, 0xA3, 0x6D, 0xC0, 
			0x41, 0x3B, 0x16, 0xF3, 0x71, 0xF1, 0x21, 0x2E, 0x4C, 0x5B, 0x02, 0x01, 0x72, 0xFA, 0x69, 0x11, 
			0x4B, 0x0E, 0x72, 0x13, 0x4C, 0x3E, 0x07, 0x57, 0x3E, 0xE5, 0x4C, 0x65, 0x1B, 0x18, 0x5A, 0x62, 
			0x3F, 0xC4, 0x08, 0x3D, 0x64, 0x79, 0x76, 0xCB, 0x49, 0xBA, 0x35, 0x5B, 0x12, 0xE7, 0x1D, 0x72, 
			0x32, 0x22, 0x0C, 0xA3, 0x31, 0xB8, 0x79, 0xC8, 0x3F, 0xF9, 0x17, 0x07, 0x01, 0x89, 0x54, 0x5F, 
			0x23, 0x81, 0x36, 0xB9, 0x42, 0x04, 0x25, 0xB1, 0x49, 0x16, 0x1E, 0x90, 0x4C, 0xD1, 0x35, 0x2F, 
			0x43, 0x4E, 0x03, 0xB9, 0x5D, 0xDB, 0x30, 0x8C, 0x48, 0x56, 0x15, 0xA1, 0x24, 0xC6, 0x67, 0x49, 
			0x7E, 0x8A, 0x08, 0x21, 0x7B, 0xB0, 0x16, 0x83, 0x14, 0x55, 0x3B, 0xEA, 0x5F, 0x26, 0x70, 0xFB, 
			0x5F, 0x4A, 0x51, 0x31, 0x03, 0x67, 0x4F, 0x65, 0x7E, 0x82, 0x30, 0x9D, 0x22, 0xDE, 0x34, 0xF3, 
			0x3C, 0x36, 0x56, 0x70, 0x61, 0xDC, 0x14, 0x24, 0x2A, 0x9F, 0x6B, 0xF3, 0x63, 0x94, 0x7F, 0xC8, 
			0x38, 0x0C, 0x7B, 0x26, 0x6C, 0x5B, 0x34, 0x56, 0x36, 0x44, 0x48, 0xA7, 0x2D, 0x20, 0x25, 0x71, 
			0x11, 0x1D, 0x1F, 0xE1, 0x14, 0x28, 0x7B, 0xB6, 0x30, 0x5B, 0x1D, 0x78, 0x3F, 0x0C, 0x2E, 0xCA, 
			0x40, 0xCC, 0x43, 0xF2, 0x69, 0xF6, 0x27, 0xA3, 0x60, 0xA3, 0x66, 0xA9, 0x78, 0x1A, 0x17, 0x15, 
			0x6B, 0x14, 0x36, 0xEF, 0x71, 0x71, 0x6C, 0x9E, 0x5F, 0x2F, 0x7F, 0x82, 0x11, 0xBC, 0x19, 0x75, 
			0x1E, 0x00, 0x5A, 0x31, 0x44, 0xB7, 0x0B, 0xCF, 0x7B, 0xE7, 0x6B, 0xCE, 0x2B, 0x9B, 0x0E, 0x72, 
			0x61, 0x33, 0x72, 0x56, 0x07, 0xD9, 0x78, 0x7E, 0x76, 0x04, 0x31, 0x5D, 0x27, 0x13, 0x59, 0x79,
		};
		GrapaInt bi(rp, 384);
		return(bi);
	}
	else if (bits == 4096)
	{
		u8 rp[512] = {
			0xFD, 0xC5, 0x6F, 0xE2, 0x4F, 0xE3, 0x0F, 0x51, 0x24, 0x61, 0x48, 0xF4, 0x46, 0x29, 0x41, 0xC3, 
			0x33, 0x04, 0x49, 0xEA, 0x71, 0x07, 0x11, 0xFB, 0x6C, 0xA5, 0x7A, 0x49, 0x0F, 0xAA, 0x4B, 0x62, 
			0x64, 0xCD, 0x3C, 0xE7, 0x74, 0xA4, 0x24, 0x2E, 0x68, 0x11, 0x06, 0x77, 0x31, 0x8A, 0x2A, 0x95, 
			0x59, 0xCB, 0x34, 0x5A, 0x5D, 0xCD, 0x60, 0x31, 0x23, 0x1A, 0x2F, 0xA3, 0x34, 0xD7, 0x4F, 0xB4, 
			0x0F, 0x1B, 0x48, 0x6A, 0x07, 0x69, 0x28, 0xCF, 0x30, 0x68, 0x75, 0x54, 0x3C, 0x33, 0x2E, 0x5F, 
			0x2B, 0xD0, 0x63, 0x61, 0x5B, 0xB3, 0x02, 0xD8, 0x04, 0x56, 0x01, 0xF7, 0x63, 0x51, 0x1E, 0xF5, 
			0x44, 0x6E, 0x77, 0x2E, 0x5B, 0xB9, 0x7E, 0x9F, 0x20, 0x76, 0x28, 0x5D, 0x6E, 0x7A, 0x50, 0x1C, 
			0x6E, 0x71, 0x42, 0xE3, 0x76, 0xDF, 0x31, 0x7A, 0x0F, 0x09, 0x71, 0x39, 0x4A, 0x0C, 0x48, 0x3D, 
			0x23, 0xC5, 0x28, 0x37, 0x32, 0x5B, 0x3F, 0x47, 0x2E, 0x89, 0x38, 0xA5, 0x59, 0xF8, 0x77, 0x07, 
			0x76, 0x4D, 0x11, 0x04, 0x20, 0xB9, 0x73, 0xE4, 0x4D, 0x21, 0x5B, 0x9C, 0x19, 0x41, 0x56, 0xEA, 
			0x13, 0x5F, 0x64, 0xC8, 0x29, 0x57, 0x6C, 0xB4, 0x14, 0x31, 0x75, 0x7F, 0x09, 0x82, 0x1E, 0x9B, 
			0x17, 0x44, 0x0E, 0x25, 0x1F, 0xE8, 0x52, 0x1F, 0x43, 0xCC, 0x2D, 0x93, 0x72, 0x67, 0x02, 0x95, 
			0x30, 0xBB, 0x10, 0x63, 0x2B, 0xF3, 0x21, 0x11, 0x3E, 0x3C, 0x14, 0x7F, 0x71, 0x31, 0x06, 0x94, 
			0x14, 0x74, 0x2C, 0xEB, 0x00, 0x52, 0x04, 0x77, 0x63, 0x7B, 0x11, 0xD0, 0x58, 0x34, 0x5F, 0x19, 
			0x40, 0x97, 0x18, 0xCB, 0x62, 0xB6, 0x3E, 0xC7, 0x3C, 0xBA, 0x61, 0x77, 0x5E, 0x5A, 0x62, 0xEB, 
			0x10, 0x3D, 0x42, 0x37, 0x03, 0x20, 0x23, 0x78, 0x77, 0xDC, 0x21, 0x47, 0x1E, 0x9F, 0x0C, 0x93, 
			0x1E, 0xF3, 0x26, 0x04, 0x23, 0x67, 0x20, 0x83, 0x32, 0xF9, 0x6E, 0x7A, 0x67, 0x95, 0x0B, 0xDD, 
			0x7C, 0x3C, 0x35, 0x2E, 0x0E, 0xB7, 0x57, 0xEB, 0x17, 0xDE, 0x13, 0x2B, 0x5A, 0xE0, 0x67, 0x5C, 
			0x2F, 0x0E, 0x4A, 0x51, 0x61, 0x0D, 0x49, 0x30, 0x47, 0x89, 0x43, 0xDB, 0x5C, 0xBA, 0x2D, 0xE4, 
			0x09, 0x53, 0x2F, 0x31, 0x1E, 0xBE, 0x0A, 0xDB, 0x15, 0xB0, 0x6C, 0xEF, 0x53, 0x6E, 0x38, 0x0F, 
			0x4B, 0x67, 0x32, 0x34, 0x1B, 0xF0, 0x13, 0xF8, 0x14, 0x39, 0x10, 0x2E, 0x36, 0xDE, 0x79, 0xBB, 
			0x17, 0x9E, 0x4B, 0xE5, 0x34, 0x1E, 0x15, 0x95, 0x6E, 0xC1, 0x32, 0x45, 0x6F, 0xFF, 0x63, 0x88, 
			0x35, 0xBD, 0x54, 0x71, 0x51, 0x96, 0x64, 0x46, 0x16, 0x17, 0x58, 0x45, 0x08, 0x59, 0x54, 0x5D, 
			0x26, 0x7E, 0x49, 0x2B, 0x44, 0xFC, 0x71, 0xA3, 0x3B, 0xC0, 0x15, 0x22, 0x29, 0x8A, 0x1A, 0xE2, 
			0x07, 0x0E, 0x22, 0x0A, 0x6C, 0xC6, 0x55, 0xC6, 0x1D, 0x73, 0x27, 0x33, 0x6C, 0xC2, 0x07, 0x05, 
			0x44, 0x91, 0x37, 0x28, 0x2C, 0xBF, 0x68, 0xCF, 0x20, 0x9C, 0x25, 0xB7, 0x7A, 0x47, 0x0B, 0x78, 
			0x1F, 0x9B, 0x36, 0x43, 0x35, 0x87, 0x6C, 0x62, 0x3D, 0xDC, 0x3E, 0x4C, 0x78, 0xF0, 0x6F, 0x2F, 
			0x7F, 0xB6, 0x28, 0x3D, 0x1C, 0x10, 0x45, 0x26, 0x3C, 0x85, 0x02, 0x77, 0x4D, 0xAC, 0x0E, 0xE4, 
			0x16, 0xE1, 0x06, 0x9E, 0x41, 0x20, 0x44, 0x46, 0x3E, 0x20, 0x45, 0x22, 0x2A, 0xFA, 0x2E, 0x94, 
			0x55, 0x0D, 0x61, 0x0F, 0x08, 0xD4, 0x00, 0xF1, 0x19, 0xE8, 0x08, 0x17, 0x70, 0x71, 0x5B, 0x02, 
			0x2B, 0xA0, 0x12, 0xDE, 0x62, 0x19, 0x41, 0xDC, 0x08, 0x4E, 0x79, 0x88, 0x5A, 0x37, 0x5B, 0x31, 
			0x20, 0xF4, 0x08, 0x7F, 0x1E, 0x33, 0x76, 0xBC, 0x1E, 0x76, 0x01, 0x88, 0x00, 0x8C, 0x31, 0xED,
		};
		GrapaInt bi(rp, 512);
		return(bi);
	}
	else
	{
		GrapaInt x;
		return(x);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//void GrapaPrime::SieveOfAtkin(u32 limit)
//{
//	BtBitField8 mybitfields[1];
//
//	limit -= (limit % 8);
//	u32 size = (limit / 8) * sizeof(mybitfields);
//	mybitfields *isPrime = (mybitfields*)malloc(size);
//	memset(isPrime, 0, size);
//	u64 sqrt = limit ^ 2;
//	for (u64 x = 1; x <= sqrt; x++)
//	{
//		for (u64 y = 1; y <= sqrt; y++)
//		{
//			u64 n = 4 * x * x + y * y;
//			if (n <= limit && (n % 12 == 1 || n % 12 == 5))
//			{
//				switch (n % 8)
//				{
//				case 0: isPrime[n / 8].b0 ^= true; break;
//				case 1: isPrime[n / 8].b1 ^= true; break;
//				case 2: isPrime[n / 8].b2 ^= true; break;
//				case 3: isPrime[n / 8].b3 ^= true; break;
//				case 4: isPrime[n / 8].b4 ^= true; break;
//				case 5: isPrime[n / 8].b5 ^= true; break;
//				case 6: isPrime[n / 8].b6 ^= true; break;
//				case 7: isPrime[n / 8].b7 ^= true; break;
//				}
//			}
//
//			n = 3 * x * x + y * y;
//			if (n <= limit && n % 12 == 7)
//			{
//				switch (n % 8)
//				{
//				case 0: isPrime[n / 8].b0 ^= true; break;
//				case 1: isPrime[n / 8].b1 ^= true; break;
//				case 2: isPrime[n / 8].b2 ^= true; break;
//				case 3: isPrime[n / 8].b3 ^= true; break;
//				case 4: isPrime[n / 8].b4 ^= true; break;
//				case 5: isPrime[n / 8].b5 ^= true; break;
//				case 6: isPrime[n / 8].b6 ^= true; break;
//				case 7: isPrime[n / 8].b7 ^= true; break;
//				}
//			}
//
//			n = 3 * x * x - y * y;
//			if (x > y && n <= limit && n % 12 == 11)
//			{
//				switch (n % 8)
//				{
//				case 0: isPrime[n / 8].b0 ^= true; break;
//				case 1: isPrime[n / 8].b1 ^= true; break;
//				case 2: isPrime[n / 8].b2 ^= true; break;
//				case 3: isPrime[n / 8].b3 ^= true; break;
//				case 4: isPrime[n / 8].b4 ^= true; break;
//				case 5: isPrime[n / 8].b5 ^= true; break;
//				case 6: isPrime[n / 8].b6 ^= true; break;
//				case 7: isPrime[n / 8].b7 ^= true; break;
//				}
//			}
//		}
//	}
//	for (u64 n = 5; n <= sqrt; n++)
//	{
//		u8 bit = 0;
//		switch (n % 8)
//		{
//		case 0: bit = isPrime[n / 8].b0; break;
//		case 1: bit = isPrime[n / 8].b1; break;
//		case 2: bit = isPrime[n / 8].b2; break;
//		case 3: bit = isPrime[n / 8].b3; break;
//		case 4: bit = isPrime[n / 8].b4; break;
//		case 5: bit = isPrime[n / 8].b5; break;
//		case 6: bit = isPrime[n / 8].b6; break;
//		case 7: bit = isPrime[n / 8].b7; break;
//		}
//		if (bit)
//		{
//			u64 s = n * n;
//			for (u64 k = s; k <= limit; k += s)
//			{
//				switch (k % 8)
//				{
//				case 0: isPrime[k / 8].b0 ^= false; break;
//				case 1: isPrime[k / 8].b1 ^= false; break;
//				case 2: isPrime[k / 8].b2 ^= false; break;
//				case 3: isPrime[k / 8].b3 ^= false; break;
//				case 4: isPrime[k / 8].b4 ^= false; break;
//				case 5: isPrime[k / 8].b5 ^= false; break;
//				case 6: isPrime[k / 8].b6 ^= false; break;
//				case 7: isPrime[k / 8].b7 ^= false; break;
//				}
//			}
//		}
//	}
//	//primes.Add(2);
//	//primes.Add(3);
//	for (u64 n = 5; n <= limit; n += 2)
//	{
//		u8 bit = 0;
//		switch (n % 8)
//		{
//		case 0: bit = isPrime[n / 8].b0; break;
//		case 1: bit = isPrime[n / 8].b1; break;
//		case 2: bit = isPrime[n / 8].b2; break;
//		case 3: bit = isPrime[n / 8].b3; break;
//		case 4: bit = isPrime[n / 8].b4; break;
//		case 5: bit = isPrime[n / 8].b5; break;
//		case 6: bit = isPrime[n / 8].b6; break;
//		case 7: bit = isPrime[n / 8].b7; break;
//		}
//		if (bit)
//		{
//			u32 x = n;
//			;// primes.Add(n);
//		}
//	}
//	free(isPrime);
//}

//bool GrapaPrime::IsPrime(u32 n)
//{
//	GrapaInt x;
//	return(x.isPrime32(n));
//}
//
//u32 GrapaPrime::GenPrime16(u32 min, u32 max)
//{
//	GrapaInt x;
//	return(x.GenPrime16(min,max));
//}
//
//u32 GrapaPrime::GenPrime32(u32 min, u32 max)
//{
//	if (min == max) return(0);
//	u32 n=0,ns=0;
//	Random(ns);
//	n = ns = min + (ns % (max - min));
//	while (n<max&&!IsPrime(n)) n++;
//	if (n == max)
//	{
//		n = ns-1;
//		while (n>min&&!IsPrime(n)) n--;
//		if (n == min) return(0);
//	}
//	return(n);
//}
//
//u32 GrapaPrime::NextPrime32(u32 n, u32 min, u32 max)
//{
//	if (min == max) return(0);
//	u32 ns = n;
//	n++;
//	while (n<max&&!IsPrime(n)) n++;
//	if (n == max)
//	{
//		n = ns - 1;
//		while (n>min&&!IsPrime(n)) n--;
//		if (n == min) return(0);
//	}
//	return(n);
//}
//
//u32 GrapaPrime::PriorPrime32(u32 n, u32 min, u32 max)
//{
//	if (min == max) return(0);
//	u32 ns = n;
//	n--;
//	while (n>min&&!IsPrime(n)) n--;
//	if (n == min)
//	{
//		n = ns + 1;
//		while (n<max&&!IsPrime(n)) n++;
//		if (n == max) return(0);
//	}
//	return(n);
//}
//
//u32 GrapaPrime::FindPrimeFactor(u32 pNum)
//{
//	u32 n = pNum;
//	u32 lastBit=0;
//	for (u32 j = 0; j < 32; j++)
//	{
//		if (n & 1) lastBit = j + 1;
//		n = n >> 1;
//	}
//	n = pNum >> ((lastBit-1) / 2);
//	u32 r = pNum % n;
//	u32 n2 = r >> ((lastBit-1) / 4);
//	n += n2;
//	if (n2) n += r % n2;
//	u32 x = PriorPrime32(n, 1, 0xFFFF);
//	while (pNum % x) 
//		x = PriorPrime32(x, 1, 0xFFFF);
//	return(x);
//}

//void GrapaPrime::Running()
//{
//	BtRandom rand;
//	GrapaInt zero(0);
//	mFound = false;
//	while (!mStop)
//	{
//		genRandomBits(mBits, rand );
//		data[0] |= 0x01;		// make it odd
//		if (TestPrime(50))
//			break;
//	}
//	mFound = true;
//	if (vCond) vCond->SendCondition();
//	vCond = NULL;
//}
//
//void GrapaPrime::Stopping()
//{
//
//}
