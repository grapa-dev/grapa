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
#include "GrapaTime.h"

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
//	GrapaTime t;
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
	GrapaTime t;
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


///////////////////////////////////////////////////////////////////////////////////////////////////
