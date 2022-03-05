// GrapaDatabase.cpp
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

#include "GrapaSystem.h"
#include "GrapaDatabase.h"
#include "GrapaState.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "GrapaCompress.h"

#if defined(__MINGW32__) || defined(__GNUC__)
#include <pthread.h>
#include <unistd.h>
#else
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#endif

extern GrapaSystem* gSystem;

// http://msdn.microsoft.com/en-us/library/as5kw0ze.aspx
// http://www.delorie.com/gnu/docs/glibc/libc_268.html
// char* wd = getcwd (NULL, 0); free(wd);
// chdir("xyz");
// http://www.delorie.com/gnu/docs/glibc/libc_273.html
// http://msdn.microsoft.com/en-us/library/windows/desktop/aa365200(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/windows/desktop/aa363950(v=vs.85).aspx

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaLocalDatabase::GrapaLocalDatabase()
{
	mDirId = 0;
	mDirType = 0;
	mHomeDirId = 0;
	mHomeDirType = 0;
#ifdef _WIN32
	mSep = '\\';
#else
	mSep = '/';
#endif
	vScriptState = NULL;
	mDb = NULL;
	mLocation.FROM("grapa: ");
	mVar = false;
	mHomeDir.FROM(gSystem->mWorkDir);
	mDirectoryPath = new GrapaRuleQueue();
	mDatabasePath = new GrapaRuleQueue();
	mInclude = new GrapaRuleQueue();
}

GrapaLocalDatabase::GrapaLocalDatabase(GrapaScriptState* pTokenState) 
{
	mDirId = 0;
	mDirType = 0;
	mHomeDirId = 0;
	mHomeDirType = 0;
	vScriptState = pTokenState;
#ifdef _WIN32
	mSep = '\\';
#else
	mSep = '/';
#endif
	mDb = NULL;
	mLocation.FROM("grapa: ");
	mVar = false;
	mHomeDir.FROM(gSystem->mWorkDir);
	mDirectoryPath = new GrapaRuleQueue();
	mDatabasePath = new GrapaRuleQueue();
	mInclude = new GrapaRuleQueue();
}

GrapaLocalDatabase::~GrapaLocalDatabase()
{
	CLEAR();
	if (mDirectoryPath)
	{
		mDirectoryPath->CLEAR();
		delete mDirectoryPath;
		mDirectoryPath = NULL;
	}
	if (mDatabasePath)
	{
		mDatabasePath->CLEAR();
		delete mDatabasePath;
		mDatabasePath = NULL;
	}
	if (mInclude)
	{
		mInclude->CLEAR();
		delete mInclude;
		mInclude = NULL;
	}
}

void GrapaLocalDatabase::CLEAR()
{
	if (mDb)
	{
		if (mVar)
		{
			delete mDb;
		}
		else
			gSystem->mGroupQueue.CloseFile(mDb);
		mDb = NULL;
	}
	mDirId = 0;
	mDirType = 0;
	mHomeDirId = 0;
	mHomeDirType = 0;
	mVar = false;
	mFile.Close();
}

GrapaError GrapaLocalDatabase::DirectoryList(GrapaCHAR& pName, GrapaRuleEvent* pTable)
{
	GrapaError err=0;
	GrapaInt b;
	bool extended = false;

	if (mDb == NULL)
	{
		pTable->mValue.mToken = GrapaTokenType::ARRAY;
		pTable->mValue.SetLength(0);
		if (pTable->vQueue) pTable->vQueue->CLEAR();
		else pTable->vQueue = new GrapaRuleQueue();

		GrapaCHAR path; // , dir;
		DirectoryFullPath(path);
		if (pName.mLength)
		{
			path.Append("/");
			path.Append(pName);
		}

#if defined(__MINGW32__) || defined(__GNUC__)
		DIR *dp;
		struct dirent *ep;
#ifdef __APPLE__
		struct stat filestat;
#else
        struct stat64 filestat;
#endif
		dp = opendir((char*)path.mBytes);
		if (dp != NULL)
		{
			while ((ep = readdir(dp)))
			{
				char* localFileName = ep->d_name;

				if (strcmp(localFileName, ".") == 0 || strcmp(localFileName, "..") == 0)
					continue;

				GrapaRuleEvent* item = new GrapaRuleEvent(0, GrapaCHAR(localFileName), GrapaCHAR());
				item->mValue.mToken = GrapaTokenType::LIST;
				item->vQueue = new GrapaRuleQueue();
				pTable->vQueue->PushTail(item);

				GrapaCHAR fileName(path);
				fileName.Append(mSep);
				fileName.Append(localFileName);
#ifdef __APPLE__
                int statErr = stat((char*)fileName.mBytes, &filestat);
#else
				int statErr = stat64((char*)fileName.mBytes, &filestat);
#endif
				b = filestat.st_size;
				if (statErr)
				{
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR(fileName)));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("ERR")));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
					continue;
				}
				if (S_ISDIR(filestat.st_mode))
				{
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR(localFileName)));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("DIR")));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
				}
				else
				{
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR(localFileName)));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
				}
			}
			closedir(dp);
		}
#else
#ifdef _WIN32
		WIN32_FIND_DATAA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		LARGE_INTEGER filesize;
		u64 size = 0;
		GrapaCHAR searchName(path);
		searchName.Append("\\*");
		hFind = FindFirstFileA((LPCSTR)searchName.mBytes, &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
			err = -1;
		DWORD dw = GetLastError();
		if (!err)
		{
			do
			{
				char* localFileName = ffd.cFileName;
				if (strcmp(localFileName, ".") == 0 || strcmp(localFileName, "..") == 0)
					continue;

				GrapaRuleEvent* item = new GrapaRuleEvent(0, GrapaCHAR(localFileName), GrapaCHAR());
				item->mValue.mToken = GrapaTokenType::LIST;
				item->vQueue = new GrapaRuleQueue();
				pTable->vQueue->PushTail(item);

				filesize.LowPart = ffd.nFileSizeLow;
				filesize.HighPart = ffd.nFileSizeHigh;
				size = filesize.QuadPart;
				b = size;

				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR(localFileName)));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("DIR")));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
				}
				else
				{
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR(localFileName)));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
				}
			} while (FindNextFileA(hFind, &ffd) != 0);
			FindClose(hFind);
		}
