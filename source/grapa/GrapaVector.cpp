// GrapaVector.cpp
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

#include "GrapaVector.h"

#include "GrapaSystem.h"

extern GrapaSystem* gSystem;

// https://www.calculator.net/matrix-calculator.html
// [[6.2,2,5],[3,3.2,9]]*[[7,2],[3,4],[5,4]]
// [[6.2,2,5],[3,3.2,9]]*[7,2]
// [[74.4,40.4],[75.6,54.8]]

// [[[6.2,2,5],[3,3.2,9]]*[[7,2],[3,4],[5,4]],[[6.2,2,5],[3,3.2,9]]*[[7,2],[3,4],[5,4]]]
// https://docs.microsoft.com/en-us/dotnet/api/system.numerics.vector3?view=netcore-3.1

// https://matrix.reshish.com/inverse.php

#define _minvectordatablock_ (sizeof(GrapaVectorValue*))
#define _minvectorheadblock_ 2
#define _minvectorblock_ (_minvectorheadblock_+_minvectordatablock_)
#define _maxvectorblock_ 62
#define _datavectorpos(bd,bs,bp) ((GrapaVectorItem*)&((u8*)bd)[bs*bp])

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaVectorBYTE::GrapaVectorBYTE(const GrapaBYTE& b)
{
	mBytes = NULL;
	mLength = 0;
	mToken = 0;
	if (b.mBytes && b.mLength)
	{
		mBytes = (u8*)GrapaMem::Create(b.mLength + 1);
		memcpy(mBytes, b.mBytes, b.mLength);
		mBytes[b.mLength] = 0;
		mLength = b.mLength;
		mToken = b.mToken;
	};
};

GrapaVectorBYTE::GrapaVectorBYTE(const GrapaVectorBYTE& b)
{
	mBytes = NULL;
	mLength = 0;
	mToken = 0;
	if (b.mBytes && b.mLength)
	{
		mBytes = (u8*)GrapaMem::Create(b.mLength + 1);
		memcpy(mBytes, b.mBytes, b.mLength);
		mBytes[b.mLength] = 0;
		mLength = b.mLength;
		mToken = b.mToken;
	};
};

GrapaVectorBYTE::~GrapaVectorBYTE()
{
	CLEAR();
};

void GrapaVectorBYTE::CLEAR()
{
	if (mBytes) GrapaMem::Delete(mBytes);
	mBytes = NULL;
	mLength = 0;
	mToken = 0;
};

GrapaBYTE GrapaVectorBYTE::Get()
{
	if (mBytes && mLength)
	{
		GrapaBYTE v(mBytes, mLength);
		v.mToken = mToken;
		return(v);
	}
	return GrapaBYTE();
};

GrapaVectorValue::~GrapaVectorValue()
{
	CLEAR();
};

void GrapaVectorValue::CLEAR()
{
	if (isRaw)
	{
		if (b)
		{
			b->CLEAR();
			delete b;
		}
		b = NULL;
	}
	else if (e)
	{
		e->CLEAR();
		delete e;
	};
	isRaw = 0;
	e = NULL;
};

void GrapaVectorValue::Set(GrapaRuleEvent* v)
{
	CLEAR();
	if (v)
	{
		switch (v->mValue.mToken) {
		case GrapaTokenType::STR:
		case GrapaTokenType::INT:
		case GrapaTokenType::FLOAT: 
			b = new GrapaVectorBYTE(v->mValue);
			isRaw = 1;
			break;
		default: 
			e = GrapaScriptExec::CopyItem(v);
			isRaw = 0;
			break;
		};
	};
};

void GrapaVectorValue::Set(GrapaVectorValue* v)
{
	CLEAR();
	if (v)
	{
		if (v->isRaw)
			b = new GrapaVectorBYTE(*v->b);
		else
			e = GrapaScriptExec::CopyItem(v->e);
		isRaw = v->isRaw;
	};
};

void GrapaVectorValue::Set(GrapaFloat* v)
{
	CLEAR();
	if (v)
	{
		b = new GrapaVectorBYTE(v->getBytes());
		isRaw = 1;
	};
};

void GrapaVectorValue::Set(GrapaInt* v)
{
	CLEAR();
	if (v)
	{
		b = new GrapaVectorBYTE(v->getBytes());
		isRaw = 1;
	};
};

void GrapaVectorValue::Set(GrapaBYTE* v)
{
	CLEAR();
	if (v)
	{
		b = new GrapaVectorBYTE(*v);
		isRaw = 1;
	};
};

GrapaRuleEvent* GrapaVectorValue::Get()
{
	GrapaRuleEvent* result = NULL;
	if (e)
	{
		if (isRaw)
			result = new GrapaRuleEvent(0, GrapaCHAR(""), b->Get());
		else
			result = GrapaScriptExec::CopyItem(e);
	};
	return(result);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaVector::GrapaVector()
{
	mData = NULL;
	mCounts = NULL;
	mSize = 0;
	mDim = 0;
	mBlock = _minvectorblock_;
	mMaxBlock = _minvectordatablock_;
	mSetBlock = 0;
}

static u64* _scanvectordepth(GrapaRuleEvent* value, u8 pos, u8& dim, u8 maxDim, u64 size, u64& tot)
{
	if (maxDim && pos == maxDim) return(NULL);
	if (value == NULL || value->vQueue == NULL) return(NULL);
	if ((value->mValue.mToken != GrapaTokenType::ARRAY) && (pos!=1 || value->mValue.mToken != GrapaTokenType::LIST)) return(NULL);
	u64* counts = _scanvectordepth(value->vQueue->Head(), pos+1, dim, maxDim, size* value->vQueue->mCount, tot);
	if (counts == NULL)
	{
		dim = pos+1;
		tot = size* value->vQueue->mCount;
		counts = (u64*)GrapaMem::Create(dim * sizeof(u64));
		memset(counts,0, dim * sizeof(u64));
	}
	counts[pos] = value->vQueue->mCount;
	return(counts);
}

bool GrapaVector::_scanvectorcreate(GrapaRuleEvent* value, u8 pos, u8 dim, u64* counts, GrapaVectorItem* data, u8 block, u64& p, u8& maxblock)
{
	if (value == NULL || value->vQueue == NULL) return(false);
	if ((value->mValue.mToken != GrapaTokenType::ARRAY) && (pos != 1 || value->mValue.mToken != GrapaTokenType::LIST)) return(false);
	bool hasLabels = (mLabels.mCount == 0 && pos == 1 && value->mValue.mToken == GrapaTokenType::LIST);
	GrapaInt b;
	u64 i = 0;
	GrapaCHAR nullval;
	nullval.SetNull();
	value = value->vQueue->Head();
	bool hasBlankHeader = true;
	while (value)
	{
		if ((pos + 1) < dim)
		{
			if (counts[pos+1] != value->vQueue->mCount) return(false);
			if (!_scanvectorcreate(value, pos + 1, dim, counts, data, block, p, maxblock)) return(false);
		}
		else
		{
			if (hasBlankHeader && hasLabels)
			{
				bool isInt = true;
				bool isFloat = false;
				for (u64 j = 0; j < value->mName.mLength; j++)
				{
					if (!isdigit(value->mName.mBytes[j]))
					{
						if (value->mName.mBytes[j] == '.' && !isFloat)
						{
							isFloat = true;
							isInt = false;
						}
						else
						{
							isInt = isFloat = false;
							break;
						}
					}
				}
				if (isInt)
				{
					GrapaInt y;
					y.FromString(value->mName, 10);
					s64 lv = y.LongValue();
					if (lv != i)
						hasBlankHeader = false;
				}
				else
					hasBlankHeader = false;
			}
			if (hasLabels)
				mLabels.PushTail(new GrapaRuleEvent(0, value->mName, nullval));
			GrapaVectorItem *d1 = _datavectorpos(data, block, p);
			if (value->IsNull())
			{
				d1->isValue = 0;
				d1->isNull = 1;
				d1->mLen = 0;
				d1->mToken = 0;
			}
			else
			{
				bool handled = false;
				switch (value->mValue.mToken)
				{
				case GrapaTokenType::INT:
				case GrapaTokenType::FLOAT:
				case GrapaTokenType::STR:
					if ((value->mValue.mLength + _minvectorheadblock_) <= block)
					{
						d1->isValue = 0;
						d1->isNull = 0;
						d1->mLen = value->mValue.mLength;
						d1->mToken = value->mValue.mToken;
						memcpy(d1->d, value->mValue.mBytes, value->mValue.mLength);
						handled = true;
					}
					else if (value->mValue.mToken == GrapaTokenType::INT || value->mValue.mToken == GrapaTokenType::FLOAT)
					{
						if (value->mValue.mLength > maxblock)
							maxblock = (u8)value->mValue.mLength;
					}
					break;
				}
				if (!handled)
				{
					d1->isValue = 1;
					d1->isNull = 0;
					d1->mLen = 0;
					d1->mToken = 0;
					GrapaVectorValue* d = new GrapaVectorValue(value);
					memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
				}
			}
			p++;
		}
		value = value->Next();
		i++;
	}
	if (hasBlankHeader && hasLabels)
		mLabels.CLEAR();
	return(true);
}

GrapaVector::GrapaVector(const GrapaVector& bi, u8 pBlock)
{
	mData = NULL;
	mCounts = NULL;
	mSize = 0;
	mDim = 0;
	mBlock = _minvectorblock_;
	mMaxBlock = _minvectordatablock_;
	mSetBlock = 0;
	FROM(bi, pBlock);
}

GrapaVector::~GrapaVector()
{
	CLEAR();
}

void GrapaVector::CLEAR()
{
	if (mData && mSize)
		for (u64 i = 0; i < mSize; i++)
		{
			GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, i);
			if (d1->isValue)
			{
				GrapaVectorValue* d = 0L;
				memcpy(&d, d1->d, sizeof(GrapaVectorValue*));
				d1->isValue = 0;
				d1->isNull = 0;
				d1->mLen = 0;
				d1->mToken = 0;
				if (d)
				{
					d->CLEAR();
					delete d;
				}
			}
		}
	if (mData)
		GrapaMem::Delete(mData);
	mData = NULL;
	if (mCounts)
		GrapaMem::Delete(mCounts);
	mCounts = NULL;
	mSize = 0;
	mDim = 0;
	mBlock = _minvectorblock_;
	mMaxBlock = _minvectordatablock_;
	mLabels.CLEAR();
	// mSetBlock = 0;
}

bool GrapaVector::FROM(const GrapaVector& pData, u8 pBlock)
{
	CLEAR();
	if (pBlock)
	{
		mSetBlock = pBlock;
		if (mSetBlock % 2) mSetBlock++;
		mSetBlock += _minvectorblock_ - _minvectordatablock_;
	}
	mDim = pData.mDim;
	mBlock = pData.mBlock;
	mMaxBlock = _minvectordatablock_;
	if (mSetBlock >= _minvectorblock_ && mSetBlock <= _maxvectorblock_) 
		mBlock = mSetBlock;
	if (mSetBlock == 0 && (pData.mMaxBlock + _minvectorheadblock_) > mBlock)
		mBlock = (pData.mMaxBlock + _minvectorheadblock_) < _maxvectorblock_ ? (pData.mMaxBlock + _minvectorheadblock_) : _maxvectorblock_;
	if (mBlock % 2) mBlock++;
	mSize = pData.mSize;
	if (pData.mCounts)
	{
		mCounts = (u64*)GrapaMem::Create(sizeof(u64) * mDim);
		memcpy(mCounts, pData.mCounts, mDim * sizeof(u64));
	}
	if (pData.mData)
	{
		mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
		memset(mData, 0, mBlock * mSize);
		GrapaVectorItem* d1, * d2;
		for (u64 i = 0; i < mSize; i++)
		{
			d1 = _datavectorpos(mData, mBlock, i);
			d2 = _datavectorpos(pData.mData, pData.mBlock, i);
			d1->isNull = d2->isNull;
			if (!d1->isNull)
			{
				d1->isValue = d2->isValue;
				d1->mLen = d2->mLen;
				d1->mToken = d2->mToken;
				if (d2->isValue)
				{
					GrapaVectorValue* d = 0L;
					memcpy(&d, d2->d, sizeof(GrapaVectorValue*));
					if (d)
					{
						d = new GrapaVectorValue(d);
						memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
					}
				}
				else
				{
					if (mBlock >= d2->mLen)
						memcpy(d1->d, d2->d, d2->mLen);
					else
					{
						d1->isValue = 1;
						GrapaBYTE v(d2->d, d2->mLen);
						v.mToken = d2->mToken;
						GrapaVectorValue* d = new GrapaVectorValue(&v);
						memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
					}
				}
			}
		}
	}
	GrapaRuleEvent* ev = ((GrapaVector*)&pData)->mLabels.Head();
	while (ev)
	{
		mLabels.PushTail(new GrapaRuleEvent(0,ev->mName,ev->mValue));
		ev = ev->Next();
	}
	return(true);
}

bool GrapaVector::FROM(bool pFix, s64 pMax, s64 pExtra, GrapaRuleEvent* value, u8 pDim, u8 pBlock)
{
	CLEAR();
	if (pBlock)
	{
		mSetBlock = pBlock;
		if (mSetBlock % 2) mSetBlock++;
		mSetBlock += _minvectorblock_ - _minvectordatablock_;
	}
	if (mSetBlock >= _minvectorblock_ && mSetBlock <= _maxvectorblock_) mBlock = mSetBlock;
	if (value == NULL || value->vQueue == NULL || value->vQueue->mCount == 0)
	{
		mDim = 1;
		mSize = 0;
		mCounts = (u64*)GrapaMem::Create(mDim * sizeof(u64));
		mCounts[0] = 0;
		mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
	}
	else
	{
		mCounts = _scanvectordepth(value, 0, mDim, pDim, 1, mSize);
		mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
		memset(mData, 0, mBlock * mSize);
		u64 p = 0;
		if (!_scanvectorcreate(value, 0, mDim, mCounts, mData, mBlock, p, mMaxBlock))
		{
			CLEAR();
			return(false);
		}
	}
	return(true);
}

