// GrapaSystem.cpp
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

#include "GrapaSystem.h"
#include "GrapaLink.h"

#include <openssl/rand.h>

#undef min
#undef max

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

extern GrapaSystem* gSystem;

GrapaSystem::GrapaSystem()
{
	RandSeed();
#ifdef WIN32
	mStdinRef = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(mStdinRef, &mStdinMode);
#else
	mStdinRef = STDIN_FILENO;
	tcgetattr(mStdinRef, &mStdinMode);
#endif
	mLib.mValue.FROM("$SYS");
	mPath = NULL;
	mVersion = NULL;
	mStaticLib = NULL;
	mArgv = new GrapaRuleQueue();
	mLinkInitialized = false;
}

GrapaSystem::~GrapaSystem()
{
#ifdef WIN32
	SetConsoleMode(mStdinRef, mStdinMode);
#else
	(void)tcsetattr(mStdinRef, TCSANOW, &mStdinMode);
#endif
	if (mPath)
	{
		mPath->CLEAR();
		delete mPath;
		mPath = NULL;
	}
	if (mVersion)
	{
		mVersion->CLEAR();
		delete mVersion;
		mVersion = NULL;
	}
	if (mStaticLib)
	{
		mStaticLib->CLEAR();
		delete mStaticLib;
		mStaticLib = NULL;
	}
	if (mArgv)
	{
		mArgv->CLEAR();
		delete mArgv;
		mArgv = NULL;
	}
}

//void GrapaSystem::Print(const char* pStr, bool flush)
//{
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		mGfxConsole.disp->buffer()->append((char*)pStr);
//	}
//	else
//	{
//		if (pStr) std::cout << (char*)pStr;
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::Print(const char pChar, bool flush)
//{
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		char v[2];
//		v[0] = pChar;
//		v[1] = 0;
//		mGfxConsole.disp->buffer()->append((char*)v);
//	}
//	else
//	{
//		std::cout << (char)pChar;
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::Print(const GrapaCHAR& pValue, bool flush)
//{
//	GrapaCHAR s((char*)pValue.mBytes, pValue.mLength);
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		mGfxConsole.disp->buffer()->append((char*)pValue.mBytes);
//	}
//	else
//	{
//		if (s.mBytes) std::cout << (char*)s.mBytes;
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::PrintLine(const GrapaCHAR& pValue, bool flush)
//{
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		mGfxConsole.disp->buffer()->append((char*)pValue.mBytes);
//	}
//	else
//	{
//		if (pValue.mBytes) std::cout << (char*)pValue.mBytes;
//		std::cout << (char*)"\r\n";
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::PrintTimeLine(const char* pChar, bool flush)
//{
//	GrapaTIME t;
//	t.SetNow();
//	GrapaCHAR s(t.getString());
//	for (s64 i = s.mLength; i < 27;i++)
//		s.Append(' ');
//	s.Append(pChar);
//	PrintLine(s,flush);
//}
//
//void GrapaSystem::Print(const GrapaSU64& pValue, bool flush)
//{
//	GrapaCHAR value(pValue.mNum);
//	Print(value, flush);
//}
//
//void GrapaSystem::Print(u64 pValue, bool flush)
//{
//	GrapaCHAR value(pValue);
//	Print(value, flush);
//}

int GrapaSystem::GetChar()
{
	int ch=0;
#ifdef WIN32
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms682062(v=vs.85).aspx
	DWORD read;
	BOOL x = ReadConsoleA(mStdinRef, &ch, 1, &read, NULL);
#else
	ch = getchar();
#endif
	return ch;
}