#endif
#endif
	}
	else
	{
		pTable->mValue.mToken = GrapaTokenType::ARRAY;
		pTable->mValue.SetLength(0);
		if (pTable->vQueue) pTable->vQueue->CLEAR();
		else pTable->vQueue = new GrapaRuleQueue();

		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname, true);
		if (err) return(err);

		GrapaDBTable parentDict;
		parentDict.mRef = dirId;
		parentDict.mRefType = dirType;
		if (dirType == GrapaDB::GROUP_TREE)
		{
			err = mDb->mValue.OpenTable(dirId, 0, parentDict);
			if (err) return(err);
		}
		GrapaCHAR fieldKey;
		GrapaCHAR fieldName;
		GrapaCHAR fieldValue;
		GrapaDBFieldArray* fieldList = mDb->mValue.ListFields(parentDict.mRef, parentDict.mRefType);
		if (fieldList)
		{
			for (u32 i = 0; i < fieldList->Count(); i++)
			{
				GrapaDBField *field = fieldList->GetFieldAt(i);
				if (!field) continue;
				err = mDb->mValue.GetData(field->mNameRef, fieldName);
				if (err) return(err);
			}
			u64 keyId = 0;
			err = mDb->mValue.GetNameId(parentDict.mRef, parentDict.mRefType, keyId);
			if (err) return(err);
			GrapaDBCursor cursor;
			cursor.Set(parentDict.mRef);
			if (mDb->mValue.First(cursor) == 0)
			{
				do
				{
					err = mDb->mValue.GetRecordField(cursor, keyId, fieldKey);
					if (err) return(err);
					GrapaRuleEvent* item = new GrapaRuleEvent(0, GrapaCHAR(fieldKey), GrapaCHAR());
					item->mValue.mToken = GrapaTokenType::LIST;
					item->vQueue = new GrapaRuleQueue();
					pTable->vQueue->PushTail(item);

					GrapaDBField fieldRec;
					fieldName.FROM("$KEY");
					u64 maxId;
					err = mDb->mValue.FindField(dirId, dirType, fieldName, fieldRec, maxId);
					if (err) return(err);
					err = mDb->mValue.GetRecordField(cursor, fieldRec, fieldValue);
					if (err) return(err);
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), fieldValue));

					u64 treeSize = 0;
					bool hasTreeSize = false;

					if (dirType == GrapaDB::GROUP_TREE)
					{
						GrapaDBCursor tableCursor;
						fieldValue.FROM("VALUE");
						tableCursor.Set(dirId, GrapaDB::TREE_ITEM, cursor.mKey);
						err = mDb->mValue.Search(tableCursor);
						if (!err)
						{
							u8 treeType;
							tableCursor.Set(tableCursor.mValue);
							err = mDb->mValue.GetTreeType(tableCursor, treeType);
							err = mDb->mValue.GetTreeSize(tableCursor, treeSize);
							switch (treeType)
							{
							case GrapaDB::GROUP_TREE:	fieldValue.FROM("GROUP"); break;
							case GrapaDB::RTABLE_TREE:	fieldValue.FROM("ROWS"); break;
							case GrapaDB::CTABLE_TREE:	fieldValue.FROM("COLS"); break;
							}
							if (treeType == GrapaDB::GROUP_TREE)
							{
								tableCursor.Set(parentDict.mRef, GrapaDB::TREE_ITEM, cursor.mKey);
								err = mDb->mValue.GetTreeSize(tableCursor, treeSize);
							}
						}
						fieldValue.mToken = GrapaTokenType::STR;
						item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), fieldValue));
						if (dirType == GrapaDB::GROUP_TREE)
						{
							hasTreeSize = true;
						}
					}
					else
					{
						GrapaDBCursor tableCursor;
						tableCursor.Set(dirId, GrapaDB::TREE_ITEM, cursor.mKey);
						err = mDb->mValue.Search(tableCursor);
						fieldValue.FROM("?");
						switch (tableCursor.mValueType)
						{
						case GrapaDB::GREC_ITEM: fieldValue.FROM("GROUP"); break; // ERROR: Shouldn't have allowed creating a table in a table.
						case GrapaDB::RREC_ITEM: fieldValue.FROM("ROW"); break;
						case GrapaDB::CREC_ITEM: fieldValue.FROM("COL"); break;
						}
						fieldValue.mToken = GrapaTokenType::STR;
						item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), fieldValue));
					}

					fieldName.FROM("$VALUE");
					err = mDb->mValue.FindField(dirId, dirType, fieldName, fieldRec, maxId);
					err = mDb->mValue.GetRecordField(cursor, fieldRec, fieldValue);
					b = fieldValue.mLength; fieldValue = b.getBytes();
					item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), fieldValue));
					err = 0;

					if (extended)
					{
						if (hasTreeSize)
						{
							b = treeSize; fieldValue = b.getBytes();
							item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$ITEMS"), fieldValue));
						}
						for (u32 i = 0; i < fieldList->Count(); i++)
						{
							GrapaDBField* field = fieldList->GetFieldAt(i);
							if (!field) continue;
							err = mDb->mValue.GetData(field->mNameRef, fieldName);
							if (fieldName.Cmp("$KEY") != 0)
							{
								err = mDb->mValue.GetRecordField(cursor, *field, fieldValue);
								item->vQueue->PushTail(new GrapaRuleEvent(0, fieldName, fieldValue));
							}
						}
						err = 0;
					}

				} while (!(mDb->mValue.Next(cursor)));
			}
			delete fieldList;
		}
	}
	return(err);
}

void GrapaLocalDatabase::DirectoryPath(GrapaCHAR& pName)
{
	pName.SetLength(0);
	pName.FROM(mHomeDir);
	if (pName.mLength == 0)
	{
		pName.Append(".");
	}
}

