// GrapaEncrypt.cpp
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

#include "GrapaEncrypt.h"

#include "tiny-AES-c-master/aes.hpp"
#ifndef _WIN32
#include "tiny-AES-c-master/aes.c"
#endif

#include "GrapaHash.h"
#include "GrapaSystem.h"

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

extern GrapaSystem* gSystem;

// http://msdn.microsoft.com/en-us/library/system.security.cryptography.aes(v=vs.110).aspx

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaEncrypt::GrapaEncrypt()
{
	vKey = (void*)GrapaMem::Create(sizeof(AES_ctx));
	mSet = false;
	ClearKey();
	mAES = EVP_CIPHER_CTX_new();
}

GrapaEncrypt::~GrapaEncrypt()
{
	if (vKey)
		GrapaMem::Delete((void*)vKey);
	vKey = NULL;
	mSet = false;
	if (mAES) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)mAES);
	mAES = NULL;
}

void GrapaEncrypt::ClearKey()
{
	memset(vKey, 0, sizeof(AES_ctx));
	mSet = false;
}

void GrapaEncrypt::Init(const GrapaBYTE& pKey, const GrapaBYTE& pIV, int enc)
{
	GrapaBYTE key,iv;
	if (pKey.mLength == 32)
	{
		key.FROM(pKey);
		AES_init_ctx((AES_ctx*)vKey, pKey.mBytes);
	}
	else
	{
		GrapaHash::SHAKE256(pKey, key, 32);
		AES_init_ctx((AES_ctx*)vKey, key.mBytes);
	}
	if (pIV.mLength == 16)
	{
		iv.FROM(pIV);
		AES_ctx_set_iv((AES_ctx*)vKey, pIV.mBytes);
	}
	else if (pIV.mLength)
	{
		GrapaHash::SHAKE256(pIV, iv, 16);
		AES_ctx_set_iv((AES_ctx*)vKey, iv.mBytes);
	}
	mAES = EVP_CIPHER_CTX_new();
	int err = EVP_CipherInit_ex((EVP_CIPHER_CTX*)mAES, EVP_aes_256_cbc(), NULL, key.mBytes, iv.mLength?iv.mBytes:NULL, enc);
	mSet = true;
}

void GrapaEncrypt::Encrypt(GrapaBYTE& pData)
{
	if (pData.mLength == 0 || pData.mBytes == NULL) return;

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
	u8 out[32];
	int outl = 0;
	GrapaBYTE outs;
	outs.SetSize(pData.mLength+1);
	outs.SetLength(0);
	while (pos < pData.mLength)
	{
		int inl = 16;
		outl = 0;
		if ((pData.mLength - pos) < inl) inl = pData.mLength - pos;
		int err = EVP_CipherUpdate((EVP_CIPHER_CTX*)mAES, out, &outl, &pData.mBytes[pos], inl);
		outs.Append(out, outl);
		pos += inl;
	}
	outl = 0;
	err = EVP_CipherFinal_ex((EVP_CIPHER_CTX*)mAES, out, &outl);
	outs.Append(out, outl);
	pData = outs;
	pData.mToken = GrapaTokenType::RAW;
}

void GrapaEncrypt::Decrypt(GrapaBYTE& pEnc)
{
	//if (pEnc.mLength == 0 || pEnc.mBytes == NULL) return;
	//AES_CBC_decrypt_buffer((AES_ctx*)vKey, pEnc.mBytes, (u32)pEnc.mLength);
	//u8 pad = pEnc.mBytes[pEnc.mLength - 1];
	//if (pad && pad <= 16)
	//	pEnc.SetLength(pEnc.mLength - pad);
	//pEnc.mToken = GrapaTokenType::RAW;

	int err;
	u64 pos = 0;
	u8 out[32];
	int outl = 0;
	GrapaBYTE outs;
	outs.SetSize(pEnc.mLength + 1);
	outs.SetLength(0);
	while (pos < pEnc.mLength)
	{
		int inl = 16;
		outl = 0;
		if ((pEnc.mLength - pos) < inl) inl = pEnc.mLength - pos;
		int err = EVP_CipherUpdate((EVP_CIPHER_CTX*)mAES, out, &outl, &pEnc.mBytes[pos], inl);
		outs.Append(out, outl);
		pos += inl;
	}
	outl = 0;
	err = EVP_CipherFinal_ex((EVP_CIPHER_CTX*)mAES, out, &outl);
	outs.Append(out, outl);
	pEnc = outs;
	pEnc.mToken = GrapaTokenType::RAW;
}

