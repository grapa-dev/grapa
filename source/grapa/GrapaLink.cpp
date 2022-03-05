// GrapaLink.cpp
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

#include "GrapaLink.h"
#include "GrapaSystem.h"
#include "GrapaCompress.h"
#include <iostream>
#include "GrapaStaticLib.c"

#include <openssl/ssl.h>
#include <openssl/err.h>

#if defined(__MINGW32__) || defined(__GNUC__)
#include <signal.h>
#endif

//GrapaLinkCallback gGrapaLinkCallback;

class GrapaSystem2 : public GrapaSystem
{
public:
    //virtual void Print(const char* pStr, bool flush = true) { mLock.WaitCritical();mOutput.Append(pStr);(*gGrapaLinkCallback)((const char*)mOutput.mBytes,flush);mOutput.SetLength(0); mLock.LeaveCritical();}
    //virtual void Print(const char pChar, bool flush = true) { mLock.WaitCritical();mOutput.Append(pChar);(*gGrapaLinkCallback)((const char*)mOutput.mBytes, flush);mOutput.SetLength(0); mLock.LeaveCritical();}
    //virtual void Print(const GrapaCHAR& pValue, bool flush = true) { mLock.WaitCritical();mOutput.Append(pValue);(*gGrapaLinkCallback)((const char*)mOutput.mBytes, flush);mOutput.SetLength(0); mLock.LeaveCritical();}
    //virtual void PrintLine(const GrapaCHAR& pValue, bool flush = true) { mLock.WaitCritical();mOutput.Append(pValue);mOutput.Append("\r\n");(*gGrapaLinkCallback)((const char*)mOutput.mBytes, flush);mOutput.SetLength(0); mLock.LeaveCritical();}
    //virtual void Print(const GrapaSU64& pValue, bool flush = true) { mLock.WaitCritical(); GrapaCHAR value(pValue.mNum);mOutput.Append(value);(*gGrapaLinkCallback)((const char*)mOutput.mBytes, flush);mOutput.SetLength(0); mLock.LeaveCritical();}
    //virtual void Print(u64 pValue, bool flush = true) { mLock.WaitCritical();mOutput.Append(pValue);(*gGrapaLinkCallback)((const char*)mOutput.mBytes, flush);mOutput.SetLength(0); mLock.LeaveCritical();}
private:
	GrapaCHAR mOutput;
	GrapaCritical mLock;
};

static GrapaSystem2 mSystem;
GrapaSystem* gSystem = &mSystem;

