// GrapaWidget.cpp
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

#include "GrapaWidget.h"

#include "GrapaSystem.h"

extern GrapaSystem* gSystem;

///////////////////////////////////////////////////////////////////////////////////////////////////

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Image.H>
#include <FL/x.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_JPEG_Image.H>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__MINGW32__) || defined(__GNUC__)
#else
#ifdef _WIN32
#pragma comment (lib, "fl-lib/win/fltk.lib")
#pragma comment (lib, "fl-lib/win/fltk_forms.lib")
#pragma comment (lib, "fl-lib/win/fltk_gl.lib")
#pragma comment (lib, "fl-lib/win/fltk_images.lib")
#pragma comment (lib, "fl-lib/win/fltk_jpeg.lib")
#pragma comment (lib, "fl-lib/win/fltk_png.lib")
#pragma comment (lib, "fl-lib/win/fltk_z.lib")

#pragma comment (lib, "gdiplus.lib")
#pragma comment (lib, "ComCtl32.lib")
#endif
#endif

bool gGrapaWidgetMainThread = false;
bool gGrapaWidgetInEventThread = false;

struct GrapaExecCB
{
	GrapaWidget* vWidget;
	GrapaRuleEvent* vCmd;
	GrapaScriptExec* vScriptExec;
	GrapaNames* vNameSpace;
};

struct GrapaResizeCB
{
	GrapaWidget* vWidget;
	GrapaScriptExec* vScriptExec;
	GrapaNames* vNameSpace;
	int x, y, w, h;
};

class Grapa_Widget_Helper
{
public:
	GrapaWidget* vGrapaWidget;
	GrapaRuleEvent* vOpHandle_mousewheel;
	GrapaRuleEvent* vOpHandle_keydown;
	GrapaRuleEvent* vOpHandle_drag;
	GrapaRuleEvent* vOpHandle_push;
	GrapaRuleEvent* vOpHandle_release;
	GrapaRuleEvent* vOpWhen_release;
	GrapaRuleEvent* vOpResize;
	GrapaRuleEvent* vOpTable_drawcell;
	bool mInHandle;
	bool mInResize;
	Grapa_Widget_Helper() {
		vGrapaWidget = NULL;
		vOpHandle_mousewheel = NULL;
		vOpHandle_keydown = NULL;
		vOpHandle_drag = NULL;
		vOpHandle_push = NULL;
		vOpHandle_release = NULL;
		vOpWhen_release = NULL;
		vOpResize = NULL;
		vOpTable_drawcell = NULL;
		mInHandle = false;
		mInResize = false;
	};
	~Grapa_Widget_Helper() {
		if (vOpHandle_mousewheel)
		{
			vOpHandle_mousewheel->CLEAR();
			delete vOpHandle_mousewheel;
			vOpHandle_mousewheel = NULL;
		}
		if (vOpHandle_keydown)
		{
			vOpHandle_keydown->CLEAR();
			delete vOpHandle_keydown;
			vOpHandle_keydown = NULL;
		}
		if (vOpHandle_drag)
		{
			vOpHandle_drag->CLEAR();
			delete vOpHandle_drag;
			vOpHandle_drag = NULL;
		}
		if (vOpHandle_push)
		{
			vOpHandle_push->CLEAR();
			delete vOpHandle_push;
			vOpHandle_push = NULL;
		}
		if (vOpHandle_release)
		{
			vOpHandle_release->CLEAR();
			delete vOpHandle_release;
			vOpHandle_release = NULL;
		}
		if (vOpWhen_release)
		{
			vOpWhen_release->CLEAR();
			delete vOpWhen_release;
			vOpWhen_release = NULL;
		}
		if (vOpResize)
		{
			vOpResize->CLEAR();
			delete vOpResize;
			vOpResize = NULL;
		}
		if (vOpTable_drawcell)
		{
			vOpTable_drawcell->CLEAR();
			delete vOpTable_drawcell;
			vOpTable_drawcell = NULL;
		}
		mInHandle = false;
		mInResize = false;
	};
	void del_group(GrapaWidget* w)
	{
		if (w->vWidget)
		{
			Fl_Group* g = w->vWidget->as_group();
			if (g)
			{
				while (g->children())
				{
					Fl_Widget* t = g->child(0);
					g->remove(0);
					GrapaWidget* x = (GrapaWidget*)t->user_data();
					if (x && x->mDelete)
						delete t;
				}
			}
		}
	};
	void del(Fl_Widget* wg)
	{
		GrapaWidget* w = (GrapaWidget*)wg->user_data();
		if (w && w->mDelete)
		{
			del_group(w);
			w->vWidget = NULL;
			delete w;
			wg->user_data(NULL);
		};
	}
};
class Grapa_Box : public Fl_Box
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Box(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Box(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); };
	~Grapa_Box()
	{
		wh.del(this);
	};
};
class Grapa_Window : public Fl_Window
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Window(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Window(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); pWidget->mHasChildren = true; };
	~Grapa_Window()
	{
		wh.del(this);
	};
	virtual void hide()
	{
		GrapaWidget* gw = (GrapaWidget*)user_data();;
		Fl_Window::hide();
		if (gw && gw->vOpHide)
		{
			GrapaExecCB* cb = new GrapaExecCB();
			cb->vWidget = gw;
			cb->vCmd = gw->vScriptExec->CopyItem(gw->vOpHide);
			cb->vScriptExec = gw->vScriptExec;
			cb->vNameSpace = gw->vNameSpace;
			while (cb->vNameSpace->GetParrent()) cb->vNameSpace = cb->vNameSpace->GetParrent();
			Fl::awake(GrapaWidget::ExecCB, cb);
		}
	}
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		if (pName.StrLowerCmp("name") == 0)
		{
			GrapaWidget* gw = (GrapaWidget*)user_data();;
			pValue.FROM(gw->mName);
		}
		else if (pName.StrLowerCmp("visible") == 0)
		{
			pValue.FROM(GrapaInt(visible()).getBytes());
		}
		else if (pName.StrLowerCmp("x") == 0)
		{
			pValue.FROM(GrapaInt(x()).getBytes());
		}
		else if (pName.StrLowerCmp("y") == 0)
		{
			pValue.FROM(GrapaInt(y()).getBytes());
		}
		else if (pName.StrLowerCmp("w") == 0)
		{
			pValue.FROM(GrapaInt(w()).getBytes());
		}
		else if (pName.StrLowerCmp("h") == 0)
		{
			pValue.FROM(GrapaInt(h()).getBytes());
		}
		else return -1;
		return 0;
	}
	virtual int handle(int event) {
		if (wh.mInHandle || (wh.vOpHandle_mousewheel == NULL && wh.vOpHandle_keydown == NULL))
		{
			return Fl_Window::handle(event);
		}
		int handled = 0;
		switch (event) {
		default:
			handled = Fl_Window::handle(event);
			break;
		}
		return(handled);
	}
	virtual void resize(int X, int Y, int W, int H)
	{
		if (wh.mInResize)
		{
			Fl_Window::resize(X, Y, W, H);
			return;
		}
		wh.mInResize = true;
		Fl_Window::resize(X, Y, W, H);
		wh.mInResize = false;
	}
};
class Grapa_Double_Window : public Fl_Double_Window
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Double_Window(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Double_Window(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); pWidget->mHasChildren = true;};
	~Grapa_Double_Window()
	{
		wh.del(this);
	};
	virtual void hide()
	{
		GrapaWidget* gw = (GrapaWidget*)user_data();;
		Fl_Double_Window::hide();
		if (gw && gw->vOpHide)
		{
			GrapaExecCB* cb = new GrapaExecCB();
			cb->vWidget = gw;
			cb->vCmd = gw->vScriptExec->CopyItem(gw->vOpHide);
			cb->vScriptExec = gw->vScriptExec;
			cb->vNameSpace = gw->vNameSpace;
			while (cb->vNameSpace->GetParrent()) cb->vNameSpace = cb->vNameSpace->GetParrent();
			Fl::awake(GrapaWidget::ExecCB, cb);
		}
	}
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		return -1;
	}
	virtual int handle(int event) {
		if (wh.mInHandle || (wh.vOpHandle_mousewheel == NULL && wh.vOpHandle_keydown == NULL))
		{
			return Fl_Double_Window::handle(event);
		}
		int handled = 0;
		switch (event) {
		default:
			handled = Fl_Double_Window::handle(event);
			break;
		}
		return(handled);
	}
	virtual void resize(int X, int Y, int W, int H)
	{
		if (wh.mInResize)
		{
			Fl_Double_Window::resize(X, Y, W, H);
			return;
		}
		wh.mInResize = true;
		Fl_Double_Window::resize(X, Y, W, H);
		wh.mInResize = false;
	}
};

#include <FL/Fl_Hor_Nice_Slider.H>