void GrapaSystem::SetEcho(bool enable)
{
#ifdef WIN32
	DWORD mode;
	GetConsoleMode(mStdinRef, &mode);
	if (!enable)
		mode &= ~ENABLE_ECHO_INPUT;
	else
		mode |= ENABLE_ECHO_INPUT;
	SetConsoleMode(mStdinRef, mode);

#else
	struct termios tty;
	tcgetattr(mStdinRef, &tty);
	if (!enable)
	{
		tty.c_lflag &= ~ICANON;
		tty.c_lflag &= ~ECHO;
	}
	else
	{
		tty.c_lflag |= ICANON;
		tty.c_lflag |= ECHO;
	}
	(void)tcsetattr(mStdinRef, TCSANOW, &tty);
#endif
}

void GrapaSystem::SetLineInput(bool enable)
{
#ifdef WIN32
	DWORD mode;
	GetConsoleMode(mStdinRef, &mode);
	if (!enable)
	{
		mode &= ~ENABLE_LINE_INPUT;
		mode &= ~ENABLE_EXTENDED_FLAGS;
		mode &= ~ENABLE_INSERT_MODE;
		mode &= ~ENABLE_MOUSE_INPUT;
		mode &= ~ENABLE_PROCESSED_INPUT;
		mode &= ~ENABLE_QUICK_EDIT_MODE;
		mode &= ~ENABLE_WINDOW_INPUT;
	}
	else
	{
		mode |= ENABLE_LINE_INPUT;
		mode |= ENABLE_EXTENDED_FLAGS;
		mode |= ENABLE_INSERT_MODE;
		mode |= ENABLE_MOUSE_INPUT;
		mode |= ENABLE_PROCESSED_INPUT;
		mode |= ENABLE_QUICK_EDIT_MODE;
		mode |= ENABLE_WINDOW_INPUT;
	}
	SetConsoleMode(mStdinRef, mode);

#else
	struct termios tty;
	tcgetattr(mStdinRef, &tty);
	if (!enable)
		tty.c_lflag &= ~ICANON;
	else
		tty.c_lflag |= ICANON;
	(void)tcsetattr(mStdinRef, TCSANOW, &tty);
#endif
}

void GrapaSystem::RandSeed(u64 pSeed)
{
	u32 seed = Random32();
	if (pSeed)
	{
		for (size_t i = 0; i < sizeof(u64); i++)
			seed = seed * (0xff + 2u) + ((u8*)&pSeed)[i];
	}
	GrapaTIME t;
	t.SetNow();
	u64 timeval = t.mSec.LongValue();
	unsigned char *ptr = (unsigned char *)&timeval;
	for (size_t i = 0; i < sizeof(u64); i++)
		seed = seed * (0xff + 2u) + ptr[i];
	srand(seed);
	u32 rd = Random32();
	RAND_seed((void*)&rd, 32);
}

GrapaCHAR GrapaSystem::exec(GrapaCHAR& command)
{
	char buffer[128];
	GrapaCHAR result;
#ifdef WIN32
	FILE* pipe = _popen((char*)command.mBytes, "r");
#else
	FILE* pipe = popen((char*)command.mBytes, "r");
#endif
	if (pipe)
	{
		while (!feof(pipe))
		{
			if (fgets(buffer, 128, pipe) != NULL)
				result.Append(buffer);
		}
#ifdef WIN32
		_pclose(pipe);
#else
		pclose(pipe);
#endif
	}
	else
	{ 
	}
	return result;
}

void GrapaConsole2Response::SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
{
	if (sendbuflen) std::cout << (char*)sendbuf;
	std::cout.flush();
}

void GrapaConsole2Response::SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
	if (sendbuf.mLength) std::cout << (char*)sendbuf.mBytes;
	std::cout.flush();
}