void GrapaLocalDatabase::DirectoryFullPath(GrapaCHAR& pName)
{
	GrapaCHAR dir;
	DirectoryPath(pName);
	mDirectoryPath->ListNames(dir, mSep);
	if (dir.mLength)
	{
		pName.Append("/");
		pName.Append(dir);
	}
}

void GrapaLocalDatabase::DirectoryPWD(GrapaCHAR& pName)
{
	GrapaCHAR dir;
	mDirectoryPath->ListNames(dir, '/');
	if (dir.mLength)
	{
		pName.Append("/");
		pName.Append(dir);
	}
	dir.SetLength(0);
	mDatabasePath->ListNames(dir, '/');
	if (dir.mLength)
	{
		pName.Append("/");
		pName.Append(dir);
	}
	if (pName.mLength == 0)
	{
		pName.Append("/");
	}
}

GrapaError GrapaLocalDatabase::MKDIR(const GrapaCHAR& pName)
{
	GrapaError err = 0;
#if defined(__MINGW32__) || defined(__GNUC__)
	if (mkdir((char*)pName.mBytes, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
		err = -1;
#else
#ifdef _WIN32
	if (mkdir((char*)pName.mBytes))
		err = -1;
#endif
#endif
	return(err);
}

GrapaError GrapaLocalDatabase::RMDIR(const GrapaCHAR& pName)
{
	GrapaError err = 0;
	DirectoryDeleteContents(pName);
	if (rmdir((char*)pName.mBytes))
		err = -1;
	return(err);
}

GrapaError GrapaLocalDatabase::DirectoryCreate(GrapaCHAR& pName, GrapaCHAR& pType)
{
	GrapaError err=0;
	if (mDb==NULL && (pType.mLength == 0 || pType.Cmp("DIR")==0))
	{
		GrapaCHAR path;
		DirectoryFullPath(path);
		path.Append("/");
		path.Append(pName);
		err = MKDIR(path);
	}
	else
	{
		err = DatabaseCreate(pName, pType);
	}
	return(err);
}

GrapaError GrapaLocalDatabase::DirectoryDelete(GrapaCHAR& pName)
{
	GrapaError err = 0;
	if (mDb==NULL)
	{
		GrapaCHAR path;
		DirectoryFullPath(path);
		path.Append("/");
		path.Append(pName);
		err = RMDIR(path);
		GrapaError err2 = mFile.Delete((char*)path.mBytes);
		if (err && !err2) err = err2;
	}
	else
	{
		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
		if (err) return(err);
		err = mDb->mValue.DeleteGroup(dirId, dirType, fname);
		GrapaError err2 = mDb->mValue.DeleteEntry(dirId, dirType, fname);
		if (err && !err2) err = err2;
		mDb->mValue.FlushFile();
	}
	return(err);
}

void GrapaLocalDatabase::DirectoryDeleteContents(const GrapaCHAR& pName)
{
	GrapaError err = 0;
#if defined(__MINGW32__) || defined(__GNUC__)
	DIR *dp;
	struct dirent *ep;
	struct stat filestat;
	dp = opendir((char*)pName.mBytes);
	if (dp != NULL)
	{
		while ((ep = readdir(dp)))
		{
			char* localFileName = ep->d_name;
			if (strcmp(localFileName, ".") == 0 || strcmp(localFileName, "..") == 0)
				continue;
			GrapaCHAR fileName(pName);
			fileName.Append(mSep);
			fileName.Append(localFileName);
			int statErr = stat((char*)fileName.mBytes, &filestat);
			if (statErr)
			{
				continue;
			}
			if (S_ISDIR(filestat.st_mode))
			{
				DirectoryDeleteContents(fileName);
				rmdir((char*)fileName.mBytes);
			}
			else
			{
				err = mFile.Delete((char*)fileName.mBytes);
			}
		}
		closedir(dp);
	}
#else
#ifdef _WIN32
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LARGE_INTEGER filesize;
	u64 size = 0;
	GrapaCHAR searchName(pName);
	searchName.Append("\\*");
	hFind = FindFirstFileA((LPCSTR)searchName.mBytes, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		err = -1;
	DWORD dw = GetLastError();
	if (!err)
	{
		do
		{
			char* localFileName = ffd.cFileName;
			if (strcmp(localFileName, ".") == 0 || strcmp(localFileName, "..") == 0)
				continue;
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			size = filesize.QuadPart;
			GrapaCHAR fileName(pName);
			fileName.Append(mSep);
			fileName.Append(localFileName);
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				DirectoryDeleteContents(fileName);
				int r = rmdir((char*)fileName.mBytes);
			}
			else
			{
				err = mFile.Delete((char*)fileName.mBytes);
			}
		} while (FindNextFileA(hFind, &ffd) != 0);
		FindClose(hFind);
	}
#endif
#endif
}

GrapaError GrapaLocalDatabase::HomeSwitch(GrapaCHAR& pName)
{
	if (pName.mLength == 0)
	{
		return(-1);
	}

	GrapaRuleQueue names;
	names.AppendNames((char*)pName.mBytes, "\\/");

	if (mVar)
	{
		// go to the root of the path specified
		mDirId = mDb->mValue.RootTree(mDirType);
		mHomeDir.FROM("$:");
		mHomeDirId = mDirId;
		mHomeDirType = mDirType;
		mDatabasePath->CLEAR();
	}
	else
	{
		int rval;
		// go to the root of the path specified
		names.Head()->mName.Append("/");
		gSystem->mChdLock.WaitCritical();
		rval = chdir((char*)names.Head()->mName.mBytes);
		if (rval != 0)
			return(-1);
		char* buffer = getcwd(NULL, 0);
		gSystem->mChdLock.LeaveCritical();
		mHomeDir.FROM(buffer);
		free(buffer);
		mHomeDir.Replace(GrapaBYTE("\\"), GrapaBYTE("/"));
		if (mHomeDir.mLength && mHomeDir.mBytes[mHomeDir.mLength - 1] == '/')
			mHomeDir.SetLength(mHomeDir.mLength - 1);
	}

	// now navigate through the path
	GrapaRuleEvent *ev = names.PopHead();
	if (ev)
	{
		ev->CLEAR();
		delete ev;
		ev = NULL;
	}
	GrapaObjectEvent* name = names.Head();
	while (name)
	{
		GrapaError err = DirectorySwitchSingle(name->mName);
		if (err)
		{
			return(err);
		}
		name = name->Next();
	}

	// that should now be the home
	GrapaCHAR nameStr;
	mDirectoryPath->ListNames(nameStr, mSep);
	if (nameStr.mLength)
	{
		mHomeDir.Append(mSep);
		mHomeDir.Append(nameStr);
	}
	mDirectoryPath->CLEAR();
	mDatabasePath->ListNames(nameStr, mSep);
	if (nameStr.mLength)
	{
		mHomeDir.Append(mSep);
		mHomeDir.Append(nameStr);
	}
	mDatabasePath->CLEAR();
	if (mDb)
	{
		mHomeDirId = mDirId;
		mHomeDirType = mDirType;
	}

	DirectorySwitch(GrapaCHAR("/"));
	return(0);
}

GrapaError GrapaLocalDatabase::DirectorySwitch(const GrapaCHAR& pName)
{
	GrapaRuleQueue names;
	names.AppendNames((char*)pName.mBytes, "\\/");
	GrapaObjectEvent* name = names.Head();
	while (name)
	{
		GrapaError err = DirectorySwitchSingle(name->mName);
		if (err)
		{
			return(err);
		}
		name = name->Next();
	}
	return(0);
}

GrapaError GrapaLocalDatabase::DirectorySwitchSingle(GrapaCHAR& pName)
{
	GrapaError err = -1;
	char* name = (char*)pName.mBytes;
	bool isFile = false;
	GrapaCHAR path, dir;

	if (pName.mLength == 0)
	{
		if (mVar)
		{
			mDirId = mHomeDirId;
			mDirType = mHomeDirType;
			mDatabasePath->CLEAR();
		}
		else if (mDb && mHomeDirId)
		{
			mDirId = mHomeDirId;
			mDirType = mHomeDirType;
			mDatabasePath->CLEAR();
		}
		else
		{
			mDirectoryPath->CLEAR();
			if (mDb)
			{
				mDirId = 0;
				mDirType = 0;
				if (!mVar)
				{
					gSystem->mGroupQueue.CloseFile(mDb);
					mDb = NULL;
				}
				mDatabasePath->CLEAR();
			}
		}
		return(0);
	}

	if (mDb == NULL && !mVar)
	{
		DirectoryPath(path);
		if (name[0] == '.' && name[1] == '.' && name[2] == 0)
		{
			if (GrapaObjectEvent* e = mDirectoryPath->PopTail())
			{
				e->CLEAR();
				delete e;
			}
			mDirectoryPath->ListNames(dir, mSep);
		}
		else if (name[0] == '/' || name[0] == '\\')
		{
			GrapaRuleQueue dirPath;
			dirPath.AppendNames(&name[1], "\\/");
			dirPath.ListNames(dir, mSep);
		}
		else
		{
			mDirectoryPath->ListNames(dir, mSep);
			if (dir.mLength)
				dir.Append("/");
			dir.Append(pName);
		}
		path.Append("/");
		path.Append(dir);
		bool pathExists = false;
#if defined(__MINGW32__) || defined(__GNUC__)
		struct stat filestat;
		stat((char*)path.mBytes, &filestat);
		if (S_ISDIR(filestat.st_mode))
		{
			if (DIR *dp = opendir((char*)path.mBytes))
			{
				pathExists = true;
				closedir(dp);
			}
		}
		else
			isFile = true;
#else
#ifdef _WIN32
		if (GetFileAttributesA((LPCSTR)path.mBytes) & FILE_ATTRIBUTE_DIRECTORY)
		{
			GrapaCHAR pathTest(path);
			pathTest.Append("\\*");
			WIN32_FIND_DATAA ffd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			hFind = FindFirstFileA((LPCSTR)pathTest.mBytes, &ffd);
			if (INVALID_HANDLE_VALUE != hFind)
			{
				pathExists = true;
				FindClose(hFind);
			}
		}
		else
			isFile = true;
#endif
#endif
		if (pathExists)
		{
			mDirectoryPath->CLEAR();
			mDirectoryPath->AppendNames((char*)dir.mBytes, "\\/");
			err = 0;
		}
	}
	if (isFile || mDb)
	{
		if (mDb==NULL)
		{
			if (!mVar)
				mDb = gSystem->mGroupQueue.OpenFile(path, &mFile, GrapaReadWrite);
			if (mDb)
			{
				mDirId = mDb->mValue.RootTree(mDirType);
				GrapaObjectEvent* e = new GrapaObjectEvent(name);
				e->mId = mDirId;
				e->mDirType = mDirType;
				mDatabasePath->PushTail(e);
				err = 0;
			}
		}
		else
		{
			if (name[0] == '.' && name[1] == '.' && name[2] == 0)
			{
				GrapaObjectEvent* e = mDatabasePath->PopTail();
				if (e)
				{
					e->CLEAR();
					delete e;
					e = NULL;
				}
				else
				{
					return(-1);
				}
				e = mDatabasePath->Tail();
				if (e)
				{
					mDirId = e->mId;
					mDirType = (u8)e->mDirType;
				}
				else
				{
					if (mVar)
					{
						mDirId = mDb->mValue.RootTree(mDirType);
						mHomeDir.FROM("$:");
					}
					else if (mHomeDirId)
					{
						mDirId = mHomeDirId;
						mDirType = mHomeDirType;
					}
					else
					{
						mDirId = 0;
						mDirType = 0;
						if (!mVar)
						{
							gSystem->mGroupQueue.CloseFile(mDb);
							mDb = NULL;
						}
					}
				}
				err = 0;
			}
			else
			{
				u64 newDirId;
				u8 newDirType;
				u64 tableId;
				err = mDb->mValue.OpenGroup(mDirId, mDirType, pName, newDirId, newDirType, tableId);
				if (!err)
				{
					mDirId = newDirId;
					mDirType = newDirType;
					GrapaObjectEvent* e = new GrapaObjectEvent(name);
					e->mId = mDirId;
					e->mDirType = mDirType;
					mDatabasePath->PushTail(e);
				}
			}
		}
	}
	return(err);
}

GrapaError GrapaLocalDatabase::DirectorySwitch(u64 pId)
{
	if (pId == 0)
	{
		GrapaCHAR name("..");
		return DirectorySwitch(name);
	}
	if (mDb == NULL) return(-1);
	GrapaCHAR newName;
	u64 newDirId;
	u8 newDirType;
	u64 tableId;
	GrapaError err = mDb->mValue.OpenGroup(mDirId, mDirType, pId, newDirId, newDirType, tableId, newName);
	if (!err)
	{
		mDirId = newDirId;
		mDirType = newDirType;
		GrapaObjectEvent* e = new GrapaObjectEvent((char*)newName.mBytes);
		e->mId = mDirId;
		e->mDirType = mDirType;
		mDatabasePath->PushTail(e);
	}
	return(err);
}

GrapaError GrapaLocalDatabase::DatabaseCreate(GrapaCHAR& pName, GrapaCHAR& pType)
{
	GrapaError err = 0;
	u64 newDirId;
	if (mDb == NULL)
	{
		if (!mVar)
		{
			GrapaCHAR path;
			DirectoryFullPath(path);
			path.Append("/");
			path.Append(pName);
			GrapaGroupEvent*e = gSystem->mGroupQueue.Create(path, &mFile, pType);
			gSystem->mGroupQueue.CloseFile(e);
		}
	}
	else
	{
		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
		if (err) return(err);
		err = mDb->mValue.CreateGroup(dirId, dirType, fname, pType, newDirId);
		mDb->mValue.FlushFile();
	}
	return(err);
}

void GrapaLocalDatabase::DatabaseSet(GrapaCHARFile& pValue)
{ 
	mHomeDir.FROM("");
	mDatabasePath->CLEAR();
	if (mDb)
	{
		mDirId = 0;
		mDirType = 0;
		if (!mVar)
		{
			gSystem->mGroupQueue.CloseFile(mDb);
			mDb = NULL;
		}
		mVar = false;
	}
	if (pValue.mLength)
	{
		mDb = new GrapaGroupEvent(GrapaCHAR(), &pValue);
		mDb->mValue.OpenFile(GrapaCHAR(), GrapaReadWrite);
		mDirId = mDb->mValue.RootTree(mDirType);
		mHomeDir.FROM("$:");
	}
	mVar = (pValue.mLength != 0);
}

GrapaError GrapaLocalDatabase::DataCreate(const GrapaCHAR& pName)
{
	GrapaError err = 0;
	if (mDb == NULL)
	{
		GrapaCHAR path;
		DirectoryFullPath(path);
		path.Append("/");
		path.Append(pName);
		err = mFile.Create((char*)path.mBytes);
		mFile.Close();
	}
	else
	{
		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
		if (err) return(err);
		u64 id = 0;
		err = mDb->mValue.CreateEntry(dirId, dirType, fname, id);
		mDb->mValue.FlushFile();
	}
	return(err);
}

GrapaError GrapaLocalDatabase::DataDelete(const GrapaCHAR& pName)
{
	GrapaError err = 0;
	if (mDb == NULL)
	{
		GrapaCHAR path;
		DirectoryFullPath(path);
		path.Append("/");
		path.Append(pName);
		err = mFile.Delete((char*)path.mBytes);
	}
	else
	{
		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
		if (err) return(err);
		err = mDb->mValue.DeleteEntry(dirId, dirType, fname);
		mDb->mValue.FlushFile();
	}
	return(err);
}

void GrapaLocalDatabase::DataDelete(u64 pId)
{
	GrapaError err = 0;
	if (mDb == NULL) return;
	err = mDb->mValue.DeleteEntry(mDirId, mDirType, pId);
	mDb->mValue.FlushFile();
}

GrapaError GrapaLocalDatabase::FieldCreate(GrapaCHAR& pName, GrapaCHAR& fieldType, GrapaCHAR& storeType, u64 storeSize, u64 storeGrow)
{
	GrapaError err = 0;
	if (mDb == NULL) return(-1);
	u8 listType = GrapaTokenType::STR;
	u8 listStore = GrapaDBField::STORE_VAR;
	if (fieldType.mLength)
	{
		if (fieldType.StrCmp("TIME") == 0) listType = GrapaTokenType::TIME;
		else if (fieldType.StrCmp("BOOL") == 0) listType = GrapaTokenType::BOOL;
		else if (fieldType.StrCmp("INT") == 0) listType = GrapaTokenType::INT;
		else if (fieldType.StrCmp("FLOAT") == 0) listType = GrapaTokenType::FLOAT;
		else if (fieldType.StrCmp("STR") == 0) listType = GrapaTokenType::STR;
		else if (fieldType.StrCmp("TABLE") == 0) listType = GrapaTokenType::TABLE;
		else if (fieldType.StrCmp("RAW") == 0) listType = GrapaTokenType::RAW;
	}
	if (storeType.mLength)
	{

		if (storeType.StrCmp("FIX") == 0) { listType = GrapaTokenType::STR; listStore = GrapaDBField::STORE_VAR; }
		else if (storeType.StrCmp("VAR") == 0) { listType = GrapaTokenType::TABLE; listStore = GrapaDBField::STORE_VAR; }
		else if (storeType.StrCmp("PAR") == 0) { listType = GrapaTokenType::RAW; listStore = GrapaDBField::STORE_VAR; }
	}
	if (storeSize == 0) storeSize = 32;
	if (storeGrow == 0) storeGrow = 8;
	u64 dirId = mDirId;
	u8 dirType = mDirType;
	GrapaCHAR fname;
	err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
	if (err) return(err);
	err = mDb->mValue.CreateField(dirId, dirType, fname, listType, listStore, storeSize, storeGrow);
	mDb->mValue.FlushFile();
	return(err);
}

GrapaError GrapaLocalDatabase::FieldDelete(GrapaCHAR& pName)
{
	GrapaError err = 0;
	if (mDb == NULL) return(-1);
	u64 dirId = mDirId;
	u8 dirType = mDirType;
	GrapaCHAR fname;
	err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
	if (err) return(err);
	err = mDb->mValue.DeleteField(dirId, dirType, fname);
	mDb->mValue.FlushFile();
	return(err);
}

GrapaError GrapaLocalDatabase::FieldInfo(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaRuleEvent* pTable)
{
	GrapaError err = 0;
	if (pTable == NULL)
		return(-1);

	pTable->mValue.mToken = GrapaTokenType::LIST;
	pTable->mValue.SetLength(0);
	if (pTable->vQueue) pTable->vQueue->CLEAR();
	else pTable->vQueue = new GrapaRuleQueue();

	GrapaCHAR path;
	DirectoryFullPath(path);

#if defined(__MINGW32__) || defined(__GNUC__)
#ifdef __APPLE__
   struct stat filestat;
#else
   struct stat64 filestat;
#endif

	GrapaCHAR fileName(path);
	fileName.Append(mSep);
	fileName.Append(pName);
#ifdef __APPLE__
    int statErr = stat((char*)fileName.mBytes, &filestat);
#else
    int statErr = stat64((char*)fileName.mBytes, &filestat);
#endif
    GrapaInt b(filestat.st_size);
	if (statErr)
	{
		pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("ERR")));
		pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
	}
	else if (S_ISDIR(filestat.st_mode))
	{
		pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("DIR")));
		pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
	}
	else
	{
		pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
		pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
	}
