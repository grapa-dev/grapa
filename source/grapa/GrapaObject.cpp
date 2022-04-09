// GrapaObject.cpp 
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

#include "GrapaMem.h"
#include "GrapaObject.h"
#include "GrapaHash.h"
#include "GrapaDatabase.h"
#include "GrapaNetConnect.h"
#include "GrapaState.h"
#include "GrapaVector.h"
#include "GrapaWidget.h"

////////////////////////////////////////////////////////////////////////////////

void GrapaObjectEvent::INIT() 
{ 
	mDirType = 0; mId = 0;  vQueue = NULL; vRulePointer = NULL; vDatabase = NULL; vNetConnect = NULL; vVector = NULL; vWorker = NULL; vWidget = NULL;
	mSkip = mRun = mStart = mEnd = mEscape = mExit = mNull = false;
	mTouch = false;
}

void GrapaObjectEvent::CLEAR()
{
	if (vNetConnect)
	{
		vNetConnect->Disconnect();
	}
	if (vWorker)
	{
		vWorker->Stop();
		delete vWorker;
		vWorker = NULL;
	}
	if (vDatabase)
	{
		vDatabase->CLEAR();
		delete vDatabase;
		vDatabase = NULL;
	}
	if (vVector)
	{
		vVector->CLEAR();
		delete vVector;
		vVector = NULL;
	}
	if (vNetConnect)
	{
		vNetConnect->Disconnect();
		delete vNetConnect;
		vNetConnect = NULL;
	}
	if (vWidget)
	{
		vWidget->CLEAR();
		delete vWidget;
		vWidget = NULL;
	}
	mValue.SetNull();
	mNull = false;
	if (vQueue)
	{
		vQueue->CLEAR();
		delete vQueue;
		vQueue = NULL;
	}
	vRulePointer = NULL;
	GrapaEvent::CLEAR();
}

void GrapaObjectEvent::SetItem(u64 pId, const GrapaCHAR& pName, const GrapaBYTE& pValue)
{
	mId = pId;
	if (pName.mToken == GrapaTokenType::SYSID && (pName.mLength && pName.mBytes[0] != '$'))
	{
		mName.FROM("$");
		mName.Append(pName);
	}
	else
		mName.FROM(pName);
	mValue.FROM(pValue);
	mName.SetSize(mName.GetLength());
	mValue.SetSize(mValue.GetLength());
}

u64 GrapaObjectEvent::HashId(const char* pValue, u64 pLen, u8 pType)
{
	//GrapaHash h;
	//h.Start();
	GrapaBYTE h((u8*)& pType, sizeof(pType));
	//h.Append((u8*)&pType, sizeof(pType));
	if (pValue&&pLen) h.Append((u8*)pValue, pLen);
	GrapaBYTE enc;
	//h.End(enc, sizeof(u64));
	GrapaHash::SHAKE256(h, enc, sizeof(u64));
	return (u64)*(u64*)enc.mBytes;
}

u64 GrapaObjectEvent::HashId(const GrapaBYTE& pValue, u8 pType)
{
	//GrapaHash h;
	//h.Start();
	GrapaBYTE h((u8*)& pType, sizeof(pType));
	//h.Append((u8*)&pType, sizeof(pType));
	if (pValue.mLength) h.Append((u8*)pValue.mBytes, pValue.mLength);
	GrapaBYTE enc;
	//h.End(enc, sizeof(u64));
	GrapaHash::SHAKE256(h, enc, sizeof(u64));
	return (u64)*(u64*)enc.mBytes;
}

u64 GrapaObjectEvent::HashId(u8 pType)
{
	return HashId(mValue, pType);
}

u64 GrapaObjectEvent::HashIdUpper(u8 pType)
{
	GrapaCHAR s(mValue);
	s.ToUpper();
	return HashId((char*)s.mBytes, s.mLength, pType);
}

GrapaObjectEvent* GrapaObjectQueue::Search(const char* pName, s64& pIndex)
{
	GrapaCHAR name(pName);
	return(Search(name, pIndex));
}