class Grapa_Hor_Nice_Slider : public Fl_Hor_Nice_Slider
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Hor_Nice_Slider(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Hor_Nice_Slider(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); };
	~Grapa_Hor_Nice_Slider()
	{
		wh.del(this);
	};
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		return -1;
	}
};
class Grapa_Scroll : public Fl_Scroll
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Scroll(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Scroll(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); };
	~Grapa_Scroll()
	{
		wh.del(this);
	};
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		return -1;
	}
};
class Grapa_Scrollbar : public Fl_Scrollbar
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Scrollbar(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Scrollbar(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget);  pWidget->mHasChildren = true;};
	~Grapa_Scrollbar()
	{
		wh.del(this);
	};
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		return -1;
	}
	virtual int handle(int event) {
		int handled = 0;
		if (visible() && !wh.mInHandle && ((event == FL_RELEASE && wh.vOpHandle_release)))
		{
			wh.mInHandle = true;
			GrapaRuleQueue q;
			GrapaRuleEvent wid;
			//wid.mName.FROM("o");
			wid.mValue.mToken = GrapaTokenType::WIDGET;
			wid.vWidget = (GrapaWidget*)user_data();
			wid.vClass = wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
			q.PushTail(&wid);
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(event).getBytes()));
			GrapaRuleEvent* e = NULL;
			bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
			gGrapaWidgetInEventThread = true;
			if (wh.vGrapaWidget->vThread)
			{
				GrapaNames n;
				n.SetParrent(&wh.vGrapaWidget->mNameSpace);
				if (event == FL_RELEASE && wh.vOpHandle_release)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_release, &wid);
			}
			else
			{
				GrapaNames n;
				n.SetParrent(wh.vGrapaWidget->vNameSpace);
				if (event == FL_RELEASE && wh.vOpHandle_release)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_release, &wid);
			}
			gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
			if (e)
			{
				GrapaInt d;
				d.FromBytes(e->mValue);
				handled = d.LongValue();
				delete e;
			}
			//wid.vClass = NULL;
			wid.vWidget = NULL;
			q.PopHead();
			q.CLEAR();
			wh.mInHandle = false;
		}
		else
		{
			handled = Fl_Scrollbar::handle(event);
		}
		return(handled);
	}
};
class Grapa_Resizable_Scroll : public Grapa_Scroll
{
public:
	Grapa_Resizable_Scroll(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Grapa_Scroll(pWidget, X, Y, W, H, l) { };
	virtual void resize(int X, int Y, int W, int H) {
		Fl::lock();
		Fl_Group::resize(X, Y, W, H);
		Fl::unlock();
	};
};
class Grapa_Button : public Fl_Button
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Button(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Button(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); };
	~Grapa_Button()
	{
		wh.del(this);
	};
	static void When(Fl_Widget* pItem)
	{
		Grapa_Button* item = (Grapa_Button*)pItem;
		if (pItem->visible() && item->wh.vOpWhen_release)
		{
			GrapaRuleQueue q;
			GrapaRuleEvent wid;
			//wid.mName.FROM("o");
			wid.mValue.mToken = GrapaTokenType::WIDGET;
			wid.vWidget = (GrapaWidget*)item->user_data();
			wid.vClass = item->wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(item->wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
			q.PushTail(&wid);
			GrapaRuleEvent* e = NULL;
			bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
			gGrapaWidgetInEventThread = true;
			GrapaNames n;
			n.SetParrent(item->wh.vGrapaWidget->vNameSpace);
			e = item->wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, item->wh.vOpWhen_release, &wid);
			gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
			if (e) delete e;
			wid.vWidget = NULL;
			q.PopHead();
			q.CLEAR();
		}
	}
};
class Grapa_Radio_Button : public Fl_Radio_Button
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Radio_Button(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Radio_Button(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); };
	~Grapa_Radio_Button()
	{
		wh.del(this);
	};
};
class Grapa_Toggle_Button : public Fl_Toggle_Button
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Toggle_Button(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Toggle_Button(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); };
	~Grapa_Toggle_Button()
	{
		wh.del(this);
	};
};
class Grapa_Text_Editor : public Fl_Text_Editor
{
public:
	Grapa_Widget_Helper wh;
	int mEventButton;
	Grapa_Text_Editor(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Text_Editor(X, Y, W, H, l) { mEventButton = 0;  wh.vGrapaWidget = pWidget; user_data(pWidget); buffer(new Fl_Text_Buffer()); };
	~Grapa_Text_Editor()
	{
		wh.del(this);
	};
	void calc_last_char() { Fl_Text_Display::calc_last_char(); };
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		if (pName.StrLowerCmp("mNBufferLines") == 0)
			pValue.FROM(GrapaInt(mNBufferLines).getBytes());
		else if (pName.StrLowerCmp("mMaxsize") == 0)
			pValue.FROM(GrapaInt(mMaxsize).getBytes());
		else if (pName.StrLowerCmp("text") == 0)
		{
			char* s = buffer()->text();
			pValue.FROM(s);
			free(s);
		}
		else if (pName.StrLowerCmp("selection") == 0)
		{
			char* s = buffer()->selection_text();
			pValue.FROM(s);
			free(s);
		}
		else if (pName.StrLowerCmp("start") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(start).getBytes());
		}
		else if (pName.StrLowerCmp("end") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(end).getBytes());
		}
		else if (pName.StrLowerCmp("row") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(buffer()->count_lines(0, start)).getBytes());
		}
		else if (pName.StrLowerCmp("col") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(start-buffer()->line_start(start)).getBytes());
		}
		else return -1;
		return 0;
	}
	GrapaError Set(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		if (pName.StrLowerCmp("cursor_color") == 0)
		{
			cursor_color(wh.vGrapaWidget->TableColor(pName));
		}
		else if (pName.StrLowerCmp("cursor_style") == 0)
		{
			cursor_style(wh.vGrapaWidget->TableCursorStyle(pName));
		}
		else if (pName.StrLowerCmp("cursor_visible") == 0)
		{
			if (pValue.mLength == 0 || *pValue.mBytes == 0 || pValue.StrLowerCmp("false") == 0)
				hide_cursor();
			else
				show_cursor();
		}
		else return -1;
		return 0;
	}
	virtual int handle(int event) {
		int handled = 0;
		if (visible() && !wh.mInHandle && ((event == FL_MOUSEWHEEL && wh.vOpHandle_mousewheel) || (event == FL_KEYDOWN && wh.vOpHandle_keydown)))
		{
			wh.mInHandle = true;
			GrapaRuleQueue q;
			GrapaRuleEvent wid;
			//wid.mName.FROM("o");
			wid.mValue.mToken = GrapaTokenType::WIDGET;
			wid.vWidget = (GrapaWidget*)user_data(); 
			wid.vClass = wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
			q.PushTail(&wid);
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(event).getBytes()));
			GrapaRuleEvent* e = NULL;
			bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
			gGrapaWidgetInEventThread = true;

			if (wh.vGrapaWidget->vThread)
			{
				GrapaNames n;
				n.SetParrent(&wh.vGrapaWidget->mNameSpace);
				if (event == FL_KEYDOWN && wh.vOpHandle_keydown)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_keydown, &wid);
				else if (event == FL_MOUSEWHEEL && wh.vOpHandle_mousewheel)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_mousewheel, &wid);
			}
			else
			{
				GrapaNames n;
				n.SetParrent(wh.vGrapaWidget->vNameSpace);
				if (event == FL_KEYDOWN && wh.vOpHandle_keydown)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_keydown, &wid);
				else if (event == FL_MOUSEWHEEL && wh.vOpHandle_mousewheel)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_mousewheel, &wid);
			}


			gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;

			if (e)
			{
				GrapaInt d;
				d.FromBytes(e->mValue);
				handled = d.LongValue();
				delete e;
			}
			//wid.vClass = NULL;
			wid.vWidget = NULL;
			q.PopHead();
			q.CLEAR();
			wh.mInHandle = false;
		}
		else if (visible() && !wh.mInHandle && ((event == FL_PUSH && wh.vOpHandle_push) || (event == FL_RELEASE && wh.vOpHandle_release) || (event == FL_DRAG && wh.vOpHandle_drag)))
		{
			wh.mInHandle = true;
			GrapaRuleQueue q;
			GrapaRuleEvent wid;
			wid.mValue.mToken = GrapaTokenType::WIDGET;
			wid.vWidget = (GrapaWidget*)user_data();
			wid.vClass = wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
			q.PushTail(&wid);
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(event).getBytes()));
			GrapaCHAR eb;
			int ebval = Fl::event_button();
			if (event == FL_DRAG) ebval = mEventButton;
			else mEventButton = ebval;
			switch (Fl::event_button())
			{
			case FL_LEFT_MOUSE: eb.FROM("LEFT"); break;
			case FL_MIDDLE_MOUSE: eb.FROM("MIDDLE"); break;
			case FL_RIGHT_MOUSE: eb.FROM("RIGHT"); break;
			}
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), eb));
			GrapaRuleEvent* ek = new GrapaRuleEvent();
			ek->mValue.mToken = GrapaTokenType::ARRAY;
			ek->vQueue = new GrapaRuleQueue();
			int ekval = Fl::event_state();
			if (ekval & FL_SHIFT) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("SHIFT")));
			if (ekval & FL_CAPS_LOCK) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("CAPS_LOCK")));
			if (ekval & FL_CTRL) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("CTRL")));
			if (ekval & FL_ALT) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("ALT")));
			if (ekval & FL_NUM_LOCK) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("NUM_LOCK")));
			if (ekval & FL_META) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("META")));
			if (ekval & FL_SCROLL_LOCK) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("SCROLL_LOCK")));
			if (ekval & FL_BUTTON1) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("BUTTON1")));
			if (ekval & FL_BUTTON2) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("BUTTON2")));
			if (ekval & FL_BUTTON3) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("BUTTON3")));
			q.PushTail(ek);
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(Fl::event_x()).getBytes()));
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(Fl::event_y()).getBytes()));

			GrapaRuleEvent* e = NULL;
			bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
			gGrapaWidgetInEventThread = true;

			if (wh.vGrapaWidget->vThread)
			{
				GrapaNames n;
				n.SetParrent(&wh.vGrapaWidget->mNameSpace);
				if (event == FL_DRAG)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_drag, &wid);
				else if (event == FL_PUSH)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_push, &wid);
				else if (event == FL_RELEASE)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_release, &wid);
			}
			else
			{
				GrapaNames n;
				n.SetParrent(wh.vGrapaWidget->vNameSpace);
				if (event == FL_DRAG)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_drag, &wid);
				else if (event == FL_PUSH)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_push, &wid);
				else if (event == FL_RELEASE)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_release, &wid);
			}

			gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
			if (e)
			{
				GrapaInt d;
				d.FromBytes(e->mValue);
				handled = d.LongValue();
				delete e;
			}
			wid.vWidget = NULL;
			q.PopHead();
			q.CLEAR();
			wh.mInHandle = false;
		}
		else
		{
			handled = Fl_Text_Editor::handle(event);
		}
		//int handled = 0;
		//switch (event) {
		//default:
		//	handled = Fl_Text_Editor::handle(event);
		//	break;
		//}
		return(handled);
	}
	virtual void resize(int X, int Y, int W, int H)
	{
		if (!visible() || wh.mInResize || (wh.vOpResize == NULL || wh.vOpResize->mValue.mToken != GrapaTokenType::OP))
		{
			Fl_Text_Editor::resize(X, Y, W, H);
			return;
		}
		wh.mInResize = true;
		//Fl_Text_Editor::resize(X, Y, W, H);
		//if (wh.vOpResize && H != ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize)
		//{
		//   Fl_Text_Editor::resize(X, Y, W, ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize);
		//   if (parent())
		//	   parent()->parent()->parent()->parent()->redraw();
		//}
		//return;

		GrapaRuleQueue q;
		GrapaRuleEvent wid;
		//wid.mName.FROM("o");
		wid.mValue.mToken = GrapaTokenType::WIDGET;
		wid.vWidget = (GrapaWidget*)user_data();
		wid.vClass = wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
		q.PushTail(&wid);
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(X).getBytes()));
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(Y).getBytes()));
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(W).getBytes()));
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(H).getBytes()));
		bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
		gGrapaWidgetInEventThread = true;


		Fl::lock();
		GrapaRuleEvent* e = NULL;
		if (wh.vGrapaWidget->vThread)
		{
			GrapaNames n;
			n.SetParrent(&wh.vGrapaWidget->mNameSpace);
			GrapaRuleEvent* e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpResize, &wid);
		}
		else
		{
			GrapaNames n;
			n.SetParrent(wh.vGrapaWidget->vNameSpace);
			GrapaRuleEvent* e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpResize, &wid);
		}
		Fl::unlock();


		gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
		if (e) delete e;
		wid.vWidget = NULL;
		q.PopHead();
		q.CLEAR();

		//if (parent())
		//	parent()->parent()->parent()->parent()->redraw();
		wh.mInResize = false;
	}
};
class Grapa_Text_Display : public Fl_Text_Display
{
public:
	Grapa_Widget_Helper wh;
	int mEventButton;
	Grapa_Text_Display(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Text_Display(X, Y, W, H, l) { mEventButton = 0; wh.vGrapaWidget = pWidget; user_data(pWidget); buffer(new Fl_Text_Buffer()); };
	~Grapa_Text_Display()
	{
		wh.del(this);
	};
	void calc_last_char() { Fl_Text_Display::calc_last_char(); };
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		if (pName.StrLowerCmp("mNBufferLines") == 0)
			pValue.FROM(GrapaInt(mNBufferLines).getBytes());
		else if (pName.StrLowerCmp("mMaxsize") == 0)
			pValue.FROM(GrapaInt(mMaxsize).getBytes());
		else if (pName.StrLowerCmp("text") == 0)
		{
			char* s = buffer()->text();
			pValue.FROM(s);
			free(s);
		}
		else if (pName.StrLowerCmp("selection") == 0)
		{
			char* s = buffer()->selection_text();
			pValue.FROM(s);
			free(s);
		}
		else if (pName.StrLowerCmp("start") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(start).getBytes());
		}
		else if (pName.StrLowerCmp("end") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(end).getBytes());
		}
		else if (pName.StrLowerCmp("row") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(buffer()->count_lines(0, start)).getBytes());
		}
		else if (pName.StrLowerCmp("col") == 0)
		{
			int start; int end;
			buffer()->selection_position(&start, &end);
			pValue.FROM(GrapaInt(start - buffer()->line_start(start)).getBytes());
		}
		else return -1;
		return 0;
	}
	GrapaError Set(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		if (pName.StrLowerCmp("cursor_color") == 0)
		{
			cursor_color(wh.vGrapaWidget->TableColor(pName));
		}
		else if (pName.StrLowerCmp("cursor_style") == 0)
		{
			cursor_style(wh.vGrapaWidget->TableCursorStyle(pName));
		}
		else if (pName.StrLowerCmp("cursor_visible") == 0)
		{
			if (pValue.mLength == 0 || *pValue.mBytes == 0 || pValue.StrLowerCmp("false") == 0)
				hide_cursor();
			else
				show_cursor();
		}
		else return -1;
		return 0;
	}
	virtual int handle(int event) {
		int handled = 0;
		if (visible() && !wh.mInHandle && ((event == FL_MOUSEWHEEL && wh.vOpHandle_mousewheel) || (event == FL_KEYDOWN && wh.vOpHandle_keydown)))
		{
			wh.mInHandle = true;
			GrapaRuleQueue q;
			GrapaRuleEvent wid;
			//wid.mName.FROM("o");
			wid.mValue.mToken = GrapaTokenType::WIDGET;
			wid.vWidget = (GrapaWidget*)user_data();
			wid.vClass = wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
			q.PushTail(&wid);
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(event).getBytes()));
			GrapaRuleEvent* e = NULL;
			bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
			gGrapaWidgetInEventThread = true;

			if (wh.vGrapaWidget->vThread)
			{
				GrapaNames n;
				n.SetParrent(&wh.vGrapaWidget->mNameSpace);
				if (event == FL_KEYDOWN && wh.vOpHandle_keydown)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_keydown, &wid);
				else if (event == FL_MOUSEWHEEL && wh.vOpHandle_mousewheel)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_mousewheel, &wid);
			}
			else
			{
				GrapaNames n;
				n.SetParrent(wh.vGrapaWidget->vNameSpace);
				if (event == FL_KEYDOWN && wh.vOpHandle_keydown)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_keydown, &wid);
				else if (event == FL_MOUSEWHEEL && wh.vOpHandle_mousewheel)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_mousewheel, &wid);
			}

			gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
			if (e)
			{
				GrapaInt d;
				d.FromBytes(e->mValue);
				handled = d.LongValue();
				delete e;
			}
			//wid.vClass = NULL;
			wid.vWidget = NULL;
			q.PopHead();
			q.CLEAR();
			wh.mInHandle = false;
		}
		else if (visible() && !wh.mInHandle && ((event == FL_PUSH && wh.vOpHandle_push) || (event == FL_RELEASE && wh.vOpHandle_release) || (event == FL_DRAG && wh.vOpHandle_drag)))
		{
			wh.mInHandle = true;
			GrapaRuleQueue q;
			GrapaRuleEvent wid;
			wid.mValue.mToken = GrapaTokenType::WIDGET;
			wid.vWidget = (GrapaWidget*)user_data();
			wid.vClass = wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
			q.PushTail(&wid);
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(event).getBytes()));
			GrapaCHAR eb;
			int ebval = Fl::event_button();
			if (event == FL_DRAG) ebval = mEventButton;
			else mEventButton = ebval;
			switch (Fl::event_button())
			{
			case FL_LEFT_MOUSE: eb.FROM("LEFT"); break;
			case FL_MIDDLE_MOUSE: eb.FROM("MIDDLE"); break;
			case FL_RIGHT_MOUSE: eb.FROM("RIGHT"); break;
			}
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), eb));
			GrapaRuleEvent* ek = new GrapaRuleEvent();
			ek->mValue.mToken = GrapaTokenType::ARRAY;
			ek->vQueue = new GrapaRuleQueue();
			int ekval = Fl::event_state();
			if (ekval & FL_SHIFT) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("SHIFT")));
			if (ekval & FL_CAPS_LOCK) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("CAPS_LOCK")));
			if (ekval & FL_CTRL) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("CTRL")));
			if (ekval & FL_ALT) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("ALT")));
			if (ekval & FL_NUM_LOCK) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("NUM_LOCK")));
			if (ekval & FL_META) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("META")));
			if (ekval & FL_SCROLL_LOCK) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("SCROLL_LOCK")));
			if (ekval & FL_BUTTON1) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("BUTTON1")));
			if (ekval & FL_BUTTON2) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("BUTTON2")));
			if (ekval & FL_BUTTON3) ek->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("BUTTON3")));
			q.PushTail(ek);
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(Fl::event_x()).getBytes()));
			q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(Fl::event_y()).getBytes()));

			GrapaRuleEvent* e = NULL;
			bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
			gGrapaWidgetInEventThread = true;

			if (wh.vGrapaWidget->vThread)
			{
				GrapaNames n;
				n.SetParrent(&wh.vGrapaWidget->mNameSpace);
				if (event == FL_DRAG)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_drag, &wid);
				else if (event == FL_PUSH)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_push, &wid);
				else if (event == FL_RELEASE)
					e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpHandle_release, &wid);
			}
			else
			{
				GrapaNames n;
				n.SetParrent(wh.vGrapaWidget->vNameSpace);
				if (event == FL_DRAG)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_drag, &wid);
				else if (event == FL_PUSH)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_push, &wid);
				else if (event == FL_RELEASE)
					e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpHandle_release, &wid);
			}

			gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
			if (e)
			{
				GrapaInt d;
				d.FromBytes(e->mValue);
				handled = d.LongValue();
				delete e;
			}
			wid.vWidget = NULL;
			q.PopHead();
			q.CLEAR();
			wh.mInHandle = false;
		}
		else
		{
			handled = Fl_Text_Display::handle(event);
		}
		return(handled);
	}
	virtual void resize(int X, int Y, int W, int H)
	{
		if (!visible() || wh.mInResize || (wh.vOpResize == NULL || wh.vOpResize->mValue.mToken != GrapaTokenType::OP))
		{
			Fl_Text_Display::resize(X, Y, W, H);
			return;
		}

		wh.mInResize = true;
		GrapaRuleQueue q;
		GrapaRuleEvent wid;
		//wid.mName.FROM("o");
		wid.mValue.mToken = GrapaTokenType::WIDGET;
		wid.vWidget = (GrapaWidget*)user_data();
		wid.vClass = wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
		q.PushTail(&wid);
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(X).getBytes()));
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(Y).getBytes()));
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(W).getBytes()));
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(H).getBytes()));
		bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
		gGrapaWidgetInEventThread = true;

		Fl::lock();
		GrapaRuleEvent* e = NULL;
		if (wh.vGrapaWidget->vThread)
		{
			GrapaNames n;
			n.SetParrent(&wh.vGrapaWidget->mNameSpace);
			GrapaRuleEvent* e = wh.vGrapaWidget->vThread->mSend.mScriptState.vScriptExec->ProcessPlan(&n, wh.vOpResize, &wid);
		}
		else
		{
			GrapaNames n;
			n.SetParrent(wh.vGrapaWidget->vNameSpace);
			GrapaRuleEvent* e = wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, wh.vOpResize, &wid);
		}
		Fl::unlock();

		gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
		if (e) delete e;
		//wid.vClass = NULL;
		wid.vWidget = NULL;
		q.PopHead();
		q.CLEAR();
		wh.mInResize = false;
	}
};
class Grapa_Group : public Fl_Group
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Group(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Group(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); pWidget->mHasChildren = true;};
	~Grapa_Group()
	{
		wh.del(this);
	};
};
class Grapa_Pack : public Fl_Pack
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Pack(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Pack(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); pWidget->mHasChildren = true;};
	~Grapa_Pack()
	{
		wh.del(this);
	};
};
class Grapa_Menu_Bar : public Fl_Menu_Bar
{
public:
	Grapa_Widget_Helper wh;
	Grapa_Menu_Bar(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Menu_Bar(X, Y, W, H, l) { wh.vGrapaWidget = pWidget; user_data(pWidget); };
	~Grapa_Menu_Bar()
	{
		wh.del(this);
	};
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		return -1;
	}
	static void Callback(Fl_Widget* w, void* data)
	{
		if (w->visible())
		{
			Grapa_Menu_Bar* item = (Grapa_Menu_Bar*)w;
			const Fl_Menu_Item* value = item->mvalue();
			char ipath[256]; item->item_pathname(ipath, sizeof(ipath));
			s64 index;
			GrapaWidgetMenuList* ml = item->wh.vGrapaWidget->MenuFromPath(ipath);
			GrapaRuleEvent* cb = NULL;
			if (ml) cb = ml->vCallback;
			if (cb)
			{
				GrapaRuleQueue q;
				GrapaRuleEvent wid;
				//wid.mName.FROM("o");
				wid.mValue.mToken = GrapaTokenType::WIDGET;
				wid.vWidget = (GrapaWidget*)item->user_data();
				wid.vClass = item->wh.vGrapaWidget->vScriptExec->vScriptState->GetClass(item->wh.vGrapaWidget->vNameSpace, GrapaCHAR("$WIDGET"));
				q.PushTail(&wid);
				GrapaRuleEvent wid2;
				if (ml->vData == NULL)
					wid2.SetNull();
				else
				{
					wid2.mValue.mToken = GrapaTokenType::PTR;
					wid2.vRulePointer = ml->vData;
					while (wid2.vRulePointer && wid2.vRulePointer->mValue.mToken == GrapaTokenType::PTR && wid2.vRulePointer->vRulePointer) wid2.vRulePointer = wid2.vRulePointer->vRulePointer;
					wid2.mVar = true;
				}
				//wid2.mName.FROM("data");
				q.PushTail(&wid2);
				q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR(ipath)));
				GrapaRuleEvent* e = NULL;
				bool oldGrapaWidgetInEventThread = gGrapaWidgetInEventThread;
				gGrapaWidgetInEventThread = true;
				GrapaNames n;
				n.SetParrent(item->wh.vGrapaWidget->vNameSpace);
				e = item->wh.vGrapaWidget->vScriptExec->ProcessPlan(&n, cb, &wid);
				gGrapaWidgetInEventThread = oldGrapaWidgetInEventThread;
				if (e) delete e;
				wid.vWidget = NULL;
				q.PopHead();
				q.PopHead();
				q.CLEAR();
			}
		}
	}
};