#else
#ifdef _WIN32
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LARGE_INTEGER filesize;
	u64 size = 0;
	GrapaCHAR searchName(path);
	searchName.Append("\\");
	searchName.Append(pName);
	hFind = FindFirstFileA((LPCSTR)searchName.mBytes, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		err = -1;
	DWORD dw = GetLastError();
	if (!err)
	{
		filesize.LowPart = ffd.nFileSizeLow;
		filesize.HighPart = ffd.nFileSizeHigh;
		size = filesize.QuadPart;
		GrapaInt b(size);

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("DIR")));
			pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
		}
		else
		{
			pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
			pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), b.getBytes()));
		}
	}
#endif
#endif

	return err;
}

GrapaError GrapaLocalDatabase::FieldSet(const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaCHAR& pValue)
{
	GrapaError err = 0;
	if (mDb == NULL)
	{
		GrapaCHAR path;
		DirectoryFullPath(path);
		if (path.mLength && path.StrNCmp((char*)pName.mBytes) == 0)
		{
			GrapaCHAR name2;
			name2.FROM((char*)&pName.mBytes[path.mLength], pName.mLength - path.mLength);
			path.Append(name2);
		}
		else
		{
			path.Append("/");
			path.Append(pName);
		}
		err = mFile.Open((char*)path.mBytes, GrapaReadWriteCreate);
		if (err) return(err);
		u64 size = pValue.mLength;
		err = mFile.SetSize(size);
		err = mFile.Write(0, 0, 0, size, pValue.mBytes);
		mFile.Purge(size, 1);
		err = mFile.Close();
	}
	else
	{
		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
		if (err) return(err);
		GrapaCHAR field(pField);
		if (field.mLength == 0) field.FROM("$VALUE");
		GrapaCHAR pValue2;
		err = mDb->mValue.SetField(dirId, dirType, fname, field, pValue);
		mDb->mValue.FlushFile();
	}
	return(err);
}