bool GrapaVector::FROM(GrapaScriptExec* pScriptExec, GrapaRuleEvent* value, u8 pDim, u8 pBlock)
{
	return FROM(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, value, pDim, pBlock);
}

char* _next_vector_token(GrapaCHAR& sep, char*& sC, u64& lenC, char*& vS, u64& vL, bool& isQuoted, bool& isSYS)
{
	vS = NULL;
	vL = 0;
	isQuoted = false;
	isSYS = false;
	char* ret = NULL;
	if (lenC == 0)
	{
		sC = NULL;
		return ret;
	}
	if (*sC == '"' || (lenC>=7 && sC[0] == '"' && sC[1] == '=' && sC[2] == '"' && sC[3] == '"'))
	{
		isQuoted = true;
		if (lenC >= 7 && sC[0] == '"' && sC[1] == '=' && sC[2] == '"' && sC[3] == '"')
			isSYS = true;
		char* sC2 = sC + 1 + (isSYS ? 3 : 0);
		char* v = sC2;
		u64 lenC2 = lenC - 1 - (isSYS ? 3 : 0);
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
		vS = &sC[1 +(isSYS ? 3 : 0)];
		vL = sC2 - sC - 1 - (isSYS ? 4 : 0);
		lenC = lenC - (v - sC) - 1;
		sC = v + 1;
		if (sep.mLength == 0)
		{
			u64 lenC2 = lenC;
			char* sC2 = sC;
			while (lenC2 && *sC2 != ',' && *sC2 != '\t')
			{
				lenC2--;
				sC2 = sC2 + 1;
			}
			if (lenC2 && *sC2 == '\t') sep.Append('\t');
			else sep.Append(',');
		}
		if (lenC>=sep.mLength && memcmp(sC, sep.mBytes, sep.mLength)==0)
		{
			lenC -= sep.mLength;
			sC = sC + sep.mLength;
			ret = v + sep.mLength;
		}
	}
	else
	{
		if (sep.mLength == 0)
		{
			u64 lenC2 = lenC;
			char* sC2 = sC;
			while (lenC2 && *sC2 != ',' && *sC2 != '\t')
			{
				lenC2--;
				sC2 = sC2 + 1;
			}
			if (lenC2 && *sC2 == '\t') sep.Append('\t');
			else sep.Append(',');
		}
		char* sC2 = (char*)GrapaMem::MemFind(sC, lenC, sep.mBytes, sep.mLength);
		if (sC2)
		{
			vS = sC;
			vL = sC2 - sC;
			lenC = lenC - (sC2 - sC) - sep.mLength;
			sC = sC2 + sep.mLength;
			ret = sC2;
		}
		else
		{
			vS = sC;
			vL = lenC;
			lenC = 0;
			sC = NULL;
		}
	}
	return ret;
}

bool GrapaVector::FROM(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, bool pFix, s64 pMax, s64 pExtra, const GrapaBYTE& pData, u8 pBlock)
{
	CLEAR();
	if (pBlock)
	{
		mSetBlock = pBlock;
		if (mSetBlock % 2) mSetBlock++;
		mSetBlock += _minvectorblock_ - _minvectordatablock_;
	}
	if (pData.mBytes == NULL || pData.mLength == 0)
		return(false);
	char* s = (char*)pData.mBytes;
	u64 len = pData.mLength;

	GrapaCHAR del;
	char* pr = (char*)GrapaMem::MemFind(s, '\r', len);
	char* pn = (char*)GrapaMem::MemFind(s, '\n', len);
	if (pr && pn && pn > (pr + 1)) pn = NULL;
	del.FROM("\n");
	if (pr && (&pr[1]) == pn) del.FROM("\r\n");

	GrapaCHAR sep;

	char* s2 = (char*)GrapaMem::MemFind(s, len, (void*)del.mBytes, del.mLength);
	if (s == s2)
		return(false);
	u64 linelen = s2 ? (s2 - s) : len;
	u64 cols = 0;
	s2 = s;
	bool hasBlankHeader = true;
	while (s2)
	{
		cols++;
		char* sC = s2;
		u64 lenC = linelen - (s2 - s);
		char* vS = NULL;
		u64 vL = 0;
		bool isQuoted = false;
		bool isSYS = false;
		s2 = _next_vector_token(sep, sC, lenC, vS, vL, isQuoted, isSYS);
		if (cols == 1 && !isQuoted && vL >= 3 && memcmp(vS, "ï»¿", 3) == 0)
		{
			vS += 3;
			vL -= 3;
		}
		mLabels.PushTail(new GrapaRuleEvent(0, GrapaBYTE(vS, vL), GrapaCHAR("")));
		if (hasBlankHeader)
		{
			bool isInt = true;
			bool isFloat = false;
			for (u64 j = 0; j < vL; j++)
			{
				if (!isdigit(vS[j]))
				{
					if (vS[j] == '.' && !isFloat)
					{
						isFloat = true;
						isInt = false;
					}
					else
					{
						isInt = isFloat = false;
						break;
					}
				}
			}
			if (isInt)
			{
				GrapaInt y;
				y.FromString(GrapaBYTE(vS, vL), 10);
				s64 lv = y.LongValue();
				if (lv != (cols - 1))
					hasBlankHeader = false;
			}
			else
				hasBlankHeader = false;
		}

		if (s2 == NULL)
			break;
		s2 += sep.mLength;
	}
	if (hasBlankHeader)
		mLabels.CLEAR();
	u64 rows = 0;
	s2 = s;
	while (s2)
	{
		rows++;
		s2 = (char*)GrapaMem::MemFind(s2, len-(s2-s), (void*)del.mBytes, del.mLength);
		if (s2 == NULL)
			break;
		if (len == (s2 - s) + del.mLength)
			break;
		s2 += del.mLength;
	}
	mDim = 2;
	mBlock = _minvectorblock_;
	mMaxBlock = _minvectordatablock_;
	if (mSetBlock >= _minvectorblock_ && mSetBlock <= _maxvectorblock_) mBlock = mSetBlock;
	mSize = (rows-1)*cols;
	mCounts = (u64*)GrapaMem::Create(sizeof(u64) * mDim);
	mCounts[0] = rows-1;
	mCounts[1] = cols;
	mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
	memset(mData, 0, mBlock * mSize);

	u64 rows2 = 0;
	u64 i = 0;
	s2 = s;
	while (s2 && (s2 - s)<len)
	{
		rows2++;
		if (rows2 > rows)
		{
			CLEAR();
			return(false);
		}

		char* s1 = s2;
		u64 rowslen = len - (s2 - s);
		s2 = (char*)GrapaMem::MemFind(s2, rowslen, (void*)del.mBytes, del.mLength);
		u64 linelen = rowslen;
		//GrapaCHAR gg(s1, s2 - s1);
		if (s2)
		{
			linelen = s2 - s1;
			rowslen -= linelen;
			rowslen -= del.mLength;
		}
		if (rows2 == 1)
		{
			if (s2 == NULL)
				break;
			s2 += del.mLength;
			continue;
		}
		u64 cols2 = 0;
		while (linelen)
		{
			cols2++;
			if (cols2 > cols)
			{
				CLEAR();
				return(false);
			}

			char* sC = s1;
			u64 lenC = linelen;
			char* vS = NULL;
			u64 vL = 0;
			bool isQuoted = false;
			bool isSYS = false;
			char* s3 = _next_vector_token(sep, sC, lenC, vS, vL, isQuoted, isSYS);

			u64 itemlen = linelen;
			if (s3)
				itemlen = s3 - s1;

			bool isInt = true;
			bool isFloat = false;
			bool isNull = false;
			bool isOp = false;
			if (vL == 0)
				isNull = true;
			else if (isQuoted)
				isInt = false;
			else
			{
				for (int j = 0; j < vL; j++)
				{
					if (!isdigit(vS[j]))
					{
						if (vS[j] == '.' && !isFloat)
						{
							isFloat = true;
							isInt = false;
						}
						else
						{
							isInt = isFloat = false;
							break;
						}
					}
				}
			}
			GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, i);
			if (isNull)
			{
				d1->isValue = 0;
				d1->isNull = 1;
				d1->mLen = 0;
				d1->mToken = 0;
			}
			else if (isInt || isFloat)
			{
				GrapaBYTE z;
				d1->isNull = 0;
				d1->isValue = 0;
				if (isInt)
				{
					GrapaInt y;
					y.FromString(GrapaBYTE(vS, vL), 10);
					z = y.getBytes();
				}
				else if (isFloat)
				{
					GrapaFloat y(pFix, pMax, pExtra, 0);
					y.FromString(GrapaBYTE(vS, vL), 10);
					z = y.getBytes();
				}
				if ((z.mLength + _minvectorheadblock_) <= mBlock)
				{
					memcpy(d1->d, z.mBytes, z.mLength);
					d1->mLen = z.mLength;
					d1->mToken = z.mToken;
				}
				else
				{
					if (z.mLength >= mMaxBlock)
					{
						mMaxBlock = (u8)z.mLength;
						if (mMaxBlock % 2) mMaxBlock++;
					}
					GrapaVectorValue* d = new GrapaVectorValue(&z);
					memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
					d1->isValue = 1;
				}
			}
			else
			{
				GrapaCHAR v(vS, vL);
				if (!isSYS)
				{
					if ((vL + _minvectorheadblock_) <= mBlock)
					{
						memcpy(d1->d, vS, vL);
						d1->mLen = vL;
						d1->mToken = GrapaTokenType::STR;
					}
					else
					{
						if (vL >= mMaxBlock)
						{
							mMaxBlock = (u8)vL;
							if (mMaxBlock % 2) mMaxBlock++;
						}
						GrapaVectorValue* d = new GrapaVectorValue(&v);
						memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
						d1->isValue = 1;
					}
				}
				else
				{
					GrapaRuleEvent* rulexx = pScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$start"));
					GrapaRuleEvent* plan = pScriptExec->Plan(pNameSpace, v, rulexx, 0);
					if (plan && plan->mValue.mToken == GrapaTokenType::OP)
					{
						GrapaRuleEvent* temp = pScriptExec->ProcessPlan(pNameSpace, plan);
						plan->CLEAR();
						delete plan;
						plan = temp;
					}
					GrapaVectorValue* d = new GrapaVectorValue(plan);
					memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
					d1->isValue = 1;
				}
			}
			i++;

			if (s3 == NULL)
				break;
			itemlen += sep.mLength;
			s1 += itemlen;
			linelen -= itemlen;
		}

		if (s2 == NULL)
			break;
		s2 += del.mLength;
	}

	return(true);
}

void GrapaVector::FROM(const GrapaBYTE& pValue)
{
	CLEAR();
}

void GrapaVector::FromValue(GrapaScriptExec* pScriptExec, const GrapaBYTE& pValue)
{
	CLEAR();
	GrapaInt t;
	GrapaFloat u(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	mDim = 1;
	mCounts = (u64*)GrapaMem::Create(sizeof(u64));
	mCounts[0] = 1;
	mSize = 1;
	mData = (GrapaVectorItem*)GrapaMem::Create(mBlock);
	memset(mData, 0, mBlock);
	switch (pValue.mToken)
	{
	case GrapaTokenType::INT:
		t.FromBytes(pValue);
		Set(0, t);
		break;
	case GrapaTokenType::FLOAT:
		u.FromBytes(pValue);
		Set(0, u);
		break;
	}
}

void GrapaVector::TO(GrapaBYTE& pValue)
{
	pValue.SetLength(0);
	pValue.mToken = 0;
	if (mData == NULL)
		return;
}

void GrapaVector::TO(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* delim, GrapaBYTE& pValue)
{
	pValue.SetLength(0);
	pValue.mToken = 0;
	if (mData == NULL)
		return;
	u64 p = 0;
	u64 oldG = pValue.mGrow;
	pValue.mGrow = 1024 * 32;
	_tocsv(pScriptExec, pNameSpace, 0, p, (delim&&delim->vQueue)?delim->vQueue->Head():NULL, pValue);
	pValue.mGrow = oldG;
	pValue.SetSize(pValue.mSize);
}

void GrapaVector::_tocsv(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, u64 pos, u64& p, GrapaRuleEvent* delim, GrapaBYTE& pValue)
{
	bool hadItem = false;
	for (u64 i = 0; i < mCounts[pos]; i++)
	{
		if ((pos + 1) < mDim)
		{
			if (i == 0 && pos == 0)
			{
				bool hadItem2 = false;
				GrapaRuleEvent* delim2 = delim?delim->Next():NULL;
				GrapaRuleEvent* ev = mLabels.Head();
				if (ev)
				{
					while (ev)
					{
						if (hadItem2 && delim2)
							pValue.Append(delim2->mValue);
						hadItem2 = true;
						pValue.Append(ev->mName);
						ev = ev->Next();
					}
				}
				else
				{
					for (u64 j = 0; j < mCounts[pos + 1]; j++)
					{
						if (hadItem2 && delim2)
							pValue.Append(delim2->mValue);
						hadItem2 = true;
						pValue.Append(GrapaInt(j).ToString());
					}
				}
				if (delim)
					pValue.Append(delim->mValue);
			}
			if (hadItem && delim)
				pValue.Append(delim->mValue);
			hadItem = true;
			_tocsv(pScriptExec, pNameSpace, pos + 1, p, delim?delim->Next():NULL, pValue);
		}
		else
		{
			if (hadItem && delim)
				pValue.Append(delim->mValue);
			hadItem = true;
			GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, p);
			GrapaRuleEvent* val = NULL;
			if (d1->isNull)
			{
			}
			else if (d1->isValue)
			{
				GrapaVectorValue* d = 0L;
				memcpy(&d, d1->d, sizeof(GrapaVectorValue*));
				if (d)
					val = d->Get();
			}
			else
			{
				GrapaBYTE z(d1->d, d1->mLen);
				z.mToken = d1->mToken;
				val = new GrapaRuleEvent(0, GrapaCHAR(""), z);
			}
			if (val)
			{
				GrapaSystemSend send;
				send.isActive = false;
				if (val->vQueue) pScriptExec->EchoList(&send, val, false, false, false);
				else pScriptExec->EchoValue(&send, val, false, false, false);
				GrapaCHAR buf;
				send.GetStr(buf);
				switch (val->mValue.mToken)
				{
				case GrapaTokenType::LIST:
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::XML:
				case GrapaTokenType::TAG:
				case GrapaTokenType::EL:
				case GrapaTokenType::TABLE:
				case GrapaTokenType::VECTOR:
				case GrapaTokenType::WIDGET:
				case GrapaTokenType::OP:
				case GrapaTokenType::CODE:
				case GrapaTokenType::CLASS:
				case GrapaTokenType::OBJ:
				case GrapaTokenType::RULE:
				case GrapaTokenType::TOKEN:
					//pValue.Append("\"=\"\"");
					//buf.Replace(GrapaCHAR("\""), GrapaCHAR("\"\""));
					//pValue.Append(buf);
					//pValue.Append("\"\"\"");
					pValue.Append("\"");
					buf.Replace(GrapaCHAR("\""), GrapaCHAR("\"\""));
					pValue.Append(buf);
					pValue.Append("\"");
					break;
				default:
					if (delim && GrapaMem::MemFind(buf.mBytes, buf.mLength, delim->mValue.mBytes, delim->mValue.mLength) != NULL)
					{
						pValue.Append("\"");
						buf.Replace(GrapaCHAR("\""), GrapaCHAR("\"\""));
						pValue.Append(buf);
						pValue.Append("\"");
					}
					else
					{
						pValue.Append(buf);
					}
					break;
				}
				val->CLEAR();
				delete val;
			}
			p++;
		}
	}
	return;
}