GrapaObjectEvent* GrapaObjectQueue::SearchLower(const char* pName, s64& pIndex)
{
	GrapaCHAR name(pName);
	return(SearchLower(name, pIndex));
}

GrapaObjectEvent* GrapaObjectQueue::Search(const GrapaCHAR& pName, s64& pIndex)
{
	return(SearchCase(pName, pIndex, false));
}

GrapaObjectEvent* GrapaObjectQueue::SearchLower(const GrapaCHAR& pName, s64& pIndex)
{
	return(SearchCase(pName, pIndex, true));
}

GrapaObjectEvent* GrapaObjectQueue::SearchCase(const GrapaCHAR& pName, s64& pIndex, bool pLower)
{
	GrapaObjectEvent* item = Head();
	pIndex = 0;
	while (item)
	{
		if (item->mName.mLength && item->mValue.mToken != GrapaTokenType::EL)
		{
			if (pLower && item->mName.StrLowerCmp(pName) == 0)
				return(item);
			else if (!pLower && item->mName.StrCmp(pName) == 0)
				return(item);
		}
		else
		{
			switch (item->mValue.mToken)
			{
			case GrapaTokenType::XML:
				if (item->vQueue && item->vQueue->mCount)
				{
					GrapaObjectEvent* xitem = item->vQueue->Head();
					if (xitem->vQueue && xitem->vQueue->mCount)
					{
						xitem = xitem->vQueue->Head();
						if (pLower && xitem->mName.StrLowerCmp(pName) == 0)
							return(item);
						else if (!pLower && xitem->mName.StrCmp(pName) == 0)
							return(item);
					}
				}
				break;
			case GrapaTokenType::TAG:
				//if (item->mName.StrCmp(pName) == 0)
				//	return(item);
				//if (item->mName.mLength)
				//	break;
				if (item->vQueue && item->vQueue->mCount)
				{
					GrapaObjectEvent* xitem = item->vQueue->Head();
					if (pLower && xitem->mName.StrLowerCmp(pName) == 0)
						return(item);
					else if (!pLower && xitem->mName.StrCmp(pName) == 0)
						return(item);
				}
				break;
			case GrapaTokenType::EL:
				//if (item->mName.mLength == 0)
				//	break;
				if (item->vQueue && item->vQueue->mCount)
				{
					s64 idx;
					GrapaObjectEvent* xitem = NULL;
					if (((GrapaCHAR*)&pName)->StrCmp("attr") == 0) xitem = item->vQueue->Head();
					else if (((GrapaCHAR*)&pName)->StrCmp("$LIST") == 0) xitem = item->vQueue->Head();
					else if (((GrapaCHAR*)&pName)->StrCmp("xml") == 0) xitem = item->vQueue->Head(1);
					else if (((GrapaCHAR*)&pName)->StrCmp("$XML") == 0) xitem = item->vQueue->Head(1);
					else
					{
						GrapaRuleEvent* e = item->vQueue->Head(1);
						if (e && e->vQueue)
						{
							if (pLower)
								xitem = e->vQueue->SearchLower(pName, idx);
							else
								xitem = e->vQueue->Search(pName, idx);
						}
					}
					if (xitem)
						return(xitem);
				}
				break;
			case GrapaTokenType::ARRAY:
			case GrapaTokenType::LIST:
				if (item->vQueue && item->vQueue->mCount)
				{
					GrapaObjectEvent* xitem = item->vQueue->Head();
					if (xitem->mValue.mToken == GrapaTokenType::INT)
					{
						GrapaCHAR searchStr;
						GrapaInt searchInt;
						searchInt.FromBytes(xitem->mValue);
						searchStr = searchInt.ToString();
						if (pLower && searchStr.StrLowerCmp(pName) == 0)
							return(item);
						else if (!pLower && searchStr.StrCmp(pName) == 0)
							return(item);
					}
					else
					{
						if (pLower && xitem->mValue.StrLowerCmp(pName) == 0)
							return(item);
						else if (!pLower && xitem->mValue.StrCmp(pName) == 0)
							return(item);
					}
				}
				break;
			default:
				if (pLower && item->mValue.StrLowerCmp(pName) == 0)
					return(item);
				else if (!pLower && item->mValue.StrCmp(pName) == 0)
					return(item);
				break;
			}
		}
		pIndex++;
		item = item->Next();
	}
	pIndex = -1;
	return(NULL);
}

