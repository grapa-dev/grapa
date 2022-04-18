// GrapaWidget.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaWidget_
#define _GrapaWidget_

#include "GrapaState.h"
#include "GrapaConsole.h"
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_JPEG_Image.H>
#include <list>

class GrapaWidget;

class GrapaWidgetLock : public GrapaCritical
{
public:
	GrapaWidget* widget;
	const char* widgetName;
	s64 x, y, w, h;
	const char* label;
	GrapaRuleEvent* attr;
	GrapaError err;
};

class GrapaWidgetHandleLock : public GrapaCritical
{
public:
	GrapaWidget* widget;
	int event, result;
	GrapaError err;
};

class GrapaWidgetThread : public GrapaConsoleResponse
{
public:
	GrapaWidget* widget;
	GrapaConsoleSend mSend;
	GrapaScriptExec mScriptExec;
	GrapaWorker mWorker;
	GrapaRuleEvent* startCB;
	GrapaRuleEvent* messageCB;
	GrapaRuleEvent* echoCB;
	GrapaRuleEvent* promptCB;
	GrapaRuleEvent* endCB;
	GrapaWidgetThread();
	~GrapaWidgetThread();
	virtual void SendStart(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace);
	virtual GrapaRuleEvent* SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pMessage);
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
	virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
	virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};

struct GrapaWidgetMenuList {
public:
	GrapaCHAR mName;
	GrapaCHAR mPath;
	GrapaRuleEvent* vCallback;
	GrapaRuleEvent* vData;
};

class GrapaWidget
{
public:
	GrapaScriptExec* vScriptExec;
	bool mNameSet, mThreadSet;
	GrapaNames mNameSpace;
	GrapaRuleEvent mNameQueue;
	GrapaNames* vNameSpace;
	GrapaWidgetThread mThread;
	GrapaWidgetThread* vThread;
	bool mDelete;
	bool mHasChildren;
	GrapaWidgetLock mLock;
	GrapaWidgetHandleLock mHandleLock;
	GrapaRuleEvent* vEvent;
	GrapaRuleEvent* vPtrEvent;
	Fl_Widget* vWidget;
	GrapaWidget* vParent;
	GrapaCHAR mName,mWidgetName,mLabel,mResizable;
	GrapaRuleEvent* vOpHide;
	std::list <GrapaWidgetMenuList*> mOpListMenu;
	Fl_JPEG_Image* mJpeg;
	Fl_Image* mJpegImage;
	Fl_Image* mJpegDeImage;
	GrapaWidget(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace);
	~GrapaWidget();
	virtual void CLEAR();
	virtual void TO(GrapaBYTE& pValue);
	GrapaWidget& operator=(const GrapaWidget& that);
	bool FROM(const GrapaWidget& pData);
	void FROM(const GrapaBYTE& pValue);
	GrapaError New(const char* widget, s64 x = 0, s64 y = 0, s64 w = 1, s64 h = 1, const char* label = NULL, GrapaRuleEvent* attr = NULL);
	GrapaError SetNamesChildren(Fl_Group* g, GrapaNames* pNameSpace);
	GrapaError SetThreadChildren(Fl_Group* g, GrapaWidgetThread* pThread);
	GrapaError Add(GrapaCHAR& pName, GrapaWidget* bi, GrapaWidget* b2=NULL);
	void Add(GrapaRuleEvent* pList, GrapaRuleEvent* pAt=NULL);
	GrapaError Extend(GrapaWidget* bi);
	GrapaError Remove(GrapaRuleEvent *pList);
	GrapaError Remove(s64 pIndex);
	GrapaError Join(GrapaRuleEvent* pList);
	static void CloseCB(void* b);
	static void ShowCB(void* b);
	GrapaError Show();
	static void HideCB(void* b);
	GrapaError Hide();
	GrapaError Redraw();
	static void ResizableCB(void* b);
	GrapaError Resizable(GrapaWidget* bi);
	static void ResizeCB(void* b);
	GrapaError Resize(s64 x = 0, s64 y = 0, s64 w = 1, s64 h = 1);
	GrapaError Handle(int event, int& r);
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue);
	GrapaError Set(GrapaCHAR& pName, GrapaCHAR& pValue);
	GrapaError EventKey(int key, int& r);
	GrapaError Append(GrapaCHAR& pValue);
	static void ExecCB(void* b);
	GrapaRuleEvent* Child(GrapaCHAR& pValue);
	GrapaRuleEvent* Child(s64 pIndex=0);
	GrapaRuleEvent* Set(GrapaRuleEvent* data);
	GrapaRuleEvent* Get(GrapaRuleEvent* data);
	unsigned int TableColor(GrapaCHAR& pValue);
	unsigned int TableFont(GrapaCHAR& pValue);
	unsigned int TableCursorStyle(GrapaCHAR& pValue);
	GrapaError Post(GrapaRuleEvent* pRunCode, GrapaRuleEvent* pParam, GrapaRuleEvent* pDoneCode);
	void SetThread();
	void Clear();
	GrapaWidgetMenuList* MenuFromName(char* pName);
	GrapaWidgetMenuList* MenuFromPath(char* pLabel);
	void MenuDeleteFromName(GrapaCHAR& pName);
	void MenuDeleteFromPath(GrapaCHAR& pLabel);
};

#endif //_GrapaWidget_

////////////////////////////////////////////////////////////////////////////////