GrapaCHAR GrapaLink::Start(bool& needExit, bool& showConsole, bool& showWidget, GrapaCHAR& inStr, GrapaCHAR& outStr, GrapaCHAR& runStr)
{
	bool showVersion = true;
	bool showHelp = false;
	bool showEnv = false;

	inStr.SetLength(0);
	outStr.SetLength(0);

	GrapaRuleEvent* e = gSystem->mArgv->Head();
	while (e)
	{
		if ((e->mValue.Cmp("-h") == 0) || (e->mValue.Cmp("--help") == 0))
		{
			showHelp = true;
			needExit = true;
		}
		else if ((e->mValue.Cmp("-v") == 0) || (e->mValue.Cmp("--version") == 0))
		{
			showVersion = true;
			needExit = true;
		}
		else if ((e->mValue.Cmp("-q") == 0) || (e->mValue.Cmp("--quite") == 0))
		{
			showVersion = false;
		}
		else if ((e->mValue.Cmp("-e") == 0) || (e->mValue.Cmp("--env") == 0))
		{
			showEnv = true;
		}
		else if ((e->mValue.Cmp("-c") == 0) || (e->mValue.Cmp("--console") == 0))
		{
			showConsole = true;
		}
        else if ((e->mValue.Cmp("-w") == 0) || (e->mValue.Cmp("--widget") == 0))
        {
            inStr.FROM("widget.grz");
            showWidget = true;
        }
        else if (e->mValue.Cmp("-wfile") == 0)
        {
            e = e->Next();
            if (e)
                inStr.FROM(e->mValue);
            if (inStr.mLength==0)
                inStr.FROM("widget.grz");
            showWidget = true;
        }
		else if (e->mValue.Cmp("-ccmd") == 0)
		{
			e = e->Next();
			if (e)
				runStr.FROM(e->mValue);
			needExit = true;
		}
		else if (e->mValue.Cmp("-cfile") == 0)
		{
			e = e->Next();
			GrapaCHAR fn;
			if (e)
			{
				fn.FROM(e->mValue);
				GrapaFileIO fp;
				if (fp.Open((char*)fn.mBytes) == 0)
				{
					u64 sz = 0;
					fp.GetSize(sz);
					runStr.SetLength(sz);
					if (sz)
					{
						if (fp.Read(0, 0, 0, sz, runStr.mBytes))
							runStr.SetLength(0);
					}
				}
			}
			needExit = true;
		}
		else if (e->mValue.Cmp("-ccin") == 0)
		{
			needExit = true;
			char c;
			runStr.SetLength(0);
			while (std::cin >> c && !std::cin.eof())
				runStr.Append((char)c);
		}
		else if (e->mValue.Cmp("-argcin") == 0)
		{
			needExit = true;
			char c;
			gSystem->mArgcin.SetLength(0);
			while (std::cin >> c && !std::cin.eof())
				gSystem->mArgcin.Append((char)c);
		}
		else if (e->mValue.Cmp("-argv") == 0)
		{
			e = e->Next();
			while (gSystem->mArgv->Head() != e)
			{
				delete gSystem->mArgv->PopHead();
			}
			break;
		}
		else if (e->mValue.Cmp("-") == 0)
		{
			needExit = false;
		}
		else
		{
			if (inStr.mLength == 0 && e->mValue.mLength > 4)
			{
				inStr.FROM(e->mValue);
				//if ((GrapaMem::StrLwrCmp(".txt", 4, (char*)&e->mValue.mBytes[e->mValue.mLength - 4], 4) == 0) ||
				//	(GrapaMem::StrLwrCmp(".grc", 4, (char*)&e->mValue.mBytes[e->mValue.mLength - 4], 4) == 0)
				//	)
				//{
				//	GrapaFileIO gf;
				//	GrapaError err = gf.Open((char*)e->mValue.mBytes);
				//	if (err == 0)
				//	{
				//		u64 fsize = 0;
				//		err = gf.GetSize(fsize);
				//		inStr.SetLength(fsize);
				//		err = gf.Read(0, 1, 0, fsize, inStr.mBytes);
				//		gf.Close();
				//		inStr.SetLength(fsize);
				//	}
				//}
			}
		}
		e = e->Next();
	}

#if defined(__MINGW32__) || defined(__GNUC__)
    signal(SIGPIPE, SIG_IGN);
#endif

	GrapaCHAR grname("$grapa.grz");
	GrapaCHAR grresult("");

	//gGrapaLinkCallback = pCallback;
    time(&gSystem->mStartTime);
    gSystem->mStartClock = (clock() * 1000) / CLOCKS_PER_SEC;
	char* buffer = getcwd(NULL, 0);
	gSystem->mWorkDir.FROM(buffer);
	free(buffer);

#ifdef WIN32
	gSystem->mHomeDir.FROM(getenv("HOMEPATH"));
	gSystem->mTempDir.FROM(getenv("TEMP"));
#else
	gSystem->mHomeDir.FROM(getenv("HOME"));
	gSystem->mTempDir.FROM(getenv("TMPDIR"));
	if (gSystem->mTempDir.mLength == 0) gSystem->mTempDir.FROM("/tmp");
	struct stat sb;
	if (!(stat((char*)gSystem->mTempDir.mBytes, &sb) == 0 && S_ISDIR(sb.st_mode)))
		gSystem->mTempDir.SetLength(0);
#endif


#ifdef WIN32
	gSystem->mBinDir.FROM(_pgmptr);
	if (gSystem->mBinDir.mLength == 0)
	{
		GrapaCHAR ss;
		ss.SetLength(MAX_PATH);
		GetModuleFileName(NULL, (LPWSTR)(char*)ss.mBytes, MAX_PATH);
		gSystem->mBinDir.SetLength(MAX_PATH);
		std::wcstombs((char*)gSystem->mBinDir.mBytes, (wchar_t*)ss.mBytes, MAX_PATH);
		gSystem->mBinDir.SetLength(strlen((char*)gSystem->mBinDir.mBytes));
	}
	if (gSystem->mBinDir.mLength)
	{
		char* pathDel = strrchr((char*)gSystem->mBinDir.mBytes, '\\');
		u64 pathLen = pathDel ? (pathDel - (char*)gSystem->mBinDir.mBytes) : 0;
		gSystem->mBinName.FROM((char*)&gSystem->mBinDir.mBytes[pathLen ? (pathLen + 1) : 0], gSystem->mBinDir.mLength - (pathLen ? (pathLen + 1) : pathLen));
		gSystem->mBinDir.SetLength(pathLen);
		if (gSystem->mBinName.mLength > 4 && gSystem->mBinName.mBytes[gSystem->mBinName.mLength - 4] == '.')
			gSystem->mBinName.SetLength(gSystem->mBinName.mLength - 4);
	}
#else
	gSystem->mBinDir.FROM(gSystem->mHomeDir);
	char* pathDel = gSystem->mBinDir.mBytes?strrchr((char*)gSystem->mBinDir.mBytes, '/'):NULL;
	u64 pathLen = pathDel? (pathDel - (char*)gSystem->mBinDir.mBytes):0;
	gSystem->mBinName.FROM((char*)&gSystem->mBinDir.mBytes[pathLen ? (pathLen + 1): 0], gSystem->mBinDir.mLength - (pathLen?(pathLen + 1): pathLen));
	gSystem->mBinDir.SetLength(pathLen);
	if (gSystem->mBinDir.mLength&&gSystem->mBinDir.mBytes[0]=='.')
	{
		GrapaCHAR temp(gSystem->mWorkDir);
		temp.Append((char*)&gSystem->mBinDir.mBytes[1],gSystem->mBinDir.mLength-1);
		gSystem->mBinDir.FROM(temp);
	}
	else if (gSystem->mBinName.StrCmp(gSystem->mHomeDir)==0)
	{
		gSystem->mBinDir.FROM("/usr/bin");
	}
    gSystem->mLibDir.FROM(gSystem->mWorkDir); gSystem->mLibDir.Append("/lib/grapa");
    if (!(stat((char*)gSystem->mLibDir.mBytes,&sb)==0 && S_ISDIR(sb.st_mode)))
        gSystem->mLibDir.SetLength(0);
	if (gSystem->mLibDir.mLength==0)
    {
        gSystem->mLibDir.FROM("/usr/lib/grapa");
        if (!(stat((char*)gSystem->mLibDir.mBytes,&sb)==0 && S_ISDIR(sb.st_mode)))
            gSystem->mLibDir.SetLength(0);
    }
	if (gSystem->mLibDir.mLength==0)
    {
        gSystem->mLibDir.FROM("/usr/local/lib/grapa");
        if (!(stat((char*)gSystem->mLibDir.mBytes,&sb)==0 && S_ISDIR(sb.st_mode)))
            gSystem->mLibDir.SetLength(0);
    }
	if (gSystem->mLibDir.mLength==0)
    {
        gSystem->mLibDir.FROM(gSystem->mBinDir); gSystem->mLibDir.Append("/lib/grapa");
         if (!(stat((char*)gSystem->mLibDir.mBytes,&sb)==0 && S_ISDIR(sb.st_mode)))
            gSystem->mLibDir.SetLength(0);
    }
#endif
	GrapaLocalDatabase dir;
	gSystem->mLibDir.FROM(gSystem->mWorkDir); gSystem->mLibDir.Append("/lib/grapa");
	dir.mHomeDir = gSystem->mLibDir;
	dir.FieldGet(GrapaCHAR("$grapa.grz"), GrapaCHAR(), gSystem->mGrammar);
	//if (gSystem->mGrammar.mLength == 0)
	//	gSystem->mLibDir.SetLength(0);
#ifdef WIN32
#else
	if (gSystem->mLibDir.mLength == 0)
	{
		gSystem->mLibDir.Append("/usr/local/lib/grapa");
		dir.mHomeDir = gSystem->mLibDir;
		dir.FieldGet(GrapaCHAR("$grapa.grz"), GrapaCHAR(), gSystem->mGrammar);
		if (gSystem->mGrammar.mLength == 0)
			gSystem->mLibDir.SetLength(0);
	}
	if (gSystem->mLibDir.mLength == 0)
	{
		gSystem->mLibDir.Append("/usr/lib/grapa");
		dir.mHomeDir = gSystem->mLibDir;
		dir.FieldGet(GrapaCHAR("$grapa.grz"), GrapaCHAR(), gSystem->mGrammar);
		if (gSystem->mGrammar.mLength == 0)
			gSystem->mLibDir.SetLength(0);
	}
#endif
	GrapaCHAR stlib;
	u32 sti = 0;
	while (GrapaStaticLib::staticlist[sti] != NULL && GrapaStaticLib::staticlist[sti][0])
		stlib.Append(GrapaStaticLib::staticlist[sti++]);
	GrapaCHAR stb(stlib.FromBase(64));
	GrapaCHAR stl2;
	GrapaCompress::Expand(stb, stl2);
	if (gSystem->mStaticLib) gSystem->mStaticLib->CLEAR();
	if (!gSystem->mStaticLib) gSystem->mStaticLib = new GrapaRuleQueue();
	gSystem->mStaticLib->FROM(NULL, NULL, stl2);

	if (gSystem->mGrammar.mLength == 0)
	{
		GrapaCHAR binlib(gSystem->mBinDir);
		binlib.Append("/lib/grapa");
		dir.mHomeDir = binlib;
		dir.FieldGet(GrapaCHAR("$grapa.grz"), GrapaCHAR(), gSystem->mGrammar);
		if (gSystem->mGrammar.mLength)
		{
			gSystem->mLibDir.FROM(gSystem->mBinDir); gSystem->mLibDir.Append("/lib/grapa");
		}
	}

	if (gSystem->mGrammar.mLength == 0 && gSystem->mStaticLib && gSystem->mStaticLib->mCount)
	{
		s64 idx;
		GrapaRuleEvent* ev = gSystem->mStaticLib->Search("$grapa.grz",idx);
		if (ev)
		{
			gSystem->mGrammar.FROM(ev->mValue);
		}
	}

	if (gSystem->mPath) gSystem->mPath->CLEAR();
	if (!gSystem->mPath) gSystem->mPath = new GrapaRuleQueue();
	gSystem->mPath->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR()));
	//if (gSystem->mLibDir.mLength)
	//	gSystem->mPath->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mLibDir));

	GrapaCHAR str(__DATE__);
	GrapaCHAR dt, mo;
	dt.Append((char*)&str.mBytes[7], 4);
	dt.Append('-');
	mo.Append((char*)str.mBytes, 3);
	GrapaCHAR moStr;
	if (mo.Cmp("Jan") == 0) moStr.FROM("01");
	else if (mo.Cmp("Feb") == 0) moStr.FROM("02");
	else if (mo.Cmp("Mar") == 0) moStr.FROM("03");
	else if (mo.Cmp("Apr") == 0) moStr.FROM("04");
	else if (mo.Cmp("May") == 0) moStr.FROM("05");
	else if (mo.Cmp("Jun") == 0) moStr.FROM("06");
	else if (mo.Cmp("Jul") == 0) moStr.FROM("07");
	else if (mo.Cmp("Aug") == 0) moStr.FROM("08");
	else if (mo.Cmp("Sep") == 0) moStr.FROM("09");
	else if (mo.Cmp("Oct") == 0) moStr.FROM("10");
	else if (mo.Cmp("Nov") == 0) moStr.FROM("11");
	else if (mo.Cmp("Dec") == 0) moStr.FROM("12");
	else moStr.FROM("00");
	dt.Append(moStr);
	dt.Append('-');
	dt.Append((char*)&str.mBytes[4], 2);
	dt.Append("T");
	dt.Append(__TIME__);
	dt.Append(".000000");
	GrapaTIME t;
	t.FromString(dt);
	t.mSec -= t.LocalOffset();
	if (gSystem->mVersion) gSystem->mVersion->CLEAR();
	if (!gSystem->mVersion) gSystem->mVersion = new GrapaRuleQueue();
	gSystem->mVersion->PushTail(new GrapaRuleEvent(0, GrapaCHAR("major"), GrapaInt(grapa_major).getBytes()));
	gSystem->mVersion->PushTail(new GrapaRuleEvent(0, GrapaCHAR("minor"), GrapaInt(grapa_minor).getBytes()));
	gSystem->mVersion->PushTail(new GrapaRuleEvent(0, GrapaCHAR("micro"), GrapaInt(grapa_micro).getBytes()));
	gSystem->mVersion->PushTail(new GrapaRuleEvent(0, GrapaCHAR("releaselevel"), GrapaCHAR(grapa_releaselevel))); // alpha, beta, candidate, final
	gSystem->mVersion->PushTail(new GrapaRuleEvent(0, GrapaCHAR("serial"), GrapaInt(grapa_serial).getBytes()));
	gSystem->mVersion->PushTail(new GrapaRuleEvent(0, GrapaCHAR("date"), t.getBytes()));
	//gSystem->mConsoleConnect.mScriptState.mRuleVariables.SetResponse(&gSystem->mConsoleResponse);
	//GrapaError err = gSystem->mConsoleConnect.Start();
	//if (err) Stop();
	//if (gSystem->mGrammar.mLength) grresult = Send(gSystem->mGrammar);

	if (showVersion)
	{
		outStr.Append("Version: ");
		outStr.Append(GrapaInt(grapa_major).ToString());
		outStr.Append(".");
		outStr.Append(GrapaInt(grapa_minor).ToString());
		outStr.Append(".");
		outStr.Append(GrapaInt(grapa_micro).ToString());
		outStr.Append("\n");
	}
	if (showEnv)
	{
		outStr.Append("Environment:\n");
		outStr.Append("\t$NAME\t:");
		outStr.Append(gSystem->mBinName);
		outStr.Append("\n");
		outStr.Append("\t$BIN\t:");
		outStr.Append(gSystem->mBinDir);
		outStr.Append("\n");
		outStr.Append("\t$LIB\t:");
		outStr.Append(gSystem->mLibDir);
		outStr.Append("\n");
		outStr.Append("\t$WORK\t:");
		outStr.Append(gSystem->mWorkDir);
		outStr.Append("\n");
		outStr.Append("\t$HOME\t:");
		outStr.Append(gSystem->mHomeDir);
		outStr.Append("\n");
		outStr.Append("\t$TEMP\t:");
		outStr.Append(gSystem->mTempDir);
		outStr.Append("\n");
	}
	if (showHelp)
	{
		outStr.Append("Options:\n");
		outStr.Append("\t-h,--help\t:Show this help message\n");
		outStr.Append("\t-v,--version \t:Show version\n");
		outStr.Append("\t-q,--quite\t:Suppress header\n");
		outStr.Append("\t-c,--console\t:Show console\n");
		outStr.Append("\t-e,--env\t:Show environment details\n");
		outStr.Append("\t-ccmd script\t:Run from script\n");
		outStr.Append("\t-cfile file\t:Run from file\n");
		outStr.Append("\t-w\t\t:Editor\n");
		outStr.Append("\t-wfile file\t:Editor from file\n");
		outStr.Append("\t-ccin\t\t:Run from stdin\n");
		outStr.Append("\t-argcin\t\t:Places std:cin into $ARGCIN environment variable (use with -ccmd|-cfile)\n");
	}

	if (grresult.mLength)
	{
		outStr.Append("Load:\n");
		outStr.Append(grresult);
		outStr.Append("\n");
	}

	inStr.RTrim('\n');
	outStr.RTrim('\n');

	return grresult;
}

