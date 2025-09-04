// GrapaNet.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaNet_
#define _GrapaNet_

#include "GrapaThread.h"
#include "GrapaValue.h"

#include <openssl/ssl.h>

class GrapaRuleEvent;
class GrapaRuleQueue;
class GrapaScriptExec;

class GrapaNet : public GrapaCritical
{
public:
	bool mStarted, mConnected, mBound;
	char mHostName[256];
	char mIpName[256];
	s16 mFamily;
	u16 mPort;
	u64 mMAC;
	char mName[256];
	GrapaCHAR vpResponse;

	GrapaNet();
    virtual ~GrapaNet();

	virtual GrapaError Connect(const GrapaCHAR& pURL);
	virtual GrapaError Bind(const GrapaCHAR& pURL);
	virtual GrapaError Listen(GrapaNet& pClient);
	virtual GrapaError Disconnect();

	virtual GrapaError Send(u8* sendbuf, u64 sendbuflen, u64& sendlen);
	virtual GrapaError Receive(u8* recvbuf, u64 recvbuflen, u64& recvlen);

	virtual GrapaError Send(const GrapaBYTE& sendbuf);
	virtual GrapaError Receive(GrapaBYTE& recvbuf);
	virtual GrapaError Pending(u64& hasmore);

	virtual void GetService(const GrapaCHAR& pURL, GrapaCHAR& pNodeName, GrapaCHAR& pServiceName, bool& isIPv6);
	virtual GrapaError GetPeerName();
	virtual GrapaError GetLocalName();
	static GrapaError GetMac(char* pName, char* pIp, u64& pMAC);
	static GrapaRuleEvent* GetInterfaces();
	
	virtual GrapaError Proxy(GrapaCHAR& ptype, GrapaCHAR& pid, GrapaCHAR& ph);
	virtual GrapaError Certificate(GrapaCHAR& certFile);
	virtual GrapaError Private(GrapaCHAR& keyFile, void* passOp, void* passParam);
	virtual GrapaError Trusted(GrapaCHAR& caFile, GrapaCHAR& caPath);
	virtual GrapaError Verify(GrapaCHAR& cert);
	virtual GrapaRuleQueue* Chain();
	static GrapaRuleQueue* Decode(const GrapaCHAR& cert);
	static void X509_to_PEM(X509* cert, GrapaCHAR& pem);
	static X509* PEM_to_X509(const GrapaCHAR& pem);

private:
	void* vInstance;
	virtual GrapaError Startup();
	virtual void Shutdown();

private:
	bool vIsSSL, vIsBlocking, vCriticalError;
	bool vIsProxy;

	SSL_CTX* vDelCTX;
	SSL_CTX* vXCTX;
	BIO* vBIO;
	BIO* aBIO;
	SSL* vSSL;

	GrapaCHAR vCertFile, vKeyFile;
	void* vPassOp;
	void* vPassParam;
	GrapaCHAR vcaFile, vcaPath;
	GrapaCHAR vpType, vpId, vPh;
};

////////////////////////////////////////////////////////////////////////////////

#endif //_GrapaNet_
