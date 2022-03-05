// GrapaConsole.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaConsole_
#define _GrapaConsole_

#include "GrapaMem.h"
#include "GrapaState.h"

class GrapaConsoleResponse : public GrapaCritical, public GrapaSystemSend
{
public:
	GrapaCHAR mPrompt;
	//virtual void SendCommand(const void* sendbuf, u64 sendbuflen);
	virtual void UpdatePrompt();
};

class GrapaConsoleSend : public GrapaSystemSend
{
public:
	GrapaScriptState mScriptState;
	GrapaObjectQueue mScriptQueue;
public:
	GrapaConsoleSend();
	~GrapaConsoleSend();
public:
	virtual GrapaError Start();
	virtual void Stop();
	virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen);
	virtual GrapaCHAR SendSync(GrapaCHAR& pIn);

};

////////////////////////////////////////////////////////////////////////////////

#endif //_GrapaConsole_
