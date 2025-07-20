// GrapaValue.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaValue_
#define _GrapaValue_

#include "GrapaThread.h"
#include "GrapaFile.h"
#include "GrapaBtree.h"

class GrapaCHAR;
class GrapaSS64;
class GrapaSU64;
class GrapaDU64;
class GrapaGUID;
class GrapaBYTE;
class GrapaRuleEvent;

class GrapaTokenType {
public: enum {
	START = 0,
	ERR = 1,
	RAW = 2,
	BOOL = 3,
	ID = 4,
	STR = 5,
	INPUT = 6,
	XML = 7,
	INT = 8,
	FLOAT = 9,
	SYM = 10,
	SYSSYM = 11,
	TIME = 12,
	ARRAY = 13,
	LIST = 14,
	EL = 15,
	TAG = 16,
	TABLE = 17,
	SYSID = 18,
	SYSSTR = 19,
	SYSINT = 20,
	OP = 21,
	CODE = 22,
	VECTOR = 23,
	CLASS = 24,
	OBJ = 25,
	REF = 26,
	RULE = 27,
	TOKEN = 28,
	WIDGET = 29,
	TUPLE = 30,
	X1 = 128,  // don't change above numbers
	OTHER,
	PTR,
	RULEREF,
	RULEOP,
	MAX
};
};

class GrapaValue
{
public:
	u8 mToken;
};

class GrapaBYTE : public GrapaValue
{
public:
	u8* mBytes;
	u64 mSize;
	u64 mLength;
	u64 mPos;
	u64 mGrow;
public:
	GrapaBYTE();
	GrapaBYTE(u64 pSize, const void *pValue = NULL);
	GrapaBYTE(const void *pValue, u64 pSize);
	GrapaBYTE(const char *pStr);
	GrapaBYTE(const GrapaBYTE& pBlob);
	~GrapaBYTE();
	GrapaBYTE& operator=(const GrapaBYTE& that);
	GrapaBYTE& operator=(const GrapaCHAR& that);
protected:
	void INIT();
public:
	virtual void FROM(u64 pSize, const void *pValue);
	virtual void FROM(const GrapaBYTE& pValue);
	virtual void FROM(const char *pStr);
	virtual void ToHex(GrapaBYTE& pValue);
	virtual void FromHex(GrapaBYTE& pValue);
	virtual void ToRaw(GrapaBYTE& pValue);
	virtual GrapaCHAR ToStr();
public:
	virtual void* GetPtr();
	virtual void SetNull();
	virtual u64 GetLength();
	virtual void SetLength(u64 pLength, bool pCopy = true);
	virtual void SetSize(u64 pSize, bool pCopy=true);
	virtual u64 GetSize();
	virtual void GrowSize(u64 pSize, bool pCopy=true);
	virtual void Append(const GrapaCHAR& pValue);
	virtual void Append(const GrapaBYTE& pValue);
	virtual void Append(void *pValue, u64 pSize);
	virtual void Append(const u8 pValue);
	virtual s64 Cmp(const GrapaBYTE& pValue);
	virtual s64 Cmp(const char* pStr);
	virtual void Pad(u64 pSize, u8 pValue = 0);
	virtual u8 ToDbType();
	virtual void FromDbType(u8 pType);
};