#include "GrapaVector.h"

class Grapa_Table_Row : public Fl_Table_Row
{
public:
	Grapa_Widget_Helper wh;
	GrapaVector* vVector;
	bool mDeleteVector;
	Grapa_Table_Row(GrapaWidget* pWidget, int X, int Y, int W, int H, const char* l = 0) : Fl_Table_Row(X, Y, W, H, l) 
	{
		wh.vGrapaWidget = pWidget; 
		user_data(pWidget); 
		vVector = NULL;
		mDeleteVector = false;
		selection_color(FL_YELLOW);
		//when(FL_WHEN_RELEASE | FL_WHEN_CHANGED);
		table_box(FL_NO_BOX);
		col_resize_min(4);
		row_resize_min(4);
		row_header(1);
		row_header_width(60);
		row_resize(1);
		row_height_all(20);
		col_header(1);
		col_header_height(25);
		col_resize(1);
		col_width_all(80);
	};
	~Grapa_Table_Row()
	{
		wh.del(this);
		if (mDeleteVector && vVector)
			delete vVector;
		mDeleteVector = false;
		vVector = NULL;
	};
	GrapaError Get(GrapaCHAR& pName, GrapaCHAR& pValue)
	{
		return -1;
	};
	void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
		GrapaCHAR s;
		if (vVector && vVector->mDim==2)
		{
			bool hasHeader = false;
			if (context == CONTEXT_COL_HEADER)
			{
				s.FROM(vVector->mLabels.Head(C)->mName);
				hasHeader = true;
			}
			else if (context == CONTEXT_ROW_HEADER)
			{
				s.FROM(GrapaInt(R).ToString());
			}
			if ((!hasHeader && context == CONTEXT_COL_HEADER) || context == CONTEXT_CELL)
			{
				GrapaVectorItem* d1 = ((GrapaVectorItem*)&((u8*)vVector->mData)[vVector->mBlock * (R * vVector->mCounts[1] + C)]);
				GrapaRuleEvent* val = NULL;
				if (d1->isNull)
				{
					val = new GrapaRuleEvent(true);
				}
				else if (d1->isValue)
				{
					GrapaVectorValue* d = 0L;
					memcpy(&d, d1->d, sizeof(GrapaVectorValue*));
					if (d)
					{
						val = d->Get();
					}
					else
					{
						val = new GrapaRuleEvent(true);
					}
				}
				else
				{
					GrapaBYTE z(d1->d, d1->mLen);
					z.mToken = d1->mToken;
					val = new GrapaRuleEvent(0, GrapaCHAR(""), z);
				}
				GrapaSystemSend send;
				send.isActive = false;
				if (val->vQueue) wh.vGrapaWidget->vScriptExec->EchoList(&send, val, false, false, false);
				else wh.vGrapaWidget->vScriptExec->EchoValue(&send, val, false, false, false);
				send.GetStr(s);
			}
		}
		else
		{
			s.SetLength(40);
			sprintf((char*)s.mBytes, "%d/%d", R, C);			  // text for each cell
			s.SetLength(strlen((char*)s.mBytes));
		}
		switch (context) {
		case CONTEXT_STARTPAGE:			 // Fl_Table telling us it's starting to draw page
			fl_font(FL_HELVETICA, 10);
			return;

		case CONTEXT_ROW_HEADER:			// Fl_Table telling us to draw row/col headers
			fl_push_clip(X, Y, W, H);
			{
				fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
				fl_color(FL_BLACK);
				fl_draw((char*)s.mBytes, X, Y, W, H, FL_ALIGN_RIGHT);
			}
			fl_pop_clip();
			return;
		case CONTEXT_COL_HEADER:
			fl_push_clip(X, Y, W, H);
			{
				fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
				fl_color(FL_BLACK);
				fl_draw((char*)s.mBytes, X, Y, W, H, FL_ALIGN_CENTER);
			}
			fl_pop_clip();
			return;

		case CONTEXT_CELL:				  // Fl_Table telling us to draw cells
			fl_push_clip(X, Y, W, H);
			{
				// BG COLOR
				fl_color(row_selected(R) ? selection_color() : FL_WHITE);
				fl_rectf(X, Y, W, H);

				// TEXT
				fl_color(FL_BLACK);
				fl_draw((char*)s.mBytes, X, Y, W, H, FL_ALIGN_CENTER);

				// BORDER
				fl_color(FL_LIGHT2);
				fl_rect(X, Y, W, H);
			}
			fl_pop_clip();
			return;

		case CONTEXT_TABLE:
			//fprintf(stderr, "TABLE CONTEXT CALLED\n");
			return;

		case CONTEXT_ENDPAGE:
		case CONTEXT_RC_RESIZE:
		case CONTEXT_NONE:
			return;
		default:
			return;
		}
		//NOTREACHED
	};
};

GrapaWidget::GrapaWidget(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace)
{
	vEvent = NULL;
	vWidget = NULL;
	vParent = NULL;
	mDelete = false;
	mNameSet = false;
	mThreadSet = false;
	vScriptExec = pScriptExec;
	vThread = NULL;
	vPtrEvent = new GrapaRuleEvent();
	vPtrEvent->mValue.mToken = GrapaTokenType::WIDGET;
	vPtrEvent->vWidget = this;
	vOpHide = NULL;
	mThread.widget = this;
	mHasChildren = false;
	mNameQueue.mValue.mToken = GrapaTokenType::LIST;
	mNameQueue.vQueue = new GrapaRuleQueue();
	mNameSpace.GetNameQueue()->PushTail(&mNameQueue);
	GrapaRuleEvent* e2 = new GrapaRuleEvent();
	e2->mValue.mToken = GrapaTokenType::PTR;
	e2->vRulePointer = vPtrEvent;
	e2->mName.FROM("$self");
	mNameQueue.vQueue->PushTail(e2);
	mNameSpace.mSkip = false;
	mNameSpace.SetParrent(pNameSpace);
	vNameSpace = &mNameSpace;
	mJpeg = NULL;
	mJpegImage = NULL;
	mJpegDeImage = NULL;
}

GrapaWidget::~GrapaWidget()
{
	CLEAR();
	vPtrEvent->vWidget = NULL;
	delete vPtrEvent;
	vPtrEvent = NULL;
	if (vOpHide)
	{
		vOpHide->CLEAR();
		delete vOpHide;
		vOpHide = NULL;
	}
}

void GrapaWidget::CLEAR()
{
	Fl::lock();
	mDelete = false;
	if (vWidget)
	{
		if (mJpeg)
		{
			delete mJpeg;
			mJpeg = NULL;
		}
		if (mJpegImage)
		{
			vWidget->image(NULL);
			delete mJpeg;
			mJpeg = NULL;
		}
		if (mJpegDeImage)
		{
			vWidget->deimage(NULL);
			delete mJpegDeImage;
			mJpegDeImage = NULL;
		}
		if (vParent)
		{
			if (vParent->vWidget)
			{
				GrapaWidget* x = (GrapaWidget*)vParent->vWidget->user_data();
				Fl_Group* g = NULL;
				if (x && x->vWidget) g = x->vWidget->as_group();
				if (g)
				{
					g->remove(vWidget);
					g->redraw();
				}
			}
			vParent = NULL;
		}
		vScriptExec->vScriptState->ClearQueue();
		//vScriptExec->vScriptState->Stop();
		mThread.mScriptExec.vScriptState->ClearQueue();
		mThread.mScriptExec.vScriptState->Stop();
		vWidget->user_data(NULL);
		Fl_Widget* oldwidget = vWidget;
		vWidget = NULL;
		Fl::awake(GrapaWidget::CloseCB, oldwidget);
		//Fl::delete_widget(oldwidget);
	}
	mNameSpace.GetNameQueue()->PopEvent(&mNameQueue);
	mNameSpace.CLEAR();
	Fl::unlock();
}

void GrapaWidget::TO(GrapaBYTE& pValue)
{
	pValue.SetLength(0);
	pValue.mToken = 0;
}

GrapaWidget& GrapaWidget::operator =(const GrapaWidget& that)
{
	if (this != &that)
		FROM(that);
	return *this;
}

bool GrapaWidget::FROM(const GrapaWidget& pData)
{
	GrapaError err = -1;
	CLEAR();
	return(err==0);
}

void GrapaWidget::FROM(const GrapaBYTE& pValue)
{
	CLEAR();
}

