// GrapaEncode.cpp
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

#include "GrapaEncode.h"
#include "GrapaTinyAES.h"
#include "GrapaHash.h"
#include "GrapaMem.h"
#include "GrapaState.h"
#include "GrapaFloat.h"

#include <vector>

#include <openssl/rsa.h>

#if defined(__MINGW32__) || defined(__GNUC__)
#else
#ifdef _WIN32
#pragma comment(lib, "openssl-lib/win/libcrypto_static.lib")
#pragma comment(lib, "openssl-lib/win/libssl_static.lib")
#endif
#endif

// https://www.slideserve.com/ally/rsa-public-key-encryption-algorithm
// https://en.wikipedia.org/wiki/PKCS

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <openssl/bn.h>
#include <openssl/rand.h>
#ifdef OPENSSL_FIPS
#include <openssl/fips.h>
#endif

#include <openssl/evp.h>
#include <openssl/engine.h>
#include <openssl/ec.h>
#include <openssl/kdf.h>			

//static bool gLoadedDigestsSSL = false;

struct RSAs
{
	u32 pad;
	u32 version;
	RSA_METHOD* meth;
	ENGINE* engine;
	BIGNUM* n;              // public modulus
	BIGNUM* e;              // public exponent
	BIGNUM* d;              // private exponent
	BIGNUM* p;              // secret prime factor
	BIGNUM* q;              // secret prime factor
	BIGNUM* dmp1;           // d mod (p-1)
	BIGNUM* dmq1;           // d mod (q-1)
	BIGNUM* iqmp;           // q^-1 mod p
	// ...
};

GrapaEncode::GrapaEncode()
{
	//mKeySize = (u64)64;
	//mEncSize = mKeySize - 8; // amount the data expands by (any size of data)
	//mDataSize = mEncSize - 24; // 2 for pad length, 22 for required padding
	//cZero = (u64)0;
	//cOne = (u64)1;
	//m_n = cZero;
	//m_e = (u64)65537;   // 2 bits: 17, 257, 65537, 4294967297, 18446744073709551617 - Fermat number
	//m_d = cZero;
	//m_p = cZero;
	//m_q = cZero;
	//m_dmp1 = cZero;
	//m_dmq1 = cZero;
	//m_iqmp = cZero;
	mRSA = NULL;
	mDH = NULL;
	mEC = NULL;
	mBCe = NULL;
	mBCd = NULL;
	mMD = NULL;
	mRPK = NULL;
	//EC_builtin_curve* gg = NULL;
	//size_t g = 0;
	//if (gLoadedDigestsSSL == false)
	//{
	//	gLoadedDigestsSSL = true;
	//	OpenSSL_add_all_digests();
	//	OpenSSL_add_all_ciphers();
	//}
}

GrapaEncode::~GrapaEncode()
{
	Clear();
}

void GrapaEncode::Clear()
{
	if (mRSA) RSA_free((RSA*)mRSA);
	mRSA = NULL;
	if (mDH) DH_free((DH*)mDH);
	mDH = NULL;
	if (mEC) EVP_PKEY_CTX_free((EVP_PKEY_CTX*)mEC);
	mEC = NULL;
	if (mBCe) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCe);
	mBCe = NULL;
	if (mBCd) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCd);
	mBCd = NULL;
	if (mMD) EVP_MD_CTX_free((EVP_MD_CTX*)mMD);
	mMD = NULL;
	if (mRPK) EVP_PKEY_free((EVP_PKEY*)mRPK);
	mRPK = NULL;
}