void GrapaConsole2Response::SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (pValue)
	{
		if (pValue->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
	std::cout << (char*)"\n";
	std::cout.flush();
}

My_Console::My_Console()
{
	mRuleVariables.SetResponse(&mConsole2Response);
	mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
}

My_Console::~My_Console()
{
	Stop();
}

void My_Console::Start(GrapaCHAR& in, GrapaCHAR& out, GrapaCHAR& run)
{
	GrapaError err = mConsoleSend.Start();
	GrapaCHAR grresult;
	if (gSystem->mGrammar.mLength) grresult = mConsoleSend.SendSync(gSystem->mGrammar);
	if (grresult.mLength)
	{
		std::cout << (char*)grresult.mBytes;
		std::cout << (char*)"\n";
	}
	if (out.mLength)
	{
		std::cout << (char*)out.mBytes;
		std::cout << (char*)"\n";
	}
	if (run.mLength) grresult = mConsoleSend.SendSync(run);
	if (grresult.mLength)
	{
		std::cout << (char*)grresult.mBytes;
		std::cout << (char*)"\n";
	}
}

void My_Console::Stop()
{
	mConsoleSend.mScriptState.mSync = false;
	mConsoleSend.Stop();
}

void My_Console::Run(GrapaCB cb, void* data)
{
	mConsoleSend.mScriptState.EnablePrompt(&mRuleVariables);
	while (!gSystem->mStop)
	{
		GrapaCHAR sendBuffer;
		int ch = 0;
		do {
			while (((ch = gSystem->GetChar()) != EOF) && (ch != '\n') && (ch != '\r'))
				sendBuffer.Append((char)ch);
			if (sendBuffer.mLength == 1 && sendBuffer.StrNCmp(".") == 0)
				return;
			if (ch == EOF || ch == '\n')
			{
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec,&mRuleVariables,(char*)sendBuffer.mBytes, sendBuffer.mLength);
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec, &mRuleVariables, (u8*)"$\n", 2);
				sendBuffer.SetLength(0);
			}
		} while (!gSystem->mStop && (sendBuffer.mLength || ch == '\n' || ch == '\r') && ch != EOF);
	}
}

My_Pack::My_Pack(int x, int y, int w, int h, const char* l)
	: Fl_Pack(x, y, w, h, l)
{
};

My_Text_Editor::My_Text_Editor(int X, int Y, int W, int H, const char* l)
	: Fl_Text_Editor(X, Y, W, H, l) {
	vConsoleSend = NULL;
	vBuff = new Fl_Text_Buffer();
	buffer(vBuff);
	wrap_mode(WRAP_AT_BOUNDS, 0);
	show_cursor();
	//vBuff->add_modify_callback(My_Text_Editor::buffer_modified_cb2, this);
	resize(x(), y(), w(), h());
}

My_Text_Editor::~My_Text_Editor()
{
}

int My_Text_Editor::handle(int event) {
	int handled = 0;
	switch (event) {
	case FL_KEYDOWN:
		if ((Fl::event_key() & 0x7f) == 0x0d && Fl::event_key(0xffe1)) {
			sendtext();
			return 1;
		}
		handled = Fl_Text_Editor::handle(event);
		break;
	case FL_MOUSEWHEEL:
		return 0;
	default:
		handled = Fl_Text_Editor::handle(event);
		break;
	}
	return(handled);
}

void My_Text_Editor::cleartext()
{
	Fl_Text_Display* t = ((My_Group*)parent()->parent())->disp;
	t->buffer()->remove(0, t->buffer()->length());
}

void My_Text_Editor::sendtext()
{
	char* s = buffer()->selected() ? buffer()->selection_text() : buffer()->text();
	GrapaCHAR sendBuffer(s);
	free(s);
	if (sendBuffer.mLength)
	{
		cleartext();
		vConsoleSend->mScriptState.ClearQueue();
		vConsoleSend->SetActive(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, false);
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, sendBuffer.mBytes, sendBuffer.mLength);
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
	}
}

void My_Text_Editor::sendtext(GrapaCHAR sendBuffer)
{
	vConsoleSend->mScriptState.ClearQueue();
	vConsoleSend->SetActive(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, false);
	vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, sendBuffer.mBytes, sendBuffer.mLength);
	vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
}

