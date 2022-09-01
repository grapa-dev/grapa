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
#pragma comment(lib, "blst-lib/win/blst.lib")
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

#include <blst/blst.hpp>


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
	mPFC = NULL;
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


void _hash_digest(void* cb, const char* name)
{
	if (strcmp(name, "sha256") == 0)
	{
		((blst::DIGEST_CB*)cb)->digest = (void*)EVP_get_digestbyname("sha256");
		((blst::DIGEST_CB*)cb)->r_in_bytes = 64;
		((blst::DIGEST_CB*)cb)->b_in_bytes = 32;
	}
	else if (strcmp(name, "sha512") == 0)
	{
		((blst::DIGEST_CB*)cb)->digest = (void*)EVP_get_digestbyname("sha512");
		((blst::DIGEST_CB*)cb)->r_in_bytes = 128;
		((blst::DIGEST_CB*)cb)->b_in_bytes = 64;
	}
	else if (strcmp(name, "blake2b") == 0 || strcmp(name, "blake2b512") == 0)
	{
		((blst::DIGEST_CB*)cb)->digest = (void*)EVP_get_digestbyname("blake2b512");
		((blst::DIGEST_CB*)cb)->r_in_bytes = 128;
		((blst::DIGEST_CB*)cb)->b_in_bytes = 64;
	}
}
void _hash_size(void* cb, size_t* r_in_bytes, size_t* b_in_bytes)
{
	if (r_in_bytes) *r_in_bytes = ((blst::DIGEST_CB*)cb)->r_in_bytes;
	if (b_in_bytes) *b_in_bytes = ((blst::DIGEST_CB*)cb)->b_in_bytes;
}
void _hash_init(void* cb)
{
	if (((blst::DIGEST_CB*)cb)->digest == NULL) return;
	int err = EVP_DigestInit_ex((EVP_MD_CTX*)((blst::DIGEST_CB*)cb)->ctx, (EVP_MD*)((blst::DIGEST_CB*)cb)->digest, NULL);
}
void _hash_update(void* cb, const void* _inp, size_t len)
{
	int err = EVP_DigestUpdate((EVP_MD_CTX*)((blst::DIGEST_CB*)cb)->ctx, _inp, len);
}
void _hash_final(void* cb, unsigned char* md, size_t size, size_t* outlen)
{
	unsigned int len2 = EVP_MAX_MD_SIZE;
	unsigned char buf[EVP_MAX_MD_SIZE];
	int err = EVP_DigestFinal_ex((EVP_MD_CTX*)((blst::DIGEST_CB*)cb)->ctx, buf, &len2);
	len2 = size > len2 ? len2 : size;
	if (outlen) *outlen = len2;
	memcpy(md, buf, len2);
}

static inline void vec_zero(void* ret, size_t num)
{
	volatile blst::limb_t* rp = (volatile blst::limb_t*)ret;
	size_t i;

	num /= sizeof(blst::limb_t);

	for (i = 0; i < num; i++)
		rp[i] = 0;

#if defined(__GNUC__) && !defined(__NVCC__)
	asm volatile("" : : "r"(ret) : "memory");
#endif
}
static inline void vec_copy(void* __restrict ret, const void* a, size_t num)
{
	blst::limb_t* rp = (blst::limb_t*)ret;
	const blst::limb_t* ap = (const blst::limb_t*)a;
	size_t i;

	num /= sizeof(blst::limb_t);

	for (i = 0; i < num; i++)
		rp[i] = ap[i];
}
static void vec_xor(void* __restrict ret, const void* a,
	const void* b, size_t num)
{
	blst::limb_t* rp = (blst::limb_t*)ret;
	const blst::limb_t* ap = (const blst::limb_t*)a;
	const blst::limb_t* bp = (const blst::limb_t*)b;
	size_t i;

	num /= sizeof(blst::limb_t);

	for (i = 0; i < num; i++)
		rp[i] = ap[i] ^ bp[i];
}

static void expand_message_xmd_local(unsigned char* bytes, size_t len_in_bytes,
	const unsigned char* aug, size_t aug_len,
	const unsigned char* msg, size_t msg_len,
	const unsigned char* DST, size_t DST_len,
	blst::DIGEST_CB* cb)
{
	size_t r_in_bytes;  // r_in_bytes = input size of hash in bytes
	size_t b_in_bytes;  // b_in_bytes = output size of hash in bytes
	cb->d_size(cb, &r_in_bytes, &b_in_bytes); // get r_in_bytes and b_in_bytes from hash

	unsigned char Z_pad[512]; // max input size of hash (appears to be 256, reserving double)

	unsigned char md[128];   // max output size of hash (appears to be 64, reserving double)
	unsigned char mdXor[128];
	unsigned char DSTc[128];

	size_t outlen = 0;

	if (DST_len > 255)
	{
		cb->d_init(cb);
		cb->d_update(cb, "H2C-OVERSIZE-DST-", 17);
		cb->d_update(cb, DST, DST_len);
		cb->d_final(cb, (unsigned char*)DSTc, b_in_bytes, &outlen);
		DST = DSTc, DST_len = outlen;
	}

	const size_t ell = len_in_bytes / b_in_bytes;
	if (ell > 255) return;  // as per spec...error condition...abort

	unsigned char iBuf = 0;
	cb->d_init(cb);
	memset(Z_pad, 0, r_in_bytes);
	cb->d_update(cb, Z_pad, r_in_bytes);
	cb->d_update(cb, msg, msg_len);
	unsigned char lenBuf[2];
	lenBuf[0] = (unsigned char)(len_in_bytes >> 8);
	lenBuf[1] = (unsigned char)(len_in_bytes);
	cb->d_update(cb, lenBuf, sizeof(lenBuf));
	cb->d_update(cb, &iBuf, 1);
	cb->d_update(cb, DST, DST_len);
	iBuf = (unsigned char)DST_len;
	cb->d_update(cb, &iBuf, 1);
	cb->d_final(cb, md, b_in_bytes, &outlen);
	cb->d_init(cb);
	cb->d_update(cb, md, b_in_bytes);
	iBuf = 1;
	cb->d_update(cb, &iBuf, 1);
	cb->d_update(cb, DST, DST_len);
	iBuf = (unsigned char)DST_len;
	cb->d_update(cb, &iBuf, 1);
	cb->d_final(cb, bytes, b_in_bytes, &outlen);;
	for (size_t i = 1; i < ell; i++) {
		cb->d_init(cb);
		for (size_t j = 0; j < b_in_bytes; j++)
			mdXor[j] = md[j] ^ bytes[b_in_bytes * (i - 1) + j];
		cb->d_update(cb, mdXor, b_in_bytes);
		iBuf = (unsigned char)(i + 1);
		cb->d_update(cb, &iBuf, 1);
		cb->d_update(cb, DST, DST_len);
		iBuf = (unsigned char)DST_len;
		cb->d_update(cb, &iBuf, 1);
		cb->d_final(cb, bytes + b_in_bytes * i, b_in_bytes, &outlen);
	}
}