bool GrapaEncode::FROM(GrapaRuleEvent* pKey)
{
	Clear();

	s64 idx;
	GrapaRuleEvent* x = pKey->vQueue->Search("method", idx);
	if (x == NULL) return false;

	if (x->mValue.StrLowerCmp("rsa")==0)
	{
		mRSA = (RSAs*)RSA_new_method(NULL);

		x = pKey->vQueue->Search("n", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->n = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("e", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->e = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("d", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->d = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("p", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->p = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("q", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->q = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("dmp1", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->dmp1 = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("dmq1", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->dmq1 = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("iqmp", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) ((RSAs*)mRSA)->iqmp = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		return true;
	}
	else if (x->mValue.StrLowerCmp("ec") == 0)
	{
		int err;
		BIGNUM* n = NULL;;
		int nid = 0;

		x = pKey->vQueue->Search("curve", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x == NULL) return false;

		if (x->mValue.StrLowerCmp("secp224r1") == 0)
			nid = NID_secp224r1;
		else if (x->mValue.StrLowerCmp("prime256v1") == 0)
			nid = NID_X9_62_prime256v1;
		else
			return false;
		EC_KEY* keyy = EC_KEY_new_by_curve_name(nid);
		x = pKey->vQueue->Search("pub", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x)
		{
			n = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);
			EC_POINT* puby = EC_POINT_bn2point(EC_KEY_get0_group(keyy), n, NULL, NULL);
			err = EC_KEY_set_public_key(keyy, puby);
			EC_POINT_free(puby);
			BN_free(n);
			if (err <= 0)
			{
				EC_KEY_free(keyy);
				return false;
			}
		}
		else
		{
			EC_KEY_free(keyy);
			return false;
		}
		x = pKey->vQueue->Search("prv", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x)
		{
			n = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);
			err = EC_KEY_set_private_key(keyy, n);
			BN_free(n);
			if (err <= 0)
			{
				EC_KEY_free(keyy);
				return false;
			}
		}
		EVP_PKEY* ekeyy = EVP_PKEY_new();
		err = EVP_PKEY_set1_EC_KEY(ekeyy, keyy);
		EC_KEY_free(keyy);
		if (err <= 0)
		{
			EC_KEY_free(keyy);
			EVP_PKEY_free(ekeyy);
			return false;
		}
		mEC = EVP_PKEY_CTX_new(ekeyy, ENGINE_get_default_EC());
		EVP_PKEY_free(ekeyy);
		return true;
	}
	else if (x->mValue.StrLowerCmp("dh") == 0)
	{
		int err;

		mDH = DH_new();
		BIGNUM *p, * q, * g, * pub, * prv;
		p = q = g = pub = prv = NULL;

		x = pKey->vQueue->Search("p", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) p = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("q", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) q = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("g", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) g = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("pub", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) pub = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("prv", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) prv = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		err = DH_set0_pqg((DH*)mDH, p, q, g);
		if (err >= 1)
			err = DH_set0_key((DH*)mDH, pub, prv);
		else
		{
			BN_free(p);
			BN_free(q);
			BN_free(g);
		}
		if (err >= 1)
			return true;

		BN_free(pub);
		BN_free(prv);
		DH_free((DH*)mDH);
		mDH = NULL;
		return false;
	}
	else if (x->mValue.StrLowerCmp("bc") == 0)
	{
		GrapaCHAR ciphername("aes-256-cbc");
		GrapaBYTE key, iv;
		if (pKey && pKey->vQueue)
		{
			x = pKey->vQueue->Search("cipher", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) ciphername.FROM(x->mValue);
			x = pKey->vQueue->Search("key", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) key.FROM(x->mValue);
			x = pKey->vQueue->Search("iv", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) iv.FROM(x->mValue);
		}
		if (key.mLength == 0)
			return false;
		//const EVP_CIPHER* cipher = EVP_aes_256_cbc();
		//if (ciphername.StrLowerCmp("aes_256_cbc") == 0) cipher = EVP_aes_256_cbc();
		EVP_CIPHER* cipher = (EVP_CIPHER*)EVP_get_cipherbyname((char*)ciphername.mBytes);
		if (cipher == NULL) return false;
		mBCe = EVP_CIPHER_CTX_new();
		mBCd = EVP_CIPHER_CTX_new();
		int err = 0;
		err = EVP_CipherInit_ex((EVP_CIPHER_CTX*)mBCe, cipher, NULL, key.mLength ? key.mBytes : NULL, iv.mLength ? iv.mBytes : NULL, 1);
		if (err <= 0)
		{
			if (mBCe) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCe);
			mBCe = NULL;
			if (mBCd) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCd);
			mBCd = NULL;
			return false;
		}
		err = EVP_CipherInit_ex((EVP_CIPHER_CTX*)mBCd, cipher, NULL, key.mLength ? key.mBytes : NULL, iv.mLength ? iv.mBytes : NULL, 0);
		if (err <= 0)
		{
			if (mBCe) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCe);
			mBCe = NULL;
			if (mBCd) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCd);
			mBCd = NULL;
			return false;
		}
		return true;
	}
	else if (x->mValue.StrLowerCmp("md") == 0)
	{
		GrapaCHAR digestname("sha512");
		if (pKey && pKey->vQueue)
		{
			x = pKey->vQueue->Search("digest", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) digestname.FROM(x->mValue);
		}
		//const EVP_MD* md = EVP_sha512();
		//if (digestname.StrLowerCmp("sha512") == 0) md = EVP_sha512();
		EVP_MD* md = (EVP_MD *) EVP_get_digestbyname((char*)digestname.mBytes);
		if (md == NULL) return false;
		mMD = EVP_MD_CTX_new();
		int err = EVP_DigestInit_ex((EVP_MD_CTX*)mMD, md, NULL);
		if (err <= 0)
		{
			if (mMD) EVP_MD_CTX_free((EVP_MD_CTX*)mMD);
			mMD = NULL;
			return false;
		}
		return true;
	}
	else if (x->mValue.StrLowerCmp("rpk") == 0)
	{
		GrapaCHAR algorithmname("ED25519");
		GrapaCHAR pub,prv;
		if (pKey && pKey->vQueue)
		{
			x = pKey->vQueue->Search("alg", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) algorithmname.FROM(x->mValue);
			x = pKey->vQueue->Search("pub", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) pub.FROM(x->mValue);
			x = pKey->vQueue->Search("prv", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) prv.FROM(x->mValue);
		}

		if (prv.mLength)
		{
			int algorithm = NID_ED25519;
			if (algorithmname.StrLowerCmp("ED25519") == 0) algorithm = EVP_PKEY_ED25519;
			else if (algorithmname.StrLowerCmp("X25519") == 0) algorithm = EVP_PKEY_X25519;
			else if (algorithmname.StrLowerCmp("ED448") == 0) algorithm = EVP_PKEY_ED448;
			else if (algorithmname.StrLowerCmp("X448") == 0) algorithm = EVP_PKEY_X448;
			else if (algorithmname.StrLowerCmp("HMAC") == 0) algorithm = EVP_PKEY_HMAC;
			else if (algorithmname.StrLowerCmp("POLY1305") == 0) algorithm = EVP_PKEY_POLY1305;
			else if (algorithmname.StrLowerCmp("SIPHASH") == 0) algorithm = EVP_PKEY_SIPHASH;
			else return false;
			mRPK = EVP_PKEY_new_raw_private_key(algorithm, NULL, prv.mBytes, prv.mLength);
		}
		else
		{
			int algorithm = NID_ED25519;
			if (algorithmname.StrLowerCmp("ED25519") == 0) algorithm = EVP_PKEY_ED25519;
			else if (algorithmname.StrLowerCmp("X25519") == 0) algorithm = EVP_PKEY_X25519;
			else if (algorithmname.StrLowerCmp("ED448") == 0) algorithm = EVP_PKEY_ED448;
			else if (algorithmname.StrLowerCmp("X448") == 0) algorithm = EVP_PKEY_X448;
			else return false;
			mRPK = EVP_PKEY_new_raw_public_key(algorithm, NULL, pub.mBytes, pub.mLength);
		}

		if (mRPK == NULL)
			return false;

		return true;
	}
	return false;
}

GrapaRuleEvent* GrapaEncode::GenKeys(const GrapaCHAR& pMethod, GrapaRuleEvent* pParams)
{
	Clear();

	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* x = NULL;
	int err = 1;
	s64 idx;

	if (((GrapaCHAR*)&pMethod)->StrLowerCmp("rsa") == 0)
	{
		// https://www.openssl.org/docs/man1.0.2/man3/rsa.html

		//mKeySize = pSize;
		//if (mKeySize % 16) mKeySize += 16;
		//mKeySize = mKeySize / 16;
		//if (mKeySize < 8) mKeySize = 8;  // increase by 16 bytes, min of 128 bytes (16 data)
		////if (mKeySize > (GrapaInt::maxItems32 / 8))  // need to allocate 2x the key size, plus a bit more
		////	mKeySize = (GrapaInt::maxItems32 / 8);
		//mKeySize *= 16;
		//mEncSize = mKeySize - 8; // amount the data expands by (any size of data)
		//mDataSize = mEncSize - 24; // 2 for pad length, 22 for required padding

		u32 bits = 1024;
		GrapaInt e = (u64)65537;

		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("bits", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x)
			{
				GrapaInt b;
				b.FromBytes(x->mValue);
				bits = b.LongValue();
			}

			x = pParams->vQueue->Search("e", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) e.FromBytes(x->mValue);
			if (e.IsZero()) e = (u64)65537;
		}

		u32 sz;
		GrapaBYTE gb;
		BIGNUM* n;
		GrapaCHAR s;

		mRSA = (RSAs*)RSA_new_method(NULL);

		gb = e.getBytes();
		n = BN_bin2bn((u8*)gb.mBytes, gb.mLength, NULL);
		err = RSA_generate_key_ex((RSA*)mRSA, bits, n, NULL);
		BN_free(n);

		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();

		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("rsa")));

		GrapaInt g;
		gb.mToken = GrapaTokenType::INT;

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->n) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->n, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("n"), g.getBytes()));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->e) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->e, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("e"), g.getBytes()));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->d) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->d, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("d"), g.getBytes()));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->p) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->p, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("p"), g.getBytes()));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->q) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->q, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("q"), g.getBytes()));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->dmp1) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->dmp1, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("dmp1"), g.getBytes()));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->dmq1) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->dmq1, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("dmq1"), g.getBytes()));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->iqmp) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->iqmp, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("iqmp"), g.getBytes()));

		return result;
		//	https://en.wikipedia.org/wiki/RSA_(cryptosystem

		//m_e = pPublicE;
		//if (m_e.IsZero())
		//	m_e = (u64)65537;
		//m_p.GenPrime(mKeySize * 4, 0, cZero, cOne);
		//m_q.GenPrime(mKeySize * 4, 0, m_p, m_e);
		//if (m_p < m_q)
		//{
		//	GrapaInt message(m_p);
		//	m_p = m_q;
		//	m_q = message;
		//}
		//m_n = m_p * m_q;
		//m_d = m_e.modInverse((m_p - cOne)*(m_q - cOne));
		//m_dmp1 = m_d % (m_p - cOne);
		//m_dmq1 = m_d % (m_q - cOne);
		//m_iqmp = m_q.modInverse(m_p);
	}
	else if (((GrapaCHAR*)&pMethod)->StrLowerCmp("dh") == 0)
	{
		u32 bits = 1024;
		u32 pG = 2;
		GrapaInt pP;
		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("bits", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x)
			{
				GrapaInt b;
				b.FromBytes(x->mValue);
				bits = b.LongValue();
			}

			x = pParams->vQueue->Search("g", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x)
			{
				GrapaInt b;
				b.FromBytes(x->mValue);
				pG = b.LongValue();
			}

			x = pParams->vQueue->Search("p", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) pP.FromBytes(x->mValue);
		}

		mDH = DH_new();
		if (mDH == NULL)
			err = -1;

		if (err >= 1)
		{
			if (((GrapaInt*)&pP)->IsZero())
			{
				int gen = DH_GENERATOR_5;
				if (pG == 2 || pG == 5) gen = pG;
				err = DH_generate_parameters_ex((DH*)mDH, bits, gen, NULL);
			}
			else
			{
				GrapaBYTE gg;

				gg = ((GrapaInt*)&pP)->getBytes();
				BIGNUM* p = BN_bin2bn((u8*)gg.mBytes, gg.mLength, NULL);

				gg = GrapaInt(pG).getBytes();
				BIGNUM* g = BN_bin2bn((u8*)gg.mBytes, gg.mLength, NULL);

				err = DH_set0_pqg((DH*)mDH, p, NULL, g);
			}
		}

		int codes;

		if (err >= 1)
			err = DH_check((DH*)mDH, &codes);
		if (err >= 1 && codes != 0)
			err = -1;
		if (err >= 1)
			err = DH_generate_key((DH*)mDH);
		if (err >= 1)
		{
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::LIST;
			result->vQueue = new GrapaRuleQueue();

			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("dh")));

			u32 sz;
			GrapaBYTE gb;
			GrapaInt g;
			gb.mToken = GrapaTokenType::INT;
			BIGNUM* n;

			n = (BIGNUM*) DH_get0_p((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("p"), g.getBytes()));
			}

			n = (BIGNUM*) DH_get0_q((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("q"), g.getBytes()));
			}

			n = (BIGNUM*) DH_get0_g((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("g"), g.getBytes()));
			}

			n = (BIGNUM*) DH_get0_pub_key((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("pub"), g.getBytes()));
			}

			n = (BIGNUM*) DH_get0_priv_key((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("prv"), g.getBytes()));
			}
		}
		
		return result;

	}
	else if (((GrapaCHAR*)&pMethod)->StrLowerCmp("ec") == 0)
	{
		GrapaCHAR pCurve("prime256v1");
		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("curve", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) pCurve.FROM(x->mValue);
		}

		int nid = 0;
		if (((GrapaCHAR*)&pCurve)->StrLowerCmp("secp224r1") == 0)
			nid = NID_secp224r1;
		else if (((GrapaCHAR*)&pCurve)->StrLowerCmp("prime256v1") == 0)
			nid = NID_X9_62_prime256v1;
		else
			return result;

		int err;

		EC_KEY* key1 = EC_KEY_new_by_curve_name(nid);
		if (key1==NULL) return result;
		err = EC_KEY_generate_key(key1);
		if (err <= 0)
		{
			EC_KEY_free(key1);
			return result;
		}
		//err = EC_KEY_check_key(key1);
		//err = EC_KEY_can_sign(key1);
		BIGNUM* pub1 = EC_POINT_point2bn(EC_KEY_get0_group(key1), EC_KEY_get0_public_key(key1), POINT_CONVERSION_COMPRESSED, NULL, NULL);
		BIGNUM* prv1 = (BIGNUM*) EC_KEY_get0_private_key(key1);
		EVP_PKEY* ekey1 = EVP_PKEY_new();
		err = EVP_PKEY_set1_EC_KEY(ekey1, key1);
		EC_KEY_free(key1);
		mEC = (void*)EVP_PKEY_CTX_new(ekey1, ENGINE_get_default_EC());
		EVP_PKEY_free(ekey1);

		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();

		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("ec")));
		pCurve.ToLower();
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("curve"), pCurve));

		GrapaInt g;
		GrapaBYTE gb;
		u32 sz;

		gb.mToken = GrapaTokenType::INT;
		gb.SetSize(BN_num_bytes(pub1) + 1);
		sz = BN_bn2bin(pub1, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("pub"), g.getBytes()));

		gb.SetSize(BN_num_bytes(prv1) + 1);
		sz = BN_bn2bin(prv1, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("prv"), g.getBytes()));

		BN_free(pub1);

		return result;
	}
	else if (((GrapaCHAR*)&pMethod)->StrLowerCmp("bc") == 0)
	{
		GrapaBYTE key, iv;
		GrapaCHAR salt, data;
		GrapaCHAR ciphername("aes-256-cbc");
		GrapaCHAR digestname("sha512");
		int count=1;
		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("cipher", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) ciphername.FROM(x->mValue);
			x = pParams->vQueue->Search("key", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) key.FROM(x->mValue);
			x = pParams->vQueue->Search("iv", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) iv.FROM(x->mValue);
			x = pParams->vQueue->Search("digest", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) digestname.FROM(x->mValue);
			x = pParams->vQueue->Search("count", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) count = GrapaInt(x->mValue).LongValue();
			x = pParams->vQueue->Search("salt", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) salt.FROM(x->mValue);
			x = pParams->vQueue->Search("data", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) data.FROM(x->mValue);
		}
		//const EVP_CIPHER* cipher = EVP_aes_256_cbc();
		//if (ciphername.StrLowerCmp("aes_256_cbc") == 0) cipher = EVP_aes_256_cbc();
		EVP_CIPHER* cipher = (EVP_CIPHER*) EVP_get_cipherbyname((char*)ciphername.mBytes);
		if (cipher == NULL) return result;
		//const EVP_MD* md = EVP_sha512();
		//if (digestname.StrLowerCmp("sha512") == 0) md = EVP_sha512();
		EVP_MD* md = (EVP_MD*) EVP_get_digestbyname((char*)digestname.mBytes);
		if (md == NULL) return result;
		int err = 0;
		if (key.mLength == 0)
		{
			err = EVP_BytesToKey(cipher, md, salt.mBytes, NULL, 0, count, key.mBytes, iv.mBytes);
			if (err <= 0)
				return result;
			key.SetLength(err);
			iv.SetLength(err/2);
			err = EVP_BytesToKey(cipher, md, salt.mBytes, data.mBytes, data.mLength, count, key.mBytes, iv.mBytes);
		}
		mBCe = EVP_CIPHER_CTX_new();
		mBCd = EVP_CIPHER_CTX_new();
		err = EVP_CipherInit_ex((EVP_CIPHER_CTX*)mBCe, cipher, NULL, key.mLength ? key.mBytes : NULL, iv.mLength ? iv.mBytes : NULL, 1);
		if (err <= 0)
		{
			if (mBCe) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCe);
			mBCe = NULL;
			if (mBCd) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCd);
			mBCd = NULL;
			return result;
		}
		err = EVP_CipherInit_ex((EVP_CIPHER_CTX*)mBCd, cipher, NULL, key.mLength ? key.mBytes : NULL, iv.mLength ? iv.mBytes : NULL, 0);
		if (err <= 0)
		{
			if (mBCe) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCe);
			mBCe = NULL;
			if (mBCd) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mBCd);
			mBCd = NULL;
			return result;
		}

		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();

		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("bc")));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("cipher"), ciphername));
		key.mToken = GrapaTokenType::RAW;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("key"), key));
		iv.mToken = GrapaTokenType::RAW;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("iv"), iv));

		return result;
	}
	else if (((GrapaCHAR*)&pMethod)->StrLowerCmp("md") == 0)
	{
		GrapaCHAR digestname("sha512");
		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("digest", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) digestname.FROM(x->mValue);
		}
		//const EVP_MD* md = EVP_sha512();
		EVP_MD* md = (EVP_MD*) EVP_get_digestbyname((char*)digestname.mBytes);
		if (md == NULL) return result;
		//if (digestname.StrLowerCmp("sha512") == 0) md = EVP_sha512();
		mMD = EVP_MD_CTX_new();
		int err = EVP_DigestInit_ex((EVP_MD_CTX*)mMD, md, NULL);
		if (err <= 0)
		{
			if (mMD) EVP_MD_CTX_free((EVP_MD_CTX*)mMD);
			mMD = NULL;
			return result;
		}

		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();

		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("md")));
		digestname.ToLower();
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("digest"), digestname));

		return result;
	}
	else if (((GrapaCHAR*)&pMethod)->StrLowerCmp("rpk") == 0)
	{
		GrapaCHAR algorithmname("ED25519");
		GrapaCHAR pub, prv;
		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("alg", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) algorithmname.FROM(x->mValue);
			x = pParams->vQueue->Search("pub", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) pub.FROM(x->mValue);
			x = pParams->vQueue->Search("prv", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) prv.FROM(x->mValue);
		}

		if (prv.mLength)
		{
			int algorithm = NID_ED25519;
			if (algorithmname.StrLowerCmp("ED25519") == 0) algorithm = EVP_PKEY_ED25519;
			else if (algorithmname.StrLowerCmp("X25519") == 0) algorithm = EVP_PKEY_X25519;
			else if (algorithmname.StrLowerCmp("ED448") == 0) algorithm = EVP_PKEY_ED448;
			else if (algorithmname.StrLowerCmp("X448") == 0) algorithm = EVP_PKEY_X448;
			else if (algorithmname.StrLowerCmp("HMAC") == 0) algorithm = EVP_PKEY_HMAC;
			else if (algorithmname.StrLowerCmp("POLY1305") == 0) algorithm = EVP_PKEY_POLY1305;
			else if (algorithmname.StrLowerCmp("SIPHASH") == 0) algorithm = EVP_PKEY_SIPHASH;
			else return result;
			mRPK = EVP_PKEY_new_raw_private_key(algorithm, NULL, prv.mBytes, prv.mLength);
		}
		else if (pub.mLength)
		{
			int algorithm = NID_ED25519;
			if (algorithmname.StrLowerCmp("ED25519") == 0) algorithm = EVP_PKEY_ED25519;
			else if (algorithmname.StrLowerCmp("X25519") == 0) algorithm = EVP_PKEY_X25519;
			else if (algorithmname.StrLowerCmp("ED448") == 0) algorithm = EVP_PKEY_ED448;
			else if (algorithmname.StrLowerCmp("X448") == 0) algorithm = EVP_PKEY_X448;
			else return result;
			mRPK = EVP_PKEY_new_raw_public_key(algorithm, NULL, pub.mBytes, pub.mLength);
		}
		else
		{
			int algorithm = EVP_PKEY_ED25519;
			if (algorithmname.StrLowerCmp("ED25519") == 0) algorithm = EVP_PKEY_ED25519;
			else if (algorithmname.StrLowerCmp("X25519") == 0) algorithm = EVP_PKEY_X25519;
			else if (algorithmname.StrLowerCmp("ED448") == 0) algorithm = EVP_PKEY_ED448;
			else if (algorithmname.StrLowerCmp("X448") == 0) algorithm = EVP_PKEY_X448;
			else if (algorithmname.StrLowerCmp("HMAC") == 0) algorithm = EVP_PKEY_HMAC;
			else if (algorithmname.StrLowerCmp("POLY1305") == 0) algorithm = EVP_PKEY_POLY1305;
			else if (algorithmname.StrLowerCmp("SIPHASH") == 0) algorithm = EVP_PKEY_SIPHASH;
			else return result;
			EVP_PKEY_CTX* pkeyctx = EVP_PKEY_CTX_new_id(algorithm, NULL);
			if (pkeyctx == NULL) return result;
			int err = EVP_PKEY_keygen_init(pkeyctx);
			if (err <= 0)
			{
				if (pkeyctx) EVP_PKEY_CTX_free(pkeyctx);
				pkeyctx = NULL;
				return result;
			}
			EVP_PKEY* pkey = NULL;
			err = EVP_PKEY_keygen(pkeyctx, &pkey);
			if (err <= 0)
			{
				if (pkeyctx) EVP_PKEY_CTX_free(pkeyctx);
				pkeyctx = NULL;
				return result;
			}
			if (pkeyctx) EVP_PKEY_CTX_free(pkeyctx);
			pkeyctx = NULL;
			mRPK = pkey;
		}

		size_t len = 0;
		prv.SetLength(len);
		err = EVP_PKEY_get_raw_private_key((EVP_PKEY*)mRPK, NULL, &len);
		if (err == 1 && len)
		{
			prv.SetLength(len);
			err = EVP_PKEY_get_raw_private_key((EVP_PKEY*)mRPK, prv.mBytes, &len);
			if (err != 1)
				prv.SetLength(0);
			else
				prv.mToken = GrapaTokenType::RAW;
		}

		len = 0;
		pub.SetLength(len);
		err = EVP_PKEY_get_raw_public_key((EVP_PKEY*)mRPK, NULL, &len);
		if (err == 1 && len)
		{
			pub.SetLength(len);
			err = EVP_PKEY_get_raw_public_key((EVP_PKEY*)mRPK, pub.mBytes, &len);
			if (err != 1)
				pub.SetLength(0);
			else
				pub.mToken = GrapaTokenType::RAW;
		}

		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();

		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("rpk")));
		algorithmname.ToLower();
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("alg"), algorithmname));
		if (pub.mLength) result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("pub"), pub));
		if (prv.mLength) result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("prv"), prv));

		return result;
	}
	return result;
}

