// GrapaState.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaState_
#define _GrapaState_

#include "GrapaObject.h"
#include "GrapaThread.h"
#include "GrapaValue.h"
#include "GrapaFileIO.h"
#include "GrapaEncode.h"
#include "GrapaDatabase.h"
#include "GrapaNet.h"

////////////////////////////////////////////////////////////////////////////////

class GrapaItemParams;
class GrapaScriptExec;
class GrapaLibraryQueue;
class GrapaRuleEvent;
class GrapaRuleQueue;
class GrapaNames;

class GrapaItemEvent : public GrapaEvent
{
public:
	GrapaBYTE mMessage;
public:
	virtual inline GrapaItemEvent* Next() { return((GrapaItemEvent*)mNext); }
};

class GrapaItemState : public GrapaState
{
public:
	GrapaItemParams* mItemParams;
	GrapaCritical mLock;
	bool mClearState;
	bool mFloatFix;
	s64 mFloatMax;
	s64 mFloatExtra;
	GrapaNames* mNameSpace;
	GrapaCHAR mProfile;
public:
	GrapaItemState() { mItemParams = NULL; mClearState = false;  mFloatFix = false; mFloatMax = 16 * 8; mFloatExtra = 10; mNameSpace = NULL; }
	GrapaItemState(GrapaItemParams* pLexParams, GrapaNames* pNameSpace) { mItemParams = NULL; mFloatFix = false; mFloatMax = 16 * 8; mFloatExtra = 10; SetParams(pLexParams, pNameSpace); }
public:
	virtual inline void SetParams(GrapaItemParams* pLexParams, GrapaNames* pNameSpace) { mItemParams = pLexParams; mNameSpace = pNameSpace; }
	virtual void SetParam(u64 pLexParam, const char* pValue, bool pTouched = false);
	virtual GrapaRuleEvent* AddToken(u8 pToken, const char* pName, bool pSkip, const GrapaBYTE& pValue);
	virtual GrapaRuleEvent* SearchToken(u64 pId);
	virtual GrapaRuleEvent* SearchToken(u8 pType, const GrapaBYTE& pValue);
	virtual void PushOutput(u8 pToken, const GrapaBYTE& pValue, char quote);
	virtual void ReplaceTagEscape(GrapaCHAR& pValue);
	virtual void Starting();
	virtual void Running();
	virtual void Stopping();
};

////////////////////////////////////////////////////////////////////////////////

class GrapaLibraryEvent : public GrapaObjectEvent
{
public:
	GrapaLibraryQueue *vLibraryQueue;
	GrapaLibraryEvent() { vLibraryQueue = NULL; };
	virtual ~GrapaLibraryEvent();
public:
	virtual inline GrapaLibraryEvent* Next() { return((GrapaLibraryEvent*)mNext); }
	virtual inline GrapaLibraryEvent* Prev() { return((GrapaLibraryEvent*)mPrev); }
public:
	virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput) { return(pOperation); }
	virtual GrapaRuleEvent* Optimize(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleEvent* pParam) { return(pOperation); }
	virtual GrapaRuleEvent* Error(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaError err);
	virtual void RotateLeft(GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam, const char* v1, const char* v2, const char* v3, const char* v4);
};

class GrapaLibraryQueue : public GrapaObjectQueue
{
public:
	virtual inline GrapaLibraryEvent* Search(GrapaCHAR& pName, s64& pIndex) { return((GrapaLibraryEvent*)GrapaObjectQueue::Search(pName,pIndex)); }
};

class GrapaLibraryParam
{
public:
	GrapaRuleEvent *vVal, *vDel;
	GrapaLibraryParam(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent*v, bool pEval = true);
	virtual ~GrapaLibraryParam();
};

////////////////////////////////////////////////////////////////////////////////