class GrapaCHAR : public GrapaBYTE
{
public:
	GrapaCHAR();
	GrapaCHAR(const char *pStr, u64 pLen);
	GrapaCHAR(const GrapaBYTE& pBlob);
	GrapaCHAR(s64 pNum);
	GrapaCHAR(u64 pId);
	GrapaCHAR(du64 pId);
	GrapaCHAR& operator=(const GrapaCHAR& that);
	GrapaCHAR& operator=(const GrapaBYTE& that);
private:
	void INIT();
public:
	virtual void FROM(const char *pStr);
	virtual void FROM(const char* pStr, u64 pLen);
	virtual void FROM(s64 pNum);
	virtual void FROM(u64 pId);
	virtual void FROM(du64 pId);
	virtual void FROM(const GrapaCHAR& pChar);
	virtual void FROM(const GrapaBYTE& pValue);
public:
	virtual void SetLength(u64 pLength, bool pCopy=true);
	virtual void SetSize(u64 pSize, bool pCopy=true);
	virtual void Append(const char* pStr, u64 pLen);
	virtual void Append(const char* pStr);
	virtual void Append(const char pChar);
	virtual void Append(const GrapaBYTE& pChar);
	virtual void Append(s64 pNum);
	virtual void Append(u64 pId);
	virtual void AppendQuoted(GrapaCHAR& pStr, bool pIsOptional);
	virtual void EscapeValue(GrapaCHAR &pTo, char* e);
	virtual s64 StrNCmp(const char* pStr, u64 pLen);
	virtual s64 StrNCmp(const char* pStr);
	virtual s64 StrCmp(const char* pStr);
    virtual s64 StrCmp(const GrapaBYTE& pStr);
    virtual s64 StrCmp(const GrapaCHAR& pStr);
	virtual s64 StrUpperCmp(const char* pStr, u64 pLen = 0);
	virtual s64 StrUpperCmp(const GrapaBYTE& pStr);
	virtual s64 StrLowerCmp(const char* pStr, u64 pLen = 0);
	virtual s64 StrLowerCmp(const GrapaBYTE& pStr);
	virtual void ToUpper();
	virtual void ToLower();
	virtual void RTrim(const char c = ' ');
	virtual void LTrim(const char c = ' ');
	virtual void Trim(const char c = ' ');
	virtual void RTrim(GrapaCHAR& s);
	virtual void LTrim(GrapaCHAR& s);
	virtual void Trim(GrapaCHAR& s);
	virtual void RTrim2(GrapaRuleEvent* s);
	virtual void LTrim2(GrapaRuleEvent* s);
	virtual void Trim2(GrapaRuleEvent* s);
	virtual void Pad(u64 pSize, u8 pValue = ' ');
	virtual void Replace(const GrapaBYTE& pFrom, const GrapaBYTE& pTo);
	virtual GrapaCHAR ToBase(u32 radix);
	virtual GrapaCHAR FromBase(u32 radix);
	static GrapaCHAR SetBool(bool isTrue) { GrapaCHAR r("\0"); if (isTrue) r.FROM("\1");  r.mToken = GrapaTokenType::BOOL; return(r); };
};

class GrapaSS64 : public GrapaValue
{
public:
	s64 mNum;
public:
	GrapaSS64();
public:
	virtual void TO(GrapaCHAR& pChar);
};

class GrapaSU64 : public GrapaValue
{
public:
	u64 mNum;
public:
	GrapaSU64();
public:
	virtual void TO(GrapaCHAR& pChar);
};

class GrapaDU64 : public GrapaValue
{
public:
	du64 mNum;
public:
	GrapaDU64();
	GrapaDU64(du64 pNum);
public:
	virtual void TO(GrapaCHAR& pChar);
};

class GrapaVoidArray : public GrapaBYTE
{
public:
	GrapaVoidArray(u32 pCount=0);
	~GrapaVoidArray();
public:
	virtual void Init(u32 pCount=0);
	virtual void Append(void *pItem);
	virtual u32 Count();
	void* GetAt(u32 i);
};

class GrapaCHARArray : public GrapaVoidArray
{
public:
	GrapaCHARArray(u32 pCount=0);
	~GrapaCHARArray();
public:
	virtual void Append(char *pStr);
};

class GrapaDU64Array : public GrapaVoidArray
{
public:
	GrapaDU64Array(u32 pCount=0);
	~GrapaDU64Array();
public:
	virtual void Append(du64 pNum);
	virtual void Append(u64 key, u64 value);
};

class GrapaArray64 : public GrapaBYTE
{
public:
	GrapaArray64();
	GrapaArray64& operator=(const GrapaArray64& that);
private:
	void INIT();
public:
	virtual inline u64 GetCount();
	virtual inline void SetCount(u64 pCount);
	virtual inline void GrowCount(u64 pCount);
	virtual inline void SetItem(u64 pItem, u64 pValue);
	virtual inline u64 GetItem(u64 pItem);
};

////////////////////////////////////////////////////////////////////////////////

class GrapaEvent;
class GrapaState;