// https://web.archive.org/web/20170810025803/http://grouper.ieee.org/groups/1363/P1363a/contributions/pss-submission.pdf
// https://www.thedigitalcatonline.com/blog/2018/04/25/rsa-keys/

bool GrapaEncode::Encode(const GrapaBYTE& pData, GrapaBYTE& pEnc, const GrapaInt& pPublicE)
{
	pEnc.SetLength(0);
	if (pData.mLength == 0 || pData.mBytes == NULL) return false;
	if (mRSA)
	{
		EVP_PKEY* key = EVP_PKEY_new();
		//int err = EVP_PKEY_assign_RSA(key, mRSA);
		int err = EVP_PKEY_set1_RSA(key, (RSA*)mRSA);
		if (err <= 0) return false;
		EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(key, ENGINE_get_default_RSA());
		if (ctx == NULL)
		{
			EVP_PKEY_free(key);
			return false;
		}
		err = EVP_PKEY_encrypt_init(ctx);
		if (err >= 1)
			err = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_PKEY_encrypt(ctx, NULL, &outlen, pData.mBytes, pData.mLength);
		if (err >= 1 && outlen)
		{
			pEnc.SetLength(outlen);
			pEnc.mToken = GrapaTokenType::RAW;
			err = EVP_PKEY_encrypt(ctx, pEnc.mBytes, &outlen, pData.mBytes, pData.mLength);
			if (err >= 1)
				pEnc.SetLength(outlen);
			else
				pEnc.SetLength(0);
		}
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(key);
		return true;
	}
	else if (mRPK)
	{
		EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new((EVP_PKEY*)mRPK, NULL);
		if (ctx == NULL) return false;
		int err = EVP_PKEY_encrypt_init(ctx);
		if (err >= 1)
			err = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_PKEY_encrypt(ctx, NULL, &outlen, pData.mBytes, pData.mLength);
		if (err >= 1 && outlen)
		{
			pEnc.SetLength(outlen);
			pEnc.mToken = GrapaTokenType::RAW;
			err = EVP_PKEY_encrypt(ctx, pEnc.mBytes, &outlen, pData.mBytes, pData.mLength);
			if (err >= 1)
				pEnc.SetLength(outlen);
			else
				pEnc.SetLength(0);
		}
		EVP_PKEY_CTX_free(ctx);
		return true;
	}
	else if (mBCe)
	{
		//u8 pad = 16;
		//if (pData.mLength % 16)
		//	pad = 16 - (pData.mLength % 16);
		//u8 buffer[16];
		//memset(buffer, pad, pad);
		//pData.Append(buffer, pad);
		//AES_CBC_encrypt_buffer((AES_ctx*)vKey, pData.mBytes, (u32)pData.mLength);
		//pData.mToken = GrapaTokenType::RAW;

		int err;
		u64 pos = 0;
		u8 out[16];
		int outl = 0;
		GrapaBYTE outs;
		pEnc.SetSize(pData.mLength + 1);
		pEnc.SetLength(0);
		while (pos < pData.mLength)
		{
			int inl = 16;
			outl = 0;
			if ((pData.mLength - pos) < inl) inl = pData.mLength - pos;
			int err = EVP_CipherUpdate((EVP_CIPHER_CTX*)mBCe, out, &outl, &pData.mBytes[pos], inl);
			if (err != 1)
			{
				pEnc.SetLength(0);
				return false;
			}
			pEnc.Append(out, outl);
			pos += inl;
		}
		outl = 0;
		err = EVP_CipherFinal_ex((EVP_CIPHER_CTX*)mBCe, out, &outl);
		if (err != 1)
		{
			pEnc.SetLength(0);
			return false;
		}
		pEnc.Append(out, outl);
		pEnc.mToken = GrapaTokenType::RAW;
		return true;
	}
	else if (mMD)
	{
		int err;
		err = EVP_DigestUpdate((EVP_MD_CTX*)mMD, pData.mBytes, pData.mLength);
		if (err <= 0)
		{
			pEnc.SetLength(0);
			return false;
		}
		pEnc.SetLength(EVP_MAX_MD_SIZE);
		unsigned int len = EVP_MAX_MD_SIZE;
		err = EVP_DigestFinal_ex((EVP_MD_CTX*)mMD, pEnc.mBytes, &len);
		if (err <= 0)
		{
			pEnc.SetLength(0);
			return false;
		}
		pEnc.SetLength(len);
		pEnc.mToken = GrapaTokenType::RAW;
		return true;
	}
	//GrapaBYTE dataIn(pData);
	//pEnc.SetLength(0);
	//if (m_n.IsZero()) return(false);
	//if (m_e.IsZero()) return(false);
	//u64 inputLen = pData.mLength - pData.mPos;
	//if (inputLen > mDataSize) inputLen = mDataSize;
	//dataIn.SetLength(inputLen);
	//u32 pad = (u32)(20 + (mDataSize - dataIn.mLength));
	//GrapaInt padBits;
	//padBits.Random(((u64)pad) * 8);
	//dataIn.Append((void*)padBits.mBytes, pad);
	//dataIn.SetLength(dataIn.mLength + 4, true);
	//((u8*)dataIn.mBytes)[dataIn.mLength - 4] = 0; //opt;
	//((u8*)dataIn.mBytes)[dataIn.mLength - 3] = 1;
	//((u8*)dataIn.mBytes)[dataIn.mLength - 2] = (u8)(pad >> 8);
	//((u8*)dataIn.mBytes)[dataIn.mLength - 1] = (u8)(pad & 0xFF);
	//GrapaInt data(dataIn);
	//GrapaInt encrypted = data.modPow(((GrapaInt*)&pPublicE)->IsZero() ? m_e : pPublicE, m_n);
	//pEnc = encrypted.getBytes(true);
	//dataIn.SetLength(inputLen);
	//pEnc.mToken = GrapaTokenType::RAW;
	return false;
}