GrapaError GrapaWidget::New(const char* widget, s64 x, s64 y, s64 w, s64 h, const char* label, GrapaRuleEvent* attr)
{
	GrapaCHAR cbwidget(widget);
	cbwidget.ToLower();
	if (cbwidget.Cmp("window")
		&& cbwidget.Cmp("double_window")
		&& cbwidget.Cmp("Hor_Nice_Slider")
		&& cbwidget.Cmp("scrollbar")
		&& cbwidget.Cmp("scroll")
		&& cbwidget.Cmp("resizable_scroll")
		&& cbwidget.Cmp("button")
		&& cbwidget.Cmp("radio_button")
		&& cbwidget.Cmp("toggle_button")
		&& cbwidget.Cmp("text_editor")
		&& cbwidget.Cmp("text_display")
		&& cbwidget.Cmp("group")
		&& cbwidget.Cmp("pack")
		&& cbwidget.Cmp("menu_bar")
		&& cbwidget.Cmp("widget")
		&& cbwidget.Cmp("table_row"))
		return -1;

	mWidgetName.FROM(cbwidget);

	Fl::lock();

	if (label&&*label)
		mLabel.FROM(label);
	std::list < Fl_Widget* > children;
	int isVisible = 0;
	int isActive = 0;
	int index = -1;
	bool needsShow = false;
	Fl_Widget* oldWidget = NULL;
	Fl_Group* parent = NULL;
	if (vWidget)
	{
		oldWidget = vWidget;
		isVisible = vWidget->visible();
		isActive = vWidget->active();

		Fl_Group* g = vWidget->as_group();
		if (g)
		{
			while (g->children())
			{
				Fl_Widget* t = g->child(0);
				g->remove(0);
				GrapaWidget* x = (GrapaWidget*)t->user_data();
				if (x) x->vParent = NULL;
				children.push_back(t);
			}
		}
		if (vParent)
		{
			g = vParent->vWidget->as_group();
			if (g)
			{
				index = g->find(vWidget);
				g->remove(vWidget);
				parent = g;
			}
		}
		vWidget = NULL;
	}
	if (mWidgetName.Cmp("widget") == 0 || mWidgetName.Cmp("box") == 0)
	{
		vWidget = new Grapa_Box(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("window") == 0)
	{
		vWidget = new Grapa_Window(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("double_window") == 0)
	{
		vWidget = new Grapa_Double_Window(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("Hor_Nice_Slider") == 0)
	{
		vWidget = new Grapa_Hor_Nice_Slider(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("scroll") == 0)
	{
		vWidget = new Grapa_Scroll(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("scrollbar") == 0)
	{
		vWidget = new Grapa_Scrollbar(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("resizable_scroll") == 0)
	{
		vWidget = new Grapa_Resizable_Scroll(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("button") == 0)
	{
		vWidget = new Grapa_Button(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("radio_button") == 0)
	{
		vWidget = new Grapa_Radio_Button(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("toggle_button") == 0)
	{
		vWidget = new Grapa_Toggle_Button(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("text_editor") == 0)
	{
		vWidget = new Grapa_Text_Editor(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("text_display") == 0)
	{
		vWidget = new Grapa_Text_Display(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("group") == 0)
	{
		vWidget = new Grapa_Group(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("pack") == 0)
	{
		vWidget = new Grapa_Pack(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("menu_bar") == 0)
	{
		vWidget = new Grapa_Menu_Bar(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	else if (mWidgetName.Cmp("table_row") == 0)
	{
		vWidget = new Grapa_Table_Row(this, x, y, w, h, (char*)mLabel.mBytes);
	}
	if (vWidget)
	{
		if (vWidget->as_group()) vWidget->as_group()->end();
		if (parent)
		{
			if (index >= 0)
				parent->insert(*vWidget, index);
			else
				parent->add(vWidget);
		}
		Fl_Group* mg = vWidget->as_group();
		if (mg)
		{
			std::list < Fl_Widget* > vecOfStr(children.begin(), children.end());
			for (Fl_Widget* str : vecOfStr)
			{
				mg->add(str);
			}
		}
		if (isVisible)
		{
			needsShow = true;
			if (isActive && !vWidget->active()) vWidget->activate();
			if (vParent && vParent->vWidget)
				vParent->vWidget->redraw();
			else
				vWidget->redraw();
		}
	}
	else
	{
		vParent = NULL;
	}
	if (attr)
	{
		GrapaRuleEvent* e = Set(attr);
		if (e)
		{
			e->CLEAR();
			delete e;
		}
	}
	if (oldWidget)
	{
		oldWidget->hide();
		oldWidget = NULL;
	 }
	if (vWidget && needsShow)
		Fl::awake(ShowCB, this);
	Fl::unlock();
	return(0);
}

GrapaError GrapaWidget::SetNamesChildren(Fl_Group* g, GrapaNames* pNameSpace)
{
	if (g == NULL) return(-1);
	if (!((GrapaWidget*)g->user_data())->mHasChildren) return(0);;
	int c = g->children();
	for (int i = 0; i < c; i++)
	{
		Fl_Widget* t = g->child(i);
		if (t)
		{
			GrapaWidget* x = (GrapaWidget*)t->user_data();
			if (x)
			{
				if (x->mThreadSet)
					x->mNameSpace.UpdateParrent(pNameSpace);
				else
					x->mNameSpace.SetParrent(pNameSpace);
				//if (!x->mNameSet)
				//{
				//	x->vNameSpace = pNameSpace;
				//	if (x->vWidget && x->vWidget->as_group())
				//		SetNamesChildren(x->vWidget->as_group(), pNameSpace);
				//}
			}
		}
	}
	return 0;
}

GrapaError GrapaWidget::SetThreadChildren(Fl_Group* g, GrapaWidgetThread* pThread)
{
	if (g == NULL) return(-1);
	int c = g->children();
	for (int i = 0; i < c; i++)
	{
		Fl_Widget* t = g->child(i);
		if (t)
		{
			GrapaWidget* x = (GrapaWidget*)t->user_data();
			if (x)
			{
				if (!x->mThreadSet)
				{
					x->vThread = pThread;
					if (x->vWidget && x->vWidget->as_group())
						SetThreadChildren(x->vWidget->as_group(), pThread);
				}
			}
		}
	}
	return 0;
}

GrapaError GrapaWidget::Add(GrapaCHAR& pName, GrapaWidget* bi, GrapaWidget* b2)
{
	Fl_Group* g = vWidget?vWidget->as_group():NULL;
	if (g && bi && bi->vWidget)
	{
		Fl::lock();
		bi->vParent = this;
		bi->mName.FROM(pName);
		bi->mName.mToken = GrapaTokenType::STR;
		//if (!bi->mNameSet)
		//{
			//bi->vNameSpace = vNameSpace;
			if (bi->mThreadSet)
				bi->mNameSpace.UpdateParrent(vNameSpace);
			else
				bi->mNameSpace.SetParrent(vNameSpace);
			//if (bi->vWidget->as_group())
			//	SetNamesChildren(bi->vWidget->as_group(), vNameSpace);
		//}
		//if (!bi->mThreadSet)
		//{
		//	bi->vThread = vThread;
		//	if (bi->vWidget->as_group())
		//		SetThreadChildren(bi->vWidget->as_group(), vThread);
		//}
		if (b2)
			g->insert(*bi->vWidget, g->find(b2->vWidget));
		else
			g->add(bi->vWidget);
		if (g->visible())
			g->redraw();
		Fl::unlock();
	}
	return(0);
}

void GrapaWidget::Add(GrapaRuleEvent* pList, GrapaRuleEvent* pAt)
{
	if (mWidgetName.Cmp("menu_bar") == 0)
	{
		if (pList->mName.mLength == 0) return;
		GrapaRuleEvent* mdata = pList;
		while (mdata->mValue.mToken == GrapaTokenType::PTR) mdata = mdata->vRulePointer;
		if (mdata == NULL || mdata->vQueue == NULL || mdata->IsNull() || mdata->vQueue->mCount == 0)
		{
			GrapaWidgetMenuList* menu = MenuFromName((char*)pList->mName.mBytes);
			if (menu)
			{
				int mitem = ((Grapa_Menu_Bar*)vWidget)->find_index((char*)menu->mPath.mBytes);
				if (mitem >= 0)
					((Grapa_Menu_Bar*)vWidget)->remove(mitem);
				MenuDeleteFromName(pList->mName);
			}
			else
			{
				int mitem = ((Grapa_Menu_Bar*)vWidget)->find_index((char*)pList->mValue.mBytes);
				if (mitem >= 0)
				{
					((Grapa_Menu_Bar*)vWidget)->clear_submenu(mitem);
					((Grapa_Menu_Bar*)vWidget)->remove(mitem);
				}
				MenuDeleteFromPath(pList->mValue);
			}
		}
		else
		{
			unsigned int shortcut;
			GrapaCHAR path,relabel,rename;
			int flags = 0;
			Fl_Fontsize size;
			Fl_Color color;
			Fl_Font font;
			GrapaRuleEvent* callback = NULL;
			GrapaRuleEvent* cbdata = NULL;
			bool hasNewFlags = false;
			bool hasNewShortcut = false;
			bool hasNewPath = false;
			bool hasNewLabel = false;
			bool hasNewFont = false;
			bool hasNewSize = false;
			bool hasNewColor = false;
			bool hasNewCallback = false;
			bool hasNewSetOnly = false;
			bool hasNewName = false;
			bool hasNewData = false;
			GrapaRuleEvent* menu = mdata->vQueue->Head();
			while (menu)
			{
				GrapaRuleEvent* menudata = menu;
				while (menudata->mValue.mToken == GrapaTokenType::PTR && menudata->vRulePointer) menudata = menudata->vRulePointer;
				if (menu->mName.StrLowerCmp("shortcut") == 0)
				{
					hasNewShortcut = true;
					if (menudata->mValue.mToken == GrapaTokenType::INT)
						shortcut = GrapaInt(menudata->mValue).LongValue();
					else
						shortcut = fl_old_shortcut((char*)menudata->mValue.mBytes);
				}
				else if (menu->mName.StrLowerCmp("name") == 0)
				{
					hasNewName = true;
					rename.FROM(menudata->mValue);
				}
				else if (menu->mName.StrLowerCmp("path") == 0)
				{
					hasNewPath = true;
					path.FROM(menudata->mValue);
				}
				else if (menu->mName.StrLowerCmp("data") == 0)
				{
					hasNewData = true;
					cbdata = menu;
				}
				else if (menu->mName.StrLowerCmp("label") == 0)
				{
					hasNewLabel = true;
					relabel.FROM(menudata->mValue);
				}
				else if (menu->mName.StrLowerCmp("options") == 0 || menu->mName.StrLowerCmp("flags") == 0)
				{
					hasNewFlags = true;
					if (menudata && (menudata->mValue.mToken == GrapaTokenType::ARRAY || menudata->mValue.mToken == GrapaTokenType::STR))
					{
						GrapaRuleEvent* m2 = menudata->mValue.mToken == GrapaTokenType::ARRAY ? menudata->vQueue->Head() : menudata;
						while (m2 && m2->mValue.mLength)
						{
							GrapaRuleEvent* m2data = m2;
							while (m2data->mValue.mToken == GrapaTokenType::PTR && m2data->vRulePointer) m2data = m2data->vRulePointer;
							if (m2data->mValue.StrLowerCmp("MENU_DIVIDER") == 0) flags += FL_MENU_DIVIDER;
							else if (m2data->mValue.StrLowerCmp("INACTIVE") == 0) flags += FL_MENU_INACTIVE;
							else if (m2data->mValue.StrLowerCmp("TOGGLE") == 0) flags += FL_MENU_TOGGLE;
							else if (m2data->mValue.StrLowerCmp("VALUE") == 0) flags += FL_MENU_VALUE;
							else if (m2data->mValue.StrLowerCmp("RADIO") == 0) flags += FL_MENU_RADIO;
							else if (m2data->mValue.StrLowerCmp("INVISIBLE") == 0) flags += FL_MENU_INVISIBLE;
							else if (m2data->mValue.StrLowerCmp("POINTER") == 0) flags += FL_SUBMENU_POINTER;
							else if (m2data->mValue.StrLowerCmp("SUBMENU") == 0) flags += FL_SUBMENU;
							else if (m2data->mValue.StrLowerCmp("DIVIDER") == 0) flags += FL_MENU_DIVIDER;
							else if (m2data->mValue.StrLowerCmp("HORIZONTAL") == 0) flags += FL_MENU_HORIZONTAL;
							if (menudata->mValue.mToken != GrapaTokenType::ARRAY) break;
							m2 = m2->Next();
						}
					}
				}
				else if (menu->mName.StrLowerCmp("callback") == 0)
				{
					hasNewCallback = true;
					callback = menu;
				}
				else if (menu->mName.StrLowerCmp("labelfont") == 0)
				{
					hasNewFont = true;
					font = TableFont(menudata->mValue);
				}
				else if (menu->mName.StrLowerCmp("labelsize") == 0)
				{
					hasNewSize = true;
					size = GrapaInt(menudata->mValue).LongValue();
				}
				else if (menu->mName.StrLowerCmp("labelcolor") == 0)
				{
					hasNewColor = true;
					color = TableColor(menudata->mValue);
				}
				else if (menu->mName.StrLowerCmp("setonly") == 0)
				{
					if (menudata->mValue.mLength == 0 || *menudata->mValue.mBytes == 0 || menudata->mValue.StrLowerCmp("false") == 0)
						hasNewSetOnly = false;
					else
						hasNewSetOnly = true;
				}
				menu = menu->Next();
			}
			GrapaWidgetMenuList* menuList = MenuFromName((char*)pList->mName.mBytes);
			Grapa_Menu_Bar* mb = (Grapa_Menu_Bar*)vWidget;
			int mitem = -1;
			Fl_Menu_Item* mitemX = NULL;
			if (menuList)
			{
				mitem = ((Grapa_Menu_Bar*)vWidget)->find_index((char*)menuList->mPath.mBytes);
				if (mitem >= 0)
					mitemX = (Fl_Menu_Item*)&mb->menu()[mitem];
			}
			if (mitem >= 0 && mitemX)
			{
				if (hasNewLabel)
				{
					((Grapa_Menu_Bar*)vWidget)->replace(mitem, (char*)relabel.mBytes);
					path.SetLength(256);
					if (mb->item_pathname((char*)path.mBytes, path.mLength, mitemX) == 0)
					{
						path.SetLength(strlen((char*)path.mBytes));
						hasNewPath = true;
					}
				}
				if (hasNewFlags)
					((Grapa_Menu_Bar*)vWidget)->mode(mitem, flags);
			}
			else
			{
				int atPos = -1;
				if (pAt && pAt->mValue.mToken == GrapaTokenType::INT) atPos = GrapaInt(pAt->mValue).LongValue();
				if (pAt && pAt->mValue.mToken == GrapaTokenType::STR)
				{
					atPos = ((Grapa_Menu_Bar*)vWidget)->find_index((char*)pAt->mValue.mBytes);
					if (atPos < 0)
					{
						GrapaWidgetMenuList* menuAtList = MenuFromName((char*)pAt->mValue.mBytes);
						if (menuAtList)
						{
							atPos = ((Grapa_Menu_Bar*)vWidget)->find_index((char*)menuAtList->mPath.mBytes);
						}
					}
				}
				if (atPos >= 0)
				{
					atPos = ((Grapa_Menu_Bar*)vWidget)->insert(atPos, (char*)pList->mName.mBytes, (char*)"", Grapa_Menu_Bar::Callback, NULL, flags);
					mitemX = (Fl_Menu_Item*)&mb->menu()[atPos];
					if (hasNewLabel)
						((Grapa_Menu_Bar*)vWidget)->replace(atPos, (char*)relabel.mBytes);
					path.SetLength(256);
					if (mb->item_pathname((char*)path.mBytes, path.mLength, mitemX) == 0)
					{
						path.SetLength(strlen((char*)path.mBytes));
						hasNewPath = true;
					}
				}
				else
				{
					if (path.mLength)
						atPos = ((Grapa_Menu_Bar*)vWidget)->add((char*)path.mBytes, (char*)"", Grapa_Menu_Bar::Callback, NULL, flags);
					if (atPos>=0)
						mitemX = (Fl_Menu_Item*)&mb->menu()[atPos];
				}
			}
			if (mitemX)
			{
				if (menuList == NULL)
				{
					menuList = (GrapaWidgetMenuList*) new GrapaWidgetMenuList();
					menuList->mPath.FROM(path);
					menuList->mName.FROM(pList->mName);
					if (hasNewName)
						menuList->mName.FROM(rename);
					menuList->mName.mToken = GrapaTokenType::STR;
					menuList->vCallback = NULL;
					menuList->vData = NULL;
					mOpListMenu.push_back(menuList);
				}
				if (hasNewPath)
					menuList->mPath.FROM(path);
				if (hasNewName)
					menuList->mName.FROM(rename);
				menuList->mName.mToken = GrapaTokenType::STR;
				if (hasNewCallback)
				{
					if (menuList->vCallback)
					{
						menuList->vCallback->CLEAR();
						delete menuList->vCallback;
						menuList->vCallback = NULL;
					}
					menuList->vCallback = vScriptExec->CopyItem(callback);
				}
				if (hasNewData)
				{
					if (menuList->vData)
					{
						menuList->vData->CLEAR();
						delete menuList->vData;
						menuList->vData = NULL;
					}
					menuList->vData = vScriptExec->CopyItem(cbdata);
				}
				if (hasNewShortcut)
					mitemX->shortcut(shortcut);
				if (hasNewSize)
					mitemX->labelsize(size);
				if (hasNewColor)
					mitemX->labelcolor(color);
				if (hasNewFont)
					mitemX->labelfont(font);
				if (hasNewSetOnly)
					mitemX->setonly();
			}
		}
		Fl::lock();
		vWidget->redraw();
		Fl::unlock();
	}
	else
	{
		Fl_Group* mg = vWidget->as_group();
		if (mg)
		{
			GrapaRuleEvent* widgetV = pList->vQueue->Head(0);
			GrapaRuleEvent* xV = pList->vQueue->Head(1);
			GrapaRuleEvent* yV = pList->vQueue->Head(2);
			GrapaRuleEvent* wV = pList->vQueue->Head(3);
			GrapaRuleEvent* hV = pList->vQueue->Head(4);
			GrapaRuleEvent* labelV = pList->vQueue->Head(5);
			GrapaRuleEvent* attrV = pList->vQueue->Head(6);
			GrapaInt i;
			GrapaCHAR widget, label;
			s64 x = 0, y = 0, w = 0, h = 0;
			if (widgetV && widgetV->mValue.mToken == GrapaTokenType::STR) widget.FROM(widgetV->mValue);
			if (xV && xV->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(xV->mValue); x = i.LongValue(); }
			if (yV && yV->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(yV->mValue); y = i.LongValue(); }
			if (wV && wV->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(wV->mValue); w = i.LongValue(); }
			if (hV && hV->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(hV->mValue); h = i.LongValue(); }
			if (labelV && labelV->mValue.mToken == GrapaTokenType::STR) label.FROM(labelV->mValue);
			Fl::unlock();
			GrapaWidget* t = new GrapaWidget(vScriptExec, vNameSpace);
			GrapaError err = t->New((char*)widget.mBytes, x, y, w, h, (char*)label.mBytes, attrV);
			Fl::lock();
			if (err)
				delete t;
			else
			{
				t->mName.FROM(pList->mName);
				t->mName.mToken = GrapaTokenType::STR;
				t->mDelete = true;
				t->vParent = this;
				mg->add(t->vWidget);
				if (mResizable.mLength && pList->mName.StrLowerCmp(mResizable) == 0)
					mg->resizable(*t->vWidget);
				if (t->mThreadSet)
					t->mNameSpace.UpdateParrent(vNameSpace);
				else
					t->mNameSpace.SetParrent(vNameSpace);
				if (t->vWidget->as_group())
					SetNamesChildren(t->vWidget->as_group(), vNameSpace);
			}
		}
	}
}

GrapaError GrapaWidget::Extend(GrapaWidget* bi)
{
	return(0);
}

GrapaError GrapaWidget::Remove(GrapaRuleEvent* pList)
{
	if (pList->mValue.mToken == GrapaTokenType::WIDGET)
	{
		GrapaWidget* bi = pList->vWidget;
		Fl_Group* g = vWidget ? vWidget->as_group() : NULL;
		if (g && bi && bi->vWidget)
		{
			Fl::lock();
			bi->vParent = NULL;
			g->remove(bi->vWidget);
			if (bi->mDelete)
			{
				bi->CLEAR();
				delete bi;
				bi = NULL;
			}
			if (g->visible())
				g->redraw();
			Fl::unlock();
		}
	}
	else if (pList->mValue.mToken == GrapaTokenType::INT)
	{
		Remove(GrapaInt(pList->mValue).LongValue());
	}
	else if (pList->mValue.mToken == GrapaTokenType::STR)
	{
		if (mWidgetName.Cmp("menu_bar") == 0)
		{
			if (pList->mValue.mLength == 0) return(0);
			GrapaWidgetMenuList* menu = MenuFromName((char*)pList->mValue.mBytes);
			if (menu)
			{
				int mitem = ((Grapa_Menu_Bar*)vWidget)->find_index((char*)menu->mPath.mBytes);
				if (mitem >= 0)
				{
					((Grapa_Menu_Bar*)vWidget)->remove(mitem);
					MenuDeleteFromName(pList->mValue);
					Fl::lock();
					vWidget->redraw();
					Fl::unlock();
				}
			}
			else
			{
				int mitem = ((Grapa_Menu_Bar*)vWidget)->find_index((char*)pList->mValue.mBytes);
				if (mitem >= 0)
				{
					((Grapa_Menu_Bar*)vWidget)->clear_submenu(mitem);
					((Grapa_Menu_Bar*)vWidget)->remove(mitem);
					MenuDeleteFromPath(pList->mValue);
					Fl::lock();
					vWidget->redraw();
					Fl::unlock();
				}
			}
		}
	}
	return(0);
}

GrapaError GrapaWidget::Remove(s64 pIndex)
{
	if (mWidgetName.Cmp("menu_bar") == 0)
	{
		Grapa_Menu_Bar* mb = (Grapa_Menu_Bar*)vWidget;
		if (pIndex >= 0 && pIndex < mb->size())
		{
			Fl_Menu_Item* mitemX = (Fl_Menu_Item*) (Fl_Menu_Item*)&mb->menu()[pIndex];
			GrapaCHAR relabel;
			relabel.SetLength(256);
			if (mb->item_pathname((char*)relabel.mBytes, relabel.mLength, mitemX) == 0)
			{
				relabel.SetLength(strlen((char*)relabel.mBytes));
				((Grapa_Menu_Bar*)vWidget)->clear_submenu(pIndex);
				((Grapa_Menu_Bar*)vWidget)->remove(pIndex);
				MenuDeleteFromPath(relabel);
			}
		}
	}
	else
	{
		Fl_Group* g = vWidget->as_group();
		Fl_Widget* fw = g->child(pIndex);
		if (fw)
		{
			GrapaWidget* bi = (GrapaWidget*)fw->user_data();
			if (bi)
			{
				Fl::lock();
				bi->vParent = NULL;
				g->remove(bi->vWidget);
				if (bi->mDelete)
				{
					bi->CLEAR();
					delete bi;
					bi = NULL;
				}
				if (g->visible())
					g->redraw();
				Fl::unlock();
			}
		}
	}
	return(0);
}

GrapaError GrapaWidget::Join(GrapaRuleEvent* pList)
{
	return(0);
}

void GrapaWidget::ShowCB(void* b)
{
	GrapaWidget* d = (GrapaWidget*)b;
	if (d->vWidget)
	{
		Fl::lock();
		bool oldMainThread = gGrapaWidgetMainThread;
		gGrapaWidgetMainThread = true;
		Fl::visual(FL_DOUBLE | FL_INDEX);
		d->vWidget->show();
		if (d->vWidget->parent())
			d->vWidget->parent()->redraw();
		gGrapaWidgetMainThread = oldMainThread;
		Fl::unlock();
	}
}

GrapaError GrapaWidget::Show()
{
	if (vWidget == NULL)
		return(-1);
	Fl::awake(ShowCB, this);
	return(0);
}

void GrapaWidget::HideCB(void* b)
{
	GrapaWidget* d = (GrapaWidget*)b;
	Fl::lock();
	bool oldMainThread = gGrapaWidgetMainThread;
	gGrapaWidgetMainThread = true;
	d->vWidget->hide();
	if (d->vWidget->parent())
		d->vWidget->parent()->redraw();
	gGrapaWidgetMainThread = oldMainThread;
	Fl::unlock();
}

GrapaError GrapaWidget::Hide()
{
	if (vWidget == NULL)
		return(-1);
	Fl::awake(HideCB, this);
	if (vOpHide)
	{
		GrapaExecCB* cb = new GrapaExecCB();
		cb->vWidget = this;
		cb->vCmd = vScriptExec->CopyItem(vOpHide);
		cb->vScriptExec = vScriptExec;
		cb->vNameSpace = vNameSpace;
		while (cb->vNameSpace->GetParrent()) cb->vNameSpace = cb->vNameSpace->GetParrent();
		Fl::awake(ExecCB, cb);
	}
	return(0);
}

void GrapaWidget::CloseCB(void* b)
{
	Fl::lock();
	Fl_Widget* d = (Fl_Widget*)b;
	d->hide();
	Fl::delete_widget(d);
	Fl::unlock();
}

GrapaError GrapaWidget::Redraw()
{
	if (vWidget == NULL)
		return(-1);
	vWidget->redraw();
	return(0);
}

GrapaError GrapaWidget::Resizable(GrapaWidget* bi)
{
	if (vWidget == NULL || bi == NULL || bi->vWidget == NULL)
		return(-1);
	Fl_Group* g = bi->vWidget->as_group();
	if (g)
		g->resizable(bi->vWidget);
	return(0);
}

void GrapaWidget::ResizeCB(void* b)
{
	GrapaResizeCB* cb = (GrapaResizeCB*)b;
	cb->vWidget->vWidget->resize(cb->x, cb->y, cb->w, cb->h);
	Fl::lock();
	cb->vWidget->vWidget->redraw();
	Fl::unlock();
	delete cb;
}

GrapaError GrapaWidget::Resize(s64 x, s64 y, s64 w, s64 h)
{
	if (vWidget == NULL)
		return(-1);
	Fl_Window *win = vWidget->as_window();
	if (win)
	{
		GrapaResizeCB* cb = new GrapaResizeCB();
		cb->vWidget = this;
		cb->vScriptExec = vScriptExec;
		cb->vNameSpace = vNameSpace;
		cb->x = x;
		cb->y = y;
		cb->w = w;
		cb->h = h;
		Fl::awake(GrapaWidget::ResizeCB, cb);
	}
	else
	{
		vWidget->resize(x, y, w, h);
		Fl::lock();
		vWidget->parent()->redraw();
		vWidget->redraw();
		Fl::unlock();
	}
	return(0);
}

GrapaError GrapaWidget::Handle(int event, int& r)
{
	r = 0;
	if (vWidget == NULL)
		return(-1);
	r = vWidget->handle(event);
	return(0);
}

GrapaError GrapaWidget::Get(GrapaCHAR& pName, GrapaCHAR& pValue)
{
	GrapaError err = -1;
	pValue.SetLength(0);
	if (mWidgetName.Cmp("text_editor") == 0) err = ((Grapa_Text_Editor*)vWidget)->Get(pName, pValue);
	else if (mWidgetName.Cmp("text_display") == 0) err = ((Grapa_Text_Display*)vWidget)->Get(pName, pValue);
	else if (mWidgetName.Cmp("resizable_scroll") == 0) err = ((Grapa_Resizable_Scroll*)vWidget)->Get(pName, pValue);
	else if (mWidgetName.Cmp("window") == 0) err = ((Grapa_Window*)vWidget)->Get(pName, pValue);
	else if (mWidgetName.Cmp("double_window") == 0) err = ((Grapa_Double_Window*)vWidget)->Get(pName, pValue);
	return(err);
}

GrapaError GrapaWidget::Set(GrapaCHAR& pName, GrapaCHAR& pValue)
{
	GrapaError err = -1;
	if (mWidgetName.Cmp("text_editor") == 0) err = ((Grapa_Text_Editor*)vWidget)->Set(pName, pValue);
	else if (mWidgetName.Cmp("text_display") == 0) err = ((Grapa_Text_Display*)vWidget)->Set(pName, pValue);
	return(err);
}

GrapaError GrapaWidget::EventKey(int key, int& r)
{
	if (key)
		r = Fl::event_key(key);
	else
		r = Fl::event_key();
	return 0;
}

GrapaError GrapaWidget::Append(GrapaCHAR& pValue)
{
	if (mWidgetName.Cmp("text_editor") == 0) ((Grapa_Text_Editor*)vWidget)->buffer()->append((char*)pValue.mBytes);
	else if (mWidgetName.Cmp("text_display") == 0) ((Grapa_Text_Display*)vWidget)->buffer()->append((char*)pValue.mBytes);
	else return -1;
	return(0);
}

void GrapaWidget::ExecCB(void* b)
{
	GrapaExecCB* cb = (GrapaExecCB*)b;
	Fl::lock();
	GrapaRuleQueue q;
	GrapaRuleEvent wid;
	//wid.mName.FROM("o");
	wid.mValue.mToken = GrapaTokenType::WIDGET;
	wid.vWidget = cb->vWidget;
	wid.vClass = cb->vScriptExec->vScriptState->GetClass(cb->vNameSpace, GrapaCHAR("$WIDGET"));
	q.PushTail(&wid);
	GrapaRuleEvent* e = NULL;
	e = cb->vScriptExec->ProcessPlan(cb->vNameSpace, cb->vCmd, &wid);
	if (e) delete e;
	wid.vWidget = NULL;
	q.PopHead();
	q.CLEAR();
	cb->vCmd->CLEAR();
	delete cb->vCmd;
	delete cb;
	Fl::unlock();
}

GrapaRuleEvent* GrapaWidget::Child(GrapaCHAR& pValue)
{
	GrapaRuleEvent* result = NULL;
	Fl_Group* g = vWidget->as_group();
	if (g == NULL) return NULL;
	int c = g->children();
	for (int i = 0; i < c; i++)
	{
		Fl_Widget* t = g->child(i);
		if (t)
		{
			GrapaWidget* x = (GrapaWidget*)t->user_data();
			if (x)
			{
				if (x->mName.StrLowerCmp(pValue)==0)
					return x->vPtrEvent;
			}
		}
	}
	return NULL;
}

GrapaRuleEvent* GrapaWidget::Child(s64 pIndex)
{
	GrapaRuleEvent* result = NULL;
	Fl_Group* g = vWidget->as_group();
	if (g == NULL) return NULL;
	int c = g->children();
	if (pIndex < 0) pIndex = c + pIndex;
	if (pIndex >= c) pIndex = c - 1;
	if (pIndex < 0) return NULL;
	Fl_Widget* t = g->child(pIndex);
	return t ? ((GrapaWidget*)t->user_data())->vPtrEvent : NULL;
}

GrapaRuleEvent* GrapaWidget::Set(GrapaRuleEvent* attr)
{
	GrapaRuleEvent* result = NULL;
	if (attr->mValue.mToken == GrapaTokenType::LIST)
	{
		if (attr->vQueue)
		{
			GrapaRuleEvent* value = attr->vQueue->Head();
			while (value)
			{
				GrapaRuleEvent* data = value;
				while (data->mValue.mToken == GrapaTokenType::PTR && data->vRulePointer) data = data->vRulePointer;
				if (value->mName.StrLowerCmp("namespace") == 0)
				{
					GrapaRuleEvent* name = mNameSpace.GetNameQueue()->Head();
					if (name == &mNameQueue)
					{
						mNameSpace.GetNameQueue()->PopEvent(&mNameQueue);
						name = new GrapaRuleEvent();
						name->mValue.mToken = GrapaTokenType::PTR;
						name->vRulePointer = &mNameQueue;
						mNameSpace.GetNameQueue()->PushTail(name);
					}
					while (name && name->vRulePointer) name = name->vRulePointer;
					s64 index;
					if (name->vQueue->Search("$root", index) == NULL)
					{
						GrapaRuleEvent* e2 = new GrapaRuleEvent();
						e2->mValue.mToken = GrapaTokenType::PTR;
						e2->vRulePointer = vPtrEvent;
						e2->mName.FROM("$root");
						name->vQueue->PushTail(e2);
					}
					mNameSpace.mSkip = false;
					mNameSet = true;
					if (data->mValue.mToken == GrapaTokenType::STR)
					{
						if (data->mValue.StrLowerCmp("global") == 0)
						{
							GrapaNames* parentSpace = vNameSpace;
							while (parentSpace->GetParrent()) parentSpace = parentSpace->GetParrent();
							if (mThreadSet)
								mNameSpace.UpdateParrent(parentSpace);
							else
								mNameSpace.SetParrent(parentSpace);
							if (vWidget->as_group())
								SetNamesChildren(vWidget->as_group(), vNameSpace);
						}
					}
				}
				else if (value->mName.StrLowerCmp("on_post_start") == 0)
				{
					if (mThread.startCB)
					{
						mThread.startCB->CLEAR();
						delete mThread.startCB;
					}
					mThread.startCB = vScriptExec->CopyItem(data);
					SetThread();
				}
				else if (value->mName.StrLowerCmp("on_post_prompt") == 0)
				{
					if (mThread.promptCB)
					{
						mThread.promptCB->CLEAR();
						delete mThread.promptCB;
					}
					mThread.promptCB = vScriptExec->CopyItem(data);
					mThread.mSend.mScriptState.EnablePrompt(vNameSpace);
					SetThread();
				}
				else if (value->mName.StrLowerCmp("on_post_end") == 0)
				{
					if (mThread.endCB)
					{
						mThread.endCB->CLEAR();
						delete mThread.endCB;
					}
					mThread.endCB = vScriptExec->CopyItem(data);
					SetThread();
				}
				else if (value->mName.StrLowerCmp("on_post_echo") == 0)
				{
					if (mThread.echoCB)
					{
						mThread.echoCB->CLEAR();
						delete mThread.echoCB;
					}
					mThread.echoCB = vScriptExec->CopyItem(data);
					SetThread();
				}
				else if (value->mName.StrLowerCmp("on_post_message") == 0)
				{
					if (mThread.messageCB)
					{
						mThread.messageCB->CLEAR();
						delete mThread.messageCB;
					}
					mThread.messageCB = vScriptExec->CopyItem(data);
					SetThread();
				}
				else if (value->mName.StrLowerCmp("group") == 0 || value->mName.StrLowerCmp("child") == 0 || value->mName.StrLowerCmp("children") == 0)
				{
					if (mWidgetName.Cmp("menu_bar") == 0)
					{
						if (data->vQueue)
						{
							GrapaRuleEvent* m = data->vQueue->Head();
							while (m)
							{
								Add(m);
								m = m->Next();
							}
							Fl::lock();
							vWidget->redraw();
							Fl::unlock();
						}
					}
					else if (data->vQueue)
					{
						Fl_Group* mg = vWidget->as_group();
						if (mg)
						{
							GrapaRuleEvent* g = data->vQueue->Head();
							while (g)
							{
								if ((g->mValue.mToken == GrapaTokenType::LIST || g->mValue.mToken == GrapaTokenType::ARRAY) && g->vQueue)
								{
									Add(g);
								}
								else if ((g->mValue.mToken == GrapaTokenType::WIDGET) && g->vWidget)
								{
									GrapaWidget* t = g->vWidget;
									if (t->vWidget->as_group())
										SetNamesChildren(t->vWidget->as_group(), vNameSpace);
									g->vWidget = NULL;
									t->mName.FROM(g->mName);
									t->mName.mToken = GrapaTokenType::STR;
									t->mDelete = true;
									t->vParent = this;
									mg->add(t->vWidget);
									if (mResizable.mLength && g->mName.StrLowerCmp(mResizable) == 0)
										mg->resizable(*t->vWidget);
									if (t->mThreadSet)
										t->mNameSpace.UpdateParrent(vNameSpace);
									else
										t->mNameSpace.SetParrent(vNameSpace);
									if (t->vWidget->as_group())
										SetNamesChildren(t->vWidget->as_group(), vNameSpace);
								}
								g = g->Next();
							}
						}
					}
				}
				else if (value->mName.StrLowerCmp("hide") == 0 || value->mName.StrLowerCmp("on_hide") == 0)
				{
					if (vOpHide)
					{
						vOpHide->CLEAR();
						delete vOpHide;
					}
					vOpHide = vScriptExec->CopyItem(data);
				}
				else if (value->mName.StrLowerCmp("resizable") == 0)
				{
					mResizable.FROM(data->mValue);
					Fl_Group* g = vWidget->as_group();
					g->resizable(NULL);
					if (mResizable.mLength && vWidget->as_group() && vWidget->user_data() && ((GrapaWidget*)vWidget->user_data())->mHasChildren)
					{
						int c = g->children();
						for (int i = 0; i < c; i++)
						{
							Fl_Widget* t = g->child(i);
							if (t)
							{
								GrapaWidget* x = (GrapaWidget*)t->user_data();
								if (x)
								{
									if (x->mName.StrLowerCmp(mResizable) == 0)
									{
										g->resizable(*x->vWidget);
										break;
									}
								}
							}
						}
					}
				}
				else if (value->mName.StrLowerCmp("text") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						((Fl_Text_Display*)vWidget)->buffer()->text((char*)data->mValue.mBytes);
						//((Grapa_Text_Display*)vWidget)->calc_last_char();
					}
				}
				else if (value->mName.StrLowerCmp("append") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						((Fl_Text_Display*)vWidget)->buffer()->append((char*)data->mValue.mBytes);
					}
				}
				else if (value->mName.StrLowerCmp("spacing") == 0)
				{
					if (mWidgetName.Cmp("pack") == 0)
					{
						GrapaInt r;
						r.FromBytes(data->mValue);
						((Fl_Pack*)vWidget)->spacing((int)r.LongValue());
					}
				}
				else if (value->mName.StrLowerCmp("wrap") == 0 || value->mName.StrLowerCmp("wrap_mode") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						if (data->mValue.StrLowerCmp("NONE") == 0)
							((Fl_Text_Display*)vWidget)->wrap_mode(Fl_Text_Display::WRAP_NONE, 0);
						if (data->mValue.StrLowerCmp("COLUMN") == 0)
							((Fl_Text_Display*)vWidget)->wrap_mode(Fl_Text_Display::WRAP_AT_COLUMN, 0);
						if (data->mValue.StrLowerCmp("PIXEL") == 0)
							((Fl_Text_Display*)vWidget)->wrap_mode(Fl_Text_Display::WRAP_AT_PIXEL, 0);
						if (data->mValue.StrLowerCmp("BOUNDS") == 0)
							((Fl_Text_Display*)vWidget)->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
					}
				}
				else if (value->mName.StrLowerCmp("x") == 0)
				{
					int x = vWidget->x();
					int y = vWidget->y();
					int w = vWidget->w();
					int h = vWidget->h();
					x = GrapaInt(data->mValue).LongValue();
					Resize(x, y, w, h);
				}
				else if (value->mName.StrLowerCmp("y") == 0)
				{
					int x = vWidget->x();
					int y = vWidget->y();
					int w = vWidget->w();
					int h = vWidget->h();
					y = GrapaInt(data->mValue).LongValue();
					Resize(x, y, w, h);
				}
				else if (value->mName.StrLowerCmp("w") == 0)
				{
					int x = vWidget->x();
					int y = vWidget->y();
					int w = vWidget->w();
					int h = vWidget->h();
					w = GrapaInt(data->mValue).LongValue();
					Resize(x, y, w, h);
				}
				else if (value->mName.StrLowerCmp("h") == 0)
				{
					int x = vWidget->x();
					int y = vWidget->y();
					int w = vWidget->w();
					int h = vWidget->h();
					h = GrapaInt(data->mValue).LongValue();
					Resize(x, y, w, h);
				}
				else if (value->mName.StrLowerCmp("size") == 0)
				{
					if (data->mValue.mToken == GrapaTokenType::ARRAY)
					{
						if (data->vQueue->mCount == 2)
						{
							int x = vWidget->x();
							int y = vWidget->y();
							int w = data->vQueue->Head(0)->IsNull() ? vWidget->w() : GrapaInt(data->vQueue->Head(0)->mValue).LongValue();
							int h = data->vQueue->Head(1)->IsNull() ? vWidget->h() : GrapaInt(data->vQueue->Head(1)->mValue).LongValue();
							Resize(x, y, w, h);
						}
						else if (data->vQueue->mCount == 4)
						{
							int x = data->vQueue->Head(0)->IsNull() ? vWidget->x() : GrapaInt(data->vQueue->Head(0)->mValue).LongValue();
							int y = data->vQueue->Head(1)->IsNull() ? vWidget->y() : GrapaInt(data->vQueue->Head(1)->mValue).LongValue();
							int w = data->vQueue->Head(2)->IsNull() ? vWidget->w() : GrapaInt(data->vQueue->Head(2)->mValue).LongValue();
							int h = data->vQueue->Head(3)->IsNull() ? vWidget->h() : GrapaInt(data->vQueue->Head(3)->mValue).LongValue();
							Resize(x, y, w, h);
						}
					}
					else if(data->mValue.mToken == GrapaTokenType::LIST)
					{
						int x = vWidget->x();
						int y = vWidget->y();
						int w = vWidget->w();
						int h = vWidget->h();
						s64 index;
						GrapaRuleEvent* e = NULL;
						e = data->vQueue->Search("x", index); if (e && !e->IsNull()) x = GrapaInt(e->mValue).LongValue(); if (e) { e->CLEAR(); delete e; }
						e = data->vQueue->Search("y", index); if (e && !e->IsNull()) y = GrapaInt(e->mValue).LongValue(); if (e) { e->CLEAR(); delete e; }
						e = data->vQueue->Search("w", index); if (e && !e->IsNull()) w = GrapaInt(e->mValue).LongValue(); if (e) { e->CLEAR(); delete e; }
						e = data->vQueue->Search("h", index); if (e && !e->IsNull()) h = GrapaInt(e->mValue).LongValue(); if (e) { e->CLEAR(); delete e; }
						Resize(x, y, w, h);
					}
					Fl::lock();
					vWidget->redraw();
					Fl::unlock();
				}
				else if (value->mName.StrLowerCmp("shape") == 0)
				{
					if (mWidgetName.Cmp("table_row") == 0)
					{
						Grapa_Table_Row* g = (Grapa_Table_Row*)vWidget;
						if (data->mValue.mToken == GrapaTokenType::ARRAY)
						{
							if (data->vQueue->mCount == 2)
							{
								int r = data->vQueue->Head(0)->IsNull() ? g->rows() : GrapaInt(data->vQueue->Head(0)->mValue).LongValue();
								int c = data->vQueue->Head(1)->IsNull() ? g->cols() : GrapaInt(data->vQueue->Head(1)->mValue).LongValue();
								g->cols(r);
								g->rows(c);
							}
						}
						else if (data->mValue.mToken == GrapaTokenType::LIST)
						{
							int r = g->rows();
							int c = g->cols();
							s64 index;
							GrapaRuleEvent* e = NULL;
							e = data->vQueue->Search("r", index); if (e && !e->IsNull()) r = GrapaInt(e->mValue).LongValue(); if (e) { e->CLEAR(); delete e; }
							e = data->vQueue->Search("c", index); if (e && !e->IsNull()) c = GrapaInt(e->mValue).LongValue(); if (e) { e->CLEAR(); delete e; }
							g->cols(r);
							g->rows(c);
						}
						Fl::lock();
						vWidget->redraw();
						Fl::unlock();
					}
				}
				else if (value->mName.StrLowerCmp("vector") == 0)
				{
					if (mWidgetName.Cmp("table_row") == 0)
					{
						Grapa_Table_Row* g = (Grapa_Table_Row*)vWidget;
						g->clear();
						if (g->mDeleteVector && g->vVector)
							delete g->vVector;
						g->vVector = NULL;
						g->mDeleteVector = false;
						while (data->mValue.mToken == GrapaTokenType::PTR) data = data->vRulePointer;
						if (data && (data->mValue.mToken == GrapaTokenType::ARRAY || data->mValue.mToken == GrapaTokenType::LIST))
						{
							if (data->vQueue->mCount > 0)
							{
								g->mDeleteVector = true;
								g->vVector = new GrapaVector();
								if (!g->vVector->FROM(vScriptExec, data, 0))
								{
									delete g->vVector;
									g->vVector = NULL;
									g->mDeleteVector = false;
								}
							}
						}
						else if (data && data->mValue.mToken == GrapaTokenType::VECTOR)
						{
							if (data->vVector && data->vVector->mDim == 2)
							{
								g->mDeleteVector = true;
								g->vVector = new GrapaVector(*data->vVector);
							}
						}
						if (g->vVector)
						{
							g->cols(g->vVector->mCounts[1]);
							g->rows(g->vVector->mCounts[0]);
						}
					}
				}
				else if (value->mName.StrLowerCmp("key") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0)
					{
						if (data->mValue.StrLowerCmp("Button") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Button, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("BackSpace") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_BackSpace, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Tab") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Tab, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Iso_Key") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Iso_Key, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Enter") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Enter, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Pause") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Pause, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Scroll_Lock") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Scroll_Lock, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Escape") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Escape, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Kana") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Kana, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Eisu") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Eisu, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Yen") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Yen, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("JIS_Underscore") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_JIS_Underscore, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Home") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Home, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Left") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Left, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Up") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Up, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Right") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Right, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Down") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Down, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Page_Up") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Page_Up, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Page_Down") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Page_Down, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("End") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_End, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Print") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Print, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Insert") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Insert, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Menu") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Menu, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Help") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Help, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Num_Lock") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Num_Lock, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("KP") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_KP, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("KP_Enter") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_KP_Enter, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("KP_Last") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_KP_Last, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("F") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_F, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("F_Last") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_F_Last, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Shift_L") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Shift_L, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Shift_R") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Shift_R, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Control_L") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Control_L, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Control_R") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Control_R, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Caps_Lock") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Caps_Lock, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Meta_L") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Meta_L, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Meta_R") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Meta_R, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Alt_L") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Alt_L, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Alt_R") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Alt_R, (Grapa_Text_Editor*)vWidget);
						else if (data->mValue.StrLowerCmp("Delete") == 0)
							((Grapa_Text_Editor*)vWidget)->kf_ctrl_move(FL_Delete, (Grapa_Text_Editor*)vWidget);
					}
				}
				else if (value->mName.StrLowerCmp("color") == 0)
				{
					vWidget->color(TableColor(data->mValue));
				}
				else if (value->mName.StrLowerCmp("textsize") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						Fl_Text_Display* f = (Fl_Text_Display*)vWidget;
						f->textsize(GrapaInt(data->mValue).LongValue());
					}
					else if (mWidgetName.Cmp("menu_bar") == 0)
					{
						Fl_Menu_Bar* f = (Fl_Menu_Bar*)vWidget;
						f->textsize(GrapaInt(data->mValue).LongValue());
					}
				}
				else if (value->mName.StrLowerCmp("textfont") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						Fl_Text_Display* f = (Fl_Text_Display*)vWidget;
						f->textfont(TableFont(data->mValue));
					}
					else if (mWidgetName.Cmp("menu_bar") == 0)
					{
						Fl_Menu_Bar* f = (Fl_Menu_Bar*)vWidget;
						f->textfont(TableFont(data->mValue));
					}
				}
				else if (value->mName.StrLowerCmp("textcolor") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						Fl_Text_Display* f = (Fl_Text_Display*)vWidget;
						f->textcolor(TableColor(data->mValue));
					}
					else if (mWidgetName.Cmp("menu_bar") == 0)
					{
						Fl_Menu_Bar* f = (Fl_Menu_Bar*)vWidget;
						f->textcolor(TableColor(data->mValue));
					}
				}
				else if (value->mName.StrLowerCmp("cursor_state") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						Fl_Text_Display* f = (Fl_Text_Display*)vWidget;
						if (data->mValue.StrLowerCmp("show") == 0)
							f->show_cursor();
						else if (data->mValue.StrLowerCmp("hide") == 0)
							f->hide_cursor();
					}
				}
				else if (value->mName.StrLowerCmp("cursor_color") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						Fl_Text_Display* f = (Fl_Text_Display*)vWidget;
						f->cursor_color(TableColor(data->mValue));
					}
				}
				else if (value->mName.StrLowerCmp("cursor_style") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						Fl_Text_Display* f = (Fl_Text_Display*)vWidget;
						if (data->mValue.StrLowerCmp("CURSOR_DEFAULT") == 0)
							f->cursor_style(FL_CURSOR_DEFAULT);
						else if (data->mValue.StrLowerCmp("CURSOR_ARROW") == 0)
							f->cursor_style(FL_CURSOR_ARROW);
						else if (data->mValue.StrLowerCmp("CURSOR_CROSS") == 0)
							f->cursor_style(FL_CURSOR_CROSS);
						else if (data->mValue.StrLowerCmp("CURSOR_WAIT") == 0)
							f->cursor_style(FL_CURSOR_WAIT);
						else if (data->mValue.StrLowerCmp("CURSOR_INSERT") == 0)
							f->cursor_style(FL_CURSOR_INSERT);
						else if (data->mValue.StrLowerCmp("CURSOR_HAND") == 0)
							f->cursor_style(FL_CURSOR_HAND);
						else if (data->mValue.StrLowerCmp("CURSOR_HELP") == 0)
							f->cursor_style(FL_CURSOR_HELP);
						else if (data->mValue.StrLowerCmp("CURSOR_MOVE") == 0)
							f->cursor_style(FL_CURSOR_MOVE);
						else if (data->mValue.StrLowerCmp("CURSOR_NS") == 0)
							f->cursor_style(FL_CURSOR_NS);
						else if (data->mValue.StrLowerCmp("CURSOR_WE") == 0)
							f->cursor_style(FL_CURSOR_WE);
						else if (data->mValue.StrLowerCmp("CURSOR_NWSE") == 0)
							f->cursor_style(FL_CURSOR_NWSE);
						else if (data->mValue.StrLowerCmp("CURSOR_NESW") == 0)
							f->cursor_style(FL_CURSOR_NESW);
						else if (data->mValue.StrLowerCmp("CURSOR_NONE") == 0)
							f->cursor_style(FL_CURSOR_NONE);
					}
				}
				else if (value->mName.StrLowerCmp("scroll_type") == 0 || value->mName.StrLowerCmp("type") == 0)
				{
					if (mWidgetName.Cmp("scroll") == 0 || mWidgetName.Cmp("resizable_scroll") == 0)
					{
						if (data->mValue.StrLowerCmp("HORIZONTAL") == 0)
							vWidget->type(Fl_Scroll::HORIZONTAL);
						else if (data->mValue.StrLowerCmp("VERTICAL") == 0)
							vWidget->type(Fl_Scroll::VERTICAL);
						else if (data->mValue.StrLowerCmp("BOTH") == 0)
							vWidget->type(Fl_Scroll::BOTH);
						else if (data->mValue.StrLowerCmp("ALWAYS_ON") == 0)
							vWidget->type(Fl_Scroll::ALWAYS_ON);
						else if (data->mValue.StrLowerCmp("HORIZONTAL_ALWAYS") == 0)
							vWidget->type(Fl_Scroll::HORIZONTAL_ALWAYS);
						else if (data->mValue.StrLowerCmp("VERTICAL_ALWAYS") == 0)
							vWidget->type(Fl_Scroll::VERTICAL_ALWAYS);
						else if (data->mValue.StrLowerCmp("BOTH_ALWAYS") == 0)
							vWidget->type(Fl_Scroll::BOTH_ALWAYS);
					}
					else
					{
						if (data->mValue.StrLowerCmp("HORIZONTAL") == 0)
							vWidget->type(FL_HORIZONTAL);
						else if (data->mValue.StrLowerCmp("VERTICAL") == 0)
							vWidget->type(FL_VERTICAL);
					}
				}
				else if (value->mName.StrLowerCmp("orientation") == 0)
				{
					if (data->mValue.StrLowerCmp("HORIZONTAL") == 0)
						vWidget->type(FL_HORIZONTAL);
					else if (data->mValue.StrLowerCmp("VERTICAL") == 0)
						vWidget->type(FL_VERTICAL);
				}
				else if (value->mName.StrLowerCmp("cursor_style") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0 || mWidgetName.Cmp("text_display") == 0)
					{
						Fl_Text_Display* f = (Fl_Text_Display*)vWidget;
						if (data->mValue.StrLowerCmp("CURSOR_DEFAULT") == 0)
							f->cursor_style(FL_CURSOR_DEFAULT);
						else if (data->mValue.StrLowerCmp("CURSOR_ARROW") == 0)
							f->cursor_style(FL_CURSOR_ARROW);
						else if (data->mValue.StrLowerCmp("CURSOR_CROSS") == 0)
							f->cursor_style(FL_CURSOR_CROSS);
						else if (data->mValue.StrLowerCmp("CURSOR_WAIT") == 0)
							f->cursor_style(FL_CURSOR_WAIT);
						else if (data->mValue.StrLowerCmp("CURSOR_INSERT") == 0)
							f->cursor_style(FL_CURSOR_INSERT);
						else if (data->mValue.StrLowerCmp("CURSOR_HAND") == 0)
							f->cursor_style(FL_CURSOR_HAND);
						else if (data->mValue.StrLowerCmp("CURSOR_HELP") == 0)
							f->cursor_style(FL_CURSOR_HELP);
						else if (data->mValue.StrLowerCmp("CURSOR_MOVE") == 0)
							f->cursor_style(FL_CURSOR_MOVE);
						else if (data->mValue.StrLowerCmp("CURSOR_NS") == 0)
							f->cursor_style(FL_CURSOR_NS);
						else if (data->mValue.StrLowerCmp("CURSOR_WE") == 0)
							f->cursor_style(FL_CURSOR_WE);
						else if (data->mValue.StrLowerCmp("CURSOR_NWSE") == 0)
							f->cursor_style(FL_CURSOR_NWSE);
						else if (data->mValue.StrLowerCmp("CURSOR_NESW") == 0)
							f->cursor_style(FL_CURSOR_NESW);
						else if (data->mValue.StrLowerCmp("CURSOR_NONE") == 0)
							f->cursor_style(FL_CURSOR_NONE);
					}
				}
				else if (value->mName.StrLowerCmp("resize") == 0 || value->mName.StrLowerCmp("on_resize") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0)
					{
						if (((Grapa_Text_Editor*)vWidget)->wh.vOpResize)
							delete ((Grapa_Text_Editor*)vWidget)->wh.vOpResize;
						((Grapa_Text_Editor*)vWidget)->wh.vOpResize = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("text_display") == 0)
					{
						if (((Grapa_Text_Display*)vWidget)->wh.vOpResize)
							delete ((Grapa_Text_Display*)vWidget)->wh.vOpResize;
						((Grapa_Text_Display*)vWidget)->wh.vOpResize = vScriptExec->CopyItem(data);
					}
				}
				else if (value->mName.StrLowerCmp("event_keydown") == 0 || value->mName.StrLowerCmp("on_keydown") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0)
					{
						if (((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_keydown)
							delete ((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_keydown;
						((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_keydown = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("text_display") == 0)
					{
						if (((Grapa_Text_Display*)vWidget)->wh.vOpHandle_keydown)
							delete ((Grapa_Text_Display*)vWidget)->wh.vOpHandle_keydown;
						((Grapa_Text_Display*)vWidget)->wh.vOpHandle_keydown = vScriptExec->CopyItem(data);
					}
				}
				else if (value->mName.StrLowerCmp("event_mousewheel") == 0 || value->mName.StrLowerCmp("on_mousewheel") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0)
					{
						if (((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_mousewheel)
							delete ((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_mousewheel;
						((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_mousewheel = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("text_display") == 0)
					{
						if (((Grapa_Text_Display*)vWidget)->wh.vOpHandle_mousewheel)
							delete ((Grapa_Text_Display*)vWidget)->wh.vOpHandle_mousewheel;
						((Grapa_Text_Display*)vWidget)->wh.vOpHandle_mousewheel = vScriptExec->CopyItem(data);
					}
				}
				else if (value->mName.StrLowerCmp("when_release") == 0)
				{
					if (mWidgetName.Cmp("button") == 0)
					{
						if (((Grapa_Button*)vWidget)->wh.vOpWhen_release)
							delete ((Grapa_Button*)vWidget)->wh.vOpWhen_release;
						((Grapa_Button*)vWidget)->wh.vOpWhen_release = vScriptExec->CopyItem(data);
						((Grapa_Button*)vWidget)->callback(Grapa_Button::When);
						((Grapa_Button*)vWidget)->when(FL_WHEN_RELEASE);
					}
				}
				else if (value->mName.StrLowerCmp("on_drag") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0)
					{
						if (((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_drag)
							delete ((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_drag;
						((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_drag = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("text_display") == 0)
					{
						if (((Grapa_Text_Display*)vWidget)->wh.vOpHandle_drag)
							delete ((Grapa_Text_Display*)vWidget)->wh.vOpHandle_drag;
						((Grapa_Text_Display*)vWidget)->wh.vOpHandle_drag = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("scrollbar") == 0)
					{
						if (((Grapa_Text_Display*)vWidget)->wh.vOpHandle_drag)
							delete ((Grapa_Text_Display*)vWidget)->wh.vOpHandle_drag;
						((Grapa_Text_Display*)vWidget)->wh.vOpHandle_drag = vScriptExec->CopyItem(data);
					}
				}
				else if (value->mName.StrLowerCmp("on_push") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0)
					{
						if (((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_push)
							delete ((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_push;
						((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_push = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("text_display") == 0)
					{
						if (((Grapa_Text_Display*)vWidget)->wh.vOpHandle_push)
							delete ((Grapa_Text_Display*)vWidget)->wh.vOpHandle_push;
						((Grapa_Text_Display*)vWidget)->wh.vOpHandle_push = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("scrollbar") == 0)
					{
						if (((Grapa_Scrollbar*)vWidget)->wh.vOpHandle_push)
							delete ((Grapa_Scrollbar*)vWidget)->wh.vOpHandle_push;
						((Grapa_Scrollbar*)vWidget)->wh.vOpHandle_push = vScriptExec->CopyItem(data);
					}
				}
				else if (value->mName.StrLowerCmp("on_release") == 0)
				{
					if (mWidgetName.Cmp("text_editor") == 0)
					{
						if (((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_release)
							delete ((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_release;
						((Grapa_Text_Editor*)vWidget)->wh.vOpHandle_release = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("text_display") == 0)
					{
						if (((Grapa_Text_Display*)vWidget)->wh.vOpHandle_release)
							delete ((Grapa_Text_Display*)vWidget)->wh.vOpHandle_release;
						((Grapa_Text_Display*)vWidget)->wh.vOpHandle_release = vScriptExec->CopyItem(data);
					}
					else if (mWidgetName.Cmp("scrollbar") == 0)
					{
						if (((Grapa_Scrollbar*)vWidget)->wh.vOpHandle_release)
							delete ((Grapa_Scrollbar*)vWidget)->wh.vOpHandle_release;
						((Grapa_Scrollbar*)vWidget)->wh.vOpHandle_release = vScriptExec->CopyItem(data);
					}
				}
				else if (value->mName.StrLowerCmp("on_draw_cell") == 0)
				{
					if (mWidgetName.Cmp("table_row") == 0)
					{
						if (((Grapa_Scrollbar*)vWidget)->wh.vOpTable_drawcell)
							delete ((Grapa_Scrollbar*)vWidget)->wh.vOpTable_drawcell;
						((Grapa_Scrollbar*)vWidget)->wh.vOpTable_drawcell = vScriptExec->CopyItem(data);
					}
				}
				else if (value->mName.StrLowerCmp("jpeg") == 0)
				{
					if (mJpeg)
					{
						delete mJpeg;
						mJpeg = NULL;
					}
					if (mJpegImage)
					{
						vWidget->image(NULL);
						delete mJpeg;
						mJpeg = NULL;
					}
					if (mJpegDeImage)
					{
						vWidget->deimage(NULL);
						delete mJpegDeImage;
						mJpegDeImage = NULL;
					}
					if (data->mValue.mLength)
					{
						mJpeg = new Fl_JPEG_Image("", data->mValue.mBytes);
						mJpeg->RGB_scaling(FL_RGB_SCALING_BILINEAR);

						mJpegImage = mJpeg->copy(vWidget->w(), vWidget->h());
						mJpegImage->RGB_scaling(FL_RGB_SCALING_BILINEAR);
						vWidget->image(mJpegImage);

						mJpegDeImage = mJpeg->copy(vWidget->w(), vWidget->h());
						mJpegDeImage->RGB_scaling(FL_RGB_SCALING_BILINEAR);
						mJpegDeImage->inactive();
						vWidget->deimage(mJpegDeImage);
					}
				}
				else if (value->mName.StrLowerCmp("linesize") == 0)
				{
					if (mWidgetName.Cmp("scrollbar") == 0)
					{
						Grapa_Scrollbar* f = (Grapa_Scrollbar*)vWidget;
						if (data->mValue.mToken == GrapaTokenType::INT)
							f->linesize(GrapaInt(data->mValue).LongValue());
					}
				}
				else if (value->mName.StrLowerCmp("scrollvalue") == 0)
				{
					if (mWidgetName.Cmp("scrollbar") == 0)
					{
						Grapa_Scrollbar* f = (Grapa_Scrollbar*)vWidget;
						if (data->mValue.mToken == GrapaTokenType::INT)
							f->value(GrapaInt(data->mValue).LongValue());
						if (data->mValue.mToken == GrapaTokenType::ARRAY)
						{
							GrapaLibraryParam r1(vScriptExec, vNameSpace, data->vQueue ? data->vQueue->Head(0) : NULL);
							GrapaLibraryParam r2(vScriptExec, vNameSpace, data->vQueue ? data->vQueue->Head(1) : NULL);
							GrapaLibraryParam r3(vScriptExec, vNameSpace, data->vQueue ? data->vQueue->Head(2) : NULL);
							GrapaLibraryParam r4(vScriptExec, vNameSpace, data->vQueue ? data->vQueue->Head(3) : NULL);
							s64 pos = (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::INT) ? GrapaInt(r1.vVal->mValue).LongValue() : 0;
							s64 windowSize = (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT) ? GrapaInt(r2.vVal->mValue).LongValue() : 0;
							s64 first = (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::INT) ? GrapaInt(r3.vVal->mValue).LongValue() : 0;
							s64 total = (r4.vVal && r4.vVal->mValue.mToken == GrapaTokenType::INT) ? GrapaInt(r4.vVal->mValue).LongValue() : 0;
							f->scrollvalue(pos,windowSize,first,total);
						}
					}
				}
				else if (value->mName.StrLowerCmp("label") == 0)
				{
					vWidget->copy_label((char*)data->mValue.mBytes);
				}
				else if (value->mName.StrLowerCmp("labelfont") == 0)
				{
					vWidget->labelfont(TableFont(data->mValue));
				}
				else if (value->mName.StrLowerCmp("get") == 0)
				{
					result = Get(data);
				}
				else if (value->mName.StrLowerCmp("exec") == 0)
				{
					result = vScriptExec->ProcessPlan(vNameSpace, data);
				}
				value = value->Next();
			}
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaWidget::Get(GrapaRuleEvent* data)
{
	GrapaRuleEvent* result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::STR;
	if (data->mValue.mToken == GrapaTokenType::ARRAY)
	{
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();
	}
	if (data->mValue.mToken == GrapaTokenType::ARRAY || data->mValue.mToken == GrapaTokenType::STR)
	{
		GrapaRuleEvent* value = data->mValue.mToken == GrapaTokenType::ARRAY ? data->vQueue->Head() : data;
		while (value)
		{
			GrapaRuleEvent* result2 = new GrapaRuleEvent();
			result2->mName.FROM(value->mValue);
			result2->mName.mToken = GrapaTokenType::STR;
			if (value->mValue.StrLowerCmp("name") == 0)
			{
				result2->mValue.FROM(mName);
			}
			else if (value->mValue.StrLowerCmp("visible") == 0)
			{
				result2->mValue.FROM(GrapaInt(vWidget->visible()).getBytes());
			}
			else if (value->mValue.StrLowerCmp("x") == 0)
			{
				result2->mValue.FROM(GrapaInt(vWidget->x()).getBytes());
			}
			else if (value->mValue.StrLowerCmp("y") == 0)
			{
				result2->mValue.FROM(GrapaInt(vWidget->y()).getBytes());
			}
			else if (value->mValue.StrLowerCmp("w") == 0)
			{
				result2->mValue.FROM(GrapaInt(vWidget->w()).getBytes());
			}
			else if (value->mValue.StrLowerCmp("h") == 0)
			{
				result2->mValue.FROM(GrapaInt(vWidget->h()).getBytes());
			}
			else if (value->mValue.StrLowerCmp("size") == 0)
			{
				result2->mValue.mToken = GrapaTokenType::LIST;
				result2->vQueue = new GrapaRuleQueue();
				result2->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("x"), GrapaInt(vWidget->x()).getBytes()));
				result2->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("y"), GrapaInt(vWidget->y()).getBytes()));
				result2->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("w"), GrapaInt(vWidget->w()).getBytes()));
				result2->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("h"), GrapaInt(vWidget->h()).getBytes()));
			}
			else if (value->mValue.StrLowerCmp("r") == 0)
			{
				if (mWidgetName.Cmp("table_row") == 0)
					result2->mValue.FROM(GrapaInt(((Grapa_Table_Row*)vWidget)->rows()).getBytes());
			}
			else if (value->mValue.StrLowerCmp("c") == 0)
			{
				if (mWidgetName.Cmp("table_row") == 0)
					result2->mValue.FROM(GrapaInt(((Grapa_Table_Row*)vWidget)->cols()).getBytes());
			}
			else if (value->mName.StrLowerCmp("shape") == 0)
			{
				if (mWidgetName.Cmp("table_row") == 0)
				{
					result2->mValue.mToken = GrapaTokenType::LIST;
					result2->vQueue = new GrapaRuleQueue();
					result2->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("r"), GrapaInt(((Grapa_Table_Row*)vWidget)->rows()).getBytes()));
					result2->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("c"), GrapaInt(((Grapa_Table_Row*)vWidget)->cols()).getBytes()));
				}
			}
			else if (value->mValue.StrLowerCmp("label") == 0)
			{
				result2->mValue.FROM(vWidget->label());
			}
			else if (value->mValue.StrLowerCmp("linesize") == 0)
			{
				if (mWidgetName.Cmp("scrollbar") == 0)
				{
					Grapa_Scrollbar* f = (Grapa_Scrollbar*)vWidget;
					result2->mValue.FROM(GrapaInt(f->linesize()).getBytes());
				}
			}
			else if (value->mValue.StrLowerCmp("scrollvalue") == 0)
			{
				if (mWidgetName.Cmp("scrollbar") == 0)
				{
					Grapa_Scrollbar* f = (Grapa_Scrollbar*)vWidget;
					result2->mValue.FROM(GrapaInt(f->value()).getBytes());
				}
			}
			else if (mWidgetName.Cmp("text_display") == 0)
			{
				Grapa_Text_Display* f = (Grapa_Text_Display*)vWidget;
				f->Get(value->mValue, result2->mValue);
			}
			else if (mWidgetName.Cmp("text_editor") == 0)
			{
				Grapa_Text_Editor* f = (Grapa_Text_Editor*)vWidget;
				f->Get(value->mValue, result2->mValue);
			}
			else if (mWidgetName.Cmp("menu_bar") == 0)
			{
				if (value->mValue.mToken == GrapaTokenType::LIST && value->vQueue)
				{
					result2->mValue.mToken = GrapaTokenType::LIST;
					result2->vQueue = new GrapaRuleQueue();
					result2->mName.FROM("children");
					result2->mName.mToken = GrapaTokenType::STR;

					GrapaRuleEvent* me = value->vQueue->Head();
					while (me)
					{
						GrapaWidgetMenuList* menuList = MenuFromName((char*)me->mName.mBytes);
						if (menuList == NULL) menuList = MenuFromPath((char*)me->mName.mBytes);
						Fl_Menu_Item* mi = NULL;
						if (menuList)
							mi = (Fl_Menu_Item*)((Grapa_Menu_Bar*)vWidget)->find_item((char*)menuList->mPath.mBytes);
						if (mi)
						{
							GrapaRuleEvent* result3 = new GrapaRuleEvent();
							result2->vQueue->PushTail(result3);
							result3->mValue.mToken = GrapaTokenType::LIST;
							result3->vQueue = new GrapaRuleQueue();
							result3->mName.FROM(me->mName);
							result3->mName.mToken = GrapaTokenType::STR;
							if (me->mValue.mToken == GrapaTokenType::ARRAY || me->mValue.mToken == GrapaTokenType::STR)
							{
								GrapaRuleEvent* me2 = me->mValue.mToken == GrapaTokenType::ARRAY ? me->vQueue->Head() : me;
								while (me2)
								{
									if (me2->mValue.StrLowerCmp("shortcut") == 0)
									{
										result2->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("shortcut"), GrapaInt(mi->shortcut()).getBytes()));
									}
									else if (me2->mValue.StrLowerCmp("options") == 0 || me2->mValue.StrLowerCmp("flags") == 0)
									{
										GrapaRuleEvent* fl = new GrapaRuleEvent();
										fl->mValue.mToken = GrapaTokenType::ARRAY;
										fl->mName.FROM("flags");
										fl->mName.mToken = GrapaTokenType::STR;
										fl->vQueue = new GrapaRuleQueue();
										if (mi->flags & FL_MENU_INACTIVE) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("INACTIVE")));
										if (mi->flags & FL_MENU_TOGGLE) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("TOGGLE")));
										if (mi->flags & FL_MENU_VALUE) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("VALUE")));
										if (mi->flags & FL_MENU_RADIO) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("RADIO")));
										if (mi->flags & FL_MENU_INVISIBLE) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("INVISIBLE")));
										if (mi->flags & FL_SUBMENU_POINTER) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("POINTER")));
										if (mi->flags & FL_SUBMENU) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("SUBMENU")));
										if (mi->flags & FL_MENU_DIVIDER) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("DIVIDER")));
										if (mi->flags & FL_MENU_HORIZONTAL) fl->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR("HORIZONTAL")));
										result3->vQueue->PushTail(fl);
									}
									else if (me2->mValue.StrLowerCmp("labelsize") == 0)
									{
										result3->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("labelsize"), GrapaInt(mi->labelsize()).getBytes()));
									}
									else if (me2->mValue.StrLowerCmp("data") == 0)
									{
										GrapaRuleEvent* result4 = vScriptExec->CopyItem(menuList->vData);
										result4->mName.FROM("data");
										result4->mName.mToken = GrapaTokenType::STR;
										result3->vQueue->PushTail(result4);
									}
									else if (me2->mValue.StrLowerCmp("callback") == 0)
									{
										GrapaRuleEvent* result4 = vScriptExec->CopyItem(menuList->vCallback);
										result4->mName.FROM("callback");
										result4->mName.mToken = GrapaTokenType::STR;
										result3->vQueue->PushTail(result4);
									}
									else if (me2->mValue.StrLowerCmp("name") == 0)
									{
										result3->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("name"), menuList->mName));
									}
									else if (me2->mValue.StrLowerCmp("label") == 0)
									{
										result3->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("label"), GrapaCHAR(mi->text)));
									}
									else if (me2->mValue.StrLowerCmp("path") == 0)
									{
										result3->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("path"), menuList->mPath));
									}
									if (me->mValue.mToken != GrapaTokenType::ARRAY)
									{
										break;
									}
									me2 = me2->Next();
								}
							}
						}
						me = me->Next();
					}
				}
			}

			if (data->mValue.mToken != GrapaTokenType::ARRAY)
			{
				delete result;
				result = result2;
				break;
			}
			result->vQueue->PushTail(result2);
			value = value->Next();
		}
	}
	return(result);
}