GrapaVector& GrapaVector::operator =(const GrapaVector& that)
{
	if (this != &that)
		FROM(that);
	return *this;
}

GrapaFloat GrapaVectorParam::Op(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl)
{
	ap->mNull = false;
	ap->vRulePointer = NULL;
	if (fl && fl->d && !fl->d->isNull)
	{
		if (fl->a->isRaw)
		{
			ap->vRulePointer = new GrapaRuleEvent(0, GrapaCHAR(""), fl->a->b->Get());
		}
		else if (fl->a->e)
		{
			ap->vRulePointer = new GrapaRuleEvent(0, GrapaCHAR(""), fl->a->e->mValue);
		}
	}
	GrapaRuleEvent* temp = pScriptExec->ProcessPlan(pNameSpace, Aop, params.Head());
	if (temp)
	{
		GrapaInt t;
		switch (temp->mValue.mToken)
		{
		case GrapaTokenType::INT:
			t.FromBytes(temp->mValue);
			aa->FromInt(t);
			break;
		case GrapaTokenType::FLOAT:
			aa->FromBytes(temp->mValue);
			break;
		}
		delete temp;
	}
	if (fl && ap->vRulePointer)
	{
		ap->vRulePointer->CLEAR();
		delete ap->vRulePointer;
		ap->vRulePointer = NULL;
	}
	return *aa;
};

GrapaFloat GrapaVectorParam::Mul(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pDiv)
{
	if (Aop)
		return Op(pScriptExec, pNameSpace, fl);
	if (fl)
	{
		if (pDiv)
			return *fl->aa / *aa;
		else
			return *fl->aa * *aa;
	}
	return *aa;
};

GrapaFloat GrapaVectorParam::Pow(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pRoot)
{
	if (Aop)
		return Op(pScriptExec, pNameSpace, fl);
	if (fl)
	{
		if (pRoot)
			return fl->aa->Root2(*aa);
		else
			return fl->aa->Pow2(*aa);
	}
	return *aa;
};

GrapaFloat GrapaVectorParam::Add(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pSub)
{
	if (Aop)
		return Op(pScriptExec, pNameSpace, fl);
	if (fl)
	{
		if (pSub)
			return *fl->aa - *aa;
		else
			return *fl->aa + *aa;
	}
	return *aa;
};

int GrapaVectorParam::Cmp(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVectorParam* fl, bool pZero, bool pDecend)
{
	if (Aop)
	{
		GrapaFloat f = Op(pScriptExec, pNameSpace, fl);
		GrapaInt i = f.ToInt();
		return (int) i.LongValue();
	}
	if (fl)
	{
		if (*aa == *fl->aa) return 0;
		if (pZero)
		{
			if (aa->Abs() > fl->aa->Abs()) return pDecend ? -1 : 1;
		}
		else
		{
			if (*aa > * fl->aa) return pDecend ? -1 : 1;
		}
		return pDecend ? 1 : -1;
	}
	return pDecend ? 1 : -1;
};

GrapaVectorParam::GrapaVectorParam(GrapaScriptExec* pScriptExec, GrapaVectorItem* pData, u8 pBlock, u64 pPos)
{
	d = _datavectorpos(pData, pBlock, (pPos));
	a = NULL;
	Aop = NULL;
	aa = new GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	if (!d->isNull)
	{
		if (d->isValue)
			memcpy(&a, d->d, sizeof(GrapaVectorValue*));
		else
		{
			a = &av;
			av.isRaw = 1;
			GrapaBYTE tv(d->d, d->mLen);
			tv.mToken = d->mToken;
			av.b = new GrapaVectorBYTE(tv);
		}
		if (a->isRaw)
		{
			if (a->b)
			{
				switch (a->b->mToken)
				{
				case GrapaTokenType::INT:
					aa->FromInt(a->b->Get());
					break;
				case GrapaTokenType::FLOAT:
					aa->FromBytes(a->b->Get());
					break;
				}
			}
		}
		else
		{
			if (a->e)
			{
				GrapaInt t;
				switch (a->e->mValue.mToken)
				{
				case GrapaTokenType::INT:
					t.FromBytes(a->e->mValue);
					aa->FromInt(t);
					break;
				case GrapaTokenType::FLOAT:
					aa->FromBytes(a->e->mValue);
					break;
				case GrapaTokenType::OP:
					Aop = a->e;
					break;
				}
			}
		}
	}
	ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
	params.PushTail(ap);
}

GrapaVectorParam::~GrapaVectorParam()
{
	if (aa)
	{
		delete aa;
		aa = NULL;
	}
}

void GrapaVector::Set(u64 pPos)
{
	GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, (pPos));
	d1->isValue = 0;
	d1->isNull = 1;
	d1->mLen = 0;
	d1->mToken = 0;
}

void GrapaVector::Set(u64 pPos, GrapaRuleEvent* value)
{
	GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, pPos);
	if (value->IsNull())
	{
		d1->isValue = 0;
		d1->isNull = 1;
		d1->mLen = 0;
		d1->mToken = 0;
	}
	else
	{
		bool handled = false;
		switch (value->mValue.mToken)
		{
		case GrapaTokenType::INT:
		case GrapaTokenType::FLOAT:
		case GrapaTokenType::STR:
			if ((value->mValue.mLength + _minvectorheadblock_) <= mBlock)
			{
				d1->isValue = 0;
				d1->isNull = 0;
				d1->mLen = value->mValue.mLength;
				d1->mToken = value->mValue.mToken;
				memcpy(d1->d, value->mValue.mBytes, value->mValue.mLength);
				handled = true;
			}
			else if (value->mValue.mToken == GrapaTokenType::INT || value->mValue.mToken == GrapaTokenType::FLOAT)
			{
				if (value->mValue.mLength > mMaxBlock)
					mMaxBlock = (u8)value->mValue.mLength;
			}
			break;
		}
		if (!handled)
		{
			d1->isValue = 1;
			d1->isNull = 0;
			d1->mLen = 0;
			d1->mToken = 0;
			GrapaVectorValue* d = new GrapaVectorValue(value);
			memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
		}
	}
}

void GrapaVector::Set(u64 pPos, const GrapaVectorParam& fl)
{
	GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, (pPos));
	GrapaVectorItem* d2 = fl.d;
	d1->isNull = d2->isNull;
	if (!d1->isNull)
	{
		d1->isValue = d2->isValue;
		d1->mLen = d2->mLen;
		d1->mToken = d2->mToken;
		if (d2->isValue)
		{
			GrapaVectorValue* d = 0L;
			memcpy(&d, d2->d, sizeof(GrapaVectorValue*));
			if (d)
			{
				d = new GrapaVectorValue(d);
				memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
			}
		}
		else
		{
			if (mBlock >= d2->mLen)
				memcpy(d1->d, d2->d, d2->mLen);
			else
			{
				d1->isValue = 1;
				GrapaBYTE v(d2->d, d2->mLen);
				v.mToken = d2->mToken;
				GrapaVectorValue* d = new GrapaVectorValue(&v);
				memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
			}
		}
	}
}

void GrapaVector::Set(u64 pPos, const GrapaFloat& fl)
{
	GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, (pPos));
	d1->isNull = 0;
	GrapaBYTE v;
	if (((GrapaFloat*)&fl)->IsInt())
		v = ((GrapaFloat*)&fl)->ToInt().getBytes();
	else
		v = ((GrapaFloat*)&fl)->getBytes();
	if ((v.mLength + _minvectorheadblock_) <= mBlock)
	{
		d1->isValue = 0;
		d1->mLen = v.mLength;
		d1->mToken = v.mToken;
		memcpy(d1->d, v.mBytes, v.mLength);
	}
	else
	{
		if (v.mLength >= mMaxBlock)
		{
			mMaxBlock = (u8)v.mLength;
			if (mMaxBlock % 2) mMaxBlock++;
		}
		d1->isValue = 1;
		GrapaVectorValue* d = new GrapaVectorValue(&v);
		memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
	}
}

void GrapaVector::Set(u64 pPos, GrapaInt& fl)
{
	GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, (pPos));
	d1->isNull = 0;
	GrapaBYTE v;
	v = fl.getBytes();
	if ((v.mLength + _minvectorheadblock_) <= mBlock)
	{
		d1->isValue = 0;
		d1->mLen = v.mLength;
		d1->mToken = v.mToken;
		memcpy(d1->d, v.mBytes, v.mLength);
	}
	else
	{
		if (v.mLength >= mMaxBlock)
		{
			mMaxBlock = (u8)v.mLength;
			if (mMaxBlock % 2) mMaxBlock++;
		}
		d1->isValue = 1;
		GrapaVectorValue* d = new GrapaVectorValue(&v);
		memcpy(d1->d, &d, sizeof(GrapaVectorValue*));
	}
}

GrapaError GrapaVector::Dot(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& bi, GrapaVector& result)
{
	GrapaInt t;
	GrapaRuleQueue params;
	GrapaRuleEvent* ap;
	ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
	params.PushTail(ap);
	if (mData == NULL || bi.mData == NULL)
		return -1;
	result.CLEAR();
	bool isT = false;
	bool isT2 = false;
	if (mDim == 1 && bi.mDim == 2 && mCounts[0] == bi.mCounts[0])
	{
		isT = true;
		GrapaMem::Delete(mCounts);
		mDim = 2;
		mCounts = (u64*)GrapaMem::Create(sizeof(u64) * 2);
		mCounts[0] = 1;
		mCounts[1] = bi.mCounts[0];
	}
	else if (mDim == 2 && bi.mDim == 1 && mCounts[1] == bi.mCounts[0])
	{
		isT2 = true;
		u64* counts = (u64*)GrapaMem::Create(sizeof(u64) * 2);
		bi.mDim = 2;
		counts[0] = bi.mCounts[0];
		counts[1] = 1;
		GrapaMem::Delete(bi.mCounts);
		bi.mCounts = counts;
	}
	switch (mDim)
	{
	case 2:
		switch (bi.mDim)
		{
		case 2:
			if (mCounts[1] != bi.mCounts[0])
				return -1;
			result.mDim = 2;
			result.mSetBlock = mSetBlock;
			result.mBlock = mBlock;
			result.mMaxBlock = _minvectordatablock_;
			result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
			result.mCounts[0] = mCounts[0];
			result.mCounts[1] = bi.mCounts[1];
			result.mSize = result.mCounts[0] * result.mCounts[1];
			result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
			memset(result.mData, 0, result.mBlock * result.mSize);
			for (u64 i = 0; i < mCounts[0]; i++)
			{
				for (u64 j = 0; j < bi.mCounts[1]; j++)
				{

					GrapaFloat val(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
					bool isSet = false;
					for (u64 x = 0; x < mCounts[1]; x++)
					{
						GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + x));
						GrapaVectorParam p2(pScriptExec, bi.mData, bi.mBlock, (x * bi.mCounts[1] + j));
						if (p1.a && p2.a)
						{
							val += p2.Mul(pScriptExec, pNameSpace, &p1);
							isSet = true;
						}
					}
					if (isSet)
						result.Set((i * result.mCounts[1] + j), val);
					else
						result.Set((i * result.mCounts[1] + j));
				}
			}
			break;
		}
		break;
	}
	if (isT)
	{
		mCounts[0] = mCounts[1];
		mDim = 1;
		result.mCounts[0] = result.mCounts[1];
		result.mDim = 1;
	}
	else if (isT2)
	{
		bi.mDim = 1;
		if (result.mDim != 1)
		{
			result.mDim = 1;
		}
	}
	return 0;
}