bool GrapaEncode::Decode(const GrapaBYTE& pEnc, GrapaBYTE& pData, const GrapaInt& pPrivateD)
{
	pData.SetLength(0);
	if (pEnc.mLength == 0 || pEnc.mBytes == NULL) return false;
	if (mRSA)
	{
		EVP_PKEY* key = EVP_PKEY_new();
		//int err = EVP_PKEY_assign_RSA(key, mRSA);
		int err = EVP_PKEY_set1_RSA(key, (RSA*)mRSA);
		if (err <= 0) return false;
		EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(key, ENGINE_get_default_RSA());
		if (ctx == NULL)
		{
			EVP_PKEY_free(key);
			return false;
		}
		err = EVP_PKEY_decrypt_init(ctx);
		if (err >= 1)
			err = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_PKEY_decrypt(ctx, NULL, &outlen, pEnc.mBytes, pEnc.mLength);
		if (err >= 1 && outlen)
		{
			pData.SetLength(outlen);
			pData.mToken = GrapaTokenType::RAW;
			err = EVP_PKEY_decrypt(ctx, pData.mBytes, &outlen, pEnc.mBytes, pEnc.mLength);
			if (err >= 1)
				pData.SetLength(outlen);
			else
				pData.SetLength(0);
		}
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(key);
		return true;
	}
	else if (mRPK)
	{
		EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new((EVP_PKEY*)mRPK, NULL);
		if (ctx == NULL) return false;
		int err = EVP_PKEY_decrypt_init(ctx);
		if (err >= 1)
			err = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_PKEY_decrypt(ctx, NULL, &outlen, pEnc.mBytes, pEnc.mLength);
		if (err >= 1 && outlen)
		{
			pData.SetLength(outlen);
			pData.mToken = GrapaTokenType::RAW;
			err = EVP_PKEY_decrypt(ctx, pData.mBytes, &outlen, pEnc.mBytes, pEnc.mLength);
			if (err >= 1)
				pData.SetLength(outlen);
			else
				pData.SetLength(0);
		}
		EVP_PKEY_CTX_free(ctx);
		return true;
	}
	else if (mBCd)
	{
		int err;
		u64 pos = 0;
		u8 out[16];
		int outl = 0;
		GrapaBYTE outs;
		pData.SetSize(pEnc.mLength + 1);
		pData.SetLength(0);
		while (pos < pEnc.mLength)
		{
			int inl = 16;
			outl = 0;
			if ((pEnc.mLength - pos) < inl) inl = pEnc.mLength - pos;
			err = EVP_CipherUpdate((EVP_CIPHER_CTX*)mBCd, out, &outl, &pEnc.mBytes[pos], inl);
			if (err != 1)
			{
				pData.SetLength(0);
				return false;
			}
			pData.Append(out, outl);
			pos += inl;
		}
		outl = 0;
		err = EVP_CipherFinal_ex((EVP_CIPHER_CTX*)mBCd, out, &outl);
		if (err != 1)
		{
			pData.SetLength(0);
			return false;
		}
		pData.Append(out, outl);
		pData.mToken = GrapaTokenType::RAW;
		return true;
	}

	//pData.SetLength(0);
	//if (m_d.IsZero()) return(false);
	//if (pEnc.mLength != mKeySize) return(false);
	//GrapaInt enc; // (from);
	//enc.FromBytes(pEnc);
	//GrapaInt decrypted;
	//if (m_iqmp.IsZero() || !((GrapaInt*)&pPrivateD)->IsZero())
	//{
	//	decrypted = enc.modPow(((GrapaInt*)&pPrivateD)->IsZero() ? m_d : pPrivateD, m_n);
	//}
	//else {
	//	GrapaInt m1 = enc.modPow(m_dmp1, m_p);
	//	GrapaInt m2 = enc.modPow(m_dmq1, m_q);
	//	if (m1 + m_p < m2)
	//	{
	//		pData.SetLength(0);
	//		return(false);
	//	}
	//	GrapaInt h = (m1 >= m2) ? (m_iqmp * (m1 - m2)) % m_p : (m1 + m_p >= m2) ? (m_iqmp * (m1 + m_p - m2)) % m_p : 0;
	//	decrypted = m2 + h * m_q;
	//}
	//pData = decrypted.getBytes(true);
	//u8 opt = ((u8*)pData.mBytes)[pData.mLength - 4];
	//u8 ver = ((u8*)pData.mBytes)[pData.mLength - 3];
	//u32 pad = (((u8*)pData.mBytes)[pData.mLength - 2] << 8) | ((u8*)pData.mBytes)[pData.mLength - 1];
	//if (pad >= mEncSize)
	//{
	//	pData.SetLength(0);
	//	return(false);
	//}
	//pData.SetLength(pData.mLength - pad - 4);
	//pData.mToken = GrapaTokenType::RAW;
	return false;
}