class GrapaRuleEvent : public GrapaObjectEvent
{
public:
	GrapaRuleEvent *vValueEvent;
	GrapaLibraryEvent *vLibraryEvent;
	GrapaRuleEvent *vRuleLambda;
	GrapaRuleEvent *vRuleParent;
	GrapaRuleEvent *vClass;
	bool mAbort, mVar, mLocal, mClass, mConst;
	char mQuote;
	u8 mT;
public:
	GrapaRuleEvent() : GrapaObjectEvent() { INIT(); }
	GrapaRuleEvent(bool setNull) { INIT(); if (setNull) SetNull(); }
	GrapaRuleEvent(const GrapaCHAR& pName) { INIT(); SetItem(0, pName, GrapaBYTE()); }
	GrapaRuleEvent(u64 pTokenId, const GrapaCHAR& pName, const GrapaBYTE& pValue, GrapaObjectEvent* pTokenEvent = NULL);
	GrapaRuleEvent(u8 pToken, u64 pTokenId, const char* pName, const char* pValue = NULL, GrapaObjectEvent* pTokenEvent = NULL);
	virtual ~GrapaRuleEvent() { GrapaRuleEvent::CLEAR(); }
	virtual void CLEAR();
	virtual void SetNull();
	virtual bool IsNull();
	virtual bool IsZero();
	virtual bool IsNeg();
	virtual bool IsNullIsNegIsZero(bool& isNeg, bool& isZero);
private:
	void INIT() {
		vValueEvent = NULL; 
		vRuleLambda = NULL; vLibraryEvent = NULL;  
		mAbort = mVar = mLocal = mClass = mConst = false; vRuleParent = NULL;
		vClass = NULL;
		mQuote = 0;
		mT = GrapaTokenType::START;
	}
public:
	virtual void TOSize(u64& pSize);
	virtual void TO(GrapaBYTE& pValue);
public:
	virtual inline GrapaRuleEvent* Next() { return((GrapaRuleEvent*)GrapaObjectEvent::Next());}
    virtual inline GrapaRuleEvent* Prev() { return((GrapaRuleEvent*)GrapaObjectEvent::Prev());}
};

class GrapaRuleQueue : public GrapaObjectQueue
{
public:
	virtual GrapaRuleEvent* FROM(GrapaScriptState* pScriptState, GrapaNames* pNameSpace, const GrapaBYTE& pValue);
	virtual void TOSize(u64& pSize, u64 pClassNameSize);
	virtual void TO(GrapaBYTE& pValue, GrapaRuleEvent* pClass, u8 pType);
public:
	virtual inline GrapaRuleEvent* Search(u64 pId) { return((GrapaRuleEvent*)GrapaObjectQueue::Search(pId)); }
	virtual inline GrapaRuleEvent* Search(const GrapaCHAR& pName, s64& pIndex) { return((GrapaRuleEvent*)GrapaObjectQueue::Search(pName,pIndex)); }
	virtual inline GrapaRuleEvent* Search(const char* pName, s64& pIndex) { return((GrapaRuleEvent*)GrapaObjectQueue::Search(pName,pIndex)); }
	virtual inline s64 FindIndex(const GrapaCHAR& pName) { return(GrapaObjectQueue::FindIndex(pName)); }
	virtual inline s64 FindIndex(const char* pName) { return(GrapaObjectQueue::FindIndex(pName)); }
	virtual inline GrapaRuleEvent* Head(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaObjectQueue::Head(mSkip)); }
	virtual inline GrapaRuleEvent* Tail(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaObjectQueue::Tail(mSkip)); }
	virtual inline GrapaRuleEvent* PopHead(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaObjectQueue::PopHead(mSkip)); }
	virtual inline GrapaRuleEvent* PopTail(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaObjectQueue::PopTail(mSkip)); }
	virtual inline GrapaRuleEvent* PopEvent(GrapaRuleEvent* pEvent) { return((GrapaRuleEvent*)GrapaObjectQueue::PopEvent(pEvent)); }
	virtual void AppendNames(const char* pNameList, const char* pDelimList);
	virtual void AppendNames(GrapaCHAR& pNameList, GrapaCHAR& pDelimList);
	virtual void ListNames(GrapaCHAR& pNameList, const char pDelim);
};

