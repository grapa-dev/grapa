// GrapaLink.cpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#include "GrapaLink.h"
#include "GrapaSystem.h"
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#include <limits.h>
#include "GrapaCompress.h"
#include "GrapaTime.h"
#include <iostream>
#include "GrapaStaticLib.c"

#include <openssl/ssl.h>
#include <openssl/err.h>

#if defined(__MINGW32__) || defined(__GNUC__)
#include <signal.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#include <vector> // [FIX] For std::vector

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

GrapaSystem* GrapaLink::GetGrapaSystem() {return gSystem;}

void GrapaLink::Start(bool& needExit, bool& showConsole, GrapaCHAR& outStr, GrapaCHAR& runStr)
{
	gSystem->RandSeed();

	bool showVersion = false;
	bool showHelp = false;
	bool suppressHeader = false;
	bool interactiveMode = false;

	outStr.SetLength(0);

	// Check if input is from pipe (for Unix-like systems)
	bool isPipeInput = false;
#ifndef WIN32
	if (!isatty(STDIN_FILENO)) {
		isPipeInput = true;
	}
#endif

	// Improved robust CLI argument parsing for Grapa
	// Supports all combinations listed in 'grapa -h', regardless of flag order.
	//
	// 1. Parse all flags and their arguments, regardless of order.
	// 2. For -c, use the next non-flag argument as the command, and allow flags before or after -c <command>.
	// 3. For -f, use the next non-flag argument as the script file, and allow flags before or after -f <file>.
	// 4. Only the first positional .grc/.grz is treated as a script if neither -c nor -f is used.
	// 5. All other positional arguments are passed as $ARGV.
	//
	// This logic ensures that:
	//   - grapa -c -q "8*3" and grapa -q -c "8*3" both work
	//   - grapa file.grc and grapa -f file.grc both work
	//   - All combinations of flags and script/command execution work as documented

	GrapaRuleEvent* e = gSystem->mCliArgv->Head();
	bool found_c = false, found_f = false;
	GrapaCHAR c_arg, f_arg;
	std::vector<GrapaRuleEvent*> positional_args;
	std::vector<GrapaRuleEvent*> argv_args;
	// First pass: collect flags and their arguments
	while (e) {
		if (found_c || found_f)
		{
			positional_args.push_back(e);
		}
		else
		{
			if ((e->mValue.Cmp("-h") == 0) || (e->mValue.Cmp("--help") == 0)) {
				showHelp = true;
				needExit = true;
			}
			else if ((e->mValue.Cmp("-v") == 0) || (e->mValue.Cmp("--version") == 0)) {
				showVersion = true;
				needExit = true;
			}
			else if ((e->mValue.Cmp("-q") == 0) || (e->mValue.Cmp("--quiet") == 0)) {
				suppressHeader = true;
			}
			else if ((e->mValue.Cmp("-d") == 0) || (e->mValue.Cmp("--debug") == 0)) {
				gSystem->mDebug.mDebugMode = true;
				gSystem->mDebug.DebugPrint("Debug mode enabled");
			}
			else if (e->mValue.Cmp("--verbose") == 0) {
				showVersion = true;
				gSystem->mDebug.DebugPrint("Verbose mode enabled (showing version header)");
				/* Removed non-standard options: -o, -a */
			}
			else if ((e->mValue.Cmp("-i") == 0) || (e->mValue.Cmp("--interactive") == 0)) {
				interactiveMode = true;
				showConsole = true;
			}
			else if ((e->mValue.Cmp("--no-prompt") == 0) || (e->mValue.Cmp("--quiet-interactive") == 0)) {
				gSystem->mSuppressPrompt = true;
			}
			else if (e->mValue.Cmp("-c") == 0) {
				found_c = true;
				// Find the next non-flag argument as the command
				GrapaRuleEvent* next = e->Next();
				while (next && next->mValue.mLength > 0 && next->mValue.mBytes[0] == '-') {
					// skip flags after -c
					next = next->Next();
				}
				if (next) c_arg.FROM(next->mValue);
				needExit = true;
			}
			else if (e->mValue.Cmp("-f") == 0) {
				found_f = true;
				// Find the next non-flag argument as the file
				GrapaRuleEvent* next = e->Next();
				while (next && next->mValue.mLength > 0 && next->mValue.mBytes[0] == '-') {
					// skip flags after -f
					next = next->Next();
				}
				if (next) f_arg.FROM(next->mValue);
				needExit = true;
				/* Removed non-standard options: -s, -S (replaced with standard - option) */
			}
			else if (e->mValue.Cmp("-") == 0) {
				/* Standard stdin option (like Python/Node.js) */
				gSystem->mDebug.DebugPrint("Executing from stdin (- option)");
				needExit = true;
				if (isPipeInput) {
					char c;
					runStr.SetLength(0);
					while (std::cin >> c && !std::cin.eof())
						runStr.Append((char)c);
				}
				else {
					fprintf(stderr, "Error: - option requires pipe input (e.g., echo 'command' | grapa -)\n");
					outStr.FROM("Error: - option requires pipe input\n");
					needExit = true;
					return;
				}
			}
			else if (e->mValue.mLength > 0 && e->mValue.mBytes[0] == '-') {
				/* Unknown flag - show error and exit */
				fprintf(stderr, "Error: Unknown option '%s'\n", (char*)e->mValue.mBytes);
				fprintf(stderr, "Use 'grapa -h' for help\n");
				outStr.FROM("Error: Unknown option\n");
				needExit = true;
				return;
			}
			else {
				positional_args.push_back(e);
			}
		}
		e = e ? e->Next() : nullptr;
	}
	// --- Argument parsing improvements for $ARGV and -s/-S ---
	int used_script_arg_index = -1;
	if (found_c && c_arg.mLength) {
		char debugMsg[256];
		int len = snprintf(debugMsg, sizeof(debugMsg), "Executing command: %s", c_arg.mBytes);
		debugMsg[len] = 0;
		gSystem->mDebug.DebugPrint(debugMsg);
		runStr.FROM(c_arg);
		needExit = true;
		// Find which positional arg was used for -c (if any)
		for (size_t i = 0; i < positional_args.size(); ++i) {
			if (positional_args[i]->mValue.Cmp(c_arg) == 0) {
				used_script_arg_index = (int)i;
				break;
			}
		}
	} else if (found_f && f_arg.mLength) {
		char debugMsg[256];
		int len = snprintf(debugMsg, sizeof(debugMsg), "Executing file: %s", f_arg.mBytes);
		debugMsg[len] = 0;
		gSystem->mDebug.DebugPrint(debugMsg);
		GrapaFileIO fp;
		if (fp.Open((char*)f_arg.mBytes) == 0) {
			u64 sz = 0;
			fp.GetSize(sz);
			runStr.SetLength(sz);
			if (sz) {
				if (fp.Read(0, 0, 0, sz, runStr.mBytes))
					runStr.SetLength(0);
			}
		}
		needExit = true;
		// Find which positional arg was used for -f (if any)
		for (size_t i = 0; i < positional_args.size(); ++i) {
			if (positional_args[i]->mValue.Cmp(f_arg) == 0) {
				used_script_arg_index = (int)i;
				break;
			}
		}
	} else {
		// Look for first positional .grc/.grz
		gSystem->mDebug.DebugPrint("Looking for positional script file");
		for (size_t i = 0; i < positional_args.size(); ++i) {
			auto* arg = positional_args[i];
			if (arg->mValue.mLength > 4 &&
				(strcmp((char*)&arg->mValue.mBytes[arg->mValue.mLength - 4], ".grc") == 0 ||
				 strcmp((char*)&arg->mValue.mBytes[arg->mValue.mLength - 4], ".grz") == 0)) {
				GrapaFileIO fp;
				if (fp.Open((char*)arg->mValue.mBytes) == 0) {
					char debugMsg[256];
					int len = snprintf(debugMsg, sizeof(debugMsg), "Found script file: %s", arg->mValue.mBytes);
					debugMsg[len] = 0;
					gSystem->mDebug.DebugPrint(debugMsg);
					u64 sz = 0;
					fp.GetSize(sz);
					runStr.SetLength(sz);
					if (sz) {
						if (fp.Read(0, 0, 0, sz, runStr.mBytes))
							runStr.SetLength(0);
					}
				}
				needExit = true;
				used_script_arg_index = (int)i;
				break;
			}
		}
	}
	// Now collect $ARGV: all positional_args not used as script/command
	for (size_t i = 0; i < positional_args.size(); ++i) {
		if ((int)i != used_script_arg_index) {
			gSystem->mArgv->PushTail(new GrapaRuleEvent(0,GrapaCHAR(),positional_args[i]->mValue));
		}
	}

	// Auto-detect pipe input if no explicit stdin option and no script file
	if (isPipeInput && runStr.mLength == 0 && !interactiveMode)
	{
		gSystem->mDebug.DebugPrint("Auto-detected pipe input");
		char c;
		runStr.SetLength(0);
		while (std::cin >> c && !std::cin.eof())
			runStr.Append((char)c);
		if (runStr.mLength > 0)
		{
			needExit = true;
		}
	}

#if defined(__MINGW32__) || defined(__GNUC__)
    signal(SIGPIPE, SIG_IGN);
#endif

	//GrapaCHAR grname("$grapa.grz");

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
#else
	// Detect actual executable path on Unix-like systems
	char exePath[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
	if (len == -1) {
		// Fallback for systems without /proc/self/exe (like macOS)
#ifdef __APPLE__
		uint32_t size = sizeof(exePath);
		if (_NSGetExecutablePath(exePath, &size) == 0) {
			len = strlen(exePath);
		} else {
			// Final fallback - use current working directory
			gSystem->mBinDir.FROM(gSystem->mWorkDir);
			len = 0;
		}
#else
		// For other Unix-like systems, use current working directory as fallback
		gSystem->mBinDir.FROM(gSystem->mWorkDir);
		len = 0;
#endif
	}
	
	if (len > 0) {
		exePath[len] = '\0';
		gSystem->mBinDir.FROM(exePath);
		char* pathDel = strrchr((char*)gSystem->mBinDir.mBytes, '/');
		u64 pathLen = pathDel ? (pathDel - (char*)gSystem->mBinDir.mBytes) : 0;
		gSystem->mBinName.FROM((char*)&gSystem->mBinDir.mBytes[pathLen ? (pathLen + 1) : 0], gSystem->mBinDir.mLength - (pathLen ? (pathLen + 1) : pathLen));
		gSystem->mBinDir.SetLength(pathLen);
	} else {
		// Fallback to current working directory
		gSystem->mBinDir.FROM(gSystem->mWorkDir);
		gSystem->mBinName.FROM("grapa");
	}

#endif
	// Set library directory to $WORK/lib (simplified approach)
	gSystem->mLibDir.FROM(gSystem->mWorkDir);
	gSystem->mLibDir.Append("/lib");

	/*
	// Load grammar from $WORK/lib/grapa
	GrapaLocalDatabase dir;
	GrapaCHAR grammarPath(gSystem->mWorkDir);
	grammarPath.Append("/lib/grapa");
	dir.mHomeDir = grammarPath;
	dir.FieldGet(GrapaCHAR("$grapa.grz"), GrapaCHAR(), gSystem->mGrammar);
	*/

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

	// If grammar not found in $WORK/lib/grapa, try $GRAPA_BIN/lib/grapa as fallback
	if (gSystem->mGrammar.mLength == 0)
	{
		GrapaLocalDatabase dir;
		GrapaCHAR binlib(gSystem->mBinDir);
		binlib.Append("/lib/grapa");
		dir.mHomeDir = binlib;
		dir.FieldGet(GrapaCHAR("$grapa.grz"), GrapaCHAR(), gSystem->mGrammar);
		if (gSystem->mGrammar.mLength)
		{
			gSystem->mLibDir.FROM(gSystem->mBinDir); 
			gSystem->mLibDir.Append("/lib");
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
	// Initialize $GRAPA_PATH with $WORK/lib (the main library directory)
	if (gSystem->mLibDir.mLength)
		gSystem->mPath->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mLibDir));

	if (showVersion && !suppressHeader)
	{
		outStr.Append("Version: ");
		outStr.Append(gSystem->mVersion);
		outStr.Append("\n");
	}

	if (showHelp)
	{
		outStr.Append("Options:\n");
		outStr.Append("\t-h,--help\t:Show this help message\n");
		outStr.Append("\t-v,--version \t:Show version\n");
		outStr.Append("\t-q,--quiet\t:Suppress header\n");
		outStr.Append("\t--verbose\t:Show version header (default: hidden)\n");
		outStr.Append("\t-i,--interactive\t:Run in interactive mode\n");
		outStr.Append("\t--no-prompt\t:Suppress interactive mode prompts\n");
		outStr.Append("\t-d,--debug\t:Debug mode (shows debug output)\n");
		outStr.Append("\n");
		outStr.Append("Execution:\n");
		outStr.Append("\t-c <script>\t:Execute command/script (e.g., grapa -c \"'hello'.echo()\")\n");
		outStr.Append("\t-f <file>\t:Execute file (e.g., grapa -f script.grc)\n");
		outStr.Append("\t-\t\t:Execute from stdin (e.g., echo \"'hello'.echo()\" | grapa -)\n");
		outStr.Append("\n");
		outStr.Append("Direct Execution (no flags needed):\n");
		outStr.Append("\tscript.grc\t:Execute script file directly (e.g., grapa script.grc)\n");
		outStr.Append("\tscript.grz\t:Execute compiled script directly (e.g., grapa script.grz)\n");
	}

	outStr.RTrim('\n');
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

GrapaCHAR GrapaLink::RunFile(GrapaConsoleSend& pConsoleSend, GrapaCHAR& fname)
{
	GrapaCHAR inStr;
	inStr.FROM(fname);
	inStr.Append(".grz");
	GrapaFileIO gf;
	GrapaError err = gf.Open((char*)inStr.mBytes);
	if (err)
	{
		inStr.FROM(fname);
		inStr.Append(".grc");
		err = gf.Open((char*)inStr.mBytes);
	}
	GrapaCHAR setValue;
	GrapaCHAR grresult;
	if (err == 0)
	{
		u64 fsize = 0;
		err = gf.GetSize(fsize);
		setValue.SetLength(fsize);
		err = gf.Read(0, 1, 0, fsize, setValue.mBytes);
		gf.Close();
		setValue.SetLength(fsize);
	}

	if (err == 0 && setValue.mLength)
		grresult = pConsoleSend.SendSync(setValue, NULL, 0, GrapaCHAR());

	return grresult;
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