unsigned int GrapaWidget::TableColor(GrapaCHAR& pValue)
{
	if (pValue.mToken == GrapaTokenType::STR)
	{
		if (pValue.StrLowerCmp("BACKGROUND_COLOR") == 0) return(FL_BACKGROUND_COLOR);
		else if (pValue.StrLowerCmp("BACKGROUND2_COLOR") == 0) return(FL_BACKGROUND2_COLOR);
		else if (pValue.StrLowerCmp("FOREGROUND_COLOR") == 0) return(FL_FOREGROUND_COLOR);
		else if (pValue.StrLowerCmp("INACTIVE_COLOR") == 0) return(FL_INACTIVE_COLOR);
		else if (pValue.StrLowerCmp("SELECTION_COLOR") == 0) return(FL_SELECTION_COLOR);
		else if (pValue.StrLowerCmp("BLACK") == 0) return(FL_BLACK);
		else if (pValue.StrLowerCmp("BLUE") == 0) return(FL_BLUE);
		else if (pValue.StrLowerCmp("CYAN") == 0) return(FL_CYAN);
		else if (pValue.StrLowerCmp("DARK_BLUE") == 0) return(FL_DARK_BLUE);
		else if (pValue.StrLowerCmp("DARK_CYAN") == 0) return(FL_DARK_CYAN);
		else if (pValue.StrLowerCmp("DARK_GREEN") == 0) return(FL_DARK_GREEN);
		else if (pValue.StrLowerCmp("DARK_MAGENTA") == 0) return(FL_DARK_MAGENTA);
		else if (pValue.StrLowerCmp("DARK_RED") == 0) return(FL_DARK_RED);
		else if (pValue.StrLowerCmp("DARK_YELLOW") == 0) return(FL_DARK_YELLOW);
		else if (pValue.StrLowerCmp("GREEN") == 0) return(FL_GREEN);
		else if (pValue.StrLowerCmp("MAGENTA") == 0) return(FL_MAGENTA);
		else if (pValue.StrLowerCmp("RED") == 0) return(FL_RED);
		else if (pValue.StrLowerCmp("WHITE") == 0) return(FL_WHITE);
		else if (pValue.StrLowerCmp("YELLOW") == 0) return(FL_YELLOW);
		else if (pValue.StrLowerCmp("GRAY0") == 0) return(FL_GRAY0);
		else if (pValue.StrLowerCmp("DARK3") == 0) return(FL_DARK3);
		else if (pValue.StrLowerCmp("DARK2") == 0) return(FL_DARK2);
		else if (pValue.StrLowerCmp("DARK1") == 0) return(FL_DARK1);
		else if (pValue.StrLowerCmp("LIGHT1") == 0) return(FL_LIGHT1);
		else if (pValue.StrLowerCmp("LIGHT2") == 0) return(FL_LIGHT2);
		else if (pValue.StrLowerCmp("LIGHT3") == 0) return(FL_LIGHT3);
	}
	else if (pValue.mToken == GrapaTokenType::INT)
	{
		return GrapaInt(pValue).LongValue();
	}
	return(0);
}