GrapaError GrapaLocalDatabase::FieldGet(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaCHAR& pValue)
{
	GrapaError err = 0;
	pValue.SetLength(0);
	if (mDb == NULL)
	{
		GrapaCHAR path;
		DirectoryFullPath(path);
		if (path.mLength && path.StrNCmp((char*)pName.mBytes)==0)
		{
			GrapaCHAR name2;
			name2.FROM((char*)&pName.mBytes[path.mLength], pName.mLength - path.mLength);
			path.Append(name2);
		}
		else
		{
			path.Append("/");
			path.Append(pName);
		}
		err = mFile.Open((char*)path.mBytes);
		if (err) return(err);
		u64 size = 0;
		err = mFile.GetSize(size);
		if (size)
		{
			pValue.SetLength(size);
			err = mFile.Read(0, 0, 0, size, pValue.mBytes);
			pValue.SetLength(size);
			pValue.mToken = GrapaTokenType::RAW;
		}
		err = mFile.Close();
	}
	else
	{
		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
		if (err) return(err);
		GrapaCHAR field(pField);
		if (field.mLength == 0) field.FROM("$VALUE");
		err = mDb->mValue.GetField(dirId, dirType, fname, field, pValue);
	}
	return(err);
}

GrapaError GrapaLocalDatabase::FieldSet(u64 pId, const GrapaCHAR& pField, const GrapaCHAR& pValue)
{
	GrapaError err = 0;
	if (mDb == NULL) return(-1);
	GrapaCHAR field(pField);
	if (field.mLength == 0) field.FROM("$VALUE");
	err = mDb->mValue.SetField(mDirId, mDirType, pId, field, pValue);
	mDb->mValue.FlushFile();
	return(err);
}

