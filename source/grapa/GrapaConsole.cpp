// GrapaConsole.cpp
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

#include "GrapaConsole.h"
#include "GrapaSystem.h"

extern GrapaSystem* gSystem;

///////////////////////////////////////////////////////////////////////////////////////////////////

//void GrapaConsoleResponse::SendCommand(const void* sendbuf, u64 sendbuflen)
//{
//	if (sendbuf&&sendbuflen) gSystem->Print(GrapaCHAR((char*)sendbuf, sendbuflen));
//}

void GrapaConsoleResponse::UpdatePrompt()
{
	mPrompt.FROM("");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaConsoleSend::GrapaConsoleSend()
{
	mScriptState.vConsoleSend = this;
	mScriptState.SetQueue(&mScriptQueue);
}

GrapaConsoleSend::~GrapaConsoleSend()
{
	Stop();
	mScriptState.SetQueue(NULL);
}

GrapaError GrapaConsoleSend::Start()
{
	mScriptState.GetNameSpace()->GetResponse()->UpdatePrompt();
	mScriptState.Start();
	return 0;
}

void GrapaConsoleSend::Stop()
{
	mScriptState.Stop();
	mScriptState.CLEAR();
	mScriptQueue.CLEAR();
}

void GrapaConsoleSend::Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen)
{
	GrapaItemEvent* event = new GrapaItemEvent();
	event->mMessage.FROM(sendbuflen, sendbuf);
	mScriptState.PushInputTail(event);
}

GrapaCHAR GrapaConsoleSend::SendSync(GrapaCHAR& pIn, GrapaRuleEvent* pRule, u64 pRuleId)
{
	GrapaCHAR s;
    if (pIn.mLength)
    {
		GrapaScriptExec tokenExec;
        tokenExec.vScriptState = &mScriptState;
		tokenExec.vScriptState->WaitCritical();

		GrapaScriptExec* saveTokenExec = tokenExec.vScriptState->vScriptExec;;
		tokenExec.vScriptState->vScriptExec = &tokenExec;

		GrapaRuleEvent* result = tokenExec.Exec(tokenExec.vScriptState->GetNameSpace(), pRule, pRuleId, pIn);
        
        if (result)
        {
			GrapaSystemSend send;
            send.isActive = false;
			GrapaRuleEvent* echo = result;
            while (echo->mValue.mToken == GrapaTokenType::PTR && echo->vRulePointer)
                echo = result->vRulePointer;
            //if (echo->mValue.mToken == GrapaTokenType::ERR)
            //    send.Send("ERR: ");
            if (echo->vQueue) tokenExec.EchoList(&send, echo,false, false, false);
            else tokenExec.EchoValue(&send, echo, false, false, false);
			send.GetStr(s);
			result->CLEAR();
			delete result;
			result = NULL;
        }
        
		tokenExec.vScriptState->vScriptExec = saveTokenExec;

		tokenExec.vScriptState->LeaveCritical();
     }
    
    return s;
}

GrapaCHAR GrapaConsoleSend::SendSyncRaw(GrapaCHAR& pIn, GrapaRuleEvent* pRule, u64 pRuleId)
{
	GrapaCHAR s;
	if (pIn.mLength)
	{
		GrapaScriptExec tokenExec;
		tokenExec.vScriptState = &mScriptState;
		tokenExec.vScriptState->WaitCritical();

		GrapaScriptExec* saveTokenExec = tokenExec.vScriptState->vScriptExec;;
		tokenExec.vScriptState->vScriptExec = &tokenExec;

		GrapaRuleEvent* result = tokenExec.Exec(tokenExec.vScriptState->GetNameSpace(), pRule, pRuleId, pIn);

		if (result)
		{
			GrapaSystemSend send;
			send.isActive = false;
			GrapaRuleEvent* echo = result;
			while (echo->mValue.mToken == GrapaTokenType::PTR && echo->vRulePointer)
				echo = result->vRulePointer;
			s = echo->mValue;
			result->CLEAR();
			delete result;
			result = NULL;
		}

		tokenExec.vScriptState->vScriptExec = saveTokenExec;

		tokenExec.vScriptState->LeaveCritical();
	}

	return s;
}