class GrapaObjectIdQueue : public GrapaRuleQueue
{
public:
	virtual inline GrapaRuleEvent* Search(const GrapaCHAR& pName, s64& pIndex) { return((GrapaRuleEvent*)GrapaRuleQueue::Search(pName,pIndex)); }
	virtual inline GrapaRuleEvent* Search(const char* pName, s64& pIndex) { return((GrapaRuleEvent*)GrapaRuleQueue::Search(pName,pIndex)); }
	virtual inline u64 SearchId(const GrapaCHAR& pName) { return(GrapaRuleQueue::SearchId(pName)); }
	virtual inline u64 SearchId(const char* pName) { return(GrapaRuleQueue::SearchId(pName)); }

	virtual inline GrapaRuleEvent* Search(u8 pToken, const GrapaBYTE& pName) { return (GrapaRuleEvent*)GrapaRuleQueue::Search(HashId(pName, pToken)); }
	virtual inline GrapaRuleEvent* Search(u8 pToken, const char* pName) { return Search(HashId(pName, pToken)); }
	virtual inline GrapaRuleEvent* Search(u64 pId) { return((GrapaRuleEvent*)GrapaRuleQueue::Search(pId)); }

	virtual inline GrapaRuleEvent* Head(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaRuleQueue::Head(mSkip)); }
	virtual inline GrapaRuleEvent* Tail(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaRuleQueue::Tail(mSkip)); }
	virtual inline GrapaRuleEvent* PopHead(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaRuleQueue::PopHead(mSkip)); }
	virtual inline GrapaRuleEvent* PopTail(u64 mSkip = 0) { return((GrapaRuleEvent*)GrapaRuleQueue::PopTail(mSkip)); }
	virtual inline GrapaRuleEvent* PopEvent(GrapaRuleEvent* pEvent) { return((GrapaRuleEvent*)GrapaRuleQueue::PopEvent(pEvent)); }
};	

////////////////////////////////////////////////////////////////////////////////

class GrapaSystemSend
{
public:
	bool isActive;
public:
	GrapaSystemSend() { isActive = true; }
public:
	virtual void GetStr(GrapaCHAR& strBuf);
	virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const char* sendbuf) { Send(vScriptExec,pNameSpace,(u8*)sendbuf, strlen(sendbuf)); }
	virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf) { Send(vScriptExec,pNameSpace,sendbuf.mBytes, sendbuf.mLength);}
	virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen);
	virtual void SendStart(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace) {};
	virtual GrapaRuleEvent* SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pMessage) { return(NULL); };
	virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen) {};
	virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf) {};
	virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue) {};
	virtual void SetActive(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, bool pActive);
protected:
	GrapaQueue mItemQueue;
};

////////////////////////////////////////////////////////////////////////////////

class GrapaConsoleResponse;

