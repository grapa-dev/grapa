// main.cpp
//
////////////////////////////////////////////////////////////////////////////////


#include "grapa/GrapaLink.h"
#include "grapa/GrapaSystem.h"
#include "grapa/GrapaLibRule.h"

#include <unordered_map>
#include <functional>

// @<main,test,{a:1}>()
class GrapaLibraryRuleMainTestEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleMainTestEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
    {
        GrapaRuleEvent* result = NULL;
        GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
        result = new GrapaRuleEvent(GrapaTokenType::GOBJ, 0, "", "");
        result->vQueue = new GrapaRuleQueue();
        if (r1.vVal)
        {
            GrapaRuleEvent* p = NULL;
            p = new GrapaRuleEvent(GrapaTokenType::GOBJ, 0, "params", "");
            p->vQueue = vScriptExec->CopyQueue(pInput);
            result->vQueue->PushTail(p);
        }
        return result;
    }
};

class GrapaMainRuleEvent : public GrapaLibraryRuleEvent
{
public:
    GrapaMainRuleEvent(GrapaCHAR pName) { mName.FROM(pName); };
    GrapaLibraryEvent* HandleTest(GrapaCHAR& pName) { return new GrapaLibraryRuleMainTestEvent(pName); }
    virtual GrapaLibraryEvent* LoadLib(GrapaScriptExec* vScriptExec, GrapaRuleEvent* pLib, GrapaCHAR& pName)
    {
        if (pName.mBytes == NULL) return NULL;
        using Handler = GrapaLibraryEvent * (GrapaMainRuleEvent::*)(GrapaCHAR& pName);
        static const std::unordered_map<std::string, Handler> handlerMap = {
                { "test", &GrapaMainRuleEvent::HandleTest },
        };
        auto it = handlerMap.find((char*)pName.mBytes);
        if (it != handlerMap.end())
            return (this->*(it->second))(pName);
        return NULL;
    }
};

int main(int argc, const char* argv[])
{
	GrapaSystem* gSystem = GrapaLink::GetGrapaSystem();

    if (argv)
    {
        for (int i = 0; i < argc; i++)
        {
            GrapaCHAR ss(argv[i]);
            ss.Trim(',');
            if (ss.mLength)
                gSystem->mCliArgv->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), ss));
        }
    }

    GrapaCHAR inStr,  outStr, runStr;
    bool needExit = false, showConsole = false;
    GrapaLink::Start(needExit, showConsole, outStr, runStr);
    gSystem->mLibraryQueue.PushTail(new GrapaMainRuleEvent(GrapaCHAR("main")));

    if (outStr.mLength && outStr.mBytes)
		fprintf(stdout, "%.*s\n", (int)outStr.mLength, (char*)outStr.mBytes);

    My_Console mConsole;
    mConsole.Start(runStr);
    if (!needExit || showConsole)
        mConsole.Run(NULL, NULL);
    mConsole.Stop();

    GrapaLink::Stop();
    return 0;
}
