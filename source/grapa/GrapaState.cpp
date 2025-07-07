// GrapaState.cpp
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

// http://en.wikipedia.org/wiki/Order_of_operations

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "GrapaState.h"
#include "GrapaSystem.h"
#include "GrapaMem.h"
#include <errno.h>
#include "GrapaPrime.h"
#include "GrapaCompress.h"
#include "GrapaTinyAES.h"
#include "GrapaEncode.h"
#include "GrapaHash.h"
#include "GrapaFloat.h"
#include "GrapaVector.h"
#include "GrapaWidget.h"
#include "GrapaNetConnect.h"
#include "GrapaTime.h"

#include "GrapaLibRule.h"

extern GrapaSystem* gSystem;

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaObjectIndexQueue::GrapaObjectIndexQueue()
{
	mBtree.SetFile(&mCache);
	GrapaError err = mBtree.Create("temp");
	mBtree.CloseFile();
	err = mBtree.OpenFile("temp", GrapaReadWriteCreate);
	u8 treeType;
	mRoot = mBtree.RootTree(treeType);
	err = mBtree.NewTree(mRoot, GrapaBtree::SU64_TREE);
	err = mBtree.SetFileTree(mRoot);
}

void GrapaObjectIndexQueue::PushHead(GrapaObjectEvent* pEvent)
{
	GrapaObjectQueue::PushHead(pEvent);
	GrapaCursor c;
	c.Set(mRoot, GrapaBtree::SU64_ITEM, pEvent->mId, (u64)pEvent);
    GrapaError err = 0;
    err = mBtree.Insert(c);
}

void GrapaObjectIndexQueue::PushTail(GrapaObjectEvent* pEvent)
{
	GrapaObjectQueue::PushTail(pEvent);
	GrapaCursor c;
	c.Set(mRoot, GrapaBtree::SU64_ITEM, pEvent->mId, (u64)pEvent);
    GrapaError err = 0;
    err = mBtree.Insert(c);
}

void GrapaObjectIndexQueue::PushNext(GrapaObjectEvent* pLocation, GrapaObjectEvent* pEvent)
{
	GrapaObjectQueue::PushNext(pLocation, pEvent);
	GrapaCursor c;
	c.Set(mRoot, GrapaBtree::SU64_ITEM, pEvent->mId, (u64)pEvent);
    GrapaError err = 0;
    err = mBtree.Insert(c);
}

GrapaObjectEvent* GrapaObjectIndexQueue::Search(u8 pToken, const GrapaBYTE& pName)
{
	GrapaCursor c;
	c.Set(mRoot, GrapaBtree::SU64_ITEM, HashId(pName, pToken));
	GrapaError err = mBtree.Search(c);
	if (err) return(NULL);
	return((GrapaObjectEvent*)c.mValue);
}

GrapaObjectEvent* GrapaObjectIndexQueue::Search(u8 pToken, const char* pName)
{
	return Search(pToken, GrapaBYTE(pName));
}

GrapaObjectEvent* GrapaObjectIndexQueue::Search(u64 pId)
{
	GrapaCursor c;
	c.Set(mRoot, GrapaBtree::SU64_ITEM, pId);
	GrapaError err = mBtree.Search(c);
	if (err) return(NULL);
	return((GrapaObjectEvent*)c.mValue);
}

//void GrapaObjectIndexQueue::PrintList()
//{
//	GrapaCursor c;
//	GrapaError err;
//	c.Set(mRoot);
//	err = mBtree.First(c);
//	int i = 1;
//	while (err == 0)
//	{
//
//		gSystem->Print((u64)100 + (i++));
//		gSystem->Print(",");
//		gSystem->PrintLine(c.mKey);
//		err = mBtree.Next(c);
//	}
//}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaError GrapaCoreCHAR::CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result)
{
	GrapaCHAR* a = (GrapaCHAR*)dataCursor.mKey;
	GrapaCHAR* b = (GrapaCHAR*)treeCursor.mKey;
	s64 c = a->Cmp(*b);
	if (c < 0) result = GrapaBtree::CMP_LT;
	else if (c > 0) result = GrapaBtree::CMP_GT;
	else result = GrapaBtree::CMP_EQ;
	return(0);
}

GrapaKeyValue::GrapaKeyValue()
{
	mBtree.SetFile(&mCache);
	GrapaError err = mBtree.Create("temp");
	mBtree.CloseFile();
	err = mBtree.OpenFile("temp", GrapaReadWriteCreate);
	u8 treeType;
	mRoot = mBtree.RootTree(treeType);
	err = mBtree.NewTree(mRoot, GrapaBtree::SU64_TREE);
	err = mBtree.SetFileTree(mRoot);
}

void GrapaKeyValue::Set(GrapaRuleEvent* pValue)
{
	GrapaCursor c;
	c.Set(mRoot, 99, (u64)&pValue->mName);
	GrapaError err = mBtree.Search(c);
	if (err == 0) mBtree.Delete(c);
	c.Set(mRoot, 99, (u64)&pValue->mName, (u64)pValue);
	err = mBtree.Insert(c);
}