class GrapaNames //: public GrapaCritical
{
public:
	bool mSkip;
	GrapaRuleEvent* vLast;
	GrapaNames() { vParentNames = NULL; vLast = NULL; vConsoleResponse = NULL; mSkip = false; }
	virtual ~GrapaNames() 
	{ 
		//WaitCritical();
		vParentNames = NULL;
		if (vLast) { vLast->CLEAR(); delete vLast; vLast = NULL; }
		//LeaveCritical();
	}
	virtual void CLEAR() { mNames.CLEAR(); vLast = NULL;  vParentNames = NULL; vConsoleResponse = NULL; }
	GrapaRuleEvent* This()
	{
		//WaitCritical();
		GrapaNames* x = this;
		while (x && x->mSkip) x = x->GetParrent();
		if (x)
		{
			GrapaRuleEvent* n = x->mNames.Tail();
			while (n && n->vRulePointer == NULL) n = n->Prev();
			if (n == NULL) n = x->mNames.Head();
			while (n->vRulePointer) n = n->vRulePointer;
			//LeaveCritical();
			return(n);
		}
		//LeaveCritical();
		return(NULL);
	}
	virtual void SetParrent(GrapaNames* pParrent) { UpdateParrent(pParrent); SetResponse(pParrent ? pParrent->vConsoleResponse : NULL); }
	virtual void UpdateParrent(GrapaNames* pParrent) { vParentNames = pParrent; }
	virtual GrapaNames* GetParrent() { return vParentNames; }
	virtual GrapaNames* GetSkipThis() { GrapaNames* x = this; while (x && x->mSkip) x = x->GetParrent(); return(x); }
	virtual GrapaNames* GetSkipParrent() { GrapaNames* x = vParentNames; while (x && x->mSkip) x = x->GetParrent(); return(x); }
	virtual void SetResponse(GrapaConsoleResponse* pResponse) { vConsoleResponse = pResponse; }
	virtual GrapaConsoleResponse* GetResponse() { return vConsoleResponse; }
	virtual GrapaRuleQueue* GetThisQueue() {
		return &vParentNames->mNames;
	}
	virtual GrapaRuleQueue* GetNameQueue() {
			GrapaNames* x = GetSkipThis();
		if (x)
			return &x->mNames;
		else
			return NULL;
	}
private:
	GrapaNames* vParentNames;
	GrapaConsoleResponse* vConsoleResponse;
	GrapaRuleQueue mNames;
};


////////////////////////////////////////////////////////////////////////////////

class GrapaScriptExec
{
public:
	GrapaScriptState* vScriptState;

public:
	GrapaScriptExec() { vScriptState = NULL; }
	GrapaScriptExec(GrapaScriptState* pScriptState) { vScriptState = pScriptState; }

public:
	GrapaRuleEvent* Search(GrapaRuleQueue* pParameterQueue, u64 pTokenId);

	GrapaRuleEvent* PlanRule(GrapaNames* pNameSpace, GrapaRuleEvent* pTokenEvent, GrapaRuleEvent* pRuleEvent, GrapaRuleQueue* pOperationQueue, bool& pMatched, GrapaRuleEvent* pLastNext, bool& pAddLastNext, s8 pLeftRecursion, GrapaKeyValue& pCache, bool pTouched);
	
	void LoadLib(GrapaRuleEvent *libName);
	GrapaRuleEvent* ProcessPlan(GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam=NULL, u64 pCount=0);
	GrapaRuleEvent* PlanProcess(GrapaNames* pNameSpace, GrapaRuleQueue* pOperationQueue);

	//GrapaError GetParameterKey(GrapaNames* pNameSpace, GrapaRuleEvent* pInputParam, GrapaPublicKey& pKey);

	static GrapaRuleEvent* CopyItem(GrapaRuleEvent* pAction, bool isTAG = false, bool isConst = false);
	static GrapaRuleQueue* CopyQueue(GrapaRuleQueue* pList, bool isTAG = false, bool isConst = false);

	void ReplaceLocalQueue(GrapaRuleQueue* pList, GrapaRuleQueue* pLocal);

	void EscapeString(const GrapaCHAR& pFrom, GrapaCHAR& pTo);
	void EchoString(GrapaSystemSend* pSend, GrapaCHAR& pStr);
	void Echo(GrapaSystemSend* pSend, GrapaObjectEvent* pTokenEvent, GrapaCHAR& pStr);
	void EchoValue(GrapaSystemSend* pSend, GrapaRuleEvent* pTokenEvent, bool elMode, bool strquote, bool fromlist);
	void EchoDB(GrapaSystemSend* pSend, GrapaFile& pValue);
	void EchoList(GrapaSystemSend* pSend, GrapaRuleEvent* pTokenEvent, bool elMode, bool parentHasEl, bool fromEcho);
	void EchoTokenValue(GrapaSystemSend* pSend, GrapaRuleEvent* pEvent, bool elMode, bool pretty);
	void EchoRuleValue(GrapaSystemSend* pSend, GrapaRuleEvent* pEvent, bool elMode, bool pretty, bool noHeader);
	void EchoClassValue(GrapaSystemSend* pSend, GrapaRuleEvent* pEvent, bool elMode, bool pretty);

