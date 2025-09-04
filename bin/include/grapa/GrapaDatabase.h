// GrapaDatabase.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaDatabase_
#define _GrapaDatabase_

#include "GrapaGroup.h"
#include "GrapaFileIO.h"

////////////////////////////////////////////////////////////////////////////////

class GrapaScriptState;
class GrapaRuleEvent;

////////////////////////////////////////////////////////////////////////////////

class GrapaLocalDatabase
{
public:
	GrapaCHAR mHomeDir;
	GrapaScriptState* vScriptState;
	GrapaRuleQueue* mDirectoryPath;
	GrapaRuleQueue* mDatabasePath;
	GrapaRuleQueue* mInclude;
	GrapaCHAR mLocation;
	bool mVar;

public:
	GrapaLocalDatabase();
	GrapaLocalDatabase(GrapaScriptState* pTokenState);
	virtual ~GrapaLocalDatabase();
	virtual void CLEAR();

	virtual GrapaError DirectoryList(GrapaCHAR& pName, GrapaRuleEvent* pTable);
	virtual void DirectoryPath(GrapaCHAR& pName);
	virtual void DirectoryFullPath(GrapaCHAR& pName);
	virtual void DirectoryPWD(GrapaCHAR& pName);

	virtual GrapaError MKDIR(const GrapaCHAR& pName);
	virtual GrapaError RMDIR(const GrapaCHAR& pName);

	virtual GrapaError HomeSwitch(GrapaCHAR& pName);

	virtual GrapaError DirectoryCreate(GrapaCHAR& pName, GrapaCHAR& pType);
	virtual GrapaError DirectoryDelete(GrapaCHAR& pName);
	virtual void DirectoryDeleteContents(const GrapaCHAR& pName);
	virtual GrapaError DirectorySwitch(const GrapaCHAR& pName);
	virtual GrapaError DirectorySwitchSingle(GrapaCHAR& pName);
	virtual GrapaError DirectorySwitch(u64 pId = 0);

	virtual GrapaError DatabaseCreate(GrapaCHAR& pName, GrapaCHAR&pType);

	virtual void DatabaseSet(GrapaCHARFile& pValue);

	virtual GrapaError DataCreate(const GrapaCHAR& pName);
	virtual GrapaError DataDelete(const GrapaCHAR& pName);
	virtual void DataDelete(u64 pId);

	virtual GrapaError FieldCreate(GrapaCHAR& pName, GrapaCHAR& fieldType, GrapaCHAR& storeType, u64 storeSize, u64 storeGrow);
	virtual GrapaError FieldDelete(GrapaCHAR& pName);

	virtual GrapaError FieldInfo(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaRuleEvent* pTable);

	virtual GrapaError FieldSet(const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaCHAR& pValue);
	virtual GrapaError FieldSet(u64 pId, const GrapaCHAR& pField, const GrapaCHAR& pValue);

	virtual GrapaError FieldGet(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaCHAR& pValue);
	virtual GrapaError FieldGet(u64 pId, const GrapaCHAR& pField, GrapaCHAR& pValue);
	virtual GrapaError FieldSplit(s64 pParts, const GrapaCHAR& pName, const GrapaCHAR& pPath, const GrapaCHAR& pDelim, GrapaCHAR& pOption, GrapaRuleEvent* pTable);

	virtual GrapaError GetNameInfo(const GrapaCHAR& pFullName, u64& pDirId, u8& pDirType, GrapaCHAR& pName, bool isDir=false);

	virtual void DatabaseDump(u64 pId, GrapaCHAR& pFileName);
	virtual void DatabaseDump(u64 pId, GrapaCHARFile& mFile);

public:
	u64 mDirId;
	u8 mDirType;
	GrapaGroupEvent* mDb;
	char mSep;
	u64 mHomeDirId;
	u8 mHomeDirType;

protected:
	GrapaFileIO mFile;
};

////////////////////////////////////////////////////////////////////////////////

#endif //_GrapaDatabase_