GrapaRuleEvent* GrapaKeyValue::Get(const GrapaCHAR& pKey)
{
	GrapaCursor c;
	c.Set(mRoot, 99, (u64)&pKey);
	GrapaError err = mBtree.Search(c);
	if (err) return(NULL);
	return((GrapaRuleEvent*)c.mValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaItemState::SearchToken(u64 pId)
{
	GrapaRuleEvent* parameter = NULL;
	GrapaNames* names = mNameSpace->GetSkipThis();
	while (!parameter && names)
	{
		GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
		while (operation)
		{
			GrapaRuleEvent* op = operation;
			while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
			parameter = (GrapaRuleEvent*)(op->vQueue ? op->vQueue->Search(pId) : NULL);
			if (parameter) return(parameter);
			operation = operation->Prev();
		}
		names = names->GetSkipParrent();
	}
	return(parameter);
}

GrapaRuleEvent* GrapaItemState::SearchToken(u8 pType, const GrapaBYTE& pValue)
{
	GrapaNames* names = mNameSpace->GetSkipThis();
	while (names)
	{
		GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
		while (operation)
		{
			GrapaRuleEvent* op = operation;
			while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
			if (op->vQueue)
			{
				GrapaRuleEvent* q = ((GrapaRuleQueue*)op->vQueue)->Head();
				while (q)
				{
					if (q->mValue.mToken == GrapaTokenType::TOKEN && q->mT == pType && q->mValue.Cmp(pValue) == 0)
						return(q);
					q = q->Next();
				}
			}
			operation = operation->Prev();
		}
		names = names->GetSkipParrent();
	}
	return(NULL);
}

void GrapaItemState::PushOutput(u8 pToken, const GrapaBYTE& pValue, char quote)
{
	bool sendRaw = true;
	bool isStrChar = false;
	bool sentStr = false;

	u64 len = pValue.mLength;
	char* value = (char*)pValue.mBytes;
	switch (pToken)
	{
	case GrapaTokenType::STR:
	case GrapaTokenType::SYSSTR:
		isStrChar = true;
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
	case GrapaTokenType::INT:
	case GrapaTokenType::RAW:
	case GrapaTokenType::SYSINT:
	case GrapaTokenType::FLOAT:
	case GrapaTokenType::SYM:
	case GrapaTokenType::SYSSYM:
	case GrapaTokenType::OTHER:
		//if (GrapaRuleEvent * sval = SearchToken(mItemParams->mMapQueue.HashId(pValue, pToken)))
		if (GrapaRuleEvent * sval = SearchToken(pToken,pValue))
		{
			if (mOutput)
			{
				GrapaCHAR valueStr(value, len);
				valueStr.mToken = sval->mT;
				GrapaRuleEvent* te = new GrapaRuleEvent(sval->mId, sval->mName, valueStr);
				te->mSkip = sval->mSkip;
				te->mRun = sval->mRun;
				te->mStart = sval->mStart;
				te->mEnd = sval->mEnd;
				te->mEscape = sval->mEscape;
				te->mExit = sval->mExit;
				te->mNull = sval->mNull;
				te->mQuote = sval->mQuote;
				if (mOutput->mState && sval->mEnd)
					mLock.WaitCritical();
				PushOutputTail(te);
				if (mOutput->mState && sval->mEnd)
					mLock.WaitCondition();
			}
			len -= sval->mValue.mLength;
			if (pToken == GrapaTokenType::SYSID) pToken = GrapaTokenType::ID;
			else if (pToken == GrapaTokenType::SYSSTR) pToken = GrapaTokenType::STR;
			else if (pToken == GrapaTokenType::SYSINT) pToken = GrapaTokenType::INT;
			else if (pToken == GrapaTokenType::SYSSYM) pToken = GrapaTokenType::SYM;
			break;
		}
		//else if (GrapaRuleEvent* sval = mItemParams->mMapQueue.Search(pToken, pValue))
		//{
		//	if (mOutput)
		//	{
		//		GrapaCHAR valueStr(value, len);
		//		valueStr.mToken = sval->mValue.mToken;
		//		GrapaObjectEvent* te = new GrapaObjectEvent(sval->mId, sval->mName, valueStr);
		//		te->mSkip = sval->mSkip;
		//		te->mRun = sval->mRun;
		//		te->mStart = sval->mStart;
		//		te->mEnd = sval->mEnd;
		//		te->mEscape = sval->mEscape;
		//		te->mExit = sval->mExit;
		//		te->mNull = sval->mNull;
		//		if (mOutput->mState && sval->mEnd)
		//			mLock.WaitCritical();
		//		PushOutputTail(te);
		//		if (mOutput->mState && sval->mEnd)
		//			mLock.WaitCondition();
		//	}
		//	len -= sval->mValue.mLength;
		//	if (pToken == GrapaTokenType::SYSID) pToken = GrapaTokenType::ID;
		//	else if (pToken == GrapaTokenType::SYSSTR) pToken = GrapaTokenType::STR;
		//	else if(pToken == GrapaTokenType::SYSINT) pToken = GrapaTokenType::INT;
		//	else if (pToken == GrapaTokenType::SYSSYM) pToken = GrapaTokenType::SYM;
		//	break;
		//}
		break;
	default:
		break;
	}

	if (len || (len&&isStrChar) || (isStrChar&&!sentStr) || pToken == GrapaTokenType::INT || pToken == GrapaTokenType::RAW || pToken == GrapaTokenType::SYSINT || pToken == GrapaTokenType::FLOAT)
	{
		GrapaCHAR name;
		GrapaCHAR valueStr(value, len);
		valueStr.mToken = pToken;
		GrapaRuleEvent* te = new GrapaRuleEvent(0, name, valueStr);
		te->mSkip = !sendRaw;
		te->mQuote = quote;
		if (pToken == GrapaTokenType::SYSINT && len == 0)
			te->mNull = true;
		PushOutputTail(te);
	}
}

void GrapaItemState::SetParam(u64 pLexParam, const char* pValue, bool pTouched)
{
	if (pLexParam>=GrapaItemEnum::MAX) return;
	u64 len = (pValue) ? strlen(pValue) : 0;
	if (len)
	{
		if (len >(sizeof(mItemParams->mParam[pLexParam]) - 1)) len = sizeof(mItemParams->mParam[pLexParam]) - 1;
		GrapaMem::MemCopy(mItemParams->mParam[pLexParam], pValue, sizeof(mItemParams->mParam[pLexParam])<(len + 1) ? sizeof(mItemParams->mParam[pLexParam]) : (len + 1));
	}
	mItemParams->mParam[pLexParam][len] = 0;
	mItemParams->mTouched[pLexParam] = pTouched;
}

GrapaRuleEvent* GrapaItemState::AddToken(u8 pToken, const char* pName, bool pSkip, const GrapaBYTE& pValue)
{
	GrapaRuleEvent* token = NULL;
	u64 pTokenId = mItemParams->mMapQueue.HashId(pValue, pToken);
	GrapaCHAR name(pName);
	if (pValue.mLength && (pToken == GrapaTokenType::INT || pToken == GrapaTokenType::FLOAT))
	{
		GrapaInt a;
		a.FromString(pValue, 10);
		token = new GrapaRuleEvent(pTokenId, name, a.getBytes());
	}
	else
		token = new GrapaRuleEvent(pTokenId, name, pValue);
	if (token==NULL) return(NULL);
	token->mSkip = pSkip;
	token->mValue.mToken = pToken;
	GrapaRuleEvent *e = mItemParams->mMapQueue.Tail();
	while(e)
	{
		if (e->mValue.mLength <= token->mValue.mLength)
		{
			mItemParams->mMapQueue.PushNext(e,token);
			return(token);
		}
		e = e->Prev();
	}
	mItemParams->mMapQueue.PushTail(token);
	return(token);
}

void GrapaItemState::Starting()
{
}

void GrapaItemState::ReplaceTagEscape(GrapaCHAR& pValue)
{
// http://www.ascii.cl/htmlcodes.htm
// "   &quot;
// '   &apos;
// <   &lt;
// >   &gt;
// &   &amp;
	// &#nnnn;
	// &#xhhhh;
	u64 j = 0;
	for (u64 i = 0; i < pValue.mLength; )
	{
		if (pValue.mBytes[i] == '&')
		{
			if (pValue.mLength >= (i + 5) && pValue.mBytes[i + 1] == '#' && pValue.mBytes[i + 2] != 'x')
			{
				char s[8];
				u64 size = 0;
				for (u64 k = i + 2; k < pValue.mLength; k++)
				{
					char c = pValue.mBytes[k];
					if (c == ';') break;
					s[size++] = c;
					if (size == 4) break;
				}
				pValue.mBytes[j++] = (u8)GrapaMem::s2au64(s, size);
				i += 3 + size;
				//if (i < pValue.mLength && pValue.mBytes[i] == ' ') i++;
			}
			else if (pValue.mLength >= (i + 6) && pValue.mBytes[i + 1] == '#' && pValue.mBytes[i + 2] == 'x')
			{
				//char s[8];
				u64 size = 0;
				u64 n = 0;
				for (u64 k = i + 3; k < pValue.mLength; k++)
				{
					u8 c = pValue.mBytes[k];
					if (c == ';') break;
					n *= 16;
					if (c <= '9') n += (((u64)c) - '0');
					else if (c <= 'Z')  n += (((u64)c) + 10 - 'A');
					else if (c <= 'z')  n += (((u64)c) + 10 - 'a');
					size++;
					if (size == 4) break;
				}
				pValue.mBytes[j++] = (u8)n;
				i += 4 + size;
				//if (i < pValue.mLength && pValue.mBytes[i] == ' ') i++;
			}
			else
			{
				if ((pValue.mLength >= (i + 6)) && (strncmp((char*)&pValue.mBytes[i], "&quot;", 6) == 0))
				{
					pValue.mBytes[j++] = '"';
					i += 6;
					//if (i < pValue.mLength && pValue.mBytes[i] == ' ') i++;
				}
				else if ((pValue.mLength >= (i + 6)) && (strncmp((char*)&pValue.mBytes[i], "&apos;", 6) == 0))
				{
					pValue.mBytes[j++] = '\'';
					i += 6;
					//if (i < pValue.mLength && pValue.mBytes[i] == ' ') i++;
				}
				else if ((pValue.mLength >= (i + 4)) && (strncmp((char*)&pValue.mBytes[i], "&lt;", 4) == 0))
				{
					pValue.mBytes[j++] = '<';
					i += 4;
					//if (i < pValue.mLength && pValue.mBytes[i] == ' ') i++;
				}
				else if ((pValue.mLength >= (i + 4)) && (strncmp((char*)&pValue.mBytes[i], "&gt;", 4) == 0))
				{
					pValue.mBytes[j++] = '>';
					i += 4;
					//if (i < pValue.mLength && pValue.mBytes[i] == ' ') i++;
				}
				else if ((pValue.mLength >= (i + 5)) && (strncmp((char*)&pValue.mBytes[i], "&amp;", 5) == 0))
				{
					pValue.mBytes[j++] = '&';
					i += 5;
					//if (i < pValue.mLength && pValue.mBytes[i] == ' ') i++;
				}
				else
				{
					pValue.mBytes[j++] = pValue.mBytes[i++];
				}
			}
		}
		else
		{
			pValue.mBytes[j++] = pValue.mBytes[i++];
		}
	}
	pValue.SetLength(j);
}

#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

// Helper: encode a single code point as UTF-8
std::string utf8_encode(uint32_t codepoint) {
	std::string out;
	if (codepoint <= 0x7F) {
		out += static_cast<char>(codepoint);
	}
	else if (codepoint <= 0x7FF) {
		out += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
		out += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0xFFFF) {
		out += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
		out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0x10FFFF) {
		out += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
		out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
		out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	return out;
}

// Convert ASCII hex buffer to UTF-8 string (4 bytes per code point)
std::string hex_ascii_4byte_to_utf8(const unsigned char* buf, size_t len) {
	std::string out;
	for (size_t i = 0; i + 7 < len; i += 8) {
		// Read 8 ASCII hex digits (4 bytes)
		std::string hexstr(reinterpret_cast<const char*>(buf + i), 8);
		uint32_t codepoint = std::stoul(hexstr, nullptr, 16);
		out += utf8_encode(codepoint);
	}
	return out;
}

// For 4 hex digits (2 bytes, e.g. "0301"), use this:
std::string hex_ascii_2byte_to_utf8(const unsigned char* buf, size_t len) {
	std::string out;
	for (size_t i = 0; i + 3 < len; i += 4) {
		std::string hexstr(reinterpret_cast<const char*>(buf + i), 4);
		uint32_t codepoint = std::stoul(hexstr, nullptr, 16);
		out += utf8_encode(codepoint);
	}
	return out;
}

std::string hex_ascii_8byte_to_utf8(const unsigned char* buf, size_t len) {
	std::string out;
	for (size_t i = 0; i + 15 < len; i += 16) {
		// Read 16 ASCII hex digits (8 bytes)
		std::string hexstr(reinterpret_cast<const char*>(buf + i), 16);
		uint64_t codepoint = std::stoull(hexstr, nullptr, 16);
		out += utf8_encode(static_cast<uint32_t>(codepoint & 0x1FFFFF));
	}
	return out;
}

// Convert a single ASCII hex digit to its integer value
unsigned char hex_digit_to_int(char c) {
	if ('0' <= c && c <= '9') return c - '0';
	if ('a' <= c && c <= 'f') return 10 + (c - 'a');
	if ('A' <= c && c <= 'F') return 10 + (c - 'A');
	throw std::invalid_argument("Invalid hex digit");
}

// Convert two ASCII hex characters to a single byte
unsigned char hex_pair_to_byte(char high, char low) {
	return (hex_digit_to_int(high) << 4) | hex_digit_to_int(low);
}

void GrapaItemState::Running()
{
	GrapaCHAR msgStr, expStr;
	bool expNeg = false, isEl = false, isEndEl = false, isScriptEl = false;
	bool noEscape = false;
	int isComment = 0;
	int isCDATA = 0;
	char* from;
	class GrapaTokenItemType : public GrapaTokenType { public:enum{LEX_START = GrapaTokenType::MAX, STRING_COLLECT, ESCAPE, ESCAPEX, ESCAPEO, ESCAPEERR, NUMHEX, NUMBIN, SCOPE, EXP, IDDASH, ELESC, ELSTART };};
	u32 state = GrapaTokenItemType::START;
	u32 sendState = GrapaTokenItemType::START, saveState = GrapaTokenItemType::START;
	u64 maxPos = 0, savePos = 0;
	GrapaInt a, b;
	char saveChar = 0;
	unsigned char savehex[8];
	std::string saveustr;
	char quote = 0;
	if (!mSync) WaitCritical();
	if (GetQueue()) { if (!mSync) GetQueue()->mState = this; else GetQueue()->mState = NULL; }
	msgStr.SetSize(256);
	msgStr.SetLength(0);
	expStr.SetSize(256);
	expStr.SetLength(0);
	SendCondition();
	while (!mStop)
	{
		GrapaItemEvent* nextValue = (GrapaItemEvent*) (GetQueue() ? GetQueue()->Head() : NULL);
		if (nextValue == NULL || (nextValue->mMessage.mPos && nextValue->Next() == NULL))
		{
			if (mStop || mSync)
			{
				if (msgStr.mLength) PushOutput(state, msgStr, quote);
				msgStr.SetLength(0);
				state = GrapaTokenType::START;
				mStop = true;
				break;
			}
			if (!mSync)
			{
				mSuspend.WaitCritical();
				nextValue = (GrapaItemEvent*)(GetQueue() ? GetQueue()->Head() : NULL);
				if (nextValue == NULL || (nextValue->mMessage.mPos && nextValue->Next() == NULL))
				{
					mSuspend.WaitCondition();
					mSuspend.WaitCritical();
					if (mClearState)
					{
						mClearState = false;
						expNeg = isEl = isEndEl = isScriptEl = false;
						isComment = 0;
						isCDATA = 0;
						state = sendState = saveState = GrapaTokenItemType::START;
						maxPos = savePos = 0;
						saveChar = 0;
						quote = 0;
						msgStr.SetLength(0);
						expStr.SetLength(0);
					}
				}
				mSuspend.LeaveCritical();
			}
			continue;
		}
		if (mStop) break;
		while (nextValue->mMessage.mLength != nextValue->mMessage.mPos)
		{
			char c = ((char*)nextValue->mMessage.mBytes)[nextValue->mMessage.mPos];
			switch (state)
			{
			case GrapaTokenType::START:
				if (strchr(mItemParams->mParam[GrapaItemEnum::STR], c))
					state = GrapaTokenType::STR;
				else if (strchr(mItemParams->mParam[GrapaItemEnum::SYM], c))
					state = GrapaTokenType::SYM;
				else if (strchr(mItemParams->mParam[GrapaItemEnum::DIG], c))
					state = GrapaTokenType::INT;
				else if (strchr(mItemParams->mParam[GrapaItemEnum::ID], c))
					state = GrapaTokenType::ID;
				else
					state = GrapaTokenType::OTHER;
				break;
			case GrapaTokenType::STR:
				quote = c;
				state = GrapaTokenItemType::STRING_COLLECT;
				msgStr.SetLength(0);
				nextValue->mMessage.mPos++;
				break; 
			case GrapaTokenItemType::STRING_COLLECT:
				if (noEscape)
				{
					if ((quote && c == quote) && !(msgStr.mLength && msgStr.mBytes[msgStr.mLength - 1] == '\\'))
					{
						PushOutput((sendState ? sendState : GrapaTokenType::STR), msgStr, quote);
						msgStr.SetLength(0);
						sendState = 0;
						noEscape = false;
						state = GrapaTokenType::START;
						nextValue->mMessage.mPos++;
					}
					else
					{
						nextValue->mMessage.mPos++;
						msgStr.Append(c);
					}
					break;
				}
				if (saveState == GrapaTokenItemType::ESCAPE)
				{
					saveState = 0;
					c = saveChar;
					switch (savePos)
					{
					case 2:
						nextValue->mMessage.mPos++;
						msgStr.Append(c);
						break;
					case 4:
					case 8:
						nextValue->mMessage.mPos++;
						msgStr.Append(saveustr.c_str(),saveustr.length());
						break;
					}
					if (savePos)
					{
						saveChar = 0;
						savePos = 0;
						maxPos = 0;
						saveustr.clear();
						break;
					}
					if (c == quote || strchr(mItemParams->mParam[GrapaItemEnum::STRESC], c))
					{
						nextValue->mMessage.mPos++;
						msgStr.Append(c);
						break;
					}
				}
				if (strchr(mItemParams->mParam[GrapaItemEnum::STRESC], c))
				{
					saveState = state;
					state = GrapaTokenItemType::ESCAPE;
					saveChar = 0;
					savePos = 0;
					nextValue->mMessage.mPos++;
				}
				else if (!quote && isEl && c == '>' && isComment == 3 && msgStr.mLength > 1 && msgStr.mBytes[msgStr.mLength - 1] == '-' && msgStr.mBytes[msgStr.mLength - 2] == '-')
				{
					msgStr.SetLength(msgStr.mLength - 2);
					if (isEl) ReplaceTagEscape(msgStr);
					PushOutput((sendState ? sendState : GrapaTokenType::STR), msgStr, quote);
					msgStr.FROM("-");
					PushOutput(GrapaTokenType::SYM, msgStr, quote);
					PushOutput(GrapaTokenType::SYM, msgStr, quote);
					isComment = 0;
					isCDATA = 0;
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
				}
				else if (!quote && isEl && c == '>' && isCDATA == 2 && msgStr.mLength > 1 && msgStr.mBytes[msgStr.mLength - 1] == ']' && msgStr.mBytes[msgStr.mLength - 2] == ']')
				{
					msgStr.SetLength(msgStr.mLength - 2);
					if (isEl) ReplaceTagEscape(msgStr);
					PushOutput((sendState ? sendState : GrapaTokenType::STR), msgStr,quote);
					msgStr.FROM("]");
					PushOutput(GrapaTokenType::SYM, msgStr, quote);
					PushOutput(GrapaTokenType::SYM, msgStr, quote);
					isComment = 0;
					isCDATA = 0;
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
				}
				else if (quote && c == quote)
				{
					PushOutput((sendState ? sendState : GrapaTokenType::STR), msgStr, quote);
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
					nextValue->mMessage.mPos++;
				}
				else if (!quote && isEl && isComment != 3 && c == '<')
				{
					saveState = state;
					state = GrapaTokenItemType::ELSTART;
					saveChar = c;
					savePos = 0;
					nextValue->mMessage.mPos++;
					expStr.SetLength(0);
					isEndEl = false;
				}
				// Disabled for now...as this causes $&#x0D; to error. Can't support the escaping within...need to revisit if this is needed.
				else if (msgStr.mLength==0 && !quote && isEl && isComment != 3 && c == '$' && !isScriptEl)
				{
					saveState = state;
					state = GrapaTokenItemType::ELESC;
					saveChar = c;
					savePos = 0;
					nextValue->mMessage.mPos++;
				}
				else
				{
					nextValue->mMessage.mPos++;
					msgStr.Append(c);
				}
				break;
			case GrapaTokenItemType::ELESC:
				if (saveChar == '$' && c != '&')
				{
					msgStr.Append(saveChar);
					saveChar = 0;
					state = saveState;
					saveState = 0;
					break;
				}
				else
				{
					bool hasInfo = false;
					for (u64 i = 0; !hasInfo && i < msgStr.mLength; i++) switch (msgStr.mBytes[i]) { case ' ': case '\r': case '\n': case '\t': break; default: hasInfo = true; break; }
					if (hasInfo)
					{
						ReplaceTagEscape(msgStr);
						PushOutput((sendState ? sendState : GrapaTokenType::STR), msgStr, quote);
					}
					msgStr.SetLength(0);
					if (saveChar == '$')
					{
						isEl = !isEl;
						nextValue->mMessage.mPos++;
					}
					msgStr.SetLength(0);
					sendState = 0;
					saveChar = 0;
					noEscape = false;
					state = GrapaTokenType::START;
				}
				break;
			case GrapaTokenItemType::ELSTART:
				if (expStr.mLength==0 && saveChar == '<' && (c == '<' || c == ' '))
				{
					msgStr.Append(saveChar);
					saveChar = 0;
					state = saveState;
					saveState = 0;
					break;
				}
				else if (expStr.mLength == 0 && c == '/')
				{
					isEndEl = true;
					nextValue->mMessage.mPos++;
					break;
				}
				// if scriptMode, then wait for </script>
				else if (expStr.mLength == 0 && (strchr(mItemParams->mParam[GrapaItemEnum::ID], c) || strchr(mItemParams->mParam[GrapaItemEnum::DIG], c)))
				{
					expStr.Append(c);
					nextValue->mMessage.mPos++;
					break;
				}
				else if (expStr.mLength == 0 && isScriptEl && !(strchr(mItemParams->mParam[GrapaItemEnum::ID], c) || strchr(mItemParams->mParam[GrapaItemEnum::DIG], c)))
				{
					msgStr.Append(saveChar);
					if (isEndEl)
						msgStr.Append("/");
					isEndEl = false;
					saveChar = 0;
					state = saveState;
					saveState = 0;
					break;
				}
				else if (expStr.mLength && (strchr(mItemParams->mParam[GrapaItemEnum::ID], c) || strchr(mItemParams->mParam[GrapaItemEnum::DIG], c)))
				{
					expStr.Append(c);
					nextValue->mMessage.mPos++;
					break;
				}
				else if (expStr.mLength && isScriptEl && (!isEndEl || expStr.Cmp("script")))
				{
					msgStr.Append(saveChar);
					if (isEndEl)
						msgStr.Append("/");
					isEndEl = false;
					if (expStr.mLength)
						msgStr.Append(expStr);
					expStr.SetLength(0);
					saveChar = 0;
					state = saveState;
					saveState = 0;
					break;
				}
				else
				{
					bool hasInfo = false;
					for (u64 i = 0; !hasInfo && i < msgStr.mLength; i++) switch (msgStr.mBytes[i]) { case ' ': case '\r': case '\n': case '\t': break; default: hasInfo = true; break; }
					if (hasInfo)
					{
						ReplaceTagEscape(msgStr);
						PushOutput((sendState ? sendState : GrapaTokenType::STR), msgStr, quote);
					}
					msgStr.SetLength(0);
					if (saveChar == '<')
					{
						msgStr.FROM("<");
						PushOutput(GrapaTokenType::SYM, msgStr, quote);
						msgStr.SetLength(0);
					}
					if (isEndEl && isScriptEl && expStr.mLength && expStr.Cmp("script") == 0)
					{
						isScriptEl = false;
					}
					else if (c != '/' && !isEndEl && !isScriptEl && expStr.mLength && expStr.Cmp("script") == 0)
					{
						isScriptEl = true;
					}
					if (isEndEl)
					{
						msgStr.FROM("/");
						PushOutput(GrapaTokenType::SYM, msgStr, quote);
						msgStr.SetLength(0);
						isEndEl = false;
					}
					if (expStr.mLength)
					{
						PushOutput(GrapaTokenType::ID, expStr, quote);
						expStr.SetLength(0);
					}
					sendState = 0;
					saveChar = 0;
					noEscape = false;
					state = GrapaTokenType::START;
				}
				break;
			case GrapaTokenItemType::ESCAPE:
				if (char *from = strchr(mItemParams->mParam[GrapaItemEnum::STRFROM], c))
				{
					saveChar = mItemParams->mParam[GrapaItemEnum::STRTO][from - mItemParams->mParam[GrapaItemEnum::STRFROM]];
					state = saveState;
					saveState = GrapaTokenItemType::ESCAPE;
					break;
				}
				else if (c == '0')   // could make the \0 configurable
				{
					saveChar = 0;
					state = saveState;
					saveState = GrapaTokenItemType::ESCAPE;
					break;
				}
				else if ((c == 'x') || (c == 'X'))
				{
					state = GrapaTokenItemType::ESCAPEX;
					maxPos = 2;
					savePos = 0;
					saveChar = 0;
				}
				else if (c == 'u')
				{
					state = GrapaTokenItemType::ESCAPEX;
					maxPos = 4;
					savePos = 0;
					saveChar = 0;
				}
				else if (c == 'U')
				{
					state = GrapaTokenItemType::ESCAPEX;
					maxPos = 8;
					savePos = 0;
					saveChar = 0;
				}
				else if (strchr(mItemParams->mParam[GrapaItemEnum::OCT], c))
				{
					state = GrapaTokenItemType::ESCAPEO;
					break;
				}
				else
				{
					saveChar = 0;
					state = saveState;
					saveState = GrapaTokenItemType::ESCAPEERR;
				}
				nextValue->mMessage.mPos++;
				break;
			case GrapaTokenItemType::ESCAPEX:
				if (char *from = strchr(mItemParams->mParam[GrapaItemEnum::HEX], c))
				{
					savehex[savePos] = c;
					savePos++;
					if (savePos == maxPos)
					{
						switch (savePos)
						{
						case 2:
							saveChar = hex_pair_to_byte(savehex[0],savehex[1]);
							state = saveState;
							saveState = GrapaTokenItemType::ESCAPE;
							break;
						case 4:
							saveustr = hex_ascii_2byte_to_utf8(savehex, savePos);
							saveChar = 0; // unicode not supported yet
							state = saveState;
							saveState = GrapaTokenItemType::ESCAPE;
							break;
						case 8:
							saveustr = hex_ascii_4byte_to_utf8(savehex, savePos);
							saveChar = 0; // unicode not supported yet
							state = saveState;
							saveState = GrapaTokenItemType::ESCAPE;
							break;
						}
						break;
						//if (maxPos == 2) break;  // only supporting \xXX right now
					}
				}
				else
				{
					saveChar = 0;
					savePos = 0;
					state = saveState;
					saveState = GrapaTokenItemType::ESCAPEERR;
					break;
				}
				nextValue->mMessage.mPos++;
				break;
			case GrapaTokenItemType::ESCAPEO:
				if (char *from = strchr(mItemParams->mParam[GrapaItemEnum::OCT], c))
				{
					u64 addNum = (from - mItemParams->mParam[GrapaItemEnum::OCT]);
					saveChar = (char)(((u64)((((u64)saveChar) << ((u64)3)))) | addNum);
					savePos++;
					if (savePos == 3)
					{
						state = saveState;
						saveState = GrapaTokenItemType::ESCAPE;
						break;
					}
				}
				else
				{
					saveChar = 0;
					state = saveState;
					saveState = GrapaTokenItemType::ESCAPEERR;
					break;
				}
				nextValue->mMessage.mPos++;
				break;
			case GrapaTokenType::SYM:
				if (sendState == 0 && msgStr.mLength && strchr(mItemParams->mParam[GrapaItemEnum::SYS], ((char*)msgStr.mBytes)[msgStr.mLength - 1]) && ((strchr(mItemParams->mParam[GrapaItemEnum::DIG], c) || strchr(mItemParams->mParam[GrapaItemEnum::SYS], c))))
				{
					if (msgStr.mLength > 1)
					{
						msgStr.SetLength(msgStr.mLength - 1);
						PushOutput(state, msgStr, quote);
					}
					msgStr.SetLength(0);
					savePos = 0;
					saveChar = 0;
					if (strchr(mItemParams->mParam[GrapaItemEnum::SYS], c))
					{
						nextValue->mMessage.mPos++;
						msgStr.SetLength(0);
						PushOutput(GrapaTokenType::SYSINT, msgStr, quote);
						msgStr.SetLength(0);
						state = GrapaTokenType::START;
						break;
					}
					else
					{
						sendState = GrapaTokenType::SYSINT;
						state = GrapaTokenType::INT;
					}
					break;
				}
				else if (sendState == 0 && msgStr.mLength && strchr(mItemParams->mParam[GrapaItemEnum::SYS], ((char*)msgStr.mBytes)[msgStr.mLength - 1]) && (strchr(mItemParams->mParam[GrapaItemEnum::ID], c) || strchr(mItemParams->mParam[GrapaItemEnum::STR], c) || strchr(mItemParams->mParam[GrapaItemEnum::SYM], c)))
				{
					if (msgStr.mLength > 1)
					{
						msgStr.SetLength(msgStr.mLength - 1);
						PushOutput(state, msgStr, quote);
					}
					msgStr.SetLength(0);
					savePos = 0;
					saveChar = 0;
					if (strchr(mItemParams->mParam[GrapaItemEnum::ID], c))
					{
						sendState = GrapaTokenType::SYSID;
						state = GrapaTokenType::ID;
					}
					else if (strchr(mItemParams->mParam[GrapaItemEnum::STR], c))
					{
						sendState = GrapaTokenType::SYSSTR;
						state = GrapaTokenType::STR;
					}
					else if (c == '&')
					{
						isEl = !isEl;
						state = GrapaTokenType::START;
						nextValue->mMessage.mPos++;
						msgStr.SetLength(0);
						if (isEl)
						{
							state = GrapaTokenItemType::STRING_COLLECT;
							quote = 0;
						}
						break;
					}
					else if (strchr(mItemParams->mParam[GrapaItemEnum::SYM], c))
					{
						sendState = GrapaTokenType::SYSSYM;
						state = GrapaTokenType::SYM;
					}
					else
					{
						state = GrapaTokenType::START; // should never happen
					}
					break;
				}
				else if ((!strchr(mItemParams->mParam[GrapaItemEnum::SYM], c)) || (sendState && strchr(mItemParams->mParam[GrapaItemEnum::SYS], c)))
				{
					PushOutput((sendState ? sendState : state), msgStr, quote);
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
					break;
				}
				else if (strchr(mItemParams->mParam[GrapaItemEnum::EOL], c))
				{
					if (msgStr.mLength)
					{
						PushOutput((sendState ? sendState : state), msgStr, quote);
						msgStr.SetLength(0);
					}
					nextValue->mMessage.mPos++;
					msgStr.Append(c);
					PushOutput((sendState ? sendState : state), msgStr, quote);
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
					break;
				}
				nextValue->mMessage.mPos++;
				msgStr.Append(c);
				if (!strchr(mItemParams->mParam[GrapaItemEnum::SYS], c))
				{
					PushOutput((sendState ? sendState : state), msgStr, quote);
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
					if (isEl)
					{
						switch (c)
						{
						case '>':
							state = GrapaTokenItemType::STRING_COLLECT;
							quote = 0;
							break;
						case '<':
							isComment = 0;
							isCDATA = 0;
							break;
						case '!':
							isComment = 1;
							isCDATA = 0;
							break;
						case '-':
							isComment++;
							if (isComment == 3)
							{
								state = GrapaTokenItemType::STRING_COLLECT;
								quote = 0;
							}
							break;
						case '[':
							isCDATA++;
							if (isCDATA == 2)
							{
								state = GrapaTokenItemType::STRING_COLLECT;
								quote = 0;
							}
							break;
						}
					}
				}
				break;
			case GrapaTokenType::INT:
			case GrapaTokenType::FLOAT:
			case GrapaTokenItemType::EXP:
				if (msgStr.mLength == 1 && ((char*)msgStr.mBytes)[0] == mItemParams->mParam[GrapaItemEnum::DIG][0] && (c == 'x' || c == 'X'))
				{
					nextValue->mMessage.mPos++;
					msgStr.SetLength(0);
					savePos = 0;
					saveChar = 0;
					state = GrapaTokenItemType::NUMHEX;
					break;
				}
				else if (msgStr.mLength == 1 && ((char*)msgStr.mBytes)[0] == mItemParams->mParam[GrapaItemEnum::DIG][0] && (c == 'b' || c == 'B'))
				{
					nextValue->mMessage.mPos++;
					msgStr.SetLength(0);
					savePos = 0;
					saveChar = 0;
					state = GrapaTokenItemType::NUMBIN;
					break;
				}
				else if (state != GrapaTokenItemType::EXP && c == '.')
				{
					state = GrapaTokenType::FLOAT;
					nextValue->mMessage.mPos++;
					msgStr.Append(c);
					break;
				}
				else if (state != GrapaTokenItemType::EXP && (c == 'e' || c == 'E'))
				{
					state = GrapaTokenItemType::EXP;
					expNeg = false;
					nextValue->mMessage.mPos++;
					break;
				}
				else if (state == GrapaTokenItemType::EXP && expStr.mLength == 0 && (c == '-' || c == '+'))
				{
					if (c == '-')
						expNeg = !expNeg;
					nextValue->mMessage.mPos++;
					break;
				}
				else if (!strchr(mItemParams->mParam[GrapaItemEnum::DIG], c))
				{
					if (state == GrapaTokenType::FLOAT)
					{
						GrapaFloat d(mFloatFix, mFloatMax, mFloatExtra, 0);
						d.FromString(msgStr, 10);
						PushOutput(GrapaTokenType::FLOAT, d.getBytes(), quote);
					}
					else if (state == GrapaTokenItemType::EXP)
					{
						GrapaFloat d1(mFloatFix, mFloatMax, mFloatExtra, 0);
						d1.FromString(msgStr, 10);
						a.FromString(expStr, 10);
						PushOutput(GrapaTokenType::FLOAT, ((GrapaFloat(mFloatFix, mFloatMax, mFloatExtra, 10)).Pow(expNeg ? -a : a)*d1).getBytes(), quote);
					}
					else
					{
						if (msgStr.mLength == 0)
						{
							PushOutput((sendState ? sendState : state), msgStr,quote);
						}
						else
						{
							a.FromString(msgStr, 10);
							PushOutput((sendState ? sendState : state), a.getBytes(), quote);
						}
					}
					msgStr.SetLength(0);
					expStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
					break;
				}
				nextValue->mMessage.mPos++;
				if (state == GrapaTokenItemType::EXP) expStr.Append(c); else msgStr.Append(c);
				break;
			case GrapaTokenItemType::NUMHEX:
				if (char *from = strchr(mItemParams->mParam[GrapaItemEnum::HEX], c))
				{
					u64 addHex = (from - mItemParams->mParam[GrapaItemEnum::HEX]);
					if (addHex > 15) addHex -= 16;
					msgStr.Append(mItemParams->mParam[GrapaItemEnum::HEX][addHex]);
				}
				else if (c == '.')
				{
					msgStr.Append('.');
				}
				else
				{
					if (strchr((char*)msgStr.mBytes, '.'))
					{
						GrapaFloat d(mFloatFix, mFloatMax, mFloatExtra, 0);
						d.FromString(msgStr, 16);
						PushOutput(GrapaTokenType::FLOAT, d.getBytes(), quote);
					}
					else
					{
						a.FromString(msgStr, 16);
						GrapaCHAR m2(a.getBytes());
						u32 c1 = (msgStr.mLength+1) / 2;
						u32 c2 = m2.mLength;
						if (c1 > c2)
						{
							GrapaCHAR m3;
							while (c2++ < c1) m3.Append("\0",1);
							m3.Append(m2);
							PushOutput(GrapaTokenType::RAW, m3, quote);
						}
						else
							PushOutput(GrapaTokenType::RAW, m2, quote);
					}
					msgStr.SetLength(0);
					state = GrapaTokenType::START;
					break;
				}
				nextValue->mMessage.mPos++;
				break;
			case GrapaTokenItemType::NUMBIN:
                from = strchr(mItemParams->mParam[GrapaItemEnum::BIN], c);
				if (from)
				{
					msgStr.Append(c);
				}
				else if (c == '.')
				{
					msgStr.Append('.');
				}
				else
				{
					if (strchr((char*)msgStr.mBytes, '.'))
					{
						GrapaFloat d(mFloatFix, mFloatMax, mFloatExtra, 0);
						d.FromString(msgStr, 2);
						PushOutput(GrapaTokenType::FLOAT, d.getBytes(), quote);
					}
					else
					{
						a.FromString(msgStr, 2);
						GrapaCHAR m2(a.getBytes());
						u32 c1 = (msgStr.mLength + 7) / 8;
						u32 c2 = m2.mLength;
						if (c1 > c2)
						{
							GrapaCHAR m3;
							while (c2++ < c1) m3.Append("\0", 1);
							m3.Append(m2);
							PushOutput(GrapaTokenType::RAW, m3, quote);
						}
						else
							PushOutput(GrapaTokenType::RAW, m2, quote);
					}
					msgStr.SetLength(0);
					state = GrapaTokenType::START;
					break;
				}
				nextValue->mMessage.mPos++;
				break;
			case GrapaTokenType::ID:
				if (!strchr(mItemParams->mParam[GrapaItemEnum::ID], c) && !strchr(mItemParams->mParam[GrapaItemEnum::DIG], c))
				{
// What is this IDDASH for? See below. Need a way to support e^-10 or is it e-10, a way to specifiy a number.
					//if (c == '-')
					//{
					//	saveState = state;
					//	sendState = state;
					//	state = GrapaTokenItemType::IDDASH;
					//	nextValue->mMessage.mPos++;
					//	break;
					//}
					if ((c == '\'' || c == '"') && msgStr.mLength == 1 && msgStr.mBytes[0] == 'r')
					{
						msgStr.SetLength(0);
						sendState = 0;
						noEscape = false;
						state = GrapaTokenType::START;
						noEscape = true;
						break;
					}
					if (c != '\'')  // for an ID with a quote lie john's, not sure if this is the right way to handle this though
					{
						PushOutput((sendState ? sendState : state), msgStr, quote);
						msgStr.SetLength(0);
						sendState = 0;
						noEscape = false;
						state = GrapaTokenType::START;
						break;
					}
				}

				nextValue->mMessage.mPos++;
				msgStr.Append(c);
				break;
// What is this IDDASH for? I think it's for e-10 type stuff.
// Had to disable because it was causing "a-b" to be treated like a variable...
			case GrapaTokenItemType::IDDASH:
				if (!strchr(mItemParams->mParam[GrapaItemEnum::ID], c))
				{
					PushOutput((sendState ? sendState : state), msgStr, quote);
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
					msgStr.Append("-");
					PushOutput(GrapaTokenType::SYM, msgStr, quote);
					msgStr.SetLength(0);
					sendState = 0;
					noEscape = false;
					state = GrapaTokenType::START;
					saveState = 0;
					sendState = 0;
					break;
				}
				msgStr.Append('-');
				state = saveState;
				saveState = 0;
				sendState = 0;
				break;
			case GrapaTokenType::OTHER:
				msgStr.Append(c);
				PushOutput(state, msgStr, quote);
				msgStr.SetLength(0);
				state = GrapaTokenType::START;
				nextValue->mMessage.mPos++;
				break;
			}
		}
		if (nextValue->mMessage.mPos == nextValue->mMessage.mLength)
		{
			if (GetQueue())
			{
				if (GrapaItemEvent *e = (GrapaItemEvent*)GetQueue()->PopHead())
				{
					e->CLEAR();
					delete e;
				}
			}
		}
		if (mStop)
		{
			if (msgStr.mLength) PushOutput(state, msgStr, quote);
			msgStr.SetLength(0);
			state = GrapaTokenType::START;
			break;
		}
	}
	if (GetQueue()) GetQueue()->mState = NULL;
	if (GetQueue()) GetQueue()->CLEAR();
	if (!mSync) LeaveCritical();
}

void GrapaItemState::Stopping()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent::GrapaRuleEvent(u64 pTokenId, const GrapaCHAR& pName, const GrapaBYTE& pValue, GrapaObjectEvent* pTokenEvent)
{
	INIT();
	SetItem(pTokenId, pName, pValue);
	if (pTokenEvent)
	{
		vValueEvent = new GrapaRuleEvent(pTokenEvent->mId, pTokenEvent->mName, pTokenEvent->mValue);
		vValueEvent->mSkip = pTokenEvent->mSkip;
	}
}

GrapaRuleEvent::GrapaRuleEvent(u8 pToken, u64 pTokenId, const char* pName, const char* pValue, GrapaObjectEvent* pTokenEvent)
{
	INIT();
	SetItem(pTokenId, GrapaCHAR(pName), GrapaBYTE(pValue));
	mValue.mToken = pToken;
	if (pTokenEvent)
	{
		vValueEvent = new GrapaRuleEvent(pTokenEvent->mId, pTokenEvent->mName, pTokenEvent->mValue);
		vValueEvent->mSkip = pTokenEvent->mSkip;
	}
}

void GrapaRuleEvent::CLEAR()
{
	GrapaObjectEvent::CLEAR();
	if (vValueEvent)
	{
		vValueEvent->CLEAR();
		delete vValueEvent;
		vValueEvent = NULL;
	}
	if (vRuleLambda)
	{
		vRuleLambda->CLEAR();
		delete vRuleLambda;
		vRuleLambda = NULL;
	}
	vClass = NULL;
}

void GrapaRuleEvent::SetNull()
{
	CLEAR();
	mValue.SetNull();
	mNull = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GrapaRuleEvent::TOSize(u64& pSize)
{
	pSize = 0;
	u64 size = 0;
	switch (mValue.mToken)
	{
	case GrapaTokenType::RAW:
	case GrapaTokenType::BOOL:
	case GrapaTokenType::INT:
	case GrapaTokenType::FLOAT:
	case GrapaTokenType::STR:
	case GrapaTokenType::TIME:
	case GrapaTokenType::TABLE:
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
	case GrapaTokenType::SYSINT:
	case GrapaTokenType::SYSSTR:
		pSize += (sizeof(u64) + 1 + sizeof(u64) + (sizeof(u64) + mName.mLength) + (sizeof(u64) + mValue.mLength));
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
	case GrapaTokenType::OBJ:
		size = 0;
		if (vQueue) ((GrapaRuleQueue*)vQueue)->TOSize(size, vClass ? vClass->mName.mLength : 0);
		pSize += (sizeof(u64) + 1 + sizeof(u64) + (sizeof(u64) + mName.mLength) + (sizeof(u64) + size));
		break;
	case GrapaTokenType::PTR:
		size = 0;
		if (vRulePointer) vRulePointer->TOSize(size);
		pSize += (sizeof(u64) + 1 + sizeof(u64) + (sizeof(u64) + mName.mLength) + (sizeof(u64) + size));
		break;
	case GrapaTokenType::VECTOR:
		break;
	case GrapaTokenType::WIDGET:
		break;
	default: break;
	}
}

void GrapaRuleEvent::TO(GrapaBYTE& pValue)
{
	pValue.SetLength(0);
	pValue.mToken = 0;
	switch (mValue.mToken)
	{
	case GrapaTokenType::RAW:
	case GrapaTokenType::BOOL:
	case GrapaTokenType::TIME:
	case GrapaTokenType::INT:
	case GrapaTokenType::FLOAT:
	case GrapaTokenType::STR:
	case GrapaTokenType::TABLE:
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
	case GrapaTokenType::SYSINT:
	case GrapaTokenType::SYSSTR:
		pValue.FROM(mValue);
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
	case GrapaTokenType::OBJ:
		if (vQueue) ((GrapaRuleQueue*)vQueue)->TO(pValue, vClass, mValue.mToken);
		pValue.mToken = mValue.mToken;
		break;
	case GrapaTokenType::PTR:
		if (vRulePointer) vRulePointer->TO(pValue);
		break;
	case GrapaTokenType::VECTOR:
		pValue.mToken = mValue.mToken;
		if (vVector) vVector->TO(pValue);
		break;
	case GrapaTokenType::WIDGET:
		pValue.mToken = mValue.mToken;
		if (vWidget) vWidget->TO(pValue);
		break;
	default:
		break;
	}
}

bool GrapaRuleEvent::IsNull()
{
	bool isNeg;
	bool isNull;
	bool isZero = IsNullIsNegIsZero(isNeg, isNull);
	return(isNull);
}

bool GrapaRuleEvent::IsZero()
{
	bool isZero = false;
	switch (mValue.mToken)
	{
	case GrapaTokenType::STR:
	case GrapaTokenType::SYSSTR:
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
		if (mValue.mLength == 0)
		{
			isZero = true;
		}
		break;
	case GrapaTokenType::BOOL:
		if (mValue.mLength == 0 || mValue.mBytes[0] == 0 || mValue.mBytes[0] == '0')
		{
			isZero = true;
		}
		break;
	case GrapaTokenType::INT:
	case GrapaTokenType::SYSINT:
		if (true)
		{
			GrapaInt a;
			a.FromBytes(mValue);
			if (a.IsNull() || a.IsZero())
				isZero = true;
		}
		break;
	case GrapaTokenType::FLOAT:
		if (true)
		{
			GrapaFloat a(0);
			a.FromBytes(mValue);
			if (a.mData.IsZero())
				isZero = true;
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
	case GrapaTokenType::OBJ:
	case GrapaTokenType::RULEOP:
		if (!vQueue || !vQueue->mCount)
			isZero = true;
		break;
	case GrapaTokenType::VECTOR:
		break;
	case GrapaTokenType::WIDGET:
		break;
	}
	return(isZero);
}

bool GrapaRuleEvent::IsNeg()
{
	bool isNeg = false;
	GrapaInt a;
	switch (mValue.mToken)
	{
	case GrapaTokenType::INT:
	case GrapaTokenType::SYSINT:
		a.FromBytes(mValue);
		if (a.IsNeg())
			isNeg = true;
		break;
	}
	return(isNeg);
}

bool GrapaRuleEvent::IsNullIsNegIsZero(bool& isNeg, bool& isNull)
{
	isNull = mNull;
	isNeg = false;
	bool isZero = mNull;
	if (!mNull)
	{
		switch (mValue.mToken)
		{
		case GrapaTokenType::ERR:
			isNull = true;
			isZero = true;
			break;
		case GrapaTokenType::STR:
		case GrapaTokenType::SYSSTR:
		case GrapaTokenType::ID:
		case GrapaTokenType::SYSID:
			if (mValue.mLength == 0)
			{
				isNull = true;
				isZero = true;
			}
			break;
		case GrapaTokenType::BOOL:
			if (mValue.mLength == 0 || mValue.mBytes[0] == 0 || mValue.mBytes[0]=='0')
			{
				isNull = true;
				isZero = true;
			}
			break;
		case GrapaTokenType::INT:
		case GrapaTokenType::SYSINT:
			if (true)
			{
				GrapaInt a;
				a.FromBytes(mValue);
				if (a.IsNull() || a.IsZero())
					isZero = true;
				if (a.IsNeg())
					isNeg = true;
			}
			break;
		case GrapaTokenType::FLOAT:
			if (true)
			{
				GrapaFloat a(0);
				a.FromBytes(mValue);
				if (a.mData.IsZero())
					isZero = true;
				if (a.mSigned)
					isNeg = true;
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
		case GrapaTokenType::OBJ:
		case GrapaTokenType::RULEOP:
			if (!vQueue || !vQueue->mCount)
				isZero = true;
			break;
		case GrapaTokenType::VECTOR:
			break;
		case GrapaTokenType::WIDGET:
			break;
		}
	}
	return(isZero);
}

GrapaRuleEvent* GrapaRuleQueue::FROM(GrapaScriptState* pScriptState, GrapaNames* pNameSpace, const GrapaBYTE& pValue)
{
	GrapaRuleEvent *result=NULL;

	CLEAR();
	GrapaCHAR name;
	GrapaCHAR value;
	u8 tType = 0;
	u64 len = 0;

	u64 pos = 0, lastPos = 0;
	u64 blockSize = 0;
	u64 count = 0;
	u64 countP = 0;

	u32 headerBlockSize = 0;
	u32 headerVers = 0;

	if (pValue.mLength < (sizeof(u64) * 3)) return(result);
	memcpy(&headerVers, &pValue.mBytes[pos], sizeof(u32));		headerVers = BE_S32(headerVers);			pos += sizeof(u32);
	memcpy(&headerBlockSize, &pValue.mBytes[pos], sizeof(u32));	headerBlockSize = BE_S32(headerBlockSize);	pos += sizeof(u32);
	memcpy(&count, &pValue.mBytes[pos], sizeof(u64));			count = BE_S64(count);					pos += sizeof(u64);
	memcpy(&countP, &pValue.mBytes[pos], sizeof(u64));			countP = BE_S64(countP);				pos += sizeof(u64);

	if (count != (countP ^ (-1))) return(result);

	name.SetLength(0);
	memcpy(&len, &pValue.mBytes[pos], sizeof(u64));			name.SetLength(BE_S64(len), false);		pos += sizeof(u64);
	if (name.mBytes) memcpy(name.mBytes, &pValue.mBytes[pos], (size_t)name.mLength);						pos += name.mLength;
	if (name.mLength && pScriptState && pNameSpace)
	{
		result = pScriptState->GetClass(pNameSpace, name);
	}

	pos = lastPos = lastPos + headerBlockSize;

	for (u64 i = 0; i < count; i++)
	{
		name.SetLength(0);
		value.SetLength(0);

		memcpy(&blockSize, &pValue.mBytes[pos], sizeof(u64));	blockSize = BE_S64(blockSize);			pos += sizeof(u64);
		memcpy(&tType, &pValue.mBytes[pos], 1);															pos += 1;
		memcpy(&len, &pValue.mBytes[pos], sizeof(u64));			name.SetLength(BE_S64(len), false);		pos += sizeof(u64);
		if (name.mBytes) memcpy(name.mBytes, &pValue.mBytes[pos], (size_t)name.mLength);						pos += name.mLength;
		memcpy(&len, &pValue.mBytes[pos], sizeof(u64));			value.SetLength(BE_S64(len), false);	pos += sizeof(u64);
		if (value.mBytes) memcpy(value.mBytes, &pValue.mBytes[pos], (size_t)value.mLength);						pos += value.mLength;

		pos = lastPos = lastPos + blockSize;

		bool isNull = false;

		switch (tType)
		{
		case GrapaTokenType::ERR:
		case GrapaTokenType::BOOL:
		case GrapaTokenType::RAW:
		case GrapaTokenType::INT:
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::FLOAT:
		case GrapaTokenType::STR:
		case GrapaTokenType::SYSSTR:
		case GrapaTokenType::TIME: 
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::ID: value.mToken = tType; break;
		case GrapaTokenType::SYSID: value.mToken = GrapaTokenType::SYSID; if (value.mLength == 0) { value.SetSize(0); isNull = true; } break;
		case GrapaTokenType::VECTOR:
			break;
		case GrapaTokenType::WIDGET:
			break;
		default: value.mToken = GrapaTokenType::RAW; break;
		}

		GrapaRuleEvent *ev = new GrapaRuleEvent(0, name, value);
		ev->mNull = isNull;
		switch (value.mToken)
		{
		case GrapaTokenType::ERR:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
			ev->vQueue = new GrapaRuleQueue();
			ev->vClass = ((GrapaRuleQueue*)ev->vQueue)->FROM(pScriptState, pNameSpace, value);
			break;
		case GrapaTokenType::VECTOR:
			break;
		case GrapaTokenType::WIDGET:
			break;
		default:
            break;
		}
		PushTail(ev);
	}

	return(result);
}

void GrapaRuleQueue::TOSize(u64& pSize, u64 pClassNameSize)
{
	pSize = (sizeof(u64) * 3) + pClassNameSize;
	u64 size = 0;
	GrapaRuleEvent *ev = Head();
	while (ev)
	{
		ev->TOSize(size);
		pSize += size;
		ev = ev->Next();
	}
}

void GrapaRuleQueue::TO(GrapaBYTE& pValue, GrapaRuleEvent* pClass, u8 pType)
{
	u64 size = 0;
	u64 pos = 0, lastPos = 0;
	u64 count = 0;
	u64 countP = 0;
	u64 blockSize = 4;
	u64 len = 0;

	TOSize(size, pClass ? pClass->mName.mLength : 0);
	pValue.SetSize(size, false);
	pValue.SetLength((sizeof(u64) * 4) + (pClass ? pClass->mName.mLength : 0));
	pValue.mToken = pType;

	countP = count;
																										pos += sizeof(u64);
	count = BE_S64(count);					memcpy(&pValue.mBytes[pos], &count, sizeof(u64));			pos += sizeof(u64);
	countP = BE_S64(countP^(-1));			memcpy(&pValue.mBytes[pos], &countP, sizeof(u64));			pos += sizeof(u64);
	len = pClass ? pClass->mName.mLength : 0;
	len = BE_S64(len);						memcpy(&pValue.mBytes[pos], &len, sizeof(u64));				pos += sizeof(u64);
	if (pClass&&pClass->mName.mBytes)		memcpy(&pValue.mBytes[pos], pClass->mName.mBytes, (size_t)pClass->mName.mLength);		pos += pClass?pClass->mName.mLength:0;

	u32 headerBlockSize = 0;
	u32 headerVers = 0;
	headerVers = BE_S32(headerVers);		memcpy(&pValue.mBytes[0], &headerVers, sizeof(u32));
	headerBlockSize = (u32)pos;
	headerBlockSize = BE_S32(headerBlockSize);	memcpy(&pValue.mBytes[sizeof(u32)], &headerBlockSize, sizeof(u32));
	lastPos = pos;

	GrapaCHARFile value;
	GrapaCHARFile *valuePtr = NULL;
	GrapaRuleEvent *ev = Head();

	while (ev)
	{
		u8 tType = ev->mValue.mToken;
		switch (ev->mValue.mToken)
		{
		case GrapaTokenType::RAW:
		case GrapaTokenType::INT:
		case GrapaTokenType::BOOL:
		case GrapaTokenType::FLOAT:
		case GrapaTokenType::STR:
		case GrapaTokenType::TIME:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::ID:
		case GrapaTokenType::SYSID:
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::SYSSTR:
			tType = ev->mValue.mToken;
			valuePtr = &ev->mValue;
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
		case GrapaTokenType::OBJ:
			tType = ev->mValue.mToken;
			value.SetLength(0);
			if (ev->vQueue) ((GrapaRuleQueue*)ev->vQueue)->TO(value, ev->vClass, ev->mValue.mToken);
			valuePtr = &value;
			break;
		case GrapaTokenType::PTR:
			switch (ev->vRulePointer->mValue.mToken)
			{
			case GrapaTokenType::INT:
			case GrapaTokenType::BOOL:
			case GrapaTokenType::FLOAT:
			case GrapaTokenType::STR:
			case GrapaTokenType::RAW:
			case GrapaTokenType::TIME:
			case GrapaTokenType::TABLE:
			case GrapaTokenType::ID:
			case GrapaTokenType::SYSID:
			case GrapaTokenType::SYSINT:
			case GrapaTokenType::SYSSTR:
				tType = ev->vRulePointer->mValue.mToken;
				valuePtr = &ev->vRulePointer->mValue;
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
			case GrapaTokenType::OBJ:
				tType = ev->vRulePointer->mValue.mToken;
				value.SetLength(0);
				if (ev->vRulePointer->vQueue) ((GrapaRuleQueue*)ev->vRulePointer->vQueue)->TO(value, ev->vClass, ev->vRulePointer->mValue.mToken);
				valuePtr = &value;
				break;
			case GrapaTokenType::PTR:
				value.SetLength(0);
				ev->vRulePointer->TO(value);
				tType = value.mToken;
				valuePtr = &value;
				break;
			case GrapaTokenType::VECTOR:
				break;
			case GrapaTokenType::WIDGET:
				break;
			default:
				valuePtr = NULL;
				break;
			}
			break;
		case GrapaTokenType::VECTOR:
			break;
		case GrapaTokenType::WIDGET:
			break;
		default:
			valuePtr = NULL;
			break;
		}

		u8 fieldType = GrapaTokenType::RAW;
		switch (tType)
		{
		case GrapaTokenType::ERR:
		case GrapaTokenType::BOOL:
		case GrapaTokenType::RAW:
		case GrapaTokenType::INT:
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::FLOAT:
		case GrapaTokenType::STR:
		case GrapaTokenType::SYSSTR:
		case GrapaTokenType::TIME:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::LIST:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::OBJ:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::ID:
		case GrapaTokenType::SYSID: fieldType = tType; break;
		case GrapaTokenType::VECTOR:
		case GrapaTokenType::WIDGET:
			break;
		default: fieldType = GrapaTokenType::RAW; break;
		}

		if (valuePtr)
		{
			pos = pValue.mLength;
			pValue.SetLength(pos + (sizeof(u64) + 1 + (sizeof(u64) + ev->mName.mLength) + (sizeof(u64) + valuePtr->mLength)), true);
																															pos += sizeof(u64);
													memcpy(&pValue.mBytes[pos], &fieldType, 1);								pos += 1;
			len = BE_S64(ev->mName.mLength);		memcpy(&pValue.mBytes[pos], &len, sizeof(u64));							pos += sizeof(u64);
			if (ev->mName.mBytes)					memcpy(&pValue.mBytes[pos], ev->mName.mBytes, (size_t)ev->mName.mLength);		pos += ev->mName.mLength;
			len = BE_S64(valuePtr->mLength);		memcpy(&pValue.mBytes[pos], &len, sizeof(u64));							pos += sizeof(u64);
			if (valuePtr->mBytes)					memcpy(&pValue.mBytes[pos], valuePtr->mBytes, (size_t)valuePtr->mLength);		pos += valuePtr->mLength;
			blockSize = BE_S64((pos - lastPos));	memcpy(&pValue.mBytes[lastPos], &blockSize, sizeof(u64));
			lastPos = pos;
			count++;
		}
		ev = ev->Next();
	}

	countP = count;
	count = BE_S64(count);						memcpy(&pValue.mBytes[sizeof(u64)], &count, sizeof(u64));
	countP = BE_S64(countP ^ (-1));				memcpy(&pValue.mBytes[sizeof(u64)*2], &countP, sizeof(u64));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GrapaSystemSend::GetStr(GrapaCHAR& strBuf)
{
	strBuf.SetLength(0);
	GrapaItemEvent* item = (GrapaItemEvent*)mItemQueue.Head();
	while (item)
	{
		strBuf.Append(item->mMessage);
		item = item->Next();
	}
}

void GrapaSystemSend::Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen)
{
	if (sendbuf && sendbuflen)
	{
		if (isActive) 
			SendCommand(vScriptExec, pNameSpace, sendbuf, sendbuflen);
		else
		{
			GrapaItemEvent* event = new GrapaItemEvent();
			event->mMessage.FROM(sendbuflen, (char*)sendbuf);
			mItemQueue.PushTail(event);
		}
	}
}

void GrapaSystemSend::SetActive(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, bool pActive)
{
	isActive = pActive;
	if (isActive)
	{
		GrapaItemEvent* event = (GrapaItemEvent*)mItemQueue.PopHead();
		while (event)
		{
			Send(vScriptExec, pNameSpace, (u8*)event->mMessage.mBytes, event->mMessage.mLength);
			event->CLEAR();
			delete event;
			event = (GrapaItemEvent*)mItemQueue.PopHead();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// http://en.wikipedia.org/wiki/Flex_lexical_analyser
// http://www.quut.com/c/ANSI-C-grammar-y.html
// http://www.quut.com/c/ANSI-C-grammar-l-1999.html#check-type
// http://en.wikipedia.org/wiki/String_literal

// http://publib.boulder.ibm.com/infocenter/zvm/v6r1/index.jsp?topic=/com.ibm.zvm.v610.dmsp4/hcsp4c0022.htm

//http://www.cplusplus.com/doc/tutorial/constants/
// 014 -> Octal if it starts with 0

// yacc grammar
//http://research.cs.wisc.edu/htcondor/classad/refman/node7.html

//http://en.wikipedia.org/wiki/Syntax

//http://en.wikipedia.org/wiki/Secure_Shell

GrapaScriptState::GrapaScriptState()
{
	mInput = mQueue = mOutput = NULL;
	//mItemState.SetParams(&mItemParams,&mRuleVariables);
	SetNameSpace(NULL);
	SetInput(&mItemQueue);
	mItemState.SetQueue(&mItemQueue);
	vConsoleSend = NULL;
	mNet.vNetConnect = new GrapaNetConnect();
}

GrapaScriptState::~GrapaScriptState()
{
	Stop();
	GrapaScriptState::CLEAR();
	mItemState.SetParams(NULL,NULL);
	mItemState.SetQueue(NULL);
}

void GrapaScriptState::SetNameSpace(GrapaNames* pRuleVariables)
{
	vRuleVariables = pRuleVariables;
	mItemState.SetParams(&mItemParams, pRuleVariables);
}

void GrapaScriptState::CLEAR()
{
	if (GetQueue()) GetQueue()->CLEAR();
	mItemParams.mMapQueue.CLEAR();
	mItemState.CLEAR();
	mItemQueue.CLEAR();
	mRuleStartQueue.CLEAR();
	//mRuleVariables.vNames.CLEAR();
}

void GrapaScriptState::LoadSystem()
{
	GrapaScriptState::CLEAR();

	if (!GetNameSpace()->GetNameQueue()->mCount)
	{
		GrapaRuleEvent *e = AddRuleOperation(GetNameSpace()->GetNameQueue(), "", ""); // global variables
		e->mValue.mToken = GrapaTokenType::OBJ;
	}

	SetTokenParam(GrapaItemEnum::SYM," \t\r\n!@#$%^&*()-+={}|[]:;<>,.?/\\~");
	SetTokenParam(GrapaItemEnum::BIN, "01");
	SetTokenParam(GrapaItemEnum::OCT, "01234567");
	SetTokenParam(GrapaItemEnum::DIG, "0123456789");
	SetTokenParam(GrapaItemEnum::HEX,"0123456789ABCDEF0123456789abcdef");
	SetTokenParam(GrapaItemEnum::ID,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_");
	SetTokenParam(GrapaItemEnum::STR,"\"'");
	SetTokenParam(GrapaItemEnum::STRESC,"\\");
	SetTokenParam(GrapaItemEnum::STRFROM,"t'\"\\rn?abfv/0");
	SetTokenParam(GrapaItemEnum::STRTO,"\t'\"\\\r\n?\a\b\f\v/\0");
	SetTokenParam(GrapaItemEnum::SYS, "$");
	SetTokenParam(GrapaItemEnum::EOL, "\n");

	/////////////////////////////////////////////////////////////

	return;

	///////////////////////////////////////////////////////////////

	//GrapaObjectEvent* token;
	//GrapaRuleEvent* operation;
	//GrapaRuleEvent* parameter;

	//token = AddToken(GrapaTokenType::SYM, "$SPACE", true, " ");
	//token = AddToken(GrapaTokenType::SYM, "$TAB", true, "\t");
	//token = AddToken(GrapaTokenType::SYM, "$CR", true, "\r"); token->mRun = true;
	//token = AddToken(GrapaTokenType::SYM, "$LF", true, "\n"); token->mRun = true;
	////token = AddToken(GrapaTokenType::SYSID, "$NULL", false, "null"); token->mNull = true;

	//token = AddToken(GrapaTokenType::OTHER, "$UNKNOWN", true, NULL);

	//token = AddToken(GrapaTokenType::SYSSYM, "$BACKSLASH", true, "\\"); token->mEscape = true;
	//token = AddToken(GrapaTokenType::SYSSYM, "$LF", true, "\n"); token->mRun = true;

	//token = AddToken(GrapaTokenType::SYSSYM, "$BS", false, "["); token->mStart = true; 
	//token = AddToken(GrapaTokenType::SYSSYM, "$BE", false, "]"); token->mEnd = true; token->mRun = true;


	/////////////////////////////////////////////////////////////

	//rule = AddRule(&mRuleStartQueue, "$param_token");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "SYM");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "INT");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "FLOAT");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "ID");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "STR");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "SYSSYM");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "SYSINT");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "SYSID");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "SYSSTR");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "OTHER");

	//rule = AddRule(&mRuleStartQueue,"$param_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "var", 2);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"@");
	//		parameter = AddRawParameter(operation, GrapaTokenType::ID);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSSTR);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::STR);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID);
	//	operation = AddRuleOperation(&rule->mRuleQueue,"$SYS","lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::ID);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSINT);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::INT);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "null");
	//		parameter = AddRawParameter(operation, GrapaTokenType::ID, (char*)"", "null");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$lambda");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"<");
	//		parameter = AddRuleParameter(operation, "$code_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)">");

	//rule = AddRule(&mRuleStartQueue, "$lib_start");
	//		operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRuleParameter(operation, "start");
	//		
	//rule = AddRule(&mRuleStartQueue, "$list_value2");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$param_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"[");
	//		parameter = AddRuleParameter(operation, "$array_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"]");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRuleParameter(operation, "$lib_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");

	//rule = AddRule(&mRuleStartQueue, "$list_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "name", 1, 3);
	//		parameter = AddRawParameter(operation, GrapaTokenType::ID);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)":");
	//		parameter = AddRuleParameter(operation, "$list_value2");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$list_value2");

	//rule = AddRule(&mRuleStartQueue, "$attr");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "SKIP");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "ESC");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "RUN");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "EXIT");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "START");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "END");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "EMPTY");

	//rule = AddRule(&mRuleStartQueue, "$attr_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 2, 1);
	//		parameter = AddRuleParameter(operation, "$attr");
	//		parameter = AddRuleParameter(operation, "$attr_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0, 1);
	//		parameter = AddRuleParameter(operation, "$attr");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddTokenParameter(operation, "$NULL");

	//rule = AddRule(&mRuleStartQueue,"$lib_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 3, 1);
	//		parameter = AddRuleParameter(operation,"$param_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRuleParameter(operation,"$lib_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 3, 1);
	//		parameter = AddRuleParameter(operation, "$param_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)".");
	//		parameter = AddRuleParameter(operation, "$lib_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 2, 1);
	//		parameter = AddRuleParameter(operation, "$param_value");
	//		parameter = AddRuleParameter(operation, "$lib_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0, 1);
	//		parameter = AddRuleParameter(operation,"$param_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddTokenParameter(operation, "$NULL");

	//rule = AddRule(&mRuleStartQueue, "$lambda");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createop", 4, 7, 11);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"[");
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"[");
	//		parameter = AddRuleParameter(operation, "$array_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"]");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRuleParameter(operation, "$list_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"]");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createop", 4, 7);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"[");
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRuleParameter(operation, "$list_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"]");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createop", 4, 6, 9);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"[");
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRuleParameter(operation, "$list_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"]");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createop", 4, 6);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"[");
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"]");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createop", 4);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"[");
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"]");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "plan", 2, 5);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRuleParameter(operation, "$list_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRuleParameter(operation, "$list_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "wrap", 2, 5);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRuleParameter(operation, "$lib_start");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRuleParameter(operation, "$list_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");

	//rule = AddRule(&mRuleStartQueue, "$code_list");
	//		operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 3, 1);
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRuleParameter(operation, "$code_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createcode", 1);
	//		parameter = AddRuleParameter(operation, "$list_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createcode");
	//		//parameter = AddTokenParameter(operation, "");

	//rule = AddRule(&mRuleStartQueue, "$array_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 3, 1);
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRuleParameter(operation, "$array_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createarray", 1);
	//		parameter = AddRuleParameter(operation, "$list_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createarray");
	//		//parameter = AddTokenParameter(operation, "$NULL");

	//rule = AddRule(&mRuleStartQueue, "$list_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 3, 1);
	//		parameter = AddRuleParameter(operation, "$list_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)",");
	//		parameter = AddRuleParameter(operation, "$list_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createlist", 1);
	//		parameter = AddRuleParameter(operation, "$list_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createlist");
	//		//parameter = AddTokenParameter(operation, "");

	//rule = AddRule(&mRuleStartQueue,"$token");
	//	operation = AddRuleOperation(&rule->mRuleQueue,"$SYS","ruleval", 1, 3);
	//		parameter = AddRuleParameter(operation,"$param_token");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRuleParameter(operation,"$param_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//	operation = AddRuleOperation(&rule->mRuleQueue,"$SYS","ruleval", 1);
	//		parameter = AddRuleParameter(operation,"$param_token");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "ruleref", 2);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"@");
	//		parameter = AddRawParameter(operation, GrapaTokenType::ID);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "ruleref", 2);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"@");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID);
	//	operation = AddRuleOperation2(&rule->mRuleQueue, "$SYS", "ruleval", "SYM", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::STR);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::ID);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::INT);
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "lit", 1);
	//		parameter = AddRawParameter(operation, GrapaTokenType::FLOAT);

	//rule = AddRule(&mRuleStartQueue,"$token_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 2, 1);
	//		parameter = AddRuleParameter(operation,"$token");
	//		parameter = AddRuleParameter(operation,"$token_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0, 1);
	//		parameter = AddRuleParameter(operation,"$token");

	//rule = AddRule(&mRuleStartQueue,"$option");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0, 1);
	//		parameter = AddRuleParameter(operation,"$token_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createlist", 1, 3);
	//		parameter = AddRuleParameter(operation, "$token_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRuleParameter(operation, "$lambda");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0, 1);
	//		parameter = AddRuleParameter(operation,"$token_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "createlist", 0, 2);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"{");
	//		parameter = AddRuleParameter(operation, "$lambda");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"}");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0);
	//		//parameter = AddTokenParameter(operation, "$NULL");

	//rule = AddRule(&mRuleStartQueue,"$option_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 3, 1);
	//		parameter = AddRuleParameter(operation,"$option");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"|");
	//		parameter = AddRuleParameter(operation,"$option_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "prepend", 0, 1);
	//		parameter = AddRuleParameter(operation,"$option");

	//rule = AddRule(&mRuleStartQueue, "$command");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "rule", 2, 4);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "RULE");
	//		parameter = AddRuleParameter(operation, "$param_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"=");
	//		parameter = AddRuleParameter(operation, "$option_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "rule", 2);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "RULE");
	//		parameter = AddRuleParameter(operation, "$param_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "token", 4,2,6,7);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "TOKEN");
	//		parameter = AddRuleParameter(operation, "$param_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"=");
	//		parameter = AddRuleParameter(operation, "$param_token");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRuleParameter(operation, "$param_value");
	//		parameter = AddRuleParameter(operation, "$attr_list");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "token",4,2,6);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "TOKEN");
	//		parameter = AddRuleParameter(operation,"$param_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"=");
	//		parameter = AddRuleParameter(operation,"$param_token");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)"(");
	//		parameter = AddRuleParameter(operation, "$param_value");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)")");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "load", 2);
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "LOAD");
	//		parameter = AddRuleParameter(operation, "$param_value");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "reset");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "RESET");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "$SYS", "grammar");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYSID, (char*)"", "GRAMMAR");

	//rule = AddRule(&mRuleStartQueue, "$command_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$command");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)";");
	//		parameter = AddRuleParameter(operation, "$command_list");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$command");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)";");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$command");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRawParameter(operation, GrapaTokenType::SYM, (char*)"", (char*)";");

	//rule = GetRule(&mRuleStartQueue, "$start");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$command_list");
	//		parameter = AddTokenParameter(operation, "$BE");
	//	operation = AddRuleOperation(&rule->mRuleQueue, "", "");
	//		parameter = AddRuleParameter(operation, "$command_list");

}

void GrapaScriptState::ClearQueue()
{
	mSuspend.WaitCritical();
	mClearState = true;
	mSuspend.SendCondition();
	mSuspend.LeaveCritical();

	mItemState.mSuspend.WaitCritical();
	mItemState.mClearState = true;
	mItemState.mSuspend.SendCondition();
	mItemState.mSuspend.LeaveCritical();

	if (mItemQueue.mState)
		mItemQueue.mState->GetQueue()->CLEAR();
	mItemState.GetQueue()->CLEAR();
}

//GrapaRuleQueue* GrapaScriptState::GetTokens(GrapaRuleQueue* pTokenQueue, GrapaCHAR& pValue)
//{
//	return(GetTokens(pTokenQueue, (char*)pValue.mBytes, pValue.mLength));
//}

//GrapaRuleQueue* GrapaScriptState::GetTokens(GrapaRuleQueue* pTokenQueue, const char* pValue, u64 pLen)
//{
//	if (pLen == 0) return(NULL);
//
//	GrapaItemState paramState;
//	GrapaQueue paramQueue;
//
//	if (pTokenQueue) pTokenQueue->CLEAR();
//	else pTokenQueue = new GrapaRuleQueue();
//
//	paramState.SetParams(&mItemParams, GetNameSpace());
//	paramState.SetQueue(&paramQueue);
//	paramState.SetOutput(pTokenQueue);
//
//	GrapaItemEvent* event = new GrapaItemEvent();
//	event->mMessage.SetSize(pLen + 1);
//	event->mMessage.FROM(pLen, (char*)pValue);
//	((char*)event->mMessage.mBytes)[pLen] = 0;
//	paramQueue.PushTail(event);
//
//	paramState.StartSync();
//
//	return(pTokenQueue);
//}

void GrapaScriptState::SetTokenParam(u64 pLexParam, const char* pValue, bool pTouched)
{
	mItemState.SetParam(pLexParam,pValue,pTouched);
}

GrapaRuleEvent* GrapaScriptState::AddToken(u8 pToken, const char* pName, bool pSkip, const char* pValue)
{
	if (GrapaRuleEvent* token = mItemState.mItemParams->mMapQueue.Search(pToken, pValue)) { token->mSkip = pSkip; return(token); }
	return(mItemState.AddToken(pToken, pName, pSkip, pValue));
}

GrapaRuleEvent* GrapaScriptState::AddToken(u8 pToken, const char* pName, bool pSkip, GrapaBYTE& pValue)
{
	if (GrapaRuleEvent* token = mItemState.mItemParams->mMapQueue.Search(pToken, pValue)) { token->mSkip = pSkip; return(token); }
	return(mItemState.AddToken(pToken, pName, pSkip, pValue));
}

GrapaError GrapaScriptState::DelToken(GrapaRuleQueue* pRuleQueue, const char* pName)
{
	s64 idx;
	GrapaRuleEvent* param = pRuleQueue ? pRuleQueue->Search(pName,idx) : NULL;
	if (param==NULL) return(-1);
	if (pRuleQueue->PopEvent(param))
	{
		param->CLEAR();
		delete param;
	}
	return(0);
}

GrapaRuleEvent* GrapaScriptState::AddRule(GrapaRuleQueue* pRuleQueue, const char* pName)
{
	if (pRuleQueue == NULL) return(NULL);
	s64 idx;
	GrapaRuleEvent* rule = pRuleQueue->Search(pName,idx);
	if (rule == NULL)
	{
		rule = new GrapaRuleEvent();
		rule->vQueue = new GrapaRuleQueue();
		rule->SetItem(pRuleQueue->HashId(pName, 0), GrapaCHAR(pName), GrapaBYTE());
		rule->mValue.mToken = GrapaTokenType::RULE;
		if (rule) pRuleQueue->PushTail(rule);
	}
	return(rule);
}

GrapaRuleEvent* GrapaScriptState::GetRule(GrapaRuleQueue* pRuleQueue, const char* pName)
{
	if (pRuleQueue==NULL) return(NULL);
	s64 idx;
	return(pRuleQueue->Search(pName,idx));
}

GrapaRuleEvent* GrapaScriptState::AddRuleOperation(GrapaRuleQueue* pRuleEvent, const char* pName, const char* pValue, s64 a, s64 b, s64 c, s64 d)
{
	if (pRuleEvent == NULL) return(NULL);
	GrapaRuleEvent* operation = new GrapaRuleEvent(GrapaTokenType::RULEOP, (pName&&*pName) ? pRuleEvent->HashId(pName, 0) : 0, pName, pValue);
	if (operation == NULL) return(NULL);
	operation->vQueue = new GrapaRuleQueue();
	pRuleEvent->PushTail(operation);
	if (pValue==NULL || pValue[0]==0) return(operation);
	//if (a == -1) return(operation);
	operation->vRuleLambda = new GrapaRuleEvent(GrapaTokenType::OP, 0, "", "");
	operation->vRuleLambda->vQueue = new GrapaRuleQueue();
	if (pName&&GrapaMem::StrCmp(pName,strlen(pName),"$SYS",4)) operation->vRuleLambda->vQueue->PushTail(new GrapaRuleEvent(GrapaTokenType::STR, 0, "", pName));
	operation->vRuleLambda->vQueue->PushTail(new GrapaRuleEvent(GrapaTokenType::ID, 0, "", pValue));
	GrapaRuleEvent *v;
	v = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "", "");
	v->vQueue = new GrapaRuleQueue();
	operation->vRuleLambda->vQueue->PushTail(v);
	GrapaRuleEvent *v2;
	if (a == -1) return(operation);
	v2 = new GrapaRuleEvent(0, GrapaCHAR(), a?GrapaInt(a).getBytes() :GrapaBYTE());
	v2->mValue.mToken = GrapaTokenType::SYSINT;
	if (a == 0) v2->mNull = true;
	v->vQueue->PushTail(v2);
	if (b == -1) return(operation);
	v2 = new GrapaRuleEvent(0, GrapaCHAR(), b?GrapaInt(b).getBytes() :GrapaBYTE());
	v2->mValue.mToken = GrapaTokenType::SYSINT;
	if (b == 0) v2->mNull = true;
	v->vQueue->PushTail(v2);
	if (c == -1) return(operation);
	v2 = new GrapaRuleEvent(0, GrapaCHAR(), c?GrapaInt(c).getBytes() :GrapaBYTE());
	v2->mValue.mToken = GrapaTokenType::SYSINT;
	if (c == 0) v2->mNull = true;
	v->vQueue->PushTail(v2);
	if (d == -1) return(operation);
	v2 = new GrapaRuleEvent(0, GrapaCHAR(), d?GrapaInt(d).getBytes():GrapaBYTE());
	v2->mValue.mToken = GrapaTokenType::SYSINT;
	if (d == 0) v2->mNull = true;
	v->vQueue->PushTail(v2);
	return(operation);
}

