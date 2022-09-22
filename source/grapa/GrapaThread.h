// GrapaThread.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaThread_
#define _GrapaThread_

#include "GrapaType.h"

class GrapaCritical
{
public:
	s32 mWaitCount;
	bool mWaiting, mConst;

	GrapaCritical();
    virtual ~GrapaCritical();

	virtual bool TryCritical();
	virtual void WaitCritical();
	virtual void LeaveCritical();
	virtual void WaitCondition(bool noAdd = false);
	virtual void SendCondition(bool force = false);

private:
	void* vInstanceC;
};

class GrapaThread : public GrapaCritical
{
public:
	GrapaError mError;
	bool mStop, mSync, mSuspended;
	GrapaCritical mRunning, mSuspend;

	GrapaThread();
    virtual ~GrapaThread();

	virtual GrapaError Start(bool pSync);
	virtual GrapaError StartSync();
	virtual void Stop();
	virtual bool Started();
	virtual GrapaError Suspend();
	virtual GrapaError Resume();

	void RunFromStatic();

private:
	void* vInstanceT;
	virtual void Starting() = 0;
	virtual void Running() = 0;
	virtual void Stopping() = 0;
};

#endif //_GrapaThread_

////////////////////////////////////////////////////////////////////////////////
