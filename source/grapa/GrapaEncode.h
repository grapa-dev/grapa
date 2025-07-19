// GrapaEncode.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaEncode_
#define _GrapaEncode_

#include "GrapaPrime.h"

class GrapaEncode
{
public:
	//u32 mKeySize, mEncSize, mDataSize;
	//GrapaInt m_n, m_e, m_d;
	//GrapaPrime m_p, m_q;
	//GrapaInt m_dmp1, m_dmq1, m_iqmp;
	//GrapaInt cZero, cOne;
	void* mRSA;
	void* mDH;
	void* mEC;
	void* mBCe;
	void* mBCd;
	void* mMD;
	void* mRPK;
	void* mPFC;

public:
	GrapaEncode();
	virtual ~GrapaEncode();
	bool FROM(GrapaRuleEvent* pKey);
	GrapaRuleEvent* GenKeys(const GrapaCHAR& pMethod, GrapaRuleEvent* pParams);
	bool Encode(const GrapaBYTE& pData, GrapaBYTE& pEnc, const GrapaInt& pPublicE);
	bool Decode(const GrapaBYTE& pEnc, GrapaBYTE& pData, const GrapaInt& pPrivateD);
	bool Sign(GrapaRuleEvent* pData, GrapaBYTE& pSignature, GrapaRuleEvent* pParams);
	bool SignAdd(GrapaRuleEvent* pData, GrapaBYTE& pSignature, GrapaRuleEvent* pParams);
	bool Verify(GrapaRuleEvent* pData, const GrapaBYTE& pSignature, GrapaRuleEvent* pParams);
	bool VerifyRecover(GrapaBYTE& pData, const GrapaBYTE& pSignature);
	bool Secret(GrapaRuleEvent* pKey, GrapaBYTE& pSecret);private:
private:
	void Clear();
};


#endif //_GrapaEncode_

////////////////////////////////////////////////////////////////////////////////