GrapaRuleEvent* GrapaScriptState::AddRuleOperation2(GrapaRuleQueue* pRuleEvent, const char* pName, const char* pValue, const char* a, s64 b)
{
	if (pRuleEvent == NULL) return(NULL);
	GrapaRuleEvent* operation = new GrapaRuleEvent(GrapaTokenType::RULEOP, (pName&&*pName) ? pRuleEvent->HashId(pName, 0) : 0, pName, pValue);
	if (operation == NULL) return(NULL);
	operation->vQueue = new GrapaRuleQueue();
	pRuleEvent->PushTail(operation);
	if (a == NULL) return(operation);
	operation->vRuleLambda = new GrapaRuleEvent(GrapaTokenType::OP, 0, "", "");
	operation->vRuleLambda->vQueue = new GrapaRuleQueue();
	if (pName&&GrapaMem::StrCmp(pName, strlen(pName), "$SYS", 4)) operation->vRuleLambda->vQueue->PushTail(new GrapaRuleEvent(GrapaTokenType::STR, 0, "", pName));
	operation->vRuleLambda->vQueue->PushTail(new GrapaRuleEvent(GrapaTokenType::ID, 0, "", pValue));
	GrapaRuleEvent *v;
	v = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "", "");
	v->vQueue = new GrapaRuleQueue();
	operation->vRuleLambda->vQueue->PushTail(v);
	GrapaRuleEvent *v2;
	v2 = new GrapaRuleEvent(GrapaTokenType::SYSID, 0, "", a);
	v->vQueue->PushTail(v2);
	if (b == -1) return(operation);
	v2 = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaInt(b).getBytes());
	v2->mValue.mToken = GrapaTokenType::SYSINT;
	v->vQueue->PushTail(v2);
	return(operation);
}

GrapaRuleEvent* GrapaScriptState::AddRuleParameter(GrapaRuleEvent* pOperationEvent, const char* pName)
{
	GrapaRuleEvent* param = new GrapaRuleEvent(GrapaTokenType::RULEREF, mRuleStartQueue.HashId(pName, 0), pName);
	GrapaRuleEvent* op = pOperationEvent;
	while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
	op->vQueue->PushTail(param);
	return(param);
}

GrapaRuleEvent* GrapaScriptState::AddTokenParameter(GrapaRuleEvent* pOperationEvent, const char* pName)
{
	s64 idx;
	GrapaCHAR name(pName);
	GrapaRuleEvent* item = mItemState.mItemParams->mMapQueue.Search(name,idx);
	if (item == NULL) return(NULL);
	GrapaRuleEvent* param = new GrapaRuleEvent(item->mValue.mToken,item->mId,(char*)name.mBytes);
	param->mSkip = item->mSkip;
	param->mRun = item->mRun;
	param->mStart = item->mStart;
	param->mEnd = item->mEnd;
	param->mEscape = item->mEscape;
	param->mExit = item->mExit;
	param->mQuote = item->mQuote;
	param->mNull = item->mNull;
	GrapaRuleEvent* op = pOperationEvent;
	while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
	op->vQueue->PushTail(param);
	return(param);
}

GrapaRuleEvent* GrapaScriptState::AddRawParameter(GrapaRuleEvent* pOperationEvent, const u8 pToken)
{
	GrapaRuleEvent* param = new GrapaRuleEvent(pToken, 0, "", "");
	GrapaRuleEvent* op = pOperationEvent;
	while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
	op->vQueue->PushTail(param);
	return(param);
}

GrapaRuleEvent* GrapaScriptState::AddRawParameter(GrapaRuleEvent* pOperationEvent, u8 pToken, const char* pName, const char* pValue)
{
	GrapaRuleEvent* param = new GrapaRuleEvent(pToken, 0, pName, pValue);
	GrapaRuleEvent* op = pOperationEvent;
	while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
	op->vQueue->PushTail(param);
	return(param);
}