//void My_Text_Editor::buffer_modified_cb2(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg)
//{
//	buffer_modified_cb(pos, nInserted, nDeleted, nRestyled, deletedText, cbArg);
//	My_Text_Editor*v=(My_Text_Editor*)cbArg;
//	if (v->oldB != v->mNBufferLines)
//	{
//		v->parent()->parent()->parent()->parent()->redraw();
//		v->oldB = v->mNBufferLines;
//	}
//}

void My_Text_Editor::resize(int X, int Y, int W, int H)
{
	Fl_Text_Editor::resize(X, Y, W, H);
	if (H != ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize)
	{
		Fl_Text_Editor::resize(X, Y, W, ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize);
		if (parent())
			parent()->parent()->parent()->parent()->redraw();
	}
}

My_Text_Display::My_Text_Display(int X, int Y, int W, int H, const char* l)
	: Fl_Text_Display(X, Y, W, H, l) {
	vBuff = new Fl_Text_Buffer();
	buffer(vBuff);
	wrap_mode(WRAP_AT_BOUNDS, 0);
	hide_cursor();
	resize(x(), y(), w(), h());
}

My_Text_Display::~My_Text_Display()
{
}

int My_Text_Display::handle(int event) {
	int handled = 0;
	switch (event) {
	case FL_MOUSEWHEEL:
		break;
	default:
		handled = Fl_Text_Display::handle(event);
		break;
	}
	return(handled);
}

void My_Text_Display::resize(int X, int Y, int W, int H)
{
	Fl::lock();      // acquire the lock
	Fl_Text_Display::resize(X, Y, W, ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize);
	Fl::unlock();    // release the lock; allow other threads to access FLTK again
	Fl::awake();     // use Fl::awake() to signal main thread to refresh the GUI
}

My_Button::My_Button(int X, int Y, int W, int H, const char* l)
	: Fl_Button(X, Y, W, H, l) {
	// do initialization stuff...
}

void My_Button::add(Fl_Widget* item, void* data)
{
	My_Button* a = (My_Button*)item;
	My_Group* b = (My_Group*)data;
	My_Group* grp2 = new My_Group(10, 30, 640 - 20, 180 - 75, b->mConsoleResponse.mRuleVariables.GetParrent());
	b->parent()->insert(*grp2, b->parent()->find(b) + 1);
	b->parent()->parent()->redraw();
}

void My_Button::del(Fl_Widget* item, void* data)
{
	My_Button* a = (My_Button*)item;
	My_Group* b = (My_Group*)data;

	int n = b->parent()->children();
	int p = b->parent()->find(b)+1;
	if (n > p)
	{
		My_Group* c = (My_Group*)b->parent()->child(p);
		c->mConsoleSend.Stop();
		b->parent()->remove(*c);
		delete c;
		b->parent()->parent()->redraw();
	}
}

My_Prompt_Editor::My_Prompt_Editor(int X, int Y, int W, int H, GrapaNames* pNameSpace, My_Text_Console* pWin)
	: Fl_Text_Editor(X, Y, W, H, 0) {
	vWin = pWin;
	vBuff = new Fl_Text_Buffer();
	buffer(vBuff);
	wrap_mode(WRAP_AT_BOUNDS,0);
	show_cursor();
	vConsoleSend = &mPromptSend;
	mPromptResponse.disp = this;
	mPromptResponse.mRuleVariables.mSkip = true;
	mPromptSend.mScriptState.SetNameSpace(&mPromptResponse.mRuleVariables);
	GrapaNames* vNames = pNameSpace;
	while (vNames->GetParrent()) vNames = vNames->GetParrent();
	mPromptResponse.mRuleVariables.SetParrent(vNames);
	mPromptResponse.mRuleVariables.SetResponse(&mPromptResponse);
	mPromptSend.mScriptState.EnablePrompt(&mPromptResponse.mRuleVariables);
	mPromptSend.Start();
	//vBuff->add_modify_callback(My_Prompt_Editor::buffer_modified_cb2, this);
}

