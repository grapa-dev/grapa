// main.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include "grapa/GrapaLink.h"
#include "grapa/GrapaValue.h"
#include "grapa/GrapaSystem.h"
#include "grapa/GrapaCompress.h"
#include "grapa/GrapaLibRule.h"

#define gGrapaUseWidget false

extern bool gGrapaWidgetMainThread;

class GrapaMainResponse : public GrapaConsoleResponse
{
public:
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
    {
    };
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
    {
    };
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
    {
    };
};

class GrapaLibraryRuleMainTestEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleMainTestEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
    {
        GrapaRuleEvent* result = NULL;
        GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
        result = new GrapaRuleEvent(0, GrapaCHAR("xtest"), GrapaCHAR("xvalue"));
        return result;
    }
};

class GrapaMainRuleEvent : public GrapaLibraryRuleEvent
{
public:
    GrapaMainRuleEvent(GrapaCHAR pName) { mName.FROM(pName); };
    virtual GrapaLibraryEvent* LoadLib(GrapaScriptExec* vScriptExec, GrapaRuleEvent* pLib, GrapaCHAR& pName)
    {
        GrapaLibraryEvent* lib = NULL;
        if (lib == NULL)
        {
            if (pName.Cmp("test") == 0) lib = new GrapaLibraryRuleMainTestEvent(pName);
        }
        return(lib);
    }
};

int main(int argc, const char* argv[])
{
	GrapaSystem* gSystem = GrapaLink::GetGrapaSystem();

    if (argv)
    {
        GrapaCHAR ss;
        for (int i = 0; i < argc; i++)
        {
            ss.FROM(argv[i]);
            ss.Trim(',');
            if (ss.mLength)
            {
                gSystem->mCliArgv->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), ss));
            }
        }
    }

    GrapaCHAR inStr,  outStr, runStr;
    bool needExit=false,showConsole=false,showWidget=false;
    GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);
    gSystem->mLibraryQueue.PushTail(new GrapaMainRuleEvent(GrapaCHAR("main")));

    My_Console mConsole;
    mConsole.Start(inStr, outStr, runStr);
    if (!needExit || showConsole)
        mConsole.Run(NULL, NULL);
    mConsole.Stop();

    GrapaLink::Stop();
    return 0;
}