GrapaRuleEvent* GrapaScriptState::AddRawParameter(GrapaRuleEvent* pOperationEvent, const GrapaCHAR& pName, const GrapaBYTE& pValue)
{
	GrapaRuleEvent* param = new GrapaRuleEvent(0, pName, pValue);
	GrapaRuleEvent* op = pOperationEvent;
	while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
	op->vQueue->PushTail(param);
	return(param);
}

GrapaRuleEvent* GrapaScriptState::GetClass(GrapaNames* pNameSpace, const GrapaCHAR &name)
{
	gSystem->mLibLock.WaitCritical();
	s64 index;
	while (gSystem->mLibLockQueue.Search(name, index))
	{
		gSystem->mLibLock.LeaveCritical();
#ifdef _WIN32
		Sleep(10);
#else
		sleep(10);
#endif
		gSystem->mLibLock.WaitCritical();
	}
	GrapaRuleEvent* result = vScriptExec->vScriptState->SearchVariable(pNameSpace, name);
	if (result && !result->mNull && result->mValue.mToken == GrapaTokenType::CLASS)
	{
		gSystem->mLibLock.LeaveCritical();
		return(result);
	}
	GrapaRuleEvent* nameEvent = new GrapaRuleEvent(0, name, GrapaCHAR());
	gSystem->mLibLockQueue.PushTail(nameEvent);
	GrapaCHAR fNameOp(name);
	fNameOp.Append(".grz");
	GrapaCHAR fNameTxt(name);
	fNameTxt.Append(".grc");
	GrapaCHAR setValue;
	s64 idx;
	GrapaRuleEvent* e = (GrapaRuleEvent*)gSystem->mPath?gSystem->mPath->Head():NULL;
	while (e)
	{
		switch (e->mValue.mToken)
		{
		case GrapaTokenType::STR:
			if (e->vDatabase == NULL)
			{
				e->vDatabase = new GrapaLocalDatabase(this);
				e->vDatabase->mHomeDir = e->mValue;
			}
			break;
		case GrapaTokenType::OBJ:
			if (e->vDatabase == NULL)
				e->vDatabase = new GrapaLocalDatabase(this);
			break;
		}
		if (e->vDatabase)
		{
			e->vDatabase->FieldGet(fNameOp, GrapaCHAR(), setValue);
			if (setValue.mLength == 0)
				e->vDatabase->FieldGet(fNameTxt, GrapaCHAR(), setValue);
		}
		if (setValue.mLength)
			break;
		e = e->Next();
	}
	if (setValue.mLength == 0 && gSystem->mLibDir.mLength)
	{
		GrapaLocalDatabase gd;
		gd.mHomeDir = gSystem->mLibDir;
		gd.FieldGet(fNameOp, GrapaCHAR(), setValue);
		if (setValue.mLength == 0)
			gd.FieldGet(fNameTxt, GrapaCHAR(), setValue);
	}
	if (setValue.mLength == 0 && gSystem->mStaticLib && gSystem->mStaticLib->mCount)
	{
		s64 idx;
		GrapaRuleEvent * ev = gSystem->mStaticLib->Search(fNameOp,idx);
		if (ev)
			setValue.FROM(ev->mValue);
		if (setValue.mLength == 0)
		{
			ev = gSystem->mStaticLib->Search(fNameTxt,idx);
			if (ev)
				setValue.FROM(ev->mValue);
		}
	}
	GrapaRuleEvent *plan = NULL;
	bool needsClasses = false;
	if ((setValue.mLength >= 2 && setValue.mBytes[0] == 0 && setValue.mBytes[1] == 0) || (setValue.mLength > 4 && setValue.mBytes[0] == 'G' && setValue.mBytes[1] == 'R' && setValue.mBytes[2] == 'Z' && (setValue.mBytes[3] & 0x80) == 0))
	{
		GrapaBYTE expanded;
		GrapaCompress::Expand(setValue, expanded);
		plan = new GrapaRuleEvent();
		plan->vQueue = new GrapaRuleQueue();

		needsClasses = true;
		if (expanded.mToken == GrapaTokenType::LIST || expanded.mToken == GrapaTokenType::ARRAY || expanded.mToken == GrapaTokenType::TUPLE)
		{
			GrapaRuleQueue* tq = new GrapaRuleQueue();
			((GrapaRuleQueue*)tq)->FROM(vScriptExec->vScriptState, pNameSpace, expanded);
			GrapaRuleEvent* tx = NULL;
			if (tq->mCount > 1 && expanded.mToken == GrapaTokenType::LIST)
				tx = tq->Search("op",idx);
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
					gSystem->mLibLock.LeaveCritical();
					plan->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, tx->mName);
					gSystem->mLibLock.WaitCritical();
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
			plan->vClass = ((GrapaRuleQueue*)plan->vQueue)->FROM(vScriptExec->vScriptState, pNameSpace, expanded);
			plan->mValue.mToken = expanded.mToken;
		}

		plan->mValue.SetLength(0);
		plan->mValue.SetSize(0);
	}
	else if (setValue.mLength)
	{
		//gSystem->mLibLock.LeaveCritical();
		plan = vScriptExec->Plan(pNameSpace, setValue, NULL, 0, GrapaCHAR());
		//gSystem->mLibLock.WaitCritical();
	}
	if (plan)
	{
		gSystem->mLibLock.LeaveCritical();
		//gSystem->Print("plan:in\n");
		GrapaRuleEvent *r = vScriptExec->ProcessPlan(pNameSpace, plan);
		//gSystem->Print("plan:out\n");
		gSystem->mLibLock.WaitCritical();
		if (r)
		{
			r->CLEAR();
			delete r;
		}
		plan->CLEAR();
		delete plan;
		plan = NULL;
		result = vScriptExec->vScriptState->SearchVariable(pNameSpace, name);
	}
	gSystem->mLibLockQueue.PopEvent(nameEvent);
	delete nameEvent;
	gSystem->mLibLock.LeaveCritical();
	return(result);
}

//void GrapaScriptState::SetupClasses(GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
//{
//	if (pValue->vClass == NULL && pValue->mName.mLength && pValue->mValue.mToken==GrapaTokenType::CLASS)
//		pValue->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, pValue->mName);
//	GrapaRuleEvent* ev2 = pValue->vQueue ? pValue->vQueue->Head() : NULL;
//	GrapaRuleEvent* ev = (pValue->vClass&& pValue->vClass->vQueue) ? pValue->vClass->vQueue->Head() : NULL;
//	while (ev)
//	{
//		SetupClasses(pNameSpace, ev);
//		ev = ev->Next();
//	}
//
//}

GrapaRuleEvent* GrapaScriptState::FindClassVar(GrapaNames* pNameSpace, GrapaRuleEvent *r, const GrapaCHAR &s)
{
	s64 idx;
	if (r == NULL)
		return(NULL);
	switch (r->mValue.mToken)
	{
	case GrapaTokenType::ID:
	case GrapaTokenType::SYSID:
		if (r->vClass == NULL)
		{
			GrapaCHAR nm;
			if (r->mValue.mToken == GrapaTokenType::SYSID || r->mValue.mToken == GrapaTokenType::SYSSTR)
				nm.Append("$");
			nm.Append(r->mValue);
			r->vClass = GetClass(pNameSpace, r->mValue);
		}
		if (r->vClass)
			r = r->vClass;
		break;
	}
	switch (r->mValue.mToken)
	{
	case GrapaTokenType::CLASS:
		if (r->vQueue)
		{
			GrapaRuleEvent *r2 = (GrapaRuleEvent*)r->vQueue->Head();
			while (r2)
			{
				GrapaRuleEvent *r4 = FindClassVar(pNameSpace, r2, s);
				if (r4)
				{
					r4->mClass = true;
					return(r4);
				}
				r2 = r2->Next();
			}
		}
		break;
	default:
		if (r->vQueue)
			return ((GrapaRuleQueue*)r->vQueue)->Search(s, idx);
		break;
	}
	return(NULL);
}

void GrapaScriptState::CopyClassVars(GrapaRuleQueue *q, GrapaRuleEvent *r)
{
	return;
	s64 idx;
	switch (r->mValue.mToken)
	{
	case GrapaTokenType::CLASS:
		if (r->vQueue)
		{
			GrapaRuleEvent *r2 = (GrapaRuleEvent*)r->vQueue->Head();
			while (r2)
			{
				if (r2->vQueue)
				{
					GrapaRuleEvent *r3 = (GrapaRuleEvent*)r2->vQueue->Head();
					while (r3)
					{
						CopyClassVars(q, r3);
						r3 = r3->Next();
					}
				}
				r2 = r2->Next();
			}
		}
		break;
	case GrapaTokenType::OP:
	case GrapaTokenType::CODE:
		break;
	default:
		if (q->Search(r->mName,idx) == NULL)
			q->PushTail(vScriptExec->CopyItem(r));
		break;
	}
}

GrapaRuleEvent* GrapaScriptState::SearchVariable(GrapaNames* pNameSpace, GrapaRuleEvent *pValue)
{
	GrapaRuleEvent* parameter = NULL;
	GrapaNames *names = pNameSpace->GetSkipThis();
	s64 index = 0;
	bool isFirst = true;
	if (pValue->mValue.mToken == GrapaTokenType::INT || pValue->mValue.mToken == GrapaTokenType::SYSINT)
	{
		GrapaInt a;
		a.FromBytes(pValue->mValue);
		index = a.LongValue() - 1;
	}
	while (!parameter && names)
	{
		s64 idx;
		GrapaCHAR name;
		GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
		while (operation)
		{
			GrapaRuleEvent* op = operation;
			while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
			switch (pValue->mValue.mToken)
			{
			case GrapaTokenType::ID:
			case GrapaTokenType::REF:
			case GrapaTokenType::STR:
				parameter = (GrapaRuleEvent*)(op->vQueue?op->vQueue->Search(pValue->mValue,idx):NULL);
				if (parameter==NULL && op->mValue.mToken == GrapaTokenType::OBJ && op->vClass)
					parameter = FindClassVar(pNameSpace, op->vClass, pValue->mValue);
				//if (isFirst && parameter)
				//	parameter->mLocal = true;
				break;
			case GrapaTokenType::RULEREF:
				parameter = (GrapaRuleEvent*)(op->vQueue ? op->vQueue->Search(pValue->mName,idx) : NULL);
				if (parameter == NULL && op->mValue.mToken == GrapaTokenType::OBJ && op->vClass)
					parameter = FindClassVar(pNameSpace, op->vClass, pValue->mName);
				//if (isFirst && parameter)
				//	parameter->mLocal = true;
				break;
			case GrapaTokenType::SYSID:
			case GrapaTokenType::SYSSTR:
				name.FROM("$");
				name.Append(pValue->mValue);
				parameter = (GrapaRuleEvent*)(op->vQueue?op->vQueue->Search(name,idx):NULL);
				if (parameter == NULL && op->mValue.mToken == GrapaTokenType::OBJ && op->vClass)
					parameter = FindClassVar(pNameSpace, op->vClass, pValue->mValue);
				//if (isFirst && parameter)
				//	parameter->mLocal = true;
				break;
			case GrapaTokenType::INT:
			case GrapaTokenType::SYSINT:
				if (index >= 0)
				{
					parameter = (GrapaRuleEvent*)(op->vQueue?op->vQueue->Head(index):NULL);
					//if (isFirst && parameter)
					//	parameter->mLocal = true;
				}
				else
					parameter = pNameSpace->vLast;
				break;
			case GrapaTokenType::OP:
			case GrapaTokenType::CODE:
				parameter = vScriptExec->ProcessPlan(pNameSpace, pValue);
				break;
			}
			if (parameter)
			{
				pValue = NULL;
				GrapaRuleEvent* v = parameter;
				while (v && v->vRulePointer) 
					v = v->vRulePointer;
				if (v && v->mValue.mToken == GrapaTokenType::OP)
				{
					GrapaRuleEvent* v2 = (GrapaRuleEvent*)v->vQueue->Head(1);
					bool isVar = false;
					if (v2 && v2->vQueue && v2->vQueue->mCount)
						isVar = (v2->vQueue->Head()->mValue.Cmp("var") == 0);
					else if (v2)
						isVar = (v2->mValue.Cmp("var") == 0);
					if (isVar)
					{
						GrapaRuleEvent* v3 = (GrapaRuleEvent*)v->vQueue->Head(2);
						pValue = (GrapaRuleEvent*)v3->vQueue->Head();
						if (pValue->mValue.mToken == GrapaTokenType::INT || pValue->mValue.mToken == GrapaTokenType::SYSINT)
						{
							GrapaInt a;
							a.FromBytes(pValue->mValue);
							index = a.LongValue() - 1;
						}
					}
				}
				if (pValue == NULL)
					break;
			}
			isFirst = false;
			operation = operation->Prev();
		}
		names = names->GetSkipParrent();
	}
	return(parameter);
}

GrapaRuleEvent* GrapaScriptState::SearchVariable(GrapaNames* pNameSpace, const GrapaCHAR& pValue)
{
	GrapaRuleEvent t(0, GrapaCHAR(), pValue);
	return SearchVariable(pNameSpace, &t);
}

GrapaRuleEvent* GrapaScriptState::SearchTarget(GrapaNames* pNameSpace, GrapaRuleEvent* e)
{
	while (e && e->vRulePointer) e = e->vRulePointer;
	GrapaRuleEvent* e2 = NULL;
	if (e && !e->mNull)
	{
		switch (e->mValue.mToken)
		{
		case GrapaTokenType::ID:
		case GrapaTokenType::SYSID:
			e2 = SearchVariable(pNameSpace, e);
			break;
		case GrapaTokenType::OBJ:
			e2 = e;
			break;
		}
	}
	if (e2 == NULL)
		e2 = pNameSpace->This();
	if (e2->mValue.mToken == GrapaTokenType::OBJ)
		return(e2);
	if (e2->mValue.mToken == GrapaTokenType::TABLE)
		return(e2);
	if (e2->mValue.mToken == GrapaTokenType::WIDGET)
		return(e2);
	return(NULL);
}

GrapaRuleEvent* GrapaScriptState::SearchFileVariable(GrapaNames* pNameSpace, GrapaRuleEvent *pValue)
{
	GrapaRuleEvent *result = SearchVariable(pNameSpace, pValue);
	if (result && result->mValue.mToken != GrapaTokenType::TABLE)
		result = NULL;
	if (result && result->vDatabase == NULL)
	{
		result->vDatabase = new GrapaLocalDatabase(this);
		if (result->mValue.mToken == GrapaTokenType::TABLE)
			result->vDatabase->DatabaseSet(result->mValue);
	}
	return(result);
}

//GrapaRuleEvent* GrapaScriptState::CreateVariable(GrapaNames* pNameSpace, const GrapaCHAR& pName, bool& wasCreated)
//{
//	wasCreated = false;
//	GrapaRuleEvent* parameter = NULL;
//	GrapaRuleEvent* operation = pNameSpace->mHeadSkip ? pNameSpace->GetNameQueue()->Head() : pNameSpace->GetNameQueue()->Tail();
//	if (operation)
//	{
//		s64 idx;
//		GrapaRuleEvent* op = operation;
//		while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) op = op->vRulePointer;
//		parameter = (GrapaRuleEvent*)op->vQueue->Search(pName,idx);
//		if (parameter == NULL)
//		{
//			GrapaBYTE value;
//			value.mToken = GrapaTokenType::INT;
//			parameter = AddRawParameter(op, pName, value);
//			wasCreated = true;
//		}
//	}
//	return(parameter);
//}
//
//GrapaRuleEvent* GrapaScriptState::SetVariable(GrapaNames* pNameSpace, const GrapaCHAR& pName, const GrapaBYTE& pValue)
//{
//	bool wasCreated;
//	GrapaRuleEvent* parameter = CreateVariable(pNameSpace, pName, wasCreated);
//	if (parameter)
//	{
//		parameter->mId = 0;
//		if (parameter->vQueue)
//		{
//			parameter->vQueue->CLEAR();
//			delete parameter->vQueue;
//			parameter->vQueue = NULL;
//		}
//		parameter->vRulePointer = NULL;
//		parameter->mValue.FROM(pValue);
//	}
//	return(parameter);
//}

void GrapaScriptState::SendPrompt(GrapaNames* pNameSpace)
{
	GrapaCHAR prompt, path;
	prompt.Append(pNameSpace->GetResponse()->mPrompt);
	if (pNameSpace->GetResponse()->mPrompt.mLength) prompt.Append(' ');
	GrapaCHAR pathBuild;
	if (mPath.mLocation.mLength)
	{
		path.Append(mPath.mLocation);
		path.Append("/");
	}
	mPath.mDirectoryPath->ListNames(pathBuild, '/');
	path.Append(pathBuild);
	pathBuild.SetLength(0);
	mPath.mDatabasePath->ListNames(pathBuild, '/');
	path.Append(pathBuild);
	prompt.Append(path);
	prompt.Append((char*)">");
	pNameSpace->GetResponse()->SendPrompt(vScriptExec, pNameSpace, prompt);
	mNeedsPrompt = false;
}

void GrapaScriptState::StartResult(GrapaNames* pNameSpace)
{
	pNameSpace->GetResponse()->SendStart(vScriptExec, pNameSpace);
	//vRuleVariables->GetResponse()->SendStart(vScriptExec, pNameSpace);
}

void GrapaScriptState::EndResult(GrapaNames* pNameSpace, GrapaRuleEvent* pResult)
{
	if (pNameSpace && vScriptExec)
	{
		if (pResult && pResult->vRulePointer && pResult->mValue.mToken != GrapaTokenType::PTR)
		{
			if (pResult->mValue.mToken == GrapaTokenType::ERR)
				pResult->vClass = NULL;
			pResult->vRulePointer = NULL;
		}
		pNameSpace->GetResponse()->SendEnd(vScriptExec, pNameSpace, pResult);
	}
}

void GrapaScriptState::UpdateResult(GrapaNames* pNameSpace, GrapaRuleEvent* pResult)
{
	if (pResult == NULL) return;
	if (pResult->mValue.mToken == GrapaTokenType::PTR)
	{
		GrapaRuleEvent* echo = vScriptExec->CopyItem(pResult);
		pResult->CLEAR();
		delete pResult;
		pResult = echo;
	}
	if (pNameSpace->vLast)
	{
		pNameSpace->vLast->CLEAR();
		delete pNameSpace->vLast;
	}
	pNameSpace->vLast = pResult;
	//bool sendCR = false;
	//if (pNameSpace->vLast->mValue.mToken == GrapaTokenType::ERR)
	//{
	//	pNameSpace->GetResponse()->Send("ERR: ");
	//	sendCR = true;
	//}
}

void GrapaScriptState::Running()
{
	GrapaScriptExec tokenExec;
	tokenExec.vScriptState = this;
	vScriptExec = &tokenExec;
	GrapaScriptExecState::Running();
}

void GrapaScriptState::Stopping()
{
	GrapaScriptExecState::Stopping();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaRuleEvent* GrapaLibraryEvent::Error(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaError err)
{
	GrapaRuleEvent *result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::ERR;
	result->vQueue = new GrapaRuleQueue();
	result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaInt(err).getBytes()));
	return result;
}

void GrapaLibraryEvent::RotateLeft(GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam, const char*v1, const char*v2, const char* v3, const char* v4)
{
	if (pParam && pParam->vQueue && pParam->vQueue->mCount == 2)
	{
		GrapaRuleEvent* p1 = (GrapaRuleEvent*)pParam->vQueue->Head();
		GrapaRuleEvent* p2 = (GrapaRuleEvent*)pParam->vQueue->Head(1);
		if (p2 && p2->mValue.mToken == GrapaTokenType::OP && p2->vQueue && p2->vQueue->mCount == 2)
		{
			GrapaRuleEvent* nextOp = (GrapaRuleEvent*)p2->vQueue->Head();
			GrapaRuleEvent* nextParam = (GrapaRuleEvent*)p2->vQueue->Tail();
			if ((v1&&*v1&&nextOp->mValue.Cmp(v1) == 0) || (v2 && *v2 && nextOp->mValue.Cmp(v2) == 0) || (v3 && *v3 && nextOp->mValue.Cmp(v3) == 0) || (v4 && *v4 && nextOp->mValue.Cmp(v4) == 0))
			{
				GrapaRuleEvent* a = pParam->vQueue->PopHead();
				GrapaRuleEvent* b = pParam->vQueue->PopTail();
				GrapaRuleEvent* bb = nextParam->vQueue->PopTail();
				nextParam->vQueue->PushHead(a);
				pParam->vQueue->PushHead(b);
				pParam->vQueue->PushTail(bb);
				GrapaCHARFile v(nextOp->mValue);
				nextOp->mValue.FROM(pOperation->vQueue->Head()->mValue);
				pOperation->vQueue->Head()->mValue.FROM(v);
				GrapaLibraryEvent* e = pOperation->vQueue->Head()->vLibraryEvent;
				pOperation->vQueue->Head()->vLibraryEvent = nextOp->vLibraryEvent;
				nextOp->vLibraryEvent = e;
			}
		}
	}
}

GrapaRuleEvent* GrapaScriptExec::Search(GrapaRuleQueue* pParameterQueue, u64 pTokenId)
{
	if (pParameterQueue == NULL) return(NULL);
	GrapaRuleEvent* param = pParameterQueue->Head();
	while (param && param->mId != pTokenId) param = param->Next();
	return(param);
}

struct BtRuleParameterList
{
	GrapaRuleEvent* eventPos;					// nextEvent
	GrapaRuleEvent* operationPos;		// parameterQueue->Tail()
};

