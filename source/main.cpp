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
                gSystem->mArgv->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), ss));
            }
        }
    }

    GrapaCHAR inStr,  outStr, runStr;
    bool needExit=false,showConsole=false,showWidget=false;
    GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);
    gSystem->mLibraryQueue.PushTail(new GrapaMainRuleEvent(GrapaCHAR("main")));

    if (!showWidget && gGrapaUseWidget)
    {
        showWidget = true;
        inStr.FROM("editor.grc");
     }

    if (!showWidget)
    {
        My_Console mConsole;
        mConsole.Start(inStr, outStr, runStr);
        GrapaCHAR runStr("10;"),profile("");
        GrapaCHAR grresult = mConsole.mConsoleSend.SendSync(runStr, NULL, 0, profile);
        if (!needExit || showConsole)
            mConsole.Run(NULL, NULL);
        mConsole.Stop();
    }
    else
    {
        if (inStr.mLength && (GrapaMem::StrLwrCmp(".grz", 4, (char*)&inStr.mBytes[inStr.mLength - 4], 4) == 0 || GrapaMem::StrLwrCmp(".grc", 4, (char*)&inStr.mBytes[inStr.mLength - 4], 4) == 0))
        {
            bool scriptRan = false;

            Fl::lock();
            GrapaScriptExec mScriptExec;
            GrapaConsoleSend mConsoleSend;
            GrapaMainResponse mConsoleResponse;
            GrapaNames mRuleVariables;

            mConsoleSend.mScriptState.vScriptExec = &mScriptExec;
            mScriptExec.vScriptState = &mConsoleSend.mScriptState;
            mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
            mRuleVariables.SetResponse(&mConsoleResponse);
            mConsoleSend.Start();
            GrapaCHAR grresult;
            if (gSystem->mGrammar.mLength)
                grresult = mConsoleSend.SendSync(gSystem->mGrammar,NULL,0,GrapaCHAR());
            if (runStr.mLength)
                grresult = mConsoleSend.SendSync(runStr,NULL,0,GrapaCHAR());

            GrapaFileIO gf;
            GrapaError err = gf.Open((char*)inStr.mBytes);
            GrapaCHAR setValue;
            if (err == 0)
            {
                u64 fsize = 0;
                err = gf.GetSize(fsize);
                setValue.SetLength(fsize);
                err = gf.Read(0, 1, 0, fsize, setValue.mBytes);
                gf.Close();
                setValue.SetLength(fsize);
            }
            if (err)
            {
                s64 idx;
                GrapaRuleEvent* ev = gSystem->mStaticLib->Search("$editor.grz", idx);
                if (ev)
                    setValue.FROM(ev->mValue);
                if (setValue.mLength)
                    err = 0;
            }
            if (err == 0)
            {
                GrapaRuleEvent* plan = NULL;

                if (setValue.mLength > 4 && setValue.mBytes[0] == 'G' && setValue.mBytes[1] == 'R' && setValue.mBytes[2] == 'Z' && (setValue.mBytes[3] & 0x80) == 0)
                {
                    GrapaBYTE expanded;
                    GrapaCompress::Expand(setValue, expanded);
                    plan = new GrapaRuleEvent();
                    plan->vQueue = new GrapaRuleQueue();

                    if (expanded.mToken == GrapaTokenType::LIST || expanded.mToken == GrapaTokenType::ARRAY || expanded.mToken == GrapaTokenType::TUPLE)
                    {
                        GrapaRuleQueue* tq = new GrapaRuleQueue();
                        ((GrapaRuleQueue*)tq)->FROM(mScriptExec.vScriptState, &mRuleVariables, expanded);
                        GrapaRuleEvent* tx = NULL;
                        s64 idx = 0;
                        if (tq->mCount > 1 && expanded.mToken == GrapaTokenType::LIST)
                            tx = tq->Search("op", idx);
                        if (tx == NULL)
                            tx = tq->Head();
                        if (tx)
                        {
                            tq->PopEvent(tx);
                            plan->mValue.mToken = tx->mValue.mToken;
                            plan->vQueue = tx->vQueue;
                            tx->vQueue = NULL;
                            plan->vClass = tx->vClass;
                            if (plan->vClass == NULL && tx->mName.mLength)
                            {
                                plan->vClass = mScriptExec.vScriptState->GetClass(&mRuleVariables, tx->mName);
                            }
                            tx->CLEAR();
                            delete tx;
                            tx = NULL;
                        }
                        tq->CLEAR();
                        delete tq;
                        tq = NULL;
                    }
                    else
                    {
                        plan->vClass = ((GrapaRuleQueue*)plan->vQueue)->FROM(mScriptExec.vScriptState, &mRuleVariables, expanded);
                        plan->mValue.mToken = expanded.mToken;
                    }

                    plan->mValue.SetLength(0);
                    plan->mValue.SetSize(0);
                }
                else if (setValue.mLength)
                {
                    plan = mScriptExec.Plan(&mRuleVariables, setValue, NULL, 0, GrapaCHAR());
                }
                if (plan)
                {
                    GrapaRuleEvent* r = mScriptExec.ProcessPlan(&mRuleVariables, plan);
                    if (r)
                    {
                        r->CLEAR();
                        delete r;
                    }
                    plan->CLEAR();
                    delete plan;
                    plan = NULL;
                    scriptRan = true;
                }

                if (!scriptRan)
                    needExit = true;

            }
            if (!needExit || showConsole)
            {
                while (!gSystem->mStop)
                    Fl::wait(1);
            }

            Fl::unlock();
        }
        else
        {
            My_Text_Console* mGfxConsole = new My_Text_Console(inStr, outStr, runStr);
            if (!needExit || showConsole)
                mGfxConsole->Run(NULL, NULL);
        }
    }
    GrapaLink::Stop();
    return 0;
}