GrapaError GrapaVector::Mul(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, const GrapaVector& bi, bool pDiv)
{
	GrapaInt t;
	GrapaRuleQueue params;
	GrapaRuleEvent* ap;
	ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
	params.PushTail(ap);
	if (mData == NULL || bi.mData == NULL)
		return -1;
	switch (mDim)
	{
	case 1:
		if (bi.mDim == 1)
		{
			if (bi.mCounts[0] == 1)
			{
				GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (0));
				if (p1.a)
				{
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						GrapaVectorParam p2(pScriptExec, mData, mBlock, (i));
						if (p2.a)
							Set((i), p1.Mul(pScriptExec, pNameSpace, &p2, pDiv));
						else
							Set((i));
					}
				}
			}
			else if (bi.mCounts[0] == mCounts[0])
			{
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (i));
					GrapaVectorParam p2(pScriptExec, mData, mBlock, (i));
					if (p2.a)
						Set((i), p1.Mul(pScriptExec, pNameSpace, &p2, pDiv));
					else
						Set(i);
				}
			}
			else
				return -1;
		}
		else
			return -1;
		break;
	case 2:
		switch (bi.mDim)
		{
		case 1:
			if (bi.mCounts[0] == 1)
			{
				GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (0));
				if (p1.a)
				{
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						for (u64 j = 0; j < mCounts[1]; j++)
						{
							GrapaVectorParam p2(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
							if (p2.a)
								Set((i * mCounts[1] + j), p1.Mul(pScriptExec, pNameSpace, &p2, pDiv));
							else
								Set((i * mCounts[1] + j));
						}
					}
				}
			}
			else if (bi.mCounts[0] == mCounts[1])
			{
				for (u64 j = 0; j < mCounts[1]; j++)
				{
					GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (j));
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						GrapaVectorParam p2(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						if (p2.a)
							Set((i * mCounts[1] + j), p1.Mul(pScriptExec, pNameSpace, &p2, pDiv));
						else
							Set((i * mCounts[1] + j));
					}
				}
			}
			else
				return -1;
			break;
		case 2:
			if (mCounts[0] != bi.mCounts[0])
				return -1;
			if (bi.mCounts[1] == 1)
			{
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					GrapaVectorParam p2(pScriptExec, bi.mData, bi.mBlock, (i));
					for (u64 j = 0; j < mCounts[1]; j++)
					{
						GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						if (p1.a && p2.a)
							Set((i * mCounts[1] + j), p2.Mul(pScriptExec, pNameSpace, &p1, pDiv));
						else
							Set((i * mCounts[1] + j));
					}
				}
			}
			else
			{
				if (mCounts[1] != bi.mCounts[1])
					return -1;
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					for (u64 j = 0; j < mCounts[1]; j++)
					{
						GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						GrapaVectorParam p2(pScriptExec, bi.mData, bi.mBlock, (i * bi.mCounts[1] + j));
						if (p1.a && p2.a)
							Set((i * mCounts[1] + j), p2.Mul(pScriptExec, pNameSpace, &p1, pDiv));
						else
							Set((i * mCounts[1] + j));
					}
				}
			}
			break;
		}
		break;
	}
	return 0;
}

GrapaError GrapaVector::Pow(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, const GrapaVector& bi, bool pRoot)
{
	GrapaInt t;
	GrapaRuleQueue params;
	GrapaRuleEvent* ap;
	ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
	params.PushTail(ap);
	if (mData == NULL || bi.mData == NULL)
		return -1;
	switch (mDim)
	{
	case 1:
		switch (bi.mDim)
		{
		case 1:
			if (bi.mCounts[0] == 1)
			{
				GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (0));
				if (p1.a)
				{
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						GrapaVectorParam p2(pScriptExec, mData, mBlock, i);
						if (p2.a)
							Set(i, p1.Pow(pScriptExec, pNameSpace, &p2, pRoot));
						else
							Set(i);
					}
				}
			}
			else if (bi.mCounts[0] == mCounts[0])
			{
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (i));
					GrapaVectorParam p2(pScriptExec, mData, mBlock, (i));
					if (p2.a)
						Set((i), p1.Pow(pScriptExec, pNameSpace, &p2, pRoot));
					else
						Set(i);
				}
			}
			else
				return -1;
			break;
		default:
			return -1;
		}
		break;
	case 2:
		switch (bi.mDim)
		{
		case 1:
			if (bi.mCounts[0] == 1)
			{
				GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (0));
				if (p1.a)
				{
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						for (u64 j = 0; j < mCounts[1]; j++)
						{
							GrapaVectorParam p2(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
							if (p2.a)
								Set((i * mCounts[1] + j), p1.Pow(pScriptExec, pNameSpace, &p2, pRoot));
							else
								Set((i * mCounts[1] + j));
						}
					}
				}
			}
			else if (bi.mCounts[0] == mCounts[1])
			{
				for (u64 j = 0; j < mCounts[1]; j++)
				{
					GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (j));
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						GrapaVectorParam p2(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						if (p2.a)
							Set((i * mCounts[1] + j), p1.Pow(pScriptExec, pNameSpace, &p2, pRoot));
						else
							Set((i * mCounts[1] + j));
					}
				}
			}
			else
				return -1;
			break;
		case 2:
			if (mCounts[0] != bi.mCounts[0])
				return -1;
			if (bi.mCounts[1] == 1)
			{
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					GrapaVectorParam p2(pScriptExec, bi.mData, bi.mBlock, (i));
					for (u64 j = 0; j < mCounts[1]; j++)
					{
						GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						if (p1.a && p2.a)
							Set((i * mCounts[1] + j), p2.Pow(pScriptExec, pNameSpace, &p1, pRoot));
						else
							Set((i * mCounts[1] + j));
					}
				}
			}
			else
			{
				if (mCounts[1] != bi.mCounts[1])
					return -1;
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					for (u64 j = 0; j < mCounts[1]; j++)
					{
						GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						GrapaVectorParam p2(pScriptExec, bi.mData, bi.mBlock, (i * bi.mCounts[1] + j));
						if (p1.a && p2.a)
							Set((i * mCounts[1] + j), p2.Pow(pScriptExec, pNameSpace, &p1, pRoot));
						else
							Set((i * mCounts[1] + j));
					}
				}
			}
			break;
		}
		break;
	}
	return 0;
}

GrapaError GrapaVector::Add(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, const GrapaVector& bi, bool pSub)
{
	GrapaInt t;
	GrapaRuleQueue params;
	GrapaRuleEvent* ap;
	ap = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
	params.PushTail(ap);
	if (mData == NULL || bi.mData == NULL)
		return -1;
	switch (mDim)
	{
	case 1:
		if (bi.mDim == 1)
		{
			if (bi.mCounts[0] == 1)
			{
				GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (0));
				if (p1.a)
				{
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						GrapaVectorParam p2(pScriptExec, mData, mBlock, (i));
						if (p2.a)
							Set((i), p1.Add(pScriptExec, pNameSpace, &p2, pSub));
						else
							Set((i), pSub ? -*p1.aa : *p1.aa);
					}
				}
			}
			else if (bi.mCounts[0] == mCounts[0])
			{
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (i));
					GrapaVectorParam p2(pScriptExec, mData, mBlock, (i));
					if (p2.a)
						Set((i), p1.Add(pScriptExec, pNameSpace, &p2, pSub));
					else
						Set((i), pSub ? -*p1.aa : *p1.aa);
				}
			}
			else
				return -1;
		}
		else
			return -1;
		break;
	case 2:
		switch (bi.mDim)
		{
		case 1:
			if (bi.mCounts[0] == 1)
			{
				GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (0));
				if (p1.a)
				{
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						for (u64 j = 0; j < mCounts[1]; j++)
						{
							GrapaVectorParam p2(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
							if (p2.a)
								Set((i * mCounts[1] + j), p1.Add(pScriptExec, pNameSpace, &p2, pSub));
							else
								Set((i * mCounts[1] + j), pSub ? -*p1.aa : *p1.aa);
						}
					}
				}
			}
			else if (bi.mCounts[0] == mCounts[1])
			{
				for (u64 j = 0; j < mCounts[1]; j++)
				{
					GrapaVectorParam p1(pScriptExec, bi.mData, bi.mBlock, (j));
					for (u64 i = 0; i < mCounts[0]; i++)
					{
						GrapaVectorParam p2(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						if (p2.a)
							Set((i * mCounts[1] + j), p1.Add(pScriptExec, pNameSpace, &p2, pSub));
						else
							Set((i * mCounts[1] + j), pSub ? -*p1.aa : *p1.aa);
					}
				}
			}
			else
				return -1;
			break;
		case 2:
			if (mCounts[0] != bi.mCounts[0])
				return -1;
			if (bi.mCounts[1] == 1)
			{
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					GrapaVectorParam p2(pScriptExec, bi.mData, bi.mBlock, (i));
					for (u64 j = 0; j < mCounts[1]; j++)
					{
						GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						if (p1.a && p2.a)
							Set((i * mCounts[1] + j), p2.Add(pScriptExec, pNameSpace, &p1, pSub));
						else
							Set((i * mCounts[1] + j), pSub ? -*p1.aa : *p1.aa);
					}
				}
			}
			else
			{
				if (mCounts[1] != bi.mCounts[1])
					return -1;
				for (u64 i = 0; i < mCounts[0]; i++)
				{
					for (u64 j = 0; j < mCounts[1]; j++)
					{
						GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
						GrapaVectorParam p2(pScriptExec, bi.mData, bi.mBlock, (i * bi.mCounts[1] + j));
						if (p1.a && p2.a)
							Set((i * mCounts[1] + j), p2.Add(pScriptExec, pNameSpace, &p1, pSub));
						else
							Set((i * mCounts[1] + j), pSub ? -*p1.aa : *p1.aa);
					}
				}
			}
			break;
		}
		break;
	}
	return 0;
}

GrapaError GrapaVector::Left(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, s64 pCount, GrapaVector& result)
{
	if (mData == NULL)
		return -1;
	result.CLEAR();
	switch (mDim)
	{
	case 1:
		result.mDim = 1;
		result.mSetBlock = mSetBlock;
		result.mBlock = mBlock;
		result.mMaxBlock = _minvectordatablock_;
		result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
		if (pCount < 0) pCount = ((s64)mCounts[0]) + pCount;
		if (pCount < 0) pCount = 0;
		if (pCount > (s64)mCounts[0]) pCount = mCounts[0];
		result.mCounts[0] = pCount;
		result.mSize = result.mCounts[0];
		result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
		memset(result.mData, 0, result.mBlock * result.mSize);
		for (u64 i = 0; i < result.mCounts[0]; i++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, (i));
			result.Set((i), *p1.aa);
		}
		break;
	case 2:
		result.mDim = 2;
		result.mSetBlock = mSetBlock;
		result.mBlock = mBlock;
		result.mMaxBlock = _minvectordatablock_;
		result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
		result.mCounts[0] = mCounts[0];
		if (pCount < 0) pCount = ((s64)mCounts[1]) + pCount;
		if (pCount < 0) pCount = 0;
		if (pCount > (s64)mCounts[1]) pCount = mCounts[1];
		result.mCounts[1] = pCount;
		result.mSize = result.mCounts[0] * result.mCounts[1];
		result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
		memset(result.mData, 0, result.mBlock * result.mSize);
		for (u64 i = 0; i < mCounts[0]; i++)
		{
			for (u64 j = 0; j < result.mCounts[1]; j++)
			{
				GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
				result.Set((i * result.mCounts[1] + j), *p1.aa);
			}
		}
		break;
	default:
		return -1;
	}
	return 0;
}

GrapaError GrapaVector::Right(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, s64 pCount, GrapaVector& result)
{
	if (mData == NULL)
		return -1;
	result.CLEAR();
	switch (mDim)
	{
	case 1:
		result.mDim = 1;
		result.mSetBlock = mSetBlock;
		result.mBlock = mBlock;
		result.mMaxBlock = _minvectordatablock_;
		result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
		if (pCount < 0) pCount = ((s64)mCounts[0]) + pCount;
		if (pCount < 0) pCount = 0;
		if (pCount > (s64)mCounts[0]) pCount = mCounts[0];
		result.mCounts[0] = pCount;
		result.mSize = result.mCounts[0];
		result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
		memset(result.mData, 0, result.mBlock * result.mSize);
		for (u64 i = 0; i < result.mCounts[0]; i++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, (mCounts[0] - i - 1));
			result.Set((result.mCounts[0] - i - 1), *p1.aa);
		}
		break;
	case 2:
		result.mDim = 2;
		result.mSetBlock = mSetBlock;
		result.mBlock = mBlock;
		result.mMaxBlock = _minvectordatablock_;
		result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
		result.mCounts[0] = mCounts[0];
		if (pCount < 0) pCount = ((s64)mCounts[1]) + pCount;
		if (pCount < 0) pCount = 0;
		if (pCount > (s64)mCounts[1]) pCount = mCounts[1];
		result.mCounts[1] = pCount;
		result.mSize = result.mCounts[0] * result.mCounts[1];
		result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
		memset(result.mData, 0, result.mBlock * result.mSize);
		for (u64 i = 0; i < mCounts[0]; i++)
		{
			for (u64 j = 0; j < result.mCounts[1]; j++)
			{
				GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + mCounts[1] - j - 1));
				result.Set((i * result.mCounts[1] + result.mCounts[1] - j - 1), *p1.aa);
			}
		}
		break;
	default:
		return -1;
	}
	return 0;
}

GrapaError GrapaVector::Solve(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& result)
{
	if (mData == NULL)
		return -1;
	result.CLEAR();
	switch (mDim)
	{
	case 2:
		if ((mCounts[0]+1) != mCounts[1])
			return -1;
		if ((mCounts[0] + 1) == mCounts[1])
		{
			GrapaVector left,right;
			Left(pScriptExec, pNameSpace, mCounts[0], left);
			left.Inv(pScriptExec, pNameSpace);
			Right(pScriptExec, pNameSpace, 1, right);
			left.Dot(pScriptExec, pNameSpace, right, result);
		}
		break;
	}
	return 0;
}