// Sign(pAccount, pExpire, pSerial, pData, pSignature)
// Sends pExpire,pPublicN,pSignature to pAccount
// A portion of the PK data would have pSerial...used to verify on the backend

// Hash the account+serial, send the hash+publickey to the backend (dont' send account/serial).
// Use web browser to log into account. Add instance by entering the serial number.
// Backend hashes the account+serial, looks up the hash, and then binds the instance+publickey.
// Backend now trusts that the publickey belongs to the account+serial.

// Backend then generates a UUID and hashes account+serial+UUID, send hash+publickey to client
// , PK encrypted, (PK encrypted = PK with AES key, then payload.
// Client verifies account+serial, and stores UUID.
// Client now trusts that the publickkey belongs to the server, and has it's UUID.

// Establishing trust with another client?
// Backend knows: account, serial, UUID, publickey.
// Connect to a target (send UUID) and request the target UUID.
// Ask the server for the publickey for that UUID.
// PK encrypt using that publickey (don't trust what the target tells you).

// On the other end (listener)
// Use the UUID to lookup the publickey from the server.
// PK encrypt using that publickey.

// Support pre-fetching a list of publickey values from the server
// from a list of UUID values.
// When setting up a network of instances, send the list of UUIDs
// to all instances so that they can independantly do the lookups.

