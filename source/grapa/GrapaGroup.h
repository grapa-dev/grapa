// GrapaGroup.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaGroup_
#define _GrapaGroup_

#include "GrapaDB.h"
#include "GrapaFileIO.h"
#include "GrapaFileCache.h"
#include "GrapaFileTree.h"
#include "GrapaFileCache.h"

class GrapaRuleEvent;

class GrapaGroup : public GrapaDB
{
public:
	u64 mRootTable;
	u8 mRootType;

public:
	GrapaGroup();
	GrapaGroup(GrapaFile*pFile);
	virtual ~GrapaGroup();
	bool Opened();
	void INIT(GrapaFile*pFile);

public:
	GrapaError OpenFile(GrapaFile*pFile, char mode);
	GrapaError OpenFile(const GrapaCHAR& fileName, char mode);
	GrapaError CloseFile();

	GrapaError CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, u8 pTableType, u64& pNewTree);
	GrapaError CreateGroup(u64 parentTree, u8 parentType, GrapaCHAR pTableName, GrapaCHAR pTableType, u64& pNewTree);
	GrapaError DeleteGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName);

	//GrapaError CreateAlias(u64 parentTree, u8 parentType, GrapaCHAR& pTableName, u64 pTableId, u64 pLinkTree, GrapaCHAR& pLinkName, u64& pNewTree);
	//GrapaError GrapaDB::CreateAlias(u64 pAliasFirstTree, u64 pAliasTableId, u64 pFirstTree, u64 pTableId, GrapaDBTable& pTable)

	GrapaError OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName, u64& pNewTree, u8& pNewType, u64& pTableId);
	GrapaError OpenGroup(u64 parentTree, u8 parentType, const GrapaCHAR& pTableName, u64& pNewTree, GrapaCHAR& pNewType, u64& pTableId);
	GrapaError OpenGroup(u64 parentTree, u8 parentType, u64 pId, u64& pNewTree, u8& pNewType, u64& pTableId, GrapaCHAR& pName);
	//GrapaError ListGroup(u64 parentTree, u8 parentType, GrapaRuleEvent* pTable);

	GrapaError CreateEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId);
	GrapaError FindEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName, u64& pId);
	GrapaError DeleteEntry(u64 parentTree, u8 parentType, const GrapaCHAR& pDataName);
	GrapaError DeleteEntry(u64 parentTree, u8 parentType, u64 pId);

	GrapaError CreateField(u64 parentTree, u8 parentType, const char* pFieldName, u8 pType = GrapaTokenType::RAW, u8 pStore = GrapaDBField::STORE_VAR, u64 pSize = 32, u64 pGrow = 8);
	GrapaError CreateField(u64 parentTree, u8 parentType, GrapaCHAR& pFieldName, u8 pType = GrapaTokenType::RAW, u8 pStore = GrapaDBField::STORE_VAR, u64 pSize = 32, u64 pGrow = 8);
	GrapaError DeleteField(u64 parentTree, u8 parentType, GrapaCHAR& pField);

	GrapaError SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const char* pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, u64 pId, const char* pField, const GrapaBYTE& pValue);
	GrapaError SetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pField, const GrapaBYTE& pValue);

	GrapaError GetField(u64 parentTree, u8 parentType, const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaBYTE& pValue);
	GrapaError GetField(u64 parentTree, u8 parentType, u64 pId, const GrapaCHAR& pField, GrapaBYTE& pValue);

	GrapaError DumpGroup(u64 parentTree, u8 parentType, u64 pId=0, GrapaFile *pDumpFile=NULL);

	GrapaDBFieldArray* ListFields(u64 parentTree, u8 parentType);
	GrapaError FindField(u64 parentTree, u8 parentType, const GrapaCHAR& pFieldName, GrapaDBField& field, u64& pMaxId);
	GrapaError GetNameId(u64 parentTree, u8 parentType, u64& pNameId);
	GrapaError SetNameId(u64 parentTree, u8 parentType, u64 pNameId);
	GrapaError NextNameId(u64 parentTree, u8 parentType, u64& pNameId);

protected:
	//GrapaFileCache mCache;
	GrapaFileCache mTree;
	GrapaCritical mCritical;
};

class GrapaGroupEvent : public GrapaEvent
{
public:
	GrapaCHAR mName;
	GrapaGroup mValue;
	u64 mInstanceCount;
public:
	GrapaGroupEvent(const GrapaCHAR& pName, GrapaFile* pFile) { mName.FROM(pName); mInstanceCount = 0; mValue.INIT(pFile); }
	virtual ~GrapaGroupEvent() {}
public:
	virtual inline GrapaGroupEvent* Next() { return((GrapaGroupEvent*)mNext); }
	virtual inline GrapaGroupEvent* Prev() { return((GrapaGroupEvent*)mPrev); }
};

class GrapaGroupQueue : public GrapaQueue
{
public:
	//virtual void CLEAR() { GrapaQueue::CLEAR(); }
public:
	GrapaGroupEvent* OpenFile(const GrapaCHAR& fileName, GrapaFile* pFile, char mode);
	GrapaGroupEvent* Create(const GrapaCHAR& fileName, GrapaFile* pFile, u8 pType);
	GrapaGroupEvent* Create(const GrapaCHAR& fileName, GrapaFile* pFile, GrapaCHAR& pType);
	void CloseFile(GrapaGroupEvent* pEvent);
protected:
	virtual GrapaGroupEvent* Search(const GrapaCHAR& pName) { GrapaGroupEvent* item = Head(); while (item) { if (item->mName.StrCmp(pName) == 0) break; item = item->Next(); } return(item); }
	virtual GrapaGroupEvent* Head(u64 mSkip = 0) { GrapaGroupEvent*e = (GrapaGroupEvent*)GrapaQueue::Head(mSkip); return(e); }
	virtual GrapaGroupEvent* Tail(u64 mSkip = 0) {  GrapaGroupEvent*e = (GrapaGroupEvent*)GrapaQueue::Tail(mSkip); return(e); }
	virtual GrapaGroupEvent* PopHead() { GrapaGroupEvent*e = (GrapaGroupEvent*)GrapaQueue::PopHead();return(e); }
	virtual GrapaGroupEvent* PopTail() { GrapaGroupEvent*e = (GrapaGroupEvent*)GrapaQueue::PopTail(); return(e); }
	virtual GrapaGroupEvent* PopEvent(GrapaGroupEvent* pEvent) { GrapaGroupEvent*e = (GrapaGroupEvent*)GrapaQueue::PopEvent((GrapaEvent*)pEvent); return(e); }
protected:
	GrapaCritical mCritical;
};

#endif // _GrapaGroup_

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