GrapaError GrapaVector::Cov(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& result, bool isRows)
{
	result.CLEAR();
	if (mData == NULL)
		return -1;
	if (mDim == 2)
	{
		GrapaVector v(*this);
		u64 rows = isRows ? v.mCounts[1] : v.mCounts[0];
		u64 cols = isRows ? v.mCounts[0] : v.mCounts[1];
		for (u64 j = 0; j < cols; j++)
		{
			GrapaFloat sum(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			for (u64 i = 0; i < rows; i++)
			{
				u64 pos = isRows ? (j * v.mCounts[1] + i) : (i * v.mCounts[1] + j);
				GrapaVectorParam p1(pScriptExec, v.mData, v.mBlock, (pos));
				sum = sum + *p1.aa;
			}
			sum = sum / rows;
			for (u64 i = 0; i < rows; i++)
			{
				u64 pos = isRows ? (j * v.mCounts[1] + i) : (i * v.mCounts[1] + j);
				GrapaVectorParam p1(pScriptExec, v.mData, v.mBlock, (pos));
				*p1.aa = *p1.aa - sum;
				v.Set(pos, *p1.aa);
			}
		}
		GrapaVector v2;
		v2.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * v.mDim);
		memcpy(v2.mCounts, v.mCounts, v.mDim * sizeof(u64));
		v2.mData = (GrapaVectorItem*)GrapaMem::Create(v.mBlock * v.mSize);
		memcpy(v2.mData, v.mData, v.mSize * v.mBlock);
		v2.mSize = v.mSize;
		v2.mDim = v.mDim;
		v2.mBlock = v.mBlock;
		v2.mMaxBlock = v.mMaxBlock;
		v2.mSetBlock = v.mSetBlock;
		v.Transpose();
		u64 rowscols = isRows ? v.mCounts[1] : v.mCounts[0];
		cols = isRows ? v.mCounts[0] : v.mCounts[1];
		result.mDim = 2;
		result.mSetBlock = v.mSetBlock;
		result.mBlock = v.mBlock;
		result.mMaxBlock = _minvectordatablock_;
		result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
		result.mCounts[0] = rowscols;
		result.mCounts[1] = rowscols;
		result.mSize = rowscols * rowscols;
		result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
		memset(result.mData, 0, result.mBlock * result.mSize);
		for (u64 i = 0; i < rowscols; i++)
		{
			for (u64 j = i; j < rowscols; j++)
			{
				GrapaFloat val(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
				bool isSet = false;
				for (u64 x = 0; x < cols; x++)
				{
					u64 pos1 = isRows ? (x * v.mCounts[1] + i) : (i * v.mCounts[1] + x);
					u64 pos2 = isRows ? (j * v2.mCounts[1] + x) : (x * v2.mCounts[1] + j);
					GrapaVectorParam p1(pScriptExec, v.mData, v.mBlock, (pos1));
					GrapaVectorParam p2(pScriptExec, v2.mData, v2.mBlock, (pos2));
					if (p1.a && p2.a)
					{
						val += p2.Mul(pScriptExec, pNameSpace, &p1);
						isSet = true;
					}
				}
				if (cols>1) val = val / (cols-1);
				if (isSet)
					result.Set((i * rowscols + j), val);
				else
					result.Set((i * rowscols + j));
				if (i != j)
				{
					if (isSet)
						result.Set((j * rowscols + i), val);
					else
						result.Set((j * rowscols + i));
				}
			}
		}
		GrapaMem::Delete(v2.mCounts);
		v2.mCounts = NULL;
		GrapaMem::Delete(v2.mData);
		v2.mData = NULL;
		return 0;
	}

	return -1;
}

GrapaError GrapaVector::Sum(GrapaScriptExec* pScriptExec, GrapaVector& result, bool isRows)
{
	result.CLEAR();
	if (mData == NULL)
		return -1;
	if (mDim == 2)
	{
		u64 rows = isRows ? mCounts[1] : mCounts[0];
		u64 cols = isRows ? mCounts[0] : mCounts[1];
		result.mDim = isRows ? 2 : 1;
		result.mSetBlock = mSetBlock;
		result.mBlock = mBlock;
		result.mMaxBlock = _minvectordatablock_;
		result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
		result.mCounts[0] = cols;
		if (isRows) result.mCounts[1] = 1;
		result.mSize = cols;
		result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
		memset(result.mData, 0, result.mBlock * result.mSize);
		for (u64 j = 0; j < cols; j++)
		{
			GrapaFloat sum(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			for (u64 i = 0; i < rows; i++)
			{
				u64 pos = isRows ? (j * mCounts[1] + i) : (i * mCounts[1] + j);
				GrapaVectorParam p1(pScriptExec, mData, mBlock, (pos));
				sum = sum + *p1.aa;
			}
			result.Set(j, sum);
		}
		return 0;
	}
	return -1;
}

GrapaError GrapaVector::Mean(GrapaScriptExec* pScriptExec, GrapaVector& result, bool isRows)
{
	result.CLEAR();
	if (mData == NULL)
		return -1;
	if (mDim == 2)
	{
		u64 rows = isRows ? mCounts[1] : mCounts[0];
		u64 cols = isRows ? mCounts[0] : mCounts[1];
		result.mDim = isRows ? 2 : 1;
		result.mSetBlock = mSetBlock;
		result.mBlock = mBlock;
		result.mMaxBlock = _minvectordatablock_;
		result.mCounts = (u64*)GrapaMem::Create(sizeof(u64) * result.mDim);
		result.mCounts[0] = cols;
		if (isRows) result.mCounts[1] = 1;
		result.mSize = cols;
		result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
		memset(result.mData, 0, result.mBlock * result.mSize);
		for (u64 j = 0; j < cols; j++)
		{
			GrapaFloat sum(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
			for (u64 i = 0; i < rows; i++)
			{
				u64 pos = isRows ? (j * mCounts[1] + i) : (i * mCounts[1] + j);
				GrapaVectorParam p1(pScriptExec, mData, mBlock, (pos));
				sum = sum + *p1.aa;
			}
			result.Set(j, sum/rows);
		}
		return 0;
	}
	return -1;
}

GrapaRuleEvent* GrapaVector::ToArray()
{
	if (mData == NULL)
		return NULL;
	u64 p = 0;
	return _toarray(0, p);
}

GrapaRuleEvent* GrapaVector::_toarray(u64 pos, u64& p)
{
	GrapaRuleEvent* result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::ARRAY;
	result->vQueue = new GrapaRuleQueue();
	GrapaRuleEvent* label = mLabels.Head();
	if (pos == 1 && label)
	{
		result->mValue.mToken = GrapaTokenType::LIST;
	}
	for (u64 i = 0; i < mCounts[pos]; i++)
	{
		GrapaRuleEvent* val = NULL;
		if ((pos + 1) < mDim)
			val = _toarray(pos + 1, p);
		else
		{
			GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, p);
			if (d1->isNull)
			{
				val = new GrapaRuleEvent();
				val->mNull = true;
			}
			else if (d1->isValue)
			{
				GrapaVectorValue* d = 0L;
				memcpy(&d, d1->d, sizeof(GrapaVectorValue*));
				if (d)
				{
					val = d->Get();
				}
				else
				{
					val = new GrapaRuleEvent(true);
				}
			}
			else
			{
				GrapaBYTE z(d1->d, d1->mLen);
				z.mToken = d1->mToken;
				val = new GrapaRuleEvent(0, GrapaCHAR(""), z);
			}
			p++;
		}
		if (val)
		{
			if (label && result->mValue.mToken == GrapaTokenType::LIST)
				val->mName.FROM(label->mName);
			result->vQueue->PushTail(val);
		}
		if (label)
			label = label->Next();
	}
	return(result);
}

GrapaBYTE GrapaVector::getBytes()
{
	GrapaBYTE result;
	return result;
}

bool GrapaVector::Extend(const GrapaVector& bi)
{
	switch (mDim)
	{
	case 2:
		switch (bi.mDim)
		{
		case 2:
			if (bi.mSize && mSize == 0)
			{
				u64* oCounts = mCounts;
				GrapaVectorItem* oData = mData;
				u64 oSize = mSize;
				u8 oDim = mDim, oBlock = mBlock, oMaxBlock = mMaxBlock, oSetBlock = mSetBlock;
				GrapaVector v2(bi);
				mCounts = v2.mCounts;
				mData = v2.mData;
				mDim = v2.mDim;
				mBlock = v2.mBlock;
				mMaxBlock = v2.mMaxBlock;
				mSetBlock = v2.mSetBlock;
				mSize = v2.mSize;
				v2.mCounts = oCounts;
				v2.mData = oData;
				v2.mDim = oDim;
				v2.mBlock = oBlock;
				v2.mMaxBlock = oMaxBlock;
				v2.mSetBlock = oSetBlock;
				v2.mSize = oSize;
			}
			else if (mCounts[1] != bi.mCounts[1])
				return false;
			else
			{
				u8 newBlock = mBlock > bi.mBlock ? mBlock : bi.mBlock;
				GrapaVector v2(bi, newBlock);
				GrapaVectorItem *data2 = (GrapaVectorItem*)GrapaMem::Create(newBlock * (mSize + v2.mSize));
				if (mBlock!=newBlock)
				{
					u64* oCounts = mCounts;
					GrapaVectorItem* oData = mData;
					u64 oSize = mSize;
					u8 oDim = mDim, oBlock = mBlock, oMaxBlock = mMaxBlock, oSetBlock = mSetBlock;
					GrapaVector v3(*this, newBlock);
					mCounts = v3.mCounts;
					mData = v3.mData;
					mDim = v3.mDim;
					mBlock = v3.mBlock;
					mMaxBlock = v3.mMaxBlock;
					mSetBlock = v3.mSetBlock;
					mSize = v3.mSize;
					v3.mCounts = oCounts;
					v3.mData = oData;
					v3.mDim = oDim;
					v3.mBlock = oBlock;
					v3.mMaxBlock = oMaxBlock;
					v3.mMaxBlock = oMaxBlock;
					v3.mSize = oSize;
				}
				memcpy(data2, mData, newBlock * mSize);
				GrapaVectorItem* d1 = _datavectorpos(data2, newBlock, mSize);
				memcpy(d1, v2.mData, newBlock * v2.mSize);
				GrapaMem::Delete(mData);
				mData = data2;
				mSize += v2.mSize;
				mCounts[0] += v2.mCounts[0];
				mBlock = newBlock;
				memset(v2.mData, 0, newBlock * v2.mSize);
			}
			break;
		default:
			return false;
		}
		break;
	default:
		return false;
	}
	return true;
}

GrapaRuleEvent* GrapaVector::Split(u64 num, u64 axis)
{
	GrapaRuleEvent* result = NULL;

	if (mDim == 2)
	{
		result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
		result->mValue.mToken = GrapaTokenType::ARRAY;
		result->vQueue = new GrapaRuleQueue();

		GrapaVector tmp(*this);
		u64 items = 0, lastitems = 0, width = 0;

		if (axis == 0)
		{
			width = tmp.mCounts[1];
			items = tmp.mCounts[0] / num;
			if (tmp.mCounts[0] % num) items++;
			lastitems = items - ((items * num) - tmp.mCounts[0]);
		}
		else if (axis == 1)
		{
			width = tmp.mCounts[0];
			items = tmp.mCounts[1] / num;
			if (tmp.mCounts[1] % num) items++;
			lastitems = items - ((items * num) - tmp.mCounts[1]);
		}
		else
		{
			return result;
		}

		for (u64 i = 0; i < num; i++)
		{
			bool isLast = false;
			if (i == (num - 1))
				isLast = true;
			if (items)
			{
				GrapaRuleEvent* val = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
				val->mValue.mToken = GrapaTokenType::VECTOR;
				result->vQueue->PushTail(val);
				val->vVector = new GrapaVector();
				val->vVector->mSize = (isLast ? lastitems : items) * width;
				val->vVector->mDim = tmp.mDim;
				val->vVector->mBlock = tmp.mBlock;
				val->vVector->mMaxBlock = tmp.mMaxBlock;
				val->vVector->mSetBlock = tmp.mSetBlock;
				val->vVector->mCounts = (u64*)GrapaMem::Create(val->vVector->mDim * sizeof(u64));
				val->vVector->mData = (GrapaVectorItem*)GrapaMem::Create(val->vVector->mBlock * val->vVector->mSize);
				if (axis == 0)
				{
					val->vVector->mCounts[0] = isLast ? lastitems : items;
					val->vVector->mCounts[1] = width;
					GrapaVectorItem* d1 = _datavectorpos(tmp.mData, tmp.mBlock, (i * items * width));
					memcpy(val->vVector->mData, d1, val->vVector->mSize * val->vVector->mBlock);
					if (mLabels.mCount)
					{
						GrapaRuleEvent* ev = mLabels.Head();
						for (u64 k = 0; k < width; k++)
						{
							if (ev)
							{
								val->vVector->mLabels.PushTail(new GrapaRuleEvent(0, ev->mName, ev->mValue));
								ev = ev->Next();
							}
							else
							{
								val->vVector->mLabels.PushTail(new GrapaRuleEvent(true));
							}
						}
					}
				}
				else if (axis == 1)
				{
					val->vVector->mCounts[0] = width;
					val->vVector->mCounts[1] = isLast ? lastitems : items;
					for (u64 j = 0; j < width; j++)
					{
						GrapaVectorItem* d1 = _datavectorpos(tmp.mData, tmp.mBlock, (j * tmp.mCounts[1] + i * items));
						GrapaVectorItem* d2 = _datavectorpos(val->vVector->mData, val->vVector->mBlock, (j * val->vVector->mCounts[1]));
						memcpy(d2, d1, (isLast ? lastitems : items) * val->vVector->mBlock);
					}
					if (mLabels.mCount)
					{
						GrapaRuleEvent* ev = mLabels.Head(i * items);
						for (u64 k = 0; k < (isLast ? lastitems : items); k++)
						{
							if (ev)
							{
								val->vVector->mLabels.PushTail(new GrapaRuleEvent(0, ev->mName, ev->mValue));
								ev = ev->Next();
							}
							else
							{
								val->vVector->mLabels.PushTail(new GrapaRuleEvent(true));
							}
						}
					}
				}
			}
		}

		memset(tmp.mData, 0, tmp.mSize * tmp.mBlock);
	}

	return result;
}

bool GrapaVector::Join(GrapaRuleEvent* event)
{
	CLEAR();

	if (event == NULL || !(event->mValue.mToken == GrapaTokenType::ARRAY || event->mValue.mToken == GrapaTokenType::LIST))
		return false;

	u64 rows = 0;
	u64 cols = 0;
	u8 block = 0;
	u8 maxBlock = 0;
	u8 setBlock = 0;

	GrapaRuleEvent* evx = event->vQueue->Head();
	while (evx)
	{
		GrapaRuleEvent* ev = evx;
		while (ev->mValue.mToken == GrapaTokenType::PTR && ev->vRulePointer) ev = ev->vRulePointer;
		if (ev)
		{
			if (ev->mValue.mToken != GrapaTokenType::VECTOR || ev->vVector == NULL || ev->vVector->mCounts == NULL || ev->vVector->mData == NULL || ev->vVector->mDim != 2 || ev->vVector->mCounts[1] == 0)
				return false;

			if (cols == 0)
			{
				cols = ev->vVector->mCounts[1];
				block = ev->vVector->mBlock;
				maxBlock = ev->vVector->mMaxBlock;
				setBlock = ev->vVector->mSetBlock;
			}

			if (ev->vVector->mCounts[1] != cols)
				return false;

			if (ev->vVector->mBlock > block) block = ev->vVector->mBlock;
			if (ev->vVector->mMaxBlock > maxBlock) maxBlock = ev->vVector->mMaxBlock;
			if (ev->vVector->mSetBlock > setBlock) setBlock = ev->vVector->mSetBlock;

			rows += ev->vVector->mCounts[0];
		};

		evx = evx->Next();
	}

	mSize = rows*cols;
	mDim = 2;
	mBlock = block;
	mMaxBlock = maxBlock;
	mSetBlock = setBlock;
	mCounts = (u64*)GrapaMem::Create(mDim * sizeof(u64));
	mCounts[0] = rows;
	mCounts[1] = cols;
	mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);

	u64 pos = 0;
	evx = event->vQueue->Head();
	while (evx)
	{
		GrapaRuleEvent* ev = evx;
		while (ev->mValue.mToken == GrapaTokenType::PTR && ev->vRulePointer) ev = ev->vRulePointer;
		if (ev)
		{
			GrapaVector tmp(*ev->vVector, block - _minvectorheadblock_);
			GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, pos);
			memcpy(d1, tmp.mData, tmp.mSize * tmp.mBlock);
			memset(tmp.mData, 0, tmp.mSize * tmp.mBlock);
			pos += tmp.mSize;
		}
		evx = evx->Next();
	}
	evx = event->vQueue->Head();
	if (evx)
	{
		GrapaRuleEvent* ev = evx;
		while (ev->mValue.mToken == GrapaTokenType::PTR && ev->vRulePointer) ev = ev->vRulePointer;
		if (ev && ev->vVector->mLabels.mCount)
		{
			GrapaRuleEvent* ev2 = ev->vVector->mLabels.Head();
			for (u64 k = 0; k < ev->vVector->mLabels.mCount; k++)
			{
				if (ev2)
				{
					mLabels.PushTail(new GrapaRuleEvent(0, ev2->mName, ev2->mValue));
					ev2 = ev2->Next();
				}
				else
				{
					mLabels.PushTail(new GrapaRuleEvent(true));
				}
			}
		}
	}

	return true;
}