My_Prompt_Editor::~My_Prompt_Editor()
{
	mPromptSend.mScriptState.mSync = false;
	mPromptSend.Stop();
}

int My_Prompt_Editor::handle(int event) {
	int handled = 0;
	switch (event) {
	case FL_KEYDOWN:
		if ((Fl::event_key() & 0x7f) == 0x0d) {
			sendtext();
			return 1;
		}
		handled = Fl_Text_Editor::handle(event);
		break;
	default:
		handled = Fl_Text_Editor::handle(event);
		break;
	}
	return(handled);
}

void My_Prompt_Editor::sendtext()
{
	bool isSelected = buffer()->selected();
	char* s = NULL;
	if (isSelected)
	{
		s = buffer()->selection_text();
	}
	else
	{
		s = buffer()->line_text(buffer()->length());
	}
	GrapaCHAR sendBuffer;
	if (s)
	{
		sendBuffer.FROM(s);
		free(s);
		s = NULL;
	}
	if (!isSelected && sendBuffer.StrNCmp((char*)mLastPrompt.mBytes, mLastPrompt.mLength) == 0)
	{
		sendBuffer.mLength = GrapaMem::MoveLeft(sendBuffer.mBytes, mLastPrompt.mLength, sendBuffer.mLength);
		//memcpy(sendBuffer.mBytes, &sendBuffer.mBytes[mLastPrompt.mLength], sendBuffer.mLength - mLastPrompt.mLength);
		//sendBuffer.SetLength(sendBuffer.mLength - mLastPrompt.mLength);
	}
	if (sendBuffer.mLength == 0 && !isSelected)
	{
		buffer()->append((char*)"\n");
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
	}
	else if (sendBuffer.mLength)
	{
		vConsoleSend->mScriptState.ClearQueue();
		vConsoleSend->SetActive(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, false);
		if (isSelected) buffer()->append((char*)sendBuffer.mBytes);
		buffer()->append((char*)"\n");
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, sendBuffer.mBytes, sendBuffer.mLength);
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
	}
}

#include <FL/Fl_Menu_Bar.H>
#include <FL/filename.H>
#include <FL/Fl_Button.H>

static void MyMenuCallback(Fl_Widget* w, void* d) {
	Fl_Menu_Bar* bar = (Fl_Menu_Bar*)w;
	My_Text_Console* con = (My_Text_Console*)d;
	const Fl_Menu_Item* item = bar->mvalue();
	char ipath[256]; bar->item_pathname(ipath, sizeof(ipath));
	if (strcmp(item->label(), "&New") == 0)
	{
		My_Text_Console *t = new My_Text_Console();
	}
	else if (strcmp(item->label(), "&Quit") == 0)
	{
		for (My_Text_Console* gfx : gSystem->mConsoleList)
			gfx->hide();
	}
	else if (strcmp(item->label(), "Version") == 0)
	{
		con->ExecInFocus("$sys().getenv('VERSION');");
	}
	else if (strcmp(item->label(), "License") == 0)
	{
		con->ExecInFocus("$sys().getenv('LICENCE');");
	}
	else if (strcmp(item->label(), "Dir") == 0)
	{
		con->ExecInFocus("$file().ls().vector().str(['\\n','\\t']);");
	}
	else if (strcmp(item->label(), "Console") == 0)
	{
		if (con->prompt->visible())
		{
			con->prompt->hide();
			con->remove(con->prompt);
			int H = con->prompt->h();
			con->scr->resize(con->scr->x(), con->scr->y(), con->scr->w(), con->scr->h() + con->prompt->h());
		}
		else
		{
			if (con->h() < 180 + 140)
			{
				con->resize(con->x(), con->y(), con->w(), 180 + 140);
			}
			int H = con->prompt->h();
			con->scr->resize(con->scr->x(), con->scr->y(), con->scr->w(), con->scr->h() - con->prompt->h());
			con->prompt->resize(con->scr->x(), con->scr->y() + con->scr->h(), con->scr->w() + 2, con->prompt->h() + 2);
			con->add(con->prompt);
			con->prompt->show();
			con->scr->parent()->redraw();
		}
	}
}