// Need a way to replace a current publickey if for some reason trust is broken.
// Or maybe have them regenerated on a regular basis.
// Maybe regenererate on startup. 
// The publickey is only set on the server if the instance is active.

// Active = client has a connection with the server.
// Or connection within some configurable tollerance (up to a max value).
// After becoming deactive, the publickey needs to be reset.
// Force resetting the public key after some period of time.

// Each instance has a UUID. The serial is a short hash of the UUID.
// Account+Serial=UUID. Devices can connect to a UUID, or Account+Serial.

// Client sends "capabilities" to backend, and updates state on a regular basis.
// Backend manages workload queue's for client, and notifies client when work is available.
// Client can post work for other clients.

// Establishing trust accross accounts?
// Need a way to establish "rights".
// When access is requested, grant rights. 

// How does a client process general workloads from potentially untrusted clients?
// The libraries need to be security controlled.

bool GrapaEncode::Sign(GrapaRuleEvent* pData, GrapaBYTE& pSignature, GrapaRuleEvent* pParams)
{
	if (mRSA)
	{
		switch (pData->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			break;
		default:
			return false;
		}
		EVP_PKEY* key = EVP_PKEY_new();
		int err = EVP_PKEY_set1_RSA(key, (RSA*)mRSA);
		if (err <= 0) return false;
		EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(key, ENGINE_get_default_RSA());
		if (ctx == NULL)
		{
			EVP_PKEY_free(key);
			return false;
		}
		err = EVP_PKEY_sign_init(ctx);
		if (err >= 1)
			err = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
		//if (err >= 1)
		//	err = EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256());
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_PKEY_sign(ctx, NULL, &outlen, pData->mValue.mBytes, pData->mValue.mLength);
		if (err >= 1 && outlen)
		{
			pSignature.SetLength(outlen);
			pSignature.mToken = GrapaTokenType::RAW;
			err = EVP_PKEY_sign(ctx, pSignature.mBytes, &outlen, pData->mValue.mBytes, pData->mValue.mLength);
			if (err >= 1)
				pSignature.SetLength(outlen);
			else
				pSignature.SetLength(0);
		}
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(key);
		return(err >= 1);
	}
	else if (mRPK)
	{
		switch (pData->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			break;
		default:
			return false;
		}
		int err;
		EVP_MD_CTX* ctxm = EVP_MD_CTX_new();
		err = EVP_DigestSignInit(ctxm, NULL, NULL, NULL, (EVP_PKEY*)mRPK);
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_DigestSign(ctxm, NULL, &outlen, pData->mValue.mBytes, pData->mValue.mLength);
		if (err >= 1)
			pSignature.SetLength(outlen);
		if (err >= 1)
			err = EVP_DigestSign(ctxm, pSignature.mBytes, &outlen, pData->mValue.mBytes, pData->mValue.mLength);
		if (err >= 1)
			pSignature.SetLength(outlen);
		else
			pSignature.SetLength(0);
		EVP_MD_CTX_free(ctxm);
		return(err >= 1);
	}
	else if (mEC)
	{
		switch (pData->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			break;
		default:
			return false;
		}
		int err;
		EVP_PKEY_CTX* ctx1 = (EVP_PKEY_CTX*)mEC;
		err = EVP_PKEY_sign_init(ctx1);
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_PKEY_sign(ctx1, NULL, &outlen, pData->mValue.mBytes, pData->mValue.mLength);
		if (err >= 1 && outlen)
		{
			pSignature.SetLength(outlen);
			pSignature.mToken = GrapaTokenType::RAW;
			err = EVP_PKEY_sign(ctx1, pSignature.mBytes, &outlen, pData->mValue.mBytes, pData->mValue.mLength);
			if (err >= 1)
				pSignature.SetLength(outlen);
			else
				pSignature.SetLength(0);
		}
		return(err >= 1);
	}

	//GrapaBYTE hash;
	//GrapaHash::SHAKE256(pData, hash, mDataSize);
	//return Encode(hash, pSignature, m_d);
	return(false);
}

