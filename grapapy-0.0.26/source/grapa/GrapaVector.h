// GrapaVector.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaVector_
#define _GrapaVector_

#include "GrapaFloat.h"
#include "GrapaState.h"

class GrapaVectorBYTE
{
public:
	u8 mToken;
	u8* mBytes;
	u64 mLength;
	GrapaVectorBYTE(const GrapaBYTE& b);
	GrapaVectorBYTE(const GrapaVectorBYTE& b);
	~GrapaVectorBYTE();
	virtual void CLEAR();
	GrapaBYTE Get();
};

class GrapaVectorValue
{
public:
	u8 isRaw;
	union {
		GrapaRuleEvent* e;
		GrapaVectorBYTE* b;
	};
	GrapaVectorValue() { isRaw = 0; e = NULL; };
	GrapaVectorValue(GrapaRuleEvent* v) { isRaw = 0; e = NULL; Set(v); };
	GrapaVectorValue(GrapaVectorValue* v) { isRaw = 0; e = NULL; Set(v); };
	GrapaVectorValue(GrapaFloat* v) { isRaw = 0; e = NULL; Set(v); };
	GrapaVectorValue(GrapaInt* v) { isRaw = 0; e = NULL; Set(v); };
	GrapaVectorValue(GrapaBYTE* v) { isRaw = 0; e = NULL; Set(v); };
	~GrapaVectorValue();
	virtual void CLEAR();
	void Set(GrapaRuleEvent* v);
	void Set(GrapaVectorValue* v);
	void Set(GrapaFloat* v);
	void Set(GrapaInt* v);
	void Set(GrapaBYTE* v);
	GrapaRuleEvent* Get();
};

struct GrapaVectorItem
{
	u8 isValue : 1;
	u8 isNull : 1;
	u8 mLen:6;
	u8 mToken;
	u8 d[sizeof(GrapaVectorValue*)];
};

class GrapaVectorParam
{
public:
	GrapaVectorItem* d;
	GrapaVectorValue av;
	GrapaVectorValue* a;
	GrapaFloat* aa;
	GrapaRuleEvent* Aop;
	GrapaRuleQueue params;
	GrapaRuleEvent* ap;
	GrapaVectorParam(GrapaScriptExec* pScriptExec, GrapaVectorItem* pData, u8 pBlock, u64 pPos);
	~GrapaVectorParam();
	GrapaFloat Op(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl);
	GrapaFloat Mul(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pDiv = false);
	GrapaFloat Pow(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pRoot = false);
	GrapaFloat Add(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pSub = false);
	int Cmp(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pZero = false, bool pDecend = false);
};

class GrapaVector
{
public:
	u64* mCounts;
	GrapaVectorItem* mData;
	u64 mSize;
	u8 mDim, mBlock, mMaxBlock, mSetBlock;
	GrapaRuleQueue mLabels;
	GrapaVector();
	GrapaVector(const GrapaVector& bi, u8 pBlock = 0);
	~GrapaVector();
	virtual void CLEAR();
	virtual bool FROM(const GrapaVector& pData, u8 pBlock = 0);
	virtual bool FROM(bool pFix, s64 pMax, s64 pExtra, GrapaRuleEvent* value, u8 pDim, u8 pBlock = 0);
	virtual bool FROM(GrapaScriptExec* pScriptExec, GrapaRuleEvent* value, u8 pDim, u8 pBlock = 0);
	virtual bool FROM(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, bool pFix, s64 pMax, s64 pExtra, const GrapaBYTE& pData, u8 pBlock = 0);
	virtual void FROM(const GrapaBYTE& pValue);
	virtual void FromValue(GrapaScriptExec* pScriptExec, const GrapaBYTE& pValue);
	virtual void TO(GrapaBYTE& pValue);
	virtual void TO(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* delim, GrapaBYTE& pValue);
	void _tocsv(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, u64 pos, u64& p, GrapaRuleEvent* delim, GrapaBYTE& pValue);
	GrapaVector& operator=(const GrapaVector& that);
	GrapaError Dot(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& bi, GrapaVector& result);
	GrapaError Mul(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, const GrapaVector& bi, bool pDiv = false);
	GrapaError Pow(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, const GrapaVector& bi, bool pRoot = false);
	GrapaError Add(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, const GrapaVector& bi, bool pSub=false);
	GrapaError Left(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, s64 pCount, GrapaVector& result);
	GrapaError Right(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, s64 pCount, GrapaVector& result);
	GrapaError Solve(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& result);
	GrapaError Cov(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& result, bool rowvar=true);
	GrapaError Sum(GrapaScriptExec* pScriptExec, GrapaVector& result, bool rowvar = true);
	GrapaError Mean(GrapaScriptExec* pScriptExec, GrapaVector& result, bool rowvar = true);
	virtual GrapaRuleEvent* ToArray();
	virtual GrapaRuleEvent* _toarray(u64 pos, u64& p);
	virtual GrapaRuleEvent* ToTuple();
	virtual GrapaRuleEvent* _totuple(u64 pos, u64& p);
	bool _scanvectorcreate(GrapaRuleEvent* value, u8 pos, u8 dim, u64* counts, GrapaVectorItem* data, u8 block, u64& p, u8& maxblock);
	GrapaBYTE getBytes();
	virtual bool Extend(const GrapaVector& bi);
	virtual GrapaRuleEvent* Split(u64 num, u64 axis=0);
	virtual bool Join(GrapaRuleEvent* event);
	virtual bool JoinH(GrapaRuleEvent* event);
	virtual GrapaRuleEvent* Shape();
	static GrapaRuleEvent* Shape(GrapaRuleEvent* event);
	static u64* ToShape(GrapaRuleEvent* event, u64& dim);
	virtual GrapaError ReShape(GrapaRuleEvent* shape);
	virtual bool Transpose();
	virtual bool Sort(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector* pOther, GrapaVector* pIndices, GrapaVector* pReturnIndices, bool rowvar, bool pZero, bool pDecend, GrapaRuleEvent *pOp, GrapaVector& pResult);
	virtual bool Reverse(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace);
	virtual bool Ref(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, bool reduce = false);
	virtual bool Rref(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace);
	virtual bool Inv(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace);
	virtual GrapaFloat Determinant(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace);
	virtual GrapaInt Rank(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace);
	virtual void Set(u64 pPos);
	virtual void Set(u64 pPos, GrapaRuleEvent* value);
	virtual void Set(u64 pPos, const GrapaVectorParam& fl);
	virtual void Set(u64 pPos, const GrapaFloat& fl);
	virtual void Set(u64 pPos, GrapaInt& fl);
	virtual void Identity(GrapaScriptExec* pScriptExec, u64 n);
	virtual bool Diagonal(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result);
	virtual bool TriU(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result);
	virtual bool TriL(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result);
	virtual bool EigH(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& w, GrapaVector& p);
};

#endif //_GrapaVector_

////////////////////////////////////////////////////////////////////////////////