void GrapaEditorResponse::SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
{
	//Fl::lock();      // acquire the lock
	disp->buffer()->append((char*)sendbuf);
	//disp->parent()->parent()->parent()->parent()->redraw();
	//Fl::unlock();    // release the lock; allow other threads to access FLTK again
}

void GrapaEditorResponse::SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{

}

void GrapaEditorResponse::SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (pValue)
	{
		if (pNameSpace->vLast->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
	//Fl::lock();      // acquire the lock
	disp->parent()->parent()->parent()->parent()->redraw();
	//Fl::unlock();    // release the lock; allow other threads to access FLTK again
}

void GrapaPromptResponse::SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
{
	disp->buffer()->append((char*)sendbuf);
	Fl_Text_Editor::kf_ctrl_move(FL_End, disp);
	//if (disp->vWin) disp->vWin->redraw();
}

void GrapaPromptResponse::SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
	disp->buffer()->append((char*)sendbuf.mBytes);
	disp->mLastPrompt.FROM(sendbuf);
	Fl_Text_Editor::kf_ctrl_move(FL_End, disp);
	//if (disp->vWin) disp->vWin->redraw();
}

void GrapaPromptResponse::SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (pValue)
	{
		if (pNameSpace->vLast->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
	disp->buffer()->append((char*)"\n");
	Fl_Text_Editor::kf_ctrl_move(FL_End, disp);
	//if (disp->vWin) disp->vWin->redraw();
}

My_Group::My_Group(int X, int Y, int W, int H, GrapaNames* pNameSpace)
	: My_Pack(X, Y, W, H, 0) {
	edit = NULL;
	disp = NULL;
	end();
	spacing(4);
	My_Pack* top = new My_Pack(X, Y, W - 15, H-30);
	top->end();
	add(top);
	edit = new My_Text_Editor(X, Y, W-15, H/2 -15);
	edit->end();
	top->add(edit);
	disp = new My_Text_Display(X, Y + H / 2 -15, W-15, H / 2 - 15);
	disp->end();
	top->add(disp);
	edit->vConsoleSend = &mConsoleSend;
	mConsoleResponse.disp = disp;
	mConsoleResponse.mRuleVariables.mSkip = true;
	mConsoleSend.mScriptState.SetNameSpace(&mConsoleResponse.mRuleVariables);
	GrapaNames* vNames = pNameSpace;
	while (vNames->GetParrent()) vNames = vNames->GetParrent();
	mConsoleResponse.mRuleVariables.SetParrent(vNames);
	mConsoleResponse.mRuleVariables.SetResponse(&mConsoleResponse);
	disp->color(FL_LIGHT1);
	My_Pack* enc = new My_Pack(X, Y+H-30, W - 15, 30);
	enc->spacing(4);
	enc->type(HORIZONTAL);
	enc->end();
	add(enc);
	Fl_Widget* blank = new Fl_Group(X, Y, 2, 30);
	enc->add(blank);
	My_Button* butAdd = new My_Button(X, Y, 30, 30, (char*)"+");
	enc->add(butAdd);
	butAdd->callback(My_Button::add, this);
	butAdd->when(FL_WHEN_RELEASE);
	My_Button* butDel = new My_Button(X, Y, 30, 30, (char*)"-");
	enc->add(butDel);
	butDel->callback(My_Button::del, this);
	butDel->when(FL_WHEN_RELEASE);
	resizable(top);
	mConsoleSend.Start();
}

My_Group::~My_Group()
{
	mConsoleSend.mScriptState.mSync = false;
	mConsoleSend.Stop();
	delete edit;
	delete disp;
	edit = NULL;
	disp = NULL;
}

My_ResizableScroll::My_ResizableScroll(int X, int Y, int W, int H)
	: Fl_Scroll(X, Y, W, H, 0) {
}
void My_ResizableScroll::resize(int X, int Y, int W, int H)
{
	Fl_Group::resize(X, Y, W, H);
}

My_Text_Console::My_Text_Console()
	: Fl_Double_Window(640, 180, "Grapa Editor")
{
	end();

	scr = NULL;
	prompt = NULL;

	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, 640, 30);		// Create menubar, items..
	menu->add("&File/&New", "^n", MyMenuCallback, this);
	menu->add("&File/&Open", "^o", MyMenuCallback, this);
	menu->add("&File/&Save", "^s", MyMenuCallback, this, FL_MENU_DIVIDER);
	menu->add("&File/&Quit", "^q", MyMenuCallback, this);
	menu->add("&Edit/&Copy", "^c", MyMenuCallback, this);
	menu->add("&Edit/&Paste", "^v", MyMenuCallback, this, FL_MENU_DIVIDER);
	menu->add("Script/Console", "", MyMenuCallback, this, FL_MENU_DIVIDER);
	menu->add("Script/Version", "", MyMenuCallback, this);
	menu->add("Script/License", "", MyMenuCallback, this);
	menu->add("Script/Dir", "", MyMenuCallback, this);
	add(menu);

	scr = new My_ResizableScroll(0, 30, 640, 180 - 30);
	scr->end();
	scr->type(Fl_Scroll::VERTICAL_ALWAYS);
	add(scr);

	pk = new My_Pack(0, 30, 640 - 18, 180 - 30);
	pk->end();
	pk->spacing(4);
	scr->add(pk);

	grp = new My_Group(0, 30, 640 - 18, 60, &mRuleVariables);
	pk->add(grp);

	prompt = new My_Prompt_Editor(0, 180, 640, 140, &mRuleVariables, this);
	prompt->hide();

	scr->resizable(*pk);
	resizable(*scr);

	gSystem->mConsoleList.push_back(this);

	Fl::visual(FL_DOUBLE | FL_INDEX);
	show();

	GrapaCHAR grresult;
	if (gSystem->mGrammar.mLength) grresult = grp->mConsoleSend.SendSync(gSystem->mGrammar);
	if (grresult.mLength)
	{
		grp->disp->buffer()->append((char*)grresult.mBytes);
	}
}