bool GrapaVector::JoinH(GrapaRuleEvent* event)
{
	CLEAR();

	if (event == NULL || !(event->mValue.mToken == GrapaTokenType::ARRAY || event->mValue.mToken == GrapaTokenType::LIST))
		return false;

	u64 rows = 0;
	u64 cols = 0;
	u8 block = 0;
	u8 maxBlock = 0;
	u8 setBlock = 0;

	GrapaRuleEvent* ev = event->vQueue->Head();
	while (ev)
	{
		if (ev->mValue.mToken != GrapaTokenType::VECTOR || ev->vVector == NULL || ev->vVector->mCounts == NULL || ev->vVector->mData == NULL || ev->vVector->mDim != 2 || ev->vVector->mCounts[1] == 0)
			return false;

		if (cols == 0)
		{
			rows = ev->vVector->mCounts[0];
			block = ev->vVector->mBlock;
			maxBlock = ev->vVector->mMaxBlock;
			setBlock = ev->vVector->mSetBlock;
		}

		if (ev->vVector->mCounts[0] != rows)
			return false;

		if (ev->vVector->mBlock > block) block = ev->vVector->mBlock;
		if (ev->vVector->mMaxBlock > maxBlock) maxBlock = ev->vVector->mMaxBlock;
		if (ev->vVector->mSetBlock > setBlock) setBlock = ev->vVector->mSetBlock;

		cols += ev->vVector->mCounts[1];

		ev = ev->Next();
	}

	mSize = rows * cols;
	mDim = 2;
	mBlock = block;
	mMaxBlock = maxBlock;
	mSetBlock = setBlock;
	mCounts = (u64*)GrapaMem::Create(mDim * sizeof(u64));
	mCounts[0] = rows;
	mCounts[1] = cols;
	mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);

	bool hasLabels = false;
	u64 pos = 0;
	ev = event->vQueue->Head();
	while (ev)
	{
		GrapaVector tmp(*ev->vVector, block - _minvectorheadblock_);
		u64 copycols = ev->vVector->mCounts[1];
		for (u64 i = 0; i < rows; i++)
		{
			GrapaVectorItem* d1 = _datavectorpos(ev->vVector->mData, ev->vVector->mBlock, (i * copycols));
			GrapaVectorItem* d2 = _datavectorpos(mData, mBlock, (i * mCounts[1] + pos));
			memcpy(d2, d1, copycols * tmp.mBlock);
		}
		GrapaRuleEvent* ev2 = ev->vVector->mLabels.Head();
		for (u64 k = 0; k < copycols; k++)
		{
			if (ev2)
			{
				hasLabels = true;
				mLabels.PushTail(new GrapaRuleEvent(0, ev2->mName, ev2->mValue));
				ev2 = ev2->Next();
			}
			else
			{
				mLabels.PushTail(new GrapaRuleEvent(true));
			}
		}
		pos += copycols;
		ev = ev->Next();
	}
	if (!hasLabels)
		mLabels.CLEAR();

	return true;
}

GrapaRuleEvent* GrapaVector::Shape()
{
	GrapaRuleEvent* result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
	result->mValue.mToken = GrapaTokenType::ARRAY;
	result->vQueue = new GrapaRuleQueue();
	for (u64 i = 0; i < mDim; i++)
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(""), GrapaInt(mCounts[i]).getBytes()));
	return result;
}

GrapaRuleEvent* GrapaVector::Shape(GrapaRuleEvent* event)
{
	if (event == NULL || !(event->mValue.mToken == GrapaTokenType::ARRAY || event->mValue.mToken == GrapaTokenType::LIST))
		return NULL;

	u64 size;
	u8 dim;
	u64* counts = _scanvectordepth(event, 0, dim, 0, 1, size);

	GrapaRuleEvent* result = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(""));
	result->mValue.mToken = GrapaTokenType::ARRAY;
	result->vQueue = new GrapaRuleQueue();
	for (u64 i = 0; i < dim; i++)
		result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(""), GrapaInt(counts[i]).getBytes()));

	GrapaMem::Delete(counts);

	return result;
}

u64* GrapaVector::ToShape(GrapaRuleEvent* event, u64& dim)
{
	u64* counts = NULL;
	dim = 0;
	if (event && !event->mNull)
	{
		GrapaRuleEvent* r2V = event;
		bool delr2V = false;
		if (event->mValue.mToken == GrapaTokenType::VECTOR)
		{
			r2V = event->vVector->ToArray();
			delr2V = true;
		}
		GrapaRuleEvent* e = NULL;
		e = r2V->vQueue->Head();
		while (e)
		{
			if (e->mValue.mToken != GrapaTokenType::INT)
				break;
			dim++;
			e = e->Next();
		}
		if (dim)
		{
			counts = (u64*)GrapaMem::Create(dim * sizeof(u64));
			dim = 0;
			e = r2V->vQueue->Head();
			while (e)
			{
				if (e->mValue.mToken != GrapaTokenType::INT)
					break;
				GrapaInt n(e->mValue);
				counts[dim++] = n.GetItem(0);
				e = e->Next();
			}
		}
		if (delr2V)
		{
			r2V->CLEAR();
			delete r2V;
		}
	}
	return counts;
}

GrapaError GrapaVector::ReShape(GrapaRuleEvent* shape)
{
	GrapaError err = -1;
	u64 dim = 0;
	u64* counts = GrapaVector::ToShape(shape, dim);
	if (counts)
	{
		// be sure counts match up....if so, replace counts
		u64 size = 1;
		for (u64 i = 0; i < dim; i++) size *= counts[i];
		if (mSize == size)
		{
			u64* old = mCounts;
			mCounts = counts;
			mDim = (u8)dim;
			counts = old;
			err = 0;
		}
		GrapaMem::Delete(counts);
	}
	return err;
}

bool GrapaVector::Transpose()
{
	mLabels.CLEAR();

	if (mDim == 2 && mCounts[1]==1)
	{
		mCounts[1] = mCounts[0];
		mCounts[0] = 1;
		return true;
	}
	else if (mDim == 2 && mCounts[0] == 1)
	{
		mCounts[0] = mCounts[1];
		mCounts[1] = 1;
		return true;
	}
	if (mDim != 2)
		return false;

	GrapaVectorItem* data = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
	memcpy(data, mData, mSize * mBlock);
	u64* counts = (u64*)GrapaMem::Create(mDim * sizeof(u64));
	memcpy(counts, mCounts, mDim * sizeof(u64));

	u64 pos = 0;
	GrapaVectorItem* dp = data;
	for (u64 i = 0; i < mCounts[0]; i++)
		for (u64 j = 0; j < mCounts[1]; j++)
			memcpy(_datavectorpos(mData, mBlock, (j * mCounts[0] + i)), _datavectorpos(data, mBlock, (i * mCounts[1] + j)), mBlock);

	mCounts[0] = counts[1];
	mCounts[1] = counts[0];

	GrapaMem::Delete(data);
	GrapaMem::Delete(counts);

	return true;
}

struct GrapaVectorStruct
{
	GrapaScriptExec* vScriptExec;
	GrapaNames* vNameSpace;
	GrapaRuleEvent* vOp;
	GrapaRuleQueue* vParams;
	GrapaVectorItem* mData2;
	u8 mBlock;
	bool mZero, mDecend;
	u64 mRows,mCols;
	bool isCol;
	u64 mPos;
};

static int GrapaVectorSort(const void* arg1, const void* arg2)
{
	GrapaVectorStruct* a = (GrapaVectorStruct*)arg1;
	GrapaVectorStruct* b = (GrapaVectorStruct*)arg2;
	for (u64 i = 0; i < (a->isCol ? a->mRows : a->mCols); i++)
	{
		GrapaVectorParam p1(a->vScriptExec, a->mData2, a->mBlock, (a->isCol ? i * a->mCols + a->mPos : a->mPos * a->mCols + i));
		GrapaVectorParam p2(b->vScriptExec, b->mData2, b->mBlock, (b->isCol ? i * b->mCols + b->mPos : b->mPos * b->mCols + i));
		if (a->vOp && a->vOp->mValue.mToken==GrapaTokenType::OP)
		{
			a->vParams->Head(0)->mValue.FROM(p1.aa->getBytes());
			a->vParams->Head(1)->mValue.FROM(p2.aa->getBytes());
			GrapaRuleEvent* temp = a->vScriptExec->ProcessPlan(a->vNameSpace, a->vOp, a->vParams->Head());
			int c = -1;
			if (temp)
			{
				if (temp->mValue.mToken == GrapaTokenType::INT)
				{
					GrapaInt a;
					a.FromBytes(temp->mValue);
					c = (u8)a.LongValue();
				}
				delete temp;
			}
			if (c) return a->mDecend ? -c : c;
		}
		else
		{
			int c = p1.Cmp(a->vScriptExec, a->vNameSpace, &p2, a->mZero, a->mDecend);
			if (c) return c;
		}
	}
	return a->mDecend ? 1 : -1;
}

