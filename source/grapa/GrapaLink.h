#ifndef _GrapaLink_
#define _GrapaLink_

#include "GrapaValue.h"
#include "GrapaConsole.h"

#define grapa_version "0.1.57"

typedef void (*GrapaLinkCallback)(const char* pStr, bool pFlush);
class GrapaSystem;

class GrapaLink
{
public:
	static GrapaSystem* GetGrapaSystem();
	static void Start(bool& needExit, bool& showConsole, GrapaCHAR& outStr, GrapaCHAR& runStr);
	static void Stop();
	static bool IsStopped();
	static GrapaCHAR RunFile(GrapaConsoleSend& pConsoleSend, GrapaCHAR& fname);
	//static void SetStartGrammar(const GrapaCHAR& pGrammar);
	//static void EnablePrompt(bool pEnable = true);
	//static int GetChar();
	//static GrapaCHAR Send(GrapaCHAR& pIn);
	////static GrapaCHAR SendBootFile(const GrapaCHAR& pFileName);
	//static void SendAsync(GrapaCHAR& pIn, bool pRun = true);
	//static void Test();
};

#endif //_GrapaLink_
