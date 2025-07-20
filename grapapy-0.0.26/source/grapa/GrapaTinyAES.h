// GrapaTinyAES.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaTinyAES_
#define _GrapaTinyAES_

#include "GrapaValue.h"

class GrapaTinyAES
{
public:
	bool mSet;
	GrapaTinyAES();
	virtual ~GrapaTinyAES();

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


#endif //_GrapaTinyAES_

////////////////////////////////////////////////////////////////////////////////
