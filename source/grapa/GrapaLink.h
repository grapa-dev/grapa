#ifndef _GrapaLink_
#define _GrapaLink_

#include "GrapaValue.h"

#define grapa_version "0.0.279"

typedef void (*GrapaLinkCallback)(const char* pStr, bool pFlush);
class GrapaSystem;

class GrapaLink
{
public:
	static GrapaSystem* GetGrapaSystem();
    static GrapaCHAR Start(bool& needExit, bool& showConsole, bool& showWidget, GrapaCHAR& inStr, GrapaCHAR& outStr, GrapaCHAR& runStr);
	static void Stop();
	static bool IsStopped();
	//static void SetStartGrammar(const GrapaCHAR& pGrammar);
	//static void EnablePrompt(bool pEnable = true);
	//static int GetChar();
	//static GrapaCHAR Send(GrapaCHAR& pIn);
	////static GrapaCHAR SendBootFile(const GrapaCHAR& pFileName);
	//static void SendAsync(GrapaCHAR& pIn, bool pRun = true);
	//static void Test();
};

#endif //_GrapaLink_