GrapaError GrapaLocalDatabase::FieldGet(u64 pId, const GrapaCHAR& pField, GrapaCHAR& pValue)
{
	GrapaError err = 0;
	pValue.SetLength(0);
	if (mDb == NULL) return(-1);
	GrapaCHAR field(pField);
	if (field.mLength == 0) field.FROM("$VALUE");
	err = mDb->mValue.GetField(mDirId, mDirType, pId, field, pValue);
	return(err);
}

GrapaError GrapaLocalDatabase::FieldSplit(s64 pParts, const GrapaCHAR& pName, const GrapaCHAR& pPath, const GrapaCHAR& pDelim, GrapaCHAR& pOption, GrapaRuleEvent* pTable)
{
	GrapaError err = 0;
	GrapaCHAR delim(pDelim);
	if (pTable)
	{
		pTable->mValue.mToken = GrapaTokenType::ARRAY;
		pTable->mValue.SetLength(0);
		if (pTable->vQueue) pTable->vQueue->CLEAR();
		else pTable->vQueue = new GrapaRuleQueue();
	}
	if (mDb == NULL)
	{
		GrapaCHAR path;
		DirectoryFullPath(path);
		if (path.mLength && path.StrNCmp((char*)pName.mBytes) == 0)
		{
			GrapaCHAR name2;
			name2.FROM((char*)&pName.mBytes[path.mLength], pName.mLength - path.mLength);
			path.Append(name2);
		}
		else
		{
			path.Append("/");
			path.Append(pName);
		}
		GrapaCHAR pNameLast;
		char* lastSlash = strrchr((char*)pName.mBytes, '/');
		if (lastSlash)
			pNameLast.FROM(lastSlash + 1, pName.mLength - (lastSlash - ((char*)pName.mBytes) - 1));
		else
		{
			lastSlash = strrchr((char*)pName.mBytes, '\\');
			if (lastSlash)
				pNameLast.FROM(lastSlash + 1, pName.mLength - (lastSlash - ((char*)pName.mBytes) - 1));
			else
			{
				pNameLast.FROM(pName);
			}
		}
		err = mFile.Open((char*)path.mBytes);
		if (err) return(err);
		u64 size = 0;
		err = mFile.GetSize(size);
		if (size)
		{
			s64 partsize = size / pParts;
			if (size % pParts) partsize++;
			s64 pos = 0;
			GrapaBYTE buf;
			buf.SetSize(partsize);
			s64 count = 1;
			GrapaCHAR zeros,firstLine;
			s64 partsrem = pParts;
			s64 numdig = 0;
			DirectoryFullPath(path);
			GrapaCHAR filepath(path);
			if (pPath.mLength && pPath.mBytes[0] != '/')
				filepath.Append("/");
			filepath.Append(pPath);
			filepath.Append("/");
			while (true)
			{
				partsrem /= 10;
				numdig++;
				if (partsrem == 0) break;
				zeros.Append('0');
			}
			while (size)
			{
				s64 sz = size > partsize ? partsize : size;
				err = mFile.Read(0, 0, pos, sz, buf.mBytes);
				pos += sz;
				size -= sz;
				GrapaCHAR nstr(zeros);
				nstr.Append(GrapaInt(count).ToString());
				if (nstr.mLength - numdig)
				{
					nstr.mLength = GrapaMem::MoveLeft(nstr.mBytes, nstr.mLength - numdig, nstr.mLength);
					//memcpy(nstr.mBytes, &nstr.mBytes[nstr.mLength - numdig], numdig);
					//nstr.SetLength(numdig);
				}
				nstr.Append('.');
				nstr.Append(pNameLast);
				GrapaCHAR xpath(filepath);
				xpath.Append(nstr);
				if (pTable)
					pTable->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), nstr));
				GrapaFileIO filepart;
				err = filepart.Create((char*)xpath.mBytes);
				if (err == 0)
				{
					if (firstLine.mLength)
						err = filepart.Write(0, 0, 0, firstLine.mLength, firstLine.mBytes);
					err = filepart.Write(0, 0, firstLine.mLength, sz, buf.mBytes);
					if (delim.mLength == 0 && count == 1 && pOption.Cmp("csv") == 0)
					{
						char* pr = (char*)GrapaMem::MemFind(buf.mBytes, '\r', sz);
						char* pn = (char*)GrapaMem::MemFind(buf.mBytes, '\n', sz);
						if (pr && pn && pn > (pr + 1)) pn = NULL;
						delim.FROM("\n");
						if (pr && (&pr[1]) == pn) delim.FROM("\r\n");
					}
					if (delim.mLength)
					{
						if (count == 1)
						{
							if (pOption.Cmp("csv") == 0)
							{
								char* s = (char*)GrapaMem::MemFind(buf.mBytes, sz, (void*)delim.mBytes, delim.mLength);
								if (s)
									firstLine.FROM((char*)buf.mBytes, (u64)(s - (char*)buf.mBytes));
							}
						}
						if (pOption.Cmp("start") == 0)
						{
							if (size >= delim.mLength)
							{
								err = mFile.Read(0, 0, pos, delim.mLength, buf.mBytes);
								while ((size > delim.mLength) && memcmp(buf.mBytes, delim.mBytes, delim.mLength) != 0)
								{
									err = filepart.Write(0, 0, sz, 1, &buf.mBytes[0]);
									GrapaMem::MoveLeft(buf.mBytes, 1, delim.mLength);
									//memcpy(buf.mBytes, &buf.mBytes[1], delim.mLength - 1);
									pos++;
									size--;
									sz++;
									if (size == 0) break;
									err = mFile.Read(0, 0, pos + delim.mLength-1, 1, &buf.mBytes[delim.mLength - 1]);
								}
							}
							if (size < delim.mLength || (size == delim.mLength && memcmp(buf.mBytes, delim.mBytes, delim.mLength) != 0))
							{
								err = mFile.Read(0, 0, pos, size, buf.mBytes);
								err = filepart.Write(0, 0, sz, size, buf.mBytes);
								pos += size;
								size -= size;
								sz += size;
							}
						}
						else
						{
							memset(buf.mBytes, 0, delim.mLength);
							if (sz >= delim.mLength)
								GrapaMem::MoveLeft(buf.mBytes, sz - delim.mLength, sz);
								//memcpy(buf.mBytes, &buf.mBytes[sz - delim.mLength], delim.mLength);
							while (size && memcmp(buf.mBytes, delim.mBytes, delim.mLength) != 0)
							{
								GrapaMem::MoveLeft(buf.mBytes, 1, delim.mLength);
								//memcpy(buf.mBytes, &buf.mBytes[1], delim.mLength - 1);
								err = mFile.Read(0, 0, pos, 1, &buf.mBytes[delim.mLength - 1]);
								err = filepart.Write(0, 0, (count == 1) ? sz : firstLine.mLength + sz, 1, &buf.mBytes[delim.mLength - 1]);
								pos++;
								size--;
								sz++;
							}
							if (count == 1)
							{
								if (pOption.Cmp("csv") == 0)
								{
									if (firstLine.mLength == 0)
									{
										firstLine.SetLength(pos);
										err = mFile.Read(0, 0, 0, pos, firstLine.mBytes);
									}
									firstLine.Append(delim);
								}
							}
						}
					}
					err = filepart.Close();
				}
				count++;
			}
		}
		err = mFile.Close();
	}
	else
	{
		err = -1;
		// NOT IMPLEMENTED YET
		u64 dirId = mDirId;
		u8 dirType = mDirType;
		GrapaCHAR fname;
		err = GrapaLocalDatabase::GetNameInfo(pName, dirId, dirType, fname);
		if (err) return(err);
	}
	return(err);
}