My_Text_Console::My_Text_Console(GrapaCHAR& in, GrapaCHAR& out, GrapaCHAR& run)
	: My_Text_Console()
{
	Fl::lock();
	GrapaCHAR grresult;
	if (run.mLength) grresult = grp->mConsoleSend.SendSync(run);
	if (grresult.mLength)
	{
		grp->disp->buffer()->append((char*)grresult.mBytes);
	}
}

My_Text_Console::~My_Text_Console()
{
	gSystem->mConsoleList.remove(this);
	scr = NULL;
	prompt = NULL;
}

void My_Text_Console::hide()
{
	delete this;
}

void My_Text_Console::Run(GrapaCB cb, void* data)
{
	if (cb)
	{
		while (visible())
		{
			Fl::wait(1);
			cb(data);
		}
	}
	else
	{
		Fl::run();
	}
}

void My_Text_Console::ExecInFocus(const char* in)
{
	Fl_Widget* v = (Fl_Widget*)Fl::focus();
	if (v)
	{
		if (v == (Fl_Widget*)prompt)
		{
			My_Prompt_Editor* t = (My_Prompt_Editor*)v;
			t->buffer()->append(in);
			t->sendtext();
		}
		else
		{
			My_Text_Editor* t = ((My_Group*)v->parent()->parent())->edit;
			t->buffer()->remove(0, t->buffer()->length());
			t->buffer()->append(in);
			t->sendtext();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////