GrapaRuleEvent* GrapaScriptExec::PlanRule(GrapaNames* pNameSpace, GrapaRuleEvent* pTokenEvent, GrapaRuleEvent* pRuleEvent, GrapaRuleQueue* pOperationQueue, bool& pMatched, GrapaRuleEvent* pLastNext, bool& pAddLastNext, s8 pLeftRecursion, GrapaKeyValue& pCache, bool pTouched)
{
	GrapaRuleEvent* operation = NULL;
	GrapaRuleEvent* operationEvent = NULL;
	GrapaRuleQueue* parameterQueue = NULL;
	GrapaRuleEvent* parameter = NULL;
	GrapaRuleEvent* nextEvent = NULL;
	pAddLastNext = false;

	u64 maxParams = 0;
	operation = (GrapaRuleEvent*)pRuleEvent->vQueue->Head();
	if (operation == NULL) return(pTokenEvent);
	while (operation)
	{
		if (pLeftRecursion)
		{
			while (operation)
			{
				parameter = (GrapaRuleEvent*)operation->vQueue->Head();
				if (parameter == NULL) break;
				if (pLeftRecursion == 1 && parameter->mName.StrCmp(pRuleEvent->mName) != 0) break;
				if (pLeftRecursion == 2 && parameter->mName.StrCmp(pRuleEvent->mName) == 0) break;
				operation = operation->Next();
			}
		}
		//if (operation == NULL) break;
		if (operation)
		{
			if (operation->vQueue->mCount > maxParams) maxParams = operation->vQueue->mCount;
			operation = operation->Next();
		}
	}
	operation = (GrapaRuleEvent*)pRuleEvent->vQueue->Head();
	if (pLeftRecursion)
	{
		while (operation)
		{
			parameter = (GrapaRuleEvent*)operation->vQueue->Head();
			if (parameter == NULL) break;
			if (pLeftRecursion == 1 && parameter->mName.StrCmp(pRuleEvent->mName) != 0) break;
			if (pLeftRecursion == 2 && parameter->mName.StrCmp(pRuleEvent->mName) == 0) break;
			operation = operation->Next();
		}
	}
	if (operation == NULL) return(pTokenEvent);
	operationEvent = new GrapaRuleEvent(operation->mValue.mToken, operation->mId, (char*)operation->mName.mBytes, (char*)operation->mValue.mBytes);
	parameterQueue = new GrapaRuleQueue();
	operationEvent->vQueue = parameterQueue;
	parameter = (GrapaRuleEvent*)operation->vQueue->Head();
	nextEvent = pTokenEvent;

	// should change this...hard coded max limit on number of tokens for a rule to 32
	int paramListSize = ((int)maxParams) * sizeof(BtRuleParameterList);
	if (paramListSize < (32 * sizeof(BtRuleParameterList))) paramListSize = 32 * sizeof(BtRuleParameterList);
	BtRuleParameterList* paramList = (BtRuleParameterList*)GrapaMem::Create(paramListSize);
	memset(paramList, 0, paramListSize);
	u32 paramPos = 0;
	s64 idx;

	pMatched = false;

	while (operation)
	{
		GrapaRuleEvent* lastOperation = operation;
		bool failed = false;
		bool performedCatchall = false;

		while (nextEvent && nextEvent->mSkip)
		{
			nextEvent->mTouch = pTouched;
			nextEvent = nextEvent->Next();
		}

		while (parameter && nextEvent)
		{
			if (pTouched)
			{
				GrapaRuleEvent* tn = nextEvent->Prev();
				while (tn && tn->mTouch == false) tn->mTouch = pTouched, tn = tn->Prev();
			}
			nextEvent->mTouch = pTouched;
			bool pushExtraToken = false;
			GrapaRuleQueue operationQueue, operationEvalQueue;
			GrapaRuleEvent* operationEvalEvent = NULL;
			if (parameter->mValue.mToken == GrapaTokenType::REF || parameter->mValue.mToken == GrapaTokenType::LIST)
			{
				GrapaCHAR s(nextEvent->mValue);
				GrapaCHAR val;
				GrapaRuleQueue* paramList = NULL;
				GrapaRuleEvent* e = NULL;
				GrapaLocalDatabase* dList = NULL;
				if (parameter->mValue.mToken == GrapaTokenType::REF)
				{

					// This is the section that hanbdles the variable reference. Different actions based on the type of the variable
					// Needs to be extended to use the latest syntax supporting passing parameters and chaining objects 
					// And should have a way to cause the rule to fail...like the LIST lookup is doing when a search fails. 

					GrapaRuleEvent* p = vScriptState->SearchVariable(vScriptState->GetNameSpace(), parameter);
					if (p && p->mValue.mToken == GrapaTokenType::LIST)
					{
						paramList = (GrapaRuleQueue*)p->vQueue;
						if (paramList)
							e = paramList->Search(s, idx);
					}
					else if (p && p->mValue.mToken == GrapaTokenType::BOOL)
					{
						failed = true;
						break;
						if (p->mValue.mLength == 0 || p->mValue.mBytes[0] == 0 || p->mValue.mBytes[0] == '0')
						{
							failed = true;
							break;
						}
						parameter = parameter->Next();
						continue;
					}
					else if (p && p->mValue.mToken == GrapaTokenType::OBJ)
					{
						failed = true;
						break;
						dList = p->vDatabase;
						if (dList)
							dList->FieldGet(s, GrapaCHAR(), val);
					}
					else if (p && p->mValue.mToken == GrapaTokenType::OP)
					{
						failed = true;
						break;
						GrapaRuleEvent* param = ProcessPlan(vScriptState->GetNameSpace(), p);
						if (param)
						{
							if (param->mValue.mToken == GrapaTokenType::ERR)
							{
								failed = true;
								param->CLEAR();
								delete param;
								break;
							}
							if (param->mValue.mLength == 0 || param->mValue.mBytes[0] == 0 || param->mValue.mBytes[0] == '0')
							{
								failed = true;
								param->CLEAR();
								delete param;
								break;
							}
							param->CLEAR();
							delete param;
						}
						parameter = parameter->Next();
						continue;
					}
					else
					{
						failed = true;
						break;
					}
				}
				else
				{
					paramList = (GrapaRuleQueue*)parameter->vQueue;
					if (paramList)
						e = paramList->Search(s, idx);
				}
				if (e == NULL && val.mLength == 0)
				{
					failed = true;
					break;
				}
				GrapaRuleEvent* n = nextEvent->Next();
				while (n)
				{
					s.Append(n->mValue);
					while (n && (n->mSkip || n->mValue.mToken == GrapaTokenType::SYM))
					{
						n = n->Next();
						if (n) s.Append(n->mValue);
					}
					if (paramList)
					{
						GrapaRuleEvent* e2 = paramList->Search(s, idx);
						if (e2 == NULL)
							break;
						e = e2;
					}
					else if (dList)
					{
						GrapaCHAR val2;
						dList->FieldGet(s, GrapaCHAR(), val2);
						if (val2.mLength == 0)
							break;
						val.FROM(val2);
					}
					if (n)
						n = n->Next();
				}
				if (paramList)
				{
					GrapaRuleEvent* ev = CopyItem(e);
					ev->mName.SetLength(0);
					parameterQueue->PushTail(ev);
				}
				else if (dList)
				{
					e = new GrapaRuleEvent();
					e->mValue.FROM(val);
					if (e->mValue.mToken == GrapaTokenType::RAW)
						e->mValue.mToken = GrapaTokenType::STR;
					else if (e->mValue.mToken == GrapaTokenType::ARRAY || e->mValue.mToken == GrapaTokenType::TUPLE || e->mValue.mToken == GrapaTokenType::LIST || e->mValue.mToken == GrapaTokenType::XML || e->mValue.mToken == GrapaTokenType::EL || e->mValue.mToken == GrapaTokenType::TAG || e->mValue.mToken == GrapaTokenType::OP || e->mValue.mToken == GrapaTokenType::CODE || e->mValue.mToken == GrapaTokenType::ERR)
					{
						e->vQueue = new GrapaRuleQueue();
						e->vClass = ((GrapaRuleQueue*)e->vQueue)->FROM(vScriptState, pNameSpace, e->mValue);
						if (e->mValue.mLength == 0)
							e->mNull = true;
						e->mValue.SetLength(0);
						e->mValue.SetSize(0);
					}
					parameterQueue->PushTail(e);
				}
				nextEvent = n;
			}
			else if (parameter->mValue.mToken == GrapaTokenType::TABLE)
			{

			}
			else if (parameter->mValue.mToken == GrapaTokenType::RULE)
			{
				GrapaRuleEvent* evalEvent = nextEvent;
				GrapaRuleEvent* lastNext = parameter->Next();
				while (lastNext && lastNext->mValue.mToken == GrapaTokenType::RULEREF && (lastNext->mId == 0 || lastNext->mName.mLength == 0))
					lastNext = lastNext->Next();
				bool matched = false;
				evalEvent = PlanRule(pNameSpace, nextEvent, parameter, &operationQueue, matched, lastNext ? lastNext : pLastNext, pushExtraToken, 0, pCache, true);
				if (evalEvent == nextEvent && !matched)
				{
					failed = true;
					break;
				}
				while (operationQueue.Head())
				{
					// does this ever happen more than 1 itteration?
					parameterQueue->PushTail(operationQueue.PopHead());
				}
				nextEvent = evalEvent;
			}
			else if (parameter->mValue.mToken == GrapaTokenType::RULEREF)
			{
				// if the first token of the rule is a self reference, we hve left recursion
				// which we need special handling for.
				// For left recursion, split the rule options into 2 parts. Part 1 is the options that start with the self reference. Part 2 is the rest.
				// First do a match on part2. If a match is found, then recursively match on part 1, building up the tree. 
				// Otherwise do the following....

				GrapaRuleEvent* evalEvent = nextEvent;
				if (parameter->mId && parameter->mName.mLength)
				{
					s8 isFirstisRule = 0;
					if (parameter->Queue()->Head() == parameter && parameter->mName.StrCmp(pRuleEvent->mName) == 0)
						isFirstisRule = 1;
					GrapaRuleEvent* lastNext = parameter->Next();
					while (lastNext && lastNext->mValue.mToken == GrapaTokenType::RULEREF && (lastNext->mId == 0 || lastNext->mName.mLength == 0))
						lastNext = lastNext->Next();
					bool matched = false;
					GrapaRuleEvent* rulexx;// = vScriptState->mRuleStartQueue.Search(parameter->mId);
					rulexx = (GrapaRuleEvent*)pCache.Get(parameter->mName);
					if (rulexx == NULL)
					{
						rulexx = vScriptState->SearchVariable(pNameSpace, parameter->mName);
						if (rulexx == NULL || rulexx->mValue.mToken != GrapaTokenType::RULE)
							rulexx = vScriptState->mRuleStartQueue.Search(parameter->mName, idx);
						if (rulexx) pCache.Set(rulexx);
					}
					if (rulexx == NULL)
					{
						failed = true;
						break;
					}
					if (!isFirstisRule)
					{
						evalEvent = PlanRule(pNameSpace, nextEvent, rulexx, &operationQueue, matched, lastNext ? lastNext : pLastNext, pushExtraToken, 0, pCache, true);
						if (evalEvent == nextEvent && !matched)
						{
							failed = true;
							break;
						}
					}
					else
					{
						if (pLeftRecursion == 0)
						{
							evalEvent = PlanRule(pNameSpace, nextEvent, rulexx, &operationQueue, matched, lastNext ? lastNext : pLastNext, pushExtraToken, 1, pCache, true);
							if (evalEvent == nextEvent && !matched)
							{
								failed = true;
								break;
							}
							while (true)
							{
								GrapaRuleEvent* oldEvalEvent = evalEvent;
								evalEvent = PlanRule(pNameSpace, evalEvent, rulexx, &operationQueue, matched, lastNext ? lastNext : pLastNext, pushExtraToken, 2, pCache, true);
								if (evalEvent == oldEvalEvent && !matched)
								{
									evalEvent = oldEvalEvent;
									break;
								}
							}
							while (operationQueue.Head())
							{
								// does this ever happen more than 1 itteration?
								pOperationQueue->PushTail(operationQueue.PopHead());
							}
							if (operationEvent)
							{
								operationEvent->CLEAR();
								delete operationEvent;
							}
							GrapaMem::Delete(paramList);
							return(evalEvent);
						}
						else if (pLeftRecursion == 2)
						{
							operationQueue.PushTail(vScriptState->vScriptExec->CopyItem(pOperationQueue->Tail()));
						}
					}
				}
				else if (parameter->vQueue == NULL || (parameter->vQueue->mCount == 0 || (parameter->vQueue->mCount && !parameter->mName.mLength)))
				{
					performedCatchall = true;
					GrapaRuleEvent* lastNext = pLastNext;
					GrapaRuleEvent* next = parameter->Next();
					if (next)
					{
						if (next->mValue.mToken == GrapaTokenType::OP)
						{
							failed = true;
							break;
						}
						lastNext = next;
						while (lastNext && lastNext->mValue.mToken == GrapaTokenType::RULEREF && (lastNext->mId == 0 || lastNext->mName.mLength == 0))
							lastNext = lastNext->Next();
					}
					GrapaRuleEvent* start = NULL;
					if (paramPos)
					{
						start = paramList[paramPos - 1].eventPos;
						//if (start)
						//	start = start->Next();
					}
					else if (pTokenEvent)
					{
						start = pTokenEvent;
					}
					//while (start && start->mSkip)
					//	start = start->Prev();
					GrapaRuleEvent* end = NULL;
					bool opMatched = true;
					if (lastNext)
					{
						end = nextEvent;
						opMatched = false;
						while (end && !opMatched)
						{
							next = lastNext;
							if (next->mValue.mToken == GrapaTokenType::RULEREF || next->mValue.mToken == GrapaTokenType::RULE)
							{
								operationEvalQueue.CLEAR();
								GrapaRuleEvent* lastNext = next->Next();
								while (lastNext && (lastNext->mValue.mToken == GrapaTokenType::RULEREF || lastNext->mValue.mToken == GrapaTokenType::RULE) && (lastNext->mId == 0 || lastNext->mName.mLength == 0))
									lastNext = lastNext->Next();
								bool matched = false;
								GrapaRuleEvent* rulexx;// = vScriptState->mRuleStartQueue.Search(next->mId);
								if (next->mValue.mToken == GrapaTokenType::RULE)
									rulexx = next;
								else
								{
									rulexx = (GrapaRuleEvent*)pCache.Get(next->mName);
									if (rulexx == NULL)
									{
										rulexx = vScriptState->SearchVariable(pNameSpace, next->mName);
										if (rulexx) pCache.Set(rulexx);
									}
								}
								if (rulexx == NULL || rulexx->mValue.mToken != GrapaTokenType::RULE)
									rulexx = vScriptState->mRuleStartQueue.Search(next->mName, idx);
								if (rulexx)
									operationEvalEvent = PlanRule(pNameSpace, end, rulexx, &operationEvalQueue, matched, lastNext, (lastNext == pLastNext) ? pAddLastNext : pushExtraToken, 0, pCache, false);
								else
									operationEvalEvent = end;
								if (operationEvalEvent != end || matched)
								{
									opMatched = true;
									break;
								}
								operationEvalEvent = NULL;
							}
							else if (end->mValue.mToken == next->mValue.mToken && (next->mId == 0 || end->mId == next->mId))
							{
								if (next->mId)
									opMatched = true;
								else if (next->mValue.mLength)
								{
									if (next->mValue.mToken == GrapaTokenType::FLOAT)
									{
										GrapaFloat a(0), b(0);
										a.FromBytes(next->mValue);
										b.FromBytes(end->mValue);
										if (a == b)
											opMatched = true;
									}
									else if (next->mValue.StrCmp(end->mValue) == 0)
										opMatched = true;
								}
								else
									opMatched = true;
							}
							if (opMatched)
							{
								if (lastNext != pLastNext)
									pushExtraToken = true;
								else
									pAddLastNext = true;
								break;
							}
							end = end->Next();
							while (end && end->mSkip) end = end->Next();
						}
					}
					if (!opMatched)
					{
						failed = true;
						break;
					}
					pTouched = true;
					if (start)
					{
						GrapaRuleEvent* e = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "");
						e->vQueue = new GrapaRuleQueue();
						operationQueue.PushTail(e);
						while (start && (end == NULL || start != end) && !start->mRun)
						{
							GrapaRuleEvent* e2 = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "");
							e2->vRulePointer = start;
							e->vQueue->PushTail(e2);
							start->mTouch = true;
							start = start->Next();
						}
					}
					start->mTouch = true;
					end->mTouch = true;
					evalEvent = end;
				}
				if (parameter && parameter->vQueue && parameter->vQueue->mCount)
				{
					GrapaRuleQueue operationCallQueue;
					if (parameter->mId && parameter->mName.mLength)
					{
						GrapaRuleEvent* e2 = operationQueue.PopHead();
						while (e2)
						{
							// does this ever happen more than 1 itteration? 
							if (e2->vRuleParent && e2->vRuleParent->vRuleLambda)
							{
								GrapaRuleEvent* e3 = Plan(vScriptState->GetNameSpace(), e2);
								e2->CLEAR();
								delete e2;
								e2 = e3;
								if (e2 && e2->mValue.mToken == GrapaTokenType::OP)
								{
									GrapaRuleEvent* param = ProcessPlan(vScriptState->GetNameSpace(), e2);
									e2->CLEAR();
									delete e2;
									e2 = param;
								}
							}
							operationCallQueue.PushTail(e2);
							e2 = operationQueue.PopHead();
						}
					}
					else
					{
						GrapaRuleEvent* e = operationQueue.PopHead();
						while (e)
						{
							// does this ever happen more than 1 itteration? 
							operationCallQueue.PushTail(e);
							e = operationQueue.PopHead();
						}
					}
					GrapaRuleEvent* callop = (GrapaRuleEvent*)parameter->vQueue->Head();
					GrapaRuleEvent* param = ProcessPlan(vScriptState->GetNameSpace(), callop, (GrapaRuleEvent*)operationCallQueue.Head(), operationCallQueue.mCount);
					if (param == NULL)
					{
						param = new GrapaRuleEvent();
						param->SetNull();
					}
					operationQueue.PushTail(param);
					if (param->mValue.mToken == GrapaTokenType::ERR)
					{
						failed = true;
						break;
					}
				}
				while (operationQueue.Head())
				{
					// does this ever happen more than 1 itteration? 
					parameterQueue->PushTail(operationQueue.PopHead());
				}
				nextEvent = evalEvent;
			}
			else if (nextEvent->mValue.mToken != parameter->mValue.mToken || (parameter->mId && nextEvent->mId != parameter->mId))
			{
				failed = true;
				break;
			}
			else
			{
				if (parameter->mId == 0 && parameter->mValue.mLength)
				{
					if (parameter->mValue.mToken == GrapaTokenType::FLOAT)
					{
						GrapaFloat a(0), b(0);
						a.FromBytes(parameter->mValue);
						b.FromBytes(nextEvent->mValue);
						if (a != b)
						{
							failed = true;
							break;
						}
					}
					else if (parameter->mValue.StrCmp(nextEvent->mValue))
					{
						failed = true;
						break;
					}
				}
				GrapaRuleEvent* param = new GrapaRuleEvent(parameter->mId, parameter->mName, parameter->mValue, nextEvent);
				param->mSkip = parameter->mSkip;
				param->mRun = parameter->mRun;
				param->mStart = parameter->mStart;
				param->mEnd = parameter->mEnd;
				param->mEscape = parameter->mEscape;
				param->mExit = parameter->mExit;
				param->mQuote = parameter->mQuote;
				param->mNull = parameter->mNull;
				parameterQueue->PushTail(param);
				nextEvent->mTouch = pTouched;
				nextEvent = nextEvent->Next();
			}

			paramList[paramPos].eventPos = nextEvent;
			paramList[paramPos].operationPos = parameterQueue->Tail();
			paramPos++;

			if (parameter)
				parameter = parameter->Next();
			while (nextEvent && nextEvent->mSkip)
			{
				nextEvent->mTouch = pTouched;
				nextEvent = nextEvent->Next();
			}

			if (performedCatchall)
			{
				while (parameter && parameter->mValue.mToken == GrapaTokenType::RULEREF && (parameter->mId == 0 || parameter->mName.mLength == 0))
				{
					GrapaRuleEvent* callop = (GrapaRuleEvent*)parameter->vQueue->Head();
					GrapaRuleEvent* param = ProcessPlan(vScriptState->GetNameSpace(), callop);
					if (param)
					{
						parameterQueue->PushTail(param);
						if (param->mValue.mToken == GrapaTokenType::ERR)
						{
							failed = true;
							break;
						}
					}
					parameter = parameter->Next();
				}
				if (failed)
					break;
			}

			if (pushExtraToken)
			{
				GrapaRuleEvent* param = new GrapaRuleEvent(parameter->mId, parameter->mName, parameter->mValue, nextEvent);
				param->mSkip = parameter->mSkip;
				param->mRun = parameter->mRun;
				param->mStart = parameter->mStart;
				param->mEnd = parameter->mEnd;
				param->mEscape = parameter->mEscape;
				param->mExit = parameter->mExit;
				param->mQuote = parameter->mQuote;
				param->mNull = parameter->mNull;
				parameterQueue->PushTail(param);
				if (operationEvalEvent)
					nextEvent = operationEvalEvent;
				else
				{
					nextEvent->mTouch = pTouched;
					nextEvent = nextEvent->Next();
				}
				paramList[paramPos].eventPos = nextEvent;
				paramList[paramPos].operationPos = parameterQueue->Tail();
				paramPos++;
				parameter = parameter->Next();
				while (nextEvent && nextEvent->mSkip)
				{
					nextEvent->mTouch = pTouched;
					nextEvent = nextEvent->Next();
				}
			}
			else if (operationEvalEvent)
			{
				while (operationEvalQueue.Head())
					parameterQueue->PushTail(operationEvalQueue.PopHead());
				nextEvent = operationEvalEvent;
				paramList[paramPos].eventPos = nextEvent;
				paramList[paramPos].operationPos = parameterQueue->Tail();
				paramPos++;
				parameter = parameter->Next();
				while (nextEvent && nextEvent->mSkip)
				{
					nextEvent->mTouch = pTouched;
					nextEvent = nextEvent->Next();
				}
			}
		}

		if (failed || parameter)
			nextEvent = pTokenEvent;
		else
			pMatched = true;

		if ((nextEvent != pTokenEvent) || pMatched)
		{
			if (pLeftRecursion == 2)
			{
				if (pOperationQueue->Tail())
					delete pOperationQueue->PopTail();
			}
			operationEvent->vRuleParent = operation;
			pOperationQueue->PushTail(operationEvent);
			GrapaMem::Delete(paramList);
			return(nextEvent);
		}

		pAddLastNext = false;

		operation = operation->Next();
		if (pLeftRecursion)
		{
			while (operation)
			{
				parameter = (GrapaRuleEvent*)operation->vQueue->Head();
				if (parameter == NULL) break;
				if (pLeftRecursion == 1 && parameter->mName.StrCmp(pRuleEvent->mName) != 0) break;
				if (pLeftRecursion == 2 && parameter->mName.StrCmp(pRuleEvent->mName) == 0) break;
				operation = operation->Next();
			}
		}
		if (operation == NULL) break;

		if (parameter)
		{
			// skip any options starting with the same set of tokens that just failed
			while (operation)
			{
				GrapaRuleEvent* lastParam = parameter;
				GrapaRuleEvent* nextParam = (GrapaRuleEvent*)(operation ? operation->vQueue->Head(paramPos) : NULL);
				if (nextParam == NULL) break;
				if (lastParam->mValue.mToken == GrapaTokenType::RULEREF && (lastParam->mId == 0 || lastParam->mName.mLength == 0) && parameter->Next())
				{  // if OP and Next(),  include next for lastParam and nextParam in this comparison
					if (lastParam->vQueue && nextParam->vQueue)
					{
						if (lastParam->vQueue->mCount != nextParam->vQueue->mCount)
							break;
					}
					else
					{
						if (lastParam->vQueue && !nextParam->vQueue)
							break;
						if (!lastParam->vQueue && nextParam->vQueue)
							break;
					}
					lastParam = lastParam->Next();
					nextParam = nextParam->Next();
					if (lastParam == NULL || nextParam == NULL) break;
				}
				if ((lastParam->mValue.mToken != nextParam->mValue.mToken) || (lastParam->mId && lastParam->mId != nextParam->mId))
					break;
				else if (lastParam->mValue.mToken == GrapaTokenType::RULEREF)
				{
				}
				else
				{
					if (lastParam->mValue.mToken == GrapaTokenType::FLOAT)
					{
						GrapaFloat a(0), b(0);
						a.FromBytes(lastParam->mValue);
						b.FromBytes(nextParam->mValue);
						if (a != b)
							break;
					}
					else if (lastParam->mValue.mLength != nextParam->mValue.mLength)
						break;
					else if (lastParam->mValue.StrCmp(nextParam->mValue))
						break;
				}
				bool isEq = false;
				while (true)
				{
					lastParam = lastParam->Prev();
					nextParam = nextParam->Prev();
					if (lastParam == NULL || nextParam == NULL)
					{
						isEq = true;
						break;
					}
					if ((lastParam->mValue.mToken != nextParam->mValue.mToken) || (lastParam->mId && lastParam->mId != nextParam->mId))
						break;
					else if (lastParam->mValue.mToken == GrapaTokenType::RULEREF)
					{
						if (lastParam->vQueue && nextParam->vQueue)
						{
							if (lastParam->vQueue->mCount != nextParam->vQueue->mCount)
								break;
						}
						else
						{
							if (lastParam->vQueue && !nextParam->vQueue)
								break;
							if (!lastParam->vQueue && nextParam->vQueue)
								break;
						}
					}
					else if (lastParam->mValue.mToken == GrapaTokenType::FLOAT)
					{
						GrapaFloat a(0), b(0);
						a.FromBytes(lastParam->mValue);
						b.FromBytes(nextParam->mValue);
						if (a != b)
							break;
					}
					else if (lastParam->mValue.mLength != nextParam->mValue.mLength)
						break;
					else if (lastParam->mValue.StrCmp(nextParam->mValue))
						break;
				}
				if (!isEq) break;
				operation = operation->Next();
			}
		}

		if (operation == NULL)
			break;

		operationEvent->mId = operation->mId;
		operationEvent->mName.FROM(operation->mName);
		operationEvent->mValue.FROM(operation->mValue);

		GrapaRuleEvent* lastParam = (GrapaRuleEvent*)lastOperation->vQueue->Head();;
		GrapaRuleEvent* nextParam = (GrapaRuleEvent*)operation->vQueue->Head();;

		u32 oldPos = paramPos;
		paramPos = 0;

		while (oldPos && lastParam && nextParam)
		{
			if ((lastParam->mValue.mToken != nextParam->mValue.mToken) || (lastParam->mId && lastParam->mId != nextParam->mId))
				break;
			if (lastParam->mValue.mToken != GrapaTokenType::RULEREF && lastParam->mValue.mToken != GrapaTokenType::OP)
			{
				if (lastParam->mValue.mLength != nextParam->mValue.mLength)
					break;
				if (lastParam->mValue.StrCmp(nextParam->mValue))
					break;
			}
			parameter = nextParam;
			lastParam = lastParam->Next();
			nextParam = nextParam->Next();
			oldPos--;
			paramPos++;
		}
		if (paramPos)
		{
			parameter = parameter->Next();
			nextEvent = paramList[paramPos - 1].eventPos;
			GrapaRuleEvent* last = paramList[paramPos - 1].operationPos;
			while (parameterQueue->Tail() != last)
			{
				GrapaRuleEvent* e = parameterQueue->PopTail();
				e->CLEAR();
				delete e;
			}
		}
		else
		{
			parameter = (GrapaRuleEvent*)operation->vQueue->Head();
			nextEvent = pTokenEvent;
			parameterQueue->CLEAR();
		}
	}
	if (operationEvent)
	{
		operationEvent->CLEAR();
		delete operationEvent;
	}
	GrapaMem::Delete(paramList);
	return(pTokenEvent);
}

void GrapaScriptExec::LoadLib(GrapaRuleEvent *libName)
{
	if (libName && libName->vLibraryEvent == NULL)
	{
		s64 idx;
		GrapaCHAR s;
		if (libName->mValue.mToken == GrapaTokenType::SYSID)
		{
			s.FROM("$");
			s.Append(libName->mValue);
		}
		else
			s.FROM(libName->mValue);
		libName->vLibraryEvent = gSystem->mLibraryQueue.Search(s,idx);
		if (libName->vLibraryEvent == NULL)
		{
			if (s.StrCmp("$SYS") == 0) libName->vLibraryEvent = new GrapaLibraryRuleEvent(s);
			if (libName->vLibraryEvent)
				gSystem->mLibraryQueue.PushTail(libName->vLibraryEvent);
		}
	}
}

GrapaRuleEvent* GrapaScriptExec::ProcessPlan(GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleEvent* pParam, u64 pCount)
{
	GrapaRuleEvent* result = NULL;
	s64 idx;
	if (!pOperation) return(result);
	while (pOperation->mValue.mToken == GrapaTokenType::PTR && pOperation->vRulePointer) pOperation = pOperation->vRulePointer;
	if (pOperation->mValue.mToken == GrapaTokenType::CODE)
	{
		if (pOperation->vQueue && pOperation->vQueue->mCount)
		{
			GrapaRuleEvent *item = (GrapaRuleEvent*)pOperation->vQueue->Head();
			while (item)
			{
				GrapaRuleEvent* oldresult = result;
				result = ProcessPlan(pNameSpace, item, pParam, pCount);
				if (oldresult)
				{
					oldresult->CLEAR();
					delete oldresult;
					oldresult = NULL;
				}
				item = item->Next();
			}
		}
	}
	else if (pOperation->mValue.mToken == GrapaTokenType::OP)
	{
		if (pOperation->vQueue && pOperation->vQueue->mCount)
		{
			GrapaRuleEvent *libName = &gSystem->mLib;
			GrapaRuleEvent *libParam = (GrapaRuleEvent*)pOperation->vQueue->Head();
			GrapaRuleEvent *inputItem = libParam? libParam->Next():NULL;
			switch (pOperation->vQueue->mCount)
			{
			case 1:
				libName = &gSystem->mLib;
				libParam = (GrapaRuleEvent*)pOperation->vQueue->Head();
				break;
			case 2:
				inputItem = (GrapaRuleEvent*)pOperation->vQueue->Tail();
				if (inputItem && inputItem->mValue.mToken == GrapaTokenType::LIST && !inputItem->mNull)
				{
					libName = &gSystem->mLib;
					libParam = (GrapaRuleEvent*)pOperation->vQueue->Head();
				}
				else
				{
					libName = (GrapaRuleEvent*)pOperation->vQueue->Head();
					libParam = inputItem;
					inputItem = NULL;
					if (libName==NULL || libName->mValue.StrCmp("$SYS") == 0 || (libName->mValue.mToken == GrapaTokenType::SYSID && libName->mValue.StrCmp("SYS") == 0)) libName = &gSystem->mLib;
				}
				break;
			case 3:
				libName = (GrapaRuleEvent*)pOperation->vQueue->Head();
				libParam = libName ? libName->Next() : NULL;
				inputItem = libParam ? libParam->Next() : NULL;
				if (libName == NULL || libName->mValue.StrCmp("$SYS") == 0 || (libName->mValue.mToken == GrapaTokenType::SYSID && libName->mValue.StrCmp("SYS") == 0)) libName = &gSystem->mLib;
				break;
			}
			if (libName)
			{
				GrapaRuleQueue *input = inputItem ? (GrapaRuleQueue*)inputItem->vQueue : NULL;
				GrapaRuleQueue *inputDel = NULL;
				if (pParam)
				{
					GrapaRuleQueue* paramQueue = new GrapaRuleQueue();
					GrapaRuleEvent *e = input ? input->Head() : NULL;
					while (e)
					{
						GrapaRuleEvent *v = new GrapaRuleEvent(GrapaTokenType::PTR, 0, (char*)e->mName.mBytes);
						GrapaRuleEvent *eH = e;
						while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
						if (v->mName.mLength == 0)
						{
							v->mName.FROM(eH->mValue);
							v->SetNull();
						}
						else
						{
							v->vRulePointer = eH;
						}
						paramQueue->PushTail(v);
						e = e->Next();
					}
					e = (GrapaRuleEvent*)paramQueue->Head();
					while (pParam)
					{
						if (pParam->mName.mLength)
						{
							GrapaRuleEvent *v = (GrapaRuleEvent*)paramQueue->Search(pParam->mName,idx);
							if (v)
							{
								v->mValue.mToken = GrapaTokenType::PTR;
								v->mNull = false;
								v->vRulePointer = pParam;
								pParam = pParam->Next();
							}
							else
							{
								GrapaRuleEvent *v = new GrapaRuleEvent(GrapaTokenType::PTR, 0, (char*)pParam->mName.mBytes);
								GrapaRuleEvent *eH = pParam;
								while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
								v->vRulePointer = eH;
								paramQueue->PushTail(v);
								pParam = pParam->Next();
							}
						}
						else if (e)
						{
							e->mValue.mToken = GrapaTokenType::PTR;
							e->mNull = false;
							e->vRulePointer = pParam;
							pParam = pParam->Next();
							e = e->Next();
						}
						else
						{
							GrapaRuleEvent *v = new GrapaRuleEvent(GrapaTokenType::PTR, 0, (char*)pParam->mName.mBytes);
							GrapaRuleEvent *eH = pParam;
							while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
							v->vRulePointer = eH;
							paramQueue->PushTail(v);
							pParam = pParam->Next();
						}
						if (pCount)
						{
							pCount--;
							if (!pCount)
								break;
						}
					}
					input = inputDel = paramQueue;
				}
				LoadLib(libName);
				if (libName->vLibraryEvent)
				{
					result = libName->vLibraryEvent->Run(this, pNameSpace, libParam, input);
					if (result && !result->mVar && result->mValue.mToken == GrapaTokenType::PTR && result->vRulePointer)
					{
						GrapaRuleEvent *v = result->vRulePointer;
						while (v->mValue.mToken == GrapaTokenType::PTR && v->vRulePointer) v = v->vRulePointer;
						result->vRulePointer = v;
					}
					//if (result && result->mLocal)
					//{
					//	GrapaRuleEvent *v = CopyItem(result);
					//	result->CLEAR();
					//	delete result;
					//	result = v;
					//	if (result->mValue.mToken == GrapaTokenType::RULEOP)
					//		result->mValue.mToken = GrapaTokenType::LIST;
					//}
				}
				if (inputDel)
				{
					inputDel->CLEAR();
					delete inputDel;
					inputDel = NULL;
				}
			}
		}
	}
	else if (!pOperation->mNull)
	{
		result = CopyItem(pOperation);
	}
	if (result && !result->mVar && result->mValue.mToken == GrapaTokenType::PTR) //NEED TO FIX THIS
	{
		GrapaRuleEvent *v = CopyItem(result);
		result->CLEAR();
		delete result;
		result = v;
	}
	return(result);
}

GrapaRuleEvent* GrapaScriptExec::PlanProcess(GrapaNames* pNameSpace, GrapaRuleQueue *pOperationQueue)
{
	if (pOperationQueue == NULL || !pOperationQueue->mCount) return(NULL);
	GrapaRuleEvent* result = NULL;
	result = new GrapaRuleEvent(GrapaTokenType::CODE, 0, "", "");
	result->vQueue = new GrapaRuleQueue();
	GrapaRuleEvent* scanOps2 = pOperationQueue->Head();
	while (scanOps2)
	{
		GrapaRuleEvent* result3 = NULL;
		GrapaRuleEvent* scanOps3 = scanOps2;
		while (scanOps3->vQueue && !scanOps3->mName.mLength && scanOps3->vQueue->mCount == 1)
			scanOps3 = ((GrapaRuleQueue*)scanOps3->vQueue)->Head();
		if (scanOps3->vQueue)
		{
			if (scanOps3->mName.mLength == 0)
			{
				while (scanOps3->vQueue->mCount)
					pOperationQueue->PushNext(scanOps2, ((GrapaRuleQueue*)scanOps3->vQueue)->PopTail());
			}
			else
			{
				result3 = Plan(pNameSpace,scanOps3);
				if (result3) result->vQueue->PushTail(result3);
			}
		}
		scanOps2 = scanOps2->Next();
	}
	if (result)
	{
		while (result->mValue.mToken == GrapaTokenType::CODE && result->vQueue)
		{
			if (result->vQueue->mCount == 1)
			{
				GrapaRuleEvent *t = (GrapaRuleEvent*)result->vQueue->PopHead();
				result->CLEAR();
				delete result;
				result = t;
			}
			else if (result->vQueue->mCount == 0)
			{
				result->SetNull();
			}
			else
				break;
		}
	}
	return(result);
}

GrapaLibraryEvent::~GrapaLibraryEvent()
{ 
	if (vLibraryQueue) 
	{ 
		delete vLibraryQueue; 
		vLibraryQueue = NULL; 
	} 
}