class Grapa_bls12_381
{
public:
	bool mSignInG1;
	bool mSignHash;
	GrapaCHAR mAlg;
	blst::blst_scalar mSK;
	std::vector<blst::blst_p1*> mPK1;
	std::vector<blst::blst_p2*> mPK2;
	blst::DIGEST_CB mCB;
	bool mBBS;
	blst::blst_p1 mh0_1;
	std::vector<blst::blst_p1*> mh_1;
	blst::blst_p2 mh0_2;
	std::vector<blst::blst_p2*> mh_2;
	Grapa_bls12_381()
	{
		mSignInG1 = true;
		mSignHash = true;
		memset(&mSK, 0, sizeof(mSK));
		mCB.ctx = (void*)EVP_MD_CTX_create();
		mCB.d_digest = _hash_digest;
		mCB.d_size = _hash_size;
		mCB.d_init = _hash_init;
		mCB.d_update = _hash_update;
		mCB.d_final = _hash_final;
		mCB.d_digest(&mCB, "sha256");
		mBBS = false;
	};
	virtual ~Grapa_bls12_381()
	{
		ClearPub();
		if (mCB.ctx) EVP_MD_CTX_destroy((EVP_MD_CTX*)mCB.ctx);
		mCB.ctx = NULL;
	};
	virtual bool SetAlg(const GrapaCHAR& pAlg)
	{
		mAlg.FROM(pAlg);
		if (mAlg.StrLowerCmp("BLS12381G1_XMD:SHA-256_SSWU_RO_") == 0)
		{
			mSignInG1 = true;
			mSignHash = true;
			mCB.d_digest(&mCB, "sha256");
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G2_XMD:SHA-256_SSWU_RO_") == 0)
		{
			mSignInG1 = false;
			mSignHash = true;
			mCB.d_digest(&mCB, "sha256");
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G1_XMD:SHA-256_SSWU_NU_") == 0)
		{
			mSignInG1 = true;
			mSignHash = false;
			mCB.d_digest(&mCB, "sha256");
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G2_XMD:SHA-256_SSWU_NU_") == 0)
		{
			mSignInG1 = false;
			mSignHash = false;
			mCB.d_digest(&mCB, "sha256");
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G1_XMD:BLAKE2B_SSWU_RO_") == 0)
		{
			mSignInG1 = true;
			mSignHash = true;
			mCB.d_digest(&mCB, "blake2b512");
			mBBS = true;
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G2_XMD:BLAKE2B_SSWU_RO_") == 0)
		{
			mSignInG1 = false;
			mSignHash = true;
			mCB.d_digest(&mCB, "blake2b512");
			mBBS = true;
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G1_XMD:BLAKE2B_SSWU_NU_") == 0)
		{
			mSignInG1 = true;
			mSignHash = false;
			mCB.d_digest(&mCB, "blake2b512");
			mBBS = true;
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G2_XMD:BLAKE2B_SSWU_NU_") == 0)
		{
			mSignInG1 = false;
			mSignHash = false;
			mCB.d_digest(&mCB, "blake2b512");
			mBBS = true;
			return true;
		}
		else if (mAlg.StrLowerCmp("BLS12381G1_XOF:SHAKE-256_SSWU_RO_") == 0)
		{
			mSignInG1 = true;
			mSignHash = true;
			mCB.d_digest(&mCB, "shake256");
			mBBS = true;
			return true;
		}
		return false;
	};
	virtual void ClearPub()
	{
		for (int i = 0; i < mPK1.size(); i++)
			delete mPK1[i];
		mPK1.resize(0);
		for (int i = 0; i < mPK2.size(); i++)
			delete mPK2[i];
		mPK2.resize(0);
		ClearH();
	};
	void ClearH()
	{
		for (int i = 0; i < mh_1.size(); i++)
			delete mh_1[i];
		mh_1.resize(0);
		for (int i = 0; i < mh_2.size(); i++)
			delete mh_2[i];
		mh_2.resize(0);
	};
	virtual void KeyGen(const GrapaBYTE& pSecret, const GrapaBYTE& pData, GrapaBYTE& pKey)
	{
		blst::blst_scalar tempKey;
		//GrapaBYTE secret;
		//blst::blst_scalar_from_bendian(&tempKey, pSecret.mBytes);
		//secret.SetLength(pSecret.mLength);
		//blst::blst_lendian_from_scalar(secret.mBytes, &tempKey);
		pKey.SetLength(32);
		blst::blst_keygen((blst::blst_scalar*)pKey.mBytes, pSecret.mBytes, pSecret.mLength, pData.mBytes, pData.mLength);
		pKey.mToken = GrapaTokenType::RAW;
		blst::blst_scalar_from_lendian(&tempKey, pKey.mBytes);
		blst::blst_bendian_from_scalar(pKey.mBytes, &tempKey);
		return;

		GrapaBYTE salt("BLS-SIG-KEYGEN-SALT-");

		blst::DIGEST_CB d;
		size_t dlen;
		d.ctx = (void*)EVP_MD_CTX_create();
		d.d_digest = _hash_digest;
		d.d_size = _hash_size;
		d.d_init = _hash_init;
		d.d_update = _hash_update;
		d.d_final = _hash_final;
		d.d_digest(&d, "sha256");
		d.d_init(&d);
		d.d_update(&d, "BLS-SIG-KEYGEN-SALT-", 20);
		salt.SetLength(64);
		d.d_final(&d, (unsigned char*)salt.mBytes, 64, &dlen);
		salt.SetLength(dlen);
		EVP_MD_CTX_destroy((EVP_MD_CTX*)d.ctx);

		//GrapaBYTE secret;
		//blst::blst_scalar_from_bendian(&tempKey, pSecret.mBytes);
		//secret.SetLength(pSecret.mLength);
		//blst::blst_lendian_from_scalar(secret.mBytes, &tempKey);
		EVP_MD* md = (EVP_MD*)EVP_get_digestbyname((char*)"sha256");
		EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
		pKey.mToken = GrapaTokenType::RAW;
		int err = 0;
		err = EVP_PKEY_derive_init(pctx);
		err = EVP_PKEY_CTX_set_hkdf_md(pctx, md);
		err = EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt.mBytes, salt.mLength);
		err = EVP_PKEY_CTX_set1_hkdf_key(pctx, pSecret.mBytes, pSecret.mLength);
		err = EVP_PKEY_CTX_add1_hkdf_info(pctx, pData.mBytes, pData.mLength);
		size_t outlen = 32;
		pKey.SetLength(outlen);
		err = EVP_PKEY_derive(pctx, pKey.mBytes, &outlen);
		pKey.SetLength(outlen);
		pKey.mToken = GrapaTokenType::RAW;
		EVP_PKEY_CTX_free(pctx);
		blst::blst_scalar_from_lendian(&tempKey, pKey.mBytes);
		blst::blst_bendian_from_scalar(pKey.mBytes, &tempKey);
		//u8 rp[32] = {
		//	0x73, 0xed, 0xa7, 0x53, 0x29, 0x9d, 0x7d, 0x48, 0x33, 0x39, 0xd8, 0x08, 0x09, 0xa1, 0xd8, 0x05, 
		//	0x53, 0xbd, 0xa4, 0x02, 0xff, 0xfe, 0x5b, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01 
		//};
		//GrapaInt r(rp,32);
		////r = 0x73eda753299d7d483339d80809a1d80553bda402fffe5bfeffffffff00000001
		//GrapaInt d;
		//d.FromBytes(pKey, true);
		//d = d % r;
		//pKey = d.getBytes(true);
	};
	virtual bool SetPrv(const GrapaBYTE& pPrv, GrapaBYTE& pPub)
	{
		ClearPub();
		if (pPrv.mLength != 32) return false;
		blst::blst_scalar_from_bendian(&mSK, pPrv.mBytes);
		if (mSignInG1)
		{
			mPK2.push_back(new(blst::blst_p2));
			blst::blst_sk_to_pk_in_g2(mPK2.front(), &mSK);
			pPub.SetLength(96);
			blst::blst_p2_compress(pPub.mBytes, mPK2.front());
		}
		else
		{
			/*
			let sk = gen_sk(seed_data.as_slice());
			let mut pk = G::one();
			pk.mul_assign(sk);
			let mut pk_bytes = Vec::new();
			pk.serialize(&mut pk_bytes, true).unwrap();
			*/
			mPK1.push_back(new(blst::blst_p1));
			//blst::blst_fp12 ret;
			//blst::blst_fp12_mul(&ret, blst::blst_fp12_one(), mSK.b);
			blst::blst_sk_to_pk_in_g1(mPK1.front(), &mSK);
			pPub.SetLength(48);
			blst::blst_p1_compress(pPub.mBytes, mPK1.front());
			//pPub.SetLength(96);
			//blst::blst_p1_serialize(pPub.mBytes, mPK1.front());
		}
		return true;
	};
	virtual bool SetPub(const GrapaBYTE& pPub)
	{
		ClearH();
		blst::BLST_ERROR berr;
		if (pPub.mLength == 96)
		{
			mSignInG1 = true;
			for (int i = 0; i < mPK2.size(); i++)
				delete mPK2[i];
			mPK2.resize(0);
			blst::blst_p2_affine in;
			berr = blst::blst_p2_uncompress(&in, pPub.mBytes);
			if (berr) return false;
			if (!blst::blst_p2_affine_on_curve(&in)) return false;
			mPK2.push_back(new(blst::blst_p2));
			blst::blst_p2_from_affine(mPK2.front(), &in);
			return true;
		}
		else if (pPub.mLength == 48)
		{
			mSignInG1 = false;
			for (int i = 0; i < mPK1.size(); i++)
				delete mPK1[i];
			mPK1.resize(0);
			blst::blst_p1_affine in;
			berr = blst::blst_p1_uncompress(&in, pPub.mBytes);
			if (berr) return false;
			if (!blst::blst_p1_affine_on_curve(&in)) return false;
			mPK1.push_back(new(blst::blst_p1));
			blst::blst_p1_from_affine(mPK1.front(), &in);
			return true;
		}
		return false;
	}
	virtual bool SetH(u32 pCount)
	{
		if (!mBBS) return false;

		GrapaBYTE DST("BBS_BLS12381G1_XOF:SHAKE-256_SSWU_RO_");
		GrapaBYTE SEED("BBS_BLS12381G1_XOF:SHAKE-256_SSWU_RO_MESSAGE_GENERATOR_SEED");

		GrapaBYTE countStr = GrapaInt(pCount).getBytes(true);
		if (countStr.mLength > 4) return(false);

		if (mSignInG1)
		{
			unsigned char buf[192 + 10];
			if (mh_1.size() == 0)
			{
				GrapaBYTE nStr = GrapaInt(0).getBytes(true);
				memset(buf, 0, 192 + 10);
				blst::blst_p2_serialize(buf, mPK2.front());
				buf[192] = 0;
				memcpy(&buf[192 + 1 + 4 - nStr.mLength], nStr.mBytes, nStr.mLength);
				buf[192 + 1 + 4] = 0;
				memcpy(&buf[192 + 1 + 4 + 1 + 4 - countStr.mLength], countStr.mBytes, countStr.mLength);
				if (mSignHash)
					blst::blst_hash_to_g1(&mh0_1, buf, 192 + 10, DST.mBytes, DST.mLength, NULL, 0, NULL);
				else
					blst::blst_encode_to_g1(&mh0_1, buf, 192 + 10, DST.mBytes, DST.mLength, NULL, 0, NULL);
			}
			for (u32 i = mh_1.size(); i < pCount; i++)
			{
				GrapaBYTE nStr = GrapaInt(i + 1).getBytes(true);
				memset(buf, 0, 192 + 10);
				blst::blst_p2_serialize(buf, mPK2.front());
				buf[192] = 0;
				memcpy(&buf[192 + 1 + 4 - nStr.mLength], nStr.mBytes, nStr.mLength);
				buf[192 + 1 + 4] = 0;
				memcpy(&buf[192 + 1 + 4 + 1 + 4 - countStr.mLength], countStr.mBytes, countStr.mLength);
				blst::blst_p1* h = new(blst::blst_p1);
				if (mSignHash)
					blst::blst_hash_to_g1(h, buf, 192 + 10, DST.mBytes, DST.mLength, NULL, 0, NULL);
				else
					blst::blst_encode_to_g1(h, buf, 192 + 10, DST.mBytes, DST.mLength, NULL, 0, NULL);
				mh_1.push_back(h);
			}
		}
		else
		{
			unsigned char buf[96 + 10];
			if (mh_2.size() == 0)
			{
				GrapaBYTE nStr = GrapaInt(0).getBytes(true);
				memset(buf, 0, 96 + 10);
				blst::blst_p1_serialize(buf, mPK1.front());
				buf[96] = 0;
				memcpy(&buf[96 + 1 + 4 - nStr.mLength], nStr.mBytes, nStr.mLength);
				buf[96 + 1 + 4] = 0;
				memcpy(&buf[96 + 1 + 4 + 1 + 4 - countStr.mLength], countStr.mBytes, countStr.mLength);
				if (mSignHash)
					blst::blst_hash_to_g2(&mh0_2, buf, 96 + 10, DST.mBytes, DST.mLength, NULL, 0, &mCB);
				else
					blst::blst_encode_to_g2(&mh0_2, buf, 96 + 10, DST.mBytes, DST.mLength, NULL, 0, &mCB);
			}
			for (u32 i = mh_2.size(); i < pCount; i++)
			{
				GrapaBYTE nStr = GrapaInt(i + 1).getBytes(true);
				memset(buf, 0, 96 + 10);
				blst::blst_p1_serialize(buf, mPK1.front());
				buf[96] = 0;
				memcpy(&buf[96 + 1 + 4 - nStr.mLength], nStr.mBytes, nStr.mLength);
				buf[96 + 1 + 4] = 0;
				memcpy(&buf[96 + 1 + 4 + 1 + 4 - countStr.mLength], countStr.mBytes, countStr.mLength);
				blst::blst_p2* h = new(blst::blst_p2);
				if (mSignHash)
					blst::blst_hash_to_g2(h, buf, 96 + 10, DST.mBytes, DST.mLength, NULL, 0, &mCB);
				else
					blst::blst_encode_to_g2(h, buf, 96 + 10, DST.mBytes, DST.mLength, NULL, 0, &mCB);
				mh_2.push_back(h);
			}
		}

		return true;
	}
	virtual bool AddPub(const GrapaBYTE& pPub)
	{
		blst::BLST_ERROR berr;
		if (pPub.mLength == 48)
		{
			blst::blst_p1* PK1x = new blst::blst_p1();
			blst::blst_p1_affine in;
			berr = blst::blst_p1_uncompress(&in, pPub.mBytes);
			if (!blst::blst_p1_affine_on_curve(&in)) return false;
			blst::blst_p1_from_affine(PK1x, &in);
			mPK1.push_back(PK1x);
			return true;
		}
		else if (pPub.mLength == 96)
		{
			blst::blst_p2* PK2x = new blst::blst_p2();
			blst::blst_p2_affine in;
			berr = blst::blst_p2_uncompress(&in, pPub.mBytes);
			if (!blst::blst_p2_affine_on_curve(&in)) return false;
			blst::blst_p2_from_affine(PK2x, &in);
			mPK2.push_back(PK2x);
			return true;
		}
		return false;
	};
	virtual bool Sign(GrapaRuleEvent* pData, GrapaBYTE& pR, GrapaRuleEvent* pParams)
	{
		if (mBBS)
		{
			GrapaInt rn;
			GrapaInt a, ra, e, s;
			GrapaBYTE buf;
			size_t outlen;

			rn.Random(1024);
			buf = rn.getBytes(true);
			mCB.d_init(&mCB);
			mCB.d_update(&mCB, buf.mBytes, buf.mLength);
			//buf.SetLength(32);
			mCB.d_final(&mCB, buf.mBytes, 32, &outlen);
			buf.SetLength(outlen);
			a.FromBytes(buf,true);
			a.Div(ra, e);
			buf = e.getBytes(true);
			blst::blst_scalar es;
			blst::blst_scalar_from_bendian(&es, buf.mBytes);

			rn.Random(1024);
			buf = rn.getBytes(true);
			mCB.d_init(&mCB);
			mCB.d_update(&mCB, buf.mBytes, buf.mLength);
			//buf.SetLength(32);
			mCB.d_final(&mCB, buf.mBytes, 32, &outlen);
			buf.SetLength(outlen);
			a.FromBytes(buf,true);
			a.Div(ra, s);
			buf = s.getBytes(true);
			blst::blst_scalar ss;
			blst::blst_scalar_from_bendian(&ss, buf.mBytes);

			blst::blst_fr SK;
			blst::blst_fr_from_scalar(&SK, &mSK);

			blst::blst_fr efr;
			blst::blst_fr_from_scalar(&efr, &es);
			blst::blst_fr SKe;
			blst::blst_fr_add(&SKe, &SK, &efr);

			blst::blst_fr InvSKe;
			blst::blst_fr_eucl_inverse(&InvSKe, &SKe);
			blst::blst_scalar InvSKes;
			blst::blst_scalar_from_fr(&InvSKes, &InvSKe);

			GrapaRuleEvent* ep = pData->vQueue ? pData->vQueue->Head() : NULL;
			int mcount = ep ? pData->vQueue->mCount : 1;
			SetH(mcount);

			if (mSignInG1)
			{
				blst::blst_p1 b = *blst::blst_p1_generator();
				blst::blst_p1 out2;
				blst::blst_p1_mult(&out2, &mh0_1, (blst::byte*)ss.b, 256);
				blst::blst_p1_add(&b, &b, &out2);
				for (int i = 0; i < mcount; i++)
				{
					GrapaRuleEvent* e2 = ep;
					if (ep == NULL) e2 = pData;
					else while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
					blst::blst_p1_mult(&out2, mh_1[i], (blst::byte*)e2->mValue.mBytes, e2->mValue.mLength * 8);
					blst::blst_p1_add(&b, &b, &out2);
					if (ep) ep = ep->Next();
				}
				blst::blst_p1 A;
				blst::blst_p1_mult(&A, &b, (blst::byte*)InvSKes.b, 256);
				pR.SetLength(48 + 32 + 32);
				blst::blst_p1_compress(pR.mBytes, &A);
				blst_bendian_from_scalar(&pR.mBytes[48], &es);
				blst_bendian_from_scalar(&pR.mBytes[48 + 32], &ss);

			}
			else
			{
				blst::blst_p2 b = *blst::blst_p2_generator();
				blst::blst_p2 out2;
				blst::blst_p2_mult(&out2, &mh0_2, (blst::byte*)ss.b, 256);
				blst::blst_p2_add(&b, &b, &out2);
				for (int i = 0; i < mcount; i++)
				{
					GrapaRuleEvent* e2 = ep;
					if (ep == NULL) e2 = pData;
					else while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
					blst::blst_p2_mult(&out2, mh_2[i], (blst::byte*)e2->mValue.mBytes, e2->mValue.mLength * 8);
					blst::blst_p2_add(&b, &b, &out2);
					if (ep) ep = ep->Next();
				}
				blst::blst_p2 A;
				blst::blst_p2_mult(&A, &b, (blst::byte*)InvSKes.b, 256);
				pR.SetLength(96 + 32 + 32);
				blst::blst_p2_compress(pR.mBytes, &A);
				blst_bendian_from_scalar(&pR.mBytes[96], &es);
				blst_bendian_from_scalar(&pR.mBytes[96 + 32], &ss);
			}

		}
		else
		{
			blst::BLST_ERROR berr;
			GrapaBYTE DST("BLS_SIG_");
			DST.Append(mAlg);
			GrapaRuleEvent* aug = NULL, * aug2 = NULL;
			bool hasdst = false;
			bool hasaug = false;
			if (pParams && pParams->vQueue)
			{
				GrapaRuleEvent* x;
				s64 index;
				x = pParams->vQueue->Search(GrapaCHAR("dst"), index);
				while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
				if (x && x->mValue.mLength)
				{
					DST.FROM(x->mValue);
					hasdst = true;
				}
				x = pParams->vQueue->Search(GrapaCHAR("aug"), index);
				while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
				if (x && (x->mValue.mLength || (x->vQueue && x->vQueue->mCount)))
				{
					hasaug = true;
					aug = x;
					aug2 = x;
					if (x->mValue.mToken == GrapaTokenType::ARRAY)
					{
						aug = x->vQueue->Head();
						aug2 = aug;
						while (aug2 && aug2->mValue.mToken == GrapaTokenType::PTR && aug2->vRulePointer) aug2 = aug2->vRulePointer;
					}
				}
			}
			if (!hasdst)
			{
				if (hasaug)
					DST.Append("NULL_");
				else
					DST.Append("AUG_");
			}
			if (mSignInG1)
			{
				blst::blst_p1 out1, out_sig1, temp_sig;
				bool first = true;
				GrapaRuleEvent* e = NULL;
				GrapaRuleEvent* e2 = pData;
				if (pData->mValue.mToken == GrapaTokenType::ARRAY)
				{
					if (pData->vQueue == NULL) return false;
					e = pData->vQueue->Head();
					e2 = e;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
				}
				while (e2)
				{
					switch (e2->mValue.mToken)
					{
					case GrapaTokenType::STR:
					case GrapaTokenType::RAW:
						if (first)
						{
							if (mSignHash)
								blst::blst_hash_to_g1(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
							else
								blst::blst_encode_to_g1(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
							blst_sign_pk_in_g2(&out_sig1, &out1, &mSK);
							first = false;
							break;
						}
						if (mSignHash)
							blst::blst_hash_to_g1(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
						else
							blst::blst_encode_to_g1(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
						blst_sign_pk_in_g2(&temp_sig, &out1, &mSK);
						blst::blst_p1_add_or_double(&out_sig1, &out_sig1, &temp_sig);
						break;
					}
					if (e) e = e->Next();
					e2 = e;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
					if (aug && aug->Next())
					{
						aug = aug->Next();
						aug2 = aug;
						while (aug2 && aug2->mValue.mToken == GrapaTokenType::PTR && aug2->vRulePointer) aug2 = aug2->vRulePointer;
					}
				}
				pR.SetLength(48);
				blst::blst_p1_compress(pR.mBytes, &out_sig1);
			}
			else
			{
				blst::blst_p2 out1, out_sig1, temp_sig;
				bool first = true;
				GrapaRuleEvent* e = NULL;
				GrapaRuleEvent* e2 = pData;
				if (pData->mValue.mToken == GrapaTokenType::ARRAY)
				{
					if (pData->vQueue == NULL) return false;
					e = pData->vQueue->Head();
					e2 = e;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
				}
				while (e2)
				{
					switch (e2->mValue.mToken)
					{
					case GrapaTokenType::STR:
					case GrapaTokenType::RAW:
						if (first)
						{
							if (mSignHash)
								blst::blst_hash_to_g2(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
							else
								blst::blst_encode_to_g2(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
							blst_sign_pk_in_g1(&out_sig1, &out1, &mSK);
							first = false;
							break;
						}
						if (mSignHash)
							blst::blst_hash_to_g2(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
						else
							blst::blst_encode_to_g2(&out1, e2->mValue.mBytes, e2->mValue.mLength, DST.mBytes, DST.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
						blst_sign_pk_in_g1(&temp_sig, &out1, &mSK);
						blst::blst_p2_add_or_double(&out_sig1, &out_sig1, &temp_sig);
						break;
					}
					if (e) e = e->Next();
					e2 = e;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
					if (aug && aug->Next())
					{
						aug = aug->Next();
						aug2 = aug;
						while (aug2 && aug2->mValue.mToken == GrapaTokenType::PTR && aug2->vRulePointer) aug2 = aug2->vRulePointer;
					}
				}
				pR.SetLength(96);
				blst::blst_p2_compress(pR.mBytes, &out_sig1);
			}
		}
		return true;
	};
	virtual bool SignAdd(GrapaRuleEvent* pData, GrapaBYTE& pR, GrapaRuleEvent* pParams)
	{
		blst::BLST_ERROR berr;
		blst::blst_p1 out_sig1;
		blst::blst_p2 out_sig2;
		bool first1 = true;
		bool first2 = true;
		GrapaRuleEvent* e = pData->vQueue->Head();
		while (e)
		{
			GrapaRuleEvent* e2 = e;
			while (e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
			switch (e2->mValue.mToken)
			{
			case GrapaTokenType::STR:
			case GrapaTokenType::RAW:
				if (e2->mValue.mLength == 48)
				{
					blst::blst_p1_affine R1;
					if (first1)
					{
						berr = blst::blst_p1_uncompress(&R1, e2->mValue.mBytes);
						blst::blst_p1_from_affine(&out_sig1, &R1);
						if (!blst::blst_p1_on_curve(&out_sig1))
							return false;
						first1 = false;
						break;
					}
					blst::blst_p1 Rx;
					berr = blst::blst_p1_uncompress(&R1, e2->mValue.mBytes);
					blst::blst_p1_from_affine(&Rx, &R1);
					if (!blst::blst_p1_on_curve(&Rx))
						return false;
					blst::blst_p1_add_or_double(&out_sig1, &out_sig1, &Rx);
				}
				else if (e2->mValue.mLength == 96)
				{
					blst::blst_p2_affine R2;
					if (first2)
					{
						berr = blst::blst_p2_uncompress(&R2, e2->mValue.mBytes);
						blst::blst_p2_from_affine(&out_sig2, &R2);
						if (!blst::blst_p2_on_curve(&out_sig2))
							return false;
						first2 = false;
						break;
					}
					blst::blst_p2 Rx;
					berr = blst::blst_p2_uncompress(&R2, e2->mValue.mBytes);
					blst::blst_p2_from_affine(&Rx, &R2);
					if (!blst::blst_p2_on_curve(&Rx))
						return false;
					blst::blst_p2_add_or_double(&out_sig2, &out_sig2, &Rx);
				}
				break;
			}
			e = e->Next();
		}
		if (first2)
		{
			pR.SetLength(48);
			blst::blst_p1_compress(pR.mBytes, &out_sig1);
		}
		else
		{
			pR.SetLength(96);
			blst::blst_p2_compress(pR.mBytes, &out_sig2);
		}
		return true;
	};
	virtual bool Verify(GrapaRuleEvent* pData, const GrapaBYTE& pR, GrapaRuleEvent* pParams)
	{
		if (mBBS)
		{

			GrapaRuleEvent* ep = pData->vQueue ? pData->vQueue->Head() : NULL;
			int mcount = ep ? pData->vQueue->mCount : 1;
			SetH(mcount);

			bool ismatch = false;
			// "w*P2^e" => w+P2*e == P2*(SK+e)

			if (mSignInG1)
			{
				blst::blst_p1_affine Aa;
				blst::BLST_ERROR berr = blst::blst_p1_uncompress(&Aa, pR.mBytes);

				blst::blst_scalar es, ss;
				blst_scalar_from_bendian(&es, &pR.mBytes[48]);
				blst_scalar_from_bendian(&ss, &pR.mBytes[48 + 32]);

				blst::blst_p1 b = *blst::blst_p1_generator();
				blst::blst_p1 out2;
				blst::blst_p1_mult(&out2, &mh0_1, (blst::byte*)ss.b, 256);
				blst::blst_p1_add(&b, &b, &out2);
				for (int i = 0; i < mcount; i++)
				{
					GrapaRuleEvent* e2 = ep;
					if (ep == NULL) e2 = pData;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
					blst::blst_p1_mult(&out2, mh_1[i], (blst::byte*)e2->mValue.mBytes, e2->mValue.mLength * 8);
					blst::blst_p1_add(&b, &b, &out2);
					if (ep) ep = ep->Next();
				}

				blst::blst_p2 P2 = *blst::blst_p2_generator();

				blst::blst_p2 P2e;
				blst::blst_p2_mult(&P2e, &P2, (blst::byte*)es.b, 256);
				blst::blst_p2 wP2e;
				blst::blst_p2_add(&wP2e, &P2e, mPK2.at(0));
				blst::blst_p2_affine wP2ea;
				blst::blst_p2_to_affine(&wP2ea, &wP2e);

				blst::blst_p2_affine P2a;
				blst::blst_p2_to_affine(&P2a, &P2);

				blst::blst_p1_affine ba;
				blst::blst_p1_to_affine(&ba, &b);

				blst::blst_fp12 C1, C2;
				blst::blst_miller_loop(&C1, &wP2ea, &Aa);
				blst::blst_miller_loop(&C2, &P2a, &ba);

				ismatch = blst::blst_fp12_finalverify(&C1, &C2) == 1;
			}
			else
			{
				blst::blst_p2_affine Aa;
				blst::BLST_ERROR berr = blst::blst_p2_uncompress(&Aa, pR.mBytes);

				blst::blst_scalar es, ss;
				blst_scalar_from_bendian(&es, &pR.mBytes[96]);
				blst_scalar_from_bendian(&ss, &pR.mBytes[96 + 32]);

				blst::blst_p2 b = *blst::blst_p2_generator();
				blst::blst_p2 out2;
				blst::blst_p2_mult(&out2, &mh0_2, (blst::byte*)ss.b, 256);
				blst::blst_p2_add(&b, &b, &out2);
				for (int i = 0; i < mcount; i++)
				{
					GrapaRuleEvent* e2 = ep;
					if (ep == NULL) e2 = pData;
					else while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
					blst::blst_p2_mult(&out2, mh_2[i], (blst::byte*)e2->mValue.mBytes, e2->mValue.mLength * 8);
					blst::blst_p2_add(&b, &b, &out2);
					if (ep) ep = ep->Next();
				}

				blst::blst_p1 P2 = *blst::blst_p1_generator();

				blst::blst_p1 P2e;
				blst::blst_p1_mult(&P2e, &P2, (blst::byte*)es.b, 256);
				blst::blst_p1 wP2e;
				blst::blst_p1_add(&wP2e, &P2e, mPK1.at(0));
				blst::blst_p1_affine wP2ea;
				blst::blst_p1_to_affine(&wP2ea, &wP2e);

				blst::blst_p1_affine P2a;
				blst::blst_p1_to_affine(&P2a, &P2);

				blst::blst_p2_affine ba;
				blst::blst_p2_to_affine(&ba, &b);

				blst::blst_fp12 C1, C2;
				blst::blst_miller_loop(&C1, &Aa, &wP2ea);
				blst::blst_miller_loop(&C2, &ba, &P2a);

				ismatch = blst::blst_fp12_finalverify(&C1, &C2) == 1;
			}

			return ismatch;
		}
		else
		{
			blst::BLST_ERROR berr;
			GrapaBYTE DST("BLS_SIG_");
			DST.Append(mAlg);
			GrapaRuleEvent* aug = NULL, * aug2 = NULL;
			bool hasdst = false;
			bool hasaug = false;
			if (pParams && pParams->vQueue)
			{
				GrapaRuleEvent* x;
				s64 index;
				x = pParams->vQueue->Search(GrapaCHAR("dst"), index);
				while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
				if (x && x->mValue.mLength)
				{
					DST.FROM(x->mValue);
					hasdst = true;
				}
				x = pParams->vQueue->Search(GrapaCHAR("aug"), index);
				while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
				if (x && (x->mValue.mLength || (x->vQueue && x->vQueue->mCount)))
				{
					hasaug = true;
					aug = x;
					aug2 = x;
					if (x->mValue.mToken == GrapaTokenType::ARRAY)
					{
						aug = x->vQueue->Head();
						aug2 = aug;
						while (aug2 && aug2->mValue.mToken == GrapaTokenType::PTR && aug2->vRulePointer) aug2 = aug2->vRulePointer;
					}
				}
			}
			if (!hasdst)
			{
				if (hasaug)
					DST.Append("NULL_");
				else
					DST.Append("AUG_");
			}
			if (pR.mLength == 96 && mPK1.size())
			{
				blst::blst_p2_affine signature;
				berr = blst::blst_p2_uncompress(&signature, pR.mBytes);
				GrapaRuleEvent* e = NULL;
				GrapaRuleEvent* e2 = pData;
				if (pData->mValue.mToken == GrapaTokenType::ARRAY)
				{
					if (pData->vQueue == NULL) return false;
					e = pData->vQueue->Head();
					e2 = e;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
				}
				int pkpos = 0;
				bool isfirst = true;
				bool ischanged = true;
				blst::blst_pairing* ctx = (blst::blst_pairing*)calloc(1, blst::blst_pairing_sizeof());
				blst::blst_pairing_init(ctx, mSignHash, DST.mBytes, DST.mLength);
				while (ischanged && e2 && pkpos < mPK1.size())
				{
					blst::blst_p1* pk1x = mPK1.at(pkpos);
					if (pk1x == NULL) return false;
					blst::blst_p1_affine pk1;
					blst::blst_p1_to_affine(&pk1, pk1x);
					blst::blst_pairing_aggregate_pk_in_g1(ctx, &pk1, isfirst ? &signature : NULL, e2->mValue.mBytes, e2->mValue.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
					isfirst = false;
					ischanged = false;
					if (e && e->Next())
					{
						e = e->Next();
						e2 = e;
						while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
						ischanged = true;
					}
					if (aug && aug->Next())
					{
						aug = aug->Next();
						aug2 = aug;
						while (aug2 && aug2->mValue.mToken == GrapaTokenType::PTR && aug2->vRulePointer) aug2 = aug2->vRulePointer;
					}
					if ((pkpos + 1) < mPK1.size())
					{
						pkpos++;
						ischanged = true;
					}
				}
				blst::blst_pairing_commit(ctx);
				bool ismatch = blst::blst_pairing_finalverify(ctx, NULL);
				free(ctx);
				return ismatch;
			}
			else if (pR.mLength == 48 && mPK2.size())
			{

				//blst::blst_p1 out;
				//blst::blst_p1_affine out1;
				//blst::blst_hash_to_g1(&out, (blst::byte*)"abc", 3,
				//	(blst::byte*)"QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_RO_", 50,
				//	(blst::byte*)"", 0,
				//	NULL);
				//blst::blst_p1_to_affine(&out1, &out);
				//GrapaBYTE rr;
				//rr.SetLength(48);
				//blst::blst_p1_compress(rr.mBytes, &out);

				blst::blst_p1_affine signature;
				berr = blst::blst_p1_uncompress(&signature, pR.mBytes);
				GrapaRuleEvent* e = NULL;
				GrapaRuleEvent* e2 = pData;
				if (pData->mValue.mToken == GrapaTokenType::ARRAY)
				{
					if (pData->vQueue == NULL) return false;
					e = pData->vQueue->Head();
					e2 = e;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
				}
				int pkpos = 0;
				bool isfirst = true;
				bool ischanged = true;
				blst::blst_pairing* ctx = (blst::blst_pairing*)calloc(1, blst::blst_pairing_sizeof());
				blst::blst_pairing_init(ctx, mSignHash, DST.mBytes, DST.mLength);
				while (ischanged && e2 && pkpos < mPK2.size())
				{
					blst::blst_p2* pk2x = mPK2.at(pkpos);
					if (pk2x == NULL) return false;
					blst::blst_p2_affine pk2;
					blst::blst_p2_to_affine(&pk2, pk2x);
					blst::blst_pairing_aggregate_pk_in_g2(ctx, &pk2, isfirst ? &signature : NULL, e2->mValue.mBytes, e2->mValue.mLength, aug2 ? aug2->mValue.mBytes : NULL, aug2 ? aug2->mValue.mLength : 0, &mCB);
					isfirst = false;
					ischanged = false;
					if (e && e->Next())
					{
						e = e->Next();
						e2 = e;
						while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
						ischanged = true;
					}
					if (aug && aug->Next())
					{
						aug = aug->Next();
						aug2 = aug;
						while (aug2 && aug2->mValue.mToken == GrapaTokenType::PTR && aug2->vRulePointer) aug2 = aug2->vRulePointer;
					}
					if ((pkpos + 1) < mPK2.size())
					{
						pkpos++;
						ischanged = true;
					}
				}
				blst::blst_pairing_commit(ctx);
				bool ismatch = blst::blst_pairing_finalverify(ctx, NULL);
				free(ctx);
				return ismatch;
			}
		}
		return false;
	};
	virtual bool SignAdd(const GrapaBYTE& pR1, GrapaBYTE& pR)
	{
		if (pR.mLength == 0)
		{
			blst::BLST_ERROR berr;
			blst::blst_p1_affine R1;
			blst::blst_p1 R1x;
			berr = blst::blst_p1_uncompress(&R1, pR1.mBytes);
			blst::blst_p1_from_affine(&R1x, &R1);
			if (!blst::blst_p1_on_curve(&R1x))
				return false;
			pR.FROM(pR1);
			return true;
		}
		if (pR1.mLength == 48 && pR.mLength == 48)
		{
			blst::BLST_ERROR berr;
			blst::blst_p1_affine R1, R;
			blst::blst_p1 Rx, out_sig1;
			berr = blst::blst_p1_uncompress(&R1, pR1.mBytes);
			berr = blst::blst_p1_uncompress(&R, pR.mBytes);
			blst::blst_p1_from_affine(&Rx, &R);
			if (!blst::blst_p1_on_curve(&Rx))
				return false;
			blst::blst_p1_add_or_double_affine(&out_sig1, &Rx, &R1);
			if (!blst::blst_p1_on_curve(&out_sig1))
				return false;
			blst::blst_p1_compress(pR.mBytes, &out_sig1);
			return true;
		}
		else if (pR1.mLength == 96 && pR.mLength == 96)
		{
			blst::BLST_ERROR berr;
			blst::blst_p2_affine R2, R;
			blst::blst_p2 Rx, out_sig2;
			berr = blst::blst_p2_uncompress(&R2, pR1.mBytes);
			berr = blst::blst_p2_uncompress(&R, pR.mBytes);
			blst::blst_p2_from_affine(&Rx, &R);
			if (!blst::blst_p2_on_curve(&Rx))
				return false;
			blst::blst_p2_add_or_double_affine(&out_sig2, &Rx, &R2);
			if (!blst::blst_p2_on_curve(&out_sig2))
				return false;
			blst::blst_p2_compress(pR.mBytes, &out_sig2);
			return true;
		}
		return false;
	};
	virtual bool Secret(Grapa_bls12_381* mKey, GrapaBYTE& pSecret)
	{
		if (mSignInG1)
		{
			if (mKey->mPK2.size() == 0) return false;
			blst::blst_p2 out;
			blst::blst_p2_mult(&out, mKey->mPK2[0], (blst::byte*)&mSK, 256);
			pSecret.SetLength(96);
			blst::blst_p2_compress(pSecret.mBytes, &out);
			pSecret.mToken = GrapaTokenType::RAW;
			return true;
		}
		else
		{
			if (mKey->mPK1.size() == 0) return false;
			blst::blst_p1 out;
			blst::blst_p1_mult(&out, mKey->mPK1[0], (blst::byte*)&mSK, 256);
			pSecret.SetLength(48);
			blst::blst_p1_compress(pSecret.mBytes, &out);
			pSecret.mToken = GrapaTokenType::RAW;
		}
		return true;
	};
	virtual bool Hash(const char* type, const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pSize)
	{
		return Hash(EVP_get_digestbyname(type), pData, pEnc, pSize);
	}
	virtual bool Hash(const EVP_MD* type, const GrapaBYTE& pData, GrapaBYTE& pEnc, u64 pSize)
	{
		if (type == NULL) return false;
		EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
		int err = EVP_DigestInit_ex(mdctx, type, NULL);
		if (err <= 0)
		{
			if (mdctx) EVP_MD_CTX_destroy(mdctx);
			mdctx = NULL;
			return false;
		}
		err = EVP_DigestUpdate(mdctx, pData.mBytes, pData.mLength);
		if (err <= 0)
		{
			pEnc.SetLength(0);
			EVP_MD_CTX_destroy(mdctx);
			return false;
		}
		unsigned int len = EVP_MAX_MD_SIZE;
		pEnc.SetLength(len);
		err = EVP_DigestFinal_ex(mdctx, pEnc.mBytes, &len);
		if (err <= 0)
		{
			pEnc.SetLength(0);
			EVP_MD_CTX_destroy(mdctx);
			return false;
		}
		pEnc.SetLength(pSize ? ((pSize < len) ? pSize : len) : len);
		pEnc.mToken = GrapaTokenType::RAW;
		EVP_MD_CTX_destroy(mdctx);
		return true;
	};
};

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
	if (mPFC) delete (Grapa_bls12_381*)mPFC;
	mPFC = NULL;
}

bool GrapaEncode::FROM(GrapaRuleEvent* pKey)
{
	Clear();

	s64 idx;
	GrapaRuleEvent* x = pKey->vQueue->Search("method", idx);
	while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
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
	else if (x->mValue.StrLowerCmp("pfc") == 0)
	{
		if (pKey && pKey->vQueue)
		{
			GrapaCHAR algorithmname("BLS12381G1_XMD:SHA-256_SSWU_RO_");
			x = pKey->vQueue->Search("alg", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) algorithmname.FROM(x->mValue);
			Grapa_bls12_381* bls = new Grapa_bls12_381();
			mPFC = bls;
			if (!bls->SetAlg(algorithmname))
			{
				delete bls;
				mPFC = NULL;
				return false;
			}
			x = pKey->vQueue->Search("sk", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x && x->mValue.mLength)
			{
				GrapaCHAR pub;
				if (!bls->SetPrv(x->mValue, pub))
				{
					delete bls;
					mPFC = NULL;
					return false;
				}
			}
			else
			{
				x = pKey->vQueue->Search("pk", idx);
				while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
				if (x)
				{
					bls->ClearPub();
					switch (x->mValue.mToken)
					{
					case GrapaTokenType::STR:
					case GrapaTokenType::RAW:
						if (x->mValue.mLength)
							bls->SetPub(x->mValue);
						break;
					case GrapaTokenType::ARRAY:
						if (x->vQueue && x->vQueue->mCount)
						{
							GrapaRuleEvent* e = x->vQueue->Head();
							while (e)
							{
								GrapaRuleEvent* e2 = e;
								while (e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
								switch (e2->mValue.mToken)
								{
								case GrapaTokenType::STR:
								case GrapaTokenType::RAW:
									bls->AddPub(e2->mValue);
									break;
								}
								e = e->Next();
							}
						}
						break;
					}
				}
			}
		}
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
				b.FromBytes(x->mValue,true);
				bits = b.LongValue();
			}

			x = pParams->vQueue->Search("e", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) e.FromBytes(x->mValue,true);
			if (e.IsZero()) e = (u64)65537;
		}

		u32 sz;
		GrapaBYTE gb;
		BIGNUM* n;
		GrapaCHAR s;

		mRSA = (RSAs*)RSA_new_method(NULL);

		gb = e.getBytes(true);
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
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("n"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->e) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->e, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("e"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->d) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->d, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("d"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->p) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->p, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("p"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->q) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->q, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("q"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->dmp1) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->dmp1, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("dmp1"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->dmq1) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->dmq1, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("dmq1"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(((RSAs*)mRSA)->iqmp) + 1);
		sz = BN_bn2bin(((RSAs*)mRSA)->iqmp, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("iqmp"), g.getBytes(true)));

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
				b.FromBytes(x->mValue,true);
				bits = b.LongValue();
			}

			x = pParams->vQueue->Search("g", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x)
			{
				GrapaInt b;
				b.FromBytes(x->mValue,true);
				pG = b.LongValue();
			}

			x = pParams->vQueue->Search("p", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) pP.FromBytes(x->mValue,true);
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

				gg = ((GrapaInt*)&pP)->getBytes(true);
				BIGNUM* p = BN_bin2bn((u8*)gg.mBytes, gg.mLength, NULL);

				gg = GrapaInt(pG).getBytes(true);
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
				g.FromBytes(gb,true);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("p"), g.getBytes(true)));
			}

			n = (BIGNUM*) DH_get0_q((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb,true);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("q"), g.getBytes(true)));
			}

			n = (BIGNUM*) DH_get0_g((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb,true);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("g"), g.getBytes(true)));
			}

			n = (BIGNUM*) DH_get0_pub_key((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb,true);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("pub"), g.getBytes(true)));
			}

			n = (BIGNUM*) DH_get0_priv_key((DH*)mDH);
			if (n)
			{
				gb.SetSize(BN_num_bytes(n) + 1);
				sz = BN_bn2bin(n, (u8*)gb.mBytes);
				gb.SetLength(sz);
				g.FromBytes(gb,true);
				g.dataSigned = 0;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("prv"), g.getBytes(true)));
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
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("pub"), g.getBytes(true)));

		gb.SetSize(BN_num_bytes(prv1) + 1);
		sz = BN_bn2bin(prv1, (u8*)gb.mBytes);
		gb.SetLength(sz);
		g.FromBytes(gb,true);
		g.dataSigned = 0;
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("prv"), g.getBytes(true)));

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
	else if (((GrapaCHAR*)&pMethod)->StrLowerCmp("kdf") == 0)
	{
		GrapaBYTE salt, secret, data, key;
		GrapaCHAR algorithmname("hkdf");
		GrapaCHAR digestname("sha256");
		int count = 1;
		int size = 256;
		GrapaCHAR pub, prv;
		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("alg", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) algorithmname.FROM(x->mValue);
			x = pParams->vQueue->Search("digest", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) digestname.FROM(x->mValue);
			x = pParams->vQueue->Search("count", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) count = GrapaInt(x->mValue).LongValue();
			x = pParams->vQueue->Search("size", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) size = GrapaInt(x->mValue).LongValue();
			x = pParams->vQueue->Search("salt", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) salt.FROM(x->mValue);
			x = pParams->vQueue->Search("secret", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) secret.FROM(x->mValue);
			x = pParams->vQueue->Search("info", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) data.FROM(x->mValue);
		}
		EVP_MD* md = (EVP_MD*)EVP_get_digestbyname((char*)digestname.mBytes);
		if (md == NULL && digestname.mLength) return result;
		if (algorithmname.StrLowerCmp("PBKDF2") == 0)
		{
			key.SetLength(size);
			err = PKCS5_PBKDF2_HMAC((const char*)secret.mBytes, secret.mLength, salt.mBytes, salt.mLength, count, md, size, key.mBytes);
			if (err >= 1)
			{
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::LIST;
				result->vQueue = new GrapaRuleQueue();
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("kdf")));
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("alg"), algorithmname));
				key.mToken = GrapaTokenType::RAW;
				if (key.mLength) result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("sk"), key));
			}
		}
		else if (algorithmname.StrLowerCmp("hkdf") == 0)
		{
			EVP_PKEY_CTX* pctx;
			key.mToken = GrapaTokenType::RAW;
			pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
			int err = 0;
			err = EVP_PKEY_derive_init(pctx);
			if (err >= 1 && md)
				err = EVP_PKEY_CTX_set_hkdf_md(pctx, md);
			if (err >= 1 && salt.mLength)
				err = EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt.mBytes, salt.mLength);
			if (err >= 1 && secret.mLength)
				err = EVP_PKEY_CTX_set1_hkdf_key(pctx, secret.mBytes, secret.mLength);
			if (err >= 1 && data.mLength)
				err = EVP_PKEY_CTX_add1_hkdf_info(pctx, data.mBytes, data.mLength);
			if (err >= 1)
			{
				size_t outlen = size;
				key.SetLength(outlen);
				err = EVP_PKEY_derive(pctx, key.mBytes, &outlen);
				if (err >= 1)
				{
					key.SetLength(outlen);
					result = new GrapaRuleEvent();
					result->mValue.mToken = GrapaTokenType::LIST;
					result->vQueue = new GrapaRuleQueue();
					result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("kdf")));
					result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("alg"), algorithmname));
					key.mToken = GrapaTokenType::RAW;
					if (key.mLength) result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("sk"), key));
				}
			}
			EVP_PKEY_CTX_free(pctx);
		}
		else if (algorithmname.StrLowerCmp("argon2id") == 0)
		{
			// https://github.com/p-h-c/phc-winner-argon2
			// https://github.com/mattrglobal/ffi-bbs-signatures/blob/master/src/lib.rs
			// gen_sk
		}
		return result;
	}
	else if (((GrapaCHAR*)&pMethod)->StrLowerCmp("pfc") == 0)
	{
		GrapaCHAR algorithmname("BLS12381G1_XMD:SHA-256_SSWU_RO_");
		GrapaBYTE prv, secret, data;
		if (pParams && pParams->vQueue)
		{
			x = pParams->vQueue->Search("alg", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) algorithmname.FROM(x->mValue);
			x = pParams->vQueue->Search("sk", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) prv.FROM(x->mValue);
			x = pParams->vQueue->Search("ikm", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) secret.FROM(x->mValue);
			x = pParams->vQueue->Search("info", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) data.FROM(x->mValue);
		}
		if (prv.mLength || secret.mLength)
		{
			GrapaBYTE pub;
			Grapa_bls12_381* bls = new Grapa_bls12_381();
			mPFC = bls;
			if (!bls->SetAlg(algorithmname))
			{
				delete bls;
				mPFC = NULL;
				return result;
			}
			if (secret.mLength)
				bls->KeyGen(secret, data, prv);
			if (prv.mLength)
				bls->SetPrv(prv, pub);
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::LIST;
			result->vQueue = new GrapaRuleQueue();
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("method"), GrapaCHAR("pfc")));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("alg"), algorithmname));
			prv.mToken = GrapaTokenType::RAW;
			if (prv.mLength) result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("sk"), prv));
			pub.mToken = GrapaTokenType::RAW;
			if (pub.mLength) result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("pk"), pub));

		}

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
	//enc.FromBytes(pEnc,true);
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
	else if (mPFC)
	{
		((Grapa_bls12_381*)mPFC)->Sign(pData, pSignature, pParams);

		return true;
	}

	//GrapaBYTE hash;
	//GrapaHash::SHAKE256(pData, hash, mDataSize);
	//return Encode(hash, pSignature, m_d);
	return(false);
}