bool GrapaVector::Sort(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector* pOther, GrapaVector* pIndices, GrapaVector* pReturnIndices, bool rowvar, bool pZero, bool pDecend, GrapaRuleEvent* pOp, GrapaVector& pResult)
{
	pResult.CLEAR();

	bool isT = false;
	if (mDim == 1)
	{
		if (rowvar)
		{
			u64* counts = (u64*)GrapaMem::Create(2 * sizeof(u64));
			counts[0] = mCounts[0];
			counts[1] = 1;
			GrapaMem::Delete(mCounts);
			mCounts = counts;
			mDim = 2;
		}
		else
		{
			u64* counts = (u64*)GrapaMem::Create(2 * sizeof(u64));
			counts[0] = 1;
			counts[1] = mCounts[0];
			GrapaMem::Delete(mCounts);
			mCounts = counts;
			mDim = 2;
		}
		isT = true;
	}
	else if (mDim != 2)
		return false;

	bool hasOther = false;
	if (pOther && pOther->mCounts && pOther->mDim == 2)
	{
		if ((rowvar && pOther->mCounts[0] == mCounts[0]) || (!rowvar && pOther->mCounts[1] == mCounts[1]))
			hasOther = true;
	}

	bool hasIndices = false;
	if (pIndices)
	{
		hasIndices = true;
	}

	bool hasReturnIndices = false;
	if (pReturnIndices)
	{
		hasReturnIndices = true;
		pReturnIndices->CLEAR();
	}

	s64 rowcount = rowvar ? (s64)mCounts[0] : (s64)mCounts[1];
	s64 colcount = rowvar ? (s64)mCounts[1] : (s64)mCounts[0];

	GrapaRuleEvent nullval(true);

	if (rowvar)
	{
		GrapaRuleEvent* ev = mLabels.Head();
		while (ev)
		{
			pResult.mLabels.PushTail(new GrapaRuleEvent(0, ev->mName, ev->mValue));
			ev = ev->Next();
		}
	}

	if (pIndices)
	{
		if (pIndices->mDim != 1 || pIndices->mCounts == NULL)
			return false;

		pResult.mDim = mDim;
		pResult.mBlock = mBlock;
		pResult.mMaxBlock = mMaxBlock;
		pResult.mSetBlock = mSetBlock;
		pResult.mCounts = (u64*)GrapaMem::Create(pResult.mDim * sizeof(u64));
		if (rowvar)
		{
			pResult.mCounts[0] = pIndices->mCounts[0];
			pResult.mCounts[1] = mCounts[1];
		}
		else
		{
			pResult.mCounts[0] = mCounts[0];
			pResult.mCounts[1] = pIndices->mCounts[0];
		}
		pResult.mSize = pResult.mCounts[0] * pResult.mCounts[1];
		pResult.mData = (GrapaVectorItem*)GrapaMem::Create(pResult.mBlock * pResult.mSize);
		memset(pResult.mData, 0, pResult.mBlock * pResult.mSize);

		s64 rescols = rowvar ? (s64)pResult.mCounts[1] : (s64)pResult.mCounts[0];

		for (u64 i = 0; i < pIndices->mCounts[0]; i++)
		{
			GrapaVectorParam p1(pScriptExec, pIndices->mData, pIndices->mBlock, (i));
			s64 pos = -1;
			GrapaCHAR posName;
			GrapaRuleEvent* defaultValue = NULL;
			if (!p1.d->isNull && p1.a)
			{
				u8 token = p1.d->isValue ? (p1.a->isRaw ? p1.a->b->mToken : p1.a->e->mValue.mToken) : p1.d->mToken;
				if (token == GrapaTokenType::INT)
				{
					GrapaInt posInt = p1.aa->ToInt();
					posName = posInt.ToString();
					pos = posInt.LongValue();
					if (pos < 0) pos = rowcount + pos;
					if (pos < 0) pos = 0;
					if (pos > rowcount) pos = rowcount;
				}
				else if (token == GrapaTokenType::STR || token == GrapaTokenType::ID)
				{
					GrapaRuleEvent* a = p1.a->Get();
					if (a)
					{
						posName.FROM(a->mValue);
						s64 index = 0;
						GrapaRuleEvent* b = mLabels.Search(a->mValue, index);
						if (b)
							pos = index;
						delete a;
					}
					else
					{
						posName.FROM("NULL");
					}
				}
				else if (token == GrapaTokenType::LIST)
				{
					GrapaRuleEvent* a = p1.a->Get();
					if (a && a->vQueue->mCount)
					{
						defaultValue = a;
					}
					else
					{
						delete a;
						posName = GrapaInt(i).ToString();
					}
				}
				else
				{
					posName = GrapaInt(i).ToString();
				}
			}
			else
			{
				posName = GrapaInt(i).ToString();
			}
			if (pos >= 0 && pos < rowcount)
			{
				if (!rowvar)
				{
					GrapaRuleEvent* label = mLabels.Head(pos);
					if (label)
						pResult.mLabels.PushTail(pScriptExec->CopyItem(label));
					else
						pResult.mLabels.PushTail(new GrapaRuleEvent(0, posName, GrapaCHAR()));
				}
				for (s64 j = 0; j < rescols; j++)
				{
					GrapaVectorParam p1(pScriptExec, mData, mBlock, rowvar ? (pos * mCounts[1] + j) : (j * mCounts[1] + pos));
					pResult.Set(rowvar ? i * pResult.mCounts[1] + j : j * pResult.mCounts[1] + i, p1);
				}
			}
			else if (defaultValue)
			{
				if (!rowvar)
				{
					posName.FROM(defaultValue->vQueue->Head()->mName);
					pResult.mLabels.PushTail(new GrapaRuleEvent(0, posName, GrapaCHAR()));
				}
				for (s64 j = 0; j < rescols; j++)
				{
					pResult.Set(rowvar ? i * pResult.mCounts[1] + j : j * pResult.mCounts[1] + i, defaultValue->vQueue->Head());
				}
			}
			else
			{
				if (!rowvar)
					pResult.mLabels.PushTail(new GrapaRuleEvent(0, posName,GrapaCHAR()));
				for (s64 j = 0; j < rescols; j++)
				{
					pResult.Set(rowvar ? i * pResult.mCounts[1] + j : j * pResult.mCounts[1] + i);
				}
			}
			if (defaultValue)
			{
				delete defaultValue;
			}
		}
	}
	else
	{
		GrapaRuleQueue pq;
		{
			pq.PushTail(new GrapaRuleEvent(GrapaTokenType::FLOAT, 0, "", ""));
			pq.PushTail(new GrapaRuleEvent(GrapaTokenType::FLOAT, 0, "", ""));
		}

		GrapaVectorStruct* rows = (GrapaVectorStruct*)GrapaMem::Create(rowcount * sizeof(GrapaVectorStruct));
		for (s64 i = 0; i < rowcount; i++)
		{
			rows[i].vScriptExec = pScriptExec;
			rows[i].vNameSpace = pNameSpace;
			rows[i].vOp = pOp;
			rows[i].vParams = &pq;
			rows[i].mData2 = mData;
			rows[i].mBlock = mBlock;
			rows[i].mZero = pZero;
			rows[i].mDecend = pDecend;
			rows[i].mRows = mCounts[0];
			rows[i].mCols = mCounts[1];
			if (rowvar)
				rows[i].isCol = false;
			else
				rows[i].isCol = true;
			rows[i].mPos = i;
		}

		qsort(rows, rowcount, sizeof(GrapaVectorStruct), GrapaVectorSort);

		if (hasReturnIndices)
		{
			pReturnIndices->mDim = 1;
			pReturnIndices->mBlock = _minvectorblock_;
			pReturnIndices->mMaxBlock = _minvectordatablock_;
			pReturnIndices->mSetBlock = 0;
			pReturnIndices->mCounts = (u64*)GrapaMem::Create(pReturnIndices->mDim * sizeof(u64));
			pReturnIndices->mCounts[0] = mCounts[0];
			pReturnIndices->mSize = mCounts[0];
			pReturnIndices->mData = (GrapaVectorItem*)GrapaMem::Create(pReturnIndices->mBlock * pReturnIndices->mSize);
			for (u64 i = 0; i < mCounts[0]; i++)
			{
				GrapaFloat sum(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, rows[i].mPos);
				pReturnIndices->Set(i, sum);
			}
		}
		else
		{
			pResult.mDim = mDim;
			pResult.mBlock = mBlock;
			pResult.mMaxBlock = mMaxBlock;
			pResult.mSetBlock = mSetBlock;
			pResult.mCounts = (u64*)GrapaMem::Create(pResult.mDim * sizeof(u64));
			pResult.mCounts[0] = mCounts[0];
			pResult.mCounts[1] = mCounts[1];
			pResult.mSize = pResult.mCounts[0] * pResult.mCounts[1];
			pResult.mData = (GrapaVectorItem*)GrapaMem::Create(pResult.mBlock * pResult.mSize);
			memset(pResult.mData, 0, pResult.mBlock* pResult.mSize);
			GrapaVectorItem* dataOther = NULL;
			if (hasOther)
			{
				dataOther = pOther->mData;
				pOther->mData = (GrapaVectorItem*)GrapaMem::Create(pOther->mBlock * pOther->mSize);
			}
			for (s64 i = 0; i < rowcount; i++)
			{
				for (s64 j = 0; j < colcount; j++)
				{
					GrapaVectorParam p1(pScriptExec, mData, mBlock, rowvar ? (rows[i].mPos * mCounts[1] + j) : (j * mCounts[1] + rows[i].mPos));
					pResult.Set(rowvar ? i * mCounts[1] + j : j * mCounts[1] + i, *p1.aa);
				}
				if (hasOther)
				{
					if (rowvar)
						memcpy(_datavectorpos(pOther->mData, pOther->mBlock, (i * pOther->mCounts[1])), _datavectorpos(dataOther, pOther->mBlock, (rows[i].mPos * pOther->mCounts[1])), pOther->mBlock * pOther->mCounts[1]);
					else
						for (u64 j = 0; j < pOther->mCounts[0]; j++)
							memcpy(_datavectorpos(pOther->mData, pOther->mBlock, (j * pOther->mCounts[1] + i)), _datavectorpos(dataOther, pOther->mBlock, (j * pOther->mCounts[1] + rows[i].mPos)), pOther->mBlock);

				}
			}
			if (hasOther)
				GrapaMem::Delete(dataOther);
		}

		GrapaMem::Delete(rows);
	}

	if (isT)
	{
		if (rowvar)
		{
			mDim = 1;
			pResult.mDim = 1;
		}
		else if (mDim == 2 && mCounts[0] == 1)
		{
			mCounts[0] = mCounts[1];
			mDim = 1;
			pResult.mCounts[0] = pResult.mCounts[1];
			pResult.mDim = 1;
		}

	}

	return true;
}

bool GrapaVector::Reverse(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace)
{
	if (mDim != 2)
		return false;

	GrapaVectorItem* data = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
	memcpy(data, mData, mSize * mBlock);

	for (u64 i = 0; i < mCounts[0]; i++)
		memcpy(_datavectorpos(mData, mBlock, (i * mCounts[1])), _datavectorpos(data, mBlock, ((mCounts[0] - i - 1) * mCounts[1])), mBlock * mCounts[1]);

	GrapaMem::Delete(data);

	return true;
}

bool GrapaVector::Ref(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, bool reduce)
{
	if (mDim != 2)
		return false;

	for (u64 i = 0; i < mCounts[0] - 1; i++)
	{
		for (u64 j = 0; j < mCounts[1]; j++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
			if (p1.a && p1.aa->IsZero())
				continue;
			for (u64 l = i + 1; l < mCounts[0]; l++)
			{
				GrapaVectorParam p2(pScriptExec, mData, mBlock, (l * mCounts[1] + j));
				Set((l * mCounts[1] + j), GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0));
				for (u64 m = j + 1; m < mCounts[1]; m++)
				{
					GrapaVectorParam p3(pScriptExec, mData, mBlock, (i * mCounts[1] + m));
					GrapaVectorParam p4(pScriptExec, mData, mBlock, (l * mCounts[1] + m));
					GrapaFloat f = (*p4.aa * *p1.aa) - (*p3.aa * *p2.aa);
					Set((l * mCounts[1] + m), f);
				}
			}
			break;
		}
	}

	if (reduce)
	{
		GrapaVector vv(*this);
		vv.Sort(pScriptExec, pNameSpace, NULL, NULL, NULL, true, true, true, NULL, *this);
		for (u64 i = 0; i < mCounts[0]; i++)
		{
			for (u64 j = 0; j < mCounts[1]; j++)
			{
				GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
				if (p1.a && p1.aa->IsZero())
				{
					if (j + 1 == mCounts[1])
					{
						for (u64 k = i * mCounts[1]; k < mSize; k++)
						{
							GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, k);
							if (d1->isValue)
							{
								GrapaVectorValue* d = 0L;
								memcpy(&d, d1->d, sizeof(GrapaVectorValue*));
								d1->isValue = 0;
								d1->isNull = 0;
								d1->mLen = 0;
								d1->mToken = 0;
								if (d)
								{
									d->CLEAR();
									delete d;
								}
							}
						}
						mCounts[0] = i;
						mSize = mCounts[0] * mCounts[1];
						GrapaVectorItem* data = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
						memcpy(data, mData, mSize * mBlock);
						GrapaMem::Delete(mData);
						mData = data;
					}
					continue;
				}
				break;
			}
		}
	}

	return true;
}