	GrapaRuleEvent* Exec(GrapaNames* pNameSpace, GrapaRuleEvent* pRule, u64 pRuleId, GrapaCHAR pProfile, GrapaCHAR& pValue);
	GrapaRuleEvent* Plan(GrapaNames* pNameSpace, GrapaRuleEvent* pOperation);
	GrapaRuleEvent* Plan(GrapaNames* pNameSpace, GrapaCHAR& pInput, GrapaRuleEvent* pRule, u64 pRuleId, GrapaCHAR pProfile);

	GrapaRuleEvent* Exec(GrapaNames* pNameSpace, GrapaRuleEvent* pRule, u64 pRuleId, GrapaRuleEvent* token, GrapaRuleEvent** resultPtr);

};

////////////////////////////////////////////////////////////////////////////////

class GrapaScriptExecState : public GrapaState
{
public:
	GrapaItemState mItemState;
	GrapaQueue mItemQueue;
	GrapaScriptExec *vScriptExec;
	//GrapaRuleEvent* vResult;
	bool mEnablePrompt,mNeedsPrompt;
	u64 mRuleId;
	bool mClearState;
public:
	GrapaScriptExecState();
	virtual ~GrapaScriptExecState();
public:
	virtual void LoadSystem() {}
	virtual void EnablePrompt(GrapaNames* pNameSpace, bool pEnablePrompt = true);
	virtual void SendPrompt(GrapaNames* pNameSpace) {};
	virtual void StartResult(GrapaNames* pNameSpace) {};
	virtual void EndResult(GrapaNames* pNameSpace, GrapaRuleEvent* pResult) {};
	virtual void UpdateResult(GrapaNames* pNameSpace, GrapaRuleEvent* pResult);
	virtual GrapaError Start();
	virtual void Starting();
	virtual void Running();
	virtual void Stopping();
};

class GrapaItemEnum
{
public:
	enum { SYM = 0, BIN, OCT, DIG, HEX, ID, STR, STRESC, STRFROM, STRTO, SYS, EOL, MAX };
};

class GrapaItemParams
{
public:
	GrapaObjectIdQueue mMapQueue;
	char mParam[GrapaItemEnum::MAX][256];
	bool mTouched[GrapaItemEnum::MAX];
public:
	GrapaItemParams() 
	{ 
		for (u64 i = 0; i<GrapaItemEnum::MAX; i++) mParam[i][0] = 0; 
		for (u64 i = 0; i<GrapaItemEnum::MAX; i++) mTouched[i] = false; 
		memset(mParam, 0, sizeof(mParam));
		memset(mTouched, 0, sizeof(mTouched));
	}
};

class GrapaConsoleSend;
class GrapaConsoleResponse;

class GrapaScriptState : public GrapaScriptExecState
{
public:
	GrapaItemParams mItemParams;
	GrapaRuleQueue mRuleStartQueue;
	GrapaNames* vRuleVariables;
	//GrapaRuleQueue mClass;

	GrapaLocalDatabase mPath;
	GrapaRuleEvent mNet;

	GrapaConsoleSend* vConsoleSend;

public:
	GrapaScriptState();
	virtual ~GrapaScriptState();
public:
	virtual void CLEAR();
	virtual void LoadSystem();
	virtual void ClearQueue();

	virtual void SetNameSpace(GrapaNames* pRuleVariables);
	virtual GrapaNames* GetNameSpace() { return vRuleVariables; };

	virtual void SetTokenParam(u64 pParam, const char* pValue, bool pTouched=false);
	virtual inline void SetQueue(GrapaQueue* pQueue) { mQueue = pQueue; mItemState.SetOutput(pQueue); }
	virtual inline GrapaRuleQueue* GetQueue() { return((GrapaRuleQueue*)GrapaState::GetQueue()); }