GrapaLibraryParam::GrapaLibraryParam(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent*v, bool pEval)
{
	vVal = v;
	while (vVal && vVal->mValue.mToken == GrapaTokenType::PTR) vVal = vVal->vRulePointer;
	vDel = NULL;
	if (pEval && vVal && (vVal->mValue.mToken == GrapaTokenType::OP || vVal->mValue.mToken == GrapaTokenType::CODE))
	{
		vVal = vDel = vScriptExec->ProcessPlan(pNameSpace, vVal);
		while (vVal && vVal->mValue.mToken == GrapaTokenType::PTR) vVal = vVal->vRulePointer;
	}
}

GrapaLibraryParam::~GrapaLibraryParam() 
{ 
	if (vDel)
	{
		vDel->CLEAR();
		delete vDel;
		vDel = NULL;
	}
}
//
//GrapaError GrapaScriptExec::GetParameterKey(GrapaNames* pNameSpace, GrapaRuleEvent* pInputParam, GrapaEncode& pKey)
//{
//	GrapaError err = 0;
//	GrapaRuleEvent* n = NULL;
//	GrapaRuleEvent* e = NULL;
//	GrapaRuleEvent* d = NULL;
//	GrapaRuleEvent* p = NULL;
//	GrapaRuleEvent* q = NULL;
//	GrapaRuleEvent* dp = NULL;
//	GrapaRuleEvent* dq = NULL;
//	GrapaRuleEvent* qi = NULL;
//	GrapaRuleEvent* r1 = NULL;
//	if (GrapaRuleEvent* p1 = pInputParam)
//	{
//		s64 idx;
//		GrapaRuleEvent* item = p1;
//		if (item->mValue.mToken == GrapaTokenType::LIST)
//		{
//			if (item->vQueue)
//			{
//				GrapaRuleQueue* sq = (GrapaRuleQueue*)item->vQueue;
//				n = (GrapaRuleEvent*)sq->Search("n",idx);
//				e = (GrapaRuleEvent*)sq->Search("e", idx);
//				d = (GrapaRuleEvent*)sq->Search("d", idx);
//				p = (GrapaRuleEvent*)sq->Search("p", idx);
//				q = (GrapaRuleEvent*)sq->Search("q", idx);
//				dp = (GrapaRuleEvent*)sq->Search("dp", idx);
//				dq = (GrapaRuleEvent*)sq->Search("dq", idx);
//				qi = (GrapaRuleEvent*)sq->Search("qi", idx);
//			}
//		}
//		else if (item->mValue.mToken == GrapaTokenType::PTR && item->vRulePointer)
//		{
//			if (item->vRulePointer->vQueue)
//			{
//				GrapaRuleQueue* sq = (GrapaRuleQueue*)item->vRulePointer->vQueue;
//				n = (GrapaRuleEvent*)sq->Search("n", idx);
//				e = (GrapaRuleEvent*)sq->Search("e", idx);
//				d = (GrapaRuleEvent*)sq->Search("d", idx);
//				p = (GrapaRuleEvent*)sq->Search("p", idx);
//				q = (GrapaRuleEvent*)sq->Search("q", idx);
//				dp = (GrapaRuleEvent*)sq->Search("dp", idx);
//				dq = (GrapaRuleEvent*)sq->Search("dq", idx);
//				qi = (GrapaRuleEvent*)sq->Search("qi", idx);
//			}
//		}
//		else if (item->mValue.mToken == GrapaTokenType::OP && item->vQueue)
//		{
//            r1 = ProcessPlan(pNameSpace,item,NULL);
//			if (r1)
//			{
//				if (r1->mValue.mToken == GrapaTokenType::LIST)
//				{
//					GrapaRuleQueue* sq = (GrapaRuleQueue*)r1->vQueue;
//					n = (GrapaRuleEvent*)sq->Search("n", idx);
//					e = (GrapaRuleEvent*)sq->Search("e", idx);
//					d = (GrapaRuleEvent*)sq->Search("d", idx);
//					p = (GrapaRuleEvent*)sq->Search("p", idx);
//					q = (GrapaRuleEvent*)sq->Search("q", idx);
//					dp = (GrapaRuleEvent*)sq->Search("dp", idx);
//					dq = (GrapaRuleEvent*)sq->Search("dq", idx);
//					qi = (GrapaRuleEvent*)sq->Search("qi", idx);
//				}
//				else if (r1->mValue.mToken == GrapaTokenType::PTR && r1->vRulePointer)
//				{
//					if (r1->vRulePointer->vQueue)
//					{
//						GrapaRuleQueue* sq = (GrapaRuleQueue*)r1->vRulePointer->vQueue;
//						n = (GrapaRuleEvent*)sq->Search("n", idx);
//						e = (GrapaRuleEvent*)sq->Search("e", idx);
//						d = (GrapaRuleEvent*)sq->Search("d", idx);
//						p = (GrapaRuleEvent*)sq->Search("p", idx);
//						q = (GrapaRuleEvent*)sq->Search("q", idx);
//						dp = (GrapaRuleEvent*)sq->Search("dp", idx);
//						dq = (GrapaRuleEvent*)sq->Search("dq", idx);
//						qi = (GrapaRuleEvent*)sq->Search("qi", idx);
//					}
//				}
//			}
//		}
//		else if (item->mValue.mToken == GrapaTokenType::ID && item->vValueEvent)
//		{
//			GrapaRuleEvent* operation = pNameSpace->GetNameQueue()->Head();
//			while (operation)
//			{
//				if (GrapaRuleEvent* parameter = (GrapaRuleEvent*)operation->vQueue->Search(item->vValueEvent->mValue, idx))
//				{
//					if (parameter->vQueue)
//					{
//						GrapaRuleQueue* sq = (GrapaRuleQueue*)parameter->vQueue;
//						n = (GrapaRuleEvent*)sq->Search("n", idx);
//						e = (GrapaRuleEvent*)sq->Search("e", idx);
//						d = (GrapaRuleEvent*)sq->Search("d", idx);
//						p = (GrapaRuleEvent*)sq->Search("p", idx);
//						q = (GrapaRuleEvent*)sq->Search("q", idx);
//						dp = (GrapaRuleEvent*)sq->Search("dp", idx);
//						dq = (GrapaRuleEvent*)sq->Search("dq", idx);
//						qi = (GrapaRuleEvent*)sq->Search("qi", idx);
//					}
//					break;
//				}
//				operation = operation->Prev();
//			}
//		}
//	}
//	err = -1;
//	if (n && e && d)
//	{
//		GrapaInt nx,ex,dx,px,qx,dpx,dqx,qix;
//		nx.FromBytes(n->mValue);
//		ex.FromBytes(e->mValue);
//		dx.FromBytes(d->mValue);
//		if (p && q)
//		{
//			px.FromBytes(p->mValue);
//			qx.FromBytes(q->mValue);
//		}
//		if (dp && dq && qi)
//		{
//			dpx.FromBytes(dp->mValue);
//			dqx.FromBytes(dq->mValue);
//			qix.FromBytes(qi->mValue);
//		}
//		pKey.SetPrivateKey(nx, ex, dx, px, qx, dpx, dqx, qix);
//		err = 0;
//	}
//	else if (n && e)
//	{
//		GrapaInt num1, num2;
//		num1.FromBytes(e->mValue);
//		num2.FromBytes(n->mValue);
//		pKey.SetPublicKey(num1, num2);
//		err = 0;
//	}
//	if (r1) 
//	{ 
//		r1->CLEAR(); 
//		delete r1; 
//	}
//	return(err);
//}

GrapaRuleEvent* GrapaScriptExec::CopyItem(GrapaRuleEvent* pAction, bool isTAG, bool isConst)
{
	if (pAction == NULL) return(NULL);
	GrapaRuleEvent* p = pAction;
	while (p->mValue.mToken == GrapaTokenType::PTR && p->vRulePointer) p = p->vRulePointer;
	GrapaRuleEvent* result = new GrapaRuleEvent(pAction->mId, pAction->mName, p->mValue);
	if (p->vQueue) result->vQueue = CopyQueue((GrapaRuleQueue*)p->vQueue, isTAG, isConst);
	if (p->vRuleLambda) result->vRuleLambda = CopyItem(p->vRuleLambda,false, isConst);
	if (p->vValueEvent) result->vValueEvent = CopyItem(p->vValueEvent, false, isConst);
	result->vRulePointer = p->vRulePointer;
	result->vRuleParent = p->vRuleParent;
	result->vClass = p->vClass;
	result->mVar = p->mVar;
	result->mSkip = p->mSkip;
	result->mT = p->mT;
	result->mRun = p->mRun;
	result->mStart = p->mStart;
	result->mEnd = p->mEnd;
	result->mEscape = p->mEscape;
	result->mExit = p->mExit;
	result->mNull = p->mNull;
	result->mQuote = p->mQuote;
	result->mConst = isConst;
	if (isTAG)
	{
		if (result->mValue.mToken == GrapaTokenType::TAG)
			result->mValue.mToken = GrapaTokenType::LIST;
		else if (result->mValue.mToken == GrapaTokenType::EL)
			result->mValue.mToken = GrapaTokenType::ARRAY;
		else if (result->mValue.mToken == GrapaTokenType::XML)
			result->mValue.mToken = GrapaTokenType::ARRAY;
	}

	if (p->vDatabase)
	{
		result->vDatabase = new GrapaLocalDatabase(p->vDatabase->vScriptState);
		result->vDatabase->mHomeDir.FROM(p->vDatabase->mHomeDir);
		result->vDatabase->vScriptState = p->vDatabase->vScriptState;
		if (p->vDatabase->mInclude) result->vDatabase->mInclude = CopyQueue((GrapaRuleQueue*)p->vDatabase->mInclude,false,isConst);
		result->vDatabase->mLocation.FROM(p->vDatabase->mLocation);
		
		if (p->vDatabase->mVar)
			result->vDatabase->DatabaseSet(result->mValue);
		else
			result->vDatabase->HomeSwitch(p->vDatabase->mHomeDir);
		GrapaCHAR path;
		p->vDatabase->DirectoryPWD(path);
		result->vDatabase->DirectorySwitch(path);

		result->vDatabase->mSep = p->vDatabase->mSep;
	}
	if (p->vVector)
	{
		result->vVector = new GrapaVector();
		result->vVector->FROM(*p->vVector);
	}
	if (p->vWidget)
	{
		if (pAction->mValue.mToken == GrapaTokenType::PTR)
		{
			result->mValue.mToken = GrapaTokenType::PTR;
			result->vRulePointer = p->vWidget->vPtrEvent;
		}
		else
		{
			result->vWidget = new GrapaWidget(p->vWidget->vScriptExec, p->vWidget->vNameSpace);
			result->vWidget->FROM(*p->vWidget);
			result->vWidget->vEvent = result;
		}
	}
	if (p->vNetConnect)
	{
	}
	if (p->vWorker)
	{
	}

	return(result);
}

GrapaRuleQueue* GrapaScriptExec::CopyQueue(GrapaRuleQueue* pList, bool isTAG, bool isConst)
{
	if (pList == NULL) return(NULL);
	GrapaRuleQueue* result = new GrapaRuleQueue();
	GrapaRuleEvent* item = pList->Head();
	while (item)
	{
		GrapaRuleEvent* ev = CopyItem(item, isTAG, isConst);
		if (isTAG)
		{
			if (ev->mValue.mToken == GrapaTokenType::TAG)
				ev->mValue.mToken = GrapaTokenType::LIST;
			else if (ev->mValue.mToken == GrapaTokenType::EL)
				ev->mValue.mToken = GrapaTokenType::LIST;
			else if (ev->mValue.mToken == GrapaTokenType::XML)
				ev->mValue.mToken = GrapaTokenType::ARRAY;
		}
		result->PushTail(ev);
		item = item->Next();
	}
	result->mConst = isConst;
	return(result);
}

void GrapaScriptExec::ReplaceLocalQueue(GrapaRuleQueue* pList, GrapaRuleQueue* pLocal)
{
	return;
	if (pList == NULL) return;
	GrapaRuleEvent* item = pList->Head();
	while (item)
	{
		switch (item->mValue.mToken)
		{
		case GrapaTokenType::PTR:
			if (item->mVar && pLocal->HasEvent(item->vRulePointer))
			{
				GrapaRuleEvent* v = CopyItem(item);
				pList->ReplaceEvent(item, v);
				item->CLEAR();
				delete item;
				item = v;
			}
			break;
		case GrapaTokenType::LIST:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::OBJ:
			ReplaceLocalQueue((GrapaRuleQueue*)item->vQueue, pLocal);
			break;
		}
		item = item->Next();
	}
}

void GrapaScriptExec::EscapeString(const GrapaCHAR& pFrom, GrapaCHAR& pTo)
{
	char s[12];
	char* str = (char*)pFrom.mBytes;
	pTo.SetSize(pFrom.mLength * 2 + 1);
	pTo.SetLength(0);
	// look in mIdentifiers, mEscapeFrom, mEscapeTo, mEscapeHex

	for (u64 i = 0; i<pFrom.mLength; i++)
	{
		// if in mIdentifiers
		if (char *from = strchr(vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::STRTO], str[i]))
		{
			s[0] = vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::STRESC][0];
			s[1] = vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::STRFROM][from - vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::STRTO]];
			s[2] = 0;
		}
		else if (strchr(vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::ID], str[i]))
		{
			s[0] = str[i];
			s[1] = 0;
		}
		else if (strchr(vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::DIG], str[i]))
		{
			s[0] = str[i];
			s[1] = 0;
		}
		else if (strchr(vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::SYM], str[i]))
		{
			s[0] = str[i];
			s[1] = 0;
		}
		else
		{
			s[0] = vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::STRESC][0];
			s[1] = 'x';
			s[2] = vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::HEX][((str[i] >> 4) & 0x0F) + 16];
			s[3] = vScriptState->mItemState.mItemParams->mParam[GrapaItemEnum::HEX][(str[i] & 0x0F) + 16];
			s[4] = 0;
		}
		pTo.Append(s);
	}
	pTo.SetSize(pTo.mLength);
}

void GrapaScriptExec::EchoString(GrapaSystemSend* pSend, GrapaCHAR& pStr)
{
	GrapaCHAR to;
	EscapeString(pStr, to);
	pSend->Send(this, vScriptState->vRuleVariables, to);
}

void GrapaScriptExec::Echo(GrapaSystemSend* pSend, GrapaObjectEvent* pTokenEvent, GrapaCHAR& msg)
{
	GrapaInt a;
	char istr[128];

	msg.SetLength(0);

	if (pTokenEvent==NULL) return;
	while(pTokenEvent)
	{
		if (pTokenEvent->mId)
		{
			msg.Append("TOKEN(");
			switch (pTokenEvent->mValue.mToken)
			{
			case GrapaTokenType::SYM:
				msg.Append("SYM,");
				break;
			case GrapaTokenType::BOOL:
				msg.Append("BOOL,");
				break;
			case GrapaTokenType::INT:
				msg.Append("INT,");
				break;
				//case GrapaBYTE::DEC:
				//	msg.Append("DEC,");
				//	break;
			case GrapaTokenType::FLOAT:
				msg.Append("FLOAT,");
				break;
			case GrapaTokenType::ID:
				msg.Append("ID,");
				break;
			case GrapaTokenType::STR:
				msg.Append("STR,");
				break;
			case GrapaTokenType::SYSSYM:
				msg.Append("SYSSYM,");
				break;
			case GrapaTokenType::SYSINT:
				msg.Append("SYSINT,");
				break;
			case GrapaTokenType::SYSID:
				msg.Append("SYSID,");
				break;
			case GrapaTokenType::SYSSTR:
				msg.Append("SYSSTR,");
				break;
			default: break;
			}
			msg.AppendQuoted(pTokenEvent->mName,true);
			//msg.Append(",");
			//GrapaMem::a2s64(pTokenEvent->mId, istr, sizeof(istr));
			//msg.Append(istr);
			if (pTokenEvent->mTouch)
				msg.Append(",1");
			else 
				msg.Append(",0");
			msg.Append(") ");
		}
		else 
		{
			switch (pTokenEvent->mValue.mToken)
			{
				case GrapaTokenType::RULEREF:
					msg.Append("RULE(");
					msg.Append(pTokenEvent->mValue);
					if (pTokenEvent->mTouch)
						msg.Append(",1");
					else
						msg.Append(",0");
					msg.Append(") ");
					break;
				case GrapaTokenType::MAX:
					msg.Append("TOKEN(");
					msg.Append(pTokenEvent->mValue);
					if (pTokenEvent->mTouch)
						msg.Append(",1");
					else
						msg.Append(",0");
					msg.Append(") ");
					break;
				default:
					msg.Append("ITEM(");
					switch (pTokenEvent->mValue.mToken)
					{
						case GrapaTokenType::SYM:
							msg.Append("SYM");
							if (pTokenEvent->mValue.mLength) { msg.Append(","); msg.Append(pTokenEvent->mValue); }
							break;
						case GrapaTokenType::INT:
							a.FromBytes(pTokenEvent->mValue);
							msg.Append("INT");
							msg.Append(",");
							msg.Append(a.ToString(10));
							break;
						case GrapaTokenType::FLOAT:
						{
							GrapaFloat d(vScriptState->mItemState.mFloatFix, vScriptState->mItemState.mFloatMax, vScriptState->mItemState.mFloatExtra, 0);
							d.FromBytes(pTokenEvent->mValue);
							msg.Append("FLOAT");
							msg.Append(",");
							msg.Append(d.ToString(10));
						}
							break;
						case GrapaTokenType::ID:
							msg.Append("ID");
							if (pTokenEvent->mValue.mLength) { msg.Append(","); msg.Append(pTokenEvent->mValue); }
							break;
						case GrapaTokenType::STR:
							msg.Append("STR");
							if (pTokenEvent->mValue.mLength)
							{
								msg.Append(",");
								msg.AppendQuoted(pTokenEvent->mValue,false);
							}
							break;
						case GrapaTokenType::BOOL:
							msg.Append(",");
							msg.Append((pTokenEvent->mValue.mBytes && pTokenEvent->mValue.mLength && pTokenEvent->mValue.mBytes[0] && pTokenEvent->mValue.mBytes[0] != '0' && !pTokenEvent->mNull)?"true":"false");
							break;
						case GrapaTokenType::SYSSYM:
							msg.Append("SYSSYM");
							if (pTokenEvent->mValue.mLength) { msg.Append(","); msg.Append(pTokenEvent->mValue); }
							break;
						case GrapaTokenType::SYSINT:
							a.FromBytes(pTokenEvent->mValue);
							msg.Append("SYSINT");
							msg.Append(",");
							if (pTokenEvent->mNull)
								msg.Append("$");
							else
								msg.Append(a.ToString(10));
							break;
						case GrapaTokenType::SYSID:
							msg.Append("SYSID");
							if (pTokenEvent->mValue.mLength) { msg.Append(","); msg.Append(pTokenEvent->mValue); }
							break;
						case GrapaTokenType::SYSSTR:
							msg.Append("SYSSTR");
							if (pTokenEvent->mValue.mLength) 
							{ 
								msg.Append(",");
								msg.AppendQuoted(pTokenEvent->mValue,false);
							}
							break;
						case GrapaTokenType::OTHER:
							msg.Append("OTHER,");
							GrapaMem::a2s64((u64)pTokenEvent->mValue.mLength, istr, sizeof(istr));
							msg.Append(istr);
							break;
                        default: break;
					}
					if (pTokenEvent->mTouch)
						msg.Append(",1");
					else
						msg.Append(",0");
					msg.Append(") ");
			}
		}
		pTokenEvent = pTokenEvent->Next();
	}
}

void GrapaScriptExec::EchoValue(GrapaSystemSend* pSend, GrapaRuleEvent* pTokenEvent, bool elMode, bool strquote, bool fromlist)
{
	GrapaInt n;
	GrapaFloat d(vScriptState->mItemState.mFloatFix, vScriptState->mItemState.mFloatMax, vScriptState->mItemState.mFloatExtra, 0);
	GrapaTime t;
	GrapaCHAR s,s2;
	GrapaRuleEvent* data;
	if (!elMode && (fromlist || pTokenEvent->mNull) && pTokenEvent->mValue.mToken!=GrapaTokenType::BOOL && pTokenEvent->mValue.mToken != GrapaTokenType::PTR && pTokenEvent->mValue.mToken != GrapaTokenType::VECTOR && pTokenEvent->vQueue == NULL)
	{
		if (pTokenEvent == NULL || pTokenEvent->mValue.mBytes == NULL || (pTokenEvent->mValue.mToken == GrapaTokenType::SYSID && pTokenEvent->mValue.mLength == 0))
		{
			pSend->Send(this, vScriptState->vRuleVariables, "null");
			return;
		}
	}
	switch (pTokenEvent->mValue.mToken)
	{
	case GrapaTokenType::RAW:
		pTokenEvent->mValue.ToHex(s);
		pSend->Send(this, vScriptState->vRuleVariables, "0x");
		pSend->Send(this, vScriptState->vRuleVariables, s);
		break;
	case GrapaTokenType::BOOL:
		pSend->Send(this, vScriptState->vRuleVariables, (pTokenEvent->mValue.mBytes && pTokenEvent->mValue.mLength && pTokenEvent->mValue.mBytes[0] && pTokenEvent->mValue.mBytes[0] != '0' && !pTokenEvent->mNull) ? "true" : "false");
		break;
	case GrapaTokenType::FLOAT:
		d.FromBytes(pTokenEvent->mValue);
		pSend->Send(this, vScriptState->vRuleVariables, d.ToString(10));
		break;
	case GrapaTokenType::TIME:
		t.FromBytes(pTokenEvent->mValue);
		pSend->Send(this, vScriptState->vRuleVariables, t.getString());
		//pSend->Send(GrapaTime::Epoch(n.LongValue()));
		break;
	case GrapaTokenType::TABLE:
		if (pTokenEvent->mValue.mLength) EchoDB(pSend, pTokenEvent->mValue);
		break;
	case GrapaTokenType::PTR:
		if (pTokenEvent->vRulePointer && (pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::ERR || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::ARRAY || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::TUPLE || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::LIST || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::XML || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::EL || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::TAG || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::OP || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::OBJ || pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::ERR))
			EchoList(pSend, pTokenEvent->vRulePointer,elMode, false, false);
		else if (pTokenEvent->vRulePointer && pTokenEvent->vRulePointer->mValue.mToken == GrapaTokenType::PTR)
			EchoList(pSend, pTokenEvent->vRulePointer,elMode, false, false);
		else
			EchoValue(pSend, pTokenEvent->vRulePointer,elMode, strquote, fromlist);
		break;
	case GrapaTokenType::SYSINT:
		pSend->Send(this, vScriptState->vRuleVariables, "$");
	case GrapaTokenType::INT:
		n.FromBytes(pTokenEvent->mValue);
		pSend->Send(this, vScriptState->vRuleVariables, n.ToString(10));
		break;
	case GrapaTokenType::SYSID:
		pSend->Send(this, vScriptState->vRuleVariables, "$");
	case GrapaTokenType::ID:
		if (elMode || !strquote)
		{
			pSend->Send(this, vScriptState->vRuleVariables, pTokenEvent->mValue);
		}
		else
		{
			s.AppendQuoted(pTokenEvent->mValue, true);
			pSend->Send(this, vScriptState->vRuleVariables, s);
		}
		break;
	case GrapaTokenType::SYSSTR:
		pSend->Send(this, vScriptState->vRuleVariables, "$");
	case GrapaTokenType::STR:
		if (elMode || !strquote)
		{
			pSend->Send(this, vScriptState->vRuleVariables, pTokenEvent->mValue);
		}
		else
		{
			s.AppendQuoted(pTokenEvent->mValue, false);
			//if (fromlist)
			//{
			//	EscapeString(s, s2);
			//	pSend->Send(s2);
			//}
			//else
				pSend->Send(this, vScriptState->vRuleVariables, s);
		}
		break;
	case GrapaTokenType::SYSSYM:
		pSend->Send(this, vScriptState->vRuleVariables, "$");
	case GrapaTokenType::SYM:
		s.AppendQuoted(pTokenEvent->mValue, false);
		pSend->Send(this, vScriptState->vRuleVariables, s);
		break;
	case GrapaTokenType::CLASS:
		EchoClassValue(pSend, pTokenEvent, elMode, false);
		break;
	case GrapaTokenType::TOKEN:
		EchoTokenValue(pSend, pTokenEvent, elMode, false);
		break;
	case GrapaTokenType::VECTOR:
		pSend->Send(this, vScriptState->vRuleVariables, "#");
		data = pTokenEvent->vVector->ToArray();
		EchoList(pSend, data, elMode, false, false);
		delete data;
		pSend->Send(this, vScriptState->vRuleVariables, "#");
		break;
	case GrapaTokenType::WIDGET:
		pSend->Send(this, vScriptState->vRuleVariables, "$WIDGET");
		break;
	case GrapaTokenType::RULE:
		EchoRuleValue(pSend, pTokenEvent, elMode, false, true);
		break;
	default:
		s.AppendQuoted(pTokenEvent->mValue,false);
		pSend->Send(this, vScriptState->vRuleVariables, s);
		break;
	}
}

