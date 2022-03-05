// GrapaAES256.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaEncrypt_
#define _GrapaEncrypt_

#include "GrapaValue.h"

class GrapaEncrypt
{
public:
	bool mSet;
	GrapaEncrypt();
	virtual ~GrapaEncrypt();

	void ClearKey();
	void Init(const GrapaBYTE& pKey, const GrapaBYTE& pIV, int enc);

	void Encrypt(GrapaBYTE& pData);
	void Decrypt(GrapaBYTE& pEnc);
	void Encrypt(GrapaBYTE& pData, GrapaBYTE& pEnc);
	void Decrypt(GrapaBYTE& pEnc, GrapaBYTE& pData);

	//void EncryptSalt16(u8* pData, u8* out, u8 saltCount = 0);
	//void DecryptSalt16(u8* pEnc, u8* out, u8 saltCount = 0);
	//void EncryptSalt(GrapaBYTE& pData, GrapaBYTE& pEnc, u8 pSaltCount = 0);
	//void DecryptSalt(GrapaBYTE& pEnc, GrapaBYTE& pData);
	//void EncryptSalt(GrapaBYTE& pData, u8 pSaltCount = 0);
	//void DecryptSalt(GrapaBYTE& pEnc);

private:
	void* vKey;
	void* mAES;
};


#endif //_GrapaEncrypt_

////////////////////////////////////////////////////////////////////////////////
