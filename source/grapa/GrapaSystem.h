// GrapaSystem.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaSystem_
#define _GrapaSystem_

#include "GrapaConsole.h"
#include "GrapaConsole.h"
#include "GrapaLink.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Button.H>

#include <list>

class GrapaConsole2Response : public GrapaConsoleResponse
{
public:
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
	virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
	virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};

class My_Console
{
public:
	GrapaConsoleSend mConsoleSend;
	GrapaConsole2Response mConsole2Response;
	GrapaNames mRuleVariables;
	My_Console();
	~My_Console();
	void Start(GrapaCHAR& in, GrapaCHAR& out, GrapaCHAR& run);
	void Stop();
	void Run(GrapaCB cb, void* data);
};

class My_Pack : public Fl_Pack
{
public:
	My_Pack(int x, int y, int w, int h, const char* l = 0);
};

class My_Text_Editor : public Fl_Text_Editor
{
public:
	GrapaConsoleSend* vConsoleSend;
	Fl_Text_Buffer* vBuff;
	int oldB;
	My_Text_Editor(int X, int Y, int W, int H, const char* l = 0);
	~My_Text_Editor();
	virtual int handle(int event);
	virtual void cleartext();
	virtual void sendtext();
	virtual void sendtext(GrapaCHAR buf);
	virtual void resize(int X, int Y, int W, int H);
	//static void buffer_modified_cb2(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg);
};

class My_Text_Display : public Fl_Text_Display
{
public:
	Fl_Text_Buffer* vBuff;
	My_Text_Display(int X, int Y, int W, int H, const char* l = 0);
	~My_Text_Display();
	virtual int handle(int event);
	virtual void resize(int X, int Y, int W, int H);
};

class My_Button : public Fl_Button
{
public:
	My_Button(int X, int Y, int W, int H, const char* l = 0);
	static void add(Fl_Widget* item, void* data);
	static void del(Fl_Widget* item, void* data);
};

class GrapaEditorResponse : public GrapaConsoleResponse
{
public:
	Fl_Text_Display* disp;
	GrapaNames mRuleVariables;
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
	virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
	virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};

class My_Prompt_Editor;

class GrapaPromptResponse : public GrapaConsoleResponse
{
public:
	My_Prompt_Editor* disp;
	GrapaNames mRuleVariables;
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
	virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
	virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};

class My_Text_Console;

class My_Prompt_Editor : public Fl_Text_Editor
{
public:
	My_Text_Console* vWin;
	GrapaConsoleSend* vConsoleSend;
	GrapaCHAR mLastPrompt;
	GrapaConsoleSend mPromptSend;
	GrapaPromptResponse mPromptResponse;
	Fl_Text_Buffer* vBuff;
	My_Prompt_Editor(int X, int Y, int W, int H, GrapaNames* pNameSpace, My_Text_Console* pWin);
	~My_Prompt_Editor();
	virtual int handle(int event);
	virtual void sendtext();
	//static void buffer_modified_cb2(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg);
};

class My_Group : public My_Pack
{
public:
	GrapaConsoleSend mConsoleSend;
	GrapaEditorResponse mConsoleResponse;
	My_Text_Editor* edit;
	My_Text_Display* disp;
	My_Group(int X, int Y, int W, int H, GrapaNames* pNameSpace);
	~My_Group();
};

class My_ResizableScroll : public Fl_Scroll
{
public:
	My_ResizableScroll(int X, int Y, int W, int H);
	virtual void resize(int X, int Y, int W, int H);
};

class My_Text_Console : public Fl_Double_Window
{
public:
	GrapaNames mRuleVariables;
	My_ResizableScroll* scr;
	My_Pack* pk;
	My_Group* grp;
	My_Prompt_Editor* prompt;
	My_Text_Console();
	My_Text_Console(GrapaCHAR& in, GrapaCHAR& out, GrapaCHAR& run);
	~My_Text_Console();
	virtual void hide();
	void Run(GrapaCB cb, void* data);
	void ExecInFocus(const char* in);
};

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
