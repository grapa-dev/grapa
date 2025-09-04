// GrapaObject.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaObject_
#define _GrapaObject_

#include "GrapaValue.h"

class GrapaObjectQueue;
class GrapaRuleEvent;
class GrapaRuleQueue;
class GrapaLocalDatabase;
class GrapaNetConnect;
class GrapaWorker;
class GrapaVector;
class GrapaWidget;

class GrapaObjectEvent : public GrapaEvent
{
public:
	u64 mId;
	GrapaCHAR mName;
	GrapaCHARFile mValue;
	GrapaRuleQueue *vQueue;
	GrapaRuleEvent *vRulePointer;
	GrapaLocalDatabase *vDatabase;
	GrapaNetConnect* vNetConnect;
	GrapaVector* vVector;
	GrapaWorker* vWorker;
	GrapaWidget* vWidget;
	bool mSkip, mRun, mStart, mEnd, mEscape, mExit, mNull, mTouch;
	u8 mDirType;
public:
	GrapaObjectEvent() : GrapaEvent() { INIT(); }
	GrapaObjectEvent(const char* pName) { INIT(); SetItem(0, GrapaCHAR(pName), GrapaBYTE()); }
	GrapaObjectEvent(const GrapaCHAR& pName) { INIT(); SetItem(0, pName, GrapaBYTE()); }
	GrapaObjectEvent(u64 pId, const GrapaCHAR& pName, const GrapaBYTE& pValue) { INIT(); SetItem(pId, pName, pValue); }
	virtual ~GrapaObjectEvent() { GrapaObjectEvent::CLEAR(); }
	void CLEAR();
private:
	void INIT();
public:
	virtual void SetItem(u64 pId, const GrapaCHAR& pName, const GrapaBYTE& pValue);
	virtual inline GrapaObjectEvent* Next() { return((GrapaObjectEvent*)GrapaEvent::Next()); }
	virtual inline GrapaObjectEvent* Prev() { return((GrapaObjectEvent*)GrapaEvent::Prev()); }
	static u64 HashId(const char* pValue, u64 pLen, u8 pType);
	static u64 HashId(const GrapaBYTE& pValue, u8 pType);
	virtual u64 HashId(u8 pType);
	virtual u64 HashIdUpper(u8 pType);
};

class GrapaObjectQueue : public GrapaQueue
{
public:
	virtual GrapaObjectEvent* Search(const char* pName, s64& pIndex);
	virtual GrapaObjectEvent* Search(const GrapaCHAR& pName, s64& pIndex);
	virtual GrapaObjectEvent* SearchLower(const char* pName, s64& pIndex);
	virtual GrapaObjectEvent* SearchLower(const GrapaCHAR& pName, s64& pIndex);
	virtual GrapaObjectEvent* SearchCase(const GrapaCHAR& pName, s64& pIndex, bool pLower);
	virtual GrapaObjectEvent* Search(u64 pId);
	virtual u64 SearchId(const char* pName);
	virtual u64 SearchId(const GrapaCHAR& pName);
	virtual s64 FindIndex(const char* pValue);
	virtual s64 FindIndex(const GrapaCHAR& pValue);
	virtual inline GrapaObjectEvent* Head(u64 mSkip = 0) { return((GrapaObjectEvent*)GrapaQueue::Head(mSkip)); }
	virtual inline GrapaObjectEvent* Tail(u64 mSkip = 0) { return((GrapaObjectEvent*)GrapaQueue::Tail(mSkip)); }
	virtual inline GrapaObjectEvent* PopHead(u64 mSkip = 0) { return((GrapaObjectEvent*)GrapaQueue::PopHead(mSkip)); }
	virtual inline GrapaObjectEvent* PopTail(u64 mSkip = 0) { return((GrapaObjectEvent*)GrapaQueue::PopTail(mSkip)); }
	virtual inline GrapaObjectEvent* PopEvent(GrapaObjectEvent* pEvent) { return((GrapaObjectEvent*)GrapaQueue::PopEvent(pEvent)); }
	virtual u64 HashId(const char* pValue, u64 pLen, u8 pType);
	virtual u64 HashId(const GrapaBYTE& pValue, u8 pType);
	virtual u64 HashIdUpper(const GrapaBYTE& pValue, u8 pType);
};