unsigned int GrapaWidget::TableFont(GrapaCHAR& pValue)
{
	if (pValue.StrLowerCmp("HELVETICA") == 0) return(FL_HELVETICA);
	else if (pValue.StrLowerCmp("HELVETICA_BOLD") == 0) return(FL_HELVETICA_BOLD);
	else if (pValue.StrLowerCmp("HELVETICA_ITALIC") == 0) return(FL_HELVETICA_ITALIC);
	else if (pValue.StrLowerCmp("HELVETICA_BOLD_ITALIC") == 0) return(FL_HELVETICA_BOLD_ITALIC);
	else if (pValue.StrLowerCmp("COURIER") == 0) return(FL_COURIER);
	else if (pValue.StrLowerCmp("COURIER_BOLD") == 0) return(FL_COURIER_BOLD);
	else if (pValue.StrLowerCmp("COURIER_ITALIC") == 0) return(FL_COURIER_ITALIC);
	else if (pValue.StrLowerCmp("COURIER_BOLD_ITALIC") == 0) return(FL_COURIER_BOLD_ITALIC);
	else if (pValue.StrLowerCmp("TIMES") == 0) return(FL_TIMES);
	else if (pValue.StrLowerCmp("TIMES_BOLD") == 0) return(FL_TIMES_BOLD);
	else if (pValue.StrLowerCmp("TIMES_ITALIC") == 0) return(FL_TIMES_ITALIC);
	else if (pValue.StrLowerCmp("TIMES_BOLD_ITALIC") == 0) return(FL_TIMES_BOLD_ITALIC);
	else if (pValue.StrLowerCmp("SYMBOL") == 0) return(FL_SYMBOL);
	else if (pValue.StrLowerCmp("SCREEN") == 0) return(FL_SCREEN);
	else if (pValue.StrLowerCmp("SCREEN_BOLD") == 0) return(FL_SCREEN_BOLD);
	else if (pValue.StrLowerCmp("ZAPF_DINGBATS") == 0) return(FL_ZAPF_DINGBATS);
	else if (pValue.StrLowerCmp("FREE_FONT") == 0) return(FL_FREE_FONT);
	return(0);
}