GrapaError GrapaLocalDatabase::GetNameInfo(const GrapaCHAR& pFullName, u64& pDirId, u8& pDirType, GrapaCHAR& pName, bool isDir)
{
	GrapaError err = 0;
	pName.SetLength(0);
	if (mDb == NULL)
	{
		return(-1);
	}
	else
	{
		pDirId = mDirId;
		pDirType = mDirType;
		GrapaRuleQueue names;
		names.AppendNames((char*)pFullName.mBytes, "\\/");
		GrapaObjectEvent* name = names.Head();
		while (name)
		{
			if (!isDir && !name->Next())
				break;
			if (name->mName.mLength == 0)
			{
				pDirId = mHomeDirId;
				pDirType = mHomeDirType;
			}
			else
			{
				u64 newDirId = 0;
				u8 newDirType = 0;
				u64 tableId = 0;
				err = mDb->mValue.OpenGroup(pDirId, pDirType, name->mName, newDirId, newDirType, tableId);
				if (!err)
				{
					pDirId = newDirId;
					pDirType = newDirType;
				}
			}
			if (err)
			{
				return(err);
			}
			name = name->Next();
		}
		if (!isDir)
		{
			if (name && name->mName.mLength) pName.FROM(name->mName); else pName.FROM(pFullName);
		}
	}
	return(err);
}


