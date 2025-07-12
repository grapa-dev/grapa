// GrapaLibRule.cpp
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

#include "GrapaLibRule.h"
#include "GrapaSystem.h"
#include "GrapaState.h"
#include "GrapaCompress.h"
#include "GrapaTinyAES.h"
#include "GrapaHash.h"
#include "GrapaFloat.h"
#include "GrapaVector.h"
#include "GrapaWidget.h"
#include "GrapaLink.h"
#include "GrapaNetConnect.h"
#include "GrapaTime.h"

#include <thread>

extern GrapaSystem* gSystem;

class GrapaRuleQueue;

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleResetEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleResetEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleReset(GrapaCHAR& pName) { return new GrapaLibraryRuleResetEvent(pName); }

class GrapaLibraryRuleExitEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleExitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleExit(GrapaCHAR& pName) { return new GrapaLibraryRuleExitEvent(pName); }

class GrapaLibraryRuleGetEnvEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGetEnvEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGetEnv(GrapaCHAR& pName) { return new GrapaLibraryRuleGetEnvEvent(pName); }

class GrapaLibraryRulePutEnvEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePutEnvEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePutEnv(GrapaCHAR& pName) { return new GrapaLibraryRulePutEnvEvent(pName); }

class GrapaLibraryRuleShellEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleShellEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleShell(GrapaCHAR& pName) { return new GrapaLibraryRuleShellEvent(pName); }

class GrapaLibraryRuleEvalEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleEvalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleEval(GrapaCHAR& pName) { return new GrapaLibraryRuleEvalEvent(pName); }

class GrapaLibraryRuleExecEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleExecEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleExec(GrapaCHAR& pName) { return new GrapaLibraryRuleExecEvent(pName); }

class GrapaLibraryRulePostEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePostEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePost(GrapaCHAR& pName) { return new GrapaLibraryRulePostEvent(pName); }

class GrapaLibraryRuleSleepEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSleepEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSleep(GrapaCHAR& pName) { return new GrapaLibraryRuleSleepEvent(pName); }

class GrapaLibraryRuleTokenEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTokenEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleToken(GrapaCHAR& pName) { return new GrapaLibraryRuleTokenEvent(pName); }

class GrapaLibraryRuleRuleValEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRuleValEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRuleVal(GrapaCHAR& pName) { return new GrapaLibraryRuleRuleValEvent(pName); }

class GrapaLibraryRuleRuleRefEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRuleRefEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRuleRef(GrapaCHAR& pName) { return new GrapaLibraryRuleRuleRefEvent(pName); }

class GrapaLibraryRuleRuleEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRuleEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRule(GrapaCHAR& pName) { return new GrapaLibraryRuleRuleEvent(pName); }

class GrapaLibraryRuleGrammarEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGrammarEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGrammar(GrapaCHAR& pName) { return new GrapaLibraryRuleGrammarEvent(pName); }

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleNameEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNameEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleName(GrapaCHAR& pName) { return new GrapaLibraryRuleNameEvent(pName); }

class GrapaLibraryRuleGetNameEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGetNameEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGetName(GrapaCHAR& pName) { return new GrapaLibraryRuleGetNameEvent(pName); }

class GrapaLibraryRuleLitEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLit(GrapaCHAR& pName) { return new GrapaLibraryRuleLitEvent(pName); }

class GrapaLibraryRuleRefEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRefEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRef(GrapaCHAR& pName) { return new GrapaLibraryRuleRefEvent(pName); }

class GrapaLibraryRuleNullEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNullEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleNull(GrapaCHAR& pName) { return new GrapaLibraryRuleNullEvent(pName); }

class GrapaLibraryRuleVarEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleVarEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleVar(GrapaCHAR& pName) { return new GrapaLibraryRuleVarEvent(pName); }

class GrapaLibraryRuleAssignEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleAssignEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleAssign(GrapaCHAR& pName) { return new GrapaLibraryRuleAssignEvent(pName); }

class GrapaLibraryRuleAssignAppendEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleAssignAppendEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleAssignAppend(GrapaCHAR& pName) { return new GrapaLibraryRuleAssignAppendEvent(pName); }

class GrapaLibraryRuleAssignExtendEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleAssignExtendEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleAssignExtend(GrapaCHAR& pName) { return new GrapaLibraryRuleAssignExtendEvent(pName); }

class GrapaLibraryRuleCreateArrayEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateArrayEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateArray(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateArrayEvent(pName); }

class GrapaLibraryRuleCreateTupleEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateTupleEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateTuple(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateTupleEvent(pName); }

class GrapaLibraryRuleCreateListEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateListEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateList(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateListEvent(pName); }

class GrapaLibraryRuleCreateXmlEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateXmlEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateXml(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateXmlEvent(pName); }

class GrapaLibraryRuleCreateElEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateElEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateEl(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateElEvent(pName); }

class GrapaLibraryRuleCreateTagEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateTagEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateTag(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateTagEvent(pName); }

class GrapaLibraryRuleCreateOpEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateOpEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateOp(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateOpEvent(pName); }

class GrapaLibraryRuleCreateCodeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateCodeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateCode(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateCodeEvent(pName); }

class GrapaLibraryRuleCreateRuleEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCreateRuleEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCreateRule(GrapaCHAR& pName) { return new GrapaLibraryRuleCreateRuleEvent(pName); }

class GrapaLibraryRuleExtendEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleExtendEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleExtend(GrapaCHAR& pName) { return new GrapaLibraryRuleExtendEvent(pName); }

class GrapaLibraryRuleRemoveEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRemoveEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRemove(GrapaCHAR& pName) { return new GrapaLibraryRuleRemoveEvent(pName); }

class GrapaLibraryRulePrependEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePrependEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePrepend(GrapaCHAR& pName) { return new GrapaLibraryRulePrependEvent(pName); }

class GrapaLibraryRulePopEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePopEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePop(GrapaCHAR& pName) { return new GrapaLibraryRulePopEvent(pName); }

class GrapaLibraryRuleCallEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCallEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCall(GrapaCHAR& pName) { return new GrapaLibraryRuleCallEvent(pName); }

class GrapaLibraryRuleSearchEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSearchEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSearch(GrapaCHAR& pName) { return new GrapaLibraryRuleSearchEvent(pName); }

class GrapaLibraryRuleFindAllEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleFindAllEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleFindAll(GrapaCHAR& pName) { return new GrapaLibraryRuleFindAllEvent(pName); }

class GrapaLibraryRuleSortEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSortEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSort(GrapaCHAR& pName) { return new GrapaLibraryRuleSortEvent(pName); }

class GrapaLibraryRuleArgSortEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleArgSortEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleArgSort(GrapaCHAR& pName) { return new GrapaLibraryRuleArgSortEvent(pName); }

class GrapaLibraryRuleUniqueEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleUniqueEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleUnique(GrapaCHAR& pName) { return new GrapaLibraryRuleUniqueEvent(pName); }

class GrapaLibraryRuleGroupEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGroupEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGroup(GrapaCHAR& pName) { return new GrapaLibraryRuleGroupEvent(pName); }

class GrapaLibraryRuleClassEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleClassEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleClass(GrapaCHAR& pName) { return new GrapaLibraryRuleClassEvent(pName); }

class GrapaLibraryRuleObjEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleObjEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleObj(GrapaCHAR& pName) { return new GrapaLibraryRuleObjEvent(pName); }

class GrapaLibraryRuleGlobalEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGlobalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGlobal(GrapaCHAR& pName) { return new GrapaLibraryRuleGlobalEvent(pName); }

class GrapaLibraryRuleParentEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleParentEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleParent(GrapaCHAR& pName) { return new GrapaLibraryRuleParentEvent(pName); }

class GrapaLibraryRuleThisEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleThisEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleThis(GrapaCHAR& pName) { return new GrapaLibraryRuleThisEvent(pName); }

class GrapaLibraryRuleLocalEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLocalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLocal(GrapaCHAR& pName) { return new GrapaLibraryRuleLocalEvent(pName); }

class GrapaLibraryRuleStaticEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleStaticEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleStatic(GrapaCHAR& pName) { return new GrapaLibraryRuleStaticEvent(pName); }

class GrapaLibraryRuleConstEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleConstEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleConst(GrapaCHAR& pName) { return new GrapaLibraryRuleConstEvent(pName); }

class GrapaLibraryRuleSetConstEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSetConstEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSetConst(GrapaCHAR& pName) { return new GrapaLibraryRuleSetConstEvent(pName); }

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleReturnEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleReturnEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleReturn(GrapaCHAR& pName) { return new GrapaLibraryRuleReturnEvent(pName); }

class GrapaLibraryRuleBreakEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBreakEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBreak(GrapaCHAR& pName) { return new GrapaLibraryRuleBreakEvent(pName); }

class GrapaLibraryRuleIfEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIfEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleIf(GrapaCHAR& pName) { return new GrapaLibraryRuleIfEvent(pName); }

class GrapaLibraryRuleWhileEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWhileEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWhile(GrapaCHAR& pName) { return new GrapaLibraryRuleWhileEvent(pName); }

class GrapaLibraryRuleScopeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleScopeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleScope(GrapaCHAR& pName) { return new GrapaLibraryRuleScopeEvent(pName); }

class GrapaLibraryRuleSwitchEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSwitchEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSwitch(GrapaCHAR& pName) { return new GrapaLibraryRuleSwitchEvent(pName); }

class GrapaLibraryRuleCaseEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCaseEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCase(GrapaCHAR& pName) { return new GrapaLibraryRuleCaseEvent(pName); }

class GrapaLibraryRulePlanEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePlanEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePlan(GrapaCHAR& pName) { return new GrapaLibraryRulePlanEvent(pName); }

class GrapaLibraryRuleWrapEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWrapEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWrap(GrapaCHAR& pName) { return new GrapaLibraryRuleWrapEvent(pName); }

class GrapaLibraryRuleOpEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleOpEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleOp(GrapaCHAR& pName) { return new GrapaLibraryRuleOpEvent(pName); }

class GrapaLibraryRuleMapEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMapEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMap(GrapaCHAR& pName) { return new GrapaLibraryRuleMapEvent(pName); }

class GrapaLibraryRuleReduceEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleReduceEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleReduce(GrapaCHAR& pName) { return new GrapaLibraryRuleReduceEvent(pName); }

class GrapaLibraryRuleFilterEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleFilterEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleFilter(GrapaCHAR& pName) { return new GrapaLibraryRuleFilterEvent(pName); }

class GrapaLibraryRuleRangeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRangeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRange(GrapaCHAR& pName) { return new GrapaLibraryRuleRangeEvent(pName); }

class GrapaLibraryRuleIsIntEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIsIntEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleIsInt(GrapaCHAR& pName) { return new GrapaLibraryRuleIsIntEvent(pName); }

class GrapaLibraryRuleIfErrEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIfErrEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleIfErr(GrapaCHAR& pName) { return new GrapaLibraryRuleIfErrEvent(pName); }


///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleTableEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTableEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTable(GrapaCHAR& pName) { return new GrapaLibraryRuleTableEvent(pName); }

class GrapaLibraryRulePwdEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePwdEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePwd(GrapaCHAR& pName) { return new GrapaLibraryRulePwdEvent(pName); }

class GrapaLibraryRuleCdEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCdEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCd(GrapaCHAR& pName) { return new GrapaLibraryRuleCdEvent(pName); }

class GrapaLibraryRulePhdEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePhdEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePhd(GrapaCHAR& pName) { return new GrapaLibraryRulePhdEvent(pName); }

class GrapaLibraryRuleChdEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleChdEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleChd(GrapaCHAR& pName) { return new GrapaLibraryRuleChdEvent(pName); }

class GrapaLibraryRuleLsEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLsEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLs(GrapaCHAR& pName) { return new GrapaLibraryRuleLsEvent(pName); }

class GrapaLibraryRuleMkEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMkEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMk(GrapaCHAR& pName) { return new GrapaLibraryRuleMkEvent(pName); }

class GrapaLibraryRuleRmEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRmEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRm(GrapaCHAR& pName) { return new GrapaLibraryRuleRmEvent(pName); }

class GrapaLibraryRuleMkFieldEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMkFieldEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMkField(GrapaCHAR& pName) { return new GrapaLibraryRuleMkFieldEvent(pName); }

class GrapaLibraryRuleRmFieldEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRmFieldEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRmField(GrapaCHAR& pName) { return new GrapaLibraryRuleRmFieldEvent(pName); }

//class GrapaLibraryRuleCpEvent : public GrapaLibraryEvent
//{
//public:
//	GrapaLibraryRuleCpEvent(GrapaCHAR& pName) { mName.FROM(pName); };
//	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
//};

class GrapaLibraryRuleInfoEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleInfoEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleInfo(GrapaCHAR& pName) { return new GrapaLibraryRuleInfoEvent(pName); }

class GrapaLibraryRuleSetEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSetEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSet(GrapaCHAR& pName) { return new GrapaLibraryRuleSetEvent(pName); }

class GrapaLibraryRuleGetEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGetEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGet(GrapaCHAR& pName) { return new GrapaLibraryRuleGetEvent(pName); }

class GrapaLibraryRuleFileSplitEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleFileSplitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleFileSplit(GrapaCHAR& pName) { return new GrapaLibraryRuleFileSplitEvent(pName); }


//class GrapaLibraryRuleSetOpEvent : public GrapaLibraryEvent
//{
//public:
//	GrapaLibraryRuleSetOpEvent(GrapaCHAR& pName) { mName.FROM(pName); };
//	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
//};
//
//class GrapaLibraryRuleGetOpEvent : public GrapaLibraryEvent
//{
//public:
//	GrapaLibraryRuleGetOpEvent(GrapaCHAR& pName) { mName.FROM(pName); };
//	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
//};
//
//class GrapaLibraryRuleRunOpEvent : public GrapaLibraryEvent
//{
//public:
//	GrapaLibraryRuleRunOpEvent(GrapaCHAR& pName) { mName.FROM(pName); };
//	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
//};

class GrapaLibraryRuleIncludeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIncludeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleInclude(GrapaCHAR& pName) { return new GrapaLibraryRuleIncludeEvent(pName); }

class GrapaLibraryRuleDebugEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDebugEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDebug(GrapaCHAR& pName) { return new GrapaLibraryRuleDebugEvent(pName); }

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleMacEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMacEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMac(GrapaCHAR& pName) { return new GrapaLibraryRuleMacEvent(pName); }

class GrapaLibraryRuleInterfacesEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleInterfacesEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleInterfaces(GrapaCHAR& pName) { return new GrapaLibraryRuleInterfacesEvent(pName); }

class GrapaLibraryRuleConnectEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleConnectEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleConnect(GrapaCHAR& pName) { return new GrapaLibraryRuleConnectEvent(pName); }

class GrapaLibraryRuleBindEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBindEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBind(GrapaCHAR& pName) { return new GrapaLibraryRuleBindEvent(pName); }

class GrapaLibraryRuleListenEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleListenEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleListen(GrapaCHAR& pName) { return new GrapaLibraryRuleListenEvent(pName); }

class GrapaLibraryRuleOnListenEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleOnListenEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleOnListen(GrapaCHAR& pName) { return new GrapaLibraryRuleOnListenEvent(pName); }

class GrapaLibraryRuleDisconnectEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDisconnectEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDisconnect(GrapaCHAR& pName) { return new GrapaLibraryRuleDisconnectEvent(pName); }

class GrapaLibraryRuleProxyEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleProxyEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleProxy(GrapaCHAR& pName) { return new GrapaLibraryRuleProxyEvent(pName); }

class GrapaLibraryRuleCertificateEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCertificateEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCertificate(GrapaCHAR& pName) { return new GrapaLibraryRuleCertificateEvent(pName); }

class GrapaLibraryRulePrivateEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePrivateEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePrivate(GrapaCHAR& pName) { return new GrapaLibraryRulePrivateEvent(pName); }

class GrapaLibraryRuleTrustedEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTrustedEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTrusted(GrapaCHAR& pName) { return new GrapaLibraryRuleTrustedEvent(pName); }

class GrapaLibraryRuleNetVerifyEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNetVerifyEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleNetVerify(GrapaCHAR& pName) { return new GrapaLibraryRuleNetVerifyEvent(pName); }

class GrapaLibraryRuleNetChainEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNetChainEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleNetChain(GrapaCHAR& pName) { return new GrapaLibraryRuleNetChainEvent(pName); }

class GrapaLibraryRuleNetHostEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNetHostEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleNetHost(GrapaCHAR& pName) { return new GrapaLibraryRuleNetHostEvent(pName); }

class GrapaLibraryRuleSendEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSendEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSend(GrapaCHAR& pName) { return new GrapaLibraryRuleSendEvent(pName); }

class GrapaLibraryRuleReceiveEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleReceiveEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleReceive(GrapaCHAR& pName) { return new GrapaLibraryRuleReceiveEvent(pName); }

class GrapaLibraryRulePendingEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePendingEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePending(GrapaCHAR& pName) { return new GrapaLibraryRulePendingEvent(pName); }

class GrapaLibraryRuleOnReceiveEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleOnReceiveEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleOnReceive(GrapaCHAR& pName) { return new GrapaLibraryRuleOnReceiveEvent(pName); }

class GrapaLibraryRuleHttpReadEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleHttpReadEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleHttpRead(GrapaCHAR& pName) { return new GrapaLibraryRuleHttpReadEvent(pName); }

class GrapaLibraryRuleHttpSendEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleHttpSendEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleHttpSend(GrapaCHAR& pName) { return new GrapaLibraryRuleHttpSendEvent(pName); }

class GrapaLibraryRuleHttpMessageEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleHttpMessageEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleHttpMessage(GrapaCHAR& pName) { return new GrapaLibraryRuleHttpMessageEvent(pName); }

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleGenPrimeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGenPrimeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGenPrime(GrapaCHAR& pName) { return new GrapaLibraryRuleGenPrimeEvent(pName); }

class GrapaLibraryRuleStaticPrimeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleStaticPrimeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleStaticPrime(GrapaCHAR& pName) { return new GrapaLibraryRuleStaticPrimeEvent(pName); }

class GrapaLibraryRuleIsPrimeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIsPrimeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleIsPrime(GrapaCHAR& pName) { return new GrapaLibraryRuleIsPrimeEvent(pName); }

class GrapaLibraryRuleIsAksEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIsAksEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleIsAks(GrapaCHAR& pName) { return new GrapaLibraryRuleIsAksEvent(pName); }

class GrapaLibraryRuleRandomEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRandomEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRandom(GrapaCHAR& pName) { return new GrapaLibraryRuleRandomEvent(pName); }

class GrapaLibraryRuleSetBitEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSetBitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSetBit(GrapaCHAR& pName) { return new GrapaLibraryRuleSetBitEvent(pName); }

class GrapaLibraryRuleClearBitEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleClearBitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleClearBit(GrapaCHAR& pName) { return new GrapaLibraryRuleClearBitEvent(pName); }

class GrapaLibraryRuleGenBitsEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGenBitsEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGenBits(GrapaCHAR& pName) { return new GrapaLibraryRuleGenBitsEvent(pName); }

class GrapaLibraryRuleGenKeysEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGenKeysEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGenKeys(GrapaCHAR& pName) { return new GrapaLibraryRuleGenKeysEvent(pName); }

/*
class GrapaLibraryRuleGenECEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGenECEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGenEC(GrapaCHAR& pName) { return new GrapaLibraryRuleGenECEvent(pName); }
*/

class GrapaLibraryRuleEncodeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleEncodeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleEncode(GrapaCHAR& pName) { return new GrapaLibraryRuleEncodeEvent(pName); }

class GrapaLibraryRuleDecodeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDecodeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDecode(GrapaCHAR& pName) { return new GrapaLibraryRuleDecodeEvent(pName); }

class GrapaLibraryRuleSignEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSignEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSign(GrapaCHAR& pName) { return new GrapaLibraryRuleSignEvent(pName); }

class GrapaLibraryRuleSignAddEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSignAddEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSignAdd(GrapaCHAR& pName) { return new GrapaLibraryRuleSignAddEvent(pName); }

class GrapaLibraryRuleVerifyEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleVerifyEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleVerify(GrapaCHAR& pName) { return new GrapaLibraryRuleVerifyEvent(pName); }

class GrapaLibraryRuleVerifyRecoverEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleVerifyRecoverEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleVerifyRecover(GrapaCHAR& pName) { return new GrapaLibraryRuleVerifyRecoverEvent(pName); }

class GrapaLibraryRuleSecretEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSecretEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSecret(GrapaCHAR& pName) { return new GrapaLibraryRuleSecretEvent(pName); }

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleMessageEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMessageEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMessage(GrapaCHAR& pName) { return new GrapaLibraryRuleMessageEvent(pName); }

class GrapaLibraryRuleStringEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleStringEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleString(GrapaCHAR& pName) { return new GrapaLibraryRuleStringEvent(pName); }

class GrapaLibraryRuleEchoEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleEchoEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleEcho(GrapaCHAR& pName) { return new GrapaLibraryRuleEchoEvent(pName); }

class GrapaLibraryRuleConsoleEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleConsoleEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleConsole(GrapaCHAR& pName) { return new GrapaLibraryRuleConsoleEvent(pName); }

class GrapaLibraryRulePromptEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePromptEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePrompt(GrapaCHAR& pName) { return new GrapaLibraryRulePromptEvent(pName); }

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleAddEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleAddEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleAdd(GrapaCHAR& pName) { return new GrapaLibraryRuleAddEvent(pName); }

class GrapaLibraryRuleSubEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSubEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSub(GrapaCHAR& pName) { return new GrapaLibraryRuleSubEvent(pName); }

class GrapaLibraryRuleMulEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMulEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMul(GrapaCHAR& pName) { return new GrapaLibraryRuleMulEvent(pName); }

class GrapaLibraryRuleDivEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDivEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDiv(GrapaCHAR& pName) { return new GrapaLibraryRuleDivEvent(pName); }

class GrapaLibraryRuleModEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleModEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMod(GrapaCHAR& pName) { return new GrapaLibraryRuleModEvent(pName); }

class GrapaLibraryRulePowEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePowEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePow(GrapaCHAR& pName) { return new GrapaLibraryRulePowEvent(pName); }

class GrapaLibraryRuleSetFloatEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSetFloatEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSetFloat(GrapaCHAR& pName) { return new GrapaLibraryRuleSetFloatEvent(pName); }

class GrapaLibraryRuleSetFixEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSetFixEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSetFix(GrapaCHAR& pName) { return new GrapaLibraryRuleSetFixEvent(pName); }

class GrapaLibraryRuleRootEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRootEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRoot(GrapaCHAR& pName) { return new GrapaLibraryRuleRootEvent(pName); }

class GrapaLibraryRulePiEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePiEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandlePi(GrapaCHAR& pName) { return new GrapaLibraryRulePiEvent(pName); }

class GrapaLibraryRuleSinEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSinEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSin(GrapaCHAR& pName) { return new GrapaLibraryRuleSinEvent(pName); }

class GrapaLibraryRuleCosEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCosEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCos(GrapaCHAR& pName) { return new GrapaLibraryRuleCosEvent(pName); }

class GrapaLibraryRuleTanEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTanEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTan(GrapaCHAR& pName) { return new GrapaLibraryRuleTanEvent(pName); }

class GrapaLibraryRuleCotEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCotEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCot(GrapaCHAR& pName) { return new GrapaLibraryRuleCotEvent(pName); }

class GrapaLibraryRuleSecEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSecEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSec(GrapaCHAR& pName) { return new GrapaLibraryRuleSecEvent(pName); }

class GrapaLibraryRuleCscEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCscEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCsc(GrapaCHAR& pName) { return new GrapaLibraryRuleCscEvent(pName); }



class GrapaLibraryRuleASinEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleASinEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleASin(GrapaCHAR& pName) { return new GrapaLibraryRuleASinEvent(pName); }

class GrapaLibraryRuleACosEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleACosEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleACos(GrapaCHAR& pName) { return new GrapaLibraryRuleACosEvent(pName); }

class GrapaLibraryRuleATanEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleATanEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleATan(GrapaCHAR& pName) { return new GrapaLibraryRuleATanEvent(pName); }

class GrapaLibraryRuleACotEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleACotEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleACot(GrapaCHAR& pName) { return new GrapaLibraryRuleACotEvent(pName); }

class GrapaLibraryRuleASecEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleASecEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleASec(GrapaCHAR& pName) { return new GrapaLibraryRuleASecEvent(pName); }

class GrapaLibraryRuleACscEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleACscEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleACsc(GrapaCHAR& pName) { return new GrapaLibraryRuleACscEvent(pName); }



class GrapaLibraryRuleSinHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSinHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSinH(GrapaCHAR& pName) { return new GrapaLibraryRuleSinHEvent(pName); }

class GrapaLibraryRuleCosHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCosHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCosH(GrapaCHAR& pName) { return new GrapaLibraryRuleCosHEvent(pName); }

class GrapaLibraryRuleTanHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTanHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTanH(GrapaCHAR& pName) { return new GrapaLibraryRuleTanHEvent(pName); }

class GrapaLibraryRuleCotHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCotHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCotH(GrapaCHAR& pName) { return new GrapaLibraryRuleCotHEvent(pName); }

class GrapaLibraryRuleSecHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSecHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSecH(GrapaCHAR& pName) { return new GrapaLibraryRuleSecHEvent(pName); }

class GrapaLibraryRuleCscHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCscHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCscH(GrapaCHAR& pName) { return new GrapaLibraryRuleCscHEvent(pName); }



class GrapaLibraryRuleASinHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleASinHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleASinH(GrapaCHAR& pName) { return new GrapaLibraryRuleASinHEvent(pName); }

class GrapaLibraryRuleACosHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleACosHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleACosH(GrapaCHAR& pName) { return new GrapaLibraryRuleACosHEvent(pName); }

class GrapaLibraryRuleATanHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleATanHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleATanH(GrapaCHAR& pName) { return new GrapaLibraryRuleATanHEvent(pName); }

class GrapaLibraryRuleACotHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleACotHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleACotH(GrapaCHAR& pName) { return new GrapaLibraryRuleACotHEvent(pName); }

class GrapaLibraryRuleASecHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleASecHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleASecH(GrapaCHAR& pName) { return new GrapaLibraryRuleASecHEvent(pName); }

class GrapaLibraryRuleACscHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleACscHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleACscH(GrapaCHAR& pName) { return new GrapaLibraryRuleACscHEvent(pName); }

class GrapaLibraryRuleATan2Event : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleATan2Event(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleATan2(GrapaCHAR& pName) { return new GrapaLibraryRuleATan2Event(pName); }

class GrapaLibraryRuleHypotEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleHypotEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleHypot(GrapaCHAR& pName) { return new GrapaLibraryRuleHypotEvent(pName); }

class GrapaLibraryRuleLnEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLnEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLn(GrapaCHAR& pName) { return new GrapaLibraryRuleLnEvent(pName); }

class GrapaLibraryRuleLogEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLogEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLog(GrapaCHAR& pName) { return new GrapaLibraryRuleLogEvent(pName); }

class GrapaLibraryRuleEEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleEEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleE(GrapaCHAR& pName) { return new GrapaLibraryRuleEEvent(pName); }

class GrapaLibraryRuleModPowEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleModPowEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleModPow(GrapaCHAR& pName) { return new GrapaLibraryRuleModPowEvent(pName); }

class GrapaLibraryRuleModInvEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleModInvEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleModInv(GrapaCHAR& pName) { return new GrapaLibraryRuleModInvEvent(pName); }

class GrapaLibraryRuleAbsEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleAbsEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleAbs(GrapaCHAR& pName) { return new GrapaLibraryRuleAbsEvent(pName); }

class GrapaLibraryRuleGcdEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGcdEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGcd(GrapaCHAR& pName) { return new GrapaLibraryRuleGcdEvent(pName); }

class GrapaLibraryRuleBslEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBslEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBsl(GrapaCHAR& pName) { return new GrapaLibraryRuleBslEvent(pName); }

class GrapaLibraryRuleBsrEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBsrEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBsr(GrapaCHAR& pName) { return new GrapaLibraryRuleBsrEvent(pName); }

class GrapaLibraryRuleBorEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBorEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBor(GrapaCHAR& pName) { return new GrapaLibraryRuleBorEvent(pName); }

class GrapaLibraryRuleBandEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBandEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBand(GrapaCHAR& pName) { return new GrapaLibraryRuleBandEvent(pName); }

class GrapaLibraryRuleOrEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleOrEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleOr(GrapaCHAR& pName) { return new GrapaLibraryRuleOrEvent(pName); }

class GrapaLibraryRuleAndEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleAndEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleAnd(GrapaCHAR& pName) { return new GrapaLibraryRuleAndEvent(pName); }

class GrapaLibraryRuleXOrEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleXOrEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleXOr(GrapaCHAR& pName) { return new GrapaLibraryRuleXOrEvent(pName); }

class GrapaLibraryRuleInvEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleInvEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleInv(GrapaCHAR& pName) { return new GrapaLibraryRuleInvEvent(pName); }

class GrapaLibraryRuleTransposeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTransposeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTranspose(GrapaCHAR& pName) { return new GrapaLibraryRuleTransposeEvent(pName); }

class GrapaLibraryRuleRrefEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRrefEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRref(GrapaCHAR& pName) { return new GrapaLibraryRuleRrefEvent(pName); }

class GrapaLibraryRuleDetEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDetEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDet(GrapaCHAR& pName) { return new GrapaLibraryRuleDetEvent(pName); }

class GrapaLibraryRuleRankEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRankEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRank(GrapaCHAR& pName) { return new GrapaLibraryRuleRankEvent(pName); }

class GrapaLibraryRuleSolveEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSolveEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSolve(GrapaCHAR& pName) { return new GrapaLibraryRuleSolveEvent(pName); }

class GrapaLibraryRuleCovEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCovEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCov(GrapaCHAR& pName) { return new GrapaLibraryRuleCovEvent(pName); }

class GrapaLibraryRuleNegEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNegEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleNeg(GrapaCHAR& pName) { return new GrapaLibraryRuleNegEvent(pName); }

class GrapaLibraryRuleBitsEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBitsEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBits(GrapaCHAR& pName) { return new GrapaLibraryRuleBitsEvent(pName); }

class GrapaLibraryRuleBytesEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBytesEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBytes(GrapaCHAR& pName) { return new GrapaLibraryRuleBytesEvent(pName); }

class GrapaLibraryRuleLenEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLenEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLen(GrapaCHAR& pName) { return new GrapaLibraryRuleLenEvent(pName); }

class GrapaLibraryRuleBoolEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBoolEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBool(GrapaCHAR& pName) { return new GrapaLibraryRuleBoolEvent(pName); }

class GrapaLibraryRuleNotEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNotEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleNot(GrapaCHAR& pName) { return new GrapaLibraryRuleNotEvent(pName); }

class GrapaLibraryRuleIntEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIntEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleInt(GrapaCHAR& pName) { return new GrapaLibraryRuleIntEvent(pName); }

class GrapaLibraryRuleRawEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRawEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRaw(GrapaCHAR& pName) { return new GrapaLibraryRuleRawEvent(pName); }

class GrapaLibraryRuleBaseEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleBaseEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleBase(GrapaCHAR& pName) { return new GrapaLibraryRuleBaseEvent(pName); }

class GrapaLibraryRuleStrEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleStrEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleStr(GrapaCHAR& pName) { return new GrapaLibraryRuleStrEvent(pName); }

class GrapaLibraryRuleListEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleListEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleList(GrapaCHAR& pName) { return new GrapaLibraryRuleListEvent(pName); }

class GrapaLibraryRuleArrayEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleArrayEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleArray(GrapaCHAR& pName) { return new GrapaLibraryRuleArrayEvent(pName); }

class GrapaLibraryRuleVectorEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleVectorEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleVector(GrapaCHAR& pName) { return new GrapaLibraryRuleVectorEvent(pName); }

class GrapaLibraryRuleXmlEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleXmlEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleXml(GrapaCHAR& pName) { return new GrapaLibraryRuleXmlEvent(pName); }

class GrapaLibraryRuleFloatEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleFloatEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleFloat(GrapaCHAR& pName) { return new GrapaLibraryRuleFloatEvent(pName); }

class GrapaLibraryRuleTimeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTimeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTime(GrapaCHAR& pName) { return new GrapaLibraryRuleTimeEvent(pName); }

class GrapaLibraryRuleTypeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTypeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleType(GrapaCHAR& pName) { return new GrapaLibraryRuleTypeEvent(pName); }

class GrapaLibraryRuleDescribeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDescribeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDescribe(GrapaCHAR& pName) { return new GrapaLibraryRuleDescribeEvent(pName); }

class GrapaLibraryRuleLeftEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLeftEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLeft(GrapaCHAR& pName) { return new GrapaLibraryRuleLeftEvent(pName); }

class GrapaLibraryRuleRightEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRightEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRight(GrapaCHAR& pName) { return new GrapaLibraryRuleRightEvent(pName); }

class GrapaLibraryRuleMidEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMidEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMid(GrapaCHAR& pName) { return new GrapaLibraryRuleMidEvent(pName); }

class GrapaLibraryRuleMidTrimEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMidTrimEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMidTrim(GrapaCHAR& pName) { return new GrapaLibraryRuleMidTrimEvent(pName); }

class GrapaLibraryRuleRTrimEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRTrimEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRTrim(GrapaCHAR& pName) { return new GrapaLibraryRuleRTrimEvent(pName); }

class GrapaLibraryRuleLTrimEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLTrimEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLTrim(GrapaCHAR& pName) { return new GrapaLibraryRuleLTrimEvent(pName); }

class GrapaLibraryRuleTrimEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTrimEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTrim(GrapaCHAR& pName) { return new GrapaLibraryRuleTrimEvent(pName); }

class GrapaLibraryRuleRRotateEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRRotateEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRRotate(GrapaCHAR& pName) { return new GrapaLibraryRuleRRotateEvent(pName); }

class GrapaLibraryRuleLRotateEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLRotateEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLRotate(GrapaCHAR& pName) { return new GrapaLibraryRuleLRotateEvent(pName); }

class GrapaLibraryRuleRPadEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleRPadEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleRPad(GrapaCHAR& pName) { return new GrapaLibraryRuleRPadEvent(pName); }

class GrapaLibraryRuleLPadEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLPadEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLPad(GrapaCHAR& pName) { return new GrapaLibraryRuleLPadEvent(pName); }

class GrapaLibraryRuleReverseEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleReverseEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleReverse(GrapaCHAR& pName) { return new GrapaLibraryRuleReverseEvent(pName); }

class GrapaLibraryRuleReplaceEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleReplaceEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleReplace(GrapaCHAR& pName) { return new GrapaLibraryRuleReplaceEvent(pName); }

class GrapaLibraryRuleGrepEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGrepEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGrep(GrapaCHAR& pName) { return new GrapaLibraryRuleGrepEvent(pName); }

class GrapaLibraryRuleSplitEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSplitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSplit(GrapaCHAR& pName) { return new GrapaLibraryRuleSplitEvent(pName); }

class GrapaLibraryRuleJoinEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleJoinEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleJoin(GrapaCHAR& pName) { return new GrapaLibraryRuleJoinEvent(pName); }

class GrapaLibraryRuleShapeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleShapeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleShape(GrapaCHAR& pName) { return new GrapaLibraryRuleShapeEvent(pName); }

class GrapaLibraryRuleReShapeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleReShapeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleReShape(GrapaCHAR& pName) { return new GrapaLibraryRuleReShapeEvent(pName); }

class GrapaLibraryRuleDotEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDotEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDot(GrapaCHAR& pName) { return new GrapaLibraryRuleDotEvent(pName); }

class GrapaLibraryRuleIdentityEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleIdentityEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleIdentity(GrapaCHAR& pName) { return new GrapaLibraryRuleIdentityEvent(pName); }

class GrapaLibraryRuleDiagonalEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleDiagonalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleDiagonal(GrapaCHAR& pName) { return new GrapaLibraryRuleDiagonalEvent(pName); }

class GrapaLibraryRuleTriUEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTriUEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTriU(GrapaCHAR& pName) { return new GrapaLibraryRuleTriUEvent(pName); }

class GrapaLibraryRuleTriLEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTriLEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTriL(GrapaCHAR& pName) { return new GrapaLibraryRuleTriLEvent(pName); }

class GrapaLibraryRuleEigHEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleEigHEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleEigH(GrapaCHAR& pName) { return new GrapaLibraryRuleEigHEvent(pName); }

class GrapaLibraryRuleSumEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSumEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSum(GrapaCHAR& pName) { return new GrapaLibraryRuleSumEvent(pName); }

class GrapaLibraryRuleMeanEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleMeanEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleMean(GrapaCHAR& pName) { return new GrapaLibraryRuleMeanEvent(pName); }

class GrapaLibraryRuleLowerEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLowerEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLower(GrapaCHAR& pName) { return new GrapaLibraryRuleLowerEvent(pName); }

class GrapaLibraryRuleUpperEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleUpperEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleUpper(GrapaCHAR& pName) { return new GrapaLibraryRuleUpperEvent(pName); }

class GrapaLibraryRuleUtcEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleUtcEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleUtc(GrapaCHAR& pName) { return new GrapaLibraryRuleUtcEvent(pName); }

class GrapaLibraryRuleTzEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTzEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTz(GrapaCHAR& pName) { return new GrapaLibraryRuleTzEvent(pName); }

class GrapaLibraryRuleEqEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleEqEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleEq(GrapaCHAR& pName) { return new GrapaLibraryRuleEqEvent(pName); }

class GrapaLibraryRuleNEqEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleNEqEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleNEq(GrapaCHAR& pName) { return new GrapaLibraryRuleNEqEvent(pName); }

class GrapaLibraryRuleGtEqEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGtEqEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGtEq(GrapaCHAR& pName) { return new GrapaLibraryRuleGtEqEvent(pName); }

class GrapaLibraryRuleGtEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleGtEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleGt(GrapaCHAR& pName) { return new GrapaLibraryRuleGtEvent(pName); }

class GrapaLibraryRuleLtEqEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLtEqEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLtEq(GrapaCHAR& pName) { return new GrapaLibraryRuleLtEqEvent(pName); }

class GrapaLibraryRuleLtEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLtEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLt(GrapaCHAR& pName) { return new GrapaLibraryRuleLtEvent(pName); }

class GrapaLibraryRuleCmpEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleCmpEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleCmp(GrapaCHAR& pName) { return new GrapaLibraryRuleCmpEvent(pName); }

///////////////////////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleTryLockEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleTryLockEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleTryLock(GrapaCHAR& pName) { return new GrapaLibraryRuleTryLockEvent(pName); }

class GrapaLibraryRuleLockEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleLockEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleLock(GrapaCHAR& pName) { return new GrapaLibraryRuleLockEvent(pName); }

class GrapaLibraryRuleUnLockEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleUnLockEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleUnLock(GrapaCHAR& pName) { return new GrapaLibraryRuleUnLockEvent(pName); }

class GrapaLibraryRuleWaitEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWaitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWait(GrapaCHAR& pName) { return new GrapaLibraryRuleWaitEvent(pName); }

class GrapaLibraryRuleSignalEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSignalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSignal(GrapaCHAR& pName) { return new GrapaLibraryRuleSignalEvent(pName); }

class GrapaLibraryRuleWaitingEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWaitingEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWaiting(GrapaCHAR& pName) { return new GrapaLibraryRuleWaitingEvent(pName); }

class GrapaLibraryRuleStartEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleStartEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleStart(GrapaCHAR& pName) { return new GrapaLibraryRuleStartEvent(pName); }

class GrapaLibraryRuleStopEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleStopEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleStop(GrapaCHAR& pName) { return new GrapaLibraryRuleStopEvent(pName); }

class GrapaLibraryRuleStartedEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleStartedEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleStarted(GrapaCHAR& pName) { return new GrapaLibraryRuleStartedEvent(pName); }

class GrapaLibraryRuleSuspendEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSuspendEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSuspend(GrapaCHAR& pName) { return new GrapaLibraryRuleSuspendEvent(pName); }

class GrapaLibraryRuleResumeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleResumeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleResume(GrapaCHAR& pName) { return new GrapaLibraryRuleResumeEvent(pName); }

class GrapaLibraryRuleSuspendedEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleSuspendedEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleSuspended(GrapaCHAR& pName) { return new GrapaLibraryRuleSuspendedEvent(pName); }

class GrapaLibraryRuleWidgetNewEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetNewEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetNew(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetNewEvent(pName); }

class GrapaLibraryRuleWidgetShowEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetShowEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetShow(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetShowEvent(pName); }

class GrapaLibraryRuleWidgetHideEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetHideEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetHide(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetHideEvent(pName); }

class GrapaLibraryRuleWidgetRedrawEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetRedrawEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetRedraw(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetRedrawEvent(pName); }

class GrapaLibraryRuleWidgetResizableEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetResizableEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetResizable(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetResizableEvent(pName); }

class GrapaLibraryRuleWidgetResizeEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetResizeEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetResize(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetResizeEvent(pName); }

class GrapaLibraryRuleWidgetParentEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetParentEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetParent(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetParentEvent(pName); }

class GrapaLibraryRuleWidgetChildEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetChildEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetChild(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetChildEvent(pName); }

class GrapaLibraryRuleWidgetNextEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetNextEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetNext(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetNextEvent(pName); }

class GrapaLibraryRuleWidgetFocusEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetFocusEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetFocus(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetFocusEvent(pName); }

class GrapaLibraryRuleWidgetGetEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetGetEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetGet(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetGetEvent(pName); }

class GrapaLibraryRuleWidgetSetEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetSetEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetSet(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetSetEvent(pName); }

class GrapaLibraryRuleWidgetHandleEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetHandleEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetHandle(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetHandleEvent(pName); }

class GrapaLibraryRuleWidgetCallbackEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetCallbackEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetCallback(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetCallbackEvent(pName); }

class GrapaLibraryRuleWidgetEventKeyEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetEventKeyEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetEventKey(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetEventKeyEvent(pName); }

class GrapaLibraryRuleWidgetAppendEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetAppendEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetAppend(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetAppendEvent(pName); }

class GrapaLibraryRuleWidgetPostEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetPostEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetPost(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetPostEvent(pName); }

class GrapaLibraryRuleWidgetClearEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleWidgetClearEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
GrapaLibraryEvent* GrapaLibraryRuleEvent::HandleWidgetClear(GrapaCHAR& pName) { return new GrapaLibraryRuleWidgetClearEvent(pName); }



///////////////////////////////////////////////////////////////////////////////////////////////////

void GrapaLibraryRuleEvent::LoadLibWrap(GrapaScriptExec* vScriptExec, GrapaRuleEvent* pOperation)
{
	if (!pOperation || pOperation->vLibraryEvent) return;
	GrapaCHAR pName;
	s64 idx;
	if (pOperation->vQueue && pOperation->vQueue->mCount)
		pName.FROM(pOperation->vQueue->Head()->mValue);
	else
		pName.FROM(pOperation->mValue);
	if (vLibraryQueue == NULL) vLibraryQueue = new GrapaLibraryQueue();
	GrapaLibraryEvent* lib = vLibraryQueue->Search(pName, idx);
	if (lib == NULL)
	{
		lib = LoadLib(vScriptExec, pOperation, pName);
		if (lib) vLibraryQueue->PushTail(lib);
	}
	pOperation->vLibraryEvent = lib;
}

#include <unordered_map>
#include <functional>

GrapaLibraryEvent* GrapaLibraryRuleEvent::LoadLib(GrapaScriptExec *vScriptExec, GrapaRuleEvent *pOperation, GrapaCHAR& pName)
{
	if (pName.mBytes == NULL) return NULL;
	using Handler = GrapaLibraryEvent * (GrapaLibraryRuleEvent::*)(GrapaCHAR& pName);
	static const std::unordered_map<std::string, Handler> handlerMap = {
		{ "rule", &GrapaLibraryRuleEvent::HandleRule },
		{ "ruleval", &GrapaLibraryRuleEvent::HandleRuleVal },
		{ "ruleref", &GrapaLibraryRuleEvent::HandleRuleRef },
		{ "token", &GrapaLibraryRuleEvent::HandleToken },
		{ "grammar", &GrapaLibraryRuleEvent::HandleGrammar },
		{ "reset", &GrapaLibraryRuleEvent::HandleReset },
		{ "shell", &GrapaLibraryRuleEvent::HandleShell },
		{ "eval", &GrapaLibraryRuleEvent::HandleEval },
		{ "exec", &GrapaLibraryRuleEvent::HandleExec },
		{ "post", &GrapaLibraryRuleEvent::HandlePost },
		{ "sleep", &GrapaLibraryRuleEvent::HandleSleep },
		{ "exit", &GrapaLibraryRuleEvent::HandleExit },
		{ "getenv", &GrapaLibraryRuleEvent::HandleGetEnv },
		{ "putenv", &GrapaLibraryRuleEvent::HandlePutEnv },
		{ "include", &GrapaLibraryRuleEvent::HandleInclude },
		{ "name", &GrapaLibraryRuleEvent::HandleName },
		{ "getname", &GrapaLibraryRuleEvent::HandleGetName },
		{ "lit", &GrapaLibraryRuleEvent::HandleLit },
		{ "ref", &GrapaLibraryRuleEvent::HandleRef },
		{ "setnull", &GrapaLibraryRuleEvent::HandleNull },
		{ "var", &GrapaLibraryRuleEvent::HandleVar },
		{ "assign", &GrapaLibraryRuleEvent::HandleAssign },
		{ "assignappend", &GrapaLibraryRuleEvent::HandleAssignAppend },
		{ "assignextend", &GrapaLibraryRuleEvent::HandleAssignExtend },
		{ "createarray", &GrapaLibraryRuleEvent::HandleCreateArray },
		{ "createtuple", &GrapaLibraryRuleEvent::HandleCreateTuple },
		{ "createlist", &GrapaLibraryRuleEvent::HandleCreateList },
		{ "createxml", &GrapaLibraryRuleEvent::HandleCreateXml },
		{ "createel", &GrapaLibraryRuleEvent::HandleCreateEl },
		{ "createtag", &GrapaLibraryRuleEvent::HandleCreateTag },
		{ "createop", &GrapaLibraryRuleEvent::HandleCreateOp },
		{ "createcode", &GrapaLibraryRuleEvent::HandleCreateCode },
		{ "createrule", &GrapaLibraryRuleEvent::HandleCreateRule },
		{ "extend", &GrapaLibraryRuleEvent::HandleExtend },
		{ "remove", &GrapaLibraryRuleEvent::HandleRemove },
		{ "prepend", &GrapaLibraryRuleEvent::HandlePrepend },
		{ "pop", &GrapaLibraryRuleEvent::HandlePop },
		{ "call", &GrapaLibraryRuleEvent::HandleCall },
		{ "search", &GrapaLibraryRuleEvent::HandleSearch },
		{ "findall", &GrapaLibraryRuleEvent::HandleFindAll },
		{ "sort", &GrapaLibraryRuleEvent::HandleSort },
		{ "argsort", &GrapaLibraryRuleEvent::HandleArgSort },
		{ "unique", &GrapaLibraryRuleEvent::HandleUnique },
		{ "group", &GrapaLibraryRuleEvent::HandleGroup },
		{ "class", &GrapaLibraryRuleEvent::HandleClass },
		{ "obj", &GrapaLibraryRuleEvent::HandleObj },
		{ "global", &GrapaLibraryRuleEvent::HandleGlobal },
		{ "parent", &GrapaLibraryRuleEvent::HandleParent },
		{ "this", &GrapaLibraryRuleEvent::HandleThis },
		{ "local", &GrapaLibraryRuleEvent::HandleLocal },
		{ "static", &GrapaLibraryRuleEvent::HandleStatic },
		{ "const", &GrapaLibraryRuleEvent::HandleConst },
		{ "setconst", &GrapaLibraryRuleEvent::HandleSetConst },
		{ "return", &GrapaLibraryRuleEvent::HandleReturn },
		{ "break", &GrapaLibraryRuleEvent::HandleBreak },
		{ "if", &GrapaLibraryRuleEvent::HandleIf },
		{ "while", &GrapaLibraryRuleEvent::HandleWhile },
		{ "scope", &GrapaLibraryRuleEvent::HandleScope },
		{ "switch", &GrapaLibraryRuleEvent::HandleSwitch },
		{ "case", &GrapaLibraryRuleEvent::HandleCase },
		{ "plan", &GrapaLibraryRuleEvent::HandlePlan },
		{ "wrap", &GrapaLibraryRuleEvent::HandleWrap },
		{ "op", &GrapaLibraryRuleEvent::HandleOp },
		{ "map", &GrapaLibraryRuleEvent::HandleMap },
		{ "reduce", &GrapaLibraryRuleEvent::HandleReduce },
		{ "filter", &GrapaLibraryRuleEvent::HandleFilter },
		{ "range", &GrapaLibraryRuleEvent::HandleRange },
		{ "isint", &GrapaLibraryRuleEvent::HandleIsInt },
		{ "iferr", &GrapaLibraryRuleEvent::HandleIfErr },
		{ "message", &GrapaLibraryRuleEvent::HandleMessage },
		{ "string", &GrapaLibraryRuleEvent::HandleString },
		{ "echo", &GrapaLibraryRuleEvent::HandleEcho },
		{ "console", &GrapaLibraryRuleEvent::HandleConsole },
		{ "prompt", &GrapaLibraryRuleEvent::HandlePrompt },
		{ "genprime", &GrapaLibraryRuleEvent::HandleGenPrime },
		{ "staticprime", &GrapaLibraryRuleEvent::HandleStaticPrime },
		{ "isprime", &GrapaLibraryRuleEvent::HandleIsPrime },
		{ "isaks", &GrapaLibraryRuleEvent::HandleIsAks },
		{ "random", &GrapaLibraryRuleEvent::HandleRandom },
		{ "setbit", &GrapaLibraryRuleEvent::HandleSetBit },
		{ "clearbit", &GrapaLibraryRuleEvent::HandleClearBit },
		{ "genbits", &GrapaLibraryRuleEvent::HandleGenBits },
		{ "genkeys", &GrapaLibraryRuleEvent::HandleGenKeys },
		{ "encode", &GrapaLibraryRuleEvent::HandleEncode },
		{ "encoderaw", &GrapaLibraryRuleEvent::HandleEncode },
		{ "decode", &GrapaLibraryRuleEvent::HandleDecode },
		{ "sign", &GrapaLibraryRuleEvent::HandleSign },
		{ "signadd", &GrapaLibraryRuleEvent::HandleSignAdd },
		{ "verify", &GrapaLibraryRuleEvent::HandleVerify },
		{ "verifyrecover", &GrapaLibraryRuleEvent::HandleVerifyRecover },
		{ "secret", &GrapaLibraryRuleEvent::HandleSecret },
		{ "setfloat", &GrapaLibraryRuleEvent::HandleSetFloat },
		{ "setfix", &GrapaLibraryRuleEvent::HandleSetFix },
		{ "root", &GrapaLibraryRuleEvent::HandleRoot },
		{ "pow", &GrapaLibraryRuleEvent::HandlePow },
		{ "mod", &GrapaLibraryRuleEvent::HandleMod },
		{ "modpow", &GrapaLibraryRuleEvent::HandleModPow },
		{ "modinv", &GrapaLibraryRuleEvent::HandleModInv },
		{ "abs", &GrapaLibraryRuleEvent::HandleAbs },
		{ "gcd", &GrapaLibraryRuleEvent::HandleGcd },
		{ "e", &GrapaLibraryRuleEvent::HandleE },
		{ "pi", &GrapaLibraryRuleEvent::HandlePi },
		{ "ln", &GrapaLibraryRuleEvent::HandleLn },
		{ "log", &GrapaLibraryRuleEvent::HandleLog },
		{ "add", &GrapaLibraryRuleEvent::HandleAdd },
		{ "sub", &GrapaLibraryRuleEvent::HandleSub },
		{ "mul", &GrapaLibraryRuleEvent::HandleMul },
		{ "div", &GrapaLibraryRuleEvent::HandleDiv },
		{ "sin", &GrapaLibraryRuleEvent::HandleSin },
		{ "cos", &GrapaLibraryRuleEvent::HandleCos },
		{ "tan", &GrapaLibraryRuleEvent::HandleTan },
		{ "cot", &GrapaLibraryRuleEvent::HandleCot },
		{ "sec", &GrapaLibraryRuleEvent::HandleSec },
		{ "csc", &GrapaLibraryRuleEvent::HandleCsc },
		{ "asin", &GrapaLibraryRuleEvent::HandleASin },
		{ "acos", &GrapaLibraryRuleEvent::HandleACos },
		{ "atan", &GrapaLibraryRuleEvent::HandleATan },
		{ "acot", &GrapaLibraryRuleEvent::HandleACot },
		{ "asec", &GrapaLibraryRuleEvent::HandleASec },
		{ "acsc", &GrapaLibraryRuleEvent::HandleACsc },
		{ "sinh", &GrapaLibraryRuleEvent::HandleSinH },
		{ "cosh", &GrapaLibraryRuleEvent::HandleCosH },
		{ "tanh", &GrapaLibraryRuleEvent::HandleTanH },
		{ "coth", &GrapaLibraryRuleEvent::HandleCotH },
		{ "sech", &GrapaLibraryRuleEvent::HandleSecH },
		{ "csch", &GrapaLibraryRuleEvent::HandleCscH },
		{ "asinh", &GrapaLibraryRuleEvent::HandleASinH },
		{ "acosh", &GrapaLibraryRuleEvent::HandleACosH },
		{ "atanh", &GrapaLibraryRuleEvent::HandleATanH },
		{ "acoth", &GrapaLibraryRuleEvent::HandleACotH },
		{ "asech", &GrapaLibraryRuleEvent::HandleASecH },
		{ "acsch", &GrapaLibraryRuleEvent::HandleACscH },
		{ "atan2", &GrapaLibraryRuleEvent::HandleATan2 },
		{ "hypot", &GrapaLibraryRuleEvent::HandleHypot },
		{ "bsl", &GrapaLibraryRuleEvent::HandleBsl },
		{ "bsr", &GrapaLibraryRuleEvent::HandleBsr },
		{ "bor", &GrapaLibraryRuleEvent::HandleBor },
		{ "band", &GrapaLibraryRuleEvent::HandleBand },
		{ "or", &GrapaLibraryRuleEvent::HandleOr },
		{ "and", &GrapaLibraryRuleEvent::HandleAnd },
		{ "xor", &GrapaLibraryRuleEvent::HandleXOr },
		{ "inv", &GrapaLibraryRuleEvent::HandleInv },
		{ "t", &GrapaLibraryRuleEvent::HandleTranspose },
		{ "rref", &GrapaLibraryRuleEvent::HandleRref },
		{ "det", &GrapaLibraryRuleEvent::HandleDet },
		{ "rank", &GrapaLibraryRuleEvent::HandleRank },
		{ "solve", &GrapaLibraryRuleEvent::HandleSolve },
		{ "cov", &GrapaLibraryRuleEvent::HandleCov },
		{ "neg", &GrapaLibraryRuleEvent::HandleNeg },
		{ "bits", &GrapaLibraryRuleEvent::HandleBits },
		{ "bytes", &GrapaLibraryRuleEvent::HandleBytes },
		{ "len", &GrapaLibraryRuleEvent::HandleLen },
		{ "bool", &GrapaLibraryRuleEvent::HandleBool },
		{ "not", &GrapaLibraryRuleEvent::HandleNot },
		{ "int", &GrapaLibraryRuleEvent::HandleInt },
		{ "raw", &GrapaLibraryRuleEvent::HandleRaw },
		{ "base", &GrapaLibraryRuleEvent::HandleBase },
		{ "str", &GrapaLibraryRuleEvent::HandleStr },
		{ "list", &GrapaLibraryRuleEvent::HandleList },
		{ "array", &GrapaLibraryRuleEvent::HandleArray },
		{ "vector", &GrapaLibraryRuleEvent::HandleVector },
		{ "xml", &GrapaLibraryRuleEvent::HandleXml },
		{ "float", &GrapaLibraryRuleEvent::HandleFloat },
		{ "fix", &GrapaLibraryRuleEvent::HandleFloat },
		{ "time", &GrapaLibraryRuleEvent::HandleTime },
		{ "type", &GrapaLibraryRuleEvent::HandleType },
		{ "describe", &GrapaLibraryRuleEvent::HandleDescribe },
		{ "left", &GrapaLibraryRuleEvent::HandleLeft },
		{ "right", &GrapaLibraryRuleEvent::HandleRight },
		{ "mid", &GrapaLibraryRuleEvent::HandleMid },
		{ "midtrim", &GrapaLibraryRuleEvent::HandleMidTrim },
		{ "rtrim", &GrapaLibraryRuleEvent::HandleRTrim },
		{ "ltrim", &GrapaLibraryRuleEvent::HandleLTrim },
		{ "trim", &GrapaLibraryRuleEvent::HandleTrim },
		{ "rrot", &GrapaLibraryRuleEvent::HandleRRotate },
		{ "lrot", &GrapaLibraryRuleEvent::HandleLRotate },
		{ "rpad", &GrapaLibraryRuleEvent::HandleRPad },
		{ "lpad", &GrapaLibraryRuleEvent::HandleLPad },
		{ "reverse", &GrapaLibraryRuleEvent::HandleReverse },
		{ "replace", &GrapaLibraryRuleEvent::HandleReplace },
		{ "grep", &GrapaLibraryRuleEvent::HandleGrep },
		{ "split", &GrapaLibraryRuleEvent::HandleSplit },
		{ "join", &GrapaLibraryRuleEvent::HandleJoin },
		{ "shape", &GrapaLibraryRuleEvent::HandleShape },
		{ "reshape", &GrapaLibraryRuleEvent::HandleReShape },
		{ "dot", &GrapaLibraryRuleEvent::HandleDot },
		{ "identity", &GrapaLibraryRuleEvent::HandleIdentity },
		{ "diagonal", &GrapaLibraryRuleEvent::HandleDiagonal },
		{ "triu", &GrapaLibraryRuleEvent::HandleTriU },
		{ "tril", &GrapaLibraryRuleEvent::HandleTriL },
		{ "eigh", &GrapaLibraryRuleEvent::HandleEigH },
		{ "sum", &GrapaLibraryRuleEvent::HandleSum },
		{ "mean", &GrapaLibraryRuleEvent::HandleMean },
		{ "lower", &GrapaLibraryRuleEvent::HandleLower },
		{ "upper", &GrapaLibraryRuleEvent::HandleUpper },
		{ "eq", &GrapaLibraryRuleEvent::HandleEq },
		{ "neq", &GrapaLibraryRuleEvent::HandleNEq },
		{ "gteq", &GrapaLibraryRuleEvent::HandleGtEq },
		{ "gt", &GrapaLibraryRuleEvent::HandleGt },
		{ "lteq", &GrapaLibraryRuleEvent::HandleLtEq },
		{ "lt", &GrapaLibraryRuleEvent::HandleLt },
		{ "cmp", &GrapaLibraryRuleEvent::HandleCmp },
		{ "utc", &GrapaLibraryRuleEvent::HandleUtc },
		{ "tz", &GrapaLibraryRuleEvent::HandleTz },
		{ "file_table", &GrapaLibraryRuleEvent::HandleTable },
		{ "file_pwd", &GrapaLibraryRuleEvent::HandlePwd },
		{ "file_cd", &GrapaLibraryRuleEvent::HandleCd },
		{ "file_phd", &GrapaLibraryRuleEvent::HandlePhd },
		{ "file_chd", &GrapaLibraryRuleEvent::HandleChd },
		{ "file_ls", &GrapaLibraryRuleEvent::HandleLs },
		{ "file_mk", &GrapaLibraryRuleEvent::HandleMk },
		{ "file_rm", &GrapaLibraryRuleEvent::HandleRm },
		{ "file_mkfield", &GrapaLibraryRuleEvent::HandleMkField },
		{ "file_rmfield", &GrapaLibraryRuleEvent::HandleRmField },
		{ "file_info", &GrapaLibraryRuleEvent::HandleInfo },
		{ "file_set", &GrapaLibraryRuleEvent::HandleSet },
		{ "file_get", &GrapaLibraryRuleEvent::HandleGet },
		{ "file_split", &GrapaLibraryRuleEvent::HandleFileSplit },
		{ "file_debug", &GrapaLibraryRuleEvent::HandleDebug },
		{ "net_mac", &GrapaLibraryRuleEvent::HandleMac },
		{ "net_interfaces", &GrapaLibraryRuleEvent::HandleInterfaces },
		{ "net_connect", &GrapaLibraryRuleEvent::HandleConnect },
		{ "net_bind", &GrapaLibraryRuleEvent::HandleBind },
		{ "net_listen", &GrapaLibraryRuleEvent::HandleListen },
		{ "net_onlisten", &GrapaLibraryRuleEvent::HandleOnListen },
		{ "net_disconnect", &GrapaLibraryRuleEvent::HandleDisconnect },
		{ "net_proxy", &GrapaLibraryRuleEvent::HandleProxy },
		{ "net_certificate", &GrapaLibraryRuleEvent::HandleCertificate },
		{ "net_private", &GrapaLibraryRuleEvent::HandlePrivate },
		{ "net_trusted", &GrapaLibraryRuleEvent::HandleTrusted },
		{ "net_verify", &GrapaLibraryRuleEvent::HandleNetVerify },
		{ "net_chain", &GrapaLibraryRuleEvent::HandleNetChain },
		{ "net_host", &GrapaLibraryRuleEvent::HandleNetHost },
		{ "net_send", &GrapaLibraryRuleEvent::HandleSend },
		{ "net_receive", &GrapaLibraryRuleEvent::HandleReceive },
		{ "net_pending", &GrapaLibraryRuleEvent::HandlePending },
		{ "net_onreceive", &GrapaLibraryRuleEvent::HandleOnReceive },
		{ "http_read", &GrapaLibraryRuleEvent::HandleHttpRead },
		{ "http_send", &GrapaLibraryRuleEvent::HandleHttpSend },
		{ "http_message", &GrapaLibraryRuleEvent::HandleHttpMessage },
		{ "thread_trylock", &GrapaLibraryRuleEvent::HandleTryLock },
		{ "thread_lock", &GrapaLibraryRuleEvent::HandleLock },
		{ "thread_unlock", &GrapaLibraryRuleEvent::HandleUnLock },
		{ "thread_wait", &GrapaLibraryRuleEvent::HandleWait },
		{ "thread_signal", &GrapaLibraryRuleEvent::HandleSignal },
		{ "thread_waiting", &GrapaLibraryRuleEvent::HandleWaiting },
		{ "thread_start", &GrapaLibraryRuleEvent::HandleStart },
		{ "thread_stop", &GrapaLibraryRuleEvent::HandleStop },
		{ "thread_started", &GrapaLibraryRuleEvent::HandleStarted },
		{ "thread_suspend", &GrapaLibraryRuleEvent::HandleSuspend },
		{ "thread_resume", &GrapaLibraryRuleEvent::HandleResume },
		{ "thread_suspended", &GrapaLibraryRuleEvent::HandleSuspended },
		{ "widget_new", &GrapaLibraryRuleEvent::HandleWidgetNew },
		{ "widget_show", &GrapaLibraryRuleEvent::HandleWidgetShow },
		{ "widget_hide", &GrapaLibraryRuleEvent::HandleWidgetHide },
		{ "widget_redraw", &GrapaLibraryRuleEvent::HandleWidgetRedraw },
		{ "widget_resizable", &GrapaLibraryRuleEvent::HandleWidgetResizable },
		{ "widget_resize", &GrapaLibraryRuleEvent::HandleWidgetResize },
		{ "widget_parent", &GrapaLibraryRuleEvent::HandleWidgetParent },
		{ "widget_child", &GrapaLibraryRuleEvent::HandleWidgetChild },
		{ "widget_next", &GrapaLibraryRuleEvent::HandleWidgetNext },
		{ "widget_focus", &GrapaLibraryRuleEvent::HandleWidgetFocus },
		{ "widget_get", &GrapaLibraryRuleEvent::HandleWidgetGet },
		{ "widget_set", &GrapaLibraryRuleEvent::HandleWidgetSet },
		{ "widget_handle", &GrapaLibraryRuleEvent::HandleWidgetHandle },
		{ "widget_callback", &GrapaLibraryRuleEvent::HandleWidgetCallback },
		{ "widget_event_key", &GrapaLibraryRuleEvent::HandleWidgetEventKey },
		{ "widget_append", &GrapaLibraryRuleEvent::HandleWidgetAppend },
		{ "widget_post", &GrapaLibraryRuleEvent::HandleWidgetPost },
		{ "widget_clear", &GrapaLibraryRuleEvent::HandleWidgetClear },
	};
	auto it = handlerMap.find((char*)pName.mBytes);
	if (it != handlerMap.end())
		return (this->*(it->second))(pName);
	return NULL;

	/*
	GrapaLibraryEvent* lib = NULL;
	if (lib == NULL)
	{
		if (pName.Cmp("rule") == 0) lib = new GrapaLibraryRuleRuleEvent(pName);
		else if (pName.Cmp("ruleval") == 0) lib = new GrapaLibraryRuleRuleValEvent(pName);
		else if (pName.Cmp("ruleref") == 0) lib = new GrapaLibraryRuleRuleRefEvent(pName);
		else if (pName.Cmp("token") == 0) lib = new GrapaLibraryRuleTokenEvent(pName);
		else if (pName.Cmp("grammar") == 0) lib = new GrapaLibraryRuleGrammarEvent(pName);
		else if (pName.Cmp("reset") == 0) lib = new GrapaLibraryRuleResetEvent(pName);
		else if (pName.Cmp("shell") == 0) lib = new GrapaLibraryRuleShellEvent(pName);
		else if (pName.Cmp("eval") == 0) lib = new GrapaLibraryRuleEvalEvent(pName);
		else if (pName.Cmp("exec") == 0) lib = new GrapaLibraryRuleExecEvent(pName);
		else if (pName.Cmp("post") == 0) lib = new GrapaLibraryRulePostEvent(pName);
		else if (pName.Cmp("sleep") == 0) lib = new GrapaLibraryRuleSleepEvent(pName);
		else if (pName.Cmp("exit") == 0) lib = new GrapaLibraryRuleExitEvent(pName);
		else if (pName.Cmp("getenv") == 0) lib = new GrapaLibraryRuleGetEnvEvent(pName);
		else if (pName.Cmp("putenv") == 0) lib = new GrapaLibraryRulePutEnvEvent(pName);
		else if (pName.Cmp("include") == 0) lib = new GrapaLibraryRuleIncludeEvent(pName);
		if (lib == NULL)
        {
            if (pName.Cmp("name") == 0) lib = new GrapaLibraryRuleNameEvent(pName);
			else if (pName.Cmp("getname") == 0) lib = new GrapaLibraryRuleGetNameEvent(pName);
			else if (pName.Cmp("lit") == 0) lib = new GrapaLibraryRuleLitEvent(pName);
			else if (pName.Cmp("ref") == 0) lib = new GrapaLibraryRuleRefEvent(pName);
            else if (pName.Cmp("setnull") == 0) lib = new GrapaLibraryRuleNullEvent(pName);
            else if (pName.Cmp("var") == 0) lib = new GrapaLibraryRuleVarEvent(pName);
            else if (pName.Cmp("assign") == 0) lib = new GrapaLibraryRuleAssignEvent(pName);
			else if (pName.Cmp("assignappend") == 0) lib = new GrapaLibraryRuleAssignAppendEvent(pName);
			else if (pName.Cmp("assignextend") == 0) lib = new GrapaLibraryRuleAssignExtendEvent(pName);
			else if (pName.Cmp("createarray") == 0) lib = new GrapaLibraryRuleCreateArrayEvent(pName);
			else if (pName.Cmp("createtuple") == 0) lib = new GrapaLibraryRuleCreateTupleEvent(pName);
			else if (pName.Cmp("createlist") == 0) lib = new GrapaLibraryRuleCreateListEvent(pName);
			else if (pName.Cmp("createxml") == 0) lib = new GrapaLibraryRuleCreateXmlEvent(pName);
			else if (pName.Cmp("createel") == 0) lib = new GrapaLibraryRuleCreateElEvent(pName);
			else if (pName.Cmp("createtag") == 0) lib = new GrapaLibraryRuleCreateTagEvent(pName);
			else if (pName.Cmp("createop") == 0) lib = new GrapaLibraryRuleCreateOpEvent(pName);
			else if (pName.Cmp("createcode") == 0) lib = new GrapaLibraryRuleCreateCodeEvent(pName);
			else if (pName.Cmp("createrule") == 0) lib = new GrapaLibraryRuleCreateRuleEvent(pName);
			else if (pName.Cmp("extend") == 0) lib = new GrapaLibraryRuleExtendEvent(pName);
			else if (pName.Cmp("remove") == 0) lib = new GrapaLibraryRuleRemoveEvent(pName);
			else if (pName.Cmp("prepend") == 0) lib = new GrapaLibraryRulePrependEvent(pName);
			else if (pName.Cmp("pop") == 0) lib = new GrapaLibraryRulePopEvent(pName);
			else if (pName.Cmp("call") == 0) lib = new GrapaLibraryRuleCallEvent(pName);
			else if (pName.Cmp("search") == 0) lib = new GrapaLibraryRuleSearchEvent(pName);
			else if (pName.Cmp("findall") == 0) lib = new GrapaLibraryRuleFindAllEvent(pName);
			else if (pName.Cmp("sort") == 0) lib = new GrapaLibraryRuleSortEvent(pName);
			else if (pName.Cmp("argsort") == 0) lib = new GrapaLibraryRuleArgSortEvent(pName);
			else if (pName.Cmp("unique") == 0) lib = new GrapaLibraryRuleUniqueEvent(pName);
			else if (pName.Cmp("group") == 0) lib = new GrapaLibraryRuleGroupEvent(pName);
			else if (pName.Cmp("class") == 0) lib = new GrapaLibraryRuleClassEvent(pName);
			else if (pName.Cmp("obj") == 0) lib = new GrapaLibraryRuleObjEvent(pName);
			else if (pName.Cmp("global") == 0) lib = new GrapaLibraryRuleGlobalEvent(pName);
            else if (pName.Cmp("parent") == 0) lib = new GrapaLibraryRuleParentEvent(pName);
            else if (pName.Cmp("this") == 0) lib = new GrapaLibraryRuleThisEvent(pName);
			else if (pName.Cmp("local") == 0) lib = new GrapaLibraryRuleLocalEvent(pName);
			else if (pName.Cmp("static") == 0) lib = new GrapaLibraryRuleStaticEvent(pName);
			else if (pName.Cmp("const") == 0) lib = new GrapaLibraryRuleConstEvent(pName);
			else if (pName.Cmp("setconst") == 0) lib = new GrapaLibraryRuleSetConstEvent(pName);
		}
        if (lib == NULL)
        {
            if (pName.Cmp("return") == 0) lib = new GrapaLibraryRuleReturnEvent(pName);
            else if (pName.Cmp("break") == 0) lib = new GrapaLibraryRuleBreakEvent(pName);
            else if (pName.Cmp("if") == 0) lib = new GrapaLibraryRuleIfEvent(pName);
            else if (pName.Cmp("while") == 0) lib = new GrapaLibraryRuleWhileEvent(pName);
            else if (pName.Cmp("scope") == 0) lib = new GrapaLibraryRuleScopeEvent(pName);
            else if (pName.Cmp("switch") == 0) lib = new class GrapaLibraryRuleSwitchEvent(pName);
            else if (pName.Cmp("case") == 0) lib = new GrapaLibraryRuleCaseEvent(pName);
            else if (pName.Cmp("plan") == 0) lib = new GrapaLibraryRulePlanEvent(pName);
            else if (pName.Cmp("wrap") == 0) lib = new GrapaLibraryRuleWrapEvent(pName);
			else if (pName.Cmp("op") == 0) lib = new GrapaLibraryRuleOpEvent(pName);
			else if (pName.Cmp("map") == 0) lib = new GrapaLibraryRuleMapEvent(pName);
            else if (pName.Cmp("reduce") == 0) lib = new GrapaLibraryRuleReduceEvent(pName);
			else if (pName.Cmp("filter") == 0) lib = new GrapaLibraryRuleFilterEvent(pName);
			else if (pName.Cmp("range") == 0) lib = new GrapaLibraryRuleRangeEvent(pName);
			else if (pName.Cmp("isint") == 0) lib = new GrapaLibraryRuleIsIntEvent(pName);
			else if (pName.Cmp("iferr") == 0) lib = new GrapaLibraryRuleIfErrEvent(pName);
			else if (pName.Cmp("message") == 0) lib = new GrapaLibraryRuleMessageEvent(pName);
			else if (pName.Cmp("string") == 0) lib = new GrapaLibraryRuleStringEvent(pName);
			else if (pName.Cmp("echo") == 0) lib = new GrapaLibraryRuleEchoEvent(pName);
			else if (pName.Cmp("console") == 0) lib = new GrapaLibraryRuleConsoleEvent(pName);
			else if (pName.Cmp("prompt") == 0) lib = new GrapaLibraryRulePromptEvent(pName);
		}
         if (lib == NULL)
        {
            if (pName.Cmp("genprime") == 0) lib = new GrapaLibraryRuleGenPrimeEvent(pName);
            else if (pName.Cmp("staticprime") == 0) lib = new GrapaLibraryRuleStaticPrimeEvent(pName);
            else if (pName.Cmp("isprime") == 0) lib = new GrapaLibraryRuleIsPrimeEvent(pName);
            else if (pName.Cmp("isaks") == 0) lib = new GrapaLibraryRuleIsAksEvent(pName);
            else if (pName.Cmp("random") == 0) lib = new GrapaLibraryRuleRandomEvent(pName);
            else if (pName.Cmp("setbit") == 0) lib = new GrapaLibraryRuleSetBitEvent(pName);
            else if (pName.Cmp("clearbit") == 0) lib = new GrapaLibraryRuleClearBitEvent(pName);
            else if (pName.Cmp("genbits") == 0) lib = new GrapaLibraryRuleGenBitsEvent(pName);
			else if (pName.Cmp("genkeys") == 0) lib = new GrapaLibraryRuleGenKeysEvent(pName);
			else if (pName.Cmp("encode") == 0) lib = new GrapaLibraryRuleEncodeEvent(pName);
			else if (pName.Cmp("encoderaw") == 0) lib = new GrapaLibraryRuleEncodeEvent(pName);
			else if (pName.Cmp("decode") == 0) lib = new GrapaLibraryRuleDecodeEvent(pName);
			else if (pName.Cmp("sign") == 0) lib = new GrapaLibraryRuleSignEvent(pName);
			else if (pName.Cmp("signadd") == 0) lib = new GrapaLibraryRuleSignAddEvent(pName);
			else if (pName.Cmp("verify") == 0) lib = new GrapaLibraryRuleVerifyEvent(pName);
			else if (pName.Cmp("verifyrecover") == 0) lib = new GrapaLibraryRuleVerifyRecoverEvent(pName);
			else if (pName.Cmp("secret") == 0) lib = new GrapaLibraryRuleSecretEvent(pName);

			else if (pName.Cmp("setfloat") == 0) lib = new GrapaLibraryRuleSetFloatEvent(pName);
			else if (pName.Cmp("setfix") == 0) lib = new GrapaLibraryRuleSetFixEvent(pName);

			else if (pName.Cmp("root") == 0) lib = new GrapaLibraryRuleRootEvent(pName);
			else if (pName.Cmp("pow") == 0) lib = new GrapaLibraryRulePowEvent(pName);
			else if (pName.Cmp("mod") == 0) lib = new GrapaLibraryRuleModEvent(pName);
			else if (pName.Cmp("modpow") == 0) lib = new GrapaLibraryRuleModPowEvent(pName);
			else if (pName.Cmp("modinv") == 0) lib = new GrapaLibraryRuleModInvEvent(pName);
			else if (pName.Cmp("abs") == 0) lib = new GrapaLibraryRuleAbsEvent(pName);
			else if (pName.Cmp("gcd") == 0) lib = new GrapaLibraryRuleGcdEvent(pName);

			else if (pName.Cmp("e") == 0) lib = new GrapaLibraryRuleEEvent(pName);
			else if (pName.Cmp("pi") == 0) lib = new GrapaLibraryRulePiEvent(pName);
			else if (pName.Cmp("ln") == 0) lib = new GrapaLibraryRuleLnEvent(pName);
			else if (pName.Cmp("log") == 0) lib = new GrapaLibraryRuleLogEvent(pName);

			else if (pName.Cmp("add") == 0) lib = new GrapaLibraryRuleAddEvent(pName);
            else if (pName.Cmp("sub") == 0) lib = new GrapaLibraryRuleSubEvent(pName);
            else if (pName.Cmp("mul") == 0) lib = new GrapaLibraryRuleMulEvent(pName);
            else if (pName.Cmp("div") == 0) lib = new GrapaLibraryRuleDivEvent(pName);
        }
        if (lib == NULL)
        {
            if (pName.Cmp("sin") == 0) lib = new GrapaLibraryRuleSinEvent(pName);
            else if (pName.Cmp("cos") == 0) lib = new GrapaLibraryRuleCosEvent(pName);
            else if (pName.Cmp("tan") == 0) lib = new GrapaLibraryRuleTanEvent(pName);
            else if (pName.Cmp("cot") == 0) lib = new GrapaLibraryRuleCotEvent(pName);
            else if (pName.Cmp("sec") == 0) lib = new GrapaLibraryRuleSecEvent(pName);
            else if (pName.Cmp("csc") == 0) lib = new GrapaLibraryRuleCscEvent(pName);

            else if (pName.Cmp("asin") == 0) lib = new GrapaLibraryRuleASinEvent(pName);
            else if (pName.Cmp("acos") == 0) lib = new GrapaLibraryRuleACosEvent(pName);
            else if (pName.Cmp("atan") == 0) lib = new GrapaLibraryRuleATanEvent(pName);
            else if (pName.Cmp("acot") == 0) lib = new GrapaLibraryRuleACotEvent(pName);
            else if (pName.Cmp("asec") == 0) lib = new GrapaLibraryRuleASecEvent(pName);
            else if (pName.Cmp("acsc") == 0) lib = new GrapaLibraryRuleACscEvent(pName);

            else if (pName.Cmp("sinh") == 0) lib = new GrapaLibraryRuleSinHEvent(pName);
            else if (pName.Cmp("cosh") == 0) lib = new GrapaLibraryRuleCosHEvent(pName);
            else if (pName.Cmp("tanh") == 0) lib = new GrapaLibraryRuleTanHEvent(pName);
            else if (pName.Cmp("coth") == 0) lib = new GrapaLibraryRuleCotHEvent(pName);
            else if (pName.Cmp("sech") == 0) lib = new GrapaLibraryRuleSecHEvent(pName);
            else if (pName.Cmp("csch") == 0) lib = new GrapaLibraryRuleCscHEvent(pName);

            else if (pName.Cmp("asinh") == 0) lib = new GrapaLibraryRuleASinHEvent(pName);
            else if (pName.Cmp("acosh") == 0) lib = new GrapaLibraryRuleACosHEvent(pName);
            else if (pName.Cmp("atanh") == 0) lib = new GrapaLibraryRuleATanHEvent(pName);
            else if (pName.Cmp("acoth") == 0) lib = new GrapaLibraryRuleACotHEvent(pName);
            else if (pName.Cmp("asech") == 0) lib = new GrapaLibraryRuleASecHEvent(pName);
            else if (pName.Cmp("acsch") == 0) lib = new GrapaLibraryRuleACscHEvent(pName);

            else if (pName.Cmp("atan2") == 0) lib = new GrapaLibraryRuleATan2Event(pName);
            else if (pName.Cmp("hypot") == 0) lib = new GrapaLibraryRuleHypotEvent(pName);

			else if (pName.Cmp("bsl") == 0) lib = new GrapaLibraryRuleBslEvent(pName);
            else if (pName.Cmp("bsr") == 0) lib = new GrapaLibraryRuleBsrEvent(pName);
            else if (pName.Cmp("bor") == 0) lib = new GrapaLibraryRuleBorEvent(pName);
            else if (pName.Cmp("band") == 0) lib = new GrapaLibraryRuleBandEvent(pName);
            else if (pName.Cmp("or") == 0) lib = new GrapaLibraryRuleOrEvent(pName);
            else if (pName.Cmp("and") == 0) lib = new GrapaLibraryRuleAndEvent(pName);
            else if (pName.Cmp("xor") == 0) lib = new GrapaLibraryRuleXOrEvent(pName);
			else if (pName.Cmp("inv") == 0) lib = new GrapaLibraryRuleInvEvent(pName);
			else if (pName.Cmp("t") == 0) lib = new GrapaLibraryRuleTransposeEvent(pName);
			else if (pName.Cmp("rref") == 0) lib = new GrapaLibraryRuleRrefEvent(pName);
			else if (pName.Cmp("det") == 0) lib = new GrapaLibraryRuleDetEvent(pName);
			else if (pName.Cmp("rank") == 0) lib = new GrapaLibraryRuleRankEvent(pName);
			else if (pName.Cmp("solve") == 0) lib = new GrapaLibraryRuleSolveEvent(pName);
			else if (pName.Cmp("cov") == 0) lib = new GrapaLibraryRuleCovEvent(pName);
			else if (pName.Cmp("neg") == 0) lib = new GrapaLibraryRuleNegEvent(pName);
            else if (pName.Cmp("bits") == 0) lib = new GrapaLibraryRuleBitsEvent(pName);
            else if (pName.Cmp("bytes") == 0) lib = new GrapaLibraryRuleBytesEvent(pName);
            else if (pName.Cmp("len") == 0) lib = new GrapaLibraryRuleLenEvent(pName);

            else if (pName.Cmp("bool") == 0) lib = new GrapaLibraryRuleBoolEvent(pName);
            else if (pName.Cmp("not") == 0) lib = new GrapaLibraryRuleNotEvent(pName);
            else if (pName.Cmp("int") == 0) lib = new GrapaLibraryRuleIntEvent(pName);
			else if (pName.Cmp("raw") == 0) lib = new GrapaLibraryRuleRawEvent(pName);
			else if (pName.Cmp("base") == 0) lib = new GrapaLibraryRuleBaseEvent(pName);
			else if (pName.Cmp("str") == 0) lib = new GrapaLibraryRuleStrEvent(pName);
			else if (pName.Cmp("list") == 0) lib = new GrapaLibraryRuleListEvent(pName);
			else if (pName.Cmp("array") == 0) lib = new GrapaLibraryRuleArrayEvent(pName);
			else if (pName.Cmp("vector") == 0) lib = new GrapaLibraryRuleVectorEvent(pName);
			else if (pName.Cmp("xml") == 0) lib = new GrapaLibraryRuleXmlEvent(pName);
			else if (pName.Cmp("float") == 0) lib = new GrapaLibraryRuleFloatEvent(pName);
            else if (pName.Cmp("fix") == 0) lib = new GrapaLibraryRuleFloatEvent(pName);
			else if (pName.Cmp("time") == 0) lib = new GrapaLibraryRuleTimeEvent(pName);
			else if (pName.Cmp("type") == 0) lib = new GrapaLibraryRuleTypeEvent(pName);
			else if (pName.Cmp("describe") == 0) lib = new GrapaLibraryRuleDescribeEvent(pName);

            else if (pName.Cmp("left") == 0) lib = new GrapaLibraryRuleLeftEvent(pName);
            else if (pName.Cmp("right") == 0) lib = new GrapaLibraryRuleRightEvent(pName);
			else if (pName.Cmp("mid") == 0) lib = new GrapaLibraryRuleMidEvent(pName);
			else if (pName.Cmp("midtrim") == 0) lib = new GrapaLibraryRuleMidTrimEvent(pName);
			else if (pName.Cmp("rtrim") == 0) lib = new GrapaLibraryRuleRTrimEvent(pName);
            else if (pName.Cmp("ltrim") == 0) lib = new GrapaLibraryRuleLTrimEvent(pName);
			else if (pName.Cmp("trim") == 0) lib = new GrapaLibraryRuleTrimEvent(pName);
			else if (pName.Cmp("rrot") == 0) lib = new GrapaLibraryRuleRRotateEvent(pName);
			else if (pName.Cmp("lrot") == 0) lib = new GrapaLibraryRuleLRotateEvent(pName);
			else if (pName.Cmp("rpad") == 0) lib = new GrapaLibraryRuleRPadEvent(pName);
			else if (pName.Cmp("lpad") == 0) lib = new GrapaLibraryRuleLPadEvent(pName);
			else if (pName.Cmp("reverse") == 0) lib = new GrapaLibraryRuleReverseEvent(pName);
			else if (pName.Cmp("replace") == 0) lib = new GrapaLibraryRuleReplaceEvent(pName);
			else if (pName.Cmp("split") == 0) lib = new GrapaLibraryRuleSplitEvent(pName);
			else if (pName.Cmp("join") == 0) lib = new GrapaLibraryRuleJoinEvent(pName);
			else if (pName.Cmp("shape") == 0) lib = new GrapaLibraryRuleShapeEvent(pName);
			else if (pName.Cmp("reshape") == 0) lib = new GrapaLibraryRuleReShapeEvent(pName);
			else if (pName.Cmp("dot") == 0) lib = new GrapaLibraryRuleDotEvent(pName);
			else if (pName.Cmp("identity") == 0) lib = new GrapaLibraryRuleIdentityEvent(pName);
			else if (pName.Cmp("diagonal") == 0) lib = new GrapaLibraryRuleDiagonalEvent(pName);
			else if (pName.Cmp("triu") == 0) lib = new GrapaLibraryRuleTriUEvent(pName);
			else if (pName.Cmp("tril") == 0) lib = new GrapaLibraryRuleTriLEvent(pName);
			else if (pName.Cmp("eigh") == 0) lib = new GrapaLibraryRuleEigHEvent(pName);
			else if (pName.Cmp("sum") == 0) lib = new GrapaLibraryRuleSumEvent(pName);
			else if (pName.Cmp("mean") == 0) lib = new GrapaLibraryRuleMeanEvent(pName);
			else if (pName.Cmp("lower") == 0) lib = new GrapaLibraryRuleLowerEvent(pName);
            else if (pName.Cmp("upper") == 0) lib = new GrapaLibraryRuleUpperEvent(pName);

            else if (pName.Cmp("eq") == 0) lib = new GrapaLibraryRuleEqEvent(pName);
            else if (pName.Cmp("neq") == 0) lib = new GrapaLibraryRuleNEqEvent(pName);
            else if (pName.Cmp("gteq") == 0) lib = new GrapaLibraryRuleGtEqEvent(pName);
            else if (pName.Cmp("gt") == 0) lib = new GrapaLibraryRuleGtEvent(pName);
            else if (pName.Cmp("lteq") == 0) lib = new GrapaLibraryRuleLtEqEvent(pName);
			else if (pName.Cmp("lt") == 0) lib = new GrapaLibraryRuleLtEvent(pName);
			else if (pName.Cmp("cmp") == 0) lib = new GrapaLibraryRuleCmpEvent(pName);

			else if (pName.Cmp("utc") == 0) lib = new GrapaLibraryRuleUtcEvent(pName);
			else if (pName.Cmp("tz") == 0) lib = new GrapaLibraryRuleTzEvent(pName);
		}
		if (lib == NULL)
		{
			if (pName.Cmp("file_table") == 0) lib = new GrapaLibraryRuleTableEvent(pName);
			else if (pName.Cmp("file_pwd") == 0) lib = new GrapaLibraryRulePwdEvent(pName);
			else if (pName.Cmp("file_cd") == 0) lib = new GrapaLibraryRuleCdEvent(pName);
			else if (pName.Cmp("file_phd") == 0) lib = new GrapaLibraryRulePhdEvent(pName);
			else if (pName.Cmp("file_chd") == 0) lib = new GrapaLibraryRuleChdEvent(pName);
			else if (pName.Cmp("file_ls") == 0) lib = new GrapaLibraryRuleLsEvent(pName);
			else if (pName.Cmp("file_mk") == 0) lib = new GrapaLibraryRuleMkEvent(pName);
			else if (pName.Cmp("file_rm") == 0) lib = new GrapaLibraryRuleRmEvent(pName);
			else if (pName.Cmp("file_mkfield") == 0) lib = new GrapaLibraryRuleMkFieldEvent(pName);
			else if (pName.Cmp("file_rmfield") == 0) lib = new GrapaLibraryRuleRmFieldEvent(pName);
			else if (pName.Cmp("file_info") == 0) lib = new GrapaLibraryRuleInfoEvent(pName);
			else if (pName.Cmp("file_set") == 0) lib = new GrapaLibraryRuleSetEvent(pName);
			else if (pName.Cmp("file_get") == 0) lib = new GrapaLibraryRuleGetEvent(pName);
			else if (pName.Cmp("file_split") == 0) lib = new GrapaLibraryRuleFileSplitEvent(pName);
			//else if (pName.Cmp("cp") == 0) lib = new GrapaLibraryRuleCpEvent(pName);
			//else if (pName.Cmp("setop") == 0) lib = new GrapaLibraryRuleSetOpEvent(pName);
			//else if (pName.Cmp("getop") == 0) lib = new GrapaLibraryRuleGetOpEvent(pName);
			//else if (pName.Cmp("runop") == 0) lib = new GrapaLibraryRuleRunOpEvent(pName);
			else if (pName.Cmp("file_debug") == 0) lib = new GrapaLibraryRuleDebugEvent(pName);
		}
		if (lib == NULL)
		{
			if (pName.Cmp("net_mac") == 0) lib = new GrapaLibraryRuleMacEvent(pName);
			else if (pName.Cmp("net_interfaces") == 0) lib = new GrapaLibraryRuleInterfacesEvent(pName);
			else if (pName.Cmp("net_connect") == 0) lib = new GrapaLibraryRuleConnectEvent(pName);
			else if (pName.Cmp("net_bind") == 0) lib = new GrapaLibraryRuleBindEvent(pName);
			else if (pName.Cmp("net_listen") == 0) lib = new GrapaLibraryRuleListenEvent(pName);
			else if (pName.Cmp("net_onlisten") == 0) lib = new GrapaLibraryRuleOnListenEvent(pName);
			else if (pName.Cmp("net_disconnect") == 0) lib = new GrapaLibraryRuleDisconnectEvent(pName);
			else if (pName.Cmp("net_proxy") == 0) lib = new GrapaLibraryRuleProxyEvent(pName);
			else if (pName.Cmp("net_certificate") == 0) lib = new GrapaLibraryRuleCertificateEvent(pName);
			else if (pName.Cmp("net_private") == 0) lib = new GrapaLibraryRulePrivateEvent(pName);
			else if (pName.Cmp("net_trusted") == 0) lib = new GrapaLibraryRuleTrustedEvent(pName);
			else if (pName.Cmp("net_verify") == 0) lib = new GrapaLibraryRuleNetVerifyEvent(pName);
			else if (pName.Cmp("net_chain") == 0) lib = new GrapaLibraryRuleNetChainEvent(pName);
			else if (pName.Cmp("net_host") == 0) lib = new GrapaLibraryRuleNetHostEvent(pName);
			else if (pName.Cmp("net_send") == 0) lib = new GrapaLibraryRuleSendEvent(pName);
			else if (pName.Cmp("net_receive") == 0) lib = new GrapaLibraryRuleReceiveEvent(pName);
			else if (pName.Cmp("net_pending") == 0) lib = new GrapaLibraryRulePendingEvent(pName);
			else if (pName.Cmp("net_onreceive") == 0) lib = new GrapaLibraryRuleOnReceiveEvent(pName);
		}
		if (lib == NULL)
		{
			if (pName.Cmp("http_read") == 0) lib = new GrapaLibraryRuleHttpReadEvent(pName);
			if (pName.Cmp("http_send") == 0) lib = new GrapaLibraryRuleHttpSendEvent(pName);
			if (pName.Cmp("http_message") == 0) lib = new GrapaLibraryRuleHttpMessageEvent(pName);
		}
		if (lib == NULL)
        {
            if (pName.Cmp("thread_trylock") == 0) lib = new GrapaLibraryRuleTryLockEvent(pName);
            else if (pName.Cmp("thread_lock") == 0) lib = new GrapaLibraryRuleLockEvent(pName);
            else if (pName.Cmp("thread_unlock") == 0) lib = new GrapaLibraryRuleUnLockEvent(pName);
            else if (pName.Cmp("thread_wait") == 0) lib = new GrapaLibraryRuleWaitEvent(pName);
            else if (pName.Cmp("thread_signal") == 0) lib = new GrapaLibraryRuleSignalEvent(pName);
            else if (pName.Cmp("thread_waiting") == 0) lib = new GrapaLibraryRuleWaitingEvent(pName);
            else if (pName.Cmp("thread_start") == 0) lib = new GrapaLibraryRuleStartEvent(pName);
            else if (pName.Cmp("thread_stop") == 0) lib = new GrapaLibraryRuleStopEvent(pName);
            else if (pName.Cmp("thread_started") == 0) lib = new GrapaLibraryRuleStartedEvent(pName);
            else if (pName.Cmp("thread_suspend") == 0) lib = new GrapaLibraryRuleSuspendEvent(pName);
            else if (pName.Cmp("thread_resume") == 0) lib = new GrapaLibraryRuleResumeEvent(pName);
            else if (pName.Cmp("thread_suspended") == 0) lib = new GrapaLibraryRuleSuspendedEvent(pName);
        }
		if (lib == NULL)
		{
			if (pName.Cmp("widget_new") == 0) lib = new GrapaLibraryRuleWidgetNewEvent(pName);
			else if (pName.Cmp("widget_show") == 0) lib = new GrapaLibraryRuleWidgetShowEvent(pName);
			else if (pName.Cmp("widget_hide") == 0) lib = new GrapaLibraryRuleWidgetHideEvent(pName);
			else if (pName.Cmp("widget_redraw") == 0) lib = new GrapaLibraryRuleWidgetRedrawEvent(pName);
			else if (pName.Cmp("widget_resizable") == 0) lib = new GrapaLibraryRuleWidgetResizableEvent(pName);
			else if (pName.Cmp("widget_resize") == 0) lib = new GrapaLibraryRuleWidgetResizeEvent(pName);
			else if (pName.Cmp("widget_parent") == 0) lib = new GrapaLibraryRuleWidgetParentEvent(pName);
			else if (pName.Cmp("widget_child") == 0) lib = new GrapaLibraryRuleWidgetChildEvent(pName);
			else if (pName.Cmp("widget_next") == 0) lib = new GrapaLibraryRuleWidgetNextEvent(pName);
			else if (pName.Cmp("widget_focus") == 0) lib = new GrapaLibraryRuleWidgetFocusEvent(pName);
			else if (pName.Cmp("widget_get") == 0) lib = new GrapaLibraryRuleWidgetGetEvent(pName);
			else if (pName.Cmp("widget_set") == 0) lib = new GrapaLibraryRuleWidgetSetEvent(pName);
			else if (pName.Cmp("widget_handle") == 0) lib = new GrapaLibraryRuleWidgetHandleEvent(pName);
			else if (pName.Cmp("widget_callback") == 0) lib = new GrapaLibraryRuleWidgetCallbackEvent(pName);
			else if (pName.Cmp("widget_event_key") == 0) lib = new GrapaLibraryRuleWidgetEventKeyEvent(pName);
			else if (pName.Cmp("widget_append") == 0) lib = new GrapaLibraryRuleWidgetAppendEvent(pName);
			else if (pName.Cmp("widget_post") == 0) lib = new GrapaLibraryRuleWidgetPostEvent(pName);
			else if (pName.Cmp("widget_clear") == 0) lib = new GrapaLibraryRuleWidgetClearEvent(pName);
		}
    }
	return(lib);
	*/
}

GrapaRuleEvent* GrapaLibraryRuleEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	if (!pOperation) return NULL;
	LoadLibWrap(vScriptExec, pOperation);
	if (pOperation->vLibraryEvent == NULL) return NULL;
	return pOperation->vLibraryEvent->Run(vScriptExec, pNameSpace, pOperation, pInput);
}

GrapaRuleEvent* GrapaLibraryRuleEvent::Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent *pParam)
{
	LoadLibWrap(vScriptExec, pParam);
	if (pParam && pParam->vLibraryEvent) pOperation = pParam->vLibraryEvent->Optimize(vScriptExec, pNameSpace, pOperation, pParam->Next());
	return(pOperation);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleResetEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	vScriptExec->vScriptState->LoadSystem();
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleExitEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	gSystem->mStop = true;
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGetEnvEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	s64 idx;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		if (r1.vVal->mValue.Cmp("$PATH") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("PATH") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::ARRAY;
			result->vQueue = vScriptExec->CopyQueue(gSystem->mPath);
		}
		else if (r1.vVal->mValue.Cmp("$STATICLIB") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("STATICLIB") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::LIST;
			result->vQueue = vScriptExec->CopyQueue(gSystem->mStaticLib);
		}
		else if (r1.vVal->mValue.Cmp("$ARGCIN") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("ARGCIN") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mArgcin);
		}
		else if (r1.vVal->mValue.Cmp("$ARGV") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("ARGV") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::ARRAY;
			result->vQueue = vScriptExec->CopyQueue(gSystem->mArgv);
		}
		else if (r1.vVal->mValue.Cmp("$LIB") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("LIB") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mLibDir);
		}
		else if (r1.vVal->mValue.Cmp("$BIN") == 0 || (r1.vVal->mValue.mToken==GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("BIN") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mBinDir);
		}
		else if (r1.vVal->mValue.Cmp("$NAME") == 0 || (r1.vVal->mValue.mToken==GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("NAME") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mBinName);
		}
		else if (r1.vVal->mValue.Cmp("$WORK") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("WORK") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mWorkDir);
		}
		else if (r1.vVal->mValue.Cmp("$HOME") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("HOME") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mHomeDir);
		}
		else if (r1.vVal->mValue.Cmp("$TEMP") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("TEMP") == 0))
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), gSystem->mTempDir);
		}
		else if (r1.vVal->mValue.Cmp("$VERSION") == 0 || r1.vVal->mValue.Cmp("VERSION") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR("version"), gSystem->mVersion);
		}
		else if (r1.vVal->mValue.Cmp("$LICENCE") == 0 || r1.vVal->mValue.Cmp("LICENCE") == 0)
		{
			err = 0;
			if (gSystem->mStaticLib && gSystem->mStaticLib->mCount)
			{
				GrapaRuleEvent* ev = gSystem->mStaticLib->Search("LICENCE",idx);
				if (ev)
				{
					result = new GrapaRuleEvent(0, GrapaCHAR(), ev->mValue);
					result->mValue.mToken = GrapaTokenType::STR;
				}
			}
		}
		else if (r1.vVal->mValue.Cmp("$PLATFORM") == 0 || r1.vVal->mValue.Cmp("PLATFORM") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::LIST;
			result->vQueue = new GrapaRuleQueue();
			GrapaCHAR trueVal;
			trueVal = GrapaCHAR::SetBool(true);
#ifdef __APPLE__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__APPLE__"), GrapaInt(__APPLE__).getBytes()));
#endif
#ifdef _WIN32
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("_WIN32"), GrapaInt(_WIN32).getBytes()));
#endif
#ifdef _WIN64
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("_WIN64"), GrapaInt(_WIN64).getBytes()));
#endif
#ifdef __linux__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__linux__"), GrapaInt(__linux__).getBytes()));
#endif
#ifdef __ANDROID__ 
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__ANDROID__"), GrapaInt(__ANDROID__).getBytes()));
#endif
#ifdef _MSC_VER 
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("_MSC_VER"), GrapaInt(_MSC_VER).getBytes()));
#endif
#ifdef _MSC_FULL_VER 
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("_MSC_FULL_VER"), GrapaInt(_MSC_FULL_VER).getBytes()));
#endif
#ifdef __GNUC__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__GNUC__"), GrapaInt(__GNUC__).getBytes()));
#endif
#ifdef __GNUC_MINOR__ 
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__GNUC_MINOR__"), GrapaInt(__GNUC_MINOR__).getBytes()));
#endif
#ifdef __clang__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__clang__"), GrapaInt(__clang__).getBytes()));
#endif
#ifdef __MINGW32__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__MINGW32__"), GrapaInt(__MINGW32__).getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__MINGW32_MAJOR_VERSION"), GrapaInt(__MINGW32_MAJOR_VERSION).getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__MINGW32_MINOR_VERSION"), GrapaInt(__MINGW32_MINOR_VERSION).getBytes()));
#endif
#ifdef __MINGW64__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__MINGW64__"), GrapaInt(__MINGW64__).getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__MINGW64_VERSION_MAJOR "), GrapaInt(__MINGW64_VERSION_MAJOR).getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__MINGW64_VERSION_MINOR"), GrapaInt(__MINGW64_VERSION_MINOR).getBytes()));
#endif
#ifdef __i386__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__i386__"), GrapaInt(__i386__).getBytes()));
#endif
#ifdef __x86_64__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__x86_64__"), GrapaInt(__x86_64__).getBytes()));
#endif
#ifdef __arm__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__arm__"), GrapaInt(__arm__).getBytes()));
#endif
#ifdef __ARM_ARCH_5T__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__ARM_ARCH_5T__"), GrapaInt(__ARM_ARCH_5T__).getBytes()));
#endif
#ifdef __ARM_ARCH_7A__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__ARM_ARCH_7A__"), GrapaInt(__ARM_ARCH_7A__).getBytes()));
#endif
#ifdef __powerpc64__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__powerpc64__"), GrapaInt(__powerpc64__).getBytes()));
#endif
#ifdef __aarch64__
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("__aarch64__"), GrapaInt(__aarch64__).getBytes()));
#endif
		}
		else if (r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] != '$')
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR(getenv((char*)r1.vVal->mValue.mBytes)));
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}
 
GrapaRuleEvent* GrapaLibraryRulePutEnvEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal)
	{
		if (r1.vVal->mValue.Cmp("$PATH") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("PATH") == 0))
		{
			if (r2.vVal && (r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::LIST))
			{
				err = 0;
				if (gSystem->mPath)
				{
					gSystem->mPath->CLEAR();
					delete gSystem->mPath;
					gSystem->mPath = NULL;
				}
				gSystem->mPath = vScriptExec->CopyQueue(r2.vVal->vQueue);
			}
		}
		else if (r1.vVal->mValue.Cmp("$LIB") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("LIB") == 0))
		{
			err = 0;
			gSystem->mLibDir.FROM(r2.vVal? r2.vVal->mValue:GrapaCHAR());
		}
		else if (r1.vVal->mValue.Cmp("$BIN") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("BIN") == 0))
		{
			err = 0;
			gSystem->mBinDir.FROM(r2.vVal ? r2.vVal->mValue : GrapaCHAR());
		}
		else if (r1.vVal->mValue.Cmp("$NAME") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("NAME") == 0))
		{
			err = 0;
			gSystem->mBinName.FROM(r2.vVal ? r2.vVal->mValue : GrapaCHAR());
		}
		else if (r1.vVal->mValue.Cmp("$WORK") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("WORK") == 0))
		{
			gSystem->mWorkDir.FROM(r2.vVal ? r2.vVal->mValue : GrapaCHAR());
		}
		else if (r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] != '$')
		{
			err = 0;
#ifdef WIN32
			_putenv_s((char*)r1.vVal->mValue.mBytes, r2.vVal ? (char*)r2.vVal->mValue.mBytes : "");
#else
			setenv((char*)r1.vVal->mValue.mBytes, r2.vVal ? (char*)r2.vVal->mValue.mBytes : "", 1);
#endif
		}
	}
	return(result);

}

GrapaRuleEvent* GrapaLibraryRuleShellEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::STR)
	{
		GrapaCHAR r = gSystem->exec(r1.vVal->mValue);
		result = new GrapaRuleEvent(0, GrapaCHAR(), r);
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleEvalEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam cmdstr(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam paramstr(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam rulestr(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam profilestr(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	if (cmdstr.vVal && (cmdstr.vVal->mValue.mToken == GrapaTokenType::STR || cmdstr.vVal->mValue.mToken == GrapaTokenType::RAW))
	{
		if (GrapaRuleEvent* operation = vScriptExec->vScriptState->AddRuleOperation(pNameSpace->GetNameQueue(), "", ""))
		{
			GrapaRuleEvent* vLocals = new GrapaRuleEvent();
			vLocals->mValue.mToken = GrapaTokenType::LIST;
			vLocals->vQueue = new GrapaRuleQueue();

			GrapaRuleEvent* e = paramstr.vVal ? vScriptExec->CopyItem(paramstr.vVal, false, false) : NULL;
			GrapaRuleEvent* q = pNameSpace->GetNameQueue()->Tail();
			GrapaRuleEvent* op = q;
			while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
			if (e && e->mValue.mToken == GrapaTokenType::LIST && e->vQueue)
			{
				while (e->vQueue->Head())
					op->vQueue->PushTail(e->vQueue->PopHead());
			}
			if (e)
			{
				e->CLEAR();
				delete e;
			}
			GrapaRuleEvent* rulexx = NULL;
			if (rulestr.vVal && rulestr.vVal->mValue.mToken == GrapaTokenType::STR)
				rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, rulestr.vVal->mValue);
			GrapaCHAR profStr;
			if (profilestr.vVal && profilestr.vVal->mValue.mToken == GrapaTokenType::STR)
				profStr.FROM(profilestr.vVal->mValue);

			//GrapaScriptExec tokenExec;
			//tokenExec.vScriptState = vScriptExec->vScriptState;
			//tokenExec.vScriptState->WaitCritical();
			//GrapaScriptExec* saveTokenExec = tokenExec.vScriptState->vScriptExec;;
			//tokenExec.vScriptState->vScriptExec = &tokenExec;

			pNameSpace->GetNameQueue()->PushTail(vLocals);

			//result = tokenExec.Exec(pNameSpace, rulexx, 0, profStr, cmdstr.vVal->mValue);
			result = vScriptExec->Exec(pNameSpace, rulexx, 0, profStr, cmdstr.vVal->mValue);

			//tokenExec.vScriptState->vScriptExec = saveTokenExec;

			if (result && result->mValue.mToken == GrapaTokenType::PTR)
			{
				GrapaRuleEvent* old = result;
				result = vScriptExec->CopyItem(old);
				old->CLEAR();
				delete old;
			}
			pNameSpace->GetNameQueue()->PopEvent(vLocals);

			if (vLocals)
			{
				vLocals->CLEAR();
				delete vLocals;
			}

			if (pNameSpace->GetNameQueue()->PopEvent(operation))
			{
				operation->CLEAR();
				delete operation;
				operation = NULL;
			}

			//tokenExec.vScriptState->LeaveCritical();
		}
	}
	return(result);
}

class GrapaLibraryRuleExecEventResponse : public GrapaConsoleResponse
{
public:
	GrapaRuleEvent* cb;
	GrapaLibraryRuleExecEventResponse() : GrapaConsoleResponse() { cb = NULL; };
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
	{
		if (cb && vScriptExec && pNameSpace)
		{
			GrapaRuleQueue q;
			GrapaRuleEvent wid;
			wid.mName.FROM("data");
			wid.mValue.FROM((char*)sendbuf, sendbuflen);
			wid.vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$STR"));
			q.PushTail(&wid);
			GrapaRuleEvent* e = NULL;
			e = vScriptExec->ProcessPlan(pNameSpace, cb, &wid);
			if (e) delete e;
			wid.vClass = NULL;
			wid.vWidget = NULL;
			q.PopHead();
			q.CLEAR();
		}
	};
	virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
	{
	};
	virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
	{
	};
};

GrapaRuleEvent* GrapaLibraryRuleExecEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam cb(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::STR)
	{
		GrapaNames* ns = pNameSpace;
		while (ns && (ns->mSkip || ns->GetNameQueue()->mCount == 0))
			ns = ns->GetParrent();
		if (ns)
		{
			GrapaRuleEvent* plan = vScriptExec->Plan(ns, r1.vVal->mValue, NULL, 0, GrapaCHAR());
			GrapaRuleEvent* e = NULL;
			if (plan)
			{
				GrapaLibraryRuleExecEventResponse r;
				GrapaConsoleResponse* oldr = pNameSpace->GetResponse();
				if (cb.vVal && cb.vVal->mValue.mToken == GrapaTokenType::OP)
				{
					r.cb = cb.vVal;
					ns->SetResponse(&r);
				}
				bool callByObject = false;
				GrapaRuleQueue q;
				if (ns->GetNameQueue()->mCount >= 3)
				{
					GrapaRuleEvent* ne = ns->GetNameQueue()->Tail(1);
					while (ne && ne->vRulePointer) ne = ne->vRulePointer;
					if (ne == r1.vVal)
					{
						callByObject = true;
						q.PushTail(ns->GetNameQueue()->PopTail());
						q.PushTail(ns->GetNameQueue()->PopTail());
					}
				}
				result = vScriptExec->ProcessPlan(ns, plan);
				if (callByObject)
				{
					ns->GetNameQueue()->PushTail(q.PopTail());
					ns->GetNameQueue()->PushTail(q.PopTail());
				}
				if (cb.vVal && cb.vVal->mValue.mToken == GrapaTokenType::OP)
				{
					ns->SetResponse(oldr);
				}
			}
			else
				result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePostEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::STR)
	{
		vScriptExec->vScriptState->vConsoleSend->Send(vScriptExec, pNameSpace, r1.vVal->mValue.mBytes, r1.vVal->mValue.mLength);
		vScriptExec->vScriptState->vConsoleSend->Send(vScriptExec, pNameSpace, (u8*)"$\n", 2);
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSleepEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt rInt;
		rInt.FromBytes(r1.vVal->mValue);
#ifdef _THREAD_
		std::this_thread::sleep_for(std::chrono::milliseconds(rInt.GetItem(0)));
#endif
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTokenEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	u8 setTypeId = GrapaTokenType::START;
	bool setSuppress = false, setRun = false, setStart = false, setEnd = false, setEsc = false, setExit = false, setEmpty = false;
	if (GrapaRuleEvent* r1 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(0)))
	{
		if (r1->mValue.Cmp("SYM") == 0) setTypeId = GrapaTokenType::SYM;
		else if (r1->mValue.Cmp("BOOL") == 0) setTypeId = GrapaTokenType::BOOL;
		else if (r1->mValue.Cmp("INT") == 0) setTypeId = GrapaTokenType::INT;
		else if (r1->mValue.Cmp("FLOAT") == 0) setTypeId = GrapaTokenType::FLOAT;
		else if (r1->mValue.Cmp("ID") == 0) setTypeId = GrapaTokenType::ID;
		else if (r1->mValue.Cmp("STR") == 0) setTypeId = GrapaTokenType::STR;
		else if (r1->mValue.Cmp("SYSSYM") == 0) setTypeId = GrapaTokenType::SYSSYM;
		else if (r1->mValue.Cmp("SYSINT") == 0) setTypeId = GrapaTokenType::SYSINT;
		else if (r1->mValue.Cmp("SYSID") == 0) setTypeId = GrapaTokenType::SYSID;
		else if (r1->mValue.Cmp("SYSSTR") == 0) setTypeId = GrapaTokenType::SYSSTR;
		else if (r1->mValue.Cmp("OTHER") == 0) setTypeId = GrapaTokenType::OTHER;
		r1->CLEAR();
		delete r1;
	}
	GrapaCHAR name;
	if (GrapaRuleEvent* r2 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(1)))
	{
		if (r2->mValue.mLength)
		{
			if (r2->mValue.mToken == GrapaTokenType::SYSID) 
				name.Append("$");
			name.Append(r2->mValue);
		}
		r2->CLEAR();
		delete r2;
	}
	GrapaBYTE setValue;
	GrapaRuleEvent* r3 = NULL;
	if ((r3 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(2))))
		setValue.FROM(r3->mValue);
	if (GrapaRuleEvent* r4 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(3)))
	{
		if (GrapaRuleEvent* option = (GrapaRuleEvent*)(r4->vQueue ? r4->vQueue->Head() : NULL))
		{
			while (option)
			{
				if (option->mValue.Cmp("SKIP") == 0) setSuppress = true;
				else if (option->mValue.Cmp("RUN") == 0) setRun = true;
				else if (option->mValue.Cmp("START") == 0) setStart = true;
				else if (option->mValue.Cmp("END") == 0) setEnd = true;
				else if (option->mValue.Cmp("ESC") == 0) setEsc = true;
				else if (option->mValue.Cmp("EXIT") == 0) setExit = true;
				else if (option->mValue.Cmp("EMPTY") == 0) setEmpty = true;
				option = option->Next();
			}
		}
		r4->CLEAR();
		delete r4;
	}
	if (setTypeId != GrapaTokenType::START && name.mLength)
	{
		GrapaRuleEvent* token = vScriptExec->vScriptState->AddToken(setTypeId, (char*)name.mBytes, setSuppress, setValue);
		if (token)
		{
			if (setRun) token->mRun = true;
			if (setStart) token->mStart = true;
			if (setEnd) token->mEnd = true;
			if (setEsc) token->mEscape = true;
			if (setExit) token->mExit = true;
			if (setEmpty) token->mNull = true;
		}
	}
	else
	{
		result = new GrapaRuleEvent();
		u64 pTokenId = result->HashId(setValue, setTypeId);
		if (setValue.mLength && (setTypeId == GrapaTokenType::INT || setTypeId == GrapaTokenType::FLOAT))
		{
			GrapaInt a;
			a.FromString(setValue, 10);
			result->SetItem(pTokenId, name, a.getBytes());
		}
		else
			result->SetItem(pTokenId, name, setValue);
		result->mSkip = setSuppress;
		result->mValue.mToken = GrapaTokenType::TOKEN;
		result->mT = setTypeId;
		if (setRun) result->mRun = true;
		if (setStart) result->mStart = true;
		if (setEnd) result->mEnd = true;
		if (setEsc) result->mEscape = true;
		if (setExit) result->mExit = true;
		if (setEmpty) result->mNull = true;
	}
	if (r3) { r3->CLEAR(); delete r3; }

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRuleValEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	u8 setTypeId = GrapaTokenType::START;
	if (GrapaRuleEvent* p1 = pInput->Head(0))
	{
		if (GrapaRuleEvent* r1 = vScriptExec->ProcessPlan(pNameSpace, p1))
		{
			if (r1->mValue.Cmp("ID") == 0) setTypeId = GrapaTokenType::ID;
			else if (r1->mValue.Cmp("TIME") == 0) setTypeId = GrapaTokenType::TIME;
			else if (r1->mValue.Cmp("BOOL") == 0) setTypeId = GrapaTokenType::BOOL;
			else if (r1->mValue.Cmp("INT") == 0) setTypeId = GrapaTokenType::INT;
			else if (r1->mValue.Cmp("FLOAT") == 0) setTypeId = GrapaTokenType::FLOAT;
			else if (r1->mValue.Cmp("STR") == 0) setTypeId = GrapaTokenType::STR;
			else if (r1->mValue.Cmp("TABLE") == 0) setTypeId = GrapaTokenType::TABLE;
			else if (r1->mValue.Cmp("RAW") == 0) setTypeId = GrapaTokenType::RAW;
			else if (r1->mValue.Cmp("SYM") == 0) setTypeId = GrapaTokenType::SYM;
			else if (r1->mValue.Cmp("SYSSYM") == 0) setTypeId = GrapaTokenType::SYSSYM;
			else if (r1->mValue.Cmp("SYSINT") == 0) setTypeId = GrapaTokenType::SYSINT;
			else if (r1->mValue.Cmp("SYSID") == 0) setTypeId = GrapaTokenType::SYSID;
			else if (r1->mValue.Cmp("SYSSTR") == 0) setTypeId = GrapaTokenType::SYSSTR;
			else if (r1->mValue.Cmp("OTHER") == 0) setTypeId = GrapaTokenType::OTHER;
			r1->CLEAR();
			delete r1;
		}
	}
	if (setTypeId != GrapaTokenType::START)
	{
		result = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(1));
		if (result)
		{
			result->mName.SetLength(0);
			result->mValue.mToken = setTypeId;
		}
		else
		{
			result = new GrapaRuleEvent(setTypeId, 0, "");
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRuleRefEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	result = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(0));
	if (result)
	{
		if (result->mValue.mToken == GrapaTokenType::SYSID && result->mValue.mBytes && result->mValue.mBytes[0]!='$')
		{
			GrapaCHARFile s(result->mValue);
			result->mValue.FROM("$");
			result->mValue.Append(s);
		}
		result->mValue.mToken = GrapaTokenType::RULEREF;
	}
	else
	{
		result = new GrapaRuleEvent();
		result->SetNull();
		result->mValue.mToken = GrapaTokenType::RULEREF;
	}
	GrapaRuleEvent* p2 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(1));
	if (p2)
	{
		if (p2->mValue.mToken == GrapaTokenType::OP || p2->mValue.mToken == GrapaTokenType::CODE)
		{
			if (result->vQueue)
			{
				result->vQueue->CLEAR();
				delete result->vQueue;
			}
			result->vQueue = new GrapaRuleQueue();
			result->vQueue->PushTail(p2);
		}
		else
		{
			p2->CLEAR();
			delete p2;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRuleEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* rule = NULL;
	GrapaRuleEvent* operation = NULL;
	GrapaRuleEvent* parameter = NULL;
	GrapaCHAR name;
	u64 ruleDelCount = 0;
	if (GrapaRuleEvent* r1 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(0)))
	{
		if (r1->mValue.mLength)
		{
			if (r1->mValue.mToken == GrapaTokenType::SYSID) name.Append("$");
			name.Append(r1->mValue);
		}
		r1->CLEAR();
		delete r1;
	}
	if (name.mLength)
	{
		rule = vScriptExec->vScriptState->AddRule(&vScriptExec->vScriptState->mRuleStartQueue, (char*)name.mBytes);
		ruleDelCount = rule->vQueue->mCount;
	}
	else
	{
		rule = new GrapaRuleEvent();
		rule->vQueue = new GrapaRuleQueue();
		rule->mValue.mToken = GrapaTokenType::RULE;
		result = rule;
	}
	if (rule)
	{
		if (GrapaRuleEvent* r2 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(1)))
		{
			GrapaRuleEvent* option = (GrapaRuleEvent*)(r2->vQueue ? r2->vQueue->Head() : NULL);
			while (option)
			{
				//if (option->vQueue)
				{
					name.SetLength(0);
					GrapaRuleEvent* actions = (GrapaRuleEvent*)(option->vQueue?option->vQueue->Head(1):NULL);
					if (actions && actions->vQueue)
					{
						GrapaRuleEvent *v = (GrapaRuleEvent*)actions->vQueue->Head();
						name.FROM("$SYS");
						switch (actions->vQueue->mCount)
						{
						case 2:
							v = (GrapaRuleEvent*)actions->vQueue->Tail();
							if (v && v->mValue.mToken != GrapaTokenType::LIST && !v->mNull)
							{
								v = (GrapaRuleEvent*)actions->vQueue->Head();
								if (v)
								{
									if (v->mValue.mToken == GrapaTokenType::SYSID)
										name.Append('$');
									name.Append((char*)v->mValue.mBytes);
								}
							}
							break;
						case 3:
							v = (GrapaRuleEvent*)pOperation->vQueue->Head();
							if (v)
							{
								if (v->mValue.mToken == GrapaTokenType::SYSID)
									name.Append('$');
								name.Append((char*)v->mValue.mBytes);
							}
							break;
						}
					}
					operation = vScriptExec->vScriptState->AddRuleOperation((GrapaRuleQueue*)rule->vQueue, (char*)name.mBytes, "");
					if (actions && actions->mValue.mToken == GrapaTokenType::OP)
						operation->vRuleLambda = vScriptExec->CopyItem(actions);
				}
				if (operation)
				{
					GrapaRuleEvent* items = (GrapaRuleEvent*)(option->vQueue?option->vQueue->Head(0):NULL);
					if (items && items->vQueue)
					{
						GrapaObjectEvent* item = (GrapaObjectEvent*)items->vQueue->Head();
						while (item)
						{
							switch (item->mValue.mToken)
							{
							case GrapaTokenType::RULEREF:
								parameter = vScriptExec->vScriptState->AddRuleParameter(operation, (char*)item->mValue.mBytes);
								if (parameter->vQueue)
								{
									parameter->vQueue->CLEAR();
									delete parameter->vQueue;
								}
								parameter->vQueue = NULL;
								if (item->vQueue) parameter->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)item->vQueue);
								break;
							case GrapaTokenType::RULE:
								parameter = vScriptExec->vScriptState->AddRawParameter(operation, GrapaCHAR(), item->mValue);
								if (item->vQueue)
									parameter->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)item->vQueue);
								break;
							case GrapaTokenType::MAX:
								parameter = vScriptExec->vScriptState->AddTokenParameter(operation, (char*)item->mValue.mBytes);
								if (parameter == NULL)
									parameter = vScriptExec->vScriptState->AddRawParameter(operation, (u8)item->mId, (char*)"", (char*)item->mValue.mBytes);
								break;
							default:
								parameter = vScriptExec->vScriptState->AddRawParameter(operation, GrapaCHAR(), item->mValue);
								if (item->vQueue)
									parameter->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)item->vQueue);
								break;
							}
							item = item->Next();
						}
					}
				}
				option = option->Next();
			}
			r2->CLEAR();
			delete r2;
		}
	}
	if (rule && ruleDelCount)
	{
		while (ruleDelCount--)
		{
			operation = (GrapaRuleEvent*)rule->vQueue->PopHead();
			if (operation)
			{
				operation->CLEAR();
				delete operation;
			}
		}
	}

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGrammarEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;

	result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");

	result->mValue.SetSize(30000);
	result->mValue.SetLength(0);

	GrapaRuleEvent* token;
	//token = vScriptExec->vScriptState->mItemState.mItemParams->mMapQueue.Head();
	token = vScriptExec->vScriptState->GetNameSpace()->GetNameQueue()->Head();
	if (token && token->vQueue)
		token = token->vQueue->Head();
	while (token)
	{
		if (token->mName.mLength && token->mValue.mToken == GrapaTokenType::TOKEN)
		{
			GrapaSystemSend send;
			send.isActive = false;
			vScriptExec->EchoTokenValue(&send, token, false, true);
			GrapaCHAR s;
			send.GetStr(s);
			result->mValue.GrapaCHAR::Append(s);

			//result->mValue.GrapaCHAR::Append("$TOKEN ");
			//result->mValue.GrapaCHAR::Append(token->mName);
			//result->mValue.GrapaCHAR::Append(" = ");
			//switch (token->mValue.mToken)
			//{
			//case GrapaTokenType::ERR:   result->mValue.GrapaCHAR::Append("$ERR"); break;
			//case GrapaTokenType::RAW:   result->mValue.GrapaCHAR::Append("$RAW"); break;
			//case GrapaTokenType::BOOL: result->mValue.GrapaCHAR::Append("$BOOL"); break;
			//case GrapaTokenType::INT: result->mValue.GrapaCHAR::Append("$INT"); break;
			//case GrapaTokenType::FLOAT: result->mValue.GrapaCHAR::Append("$FLOAT"); break;
			//case GrapaTokenType::STR:   result->mValue.GrapaCHAR::Append("$STR"); break;
			//case GrapaTokenType::TIME:  result->mValue.GrapaCHAR::Append("$TIME"); break;
			//case GrapaTokenType::ARRAY: result->mValue.GrapaCHAR::Append("$ARRAY "); break;
			//case GrapaTokenType::TUPLE: result->mValue.GrapaCHAR::Append("$TUPLE "); break;
			//case GrapaTokenType::LIST: result->mValue.GrapaCHAR::Append("$LIST "); break;
			//case GrapaTokenType::EL: result->mValue.GrapaCHAR::Append("$EL "); break;
			//case GrapaTokenType::TAG: result->mValue.GrapaCHAR::Append("$TAG "); break;
			//case GrapaTokenType::OP: result->mValue.GrapaCHAR::Append("$OP "); break;
			//case GrapaTokenType::CODE: result->mValue.GrapaCHAR::Append("$CODE "); break;
			//case GrapaTokenType::TABLE:   result->mValue.GrapaCHAR::Append("$DB"); break;
			//case GrapaTokenType::ID:  result->mValue.GrapaCHAR::Append("$ID"); break;
			//case GrapaTokenType::REF:  result->mValue.GrapaCHAR::Append("$REF"); break;
			//case GrapaTokenType::SYM: result->mValue.GrapaCHAR::Append("$SYM"); break;
			//case GrapaTokenType::SYSSYM: result->mValue.GrapaCHAR::Append("$SYSSYM"); break;
			//case GrapaTokenType::SYSINT: result->mValue.GrapaCHAR::Append("$SYSINT"); break;
			//case GrapaTokenType::SYSID: result->mValue.GrapaCHAR::Append("$SYSID"); break;
			//case GrapaTokenType::SYSSTR: result->mValue.GrapaCHAR::Append("$SYSSTR"); break;
			//case GrapaTokenType::OTHER: result->mValue.GrapaCHAR::Append("$OTHER"); break;
			//case GrapaTokenType::RULE: result->mValue.GrapaCHAR::Append("$RULE"); break;
			//case GrapaTokenType::PTR: result->mValue.GrapaCHAR::Append("$PTR"); break;
			//default: result->mValue.GrapaCHAR::Append("????"); break;
			//}
			//result->mValue.GrapaCHAR::Append("(\"");
			//GrapaCHAR to;
			//vScriptExec->EscapeString(token->mValue, to);
			//result->mValue.GrapaCHAR::Append(to);
			//result->mValue.GrapaCHAR::Append("\"");
			//if (token->mSkip) result->mValue.GrapaCHAR::Append(" $SKIP");
			//if (token->mRun) result->mValue.GrapaCHAR::Append(" $RUN");
			//if (token->mStart) result->mValue.GrapaCHAR::Append(" $START");
			//if (token->mEnd) result->mValue.GrapaCHAR::Append(" $END"); 
			//if (token->mEscape) result->mValue.GrapaCHAR::Append(" $ESC"); 
			//if (token->mExit) result->mValue.GrapaCHAR::Append(" $EXIT"); 
			//if (token->mNull) result->mValue.GrapaCHAR::Append(" $EMPTY"); 
			//result->mValue.GrapaCHAR::Append(")");
			//result->mValue.GrapaCHAR::Append(";\r\n");
		}
		token = token->Next();
	}

	GrapaRuleEvent* rule;
	//rule = vScriptExec->vScriptState->mRuleStartQueue.Head();
	rule = vScriptExec->vScriptState->GetNameSpace()->GetNameQueue()->Head();
	if (rule&& rule->vQueue)
		rule = rule->vQueue->Head();
	while (rule)
	{
		if (rule->mName.mLength && rule->mValue.mToken== GrapaTokenType::RULE)
		{
			GrapaSystemSend send;
			send.isActive = false;
			vScriptExec->EchoRuleValue(&send, rule, false,true,false);
			GrapaCHAR s;
			send.GetStr(s);
			result->mValue.GrapaCHAR::Append(s);

			//result->mValue.GrapaCHAR::Append("$RULE ");
			//result->mValue.GrapaCHAR::Append("<");
			//result->mValue.GrapaCHAR::Append(rule->mName);
			//result->mValue.GrapaCHAR::Append(">");
			//result->mValue.GrapaCHAR::Append("\r\n");
			//GrapaRuleEvent* items = (GrapaRuleEvent*)rule->vQueue->Head();
			//if (items) result->mValue.GrapaCHAR::Append("\t=");
			//while (items)
			//{
			//	GrapaRuleEvent* params = (GrapaRuleEvent*)items->vQueue->Head();
			//	if (params)
			//	{
			//		while (params)
			//		{
			//			result->mValue.GrapaCHAR::Append(" ");
			//			if (params->mId)
			//			{
			//				if (params->mValue.mToken == GrapaTokenType::RULEREF)
			//				{
			//					//result->mValue.GrapaCHAR::Append("@");
			//					//result->mValue.GrapaCHAR::Append(params->mName);
			//					result->mValue.GrapaCHAR::Append("<");
			//					result->mValue.GrapaCHAR::Append(params->mName);
			//					if (params->vQueue && params->vQueue->mCount)
			//					{
			//						GrapaSystemSend send;
			//						send.isActive = false;
			//						vScriptExec->EchoList(&send, (GrapaRuleEvent*)params->vQueue->Head(), false);
			//						GrapaCHAR s;
			//						send.GetStr(s);
			//						if (params->mName.mLength)
			//							result->mValue.GrapaCHAR::Append(" ");
			//						result->mValue.GrapaCHAR::Append("{");
			//						result->mValue.GrapaCHAR::Append(s);
			//						result->mValue.GrapaCHAR::Append("}");
			//					}
			//					result->mValue.GrapaCHAR::Append(">");

			//				}
			//				else
			//				{
			//					result->mValue.GrapaCHAR::Append(params->mName);
			//				}
			//				//if (params->mValue.mToken == GrapaTokenType::RULEREF) result->mValue.GrapaCHAR::Append(">");
			//			}
			//			else
			//			{
			//				if (params->mValue.mToken == GrapaTokenType::ID && params->mValue.mLength)
			//				{
			//					result->mValue.GrapaCHAR::Append(params->mValue);
			//				}
			//				else if (params->mValue.mToken == GrapaTokenType::SYSID && params->mValue.mLength)
			//				{
			//					result->mValue.GrapaCHAR::Append("$SYSID(\"");
			//					result->mValue.GrapaCHAR::Append(params->mValue);
			//					result->mValue.GrapaCHAR::Append("\")");
			//				}
			//				else if (params->mValue.mToken == GrapaTokenType::REF && params->mValue.mLength)
			//				{
			//					result->mValue.GrapaCHAR::Append("@");
			//					result->mValue.GrapaCHAR::Append(params->mValue);
			//				}
			//				else if (params->mValue.mToken == GrapaTokenType::SYM && params->mValue.mLength)
			//				{
			//					result->mValue.GrapaCHAR::Append("'");
			//					result->mValue.GrapaCHAR::Append(params->mValue);
			//					result->mValue.GrapaCHAR::Append("'");
			//				}
			//				else if (params->mValue.mToken == GrapaTokenType::INT && params->mValue.mLength)
			//				{
			//					GrapaInt i;
			//					i.FromBytes(params->mValue);
			//					result->mValue.GrapaCHAR::Append(i.ToString());
			//				}
			//				else if (params->mValue.mToken == GrapaTokenType::OP)
			//				{
			//					GrapaSystemSend send;
			//					send.isActive = false;
			//					vScriptExec->EchoList(&send, params, false);
			//					GrapaCHAR s;
			//					send.GetStr(s);
			//					result->mValue.GrapaCHAR::Append(" <");
			//					result->mValue.GrapaCHAR::Append(s);
			//					result->mValue.GrapaCHAR::Append(">");
			//				}
			//				else if (params->mValue.mToken == GrapaTokenType::FLOAT && params->mValue.mLength)
			//				{
			//					GrapaFloat i(0);
			//					i.FromBytes(params->mValue);
			//					result->mValue.GrapaCHAR::Append(i.ToString());
			//				}
			//				else
			//				{
			//					switch (params->mValue.mToken)
			//					{
			//					case GrapaTokenType::ERR:   result->mValue.GrapaCHAR::Append("$ERR"); break;
			//					case GrapaTokenType::RAW:   result->mValue.GrapaCHAR::Append("$RAW"); break;
			//					case GrapaTokenType::BOOL:  result->mValue.GrapaCHAR::Append("$BOOL"); break;
			//					case GrapaTokenType::ID:  result->mValue.GrapaCHAR::Append("$ID"); break;
			//					case GrapaTokenType::INT: result->mValue.GrapaCHAR::Append("$INT"); break;
			//					case GrapaTokenType::FLOAT: result->mValue.GrapaCHAR::Append("$FLOAT"); break;
			//					case GrapaTokenType::STR:   result->mValue.GrapaCHAR::Append("$STR"); break;
			//					case GrapaTokenType::TIME:  result->mValue.GrapaCHAR::Append("$TIME"); break;
			//					case GrapaTokenType::ARRAY: result->mValue.GrapaCHAR::Append("$ARRAY"); break;
			//					case GrapaTokenType::TUPLE: result->mValue.GrapaCHAR::Append("$TUPLE"); break;
			//					case GrapaTokenType::LIST: result->mValue.GrapaCHAR::Append("$LIST"); break;
			//					case GrapaTokenType::EL: result->mValue.GrapaCHAR::Append("$EL"); break;
			//					case GrapaTokenType::TAG: result->mValue.GrapaCHAR::Append("$TAG"); break;
			//					case GrapaTokenType::OP: result->mValue.GrapaCHAR::Append("$OP"); break;
			//					case GrapaTokenType::CODE: result->mValue.GrapaCHAR::Append("$CODE"); break;
			//					case GrapaTokenType::TABLE:   result->mValue.GrapaCHAR::Append("$DB"); break;
			//					case GrapaTokenType::SYM: result->mValue.GrapaCHAR::Append("$SYM"); break;
			//					case GrapaTokenType::SYSSYM: result->mValue.GrapaCHAR::Append("$SYSSYM"); break;
			//					case GrapaTokenType::SYSINT: result->mValue.GrapaCHAR::Append("$SYSINT"); break;
			//					case GrapaTokenType::SYSID: result->mValue.GrapaCHAR::Append("$SYSID"); break;
			//					case GrapaTokenType::SYSSTR: result->mValue.GrapaCHAR::Append("$SYSSTR"); break;
			//					case GrapaTokenType::OTHER: result->mValue.GrapaCHAR::Append("$OTHER"); break;
			//					case GrapaTokenType::RULE: result->mValue.GrapaCHAR::Append("$RULE"); break;
			//					case GrapaTokenType::PTR: result->mValue.GrapaCHAR::Append("$PTR"); break;
			//					case GrapaTokenType::REF: result->mValue.GrapaCHAR::Append("$REF"); break;
			//					default: result->mValue.GrapaCHAR::Append("????"); break;
			//					}
			//					if (params->mValue.mLength)
			//					{
			//						result->mValue.GrapaCHAR::Append("(\"");
			//						result->mValue.GrapaCHAR::Append(params->mValue);
			//						result->mValue.GrapaCHAR::Append("\")");
			//					}
			//					//break;
			//				}
			//			}
			//			params = params->Next();
			//		}
			//	}
			//	if (items->vRuleLambda)
			//	{
			//		GrapaSystemSend send;
			//		send.isActive = false;
			//		vScriptExec->EchoList(&send, items->vRuleLambda, false);
			//		GrapaCHAR s;
			//		send.GetStr(s);
			//		result->mValue.GrapaCHAR::Append(" {");
			//		result->mValue.GrapaCHAR::Append(s);
			//		result->mValue.GrapaCHAR::Append("}");
			//	}
			//	else if (items->mName.mLength)
			//	{
			//		result->mValue.GrapaCHAR::Append(" {");
			//		result->mValue.GrapaCHAR::Append(items->mName);
			//		result->mValue.GrapaCHAR::Append(".");
			//		result->mValue.GrapaCHAR::Append(items->mValue);
			//		result->mValue.GrapaCHAR::Append("}");
			//	}
			//	result->mValue.GrapaCHAR::Append("\r\n");
			//	items = items->Next();
			//	if (items) result->mValue.GrapaCHAR::Append("\t|");
			//}
			//result->mValue.GrapaCHAR::Append("\t;\r\n");
		}
		rule = rule->Next();
	}
	token = vScriptExec->vScriptState->GetNameSpace()->GetNameQueue()->Head();
	if (token && token->vQueue)
		token = token->vQueue->Head();
	while (token)
	{
		if (token->mName.mLength && token->mValue.mToken == GrapaTokenType::CLASS)
		{
			GrapaSystemSend send;
			send.isActive = false;
			vScriptExec->EchoClassValue(&send, token, false, true);
			GrapaCHAR s;
			send.GetStr(s);
			result->mValue.GrapaCHAR::Append(s);
		}
		token = token->Next();
	}
	return(result);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleNameEvent::Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam)
{
	if (pParam && pParam->vQueue && pParam->vQueue->mCount)
	{
		GrapaRuleEvent *name = (GrapaRuleEvent*)pParam->vQueue->Head();
		GrapaRuleEvent *v = (GrapaRuleEvent*)pParam->vQueue->Head(1);
		if (name && (/*name->mValue.mToken == GrapaTokenType::STR ||*/ name->mValue.mToken == GrapaTokenType::SYSSTR))
		{
			v = (GrapaRuleEvent*)pParam->vQueue->PopHead(1);
			if (v) v->mName.FROM(name->mValue);
			pOperation->CLEAR();
			delete pOperation;
			pOperation = v;
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRuleNameEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam i1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam i2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	if (i2.vVal)
	{
		if (i2.vDel)
		{
			result = i2.vDel;
			i2.vDel = NULL;
		}
		else
		{
			result = vScriptExec->CopyItem(i2.vVal);
		}
		if (i1.vVal)
		{
			if (i1.vVal->mValue.mToken == GrapaTokenType::SYSID && (i1.vVal->mValue.mLength && i1.vVal->mValue.mBytes[0] != '$'))
			{
				result->mName.FROM("$");
				result->mName.Append(i1.vVal->mValue);
			}
			else
				result->mName.FROM(i1.vVal->mValue);

		}
	}
	else
	{
		GrapaCHAR name;
		result = new GrapaRuleEvent(0, i1.vVal ? i1.vVal->mValue : name, name);
	}

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGetNameEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID || r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR))
	{
		GrapaRuleEvent* parameter = NULL;
		GrapaCHAR valueName;
		s64 idx;
		if (r1.vVal->mValue.mToken == GrapaTokenType::SYSID || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR)
			valueName.FROM("$");
		valueName.Append(r1.vVal->mValue);
		//if (r1.vDel && r1.vDel->mVar)
		//{
		//	parameter = r1.vVal;
		//}
		//else
		{
			GrapaNames* names = pNameSpace->GetSkipThis();
			while (!parameter && names)
			{
				GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
				while (operation)
				{
					GrapaRuleEvent* op = operation;
					while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
					parameter = (GrapaRuleEvent*)(op->vQueue ? op->vQueue->Search(valueName,idx) : NULL);
					if (parameter)
						break;
					operation = operation->Prev();
				}
				names = names->GetSkipParrent();
			}
		}
		while (parameter && parameter->vRulePointer) parameter = parameter->vRulePointer;
		if (parameter && parameter->mValue.mToken == GrapaTokenType::TAG)
			parameter = parameter->vQueue->Head();
		if (parameter)
			result = new GrapaRuleEvent(0,GrapaCHAR(),parameter->mName);
	}
	else if (r1.vVal && r1.vVal->vQueue && (r1.vVal->mValue.mToken == GrapaTokenType::LIST|| r1.vVal->mValue.mToken == GrapaTokenType::TAG))
	{
		GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
		s64 index = 0;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt rInt;
			rInt.FromBytes(r2.vVal->mValue);
			if (rInt.IsNeg())
			{
				rInt = -rInt;
				index = -((s32)rInt.GetItem(0));
			}
			else
				index = rInt.GetItem(0);
		}
		GrapaRuleEvent* parameter = NULL;
		if (index>=0)
			parameter = r1.vVal->vQueue->Head(index);
		else
			parameter = r1.vVal->vQueue->Tail(-index-1L);
		while (parameter && parameter->vRulePointer) parameter = parameter->vRulePointer;
		if (parameter)
			result = new GrapaRuleEvent(0, GrapaCHAR(), parameter->mName);

	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLitEvent::Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam)
{
	if (pParam && pParam->vQueue && pParam->vQueue->mCount)
	{
		GrapaRuleEvent *v = (GrapaRuleEvent*)pParam->vQueue->Head();
		if (v->mValue.mToken != GrapaTokenType::OP && v->mValue.mToken != GrapaTokenType::CODE)
		{
			GrapaRuleEvent *v = (GrapaRuleEvent*)pParam->vQueue->PopHead();
			pOperation->CLEAR();
			delete pOperation;
			pOperation = v;
			pOperation->mName.SetLength(0);
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRuleLitEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam p1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL, false);
	if (p1.vVal)
	{
		switch (p1.vVal->mValue.mToken)
		{
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::VECTOR:
		case GrapaTokenType::WIDGET:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
			result = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
			result->mName.FROM(p1.vVal->mName);
			result->vRulePointer = p1.vVal;
			break;
		default:
			result = vScriptExec->CopyItem(p1.vVal);
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRefEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam p1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL, false);
	if (p1.vVal)
	{
		switch (p1.vVal->mValue.mToken)
		{
		case GrapaTokenType::ID:
			result = new GrapaRuleEvent();
			result->mValue.FROM(p1.vVal->mValue);
			result->mValue.mToken = GrapaTokenType::REF;
			break;
		case GrapaTokenType::SYSID:
			result = new GrapaRuleEvent();
			result->mValue.FROM("$");
			result->mValue.Append(p1.vVal->mValue);
			result->mValue.mToken = GrapaTokenType::REF;
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleNullEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	result = new GrapaRuleEvent();
	result->SetNull();
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleVarEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam i1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	if (i1.vVal == NULL)
		return(result);

	GrapaRuleEvent* parameter = NULL;

	switch (i1.vVal->mValue.mToken)
	{
	case GrapaTokenType::ID:
	case GrapaTokenType::STR:
	case GrapaTokenType::INT:
	case GrapaTokenType::SYSID:
	case GrapaTokenType::SYSSTR:
	case GrapaTokenType::SYSINT:
		parameter = vScriptExec->vScriptState->SearchVariable(pNameSpace, i1.vVal);
		if (parameter == NULL)
		{
			GrapaRuleEvent *n = pNameSpace->GetNameQueue()->Tail();
			while (n && n->vRulePointer == NULL) n = n->Prev();
			if (n == NULL) n = pNameSpace->GetNameQueue()->Head();
			while (n && n->vRulePointer) n = n->vRulePointer;
			if (n && n->vClass) n = n->vClass;
			parameter = vScriptExec->vScriptState->FindClassVar(pNameSpace, n, i1.vVal->mValue);
		}
		if (parameter)
		{
			result = new GrapaRuleEvent();
			//result->mName.FROM(parameter->mName);
			result->mValue.mToken = GrapaTokenType::PTR;
			result->mLocal = parameter->mLocal;
			while (parameter && parameter->mValue.mToken == GrapaTokenType::PTR) parameter = parameter->vRulePointer;
			result->vRulePointer = parameter;
			result->mVar = true;
			//result->mLocal = true;
		}
		break;
	default:
		result = vScriptExec->CopyItem(i1.vVal);
		break;
	}

	if (result == NULL)
	{
		//result = new GrapaRuleEvent(GrapaTokenType::ERR, 0, "", "");
		//result->vQueue = new GrapaRuleQueue();
		//result->mAbort = true;
		result = Error(vScriptExec, pNameSpace, -1);
	}
	return(result);
}

static bool ItemAssignCheck(GrapaRuleEvent* parameter, GrapaRuleEvent* r)
{
	if (!r) return(false);
	if (parameter == r) return(true);
	if (parameter == r->vRulePointer) return(true);
	if (r->vQueue) {
		GrapaRuleEvent* pr = ((GrapaRuleQueue*)r->vQueue)->Head();
		while (pr)
		{
			if (ItemAssignCheck(parameter,pr)) break;
			pr = pr->Next();
		}
	}
	return(true);
}

static GrapaRuleEvent* ItemAssignRun(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput, const GrapaCHAR& pName, GrapaError& err)
{
	err = 0;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ERR && r1.vVal->mVar && r1.vVal->vRulePointer && r1.vDel && r1.vDel->vClass)
	{
		r1.vDel->vRulePointer = vScriptExec->vScriptState->AddRawParameter(r1.vDel->vClass, r1.vVal->mName, GrapaBYTE());
		r1.vVal = r1.vDel->vRulePointer;
	}
	if (r1.vVal && r1.vVal->mClass && r1.vDel && r1.vDel->vClass == NULL)
	{
		err = -1;
		return result;
	}
	if (r1.vVal && r1.vVal->mClass && r1.vDel && r1.vDel->vClass)
	{
		r1.vVal = vScriptExec->vScriptState->AddRawParameter(r1.vDel->vRuleParent, r1.vVal->mName, GrapaBYTE());
	}

	if (r1.vVal)
	{
		GrapaRuleEvent* parameter = NULL;
		GrapaCHAR valueName;
		s64 idx;
		if (r1.vVal->mValue.mToken == GrapaTokenType::SYSID || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR)
			valueName.FROM("$");
		valueName.Append(r1.vVal->mValue);
		if (r1.vDel && r1.vDel->mVar)
		{
			parameter = r1.vVal;
			//parameter = r1.vDel;
		}
		else
		{
			GrapaNames* names = pNameSpace->GetSkipThis();
			while (!parameter && names)
			{
				GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
				//GrapaRuleEvent* operationStart = operation;
				while (operation)
				{
					GrapaRuleEvent* op = operation;
					while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
					parameter = (GrapaRuleEvent*)(op->vQueue?op->vQueue->Search(valueName,idx):NULL);
					if (parameter == NULL && op->mValue.mToken == GrapaTokenType::OBJ && op->vClass)
					{
						parameter = vScriptExec->vScriptState->FindClassVar(pNameSpace, op->vClass, valueName);
						if (parameter)
						{
							parameter = vScriptExec->CopyItem(parameter);
							op->vQueue->PushTail(parameter);
						}
					}
					if (parameter)
						break;
					operation = operation->Prev();
				}
				names = names->GetSkipParrent();
			}
			if (pNameSpace && parameter == NULL)
			{
				parameter = vScriptExec->vScriptState->AddRawParameter(pNameSpace->GetNameQueue()->Tail(), valueName, GrapaBYTE(""));
			}
		}
		while (parameter && parameter->vRulePointer) parameter = parameter->vRulePointer;
		GrapaRuleEvent* r = pInput->Head(1), *rDel = NULL;
		while (r && r->vRulePointer) r = r->vRulePointer;
		bool isAbort = r?r->mAbort:false;
		GrapaBYTE isName;
		if (r) isName.FROM(r->mName);
		if (r && (r->mValue.mToken == GrapaTokenType::OP))
		{
			r = rDel = vScriptExec->ProcessPlan(pNameSpace, r);
			if (r)
			{
				isAbort = r->mAbort;
				isName.FROM(r->mName);
			}
			if (rDel && rDel->mValue.mToken == GrapaTokenType::PTR)
			{
				if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") != 0 && GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") != 0)
				{
					GrapaRuleEvent* rold = rDel;
					r = rDel = vScriptExec->CopyItem(r);
					rold->CLEAR();
					delete rold;
				}
			}
		}
		while (r && r->mValue.mToken == GrapaTokenType::PTR && r->vRulePointer) r = r->vRulePointer;
		if (parameter && GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") != 0 && GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") != 0)
		{
			//if (!ItemAssignCheck(parameter, r))
			//	; // issue need to address when assigning something to itself, like a=@a or a={@a}
			if (!parameter->mConst)
			{
				//if (parameter->mValue.mToken == GrapaTokenType::TAG)
				//{
				//	parameter->vQueue->Head(0)->vQueue->Head(1)->vQueue->CLEAR();
				//}
				//else
				//{
					parameter->CLEAR();
				//}
			}
		}
		if (!r)
		{
			if (parameter)
			{
				//if (parameter->mValue.mToken == GrapaTokenType::TAG)
				//{
				//	parameter->vQueue->Head(0)->vQueue->Head(1)->vQueue->CLEAR();
				//}
				//else
				//{
					parameter->mNull = true;
					parameter->mValue.mToken = GrapaTokenType::SYSID;
				//}
			}
		}
		else
		{
			if (isAbort)
			{
				result = r;
			}
			else if (parameter && !parameter->mConst)
			{
				if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") == 0 || GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") == 0)
				{
					GrapaInt a, b;
					GrapaFloat n1(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
					GrapaFloat n2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
					if (parameter->mValue.mToken == GrapaTokenType::TAG)
					{
						parameter = parameter->vQueue->Head(0);
						if (parameter->vQueue->mCount == 0)
						{
							GrapaRuleEvent* ev = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
							ev->vQueue = new GrapaRuleQueue();
							ev->mValue.mToken = GrapaTokenType::LIST;
							parameter->vQueue->PushTail(ev);
						}
						if (parameter->vQueue->mCount == 1)
						{
							GrapaRuleEvent* ev = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
							ev->vQueue = new GrapaRuleQueue();
							ev->mValue.mToken = GrapaTokenType::XML;
							parameter->vQueue->PushTail(ev);
						}
						parameter = parameter->vQueue->Head(1);
					}
					switch (parameter->mValue.mToken)
					{
					case GrapaTokenType::STR:
						switch (r->mValue.mToken)
						{
						case GrapaTokenType::STR:
							parameter->mValue.GrapaCHAR::Append(r->mValue);
							break;
						case GrapaTokenType::INT:
							a.FromBytes(r->mValue);
							parameter->mValue.GrapaCHAR::Append(a.ToString());
							break;
						case GrapaTokenType::FLOAT:
							n1.FromBytes(r->mValue);
							parameter->mValue.GrapaCHAR::Append(n1.ToString());
							break;
						}
						break;
					case GrapaTokenType::INT:
						switch (r->mValue.mToken)
						{
						case GrapaTokenType::INT:
							a.FromBytes(parameter->mValue);
							b.FromBytes(r->mValue);
							parameter->mValue.FROM((a + b).getBytes());
							break;
						case GrapaTokenType::FLOAT:
							a.FromBytes(parameter->mValue);
							n1 = a;
							n2.FromBytes(r->mValue);
							parameter->mValue.FROM((n1 + n2).getBytes());
							break;
						}
						break;
					case GrapaTokenType::FLOAT:
						switch (r->mValue.mToken)
						{
						case GrapaTokenType::INT:
							n1.FromBytes(parameter->mValue);
							b.FromBytes(r->mValue);
							n2 = b;
							parameter->mValue.FROM((n1 + n2).getBytes());
							break;
						case GrapaTokenType::FLOAT:
							n1.FromBytes(parameter->mValue);
							n2.FromBytes(r->mValue);
							parameter->mValue.FROM((n1 + n2).getBytes());
							break;
						}
						break;
					case GrapaTokenType::ARRAY:
					case GrapaTokenType::TUPLE:
					case GrapaTokenType::LIST:
					case GrapaTokenType::XML:
					case GrapaTokenType::EL:
					case GrapaTokenType::TAG:
					case GrapaTokenType::OP:
					case GrapaTokenType::CODE:
					case GrapaTokenType::RULE:
					case GrapaTokenType::OBJ:
						if (parameter->vQueue)
						{
							//if (parameter->mValue.mToken == GrapaTokenType::TAG)
							//	parameter = parameter->vQueue->Head(0)->vQueue->Head(1);
							if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") == 0)
							{
								GrapaRuleEvent* at = pInput->Head(2), * atDel = NULL;
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;
								if (at && (at->mValue.mToken == GrapaTokenType::OP)) at = atDel = vScriptExec->ProcessPlan(pNameSpace, at);
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;

								if (r->mValue.mToken == GrapaTokenType::XML)
								{
									GrapaRuleEvent* w = r ? r->vQueue->Head() : NULL;
									while (w)
									{
										GrapaRuleEvent* w2 = w;
										while (w2 && w2->mValue.mToken == GrapaTokenType::PTR && w2->vRulePointer) w2 = w2->vRulePointer;
										GrapaRuleEvent* w3 = vScriptExec->CopyItem(w2);
										w3->mName.FROM(w->mName);
										if (at)
											parameter->vQueue->Push(w3, at);
										else
											parameter->vQueue->PushTail(w3);
										w = w->Next();
									}
								}
								else
								{
									GrapaRuleEvent* w = vScriptExec->CopyItem(r);
									w->mName.FROM(isName);
									if (at)
										parameter->vQueue->Push(w, at);
									else
										parameter->vQueue->PushTail(w);
								}
								if (atDel) { atDel->CLEAR(); delete atDel; }
							}
							else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") == 0 && r->vQueue)
							{
								GrapaRuleEvent* at = pInput->Head(2), * atDel = NULL;
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;
								if (at && (at->mValue.mToken == GrapaTokenType::OP)) at = atDel = vScriptExec->ProcessPlan(pNameSpace, at);
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;

								if (r->mValue.mToken == GrapaTokenType::TAG)
									r = r->vQueue->Head()->vQueue->Head(1);
								GrapaRuleEvent* w = r?r->vQueue->Head():NULL;
								while (w)
								{
									GrapaRuleEvent* w2 = w;
									while (w2 && w2->mValue.mToken == GrapaTokenType::PTR && w2->vRulePointer) w2 = w2->vRulePointer;
									GrapaRuleEvent* w3 = vScriptExec->CopyItem(w2);
									w3->mName.FROM(w->mName);
									if (at)
										parameter->vQueue->Push(w3, at);
									else
										parameter->vQueue->PushTail(w3);
									w = w->Next();
								}

								if (atDel) { atDel->CLEAR(); delete atDel; }
							}
						}
						break;
					case GrapaTokenType::VECTOR:
						switch (r->mValue.mToken)
						{
						case GrapaTokenType::VECTOR:
							if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") == 0)
							{
								parameter->vVector->Extend(*r->vVector);
							}
							break;
						}
						break;
					case GrapaTokenType::WIDGET:
						switch (r->mValue.mToken)
						{
						case GrapaTokenType::WIDGET:
							if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") == 0)
							{
								GrapaRuleEvent* at = pInput->Head(2), * atDel = NULL;
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;
								if (at && (at->mValue.mToken == GrapaTokenType::OP)) at = atDel = vScriptExec->ProcessPlan(pNameSpace, at);
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;
								if (at && at->IsNull()) at = NULL;
								parameter->vWidget->Add(r->mName, r->vWidget, at?at->vWidget:NULL);
								if (rDel && rDel->vWidget == r->vWidget)
								{
									r->vWidget->mDelete = true;
									rDel->vWidget = NULL;
								}
								if (atDel) { atDel->CLEAR(); delete atDel; }
							}
							else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") == 0)
							{
								parameter->vWidget->Extend(r->vWidget);
							}
							break;
						case GrapaTokenType::LIST:
							if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") == 0)
							{
								GrapaRuleEvent* at = pInput->Head(2), * atDel = NULL;
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;
								if (at && (at->mValue.mToken == GrapaTokenType::OP)) at = atDel = vScriptExec->ProcessPlan(pNameSpace, at);
								while (at && at->mValue.mToken == GrapaTokenType::PTR && at->vRulePointer) at = at->vRulePointer;
								if (at && at->IsNull()) at = NULL;
								parameter->vWidget->Add(r, at);
								if (atDel) { atDel->CLEAR(); delete atDel; }
							}
							else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") == 0)
							{
							}
							break;
						case GrapaTokenType::ARRAY:
						case GrapaTokenType::TUPLE:
							if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") == 0)
							{
								parameter->vWidget->Add(r->mName, r->vWidget);
							}
							else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") == 0)
							{
								parameter->vWidget->Join(r);
							}
						}
						break;
					}
				}
				else
				{
					//if (parameter->mValue.mToken == GrapaTokenType::TAG)
					//{
					//	parameter = parameter->vQueue->Head(0)->vQueue->Head(1);
					//	if (r->mValue.mToken == GrapaTokenType::TAG)
					//	{
					//		parameter->CLEAR();
					//		//r = r->vQueue->Head(0)->vQueue->Head(1);
					//	}
					//	else
					//	{
					//		parameter->vQueue->CLEAR();
					//		parameter->vQueue->PushTail(new GrapaRuleEvent());
					//		parameter = parameter->vQueue->Tail();
					//	}
					//}
					switch (r->mValue.mToken)
					{
					//case GrapaTokenType::CLASS:
					//	parameter->mValue.mToken = GrapaTokenType::OBJ;
					//	parameter->vClass = r;
					//	parameter->vQueue = new GrapaRuleQueue();
					//	vScriptExec->vScriptState->CopyClassVars((GrapaRuleQueue*)parameter->vQueue, r);
					//	break;
					case GrapaTokenType::OBJ:
						parameter->mValue.FROM(r->mValue);
						parameter->mValue.mToken = GrapaTokenType::OBJ;
						parameter->mNull = r->mNull;
						//parameter->mId = r->mId;
						parameter->vClass = r->vClass;
						if (r->vValueEvent == r->vClass)
						{
							parameter->vValueEvent = r->vValueEvent;
							r->vValueEvent = NULL;
						}
						if (parameter->vQueue) {
							delete parameter->vQueue;
							parameter->vQueue = NULL;
						}
						if (r->vQueue)
							parameter->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)r->vQueue);
						else
							parameter->vQueue = new GrapaRuleQueue();
						if (parameter->vDatabase) {
							delete parameter->vDatabase;
							parameter->vDatabase = NULL;
						}
						if (parameter->vVector) {
							parameter->vVector->CLEAR();
							delete parameter->vVector;
							parameter->vVector = NULL;
						}
						if (parameter->vWidget) {
							parameter->vWidget->CLEAR();
							delete parameter->vWidget;
							parameter->vWidget = NULL;
						}
						if (r->vDatabase)
						{
							parameter->vDatabase = new GrapaLocalDatabase(r->vDatabase->vScriptState);
							parameter->vDatabase->mHomeDir.FROM(r->vDatabase->mHomeDir);
							//if (r->vDatabase->mDirectoryPath) parameter->vDatabase->mDirectoryPath = vScriptExec->CopyQueue(r->vDatabase->mDirectoryPath);
							//if (r->vDatabase->mDatabasePath) parameter->vDatabase->mDatabasePath = vScriptExec->CopyQueue(r->vDatabase->mDatabasePath);
							if (r->vDatabase->mInclude) parameter->vDatabase->mInclude = vScriptExec->CopyQueue(r->vDatabase->mInclude);
							parameter->vDatabase->mLocation.FROM(r->vDatabase->mLocation);
							//parameter->vDatabase->mVar = r->vDatabase->mVar;
							//parameter->vDatabase->mDirId = r->vDatabase->mDirId;
							//parameter->vDatabase->mDirType = r->vDatabase->mDirType;

							if (r->vDatabase->mVar)
								parameter->vDatabase->DatabaseSet(parameter->mValue);
							else
								parameter->vDatabase->HomeSwitch(r->vDatabase->mHomeDir);
							GrapaCHAR path;
							r->vDatabase->DirectoryPWD(path);
							parameter->vDatabase->DirectorySwitch(path);

							parameter->vDatabase->mSep = r->vDatabase->mSep;
						}
						if (r->vVector)
						{
							parameter->vVector = new GrapaVector();
							parameter->vVector->FROM(*r->vVector);
						}
						if (r->vWidget)
						{
							parameter->vWidget = new GrapaWidget(vScriptExec,pNameSpace);
							parameter->vWidget->vEvent = parameter;
							parameter->vWidget->FROM(*r->vWidget);
						}
						break;
					default:
						parameter->mValue.FROM(r->mValue);
						parameter->vRuleParent = r->vRuleParent;
						parameter->mValue.mToken = r->mValue.mToken;
						parameter->mNull = r->mNull;
						parameter->mId = r->mId;
						parameter->vClass = r->vClass;
						if (parameter->mValue.mToken == GrapaTokenType::ERR)
						{
							parameter->vRulePointer = NULL;
							parameter->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ERR"));
						}
						parameter->mVar = r->mVar;
						parameter->mSkip = r->mSkip;
						parameter->mT = r->mT;
						parameter->mRun = r->mRun;
						parameter->mStart = r->mStart;
						parameter->mEnd = r->mEnd;
						parameter->mEscape = r->mEscape;
						parameter->mExit = r->mExit;
						parameter->mNull = r->mNull;
						parameter->mConst = r->mConst;
						if (r->mValue.mToken!= GrapaTokenType::ERR && r->vRulePointer)
						{
							GrapaRuleEvent *ev = r->vRulePointer;
							while (ev->vRulePointer) ev = ev->vRulePointer;
							if (ev)
							{
								parameter->mValue.mToken = ev->mValue.mToken;
								if (ev->vQueue)
									parameter->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)ev->vQueue);
								else
									parameter->mValue.FROM(ev->mValue);
							}
							if (parameter->vQueue)
							{
								parameter->vQueue->CLEAR();
								delete parameter->vQueue;
								parameter->vQueue = NULL;
							}
						}
						else
						{
							if (parameter->vQueue)
							{
								parameter->vQueue->CLEAR();
								delete parameter->vQueue;
								parameter->vQueue = NULL;
							}
							if (parameter->vRuleLambda)
							{
								parameter->vRuleLambda->CLEAR();
								delete parameter->vRuleLambda;
								parameter->vRuleLambda = NULL;
							}
							if (parameter->vValueEvent)
							{
								parameter->vValueEvent->CLEAR();
								delete parameter->vValueEvent;
								parameter->vValueEvent = NULL;
							}
							if (parameter->vDatabase)
							{
								parameter->vDatabase->CLEAR();
								delete parameter->vDatabase;
								parameter->vDatabase = NULL;
							}
							if (parameter->vVector)
							{
								parameter->vVector->CLEAR();
								delete parameter->vVector;
								parameter->vVector = NULL;
							}
							if (parameter->vWidget)
							{
								parameter->vWidget->CLEAR();
								delete parameter->vWidget;
								parameter->vWidget = NULL;
							}
							if (r->vQueue)
							{
								if (true || rDel == NULL || r->mValue.mToken == GrapaTokenType::PTR)
								{
									parameter->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)r->vQueue, false, r->mConst);
								}
								else
								{
									parameter->vQueue = r->vQueue;
									r->vQueue = NULL;
								}
							}
							if (r->vRuleLambda)
							{
								if (true || rDel == NULL || r->mValue.mToken == GrapaTokenType::PTR)
								{
									parameter->vRuleLambda = vScriptExec->CopyItem((GrapaRuleEvent*)r->vRuleLambda, false, r->mConst);
								}
								else
								{
									parameter->vRuleLambda = r->vRuleLambda;
									r->vRuleLambda = NULL;
								}
							}
							if (r->vValueEvent)
							{
								if (true || rDel == NULL || r->mValue.mToken == GrapaTokenType::PTR)
								{
									parameter->vValueEvent = vScriptExec->CopyItem((GrapaRuleEvent*)r->vValueEvent, false, r->mConst);
								}
								else
								{
									parameter->vValueEvent = r->vValueEvent;
									r->vValueEvent = NULL;
								}
							}
							if (r->vDatabase)
							{
								if (rDel==NULL || r->mValue.mToken == GrapaTokenType::PTR)
								{
									parameter->vDatabase = new GrapaLocalDatabase(r->vDatabase->vScriptState);
									parameter->vDatabase->mHomeDir.FROM(r->vDatabase->mHomeDir);
									//if (r->vDatabase->mDirectoryPath) parameter->vDatabase->mDirectoryPath = vScriptExec->CopyQueue(r->vDatabase->mDirectoryPath);
									//if (r->vDatabase->mDatabasePath) parameter->vDatabase->mDatabasePath = vScriptExec->CopyQueue(r->vDatabase->mDatabasePath);
									if (r->vDatabase->mInclude) parameter->vDatabase->mInclude = vScriptExec->CopyQueue(r->vDatabase->mInclude, false, r->mConst);
									parameter->vDatabase->mLocation.FROM(r->vDatabase->mLocation);
									//parameter->vDatabase->mVar = r->vDatabase->mVar;
									//parameter->vDatabase->mDirId = r->vDatabase->mDirId;
									//parameter->vDatabase->mDirType = r->vDatabase->mDirType;

									if (r->vDatabase->mVar)
										parameter->vDatabase->DatabaseSet(parameter->mValue);
									else
										parameter->vDatabase->HomeSwitch(r->vDatabase->mHomeDir);
									GrapaCHAR path;
									r->vDatabase->DirectoryPWD(path);
									parameter->vDatabase->DirectorySwitch(path);

									parameter->vDatabase->mSep = r->vDatabase->mSep;
								}
								else
								{
									parameter->vDatabase = r->vDatabase;
									r->vDatabase = NULL;
								}
							}
							if (r->vVector)
							{
								if (true || rDel == NULL || r->mValue.mToken == GrapaTokenType::PTR)
								{
									parameter->vVector = new GrapaVector();
									parameter->vVector->FROM(*r->vVector);
								}
								else
								{
									parameter->vVector = r->vVector;
									r->vVector = NULL;
								}
							}
							if (r->vWidget)
							{
								if (rDel == NULL || r->mValue.mToken == GrapaTokenType::PTR)
								{
									parameter->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
									parameter->vWidget->vEvent = parameter;
									parameter->vWidget->FROM(*r->vWidget);
								}
								else
								{
									parameter->vWidget = r->vWidget;
									r->vWidget = NULL;
								}
								parameter->vWidget->vEvent = parameter;
							}
						}
						break;
					}
				}
			}
		}
		if (rDel) 
		{ 
			rDel->CLEAR(); 
			delete rDel; 
			rDel = NULL;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleAssignEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = 0;
	GrapaRuleEvent *result = ItemAssignRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("assign"), err);
	if (err && result == NULL)
		result = Error(vScriptExec,pNameSpace,-1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleAssignAppendEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = 0;
	GrapaRuleEvent* result = ItemAssignRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("assignappend"), err);
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleAssignExtendEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = 0;
	GrapaRuleEvent* result = ItemAssignRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("assignextend"), err);
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleExtendEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && r2.vVal->mValue.mToken == GrapaTokenType::ARRAY)
	{
		if (r1.vDel->vRulePointer)
		{
			result = vScriptExec->CopyItem(r1.vDel);
		}
		else
		{
			result = r1.vDel;
			r1.vDel = NULL;
		}
		GrapaRuleEvent* w = r2.vVal->vQueue ? r2.vVal->vQueue->Head() : NULL;
		while (w)
		{
			GrapaRuleEvent* w2 = w;
			while (w2 && w2->mValue.mToken == GrapaTokenType::PTR && w2->vRulePointer) w2 = w2->vRulePointer;
			GrapaRuleEvent* w3 = vScriptExec->CopyItem(w2);
			w3->mName.FROM(w->mName);
			result->vQueue->PushTail(w3);
			w = w->Next();
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRemoveEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && r2.vVal->mValue.mToken == GrapaTokenType::ARRAY)
	{
		if (r1.vDel->vRulePointer)
		{
			result = vScriptExec->CopyItem(r1.vDel);
		}
		else
		{
			result = r1.vDel;
			r1.vDel = NULL;
		}
		GrapaRuleEvent* w = r2.vVal->vQueue ? r2.vVal->vQueue->Head() : NULL;
		while (w)
		{
			GrapaRuleEvent* w2 = w;
			while (w2 && w2->mValue.mToken == GrapaTokenType::PTR && w2->vRulePointer) w2 = w2->vRulePointer;

			s64 index = 0;
			GrapaRuleEvent* e = NULL;
			while (true)
			{
				GrapaRuleEvent* e = result->vQueue->Search(w2->mValue, index);
				if (e == NULL) break;
				result->vQueue->PopEvent(e);
				e->CLEAR();
				delete e;
			}

			w = w->Next();
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

static GrapaRuleEvent* ItemPrependRun(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput, const GrapaCHAR& pName)
{
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam firstItem(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	if (pInput && (pInput->mCount == 0))
	{
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();
		if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createarray") == 0)
			result->mValue.mToken = GrapaTokenType::ARRAY;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createtuple") == 0)
			result->mValue.mToken = GrapaTokenType::TUPLE;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createxml") == 0)
			result->mValue.mToken = GrapaTokenType::XML;
		else if (GrapaMem::StrCmp((char*)pName.mBytes,pName.mLength,"createel") == 0)
			result->mValue.mToken = GrapaTokenType::EL;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createtag") == 0)
			result->mValue.mToken = GrapaTokenType::TAG;
		else if (GrapaMem::StrCmp((char*)pName.mBytes,pName.mLength,"createop") == 0)
			result->mValue.mToken = GrapaTokenType::OP;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createcode") == 0)
			result->mValue.mToken = GrapaTokenType::CODE;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createrule") == 0)
			result->mValue.mToken = GrapaTokenType::RULE;
		return(result);
	}

	if (GrapaMem::StrCmp((char*)pName.mBytes,pName.mLength,"prepend") == 0)
	{
		result = firstItem.vDel; firstItem.vDel = NULL;
		if (!result) result = vScriptExec->CopyItem(firstItem.vVal);
		if (result->vQueue == NULL)
		{
			result->mValue.mToken = GrapaTokenType::LIST;
			result->vRulePointer = NULL;
			result->vQueue = new GrapaRuleQueue();
		}
	}
	else
	{
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();
		GrapaRuleEvent *p = firstItem.vDel; firstItem.vDel = NULL;
		if (!p) 
			p = vScriptExec->CopyItem(firstItem.vVal);
		if (!p) 
		{ 
			p = new GrapaRuleEvent(); 
			p->SetNull(); 
		}
		if (p)
			result->vQueue->PushHead(p);
		if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createarray") == 0)
			result->mValue.mToken = GrapaTokenType::ARRAY;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createtuple") == 0)
			result->mValue.mToken = GrapaTokenType::TUPLE;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createxml") == 0)
			result->mValue.mToken = GrapaTokenType::XML;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createel") == 0)
			result->mValue.mToken = GrapaTokenType::EL;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createtag") == 0)
			result->mValue.mToken = GrapaTokenType::TAG;
		else if (GrapaMem::StrCmp((char*)pName.mBytes,pName.mLength,"createop") == 0)
			result->mValue.mToken = GrapaTokenType::OP;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createcode") == 0)
			result->mValue.mToken = GrapaTokenType::CODE;
		else if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "createrule") == 0)
			result->mValue.mToken = GrapaTokenType::RULE;
	}

	if (result && result->vQueue && pInput)
	{
		for (int i = 1; i < pInput->mCount; i++)
		{
			GrapaRuleEvent* r = pInput ? pInput->Head(i) : NULL;
			GrapaRuleEvent* rDel = NULL;
			while (r && r->mValue.mToken == GrapaTokenType::PTR) r = r->vRulePointer;
			if (r && r->mValue.mToken == GrapaTokenType::OP) r = rDel = vScriptExec->ProcessPlan(pNameSpace, r);
			GrapaRuleEvent* p = rDel;
			if (!p) p = vScriptExec->CopyItem(r);
			if (!p) { p = new GrapaRuleEvent(); p->SetNull(); }
			if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "prepend") == 0)
				result->vQueue->PushHead(p);
			else
				result->vQueue->PushTail(p);
		}
	}

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCreateArrayEvent::Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam)
{
	if (pParam && pParam->vQueue && pParam->vQueue->mCount)
	{
		GrapaRuleEvent* v = (GrapaRuleEvent*)pParam->vQueue->Head();
		bool noLambda = true;
		while (v)
		{
			if (v->mValue.mToken == GrapaTokenType::OP || v->mValue.mToken == GrapaTokenType::CODE)
			{
				noLambda = false;
				break;
			}
			v = v->Next();
		}
		if (noLambda)
		{
			v = (GrapaRuleEvent*)pOperation->vQueue->PopTail();
			v->mValue.mToken = GrapaTokenType::ARRAY;
			pOperation->CLEAR();
			delete pOperation;
			pOperation = v;
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRuleCreateArrayEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createarray"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateTupleEvent::Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam)
{
	if (pParam && pParam->vQueue && pParam->vQueue->mCount)
	{
		GrapaRuleEvent* v = (GrapaRuleEvent*)pParam->vQueue->Head();
		bool noLambda = true;
		while (v)
		{
			if (v->mValue.mToken == GrapaTokenType::OP || v->mValue.mToken == GrapaTokenType::CODE)
			{
				noLambda = false;
				break;
			}
			v = v->Next();
		}
		if (noLambda)
		{
			v = (GrapaRuleEvent*)pOperation->vQueue->PopTail();
			v->mValue.mToken = GrapaTokenType::TUPLE;
			pOperation->CLEAR();
			delete pOperation;
			pOperation = v;
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRuleCreateTupleEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createtuple"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateListEvent::Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam)
{
	if (pParam && pParam->vQueue && pParam->vQueue->mCount)
	{
		GrapaRuleEvent *v = (GrapaRuleEvent*)pParam->vQueue->Head();
		bool noLambda = true;
		while (v)
		{
			if (v->mValue.mToken == GrapaTokenType::OP || v->mValue.mToken == GrapaTokenType::CODE)
			{
				noLambda = false;
				break;
			}
			v = v->Next();
		}
		if (noLambda)
		{
			v = (GrapaRuleEvent*)pOperation->vQueue->PopTail();
			v->mValue.mToken = GrapaTokenType::LIST;
			pOperation->CLEAR();
			delete pOperation;
			pOperation = v;
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRuleCreateListEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createlist"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateXmlEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createxml"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateElEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createel"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateTagEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createtag"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateOpEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createop"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateCodeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createcode"));
}

GrapaRuleEvent* GrapaLibraryRuleCreateRuleEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createrule"));
}

GrapaRuleEvent* GrapaLibraryRulePrependEvent::Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam)
{
	//return(pOperation);
	if (pParam && pParam->vQueue && pParam->vQueue->mCount)
	{
		GrapaRuleEvent *v = (GrapaRuleEvent*)pParam->vQueue->Head();
		if ((v->mValue.mToken == GrapaTokenType::ARRAY || v->mValue.mToken == GrapaTokenType::TUPLE || v->mValue.mToken == GrapaTokenType::LIST || v->mValue.mToken == GrapaTokenType::XML || v->mValue.mToken == GrapaTokenType::EL || v->mValue.mToken == GrapaTokenType::TAG || v->mValue.mToken == GrapaTokenType::OBJ) && v->vQueue)
		{
			bool noLambda = true;
			while (v)
			{
				if (v->mValue.mToken == GrapaTokenType::OP || v->mValue.mToken == GrapaTokenType::CODE)
				{
					noLambda = false;
					break;
				}
				v = v->Next();
			}
			if (noLambda)
			{
				v = (GrapaRuleEvent*)pParam->vQueue->PopHead();
				if (v->vQueue == NULL)
				{
					v->mValue.mToken = GrapaTokenType::LIST;
					v->vRulePointer = NULL;
					v->vQueue = new GrapaRuleQueue();
				}
				GrapaRuleEvent *v2 = (GrapaRuleEvent*)pParam->vQueue->PopHead();
				while (v2)
				{
					v->vQueue->PushHead(v2);
					v2 = (GrapaRuleEvent*)pParam->vQueue->PopHead();
				}
				pOperation->CLEAR();
				delete pOperation;
				pOperation = v;
			}
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRulePrependEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("prepend"));
}

GrapaRuleEvent* GrapaLibraryRulePopEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* r1 = NULL;
	GrapaRuleQueue* q = NULL;
	u8 t = 0;
	GrapaWidget* wg = NULL;
	GrapaRuleEvent* parameter = NULL;
	if (GrapaRuleEvent* p1 = pInput ? pInput->Head(0) : NULL)
	{
		GrapaRuleEvent* item = p1;
		if (item->mValue.mToken == GrapaTokenType::PTR && item->vRulePointer && item->vRulePointer->vQueue)
		{
			q = (GrapaRuleQueue*)item->vRulePointer->vQueue;
			t = item->vRulePointer->mValue.mToken;
		}
		else if (item->mValue.mToken == GrapaTokenType::OP && item->vQueue)
		{
			if ((r1 = vScriptExec->ProcessPlan(pNameSpace, item)))
			{
				if (r1->mValue.mToken == GrapaTokenType::PTR && r1->vRulePointer && r1->vRulePointer->vQueue)
				{
					q = (GrapaRuleQueue*)r1->vRulePointer->vQueue;
					t = r1->vRulePointer->mValue.mToken;
				}
				else if (r1->mValue.mToken == GrapaTokenType::PTR && r1->vRulePointer && r1->vRulePointer->vWidget)
					wg = r1->vRulePointer->vWidget;
				else if (r1->mValue.mToken == GrapaTokenType::WIDGET && r1->vWidget)
					wg = r1->vWidget;
			}
		}
		else if (item->mValue.mToken == GrapaTokenType::ID && item->mValue.mLength)
		{
			parameter = vScriptExec->vScriptState->SearchVariable(pNameSpace, item->mValue);
			if (parameter && parameter->vQueue)
			{
				if (parameter->mValue.mToken == GrapaTokenType::TAG)
					parameter = parameter->vQueue->Head(0)->vQueue->Head(1);
				q = (GrapaRuleQueue*)parameter->vQueue;
				t = parameter->mValue.mToken;
			}
			else
			{
				GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
				if (r2.vVal && parameter)
				{
					switch (parameter->mValue.mToken)
					{
					case GrapaTokenType::STR:
						break;
					case GrapaTokenType::INT:
						if (r2.vVal->mValue.mToken == GrapaTokenType::INT)
						{
							GrapaInt a, b;
							a.FromBytes(parameter->mValue);
							b.FromBytes(r2.vVal->mValue);
							parameter->mValue.FROM((a - b).getBytes());
						}
						break;
					}
				}
			}
		}
		else if (item->mValue.mToken == GrapaTokenType::PTR && item->vRulePointer && item->vRulePointer->vWidget)
		{
			wg = item->vRulePointer->vWidget;
		}
		else if (item->mValue.mToken == GrapaTokenType::WIDGET && item->vWidget)
		{
			wg = item->vWidget;
		}
	}
	if (q)
	{
		GrapaRuleEvent* r2 = NULL;
		GrapaRuleEvent* p2 = pInput ? pInput->Head(1) : NULL;
		if (p2 && pNameSpace)
		{
			if (p2->mValue.mToken == GrapaTokenType::OP)
				r2 = vScriptExec->ProcessPlan(pNameSpace, p2);
		}
		GrapaRuleEvent* r3 = NULL;
		GrapaRuleEvent* p3 = pInput ? pInput->Head(2) : NULL;
		if (p3 && pNameSpace)
		{
			if (p3->mValue.mToken == GrapaTokenType::OP)
				r3 = vScriptExec->ProcessPlan(pNameSpace, p3);
		}
		if (r2)
		{
			GrapaRuleEvent* temp = NULL;
			GrapaRuleEvent* cur = r2->vRulePointer ? r2->vRulePointer : r2;
			GrapaRuleEvent* next = cur ? cur->Next() : NULL;
			if (t == GrapaTokenType::TAG)
			{
				if (q->Head(0)->vQueue->mCount==2)
					temp = q->Head(0)->vQueue->Head(1)->vQueue->PopEvent(cur);
			}
			else
				temp = q->PopEvent(cur);
			while (temp && r3 && temp!=r3->vRulePointer)
			{
				if (temp) { temp->CLEAR(); delete temp; temp = NULL; }
				cur = (next&&next->vRulePointer) ? next->vRulePointer : next;
				next = cur ? cur->Next() : NULL;
				if (t == GrapaTokenType::TAG)
				{
					if (q->Head(0)->vQueue->mCount == 2)
						temp = q->Head(0)->vQueue->Head(1)->vQueue->PopEvent(cur);
				}
				else
					temp = q->PopEvent(cur);
			}
			if (temp) { temp->CLEAR(); delete temp; temp = NULL; }
		}
		if (r2) { r2->CLEAR(); delete r2; }
	}
	if (wg)
	{
		GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
		GrapaRuleEvent* at = NULL;
		if (r2.vVal && !r2.vVal->IsNull()) at = r2.vVal;
		if (at)
			wg->Remove(at);
	}

	if (r1) { r1->CLEAR(); delete r1; }
	return(result);
}

static GrapaRuleEvent* ItemSearchCall(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pClass, GrapaRuleEvent *pCmd, GrapaRuleEvent *pParam, GrapaError& err)
{

	err = -1;
	GrapaRuleEvent* result = NULL;
	if (pCmd)
	{
		GrapaRuleEvent m;
		GrapaRuleEvent* o, * p;
		switch (pCmd->mValue.mToken)
		{
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			if (pClass)
			{
				m.mValue.mToken = GrapaTokenType::PTR;
				m.vRulePointer = pClass;
				pNameSpace->GetNameQueue()->PushTail(&m);
			}
			if (pParam && pParam->vQueue)
				result = vScriptExec->ProcessPlan(pNameSpace, pCmd, (GrapaRuleEvent*)pParam->vQueue->Head(), pParam->vQueue->mCount);
			else
				result = vScriptExec->ProcessPlan(pNameSpace, pCmd);
			if (pClass)
			{
				pNameSpace->GetNameQueue()->PopTail();
			}
			err = 0;
			break;
		case GrapaTokenType::ID:
		case GrapaTokenType::STR:
		case GrapaTokenType::SYSID:
		case GrapaTokenType::SYSSTR:
			if (!pCmd->mValue.mBytes || !pCmd->mValue.mLength)
			{
				break;
			}
			o = vScriptExec->vScriptState->SearchVariable(pNameSpace, pCmd);
			if (o && (o->mValue.mToken == GrapaTokenType::OP || o->mValue.mToken == GrapaTokenType::CODE))
			{
				if (pClass)
				{
					m.mValue.mToken = GrapaTokenType::PTR;
					m.vRulePointer = pClass;
					pNameSpace->GetNameQueue()->PushTail(&m);
				}
				if (pParam && pParam->vQueue)
					result = vScriptExec->ProcessPlan(pNameSpace, o, (GrapaRuleEvent*)pParam->vQueue->Head(), pParam->vQueue->mCount);
				else
					result = vScriptExec->ProcessPlan(pNameSpace, o);
				if (pClass)
				{
					pNameSpace->GetNameQueue()->PopTail();
				}
				err = 0;
				break;
			}
			if (pCmd->mValue.mBytes && pCmd->mValue.mLength)
			{
				GrapaCHAR name;
				if ((pCmd->mValue.mToken == GrapaTokenType::SYSID || pCmd->mValue.mToken == GrapaTokenType::SYSSTR) && pCmd->mValue.mBytes && pCmd->mValue.mBytes[0] != '$')
					name.FROM("$");
				name.Append(pCmd->mValue);
				o = vScriptExec->vScriptState->GetClass(pNameSpace, name);
				if (o && o->mValue.mToken == GrapaTokenType::OP)
				{
					if (pParam && pParam->vQueue)
						result = vScriptExec->ProcessPlan(pNameSpace, o, (GrapaRuleEvent*)pParam->vQueue->Head(), pParam->vQueue->mCount);
					else
						result = vScriptExec->ProcessPlan(pNameSpace, o);
					break;
				}
			}
			if (o)
			{
				result = new GrapaRuleEvent();// pCmd->mValue);
				result->mValue.mToken = GrapaTokenType::OBJ;
				if (o->mName.Cmp("$ARRAY") == 0)
				{
					result->mValue.mToken = GrapaTokenType::ARRAY;
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				else if (o->mName.Cmp("$TUPLE") == 0)
				{
					result->mValue.mToken = GrapaTokenType::TUPLE;
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				else if (o->mName.Cmp("$VECTOR") == 0)
				{
					result->mValue.FROM(GrapaVector().getBytes());
				}
				else if (o->mName.Cmp("$WIDGET") == 0)
				{
					result->mValue.mToken = GrapaTokenType::WIDGET;
					result->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
					result->vWidget->vEvent = result;
				}
				else if (o->mName.Cmp("$BOOL") == 0)
				{
					result->mValue.mToken = GrapaTokenType::BOOL;
				}
				else if (o->mName.Cmp("$ERR") == 0)
				{
					result->mValue.mToken = GrapaTokenType::ERR;
				}
				else if (o->mName.Cmp("$FLOAT") == 0)
				{
					//result->mValue.mToken = GrapaTokenType::FLOAT;
					result->mValue.FROM(GrapaFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0).getBytes());
				}
				else if (o->mName.Cmp("$ID") == 0)
				{
					result->mValue.mToken = GrapaTokenType::ID;
				}
				else if (o->mName.Cmp("$INT") == 0)
				{
					result->mValue.FROM(GrapaInt(0).getBytes());
					//result->mValue.mToken = GrapaTokenType::INT;
				}
				else if (o->mName.Cmp("$LIST") == 0)
				{
					result->mValue.mToken = GrapaTokenType::LIST;
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				else if (o->mName.Cmp("$RAW") == 0)
				{
					result->mValue.mToken = GrapaTokenType::RAW;
				}
				else if (o->mName.Cmp("$RULE") == 0)
				{
					result->mValue.mToken = GrapaTokenType::RULE;
				}
				else if (o->mName.Cmp("$STR") == 0)
				{
					result->mValue.mToken = GrapaTokenType::STR;
				}
				else if (o->mName.Cmp("$SYSID") == 0)
				{
					result->mValue.mToken = GrapaTokenType::SYSID;
				}
				else if (o->mName.Cmp("$SYSINT") == 0)
				{
					result->mValue.mToken = GrapaTokenType::SYSINT;
				}
				else if (o->mName.Cmp("$SYSSTR") == 0)
				{
					result->mValue.mToken = GrapaTokenType::SYSSTR;
				}
				else if (o->mName.Cmp("$TABLE") == 0)
				{
					result->mValue.mToken = GrapaTokenType::TABLE;
				}
				else if (o->mName.Cmp("$TIME") == 0)
				{
					result->mValue.mToken = GrapaTokenType::TIME;
				}
				else if (o->mName.Cmp("$XML") == 0)
				{
					result->mValue.mToken = GrapaTokenType::XML;
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				else if (o->mName.Cmp("$EL") == 0)
				{
					result->mValue.mToken = GrapaTokenType::EL;
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				else if (o->mName.Cmp("$TAG") == 0)
				{
					result->mValue.mToken = GrapaTokenType::TAG;
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				else if (o->mName.Cmp("$OBJ") == 0)
				{
					result->mValue.mToken = GrapaTokenType::OBJ;
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				else if (result->mValue.mToken == GrapaTokenType::OBJ)
				{
					if (result->vQueue == NULL)
						result->vQueue = new GrapaRuleQueue();
				}
				result->vClass = o;
				if (result->vQueue==NULL)
					result->vQueue = new GrapaRuleQueue();
				vScriptExec->vScriptState->CopyClassVars((GrapaRuleQueue*)result->vQueue, o);
				err = 0;
				p = vScriptExec->vScriptState->FindClassVar(pNameSpace, o, GrapaCHAR("$new"));
				if (p)
				{
					m.mValue.mToken = GrapaTokenType::PTR;
					m.vRulePointer = result;
					pNameSpace->GetNameQueue()->PushTail(&m);
					GrapaRuleEvent* r = NULL;
					if (pParam && pParam->vQueue)
						r = vScriptExec->ProcessPlan(pNameSpace, p, (GrapaRuleEvent*)pParam->vQueue->Head(), pParam->vQueue->mCount);
					else
						r = vScriptExec->ProcessPlan(pNameSpace, p);
					pNameSpace->GetNameQueue()->PopTail();
					if (r)
					{
						if (r->mValue.mToken == GrapaTokenType::ERR)
						{
							result->CLEAR();
							delete result;
							result = r;
						}
						else
						{
							r->CLEAR();
							delete r;
						}
					}
				}
			}
			break;
		default:
			if (!pCmd->mValue.mBytes || !pCmd->mValue.mLength)
			{
				m.mAbort = false;
			}
			result = vScriptExec->CopyItem(pCmd);
			err = 0;
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCallEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal->mValue.Cmp("WIDGET") == 0)
	{
		int x = 1;
	}
	result = ItemSearchCall(vScriptExec, pNameSpace, r1.vDel ? r1.vDel->vClass : NULL, r1.vVal, r2.vVal, err);
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSearchEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* c = NULL;
	GrapaRuleQueue* q = NULL;
	GrapaRuleQueue* qParent = NULL;
	GrapaRuleEvent* root = NULL;
	GrapaRuleQueue delQueue;
	GrapaRuleQueue emptyQueue;
	GrapaCHAR varname;
	s64 idx;
	bool isVar = false;
	GrapaError err = 0;
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaRuleEvent* r1=pInput ? pInput->Head(0) : NULL;
	GrapaRuleEvent* item = r1;
	GrapaRuleEvent* r1Del = NULL;
	GrapaRuleEvent* objitem = NULL;
	if (item)
	{
		switch (item->mValue.mToken)
		{
		case GrapaTokenType::PTR:
			if (item->vRulePointer && item->vRulePointer->vQueue)
				q = (GrapaRuleQueue*)item->vRulePointer->vQueue;
			break;
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			if (item->vQueue)
			{
				r1 = vScriptExec->ProcessPlan(pNameSpace, item);
				if (r1)
				{
					r1Del = r1;
					isVar = r1->mVar;
					item = r1;
					while (item->mValue.mToken == GrapaTokenType::PTR && item->vRulePointer) item = item->vRulePointer;
					if (item->mValue.mToken == GrapaTokenType::ERR && item->vRulePointer)
					{
						item->vRulePointer = NULL;
						item->vClass = NULL;
					}
				}
			}
			break;
		}
		switch (item->mValue.mToken)
		{
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			q = (GrapaRuleQueue*)item->vQueue;
			c = NULL;
			root = pNameSpace->GetNameQueue()->Tail();
			break;
		case GrapaTokenType::CLASS:
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::OBJ:
		case GrapaTokenType::RULEOP:
			q = (GrapaRuleQueue*)item->vQueue;
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::SYSID:
			//result = ItemSearchCall(vScriptExec, pNameSpace, NULL, item, r2.vVal, err);  ///can't remember why this is here...I think it is to reference a parameter...need to verify
			//if (err && result == NULL)
			//{
			//	if (item->vClass == NULL)
			//		item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$SYSID"));
			//	c = item->vClass;
			//	q = c ? &emptyQueue : NULL;
			//	root = item;
			//}
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$SYSID"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::ARRAY:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ARRAY"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::TUPLE:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TUPLE"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::VECTOR:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$VECTOR"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::WIDGET:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::LIST:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$LIST"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::XML:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$XML"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::EL:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$EL"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::TAG:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TAG"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::RULE:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$RULE"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::ERR:
			q = (GrapaRuleQueue*)item->vQueue;
			if (item->vRulePointer)
			{
				item->vRulePointer = NULL;
				item->vClass = NULL;
			}
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ERR"));
			c = item->vClass;
			root = item;
			break;
		case GrapaTokenType::TABLE:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TABLE"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::ID:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ID"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::INT:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$INT"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::SYSINT:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$SYSINT"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::STR:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$STR"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::SYSSTR:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$SYSSTR"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::FLOAT:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$FLOAT"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::RAW:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$RAW"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::BOOL:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$BOOL"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::TIME:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TIME"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		case GrapaTokenType::TOKEN:
			if (item->vClass == NULL)
				item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TOKEN"));
			c = item->vClass;
			q = c ? &emptyQueue : NULL;
			root = item;
			break;
		}
	}
	if (q)
	{
		if (r2.vVal)
		{
			GrapaRuleEvent* e = NULL;
			GrapaRuleEvent* attrList = r2.vVal;
			while (attrList->mValue.mToken == GrapaTokenType::PTR && attrList->vRulePointer) attrList = attrList->vRulePointer;
			if (attrList->vQueue)
			{
				switch (attrList->mValue.mToken)
				{
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::TUPLE:
				case GrapaTokenType::LIST:
				case GrapaTokenType::XML:
				case GrapaTokenType::EL:
				case GrapaTokenType::TAG:
				case GrapaTokenType::OP:
				case GrapaTokenType::ERR:
				case GrapaTokenType::OBJ:
					attrList = (GrapaRuleEvent*)attrList->vQueue->Head();
					break;
				}
			}
			while (q && attrList) //a={"one"(1)=11,"two"(2)=22,"three"(3)=33}
			{
				e = NULL;
				GrapaRuleEvent* attr = attrList;
				while (attr->mValue.mToken == GrapaTokenType::PTR) attr = attr->vRulePointer;
				if (attr->mValue.mToken == GrapaTokenType::OP)
				{
					if (item && item != pNameSpace->vLast)
					{
						item = vScriptExec->CopyItem(root);
						delQueue.PushTail(item);
						if (pNameSpace->vLast)
						{
							pNameSpace->vLast->CLEAR();
							delete pNameSpace->vLast;
							pNameSpace->vLast = NULL;
						}
						pNameSpace->vLast = new GrapaRuleEvent();
						pNameSpace->vLast->mValue.mToken = GrapaTokenType::PTR;
						pNameSpace->vLast->vRulePointer = item;
					}
					GrapaRuleEvent *r = ItemSearchCall(vScriptExec, pNameSpace, root, attr, NULL, err);
					if (r) delQueue.PushTail(r);
					e = r;
					if (err && result == NULL)
					{
						e = Error(vScriptExec, pNameSpace, err);
						delQueue.PushTail(e);
					}
				}
				else if (!attrList->mVar && attr->mName.mLength)
				{
					varname.FROM(attr->mName);
					if (root && root->mValue.mToken == GrapaTokenType::TABLE)
					{
						if (root->vDatabase == NULL)
						{
							root->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);
							root->vDatabase->DatabaseSet(root->mValue);
						}
					}
					idx = -1;
					if (e == NULL && q)
						e = q->Search(varname,idx);
					if (e == NULL && c == NULL && root)
					{
						if (item->mValue.mToken == GrapaTokenType::OBJ)
						{
							if (root->vClass == NULL)
								root->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$OBJ"));
							c = root->vClass;
						}
						else if (item->mValue.mToken == GrapaTokenType::OP)
						{
							if (root->vClass == NULL)
								root->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$OP"));
							c = root->vClass;
						}
					}
					if (e == 0)
					{
						if (item->mValue.mToken == GrapaTokenType::OP || item->mValue.mToken == GrapaTokenType::CODE)
							root = item;
					}
					if (e == NULL && c && c->vQueue && attr->mValue.mToken != GrapaTokenType::STR)
					{
						e = vScriptExec->vScriptState->FindClassVar(pNameSpace, c, varname);
						if (e && objitem == NULL) objitem = item;
					}
					if (e == NULL)
						e = vScriptExec->vScriptState->SearchVariable(pNameSpace, varname);  // NEED TO VERIFY - can this be merged with the prior search?
					if (e == NULL)
					{
						e = Error(vScriptExec, pNameSpace, -1);
						delQueue.PushTail(e);
					}
					if (e)
					{
						GrapaRuleEvent *f = e;
						while (f->mValue.mToken == GrapaTokenType::PTR && f->vRulePointer) f = f->vRulePointer;
						if (root && root != pNameSpace->vLast)
						{
							//root = vScriptExec->CopyItem(root);
							//delQueue.PushTail(root);
							if (pNameSpace->vLast)
							{
								pNameSpace->vLast->CLEAR();
								delete pNameSpace->vLast;
								pNameSpace->vLast = NULL;
							}
							pNameSpace->vLast = new GrapaRuleEvent();
							pNameSpace->vLast->mValue.mToken = GrapaTokenType::PTR;
							pNameSpace->vLast->vRulePointer = root;
						}
						GrapaRuleEvent *r = ItemSearchCall(vScriptExec, pNameSpace, root, f, attr, err);
						if (r) delQueue.PushTail(r);
						e = r;
						if (err && result == NULL)
						{
							e = Error(vScriptExec, pNameSpace, err);
							delQueue.PushTail(e);
						}
					}
				}
				else if (attr->mNull)
				{
					if (root->mValue.mToken == GrapaTokenType::TAG)
						q = q->Head()->vQueue->Head(1)->vQueue;
					result = new GrapaRuleEvent();
					result->mValue.FROM(GrapaInt(q->mCount).getBytes());
					break;
				}
				else if (attr->mName.mLength == 0 && attr->mName.mToken == GrapaTokenType::SYSINT)
				{
					if (root->mValue.mToken == GrapaTokenType::TAG)
						q = q->Head()->vQueue->Head(1)->vQueue;
					result = new GrapaRuleEvent();
					result->mValue.FROM(GrapaInt(q->FindIndex(attr->mValue)).getBytes());
					break;
				}
				else if (attr->mValue.mToken == GrapaTokenType::STR || attr->mValue.mToken == GrapaTokenType::ID || attr->mValue.mToken == GrapaTokenType::SYSSTR || attr->mValue.mToken == GrapaTokenType::SYSID)
				{
					varname.SetLength(0);
					if (attr->mValue.mToken == GrapaTokenType::SYSSTR || attr->mValue.mToken == GrapaTokenType::SYSID)
						varname.FROM("$");
					varname.Append(attr->mValue);
					if (root && root->mValue.mToken == GrapaTokenType::TABLE)
					{
						if (root->vDatabase == NULL)
						{
							root->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);
							root->vDatabase->DatabaseSet(root->mValue);
						}
					}
					idx = -1;
					if (e == NULL && q)
						e = q->Search(varname,idx);
					//if (e == NULL && c == NULL && root && root->vClass == NULL && item->mValue.mToken == GrapaTokenType::OBJ)
					//{
					//	root->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$OBJ"));
					//	c = root->vClass;
					//}
					if (e == NULL && c && c->vQueue && attr->mValue.mToken != GrapaTokenType::STR)
					{
						e = vScriptExec->vScriptState->FindClassVar(pNameSpace, c, varname);
						if (e && objitem == NULL) objitem = item;
					}
					if (e && delQueue.Tail() == item)
					{
						e = vScriptExec->CopyItem(e);
						delQueue.PushTail(e);
					}
					else if (e && (item->mValue.mToken == GrapaTokenType::ARRAY || item->mValue.mToken == GrapaTokenType::TUPLE))
					{
						e = new GrapaRuleEvent(GrapaTokenType::INT, GrapaCHAR(""), GrapaInt(idx).getBytes());
						delQueue.PushTail(e);
					}
					if (e == NULL)
					{
						if (attrList->Next() == NULL)
						{
							// this is a HACK to pass the item back to Assign...look at the Assign that checks for ERR
							// ended up needing to put code in various places to remove the vRulePointer and vClass if not used
							result = Error(vScriptExec, pNameSpace, -1);
							result->mVar = true;
							result->mLocal = true;
							result->vRulePointer = attr;
							result->vClass = root;
							result->mName.FROM(attr->mValue);
							if (result->vClass->mValue.mToken == GrapaTokenType::TAG)
							{
								result->vClass = root->vQueue->Head()->vQueue->Head(1);
							}
							break;
						}
						e = Error(vScriptExec, pNameSpace, -1);
						delQueue.PushTail(e);
					}
				}
				else if (attr->mValue.mToken == GrapaTokenType::INT)
				{
					if (root->mValue.mToken == GrapaTokenType::TAG)
					{
						if (q->Head()->vQueue->mCount == 2) q = q->Head()->vQueue->Head(1)->vQueue;
						else q = NULL;
					}
					if (q)
					{
						GrapaInt a;
						a.FromBytes(attr->mValue);
						s64 aVal = a.LongValue();
						if (aVal < 0)
							e = q->Tail((u64)((-1) - aVal));
						else
							e = q->Head((u64)aVal);
						if (e && delQueue.Tail() == item)
						{
							e = vScriptExec->CopyItem(e);
							delQueue.PushTail(e);
						}
					}
					if (e == NULL)
					{
						e = Error(vScriptExec, pNameSpace, -1);
						delQueue.PushTail(e);
					}
				}
				else if (attr->mId)
				{
					if (root->mValue.mToken == GrapaTokenType::TAG)
						q = q->Head()->vQueue->Head(1)->vQueue;
					GrapaInt a;
					a.FromBytes(attr->mValue);
					e = q->Search((u64)a.LongValue());
					if (e && delQueue.Tail() == item)
					{
						e = vScriptExec->CopyItem(e);
						delQueue.PushTail(e);
					}
					if (e == NULL)
						if (e == NULL)
					{
						e = Error(vScriptExec, pNameSpace, -1);
						delQueue.PushTail(e);
					}
				}
				else
					e = NULL;
				qParent = q;
				q = NULL;
				if (e)
				{
					GrapaRuleEvent *item2 = e;
					item = e;
					while (item2->mValue.mToken == GrapaTokenType::PTR && item2->vRulePointer) item2 = item2->vRulePointer;
					switch (item2->mValue.mToken)
					{
					case GrapaTokenType::OP:
					case GrapaTokenType::CODE:
						q = (GrapaRuleQueue*)item2->vQueue;
						break;
					case GrapaTokenType::CLASS:
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::OBJ:
					case GrapaTokenType::RULEOP:
						q = (GrapaRuleQueue*)item2->vQueue;
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::SYSID:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$SYSID"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::ARRAY:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ARRAY"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::TUPLE:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TUPLE"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::VECTOR:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$VECTOR"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::WIDGET:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::LIST:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$LIST"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::XML:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$XML"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::EL:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$EL"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::TAG:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TAG"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::RULE:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$RULE"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::ERR:
						q = (GrapaRuleQueue*)item2->vQueue;
						if (item2->vRulePointer)
						{
							item2->vRulePointer = NULL;
							item2->vClass = NULL;
						}
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ERR"));
						c = item2->vClass;
						root = item2;
						break;
					case GrapaTokenType::TABLE:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TABLE"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::ID:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ID"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::INT:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$INT"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::SYSINT:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$SYSINT"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::STR:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$STR"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::SYSSTR:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$SYSSTR"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::FLOAT:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$FLOAT"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::RAW:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$RAW"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::BOOL:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$BOOL"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::TIME:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TIME"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					case GrapaTokenType::TOKEN:
						if (item2->vClass == NULL)
							item2->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TOKEN"));
						c = item2->vClass;
						q = c ? &emptyQueue : NULL;
						root = item2;
						break;
					}
				}
				attrList = attrList->Next();
			}
			if (attrList)
				e = NULL;
			if (result == NULL && e)
			{
				if (e == delQueue.Tail())
					result = delQueue.PopTail();
				else
				{

// Not sure whwt is happening here
// For some reason I had the code change below to return a copy...maybe there was a crash this was fixing
// But this resulted in not being able to update an element in an array
// So I had to revert the change, but can't remember how to reproduce the issue that caused me to make this change

					GrapaRuleEvent *f = e;
					while (f->mValue.mToken == GrapaTokenType::PTR && f->vRulePointer) f = f->vRulePointer;
					result = new GrapaRuleEvent();
					result->mValue.mToken = GrapaTokenType::PTR;
					result->vRulePointer = f;
					result->vClass = root;
					result->mVar = isVar;
					result->mLocal = !isVar;
					result->vRuleParent = objitem? objitem:item;
					//result = vScriptExec->CopyItem(e);
				}
			}
		}
	}
	if (r1)
	{
		// I only THINK that r1 is goind away...created r1Del for what will go away...
		// If the result has anything to do with r1, then need to make a copy because r1 is going away
		// For some reason we need to ignore result->mVar, not exactly sure why. Need to investigate this more.
		// Not copying name over...not sure why, or if this matters...need to run some tests
		if (!isVar && result && result->mValue.mToken == GrapaTokenType::PTR && result->vRulePointer)
		{
			//GrapaRuleEvent *ev = result->vRulePointer;
			//while (ev->vRulePointer) ev = ev->vRulePointer;
			//result->mValue.mToken = ev->mValue.mToken;
			//result->vRulePointer = NULL;
			//result->vClass = ev->vClass;
			//result->mValue.FROM(ev->mValue);
			//if (ev->vQueue)
			//{
			//	result->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)ev->vQueue);
			//}
			result = vScriptExec->CopyItem(result->vRulePointer);
		}
	}
	if (r1Del)
	{
		r1Del->CLEAR();
		delete r1Del;
	}
	return(result);
}

GrapaRuleEvent* FindAll(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p);

bool MatchTAG(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p, bool all=false)
{

	s64 index = 0;
	bool n = false;
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* pname = p->vQueue->Search(GrapaCHAR("name"), index);
		if (pname)
		{
			n = true;
			if ((pname->mValue.mToken == GrapaTokenType::STR) || (pname->mValue.mToken == GrapaTokenType::ID))
			{
				if (pname->mValue.Cmp(b->mName) == 0)
					n = false;
			}
			else if (pname->vQueue)
			{
				GrapaRuleEvent* pn = pname->vQueue->Head();
				while ((n == true) && (pn))
				{
					if (b->mName.Cmp(pn->mValue) == 0)
						n = false;
					pn = pn->Next();
				};
			};
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* pvalue = p->vQueue->Search(GrapaCHAR("value"), index);
		if (pvalue)
		{
			n = true;
			if ((pvalue->mValue.mToken == GrapaTokenType::STR) || (pvalue->mValue.mToken == GrapaTokenType::ID))
			{
				GrapaRuleEvent* bj = b->vQueue->mCount>1?b->vQueue->Head(1)->vQueue->Head():NULL;
				while ((n == true) && (bj))
				{
					if ((bj->mValue.mToken == GrapaTokenType::STR) || (bj->mValue.mToken == GrapaTokenType::ID))
					{
						GrapaCHAR v(bj->mValue);
						v.Trim();
						if (v.Cmp(pvalue->mValue)==0)
							n = false;
					};
					bj = bj->Next();
				};
			}
			else if (pvalue->vQueue)
			{
				u64 i = 0;
				u64 m = pvalue->vQueue->mCount;
				while ((n == true) && (i < m))
				{
					if ((pvalue->vQueue->Head(i)->mValue.mToken == GrapaTokenType::STR) || (pvalue->vQueue->Head(i)->mValue.mToken == GrapaTokenType::ID))
					{
						GrapaRuleEvent* bj = b->vQueue->Head(1)->vQueue->Head();
						while ((n == true) && (bj))
						{
							if ((bj->mValue.mToken == GrapaTokenType::STR) || (bj->mValue.mToken == GrapaTokenType::ID))
							{
								GrapaCHAR v(bj->mValue);
								v.Trim();
								if (v.Cmp(pvalue->vQueue->Head(i)->mValue)==0)
									n = false;
							};
							bj = bj->Next();
						};
					};
					i += 1;
				};
			};
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* pattr = p->vQueue->Search(GrapaCHAR("attr"), index);
		if (pattr && pattr->vQueue && pattr->vQueue->mCount)
		{
			if (pattr->mValue.mToken == GrapaTokenType::LIST)
			{
				n = false;
				GrapaRuleEvent* m = pattr->vQueue->Head();
				while ((n == false) && (m))
				{
					index = 0;
					GrapaRuleEvent* at = b->vQueue->Head(0)->vQueue->Search(m->mName, index);
					if (at==NULL || at->mValue.Cmp(m->mValue)!=0)
						n = true;
					m = m->Next();
				};
			}
			else if (pattr->mValue.mToken == GrapaTokenType::ARRAY || pattr->mValue.mToken == GrapaTokenType::TUPLE)
			{
				bool n3 = false;
				GrapaRuleEvent* jl = pattr->vQueue->Head();
				while ((n3 == false) && (jl))
				{
					if (jl->vQueue && jl->vQueue->mCount)
					{
						bool n2 = false;
						GrapaRuleEvent* m = jl->vQueue->Head();
						while ((n2 == false) && (m))
						{
							index = 0;
							GrapaRuleEvent* at = b->vQueue->Head(0)->vQueue->Search(m->mName, index);
							if (at == NULL || at->mValue.Cmp(m->mValue) != 0)
								n2 = true;
							m = m->Next();
						};
						if (n2 == false)
							n3 = true;
					}
					jl = jl->Next();
				};
				if (n3 == false)
					n = true;
			};
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* pand = p->vQueue->Search(GrapaCHAR("and"), index);
		if (pand && pand->vQueue && pand->vQueue->mCount)
		{
			n = false;
			GrapaRuleEvent* m = pand->vQueue->Head();
			while ((n == false) && (m))
			{
				n = MatchTAG(vScriptExec, b, m);
				m = m->Next();
			};
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* nand = p->vQueue->Search(GrapaCHAR("nand"), index);
		if (nand && nand->vQueue && nand->vQueue->mCount)
		{
			n = false;
			GrapaRuleEvent* m = nand->vQueue->Head();
			while ((n == false) && (m))
			{
				n = MatchTAG(vScriptExec, b, m);
				m = m->Next();
			};
			n = !n;
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* por = p->vQueue->Search(GrapaCHAR("or"), index);
		if (por && por->vQueue && por->vQueue->mCount)
		{
			n = true;
			GrapaRuleEvent* m = por->vQueue->Head();
			while ((n == true) && (m))
			{
				n = MatchTAG(vScriptExec, b, m);
				m = m->Next();
			};
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* nor = p->vQueue->Search(GrapaCHAR("nor"), index);
		if (nor && nor->vQueue && nor->vQueue->mCount)
		{
			n = true;
			GrapaRuleEvent* m = nor->vQueue->Head();
			while ((n == true) && (m))
			{
				n = MatchTAG(vScriptExec, b, m);
				m = m->Next();
			};
			n = !n;
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* has = p->vQueue->Search(GrapaCHAR("has"), index);
		if (has && has->vQueue && has->vQueue->mCount)
		{
			if (b->vQueue && b->vQueue->mCount > 1)
			{
				n = true;
				if (has->mValue.mToken == GrapaTokenType::LIST)
				{
					GrapaRuleEvent* m = has;
					GrapaRuleEvent* jm = b->vQueue->Head(1)->vQueue->Head();
					while ((n == true) && (jm))
					{
						if (jm->mValue.mToken == GrapaTokenType::TAG)
							n = MatchTAG(vScriptExec, jm->vQueue->Head(), m, true);
						jm = jm->Next();
					};
				}
				else
				{
					GrapaRuleEvent* m = has->vQueue->Head();
					while ((n == true) && (m))
					{
						GrapaRuleEvent* jm = b->vQueue->Head(1)->vQueue->Head();
						while ((n == true) && (jm))
						{
							if (jm->mValue.mToken == GrapaTokenType::TAG)
								n = MatchTAG(vScriptExec, jm->vQueue->Head(), m, true);
							jm = jm->Next();
						};
						m = m->Next();
					};
				};
			}
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* has = p->vQueue->Search(GrapaCHAR("nhas"), index);
		if (has && has->vQueue && has->vQueue->mCount)
		{
			if (b->vQueue && b->vQueue->mCount > 1)
			{
				n = true;
				if (has->mValue.mToken == GrapaTokenType::LIST)
				{
					GrapaRuleEvent* m = has;
					GrapaRuleEvent* jm = b->vQueue->Head(1)->vQueue->Head();
					while ((n == true) && (jm))
					{
						if (jm->mValue.mToken == GrapaTokenType::TAG)
							n = MatchTAG(vScriptExec, jm->vQueue->Head(), m, true);
						jm = jm->Next();
					};
				}
				else
				{
					GrapaRuleEvent* m = has->vQueue->Head();
					while ((n == true) && (m))
					{
						GrapaRuleEvent* jm = b->vQueue->Head(1)->vQueue->Head();
						while ((n == true) && (jm))
						{
							if (jm->mValue.mToken == GrapaTokenType::TAG)
								n = MatchTAG(vScriptExec, jm->vQueue->Head(), m, true);
							jm = jm->Next();
						};
						m = m->Next();
					};
				};
			}
			n = !n;
		};
	};
	if (n == true && all && b->vQueue && b->vQueue->mCount > 1)
	{
		GrapaRuleEvent* jm = b->vQueue->Head(1)->vQueue->Head();
		while ((n == true) && (jm))
		{
			if (jm->mValue.mToken == GrapaTokenType::TAG)
				n = MatchTAG(vScriptExec, jm->vQueue->Head(), p, true);
			jm = jm->Next();
		}
	}
	return(n);
};

bool MatchLIST(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b,  GrapaRuleEvent* p, bool all=false)
{
	s64 index = 0;
	bool n = false;
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* pname = p->vQueue->Search(GrapaCHAR("name"), index);
		if (pname)
		{
			bool n2 = true;
			if ((pname->mValue.mToken == GrapaTokenType::STR) || (pname->mValue.mToken == GrapaTokenType::ID))
			{
				if (pname->mValue.Cmp(b->mName) == 0)
					n2 = false;
			}
			else if (pname->vQueue)
			{
				GrapaCHAR v(b->mName);
				GrapaRuleEvent* m = pname->vQueue->Head();
				while ((n2 == true) && (m))
				{
					if (v.Cmp(m->mValue) == 0)
						n2 = false;
					m = m->Next();
				};
			};
			n = n2;
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* pvalue = p->vQueue->Search(GrapaCHAR("value"), index);
		if (pvalue)
		{
			bool n2 = true;
			if ((b->mValue.mToken == GrapaTokenType::STR) || (b->mValue.mToken == GrapaTokenType::ID))
			{
				if ((pvalue->mValue.mToken == GrapaTokenType::STR) || (pvalue->mValue.mToken == GrapaTokenType::ID))
				{
					GrapaCHAR bc(b->mValue);
					bc.Trim();
					if (pvalue->mValue.Cmp(bc) == 0)
						n2 = false;
				}
				else if (pvalue->vQueue)
				{
					GrapaRuleEvent* m = pvalue->vQueue->Head();
					while ((n2 == true) && (m))
					{
						if ((m->mValue.mToken == GrapaTokenType::STR) || (m->mValue.mToken == GrapaTokenType::ID))
						{
							GrapaCHAR bc(b->mValue);
							bc.Trim();
							if (m->mValue.Cmp(bc) == 0)
								n2 = false;
						}
						m = m->Next();
					};
				};
			};
			n = n2;
		};
	};
	if (n == false && b->mValue.mToken == GrapaTokenType::LIST && b->vQueue)
	{
		index = 0;
		GrapaRuleEvent* pattr = p->vQueue->Search(GrapaCHAR("attr"), index);
		if (pattr && pattr->vQueue && pattr->vQueue->mCount)
		{
			if (pattr->mValue.mToken == GrapaTokenType::LIST)
			{
				n = false;
				GrapaRuleEvent* m = pattr->vQueue->Head();
				while ((n == false) && (m))
				{
					index = 0;
					GrapaRuleEvent* at = b->vQueue->Search(m->mName, index);
					if (at == NULL || at->mValue.Cmp(m->mValue) != 0)
						n = true;
					m = m->Next();
				};
			}
			else if (pattr->mValue.mToken == GrapaTokenType::ARRAY || pattr->mValue.mToken == GrapaTokenType::TUPLE)
			{
				bool n3 = false;
				GrapaRuleEvent* jl = pattr->vQueue->Head();
				while ((n3 == false) && (jl))
				{
					if (jl->vQueue && jl->vQueue->mCount)
					{
						bool n2 = false;
						GrapaRuleEvent* m = jl->vQueue->Head();
						while ((n2 == false) && (m))
						{
							index = 0;
							GrapaRuleEvent* at = b->vQueue->Search(m->mName, index);
							if (at == NULL || at->mValue.Cmp(m->mValue) != 0)
								n2 = true;
							m = m->Next();
						};
						if (n2 == false)
							n3 = true;
					}
					jl = jl->Next();
				};
				if (n3 == false)
					n = true;
			};
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* pand = p->vQueue->Search(GrapaCHAR("and"), index);
		if (pand && pand->vQueue && pand->vQueue->mCount)
		{
			bool n2 = false;
			GrapaRuleEvent* m = pand->vQueue->Head();
			while ((n2 == false) && (m))
			{
				n2 = MatchLIST(vScriptExec, b, m);
				m = m->Next();
			};
			n = n2;
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* nand = p->vQueue->Search(GrapaCHAR("nand"), index);
		if (nand && nand->vQueue && nand->vQueue->mCount)
		{
			bool n2 = false;
			GrapaRuleEvent* m = nand->vQueue->Head();
			while ((n2 == false) && (m))
			{
				n2 = MatchLIST(vScriptExec, b, m);
				m = m->Next();
			};
			n = !n2;
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* por = p->vQueue->Search(GrapaCHAR("or"), index);
		if (por && por->vQueue && por->vQueue->mCount)
		{
			bool n2 = true;
			GrapaRuleEvent* m = por->vQueue->Head();
			while ((n2 == true) && (m))
			{
				n2 = MatchLIST(vScriptExec, b, m);
				m = m->Next();
			};
			n = n2;
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* nor = p->vQueue->Search(GrapaCHAR("nor"), index);
		if (nor && nor->vQueue && nor->vQueue->mCount)
		{
			bool n2 = true;
			GrapaRuleEvent* m = nor->vQueue->Head();
			while ((n2 == true) && (m))
			{
				n2 = MatchLIST(vScriptExec, b, m);
				m = m->Next();
			};
			n = !n2;
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* has = p->vQueue->Search(GrapaCHAR("has"), index);
		if (has && has->vQueue && has->vQueue->mCount)
		{
			n = true;
			if (has->mValue.mToken == GrapaTokenType::LIST)
			{
				GrapaRuleEvent* m = has;
				GrapaRuleEvent* jm = b->vQueue->mCount ? b->vQueue->Head() : NULL;
				while ((n == true) && (jm))
				{
					n = MatchLIST(vScriptExec, jm, m, true);
					jm = jm->Next();
				};
			}
			else
			{
				GrapaRuleEvent* m = has->vQueue->Head();
				while ((n == true) && (m))
				{
					GrapaRuleEvent* jm = b->vQueue->mCount ? b->vQueue->Head() : NULL;
					while ((n == true) && (jm))
					{
						n = MatchLIST(vScriptExec, jm, m, true);
						jm = jm->Next();
					};
					m = m->Next();
				};
			}
		};
	};
	if (n == false)
	{
		index = 0;
		GrapaRuleEvent* has = p->vQueue->Search(GrapaCHAR("nhas"), index);
		if (has && has->vQueue && has->vQueue->mCount)
		{
			n = true;
			if (has->mValue.mToken == GrapaTokenType::LIST)
			{
				GrapaRuleEvent* m = has;
				GrapaRuleEvent* jm = b->vQueue->mCount ? b->vQueue->Head() : NULL;
				while ((n == true) && (jm))
				{
					n = MatchLIST(vScriptExec, jm, m, true);
					jm = jm->Next();
				};
			}
			else
			{
				GrapaRuleEvent* m = has->vQueue->Head();
				while ((n == true) && (m))
				{
					GrapaRuleEvent* jm = b->vQueue->mCount ? b->vQueue->Head() : NULL;
					while ((n == true) && (jm))
					{
						n = MatchLIST(vScriptExec, jm, m, true);
						jm = jm->Next();
					};
					m = m->Next();
				};
			}
			n = !n;
		};
	};
	if (n == true && all && b->vQueue && b->vQueue->mCount)
	{
		GrapaRuleEvent* jm = b->vQueue->Head();
		while ((n == true) && (jm))
		{
			n = MatchLIST(vScriptExec, jm, p, true);
			jm = jm->Next();
		};
	}
	return(n);
};

GrapaRuleEvent* FindAll(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p)
{
	GrapaRuleEvent* a = NULL;
	while (b->mValue.mToken == GrapaTokenType::PTR && b->vRulePointer) b = b->vRulePointer;
	if ((b && b->vQueue) && (p && p->vQueue))
	{
		if (b->mValue.mToken == GrapaTokenType::XML)
			a = new GrapaRuleEvent(GrapaTokenType::XML, 0, "", "");
		else if (b->mValue.mToken == GrapaTokenType::TAG)
			a = new GrapaRuleEvent(GrapaTokenType::XML, 0, "", "");
		else if (b->mValue.mToken == GrapaTokenType::LIST)
			a = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "", "");
		else if (b->mValue.mToken == GrapaTokenType::TUPLE)
			a = new GrapaRuleEvent(GrapaTokenType::TUPLE, 0, "", "");
		else
			a = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
		a->vQueue = new GrapaRuleQueue();
		s64 index = 0;
		if (b->mValue.mToken == GrapaTokenType::TAG)
		{
			bool n = MatchTAG(vScriptExec, b->vQueue->Head(), p);
			if (n == false)
			{
				a->vQueue->PushTail(vScriptExec->CopyItem(b));
			}
			if ((n == true) && (b->vQueue->Head()->vQueue->mCount > 1 && b->vQueue->Head()->vQueue->Head(1) && b->vQueue->Head()->vQueue->Head(1)->vQueue))
			{
				GrapaRuleEvent* m = b->vQueue->Head()->vQueue->Head(1)->vQueue->Head();
				while (m)
				{
					GrapaRuleEvent* e = FindAll(vScriptExec, m, p);
					if (e && e->vQueue && e->vQueue->mCount)
					{
						while (e->vQueue->Head())
							a->vQueue->PushTail(e->vQueue->PopHead());
					}
					if (e)
					{
						e->CLEAR();
						delete e;
					}
					m = m->Next();
				};
			};
		}
		else if (b->mValue.mToken == GrapaTokenType::XML)
		{
			GrapaRuleEvent* m = b->vQueue->Head();
			while (m)
			{
				if (m->mValue.mToken == GrapaTokenType::TAG)
				{
					GrapaRuleEvent* e = FindAll(vScriptExec, m, p);
					if (e && e->vQueue && e->vQueue->mCount)
					{
						while (e->vQueue->Head())
							a->vQueue->PushTail(e->vQueue->PopHead());
					}
					if (e)
					{
						e->CLEAR();
						delete e;
					}
				}
				m = m->Next();
			};
		}
		else if (b->mValue.mToken == GrapaTokenType::LIST)
		{
			bool n = MatchLIST(vScriptExec, b, p);
			if (n == false)
			{
				a->vQueue->PushTail(vScriptExec->CopyItem(b));
			}
			if (n == true)
			{
				GrapaRuleEvent* jm = b->vQueue->Head();
				while (jm)
				{
					if (jm->mValue.mToken == GrapaTokenType::LIST || jm->mValue.mToken == GrapaTokenType::ARRAY || jm->mValue.mToken == GrapaTokenType::TUPLE || jm->mValue.mToken == GrapaTokenType::XML || jm->mValue.mToken == GrapaTokenType::TAG)
					{
						GrapaRuleEvent* e = FindAll(vScriptExec, jm, p);
						if (e && e->vQueue && e->vQueue->mCount)
						{
							while (e->vQueue->Head())
							{
								GrapaRuleEvent* ep = e->vQueue->PopHead();
								if ((a->mValue.mToken == GrapaTokenType::ARRAY || a->mValue.mToken == GrapaTokenType::TUPLE) && ep->mValue.mToken == GrapaTokenType::LIST)
									a->mValue.mToken = GrapaTokenType::LIST;
								a->vQueue->PushTail(ep);
							}
						}
						if (e)
						{
							e->CLEAR();
							delete e;
						}
					}
					jm = jm->Next();
				}
			};
		}
		else if (b->mValue.mToken == GrapaTokenType::ARRAY || b->mValue.mToken == GrapaTokenType::TUPLE)
		{
			GrapaRuleEvent* jm = b->vQueue->Head();
			while (jm)
			{
				if (jm->mValue.mToken == GrapaTokenType::LIST || jm->mValue.mToken == GrapaTokenType::ARRAY || jm->mValue.mToken == GrapaTokenType::TUPLE || jm->mValue.mToken == GrapaTokenType::XML || jm->mValue.mToken == GrapaTokenType::TAG)
				{
					bool n = MatchLIST(vScriptExec, jm, p);
					if (n == false)
					{
						if ((a->mValue.mToken == GrapaTokenType::ARRAY || a->mValue.mToken == GrapaTokenType::TUPLE) && b->mValue.mToken == GrapaTokenType::LIST)
							a->mValue.mToken = GrapaTokenType::LIST;
						a->vQueue->PushTail(vScriptExec->CopyItem(jm));
					}
					if (n == true)
					{
						GrapaRuleEvent* e = FindAll(vScriptExec, jm, p);
						if (e && e->vQueue && e->vQueue->mCount)
						{
							while (e->vQueue->Head())
							{
								GrapaRuleEvent* ep = e->vQueue->PopHead();
								if ((a->mValue.mToken == GrapaTokenType::ARRAY || a->mValue.mToken == GrapaTokenType::TUPLE) && ep->mValue.mToken == GrapaTokenType::LIST)
									a->mValue.mToken = GrapaTokenType::LIST;
								a->vQueue->PushTail(ep);
							}
						}
						if (e)
						{
							e->CLEAR();
							delete e;
						}
					}
				}
				jm = jm->Next();
			};
		}
	}
	return(a);
};

GrapaRuleEvent* GrapaLibraryRuleFindAllEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaRuleEvent* b = r1.vVal;
	GrapaRuleEvent* p = r2.vVal;
	result = FindAll(vScriptExec, b, p);
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
};

struct CompareOPStruct
{
	GrapaScriptExec* vScriptExec;
	GrapaNames* pNameSpace;
	GrapaRuleEvent* op;
	GrapaRuleQueue* params;
	GrapaRuleEvent* ap;
	GrapaRuleEvent* bp;
	bool mDecend, mZero, mKeyOnly;
};

int GrapaLibraryItemSortEventCompareValue(const void *arg1, const void *arg2)
{
	CompareOPStruct* cs = (CompareOPStruct*)(((GrapaRuleEvent**)arg2)[1]);
	GrapaRuleEvent* a = *(GrapaRuleEvent**)arg1;
	GrapaRuleEvent* b = *(GrapaRuleEvent**)arg2;
	int cm = 0;
	
	switch (a->mValue.mToken)
	{
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
	case GrapaTokenType::STR:
	case GrapaTokenType::SYSSTR:
		if (cs->mZero)
		{
			cm = (int)a->mValue.StrUpperCmp(b->mValue);
			if (cm==0)
				cm = (int)(a->mValue.StrCmp(b->mValue));
		}
		else
		{
			cm = (int)(a->mValue.StrCmp(b->mValue));
		}
		if (cm == 0) return 0;
		return cs->mDecend ? -cm : cm;
	case GrapaTokenType::INT:
	case GrapaTokenType::SYSINT:
	case GrapaTokenType::BOOL:
		switch (b->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::BOOL:
			{
				GrapaInt aa, bb;
				aa.FromBytes(a->mValue);
				bb.FromBytes(b->mValue);
				if (aa == bb) return 0;
				if (cs->mZero)
				{
					if (aa.Abs() > bb.Abs()) return cs->mDecend ? -1 : 1;
				}
				else
				{
					if (aa > bb) return cs->mDecend ? -1 : 1;
				}
				return cs->mDecend ? 1 : -1;
			}
			break;
		default:
			cm = (int)(a->mValue.StrCmp(b->mValue));
			if (cm == 0) return 0;
			return cs->mDecend ? -cm : cm;
		}
		break;
	case GrapaTokenType::FLOAT:
		switch (b->mValue.mToken)
		{
		case GrapaTokenType::FLOAT:
			{
				GrapaFloat aa(cs->vScriptExec->vScriptState->mItemState.mFloatFix, cs->vScriptExec->vScriptState->mItemState.mFloatMax, cs->vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				GrapaFloat bb(cs->vScriptExec->vScriptState->mItemState.mFloatFix, cs->vScriptExec->vScriptState->mItemState.mFloatMax, cs->vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(a->mValue);
				bb.FromBytes(b->mValue);
				if (aa == bb) return 0;
				if (cs->mZero)
				{
					if (aa.Abs() > bb.Abs()) return cs->mDecend ? -1 : 1;
				}
				else
				{
					if (aa > bb) return cs->mDecend ? -1 : 1;
				}
				return cs->mDecend ? 1 : -1;
		}
			break;
		default:
			return (int)(a->mValue.StrCmp(b->mValue));
		}
		break;
	case GrapaTokenType::TAG:
		switch (b->mValue.mToken)
		{
		case GrapaTokenType::TAG:
			return (int)(a->vQueue->Head()->mName.StrCmp(b->vQueue->Head()->mName));
			break;
		}
		break;
	case GrapaTokenType::ARRAY:
	case GrapaTokenType::TUPLE:
	case GrapaTokenType::LIST:
	case GrapaTokenType::XML:
	case GrapaTokenType::EL:
	case GrapaTokenType::OP:
	case GrapaTokenType::CODE:
	case GrapaTokenType::OBJ:
		if ((a->vQueue == NULL || a->vQueue->mCount==0) && (b->vQueue == NULL || b->vQueue->mCount==0))
			return 0;
		if (a->vQueue == NULL || a->vQueue->mCount == 0)
			return cs->mDecend ? 1 : -1;
		if (b->vQueue == NULL || b->vQueue->mCount == 0)
			return cs->mDecend ? -1 : 1;
		if (cs->mKeyOnly)
			return 0;
		a = a->vQueue->Head();
		b = b->vQueue->Head();
		while (a && b)
		{
			GrapaRuleEvent* aa[2];
			GrapaRuleEvent* bb[2];
			aa[0] = a;
			aa[1] = (GrapaRuleEvent*)cs;
			bb[0] = b;
			bb[1] = (GrapaRuleEvent*)cs;
			int m = GrapaLibraryItemSortEventCompareValue(&aa, &bb);
			if (m) return m;
			a = a->Next();
			b = b->Next();
		}
		if (a == NULL && b == NULL)
			return 0;
		if (a == NULL)
			return cs->mDecend ? 1 : -1;
		if (b == NULL)
			return cs->mDecend ? -1 : 1;
		break;
	case GrapaTokenType::VECTOR:
		break;
	case GrapaTokenType::WIDGET:
		break;
	}
	return(0);
}

int GrapaLibraryItemSortEventCompareName(const void *arg1, const void *arg2)
{
	CompareOPStruct* cs = (CompareOPStruct*)((GrapaRuleEvent**)arg2)[1];
	int v = (int)(*(GrapaRuleEvent**)arg1)->mName.StrCmp((*(GrapaRuleEvent**)arg2)->mName);
	if (v == 0 && cs->mKeyOnly)
		return 0;
	if (v == 0) v = GrapaLibraryItemSortEventCompareValue(arg1, arg2);
	return(cs->mDecend ? -v : v);
}

int GrapaLibraryItemSortEventCompareOP(const void* arg1, const void* arg2)
{
	CompareOPStruct* cs = (CompareOPStruct*)((GrapaRuleEvent**)arg2)[1];
	cs->ap->vRulePointer = ((GrapaRuleEvent**)arg1)[0];
	cs->bp->vRulePointer = ((GrapaRuleEvent**)arg2)[0];
	GrapaRuleEvent* temp = cs->vScriptExec->ProcessPlan(cs->pNameSpace, cs->op, cs->params->Head());
	int v = -1;
	if (temp)
	{
		if (temp->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt a;
			a.FromBytes(temp->mValue);
			v = (int)a.LongValue();
		}
		delete temp;
	}
	return(cs->mDecend ? -v : v);
}

GrapaRuleEvent* GrapaLibraryRuleSortEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam raxis(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam rorder(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam rkind(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	s64 axis = 0;
	if (raxis.vVal && raxis.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(raxis.vVal->mValue);
		axis = a.LongValue();
	}
	bool unsig = false;
	if (rkind.vVal)
	{
		if (!rkind.vVal->IsZero())
			unsig = true;
	}
	bool decend = false;
	GrapaVector* orderV = NULL;
	GrapaVector orderR;
	if (rorder.vVal)
	{
		if (!rorder.vVal->IsZero())
			decend = true;
		if (rorder.vVal && rorder.vVal->mValue.mToken == GrapaTokenType::VECTOR)
			orderV = rorder.vVal->vVector;
		else if (rorder.vVal && (rorder.vVal->mValue.mToken == GrapaTokenType::ARRAY || rorder.vVal->mValue.mToken == GrapaTokenType::TUPLE))
		{
			orderR.FROM(vScriptExec, rorder.vVal, 1);
			orderV = &orderR;
		}
	}
	GrapaRuleEvent* rx = r1.vVal;
	if (rx && rx->mValue.mToken == GrapaTokenType::TAG)
		rx = rx->vQueue->Head()->vQueue->Head(1);
	if (rx && axis==0)
	{
		// [b,a,B,c,b,A].sort(op(a,b){@a.upper()<=>@b.upper();})
		// [b,a,B,c,b,A].sort(op(a,b){@local.c=@a.upper()<=>@b.upper();if(@c==0)c=@a<=>@b;@c;})
		switch (rx->mValue.mToken)
		{
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
			if (rx->vQueue && orderV)
			{
				if (orderV->mDim == 1 && orderV->mCounts && orderV->mData)
				{
					result = new GrapaRuleEvent(rx->mValue.mToken, 0, "", "");
					result->vQueue = new GrapaRuleQueue();
					for (u64 i = 0; i < orderV->mCounts[0]; i++)
					{
						GrapaVectorParam p1(vScriptExec, orderV->mData, orderV->mBlock, (i));
						s64 pos = -1;
						if (p1.a)
						{
							pos = p1.aa->ToInt().LongValue();
							if (pos < 0) pos = ((s64)rx->vQueue->mCount) + pos;
							if (pos < 0) pos = 0;
							if (pos > (s64)rx->vQueue->mCount) pos = rx->vQueue->mCount;
						}
						GrapaRuleEvent* r = NULL;
						if (pos >= 0 && pos < (s64)rx->vQueue->mCount)
							r = rx->vQueue->Head(pos);
						if (r)
							result->vQueue->PushTail(vScriptExec->CopyItem(r));
						else
						{
							r = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
							r->SetNull();
							result->vQueue->PushTail(r);
						}
					}
				}
			}
			else if (rx->vQueue)
			{
				result = vScriptExec->CopyItem(rx);
				GrapaRuleQueue* r = (GrapaRuleQueue*)result->vQueue;
				GrapaRuleEvent** rq = (GrapaRuleEvent**)GrapaMem::Create(r->mCount * (sizeof(GrapaRuleEvent*) * 3));
				GrapaRuleEvent* rh = r->Head();
				GrapaRuleQueue params;
				CompareOPStruct cs;
				cs.vScriptExec = vScriptExec;
				cs.pNameSpace = pNameSpace;
				cs.op = rkind.vVal;
				cs.params = &params;
				cs.ap = NULL;
				cs.bp = NULL;
				cs.mDecend = decend;
				cs.mKeyOnly = false;
				cs.mZero = unsig;
				if (rkind.vVal && rkind.vVal->mValue.mToken == GrapaTokenType::OP)
				{
					cs.ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.ap);
					cs.bp = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.bp);
				}
				u64 i = 0;
				while (rh)
				{
					rq[i] = rh;
					rq[i + 1] = (GrapaRuleEvent*)&cs;
					rq[i + 2] = (GrapaRuleEvent*)(i/3);
					rh = rh->Next();
					r->PopHead();
					i += 3;
				}
				if (rkind.vVal && rkind.vVal->mValue.mToken == GrapaTokenType::OP)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareOP);
				else if (result->mValue.mToken == GrapaTokenType::ARRAY || result->mValue.mToken == GrapaTokenType::TUPLE || result->mValue.mToken == GrapaTokenType::XML)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareValue);
				else
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareName);
				while (i)
				{
					i -= 3;
					r->PushHead((GrapaRuleEvent*)rq[i]);
				}
				GrapaMem::Delete(rq);
			}
			break;
		case GrapaTokenType::VECTOR:
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR,0,"","");
			result->vVector = new GrapaVector();
			if (!rx->vVector->Sort(vScriptExec, pNameSpace, NULL, orderV, NULL, true, unsig, decend, rkind.vVal, *result->vVector))
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
			break;
		default:
			break;
		}
	}
	else if (rx && axis == 1)
	{
		GrapaVector v;
		switch (rx->mValue.mToken)
		{
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
			if (v.FROM(vScriptExec, rx, 0))
			{
				GrapaVector v3;
				if (!v.Sort(vScriptExec, pNameSpace, NULL, orderV, NULL, false, unsig, decend, rkind.vVal, v3)) break;
				result = v3.ToArray();
			}
			break;
		case GrapaTokenType::VECTOR:
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
			result->vVector = new GrapaVector();
			if (!rx->vVector->Sort(vScriptExec, pNameSpace, NULL, orderV, NULL, false, unsig, decend, rkind.vVal, *result->vVector))
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleArgSortEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam raxis(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam rorder(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam rkind(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	s64 axis = 0;
	if (raxis.vVal && raxis.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(raxis.vVal->mValue);
		axis = a.LongValue();
	}
	bool decend = false;
	if (rorder.vVal)
	{
		if (!rorder.vVal->IsZero())
			decend = true;
	}
	bool unsig = false;
	if (rkind.vVal)
	{
		if (!rkind.vVal->IsZero())
			unsig = true;
	}
	GrapaRuleEvent* rx = r1.vVal;
	if (rx && rx->mValue.mToken == GrapaTokenType::TAG)
		rx = rx->vQueue->Head()->vQueue->Head(1);
	if (rx && axis == 0)
	{
		GrapaVector v,v2;
		switch (rx->mValue.mToken)
		{
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
			if (rx->vQueue)
			{
				GrapaRuleQueue* r = (GrapaRuleQueue*)rx->vQueue;
				void** rq = (void**)GrapaMem::Create(r->mCount * (sizeof(GrapaRuleEvent*) * 3));
				GrapaRuleEvent* rh = r->Head();
				GrapaRuleQueue params;
				CompareOPStruct cs;
				cs.vScriptExec = vScriptExec;
				cs.pNameSpace = pNameSpace;
				cs.op = rkind.vVal;
				cs.params = &params;
				cs.ap = NULL;
				cs.bp = NULL;
				cs.mDecend = decend;
				cs.mZero = unsig;
				cs.mKeyOnly = false;
				if (rkind.vVal && rkind.vVal->mValue.mToken == GrapaTokenType::OP)
				{
					cs.ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.ap);
					cs.bp = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.bp);
				}
				u64 i = 0;
				while (rh)
				{
					rq[i] = (void*)rh;
					rq[i + 1] = (void*)&cs;
					rq[i + 2] = (void*)(i / 3);
					rh = rh->Next();
					i += 3;
				}
				if (rkind.vVal && rkind.vVal->mValue.mToken == GrapaTokenType::OP)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareOP);
				else if (rx->mValue.mToken == GrapaTokenType::ARRAY || rx->mValue.mToken == GrapaTokenType::TUPLE || rx->mValue.mToken == GrapaTokenType::XML)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareValue);
				else
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareName);
				result = new GrapaRuleEvent(GrapaTokenType::ARRAY,0,"","");
				result->vQueue = new GrapaRuleQueue();
				while (i)
				{
					i -= 3;
					result->vQueue->PushHead(new GrapaRuleEvent(0, GrapaCHAR(""), GrapaInt((u64)rq[i + 2]).getBytes()));
				}
				GrapaMem::Delete(rq);
			}
			break;
		case GrapaTokenType::VECTOR:
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
			result->vVector = new GrapaVector();
			if (!rx->vVector->Sort(vScriptExec, pNameSpace, NULL, NULL, result->vVector, true, unsig, decend, rkind.vVal, *result->vVector))
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
			break;
		default:
			break;
		}
	}
	else if (rx && axis == 1)
	{
		GrapaVector v,v2,v3;
		switch (rx->mValue.mToken)
		{
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
			if (v.FROM(vScriptExec, rx, 0))
			{
				if (!v.Sort(vScriptExec, pNameSpace, NULL, NULL, &v2, false, unsig, decend, rkind.vVal, v3)) break;
				result = v2.ToArray();
			}
			break;
		case GrapaTokenType::VECTOR:
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
			result->vVector = new GrapaVector();
			if (!rx->vVector->Sort(vScriptExec, pNameSpace, NULL, NULL, result->vVector, false, unsig, decend, rkind.vVal, v3))
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleUniqueEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaRuleEvent* rx = r1.vVal;
	if (rx && rx->mValue.mToken == GrapaTokenType::TAG)
		rx = rx->vQueue->Head()->vQueue->Head(1);
	if (rx)
	{
		// [b,a,B,c,b,A].sort(op(a,b){@a.upper()<=>@b.upper();})
		// [b,a,B,c,b,A].sort(op(a,b){@local.c=@a.upper()<=>@b.upper();if(@c==0)c=@a<=>@b;@c;})
		switch (rx->mValue.mToken)
		{
		case GrapaTokenType::PTR:
			break;
		case GrapaTokenType::VECTOR:
			break;
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
			if (rx->vQueue)
			{
				result = vScriptExec->CopyItem(rx);
				GrapaRuleQueue* r = (GrapaRuleQueue*)result->vQueue;
				GrapaRuleEvent** rq = (GrapaRuleEvent**)GrapaMem::Create(r->mCount * (sizeof(GrapaRuleEvent*) * 3));
				GrapaRuleEvent* rh = r->Head();
				GrapaRuleQueue params;
				CompareOPStruct cs;
				cs.vScriptExec = vScriptExec;
				cs.pNameSpace = pNameSpace;
				cs.op = r2.vVal;
				cs.params = &params;
				cs.ap = NULL;
				cs.bp = NULL;
				cs.mDecend = false;
				cs.mZero = false;
				cs.mKeyOnly = false;
				if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::OP)
				{
					cs.ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.ap);
					cs.bp = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.bp);
				}
				u64 i = 0;
				while (rh)
				{
					rq[i] = rh;
					rq[i + 1] = (GrapaRuleEvent*)&cs;
					rq[i + 2] = (GrapaRuleEvent*)(i / 3);
					rh = rh->Next();
					r->PopHead();
					i += 3;
				}
				if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::OP)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareOP);
				else if (result->mValue.mToken == GrapaTokenType::ARRAY || result->mValue.mToken == GrapaTokenType::TUPLE || result->mValue.mToken == GrapaTokenType::XML)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareValue);
				else
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareName);
				GrapaRuleEvent* last = NULL;
				while (i)
				{
					i -= 3;
					bool match = false;
					if (last)
					{
						if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::OP)
						{
							cs.ap->vRulePointer = last;
							cs.bp->vRulePointer = (GrapaRuleEvent*)rq[i];
							GrapaRuleEvent* temp = vScriptExec->ProcessPlan(pNameSpace, r2.vVal, params.Head());
							if (temp)
							{
								if (temp->mValue.mToken == GrapaTokenType::INT)
									match = temp->IsZero();
								delete temp;
							}
						}
						else if (result->mValue.mToken == GrapaTokenType::ARRAY || result->mValue.mToken == GrapaTokenType::TUPLE || result->mValue.mToken == GrapaTokenType::XML)
						{
							match = (GrapaLibraryItemSortEventCompareValue(&last, &rq[i]) == 0);
						}
						else
						{
							match = (GrapaLibraryItemSortEventCompareName(&last, &rq[i]) == 0);
						}
					}
					if (match)
					{
						delete (GrapaRuleEvent*)rq[i];
						rq[i] = NULL;
					}
					else
					{
						r->PushHead((GrapaRuleEvent*)rq[i]);
						last = (GrapaRuleEvent*)rq[i];
						rq[i] = NULL;
					}
				}
				GrapaMem::Delete(rq);
			}
			break;
		default:
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGroupEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	GrapaRuleEvent* rx = r1.vVal;
	if (rx && rx->mValue.mToken == GrapaTokenType::TAG)
		rx = rx->vQueue->Head()->vQueue->Head(1);
	if (rx)
	{
		// [b,a,B,c,b,A].sort(op(a,b){@a.upper()<=>@b.upper();})
		// [b,a,B,c,b,A].sort(op(a,b){@local.c=@a.upper()<=>@b.upper();if(@c==0)c=@a<=>@b;@c;})
		switch (rx->mValue.mToken)
		{
		case GrapaTokenType::PTR:
			break;
		case GrapaTokenType::VECTOR:
			break;
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
			if (rx->vQueue)
			{
				result = vScriptExec->CopyItem(rx);
				GrapaRuleQueue* r = (GrapaRuleQueue*)result->vQueue;
				GrapaRuleEvent** rq = (GrapaRuleEvent**)GrapaMem::Create(r->mCount * (sizeof(GrapaRuleEvent*) * 3));
				GrapaRuleEvent* rh = r->Head();
				GrapaRuleQueue params;
				CompareOPStruct cs;
				cs.vScriptExec = vScriptExec;
				cs.pNameSpace = pNameSpace;
				cs.op = r2.vVal;
				cs.params = &params;
				cs.ap = NULL;
				cs.bp = NULL;
				cs.mDecend = false;
				cs.mZero = false;
				cs.mKeyOnly = true;
				if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::OP)
				{
					cs.ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.ap);
					cs.bp = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
					params.PushTail(cs.bp);
				}
				u64 i = 0;
				while (rh)
				{
					rq[i] = rh;
					rq[i + 1] = (GrapaRuleEvent*)&cs;
					rq[i + 2] = (GrapaRuleEvent*)(i / 3);
					rh = rh->Next();
					r->PopHead();
					i += 3;
				}
				if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::OP)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareOP);
				else if (result->mValue.mToken == GrapaTokenType::ARRAY || result->mValue.mToken == GrapaTokenType::TUPLE || result->mValue.mToken == GrapaTokenType::XML)
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareValue);
				else
					qsort(rq, i / 3, sizeof(GrapaRuleEvent*) * 3, GrapaLibraryItemSortEventCompareName);
				if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::OP)
					result->mValue.mToken = GrapaTokenType::LIST;
				else if (result->mValue.mToken == GrapaTokenType::ARRAY || result->mValue.mToken == GrapaTokenType::XML)
					result->mValue.mToken = GrapaTokenType::ARRAY;
				else if (result->mValue.mToken == GrapaTokenType::TUPLE)
					result->mValue.mToken = GrapaTokenType::TUPLE;
				else
					result->mValue.mToken = GrapaTokenType::LIST;
				GrapaRuleEvent* last = NULL;
				GrapaRuleEvent* key = NULL;
				while (i)
				{
					i -= 3;
					bool match = false;
					if (last)
					{
						if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::OP)
						{
							cs.ap->vRulePointer = last;
							cs.bp->vRulePointer = (GrapaRuleEvent*)rq[i];
							GrapaRuleEvent* temp = vScriptExec->ProcessPlan(pNameSpace, r2.vVal, params.Head());
							if (temp)
							{
								if (temp->mValue.mToken == GrapaTokenType::INT)
									match = temp->IsZero();
								delete temp;
							}
						}
						else if (result->mValue.mToken == GrapaTokenType::ARRAY || result->mValue.mToken == GrapaTokenType::TUPLE || result->mValue.mToken == GrapaTokenType::XML)
						{
							match = (GrapaLibraryItemSortEventCompareValue(&last, &rq[i]) == 0);
						}
						else
						{
							match = (GrapaLibraryItemSortEventCompareName(&last, &rq[i]) == 0);
						}
					}
					if (match)
					{
						if (r4.vVal && r4.vVal->mValue.mToken == GrapaTokenType::OP)
						{
							cs.ap->vRulePointer = rq[i];
							GrapaRuleEvent* temp = vScriptExec->ProcessPlan(pNameSpace, r4.vVal, params.Head(), 1);
							key->vQueue->PushTail(temp);
						}
						else
						{
							key->vQueue->PushTail((GrapaRuleEvent*)rq[i]);
						}
						rq[i] = NULL;
					}
					else
					{
						key = new GrapaRuleEvent();
						key->mValue.mToken = GrapaTokenType::ARRAY;
						key->vQueue = new GrapaRuleQueue();
						r->PushHead(key);
						if (r4.vVal && r4.vVal->mValue.mToken == GrapaTokenType::OP)
						{
							cs.ap->vRulePointer = rq[i];
							GrapaRuleEvent* temp = vScriptExec->ProcessPlan(pNameSpace, r4.vVal, params.Head(), 1);
							key->vQueue->PushTail(temp);
						}
						else
						{
							key->vQueue->PushTail((GrapaRuleEvent*)rq[i]);
						}
						last = (GrapaRuleEvent*)rq[i];
						rq[i] = NULL;
						if (result->mValue.mToken == GrapaTokenType::LIST)
						{
							key->mName.FROM(last->mName);
							if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::OP)
							{
								cs.ap->vRulePointer = last;
								GrapaRuleEvent* temp = vScriptExec->ProcessPlan(pNameSpace, r3.vVal, params.Head(),1);
								if (temp)
								{
									key->mName.FROM(temp->mValue);
									delete temp;
								}
							}
						}
					}
				}
				GrapaMem::Delete(rq);
			}
			break;
		default:
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleClassEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaCHAR name;
	GrapaRuleEvent* o = NULL;
	if (r1.vVal)
	{
		if (r1.vVal->mValue.mToken == GrapaTokenType::SYSID || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR)
			if (r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] != '$')
				name.FROM("$");
		name.Append(r1.vVal->mValue);
	}
	if (name.mLength)
	{
		// don't set result...as this will not be returned, it is updating/setting the variable in the namespace
		o = vScriptExec->vScriptState->SearchVariable(pNameSpace, name);
		if (!o)
		{
			o = new GrapaRuleEvent(r1.vVal->mValue);
			o->mValue.mToken = GrapaTokenType::CLASS;
			pNameSpace->GetNameQueue()->PushTail(o);
		}
		if (o)
		{
			if (o->vQueue)
			{
				o->vQueue->CLEAR();
				delete o->vQueue;
				o->vQueue = NULL;
			}
			o->vRulePointer = NULL;
			o->mNull = false;
		}
	}
	else
	{
		o = new GrapaRuleEvent();
		o->mValue.mToken = GrapaTokenType::CLASS;
		result = o;
	}
	if (o)
	{
		if (r3.vVal)
		{
			if (r3.vVal->vQueue)
				o->vQueue = vScriptExec->CopyQueue((GrapaRuleQueue*)r3.vVal->vQueue);
		}
		if (r2.vVal)
		{
			if (r2.vVal->vQueue)
			{
				if (o->vQueue == NULL)
					o->vQueue = new GrapaRuleQueue();
				o->vQueue->PushHead(vScriptExec->CopyItem(r2.vVal));
			}
		}
		if (o->vQueue)
		{
			GrapaRuleEvent* p = (GrapaRuleEvent*)o->vQueue->Head();
			while (p)
			{
				switch (p->mValue.mToken)
				{
				case GrapaTokenType::ID:
				case GrapaTokenType::SYSID:
					name.SetLength(0);
					if (p->mValue.mToken == GrapaTokenType::SYSID)
						name.Append("$");
					name.Append(p->mValue);
					p->vClass = vScriptExec->vScriptState->GetClass(pNameSpace,name);
					break;
				}
				p = p->Next();
			}
			// may need to now calculate MRO (method resolution order)
			// use deapth first, but consider removing first occurances of a class found in multiple times in the MSO path. (Python style)
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleObjEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::CLASS:
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::OBJ;
			result->vClass = r1.vVal;
			if (r1.vDel && !r1.vDel->mVar)
			{
				result->vValueEvent = vScriptExec->CopyItem(r1.vVal);
				result->vClass = result->vValueEvent;
			}
			result->vQueue = new GrapaRuleQueue();
			vScriptExec->vScriptState->CopyClassVars((GrapaRuleQueue*)result->vQueue, result->vClass);
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGlobalEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;
	GrapaNames* par = pNameSpace->GetSkipThis();
	while (par->GetSkipParrent()) par = par->GetSkipParrent();
	GrapaRuleEvent* n = par->GetNameQueue()->Head();
	if (n)
	{
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::PTR;
		while (n->vRulePointer) n = n->vRulePointer;
		result->vRulePointer = n;
		result->mVar = true;
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleParentEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent *n = pNameSpace->GetNameQueue()->Tail();
	//while (n && n->vRulePointer == NULL) n = n->vRulePointer;
	if (n) 
	{
		if (n) n = n->Prev();
		//while (n && n->vRulePointer == NULL) n = n->Prev();
		if (n==NULL)
			n = pNameSpace->GetNameQueue()->Head();
	}
	if (n)
	{
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::PTR;
		while (n->vRulePointer) n = n->vRulePointer;
		result->vRulePointer = n;
		result->mVar = true;
		if (n && n->mValue.mToken != GrapaTokenType::OBJ)
			result->mLocal = true;
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleThisEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;
	GrapaNames* ns = pNameSpace;
	GrapaRuleEvent *n = ns->GetNameQueue()->Tail();
	while (n && n->vRulePointer == NULL) n = n->Prev();
	while (n == NULL && ns->GetParrent())
	{
		ns = ns->GetParrent();
		n = ns->GetNameQueue()->Tail();
		while (n && n->vRulePointer == NULL) n = n->Prev();
	}
	if (n == NULL) n = pNameSpace->GetNameQueue()->Head();
	if (n)
	{
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::PTR;
		while (n->vRulePointer) n = n->vRulePointer;
		result->vRulePointer = n;
		result->mVar = true;
		if (n && n->mValue.mToken != GrapaTokenType::OBJ)
			result->mLocal = true;
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLocalEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent *n = pNameSpace->GetNameQueue()->Tail();
	if (n)
	{
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::PTR;
		result->vRulePointer = n;
		result->mVar = true;
		result->mLocal = false;
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleStaticEvent::Optimize(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam)
{
	if (pParam && pParam->vQueue && pParam->vQueue->mCount == 1)
	{
		GrapaRuleEvent* p1 = (GrapaRuleEvent*)pParam->vQueue->Head();
		if (p1)
		{
			GrapaRuleEvent* rx1 = vScriptExec->ProcessPlan(pNameSpace, p1);
			pOperation->CLEAR();
			delete pOperation;
			pOperation = rx1;
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRuleConstEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
		result = vScriptExec->CopyItem(r1.vVal,false,true);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSetConstEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal)
	{
		if (r2.vVal && r2.vVal->mValue.mToken==GrapaTokenType::BOOL && r2.vVal->mValue.mBytes && r2.vVal->mValue.mLength && r2.vVal->mValue.mBytes[0] && r2.vVal->mValue.mBytes[0] != '0')
			r1.vVal->mConst = true;
		else
			r1.vVal->mConst = false;
		if (r1.vVal->vQueue)
			r1.vVal->vQueue->mConst = r1.vVal->mConst;
	}
	return(result);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleReturnEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	//if (vScriptExec->vScriptState->mActionParams.Tail())
	//{
	//	if (vScriptExec->vScriptState->mActionParams.Tail()->mRuleResultEvent)
	//	{
	//		GrapaRuleEvent *r = vScriptExec->vScriptState->mActionParams.Tail()->mRuleResultEvent;
	//		r->CLEAR();
	//		delete r;
	//		vScriptExec->vScriptState->mActionParams.Tail()->mRuleResultEvent = NULL;
	//	}
	GrapaRuleEvent* p1 = pInput->Head(0);
	if (p1->mValue.mToken == GrapaTokenType::OP)
	{
		GrapaRuleEvent* rx1 = vScriptExec->ProcessPlan(pNameSpace, p1);
		if (rx1)
		{
			//GrapaRuleEvent* r1 = rx1 ? (rx1->mValue.mToken == GrapaTokenType::PTR ? rx1->vRulePointer : rx1) : NULL;
			//vScriptExec->vScriptState->mActionParams.Tail()->mRuleResultEvent = new GrapaRuleEvent(r1->mId, r1->mTokenEvent->mName, r1->mTokenEvent->mValue, r1->mTokenEvent);
			//vScriptExec->vScriptState->mActionParams.Tail()->mRuleResultEvent->mValue.mToken = r1->mValue.mToken;
		}
	}
	//}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBreakEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
	result->mAbort = true;
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleIfEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	//GrapaRuleEvent* rx1 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(0));;
	//GrapaRuleEvent* r1 = rx1; while (r1 && r1->vRulePointer) r1 = r1->vRulePointer;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaRuleEvent* p4 = pInput->Head(3);
	if (p4 == NULL)
	{
		bool isTrue = true;
		if (r1.vVal == NULL || r1.vVal->IsNull() || r1.vVal->IsZero() || r1.vVal->mValue.mToken==GrapaTokenType::ERR)
			isTrue = false;
		if (isTrue)
		{
			GrapaRuleEvent* p2 = pInput->Head(1);
			if (p2)
				result = vScriptExec->ProcessPlan(pNameSpace, p2);
		}
		else
		{
			GrapaRuleEvent* p3 = pInput->Head(2);
			if (p3)
				result = vScriptExec->ProcessPlan(pNameSpace, p3);
		}
	}
	else
	{
		bool isNeg = true;
		bool isNull = true;
		bool isZero = r1.vVal->IsNullIsNegIsZero(isNeg, isNull);
		if (isZero)
		{
			GrapaRuleEvent* p3 = pInput->Head(2);
			if (p3)
				result = vScriptExec->ProcessPlan(pNameSpace, p3);
		}
		else if (isNeg)
		{
			GrapaRuleEvent* p2 = pInput->Head(1);
			if (p2)
				result = vScriptExec->ProcessPlan(pNameSpace, p2);
		}
		else
		{
			GrapaRuleEvent* p4 = pInput->Head(3);
			if (p4)
				result = vScriptExec->ProcessPlan(pNameSpace, p4);
		}
	}
	//if (rx1) { rx1->CLEAR(); delete rx1; }
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWhileEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	if (GrapaRuleEvent* p1 = pInput->Head(0))
	{
		while (true)
		{
			GrapaRuleEvent* rx1 = vScriptExec->ProcessPlan(pNameSpace, p1);
			GrapaRuleEvent* r1 = rx1 ? (rx1->mValue.mToken == GrapaTokenType::PTR ? rx1->vRulePointer : rx1) : NULL;
			bool isContinue = false;
			if (r1 && r1->mValue.mLength && r1->mValue.mBytes[0] && r1->mValue.mBytes[0] != '0' && !r1->IsNull())
			{
				if (GrapaRuleEvent* p2 = pInput->Head(1))
				{
					isContinue = true;
					if (GrapaRuleEvent* r2 = vScriptExec->ProcessPlan(pNameSpace, p2))
					{
						bool isAbort = r2->mAbort;
						r2->CLEAR();
						delete r2;
						if (isAbort) break;
					}
				}
			}
			if (rx1) { rx1->CLEAR(); delete rx1; }
			if (!isContinue) break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleScopeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	// push something onto variable stack to indicate a new scope
	if (GrapaRuleEvent* operation = vScriptExec->vScriptState->AddRuleOperation(pNameSpace->GetNameQueue(), "", ""))
	{
		if (GrapaRuleEvent* p = pInput->Head(0))
		{
			result = vScriptExec->ProcessPlan(pNameSpace, p);
			if (result && result->mValue.mToken == GrapaTokenType::PTR)
			{
				GrapaRuleEvent* old = result;
				result = vScriptExec->CopyItem(old);
				old->CLEAR();
				delete old;
			}
		}
		if (pNameSpace->GetNameQueue()->PopEvent(operation))
		{
			operation->CLEAR();
			delete operation;
			operation = NULL;
		}
	}
	if (GrapaRuleEvent* p = pInput->Head(1))
	{
		if (result)
		{
			result->CLEAR();
			delete result;
		}
		result = vScriptExec->ProcessPlan(pNameSpace, p);
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSwitchEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* rx1 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(0));
	GrapaRuleEvent* rx2 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(1));
	GrapaRuleEvent* r1 = rx1; while (r1 && r1->vRulePointer) r1 = r1->vRulePointer;
	GrapaRuleEvent* r2 = rx2; while (r2 && r2->vRulePointer) r2 = r2->vRulePointer;
	GrapaRuleEvent* caseItem = NULL;
	if (r2 && r2->vQueue)
		caseItem = (GrapaRuleEvent*)r2->vQueue->Head(0);
	GrapaRuleEvent* defaultItem = caseItem;
	if (caseItem)
		caseItem = caseItem->Next();
	bool isEqual = false;
	while (caseItem && caseItem->vQueue)
	{
		GrapaRuleEvent* caseCompare = vScriptExec->ProcessPlan(pNameSpace, (GrapaRuleEvent*)caseItem->vQueue->Head(0));
		if ((r1 == NULL || r1->IsNull()) && (caseCompare == NULL || caseCompare->IsNull())) isEqual = true;
		else if ((r1 == NULL || r1->IsNull()) || (caseCompare == NULL || caseCompare->IsNull())) isEqual = false;
		else if (r1->mId && r1->mId == caseCompare->mId) isEqual = true;
		else if (r1->mValue.StrCmp(caseCompare->mValue) == 0) isEqual = true;
		else if (!isEqual && r1 && caseCompare && r1->mValue.mBytes && caseCompare->mValue.mBytes)
		{
			GrapaInt a, b;
			if (r1->mValue.mToken == GrapaTokenType::INT && r1->mValue.mToken == caseCompare->mValue.mToken)
			{
				a.FromBytes(r1->mValue);
				b.FromBytes(caseCompare->mValue);
				if (a == b)
					isEqual = true;
			}
		}
		if (caseCompare)
		{
			caseCompare->CLEAR();
			delete caseCompare;
			caseCompare = NULL;
		}
		if (isEqual)
		{
			result = vScriptExec->ProcessPlan(pNameSpace, (GrapaRuleEvent*)caseItem->vQueue->Head(1));
			break;
		}
		caseItem = caseItem->Next();
	}
	if (!isEqual && defaultItem && defaultItem->vQueue && defaultItem->vQueue->Head(1))
	{
		result = vScriptExec->ProcessPlan(pNameSpace, (GrapaRuleEvent*)defaultItem->vQueue->Head(1));
	}
	if (rx1) { rx1->CLEAR(); delete rx1; }
	if (rx2) { rx2->CLEAR(); delete rx2; }
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCaseEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::LIST;
	result->vQueue = new GrapaRuleQueue();
	GrapaRuleEvent* p = pInput->Head();
	while (p)
	{
		GrapaRuleEvent* x = p;
		while (x->mValue.mToken == GrapaTokenType::OP && x->vRulePointer) x = x->vRulePointer;
		result->vQueue->PushTail(vScriptExec->CopyItem(x));
		p = p->Next();
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePlanEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* codeResult = NULL;

	GrapaRuleEvent *paramHead = pInput ? pInput->Head(1) : NULL, *paramHeadDel = NULL;
	while (paramHead && paramHead->mValue.mToken == GrapaTokenType::PTR) paramHead = paramHead->vRulePointer;
	if (paramHead && paramHead->mValue.mToken == GrapaTokenType::OP) { paramHead = vScriptExec->ProcessPlan(pNameSpace, paramHead); paramHeadDel = paramHead; }
	while (paramHead && paramHead->mValue.mToken == GrapaTokenType::PTR) paramHead = paramHead->vRulePointer;

	GrapaRuleEvent *scriptHead = pInput ? pInput->Head(0) : NULL, *scriptHeadDel = NULL;
	while (scriptHead && scriptHead->mValue.mToken == GrapaTokenType::PTR) scriptHead = scriptHead->vRulePointer;
	if (scriptHead && scriptHead->mValue.mToken == GrapaTokenType::OP) { scriptHead = vScriptExec->ProcessPlan(pNameSpace, scriptHead); scriptHeadDel = scriptHead; }
	while (scriptHead && scriptHead->mValue.mToken == GrapaTokenType::PTR) scriptHead = scriptHead->vRulePointer;

	GrapaRuleEvent *script = scriptHead;
	while (script && script->mValue.mToken == GrapaTokenType::PTR) script = script->vRulePointer;

	GrapaRuleEvent* rule = NULL;
	GrapaRuleEvent* profile = NULL;
	if (script && (script->mValue.mToken == GrapaTokenType::ARRAY || script->mValue.mToken == GrapaTokenType::TUPLE || script->mValue.mToken == GrapaTokenType::LIST) && script->vQueue && script->vQueue->mCount)
	{ 
		script = (GrapaRuleEvent*)script->vQueue->Head(); 
		rule = script->Next();
		if (rule) profile = rule->Next();
	}
	while (script && script->mValue.mToken == GrapaTokenType::PTR) script = script->vRulePointer;
	while (rule && rule->mValue.mToken == GrapaTokenType::PTR) rule = rule->vRulePointer;
	while (profile && profile->mValue.mToken == GrapaTokenType::PTR) profile = profile->vRulePointer;

	if (script)
	{
		if (script->mValue.mToken == GrapaTokenType::OP)
			codeResult = vScriptExec->CopyItem(script);
		else
		{
			u64 ruleId = 0;
			if (rule)
			{
				GrapaInt a;
				GrapaRuleEvent* ruleVar;
				switch (rule->mValue.mToken)
				{
				case GrapaTokenType::INT:
					a.FromBytes(rule->mValue);
					ruleId = a.LongValue();
					codeResult = vScriptExec->Plan(pNameSpace, script->mValue, NULL, ruleId, profile ? profile->mValue : GrapaCHAR());
					break;
				case GrapaTokenType::STR:
					ruleVar = vScriptExec->vScriptState->SearchVariable(pNameSpace, rule->mValue);
					if (ruleVar==NULL)
						ruleVar = vScriptExec->vScriptState->mRuleStartQueue.Search(rule->HashId(0));
					if (ruleVar && ruleVar->mValue.mToken==GrapaTokenType::RULE)
					{
						codeResult = vScriptExec->Plan(pNameSpace, script->mValue, ruleVar, 0, profile ? profile->mValue : GrapaCHAR());
					}
					else
					{
						codeResult = vScriptExec->Plan(pNameSpace, script->mValue, NULL, rule->HashId(0), profile ? profile->mValue : GrapaCHAR());
					}
					break;
				case GrapaTokenType::RULE:
					codeResult = vScriptExec->Plan(pNameSpace, script->mValue, rule, 0, profile ? profile->mValue : GrapaCHAR());
					break;
				}
			}
			else
			{
				codeResult = vScriptExec->Plan(pNameSpace, script->mValue, NULL, 0, profile ? profile->mValue : GrapaCHAR());
			}
		}
	}

	if (codeResult && codeResult->mAbort)
	{
		if (codeResult->mValue.mToken == GrapaTokenType::ERR)
			result = codeResult;
		else if ((codeResult->mValue.mToken == GrapaTokenType::ARRAY || codeResult->mValue.mToken == GrapaTokenType::TUPLE) && codeResult->vQueue && codeResult->vQueue->mCount && codeResult->vQueue->Tail()->mValue.mToken == GrapaTokenType::ERR)
		{
			result = codeResult->vQueue->PopTail();
			codeResult->CLEAR();
			delete codeResult;
			codeResult = NULL;
		}
		else
		{
			result = new GrapaRuleEvent(GrapaTokenType::ERR, 0, "", "");
			result->vQueue = new GrapaRuleQueue();
			codeResult->CLEAR();
			delete codeResult;
			codeResult = NULL;
		}
	}
	else
	{
		result = new GrapaRuleEvent(GrapaTokenType::OP, 0, "", "");
		result->vQueue = new GrapaRuleQueue();
		GrapaRuleEvent* v;

		v = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
		v->vQueue = new GrapaRuleQueue();
		GrapaRuleEvent* v2 = new GrapaRuleEvent(GrapaTokenType::ID, 0, "", "op");
		v->vQueue->PushTail(v2);
		if (codeResult) v->vQueue->PushTail(codeResult);
		result->vQueue->PushTail(v);
		if (paramHead) result->vQueue->PushTail(vScriptExec->CopyItem(paramHead));
	}

	if (scriptHeadDel) { scriptHeadDel->CLEAR(); delete scriptHeadDel; scriptHeadDel = NULL; }
	if (paramHeadDel) { paramHeadDel->CLEAR(); delete paramHeadDel; paramHeadDel = NULL; }

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWrapEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* codeResult = NULL;

	GrapaRuleEvent *paramHead = pInput ? pInput->Head(1) : NULL, *paramHeadDel = NULL;
	while (paramHead && paramHead->mValue.mToken == GrapaTokenType::PTR) paramHead = paramHead->vRulePointer;
	if (paramHead && paramHead->mValue.mToken == GrapaTokenType::OP) { paramHead = vScriptExec->ProcessPlan(pNameSpace, paramHead); paramHeadDel = paramHead; }
	while (paramHead && paramHead->mValue.mToken == GrapaTokenType::PTR) paramHead = paramHead->vRulePointer;

	GrapaRuleEvent *scriptHead = pInput ? pInput->Head(0) : NULL, *scriptHeadDel = NULL;
	while (scriptHead && scriptHead->mValue.mToken == GrapaTokenType::PTR) scriptHead = scriptHead->vRulePointer;
	if (scriptHead && scriptHead->mValue.mToken == GrapaTokenType::OP) { scriptHead = vScriptExec->ProcessPlan(pNameSpace, scriptHead); scriptHeadDel = scriptHead; }
	while (scriptHead && scriptHead->mValue.mToken == GrapaTokenType::PTR) scriptHead = scriptHead->vRulePointer;

	GrapaRuleEvent *script = scriptHead;
	while (script && script->mValue.mToken == GrapaTokenType::PTR) script = script->vRulePointer;
	codeResult = script;

	
	if ((scriptHead && scriptHead->mValue.mToken != GrapaTokenType::OP) || paramHead)
	{
		result = new GrapaRuleEvent(GrapaTokenType::OP, 0, "", "");
		result->vQueue = new GrapaRuleQueue();
		GrapaRuleEvent *v;
		//GrapaRuleEvent *v = new GrapaRuleEvent(GrapaTokenType::SYSID, 0, "", "flow");
		//result->vQueue->PushTail(v);

		v = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
		v->vQueue = new GrapaRuleQueue();
		GrapaRuleEvent *v2 = new GrapaRuleEvent(GrapaTokenType::ID, 0, "", "op");
		v->vQueue->PushTail(v2);
		if (scriptHead) v->vQueue->PushTail(vScriptExec->CopyItem(scriptHead));
		result->vQueue->PushTail(v);
		if (paramHead) result->vQueue->PushTail(vScriptExec->CopyItem(paramHead));
	}
	else if (scriptHead)
	{
		result = vScriptExec->CopyItem(scriptHead);
	}

	if (scriptHeadDel) { scriptHeadDel->CLEAR(); delete scriptHeadDel; scriptHeadDel = NULL; }
	if (paramHeadDel) { paramHeadDel->CLEAR(); delete paramHeadDel; paramHeadDel = NULL; }
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleOpEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;

	GrapaRuleEvent* code = (GrapaRuleEvent*)(pOperation && pOperation->vQueue && pOperation->vQueue->mCount ? pOperation->vQueue->Head(1) : NULL);
	while (code && code->mValue.mToken == GrapaTokenType::PTR) code = code->vRulePointer;
	GrapaRuleEvent* isLambda = code;

	GrapaRuleEvent* operation = vScriptExec->vScriptState->AddRuleOperation(pNameSpace->GetNameQueue(), "", "");
	if (operation)
	{
		GrapaRuleEvent *e = pInput ? pInput->Head() : NULL;
		while (e)
		{
			GrapaRuleEvent *v = new GrapaRuleEvent(GrapaTokenType::PTR, 0, (char*)e->mName.mBytes);
			GrapaRuleEvent *eH = e;
			while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
			v->vRulePointer = eH;
			operation->vQueue->PushTail(v);
			e = e->Next();
		}
		if (isLambda && (isLambda->mValue.mToken == GrapaTokenType::OP || isLambda->mValue.mToken == GrapaTokenType::CODE))
			result = vScriptExec->ProcessPlan(pNameSpace, code);
		else
			result = vScriptExec->CopyItem(code);
		if (result) // && (!result->mVar || result->mLocal) && result->mValue.mToken == GrapaTokenType::PTR) // NEED TO FIX THIS...when to not make a copy?
		{
			if (result->mValue.mToken == GrapaTokenType::PTR || (result->mValue.mToken != GrapaTokenType::STR && result->mValue.mToken != GrapaTokenType::RAW && result->mValue.mToken != GrapaTokenType::TABLE && result->mValue.mToken != GrapaTokenType::VECTOR && result->mValue.mToken != GrapaTokenType::WIDGET))
			{
				if (!(result->mValue.mToken == GrapaTokenType::PTR && result->mVar && result->vRulePointer->mValue.mToken == GrapaTokenType::WIDGET))
				{
					GrapaRuleEvent* v = vScriptExec->CopyItem(result);
					result->CLEAR();
					delete result;
					result = v;
				}
			}
		}
		//if (result)
		//{
		//	switch (result->mValue.mToken)
		//	{
		//	case GrapaTokenType::PTR:
		//		if (result->mVar && (operation->vQueue->HasEvent(result->vRulePointer) || result->vRulePointer==operation))
		//		{
		//			GrapaRuleEvent* v = vScriptExec->CopyItem(result);
		//			result->CLEAR();
		//			delete result;
		//			result = v;
		//		}
		//		break;
		//	case GrapaTokenType::LIST:
		//	case GrapaTokenType::ARRAY:
		//	case GrapaTokenType::TUPLE:
		//	case GrapaTokenType::OP:
		//	case GrapaTokenType::RULE:
		//		vScriptExec->ReplaceLocalQueue((GrapaRuleQueue*)result->vQueue, (GrapaRuleQueue*)operation->vQueue);
		//		break;
		//	}
		//}
		if (pNameSpace->GetNameQueue()->PopEvent(operation))
		{
			operation->CLEAR();
			delete operation;
			operation = NULL;
		}
	}

	return(result);
}

class GrapaRuleWorkEvent : public GrapaWorkEvent
{
public:
	GrapaNames mRuleVariables;
	GrapaRuleEvent* mCode, * mResult;
	GrapaRuleQueue* mParam;
	GrapaScriptExec* vScriptExec;
	GrapaCHAR mName;
	bool mSplit;
	GrapaRuleWorkEvent(GrapaWorkQueue* vQueue, GrapaScriptExec* pTokenExec, GrapaNames* pNameSpace, GrapaCHAR* pName, GrapaRuleEvent* pCode, GrapaRuleQueue* pParam, bool pSplit)
	{
		vScriptExec = pTokenExec;
		if (pName)
			mName.FROM(*pName);
		mRuleVariables.SetParrent(pNameSpace);
		//mRuleVariables.vParentNames = NULL;
		pTokenExec->vScriptState->AddRuleOperation(mRuleVariables.GetNameQueue(), "", ""); // global variables
		mCode = pCode;
		mParam = pParam;
		mResult = NULL;
		mSplit = pSplit;
		Set(vQueue);
	}
	virtual ~GrapaRuleWorkEvent()
	{
		GrapaRuleWorkEvent::CLEAR();
	}
	virtual void CLEAR()
	{
		GrapaThread::Stop();
		mRuleVariables.CLEAR();
		if (mParam)
		{
			mParam->CLEAR();
			delete mParam;
			mParam = NULL;
		}
		if (mResult)
		{
			mResult->CLEAR();
			delete mResult;
			mResult = NULL;
		}
		GrapaWorkEvent::CLEAR();
	}
	virtual void Running()
	{
		SendCondition();
		if (mSplit)
		{
			GrapaRuleEvent* first = mParam->Head();
			GrapaRuleEvent* e = first->vRulePointer;
			GrapaRuleEvent* old = first->vRulePointer;
			while (e && e->vRulePointer) e = e->vRulePointer;

			mResult = new GrapaRuleEvent(e->mValue.mToken, 0, "", "");
			mResult->vQueue = new GrapaRuleQueue();

			if (e)
			{
				e = e->vQueue->Head();
				while (e)
				{
					first->vRulePointer = e;
					if (first->vRulePointer && first->vRulePointer->vRulePointer && first->vRulePointer->mValue.mToken != GrapaTokenType::PTR)
					{
						if (first->vRulePointer->mValue.mToken == GrapaTokenType::ERR)
							first->vRulePointer->vClass = NULL;
						first->vRulePointer->vRulePointer = NULL;
					}
					while (first->vRulePointer && first->vRulePointer->vRulePointer) first->vRulePointer = first->vRulePointer->vRulePointer;
					GrapaRuleEvent* subResult = vScriptExec->ProcessPlan(&mRuleVariables, mCode, mParam->Head());
					if (subResult == NULL)
					{
						subResult = new GrapaRuleEvent(GrapaTokenType::SYSID, 0, "", "");
						subResult->SetNull();
					}
					subResult->mName.FROM(first->vRulePointer->mName);
					mResult->vQueue->PushTail(subResult);
					e = e->Next();
				}
			}

			first->vRulePointer = old;
		}
		else
		{
			GrapaRuleEvent* first = mParam->Head();
			if (first->vRulePointer && first->vRulePointer->vRulePointer && first->vRulePointer->mValue.mToken != GrapaTokenType::PTR)
			{
				if (first->vRulePointer->mValue.mToken == GrapaTokenType::ERR)
					first->vRulePointer->vClass = NULL;
				first->vRulePointer->vRulePointer = NULL;
			}
			while (first->vRulePointer && first->vRulePointer->vRulePointer) first->vRulePointer = first->vRulePointer->vRulePointer;
			mResult = vScriptExec->ProcessPlan(&mRuleVariables, mCode, mParam->Head());
			if (mResult == NULL)
			{
				mResult = new GrapaRuleEvent(GrapaTokenType::SYSID, 0, "", "");
				mResult->SetNull();
			}
			mResult->mName.FROM(first->vRulePointer->mName);
		}
	}
};

class GrapaRuleWorkQueue : public GrapaWorkQueue
{
public:
	virtual ~GrapaRuleWorkQueue() {
		GrapaRuleWorkQueue::CLEAR();
	}
	virtual void CLEAR()
	{
		GrapaRuleWorkEvent* e = (GrapaRuleWorkEvent*)PopHead();
		while (e)
		{
			e->CLEAR();
			delete e;
			e = (GrapaRuleWorkEvent*)PopHead();
		}
	}
};

GrapaRuleEvent* GrapaLibraryRuleMapEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam list(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam code(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam vars(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam threads(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* listVal = list.vVal;
	GrapaRuleEvent* delList = NULL;

	s64 threadN = 0;
	if (threads.vVal && threads.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt n;
		n.FromBytes(threads.vVal->mValue);
		threadN = n.LongValue();
	}

	while (listVal && listVal->mValue.mToken == GrapaTokenType::PTR) listVal = listVal->vRulePointer;
	GrapaRuleEvent* params = listVal && listVal->vQueue ? (GrapaRuleEvent*)listVal->vQueue->Head(0) : NULL;

	if (params)
	{

		if (listVal->mValue.mToken == GrapaTokenType::TAG)
			result = new GrapaRuleEvent(GrapaTokenType::XML, 0, "");
		else
			result = new GrapaRuleEvent(listVal->mValue.mToken, 0, "");
		result->vQueue = new GrapaRuleQueue();
	}

	if (threadN > 0 && listVal && listVal->vQueue)
	{
		delList = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
		delList->vQueue = new GrapaRuleQueue();
		if (threadN == 1)
		{
			GrapaRuleEvent* t = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
			t->vRulePointer = listVal;
			delList->vQueue->PushTail(t);
		}
		else
		{
			u64 in = listVal->vQueue->mCount / threadN;
			if (listVal->vQueue->mCount % threadN) in++;
			for (u64 i = 0; i < threadN; i++)
			{
				GrapaRuleEvent* t = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
				t->vQueue = new GrapaRuleQueue();
				delList->vQueue->PushTail(t);
				for (u64 j = 0; j < in; j++)
				{
					if (params)
					{
						GrapaRuleEvent* r = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
						r->vRulePointer = params;
						params = params->Next();
						t->vQueue->PushTail(r);
					}
				}
			}
		}
		listVal = delList;
		params = listVal && listVal->vQueue ? (GrapaRuleEvent*)listVal->vQueue->Head(0) : NULL;
	}

	GrapaRuleWorkQueue wq;
	GrapaRuleWorkEvent* we = NULL;

	GrapaRuleEvent *p = params;
	if (p && p->mValue.mToken == GrapaTokenType::EL)
		p = p->vQueue->Head(1)->vQueue->Head();
	while (p)
	{
		GrapaRuleQueue *cmp = new GrapaRuleQueue();

		GrapaRuleEvent *eH = p;
		while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
		GrapaRuleEvent* nextItem = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "");
		nextItem->vRulePointer = eH;
		cmp->PushTail(nextItem);

		eH = vars.vVal;
		while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
		if (eH)
		{
			nextItem = vScriptExec->CopyItem(eH);
			nextItem->mName.SetLength(0);
		}
		else
		{
			nextItem = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "");
			nextItem->vQueue = new GrapaRuleQueue();
		}
		cmp->PushTail(nextItem);

		we = new GrapaRuleWorkEvent(&wq, vScriptExec, pNameSpace, &p->mName, code.vVal, cmp, delList!=NULL);
		p = p->Next();
	}

	we = (GrapaRuleWorkEvent*)wq.Head();
	while (we)
	{
		we->Start(false);
		we = (GrapaRuleWorkEvent*)we->Next();
	}

	if (result)
	{
		wq.Start();
		we = (GrapaRuleWorkEvent*)wq.Head();
		while (we)
		{
			if (we->mResult)
			{
				if (delList)
				{
					while (we->mResult->vQueue->Head())
						result->vQueue->PushTail(we->mResult->vQueue->PopHead());
				}
				else
				{
					if (we->mResult->mValue.mToken == GrapaTokenType::PTR)
					{
						result->vQueue->PushTail(vScriptExec->CopyItem(we->mResult));
					}
					else
					{
						result->vQueue->PushTail(we->mResult);
						we->mResult = NULL;
					}
				}
			}
			else
				result->vQueue->PushTail(new GrapaRuleEvent(GrapaTokenType::ERR, 0, "", ""));
			we = (GrapaRuleWorkEvent*)we->Next();
		}
	}

	wq.CLEAR();

	if (delList)
	{
		delList->CLEAR();
		delete delList;
		delList = NULL;
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleFilterEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam list(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam code(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam vars(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam threads(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* listVal = list.vVal;
	GrapaRuleEvent* delList = NULL;

	s64 threadN = 0;
	if (threads.vVal && threads.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt n;
		n.FromBytes(threads.vVal->mValue);
		threadN = n.LongValue();
	}

	while (listVal && listVal->mValue.mToken == GrapaTokenType::PTR) listVal = listVal->vRulePointer;
	GrapaRuleEvent* params = listVal && listVal->vQueue ? (GrapaRuleEvent*)listVal->vQueue->Head(0) : NULL;

	if (params)
	{

		if (listVal->mValue.mToken == GrapaTokenType::TAG)
			result = new GrapaRuleEvent(GrapaTokenType::XML, 0, "");
		else
			result = new GrapaRuleEvent(listVal->mValue.mToken, 0, "");
		result->vQueue = new GrapaRuleQueue();
	}

	if (threadN > 0 && listVal && listVal->vQueue)
	{
		delList = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
		delList->vQueue = new GrapaRuleQueue();
		if (threadN == 1)
		{
			GrapaRuleEvent* t = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
			t->vRulePointer = listVal;
			delList->vQueue->PushTail(t);
		}
		else
		{
			u64 in = listVal->vQueue->mCount / threadN;
			if (listVal->vQueue->mCount % threadN) in++;
			for (u64 i = 0; i < threadN; i++)
			{
				GrapaRuleEvent* t = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
				t->vQueue = new GrapaRuleQueue();
				delList->vQueue->PushTail(t);
				for (u64 j = 0; j < in; j++)
				{
					if (params)
					{
						GrapaRuleEvent* r = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
						r->vRulePointer = params;
						params = params->Next();
						t->vQueue->PushTail(r);
					}
				}
			}
		}
		listVal = delList;
		params = listVal && listVal->vQueue ? (GrapaRuleEvent*)listVal->vQueue->Head(0) : NULL;
	}

	GrapaRuleWorkQueue wq;
	GrapaRuleWorkEvent* we = NULL;

	GrapaRuleEvent* p = params;
	if (p && p->mValue.mToken == GrapaTokenType::EL)
		p = p->vQueue->Head(1)->vQueue->Head();
	while (p)
	{
		GrapaRuleQueue* cmp = new GrapaRuleQueue();

		GrapaRuleEvent* eH = p;
		while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
		GrapaRuleEvent* nextItem = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "");
		nextItem->vRulePointer = eH;
		cmp->PushTail(nextItem);

		eH = vars.vVal;
		while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
		if (eH)
		{
			nextItem = vScriptExec->CopyItem(eH);
			nextItem->mName.SetLength(0);
		}
		else
		{
			nextItem = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "");
			nextItem->vQueue = new GrapaRuleQueue();
		}
		cmp->PushTail(nextItem);

		we = new GrapaRuleWorkEvent(&wq, vScriptExec, pNameSpace, &p->mName, code.vVal, cmp, delList != NULL);
		p = p->Next();
	}

	we = (GrapaRuleWorkEvent*)wq.Head();
	while (we)
	{
		we->Start(false);
		we = (GrapaRuleWorkEvent*)we->Next();
	}

	if (result)
	{
		wq.Start();
		we = (GrapaRuleWorkEvent*)wq.Head();
		while (we)
		{
			if (we->mResult)
			{
				if (delList)
				{
					GrapaRuleEvent* qq = we->mResult->vQueue->Head();
					GrapaRuleEvent* first = we->mParam->Head()->vRulePointer;
					while (first && first->vRulePointer) first = first->vRulePointer;
					first = first->vQueue->Head();
					while (qq && first)
					{
						p = first;
						while (p && p->vRulePointer) p = p->vRulePointer;
						if (p && p->mValue.mToken == GrapaTokenType::EL)
							p = p->vQueue->Head(1)->vQueue->Head();
						GrapaRuleEvent* rH = qq;
						while (rH && rH->mValue.mToken == GrapaTokenType::PTR) rH = rH->vRulePointer;
						if (rH && !rH->mNull && ((rH->mValue.mLength && rH->mValue.mBytes[0] && rH->mValue.mBytes[0] != '0') || (rH->vQueue && rH->vQueue->mCount)))
							result->vQueue->PushTail(vScriptExec->CopyItem(p));
						qq = qq->Next();
						first = first->Next();
					}
				}
				else
				{
					GrapaRuleEvent* p = we->mParam->Head()->vRulePointer;
					while (p && p->vRulePointer) p = p->vRulePointer;
					if (p && p->mValue.mToken == GrapaTokenType::EL)
						p = p->vQueue->Head(1)->vQueue->Head();
					GrapaRuleEvent* rH = we->mResult;
					while (rH && rH->mValue.mToken == GrapaTokenType::PTR) rH = rH->vRulePointer;
					if (rH && !rH->mNull && ((rH->mValue.mLength && rH->mValue.mBytes[0] && rH->mValue.mBytes[0] != '0') || (rH->vQueue && rH->vQueue->mCount)))
						result->vQueue->PushTail(vScriptExec->CopyItem(p));
				}
			}
			we = (GrapaRuleWorkEvent*)we->Next();
		}
	}

	wq.CLEAR();

	if (delList)
	{
		delList->CLEAR();
		delete delList;
		delList = NULL;
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleReduceEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam list(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam code(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam init(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam vars(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* params = list.vVal && list.vVal->vQueue ? (GrapaRuleEvent*)list.vVal->vQueue->Head(0) : NULL;
	while (params && params->mValue.mToken == GrapaTokenType::PTR) params = params->vRulePointer;

	GrapaRuleEvent* start = NULL;
	if (init.vVal && !init.vVal->mNull)
		start = init.vVal;
	while (start && start->mValue.mToken == GrapaTokenType::PTR) start = start->vRulePointer;
	GrapaRuleEvent* startDel = NULL;

	GrapaRuleQueue *cmp = new GrapaRuleQueue();

	GrapaRuleEvent* total = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "");
	cmp->PushTail(total);

	GrapaRuleEvent* nextItem = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "");
	cmp->PushTail(nextItem);

	GrapaRuleEvent* extra = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "");
	cmp->PushTail(extra);

	GrapaRuleEvent* eH = vars.vVal;
	while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
	extra->vRulePointer = eH;

	if (start)
		result = vScriptExec->CopyItem(start);

	GrapaRuleEvent *p = params;
	if (p && p->mValue.mToken == GrapaTokenType::EL)
		p = p->vQueue->Head(1)->vQueue->Head();
	if (result==NULL && p)
	{
		if (p->mValue.mToken == GrapaTokenType::TAG)
		{
			result = new GrapaRuleEvent(GrapaTokenType::XML, 0, "");
			result->vQueue = new GrapaRuleQueue();
		}
		else
		{
			GrapaRuleEvent* eH = p;
			while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
			result = vScriptExec->CopyItem(eH);
			p = p->Next();
		}
	}

	if (result==NULL)
		result = Error(vScriptExec, pNameSpace,-1);

	total->vRulePointer = result;

	while (p)
	{
		GrapaRuleEvent *eH = p;
		while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
		nextItem->vRulePointer = eH;
		GrapaRuleEvent *r = vScriptExec->ProcessPlan(pNameSpace, code.vVal, total);
		if (r)
		{
			if (r->vRulePointer != total->vRulePointer)
			{
				if (startDel)
				{
					startDel->CLEAR();
					delete startDel;
				}
				startDel = r;
				total->vRulePointer = r;
			}
			else
			{
				r->CLEAR();
				delete r;
			}
		}
		p = p->Next();
	}

	//result = total;

	//cmp->PopEvent(total);
	cmp->CLEAR();
	delete cmp;

	//if (result && result->mValue.mToken == GrapaTokenType::PTR)
	//{
	//	GrapaRuleEvent *v = vScriptExec->CopyItem(result);
	//	delete result;
	//	result = v;
	//}

	if (startDel)
	{
		startDel->CLEAR();
		delete startDel;
	}

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRangeEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam Pstop(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam Pstart(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam Pstep(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);

	if (Pstop.vVal && !Pstop.vVal->IsNull() && Pstop.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt startI(0);
		GrapaInt stopI(0);
		GrapaInt stepI(1);
		GrapaInt zeroI(0);
		GrapaInt oneI(1);
		GrapaInt moneI(-1);

		stopI.FromBytes(Pstop.vVal->mValue);

		if (Pstart.vVal && !Pstart.vVal->IsNull() && Pstart.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			startI.FromBytes(Pstart.vVal->mValue);
		}

		if (Pstep.vVal && !Pstep.vVal->IsNull() && Pstep.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			stepI.FromBytes(Pstep.vVal->mValue);
		}
		if (stepI == zeroI)
			stepI = oneI;

		bool done = false;
		if (startI < stopI && stepI < oneI)
			done = true;
		else if (startI > stopI && stepI > moneI)
			done = true;
		result = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "");
		result->vQueue = new GrapaRuleQueue();
		if (!done)
		{
			while (startI < stopI)
			{
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(""), startI.getBytes()));
				startI = startI + stepI;
			}
		}

	}
	return result;
}

GrapaRuleEvent* GrapaLibraryRuleIsIntEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam obj(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	bool istrue = false;
	if (obj.vVal)
	{
		if (obj.vVal->mValue.mToken == GrapaTokenType::STR && obj.vVal->mValue.mLength)
		{
			GrapaCHAR s(obj.vVal->mValue);
			s.Trim();
			if (s.mLength)
			{
				istrue = true;
				for (int i = 0; s.mBytes[i] != '\0'; i++)
				{
					if (isdigit(s.mBytes[i]) == 0)
					{
						istrue = false;
						break;
					}
				}
			}
		}
		else if (obj.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			istrue = true;
		}
		else if (obj.vVal->mValue.mToken == GrapaTokenType::SYSID)
		{
			if (obj.vVal->mNull)
				istrue = true;
		}
		else if (obj.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(obj.vVal->mValue);
			istrue = d.IsInt();
		}
	}
	result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", istrue?"1":"0");
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleIfErrEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam obj(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam iftrue(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	bool istrue = true;
	if (obj.vVal && obj.vVal->mValue.mToken != GrapaTokenType::ERR)
		istrue = false;
	if (istrue)
	{
		if (iftrue.vVal)
			result = vScriptExec->CopyItem(iftrue.vVal);
	}
	else
	{
		if (obj.vVal)
			result = vScriptExec->CopyItem(obj.vVal);
	}
	return(result);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	GrapaInt a, b;
//GrapaDatabaseEvent* dbEvent = vScriptExec->vScriptState->vActiveDatabase;
//	GrapaCHAR name;

GrapaRuleEvent* GrapaLibraryRuleTableEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	u64 firstTree;
	GrapaError err = -1;
	u8 listType = GrapaDB::GROUP_TREE;
	if (r2.vVal && r2.vVal->mValue.mToken==GrapaTokenType::STR)
	{
		if (r2.vVal->mValue.StrCmp("DIR") == 0) listType = GrapaDB::GROUP_TREE;
		else if (r2.vVal->mValue.StrCmp("GROUP") == 0) listType = GrapaDB::GROUP_TREE;
		else if (r2.vVal->mValue.StrCmp("ROW") == 0) listType = GrapaDB::RTABLE_TREE;
		else if (r2.vVal->mValue.StrCmp("COL") == 0) listType = GrapaDB::CTABLE_TREE;
	}
	GrapaRuleEvent* result = new GrapaRuleEvent(GrapaTokenType::TABLE, 0, "");
	GrapaGroup g(&result->mValue);
	err = g.Create("$", listType, firstTree);
	err = g.OpenFile(result->mValue, GrapaReadWriteCreate);
	err = g.CloseFile();
	GrapaLocalDatabase ldb(vScriptExec->vScriptState);
	ldb.DatabaseSet(result->mValue);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePwdEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent)
	{
		GrapaCHAR path;
		objEvent->vDatabase->DirectoryPWD(path);
		result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", (char*)path.mBytes);
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCdEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent)
	{
		err = 0;
		if (r2.vVal && r2.vVal->mValue.mBytes)
		{
			if (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::SYSINT)
			{
				GrapaInt a;
				a.FromBytes(r2.vVal->mValue);
				err = objEvent->vDatabase->DirectorySwitch(a.LongValue());
			}
			else if (r2.vVal->mValue.mToken == GrapaTokenType::STR || r2.vVal->mValue.mToken == GrapaTokenType::ID)
				err = objEvent->vDatabase->DirectorySwitch(r2.vVal->mValue);
		}
		else
			err = objEvent->vDatabase->DirectorySwitch(0);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePhdEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent)
	{
		result = new GrapaRuleEvent(0, GrapaCHAR(), objEvent->vDatabase->mHomeDir);
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleChdEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent)
	{
		GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
		if (r2.vVal)
		{
			err = objEvent->vDatabase->HomeSwitch(r2.vVal->mValue);
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);

}

GrapaRuleEvent* GrapaLibraryRuleLsEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent)
	{
		result = new GrapaRuleEvent();
		err = objEvent->vDatabase->DirectoryList(r2.vVal->mValue, result);
		if (err)
		{
			result->CLEAR();
			delete result;
			result = NULL;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleMkEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent && r2.vVal && r2.vVal->mValue.mLength)
	{
		GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
		GrapaCHAR gType;
		if (r3.vVal) gType.FROM(r3.vVal->mValue);
		err = objEvent->vDatabase->DirectoryCreate(r2.vVal->mValue, gType);
	}

	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRmEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent && r2.vVal && r2.vVal->mValue.mLength)
	{
		err = objEvent->vDatabase->DirectoryDelete(r2.vVal->mValue);
	}

	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

//GrapaRuleEvent* GrapaLibraryRuleRmEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
//{
//	GrapaError err = -1;
//	GrapaRuleEvent* result = NULL;
//
//	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
//	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
//
//	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
//	if (objEvent && objEvent->vDatabase == NULL)
//		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);
//
//	if (objEvent && r2.vVal && r2.vVal->mValue.mLength)
//	{
//		if (r2.vVal->mValue.mToken == GrapaTokenType::INT)
//		{
//			GrapaInt a;
//			a.FromBytes(r2.vVal->mValue);
//			objEvent->vDatabase->DataDelete(a.LongValue());
//		}
//		else
//			objEvent->vDatabase->DataDelete(r2.vVal->mValue);
//		err = 0;
//	}
//
//	if (err && result == NULL)
//		result = Error(vScriptExec, pNameSpace, err);
//	return(result);
//}

GrapaRuleEvent* GrapaLibraryRuleMkFieldEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	GrapaLibraryParam r5(vScriptExec, pNameSpace, pInput ? pInput->Head(4) : NULL);
	GrapaLibraryParam r6(vScriptExec, pNameSpace, pInput ? pInput->Head(5) : NULL);

	if (objEvent && r2.vVal && r2.vVal->mValue.mLength)
	{
		GrapaCHAR fieldType;
		if (r3.vVal) fieldType.FROM(r3.vVal->mValue);
		GrapaCHAR storeType;
		if (r4.vVal) storeType.FROM(r4.vVal->mValue);
		GrapaInt a, b;
		a = 0;
		b = 0;
		if (r5.vVal) a.FromBytes(r5.vVal->mValue);
		if (r6.vVal) b.FromBytes(r6.vVal->mValue);
		objEvent->vDatabase->FieldCreate(r2.vVal->mValue, fieldType, storeType, a.LongValue(), b.LongValue());
		err = 0;
	}

	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRmFieldEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent && r2.vVal && r2.vVal->mValue.mLength)
	{
		objEvent->vDatabase->FieldDelete(r2.vVal->mValue);
		err = 0;
	}

	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

//cp = () [[op, ()["cp", { this,()[var,{o}],null,()[var,{p}],()[var,{d}],()[var,{e}] }]], { o,p,d,e }];
//cpfield = () [[op, ()["cp", { this,()[var,{o}],()[var,{f}],()[var,{p}],()[var,{d}],()[var,{e}] }]], { o,f,p,d,e }];
//
//GrapaRuleEvent* GrapaLibraryRuleCpEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
//{
//	GrapaError err = -1;
//	GrapaRuleEvent* result = NULL;
//
//	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
//	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
//
//	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
//	if (objEvent && objEvent->vDatabase == NULL)
//		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);
//
//	GrapaCHAR sourceFile;
//	if (r2.vVal) sourceFile.FROM(r2.vVal->mValue);
//
//	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
//	GrapaCHAR sourceField;
//	if (r3.vVal) sourceField.FROM(r3.vVal->mValue);
//
//	GrapaLibraryParam r5(vScriptExec, pNameSpace, pInput ? pInput->Head(4) : NULL);
//	GrapaCHAR targetFile(sourceFile);
//	if (r5.vVal) targetFile.FROM(r5.vVal->mValue);
//
//	GrapaLibraryParam r6(vScriptExec, pNameSpace, pInput ? pInput->Head(5) : NULL);
//	GrapaCHAR targetField;
//	if (r6.vVal) targetField.FROM(r6.vVal->mValue);
//
//	if (objEvent && sourceFile.mLength)
//	{
//		GrapaCHAR value;
//		objEvent->vDatabase->FieldGet(sourceFile, sourceField, value);
//		GrapaLocalDatabase* targetSession = objEvent->vDatabase;
//
//		GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
//
//		if (r4.vVal && sourceFile.mLength)
//		{
//			GrapaRuleEvent* toList = r4.vVal;
//			if (toList->mValue.mToken == GrapaTokenType::ARRAY ||toList->mValue.mToken == GrapaTokenType::TUPLE || toList->mValue.mToken == GrapaTokenType::LIST || toList->mValue.mToken == GrapaTokenType::EL || toList->mValue.mToken == GrapaTokenType::TAG || toList->mValue.mToken == GrapaTokenType::OP)
//				toList = (GrapaRuleEvent*)toList->vQueue->Head();
//			while (toList)
//			{
//				if (toList->mValue.mLength)
//				{
//					targetSession = NULL;
//					//GrapaRuleEvent* v = vScriptExec->vScriptState->SearchFileVariable(pNameSpace, r1.vVal);
//					if (GrapaRuleEvent* searchEvent = vScriptExec->vScriptState->SearchFileVariable(pNameSpace, toList))
//					{
//						targetSession = searchEvent->vDatabase;
//						targetSession->DataCreate(targetFile);
//						targetSession->FieldSet(targetFile, targetField, value);
//						err = 0;
//					}
//				}
//				toList = toList->Next();
//			}
//		}
//	}
//
//	if (err && result == NULL)
//		result = Error(vScriptExec, pNameSpace, err);
//	return(result);
//}

GrapaRuleEvent* GrapaLibraryRuleInfoEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent && r2.vVal && r2.vVal->mValue.mBytes)
	{
		err = 0;

		GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);

		GrapaCHAR setField;
		if (r3.vVal) setField.FROM(r3.vVal->mValue);

		GrapaCHAR setValue;
		result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
		if (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::SYSINT)
		{
			GrapaInt a;
			a.FromBytes(r2.vVal->mValue);
			err = objEvent->vDatabase->FieldInfo(a.LongValue(), setField, result);
			if (err)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
		else
		{
			err = objEvent->vDatabase->FieldInfo(r2.vVal->mValue, setField, result);
			if (err)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSetEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	GrapaLibraryParam fieldValue(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaCHAR setValue; setValue.SetSize(0);

	if (fieldValue.vVal)
	{
		if (fieldValue.vVal->mValue.mToken == GrapaTokenType::ARRAY || fieldValue.vVal->mValue.mToken == GrapaTokenType::TUPLE || fieldValue.vVal->mValue.mToken == GrapaTokenType::LIST || fieldValue.vVal->mValue.mToken == GrapaTokenType::XML || fieldValue.vVal->mValue.mToken == GrapaTokenType::EL || fieldValue.vVal->mValue.mToken == GrapaTokenType::TAG || fieldValue.vVal->mValue.mToken == GrapaTokenType::OP || fieldValue.vVal->mValue.mToken == GrapaTokenType::CODE || fieldValue.vVal->mValue.mToken == GrapaTokenType::ERR)
		{
			if (fieldValue.vVal->vQueue)
				((GrapaRuleQueue*)fieldValue.vVal->vQueue)->TO(setValue, fieldValue.vVal->vClass, fieldValue.vVal->mValue.mToken);
			setValue.mToken = fieldValue.vVal->mValue.mToken;
			fieldValue.vVal = NULL;
		}
	}

	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	if (objEvent && r2.vVal && r2.vVal->mValue.mBytes)
	{
		err = 0;

		GrapaLibraryParam fieldName(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

		GrapaCHAR setField;
		if (fieldName.vVal) setField.FROM(fieldName.vVal->mValue);

		if (r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt a;
			a.FromBytes(r2.vVal->mValue);
			if (fieldValue.vVal)
				err = objEvent->vDatabase->FieldSet(a.LongValue(), setField, fieldValue.vVal->mValue);
			else
				err = objEvent->vDatabase->FieldSet(a.LongValue(), setField, setValue);
			if (err && result)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
		else
		{
			if (fieldValue.vVal)
				err = objEvent->vDatabase->FieldSet(r2.vVal->mValue, setField, fieldValue.vVal->mValue);
			else
				err = objEvent->vDatabase->FieldSet(r2.vVal->mValue, setField, setValue);
			if (err && result)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
	}

	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGetEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	if (objEvent && r2.vVal && r2.vVal->mValue.mBytes)
	{
		err = 0;

		GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);

		GrapaCHAR setField;
		if (r3.vVal) setField.FROM(r3.vVal->mValue);

		GrapaCHAR setValue;
		result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
		if (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::SYSINT)
		{
			GrapaInt a;
			a.FromBytes(r2.vVal->mValue);
			err = objEvent->vDatabase->FieldGet(a.LongValue(), setField, result->mValue);
			if (err)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
		else
		{
			err = objEvent->vDatabase->FieldGet(r2.vVal->mValue, setField, result->mValue);
			if (err)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
		if (result)
		{
			if (result->mValue.mToken == GrapaTokenType::ARRAY || result->mValue.mToken == GrapaTokenType::TUPLE || result->mValue.mToken == GrapaTokenType::LIST || result->mValue.mToken == GrapaTokenType::XML || result->mValue.mToken == GrapaTokenType::EL || result->mValue.mToken == GrapaTokenType::TAG || result->mValue.mToken == GrapaTokenType::OP || result->mValue.mToken == GrapaTokenType::CODE || result->mValue.mToken == GrapaTokenType::ERR)
			{
				result->vQueue = new GrapaRuleQueue();
				result->vClass = ((GrapaRuleQueue*)result->vQueue)->FROM(vScriptExec->vScriptState, pNameSpace, result->mValue);
				if (result->mValue.mLength == 0)
					result->mNull = true;
				result->mValue.SetLength(0);
				result->mValue.SetSize(0);
			}
		}

	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleFileSplitEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;
	
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam partsP(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam fileP(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam pathP(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	GrapaLibraryParam delimP(vScriptExec, pNameSpace, pInput ? pInput->Head(4) : NULL);
	GrapaLibraryParam optionP(vScriptExec, pNameSpace, pInput ? pInput->Head(5) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);


	if (objEvent && fileP.vVal && fileP.vVal->mValue.mBytes && partsP.vVal && partsP.vVal->mValue.mBytes)
	{
		err = 0;

		GrapaCHAR file, path, delim, option;
		GrapaInt parts;
		if (fileP.vVal) file.FROM(fileP.vVal->mValue);
		if (pathP.vVal) path.FROM(pathP.vVal->mValue);
		if (partsP.vVal && partsP.vVal->mValue.mToken == GrapaTokenType::INT) parts.FromBytes(partsP.vVal->mValue);
		if (delimP.vVal) delim.FROM(delimP.vVal->mValue);
		if (optionP.vVal) option.FROM(optionP.vVal->mValue);
		s64 partsI = parts.LongValue();

		if (partsI)
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			err = objEvent->vDatabase->FieldSplit(partsI, file, path, delim, option, result);
			if (err)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
	}

	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

//setop = () [[op, ()["setop", { this,()[var,{o}],()[var,{p}] }]], { o,p }];
//getop = () [[op, ()["getop", { this,()[var,{p}] }]], { p }];
//runop = () [[op, ()["runop", { this,()[var,{o}],()[var,{p}] }]], { o,p }];
//load = op(in){(getop(@in))()};

//GrapaRuleEvent* GrapaLibraryRuleSetOpEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
//{
//	GrapaRuleEvent* result = NULL;
//	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
//
//	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
//	if (objEvent && objEvent->vDatabase == NULL)
//		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);
//
//	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
//	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
//
//	if (objEvent && r2.vVal && r3.vVal)
//	{
//		GrapaCHAR oldpath;
//		objEvent->vDatabase->DirectoryPWD(oldpath);
//
//		GrapaCHAR setField,setValue;
//		objEvent->vDatabase->FieldGet(r2.vVal->mValue, setField, setValue);
//
//		if (setValue.mLength >= 2 && setValue.mBytes[0] == 0 && setValue.mBytes[1] == 0)
//		{
//		}
//		else
//		{
//			GrapaRuleEvent *r = NULL;
//			GrapaCHAR h(r2.vVal->mValue);
//			while (h.mLength && !(h.mBytes[h.mLength - 1] == '/' || h.mBytes[h.mLength - 1] == '\\')) h.mLength--;
//			h.SetLength(h.mLength);
//			if (h.mLength)
//			{
//				objEvent->vDatabase->DirectorySwitch(h);
//			}
//			r = vScriptExec->Plan(pNameSpace, setValue, NULL, 0);
//			if (h.mLength)
//			{
//				objEvent->vDatabase->DirectorySwitch(oldpath);
//			}
//			setValue.SetLength(0);
//			if (r)
//			{
//				GrapaBYTE s;
//				r->TO(s);
//				GrapaCompress::Compress(s, setValue);
//				r->CLEAR();
//				delete r;
//			}
//		}
//		objEvent->vDatabase->FieldSet(r3.vVal->mValue, setField, setValue);
//	}
//
//	return(result);
//}
//
//GrapaRuleEvent* GrapaLibraryRuleGetOpEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
//{
//	GrapaRuleEvent* result = NULL;
//	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
//
//	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
//	if (objEvent && objEvent->vDatabase == NULL)
//		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);
//
//	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
//
//	if (r2.vVal && objEvent && objEvent->vDatabase)
//	{
//		GrapaCHAR oldpath;
//		objEvent->vDatabase->DirectoryPWD(oldpath);
//		GrapaCHAR setField, setValue;
//		objEvent->vDatabase->FieldGet(r2.vVal->mValue, setField, setValue);
//		if (setValue.mLength >= 2 && setValue.mBytes[0] == 0 && setValue.mBytes[1] == 0)
//		{
//			GrapaBYTE expanded;
//			GrapaCompress::Expand(setValue, expanded);
//			result = new GrapaRuleEvent();
//			result->vQueue = new GrapaRuleQueue();
//			result->vClass = ((GrapaRuleQueue*)result->vQueue)->FROM(vScriptExec->vScriptState, pNameSpace, expanded);
//			result->mValue.SetLength(0);
//			result->mValue.SetSize(0);
//			result->mValue.mToken = expanded.mToken;
//		}
//		else
//		{
//			GrapaCHAR h(r2.vVal->mValue);
//			while (h.mLength && !(h.mBytes[h.mLength - 1] == '/' || h.mBytes[h.mLength - 1] == '\\')) h.mLength--;
//			h.SetLength(h.mLength);
//			if (h.mLength)
//			{
//				objEvent->vDatabase->DirectorySwitch(h);
//			}
//			result = vScriptExec->Plan(pNameSpace, setValue, NULL, 0);
//			if (h.mLength)
//			{
//				objEvent->vDatabase->DirectorySwitch(oldpath);
//			}
//		}
//	}
//	return(result);
//}
//
//GrapaRuleEvent* GrapaLibraryRuleRunOpEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
//{
//	GrapaRuleEvent* result = NULL;
//	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
//
//	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
//	if (objEvent && objEvent->vDatabase == NULL)
//		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);
//
//	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
//	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
//
//	if (r2.vVal && objEvent && objEvent->vDatabase)
//	{
//		GrapaCHAR oldpath;
//		objEvent->vDatabase->DirectoryPWD(oldpath);
//		GrapaCHAR setField, setValue;
//		objEvent->vDatabase->FieldGet(r2.vVal->mValue, setField, setValue);
//		GrapaRuleEvent *plan=NULL;
//		if (setValue.mLength >= 2 && setValue.mBytes[0] == 0 && setValue.mBytes[1] == 0)
//		{
//			GrapaBYTE expanded;
//			GrapaCompress::Expand(setValue, expanded);
//			plan = new GrapaRuleEvent();
//			plan->vQueue = new GrapaRuleQueue();
//			plan->vClass = ((GrapaRuleQueue*)plan->vQueue)->FROM(vScriptExec->vScriptState, pNameSpace, expanded);
//			plan->mValue.SetLength(0);
//			plan->mValue.SetSize(0);
//			plan->mValue.mToken = expanded.mToken;
//		}
//		else
//		{
//			GrapaCHAR h(r2.vVal->mValue);
//			while (h.mLength && !(h.mBytes[h.mLength - 1] == '/' || h.mBytes[h.mLength - 1] == '\\')) h.mLength--;
//			h.SetLength(h.mLength);
//			if (h.mLength)
//			{
//				objEvent->vDatabase->DirectorySwitch(h);
//			}
//			plan = vScriptExec->Plan(pNameSpace, setValue, NULL, 0);
//			if (h.mLength)
//			{
//				objEvent->vDatabase->DirectorySwitch(oldpath);
//			}
//		}
//
//		GrapaScriptState scriptState;
//		GrapaScriptExec scriptExec;
//		scriptState.vScriptExec = &scriptExec;
//		scriptExec.vScriptState = &scriptState;
//		scriptState.vConsoleSend = vScriptExec->vScriptState->vConsoleSend;
//		scriptState.LoadSystem();
//
//		GrapaRuleEvent* operation = scriptState.mRuleVariables.vNames.Tail();
//		if (operation)
//		{
//			operation->vQueue->CLEAR();
//			delete operation->vQueue;
//			operation->vQueue = r3.vVal->vQueue;
//		}
//
//		// $file().runop("lib/run.grc",{a=1,b=2})
//		result = scriptExec.ProcessPlan(&scriptState.mRuleVariables, plan);
//		GrapaRuleEvent *v = vScriptExec->CopyItem(result);
//		result->CLEAR();
//		delete result;
//		result = v;
//
//		if (operation) operation->vQueue = NULL;
//	}
//	return(result);
//}

GrapaRuleEvent* GrapaLibraryRuleIncludeEvent::Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam)
{
	s64 idx;

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, NULL);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	GrapaLibraryParam r1(vScriptExec, pNameSpace, (GrapaRuleEvent*)((pParam&&pParam->vQueue) ? pParam->vQueue->Head(0) : NULL));

	if (r1.vVal && objEvent && objEvent->vDatabase)
	{
		GrapaCHAR oldpath,pathfile;
		objEvent->vDatabase->DirectoryPWD(oldpath);
		pathfile.FROM(oldpath);
		pathfile.Append(r1.vVal->mValue);

		GrapaObjectEvent *t = objEvent->vDatabase->mInclude->Search(pathfile,idx);
		if (t)
		{
			pOperation->CLEAR();
			delete pOperation;
			return(NULL);
		}
		objEvent->vDatabase->mInclude->PushTail(new GrapaObjectEvent(pathfile));

		GrapaCHAR setField, setValue;
		objEvent->vDatabase->FieldGet(r1.vVal->mValue, setField, setValue);
		if ((setValue.mLength >= 2 && setValue.mBytes[0] == 0 && setValue.mBytes[1] == 0) || (setValue.mLength > 4 && setValue.mBytes[0] == 'G' && setValue.mBytes[1] == 'R' && setValue.mBytes[2] == 'Z' && (setValue.mBytes[3] & 0x80) == 0))
		{
			GrapaBYTE expanded;
			GrapaCompress::Expand(setValue, expanded);
			pOperation->SetNull();
			pOperation->mNull = false;
			pOperation->vQueue = new GrapaRuleQueue();
			pOperation->mValue.mToken = expanded.mToken;
			if (expanded.mToken == GrapaTokenType::LIST || expanded.mToken == GrapaTokenType::ARRAY || expanded.mToken == GrapaTokenType::TUPLE)
			{
				GrapaRuleQueue* tq = new GrapaRuleQueue();
				((GrapaRuleQueue*)tq)->FROM(vScriptExec->vScriptState, pNameSpace, expanded);
				GrapaRuleEvent* tx = NULL;
				if (tq->mCount > 1 && expanded.mToken == GrapaTokenType::LIST)
					tx = tq->Search("op",idx);
				if (tx==NULL)
					tx = tq->Head();
				if (tx)
				{
					tq->PopEvent(tx);
					pOperation->mValue.mToken = tx->mValue.mToken;
					pOperation->vQueue = tx->vQueue;
					tx->vQueue = NULL;
					pOperation->vClass = tx->vClass;
					if (pOperation->vClass==NULL && tx->mName.mLength)
						pOperation->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, tx->mName);
					tx->CLEAR();
					delete tx;
				}
				tq->CLEAR();
				delete tq;
			}
			else
			{
				pOperation->vClass = ((GrapaRuleQueue*)pOperation->vQueue)->FROM(vScriptExec->vScriptState, pNameSpace, expanded);
			}
			pOperation->mValue.SetLength(0);
			pOperation->mValue.SetSize(0);
		}
		else
		{
			GrapaCHAR h(r1.vVal->mValue);
			while (h.mLength && !(h.mBytes[h.mLength - 1] == '/' || h.mBytes[h.mLength - 1] == '\\')) h.mLength--;
			h.SetLength(h.mLength);
			if (h.mLength)
			{
				objEvent->vDatabase->DirectorySwitch(h);
			}
			pOperation->CLEAR();
			delete pOperation;
			pOperation = vScriptExec->Plan(pNameSpace, setValue, NULL, 0, GrapaCHAR());
			if (h.mLength)
			{
				objEvent->vDatabase->DirectorySwitch(oldpath);
			}
		}

		t = objEvent->vDatabase->mInclude->PopTail();
		if (t)
		{
			t->CLEAR();
			delete t;
		}
	}
	return(pOperation);
}

GrapaRuleEvent* GrapaLibraryRuleIncludeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleDebugEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vDatabase == NULL)
		objEvent->vDatabase = new GrapaLocalDatabase(vScriptExec->vScriptState);

	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);

	if (objEvent)
	{
		err = 0;
		GrapaInt a = 0;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
			a.FromBytes(r2.vVal->mValue);
		GrapaCHAR setFile;
		if (r3.vVal) setFile.FROM(r3.vVal->mValue);
		if (setFile.mLength)
			objEvent->vDatabase->DatabaseDump(a.LongValue(), setFile);
		else
		{
			GrapaCHARFile mFile;
			objEvent->vDatabase->DatabaseDump(a.LongValue(), mFile);
			if (mFile.mBytes)
				pNameSpace->GetResponse()->Send(vScriptExec, pNameSpace, (char*)mFile.mBytes);
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleMacEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;

	GrapaRuleEvent* r = NULL;
	u64 mac;
	char ipx[256];
	char namex[256];
	GrapaNet::GetMac(namex, ipx, mac);
	result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::LIST;
	result->vQueue = new GrapaRuleQueue();

	r = new GrapaRuleEvent(GrapaTokenType::STR, 0, "name", namex);
	result->vQueue->PushTail(r);
	r = new GrapaRuleEvent(GrapaTokenType::STR, 0, "ip", ipx);
	result->vQueue->PushTail(r);
	r = new GrapaRuleEvent(0, GrapaCHAR("mac"), GrapaInt(mac).getBytes());
	r->mValue.mToken = GrapaTokenType::RAW;
	result->vQueue->PushTail(r);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleInterfacesEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::TABLE;
	result = GrapaNet::GetInterfaces();
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleConnectEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		err = 0;

		if (r2.vVal && r2.vVal->mValue.mBytes)
			objEvent->vNetConnect->Set(r2.vVal->mValue);
		if (r3.vVal && r3.vVal->mValue.mBytes)
		{
			err = objEvent->vNetConnect->Certificate(r3.vVal);
		}
		if (!err && r4.vVal && r4.vVal->vQueue)
		{
			s64 index;
			err = objEvent->vNetConnect->Proxy(r4.vVal->vQueue->Search("type", index), r4.vVal->vQueue->Search("id", index), r4.vVal->vQueue->Search("ip", index));
		}
		if (!err)
		{
			err = objEvent->vNetConnect->Connect(objEvent);
		}
		if (err == 0)
		{
			if (!objEvent->vNetConnect->mNet.mConnected)
			{
				objEvent->vNetConnect->Disconnect();
				err = -1;
			}
		}
	}
	if (err && result == NULL)
	{
		result = Error(vScriptExec, pNameSpace, err);
	}

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBindEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		if (r2.vVal && r2.vVal->mValue.mBytes)
			objEvent->vNetConnect->Set(r2.vVal->mValue);
		err = objEvent->vNetConnect->Bind(objEvent);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleListenEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	GrapaRuleEvent* objBind = vScriptExec->vScriptState->SearchTarget(pNameSpace, r2.vVal);
	if (objBind && objBind->vNetConnect == NULL)
		objBind->vNetConnect = new GrapaNetConnect();

	if (objEvent && objBind)
	{
		err = objEvent->vNetConnect->Listen(objBind->vNetConnect);
		if (err == 0 && !objEvent->vNetConnect->mNet.mConnected)
		{
			objEvent->vNetConnect->Disconnect();
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleOnListenEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		GrapaNames* vNames = pNameSpace;
		while (vNames->GetParrent()) vNames = vNames->GetParrent();
		objEvent->vNetConnect->mNames.SetParrent(vNames);
		if (r2.vVal && r2.vVal->mValue.mBytes)
			objEvent->vNetConnect->Set(r2.vVal->mValue);
		err = objEvent->vNetConnect->OnListen(vScriptExec, objEvent, r3.vVal, r4.vVal);
		if (err == 0 && !objEvent->vNetConnect->mNet.mBound)
		{
			objEvent->vNetConnect->Disconnect();
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleDisconnectEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		err = objEvent->vNetConnect->Disconnect();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleProxyEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		err = objEvent->vNetConnect->Proxy(r2.vVal, r3.vVal, r4.vVal);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCertificateEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		err = objEvent->vNetConnect->Certificate(r2.vVal);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePrivateEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		GrapaNames* vNames = pNameSpace;
		while (vNames->GetParrent()) vNames = vNames->GetParrent();
		objEvent->vNetConnect->mNames.SetParrent(vNames);
		err = objEvent->vNetConnect->Private(vScriptExec, r2.vVal, r3.vVal, r4.vVal);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTrustedEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		err = objEvent->vNetConnect->Trusted(r2.vVal, r3.vVal);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleNetVerifyEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		GrapaCHAR cert, sn, in;
		err = objEvent->vNetConnect->Verify(cert);
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("X509_error"), GrapaInt(err).getBytes()));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("X509_certificate"), cert));
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleNetChainEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		GrapaCHAR cert, sn, in;
		GrapaRuleQueue *q = objEvent->vNetConnect->Chain();
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = q;
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleNetHostEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		//GrapaRuleEvent* r = NULL;
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("url"), GrapaCHAR(objEvent->vNetConnect->mURL)));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("host"), GrapaCHAR(objEvent->vNetConnect->mNet.mHostName)));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("ip"), GrapaCHAR(objEvent->vNetConnect->mNet.mIpName)));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("port"), GrapaInt(objEvent->vNetConnect->mNet.mPort).getBytes()));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("family"), GrapaInt(objEvent->vNetConnect->mNet.mFamily).getBytes()));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("connected"), GrapaInt(objEvent->vNetConnect->mNet.mConnected).getBytes()));
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("bound"), GrapaInt(objEvent->vNetConnect->mNet.mBound).getBytes()));
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSendEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		if (r2.vVal && r2.vVal->mValue.mBytes)
		{
			err = objEvent->vNetConnect->mNet.Send(r2.vVal->mValue);
		}
		else
		{
			err = objEvent->vNetConnect->mNet.Send(GrapaBYTE());
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleReceiveEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		u64 hasmore = 4096;
		u64 receivelen = 0;
		result = new GrapaRuleEvent();
		result->mValue.SetSize(hasmore);
		err = objEvent->vNetConnect->mNet.Receive(result->mValue.mBytes, 4096<hasmore ? 4096 : hasmore, receivelen);
		result->mValue.SetLength(receivelen);
		if (err)
		{
			result->CLEAR();
			delete result;
			result = NULL;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePendingEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		//virtual GrapaError Receive(u8* recvbuf, u64 recvbuflen, u64& recvlen);
		//virtual GrapaError Receive(GrapaBYTE& recvbuf, u64& hasmore);
		u64 hasmore;
		err = objEvent->vNetConnect->mNet.Pending(hasmore);
		if (err == 0)
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(hasmore).getBytes());
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleOnReceiveEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent && r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::OP)
	{
		GrapaNames* vNames = pNameSpace;
		while (vNames->GetParrent()) vNames = vNames->GetParrent();
		objEvent->vNetConnect->mNames.SetParrent(vNames);
		objEvent->vNetConnect->OnReceive(vScriptExec, r2.vVal);
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleHttpReadEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		GrapaNames* vNames = pNameSpace;
		while (vNames->GetParrent()) vNames = vNames->GetParrent();
		objEvent->vNetConnect->mNames.SetParrent(vNames);
		err = objEvent->vNetConnect->HttpRead(vScriptExec);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleHttpSendEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam method(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam entity(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam header(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	GrapaLibraryParam body(vScriptExec, pNameSpace, pInput ? pInput->Head(4) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent)
	{
		GrapaNames* vNames = pNameSpace;
		while (vNames->GetParrent()) vNames = vNames->GetParrent();
		objEvent->vNetConnect->mNames.SetParrent(vNames);
		GrapaCHAR headerStr;
		GrapaCHAR methodStr;
		GrapaCHAR entityStr;

		if (method.vVal && method.vVal->mValue.mToken == GrapaTokenType::STR)
			methodStr.FROM(method.vVal->mValue);
		if (methodStr.mLength == 0)
			methodStr.FROM("GET");
		if (entity.vVal && entity.vVal->mValue.mToken == GrapaTokenType::STR)
			entityStr.FROM(entity.vVal->mValue);
		if (entityStr.mLength == 0)
			entityStr.FROM("/");

		headerStr.Append(methodStr);
		headerStr.Append(" ");
		headerStr.Append(entityStr);
		headerStr.Append(" HTTP/1.1\r\n");

		if (header.vVal && header.vVal->mValue.mToken == GrapaTokenType::LIST && header.vVal->vQueue)
		{
			GrapaRuleEvent* t = header.vVal->vQueue->Head();
			while(t)
			{
				if (t->mName.StrLowerCmp("content-length") != 0)
				{
					if (t->mValue.mToken == GrapaTokenType::STR)
					{
						headerStr.Append(t->mName);
						headerStr.Append(": ");
						headerStr.Append(t->mValue);
						headerStr.Append("\r\n");
					}
					else if (t->mValue.mToken == GrapaTokenType::ARRAY || t->mValue.mToken == GrapaTokenType::TUPLE)
					{
						if (t->vQueue->mCount)
						{
							GrapaCHAR sep(", ");
							if (t->mName.StrLowerCmp("cookie") == 0)
								sep.FROM("; ");
							headerStr.Append(t->mName);
							headerStr.Append(": ");
							GrapaRuleEvent* t2 = t->vQueue->Head();
							while (t2)
							{
								headerStr.Append(t2->mValue);
								t2 = t2->Next();
								if (t2)
									headerStr.Append(sep);
							}
							headerStr.Append("\r\n");
						}
					}
					else if (t->mValue.mToken == GrapaTokenType::LIST)
					{
						if (t->vQueue->mCount)
						{
							GrapaCHAR sep(", ");
							if (t->mName.StrLowerCmp("cookie") == 0)
								sep.FROM("; ");
							headerStr.Append(t->mName);
							headerStr.Append(": ");
							GrapaRuleEvent* t2 = t->vQueue->Head();
							while (t2)
							{
								headerStr.Append(t2->mName);
								if (!t2->IsNull())
								{
									headerStr.Append("=");
									if (t2->mValue.mToken==GrapaTokenType::STR)
										headerStr.Append(t2->mValue);
									else if (t2->mValue.mToken == GrapaTokenType::INT)
									{
										headerStr.Append(GrapaInt(t2->mValue).ToString());
									}
								}
								t2 = t2->Next();
								if (t2)
									headerStr.Append(sep);
							}
							headerStr.Append("\r\n");
						}
					}
				}
				t = t->Next();
			}
		}

		if (body.vVal && body.vVal->mValue.mToken == GrapaTokenType::STR)
		{
			headerStr.Append("Content-Length: ");
			headerStr.Append(GrapaInt(body.vVal->mValue.mLength).ToString());
			headerStr.Append("\r\n");
		}
		headerStr.Append("\r\n");
		if (body.vVal && body.vVal->mValue.mToken == GrapaTokenType::STR && body.vVal->mValue.mLength)
		{
			headerStr.Append(body.vVal->mValue);
		}

		err = objEvent->vNetConnect->mNet.Send(headerStr);
		if (err==0)
			err = objEvent->vNetConnect->HttpRead(vScriptExec);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleHttpMessageEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vNetConnect == NULL)
		objEvent->vNetConnect = new GrapaNetConnect();

	if (objEvent && objEvent->vNetConnect)
	{
		if (objEvent->vNetConnect->mNet.vpResponse.mLength)
		{
			if (r2.vVal&&!r2.vVal->IsZero())
				result = new GrapaRuleEvent(0, GrapaCHAR(), objEvent->vNetConnect->mNet.vpResponse);
			else
				result = objEvent->vNetConnect->HttpMessage(vScriptExec, pNameSpace);
		}
		else
		{
			result = new GrapaRuleEvent(0, 0, "", "");
			result->SetNull();
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);

	return(result);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleGenPrimeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaCHAR name;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaInt num1 = 512;
	GrapaInt num2 = 0;
	if (r1.vVal && r1.vVal->mValue.mLength)
		num1.FromBytes(r1.vVal->mValue);
	if (r2.vVal && r2.vVal->mValue.mLength)
		num2.FromBytes(r2.vVal->mValue);
	if (num1.GetCount() == 1 && num1.GetItem(0) >= 2)
	{
		GrapaPrime p;
		p.GenPrime(num1.GetItem(0), num2.LongValue(), GrapaInt(0), GrapaInt(1));
		result = new GrapaRuleEvent(0, name, p.getBytes());
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleStaticPrimeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaPrime num1 = 512;
	if (r1.vVal && r1.vVal->mValue.mLength)
		num1.FromBytes(r1.vVal->mValue);
	if (num1.GetCount() == 1 && num1.GetItem(0) >= 2)
	{
		//GrapaPrime p;
		//u32 n = num1.GetItem(0);
		//p.StaticPrime(n);
		result = new GrapaRuleEvent(0, GrapaCHAR(), (GrapaPrime::StaticPrime(num1.GetItem(0))).getBytes());
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleIsPrimeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r1.vVal->mValue.mLength)
	{
		GrapaPrime num1, num2;
		num1.FromBytes(r1.vVal->mValue);
		num2 = 20;
		if (r2.vVal && r2.vVal->mValue.mLength)
			num2.FromBytes(r2.vVal->mValue);
		if (num1.TestPrime(num2.LongValue(),64))
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(1).getBytes());
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleIsAksEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mLength)
	{
		GrapaPrime num1;
		num1.FromBytes(r1.vVal->mValue);
		if (num1.AKS())
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(1).getBytes());
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

static GrapaRuleEvent* GrapaLibraryRuleRandomEventCreate(GrapaFloat&f, u8 token, u32 bits, u64 dim, u64 pos, u64*counts)
{
	GrapaRuleEvent*result = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
	result->vQueue = new GrapaRuleQueue();
	if (dim == pos + 1)
	{
		for (u64 i = 0; i < counts[pos]; i++)
		{
			GrapaRuleEvent* item = NULL;
			if (token == GrapaTokenType::INT)
			{
				GrapaInt num2;
				num2.Random(bits + 1);
				num2.unsetBit(bits);
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(""), num2.getBytes()));
			}
			else if (token == GrapaTokenType::FLOAT)
			{
				GrapaInt num2;
				num2.Random(bits);
				GrapaFloat a(f.mFix, f.mMax, f.mExtra, num2);
				if (a.mMax < bits) a.mMax = bits;
				a = a >> bits;
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(""), a.getBytes()));
			}
		}
		return result;
	}
	for (u64 i = 0; i < counts[pos]; i++)
		result->vQueue->PushTail(GrapaLibraryRuleRandomEventCreate(f, token, bits, dim, pos+1, counts));
	return result;
}

GrapaRuleEvent* GrapaLibraryRuleRandomEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	u32 bits = 0;
	GrapaFloat bitsF(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt num1;
		num1 = 32;
		if (r1.vVal->mValue.mLength) num1.FromBytes(r1.vVal->mValue);
		if (num1.GetCount() == 1)
			bits = num1.GetItem(0);
	}
	else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
	{
		if (r1.vVal->mValue.mLength)
		{
			GrapaInt num1;
			bitsF.FromBytes(r1.vVal->mValue);
			num1 = bitsF.ToInt();
			if (num1.GetCount() == 1)
				bits = num1.GetItem(0);
		}
	}
	if (bits)
	{
		if (r2.vVal && !r2.vVal->mNull)
		{
			u64 dim = 0;
			u64* counts = GrapaVector::ToShape(r2.vVal, dim);
			if (counts)
			{
				if ((r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE) && r2.vVal->vQueue && r2.vVal->vQueue->mCount)
				{
					result = GrapaLibraryRuleRandomEventCreate(bitsF, r1.vVal->mValue.mToken, bits, dim, 0, counts);
				}
				else if (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR)
				{
					GrapaRuleEvent *e = GrapaLibraryRuleRandomEventCreate(bitsF, r1.vVal->mValue.mToken, bits, dim, 0, counts);
					result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
					result->vVector = new GrapaVector();
					result->vVector->FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, e, 0);
					e->CLEAR();
					delete e;
				}
				GrapaMem::Delete(counts);
				err = 0;
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt num2;
			num2.Random(bits + 1);
			num2.unsetBit(bits);
			result = new GrapaRuleEvent(0, GrapaCHAR(""), num2.getBytes());
			err = 0;
		}
		else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaInt num2;
			num2.Random(bits);
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, num2);
			if (a.mMax < bits) a.mMax = bits;
			a = a >> bits;
			result = new GrapaRuleEvent(0, GrapaCHAR(""), a.getBytes());
			err = 0;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSetBitEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaInt num1, num2;
	num1 = 0;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mLength) num1.FromBytes(r1.vVal->mValue);
	num2 = 0;
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r2.vVal && r2.vVal->mValue.mLength) num2.FromBytes(r2.vVal->mValue);
	if (num2.GetCount() == 1 && num2.GetItem(0) >= 1)
	{
		num1.setBit(num2.GetItem(0));
		result = new GrapaRuleEvent(0, GrapaCHAR(), num1.getBytes());
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleClearBitEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaInt num1, num2;
	num1 = 0;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mLength) num1.FromBytes(r1.vVal->mValue);
	num2 = 0;
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r2.vVal && r2.vVal->mValue.mLength) num2.FromBytes(r2.vVal->mValue);
	if (num2.GetCount() == 1 && num2.GetItem(0) >= 1)
	{
		num1.unsetBit(num2.GetItem(0));
		result = new GrapaRuleEvent(0, GrapaCHAR(), num1.getBytes());
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGenBitsEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaInt num1, num2;
	num1 = 0;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mLength) num1.FromBytes(r1.vVal->mValue);
	if (num1.GetCount() == 1 && num1.GetItem(0) >= 1)
	{
		num2 = 0;
		for (u32 i = 0; i < num1.GetItem(0); i++) num2.setBit(i);
		result = new GrapaRuleEvent(0, GrapaCHAR(), num2.getBytes());
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGenKeysEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	if (r1.vVal)
	{
		GrapaEncode key;
		result = key.GenKeys(r1.vVal->mValue, r2.vVal);
	}

	err = 0;
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleEncodeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	s64 idx;
	bool needEval = true;
	if (mName.StrLowerCmp("encoderaw") == 0)
		needEval = false;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL, needEval);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR method;
		if (r2.vVal->mValue.mToken == GrapaTokenType::STR) method.FROM(r2.vVal->mValue);
		else if (r2.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			s64 idx;
			GrapaRuleEvent* x = r2.vVal->vQueue->Search("method", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) method.FROM(x->mValue);
		}
		if (method.StrLowerCmp("rsa") == 0|| method.StrLowerCmp("bc") == 0 || method.StrLowerCmp("md") == 0 || method.StrLowerCmp("rpk") == 0)
		{
			GrapaEncode key;
			bool isset = true;
			if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r2.vVal);
			else if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r3.vVal);
			else isset = false;
			if (isset)
			{
				err = -1;
				GrapaCHAR value;
				value.mToken = GrapaTokenType::RAW;
				result = new GrapaRuleEvent(0, GrapaCHAR(), value);
				result->mValue.SetLength(0);
				if (key.Encode(r1.vVal->mValue, result->mValue, 0))
					err = 0;
				if (err)
				{
					result->CLEAR();
					delete result;
					result = NULL;
				}
			}
		}
		else if (method.StrLowerCmp("AES256") == 0)
		{
			GrapaRuleQueue* sq = NULL;
			if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) sq = (GrapaRuleQueue*)r2.vVal->vQueue;
			else if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::LIST) sq = (GrapaRuleQueue*)r3.vVal->vQueue;
			if (sq)
			{
				GrapaRuleEvent* cipher = (GrapaRuleEvent*)sq->Search("cipher", idx);
				GrapaRuleEvent* key = (GrapaRuleEvent*)sq->Search("key", idx);
				GrapaRuleEvent* iv = (GrapaRuleEvent*)sq->Search("iv", idx);
				if (key)
				{
					err = 0;
					GrapaCHAR value;
					GrapaTinyAES aes;
					aes.Init(key->mValue, iv ? iv->mValue : GrapaBYTE(), 1);
					result = new GrapaRuleEvent(0, GrapaCHAR(), value);
					result->mValue.mToken = GrapaTokenType::RAW;
					aes.Encrypt(r1.vVal->mValue, result->mValue);
				}
			}
		}
		else if (method.StrLowerCmp("SHAKE256") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::RAW;
			if (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::OBJ)
			{
				GrapaBYTE s;
				r1.vVal->TO(s);
				GrapaHash::SHAKE256(s, result->mValue);
			}
			else
			{
				GrapaHash::SHAKE256(r1.vVal->mValue, result->mValue);
			}
		}
		else if (method.StrLowerCmp("SHAKE128") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::RAW;
			if (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::OBJ)
			{
				GrapaBYTE s;
				r1.vVal->TO(s);
				GrapaHash::SHAKE128(s, result->mValue);
			}
			else
			{
				GrapaHash::SHAKE128(r1.vVal->mValue, result->mValue);
			}
		}
		else if (method.StrLowerCmp("SHA3-224") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::RAW;
			if (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::OBJ)
			{
				GrapaBYTE s;
				r1.vVal->TO(s);
				GrapaHash::SHA3_224(s, result->mValue);
			}
			else
			{
				GrapaHash::SHA3_224(r1.vVal->mValue, result->mValue);
			}
		}
		else if (method.StrLowerCmp("SHA3-256") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::RAW;
			if (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::OBJ)
			{
				GrapaBYTE s;
				r1.vVal->TO(s);
				GrapaHash::SHA3_256(s, result->mValue);
			}
			else
			{
				GrapaHash::SHA3_256(r1.vVal->mValue, result->mValue);
			}
		}
		else if (method.StrLowerCmp("SHA3-384") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::RAW;
			if (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::OBJ)
			{
				GrapaBYTE s;
				r1.vVal->TO(s);
				GrapaHash::SHA3_384(s, result->mValue);
			}
			else
			{
				GrapaHash::SHA3_384(r1.vVal->mValue, result->mValue);
			}
		}
		else if (method.StrLowerCmp("SHA3-512") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::RAW;
			if (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::OBJ)
			{
				GrapaBYTE s;
				r1.vVal->TO(s);
				GrapaHash::SHA3_512(s, result->mValue);
			}
			else
			{
				GrapaHash::SHA3_512(r1.vVal->mValue, result->mValue);
			}
		}
		else if (method.StrLowerCmp("ZIP-GRAPA") == 0 || method.StrLowerCmp("GRZ") == 0)
		{
			GrapaBYTE s;
			err = 0;
			GrapaCHAR value;
			value.mToken = GrapaTokenType::RAW;
			result = new GrapaRuleEvent(0, GrapaCHAR(), value);
			switch (r1.vVal->mValue.mToken)
			{
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
			case GrapaTokenType::VECTOR:
			case GrapaTokenType::LIST:
			case GrapaTokenType::XML:
			case GrapaTokenType::EL:
			case GrapaTokenType::TAG:
			case GrapaTokenType::OP:
			case GrapaTokenType::OBJ:
				r1.vVal->TO(s);
				GrapaCompress::Compress(s, result->mValue);
				break;
			default:
				GrapaCompress::Compress(r1.vVal->mValue, result->mValue);
				break;
			}
		}
		else if (method.StrLowerCmp("BASE64") == 0)
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(), r1.vVal->mValue.ToBase(64));
		}
		else if (method.StrLowerCmp("BASE58") == 0)
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(), r1.vVal->mValue.ToBase(58));
		}
		else if ((method.Cmp("JSON-GRAPA") == 0)||(method.Cmp("JSON") == 0))
		{
			GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$function"));
			GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, r1.vVal->mValue, rulexx, 0, GrapaCHAR());
			result = vScriptExec->ProcessPlan(pNameSpace, plan);
			if (plan)
			{
				plan->CLEAR();
				delete plan;
			}
		}
		else if ((method.StrLowerCmp("XML-GRAPA") == 0) || (method.StrLowerCmp("XML") == 0))
		{
			GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$xmlcreate"));
			GrapaCHAR val("$&");
			val.Append(r1.vVal->mValue);
			val.Append("$&");
			GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, val, rulexx, 0, GrapaCHAR());
			result = vScriptExec->ProcessPlan(pNameSpace, plan);
			if (plan)
			{
				plan->CLEAR();
				delete plan;
			}
		}
		else if ((method.StrLowerCmp("HTML-GRAPA") == 0) || (method.StrLowerCmp("HTML") == 0))
		{
			GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$htmlcreate"));
			GrapaCHAR val("$&");
			val.Append(r1.vVal->mValue);
			val.Append("$&");
			GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, val, rulexx, 0, GrapaCHAR());
			result = vScriptExec->ProcessPlan(pNameSpace, plan);
			if (plan)
			{
				plan->CLEAR();
				delete plan;
			}
		}
		else if (method.StrLowerCmp("ESCAPE") == 0)
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			vScriptExec->EscapeString(r1.vVal->mValue,result->mValue);
		}
		else if (method.StrLowerCmp("FLOAT") == 0)
		{
			if (r1.vVal && r1.vVal->vQueue)
			{
				GrapaRuleQueue* sq = (GrapaRuleQueue*)r1.vVal->vQueue;
				GrapaRuleEvent* sign = (GrapaRuleEvent*)sq->Search("sign",idx);
				GrapaRuleEvent* trunc = (GrapaRuleEvent*)sq->Search("trunc", idx);
				GrapaRuleEvent* fix = (GrapaRuleEvent*)sq->Search("fix", idx);
				GrapaRuleEvent* exp = (GrapaRuleEvent*)sq->Search("exp", idx);
				GrapaRuleEvent* max = (GrapaRuleEvent*)sq->Search("max", idx);
				GrapaRuleEvent* prec = (GrapaRuleEvent*)sq->Search("extra", idx);
				GrapaRuleEvent* data = (GrapaRuleEvent*)sq->Search("data", idx);
				GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				if (sign && !sign->IsNull()) d.mSigned = true;
				if (trunc && !trunc->IsNull()) d.mTrunc = true;
				if (fix && !fix->IsNull()) d.mFix = true;
				u64 n = 0;
				GrapaInt ni;
				if (exp) { ni.FromBytes(exp->mValue); d.mExp = ni.GetItem(0); }
				if (max) { ni.FromBytes(max->mValue); d.mMax = ni.GetItem(0); }
				if (prec) { ni.FromBytes(prec->mValue); d.mExtra = ni.GetItem(0); }
				if (data) { d.mData.FromBytes(data->mValue); d.mBits = d.mData.bitCount(); }
				result = new GrapaRuleEvent(0, GrapaCHAR(), d.getBytes());
			}
		}
		else if (method.StrLowerCmp("URL-ASCII") == 0)
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::STR)
			{
				u8* v1 = r1.vVal->mValue.mBytes;
				GrapaCHAR r;
				r.SetSize(r1.vVal->mValue.mLength * 3);
				u8* v2 = r.mBytes;
				u64 j = 0;
				for (u64 i = 0; i < r1.vVal->mValue.mLength; i++)
				{
					u8 c = v1[i];
					if (
						(c <= 0x20) ||
						(c >= 0x22 && c <= 0x26) ||
						(c == 0x2B) ||
						(c == 0x2C) ||
						(c == 0x2F) ||
						(c >= 0x3A && c <= 0x40) ||
						(c >= 0x5B && c <= 0x5E) ||
						(c == 0x60) ||
						(c >= 0x7B)
						)
					{
						v2[j++] = '%';
						v2[j++] = '0' + c / 16;
						c = c % 16;
						v2[j++] = c <= 9 ? '0' + c : 'a' + (c - 10);
					}
					else
					{
						v2[j++] = v1[i];
					}
				}
				r.SetLength(j);
				result = new GrapaRuleEvent(0, GrapaCHAR(), r);
			}
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleDecodeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	s64 idx;
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR method;
		if (r2.vVal->mValue.mToken == GrapaTokenType::STR) method.FROM(r2.vVal->mValue);
		else if (r2.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			s64 idx;
			GrapaRuleEvent* x = r2.vVal->vQueue->Search("method", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) method.FROM(x->mValue);
		}
		if (method.StrLowerCmp("rsa") == 0 || method.StrLowerCmp("bc") == 0 || method.StrLowerCmp("rpk") == 0)
		{
			if (r1.vVal)
			{
				GrapaEncode key;
				bool isset = true;
				if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r2.vVal);
				else if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r3.vVal);
				else isset = false;
				if (isset)
				{
					err = -1;
					GrapaCHAR value;
					value.mToken = GrapaTokenType::RAW;
					result = new GrapaRuleEvent(0, GrapaCHAR(), value);
					u8 opt = 0;
					if (key.Decode(r1.vVal->mValue, result->mValue, opt))
						err = 0;
				}
			}
		}
		else if (method.StrLowerCmp("AES256") == 0)
		{
			GrapaRuleQueue* sq = NULL;
			if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) sq = (GrapaRuleQueue*)r2.vVal->vQueue;
			else if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::LIST) sq = (GrapaRuleQueue*)r3.vVal->vQueue;
			if (sq)
			{
				GrapaRuleEvent* key = (GrapaRuleEvent*)sq->Search("key",idx);
				GrapaRuleEvent* iv = (GrapaRuleEvent*)sq->Search("iv",idx);
				if (key)
				{
					err = 0;
					GrapaCHAR value;
					GrapaTinyAES aes;
					aes.Init(key->mValue, iv?iv->mValue: GrapaBYTE(), 0);
					result = new GrapaRuleEvent(0, GrapaCHAR(), value);
					result->mValue.mToken = GrapaTokenType::RAW;
					aes.Decrypt(r1.vVal->mValue, result->mValue);
				}
			}
		}
		else if (method.StrLowerCmp("ZIP-GRAPA") == 0 || method.StrLowerCmp("GRZ") == 0)
		{
			err = 0;
			GrapaCHAR value;
			value.mToken = GrapaTokenType::RAW;
			result = new GrapaRuleEvent(0, GrapaCHAR(), value);
			GrapaCompress::Expand(r1.vVal->mValue, result->mValue);
			switch (result->mValue.mToken)
			{
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
			case GrapaTokenType::LIST:
			case GrapaTokenType::XML:
			case GrapaTokenType::EL:
			case GrapaTokenType::TAG:
			case GrapaTokenType::OP:
			case GrapaTokenType::OBJ:
				result->vQueue = new GrapaRuleQueue();
				result->vClass = ((GrapaRuleQueue*)result->vQueue)->FROM(vScriptExec->vScriptState, pNameSpace, result->mValue);
				result->mValue.SetLength(0);
				result->mValue.SetSize(0);
				break;
			case GrapaTokenType::VECTOR:
				result->vVector = new GrapaVector();
				result->vVector->FROM(result->mValue);
				result->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$VECTOR"));
				break;
			case GrapaTokenType::WIDGET:
				result->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
				result->vWidget->vEvent = result;
				result->vWidget->FROM(result->mValue);
				result->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
				break;
			}
		}
		else if (method.StrLowerCmp("BASE64") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), r1.vVal->mValue.FromBase(64));
			result->mValue.mToken = GrapaTokenType::RAW;
		}
		else if (method.StrLowerCmp("BASE58") == 0)
		{
			err = 0;
			result = new GrapaRuleEvent(0, GrapaCHAR(), r1.vVal->mValue.FromBase(58));
			result->mValue.mToken = GrapaTokenType::RAW;
		}
		else if (method.StrLowerCmp("FLOAT") == 0)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::LIST;
			result->vQueue = new GrapaRuleQueue();
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("sign"), GrapaCHAR::SetBool(d.mSigned)));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("trunc"), GrapaCHAR::SetBool(d.mTrunc)));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("fix"), GrapaCHAR::SetBool(d.mFix)));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("exp"), GrapaInt(d.mExp).getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("max"), GrapaInt(d.mMax).getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("extra"), GrapaInt(d.mExtra).getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("data"), d.mData.getBytes()));
			if (d.mTail.mLength)
			{
				GrapaBYTE r;
				d.mTail.ToRaw(r);
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("tail"), r));
			}
		}
		else if (method.StrLowerCmp("PEM") == 0)
		{
			GrapaRuleQueue* q = GrapaNet::Decode(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			result->mValue.mToken = GrapaTokenType::LIST;
			result->vQueue = q;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSignEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR method;
		if (r2.vVal->mValue.mToken == GrapaTokenType::STR) method.FROM(r2.vVal->mValue);
		else if (r2.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			s64 idx;
			GrapaRuleEvent* x = r2.vVal->vQueue->Search("method", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) method.FROM(x->mValue);
		}
		GrapaEncode key;
		bool isset = true;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r2.vVal);
		else if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r3.vVal);
		else isset = false;
		if (isset)
		{
			GrapaBYTE enc;
			enc.mToken = GrapaTokenType::RAW;
			if (key.Sign(r1.vVal, enc, r3.vVal))
				result = new GrapaRuleEvent(0, GrapaCHAR(), enc);
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSignAddEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR method;
		if (r2.vVal->mValue.mToken == GrapaTokenType::STR) method.FROM(r2.vVal->mValue);
		else if (r2.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			s64 idx;
			GrapaRuleEvent* x = r2.vVal->vQueue->Search("method", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) method.FROM(x->mValue);
		}
		GrapaEncode key;
		bool isset = true;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r2.vVal);
		else if (r4.vVal && r4.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r4.vVal);
		else isset = false;
		if (isset)
		{
			GrapaBYTE enc;
			enc.mToken = GrapaTokenType::RAW;
			if (key.SignAdd(r3.vVal, enc, r4.vVal))
				result = new GrapaRuleEvent(0, GrapaCHAR(), enc);
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleVerifyEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	if (r1.vVal && r2.vVal && r3.vVal)
	{
		GrapaCHAR method;
		if (r2.vVal->mValue.mToken == GrapaTokenType::STR) method.FROM(r2.vVal->mValue);
		else if (r2.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			s64 idx;
			GrapaRuleEvent* x = r2.vVal->vQueue->Search("method", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) method.FROM(x->mValue);
		}
		GrapaEncode key;
		bool isset = true;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r2.vVal);
		else if (r4.vVal && r4.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r4.vVal);
		else isset = false;
		if (isset)
		{
			bool success = key.Verify(r3.vVal, r1.vVal->mValue, r4.vVal);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(success ? 1 : 0).getBytes());
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleVerifyRecoverEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR method;
		if (r2.vVal->mValue.mToken == GrapaTokenType::STR) method.FROM(r2.vVal->mValue);
		else if (r2.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			s64 idx;
			GrapaRuleEvent* x = r2.vVal->vQueue->Search("method", idx);
			while (x && x->mValue.mToken == GrapaTokenType::PTR) x = x->vRulePointer;
			if (x) method.FROM(x->mValue);
		}
		GrapaEncode key;
		bool isset = true;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r2.vVal);
		else if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::LIST) key.FROM(r3.vVal);
		else isset = false;
		if (isset)
		{
			GrapaBYTE d;
			bool success = key.VerifyRecover(d, r1.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), d);
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSecretEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaError err = -1;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaEncode key;
		if (key.FROM(r1.vVal))
		{
			GrapaBYTE d;
			bool success = key.Secret(r2.vVal, d);
			result = new GrapaRuleEvent(0, GrapaCHAR(), d);
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);
	return(result);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleMessageEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		result = pNameSpace->GetResponse()->SendM(vScriptExec, pNameSpace, r1.vVal);
	}
	return(result);
}


GrapaRuleEvent* GrapaLibraryRuleStringEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	if (GrapaRuleEvent* p1 = pInput->Head(0))
	{
		if (GrapaRuleEvent* r = vScriptExec->ProcessPlan(pNameSpace, p1))
		{
			if (r->mValue.mToken == GrapaTokenType::PTR && r->vRulePointer)
				vScriptExec->EchoString(pNameSpace->GetResponse(), r->vRulePointer->mValue);
			else
				vScriptExec->EchoString(pNameSpace->GetResponse(), r->mValue);
			r->CLEAR();
			delete r;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleEchoEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	if (GrapaRuleEvent* p1 = pInput->Head(0))
	{
		if (GrapaRuleEvent* r = vScriptExec->ProcessPlan(pNameSpace, p1))
		{
			if (r->mValue.mToken == GrapaTokenType::PTR && r->vRulePointer)
			{
				if (r->vRulePointer->vQueue)
				{
					vScriptExec->EchoList(pNameSpace->GetResponse(), r->vRulePointer, false, false, false);
				}
				else vScriptExec->EchoValue(pNameSpace->GetResponse(), r->vRulePointer, false, false, false);
			}
			else if (r->mValue.mToken == GrapaTokenType::ERR)
			{
				// not sure what to do
			}
			else
			{
				if (r->vQueue) vScriptExec->EchoList(pNameSpace->GetResponse(), r, false, false, false);
				else vScriptExec->EchoValue(pNameSpace->GetResponse(), r, false, false, false);
			}
			r->CLEAR();
			delete r;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleConsoleEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	if (GrapaRuleEvent* p1 = pInput->Head(0))
	{
		if (GrapaRuleEvent* r = vScriptExec->ProcessPlan(pNameSpace, p1))
		{
			if (r->mValue.mToken == GrapaTokenType::PTR && r->vRulePointer)
			{
				if (r->vRulePointer->vQueue) vScriptExec->EchoList(pNameSpace->GetResponse(), r->vRulePointer, false, false, false);
				else vScriptExec->EchoValue(pNameSpace->GetResponse(), (GrapaRuleEvent*)r->vRulePointer, false, false, false);
			}
			else if (r->mValue.mToken == GrapaTokenType::ERR)
			{
				// not sure what to do
			}
			else
			{
				if (r->vQueue) vScriptExec->EchoList(pNameSpace->GetResponse(), r, false, false, false);
				else vScriptExec->EchoValue(pNameSpace->GetResponse(), r, false, false, false);
			}
			r->CLEAR();
			delete r;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePromptEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	vScriptExec->vScriptState->SendPrompt(pNameSpace);
	return(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleAddEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;

	GrapaCHAR rStr, rStr2, item;
	GrapaInt rInt, rInt2;
	GrapaFloat rFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	GrapaFloat rFloat2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	GrapaVector* rV = NULL;
	GrapaWidget* rW = NULL;
	GrapaVector aa;

	u8 e;
	e = GrapaTokenType::STR;

	GrapaRuleEvent* p = pInput ? pInput->Head() : NULL;
	while (p)
	{
		GrapaLibraryParam r1(vScriptExec, pNameSpace, p);

		if (r1.vVal)
		{
			if (p == pInput->Head())
			{
				e = r1.vVal->mValue.mToken;

				switch (r1.vVal->mValue.mToken)
				{
				case GrapaTokenType::ID:
				case GrapaTokenType::STR:
					e = GrapaTokenType::STR;
					rStr.FROM(r1.vVal->mValue);
					break;
				case GrapaTokenType::INT:
					rInt.FromBytes(r1.vVal->mValue);
					break;
				case GrapaTokenType::FLOAT:
					rFloat.mMax = vScriptExec->vScriptState->mItemState.mFloatMax;
					rFloat.mFix = vScriptExec->vScriptState->mItemState.mFloatFix;
					rFloat.mExtra = vScriptExec->vScriptState->mItemState.mFloatExtra;
					rFloat.FromBytes(r1.vVal->mValue);
					break;
				case GrapaTokenType::TIME:
					rInt.FromBytes(r1.vVal->mValue);
					break;
				case GrapaTokenType::RAW:
					e = GrapaTokenType::RAW;
					r1.vVal->mValue.ToHex(rStr);
					break;
				case GrapaTokenType::VECTOR:
					if (rV == NULL && r1.vVal->vVector)
					{
						rV = new GrapaVector(*r1.vVal->vVector);
					}
					break;
				case GrapaTokenType::WIDGET:
					if (rW == NULL && r1.vVal->vWidget)
					{
						rW = new GrapaWidget(vScriptExec, pNameSpace);
						rW->New("group");
						rW->Add(r1.vVal->mName,r1.vVal->vWidget);
					}
					break;
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::TUPLE:
				{
						rV = new GrapaVector();
						rV->FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
					}
					break;
				default:
					e = GrapaTokenType::STR;
					rStr.FROM(r1.vVal->mValue);
					break;
				}
			}
			else
			{
				switch (e)
				{
				case GrapaTokenType::ID:
				case GrapaTokenType::STR:
					switch (r1.vVal->mValue.mToken)
					{
					case GrapaTokenType::ID:
					case GrapaTokenType::STR:
						rStr.Append(r1.vVal->mValue);
						break;
					case GrapaTokenType::INT:
						rInt.FromBytes(r1.vVal->mValue);
						rStr.Append(rInt.ToString());
						break;
					case GrapaTokenType::FLOAT:
						rFloat.mMax = vScriptExec->vScriptState->mItemState.mFloatMax;
						rFloat.mFix = vScriptExec->vScriptState->mItemState.mFloatFix;
						rFloat.mExtra = vScriptExec->vScriptState->mItemState.mFloatExtra;
						rFloat.FromBytes(r1.vVal->mValue);
						rStr.Append(rFloat.ToString());
						break;
					case GrapaTokenType::TIME:
						rInt.FromBytes(r1.vVal->mValue);
						rStr.Append(rInt.ToString());
						break;
					default:
						e = GrapaTokenType::STR;
						rStr.Append(r1.vVal->mValue);
						break;
					}
					break;
				case GrapaTokenType::RAW:
					switch (r1.vVal->mValue.mToken)
					{
					case GrapaTokenType::RAW:
						r1.vVal->mValue.ToHex(rStr2);
						rStr2.LTrim('0');
						rStr.Append(rStr2);
						break;
					}
					break;
				case GrapaTokenType::INT:
					switch (r1.vVal->mValue.mToken)
					{
					case GrapaTokenType::ID:
					case GrapaTokenType::STR:
						break;
					case GrapaTokenType::INT:
						rInt2.FromBytes(r1.vVal->mValue);
						rInt = rInt + rInt2;
						break;
					case GrapaTokenType::FLOAT:
						rFloat.mMax = vScriptExec->vScriptState->mItemState.mFloatMax;
						rFloat.mFix = vScriptExec->vScriptState->mItemState.mFloatFix;
						rFloat.mExtra = vScriptExec->vScriptState->mItemState.mFloatExtra;
						rFloat.FromBytes(r1.vVal->mValue);
						rFloat2 = rInt;
						rFloat = rFloat + rFloat2;
						e = GrapaTokenType::FLOAT;
						break;
					case GrapaTokenType::TIME:
						rInt2.FromBytes(r1.vVal->mValue);
						rInt = rInt + rInt2;
						break;
					default:
						break;
					}
					break;
				case GrapaTokenType::FLOAT:
					switch (r1.vVal->mValue.mToken)
					{
					case GrapaTokenType::ID:
					case GrapaTokenType::STR:
						break;
					case GrapaTokenType::INT:
						rInt2.FromBytes(r1.vVal->mValue);
						rFloat = rFloat + rInt2;
						break;
					case GrapaTokenType::FLOAT:
						rFloat2.mMax = vScriptExec->vScriptState->mItemState.mFloatMax;
						rFloat2.mFix = vScriptExec->vScriptState->mItemState.mFloatFix;
						rFloat2.mExtra = vScriptExec->vScriptState->mItemState.mFloatExtra;
						rFloat2.FromBytes(r1.vVal->mValue);
						rFloat = rFloat + rFloat2;
						break;
					case GrapaTokenType::TIME:
						break;
					default:
						break;
					}
					break;
				case GrapaTokenType::TIME:
					switch (r1.vVal->mValue.mToken)
					{
					case GrapaTokenType::ID:
					case GrapaTokenType::STR:
						break;
					case GrapaTokenType::INT:
						rInt2.FromBytes(r1.vVal->mValue);
						rInt = rInt + rInt2;
						break;
					case GrapaTokenType::FLOAT:
						break;
					case GrapaTokenType::TIME:
						rInt2.FromBytes(r1.vVal->mValue);
						rInt = rInt + rInt2;
						break;
					default:
						break;
					}
					break;
				case GrapaTokenType::VECTOR:
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::TUPLE:
					if (!rV) break;
					switch (r1.vVal->mValue.mToken)
					{
					case GrapaTokenType::VECTOR:
						rV->Add(vScriptExec, pNameSpace, *r1.vVal->vVector);
						break;
					case GrapaTokenType::ARRAY:
					case GrapaTokenType::TUPLE:
						aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
						rV->Add(vScriptExec, pNameSpace, aa);
						break;
					case GrapaTokenType::INT:
					case GrapaTokenType::FLOAT:
						aa.FromValue(vScriptExec, r1.vVal->mValue);
						rV->Add(vScriptExec, pNameSpace, aa);
						break;
					}
					break;
				case GrapaTokenType::WIDGET:
					if (!rV) break;
					switch (r1.vVal->mValue.mToken)
					{
					case GrapaTokenType::WIDGET:
						rW->Add(r1.vVal->mName, r1.vVal->vWidget);
						break;
					}
					break;
				}
			}
		}
		p = p->Next();
	}

	switch (e)
	{
	case GrapaTokenType::STR:
		result = new GrapaRuleEvent(0, item, rStr);
		break;
	case GrapaTokenType::RAW:
		rStr.FromHex(rStr2);
		result = new GrapaRuleEvent(0, item, rStr2);
		break;
	case GrapaTokenType::INT:
		result = new GrapaRuleEvent(0, item, rInt.getBytes());
		break;
	case GrapaTokenType::FLOAT:
		result = new GrapaRuleEvent(0, item, rFloat.getBytes());
		break;
	case GrapaTokenType::TIME:
		result = new GrapaRuleEvent(0, item, rInt.getBytes());
		result->mValue.mToken = GrapaTokenType::TIME;
		break;
	case GrapaTokenType::VECTOR:
		result = new GrapaRuleEvent(0, item, item);
		result->mValue.mToken = GrapaTokenType::VECTOR;
		result->vVector = rV;
		rV = NULL;
		break;
	case GrapaTokenType::WIDGET:
		result = new GrapaRuleEvent(0, item, item);
		result->mValue.mToken = GrapaTokenType::WIDGET;
		result->vWidget = rW;
		result->vWidget->vEvent = result;
		rW->vEvent = result;
		rW = NULL;
		break;
	case GrapaTokenType::ARRAY:
		result = rV->ToArray();
		break;
	case GrapaTokenType::TUPLE:
		result = rV->ToTuple();
		break;
	}

	if (rV)
	{
		rV->CLEAR();
		delete rV;
		rV = NULL;
	}

	if (rW)
	{
		rW->CLEAR();
		delete rW;
		rW = NULL;
	}

	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSubEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	if (r1.vVal && r2.vVal && r1.vVal->mValue.mBytes && r2.vVal->mValue.mBytes)
	{
		GrapaCHAR item;
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && (r2.vVal->mValue.mToken == GrapaTokenType::TIME || r2.vVal->mValue.mToken == GrapaTokenType::INT))
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (a - b).getBytes());
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra,0);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				d.FROM(a);
			}
			else
			{
				d.FromBytes(r1.vVal->mValue);
			}
			if (r2.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				b.FromBytes(r2.vVal->mValue);
				d2.FROM(b);
			}
			else
			{
				d2.FromBytes(r2.vVal->mValue);
			}
			result = new GrapaRuleEvent(0, item, (d - d2).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::TIME && (r2.vVal->mValue.mToken == GrapaTokenType::TIME || r2.vVal->mValue.mToken == GrapaTokenType::INT))
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (a - b).getBytes());
			result->mValue.mToken = GrapaTokenType::TIME;
			if (r2.vVal->mValue.mToken == GrapaTokenType::TIME)
				result->mValue.mToken = GrapaTokenType::INT;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::STR && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a = r1.vVal->mValue.StrCmp(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, item, a.getBytes());
		}
	}
	else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector(*r1.vVal->vVector);
		GrapaError err = -1;
		GrapaVector bb;
		switch (r2.vVal->mValue.mToken)
		{
		case GrapaTokenType::VECTOR:
			err = result->vVector->Add(vScriptExec, pNameSpace, *r2.vVal->vVector, true);
			break;
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
			bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
			err = result->vVector->Add(vScriptExec, pNameSpace, bb, true);
			break;
		case GrapaTokenType::INT:
			bb.FromValue(vScriptExec, r2.vVal->mValue);
			err = result->vVector->Add(vScriptExec, pNameSpace, bb, true);
			break;
		case GrapaTokenType::FLOAT:
			bb.FromValue(vScriptExec, r2.vVal->mValue);
			err = result->vVector->Add(vScriptExec, pNameSpace, bb, true);
			break;
		}
		if (err)
		{
			result->CLEAR();
			delete result;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaError err = -1;
		GrapaVector bb;
		switch (r2.vVal->mValue.mToken)
		{
		case GrapaTokenType::VECTOR:
			err = aa.Add(vScriptExec, pNameSpace, *r2.vVal->vVector, true);
			break;
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
			bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
			err = aa.Add(vScriptExec, pNameSpace, bb, true);
			break;
		case GrapaTokenType::INT:
			bb.FromValue(vScriptExec, r2.vVal->mValue);
			err = aa.Add(vScriptExec, pNameSpace, bb, true);
			break;
		case GrapaTokenType::FLOAT:
			bb.FromValue(vScriptExec, r2.vVal->mValue);
			err = aa.Add(vScriptExec, pNameSpace, bb, true);
			break;
		}
		if (err)
			result = Error(vScriptExec, pNameSpace, -1);
		else
			result = aa.ToArray();
	}
	else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::WIDGET && (r2.vVal->mValue.mToken == GrapaTokenType::WIDGET || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::STR))
	{
		result = new GrapaRuleEvent(GrapaTokenType::WIDGET, 0, "", "");
		result->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		result->vWidget->vEvent = result;
		result->vWidget->FROM(*r1.vVal->vWidget);
		result->vWidget->vEvent = result;
		GrapaError err = -1;
		GrapaInt bb;
		err = result->vWidget->Remove(r2.vVal);
		if (err)
		{
			result->CLEAR();
			delete result;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleMulEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a, b, c;
		GrapaFloat f1(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		GrapaFloat f2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		GrapaRuleEvent* lst=NULL;

		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			c = a * b;
			result = new GrapaRuleEvent(0, item, c.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d2.FromBytes(r2.vVal->mValue);
			d = d * d2;
			result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			b.FromBytes(r2.vVal->mValue);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
			d.FromBytes(r1.vVal->mValue);
			d = d * d2;
			result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			d2.FromBytes(r2.vVal->mValue);
			d = d * d2;
			result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::STR && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r2.vVal->mValue);
			s64 b = a.LongValue();
			char buffer[128];
			buffer[0] = 0;
			for (s64 i = 0; (i < b && strlen(buffer) < (127 - r1.vVal->mValue.mLength)); i++) GrapaMem::StrCat(buffer, sizeof(buffer), (char*)r1.vVal->mValue.mBytes);
			result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", buffer);
		}
		//else if ((r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::CODE) && r1.vVal->vQueue && r2.vVal->mValue.mToken == GrapaTokenType::STR)
		//{
		//	result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
		//	GrapaRuleEvent *e = (GrapaRuleEvent*)r1.vVal->vQueue->Head();
		//	u64 sz = 0;
		//	while (e)
		//	{
		//		sz += e->mValue.mLength;
		//		e = e->Next();
		//		if (e) sz += r2.vVal->mValue.mLength;
		//	}
		//	result->mValue.SetLength(sz, false);
		//	result->mValue.SetLength(0, false);
		//	e = (GrapaRuleEvent*)r1.vVal->vQueue->Head();
		//	while (e)
		//	{
		//		if (e->mValue.mBytes)
		//			result->mValue.Append(e->mValue);
		//		e = e->Next();
		//		if (e&&r2.vVal->mValue.mBytes) 
		//			result->mValue.Append(r2.vVal->mValue);
		//	}
		//}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE) && r1.vVal->vQueue && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::INT:
				a.FromBytes(r2.vVal->mValue);
				result = vScriptExec->CopyItem(r1.vVal);
				lst = result->vQueue->Head();
				while (lst)
				{
					switch (lst->mValue.mToken)
					{
					case GrapaTokenType::INT:
						b.FromBytes(lst->mValue);
						lst->mValue.FROM((a * b).getBytes());
						break;
					case GrapaTokenType::FLOAT:
						f2.FromBytes(lst->mValue);
						lst->mValue.FROM((f2 * a).getBytes());
						break;
					}
					lst = lst->Next();
				}
				break;
			case GrapaTokenType::FLOAT:
				f1.FromBytes(r2.vVal->mValue);
				result = vScriptExec->CopyItem(r1.vVal);
				lst = result->vQueue->Head();
				while (lst)
				{
					switch (lst->mValue.mToken)
					{
					case GrapaTokenType::INT:
						b.FromBytes(lst->mValue);
						lst->mValue.FROM((f1 * b).getBytes());
						break;
					case GrapaTokenType::FLOAT:
						f2.FromBytes(lst->mValue);
						lst->mValue.FROM((f1 * f2).getBytes());
						break;
					}
					lst = lst->Next();
				}
				break;
			}
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
			result->vVector = new GrapaVector(*r1.vVal->vVector);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = result->vVector->Mul(vScriptExec, pNameSpace, *r2.vVal->vVector, false);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = result->vVector->Mul(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Mul(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Mul(vScriptExec, pNameSpace, bb, false);
				break;
			}
			if (err)
			{
				result->CLEAR();
				delete result->vVector;
				result->vVector = NULL;
				result = Error(vScriptExec, pNameSpace, -1);
			}
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			GrapaVector aa;
			aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = aa.Mul(vScriptExec, pNameSpace, *r2.vVal->vVector, false);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = aa.Mul(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Mul(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Mul(vScriptExec, pNameSpace, bb, false);
				break;
			}
			if (err)
				result = Error(vScriptExec, pNameSpace, -1);
			else
				result = aa.ToArray();
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::CODE) && r1.vVal->vQueue && r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
		{
			if (r2.vVal->mValue.Cmp("TAG") == 0)
			{
			}
			else if (r2.vVal->mValue.Cmp("EL") == 0)
			{
			}
			else if (r2.vVal->mValue.Cmp("CSV") == 0)
			{
				// convert the list to a CSV
				result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
				GrapaRuleEvent* x = r1.vVal;
				while (x->vRulePointer) x = x->vRulePointer;
				if ((x->mValue.mToken == GrapaTokenType::ARRAY || x->mValue.mToken == GrapaTokenType::TUPLE || x->mValue.mToken == GrapaTokenType::LIST || x->mValue.mToken == GrapaTokenType::XML || x->mValue.mToken == GrapaTokenType::EL || x->mValue.mToken == GrapaTokenType::TAG) && x->vQueue)
				{
					x = (GrapaRuleEvent*)x->vQueue->Head();
					while (x)
					{
						if ((x->mValue.mToken == GrapaTokenType::ARRAY || x->mValue.mToken == GrapaTokenType::TUPLE || x->mValue.mToken == GrapaTokenType::LIST || x->mValue.mToken == GrapaTokenType::XML || x->mValue.mToken == GrapaTokenType::EL || x->mValue.mToken == GrapaTokenType::TAG || x->mValue.mToken == GrapaTokenType::OP || x->mValue.mToken == GrapaTokenType::CODE))
						{
							GrapaRuleEvent* y = (GrapaRuleEvent*)x->vQueue->Head();
							while (y)
							{
								switch (y->mValue.mToken)
								{
								case GrapaTokenType::STR:
								case GrapaTokenType::SYSSTR:
								case GrapaTokenType::ID:
								case GrapaTokenType::SYSID:
								{
									bool hasSpace = y->mValue.mBytes?strpbrk((const char*)y->mValue.mBytes, (const char*)" ,\"") != NULL : false;
									bool hasQuote = y->mValue.mBytes ? GrapaMem::MemFind(y->mValue.mBytes, '\"', y->mValue.mLength) != NULL : false;
									if (hasSpace) result->mValue.GrapaCHAR::Append('\"');
									if (hasQuote && y->mValue.mBytes)
									{
										GrapaCHAR s(y->mValue);
										s.Replace(GrapaBYTE("\""), GrapaBYTE("\"\""));
										result->mValue.GrapaCHAR::Append(s);
									}
									else
									{
										if (y->mValue.mBytes) 
											result->mValue.Append(y->mValue);
									}
									if (hasSpace) result->mValue.GrapaCHAR::Append('\"');
								}
								break;
								case GrapaTokenType::INT:
								case GrapaTokenType::SYSINT:
								{
									if (y->mValue.mBytes)
									{
										GrapaInt b;
										b.FromBytes(y->mValue);
										result->mValue.GrapaCHAR::Append(b.ToString());
									}
								}
								break;
								}
								y = y->Next();
								if (y) result->mValue.GrapaCHAR::Append(',');
							}
						}
						x = x->Next();
						if (x) result->mValue.GrapaCHAR::Append("\r\n");
					}
				}
			}
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

void _next_div_token(char& sep, char*& sC, u64& lenC, char*& vS, u64& vL)
{
	if (*sC == '"')
	{
		if (lenC)
		{
			//u64 trunc = 0;
			char* sC2 = sC + 1;
			char* v = sC2;
			u64 lenC2 = lenC - 1;
			while (lenC2)
			{
				if (*v == '"')
				{
					if (lenC2 > 1 && v[1] == '"')
					{
						v = v + 1;
						lenC2--;
					}
					else
						break;
				}
				*sC2 = *v;
				sC2 = sC2 + 1;
				v = v + 1;
				lenC2--;
			}
			vS = &sC[1];
			vL = sC2 - sC - 1;
			lenC = lenC - (v - sC) - 1;
			sC = v + 1;
			if (sep == 0)
			{
				u64 lenC2 = lenC;
				char* sC2 = sC;
				while (lenC2 && *sC2 != ',' && *sC2 != '\t')
				{
					lenC2--;
					sC2 = sC2 + 1;
				}
				if (lenC2 && *sC2 == ',') sep = ',';
				else if (lenC2 && *sC2 == '\t') sep = '\t';
				else sep = ',';
			}
			if (lenC && *sC == sep)
			{
				lenC--;
				sC = sC + 1;
			}
		}
		else
		{
			vS = sC;
			vL = 0;
			lenC = 0;
			sC = NULL;
		}
	}
	else
	{
		if (sep == 0)
		{
			u64 lenC2 = lenC;
			char* sC2 = sC;
			while (lenC2 && *sC2 != ',' && *sC2 != '\t')
			{
				lenC2--;
				sC2 = sC2 + 1;
			}
			if (lenC2 && *sC2 == ',') sep = ',';
			else if (lenC2 && *sC2 == '\t') sep = '\t';
			else sep = ',';
		}
		if (char* sC2 = strchr(sC, (int)sep))
		{
			vS = sC;
			vL = sC2 - sC;
			lenC = lenC - (sC2 - sC) - 1;
			sC = sC2 + 1;
		}
		else
		{
			vS = sC;
			vL = lenC;
			lenC = 0;
			sC = NULL;

		}
	}
}

GrapaRuleEvent* GrapaLibraryRuleDivEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a, b, c;
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::TIME) && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			d2.FromBytes(r2.vVal->mValue);
			//a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			d = d / d2;
			if (d.mNaN)
			{
				result = Error(vScriptExec, pNameSpace, -1);
			}
			else
			{
				if (d.IsInt())
					result = new GrapaRuleEvent(0, item, d.ToInt().getBytes());
				else
					result = new GrapaRuleEvent(0, item, d.getBytes());
			}
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::TIME || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			d2.FromBytes(r2.vVal->mValue);
			d = d / d2;
			if (d.mNaN)
			{
				result = Error(vScriptExec, pNameSpace, -1);
			}
			else
			{
				result = new GrapaRuleEvent(0, item, d.getBytes());
			}
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
			result->vVector = new GrapaVector(*r1.vVal->vVector);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = result->vVector->Mul(vScriptExec, pNameSpace, *r2.vVal->vVector, true);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = result->vVector->Mul(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Mul(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Mul(vScriptExec, pNameSpace, bb, true);
				break;
			}
			if (err)
			{
				result->CLEAR();
				delete result->vVector;
				result->vVector = NULL;
				result = Error(vScriptExec, pNameSpace, -1);
			}
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			GrapaVector aa;
			aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = aa.Mul(vScriptExec, pNameSpace, *r2.vVal->vVector, true);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = aa.Mul(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Mul(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Mul(vScriptExec, pNameSpace, bb, true);
				break;
			}
			if (err)
				result = Error(vScriptExec, pNameSpace, -1);
			else
				result = aa.ToArray();
		}
		//else if (r1.vVal->mValue.mToken == GrapaTokenType::STR && r2.vVal->mValue.mToken == GrapaTokenType::STR)
		//{
		//	result = new GrapaRuleEvent();
		//	result->mValue.mToken = GrapaTokenType::ARRAY;
		//	result->vQueue = new GrapaRuleQueue();
		//	if (r1.vVal->mValue.mLength)
		//	{
		//		GrapaCHAR del(r2.vVal->mValue);
		//		//if (r2.vVal->mValue.mLength == 0)
		//		//{
		//		//	char* pr = (char*)GrapaMem::MemFind(r1.vVal->mValue.mBytes, '\r', r1.vVal->mValue.mLength);
		//		//	char* pn = (char*)GrapaMem::MemFind(r1.vVal->mValue.mBytes, '\n', r1.vVal->mValue.mLength);
		//		//	if (pn > (pr + 1)) pn = NULL;
		//		//	if (pr&&pn)
		//		//	{
		//		//		if (pr < pn)
		//		//			del.FROM("\r\n");
		//		//		else
		//		//			del.FROM("\n\r");
		//		//	}
		//		//	else if (pn)
		//		//		del.FROM("\n");
		//		//	else
		//		//		del.FROM("\r");
		//		//}
		//		//else
		//		//	del.FROM(r2.vVal->mValue);
		//		GrapaRuleEvent* r = NULL;
		//		char*s = (char*)r1.vVal->mValue.mBytes;
		//		char*s2;
		//		u64 len = r1.vVal->mValue.mLength;
		//		if (del.mLength == 0)
		//		{
		//			while (len)
		//			{
		//				s2 = &s[1];
		//				r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(s, s2 - s));
		//				if (r == NULL)
		//				{
		//					s = NULL;
		//					break;
		//				}
		//				r->mValue.mToken = GrapaTokenType::STR;
		//				result->vQueue->PushTail(r);
		//				len = len - (s2 - s) - 0;
		//				s = s2 + 0;
		//			}
		//			s = NULL;
		//		}
		//		else
		//		{
		//			while ((s2 = strstr(s, (char*)del.mBytes)))
		//			{
		//				r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(s, s2 - s));
		//				if (r == NULL)
		//				{
		//					s = NULL;
		//					break;
		//				}
		//				r->mValue.mToken = GrapaTokenType::STR;
		//				result->vQueue->PushTail(r);
		//				len = len - (s2 - s) - del.mLength;
		//				s = s2 + del.mLength;
		//			}
		//		}
		//		if (s)
		//		{
		//			r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(s, len));
		//			if (r)
		//			{
		//				r->mValue.mToken = GrapaTokenType::STR;
		//				result->vQueue->PushTail(r);
		//			}
		//		}
		//	}
		//}
		//else if (r1.vVal->mValue.mToken == GrapaTokenType::STR && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		//{
		//	result = new GrapaRuleEvent();
		//	result->mValue.mToken = GrapaTokenType::ARRAY;
		//	result->vQueue = new GrapaRuleQueue();
		//	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
		//	GrapaCHAR del;
		//	if (r3.vVal)
		//		del.FROM(r3.vVal->mValue);
		//	if (r1.vVal->mValue.mLength)
		//	{
		//		a.FromBytes(r2.vVal->mValue);
		//		u32 n = a.GetItem(0);
		//		u32 p = (u32)((r1.vVal->mValue.mLength / n) + 1);
		//		u32 p_i = 0;
		//		u32 p_c = (u32) r1.vVal->mValue.mLength;
		//		GrapaRuleEvent* r = NULL;
		//		char* s = (char*)r1.vVal->mValue.mBytes;
		//		while (p_i < p_c)
		//		{
		//			u32 len = p;
		//			if ((p_i + len) > p_c)
		//				len = p_c - p_i;

		//			if (del.mLength && len>=del.mLength)
		//			{
		//				char* t_s = strstr(s + len - del.mLength, (char*)del.mBytes);
		//				if (t_s)
		//					len = (u32) (t_s - s + del.mLength);
		//				else
		//					len = p_c - p_i;
		//			}
		//			/*
		//				u32 t_i = p_i + len - del.mLength;
		//				char* t_s = s + len - del.mLength;
		//				bool found = false;
		//				while (t_s != s)
		//				{
		//					while (t_i != p_i && *t_s != *del.mBytes & t_s != s) 
		//					{ 
		//						t_i--; 
		//						t_s--; 
		//					}
		//					if (GrapaMem::MemCompare(t_s, del.mBytes, del.mLength) == 0)
		//					{
		//						len = t_i - p_i + del.mLength;
		//						found = true;
		//						break;
		//					}
		//					t_i--;
		//					t_s--;
		//				}
		//				if (!found)
		//				{
		//					u32 skip = 0;
		//					if (len >= del.mLength) skip = len - del.mLength;
		//					t_s = strstr(s + skip, (char*)del.mBytes);
		//					if (t_s)
		//						len = t_s - s + del.mLength;
		//				}
		//				*/
		//			r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(s, len));
		//			if (r)
		//			{
		//				r->mValue.mToken = GrapaTokenType::STR;
		//				result->vQueue->PushTail(r);
		//			}
		//			s += len;
		//			p_i += len;
		//		}
		//	}
		//}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::STR && r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
		{
			if (r2.vVal->mValue.Cmp("EL") == 0)
			{
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::LIST;
				result->vQueue = new GrapaRuleQueue();
				GrapaRuleEvent* r = NULL;
				char*s = (char*)r1.vVal->mValue.mBytes;
				char*s2, *s3, *s4, *s5;
				u64 len = r1.vVal->mValue.mLength;
				while ((s2 = strchr(s, (int)'<')))
				{
					s3 = strchr(&s2[1], (int)'>');
					if (s3 == NULL) { s = NULL; break; }
					s4 = strchr(&s3[1], (int)'<');
					if (s4 == NULL) { s = NULL; break; }
					s5 = strchr(&s4[1], (int)'>');
					if (s5 == NULL) { s = NULL; break; }
					r = new GrapaRuleEvent(0, GrapaCHAR(&s2[1], s3 - s2 - 1), GrapaCHAR(&s3[1], s4 - s3 - 1));
					if (r == NULL) { s = NULL; break; }
					r->mValue.mToken = GrapaTokenType::STR;
					result->vQueue->PushTail(r);
					len = len - (s5 - s) - 1;
					s = s5 + 1;
				}
			}
			else if (r2.vVal->mValue.Cmp("CSV") == 0)	// ="\"this\",is,\"a big\",test\r\nand,another" / $CSV
			{
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::ARRAY;
				result->vQueue = new GrapaRuleQueue();
				char*s = (char*)r1.vVal->mValue.mBytes;
				GrapaCHAR del;
				GrapaCHAR line;
				char* pr = (char*)GrapaMem::MemFind(r1.vVal->mValue.mBytes, '\r', r1.vVal->mValue.mLength);
				char* pn = (char*)GrapaMem::MemFind(r1.vVal->mValue.mBytes, '\n', r1.vVal->mValue.mLength);
				if (pn > (pr + 1)) pn = NULL;
				if (pr&&pn)
				{
					if (pr < pn)
						del.FROM("\r\n");
					else
						del.FROM("\n\r");
				}
				else if (pn)
					del.FROM("\n");
				else
					del.FROM("\r");
				char sep = 0;
				u64 len = r1.vVal->mValue.mLength;
				while (len)
				{
					GrapaRuleEvent* e = new GrapaRuleEvent();
					e->mValue.mToken = GrapaTokenType::ARRAY;
					e->vQueue = new GrapaRuleQueue();
					result->vQueue->PushTail(e);
					if (char*s2 = strstr(s, (char*)del.mBytes))
					{
						line.FROM(s, s2 - s);
						len = len - (s2 - s) - del.mLength;
						s = s2 + del.mLength;
					}
					else
					{
						line.FROM(s, len);
						len = 0;
						s = NULL;
					}

					u64 lenC = line.mLength;
					char*sC = (char*)line.mBytes;
					while (lenC)
					{
						GrapaRuleEvent* r = NULL;
						char* vS = NULL;
						u64 vL = 0;
						_next_div_token(sep, sC, lenC, vS, vL);
						r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(vS, vL));
						e->vQueue->PushTail(r);

						//if (*sC == '"')
						//{
						//	if (lenC)
						//	{
						//		//u64 trunc = 0;
						//		char* sC2 = sC + 1;
						//		char* v = sC2;
						//		u64 lenC2 = lenC - 1;
						//		while (lenC2)
						//		{
						//			if (*v == '"')
						//			{
						//				if (lenC2 > 1 && v[1] == '"')
						//				{
						//					v = v + 1;
						//					lenC2--;
						//				}
						//				else
						//					break;
						//			}
						//			*sC2 = *v;
						//			sC2 = sC2 + 1;
						//			v = v + 1;
						//			lenC2--;
						//		}
						//		r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(&sC[1], sC2 - sC - 1));
						//		lenC = lenC - (v - sC) - 1;
						//		sC = v + 1;
						//		if (sep == 0)
						//		{
						//			u64 lenC2 = lenC;
						//			char* sC2 = sC;
						//			while (lenC2 && *sC2 != ',' && *sC2 != '\t')
						//			{
						//				lenC2--;
						//				sC2 = sC2 + 1;
						//			}
						//			if (lenC2 && *sC2 == ',') sep = ',';
						//			else if (lenC2 && *sC2 == '\t') sep = '\t';
						//			else sep = ',';
						//		}
						//		if (lenC && *sC == sep)
						//		{
						//			lenC--;
						//			sC = sC + 1;
						//		}
						//	}
						//	else
						//	{
						//		r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
						//		lenC = 0;
						//		sC = NULL;
						//	}
						//}
						//else
						//{
						//	if (sep == 0)
						//	{
						//		u64 lenC2 = lenC;
						//		char* sC2 = sC;
						//		while (lenC2 && *sC2 != ',' && *sC2 != '\t')
						//		{
						//			lenC2--;
						//			sC2 = sC2 + 1;
						//		}
						//		if (lenC2 && *sC2 == ',') sep = ',';
						//		else if (lenC2 && *sC2 == '\t') sep = '\t';
						//		else sep = ',';
						//	}
						//	if (char* sC2 = strchr(sC, (int)sep))
						//	{
						//		r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(sC, sC2 - sC));
						//		lenC = lenC - (sC2 - sC) - 1;
						//		sC = sC2 + 1;
						//	}
						//	else
						//	{
						//		r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(sC, lenC));
						//		lenC = 0;
						//		sC = NULL;

						//	}
						//}
						//if (r)
						//{
						//	r->mValue.mToken = GrapaTokenType::STR;
						//	e->vQueue->PushTail(r);
						//}
					}
				}
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::TABLE && r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
		{
			if (r2.vVal->mValue.Cmp("CSV") == 0)
			{
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::ARRAY;
				result->vQueue = new GrapaRuleQueue();

				GrapaError err;
				GrapaGroup g;
				GrapaCHAR s;
				err = g.OpenFile(&r1.vVal->mValue, GrapaReadWrite);
				GrapaDBFieldArray* fieldList = g.ListFields(g.mRootTable, g.mRootType);
				if (fieldList)
				{
					GrapaRuleEvent* e = new GrapaRuleEvent();
					e->mValue.mToken = GrapaTokenType::LIST;
					e->vQueue = new GrapaRuleQueue();
					result->vQueue->PushTail(e);
					for (u32 i = 0; i < fieldList->Count(); i++)
					{
						GrapaDBField *field = fieldList->GetFieldAt(i);
						if (!field) continue;
						err = g.GetData(field->mNameRef, s);
						GrapaRuleEvent* r = new GrapaRuleEvent(0, s, GrapaInt(i).getBytes());
						e->vQueue->PushTail(r);
					}
					GrapaDBCursor cursor;
					cursor.Set(g.mRootTable);
					if (g.First(cursor) == 0)
					{
						do
						{
							GrapaRuleEvent* e = new GrapaRuleEvent();
							e->mValue.mToken = GrapaTokenType::ARRAY;
							e->vQueue = new GrapaRuleQueue();
							result->vQueue->PushTail(e);
							for (u32 i = 0; i < fieldList->Count(); i++)
							{
								GrapaDBField *field = fieldList->GetFieldAt(i);
								if (!field) continue;
								err = g.GetRecordField(cursor, *field, s);
								GrapaRuleEvent* r = new GrapaRuleEvent(0, GrapaCHAR(""), s);
								e->vQueue->PushTail(r);
							}
						} while (!(err = g.Next(cursor)));
					}
					delete fieldList;
				}
			}
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleModEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a, b, c;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			c = a % b;
			result = new GrapaRuleEvent(0, item, c.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d2.FromBytes(r2.vVal->mValue);
			d = d % d2;
			result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			d = d % b;
			result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			d2.FromBytes(r2.vVal->mValue);
			d = d % d2;
			result = new GrapaRuleEvent(0, item, d.getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePowEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a, b, c;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			if (!b.dataSigned)
				result = new GrapaRuleEvent(0, item, (a.Pow(b)).getBytes());
			else
			{
				if (a.IsZero())
					result = Error(vScriptExec, pNameSpace, -1);
				else
					result = new GrapaRuleEvent(0, item, (GrapaFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 1) / GrapaFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a.Pow(-b))).getBytes());
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			d = d.Pow(b);
			if (d.mNaN)
				result = Error(vScriptExec, pNameSpace, -1);
			else
				result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			b.FromBytes(r1.vVal->mValue);
			d.FromBytes(r2.vVal->mValue);
			d = GrapaFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b).Pow2(d);
			if (d.mNaN)
				result = Error(vScriptExec, pNameSpace, -1);
			else
				result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat d1(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d1.FromBytes(r1.vVal->mValue);
			d.FromBytes(r2.vVal->mValue);
			d = d1.Pow2(d);
			if (d.mNaN)
				result = Error(vScriptExec, pNameSpace, -1);
			else
				result = new GrapaRuleEvent(0, item, d.getBytes());
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
			result->vVector = new GrapaVector(*r1.vVal->vVector);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = result->vVector->Pow(vScriptExec, pNameSpace, *r2.vVal->vVector, false);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = result->vVector->Pow(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Pow(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Pow(vScriptExec, pNameSpace, bb, false);
				break;
			}
			if (err)
			{
				result->CLEAR();
				delete result->vVector;
				result->vVector = NULL;
				result = Error(vScriptExec, pNameSpace, -1);
			}
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			GrapaVector aa;
			aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = aa.Pow(vScriptExec, pNameSpace, *r2.vVal->vVector, false);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = aa.Pow(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Pow(vScriptExec, pNameSpace, bb, false);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Pow(vScriptExec, pNameSpace, bb, false);
				break;
			}
			if (err)
				result = Error(vScriptExec, pNameSpace, -1);
			else
				result = aa.ToArray();
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSetFloatEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam percision(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam extra(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaCHAR item;
	s64 max = vScriptExec->vScriptState->mItemState.mFloatMax;
	s64 extraNum = vScriptExec->vScriptState->mItemState.mFloatExtra;
	vScriptExec->vScriptState->mItemState.mFloatFix = 0;
	if (percision.vVal)
	{
		GrapaInt b;
		b.FromBytes(percision.vVal->mValue);
		s64 lb = b.LongValue();
		if (lb >= 0)
			max = lb;
	}
	if (extra.vVal)
	{
		GrapaInt b;
		b.FromBytes(extra.vVal->mValue);
		s64 lb = b.LongValue();
		if (lb >= 0)
			extraNum = lb;
	}
	vScriptExec->vScriptState->mItemState.mFloatMax = max;
	vScriptExec->vScriptState->mItemState.mFloatExtra = extraNum;
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSetFixEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam percision(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam extra(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaCHAR item;
	s64 max = vScriptExec->vScriptState->mItemState.mFloatMax;
	s64 extraNum = vScriptExec->vScriptState->mItemState.mFloatExtra;
	vScriptExec->vScriptState->mItemState.mFloatFix = 1;
	if (percision.vVal)
	{
		GrapaInt b;
		b.FromBytes(percision.vVal->mValue);
		s64 lb = b.LongValue();
		if (lb >= 0)
			max = lb;
	}
	if (extra.vVal)
	{
		GrapaInt b;
		b.FromBytes(extra.vVal->mValue);
		s64 lb = b.LongValue();
		if (lb >= 0)
			extraNum = lb;
	}
	vScriptExec->vScriptState->mItemState.mFloatMax = max;
	vScriptExec->vScriptState->mItemState.mFloatExtra = extraNum;
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRootEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Root(b)).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Root(b)).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			d2.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Root2(d2)).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			b.FromBytes(r1.vVal->mValue);
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
			d2.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Root2(d2)).getBytes());
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
			result->vVector = new GrapaVector(*r1.vVal->vVector);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = result->vVector->Pow(vScriptExec, pNameSpace, *r2.vVal->vVector, true);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = result->vVector->Pow(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Pow(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = result->vVector->Pow(vScriptExec, pNameSpace, bb, true);
				break;
			}
			if (err)
			{
				result->CLEAR();
				delete result->vVector;
				result->vVector = NULL;
				result = Error(vScriptExec, pNameSpace, -1);
			}
		}
		else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && (r2.vVal->mValue.mToken == GrapaTokenType::VECTOR || r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE || r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			GrapaVector aa;
			aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
			GrapaError err = -1;
			GrapaVector bb;
			switch (r2.vVal->mValue.mToken)
			{
			case GrapaTokenType::VECTOR:
				err = aa.Pow(vScriptExec, pNameSpace, *r2.vVal->vVector, true);
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::TUPLE:
				bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
				err = aa.Pow(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::INT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Pow(vScriptExec, pNameSpace, bb, true);
				break;
			case GrapaTokenType::FLOAT:
				bb.FromValue(vScriptExec, r2.vVal->mValue);
				err = aa.Pow(vScriptExec, pNameSpace, bb, true);
				break;
			}
			if (err)
				result = Error(vScriptExec, pNameSpace, -1);
			else
				result = aa.ToArray();
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRulePiEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	//s64 piMax = vScriptExec->vScriptState->mItemState.mFloatMax;
	GrapaFloat d(false, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra,1);
	if (r1.vVal)
	{
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt a;
			a.FromBytes(r1.vVal->mValue);
			d = a;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d.FromBytes(r1.vVal->mValue);
			//piMax = d.mMax;
		}
	}
	result = new GrapaRuleEvent(0, GrapaCHAR(), (GrapaFloat::Pi(d)).getBytes());
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSinEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.Sin()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Sin()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCosEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.Cos()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Cos()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTanEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.Tan()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Tan()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCotEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.Cot()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Cot()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSecEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.Sec()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Sec()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCscEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.Csc()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.Csc()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleASinEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ASin()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ASin()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleACosEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ACos()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ACos()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleATanEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ATan()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ATan()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleACotEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ACot()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ACot()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleASecEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ASec()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ASec()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleACscEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ACsc()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ACsc()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSinHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.SinH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.SinH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCosHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.CosH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.CosH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTanHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.TanH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.TanH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCotHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.CotH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.CotH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSecHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.SecH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.SecH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCscHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.CscH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.CscH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleASinHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ASinH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ASinH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleACosHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ACosH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ACosH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleATanHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ATanH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ATanH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleACotHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ACotH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ACotH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleASecHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			result = new GrapaRuleEvent(0, item, (d.ASecH()).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ASecH()).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleACscHEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaCHAR item;
	GrapaInt a;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal == NULL) return NULL;
	if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		a.FromBytes(r1.vVal->mValue);
		GrapaFloat d(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
		result = new GrapaRuleEvent(0, item, (d.ACscH()).getBytes());
	}
	else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
	{
		GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		d.FromBytes(r1.vVal->mValue);
		result = new GrapaRuleEvent(0, item, (d.ACscH()).getBytes());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleATan2Event::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		bool param1ok = false;
		bool param2ok = false;
		GrapaFloat d1(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			d1 = GrapaFloat(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			param1ok = true;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d1.FromBytes(r1.vVal->mValue);
			param1ok = true;
		}
		if (r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r2.vVal->mValue);
			d2 = GrapaFloat(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			param2ok = true;
		}
		else if (r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d2.FromBytes(r2.vVal->mValue);
			param2ok = true;
		}
		if (param1ok && param2ok)
			result = new GrapaRuleEvent(0, item, (GrapaFloat::ATan2(d1, d2)).getBytes());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleHypotEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		bool param1ok = false;
		bool param2ok = false;
		GrapaFloat d1(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			d1 = GrapaFloat(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			param1ok = true;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d1.FromBytes(r1.vVal->mValue);
			param1ok = true;
		}
		if (r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r2.vVal->mValue);
			d2 = GrapaFloat(true, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			param2ok = true;
		}
		else if (r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d2.FromBytes(r2.vVal->mValue);
			param2ok = true;
		}
		if (param1ok && param2ok)
			result = new GrapaRuleEvent(0, item, (GrapaFloat::Hypot(d1, d2)).getBytes());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLnEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		bool param1ok = false;
		GrapaFloat d1(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			d1 = GrapaFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			param1ok = true;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d1.FromBytes(r1.vVal->mValue);
			param1ok = true;
		}
		if (param1ok)
			result = new GrapaRuleEvent(0, item, (d1.Ln()).getBytes());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLogEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		bool param1ok = false;
		bool param2ok = true;
		GrapaFloat d1(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
		GrapaFloat d2(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 2);
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r1.vVal->mValue);
			d1 = GrapaFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			param1ok = true;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d1.FromBytes(r1.vVal->mValue);
			param1ok = true;
		}
		if (r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			a.FromBytes(r2.vVal->mValue);
			d2 = GrapaFloat(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
			param2ok = true;
		}
		else if (r2.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d2.FromBytes(r2.vVal->mValue);
			param2ok = true;
		}
		if (param1ok && param2ok)
			result = new GrapaRuleEvent(0, item, (d1.Log(d2)).getBytes());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleEEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	//s64 piMax = vScriptExec->vScriptState->mItemState.mFloatMax;
	GrapaFloat d(false, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 1);
	if (r1.vVal)
	{
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt a;
			a.FromBytes(r1.vVal->mValue);
			d = a;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			d.FromBytes(r1.vVal->mValue);
			//piMax = d.mMax;
		}
	}
	result = new GrapaRuleEvent(0, GrapaCHAR(), (GrapaFloat::E(d)).getBytes());
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleModPowEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	if (r1.vVal && r2.vVal && r3.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && r2.vVal->mValue.mToken == GrapaTokenType::INT && r3.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt a, b, c;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			c.FromBytes(r3.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (a.modPow(b, c)).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleModInvEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt a, b;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (a.modInverse(b)).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleAbsEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW)
		{
			GrapaInt a;
			a.FromBytes(r1.vVal->mValue);
			if (a.IsSignNeg()) a = a * GrapaInt(-1);
			result = new GrapaRuleEvent(0, GrapaCHAR(), a.getBytes());
			result->mValue.mToken = r1.vVal->mValue.mToken;
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT)
		{
			GrapaFloat d(false, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			if (d.mSigned) d = d * GrapaInt(-1);
			result = new GrapaRuleEvent(0, GrapaCHAR(), d.getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGcdEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
		{
			GrapaInt a, b;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), a.gcd(b).getBytes());
			result->mValue.mToken = r1.vVal->mValue.mToken;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBslEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
		{
			GrapaInt a, b;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (a << b.GetItem(0)).getBytes());
			result->mValue.mToken = r1.vVal->mValue.mToken;
		}
		if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt b;
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (d << b.GetItem(0)).getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBsrEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
		{
			GrapaInt a, b;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (a >> b.GetItem(0)).getBytes());
			result->mValue.mToken = r1.vVal->mValue.mToken;
		}
		if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt b;
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			d = d >> b.GetItem(0);
			result = new GrapaRuleEvent(0, GrapaCHAR(), d.getBytes());
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBorEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
		{
			GrapaInt a, b;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (a | b).getBytes());
			result->mValue.mToken = r1.vVal->mValue.mToken;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBandEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
		{
			GrapaInt a, b;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (a & b).getBytes());
			result->mValue.mToken = r1.vVal->mValue.mToken;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleOrEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	bool isTrueA = false;
	bool isNegA = false;
	bool isNull = false;
	if (r1.vVal) isTrueA = !r1.vVal->IsNullIsNegIsZero(isNegA, isNull);
	bool isTrueB = false;
	bool isNegB = false;
	if (r2.vVal) isTrueB = !r2.vVal->IsNullIsNegIsZero(isNegB, isNull);
	result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(isTrueA || isTrueB));
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleAndEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	bool isTrueA = false;
	bool isNegA = false;
	bool isNull = false;
	if (r1.vVal) isTrueA = !r1.vVal->IsNullIsNegIsZero(isNegA, isNull);
	bool isTrueB = false;
	bool isNegB = false;
	if (r2.vVal) isTrueB = !r2.vVal->IsNullIsNegIsZero(isNegB, isNull);
	result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(isTrueA && isTrueB));
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleXOrEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
		{
			GrapaInt a, b;
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), (a ^ b).getBytes());
			result->mValue.mToken = r1.vVal->mValue.mToken;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleInvEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::INT))
	{
		GrapaInt a;
		a.FromBytes(r1.vVal->mValue);
		result = new GrapaRuleEvent(0, GrapaCHAR(), (~a).getBytes());
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR))
	{
		result = vScriptExec->CopyItem(r1.vVal);
		result->vVector->Inv(vScriptExec, pNameSpace);
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector q;
		q.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		q.Inv(vScriptExec, pNameSpace);
		result = q.ToArray();
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTransposeEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR))
	{
		result = vScriptExec->CopyItem(r1.vVal);
		result->vVector->Transpose();
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector q;
		q.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		q.Transpose();
		result = q.ToArray();
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRrefEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR))
	{
		result = vScriptExec->CopyItem(r1.vVal);
		result->vVector->Rref(vScriptExec, pNameSpace);
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector q;
		q.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		q.Rref(vScriptExec, pNameSpace);
		result = q.ToArray();
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleDetEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR))
	{
		result = new GrapaRuleEvent(0, GrapaCHAR(""), r1.vVal->vVector->Determinant(vScriptExec, pNameSpace).getBytes());
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector q;
		q.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		result = new GrapaRuleEvent(0, GrapaCHAR(""), q.Determinant(vScriptExec, pNameSpace).getBytes());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRankEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR))
	{
		result = new GrapaRuleEvent(0, GrapaCHAR(""), r1.vVal->vVector->Rank(vScriptExec, pNameSpace).getBytes());
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector q;
		q.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		result = new GrapaRuleEvent(0, GrapaCHAR(""), q.Rank(vScriptExec, pNameSpace).getBytes());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSolveEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR))
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		r1.vVal->vVector->Solve(vScriptExec, pNameSpace, *result->vVector);
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector q;
		q.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector r;
		q.Solve(vScriptExec, pNameSpace, r);
		result = r.ToArray();
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCovEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	s64 axis = 0;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		axis = a.LongValue();
	}
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR))
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		if (r1.vVal->vVector->Cov(vScriptExec, pNameSpace, *result->vVector, axis==0))
		{
			result->CLEAR();
			delete result;
			result = NULL;
		}
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector q;
		q.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector r;
		if (!q.Cov(vScriptExec, pNameSpace, r, axis == 0))
			result = r.ToArray();
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleNegEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			a.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (-a).getBytes());
			break;
		case GrapaTokenType::FLOAT:
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			d = -d;
			result = new GrapaRuleEvent(0, item, d.getBytes());

		}
		break;
		case GrapaTokenType::BOOL:
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", (r1.vVal->mValue.mBytes&&r1.vVal->mValue.mLength&&r1.vVal->mValue.mBytes[0] && r1.vVal->mValue.mBytes[0] != '0') ? "" : "\1");
			break;
		default: break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBitsEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::FLOAT:
			a.FromBytes(r1.vVal->mValue);
			a = a.bitCount();
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		case GrapaTokenType::STR:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::RAW:
			a = r1.vVal->mValue.mLength * 8;
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		default: break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBytesEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			a.FromBytes(r1.vVal->mValue);
			a = a.bitCount() / 8;
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		case GrapaTokenType::FLOAT:
		case GrapaTokenType::STR:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::RAW:
		case GrapaTokenType::BOOL:
			a = r1.vVal->mValue.mLength;
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		default: break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLenEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item, temp;
		GrapaInt a;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::TIME:
			a.FromBytes(r1.vVal->mValue);
			temp = a.ToString(10);
			a = temp.mLength;
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		case GrapaTokenType::FLOAT:
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			temp = d.ToString(10);
			a = temp.mLength;
			result = new GrapaRuleEvent(0, item, a.getBytes());
		}
		break;
		case GrapaTokenType::STR:
		case GrapaTokenType::SYSSTR:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::RAW:
		case GrapaTokenType::BOOL:
		case GrapaTokenType::ID:
		case GrapaTokenType::SYSID:
		case GrapaTokenType::SYM:
			a = r1.vVal->mValue.mLength;
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		default:
			if (r1.vVal->mValue.mToken == GrapaTokenType::TAG)
			{
				if (r1.vVal->vQueue->Head()->vQueue->mCount == 2)
					a = r1.vVal->vQueue->Head()->vQueue->Head(1)->vQueue->mCount;
				else
					a = 0;
			}
			else
				a = r1.vVal->vQueue ? r1.vVal->vQueue->mCount : 0;
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBoolEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", (r1.vVal->vQueue && r1.vVal->vQueue->mCount) ? "\1" : "");
			break;
		default:
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", (r1.vVal->mValue.mBytes && r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] && r1.vVal->mValue.mBytes[0] != '0') ? "\1" : "");
			break;
		}
		if (result->mValue.mLength == 0 || result->mValue.mBytes[0] == 0 || result->mValue.mBytes[0] == '0')
		{
			result->SetNull();
			result->mValue.mToken = GrapaTokenType::BOOL;
		}
	}
	else
	{
		result = new GrapaRuleEvent();
		result->SetNull();
		result->mValue.mToken = GrapaTokenType::BOOL;
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleNotEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", (r1.vVal->vQueue && r1.vVal->vQueue->mCount) ? "" : "\1");
			break;
		default:
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", (r1.vVal->mValue.mBytes && r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] && r1.vVal->mValue.mBytes[0] != '0') ? "" : "\1");
			break;
		}
	}
	else
	{
		result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", "\1");
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleIntEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaInt isUnsigned = 0;
	if (r2.vVal && r2.vVal->mValue.mLength) isUnsigned.FromBytes(r2.vVal->mValue);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::BOOL:
			if (isUnsigned.GetItem(0) != 0)
			{
				a.FromBytes(r1.vVal->mValue, true);
				result = new GrapaRuleEvent(0, item, a.getBytes());
			}
			else
			{
				result = new GrapaRuleEvent(0, item, r1.vVal->mValue);
			}
			result->mValue.mToken = GrapaTokenType::INT;
			break;
		case GrapaTokenType::FLOAT:
		{
			GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			d.FromBytes(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, (d.ToInt()).getBytes());
		}
		break;
		case GrapaTokenType::STR:
			a.FromString(r1.vVal->mValue, 10);
			if (isUnsigned.GetItem(0) != 0)
				a.FromBytes(a.getBytes(true), true);
			result = new GrapaRuleEvent(0, item, a.getBytes());
			break;
		case GrapaTokenType::RAW:
		case GrapaTokenType::TIME:
			if (isUnsigned.GetItem(0) != 0)
			{
				a.FromBytes(r1.vVal->mValue, true);
				result = new GrapaRuleEvent(0, item, a.getBytes());
			}
			else
			{
				result = new GrapaRuleEvent(0, item, r1.vVal->mValue);
			}
			result->mValue.mToken = GrapaTokenType::INT;
			break;
		case GrapaTokenType::TABLE:
			break;
		case GrapaTokenType::SYSID:
			if (r1.vVal->mNull)
			{
				a = 0;
				result = new GrapaRuleEvent(0, item, a.getBytes());
			}
			else
			{
				result = Error(vScriptExec, pNameSpace, -1);
			}
			break;
		default:
			result = Error(vScriptExec ,pNameSpace, -1);
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleBaseEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaInt base = 10;
	if (r2.vVal && r2.vVal->mValue.mLength) base.FromBytes(r2.vVal->mValue);
	GrapaInt isUnsigned = 0;
	if (r3.vVal && r3.vVal->mValue.mLength) isUnsigned.FromBytes(r3.vVal->mValue);
	if (r1.vVal)
	{
		GrapaCHAR item, temp;
		GrapaInt a;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::TIME:
		case GrapaTokenType::BOOL:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::RAW:
		case GrapaTokenType::STR:
			a.FromBytes(r1.vVal->mValue, isUnsigned.GetItem(0)!=0);
			result = new GrapaRuleEvent(0, item, a.ToString(base.GetItem(0)));
			result->mValue.mToken = GrapaTokenType::STR;
			break;
		case GrapaTokenType::FLOAT:
			{
				GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				d.FromBytes(r1.vVal->mValue);
				temp = d.ToString(base.GetItem(0));
			}
			result = new GrapaRuleEvent(0, item, temp);
			result->mValue.mToken = GrapaTokenType::STR;
			break;
		default: break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRawEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaInt isUnsigned = 0;
	if (r2.vVal && r2.vVal->mValue.mLength) isUnsigned.FromBytes(r2.vVal->mValue);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::BOOL:
		case GrapaTokenType::RAW:
			if (isUnsigned.GetItem(0) != 0)
			{
				a.FromBytes(r1.vVal->mValue, true);
				result = new GrapaRuleEvent(0, item, a.getBytes(true));
			}
			else
			{
				result = new GrapaRuleEvent(0, item, r1.vVal->mValue);
			}
			result->mValue.mToken = GrapaTokenType::RAW;
			break;
		case GrapaTokenType::FLOAT:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::STR:
		case GrapaTokenType::TIME:
			result = new GrapaRuleEvent(0, item, r1.vVal->mValue);
			result->mValue.mToken = GrapaTokenType::RAW;
			break;
		default: break;
		}
	}
	return(result);
}

class GrapaStrSend : public GrapaSystemSend
{
public:
	bool isActive;
	GrapaStrSend() { isActive = true; }
	virtual void GetStr(GrapaCHAR& strBuf) { strBuf.FROM(mBuff); };
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen) { mBuff.Append((char*)sendbuf, sendbuflen); };
	GrapaCHAR mBuff;
};

GrapaRuleEvent* GrapaLibraryRuleStrEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaInt a;
		GrapaTime t;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::BOOL:
		case GrapaTokenType::RAW:
		case GrapaTokenType::STR:
		case GrapaTokenType::ID:
		case GrapaTokenType::SYSID:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::SYSSTR:
		case GrapaTokenType::TIME:
			result = new GrapaRuleEvent(0, item, r1.vVal->mValue.ToStr());
			result->mValue.mToken = GrapaTokenType::STR;
			break;
		case GrapaTokenType::FLOAT:
			{
				GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				d.FromBytes(r1.vVal->mValue);
				result = new GrapaRuleEvent(0, item, d.ToString(10));
			}
			result->mValue.mToken = GrapaTokenType::STR;
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::RULEOP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
		case GrapaTokenType::VECTOR:
			if (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && r2.vVal && !r2.vVal->IsNull())
			{
				GrapaBYTE value;
				r1.vVal->vVector->TO(vScriptExec, pNameSpace, r2.vVal, value);
				result = new GrapaRuleEvent(0, item, value);
				result->mValue.mToken = GrapaTokenType::STR;
			}
			else
			{
				GrapaStrSend send;
				GrapaRuleEvent* echo = r1.vVal;
				while (echo->mValue.mToken == GrapaTokenType::PTR && echo->vRulePointer)
					echo = echo->vRulePointer;
				if (echo->mValue.mToken == GrapaTokenType::RULEOP)
				{
					echo = echo->vQueue->Tail();
					while (echo->mValue.mToken == GrapaTokenType::PTR && echo->vRulePointer)
						echo = echo->vRulePointer;
				}
				u64 size = 0;
				echo->TOSize(size);
				send.mBuff.SetSize(size);
				send.mBuff.SetLength(0);
				if (echo->vQueue)
					vScriptExec->EchoList(&send, echo, false, false, false);
				else 
					vScriptExec->EchoValue(&send, echo, false, true, false);
				result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "");
				send.GetStr(result->mValue);
				result->mValue.mToken = GrapaTokenType::STR;
			}
			break;
		default: 
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleListEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::TAG:
		case GrapaTokenType::EL:
		case GrapaTokenType::XML:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::ERR:
			result = vScriptExec->CopyItem(r1.vVal, true);
			result->mValue.mToken = GrapaTokenType::LIST;
			break;
		case GrapaTokenType::STR:
			if (r1.vVal->mValue.mLength)
			{
				GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$function"));
				GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, r1.vVal->mValue, rulexx, 0, GrapaCHAR());
				result = vScriptExec->ProcessPlan(pNameSpace, plan);
				if (plan)
				{
					plan->CLEAR();
					delete plan;
				}
			}
			break;
		default:
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleArrayEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		if (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
		{
			if (r1.vVal->vVector)
				result = r1.vVal->vVector->ToArray();
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleVectorEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	u8 dim = 0;
	if (r2.vVal && r2.vVal->mValue.mToken==GrapaTokenType::INT)
	{
		GrapaInt v;
		v.FromBytes(r2.vVal->mValue);
		if (v.GetItem(0) < 64)
			dim = v.GetItem(0);
	}
	if (r1.vVal)
	{
		if (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
			result->mValue.mToken = GrapaTokenType::VECTOR;
			result->vVector = new GrapaVector();
			if (!result->vVector->FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, dim))
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::STR && r1.vVal->mValue.mLength)
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
			result->mValue.mToken = GrapaTokenType::VECTOR;
			result->vVector = new GrapaVector();
			if (!result->vVector->FROM(vScriptExec, pNameSpace, vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal->mValue, dim))
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
	}
	else
	{
		result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
		result->mValue.mToken = GrapaTokenType::VECTOR;
		result->vVector = new GrapaVector();
		if (!result->vVector->FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, dim))
		{
			result->CLEAR();
			delete result;
			result = NULL;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleXmlEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::RAW:
		case GrapaTokenType::STR:
			if (r1.vVal->mValue.mLength)
			{
				GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$function"));
				GrapaCHAR val;
				val.Append("$&");
				val.Append(r1.vVal->mValue);
				val.Append("$&");
				GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, val, rulexx, 0, GrapaCHAR());
				result = vScriptExec->ProcessPlan(pNameSpace, plan);
				if (plan)
				{
					plan->CLEAR();
					delete plan;
				}
			}
			break;
		default:
			break;
		}
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleFloatEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam number(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam percision(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam extra(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaCHAR item;
	s64 max = vScriptExec->vScriptState->mItemState.mFloatMax;
	s64 extraNum = vScriptExec->vScriptState->mItemState.mFloatExtra;
	bool fix = (mName.Cmp("fix") == 0);
	if (percision.vVal && percision.vVal->mValue.mToken==GrapaTokenType::INT)
	{
		GrapaInt b;
		b.FromBytes(percision.vVal->mValue);
		s64 lb = b.LongValue();
		if (lb > 0)
			max = lb;
	}
	if (extra.vVal && extra.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt b;
		b.FromBytes(extra.vVal->mValue);
		s64 lb = b.LongValue();
		if (lb > 0)
			extraNum = lb;
	}
	GrapaFloat d(fix, max, extraNum, 0);
	if (number.vVal)
	{
		GrapaInt a;
		switch (number.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			a.FromBytes(number.vVal->mValue);
			d = a;
			d.mFix = fix;
			d.mMax = percision.vVal?max: d.mMax;
			d.mExtra = extra.vVal?extraNum: d.mExtra;
			d.Truncate();
			result = new GrapaRuleEvent(0, item, d.getBytes());
			result->mValue.mToken = GrapaTokenType::FLOAT;
			break;
		case GrapaTokenType::FLOAT:
			d.FromBytes(number.vVal->mValue);
			d.mFix = fix;
			d.mMax = percision.vVal ? max : d.mMax;
			d.mExtra = extra.vVal ? extraNum : d.mExtra;
			d.Truncate();
			result = new GrapaRuleEvent(0, item, d.getBytes());
			result->mValue.mToken = GrapaTokenType::FLOAT;
			break;
		case GrapaTokenType::STR:
			d.mMax = max;
			d.FromString(number.vVal->mValue, 10);
			d.mMax = percision.vVal ? max : d.mMax;
			d.mExtra = extra.vVal ? extraNum : d.mExtra;
			d.Truncate();
			result = new GrapaRuleEvent(0, item, d.getBytes());
			result->mValue.mToken = GrapaTokenType::FLOAT;
			break;
		default:
			break;
		}
	}
	if (result==NULL)
	{
		result = new GrapaRuleEvent(0, item, d.getBytes());
		result->mValue.mToken = GrapaTokenType::FLOAT;
	}
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTimeEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		GrapaCHAR item;
		GrapaTime t;
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			result = new GrapaRuleEvent(0, item, r1.vVal->mValue);
			result->mValue.mToken = GrapaTokenType::TIME;
			break;
		case GrapaTokenType::STR:
			t.FromString(r1.vVal->mValue);
			result = new GrapaRuleEvent(0, item, t.getBytes());
			result->mValue.mToken = GrapaTokenType::TIME;
			break;
		default: break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTypeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		if (r1.vVal->mValue.mToken == GrapaTokenType::OBJ)
		{
			if (r1.vVal->vClass && r1.vVal->vClass->mName.mLength)
			{
				result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", (char*)r1.vVal->vClass->mName.mBytes);
			}
			else
			{
				result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$OBJ");
			}
		}
		else
		{
			GrapaRuleEvent* p = r1.vVal;
			//if (p->mValue.mToken == GrapaTokenType::TAG)
			//	p = p->vQueue->Head()->vQueue->Head(1);
			if (p == NULL)
			{
				result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
				result->SetNull();
			}
			else
			{
				switch (p->mValue.mToken)
				{
				case GrapaTokenType::ERR: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$ERR"); break;
				case GrapaTokenType::RAW: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$RAW"); break;
				case GrapaTokenType::BOOL: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$BOOL"); break;
				case GrapaTokenType::INT: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$INT"); break;
				case GrapaTokenType::FLOAT: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$FLOAT"); break;
				case GrapaTokenType::STR: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$STR"); break;
				case GrapaTokenType::TIME: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$TIME"); break;
				case GrapaTokenType::ARRAY: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$ARRAY"); break;
				case GrapaTokenType::TUPLE: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$TUPLE"); break;
				case GrapaTokenType::VECTOR: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$VECTOR"); break;
				case GrapaTokenType::WIDGET: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$WIDGET"); break;
				case GrapaTokenType::LIST: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$LIST"); break;
				case GrapaTokenType::XML: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$XML"); break;
				case GrapaTokenType::EL: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$EL"); break;
				case GrapaTokenType::TAG: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$TAG"); break;
				case GrapaTokenType::OP: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$OP"); break;
				case GrapaTokenType::CODE: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$CODE"); break;
				case GrapaTokenType::TABLE: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$TABLE"); break;
				case GrapaTokenType::CLASS: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$CLASS"); break;
				case GrapaTokenType::OBJ: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$OBJ"); break;
				case GrapaTokenType::RULE: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$RULE"); break;
				case GrapaTokenType::ID: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$ID"); break;
				case GrapaTokenType::SYSID: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$SYSID"); break;
				case GrapaTokenType::SYSINT: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$SYSINT"); break;
				case GrapaTokenType::SYSSTR: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$SYSSTR"); break;
				case GrapaTokenType::PTR: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$PTR"); break;
				case GrapaTokenType::TOKEN: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$TOKEN"); break;
				default: result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "$UNKNOWN"); break;
				}
			}
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}


GrapaRuleEvent* GrapaLibraryRuleDescribeEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLeftEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaVector v;
		GrapaInt a;
		s64 len = 0;
		switch (r2.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			a.FromBytes(r2.vVal->mValue);
			len = a.LongValue();
			break;
		default:
			break;
		}
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::RAW:
		case GrapaTokenType::STR:
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
			if (r1.vVal->mValue.mLength)
			{
				if (len < 0) len = r1.vVal->mValue.mLength + len;
				if (len < 0) len = 0;
				if (len > (s64)r1.vVal->mValue.mLength) len = r1.vVal->mValue.mLength;
				result->mValue.FROM((char*)r1.vVal->mValue.mBytes, len);
			}
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			break;
		case GrapaTokenType::ARRAY:
			if (v.FROM(vScriptExec, r1.vVal, 0))
			{
				GrapaVector v3;
				if (v.Left(vScriptExec, pNameSpace, len, v3)) break;
				result = v3.ToArray();
			}
			break;
		case GrapaTokenType::TUPLE:
			if (v.FROM(vScriptExec, r1.vVal, 0))
			{
				GrapaVector v3;
				if (v.Left(vScriptExec, pNameSpace, len, v3)) break;
				result = v3.ToTuple();
			}
			break;
		case GrapaTokenType::VECTOR:
			result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
			result->mValue.mToken = GrapaTokenType::VECTOR;
			result->vVector = new GrapaVector();
			if (r1.vVal->vVector->Left(vScriptExec, pNameSpace, len, *result->vVector))
			{
				delete result->vVector;
				result->vVector = NULL;
				result = Error(vScriptExec, pNameSpace, -1);
			}
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRightEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		GrapaVector v;
		GrapaInt a;
		s64 len = 0;
		switch (r2.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			a.FromBytes(r2.vVal->mValue);
			len = a.LongValue();
			break;
		default:
			break;
		}
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
			if (r1.vVal->mValue.mLength)
			{
				if (len < 0) len = r1.vVal->mValue.mLength + len;
				if (len < 0) len = 0;
				if (len > (s64)r1.vVal->mValue.mLength) len = r1.vVal->mValue.mLength;
				result->mValue.FROM((char*)&r1.vVal->mValue.mBytes[r1.vVal->mValue.mLength - len], len);
			}
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			break;
		case GrapaTokenType::ARRAY:
			if (v.FROM(vScriptExec, r1.vVal, 0))
			{
				GrapaVector v3;
				if (v.Right(vScriptExec, pNameSpace, len, v3)) break;
				result = v3.ToArray();
			}
			break;
		case GrapaTokenType::TUPLE:
			if (v.FROM(vScriptExec, r1.vVal, 0))
			{
				GrapaVector v3;
				if (v.Right(vScriptExec, pNameSpace, len, v3)) break;
				result = v3.ToTuple();
			}
			break;
		case GrapaTokenType::VECTOR:
			result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
			result->mValue.mToken = GrapaTokenType::VECTOR;
			result->vVector = new GrapaVector();
			if (r1.vVal->vVector->Right(vScriptExec, pNameSpace, len, *result->vVector))
			{
				delete result->vVector;
				result->vVector = NULL;
				result = Error(vScriptExec, pNameSpace, -1);
			}
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleMidEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	if (r1.vVal && r2.vVal && r3.vVal)
	{
		GrapaInt a;
		s64 pos = 0, len = 0;
		switch (r2.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			a.FromBytes(r2.vVal->mValue);
			pos = a.LongValue();
			break;
		default:
			break;
		}
		switch (r3.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
			a.FromBytes(r3.vVal->mValue);
			len = a.LongValue();
			break;
		default:
			break;
		}
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
			if (r1.vVal->mValue.mLength)
			{
				if (pos < 0) pos = r1.vVal->mValue.mLength + pos;
				if (pos < 0) pos = 0;
				if (pos > (s64)r1.vVal->mValue.mLength) pos = r1.vVal->mValue.mLength;
				if ((pos + len) > (s64)r1.vVal->mValue.mLength)
				{
					len = r1.vVal->mValue.mLength - pos;
				}
				else
				{
					if (len < 0) len = (r1.vVal->mValue.mLength-pos) + len;
					if (len < 0) len = 0;
				}
				result->mValue.FROM((char*)&r1.vVal->mValue.mBytes[pos], len);
			}
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::VECTOR:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleMidTrimEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam itemsV(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam offsetV(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam blocksizeV(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	if (r1.vVal && itemsV.vVal && itemsV.vVal && (itemsV.vVal->mValue.mToken == GrapaTokenType::ARRAY || itemsV.vVal->mValue.mToken == GrapaTokenType::TUPLE)
		&& (r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::RAW))
	{
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::LIST;
		result->vQueue = new GrapaRuleQueue();
		GrapaInt a;
		a.FromBytes(offsetV.vVal->mValue);
		s64 offset = a.LongValue();
		if (offset < 0) offset = r1.vVal->mValue.mLength + offset;
		if (offset < 0) offset = 0;
		if (offset > r1.vVal->mValue.mLength) offset = r1.vVal->mValue.mLength;
		a.FromBytes(blocksizeV.vVal->mValue);
		s64 blocksize = a.LongValue();
		if ((blocksize<=0)|| ((offset + blocksize) > r1.vVal->mValue.mLength)) blocksize = r1.vVal->mValue.mLength-offset;
		if ((r1.vVal->mValue.mLength)&& ((offset + blocksize) <= r1.vVal->mValue.mLength))
		{
			GrapaRuleEvent* row = itemsV.vVal->vQueue->Head();
			s64 lastPos = 0;
			while (row)
			{
				GrapaRuleEvent* item = row;
				while (item->mValue.mToken == GrapaTokenType::PTR && item->vRulePointer) item = item->vRulePointer;
				if ((item->mValue.mToken == GrapaTokenType::ARRAY || item->mValue.mToken == GrapaTokenType::TUPLE) && item->vQueue->mCount >= 3)
				{
					a.FromBytes(item->vQueue->Head(1)->mValue);
					s64 pos = a.LongValue();
					a.FromBytes(item->vQueue->Head(2)->mValue);
					s64 len = a.LongValue();
					GrapaCHAR lb(item->vQueue->Head(0)->mValue);
					GrapaRuleEvent* lt = NULL, * rt = NULL;
					if (item->vQueue->mCount >= 4) lt = item->vQueue->Head(3);
					if (item->vQueue->mCount >= 5) rt = item->vQueue->Head(4);
					GrapaRuleEvent* r = new GrapaRuleEvent(0, lb, GrapaCHAR());
					r->mValue.mToken = r1.vVal->mValue.mToken;
					if (pos == 0) pos = lastPos;
					if (pos < 0) pos = blocksize + pos;
					if (pos < 0) pos = 0;
					if (pos > (s64)blocksize) pos = blocksize;
					if ((pos + len) > (s64)blocksize)
					{
						len = blocksize - pos;
					}
					else
					{
						if (len < 0) len = (blocksize - pos) + len;
						if (len < 0) len = 0;
					}
					pos += offset;
					r->mValue.FROM((char*)&r1.vVal->mValue.mBytes[pos], len);
					lastPos = pos + len;
					if (lt) r->mValue.LTrim2(lt);
					if (rt) r->mValue.RTrim2(rt);
					if (item->vQueue->mCount >= 6)
					{
						GrapaRuleEvent* e = item->vQueue->Head(5);
						if (e && e->mValue.mToken == GrapaTokenType::OP)
						{
							GrapaRuleEvent x;
							x.mValue.mToken = GrapaTokenType::PTR;
							x.vRulePointer = r;
							GrapaRuleEvent* temp = vScriptExec->ProcessPlan(pNameSpace, e, &x);
							r->CLEAR();
							r->SetNull();
							if (temp)
							{
								delete r;
								r = temp;
							}
						}
					}
					r->mName.FROM(lb);
					result->vQueue->PushTail(r);
				}
				row = row->Next();
			}
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRTrimEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
			if (r1.vVal->mValue.mLength)
			{
				result->mValue.FROM(r1.vVal->mValue);
				if (r2.vVal && r2.vVal->IsNull())
					result->mValue.RTrim(' ');
				else
					result->mValue.RTrim2(r2.vVal);
			}
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::VECTOR:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLTrimEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
			if (r1.vVal->mValue.mLength)
			{
				result->mValue.FROM(r1.vVal->mValue);
				if (r2.vVal && r2.vVal->IsNull())
					result->mValue.LTrim(' ');
				else
					result->mValue.LTrim2(r2.vVal);
			}
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::VECTOR:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTrimEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
			if (r1.vVal->mValue.mLength)
			{
				result->mValue.FROM(r1.vVal->mValue);
				if (r2.vVal && r2.vVal->IsNull())
					result->mValue.Trim(' ');
				else
					result->mValue.Trim2(r2.vVal);
			}
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			break;
		case GrapaTokenType::VECTOR:
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRRotateEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* p = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	u64 n = 1;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		n = a.GetItem(0);
	};
	if (n == 0) n = 1;
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::LIST:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::XML:
			if (r1.vVal->vQueue == NULL)
				break;
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "");
			result->vQueue = vScriptExec->CopyQueue(r1.vVal->vQueue);
			while (result->vQueue->mCount && n--)
				result->vQueue->PushHead(result->vQueue->PopTail());
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLRotateEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* p = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	u64 n = 1;
	if (r2.vVal && r2.vVal->mValue.mToken==GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		n = a.GetItem(0);
	};
	if (n == 0) n = 1;
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::LIST:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::XML:
			if (r1.vVal->vQueue == NULL)
				break;
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "");
			result->vQueue = vScriptExec->CopyQueue(r1.vVal->vQueue);
			while (result->vQueue->mCount && n--)
				result->vQueue->PushTail(result->vQueue->PopHead());
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleRPadEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* p = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaCHAR pad(" ");
	if (r3.vVal)
	{
		switch (r3.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			pad.FROM(r3.vVal->mValue);
			break;
		default:
			break;
		}
	}
	if (pad.mLength == 0) pad.FROM(" ");
	u64 n = 1;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		n = a.GetItem(0);
	};
	if (n == 0) n = 1;
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			if (n < r1.vVal->mValue.mLength) {
				result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
				result->mValue.FROM((char*)r1.vVal->mValue.mBytes,n);
			}
			else {
				result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
				GrapaCHAR add(pad);
				while ((r1.vVal->mValue.mLength + add.mLength) < n) add.Append(pad);
				add.SetLength(n- r1.vVal->mValue.mLength);
				result->mValue.FROM(r1.vVal->mValue);
				result->mValue.GrapaCHAR::Append(add);
			}
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLPadEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaRuleEvent* p = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaCHAR pad(" ");
	if (r3.vVal)
	{
		switch (r3.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			pad.FROM(r3.vVal->mValue);
			break;
		default:
			break;
		}
	}
	if (pad.mLength == 0) pad.FROM(" ");
	u64 n = 1;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		n = a.GetItem(0);
	};
	if (n == 0) n = 1;
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			if (n < r1.vVal->mValue.mLength) {
				result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
				result->mValue.FROM((char*)&r1.vVal->mValue.mBytes[r1.vVal->mValue.mLength -n], n);
			}
			else {
				result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
				GrapaCHAR add(pad);
				while ((r1.vVal->mValue.mLength + add.mLength) < n) add.Append(pad);
				add.SetLength(n - r1.vVal->mValue.mLength);
				result->mValue.FROM(add);
				result->mValue.GrapaCHAR::Append(r1.vVal->mValue);
			}
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleReverseEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaRuleEvent* p = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::STR:
		case GrapaTokenType::RAW:
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "", "");
			result->mValue.SetLength(r1.vVal->mValue.mLength);
			for (u64 i = 0; i < r1.vVal->mValue.mLength; i++)
			{
				result->mValue.mBytes[i] = r1.vVal->mValue.mBytes[r1.vVal->mValue.mLength - i - 1];
			}
			break;
		case GrapaTokenType::LIST:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::XML:
			if (r1.vVal->vQueue == NULL)
				break;
			result = new GrapaRuleEvent(r1.vVal->mValue.mToken, 0, "");
			result->vQueue = new GrapaRuleQueue();
			p = r1.vVal->vQueue->Head();
			while (p)
			{
				result->vQueue->PushHead(vScriptExec->CopyItem(p));
				p = p->Next();
			}
			break;
		case GrapaTokenType::VECTOR:
			result = vScriptExec->CopyItem(r1.vVal);
			result->vVector->Reverse(vScriptExec, pNameSpace);
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleReplaceEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	if (r1.vVal && r2.vVal && r3.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
			result = new GrapaRuleEvent(0, r1.vVal->mName, r1.vVal->mValue);
			if (r2.vVal->mValue.mToken == r1.vVal->mValue.mToken && r3.vVal->mValue.mToken == r1.vVal->mValue.mToken)
				result->mValue.Replace(r2.vVal->mValue, r3.vVal->mValue);
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}


#include "grep/grapa_grep_unicode.hpp"
#include "grep/grapa_grep_unicode.cpp"

//#include "grep/grapa_grep_unicode.cpp"
//#include "grep/test_grapa_grep.cpp"

/*
#include <string>
#include <string_view>
#include <vector>
#include <regex>
#include <iostream>
#include <map>

struct MatchPosition {
	size_t offset;
	size_t length;
	size_t line_number = 0; // 1-based line number, 0 if unused
};

std::string normalize_newlines(std::string_view input) {
	std::string result;
	result.reserve(input.size());
	for (size_t i = 0; i < input.size(); ++i) {
		if (input[i] == '\r') {
			if (i + 1 < input.size() && input[i + 1] == '\n') ++i;
			result.push_back('\n');
		}
		else {
			result.push_back(input[i]);
		}
	}
	return result;
}

std::vector<MatchPosition> grep(
	const std::string& input,
	const std::string& pattern,
	const std::string& options,
	const std::string& line_delim = ""
) {
	bool ignore_case = options.contains('i');
	bool invert_match = options.contains('v');
	bool all_mode = options.contains('a');
	bool match_only = (options.contains('o') || all_mode) && !invert_match;
	bool line_numbers = options.contains('n');

	std::regex::flag_type flags = std::regex::ECMAScript;
	if (ignore_case) flags |= std::regex::icase;

	std::regex rx;
	try {
		rx = std::regex(pattern, flags);
	}
	catch (const std::regex_error&) {
		return {};
	}

	std::vector<MatchPosition> results;

	if (all_mode) {
		if (invert_match) {
			std::vector<MatchPosition> matches;
			for (std::sregex_iterator it(input.begin(), input.end(), rx), end; it != end; ++it) {
				matches.push_back({
					static_cast<size_t>(it->position()),
					static_cast<size_t>(it->length()),
					1
					});
			}

			size_t prev_end = 0;
			for (const auto& m : matches) {
				if (m.offset > prev_end) {
					results.push_back({ prev_end, m.offset - prev_end, 1 });
				}
				prev_end = m.offset + m.length;
			}

			if (prev_end < input.size()) {
				results.push_back({ prev_end, input.size() - prev_end, 1 });
			}
		}
		else {
			if (match_only) {
				for (std::sregex_iterator it(input.begin(), input.end(), rx), end; it != end; ++it) {
					results.push_back({
						static_cast<size_t>(it->position()),
						static_cast<size_t>(it->length()),
						1
						});
				}
			}
			else {
				if (std::regex_search(input, rx)) {
					results.push_back({ 0, input.size(), 1 });
				}
			}
		}
	}
	else {
		std::string working_input =
			(line_delim.empty() && !all_mode) ? normalize_newlines(input) : input;

		size_t offset = 0;
		size_t line_number = 1;

		while (offset < working_input.size()) {
			size_t next = line_delim.empty()
				? working_input.find('\n', offset)
				: working_input.find(line_delim, offset);

			if (next == std::string::npos) next = working_input.size();
			else next += line_delim.empty() ? 1 : line_delim.size();

			std::string_view line(working_input.data() + offset, next - offset);
			std::string line_copy(line);

			bool matched = std::regex_search(line_copy, rx);

			if ((!invert_match && matched) || (invert_match && !matched)) {
				if (!match_only || invert_match) {
					size_t line_len = line.size();
					if (!line_delim.empty() && line.ends_with(line_delim)) {
						line_len -= line_delim.size(); // strip delimiter in line mode
					}
					results.push_back({ offset, line_len, line_number });
				}
				else {
					for (std::sregex_iterator it(line_copy.begin(), line_copy.end(), rx), end; it != end; ++it) {
						results.push_back({
							offset + static_cast<size_t>(it->position()),
							static_cast<size_t>(it->length()),
							line_number
							});
					}
				}
			}

			offset = next;
			++line_number;
		}
	}

	return results;
}

std::vector<std::string> grep_extract_matches(
	const std::string& input,
	const std::string& pattern,
	const std::string& options,
	const std::string& line_delim = ""
) {
	auto matches = grep(input, pattern, options, line_delim);
	std::vector<std::string> out;

	if (options.contains('c')) {
		out.push_back(std::to_string(matches.size()));
		return out;
	}

	bool include_line_numbers = options.contains('n');
	bool group_by_line = include_line_numbers && !options.contains('o');
	bool strip_trailing_newline = line_delim.empty() && !options.contains('a');

	if (group_by_line) {
		std::map<size_t, std::vector<std::string>> grouped;
		for (const auto& m : matches) {
			std::string match = input.substr(m.offset, m.length);
			if (!match.empty() && strip_trailing_newline && match.back() == '\n') {
				match.pop_back();
			}
			grouped[m.line_number].push_back(std::move(match));
		}

		for (const auto& [line, group] : grouped) {
			std::string joined;
			for (size_t i = 0; i < group.size(); ++i) {
				if (i > 0) joined += ",";
				joined += group[i];
			}
			out.emplace_back(std::to_string(line) + ":" + joined);
		}

	}
	else {
		out.reserve(matches.size());
		for (const auto& m : matches) {
			std::string match = input.substr(m.offset, m.length);
			if (!match.empty() && strip_trailing_newline && match.back() == '\n') {
				match.pop_back();
			}

			if (include_line_numbers && m.line_number > 0) {
				out.emplace_back(std::to_string(m.line_number) + ":" + match);
			}
			else {
				out.emplace_back(std::move(match));
			}
		}
	}

	return out;
}
*/

GrapaRuleEvent* GrapaLibraryRuleGrepEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	GrapaLibraryParam r5(vScriptExec, pNameSpace, pInput ? pInput->Head(4) : NULL);
	GrapaLibraryParam r6(vScriptExec, pNameSpace, pInput ? pInput->Head(5) : NULL);
	GrapaLibraryParam r7(vScriptExec, pNameSpace, pInput ? pInput->Head(6) : NULL);

	std::string input="", pattern = "", options = "", delim = "", normstr_upper = "", procstr_upper = "";

	//input = "This test finds four word lines.\nAlso five more text.\nDone.";
	//pattern = "\\b\\w{4}\\b";
	//options = "o";

	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::RAW)) input = std::string(reinterpret_cast<const char*>(r1.vVal->mValue.mBytes), r1.vVal->mValue.mLength);
	if (r2.vVal && (r2.vVal->mValue.mToken == GrapaTokenType::STR || r2.vVal->mValue.mToken == GrapaTokenType::RAW)) pattern = std::string(reinterpret_cast<const char*>(r2.vVal->mValue.mBytes), r2.vVal->mValue.mLength);
	if (r3.vVal && (r3.vVal->mValue.mToken == GrapaTokenType::STR || r3.vVal->mValue.mToken == GrapaTokenType::RAW)) options = std::string(reinterpret_cast<const char*>(r3.vVal->mValue.mBytes), r3.vVal->mValue.mLength);
	if (r4.vVal && (r4.vVal->mValue.mToken == GrapaTokenType::STR || r4.vVal->mValue.mToken == GrapaTokenType::RAW)) delim = std::string(reinterpret_cast<const char*>(r4.vVal->mValue.mBytes), r4.vVal->mValue.mLength);
	if (r5.vVal && (r5.vVal->mValue.mToken == GrapaTokenType::STR || r5.vVal->mValue.mToken == GrapaTokenType::RAW)) normstr_upper = std::string(reinterpret_cast<const char*>(r5.vVal->mValue.mBytes), r5.vVal->mValue.mLength);
	if (r6.vVal && (r6.vVal->mValue.mToken == GrapaTokenType::STR || r6.vVal->mValue.mToken == GrapaTokenType::RAW)) procstr_upper = std::string(reinterpret_cast<const char*>(r6.vVal->mValue.mBytes), r6.vVal->mValue.mLength);

	u64 num_workers = 0;
	if (r7.vVal)
	{
		if (r7.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt a;
			a.FromBytes(r7.vVal->mValue);
			num_workers = a.GetItem(0);
		}
		else if (r7.vVal->mValue.mToken == GrapaTokenType::STR || r7.vVal->mValue.mToken == GrapaTokenType::RAW)
		{
			GrapaInt a;
			a.FromBytes(r7.vVal->mValue);
			num_workers = a.LongValue();
		}
	}

	try {
		
		
		// Remove this problematic line that's causing the error:
		// std::regex rx(pattern, std::regex::ECMAScript);
		
		GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE;
		std::transform(normstr_upper.begin(), normstr_upper.end(), normstr_upper.begin(), ::toupper);

		if (normstr_upper.empty() || normstr_upper == "NONE") {
			normalization = GrapaUnicode::NormalizationForm::NONE;
		}
		else if (normstr_upper == "NFC") {
			normalization = GrapaUnicode::NormalizationForm::NFC;
		}
		else if (normstr_upper == "NFD") {
			normalization = GrapaUnicode::NormalizationForm::NFD;
		}
		else if (normstr_upper == "NFKC") {
			normalization = GrapaUnicode::NormalizationForm::NFKC;
		}
		else if (normstr_upper == "NFKD") {
			normalization = GrapaUnicode::NormalizationForm::NFKD;
		}
		GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE;
		std::transform(procstr_upper.begin(), procstr_upper.end(), procstr_upper.begin(), ::toupper);

		if (procstr_upper.empty() || procstr_upper == "UNICODE") {
			mode = GrapaUnicode::ProcessingMode::UNICODE_MODE;
		}
		else if (procstr_upper == "BINARY") {
			mode = GrapaUnicode::ProcessingMode::BINARY_MODE;
		}
		auto matches = grep_extract_matches_unicode(input, pattern, options, delim, normalization, mode, num_workers);

		// Check for compilation error indicator
		if (!matches.empty() && matches[0] == "__COMPILATION_ERROR__") {
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::ERR;
			result->vQueue = new GrapaRuleQueue();
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaCHAR("Invalid regex pattern")));
		}
		else if (options.find('j') != std::string::npos) {
			if (matches.empty()) {
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::ERR;
				result->vQueue = new GrapaRuleQueue();
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaCHAR("empty grep result")));
			}
			else
			{
				GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$function"));
				GrapaCHAR jstr(matches[0].c_str(), matches[0].length());
				GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, jstr, rulexx, 0, GrapaCHAR());
				result = vScriptExec->ProcessPlan(pNameSpace, plan);
			}
		}
		else {
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::ARRAY;
			result->vQueue = new GrapaRuleQueue();

			for (const auto& m : matches) {
				// Ensure empty strings are handled properly - never create null tokens
				GrapaCHAR matchStr;
				if (m.empty()) {
					// Explicitly create an empty string token with STR token type
					matchStr.FROM("", 0);
					matchStr.mToken = GrapaTokenType::STR;
				} else {
					matchStr.FROM(m.c_str(), m.length());
				}
				result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), matchStr));
			}
		}
	}
	catch (const std::regex_error& e) {
		// Optionally: return empty results or propagate error
		result = new GrapaRuleEvent();
		result->mValue.mToken = GrapaTokenType::ERR;
		result->vQueue = new GrapaRuleQueue();
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaCHAR(e.what())));
	}


	return result;
}

GrapaRuleEvent* GrapaLibraryRuleSplitEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	if (r1.vVal && r2.vVal)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::RAW) && (r2.vVal->mValue.mToken != GrapaTokenType::ID && r2.vVal->mValue.mToken != GrapaTokenType::SYSID))
		{
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::ARRAY;
			result->vQueue = new GrapaRuleQueue();
			if (r1.vVal->mValue.mLength)
			{
				GrapaCHAR del;
				s64 n = 0;
				if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
				{
					if (r3.vVal && (r3.vVal->mValue.mToken == GrapaTokenType::STR || r3.vVal->mValue.mToken == GrapaTokenType::RAW))
						del.FROM(r3.vVal->mValue);
					GrapaInt a;
					a.FromBytes(r2.vVal->mValue);
					n = a.LongValue();
				}
				else
				{
					if (r2.vVal && (r2.vVal->mValue.mToken == GrapaTokenType::STR || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
						del.FROM(r2.vVal->mValue);
					if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::INT)
					{
						GrapaInt a;
						a.FromBytes(r3.vVal->mValue);
						n = a.LongValue();
					}
				}
				if (n<=0)
				{
					GrapaRuleEvent* r = NULL;
					char* s = (char*)r1.vVal->mValue.mBytes;
					char* s2;
					u64 len = r1.vVal->mValue.mLength;
					if (del.mLength == 0)
					{
						while (len)
						{
							s2 = &s[1];
							r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR((char*)s, s2 - s));
							if (r == NULL)
							{
								s = NULL;
								break;
							}
							r->mValue.mToken = r1.vVal->mValue.mToken;
							result->vQueue->PushTail(r);
							len = len - (s2 - s) - 0;
							s = s2 + 0;
						}
						s = NULL;
					}
					else
					{
						GrapaCHAR name;
						while (true)
						{
							s2 = strstr((char*)s, (char*)del.mBytes);
							if (!s2) break;
							r = new GrapaRuleEvent(0, name, GrapaCHAR(s, s2 - s));
							if (r == NULL)
							{
								s = NULL;
								break;
							}
							r->mValue.mToken = r1.vVal->mValue.mToken;
							result->vQueue->PushTail(r);
							len = len - ((s2 - s) + del.mLength);
							s = s2 + del.mLength;
						}
					}
					if (len)
					{
						r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR((char*)s, len));
						if (r)
						{
							r->mValue.mToken = r1.vVal->mValue.mToken;
							result->vQueue->PushTail(r);
						}
					}
				}
				else
				{
					u64 p = (u64)(r1.vVal->mValue.mLength / n);
					if (r1.vVal->mValue.mLength % n)
						p++;
					u64 p_i = 0;
					u64 p_c = (u64)r1.vVal->mValue.mLength;
					GrapaRuleEvent* r = NULL;
					char* s = (char*)r1.vVal->mValue.mBytes;
					while (p_i < p_c)
					{
						u64 len = p;
						if ((p_i + len) > p_c)
							len = p_c - p_i;

						if (del.mLength && len >= del.mLength)
						{
							char* t_s = (char*)GrapaMem::MemFind(s + len - del.mLength, p_c - p_i - len + del.mLength, del.mBytes, del.mLength);
							//char* t_s = strstr(s + len - del.mLength, (char*)del.mBytes);
							if (t_s)
								len = (u64)((t_s - s) + del.mLength);
							else
								len = p_c - p_i;
						}
						r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
						if (r)
						{
							if ((p_i + len) < p_c && del.mLength && len >= del.mLength)
								r->mValue.FROM(s, len - del.mLength);
							else
								r->mValue.FROM(s, len);
							r->mValue.mToken = r1.vVal->mValue.mToken;
							result->vQueue->PushTail(r);
						}
						s += len;
						p_i += len;
					}
				}
			}
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::TAG))
		{
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::ARRAY;
			result->vQueue = new GrapaRuleQueue();
			GrapaCHAR del;
			s64 n = 0;
			s64 axis = 0;
			if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				if (r3.vVal && (r3.vVal->mValue.mToken == GrapaTokenType::STR || r3.vVal->mValue.mToken == GrapaTokenType::RAW))
					del.FROM(r3.vVal->mValue);
				GrapaInt a;
				a.FromBytes(r2.vVal->mValue);
				n = a.LongValue();
			}
			else
			{
				if (r2.vVal && (r2.vVal->mValue.mToken == GrapaTokenType::STR || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
					del.FROM(r2.vVal->mValue);
				if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::INT)
				{
					GrapaInt a;
					a.FromBytes(r3.vVal->mValue);
					n = a.LongValue();
				}
			}
			if (r4.vVal && r4.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				GrapaInt a;
				a.FromBytes(r4.vVal->mValue);
				axis = a.LongValue();
			}

			if (n <= 0) n = 1;

			GrapaRuleEvent* r = NULL;
			GrapaRuleEvent* s = r1.vVal;
			if (r1.vVal->mValue.mToken == GrapaTokenType::XML)
				s = r1.vVal; //->vQueue->Head(0)->vQueue->Head(0)->vQueue->Head(1);
			else if (r1.vVal->mValue.mToken == GrapaTokenType::TAG)
				s = r1.vVal->vQueue->Head(0)->vQueue->Head(1);

			u32 p = (u32)((s->vQueue->mCount / n) + 1);
			if ((s->vQueue->mCount % n) == 0) p--;

			if (s) s = s->vQueue->Head();

			while (s)
			{
				r = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
				r->mValue.mToken = r1.vVal->mValue.mToken;
				result->vQueue->PushTail(r);
				r->vQueue = new GrapaRuleQueue();
				if (r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::TAG)
					r->mValue.mToken = GrapaTokenType::XML;
				for (u32 i = 0; s && i < p; i++)
				{
					r->vQueue->PushTail(vScriptExec->CopyItem(s));
					s = s->Next();
				}
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
		{
			GrapaCHAR del;
			s64 n = 0;
			s64 axis = 0;
			if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				if (r3.vVal && (r3.vVal->mValue.mToken == GrapaTokenType::STR || r3.vVal->mValue.mToken == GrapaTokenType::RAW))
					del.FROM(r3.vVal->mValue);
				GrapaInt a;
				a.FromBytes(r2.vVal->mValue);
				n = a.LongValue();
			}
			else
			{
				if (r2.vVal && (r2.vVal->mValue.mToken == GrapaTokenType::STR || r2.vVal->mValue.mToken == GrapaTokenType::RAW))
					del.FROM(r2.vVal->mValue);
				if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::INT)
				{
					GrapaInt a;
					a.FromBytes(r3.vVal->mValue);
					n = a.LongValue();
				}
			}
			if (r4.vVal && r4.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				GrapaInt a;
				a.FromBytes(r4.vVal->mValue);
				axis = a.LongValue();
			}
			result = r1.vVal->vVector->Split(n,axis);
			if (result == 0L)
			{
				result = Error(vScriptExec, pNameSpace, -1);
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::STR && r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
		{
			if (r2.vVal->mValue.Cmp("EL") == 0)
			{
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::LIST;
				result->vQueue = new GrapaRuleQueue();
				GrapaRuleEvent* r = NULL;
				char* s = (char*)r1.vVal->mValue.mBytes;
				char* s2, * s3, * s4, * s5;
				u64 len = r1.vVal->mValue.mLength;
				while ((s2 = strchr(s, (int)'<')))
				{
					s3 = strchr(&s2[1], (int)'>');
					if (s3 == NULL) { s = NULL; break; }
					s4 = strchr(&s3[1], (int)'<');
					if (s4 == NULL) { s = NULL; break; }
					s5 = strchr(&s4[1], (int)'>');
					if (s5 == NULL) { s = NULL; break; }
					r = new GrapaRuleEvent(0, GrapaCHAR(&s2[1], s3 - s2 - 1), GrapaCHAR(&s3[1], s4 - s3 - 1));
					if (r == NULL) { s = NULL; break; }
					r->mValue.mToken = r1.vVal->mValue.mToken;
					result->vQueue->PushTail(r);
					len = len - (s5 - s) - 1;
					s = s5 + 1;
				}
			}
			else if (r2.vVal->mValue.Cmp("CSV") == 0)	// ="\"this\",is,\"a big\",test\r\nand,another" / $CSV
			{
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::ARRAY;
				result->vQueue = new GrapaRuleQueue();
				char* s = (char*)r1.vVal->mValue.mBytes;
				GrapaCHAR del;
				GrapaCHAR line;
				char* pr = (char*)GrapaMem::MemFind(r1.vVal->mValue.mBytes, '\r', r1.vVal->mValue.mLength);
				char* pn = (char*)GrapaMem::MemFind(r1.vVal->mValue.mBytes, '\n', r1.vVal->mValue.mLength);
				if (pn > (pr + 1)) pn = NULL;
				if (pr && pn)
				{
					if (pr < pn)
						del.FROM("\r\n");
					else
						del.FROM("\n\r");
				}
				else if (pn)
					del.FROM("\n");
				else
					del.FROM("\r");

				char sep = 0;

				u64 len = r1.vVal->mValue.mLength;
				while (len)
				{
					GrapaRuleEvent* e = new GrapaRuleEvent();
					e->mValue.mToken = GrapaTokenType::ARRAY;
					e->vQueue = new GrapaRuleQueue();
					result->vQueue->PushTail(e);
					if (char* s2 = strstr(s, (char*)del.mBytes))
					{
						line.FROM(s, s2 - s);
						len = len - (s2 - s) - del.mLength;
						s = s2 + del.mLength;
					}
					else
					{
						line.FROM(s, len);
						len = 0;
						s = NULL;
					}

					u64 lenC = line.mLength;
					char* sC = (char*)line.mBytes;
					while (lenC)
					{
						GrapaRuleEvent* r = NULL;
						if (*sC == '"')
						{
							if (lenC)
							{
								//u64 trunc = 0;
								char* sC2 = sC + 1;
								char* v = sC2;
								u64 lenC2 = lenC - 1;
								while (lenC2)
								{
									if (*v == '"')
									{
										if (lenC2 > 1 && v[1] == '"')
										{
											v = v + 1;
											lenC2--;
										}
										else
											break;
									}
									*sC2 = *v;
									sC2 = sC2 + 1;
									v = v + 1;
									lenC2--;
								}
								r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(&sC[1], sC2 - sC - 1));
								lenC = lenC - (v - sC) - 1;
								sC = v + 1;
								if (sep == 0)
								{
									u64 lenC2 = lenC;
									char* sC2 = sC;
									while (lenC2 && *sC2 == ' ')
									{
										lenC2--;
										sC2 = sC2 + 1;
									}
									if (lenC2 && *sC2 == ',') sep = ',';
									else if (lenC2 && *sC2 == '\t') sep = '\t';
									else sep = ',';
								}
								if (lenC && *sC == sep)
								{
									lenC--;
									sC = sC + 1;
								}
							}
							else
							{
								r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
								lenC = 0;
								sC = NULL;
							}
						}
						else
						{
							if (sep == 0)
							{
								u64 lenC2 = lenC;
								char* sC2 = sC;
								while (lenC2 && *sC2 != ',' && *sC2 != '\t')
								{
									lenC2--;
									sC2 = sC2 + 1;
								}
								if (lenC2 && *sC2 == ',') sep = ',';
								else if (lenC2 && *sC2 == '\t') sep = '\t';
								else sep = ',';
							}
							if (char* sC2 = strchr(sC, (int)sep))
							{
								r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(sC, sC2 - sC));
								lenC = lenC - (sC2 - sC) - 1;
								sC = sC2 + 1;
							}
							else
							{
								r = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(sC, lenC));
								lenC = 0;
								sC = NULL;

							}
						}
						if (r)
						{
							r->mValue.mToken = r1.vVal->mValue.mToken;
							e->vQueue->PushTail(r);
						}
					}
				}
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::TABLE && r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
		{
			if (r2.vVal->mValue.Cmp("CSV") == 0)
			{
				result = new GrapaRuleEvent();
				result->mValue.mToken = GrapaTokenType::ARRAY;
				result->vQueue = new GrapaRuleQueue();

				GrapaError err;
				GrapaGroup g;
				GrapaCHAR s;
				err = g.OpenFile(&r1.vVal->mValue, GrapaReadWrite);
				GrapaDBFieldArray* fieldList = g.ListFields(g.mRootTable, g.mRootType);
				if (fieldList)
				{
					GrapaRuleEvent* e = new GrapaRuleEvent();
					e->mValue.mToken = GrapaTokenType::LIST;
					e->vQueue = new GrapaRuleQueue();
					result->vQueue->PushTail(e);
					for (u32 i = 0; i < fieldList->Count(); i++)
					{
						GrapaDBField* field = fieldList->GetFieldAt(i);
						if (!field) continue;
						err = g.GetData(field->mNameRef, s);
						GrapaRuleEvent* r = new GrapaRuleEvent(0, s, GrapaInt(i).getBytes());
						e->vQueue->PushTail(r);
					}
					GrapaDBCursor cursor;
					cursor.Set(g.mRootTable);
					if (g.First(cursor) == 0)
					{
						do
						{
							GrapaRuleEvent* e = new GrapaRuleEvent();
							e->mValue.mToken = GrapaTokenType::ARRAY;
							e->vQueue = new GrapaRuleQueue();
							result->vQueue->PushTail(e);
							for (u32 i = 0; i < fieldList->Count(); i++)
							{
								GrapaDBField* field = fieldList->GetFieldAt(i);
								if (!field) continue;
								err = g.GetRecordField(cursor, *field, s);
								GrapaRuleEvent* r = new GrapaRuleEvent(0, GrapaCHAR(""), s);
								e->vQueue->PushTail(r);
							}
						} while (!(err = g.Next(cursor)));
					}
					delete fieldList;
				}
			}
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt x,c;
			x.FromBytes(r1.vVal->mValue);
			c.FromBytes(r2.vVal->mValue);
			u64 sz = c.LongValue();
			s64 xLength = x.GetCount();
			s64 N = (xLength / 2) + (xLength % 2);
			GrapaInt a, b;
			for (s32 j = 0; j < N; j++) b.SetItem(j, x.GetItem(j));
			for (s32 j = N; j < xLength; j++) a.SetItem(j - N, x.GetItem(j));
			a.RTrim();
			b.RTrim();
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::ARRAY;
			result->vQueue = new GrapaRuleQueue();
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), a.getBytes()));
			result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), b.getBytes()));
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

// x=(0x12345678901234567890).split(2);[x[0].hex(),x[1].hex()];
// [x[0].hex(),x[1].hex()]

GrapaRuleEvent* GrapaLibraryRuleJoinEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	if (r1.vVal) // && (!r2.vVal || r2.vVal->mNull))
	{
		GrapaCHAR pad;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::STR)
			pad.FROM(r2.vVal->mValue);
		if ((r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::CODE) && r1.vVal->vQueue)
		{
			bool isQueue = false;
			bool isVector = false;
			bool isWidget = false;
			result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
			GrapaRuleEvent* e = (GrapaRuleEvent*)r1.vVal->vQueue->Head();
			u64 sz = 0;
			while (e)
			{
				bool addPad = false;
				GrapaRuleEvent* e2 = e;
				while (e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
				if (e2->mValue.mToken == GrapaTokenType::INT || e2->mValue.mToken == GrapaTokenType::SYSINT)
				{
					GrapaInt a;
					a.FromBytes(e2->mValue);
					GrapaCHAR b(a.ToString(10));
					sz += b.mLength;
					if (e2->mValue.mToken == GrapaTokenType::SYSINT)
						sz += 1;
					addPad = true;
				}
				else if (e2->mValue.mToken == GrapaTokenType::FLOAT)
				{
					GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
					a.FromBytes(e2->mValue);
					GrapaCHAR b(a.ToString(10));
					sz += b.mLength;
					addPad = true;
				}
				else if (e2->mValue.mToken == GrapaTokenType::STR)
				{
					if (e2->mQuote)
						sz += 2;
					sz += e2->mValue.mLength;
					addPad = true;
				}
				else if (e2->mValue.mToken == GrapaTokenType::SYSSTR)
				{
					sz += 1;
					if (e2->mQuote)
						sz += 2;
					sz += e2->mValue.mLength;
					addPad = true;
				}
				else if (e2->mValue.mToken == GrapaTokenType::SYSID || e2->mValue.mToken == GrapaTokenType::SYSSYM)
				{
					sz += 1;
					sz += e2->mValue.mLength;
					addPad = true;
				}
				else if ((e2->mValue.mToken == GrapaTokenType::ARRAY || e2->mValue.mToken == GrapaTokenType::TUPLE || e2->mValue.mToken == GrapaTokenType::LIST) && e2->vQueue)
				{
					if (!isQueue)
					{
						isQueue = true;
						result->mValue.mToken = e2->mValue.mToken;
						result->vQueue = new GrapaRuleQueue();
					}
					sz += e2->vQueue->mCount;
				}
				else if ((e2->mValue.mToken == GrapaTokenType::VECTOR) && e2->vVector)
				{
					if (!isVector)
					{
						isVector = true;
						result->mValue.mToken = e2->mValue.mToken;
						result->vVector = new GrapaVector();
					}
					if (e2->vVector->mDim)
						sz += e2->vVector->mCounts[0];
				}
				else if ((e2->mValue.mToken == GrapaTokenType::WIDGET) && e2->vWidget)
				{
					if (!isWidget)
					{
						isWidget = true;
						result->mValue.mToken = e2->mValue.mToken;
						result->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
						result->vWidget->vEvent = result;
						result->vWidget->New((char*)pad.mBytes);
					}
				}
				else
				{
					sz += e2->mValue.mLength;
					addPad = true;
				}
				e = e->Next();
				if (e && addPad) sz += pad.mLength;
			}
			if (isVector)
			{
				s64 axis = 0;
				if (r3.vVal && r3.vVal->mValue.mToken == GrapaTokenType::INT)
				{
					GrapaInt a;
					a.FromBytes(r3.vVal->mValue);
					axis = a.LongValue();
				}
				if (axis == 1)
				{
					if (!result->vVector->JoinH(r1.vVal))
					{
						result->CLEAR();
						delete result;
						result = NULL;
					}
				}
				else if (!result->vVector->Join(r1.vVal))
				{
					result->CLEAR();
					delete result;
					result = NULL;
				}
			}
			else if (isWidget)
			{
				if (result->vWidget->Join(r1.vVal))
				{
					result->CLEAR();
					delete result;
					result = NULL;
				}
			}
			else
			{
				if (!isQueue)
				{
					result->mValue.SetLength(sz, false);
					result->mValue.SetLength(0, false);
				}
				e = (GrapaRuleEvent*)r1.vVal->vQueue->Head();
				while (e)
				{
					bool addPad = false;
					GrapaRuleEvent* e2 = e;
					while (e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
					if (e2->mValue.mToken == GrapaTokenType::INT || e2->mValue.mToken == GrapaTokenType::SYSINT)
					{
						if (e2->mValue.mToken == GrapaTokenType::SYSINT)
							result->mValue.Append(1, "$");
						GrapaInt a;
						a.FromBytes(e2->mValue);
						GrapaCHAR b(a.ToString(10));
						result->mValue.Append(b.mLength, b.mBytes);
						addPad = true;
					}
					else if (e2->mValue.mToken == GrapaTokenType::FLOAT)
					{
						GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
						a.FromBytes(e2->mValue);
						GrapaCHAR b(a.ToString(10));
						result->mValue.Append(b.mLength, b.mBytes);
						addPad = true;
					}
					else if (e2->mValue.mToken == GrapaTokenType::STR)
					{
						if (e2->mQuote)
							result->mValue.Append(1, &e2->mQuote);
						if (e2->mValue.mBytes)
							result->mValue.Append(e2->mValue);
						if (e2->mQuote)
							result->mValue.Append(1, &e2->mQuote);
						addPad = true;
					}
					else if (e2->mValue.mToken == GrapaTokenType::SYSSTR)
					{
						result->mValue.Append(1, "$");
						if (e2->mQuote)
							result->mValue.Append(1, &e2->mQuote);
						if (e2->mValue.mBytes)
							result->mValue.Append(e2->mValue);
						if (e2->mQuote)
							result->mValue.Append(1, &e2->mQuote);
						addPad = true;
					}
					else if (e2->mValue.mToken == GrapaTokenType::SYSID || e2->mValue.mToken == GrapaTokenType::SYSSYM)
					{
						result->mValue.Append(1, "$");
						if (e2->mValue.mBytes)
							result->mValue.Append(e2->mValue);
						addPad = true;
					}
					else if ((e2->mValue.mToken == GrapaTokenType::ARRAY || e2->mValue.mToken == GrapaTokenType::TUPLE || e2->mValue.mToken == GrapaTokenType::LIST) && e2->vQueue)
					{
						GrapaRuleEvent* f = e2->vQueue->Head();
						while (f)
						{
							result->vQueue->PushTail(vScriptExec->CopyItem(f));
							f = f->Next();
						}
					}
					else
					{
						if (e2->mValue.mBytes)
							result->mValue.Append(e2->mValue);
						addPad = true;
					}

					e = e->Next();
					if (e && addPad && pad.mLength) result->mValue.GrapaCHAR::Append(pad);
					e2 = e;
					while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) e2 = e2->vRulePointer;
				}
			}
		}
	}
	else if (r1.vVal && r2.vVal && r2.vVal->mValue.mToken != GrapaTokenType::ERR)
	{
		if ((r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE || r1.vVal->mValue.mToken == GrapaTokenType::LIST || r1.vVal->mValue.mToken == GrapaTokenType::XML || r1.vVal->mValue.mToken == GrapaTokenType::EL || r1.vVal->mValue.mToken == GrapaTokenType::TAG || r1.vVal->mValue.mToken == GrapaTokenType::OP || r1.vVal->mValue.mToken == GrapaTokenType::CODE) && r1.vVal->vQueue && r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
		{
			if (r2.vVal->mValue.Cmp("TAG") == 0)
			{
			}
			else if (r2.vVal->mValue.Cmp("EL") == 0)
			{
			}
			else if (r2.vVal->mValue.Cmp("CSV") == 0)
			{
				// convert the list to a CSV
				result = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
				GrapaRuleEvent* x = r1.vVal;
				while (x->vRulePointer) x = x->vRulePointer;
				if ((x->mValue.mToken == GrapaTokenType::ARRAY || x->mValue.mToken == GrapaTokenType::TUPLE || x->mValue.mToken == GrapaTokenType::LIST || x->mValue.mToken == GrapaTokenType::XML || x->mValue.mToken == GrapaTokenType::EL || x->mValue.mToken == GrapaTokenType::TAG) && x->vQueue)
				{
					x = (GrapaRuleEvent*)x->vQueue->Head();
					while (x)
					{
						if ((x->mValue.mToken == GrapaTokenType::ARRAY || x->mValue.mToken == GrapaTokenType::TUPLE || x->mValue.mToken == GrapaTokenType::LIST || x->mValue.mToken == GrapaTokenType::XML || x->mValue.mToken == GrapaTokenType::EL || x->mValue.mToken == GrapaTokenType::TAG || x->mValue.mToken == GrapaTokenType::OP || x->mValue.mToken == GrapaTokenType::CODE))
						{
							GrapaRuleEvent* y = (GrapaRuleEvent*)x->vQueue->Head();
							while (y)
							{
								switch (y->mValue.mToken)
								{
								case GrapaTokenType::STR:
								case GrapaTokenType::SYSSTR:
								case GrapaTokenType::ID:
								case GrapaTokenType::SYSID:
								{
									bool hasSpace = y->mValue.mBytes ? strpbrk((const char*)y->mValue.mBytes, (const char*)" ,\"") != NULL : false;
									bool hasQuote = y->mValue.mBytes ? GrapaMem::MemFind(y->mValue.mBytes, '\"', y->mValue.mLength) != NULL : false;
									if (hasSpace) result->mValue.GrapaCHAR::Append('\"');
									if (hasQuote && y->mValue.mBytes)
									{
										GrapaCHAR s(y->mValue);
										s.Replace(GrapaBYTE("\""), GrapaBYTE("\"\""));
										result->mValue.GrapaCHAR::Append(s);
									}
									else
									{
										if (y->mValue.mBytes)
											result->mValue.Append(y->mValue);
									}
									if (hasSpace) result->mValue.GrapaCHAR::Append('\"');
								}
								break;
								case GrapaTokenType::INT:
								case GrapaTokenType::SYSINT:
								{
									if (y->mValue.mBytes)
									{
										GrapaInt b;
										b.FromBytes(y->mValue);
										result->mValue.GrapaCHAR::Append(b.ToString());
									}
								}
								break;
								}
								y = y->Next();
								if (y) result->mValue.GrapaCHAR::Append(',');
							}
						}
						x = x->Next();
						if (x) result->mValue.GrapaCHAR::Append("\r\n");
					}
				}
			}
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleShapeEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = r1.vVal->vVector->Shape();
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		result = GrapaVector::Shape(r1.vVal);
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleReShapeEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = vScriptExec->CopyItem(r1.vVal);
		if (result->vVector->ReShape(r2.vVal) != 0)
		{
			result->CLEAR();
			delete result;
			result = NULL;
		}
	}
	else if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector v;
		v.FROM(vScriptExec, r1.vVal, 0);
		if (v.ReShape(r2.vVal)==0)
		{
			result = v.ToArray();
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleDotEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::ARRAY && r2.vVal->mValue.mToken == GrapaTokenType::ARRAY)
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector bb;
		bb.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r2.vVal, 0);
		GrapaVector cc;
		GrapaError err = aa.Dot(vScriptExec, pNameSpace, bb, cc);
		if (err)
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = cc.ToArray();
		}
	}
	else if (r1.vVal && r2.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR && r2.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
		result->mValue.mToken = GrapaTokenType::VECTOR;
		result->vVector = new GrapaVector();
		GrapaError err = r1.vVal->vVector->Dot(vScriptExec, pNameSpace, *r2.vVal->vVector, *result->vVector);
		if (err)
		{
			result->vVector->CLEAR();
			delete result->vVector;
			result->vVector = NULL;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleIdentityEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r1.vVal->mValue);
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		result->vVector->Identity(vScriptExec,a.LongValue());
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleDiagonalEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	s64 d = 0;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		d = a.LongValue();
	}
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector cc;
		if (!aa.Diagonal(vScriptExec, d, cc))
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = cc.ToArray();
		}
	}
	else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		if (!r1.vVal->vVector->Diagonal(vScriptExec, d, *result->vVector))
		{
			result->vVector->CLEAR();
			delete result->vVector;
			result->vVector = NULL;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTriUEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	s64 d = 0;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		d = a.LongValue();
	}
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector cc;
		if (!aa.TriU(vScriptExec, d, cc))
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = cc.ToArray();
		}
	}
	else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		if (!r1.vVal->vVector->TriU(vScriptExec, d, *result->vVector))
		{
			result->vVector->CLEAR();
			delete result->vVector;
			result->vVector = NULL;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTriLEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	s64 d = 0;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		d = a.LongValue();
	}
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector cc;
		if (!aa.TriL(vScriptExec, d, cc))
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = cc.ToArray();
		}
	}
	else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		if (!r1.vVal->vVector->TriL(vScriptExec, d, *result->vVector))
		{
			result->vVector->CLEAR();
			delete result->vVector;
			result->vVector = NULL;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleEigHEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector cc,dd;
		if (!aa.EigH(vScriptExec, pNameSpace, cc, dd) || cc.mData==NULL || dd.mData==NULL)
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "", "");
			result->vQueue = new GrapaRuleQueue();
			GrapaRuleEvent* w = cc.ToArray();
			w->mName.FROM("w");
			result->vQueue->PushTail(w);
			GrapaRuleEvent* v = dd.ToArray();
			v->mName.FROM("v");
			result->vQueue->PushTail(v);
		}
	}
	else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "", "");
		result->vQueue = new GrapaRuleQueue();
		GrapaRuleEvent* w = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "w", "");
		w->vVector = new GrapaVector();
		result->vQueue->PushTail(w);
		GrapaRuleEvent* v = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "v", "");
		v->vVector = new GrapaVector();
		result->vQueue->PushTail(v);
		if (!r1.vVal->vVector->EigH(vScriptExec, pNameSpace, *w->vVector, *v->vVector))
		{
			result->CLEAR();
			delete result;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSumEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	s64 axis = 0;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		axis = a.LongValue();
	}
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector cc;
		if (aa.Sum(vScriptExec, cc, axis==0) || cc.mData == NULL)
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = cc.ToArray();
		}
	}
	else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		if (r1.vVal->vVector->Sum(vScriptExec, *result->vVector, axis == 0))
		{
			result->CLEAR();
			delete result;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleMeanEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	s64 axis = 0;
	if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
	{
		GrapaInt a;
		a.FromBytes(r2.vVal->mValue);
		axis = a.LongValue();
	}
	if (r1.vVal && (r1.vVal->mValue.mToken == GrapaTokenType::ARRAY || r1.vVal->mValue.mToken == GrapaTokenType::TUPLE))
	{
		GrapaVector aa;
		aa.FROM(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, r1.vVal, 0);
		GrapaVector cc;
		if (aa.Mean(vScriptExec, cc, axis == 0) || cc.mData == NULL)
		{
			result = Error(vScriptExec, pNameSpace, -1);
		}
		else
		{
			result = cc.ToArray();
		}
	}
	else if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::VECTOR)
	{
		result = new GrapaRuleEvent(GrapaTokenType::VECTOR, 0, "", "");
		result->vVector = new GrapaVector();
		if (r1.vVal->vVector->Mean(vScriptExec, *result->vVector, axis == 0))
		{
			result->CLEAR();
			delete result;
			result = Error(vScriptExec, pNameSpace, -1);
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLowerEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::ID:
			result = new GrapaRuleEvent(0, GrapaCHAR(), r1.vVal->mValue);
			result->mValue.ToLower();
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleUpperEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	if (r1.vVal)
	{
		switch (r1.vVal->mValue.mToken)
		{
		case GrapaTokenType::STR:
		case GrapaTokenType::ID:
			result = new GrapaRuleEvent(0, GrapaCHAR(), r1.vVal->mValue);
			result->mValue.ToUpper();
			break;
		default:
			break;
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleUtcEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaTime t;
	GrapaCHAR name;
	t.SetNow();
	GrapaRuleEvent* result = new GrapaRuleEvent(0, name, t.getBytes());
	result->mValue.mToken = GrapaTokenType::TIME;
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleTzEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaTime t;
	GrapaCHAR name;
	GrapaInt num1;
	num1 = t.LocalOffset();
	GrapaRuleEvent* result = new GrapaRuleEvent(0, name, num1.getBytes());
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleEqEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::BOOL)
	{
		bool isEqual = false;
		if (r1.vVal->IsNull())
		{
			if (r2.vVal == NULL || r2.vVal->IsZero())
				isEqual = true;
		}
		else
		{
			isEqual = true;
			if (r2.vVal == NULL || r2.vVal->IsZero())
				isEqual = false;
		}
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(isEqual));
		return(result);
	}
	else if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::BOOL)
	{
		bool isEqual = false;
		if (r2.vVal->IsNull())
		{
			if (r1.vVal == NULL || r1.vVal->IsZero())
				isEqual = true;
		}
		else
		{
			isEqual = true;
			if (r1.vVal == NULL || r1.vVal->IsZero())
				isEqual = false;
		}
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(isEqual));
		return(result);
	}
	if (r1.vVal && r2.vVal && r1.vVal->mValue.mBytes && r2.vVal->mValue.mBytes)
	{
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a == b));
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat b(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a == b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bb.FromBytes(r2.vVal->mValue);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa == bb));
			}
			else
			{
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(r1.vVal->mValue);
				b.FromBytes(r2.vVal->mValue);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa == bb));
			}
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID))
		{
			GrapaCHAR a;
			if (r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::SYSID)
				a.FROM("$");
			a.Append(r1.vVal->mValue);
			GrapaCHAR b;
			if (r2.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
				b.FROM("$");
			b.Append(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a.StrCmp(b) == 0));
		}
	}
	if (result == NULL)
	{
		if ((r1.vVal == NULL || r1.vVal->IsNull()) && (r2.vVal == NULL || r2.vVal->IsNull()))
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(true));
			return(result);
		}
		else if (r1.vVal && r2.vVal)
		{
			bool isEqual = false;
			if (r1.vVal->mId && r1.vVal->mId == r2.vVal->mId) isEqual = true;
			else
			{
				GrapaRuleEvent* rx1 = r1.vVal;
				GrapaRuleEvent* rx2 = r2.vVal;
				if (rx1->mValue.StrCmp(rx2->mValue) == 0) isEqual = true;
			}
			if (isEqual)
			{
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(true));
				return(result);
			}
		}
	}

	if (result == NULL)
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(false));
	
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleNEqEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r1.vVal->mValue.mToken == GrapaTokenType::BOOL)
	{
		bool isEqual = false;
		if (r1.vVal->IsNull())
		{
			if (r2.vVal == NULL || r2.vVal->IsZero())
				isEqual = true;
		}
		else
		{
			isEqual = true;
			if (r2.vVal == NULL || r2.vVal->IsZero())
				isEqual = false;
		}
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(!isEqual));
		return(result);
	}
	else if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::BOOL)
	{
		bool isEqual = false;
		if (r2.vVal->IsNull())
		{
			if (r1.vVal == NULL || r1.vVal->IsZero())
				isEqual = true;
		}
		else
		{
			isEqual = true;
			if (r1.vVal == NULL || r1.vVal->IsZero())
				isEqual = false;
		}
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(!isEqual));
		return(result);
	}
	if (r1.vVal && r2.vVal && r1.vVal->mValue.mBytes && r2.vVal->mValue.mBytes)
	{
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a != b));
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat b(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a != b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bb.FromBytes(r2.vVal->mValue);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa != bb));
			}
			else
			{
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(r1.vVal->mValue);
				b.FromBytes(r2.vVal->mValue);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa != bb));
			}
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID))
		{
			GrapaCHAR a;
			if (r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::SYSID)
				a.FROM("$");
			a.Append(r1.vVal->mValue);
			GrapaCHAR b;
			if (r2.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
				b.FROM("$");
			b.Append(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a.StrCmp(b) != 0));
		}
	}
	if (result == NULL)
	{
		if ((r1.vVal == NULL || r1.vVal->IsNull()) && (r2.vVal == NULL || r2.vVal->IsNull()))
		{
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(false));
			return(result);
		}
		else if (r1.vVal && r2.vVal)
		{
			bool isEqual = false;
			if (r1.vVal->mId && r1.vVal->mId == r2.vVal->mId) isEqual = true;
			else if (r1.vVal->mValue.StrCmp(r2.vVal->mValue) == 0) isEqual = true;
			if (isEqual)
			{
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(false));
				return(result);
			}
		}
	}
	if (result == NULL)
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(true));
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGtEqEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		//bool isEqual = false;
		GrapaCHAR name;
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a >= b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a >= b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID))
		{
			GrapaCHAR a;
			if (r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::SYSID)
				a.FROM("$");
			a.Append(r1.vVal->mValue);
			GrapaCHAR b;
			if (r2.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
				b.FROM("$");
			b.Append(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a.StrCmp(b) >= 0));
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat b(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a >= b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a >= b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bb.FromBytes(r2.vVal->mValue);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa >= bb));
			}
			else
			{
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(r1.vVal->mValue);
				b.FromBytes(r2.vVal->mValue);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa >= bb));
			}
		}
	}
	if (result == NULL)
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(false));
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleGtEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		//bool isEqual = false;
		GrapaCHAR name;
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a > b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a > b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID))
		{
			GrapaCHAR a;
			if (r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::SYSID)
				a.FROM("$");
			a.Append(r1.vVal->mValue);
			GrapaCHAR b;
			if (r2.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
				b.FROM("$");
			b.Append(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a.StrCmp(b) > 0));
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat b(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a > b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a > b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT|| r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bb.FromBytes(r2.vVal->mValue);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa > bb));
			}
			else
			{
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(r1.vVal->mValue);
				b.FromBytes(r2.vVal->mValue);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa > bb));
			}
		}
	}
	if (result == NULL)
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(false));
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLtEqEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		//bool isEqual = false;
		GrapaCHAR name;
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a <= b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a <= b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID))
		{
			GrapaCHAR a;
			if (r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::SYSID)
				a.FROM("$");
			a.Append(r1.vVal->mValue);
			GrapaCHAR b;
			if (r2.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
				b.FROM("$");
			b.Append(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a.StrCmp(b) <= 0));
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat b(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a <= b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a <= b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bb.FromBytes(r2.vVal->mValue);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa <= bb));
			}
			else
			{
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(r1.vVal->mValue);
				b.FromBytes(r2.vVal->mValue);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa <= bb));
			}
		}
	}
	if (result == NULL)
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(false));
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLtEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent *result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		//bool isEqual = false;
		GrapaCHAR name;
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a < b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a < b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID))
		{
			GrapaCHAR a;
			if (r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::SYSID)
				a.FROM("$");
			a.Append(r1.vVal->mValue);
			GrapaCHAR b;
			if (r2.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
				b.FROM("$");
			b.Append(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a.StrCmp(b) < 0));
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat b(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			//if (a < b)
			//	result = new GrapaRuleEvent(0, name, ((GrapaInt)1).getBytes());
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(a < b));
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bb.FromBytes(r2.vVal->mValue);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa < bb));
			}
			else
			{
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(r1.vVal->mValue);
				b.FromBytes(r2.vVal->mValue);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
				result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(aa < bb));
			}
		}
	}
	if (result == NULL)
		result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR::SetBool(false));
	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleCmpEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaRuleEvent* result = NULL;
	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	if (r1.vVal && r2.vVal)
	{
		//bool isEqual = false;
		GrapaCHAR name;
		GrapaInt a, b;
		if (r1.vVal->mValue.mToken == GrapaTokenType::INT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			s32 cmp = 0;
			if (a < b) cmp = -1;
			if (a > b) cmp = 1;
			result = new GrapaRuleEvent(0, name, ((GrapaInt)(cmp)).getBytes());
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::STR || r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::ID || r1.vVal->mValue.mToken == GrapaTokenType::SYSID))
		{
			GrapaCHAR a;
			if (r1.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r1.vVal->mValue.mToken == GrapaTokenType::SYSID)
				a.FROM("$");
			a.Append(r1.vVal->mValue);
			GrapaCHAR b;
			if (r2.vVal->mValue.mToken == GrapaTokenType::SYSSTR || r2.vVal->mValue.mToken == GrapaTokenType::SYSID)
				b.FROM("$");
			b.Append(r2.vVal->mValue);
			result = new GrapaRuleEvent(0, name, GrapaInt(a.StrCmp(b)).getBytes());
		}
		else if (r1.vVal->mValue.mToken == GrapaTokenType::FLOAT && r1.vVal->mValue.mToken == r2.vVal->mValue.mToken)
		{
			GrapaFloat a(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			GrapaFloat b(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			a.FromBytes(r1.vVal->mValue);
			b.FromBytes(r2.vVal->mValue);
			s32 cmp = 0;
			if (a < b) cmp = -1;
			if (a > b) cmp = 1;
			result = new GrapaRuleEvent(0, name, ((GrapaInt)(cmp)).getBytes());
		}
		else if ((r1.vVal->mValue.mToken == GrapaTokenType::INT || r1.vVal->mValue.mToken == GrapaTokenType::FLOAT) && (r2.vVal->mValue.mToken == GrapaTokenType::INT || r2.vVal->mValue.mToken == GrapaTokenType::FLOAT))
		{
			if (r1.vVal->mValue.mToken == GrapaTokenType::INT)
			{
				a.FromBytes(r1.vVal->mValue);
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, a);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bb.FromBytes(r2.vVal->mValue);
				s32 cmp = 0;
				if (aa < bb) cmp = -1;
				if (aa > bb) cmp = 1;
				result = new GrapaRuleEvent(0, name, ((GrapaInt)(cmp)).getBytes());
			}
			else
			{
				GrapaFloat aa(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				aa.FromBytes(r1.vVal->mValue);
				b.FromBytes(r2.vVal->mValue);
				GrapaFloat bb(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, b);
				s32 cmp = 0;
				if (aa < bb) cmp = -1;
				if (aa > bb) cmp = 1;
				result = new GrapaRuleEvent(0, name, ((GrapaInt)(cmp)).getBytes());
			}
		}
	}
	if (result == NULL)
		result = Error(vScriptExec, pNameSpace, -1);
	return(result);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryRuleTryLockEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		if (objEvent->vWorker->TryCritical())
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", "\1");
		else
		{
			result = new GrapaRuleEvent();
			result->SetNull();
			result->mValue.mToken = GrapaTokenType::BOOL;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleLockEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		objEvent->vWorker->WaitCritical();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}


GrapaRuleEvent* GrapaLibraryRuleUnLockEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		objEvent->vWorker->LeaveCritical();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWaitEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		objEvent->vWorker->WaitCondition();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSignalEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		objEvent->vWorker->SendCondition();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWaitingEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		if (objEvent->vWorker->mWaiting)
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", "\1");
		else
		{
			result = new GrapaRuleEvent();
			result->SetNull();
			result->mValue.mToken = GrapaTokenType::BOOL;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleStartEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent && !objEvent->vWorker->Started())
	{
		err = 0;
		objEvent->vWorker->Run(r2.vVal, r3.vVal, r4.vVal);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleStopEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		objEvent->vWorker->Stop();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleStartedEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		if (objEvent->vWorker->Started())
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", "\1");
		else
		{
			result = new GrapaRuleEvent();
			result->SetNull();
			result->mValue.mToken = GrapaTokenType::BOOL;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSuspendEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = objEvent->vWorker->Suspend();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleResumeEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = objEvent->vWorker->Resume();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleSuspendedEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWorker == NULL)
	{
		objEvent->vWorker = new GrapaWorker();
		objEvent->vWorker->Set(vScriptExec, vScriptExec->vScriptState->GetNameSpace());
	}

	if (objEvent)
	{
		err = 0;
		if (objEvent->vWorker->mSuspended)
			result = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "", "\1");
		else
		{
			result = new GrapaRuleEvent();
			result->SetNull();
			result->mValue.mToken = GrapaTokenType::BOOL;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWidgetNewEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam widgetV(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam xV(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam yV(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	GrapaLibraryParam wV(vScriptExec, pNameSpace, pInput ? pInput->Head(4) : NULL);
	GrapaLibraryParam hV(vScriptExec, pNameSpace, pInput ? pInput->Head(5) : NULL);
	GrapaLibraryParam labelV(vScriptExec, pNameSpace, pInput ? pInput->Head(6) : NULL);
	GrapaLibraryParam attrV(vScriptExec, pNameSpace, pInput ? pInput->Head(7) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		GrapaCHAR widget, label;
		GrapaInt i;
		s64 x = 0, y = 0, w = 0, h = 0;
		if (widgetV.vVal && widgetV.vVal->mValue.mToken == GrapaTokenType::STR) widget.FROM(widgetV.vVal->mValue);
		if (xV.vVal && xV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(xV.vVal->mValue); x = i.LongValue(); }
		if (yV.vVal && yV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(yV.vVal->mValue); y = i.LongValue(); }
		if (wV.vVal && wV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(wV.vVal->mValue); w = i.LongValue(); }
		if (hV.vVal && hV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(hV.vVal->mValue); h = i.LongValue(); }
		if (labelV.vVal && labelV.vVal->mValue.mToken == GrapaTokenType::STR) label.FROM(labelV.vVal->mValue);
		err = objEvent->vWidget->New((char*)widget.mBytes, x, y, w, h, (char*)label.mBytes,attrV.vVal);
		objEvent->vWidget->vEvent = objEvent;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWidgetShowEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		err = objEvent->vWidget->Show();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWidgetHideEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		err = objEvent->vWidget->Hide();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWidgetRedrawEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		err = objEvent->vWidget->Redraw();
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
}

GrapaRuleEvent* GrapaLibraryRuleWidgetResizableEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		if (r2.vVal && r2.vVal->vWidget)
			err = objEvent->vWidget->Resizable(r2.vVal->vWidget);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetResizeEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam xV(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam yV(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam wV(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
	GrapaLibraryParam hV(vScriptExec, pNameSpace, pInput ? pInput->Head(4) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent && objEvent->vWidget && objEvent->vWidget->vWidget)
	{
		GrapaInt i;
		s64 x = objEvent->vWidget->vWidget->x(), y = objEvent->vWidget->vWidget->y(), w = objEvent->vWidget->vWidget->w(), h = objEvent->vWidget->vWidget->h();
		if (xV.vVal && xV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(xV.vVal->mValue); x = i.LongValue(); }
		if (yV.vVal && yV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(yV.vVal->mValue); y = i.LongValue(); }
		if (wV.vVal && wV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(wV.vVal->mValue); w = i.LongValue(); }
		if (hV.vVal && hV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(hV.vVal->mValue); h = i.LongValue(); }
		err = objEvent->vWidget->Resize(x, y, w, h);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetParentEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent && objEvent->vWidget->vWidget && objEvent->vWidget->vWidget->parent())
	{
		GrapaRuleEvent* ch = ((GrapaWidget*)objEvent->vWidget->vWidget->parent()->user_data())->vPtrEvent;
		if (ch)
		{
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::PTR;
			result->vRulePointer = ch;
			result->mVar = true;
			err = 0;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetChildEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent && objEvent->vWidget->vWidget)
	{
		GrapaRuleEvent* ch = NULL;
		if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::STR)
			ch = objEvent->vWidget->Child(r2.vVal->mValue);
		else if (r2.vVal && r2.vVal->mValue.mToken == GrapaTokenType::INT)
		{
			GrapaInt n;
			n.FromBytes(r2.vVal->mValue);
			ch = objEvent->vWidget->Child(n.LongValue());
		}
		else
			ch = objEvent->vWidget->Child();

		if (ch)
		{
			result = new GrapaRuleEvent();
			result->mValue.mToken = GrapaTokenType::PTR;
			result->vRulePointer = ch;
			result->mVar = true;
			err = 0;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetNextEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent && objEvent->vWidget->vWidget && objEvent->vWidget->vWidget->parent())
	{
		err = 0;
		GrapaRuleEvent* we = NULL;
		int pos = objEvent->vWidget->vWidget->parent()->as_group()->find(objEvent->vWidget->vWidget);
		result = new GrapaRuleEvent();
		if ((pos+1) >= objEvent->vWidget->vWidget->parent()->as_group()->children())
		{
			result->SetNull();
		}
		else
		{
			Fl_Widget* xw = objEvent->vWidget->vWidget->parent()->as_group()->child(pos + 1);
			if (xw) 
				we = ((GrapaWidget*)xw->user_data())->vPtrEvent;
			if (we)
			{
				result->mValue.mToken = GrapaTokenType::PTR;
				result->vRulePointer = we;
				result->mVar = true;
			}
			else
				result->SetNull();
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetFocusEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent && objEvent->vWidget->vWidget && objEvent->vWidget->vWidget->parent())
	{
		err = 0;
		GrapaRuleEvent* we = NULL;
		int pos = objEvent->vWidget->vWidget->parent()->as_group()->find(objEvent->vWidget->vWidget);
		result = new GrapaRuleEvent();
		//int cld = objEvent->vWidget->vWidget->parent()->as_group()->children();
		//if ((pos + 1) >= cld)
		//{
		//	result->SetNull();
		//}
		//else
		{
			Fl_Widget* xw = Fl::focus();
			if (xw)
				we = ((GrapaWidget*)xw->user_data())->vPtrEvent;
			if (we)
			{
				result->mValue.mToken = GrapaTokenType::PTR;
				result->vRulePointer = we;
				result->mVar = true;
			}
			else
				result->SetNull();
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetGetEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		//if (r2.vVal->mValue.mToken == GrapaTokenType::STR)
		//{
		//	result = new GrapaRuleEvent();
		//	err = objEvent->vWidget->Get(r2.vVal->mValue, result->mValue);
		//	if (err)
		//	{
		//		delete result;
		//		result = NULL;
		//	}
		//}
		//else if (r2.vVal->mValue.mToken == GrapaTokenType::ARRAY || r2.vVal->mValue.mToken == GrapaTokenType::TUPLE)
		//{
			result = objEvent->vWidget->Get(r2.vVal);
			err = 0;
		//}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

class GrapaWidgetResponse : public GrapaConsoleResponse
{
public:
	GrapaCHAR s;
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
	{
		s.Append((char*)sendbuf,sendbuflen);
	}
	virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
	{
	}
	virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
	{
	}
};

GrapaRuleEvent* GrapaLibraryRuleWidgetSetEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		if (r2.vVal->mValue.mToken == GrapaTokenType::STR)
		{
			GrapaWidgetResponse r;
			u64 size = 0;
			r3.vVal->TOSize(size);
			r.s.SetSize(size);
			r.s.SetLength(0);
			if (r3.vVal->vQueue) vScriptExec->EchoList(&r, r3.vVal, false, false, false);
			else vScriptExec->EchoValue(&r, r3.vVal, false, false, false);
			err = objEvent->vWidget->Set(r2.vVal->mValue, r.s);
		}
		else if (r2.vVal->mValue.mToken == GrapaTokenType::LIST)
		{
			result = objEvent->vWidget->Set(r2.vVal);
			err = 0;
		}
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetHandleEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam eventV(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		GrapaInt i;
		int event = 0, eventR=0;
		if (eventV.vVal && eventV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(eventV.vVal->mValue); event = i.LongValue(); }
		err = objEvent->vWidget->Handle(event, eventR);
		if (err == 0)
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(eventR).getBytes());
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetCallbackEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetEventKeyEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam keyV(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		GrapaInt i;
		int key = 0, keyR = 0;
		if (keyV.vVal && keyV.vVal->mValue.mToken == GrapaTokenType::INT) { i.FromBytes(keyV.vVal->mValue); key = i.LongValue(); }
		err = objEvent->vWidget->EventKey(key, keyR);
		if (err == 0)
			result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaInt(keyR).getBytes());
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetAppendEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam dataV(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent && dataV.vVal)
	{
		err = objEvent->vWidget->Append(dataV.vVal->mValue);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetPostEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
	GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
	GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
	GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		err = objEvent->vWidget->Post(r2.vVal, r3.vVal, r4.vVal);
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};

GrapaRuleEvent* GrapaLibraryRuleWidgetClearEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
	GrapaError err = -1;
	GrapaRuleEvent* result = NULL;

	GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);

	GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
	if (objEvent && objEvent->vWidget == NULL)
	{
		objEvent->vWidget = new GrapaWidget(vScriptExec, pNameSpace);
		objEvent->vWidget->vEvent = objEvent;
	}

	if (objEvent)
	{
		objEvent->vWidget->Clear();
		err = 0;
	}
	if (err && result == NULL)
		result = Error(vScriptExec, pNameSpace, err);

	return(result);
};
