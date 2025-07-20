// GrapaNetConnect.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaNetConnect_
#define _GrapaNetConnect_

#include "GrapaState.h"

////////////////////////////////////////////////////////////////////////////////

class GrapaNetConnect : public GrapaThread
{
public:
	GrapaNet mNet;
	GrapaCHAR mURL;
	GrapaRuleEvent* vReceiveLambda, * dReceiveLambda;
	GrapaRuleEvent* vConnectLambda, * dConnectLambda;
	GrapaScriptState* vScriptState;
	bool mListenMode, mListenChild;
	GrapaRuleEvent* vObj;
	GrapaRuleQueue mListeners;
	GrapaNames mNames;
	GrapaRuleEvent* dPassLambda;
	GrapaRuleEvent* dPassParam;

public:
	GrapaNetConnect();
	virtual ~GrapaNetConnect();

	virtual void Set(GrapaCHAR& pURL);
	virtual GrapaError Connect(GrapaRuleEvent* pUrl);
	virtual GrapaError Bind(GrapaRuleEvent* pUrl);
	virtual GrapaError Listen(GrapaNetConnect* pNet);
	virtual GrapaError Disconnect();

	virtual GrapaError Proxy(GrapaRuleEvent* ptype, GrapaRuleEvent* pid, GrapaRuleEvent* ph);
	virtual GrapaError Certificate(GrapaRuleEvent* certFile);
	virtual GrapaError Private(GrapaScriptExec* pScriptExec, GrapaRuleEvent* keyFile, GrapaRuleEvent* passOp, GrapaRuleEvent* passParam);
	virtual GrapaError Trusted(GrapaRuleEvent* caFile, GrapaRuleEvent* caPath);
	virtual GrapaError Verify(GrapaCHAR& cert);
	virtual GrapaRuleQueue* Chain();
	static GrapaRuleQueue* Decode(const GrapaCHAR& cert);

	virtual GrapaError OnListen(GrapaScriptExec* pScriptExec, GrapaRuleEvent* pUrl, GrapaRuleEvent* pReceiveLambda, GrapaRuleEvent* pConnectLambda);
	virtual GrapaError OnReceive(GrapaScriptExec* pScriptExec, GrapaRuleEvent* pReceiveLambda);

	virtual void Starting();
	virtual void Running();
	virtual void Stopping();

	virtual GrapaError HttpRead(GrapaScriptExec* pScriptExec);
	virtual GrapaRuleEvent* HttpMessage(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace);
};

////////////////////////////////////////////////////////////////////////////////

#endif //_GrapaNetConnect_