GrapaObjectEvent* GrapaObjectQueue::Search(u64 pId)
{
	GrapaObjectEvent* item = Head();
	while (item)
	{
		if (pId == item->mId) return(item);
		item = item->Next();
	}
	return(NULL);
}

u64 GrapaObjectQueue::SearchId(const char* pName)
{
	if (pName == 0 || *pName == 0) return(0);
	s64 idx;
	GrapaObjectEvent* item = Search(pName,idx);
	if (item) return(item->mId);
	return(0);
}

u64 GrapaObjectQueue::SearchId(const GrapaCHAR& pName)
{
	return SearchId((char*)pName.mBytes);
}

s64 GrapaObjectQueue::FindIndex(const char* pValue)
{
	s64 index = 0;
	GrapaObjectEvent* item = Head();
	while (item)
	{
		if (item->mValue.StrCmp(pValue) == 0) return(index);
		index++;
		item = item->Next();
	}
	return(-1);
}

s64 GrapaObjectQueue::FindIndex(const GrapaCHAR& pValue)
{
	s64 index = 0;
	GrapaObjectEvent* item = Head();
	while (item)
	{
		if (item->mValue.StrCmp(pValue) == 0) return(index);
		index++;
		item = item->Next();
	}
	return(-1);
}

void GrapaRuleQueue::AppendNames(const char* pNameList, const char* pDelimList)
{
	if (pNameList == NULL || pDelimList == NULL) return;
	size_t len = strlen(pNameList);
	size_t pos;
	char* s = (char*)pNameList;
	while ((pos = strcspn(s, pDelimList)) != len)
	{
		GrapaCHAR name((char*)s, pos);
		GrapaRuleEvent* addName = new GrapaRuleEvent(name);
		PushTail(addName);
		len -= (pos+1);
		s = &s[pos+1];
	}
	if (pos)
	{
		GrapaCHAR name((char*)s, pos);
		PushTail(new GrapaRuleEvent(name));
	}
}

void GrapaRuleQueue::AppendNames(GrapaCHAR& pNameList, GrapaCHAR& pDelimList)
{
	AppendNames((char*)pNameList.mBytes, (char*)pDelimList.mBytes);
}

void GrapaRuleQueue::ListNames(GrapaCHAR& pNameList, const char pDelim)
{
	pNameList.SetLength(0);
	GrapaObjectEvent* item = Head();
	while (item)
	{
		pNameList.Append(item->mName);
		item = item->Next();
		if (item)
			pNameList.Append(pDelim);
	}
}

u64 GrapaObjectQueue::HashId(const char* pValue, u64 pLen, u8 pType)
{
	//GrapaHash h;
	//h.Start();
	GrapaBYTE h((u8*)& pType, sizeof(pType));
	//h.Append((u8*)&pType, sizeof(pType));
	if (pValue&&pLen) h.Append((u8*)pValue, pLen);
	GrapaBYTE enc;
	//h.End(enc, sizeof(u64));
	GrapaHash::SHAKE256(h, enc, sizeof(u64));
	return (u64)*(u64*)enc.mBytes;
}

u64 GrapaObjectQueue::HashId(const GrapaBYTE& pValue, u8 pType)
{
	//GrapaHash h;
	//h.Start();
	GrapaBYTE h((u8*)& pType, sizeof(pType));
	//h.Append((u8*)&pType, sizeof(pType));
	if (pValue.mLength) h.Append((u8*)pValue.mBytes, pValue.mLength);
	GrapaBYTE enc;
	//h.End(enc, sizeof(u64));
	GrapaHash::SHAKE256(h, enc, sizeof(u64));
	return (u64)*(u64*)enc.mBytes;
}

u64 GrapaObjectQueue::HashIdUpper(const GrapaBYTE& pValue, u8 pType)
{
	GrapaCHAR s(pValue);
	s.ToUpper();
	return HashId(s, pType);
}


////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