bool GrapaEncode::SignAdd(GrapaRuleEvent* pData, GrapaBYTE& pSignature, GrapaRuleEvent* pParams)
{
	if (mPFC)
	{
		((Grapa_bls12_381*)mPFC)->SignAdd(pData, pSignature, pParams);
		return true;
	}
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
	else if (mPFC)
	{
		return  ((Grapa_bls12_381*)mPFC)->Verify(pData, pSignature, pParams);
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
		GrapaObjectEvent* x = pKey->vQueue->SearchLower("method", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x == NULL) return false;
		if (x->mValue.StrLowerCmp("dh") != 0) return false;

		BIGNUM * pub;
		pub = NULL;

		x = pKey->vQueue->Search("pub", idx);
		while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
		if (x) pub = BN_bin2bn((u8*)x->mValue.mBytes, x->mValue.mLength, NULL);

		GrapaBYTE gc;
		gc.SetLength(DH_size((DH*)mDH));
		err = DH_compute_key(gc.mBytes, pub, (DH*)mDH);
		char es[1024];
		if (err <= 0)
		{
			unsigned long xer = ERR_get_error();
			ERR_error_string_n(xer, es, 1024);
		}
		BN_free(pub);
		if (err >= 1)
		{
			GrapaInt g;
			gc.SetLength(err);
			g.FromBytes(gc,true);
			pSecret = g.getBytes(true);
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
					g.FromBytes(gc,true);
					pSecret = g.getBytes(true);
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
				g.FromBytes(gc,true);
				pSecret = g.getBytes(true);
			}
		}
		EVP_PKEY_CTX_free(ctx1);
		return(err >= 1);
	}
	else if (mPFC)
	{
		GrapaEncode pk;
		pk.FROM(pKey);
		if (pk.mPFC == NULL) return false;
		return  ((Grapa_bls12_381*)mPFC)->Secret((Grapa_bls12_381*)pk.mPFC, pSecret);
	}
	return(false);
}
