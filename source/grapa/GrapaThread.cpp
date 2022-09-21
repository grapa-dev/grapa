// GrapaThread.cpp
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

/* Need to replace these Win8 only calls
WaitOnAddress, use WaitForSingleObject  maybe
WakeByAddressSingle
*/

#include "GrapaThread.h"
#include "GrapaMem.h"

//#include <signal.h>
//#include <SynchAPI.h>

#if defined(__MINGW32__) || defined(__GNUC__)
#else
#ifdef _WIN32
#pragma comment (lib, "Synchronization.lib")
#pragma comment (lib, "Kernel32.lib")
#endif
#endif

#if defined(__MINGW32__) || defined(__GNUC__)
#include <pthread.h>
#include <unistd.h>
#else
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#endif


//https://computing.llnl.gov/tutorials/pthreads/


// http://msdn.microsoft.com/en-us/library/system.threading.thread.aspx#Y6434
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms682512(v=vs.85).aspx
// http://stackoverflow.com/questions/1372967/how-do-you-use-createthread-for-functions-which-are-class-members

// https://computing.llnl.gov/tutorials/pthreads/
//   GetLastError

typedef struct _GrapaCriticalPrivate {
#if defined(__MINGW32__) || defined(__GNUC__)
	pthread_mutex_t mCritical;
	//pthread_spinlock_t mCritical;
	pthread_mutex_t mWaitCritical;
	//pthread_spinlock_t mWaitCritical;
	pthread_cond_t mCond;
#else
#ifdef _WIN32
	CRITICAL_SECTION mCritical;
	CRITICAL_SECTION mWaitCritical;
	u32 mCond;
	//HANDLE mCond;
#endif
#endif
}GrapaCriticalPrivate;

typedef struct _GrapaThreadPrivate {
#if defined(__MINGW32__) || defined(__GNUC__)
	void* mThreadId;
	pthread_t mThread;
#else
#ifdef _WIN32
	DWORD   mThreadId;
	HANDLE  mThread;
#endif
#endif
#if defined(__MINGW32__) || defined(__GNUC__)
	static void* StaticRun(void* lpParam)
	{
		((GrapaThread*)lpParam)->mRunning.WaitCritical();
		((GrapaThread*)lpParam)->RunFromStatic();
		((GrapaThread*)lpParam)->mRunning.LeaveCritical();
		pthread_exit(NULL);
	}
#else
#ifdef _WIN32
	static DWORD WINAPI StaticRun(void* lpParam)
	{
		((GrapaThread*)lpParam)->mRunning.WaitCritical();
		((GrapaThread*)lpParam)->RunFromStatic();
		((GrapaThread*)lpParam)->mRunning.LeaveCritical();
		return(0);
	}
#endif
#endif
}GrapaThreadPrivate;

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaCritical::GrapaCritical()
{
	vInstanceC = (void*)GrapaMem::Create(sizeof(GrapaCriticalPrivate));

#if defined(__MINGW32__) || defined(__GNUC__)
	pthread_mutex_init(&((GrapaCriticalPrivate*)vInstanceC)->mCritical, NULL);
	//pthread_spin_init(&mCritical, PTHREAD_PROCESS_PRIVATE);
	pthread_mutex_init(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical, NULL);
	//pthread_spin_init(&mWaitCritical, PTHREAD_PROCESS_PRIVATE);
	pthread_cond_init(&((GrapaCriticalPrivate*)vInstanceC)->mCond, NULL);
#else
#ifdef _WIN32
	if (!InitializeCriticalSectionAndSpinCount(&((GrapaCriticalPrivate*)vInstanceC)->mCritical, 0x00000400))
	{/* failed */
	}
	if (!InitializeCriticalSectionAndSpinCount(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical, 0x00000400))
	{/* failed */
	}
	((GrapaCriticalPrivate*)vInstanceC)->mCond = false;
	//mCond = CreateMutex(NULL, false, NULL);
#endif
#endif
	mWaitCount = 0;
	mWaiting = false;
	mConst = false;
}