void GrapaLink::Stop()
{
	My_Text_Console* vGfx = NULL;
	if (!gSystem->mConsoleList.empty())
		vGfx = gSystem->mConsoleList.back();
	while (vGfx)
	{
		gSystem->mConsoleList.pop_back();
		delete vGfx;
		vGfx = NULL;
		if (!gSystem->mConsoleList.empty())
			vGfx = gSystem->mConsoleList.back();
	}
	if (gSystem->mLinkInitialized)
	{
		EVP_cleanup();
		gSystem->mLinkInitialized = false;
	}
	gSystem->mStop = true;
}

bool GrapaLink::IsStopped()
{
	return gSystem->mStop;
}

//void GrapaLink::SetStartGrammar(const GrapaCHAR& pGrammar)
//{
//	gSystem->mGrammar.FROM(pGrammar);
//}
//
//void GrapaLink::EnablePrompt(bool pEnable)
//{
//	gSystem->mConsoleConnect.mScriptState.EnablePrompt(pEnable);
//}
//
//int GrapaLink::GetChar()
//{
//	return gSystem->GetChar();
//}
//
//void GrapaLink::SendAsync(GrapaCHAR& pIn, bool pRun)
//{
//	GrapaError err;
//	gSystem->mConsoleConnect.mScriptState.ClearQueue();
//	if (pIn.mLength) gSystem->mConsoleConnect.Send(pIn.mBytes, pIn.mLength);
//	if (pRun) gSystem->mConsoleConnect.Send((u8*)"$\n", 2);
//}
//
//GrapaCHAR GrapaLink::Send(GrapaCHAR& pIn)
//{
//	GrapaCHAR s;
//    if (pIn.mLength)
//    {
//		GrapaScriptExec tokenExec;
//        tokenExec.vScriptState = &gSystem->mConsoleConnect.mScriptState;
//		tokenExec.vScriptState->WaitCritical();
//
//		GrapaScriptExec* saveTokenExec = tokenExec.vScriptState->vScriptExec;;
//		tokenExec.vScriptState->vScriptExec = &tokenExec;
//
//		GrapaRuleEvent* result = tokenExec.Exec(&tokenExec.vScriptState->mRuleVariables, 0, pIn);
//        
//        if (result)
//        {
//			GrapaSystemSend send;
//            send.isActive = false;
//			GrapaRuleEvent* echo = result;
//            while (echo->mValue.mToken == GrapaTokenType::PTR && echo->vRulePointer)
//                echo = result->vRulePointer;
//            //if (echo->mValue.mToken == GrapaTokenType::ERR)
//            //    send.Send("ERR: ");
//            if (echo->vQueue) tokenExec.EchoList(&send, echo,false, false, false);
//            else tokenExec.EchoValue(&send, echo, false, true, false);
//			send.GetStr(s);
//			result->CLEAR();
//			delete result;
//			result = NULL;
//        }
//        
//		tokenExec.vScriptState->vScriptExec = saveTokenExec;
//
//		tokenExec.vScriptState->LeaveCritical();
//     }
//    
//    return s;
//}

