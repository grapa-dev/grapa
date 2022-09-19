// GrapaLibRule.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaLibRule_
#define _GrapaLibRule_

#include "GrapaState.h"

////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryRuleEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRuleEvent() { mName.FROM(""); };
	GrapaLibraryRuleEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual void LoadLibWrap(GrapaScriptExec* vScriptExec, GrapaRuleEvent* pLib);
	virtual GrapaLibraryEvent* LoadLib(GrapaScriptExec* vScriptExec, GrapaRuleEvent* pLib, GrapaCHAR& pName);
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam);
};

#endif //_GrapaLibRule_

////////////////////////////////////////////////////////////////////////////////