GrapaCritical::~GrapaCritical()
{
	if (mWaiting)
		SendCondition(true);
#if defined(__MINGW32__) || defined(__GNUC__)
	pthread_mutex_destroy(&((GrapaCriticalPrivate*)vInstanceC)->mCritical);
	//pthread_spin_destroy(&mCritical);
	pthread_mutex_destroy(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
	//pthread_spin_destroy(&mWaitCritical);
	pthread_cond_destroy(&((GrapaCriticalPrivate*)vInstanceC)->mCond);
#else
#ifdef _WIN32
	DeleteCriticalSection(&((GrapaCriticalPrivate*)vInstanceC)->mCritical);
	DeleteCriticalSection(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
	//CloseHandle(mCond);
#endif
#endif
	if (vInstanceC)
		GrapaMem::Delete(vInstanceC);
	vInstanceC = NULL;
}

bool GrapaCritical::TryCritical()
{
	if (mConst) return(true);
#if defined(__MINGW32__) || defined(__GNUC__)
	if (!pthread_mutex_trylock(&((GrapaCriticalPrivate*)vInstanceC)->mCritical)) return(true);
	//if (!pthread_spin_trylock(&mCritical)) return(true);
#else
#ifdef _WIN32
	if (TryEnterCriticalSection(&((GrapaCriticalPrivate*)vInstanceC)->mCritical)) return(true);
#endif
#endif
	return(false);
}

void GrapaCritical::WaitCritical()
{
	if (mConst) return;
#if defined(__MINGW32__) || defined(__GNUC__)
	pthread_mutex_lock(&((GrapaCriticalPrivate*)vInstanceC)->mCritical);
	//pthread_spin_lock(&mCritical);
#else
#ifdef _WIN32
	// SetCriticalSectionSpinCount
	EnterCriticalSection(&((GrapaCriticalPrivate*)vInstanceC)->mCritical);
#endif
#endif
}

void GrapaCritical::LeaveCritical()
{
	if (mConst) return;
#if defined(__MINGW32__) || defined(__GNUC__)
	pthread_mutex_unlock(&((GrapaCriticalPrivate*)vInstanceC)->mCritical);
	//pthread_spin_unlock(&mCritical);
#else
#ifdef _WIN32
	LeaveCriticalSection(&((GrapaCriticalPrivate*)vInstanceC)->mCritical);
#endif
#endif
}

void GrapaCritical::WaitCondition(bool noAdd)
{
	if (mConst) return;
	if (mWaiting || (noAdd && mWaitCount == 0))
	{
		mWaiting = false;
		mWaitCount = 0;
		LeaveCritical();
		return;
	}
#if defined(__MINGW32__) || defined(__GNUC__)
	pthread_mutex_lock(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
	//pthread_spin_lock(&mWaitCritical);
	if (!noAdd) mWaitCount++;
	mWaiting = true;
	LeaveCritical();
	pthread_cond_wait(&((GrapaCriticalPrivate*)vInstanceC)->mCond, &((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
	mWaiting = false;
	mWaitCount = 0;
	pthread_mutex_unlock(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
	//pthread_spin_unlock(&mWaitCritical);
#else
#ifdef _WIN32
	((GrapaCriticalPrivate*)vInstanceC)->mCond = 0;
	u32 oldCond = ((GrapaCriticalPrivate*)vInstanceC)->mCond;
	if (!noAdd) 
		mWaitCount++;
	mWaiting = true;
	//printf("%d:%s:%d:%d\n", this, "start wait", mWaiting, mWaitCount);
	LeaveCritical();
	while ((((GrapaCriticalPrivate*)vInstanceC)->mCond) == oldCond) {
		WaitOnAddress(&(((GrapaCriticalPrivate*)vInstanceC)->mCond), &oldCond, sizeof(u32), INFINITE);
	}
	mWaiting = false;
	mWaitCount = 0;
#endif
#endif
}

void GrapaCritical::SendCondition(bool force)
{
	if (mConst) return;
	if (force)
	{
#if defined(__MINGW32__) || defined(__GNUC__)
		if (mWaitCount)
		{
			pthread_mutex_lock(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
			//pthread_spin_lock(&mWaitCritical);
			pthread_cond_signal(&((GrapaCriticalPrivate*)vInstanceC)->mCond);
			pthread_mutex_unlock(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
			//pthread_spin_unlock(&mWaitCritical);
		}
#else
#ifdef _WIN32
		if (mWaitCount)
		{ 
			((GrapaCriticalPrivate*)vInstanceC)->mCond = !((GrapaCriticalPrivate*)vInstanceC)->mCond;
			WakeByAddressSingle(&((GrapaCriticalPrivate*)vInstanceC)->mCond);
			//SignalObjectAndWait(mCond, NULL, 0, false);
		}
#endif
#endif

		return;
	}
#if defined(__MINGW32__) || defined(__GNUC__)
	pthread_mutex_lock(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
	//pthread_spin_lock(&mWaitCritical);
	if (mWaitCount == 1 && mWaiting) 	// start with a higher mWaitCount if SendCondition needs to be called multipe times
	{
		mWaiting = false;
		mWaitCount = 0;
		pthread_cond_signal(&((GrapaCriticalPrivate*)vInstanceC)->mCond);
	}
	else if (mWaitCount)
		mWaitCount--;
	pthread_mutex_unlock(&((GrapaCriticalPrivate*)vInstanceC)->mWaitCritical);
	//pthread_spin_unlock(&mWaitCritical);
#else
#ifdef _WIN32
	//printf("%d:%s:%d:%d\n", this, "send condition start", mWaiting, mWaitCount);
	if (mWaitCount == 1 && mWaiting) 	// start with a higher mWaitCount if SendCondition needs to be called multipe times
	{
		mWaiting = false;
		mWaitCount = 0;
		((GrapaCriticalPrivate*)vInstanceC)->mCond = !((GrapaCriticalPrivate*)vInstanceC)->mCond;
		WakeByAddressSingle(&((GrapaCriticalPrivate*)vInstanceC)->mCond);
		//SignalObjectAndWait(mCond, NULL, 0, false);
	}
	else if (mWaitCount)
		mWaitCount--;
	//printf("%d:%s:%d:%d\n", this, "send condition end", mWaiting, mWaitCount);
#endif
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaThread::GrapaThread()
{
	vInstanceT = (void*)GrapaMem::Create(sizeof(GrapaCriticalPrivate));
	((GrapaThreadPrivate*)vInstanceT)->mThread = 0LL;
	((GrapaThreadPrivate*)vInstanceT)->mThreadId = 0LL;
	mError = 0;
	mStop = false;
	mSync = false;
	mSuspended = false;
}

GrapaThread::~GrapaThread()
{
	Stop();
	mError = 0;
	if (vInstanceT)
		GrapaMem::Delete(vInstanceT);
	vInstanceT = NULL;
}

GrapaError GrapaThread::Start()
{
	if (((GrapaThreadPrivate*)vInstanceT)->mThread) return(-1);
	mStop = true;
	mSync = false;
	Starting();
	WaitCritical();
#if defined(__MINGW32__) || defined(__GNUC__)
	((GrapaThreadPrivate*)vInstanceT)->mThreadId = (void*)this;
	mError = pthread_create(&((GrapaThreadPrivate*)vInstanceT)->mThread, NULL, ((GrapaThreadPrivate*)vInstanceT)->StaticRun, ((GrapaThreadPrivate*)vInstanceT)->mThreadId);
	if (mError && ((GrapaThreadPrivate*)vInstanceT)->mThread) {pthread_cancel(((GrapaThreadPrivate*)vInstanceT)->mThread); ((GrapaThreadPrivate*)vInstanceT)->mThread = 0LL;}
#else
#ifdef _WIN32
	mError = 0;//8000000000000
	((GrapaThreadPrivate*)vInstanceT)->mThread = CreateThread(NULL, 100 * 1024000, ((GrapaThreadPrivate*)vInstanceT)->StaticRun, (void*)this, STACK_SIZE_PARAM_IS_A_RESERVATION, &((GrapaThreadPrivate*)vInstanceT)->mThreadId);
	//((GrapaThreadPrivate*)vInstanceT)->mThread = CreateThread(NULL, 0, ((GrapaThreadPrivate*)vInstanceT)->StaticRun, (void*)this, 0, &((GrapaThreadPrivate*)vInstanceT)->mThreadId);
#endif
#endif

	if (((GrapaThreadPrivate*)vInstanceT)->mThread == 0LL) { ((GrapaThreadPrivate*)vInstanceT)->mThreadId = 0LL; return(-1); }
	//LeaveCritical();
	WaitCondition();
	return(0);
}

void GrapaThread::RunFromStatic()
{
	WaitCritical();
	mStop = false;
	LeaveCritical();
	Running();
	Stopping();
	((GrapaThreadPrivate*)vInstanceT)->mThreadId = 0LL;
	((GrapaThreadPrivate*)vInstanceT)->mThread = 0LL;
}

GrapaError GrapaThread::StartSync()
{
#ifdef _WIN32
	if (((GrapaThreadPrivate*)vInstanceT)->mThread) return(-1);
	mStop = true;
	mSync = true;
	Starting();
	WaitCritical();
	mError = 0;
	((GrapaThreadPrivate*)vInstanceT)->mThread = CreateThread(NULL, 100 * 1024000, ((GrapaThreadPrivate*)vInstanceT)->StaticRun, (void*)this, STACK_SIZE_PARAM_IS_A_RESERVATION, &((GrapaThreadPrivate*)vInstanceT)->mThreadId);
	if (((GrapaThreadPrivate*)vInstanceT)->mThread == 0LL) { ((GrapaThreadPrivate*)vInstanceT)->mThreadId = 0LL; return(-1); }
	//LeaveCritical();
	WaitCondition();
	while(!mStop)
		Sleep(10);
#else
	mStop = false;
	mSync = true;
	((GrapaThreadPrivate*)vInstanceT)->mThreadId = 0LL;
	((GrapaThreadPrivate*)vInstanceT)->mThread = 0LL;
	Starting();
	RunFromStatic();
	mSync = false;
#endif
	return(0);
}

void GrapaThread::Stop()
{
	mStop = true;
	if (mSync) return;
	if (!Started()) return;
	Resume();
	mRunning.WaitCritical();
	if (((GrapaThreadPrivate*)vInstanceT)->mThread)
	{
#if defined(__MINGW32__) || defined(__GNUC__)
        int err = 0;
        err = pthread_cancel(((GrapaThreadPrivate*)vInstanceT)->mThread);
#else
#ifdef _WIN32
		BOOL closed = CloseHandle(((GrapaThreadPrivate*)vInstanceT)->mThread);
#endif
#endif
		((GrapaThreadPrivate*)vInstanceT)->mThread = 0LL;
		((GrapaThreadPrivate*)vInstanceT)->mThreadId = 0;
	}
	mRunning.LeaveCritical();
}

bool GrapaThread::Started()
{
	if (((GrapaThreadPrivate*)vInstanceT)->mThread) return(true);
	return(false);
}

//http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

GrapaError GrapaThread::Suspend()
{
	GrapaError err = 0;
	if (((GrapaThreadPrivate*)vInstanceT)->mThread == 0LL) return(-1);
	if (mSync) return(-1);
	mSuspend.WaitCritical();
	mSuspended = true;
	mSuspend.WaitCondition();
	mSuspend.WaitCritical();
	mSuspend.LeaveCritical();
	return(err);
}

GrapaError GrapaThread::Resume()
{
	GrapaError err = 0;
	if (((GrapaThreadPrivate*)vInstanceT)->mThread == 0LL) return(-1);
	if (mSync) return(-1);
	//if (!mSuspend.mWaitCount) return(-1);
	mSuspend.WaitCritical();
	mSuspend.SendCondition();
	mSuspended = false;
	mSuspend.LeaveCritical();
	return(err);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