	//virtual GrapaRuleQueue* GetTokens(GrapaRuleQueue* pTokenQueue, const char* pValue, u64 pLen);
	//virtual GrapaRuleQueue* GetTokens(GrapaRuleQueue* pTokenQueue, GrapaCHAR& pValue);
	virtual GrapaRuleEvent* AddToken(u8 pToken, const char* pName, bool pSkip, const char* pValue);
	virtual GrapaRuleEvent* AddToken(u8 pToken, const char* pName, bool pSkip, GrapaBYTE& pValue);
	virtual GrapaError DelToken(GrapaRuleQueue* pRuleQueue, const char* pName);

	virtual GrapaRuleEvent* AddRule(GrapaRuleQueue* pRuleQueue, const char* pName);
	virtual GrapaRuleEvent* GetRule(GrapaRuleQueue* pRuleQueue, const char* pName);
	virtual GrapaRuleEvent* AddRuleOperation(GrapaRuleQueue* pRuleEvent, const char* pName, const char* pValue, s64 a = -1, s64 b = -1, s64 c = -1, s64 d = -1);
	virtual GrapaRuleEvent* AddRuleOperation2(GrapaRuleQueue* pRuleEvent, const char* pName, const char* pValue, const char* a = NULL, s64 b = -1);

	virtual GrapaRuleEvent* AddRuleParameter(GrapaRuleEvent* pOperationEvent, const char* pName);
	virtual GrapaRuleEvent* AddTokenParameter(GrapaRuleEvent* pOperationEvent, const char* pName);
	virtual GrapaRuleEvent* AddRawParameter(GrapaRuleEvent* pOperationEvent, u8 pToken);
	virtual GrapaRuleEvent* AddRawParameter(GrapaRuleEvent* pOperationEvent, u8 pToken, const char* pName, const char* pValue);
	virtual GrapaRuleEvent* AddRawParameter(GrapaRuleEvent* pOperationEvent, const GrapaCHAR& pName, const GrapaBYTE& pValue);

