// GrapaSystem.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaSystem_
#define _GrapaSystem_

#include "GrapaConsole.h"
#include "GrapaState.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Button.H>

#include <list>

class My_Text_Console;

class GrapaSystem
{
public:
	bool mStop, mLinkInitialized;
	GrapaCHAR mBinName, mBinDir, mWorkDir, mLibDir, mHomeDir, mTempDir, mGrammar;
	GrapaRuleQueue *mPath;
	GrapaRuleQueue* mVersion;
	GrapaRuleQueue* mStaticLib;
	GrapaRuleQueue* mArgv;
	GrapaCHAR mArgcin;
	time_t mStartTime;
	clock_t mStartClock;
	GrapaGroupQueue mGroupQueue;
	//GrapaConsoleSend mConsoleConnect;
	//GrapaConsoleResponse mConsoleResponse;
	GrapaLibraryQueue mLibraryQueue;
	GrapaRuleQueue mLibLockQueue;
	GrapaRuleEvent mLib;
	GrapaCritical mLibLock;
	GrapaCritical mTimeLock;
	GrapaCritical mChdLock;
	std::list < My_Text_Console* > mConsoleList;

public:
	GrapaSystem();
	~GrapaSystem();
public:
	//void Print(const char* pStr, bool flush = true);
	//void Print(const char pChar, bool flush = true);
	//void Print(const GrapaCHAR& pValue, bool flush = true);
	//void PrintLine(const GrapaCHAR& pValue, bool flush = true);
	//void Print(const GrapaSU64& pValue, bool flush = true);
	//void Print(u64 pValue, bool flush = true);
	void PrintTimeLine(const char* pChar, bool flush = true);
	int GetChar();
	void SetEcho(bool enable = true);
	void SetLineInput(bool enable = true);
	void RandSeed(u64 pSeed = 0);
	u32 Random32() { return(((rand() & 0xFF) << 24) | ((rand() & 0xFFF) << 12) | (rand() & 0xFFF)); }
	GrapaCHAR exec(GrapaCHAR& command);

private:
	GrapaCritical mPrintLock;
#ifdef WIN32
	HANDLE mStdinRef;
	DWORD mStdinMode;
#else
	int mStdinRef;
	struct termios mStdinMode;
#endif
};

////////////////////////////////////////////////////////////////////////////////

#endif //_GrapaSystem_