//
//GrapaCHAR GrapaLink::SendBootFile(const GrapaCHAR& pFileName)
//{
//	GrapaLocalDatabase dir;
//	GrapaCHAR s(""),s2("");
//	gSystem->mLibLock.WaitCritical();
//	gSystem->mBootDir.FROM(gSystem->mWorkDir);
//	dir.mHomeDir = gSystem->mBootDir;
//	dir.FieldGet(pFileName, GrapaCHAR(), s2);
//	if (s2.mLength == 0)
//	{
//		gSystem->mBootDir.FROM(gSystem->mLibDir);
//        dir.mHomeDir.FROM(gSystem->mBootDir);
//		dir.FieldGet(pFileName, GrapaCHAR(), s2);
//	}
//	if (s2.mLength == 0 && gSystem->mLibDir.StrCmp(gSystem->mBinDir)!=0)
//	{
//		gSystem->mBootDir.FROM(gSystem->mBinDir);
//		dir.mHomeDir.FROM(gSystem->mBootDir);
//		dir.FieldGet(pFileName, GrapaCHAR(), s2);
//	}
//	if (s2.mLength == 0)
//		gSystem->mBootDir.SetLength(0);
//	gSystem->mLibLock.LeaveCritical();
//	if (s2.mLength)
//		s = Send(s2);
//	return s;
//}

//void GrapaLink::Test()
//{
//
//	GrapaCHAR in,out;
//	in.FROM("$INCLUDE 'lib.grc'; n=$net(); @n.onlisten(':12345',@postMessageHandler,@postConnectHandler);");
//	out.FROM(Send(in));
//}
//