void GrapaLocalDatabase::DatabaseDump(u64 pId, GrapaCHAR& pFileName)
{
	if (mDb == NULL) return;
	if (GrapaObjectEvent* e = mDatabasePath->Head())
	{
		if (pFileName.mLength)
		{
			GrapaCHAR path;
			DirectoryFullPath(path);
			path.Append("/");
			path.Append(e->mName);
			path.Append(".");
			path.Append(pFileName);
			GrapaFileIO dump;
			dump.Delete((char*)path.mBytes);
			dump.Open((char*)path.mBytes, GrapaReadWriteCreate);
			mDb->mValue.DumpGroup(mDirId, mDirType, pId, &dump);
		}
		else
		{
			mDb->mValue.DumpGroup(mDirId, mDirType, pId);
		}
	}
}

void GrapaLocalDatabase::DatabaseDump(u64 pId, GrapaCHARFile& dump)
{
	if (mDb==NULL) return;
	if (0==dump.SetSize(30000))
    {
		dump.SetLength(0);
        mDb->mValue.DumpGroup(mDirId, mDirType, pId, &dump);
        dump.SetSize(dump.mLength + 1);
		dump.SetLength(dump.mLength);
    }
}

//void GrapaLocalDatabase::Running()
//{
//
//}

///////////////////////////////////////////////////////////////////////////////////////////////////


/*
mkdir mydir Create a directory. 
rmdir mydir Removes a directory. 
ls List of ﬁles. 
cd mydir Change directory. 
cd .. Change directory back. 
pico foo.cc Edit a ﬁle (works on dana). 
mcedit foo.cc Edit a ﬁle (works in cygwin). 
rm foo.cc Remove a ﬁle. 
cp source dest Copy a ﬁle. 
mv source dest Move a ﬁle. 
indent -br foo.cc Beautify C++ source. 
man g++ Get the manual page. 
g++ foo.cc Compile C++ source. Executable is a.out. 
g++ -O foo.cc Compile C++ with optimization. 
g++ -o foo foo.cc Compile C++. Executable is foo. 
./foo Run a program. 
./foo < foo.in Input from a ﬁle. 
./foo > foo.out Output to a ﬁle. 
./foo >> foo.out Output appended to a ﬁle. 
./foo1 | ./foo2 The output of foo1 is the input of foo2. 
cat foo.out Display ﬁle. 
less foo.out Read a ﬁle. 
cmp foo.out foo2.out Compare two ﬁles. 
gv foo.ps View postscript ﬁle. Only on a graphical terminal. Could be ggv not gv. 
lpr foo.ps Print a ﬁle. 
chmod +x foo Make a script executable. 
chmod a+r foo Make a ﬁle readable for everybody. 
grep main *.cc Search for lines containing main in ﬁles. 
zip zipname files Compress ﬁles into zipﬁle. 
unzip zipname.zip Uncompress ﬁles. 
kill pid Kill process with pid 
ps -a Show running processes 
xwpe foo.cc Open an integrated editor, compiler, debugger (if installed). 
mc -U Text mode ﬁle manager. Norton Commander clone. 
lpr foo.ps Prints the ﬁle. 
a2ps foo.cc Pretty prints the ﬁle. Can create postscript instead of printing. 
gnuplot foo.gnu Runs a gnuplot script.
*/