	virtual GrapaRuleEvent* GetClass(GrapaNames* pNameSpace, const GrapaCHAR &s);
	//virtual void SetupClasses(GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
	virtual GrapaRuleEvent* FindClassVar(GrapaNames* pNameSpace, GrapaRuleEvent *r, const GrapaCHAR &s);
	virtual void CopyClassVars(GrapaRuleQueue *q, GrapaRuleEvent *r);
	virtual GrapaRuleEvent* SearchVariable(GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
	virtual GrapaRuleEvent* SearchVariable(GrapaNames* pNameSpace, const GrapaCHAR& s);
	virtual GrapaRuleEvent* SearchTarget(GrapaNames* pNameSpace, GrapaRuleEvent *pValue);
	virtual GrapaRuleEvent* SearchFileVariable(GrapaNames* pNameSpace, GrapaRuleEvent *pValue);
	//virtual GrapaRuleEvent* SearchNetVariable(GrapaNames* pNameSpace, GrapaRuleEvent *pValue);

	//virtual GrapaRuleEvent* CreateVariable(GrapaNames* pNameSpace, const GrapaCHAR& pName, bool& wasCreated);
	//virtual GrapaRuleEvent* SetVariable(GrapaNames* pNameSpace, const GrapaCHAR& pName, const GrapaBYTE& pValue);

	virtual void StartResult(GrapaNames* pNameSpace);
	virtual void EndResult(GrapaNames* pNameSpace, GrapaRuleEvent* pResult);
	virtual void UpdateResult(GrapaNames* pNameSpace, GrapaRuleEvent* pResult);
	virtual void SendPrompt(GrapaNames* pNameSpace);

	virtual void Running();
	virtual void Stopping();
};

class GrapaWorker : public GrapaThread
{
public:
	GrapaRuleEvent mWorkerEvent;
	GrapaNames mRuleVariables;
	GrapaRuleEvent *vRunCode, *vDoneCode, *vResult;
	GrapaRuleEvent *vParam;
	GrapaScriptState mScriptState;
	GrapaScriptExec mScriptExec;
	GrapaWorker()
	{
		vRunCode = NULL;
		vParam = NULL;
		vRunCode = NULL;
		vDoneCode = NULL;
		vResult = NULL;
		mScriptExec.vScriptState = &mScriptState;
		mScriptState.vScriptExec = &mScriptExec;
		mScriptState.vRuleVariables = &mRuleVariables;
		mWorkerEvent.mValue.mToken = GrapaTokenType::OBJ;
		mWorkerEvent.vWorker = this;
		mWorkerEvent.vQueue = new GrapaRuleQueue();
		mRuleVariables.GetNameQueue()->PushTail(&mWorkerEvent);
	}
	~GrapaWorker() 
	{ 
		mWorkerEvent.vWorker = NULL;
		mRuleVariables.GetNameQueue()->PopEvent(&mWorkerEvent);
		if (vRunCode)
		{
			vRunCode->CLEAR();
			delete vRunCode;
		}
		if (vDoneCode)
		{
			vDoneCode->CLEAR();
			delete vDoneCode;
		}
		if (vParam)
		{
			vParam->CLEAR();
			delete vParam;
		}
		if (vResult)
		{
			vResult->CLEAR();
			delete vResult;
		}
	}
	void Set(GrapaScriptExec* pTokenExec, GrapaNames* pNameSpace)
	{
		mWorkerEvent.vClass = mScriptState.GetClass(pNameSpace, GrapaCHAR("$thread"));
		mScriptState.Stop();
		mRuleVariables.SetParrent(pNameSpace);
		mScriptState.vConsoleSend = pTokenExec->vScriptState->vConsoleSend;
		mScriptState.Start();
	}
	void Run(GrapaRuleEvent *pRunCode, GrapaRuleEvent*pParam, GrapaRuleEvent *pDoneCode)
	{
		if (vRunCode)
		{
			vRunCode->CLEAR();
			delete vRunCode;
			vRunCode = NULL;
		}
		if (pRunCode)
		{
			vRunCode = mScriptExec.CopyItem(pRunCode);
		}

		if (vDoneCode)
		{
			vDoneCode->CLEAR();
			delete vDoneCode;
			vDoneCode = NULL;
		}
		if (pDoneCode)
			vDoneCode = mScriptExec.CopyItem(pDoneCode);
		
		if (vParam)
		{
			vParam->CLEAR();
			delete vParam;
			vParam = NULL;
		}
		if (pParam)
			vParam = mScriptExec.CopyItem(pParam);

		if (vResult)
		{
			vResult->CLEAR();
			delete vResult;
			vResult = NULL;
		}

		Start(false);
	}
	virtual void Starting() {}
	virtual void Stopping()
	{
		GrapaRuleEvent *result=NULL;
		if (vDoneCode)
		{
			GrapaRuleQueue p;
			GrapaRuleEvent* p1 = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			p.PushTail(p1);
			p1->mValue.mToken = GrapaTokenType::PTR;
			p1->vRulePointer = vParam;
			GrapaRuleEvent* p2 = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			p.PushTail(p2);
			p2->mValue.mToken = GrapaTokenType::PTR;
			p2->vRulePointer = vResult;
			result = mScriptExec.ProcessPlan(&mRuleVariables, vDoneCode, p1);
		}
		if (result)
		{
			result->CLEAR();
			delete result;
		}
	}
	virtual void Running()
	{
		SendCondition();
		if (vResult)
		{
			vResult->CLEAR();
			delete vResult;
			vResult = NULL;
		}
		GrapaRuleQueue p;
		GrapaRuleEvent* p1 = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
		p.PushTail(p1);
		p1->mValue.mToken = GrapaTokenType::PTR;
		p1->vRulePointer = vParam;
		vResult = mScriptExec.ProcessPlan(&mRuleVariables, vRunCode, p1);
	}
};

////////////////////////////////////////////////////////////////////////////////

#endif //_GrapaState_

////////////////////////////////////////////////////////////////////////////////