bool GrapaEncode::SignAdd(GrapaRuleEvent* pData, GrapaBYTE& pSignature, GrapaRuleEvent* pParams)
{
	return(false);
}

// Verify(pAccount, pExpire, pData, pSignature, pPublicN)
// Send pAccount, pExpire, pSignature, pPublicN
bool GrapaEncode::Verify(GrapaRuleEvent* pData, const GrapaBYTE& pSignature, GrapaRuleEvent* pParams)
{
	bool match = false;
	if (mRSA)
	{
		switch (pData->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			break;
		default:
			return false;
		}
		EVP_PKEY* key = EVP_PKEY_new();
		int err = EVP_PKEY_set1_RSA(key, (RSA*)mRSA);
		if (err <= 0) return match;
		EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(key, ENGINE_get_default_RSA());
		if (ctx == NULL)
		{
			EVP_PKEY_free(key);
			return match;
		}
		err = EVP_PKEY_verify_init(ctx);
		if (err >= 1)
			err = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
		//if (err >= 1)
		//	err = EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256());
		if (err >= 1)
		{
			err = EVP_PKEY_verify(ctx, pSignature.mBytes, pSignature.mLength, pData->mValue.mBytes, pData->mValue.mLength);
			match = err == 1;
		}
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(key);
		return(match);
	}
	else if (mRPK)
	{
		switch (pData->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			break;
		default:
			return false;
		}
		int err;
		EVP_MD_CTX* ctxm = EVP_MD_CTX_new();
		err = EVP_DigestVerifyInit(ctxm, NULL, NULL, NULL, (EVP_PKEY*)mRPK);
		if (err >= 1)
		{
			err = EVP_DigestVerify(ctxm, pSignature.mBytes, pSignature.mLength, pData->mValue.mBytes, pData->mValue.mLength);
			match = err == 1;
		}
		EVP_MD_CTX_free(ctxm);
		return(match);
	}
	else if (mEC)
	{
		switch (pData->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			break;
		default:
			return false;
		}
		int err;
		EVP_PKEY_CTX* ctx1 = (EVP_PKEY_CTX*)mEC;
		err = EVP_PKEY_verify_init(ctx1);
		if (err >= 1)
		{
			err = EVP_PKEY_verify(ctx1, pSignature.mBytes, pSignature.mLength, pData->mValue.mBytes, pData->mValue.mLength);
			match = err == 1;
		}
		return(err >= 1);
	}

	//GrapaBYTE hash,sig;
	//GrapaCHAR input;
	//GrapaHash::SHAKE256(pData, hash, mDataSize);
	//if (Decode(pSignature, input, m_e))
	//if (strncmp((char*)input.mBytes, (char*)hash.mBytes, (size_t)input.mLength) == 0)
	//	match = true;
	return match;
}