void GrapaScriptExec::EchoDB(GrapaSystemSend* pSend, GrapaFile& pValue)
{
	GrapaError err;
	GrapaGroup g;
	GrapaCHAR b;
	GrapaInt n;

	err = g.OpenFile(&pValue, GrapaReadWrite);

	GrapaCHAR s,s2;
	char buffer[128];
	//u64 maxCol = 8;

	GrapaDBFieldArray* fieldList = g.ListFields(g.mRootTable, g.mRootType);

	if (fieldList)
	{
		for (u32 i = 0; i < fieldList->Count(); i++)
		{
			GrapaDBField *field = fieldList->GetFieldAt(i);
			if (!field) continue;
			err = g.GetData(field->mNameRef, s);
			field->mRef = s.mLength;
			if (field->mRef > 64)
				field->mRef = 64;
		}
		GrapaDBCursor cursor;
		cursor.Set(g.mRootTable);
		if (g.First(cursor) == 0)
		{
			do
			{
				for (u32 i = 0; i < fieldList->Count(); i++)
				{
					GrapaDBField *field = fieldList->GetFieldAt(i);
					if (!field) continue;
					err = g.GetRecordField(cursor, *field, s);
					switch (s.mToken)
					{
					case GrapaTokenType::ERR:
						if (field->mRef < 5)
							field->mRef = 5;
						break;
					case GrapaTokenType::BOOL:
						if (field->mRef < 6)
							field->mRef = 6;
						break;
					case GrapaTokenType::ARRAY:
						if (field->mRef < 7)
							field->mRef = 7;
						break;
					case GrapaTokenType::TUPLE:
						if (field->mRef < 7)
							field->mRef = 7;
						break;
					case GrapaTokenType::VECTOR:
						if (field->mRef < 8)
							field->mRef = 8;
						break;
					case GrapaTokenType::WIDGET:
						if (field->mRef < 8)
							field->mRef = 8;
						break;
					case GrapaTokenType::LIST:
						if (field->mRef < 6)
							field->mRef = 6;
						break;
					case GrapaTokenType::XML:
						if (field->mRef < 5)
							field->mRef = 5;
						break;
					case GrapaTokenType::EL:
						if (field->mRef < 4)
							field->mRef = 4;
						break;
					case GrapaTokenType::TAG:
						if (field->mRef < 5)
							field->mRef = 5;
						break;
					case GrapaTokenType::OP:
						if (field->mRef < 4)
							field->mRef = 4;
						break;
					case GrapaTokenType::OBJ:
						if (field->mRef < 5)
							field->mRef = 5;
						break;
					case GrapaTokenType::CODE:
						if (field->mRef < 6)
							field->mRef = 6;
						break;
					case GrapaTokenType::TABLE:
						if (field->mRef < 7)
							field->mRef = 7;
						break;
					default:
						if (field->mRef < s.mLength)
							field->mRef = s.mLength;
						if (field->mRef > 64)
							field->mRef = 64;
						break;
					}
				}
			} while (!(err = g.Next(cursor)));
		}


		for (u32 i = 0; i < fieldList->Count(); i++)
		{
			GrapaDBField *field = fieldList->GetFieldAt(i);
			if (!field) continue;
			err = g.GetData(field->mNameRef, s);
			if (s.mLength>field->mRef) s.SetLength(field->mRef);
			GrapaCHAR sformat("%-");
			sformat.Append(field->mRef);
			sformat.Append("s ");
			sprintf(buffer, (char*)sformat.mBytes, (char*)s.mBytes);
			b.Append(buffer);
		}

		cursor.Set(g.mRootTable);
		if (g.First(cursor) == 0)
		{
			do
			{
				b.Append("\r\n");
				for (u32 i = 0; i < fieldList->Count(); i++)
				{
					GrapaDBField *field = fieldList->GetFieldAt(i);
					if (!field) continue;
					err = g.GetRecordField(cursor, *field, s);
					switch (s.mToken)
					{
					case GrapaTokenType::ERR:
						if (s.mLength) 
							s.FROM("<ERR>"); 
						else 
							s.SetLength(0);
						break;
					case GrapaTokenType::BOOL:
						if (s.mBytes && s.mLength && s.mBytes[0] && s.mBytes[0]!='0')
							s.FROM("true");
						else
							s.FROM("false");
						break;
					case GrapaTokenType::ARRAY:
						if (s.mLength)
							s.FROM("<ARRAY>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::TUPLE:
						if (s.mLength)
							s.FROM("<TUPLE>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::VECTOR:
						if (s.mLength)
							s.FROM("<VECTOR>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::WIDGET:
						if (s.mLength)
							s.FROM("<WIDGET>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::LIST:
						if (s.mLength)
							s.FROM("<LIST>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::XML:
						if (s.mLength)
							s.FROM("<XML>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::EL:
						if (s.mLength)
							s.FROM("<EL>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::TAG:
						if (s.mLength)
							s.FROM("<TAG>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::OBJ:
						if (s.mLength)
							s.FROM("<OBJ>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::OP:
						if (s.mLength)
							s.FROM("<OP>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::CODE:
						if (s.mLength)
							s.FROM("<CODE>");
						else
							s.SetLength(0);
						break;
					case GrapaTokenType::TABLE:
						if (s.mLength)
							s.FROM("<DB>");
						else
							s.SetLength(0);
						break;
					default:
						EscapeString(s.ToStr(), s);
						break;
					}
					if (s.mLength>field->mRef) s.SetLength(field->mRef);
					GrapaCHAR sformat("%-");
					sformat.Append(field->mRef);
					sformat.Append("s ");
					sprintf(buffer, (char*)sformat.mBytes, (char*)s.mBytes);
					b.Append(buffer);
				}
			} while (!(err = g.Next(cursor)));
		}
		delete fieldList;
	}

	pSend->Send(this, vScriptState->vRuleVariables, b);
}

void GrapaScriptExec::EchoTokenValue(GrapaSystemSend* pSend, GrapaRuleEvent* pEvent, bool elMode, bool pretty)
{
	if (pretty && pEvent->mName.mLength)
	{
		pSend->Send(this, vScriptState->vRuleVariables, "@global[\"");
		pSend->Send(this, vScriptState->vRuleVariables, pEvent->mName);
		pSend->Send(this, vScriptState->vRuleVariables, "\"] = token ");
	}
	else
	{
		pSend->Send(this, vScriptState->vRuleVariables, "token ");
	}
	switch (pEvent->mT)
	{
	case GrapaTokenType::ERR:   pSend->Send(this,vScriptState->vRuleVariables,"$ERR"); break;
	case GrapaTokenType::RAW:   pSend->Send(this, vScriptState->vRuleVariables, "$RAW"); break;
	case GrapaTokenType::BOOL: pSend->Send(this, vScriptState->vRuleVariables, "$BOOL"); break;
	case GrapaTokenType::INT: pSend->Send(this, vScriptState->vRuleVariables, "$INT"); break;
	case GrapaTokenType::FLOAT: pSend->Send(this, vScriptState->vRuleVariables, "$FLOAT"); break;
	case GrapaTokenType::STR:   pSend->Send(this, vScriptState->vRuleVariables, "$STR"); break;
	case GrapaTokenType::TIME:  pSend->Send(this, vScriptState->vRuleVariables, "$TIME"); break;
	case GrapaTokenType::ARRAY: pSend->Send(this, vScriptState->vRuleVariables, "$ARRAY "); break;
	case GrapaTokenType::TUPLE: pSend->Send(this, vScriptState->vRuleVariables, "$TUPLE "); break;
	case GrapaTokenType::VECTOR: pSend->Send(this, vScriptState->vRuleVariables, "$VECTOR "); break;
	case GrapaTokenType::WIDGET: pSend->Send(this, vScriptState->vRuleVariables, "$WIDGET "); break;
	case GrapaTokenType::LIST: pSend->Send(this, vScriptState->vRuleVariables, "$LIST "); break;
	case GrapaTokenType::OBJ: pSend->Send(this, vScriptState->vRuleVariables, "$OBJ "); break;
	case GrapaTokenType::XML: pSend->Send(this, vScriptState->vRuleVariables, "$XML "); break;
	case GrapaTokenType::EL: pSend->Send(this, vScriptState->vRuleVariables, "$EL "); break;
	case GrapaTokenType::TAG: pSend->Send(this, vScriptState->vRuleVariables, "$TAG "); break;
	case GrapaTokenType::OP: pSend->Send(this, vScriptState->vRuleVariables, "$OP "); break;
	case GrapaTokenType::CODE: pSend->Send(this, vScriptState->vRuleVariables, "$CODE "); break;
	case GrapaTokenType::TABLE:   pSend->Send(this, vScriptState->vRuleVariables, "$DB"); break;
	case GrapaTokenType::ID:  pSend->Send(this, vScriptState->vRuleVariables, "$ID"); break;
	case GrapaTokenType::REF:  pSend->Send(this, vScriptState->vRuleVariables, "$REF"); break;
	case GrapaTokenType::SYM: pSend->Send(this, vScriptState->vRuleVariables, "$SYM"); break;
	case GrapaTokenType::SYSSYM: pSend->Send(this, vScriptState->vRuleVariables, "$SYSSYM"); break;
	case GrapaTokenType::SYSINT: pSend->Send(this, vScriptState->vRuleVariables, "$SYSINT"); break;
	case GrapaTokenType::SYSID: pSend->Send(this, vScriptState->vRuleVariables, "$SYSID"); break;
	case GrapaTokenType::SYSSTR: pSend->Send(this, vScriptState->vRuleVariables, "$SYSSTR"); break;
	case GrapaTokenType::OTHER: pSend->Send(this, vScriptState->vRuleVariables, "$OTHER"); break;
	case GrapaTokenType::RULE: pSend->Send(this, vScriptState->vRuleVariables, "$RULE"); break;
	case GrapaTokenType::PTR: pSend->Send(this, vScriptState->vRuleVariables, "$PTR"); break;
	default: pSend->Send(this, vScriptState->vRuleVariables, "????"); break;
	}
	pSend->Send(this, vScriptState->vRuleVariables, "(\"");
	GrapaCHAR to;
	EscapeString(pEvent->mValue, to);
	pSend->Send(this, vScriptState->vRuleVariables, to);
	pSend->Send(this, vScriptState->vRuleVariables, "\"");
	to.SetLength(0);

	if (pEvent->mSkip) to.Append("$SKIP,");
	if (pEvent->mRun) to.Append("$RUN,");
	if (pEvent->mStart) to.Append("$START,");
	if (pEvent->mEnd) to.Append("$END,");
	if (pEvent->mEscape) to.Append("$ESC,");
	if (pEvent->mExit) to.Append("$EXIT,");
	if (pEvent->mNull) to.Append("$EMPTY,");
	if (to.mLength) to.SetLength(to.mLength - 1);
	if (to.mLength)
	{
		pSend->Send(this, vScriptState->vRuleVariables, ",{");
		pSend->Send(this, vScriptState->vRuleVariables, to);
		pSend->Send(this, vScriptState->vRuleVariables, "}");
	}

	pSend->Send(this, vScriptState->vRuleVariables, ")");

	if (pretty)
		pSend->Send(this, vScriptState->vRuleVariables, ";\r\n");
}

void GrapaScriptExec::EchoClassValue(GrapaSystemSend* pSend, GrapaRuleEvent* pEvent, bool elMode, bool pretty)
{
	if (pretty && pEvent->mName.mLength)
	{
		pSend->Send(this, vScriptState->vRuleVariables, "@global[\"");
		pSend->Send(this, vScriptState->vRuleVariables, pEvent->mName);
		pSend->Send(this, vScriptState->vRuleVariables, "\"]\r\n\t= class");
	}
	else
	{
		pSend->Send(this, vScriptState->vRuleVariables, "class");
	}
	if (pEvent->vQueue && pEvent->vQueue->mCount)
	{
		pSend->Send(this, vScriptState->vRuleVariables, " ");
		if (pEvent->vQueue->mCount > 1 || (pEvent->vQueue->mCount == 1 && ((GrapaRuleEvent*)pEvent->vQueue->Head())->vClass))
		{
			pSend->Send(this, vScriptState->vRuleVariables, "(");
			GrapaRuleEvent* classes = (GrapaRuleEvent*)pEvent->vQueue->Head();
			bool isFirst = true;
			while (classes)
			{
				if (classes->vClass)
				{
					if (!isFirst)
						pSend->Send(this, vScriptState->vRuleVariables, ",");
					pSend->Send(this, vScriptState->vRuleVariables, classes->vClass->mName);
					isFirst = false;
				}
				classes = classes->Next();
			}
			pSend->Send(this, vScriptState->vRuleVariables, ") ");
		}
		pSend->Send(this, vScriptState->vRuleVariables, "{");
		if (pretty)
			pSend->Send(this, vScriptState->vRuleVariables, "\r\n");
		GrapaRuleEvent* params = (GrapaRuleEvent*)pEvent->vQueue->Head();
		if (params && params->vQueue)
		{
			params = (GrapaRuleEvent*)params->vQueue->Head();
			if (params)
			{
				while (params)
				{
					if (pretty)
						pSend->Send(this, vScriptState->vRuleVariables, "\t");
					pSend->Send(this, vScriptState->vRuleVariables, params->mName);
					pSend->Send(this, vScriptState->vRuleVariables, " = ");
					GrapaSystemSend send;
					send.isActive = false;
					if (params->vQueue) EchoList(&send, params, false, false, false);
					else EchoValue(&send, params, false, true, false);
					GrapaCHAR s;
					send.GetStr(s);
					pSend->Send(this, vScriptState->vRuleVariables, s);
					params = params->Next();
					pSend->Send(this, vScriptState->vRuleVariables, "; ");
					if (pretty)
						pSend->Send(this, vScriptState->vRuleVariables, "\r\n");
				}
			}
		}
		if (pretty)
			pSend->Send(this, vScriptState->vRuleVariables, "\t");
		pSend->Send(this, vScriptState->vRuleVariables, "}");
	}
	pSend->Send(this, vScriptState->vRuleVariables, ";");
	if (pretty)
		pSend->Send(this, vScriptState->vRuleVariables, "\r\n");
}

void GrapaScriptExec::EchoRuleValue(GrapaSystemSend* pSend, GrapaRuleEvent* pEvent, bool elMode, bool pretty, bool noHeader)
{
	GrapaRuleEvent* items = (GrapaRuleEvent*)pEvent->vQueue->Head();
	if (!noHeader)
	{
		if (pretty && pEvent->mName.mLength)
		{
			pSend->Send(this, vScriptState->vRuleVariables, "@global[\"");
			pSend->Send(this, vScriptState->vRuleVariables, pEvent->mName);
			pSend->Send(this, vScriptState->vRuleVariables, "\"]");
			if (items)
				pSend->Send(this, vScriptState->vRuleVariables, "\r\n\t= rule ");
		}
		else if (!noHeader)
		{
			pSend->Send(this, vScriptState->vRuleVariables, "rule ");
		}
	}
	else if (pretty)
		pSend->Send(this, vScriptState->vRuleVariables, "\t");
	while (items)
	{
		GrapaRuleEvent* params = items->vQueue ? (GrapaRuleEvent*)items->vQueue->Head() : NULL;
		if (params)
		{
			while (params)
			{
				if (params->mId)
				{
					if (params->mValue.mToken == GrapaTokenType::RULEREF)
					{
						//result->mValue.GrapaCHAR::Append("@");
						//result->mValue.GrapaCHAR::Append(params->mName);
						pSend->Send(this, vScriptState->vRuleVariables, "<");
						pSend->Send(this, vScriptState->vRuleVariables, params->mName);
						if (params->vQueue && params->vQueue->mCount)
						{
							GrapaSystemSend send;
							send.isActive = false;
							EchoList(&send, (GrapaRuleEvent*)params->vQueue->Head(), false, false, false);
							GrapaCHAR s;
							send.GetStr(s);
							pSend->Send(this, vScriptState->vRuleVariables, ",");
							pSend->Send(this, vScriptState->vRuleVariables, s);
						}
						pSend->Send(this, vScriptState->vRuleVariables, ">");

					}
					else
					{
						pSend->Send(this, vScriptState->vRuleVariables, params->mName);
					}
					//if (params->mValue.mToken == GrapaTokenType::RULEREF) result->mValue.GrapaCHAR::Append(">");
				}
				else
				{
					if (params->mValue.mToken == GrapaTokenType::ID && params->mValue.mLength)
					{
						pSend->Send(this, vScriptState->vRuleVariables, params->mValue);
					}
					else if (params->mValue.mToken == GrapaTokenType::SYSID && params->mValue.mLength)
					{
						pSend->Send(this, vScriptState->vRuleVariables, "$SYSID(\"");
						pSend->Send(this, vScriptState->vRuleVariables, params->mValue);
						pSend->Send(this, vScriptState->vRuleVariables, "\")");
					}
					else if (params->mValue.mToken == GrapaTokenType::REF && params->mValue.mLength)
					{
						pSend->Send(this, vScriptState->vRuleVariables, "@");
						pSend->Send(this, vScriptState->vRuleVariables, params->mValue);
					}
					else if (params->mValue.mToken == GrapaTokenType::SYM && params->mValue.mLength)
					{
						pSend->Send(this, vScriptState->vRuleVariables, "'");
						pSend->Send(this, vScriptState->vRuleVariables, params->mValue);
						pSend->Send(this, vScriptState->vRuleVariables, "'");
					}
					else if (params->mValue.mToken == GrapaTokenType::INT && params->mValue.mLength)
					{
						GrapaInt i;
						i.FromBytes(params->mValue);
						pSend->Send(this, vScriptState->vRuleVariables, i.ToString());
					}
					else if (params->mValue.mToken == GrapaTokenType::OP)
					{
						GrapaSystemSend send;
						send.isActive = false;
						EchoList(&send, params, false, false, false);
						GrapaCHAR s;
						send.GetStr(s);
						pSend->Send(this, vScriptState->vRuleVariables, "<");
						pSend->Send(this, vScriptState->vRuleVariables, s);
						pSend->Send(this, vScriptState->vRuleVariables, ">");
					}
					else if (params->mValue.mToken == GrapaTokenType::RULE)
					{
						GrapaSystemSend send;
						send.isActive = false;
						EchoRuleValue(&send, params, elMode, false, true);
						GrapaCHAR s;
						send.GetStr(s);
						pSend->Send(this, vScriptState->vRuleVariables, "(");
						pSend->Send(this, vScriptState->vRuleVariables, s);
						pSend->Send(this, vScriptState->vRuleVariables, ")");
					}
					else if (params->mValue.mToken == GrapaTokenType::FLOAT && params->mValue.mLength)
					{
						GrapaFloat i(0);
						i.FromBytes(params->mValue);
						pSend->Send(this, vScriptState->vRuleVariables, i.ToString());
					}
					else
					{
						switch (params->mValue.mToken)
						{
						case GrapaTokenType::ERR:   pSend->Send(this, vScriptState->vRuleVariables, "$ERR"); break;
						case GrapaTokenType::RAW:   pSend->Send(this, vScriptState->vRuleVariables, "$RAW"); break;
						case GrapaTokenType::BOOL:  pSend->Send(this, vScriptState->vRuleVariables, "$BOOL"); break;
						case GrapaTokenType::ID:  pSend->Send(this, vScriptState->vRuleVariables, "$ID"); break;
						case GrapaTokenType::INT: pSend->Send(this, vScriptState->vRuleVariables, "$INT"); break;
						case GrapaTokenType::FLOAT: pSend->Send(this, vScriptState->vRuleVariables, "$FLOAT"); break;
						case GrapaTokenType::STR:   pSend->Send(this, vScriptState->vRuleVariables, "$STR"); break;
						case GrapaTokenType::TIME:  pSend->Send(this, vScriptState->vRuleVariables, "$TIME"); break;
						case GrapaTokenType::ARRAY: pSend->Send(this, vScriptState->vRuleVariables, "$ARRAY"); break;
						case GrapaTokenType::TUPLE: pSend->Send(this, vScriptState->vRuleVariables, "$TUPLE"); break;
						case GrapaTokenType::VECTOR: pSend->Send(this, vScriptState->vRuleVariables, "$VECTOR"); break;
						case GrapaTokenType::WIDGET: pSend->Send(this, vScriptState->vRuleVariables, "$WIDGET"); break;
						case GrapaTokenType::LIST: pSend->Send(this, vScriptState->vRuleVariables, "$LIST"); break;
						case GrapaTokenType::OBJ: pSend->Send(this, vScriptState->vRuleVariables, "$OBJ"); break;
						case GrapaTokenType::XML: pSend->Send(this, vScriptState->vRuleVariables, "$XML"); break;
						case GrapaTokenType::EL: pSend->Send(this, vScriptState->vRuleVariables, "$EL"); break;
						case GrapaTokenType::TAG: pSend->Send(this, vScriptState->vRuleVariables, "$TAG"); break;
						case GrapaTokenType::OP: pSend->Send(this, vScriptState->vRuleVariables, "$OP"); break;
						case GrapaTokenType::CODE: pSend->Send(this, vScriptState->vRuleVariables, "$CODE"); break;
						case GrapaTokenType::TABLE:   pSend->Send(this, vScriptState->vRuleVariables, "$DB"); break;
						case GrapaTokenType::SYM: pSend->Send(this, vScriptState->vRuleVariables, "$SYM"); break;
						case GrapaTokenType::SYSSYM: pSend->Send(this, vScriptState->vRuleVariables, "$SYSSYM"); break;
						case GrapaTokenType::SYSINT: pSend->Send(this, vScriptState->vRuleVariables, "$SYSINT"); break;
						case GrapaTokenType::SYSID: pSend->Send(this, vScriptState->vRuleVariables, "$SYSID"); break;
						case GrapaTokenType::SYSSTR: pSend->Send(this, vScriptState->vRuleVariables, "$SYSSTR"); break;
						case GrapaTokenType::OTHER: pSend->Send(this, vScriptState->vRuleVariables, "$OTHER"); break;
						case GrapaTokenType::RULE: pSend->Send(this, vScriptState->vRuleVariables, "$RULE"); break;
						case GrapaTokenType::CLASS: pSend->Send(this, vScriptState->vRuleVariables, "$CLASS"); break;
						case GrapaTokenType::PTR: pSend->Send(this, vScriptState->vRuleVariables, "$PTR"); break;
						case GrapaTokenType::REF: pSend->Send(this, vScriptState->vRuleVariables, "$REF"); break;
						default: pSend->Send(this, vScriptState->vRuleVariables, "????"); break;
						}
						if (params->mValue.mLength)
						{
							pSend->Send(this, vScriptState->vRuleVariables, "(\"");
							pSend->Send(this, vScriptState->vRuleVariables, params->mValue);
							pSend->Send(this, vScriptState->vRuleVariables, "\")");
						}
						//break;
					}
				}
				params = params->Next();
				if (params)
					pSend->Send(this, vScriptState->vRuleVariables, " ");
			}
		}
		if (items->vRuleLambda)
		{
			GrapaSystemSend send;
			send.isActive = false;
			EchoList(&send, items->vRuleLambda, false, false, false);
			GrapaCHAR s;
			send.GetStr(s);
			pSend->Send(this, vScriptState->vRuleVariables, " {");
			pSend->Send(this, vScriptState->vRuleVariables, s);
			pSend->Send(this, vScriptState->vRuleVariables, "}");
		}
		else if (items->mName.mLength)
		{
			pSend->Send(this, vScriptState->vRuleVariables, " {");
			pSend->Send(this, vScriptState->vRuleVariables, items->mName);
			pSend->Send(this, vScriptState->vRuleVariables, ".");
			pSend->Send(this, vScriptState->vRuleVariables, items->mValue);
			pSend->Send(this, vScriptState->vRuleVariables, "}");
		}
		if (pretty)
			pSend->Send(this, vScriptState->vRuleVariables, "\r\n");
		items = items->Next();
		if (items)
		{
			if (pretty)
				pSend->Send(this, vScriptState->vRuleVariables, "\t");
			//else 
			//	pSend->Send(" ");
			pSend->Send(this, vScriptState->vRuleVariables, "| ");
		}
	}
	if (pretty)
		pSend->Send(this, vScriptState->vRuleVariables, "\t;\r\n");
}

void GrapaScriptExec::EchoList(GrapaSystemSend* pSend, GrapaRuleEvent* pEvent, bool elMode, bool arrayInEl, bool fromEcho)
{
	u8 pToken = pEvent?pEvent->mValue.mToken:0;
	GrapaRuleEvent* pTokenEvent = (GrapaRuleEvent*) ((pEvent && pEvent->vQueue) ? pEvent->vQueue->Head() : NULL);
	bool oldElMode = elMode;
	bool hasName = false;

	if (pEvent && pEvent->mName.mLength)
		hasName = true;

	bool hasElContent = false;
	bool hasElArray = false;

	switch (pToken)
	{
	case GrapaTokenType::ARRAY:
		if (!arrayInEl)
			pSend->Send(this, vScriptState->vRuleVariables, "[");
		break;
	case GrapaTokenType::TUPLE:
		if (!arrayInEl)
			pSend->Send(this, vScriptState->vRuleVariables, "(");
		break;
	case GrapaTokenType::ERR:
	case GrapaTokenType::LIST:
	case GrapaTokenType::OBJ:
	case GrapaTokenType::RULEOP:
		pSend->Send(this, vScriptState->vRuleVariables, "{");
		break;
	case GrapaTokenType::OP:
		pSend->Send(this, vScriptState->vRuleVariables, "@<");
		break;
	case GrapaTokenType::CODE:
		pSend->Send(this, vScriptState->vRuleVariables, "@[");
		break;
	case GrapaTokenType::TAG:
		elMode = true;
		//if (!oldElMode)
		//	pSend->Send("$&");
		break;
	case GrapaTokenType::RULE:
		EchoRuleValue(pSend, pEvent, elMode, false,false);
		return;
	case GrapaTokenType::CLASS:
		EchoClassValue(pSend, pEvent, elMode, false);
		return;
	case GrapaTokenType::TOKEN:
		EchoTokenValue(pSend, pEvent, elMode, false);
		return;
	case GrapaTokenType::XML:
		elMode = true;
		hasElArray = true;
		if (!oldElMode && (!pEvent->vQueue || pEvent->vQueue->mCount == 0))
			pSend->Send(this, vScriptState->vRuleVariables, "<");
		break;
	case GrapaTokenType::EL:
		elMode = true;
		hasElArray = true;
		//if (!oldElMode)
		//	pSend->Send("$&");
		if (pEvent)
		{
			if (hasName)
			{
				pSend->Send(this, vScriptState->vRuleVariables, "<");
				pSend->Send(this, vScriptState->vRuleVariables, pEvent->mName);
				//if (pEvent->mName.Cmp("!DOCTYPE") == 0)
				//	pTokenEvent = pEvent;
			}
			else
			{
				pSend->Send(this, vScriptState->vRuleVariables, "<");
			}
			bool isFirst = true;
			GrapaRuleEvent *e3 = (GrapaRuleEvent*)((pTokenEvent && pTokenEvent->vQueue) ? pTokenEvent->vQueue->Head() : NULL);
			while (e3)
			{
				if (hasName && !(pEvent->mName.Cmp("!--") == 0 || pEvent->mName.Cmp("![CDATA[") == 0))
				{
					if (hasName || !isFirst)
						pSend->Send(this, vScriptState->vRuleVariables, " ");
				}
				isFirst = false;
				if (e3->mName.mLength)
				{
					pSend->Send(this, vScriptState->vRuleVariables, e3->mName);
					pSend->Send(this, vScriptState->vRuleVariables, "=");
				}
				switch (e3->mValue.mToken)
				{
				case GrapaTokenType::ERR:
				case GrapaTokenType::ARRAY:
				case GrapaTokenType::TUPLE:
				case GrapaTokenType::LIST:
				case GrapaTokenType::CLASS:
				case GrapaTokenType::OBJ:
				case GrapaTokenType::RULEOP:
				case GrapaTokenType::XML:
				case GrapaTokenType::EL:
				case GrapaTokenType::TAG:
				case GrapaTokenType::OP:
				case GrapaTokenType::CODE:
					if (e3->vQueue)
						EchoList(pSend, e3, elMode,false,true);
					else
						pSend->Send(this, vScriptState->vRuleVariables, "\"\"");
					break;
				case GrapaTokenType::STR:
				case GrapaTokenType::SYSSTR:
					EchoValue(pSend, e3, false, (hasName && (pEvent->mName.Cmp("!--") == 0 || pEvent->mName.Cmp("![CDATA[") == 0)) ?false:true, false);
					break;
				default:
					EchoValue(pSend, e3, elMode, true, false);
					break;
				}
				e3 = e3->Next();
			}
		}
		if (pTokenEvent)
			pTokenEvent = pTokenEvent->Next();
		if (pEvent)
		{
			char cmpfirstchar = ' ';
			if (hasName)
				cmpfirstchar = pEvent->mName.mBytes[0];
			switch (cmpfirstchar)
			{
			case '?':
				pSend->Send(this, vScriptState->vRuleVariables, "?");
				if (hasName)
					pSend->Send(this, vScriptState->vRuleVariables, ">");
				else
					pSend->Send(this, vScriptState->vRuleVariables, ">");
				break;
			case '!':
				//if (pTokenEvent)
				hasElContent = true;
				break;
			default:
				if (pTokenEvent)
				{
					if (hasName)
						pSend->Send(this, vScriptState->vRuleVariables, ">");
					else
						pSend->Send(this, vScriptState->vRuleVariables, ">");
					hasElContent = true;
				}
				else
				{
					pSend->Send(this, vScriptState->vRuleVariables, "/");
					if (hasName)
						pSend->Send(this, vScriptState->vRuleVariables, ">");
					else
						pSend->Send(this, vScriptState->vRuleVariables, ">");
				}
				break;
			}
		}
		break;
	}

	while (pTokenEvent)
	{
		switch (pToken)
		{
		case GrapaTokenType::ERR:
		case GrapaTokenType::LIST:
		case GrapaTokenType::OBJ:
		case GrapaTokenType::RULEOP:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
			if (pTokenEvent->mValue.mToken == GrapaTokenType::EL)
				break;
			if (pTokenEvent->mName.mLength)
			{
				GrapaCHAR s;
				s.AppendQuoted(pTokenEvent->mName, false);
				pSend->Send(this, vScriptState->vRuleVariables, s);
			}
			//if (pTokenEvent->mId)
			//{
			//	GrapaInt n = pTokenEvent->mId;
			//	pSend->Send("(");
			//	pSend->Send(n.ToString(10));
			//	pSend->Send(")");
			//}
			if (pTokenEvent->mName.mLength || pTokenEvent->mId)
				pSend->Send(this, vScriptState->vRuleVariables, ":");
			break;
		}
		GrapaRuleEvent* p = pTokenEvent;
		while (p->mValue.mToken == GrapaTokenType::PTR && p->vRulePointer) p = p->vRulePointer;
		switch (p->mValue.mToken)
		{
		case GrapaTokenType::ERR:
		case GrapaTokenType::LIST:
		case GrapaTokenType::ARRAY:
		case GrapaTokenType::TUPLE:
		case GrapaTokenType::OBJ:
		case GrapaTokenType::RULEOP:
		case GrapaTokenType::XML:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::RULE:
			if (p->vQueue)
				EchoList(pSend, p, elMode, hasElArray,true);
			else
				pSend->Send(this, vScriptState->vRuleVariables, "null");
			break;
		case GrapaTokenType::RULEREF:
			pSend->Send(this, vScriptState->vRuleVariables, "[$\"");
			pSend->Send(this, vScriptState->vRuleVariables, p->mValue);
			pSend->Send(this, vScriptState->vRuleVariables, "\"]");
			break;
		case GrapaTokenType::MAX:
			pSend->Send(this, vScriptState->vRuleVariables, p->mValue);
			break;
		default:
			EchoValue(pSend, p, elMode, true, true);
			break;
		}
		if (pTokenEvent->Next()) 
		{ 
			if (elMode)
			{
				//if (pTokenEvent->Prev())
				//	pSend->Send(" ");
			}
			else
				pSend->Send(this, vScriptState->vRuleVariables, ","); }
		pTokenEvent = pTokenEvent->Next();
	}

	switch (pToken)
	{
	case GrapaTokenType::ARRAY:
		if (!arrayInEl)
			pSend->Send(this, vScriptState->vRuleVariables, "]");
		break;
	case GrapaTokenType::TUPLE:
		if (!arrayInEl)
			pSend->Send(this, vScriptState->vRuleVariables, ")");
		break;
	case GrapaTokenType::CLASS:
		pSend->Send(this, vScriptState->vRuleVariables, "]");
		break;
	case GrapaTokenType::ERR:
	case GrapaTokenType::LIST:
	case GrapaTokenType::OBJ:
	case GrapaTokenType::RULEOP:
		pSend->Send(this, vScriptState->vRuleVariables, "}");
		break;
	case GrapaTokenType::OP:
		pSend->Send(this, vScriptState->vRuleVariables, ">");
		break;
	case GrapaTokenType::CODE:
		pSend->Send(this, vScriptState->vRuleVariables, "]");
		break;
	case GrapaTokenType::XML:
		if (!oldElMode && (!pEvent->vQueue || pEvent->vQueue->mCount == 0))
			pSend->Send(this, vScriptState->vRuleVariables, ">");
		break;
	case GrapaTokenType::TAG:
		//if (!oldElMode)
		//	pSend->Send("$&");
		break;
	case GrapaTokenType::EL:
		if (!hasElContent)
			break;
		{
			char cmpfirstchar = ' ';
			if (hasName)
				cmpfirstchar = pEvent->mName.mBytes[0];
			switch (cmpfirstchar)
			{
			case '?':
				pSend->Send(this, vScriptState->vRuleVariables, "?>");
				break;
			case '!':
				if (pEvent->mName.Cmp("!--")==0)
					pSend->Send(this, vScriptState->vRuleVariables, "-->");
				else if (pEvent->mName.Cmp("![CDATA[") == 0)
					pSend->Send(this, vScriptState->vRuleVariables, "]]>");
				else if (pEvent->mName.Cmp("!DOCTYPE") == 0)
				{
					pSend->Send(this, vScriptState->vRuleVariables, ">");
				}
				else
				{
					pSend->Send(this, vScriptState->vRuleVariables, ">");
				}
				break;
			default:
				if (hasName)
				{
					pSend->Send(this, vScriptState->vRuleVariables, "</");
					pSend->Send(this, vScriptState->vRuleVariables, pEvent->mName);
					pSend->Send(this, vScriptState->vRuleVariables, ">");
				}
				else
				{
					pSend->Send(this, vScriptState->vRuleVariables, "(/)");
				}
				break;
			}
		}
		break;
	}
	// not clear if the following ever happens...
	if (!oldElMode && fromEcho && pToken==GrapaTokenType::XML && pEvent->vQueue && pEvent->vQueue->mCount)
		pSend->Send(this, vScriptState->vRuleVariables, "<>");
}

bool isActive = true;

GrapaRuleEvent* GrapaScriptExec::Exec(GrapaNames* pNameSpace, GrapaRuleEvent* rulexx, u64 pRuleId, GrapaRuleEvent* token, GrapaRuleEvent** resultPtr)
{
	if (resultPtr) *resultPtr = NULL;
	GrapaRuleQueue* operationQueue = new GrapaRuleQueue();
	GrapaRuleEvent* nextToken = token;
	GrapaRuleQueue iq;
	bool pMatched = false;
	bool addLastNext = false;
	if (rulexx == NULL)
	{
		rulexx = vScriptState->SearchVariable(pNameSpace, GrapaCHAR("custom_start"));
		if (rulexx == NULL) rulexx = vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$start"));
		if (rulexx == NULL)
		{
			if (pRuleId == 0) pRuleId = vScriptState->mRuleStartQueue.SearchId("custom_start");
			if (pRuleId == 0) pRuleId = vScriptState->mRuleStartQueue.SearchId("$start");
			rulexx = vScriptState->mRuleStartQueue.Search(pRuleId);
		}
	}
	GrapaKeyValue ruleCache;
	if (rulexx)
		nextToken = PlanRule(pNameSpace, token, rulexx, operationQueue, pMatched, NULL, addLastNext, 0, ruleCache, true);
	else
		nextToken = token;
	if (nextToken == token && nextToken->mRun)
		nextToken = nextToken->Next();
	if (nextToken == token)
	{
		nextToken = NULL;
		if (resultPtr)
		{
			GrapaCHAR name, value;
			*resultPtr = new GrapaRuleEvent(GrapaTokenType::ERR, 0, "", "");
			(*resultPtr)->vQueue = new GrapaRuleQueue();
			value.mToken = GrapaTokenType::STR;
			//name.FROM("tokens");
			//Echo(pNameSpace->GetResponse(), token, value);
			//(*resultPtr)->vQueue->PushTail(new GrapaRuleEvent(0, name, value));
			name.FROM("match");
			value.SetLength(0);
			while (token && token->mTouch)
			{
				switch (token->mValue.mToken)
				{
				case GrapaTokenType::INT:
					value.Append(GrapaInt(token->mValue).ToString());
					break;
				case GrapaTokenType::FLOAT:
					value.Append(GrapaFloat(token->mValue).ToString());
					break;
				default:
					if (!token->mRun)
						value.Append(token->mValue);
				}
				token = token->Next();
			}
			(*resultPtr)->vQueue->PushTail(new GrapaRuleEvent(0, name, value));
			name.FROM("fail");
			value.SetLength(0);
			while (token)
			{
				switch (token->mValue.mToken)
				{
				case GrapaTokenType::INT:
					value.Append(GrapaInt(token->mValue).ToString());
					break;
				case GrapaTokenType::FLOAT:
					value.Append(GrapaFloat(token->mValue).ToString());
					break;
				default:
					if (!token->mRun)
						value.Append(token->mValue);
				}
				token = token->Next();
			}
			(*resultPtr)->vQueue->PushTail(new GrapaRuleEvent(0, name, value));
			(*resultPtr)->mAbort = true;
		}
	}
	else if (isActive && resultPtr)
	{
		GrapaRuleEvent *v = PlanProcess(pNameSpace, operationQueue);
		*resultPtr = ProcessPlan(pNameSpace, v);
		delete v;
	}
	operationQueue->CLEAR();
	delete operationQueue;
	return(nextToken);
}

GrapaRuleEvent* GrapaScriptExec::Plan(GrapaNames* pNameSpace, GrapaRuleEvent* pOperation)
{
	if (pOperation == NULL || pOperation->vQueue == NULL) return(NULL);
	GrapaRuleEvent* scanOps = pOperation;
	GrapaRuleEvent* result2 = NULL;
	GrapaRuleEvent* result3 = NULL;
	GrapaRuleEvent* result4 = NULL;
	if (scanOps->mName.mLength == 0)
	{
		if (scanOps->vQueue->mCount)
		{
			result2 = new GrapaRuleEvent(GrapaTokenType::CODE, 0, "", "");
			result2->vQueue = new GrapaRuleQueue();
			GrapaRuleEvent* scanOps2 = ((GrapaRuleQueue*)scanOps->vQueue)->Head();
			while (scanOps2)
			{
				GrapaRuleEvent* result3 = NULL;
				GrapaRuleEvent* scanOps3 = scanOps2;
				while (scanOps3->vQueue && !scanOps3->mName.mLength && scanOps3->vQueue->mCount == 1)
					scanOps3 = ((GrapaRuleQueue*)scanOps3->vQueue)->Head();
				if (scanOps3->vQueue)
				{
					if (scanOps3->mName.mLength == 0)
					{
						while (scanOps3->vQueue->mCount)
							scanOps->vQueue->PushNext(scanOps2, ((GrapaRuleQueue*)scanOps3->vQueue)->PopTail());
					}
					else
					{
						result3 = Plan(pNameSpace,scanOps3);
						if (result3) result2->vQueue->PushTail(result3);
					}
				}
				scanOps2 = scanOps2->Next();
			}
		}
	}
	else
	{
		if (scanOps->vRuleParent && scanOps->vRuleParent->vRuleLambda)
		{
			result2 = CopyItem(scanOps->vRuleParent->vRuleLambda);
			if (result2->vQueue)
			{
				result3 = (GrapaRuleEvent*)result2->vQueue->Head();
				if (result3 && result3->mValue.mToken == GrapaTokenType::SYSID)
				{
					GrapaCHAR libName;
					libName.FROM("$");
					libName.Append(result3->mValue);
					result3->mValue.FROM(libName);
					result3->mValue.mToken = GrapaTokenType::STR;
				}
				GrapaRuleEvent* oldParams = (GrapaRuleEvent*)result2->vQueue->PopTail();
				result3 = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "", "");
				result3->vQueue = new GrapaRuleQueue();
				result2->vQueue->PushTail(result3);
				if (oldParams && oldParams->vQueue && oldParams->vQueue->mCount)
				{
					GrapaRuleEvent *param = (GrapaRuleEvent*)oldParams->vQueue->Head();
					while (param)
					{
						if (param->mValue.mToken == GrapaTokenType::SYSINT)
						{
							GrapaInt a;
							u64 p;
							a.FromBytes(param->mValue);
							p = a.LongValue();
							GrapaRuleEvent *token = NULL;
							if (p)
							{
								p--;
								token = (GrapaRuleEvent*)(scanOps->vQueue ? scanOps->vQueue->Head() : NULL);
								while (token && p)
								{
									token = token->Next();
									p--;
								}
							}
							else if (!param->mNull)
							{
								// For $0, pass in the result from the last command -> pipe. For NULL, pass in $$.
								// Do we want to null out when entering a scope? Not sure. No...the syntax should resolve this.
								token = NULL; // pLastResult;
							}
							if (p == 0)
							{
								if (token && token->mValue.mToken==GrapaTokenType::RULEOP && token->vQueue)
								{
									result4 = Plan(pNameSpace, token);
									if (result4 == NULL)
									{
										result4 = new GrapaRuleEvent(GrapaTokenType::SYSID, 0, (char*)param->mName.mBytes, "");
										result4->mNull = true;
									}
									else if (param->mName.mLength)
									{
										result4->mName.FROM(param->mName);
									}
								}
								else if (token)
								{
									if (token->vValueEvent)
										result4 = new GrapaRuleEvent(0, param->mName, token->vValueEvent->mValue);
									else
									{
										result4 = CopyItem(token);
										result4->mName.FROM(param->mName);
									}
								}
								else
								{
									result4 = new GrapaRuleEvent(GrapaTokenType::SYSID, 0, "", "");
									result4->mNull = true;
								}
								result3->vQueue->PushTail(result4);
							}
						}
						else
						{
							result3->vQueue->PushTail(CopyItem(param));
						}
						param = param->Next();
					}
				}
				if (oldParams)
				{
					oldParams->CLEAR();
					delete oldParams;
				}
			}
		}
		else
		{
			result2 = new GrapaRuleEvent(GrapaTokenType::OP, 0, "", "");
			result2->vQueue = new GrapaRuleQueue();
		}
		if (result2 && result2->vQueue)
		{
			GrapaRuleEvent* code = (GrapaRuleEvent*)result2->vQueue->Head();
			GrapaRuleEvent* param = code->Next();
			GrapaRuleEvent* ptail = (GrapaRuleEvent*)result2->vQueue->Tail();
			switch (result2->vQueue->mCount)
			{
			case 1:
				code = &gSystem->mLib;
				param = (GrapaRuleEvent*)result2->vQueue->Head();
				break;
			case 2:
				if (ptail && ptail->mValue.mToken == GrapaTokenType::LIST && !ptail->mNull)
				{
					code = &gSystem->mLib;
					param = (GrapaRuleEvent*)result2->vQueue->Head();
				}
				break;
			}
			LoadLib(code);
			if (code->vLibraryEvent) result2 = code->vLibraryEvent->Optimize(this, pNameSpace, result2, param);
		}
	}
	if (result2)
	{
		while (result2->mValue.mToken == GrapaTokenType::CODE && result2->vQueue)
		{
			if (result2->vQueue->mCount == 1)
			{
				GrapaRuleEvent *t = (GrapaRuleEvent*)result2->vQueue->PopHead();
				result2->CLEAR();
				delete result2;
				result2 = t;
			}
			else if (result2->vQueue->mCount == 0)
			{
				result2->vQueue->CLEAR();
				delete result2->vQueue;
				result2->vQueue = NULL;
				result2->mValue.mToken = GrapaTokenType::SYSID;
				result2->mValue.SetLength(0);
				result2->mNull = true;
			}
			else
				break;
		}
	}
	return(result2);
}

GrapaRuleEvent *GrapaScriptExec::Plan(GrapaNames* pNameSpace, GrapaCHAR& pInput, GrapaRuleEvent* rulexx, u64 pRuleId, GrapaCHAR pProfile)
{
	GrapaItemState itemState;
	GrapaQueue itemQueue;
	GrapaRuleQueue tokenQueue;
	itemState.SetQueue(&itemQueue);
	itemState.SetOutput(&tokenQueue);
	itemState.SetParams(&vScriptState->mItemParams, vScriptState->GetNameSpace());
	itemState.mProfile.FROM(pProfile);

	GrapaItemEvent* event = new GrapaItemEvent();
	event->mMessage.FROM(pInput);
	event->mMessage.Append("$\n");
	itemQueue.PushTail(event);

	itemState.StartSync();

	itemState.Stop();

	GrapaRuleEvent* codeResult = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
	codeResult->vQueue = new GrapaRuleQueue();

	enum { START = 0, ESCAPE, BLOCK, };
	u64 state = START;
	u64 returnState = START;

	if (rulexx == NULL)
	{
		rulexx = vScriptState->SearchVariable(pNameSpace, GrapaCHAR("custom_start"));
		if (rulexx == NULL) rulexx = vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$start"));
		if (rulexx == NULL)
		{
			if (pRuleId == 0) pRuleId = vScriptState->mRuleStartQueue.SearchId("custom_start");
			if (pRuleId == 0) pRuleId = vScriptState->mRuleStartQueue.SearchId("$start");
			rulexx = vScriptState->mRuleStartQueue.Search(pRuleId);
		}
	}

	GrapaRuleQueue evalQueue;
	bool stop = false;
	while (!stop)
	{
		GrapaRuleEvent *nextValue = tokenQueue.Head();
		if (!nextValue && evalQueue.mCount == 0 && tokenQueue.mCount == 0)
		{
			stop = true;
			break;
		}
		switch (state)
		{
		case BLOCK:
			if (!nextValue)
			{
				returnState = BLOCK;
				state = START;
				break;
			}
			if (nextValue->mEscape)
			{
				returnState = BLOCK;
				state = ESCAPE;
				GrapaRuleEvent*e = tokenQueue.PopHead();
				e->CLEAR();
				delete e;
				break;
			}
			if (!nextValue->mEnd)
			{
				evalQueue.PushTail(tokenQueue.PopHead());
				break;
			}
			state = START;
		case START:
			if (nextValue && nextValue->mEscape)
			{
				returnState = START;
				state = ESCAPE;
				GrapaRuleEvent*e = tokenQueue.PopHead();
				e->CLEAR();
				delete e;
				break;
			}
			else if (nextValue && nextValue->mStart)
			{
				GrapaRuleEvent*e = tokenQueue.PopHead();
				e->CLEAR();
				delete e;
				state = BLOCK;
				break;
			}
			else if ((tokenQueue.mCount == 0) || (nextValue && nextValue->mEnd))
			{
				//bool isRun = nextValue ? nextValue->mRun : false;
				//bool isEnd = nextValue ? nextValue->mEnd : false;
				if (nextValue)
				{
					GrapaRuleEvent* token = tokenQueue.PopHead();
					token->mSkip = false;
					evalQueue.PushTail(token);
				}
				if (evalQueue.mCount > 1)
				{
					GrapaRuleEvent* token = evalQueue.Head();
					GrapaRuleQueue* operationQueue = new GrapaRuleQueue();
					while (token)
					{
						GrapaRuleEvent* nextToken = token;
						bool matched = false;
						bool addLastNext = false;
						GrapaKeyValue ruleCache;
						if (rulexx)
							nextToken = PlanRule(pNameSpace, token, rulexx, operationQueue, matched, NULL, addLastNext, 0, ruleCache, true);
						else
							nextToken = token;
						if (nextToken == token && nextToken->mRun)
							nextToken = nextToken->Next();
						GrapaRuleEvent* evalResult = NULL;
						if (nextToken == token)
						{
							GrapaCHAR name, value;
							nextToken = NULL;
							evalResult = new GrapaRuleEvent(GrapaTokenType::ERR, 0, "", "");
							evalResult->vQueue = new GrapaRuleQueue();
							value.mToken = GrapaTokenType::STR;
							//name.FROM("tokens");
							//Echo(pNameSpace->GetResponse(), token, value);
							//evalResult->vQueue->PushTail(new GrapaRuleEvent(0, name, value));
							name.FROM("match");
							value.SetLength(0);
							while (token && token->mTouch)
							{
								switch (token->mValue.mToken)
								{
								case GrapaTokenType::INT:
									value.Append(GrapaInt(token->mValue).ToString());
									break;
								case GrapaTokenType::FLOAT:
									value.Append(GrapaFloat(token->mValue).ToString());
									break;
								default:
									if (!token->mRun)
										value.Append(token->mValue);
								}
								token = token->Next();
							}
							evalResult->vQueue->PushTail(new GrapaRuleEvent(0, name, value));
							name.FROM("fail");
							value.SetLength(0);
							while (token)
							{
								switch (token->mValue.mToken)
								{
								case GrapaTokenType::INT:
									value.Append(GrapaInt(token->mValue).ToString());
									break;
								case GrapaTokenType::FLOAT:
									value.Append(GrapaFloat(token->mValue).ToString());
									break;
								default:
									if (!token->mRun)
										value.Append(token->mValue);
								}
								token = token->Next();
							}
							evalResult->vQueue->PushTail(new GrapaRuleEvent(0, name, value));
							evalResult->mAbort = true;
							codeResult->vQueue->PushTail(evalResult);
							codeResult->mAbort = true;
							break;
						}
						else
						{
							evalResult = PlanProcess(pNameSpace, operationQueue);
							if (evalResult && evalResult->mNull)
							{
								evalResult->CLEAR();
								delete evalResult;
								evalResult = NULL;
							}
							if (evalResult)
								codeResult->vQueue->PushTail(evalResult);
						}
						token = nextToken;
						operationQueue->CLEAR();
					}
					operationQueue->CLEAR();
					delete operationQueue;
				}
				evalQueue.CLEAR();
			}
			else if (nextValue)
				evalQueue.PushTail(tokenQueue.PopHead());
			break;
		case ESCAPE:
			if (!nextValue)
			{
				state = returnState;
				break;
			}
			else if (nextValue->mExit)
			{
				GrapaRuleEvent*e = tokenQueue.PopHead();
				e->CLEAR();
				delete e;
				stop = true;
				gSystem->mStop = true;
				state = returnState;
				break;
			}
			else
			{
				bool isSkip = nextValue->mSkip;
				evalQueue.PushTail(tokenQueue.PopHead());
				nextValue = tokenQueue.Head();
				if (!isSkip || nextValue == NULL || !nextValue->mSkip)
					state = returnState;
			}
			break;
		}
	}
	if (codeResult)
	{
		/// need a unique ARRAY type for this....otherwise an data array of 1 item becomes that 1 item.
		// For now, see if only doing this once is sufficient
		if ((codeResult->mValue.mToken == GrapaTokenType::ARRAY || codeResult->mValue.mToken == GrapaTokenType::TUPLE) && codeResult->vQueue)
		{
			if (codeResult->vQueue->mCount == 1)
			{
				GrapaRuleEvent *t = codeResult->vQueue->PopHead();
				codeResult->CLEAR();
				delete codeResult;
				codeResult = t;
			}
			else if (codeResult->vQueue->mCount == 0)
			{
				codeResult->SetNull();
			}
		}
	}
	return(codeResult);
}

/////////////////////////////////////////////////////////////////

GrapaScriptExecState::GrapaScriptExecState() 
{ 
	mItemState.SetQueue(&mItemQueue);
	vScriptExec = NULL;
	//vResult = NULL; 
	mEnablePrompt = false;
	mNeedsPrompt = false;
	mRuleId = 0;
	mClearState = false;
}

GrapaScriptExecState::~GrapaScriptExecState() 
{
	//if (vResult)
	//{
	//	vResult->CLEAR();
	//	delete vResult;
	//	vResult = NULL;
	//}
}

void GrapaScriptExecState::EnablePrompt(GrapaNames* pNameSpace, bool pEnablePrompt)
{
	if (pEnablePrompt) SendPrompt(pNameSpace);
	mEnablePrompt = pEnablePrompt; 
}

void GrapaScriptExecState::UpdateResult(GrapaNames* pNameSpace, GrapaRuleEvent* pResult)
{
	if (pResult)
	{
		if (pNameSpace->vLast)
		{
			pNameSpace->vLast->CLEAR();
			delete pNameSpace->vLast;
		}
		pNameSpace->vLast = pResult;
	}
}

GrapaError GrapaScriptExecState::Start()
{
	WaitCritical();
	LoadSystem();
	LeaveCritical();
	return GrapaState::Start(false);
}

void GrapaScriptExecState::Starting()
{
	if (GetQueue())
		GetQueue()->mState = this;
	mItemState.Start(mSync);
}

void GrapaScriptExecState::Running()
{
	GrapaRuleQueue evalQueue;

	enum { START = 0, ESCAPE, BLOCK, };
	u64 state = START;
	u64 returnState = START;
	
	WaitCritical();

	GrapaRuleEvent* nextValue = NULL;

	SendCondition();

	while (!mStop)
	{
		nextValue = (GrapaRuleEvent*)(GetQueue()?GetQueue()->Head():NULL);
		if (!nextValue)
		{
			if (mSync && mItemState.GetQueue()->mCount == 0 && mQueue->mCount == 0 && evalQueue.mCount == 0)
			{
				mStop = true;
				continue;
			}
			else if ((!mSync || mItemState.GetQueue()->mCount))
			{
				LeaveCritical();
				if (!mSync) mSuspend.WaitCritical();
				if (GetQueue()==NULL || GetQueue()->mCount == 0)
				{
					if (mEnablePrompt && mNeedsPrompt && mItemQueue.mCount == 0)
					{
						SendPrompt(vScriptExec->vScriptState->GetNameSpace());
						mNeedsPrompt = false;
					}
					if (!mSync)
					{
						mSuspend.WaitCondition();
						mSuspend.WaitCritical();
						if (mClearState)
						{
							mClearState = false;
							evalQueue.CLEAR();
							state = returnState = START;
						}
					}
				}
				if (!mSync) mSuspend.LeaveCritical();
				WaitCritical();
				continue;
			}
		}
		if (mStop) break;
		switch (state)
		{
		case BLOCK:
			if (!nextValue)
			{
				returnState = BLOCK;
				state = START;
				break;
			}
			if (nextValue->mEscape)
			{
				returnState = BLOCK;
				state = ESCAPE;
				GrapaRuleEvent*e = (GrapaRuleEvent*)GetQueue()->PopHead();
				e->CLEAR();
				delete e;
				break;
			}
			if (!nextValue->mEnd)
			{
				evalQueue.PushTail(GetQueue()->PopHead());
				break;
			}
			state = START;
		case START:
			if (nextValue && nextValue->mEscape)
			{
				returnState = START;
				state = ESCAPE;
				GrapaRuleEvent*e = (GrapaRuleEvent*)GetQueue()->PopHead();
				e->CLEAR();
				delete e;
				break;
			}
			else if (nextValue && nextValue->mStart)
			{
				//evalQueue.PushTail(GetQueue()->PopHead());
				GrapaRuleEvent*e = (GrapaRuleEvent*)GetQueue()->PopHead();
				e->CLEAR();
				delete e;
				state = BLOCK;
				break;
			}
			else if ((mSync && mItemState.GetQueue()->mCount == 0 && mQueue->mCount == 0) || (!mSync && nextValue && nextValue->mRun) || (nextValue && nextValue->mEnd))
			{
				bool isRun = nextValue ? nextValue->mRun : false;
				bool isEnd = nextValue ? nextValue->mEnd : false;
				if (nextValue)
				{
					GrapaRuleEvent* token = (GrapaRuleEvent*)GetQueue()->PopHead();
					token->mSkip = false;
					evalQueue.PushTail(token);
				}
				if (evalQueue.mCount > 1)
				{
					GrapaRuleEvent* token = evalQueue.Head();
					while (token)
					{
						GrapaRuleEvent* result = NULL;
						if (token->Next() || !token->mRun)
						{
							StartResult(vScriptExec->vScriptState->GetNameSpace());
							token = vScriptExec->Exec(vScriptExec->vScriptState->GetNameSpace(), NULL, mRuleId, token, &result);
							if (result && result->mAbort)
							{
								//delete result;
								//result = NULL;
								//break;
							}
							EndResult(vScriptExec->vScriptState->GetNameSpace(), result);
							if (mSync)
								UpdateResult(vScriptExec->vScriptState->GetNameSpace(), result);
						}
						else
							token = NULL;
					}
				}
				evalQueue.CLEAR();
				if (!mSync && (isRun || isEnd))
					mNeedsPrompt = true;
				if (isEnd)
				{
					mItemState.mLock.WaitCritical();
					mItemState.mLock.SendCondition();
					mItemState.mLock.LeaveCritical();
				}
			}
			else if (nextValue)
				evalQueue.PushTail(GetQueue()->PopHead());
			break;
		case ESCAPE:
			if (!nextValue)
			{
				state = returnState;
				break;
			} 
			else if (nextValue->mExit)
			{
				GrapaEvent *e = GetQueue()->PopHead();
				e->CLEAR();
				delete e;
				mStop = true;
				gSystem->mStop = true;
				state = returnState;
				break;
			}
			else
			{
				bool isSkip = nextValue->mSkip;
				evalQueue.PushTail(GetQueue()->PopHead());
				nextValue = (GrapaRuleEvent*)GetQueue()->Head();
				if (!isSkip || nextValue == NULL || !nextValue->mSkip)
					state = returnState;
			}
			break;
		}
	}
	if (GetQueue())
		GetQueue()->mState = NULL;
	mItemState.Stop();
	CLEAR();
	LeaveCritical();
}

void GrapaScriptExecState::Stopping()
{
}

class GrapaExecSync : public GrapaThread
{
public:
	bool handled = false;
	GrapaRuleEvent* vResult;
	GrapaCHAR* pValue;
	GrapaScriptExec* e;
	GrapaScriptState *vScriptState;
	GrapaNames* pNameSpace;
	GrapaRuleEvent* pRule;
	u64 pRuleId;
	GrapaCHAR* pProfile;
	GrapaExecSync() { handled = false; vResult = NULL; pValue = NULL; e = NULL; vScriptState = NULL; pNameSpace = NULL; pRule = NULL; pRuleId = 0; pProfile = NULL; }
	virtual ~GrapaExecSync() {}
	virtual void Starting() {}
	virtual void Running()
	{
		handled = true;
		vResult = NULL;
		GrapaCHAR* pInput = pValue;
		GrapaCHAR expanded;

		WaitCritical();
		SendCondition();

		if ((*pValue).mLength >= 2)
		{
			if (((*pValue).mBytes[0] == 0 && (*pValue).mBytes[1] == 0) || ((*pValue).mLength > 4 && (*pValue).mBytes[0] == 'G' && (*pValue).mBytes[1] == 'R' && (*pValue).mBytes[2] == 'Z' && ((*pValue).mBytes[3] & 0x80) == 0))
			{
				GrapaCompress::Expand((*pValue), expanded);
				pInput = &expanded;
			}
		}

		if ((*pValue).mToken == GrapaTokenType::STR || (*pValue).mToken == GrapaTokenType::SYSSTR)
		{
			GrapaRuleEvent* plan = e->Plan(pNameSpace, (*pValue), pRule, pRuleId, *pProfile);
			vResult = e->ProcessPlan(pNameSpace, plan);
			if (plan)
			{
				plan->CLEAR();
				delete plan;
			}
			mStop = true;
			LeaveCritical();
			return;
		}

		if (pInput->mToken == GrapaTokenType::ARRAY || pInput->mToken == GrapaTokenType::TUPLE || pInput->mToken == GrapaTokenType::LIST || pInput->mToken == GrapaTokenType::XML || pInput->mToken == GrapaTokenType::EL || pInput->mToken == GrapaTokenType::TAG || pInput->mToken == GrapaTokenType::OP || pInput->mToken == GrapaTokenType::CODE)
		{
			GrapaRuleEvent cmd;
			cmd.vQueue = new GrapaRuleQueue();
			cmd.mValue.mToken = pInput->mToken;

			if (expanded.mToken == GrapaTokenType::LIST || expanded.mToken == GrapaTokenType::ARRAY || expanded.mToken == GrapaTokenType::TUPLE)
			{
				GrapaRuleQueue* tq = new GrapaRuleQueue();
				((GrapaRuleQueue*)tq)->FROM(vScriptState, pNameSpace, *pInput);
				GrapaRuleEvent* tx = NULL;
				s64 idx;
				if (tq->mCount > 1 && expanded.mToken == GrapaTokenType::LIST)
					tx = tq->Search("op", idx);
				if (tx == NULL)
					tx = tq->Head();
				if (tx)
				{
					tq->PopEvent(tx);
					cmd.mValue.mToken = tx->mValue.mToken;
					cmd.vQueue = tx->vQueue;
					tx->vQueue = NULL;
					cmd.vClass = tx->vClass;
					if (cmd.vClass == NULL && tx->mName.mLength)
						cmd.vClass = vScriptState->GetClass(pNameSpace, tx->mName);
					tx->CLEAR();
					delete tx;
				}
				tq->CLEAR();
				delete tq;
			}
			else
			{
				cmd.vClass = ((GrapaRuleQueue*)cmd.vQueue)->FROM(vScriptState, pNameSpace, *pInput);
			}

			cmd.mValue.SetLength(0);
			cmd.mValue.SetSize(0);
			vResult = e->ProcessPlan(pNameSpace, &cmd);
		}
		mStop = true;
		LeaveCritical();
	}
	virtual void Stopping() {}
};

GrapaRuleEvent* GrapaScriptExec::Exec(GrapaNames* pNameSpace, GrapaRuleEvent* pRule, u64 pRuleId, GrapaCHAR pProfile, GrapaCHAR& pValue)
{
	if (pValue.mLength == 0) return(NULL);
	GrapaExecSync ge;
	ge.pValue = &pValue;
	ge.e = this;
	ge.vScriptState = vScriptState;
	ge.pNameSpace = pNameSpace;
	ge.pRule = pRule;
	ge.pRuleId = pRuleId;
	ge.pProfile = &pProfile;
	ge.StartSync();
	if (ge.handled)
		return ge.vResult;
	return ge.vResult;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