void GrapaEncrypt::Encrypt(GrapaBYTE& pData, GrapaBYTE& pEnc)
{
	pEnc = pData;
	Encrypt(pEnc);
}

void GrapaEncrypt::Decrypt(GrapaBYTE& pEnc, GrapaBYTE& pData)
{
	pData = pEnc;
	Decrypt(pData);
}

/*
void GrapaEncrypt::EncryptSalt16(u8* value, u8* out, u8 saltCount)
{
	if (saltCount == 0)
	{
		memcpy(out, value, 16);
		AES_CBC_encrypt_buffer((AES_ctx*)vKey, out, 16);
		return;
	}
	u8 count = saltCount > 15 ? 15 : saltCount;
	memcpy(out, value, 16 - count);
	for (u8 i = (16 - count); i < 16; i++)
		out[i] = gSystem->Random32();
	AES_CBC_encrypt_buffer((AES_ctx*)vKey, out, 16);
}

void GrapaEncrypt::DecryptSalt16(u8* value, u8* out, u8 saltCount)
{
	if (saltCount == 0) {
		memcpy(out, value, 16);
		AES_CBC_decrypt_buffer((AES_ctx*)vKey, out, 16);
		return;
	}
	u8 buffer[34];
	memcpy(buffer, value, 16);
	AES_CBC_decrypt_buffer((AES_ctx*)vKey, buffer, 16);
	u8 count = saltCount > 15 ? 15 : saltCount;
	memcpy(out, buffer, 16 - count);
}

void GrapaEncrypt::EncryptSalt(GrapaBYTE& pData, u8 pSaltCount)
{
	GrapaBYTE enc;
	EncryptSalt(pData, enc, pSaltCount);
	pData.FROM(enc);
}

void GrapaEncrypt::EncryptSalt(GrapaBYTE& pData, GrapaBYTE& pEnc, u8 pSaltCount)
{
	pEnc.SetLength(0);
	u8 count = pSaltCount > 15 ? 15 : pSaltCount;
	u64 len = pData.mLength;
	if (len == 0 || pData.mBytes == NULL) return;
	u8 pad = ((len + 1) % (16 - count)) ? ((16 - count) - ((len + 1) % (16 - count))) : 0;
	u8 fieldType = pData.ToDbType();
	GrapaBYTE input;
	input.SetSize(len);
	input.FROM(pData);
	input.Pad((input.mLength + 1) + pad, pad + 1);
	pEnc.SetSize((input.mLength / (16 - count)) * 16 + 3);
	pEnc.SetLength((input.mLength / (16 - count)) * 16 + 2);
	for (u64 i = 0; i < (input.mLength / (16 - count)); i++)
		EncryptSalt16(&((u8*)input.mBytes)[i * (16 - count)], &((u8*)pEnc.mBytes)[i * 16 + 2], count);
	((u8*)pEnc.mBytes)[0] = count;
	((u8*)pEnc.mBytes)[1] = fieldType;
	pEnc.mToken = GrapaTokenType::RAW;
}

void GrapaEncrypt::DecryptSalt(GrapaBYTE& pEnc)
{
	GrapaBYTE data;
	DecryptSalt(pEnc, data);
	pEnc.FROM(data);
}

void GrapaEncrypt::DecryptSalt(GrapaBYTE& pEnc, GrapaBYTE& pData)
{
	pData.SetLength(0);
	if (pEnc.mLength <= 2 || pEnc.mBytes == NULL) return;
	u8 padCount = ((char*)pEnc.mBytes)[0];
	u8 fieldType = ((char*)pEnc.mBytes)[1];
	u8 count = padCount > 15 ? 15 : padCount;
	u64 len = pEnc.mLength - 2;
	pData.SetLength((len / 16) * (16 - count));
	for (u64 i = 0; i < (len / 16); i++)
		DecryptSalt16(&((u8*)pEnc.mBytes)[i * 16 + 2], &((u8*)pData.mBytes)[i * (16 - count)], count);
	u8* last = &((u8*)pData.mBytes)[pData.mLength - (16 - count)];
	if (last[(16 - count - 1)] == 0x80 || last[(16 - count - 1)] == 0x00)
	{
		for (u8 pos = (16 - count); pos; pos--)
		{
			if (last[pos - 1])
			{
				if (last[pos - 1] == 0x80)
					pData.SetLength(pData.mLength - (16 - count) + pos - 1);
				else
					pData.SetLength(pData.mLength - (16 - count) + pos);
				break;
			}
		}
	}
	else if (last[(16 - count - 1)] <= (16 - count))
	{
		pData.SetLength(pData.mLength - last[(16 - count - 1)]);
	}
	pData.FromDbType(fieldType);
}

*/