bool GrapaEncode::VerifyRecover(GrapaBYTE& pData, const GrapaBYTE& pSignature)
{
	if (mRSA)
	{
		EVP_PKEY* key = EVP_PKEY_new();
		int err = EVP_PKEY_set1_RSA(key, (RSA*)mRSA);
		ENGINE* eng = ENGINE_get_default_RSA();
		EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(key, eng);
		if (ctx == NULL)
		{
			EVP_PKEY_free(key);
			return false;
		}
		err = EVP_PKEY_verify_recover_init(ctx);
		if (err >= 1)
			err = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
		//if (err >= 1)
		//	err = EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256());
		size_t outlen = 0;
		if (err >= 1)
			err = EVP_PKEY_verify_recover(ctx, NULL, &outlen, pSignature.mBytes, pSignature.mLength);
		if (err >= 1)
		{
			pData.SetLength(outlen);
			pData.mToken = GrapaTokenType::RAW;
			err = EVP_PKEY_verify_recover(ctx, pData.mBytes, &outlen, pSignature.mBytes, pSignature.mLength);
			if (err >= 1)
				pData.SetLength(outlen);
			else
				pData.SetLength(0);
		}
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(key);
		return(err >= 1);
	}
	return false;
}

bool GrapaEncode::Secret(GrapaRuleEvent* pKey, GrapaBYTE& pSecret)
{
	pSecret.SetLength(0);
	pSecret.mToken = GrapaTokenType::INT;
	if (mDH)
	{
		int err = 0;
		s64 idx;
		GrapaObjectEvent* x = pKey->vQueue->SearchLower("METHOD", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x == NULL) return false;
		if (x->mValue.StrLowerCmp("dh") != 0) return false;

		BIGNUM* g, * pub;
		g = pub = NULL;

		x = pKey->vQueue->Search("g", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) g = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		x = pKey->vQueue->Search("pub", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) pub = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		GrapaBYTE gc;
		gc.SetLength(DH_size((DH*)mDH));
		err = DH_compute_key(gc.mBytes, pub, (DH*)mDH);
		BN_free(g);
		BN_free(pub);
		if (err >= 1)
		{
			GrapaInt g;
			gc.SetLength(err);
			g.FromBytes(gc);
			pSecret = g.getBytes();
			return true;
		}

		return false;
	}
	else if (mEC)
	{
		int err = 0;
		EVP_PKEY_CTX* ctx1 = (EVP_PKEY_CTX*)mEC;

		s64 idx;

		GrapaObjectEvent* x = pKey->vQueue->SearchLower("METHOD", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x == NULL) return false;
		if (x->mValue.StrLowerCmp("ec") != 0) return false;

		BIGNUM* pub1 = NULL;;
		int nid = 0;

		x = pKey->vQueue->Search("curve", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x == NULL) return false;

		if (x->mValue.StrLowerCmp("secp224r1") == 0)
			nid = NID_secp224r1;
		else if (x->mValue.StrLowerCmp("prime256v1") == 0)
			nid = NID_X9_62_prime256v1;
		else
			return false;
		x = pKey->vQueue->Search("pub", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x)
		{
			EC_KEY* keyx = EC_KEY_new_by_curve_name(nid);
			pub1 = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);
			EC_POINT* pubx = EC_POINT_bn2point(EC_KEY_get0_group(keyx), pub1, NULL, NULL);
			err = EC_KEY_set_public_key(keyx, pubx);
			EVP_PKEY* ekeyx = EVP_PKEY_new();
			err = EVP_PKEY_set1_EC_KEY(ekeyx, keyx);
			err = EVP_PKEY_derive_init(ctx1);
			if (err >= 1)
				err = EVP_PKEY_derive_set_peer(ctx1, ekeyx);
			size_t keylen = 0;
			if (err >= 1)
				err = EVP_PKEY_derive(ctx1, NULL, &keylen);
			if (err >= 1)
			{
				GrapaInt g;
				GrapaBYTE gc;
				gc.SetLength(keylen);
				err = EVP_PKEY_derive(ctx1, gc.mBytes, &keylen);
				if (err >= 1)
				{
					gc.SetLength(keylen);
					g.FromBytes(gc);
					pSecret = g.getBytes();
				}
			}
			EVP_PKEY_free(ekeyx);
			EC_POINT_free(pubx);
			EC_KEY_free(keyx);
		}
		return(err >= 1);
	}
	else if (mRPK)
	{
		GrapaEncode pk;
		pk.FROM(pKey);
		if (pk.mRPK == NULL) return false;
		EVP_PKEY_CTX* ctx1 = EVP_PKEY_CTX_new((EVP_PKEY*)mRPK, NULL);
		if (ctx1 == NULL) return false;
		int err = EVP_PKEY_derive_init(ctx1);
		if (err >= 1)
			err = EVP_PKEY_derive_set_peer(ctx1, (EVP_PKEY*)pk.mRPK);
		size_t keylen = 0;
		if (err >= 1)
			err = EVP_PKEY_derive(ctx1, NULL, &keylen);
		if (err >= 1)
		{
			GrapaInt g;
			GrapaBYTE gc;
			gc.SetLength(keylen);
			err = EVP_PKEY_derive(ctx1, gc.mBytes, &keylen);
			if (err >= 1)
			{
				gc.SetLength(keylen);
				g.FromBytes(gc);
				pSecret = g.getBytes();
			}
		}
		EVP_PKEY_CTX_free(ctx1);
		return(err >= 1);
	}
	return(false);
}