unsigned int GrapaWidget::TableCursorStyle(GrapaCHAR& pValue)
{
	if (pValue.StrLowerCmp("CURSOR_DEFAULT") == 0) return(FL_CURSOR_DEFAULT);
	else if (pValue.StrLowerCmp("CURSOR_ARROW") == 0) return(FL_CURSOR_ARROW);
	else if (pValue.StrLowerCmp("CURSOR_CROSS") == 0) return(FL_CURSOR_CROSS);
	else if (pValue.StrLowerCmp("CURSOR_WAIT") == 0) return(FL_CURSOR_WAIT);
	else if (pValue.StrLowerCmp("CURSOR_INSERT") == 0) return(FL_CURSOR_INSERT);
	else if (pValue.StrLowerCmp("CURSOR_HAND") == 0) return(FL_CURSOR_HAND);
	else if (pValue.StrLowerCmp("CURSOR_HELP") == 0) return(FL_CURSOR_HELP);
	else if (pValue.StrLowerCmp("CURSOR_MOVE") == 0) return(FL_CURSOR_MOVE);
	else if (pValue.StrLowerCmp("CURSOR_NS") == 0) return(FL_CURSOR_NS);
	else if (pValue.StrLowerCmp("CURSOR_WE") == 0) return(FL_CURSOR_WE);
	else if (pValue.StrLowerCmp("CURSOR_NWSE") == 0) return(FL_CURSOR_NWSE);
	else if (pValue.StrLowerCmp("CURSOR_NESW") == 0) return(FL_CURSOR_NESW);
	else if (pValue.StrLowerCmp("CURSOR_NONE") == 0) return(FL_CURSOR_NONE);
	return(0);
}