bool GrapaVector::Rref(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace)
{
	if (mDim != 2)
		return false;

	for (u64 i = 0; i < mCounts[0]-1; i++)
	{
		GrapaVector vv(*this);
		vv.Sort(pScriptExec, pNameSpace, NULL, NULL, NULL, true, true, true, NULL, *this);
		for (u64 j = 0; j < mCounts[1]; j++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
			if (p1.a && p1.aa->IsZero())
				continue;
			for (u64 l = i + 1; l < mCounts[0]; l++)
			{
				GrapaVectorParam p2(pScriptExec, mData, mBlock, (l * mCounts[1] + j));
				Set((l * mCounts[1] + j), GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0));
				for (u64 m = j+1; m < mCounts[1]; m++)
				{
					GrapaVectorParam p3(pScriptExec, mData, mBlock, (i * mCounts[1] + m));
					GrapaVectorParam p4(pScriptExec, mData, mBlock, (l * mCounts[1] + m));
					GrapaFloat f = (*p4.aa * *p1.aa) - (*p3.aa * *p2.aa);
					Set((l * mCounts[1] + m), f);
				}
			}
			break;
		}
	}
	for (u64 i = 0; i < mCounts[0]; i++)
	{
		for (u64 j = 0; j < mCounts[1]; j++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
			if (p1.a && p1.aa->IsZero())
			{
				if (j + 1 == mCounts[1])
				{
					for (u64 k = i * mCounts[1]; k < mSize; k++)
					{
						GrapaVectorItem* d1 = _datavectorpos(mData, mBlock, k);
						if (d1->isValue)
						{
							GrapaVectorValue* d = 0L;
							memcpy(&d, d1->d, sizeof(GrapaVectorValue*));
							d1->isValue = 0;
							d1->isNull = 0;
							d1->mLen = 0;
							d1->mToken = 0;
							if (d)
							{
								d->CLEAR();
								delete d;
							}
						}
					}
					mCounts[0] = i;
					mSize = mCounts[0] * mCounts[1];
					GrapaVectorItem* data = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
					memcpy(data, mData, mSize * mBlock);
					GrapaMem::Delete(mData);
					mData = data;
				}
				continue;
			}
		}
	}

	for (s64 i = mCounts[0] - 1; i >= 0; i--)
	{
		GrapaVector vv(*this);
		vv.Sort(pScriptExec, pNameSpace, NULL, NULL, NULL, true, true, true, NULL, *this);
		for (u64 j = 0; j < mCounts[1]; j++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
			if (p1.a && p1.aa->IsZero())
				continue;
			for (u64 m = j + 1; m < mCounts[1]; m++)
			{
				GrapaVectorParam p3(pScriptExec, mData, mBlock, (i * mCounts[1] + m));
				GrapaFloat f = (*p3.aa / *p1.aa);
				Set((i * mCounts[1] + m), f);
			}
			Set((i * mCounts[1] + j), GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 1));
			for (s64 n = i - 1; n >= 0; n--)
			{
				GrapaVectorParam p2(pScriptExec, mData, mBlock, (n * mCounts[1] + j));
				for (u64 m = j + 1; m < mCounts[1]; m++)
				{
					GrapaVectorParam p3(pScriptExec, mData, mBlock, (n * mCounts[1] + m));
					GrapaVectorParam p4(pScriptExec, mData, mBlock, (i * mCounts[1] + m));
					GrapaFloat f = *p3.aa - *p2.aa * *p4.aa;
					Set((n * mCounts[1] + m), f);
				}
				Set((n * mCounts[1] + j), GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0));
			}
			break;
		}
	}

	return true;
}

bool GrapaVector::Inv(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace)
{
	if (mDim != 2)
		return false;

	if (mCounts[0] != mCounts[1])
		return false;

	GrapaVectorItem* data = mData;
	mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize * 2);
	for (u64 i = 0; i < mCounts[0]; i++)
	{
		memcpy(_datavectorpos(mData, mBlock, (i * mCounts[1] * 2)), _datavectorpos(data, mBlock, (i * mCounts[1])), mCounts[1] * mBlock);
		for (u64 j = 0; j < mCounts[1]; j++)
		{
			if (i==j)
				Set((i * mCounts[1] * 2 + mCounts[1] + j), GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 1));
			else
				Set((i * mCounts[1] * 2 + mCounts[1] + j), GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0));
		}
	}
	GrapaMem::Delete(data);
	mSize *= 2;
	mCounts[1] *= 2;

	Rref(pScriptExec, pNameSpace);

	data = mData;
	mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize * 2);
	mSize /= 2;
	mCounts[1] /= 2;
	for (u64 i = 0; i < mCounts[0]; i++)
		memcpy(_datavectorpos(mData, mBlock, (i * mCounts[1])), _datavectorpos(data, mBlock, (i * mCounts[1] * 2 + mCounts[1])), mCounts[1] * mBlock);
	GrapaMem::Delete(data);

	return true;
}

GrapaFloat GrapaVector::Determinant(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace)
{
	GrapaVector v(*this);
	if (!v.Ref(pScriptExec, pNameSpace))
		return GrapaFloat(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	GrapaFloat f(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 1);
	for (u64 i = 0; i < v.mCounts[0]; i++)
	{
		GrapaVectorParam p1(pScriptExec, v.mData, v.mBlock, (i * v.mCounts[1] + i));
		f = f * *p1.aa;
	}
	return f;
}

GrapaInt GrapaVector::Rank(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace)
{
	if (mDim != 2)
		return 0;

	GrapaVector v(*this);
	v.Ref(pScriptExec, pNameSpace, true);

	return v.mCounts[0];
}

void GrapaVector::Identity(GrapaScriptExec* pScriptExec, u64 n)
{
	CLEAR();
	GrapaFloat zero(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	GrapaFloat one(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 1);
	mBlock = _minvectorblock_;
	mMaxBlock = _minvectordatablock_;
	mSetBlock = 0;
	mDim = 2;
	mCounts = (u64*)GrapaMem::Create(mDim * sizeof(u64));
	mCounts[0] = n;
	mCounts[1] = n;
	mSize = n * n;
	mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
	for (u64 i = 0; i < n; i++)
	{
		for (u64 j = 0; j < n; j++)
		{
			if (i==j)
				Set((i * mCounts[1] + j), one);
			else
				Set((i * mCounts[1] + j), zero);
		}
	}
}

bool GrapaVector::Diagonal(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result)
{
	result.CLEAR();
	if (mDim != 2)
		return false;
	result.mBlock = _minvectorblock_;
	result.mMaxBlock = _minvectordatablock_;
	result.mSetBlock = 0;
	result.mDim = 1;
	u64 size = 0;
	if (n >= 0)
	{
		size = mCounts[1];
		if (n > (s64)size)
			size = 0;
		else
			size -= n;
		if (size > mCounts[0])
			size = mCounts[0];
	}
	else
	{
		size = mCounts[0];
		if ((-n) > (s64)size)
			size = 0;
		else
			size += n;
		if (size > mCounts[1])
			size = mCounts[1];
	}
	result.mSize = size;
	result.mCounts = (u64*)GrapaMem::Create(result.mDim * sizeof(u64));
	result.mCounts[0] = size;
	result.mData = (GrapaVectorItem*)GrapaMem::Create(result.mBlock * result.mSize);
	if (n >= 0)
	{
		for (u64 i = 0; i < size; i++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + i + n));
			result.Set((i), *p1.aa);
		}
	}
	else
	{
		for (u64 i = 0; i < size; i++)
		{
			GrapaVectorParam p1(pScriptExec, mData, mBlock, ((i-n) * mCounts[1] + i));
			result.Set((i), *p1.aa);
		}
	}
	return true;
}

bool GrapaVector::TriU(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result)
{
	result.CLEAR();
	if (mDim != 2)
		return false;
	GrapaFloat zero(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	result.mBlock = mBlock;
	result.mMaxBlock = mMaxBlock;
	result.mSetBlock = 0;
	result.mDim = 2;
	result.mCounts = (u64*)GrapaMem::Create(mDim * sizeof(u64));
	result.mCounts[0] = mCounts[0];
	result.mCounts[1] = mCounts[1];
	result.mSize = mCounts[0] * mCounts[1];
	result.mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
	memset(result.mData, 0, mSize * mBlock);
	memcpy(result.mCounts, mCounts, mDim * sizeof(u64));
	for (s64 i = 0; i < (s64)mCounts[0]; i++)
	{
		for (s64 j = 0; j < (s64)mCounts[1]; j++)
		{
			if ((j - i) < n)
				result.Set((i * result.mCounts[1] + j), zero);
			else
			{
				GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
				result.Set((i * result.mCounts[1] + j), *p1.aa);
			}
		}
	}
	return true;
}

bool GrapaVector::TriL(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result)
{
	result.CLEAR();
	if (mDim != 2)
		return false;
	GrapaFloat zero(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	result.mBlock = mBlock;
	result.mMaxBlock = mMaxBlock;
	result.mSetBlock = 0;
	result.mDim = 2;
	result.mCounts = (u64*)GrapaMem::Create(mDim * sizeof(u64));
	result.mCounts[0] = mCounts[0];
	result.mCounts[1] = mCounts[1];
	result.mSize = mCounts[0] * mCounts[1];
	result.mData = (GrapaVectorItem*)GrapaMem::Create(mBlock * mSize);
	memset(result.mData, 0, mSize * mBlock);
	memcpy(result.mCounts, mCounts, mDim * sizeof(u64));
	for (s64 i = 0; i < (s64)mCounts[0]; i++)
	{
		for (s64 j = 0; j < (s64)mCounts[1]; j++)
		{
			if ((j - i) < (n + 1))
			{
				GrapaVectorParam p1(pScriptExec, mData, mBlock, (i * mCounts[1] + j));
				result.Set((i * result.mCounts[1] + j), *p1.aa);
			}
			else
				result.Set((i * result.mCounts[1] + j), zero);
		}
	}
	return true;
}

bool GrapaVector::EigH(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& w, GrapaVector& p)
{
	w.CLEAR();
	p.CLEAR();
	if (mDim != 2)
		return false;

	GrapaVector A(*this);

	u64 n = A.mCounts[0];
	u64 maxRot = 5 * (n * n);
	p.Identity(pScriptExec, n);
	GrapaFloat zero(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 0);
	GrapaFloat one(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 1);
	GrapaFloat two(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 2);
	GrapaFloat ten(pScriptExec->vScriptState->mItemState.mFloatFix, pScriptExec->vScriptState->mItemState.mFloatMax, pScriptExec->vScriptState->mItemState.mFloatExtra, 10);
	GrapaFloat tol = ten.Pow(-9);
	GrapaFloat tenm36 = ten.Pow(-36);
	for (u64 i = 0; i < maxRot; i++)
	{
		GrapaFloat Amax = zero;
		u64 k = 0;
		u64 l = 0;
		for (u64 ii = 0; ii < (n - 1); ii++)
		{
			for (u64 jj = ii + 1; jj < n; jj++)
			{
				GrapaVectorParam p1(pScriptExec, A.mData, A.mBlock, (ii * A.mCounts[1] + jj));
				if (p1.aa->Abs() >= Amax)
				{
					Amax = p1.aa->Abs();
					k = ii;
					l = jj;
				};
			}
		}
		if (Amax < tol)
		{
			A.Diagonal(pScriptExec, 0, w);
			GrapaVector vv(w);
			vv.Sort(pScriptExec, pNameSpace, &p, NULL, NULL, false, true, true, NULL, w);
			return true;
		}
		else
		{
			GrapaVectorParam ll(pScriptExec, A.mData, A.mBlock, (l * A.mCounts[1] + l));
			GrapaVectorParam kk(pScriptExec, A.mData, A.mBlock, (k * A.mCounts[1] + k));
			GrapaVectorParam kl(pScriptExec, A.mData, A.mBlock, (k * A.mCounts[1] + l));
			GrapaFloat Adiff = *ll.aa - *kk.aa;
			GrapaFloat t = zero;
			if (kl.aa->Abs() < (Adiff.Abs() * tenm36))
				t = *kl.aa / Adiff;
			else
			{
				GrapaFloat phi = Adiff / (*kl.aa * two);
				t = one / (phi.Abs() + ((phi * phi + one).Root(2)));
				if (phi < zero) t = -t;
			}
			GrapaFloat c = one / ((t * t + one).Root(2));
			GrapaFloat s = t * c;
			GrapaFloat tau = s / (one + c);
			GrapaFloat temp = *kl.aa;
			A.Set((k * A.mCounts[1] + l), zero);
			A.Set((k * A.mCounts[1] + k), *kk.aa - t * temp);
			A.Set((l * A.mCounts[1] + l), *ll.aa + t * temp);
			for (u64 ii = 0; ii < k; ii++)
			{
				GrapaVectorParam ik(pScriptExec, A.mData, A.mBlock, (ii * A.mCounts[1] + k));
				GrapaVectorParam il(pScriptExec, A.mData, A.mBlock, (ii * A.mCounts[1] + l));
				GrapaFloat temp = *ik.aa;
				A.Set((ii * A.mCounts[1] + k), temp - s * (*il.aa + tau * temp));
				A.Set((ii * A.mCounts[1] + l), *il.aa + s * (temp - tau * *il.aa));
			}
			for (u64 ii = (k + 1); ii < l; ii++)
			{
				GrapaVectorParam ki(pScriptExec, A.mData, A.mBlock, (k * A.mCounts[1] + ii));
				GrapaVectorParam il(pScriptExec, A.mData, A.mBlock, (ii * A.mCounts[1] + l));
				GrapaFloat temp = *ki.aa;
				A.Set((k * A.mCounts[1] + ii), temp - s * (*il.aa + tau * *ki.aa));
				A.Set((ii * A.mCounts[1] + l), *il.aa + s * (temp - tau * *il.aa));
			}
			for (u64 ii = (l + 1); ii < n; ii++)
			{
				GrapaVectorParam ki(pScriptExec, A.mData, A.mBlock, (k * A.mCounts[1] + ii));
				GrapaVectorParam li(pScriptExec, A.mData, A.mBlock, (l * A.mCounts[1] + ii));
				GrapaFloat temp = *ki.aa;
				A.Set((k * A.mCounts[1] + ii), temp - s * (*li.aa + tau * temp));
				A.Set((l * A.mCounts[1] + ii), *li.aa + s * (temp - tau * *li.aa));
			}
			for (u64 ii = 0; ii < n; ii++)
			{
				GrapaVectorParam ik(pScriptExec, p.mData, p.mBlock, (ii * p.mCounts[1] + k));
				GrapaVectorParam il(pScriptExec, p.mData, p.mBlock, (ii * p.mCounts[1] + l));
				GrapaFloat temp = *ik.aa;
				p.Set((ii * p.mCounts[1] + k), temp - s * (*il.aa + tau * *ik.aa));
				p.Set((ii * p.mCounts[1] + l), *il.aa + s * (temp - tau * *il.aa));
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