class GrapaObjectIndexQueue : public GrapaObjectQueue
{
public:
	GrapaObjectIndexQueue();
	virtual ~GrapaObjectIndexQueue() { GrapaObjectEvent* head = PopHead();  while (head) { GrapaCursor c; c.Set(mRoot, GrapaBtree::SU64_ITEM, head->mId); mBtree.Delete(c); head->CLEAR(); delete head; head = PopHead(); }; mBtree.SetFile();}
public:
	virtual inline GrapaObjectEvent* Search(const GrapaCHAR& pName, s64& pIndex) { return((GrapaObjectEvent*)GrapaObjectQueue::Search(pName,pIndex)); }
	virtual inline GrapaObjectEvent* Search(const char* pName, s64& pIndex) { return((GrapaObjectEvent*)GrapaObjectQueue::Search(pName,pIndex)); }
	virtual inline u64 SearchId(const GrapaCHAR& pName) { return(GrapaObjectQueue::SearchId(pName)); }
	virtual inline u64 SearchId(const char* pName) { return(GrapaObjectQueue::SearchId(pName)); }

	virtual GrapaObjectEvent* Search(u8 pToken, const GrapaBYTE& pName);
	virtual GrapaObjectEvent* Search(u8 pToken, const char* pName);
	virtual GrapaObjectEvent* Search(u64 pId);

	virtual void PushHead(GrapaObjectEvent* pEvent);
	virtual void PushTail(GrapaObjectEvent* pEvent);
	virtual void PushNext(GrapaObjectEvent* pLocation, GrapaObjectEvent* pEvent);

	virtual inline GrapaObjectEvent* Head(u64 mSkip = 0) { return((GrapaObjectEvent*)GrapaObjectQueue::Head(mSkip)); }
	virtual inline GrapaObjectEvent* Tail(u64 mSkip = 0) { return((GrapaObjectEvent*)GrapaObjectQueue::Tail(mSkip)); }
	virtual inline GrapaObjectEvent* PopHead(u64 mSkip = 0) { GrapaObjectEvent *e = (GrapaObjectEvent*)GrapaObjectQueue::PopHead(mSkip);  if (e) { GrapaCursor c; c.Set(mRoot, GrapaBtree::SU64_ITEM, e->mId); mBtree.Delete(c); } return(e); }
	virtual inline GrapaObjectEvent* PopTail(u64 mSkip = 0) { GrapaObjectEvent *e = (GrapaObjectEvent*)GrapaObjectQueue::PopTail(mSkip); if (e) { GrapaCursor c; c.Set(mRoot, GrapaBtree::SU64_ITEM, e->mId); mBtree.Delete(c); } return(e); }
	virtual inline GrapaObjectEvent* PopEvent(GrapaObjectEvent* pEvent) { GrapaObjectEvent *e = (GrapaObjectEvent*)GrapaObjectQueue::PopEvent(pEvent); if (e) { GrapaCursor c; c.Set(mRoot, GrapaBtree::SU64_ITEM, e->mId); mBtree.Delete(c); } return(e); }

	//virtual void PrintList();
public:
	GrapaBtree mBtree;
	u64 mRoot;
	GrapaCHARFile mCache;
};

class GrapaCoreCHAR : public GrapaBtree
{
public:
	virtual GrapaError CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result);
};

class GrapaKeyValue
{
public:
	GrapaKeyValue();
	~GrapaKeyValue() { mBtree.SetFile(); }
	void Set(GrapaRuleEvent* pValue);
	GrapaRuleEvent* Get(const GrapaCHAR& pKey);
private:
	GrapaCoreCHAR mBtree;
	u64 mRoot;
	GrapaCHARFile mCache;
};

////////////////////////////////////////////////////////////////////////////////

#endif // _AckuObject_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