class GrapaQueue : public GrapaCritical
{
public:
	u64 mCount;
	GrapaState* mState;
	GrapaCritical mLock;
public:
	GrapaQueue() { mCount = 0L; mHead = mTail = 0L; mState = 0L; }
	virtual ~GrapaQueue() { GrapaQueue::CLEAR(); }
	virtual void CLEAR();
public:
	virtual void PushHead(GrapaEvent* pEvent);
	virtual void PushTail(GrapaEvent* pEvent);
	virtual void PushNext(GrapaEvent* pLoc, GrapaEvent* pEvent);
	virtual void Push(GrapaEvent* pEvent, GrapaEvent* pLoc = NULL);
	virtual GrapaEvent* Head(u64 mSkip = 0);
	virtual GrapaEvent* Tail(u64 mSkip = 0);
	virtual GrapaEvent* PopHead(u64 mSkip = 0);
	virtual GrapaEvent* PopTail(u64 mSkip = 0);
	virtual GrapaEvent* PopEvent(GrapaEvent* pEvent);
	virtual bool HasEvent(GrapaEvent* pEvent);
	virtual void ReplaceEvent(GrapaEvent* pLoc, GrapaEvent* pEvent);
protected:
	GrapaEvent* mHead, * mTail;
};

class GrapaEvent
{
public:
	GrapaEvent() : mNext(NULL), mPrev(NULL), mQueue(NULL) {}
	virtual ~GrapaEvent() { GrapaEvent::CLEAR(); }
	virtual void CLEAR() {}
public:
	virtual inline void SetNext(GrapaEvent* pNext) { mNext = pNext; }
	virtual inline void SetPrev(GrapaEvent* pPrev) { mPrev = pPrev; }
	virtual inline void SetQueue(GrapaQueue* pQueue) { mQueue = pQueue; }
	virtual inline GrapaEvent* Next() { return(mNext); }
	virtual inline GrapaEvent* Prev() { return(mPrev); }
	virtual inline GrapaQueue* Queue() { return(mQueue); }
protected:
	GrapaEvent *mNext, *mPrev;
	GrapaQueue *mQueue;
};


class GrapaState : public GrapaThread
{
public:
	GrapaState() : mInput(NULL), mQueue(NULL), mOutput(NULL) {}
	virtual ~GrapaState() { Stop(); GrapaState::CLEAR(); }
	virtual void CLEAR() {}
public:
	virtual inline void SetInput(GrapaQueue* pInput) { mInput = pInput; }
	virtual inline void SetQueue(GrapaQueue* pQueue) { mQueue = pQueue; }
	virtual inline void SetOutput(GrapaQueue* pOutput) { mOutput = pOutput; }
	virtual inline GrapaQueue* GetQueue() { return(mQueue); }
	virtual inline void PushInputTail(GrapaEvent* pEvent) { if (mInput) mInput->PushTail(pEvent); }
	virtual inline void PushOutputTail(GrapaEvent* pEvent) { if (mOutput) mOutput->PushTail(pEvent); }
protected:
	GrapaQueue *mInput;
	GrapaQueue *mQueue;
	GrapaQueue *mOutput;
};

////////////////////////////////////////////////////////////////////////////////

class GrapaWorkQueue;

class GrapaWorkEvent : public GrapaEvent, public GrapaThread
{
public:
	GrapaWorkEvent() { vQueue = NULL; }
	virtual void Set(GrapaWorkQueue*pQueue);
	virtual void Starting();
	virtual void Stopping();
protected:
	GrapaWorkQueue*vQueue;
};

class GrapaWorkQueue : public GrapaQueue
{
public:
	virtual GrapaError Start();
};

////////////////////////////////////////////////////////////////////////////////

class GrapaCHARFile : public GrapaCHAR, public GrapaFile
{
public:
	GrapaCHARFile();
public:
	virtual bool    Opened();
	virtual GrapaError Open(const char *fileName, char mode = GrapaReadOnly);
	virtual GrapaError Close();
	virtual GrapaError GetSize(u64& pSize);
	virtual GrapaError SetSize(u64 pSize);
	virtual GrapaError Create(const char *fileName);
	virtual GrapaError Delete(const char *fileName);
	virtual GrapaError Flush();
	virtual GrapaError Purge(u64 blockCount, u16 blockSize);
	virtual GrapaError Write(u64 blockPos, u16 blockSize, u64 offset, u64 length, const void *b);
	virtual GrapaError Read(u64 blockPos, u16 blockSize, u64 offset, u64 length, void *b);
	virtual GrapaError Append(u64 length, const void *b);
	virtual GrapaError Append(const GrapaCHARFile& pValue);
};

////////////////////////////////////////////////////////////////////////////////

#endif // _GrapaValue_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