void GrapaWidget::SetThread()
{
	if (mThreadSet) return;
	mThreadSet = true;
	mNameSpace.SetResponse(&mThread);
	mThread.mSend.mScriptState.SetNameSpace(&mNameSpace);
	mThread.mSend.Start();
	vThread = &mThread;
	vScriptExec = vThread->mSend.mScriptState.vScriptExec;
	mThread.mWorker.Set(vScriptExec, &mNameSpace);
}

GrapaError GrapaWidget::Post(GrapaRuleEvent* pRunCode, GrapaRuleEvent* pParam, GrapaRuleEvent* pDoneCode)
{
	SetThread();
	mThread.mWorker.Run(pRunCode, pParam, pDoneCode);
	return(0);
}

GrapaWidgetMenuList* GrapaWidget::MenuFromName(char* pName)
{
	for (GrapaWidgetMenuList* menu : mOpListMenu)
	{
		if (menu->mName.StrLowerCmp(pName) == 0) return menu;
	}
	return(NULL);
}

GrapaWidgetMenuList* GrapaWidget::MenuFromPath(char* pLabel)
{
	for (GrapaWidgetMenuList* menu : mOpListMenu)
	{
		if (menu->mPath.StrLowerCmp(pLabel) == 0) return menu;
	}
	return(NULL);
}

void GrapaWidget::MenuDeleteFromName(GrapaCHAR& pName)
{
	std::list<GrapaWidgetMenuList*>::iterator menu = mOpListMenu.begin();
	while (menu != mOpListMenu.end())
	{
		if (((GrapaWidgetMenuList*)*menu)->mName.StrLowerCmp((char*)pName.mBytes) == 0)
		{
			if (((GrapaWidgetMenuList*)*menu)->vCallback)
			{
				((GrapaWidgetMenuList*)*menu)->vCallback->CLEAR();
				delete ((GrapaWidgetMenuList*)*menu)->vCallback;
				((GrapaWidgetMenuList*)*menu)->vCallback = NULL;
			}
			if (((GrapaWidgetMenuList*)*menu)->vData)
			{
				((GrapaWidgetMenuList*)*menu)->vData->CLEAR();
				delete ((GrapaWidgetMenuList*)*menu)->vData;
				((GrapaWidgetMenuList*)*menu)->vData = NULL;
			}
			menu = mOpListMenu.erase(menu);
		}
		else
			menu++;
	}
}

void GrapaWidget::MenuDeleteFromPath(GrapaCHAR& pLabel)
{
	std::list<GrapaWidgetMenuList*>::iterator menu = mOpListMenu.begin();
	while (menu != mOpListMenu.end())
	{
		if (((GrapaWidgetMenuList*)*menu)->mPath.StrNCmp((char*)pLabel.mBytes, pLabel.mLength) == 0)
		{
			if (((GrapaWidgetMenuList*)*menu)->vCallback)
			{
				((GrapaWidgetMenuList*)*menu)->vCallback->CLEAR();
				delete ((GrapaWidgetMenuList*)*menu)->vCallback;
				((GrapaWidgetMenuList*)*menu)->vCallback = NULL;
			}
			if (((GrapaWidgetMenuList*)*menu)->vData)
			{
				((GrapaWidgetMenuList*)*menu)->vData->CLEAR();
				delete ((GrapaWidgetMenuList*)*menu)->vData;
				((GrapaWidgetMenuList*)*menu)->vData = NULL;
			}
			menu = mOpListMenu.erase(menu);
		}
		else
			menu++;
	}
}

void GrapaWidget::Clear()
{
	if (mWidgetName.Cmp("menu_bar") == 0)
	{
		std::list<GrapaWidgetMenuList*>::iterator menu = mOpListMenu.begin();
		while (menu != mOpListMenu.end())
		{
			if (((GrapaWidgetMenuList*)*menu)->vCallback)
			{
				((GrapaWidgetMenuList*)*menu)->vCallback->CLEAR();
				delete ((GrapaWidgetMenuList*)*menu)->vCallback;
				((GrapaWidgetMenuList*)*menu)->vCallback = NULL;
			}
			if (((GrapaWidgetMenuList*)*menu)->vData)
			{
				((GrapaWidgetMenuList*)*menu)->vData->CLEAR();
				delete ((GrapaWidgetMenuList*)*menu)->vData;
				((GrapaWidgetMenuList*)*menu)->vData = NULL;
			}
			menu = mOpListMenu.erase(menu);
		}
	}
	else
	{
		Fl_Group* g = vWidget->as_group();
		if (g)
		{
			while (g->children())
			{
				Fl_Widget* t = g->child(0);
				g->remove(0);
				GrapaWidget* x = (GrapaWidget*)t->user_data();
				if (x && x->mDelete)
					delete t;
			}
		}
	}
}

GrapaWidgetThread::GrapaWidgetThread()
	: GrapaConsoleResponse()
{
	widget = NULL;
	mScriptExec.vScriptState = &mSend.mScriptState;
	mSend.mScriptState.vScriptExec = &mScriptExec;
	startCB = echoCB = promptCB = endCB = messageCB = NULL;
}

GrapaWidgetThread::~GrapaWidgetThread()
{
	if (startCB) { startCB->CLEAR(); delete startCB; startCB = NULL; }
	if (echoCB) { echoCB->CLEAR(); delete echoCB; echoCB = NULL; }
	if (promptCB) { promptCB->CLEAR(); delete promptCB; promptCB = NULL; }
	if (endCB) { endCB->CLEAR(); delete endCB; endCB = NULL; }
	if (messageCB) { messageCB->CLEAR(); delete messageCB; messageCB = NULL; }
}

void GrapaWidgetThread::SendStart(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace)
{
	if (startCB && widget)
	{
		GrapaRuleQueue q;
		GrapaRuleEvent wid;
		//wid.mName.FROM("o");
		wid.mValue.mToken = GrapaTokenType::WIDGET;
		wid.vWidget = widget;
		wid.vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
		q.PushTail(&wid);
		GrapaRuleEvent* e = NULL;
		e = vScriptExec->ProcessPlan(pNameSpace, startCB, &wid);
		if (e) delete e;
		wid.vClass = NULL;
		wid.vWidget = NULL;
		q.PopHead();
		q.CLEAR();
	}
}

GrapaRuleEvent* GrapaWidgetThread::SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	GrapaRuleEvent* result = NULL;
	if (messageCB && widget)
	{
		GrapaRuleQueue q;
		GrapaRuleEvent wid;
		//wid.mName.FROM("o");
		wid.mValue.mToken = GrapaTokenType::WIDGET;
		wid.vWidget = widget;
		wid.vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
		q.PushTail(&wid);
		GrapaRuleEvent wid2;
		wid2.mValue.mToken = GrapaTokenType::PTR;
		wid2.vRulePointer = pValue;
		while (wid2.vRulePointer && wid2.vRulePointer->mValue.mToken == GrapaTokenType::PTR && wid2.vRulePointer->vRulePointer) wid2.vRulePointer = wid2.vRulePointer->vRulePointer;
		wid2.mVar = true;
		//wid2.mName.FROM("data");
		q.PushTail(&wid2);
		GrapaRuleEvent* e = NULL;
		result = vScriptExec->ProcessPlan(pNameSpace, messageCB, &wid);
		wid.vWidget = NULL;
		q.PopHead();
		q.PopHead();
		q.CLEAR();
	}
	else if (widget)
	{
		result = widget->Set(pValue);
	}
	return(result);
}
void GrapaWidgetThread::SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
{
	if (echoCB && widget)
	{
		GrapaRuleQueue q;
		GrapaRuleEvent wid;
		//wid.mName.FROM("o");
		wid.mValue.mToken = GrapaTokenType::WIDGET;
		wid.vWidget = widget;
		wid.vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
		q.PushTail(&wid);
		q.PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR((char*)sendbuf, sendbuflen)));
		GrapaRuleEvent* e = NULL;
		e = vScriptExec->ProcessPlan(pNameSpace, echoCB, &wid);
		if (e) delete e;
		wid.vClass = NULL;
		wid.vWidget = NULL;
		q.PopHead();
		q.CLEAR();
	}
};
void GrapaWidgetThread::SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
	if (promptCB && widget)
	{
		GrapaRuleQueue q;
		GrapaRuleEvent wid;
		//wid.mName.FROM("o");
		wid.mValue.mToken = GrapaTokenType::WIDGET;
		wid.vWidget = widget;
		wid.vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
		q.PushTail(&wid);
		GrapaRuleEvent wid2;
		//wid2.mName.FROM("data");
		wid2.mValue.FROM(sendbuf);
		q.PushTail(&wid2);
		GrapaRuleEvent* e = NULL;
		e = vScriptExec->ProcessPlan(pNameSpace, promptCB, &wid);
		if (e) delete e;
		wid.vClass = NULL;
		wid.vWidget = NULL;
		q.PopHead();
		q.PopHead();
		q.CLEAR();
	}
};
void GrapaWidgetThread::SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (endCB && widget)
	{
		GrapaRuleQueue q;
		GrapaRuleEvent wid;
		//wid.mName.FROM("o");
		wid.mValue.mToken = GrapaTokenType::WIDGET;
		wid.vWidget = widget;
		wid.vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
		q.PushTail(&wid);
		GrapaRuleEvent wid2;
		wid2.mValue.mToken = GrapaTokenType::PTR;
		wid2.vRulePointer = pValue;
		while (wid2.vRulePointer && wid2.vRulePointer->mValue.mToken==GrapaTokenType::PTR && wid2.vRulePointer->vRulePointer) wid2.vRulePointer = wid2.vRulePointer->vRulePointer;
		wid2.mVar = true;
		//wid2.mName.FROM("data");
		q.PushTail(&wid2);
		GrapaRuleEvent* e = NULL;
		e = vScriptExec->ProcessPlan(pNameSpace, endCB, &wid);
		if (e) delete e;
		wid.vWidget = NULL;
		q.PopHead();
		q.PopHead();
		q.CLEAR();
	}
};

////////////////////////////////////////////////////////////////////////////////
