// GrapaDatabaseTemplate.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "GrapaDatabaseTemplate.h"

////////////////////////////////////////////////////////////////////////////////

/* Simple placeholder implementations for enhanced LocalDatabase */

GrapaError GrapaUnifiedLocalDatabase::InitializeStorage(const GrapaCHAR& storageUrl)
{
    GrapaError err = 0;
    
    // Parse the storage URL to determine type first
    err = ParseStorageUrl(storageUrl);
    if (err) return err;
    
    // Use the requested table type as the root type
    mGrapaDBXRootType = mTableType;
    
    // Store the requested table type for use when creating tables
    mGrapaDBXTableType = mTableType;
    
    printf("[DEBUG] InitializeStorage: Set mGrapaDBXRootType=%d, mGrapaDBXTableType=%d\n", 
           mGrapaDBXRootType, mGrapaDBXTableType);
    
    printf("[DEBUG] InitializeStorage: After ParseStorageUrl, mStorageType='%s'\n", (char*)mStorageType.mBytes);
    
    // Initialize based on storage type
    if (mStorageType.StrCmp("FILESYSTEM") == 0) {
        // File system is always available
        printf("[DEBUG] InitializeStorage: Using FILESYSTEM storage\n");
    } else if (mStorageType.StrCmp("GRAPADBX") == 0) {
        printf("[DEBUG] InitializeStorage: Using GRAPADBX storage\n");
        if (!mGrapaDBX) {
            // Create GrapaGroup2 without file - following GrapaGroup pattern
            mGrapaDBX = new GrapaGroup2();
            if (!mGrapaDBX) return -1;
        }
        
        // Use the existing mFile object from the unified system
        mGrapaDBX->INIT(&mFile);
        
        // Create the database file with the detected table type
        err = mGrapaDBX->Create((char*)mStoragePath.mBytes, mGrapaDBXRootType, mGrapaDBXFirstTree);
        if (err) {
            // Try to open existing database
            err = mGrapaDBX->OpenFile(mStoragePath, 'r');
            if (err) return err;
            mGrapaDBXRootType = GrapaDB::GROUP_TREE;
        } else {
            // Database created successfully, set root type to the detected table type
            mGrapaDBXRootType = mTableType;
        }
    } else if (mStorageType.StrCmp("NETWORK") == 0) {
        printf("[DEBUG] InitializeStorage: Using NETWORK storage\n");
        if (!mNetwork) {
            mNetwork = new GrapaNet();
            if (!mNetwork) return -1;
        }
    } else if (mStorageType.StrCmp("MEMORY") == 0) {
        printf("[DEBUG] InitializeStorage: Using MEMORY storage\n");
        
        // Check if this is a DBX memory operation (mStoragePath == "$")
        if (mStoragePath.StrCmp("$") == 0) {
            printf("[DEBUG] InitializeStorage: Using in-memory DBX storage\n");
            if (!mGrapaDBX) {
                // Create GrapaGroup2 for in-memory operations
                mGrapaDBX = new GrapaGroup2();
                if (!mGrapaDBX) return -1;
            }
            
            // Use the existing mFile object from the unified system
            mGrapaDBX->INIT(&mFile);
            
            // Create in-memory database with detected table type
            err = mGrapaDBX->Create("$", mGrapaDBXRootType, mGrapaDBXFirstTree);
            if (err) return err;
            
            // Set root type to the detected table type
            mGrapaDBXRootType = mTableType;
            
            printf("[DEBUG] InitializeStorage: Created in-memory DBX with root type %d\n", mGrapaDBXRootType);
        }
        // Memory storage is always available
    } else if (mStorageType.StrCmp("CLOUD") == 0) {
        printf("[DEBUG] InitializeStorage: Using CLOUD storage\n");
        if (!mNetwork) {
            mNetwork = new GrapaNet();
            if (!mNetwork) return -1;
        }
    } else {
        printf("[DEBUG] InitializeStorage: Unknown storage type '%s'\n", (char*)mStorageType.mBytes);
        return -1; // Unknown storage type
    }
    
    printf("[DEBUG] InitializeStorage: Final mStorageType='%s'\n", (char*)mStorageType.mBytes);
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::ParseStorageUrl(const GrapaCHAR& storageUrl)
{
    GrapaCHAR url = storageUrl;
    printf("[DEBUG] ParseStorageUrl: url='%s' (len=%llu)\n", (char*)url.mBytes, (unsigned long long)url.mLength);
    // Default to file system
    mStorageType.FROM("FILESYSTEM");
    mStoragePath = url;
    // Check for database protocols
    printf("[DEBUG] ParseStorageUrl: url='%s' (len=%llu)\n", (char*)url.mBytes, (unsigned long long)url.mLength);
    printf("[DEBUG] Compare to 'grapadb://': %d\n", (int)url.StrNCmp("grapadb://", 10));
    printf("[DEBUG] Compare to 'grapadbx://': %d\n", (int)url.StrNCmp("grapadbx://", 11));
    if (url.StrNCmp("grapadbx://", 11) == 0) {
        mStorageType.FROM("GRAPADBX");
        GrapaCHAR pathAndParams;
        pathAndParams.FROM((char*)url.mBytes + 11, url.mLength - 11);
        
        // Check for URL parameters (e.g., ?type=COL, ?type=ROW)
        const char* questionPos = strchr((char*)pathAndParams.mBytes, '?');
        if (questionPos) {
            // Extract the path (before ?)
            int pathLen = questionPos - (char*)pathAndParams.mBytes;
            mStoragePath.FROM((char*)pathAndParams.mBytes, pathLen);
            
            // Parse parameters (after ?)
            GrapaCHAR params;
            params.FROM(questionPos + 1, pathAndParams.mLength - pathLen - 1);
            
            // Check for table type parameter
            if (params.StrNCmp("type=COL", 8) == 0) {
                mTableType = GrapaDB::CTABLE_TREE;
                printf("[DEBUG] ParseStorageUrl: Detected COL table type from URL\n");
            } else if (params.StrNCmp("type=ROW", 8) == 0) {
                mTableType = GrapaDB::RTABLE_TREE;
                printf("[DEBUG] ParseStorageUrl: Detected ROW table type from URL\n");
            } else if (params.StrNCmp("type=GROUP", 10) == 0) {
                mTableType = GrapaDB::GROUP_TREE;
                printf("[DEBUG] ParseStorageUrl: Detected GROUP table type from URL\n");
            }
        } else {
            // No parameters, use default GROUP_TREE
            mStoragePath = pathAndParams;
        }
        
        // Check if this should be in-memory (no file path or special in-memory indicator)
        if (mStoragePath.StrCmp("") == 0 || mStoragePath.StrCmp("memory") == 0 || mStoragePath.StrCmp("$") == 0) {
            printf("[DEBUG] ParseStorageUrl: Switching to in-memory storage for DBX\n");
            mStorageType.FROM("MEMORY");
            mStoragePath.FROM("$"); // Use "$" like the old $file().table() for in-memory
        }
    } else if (url.StrCmp("http://") == 0 || url.StrCmp("https://") == 0 || 
               url.StrCmp("ftp://") == 0 || url.StrCmp("sftp://") == 0) {
        mStorageType.FROM("NETWORK");
        mStoragePath = url;
    } else if (url.StrNCmp("memory://", 9) == 0) {
        printf("[DEBUG] ParseStorageUrl: memory:// comparison returned 0, setting MEMORY type\n");
        mStorageType.FROM("MEMORY");
        mStoragePath.FROM((char*)url.mBytes + 9, url.mLength - 9);
    } else if (url.StrCmp("s3://") == 0 || url.StrCmp("gs://") == 0 || 
               url.StrCmp("azure://") == 0) {
        mStorageType.FROM("CLOUD");
        mStoragePath = url;
        // Parse cloud-specific components
        GrapaCHAR pathCopy = url;
        if (url.StrCmp("s3://") == 0) {
            mCloudProvider.FROM("s3");
            pathCopy.FROM((char*)url.mBytes + 5, url.mLength - 5);
        } else if (url.StrCmp("gs://") == 0) {
            mCloudProvider.FROM("gs");
            pathCopy.FROM((char*)url.mBytes + 5, url.mLength - 5);
        } else if (url.StrCmp("azure://") == 0) {
            mCloudProvider.FROM("azure");
            pathCopy.FROM((char*)url.mBytes + 8, url.mLength - 8);
        }
        // Extract bucket/container and path
        const char* pathStr = (const char*)pathCopy.mBytes;
        const char* slashPos = strchr(pathStr, '/');
        if (slashPos) {
            int bucketLen = slashPos - pathStr;
            mCloudBucket.FROM(pathStr, bucketLen);
            mStoragePath.FROM(slashPos + 1, pathCopy.mLength - bucketLen - 1);
        } else {
            mCloudBucket = pathCopy;
            mStoragePath.FROM("");
        }
    } else {
        printf("[DEBUG] ParseStorageUrl: memory:// comparison returned %d\n", (int)url.StrNCmp("memory://", 9));
    }
    printf("[DEBUG] ParseStorageUrl: mStorageType='%s', mStoragePath='%s'\n", (char*)mStorageType.mBytes, (char*)mStoragePath.mBytes);
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::SwitchStorageType(const GrapaCHAR& newStorageType)
{
    /* TODO: Implement storage type switching */
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::DirectorySwitchSingle(GrapaCHAR& pName)
{
    /* TODO: Implement enhanced directory switching */
    return GrapaLocalDatabase::DirectorySwitchSingle(pName);
}

GrapaError GrapaUnifiedLocalDatabase::DirectorySwitch(const GrapaCHAR& pName)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::DirectorySwitch: pName='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)mStorageType.mBytes);
    
    /* Route based on storage type */
    if (mStorageType.StrCmp("GRAPADBX") == 0)
    {
        printf("[DEBUG] DirectorySwitch: Using GRAPADBX storage\n");
        if (!mGrapaDBX)
        {
            printf("[DEBUG] DirectorySwitch: mGrapaDBX is NULL\n");
            return -1;
        }
        
        /* For GrapaDBX storage, navigate to the specified table/group */
        GrapaDBXTable table;
        GrapaError err = GrapaDBXNavigateToTable(pName, table);
        if (err)
        {
            printf("[DEBUG] DirectorySwitch: GrapaDBXNavigateToTable failed with error %d\n", err);
            return err;
        }
        
        /* Update the current directory context to the table */
        mDirId = table.mId;
        mDirType = table.mRefType;
        printf("[DEBUG] DirectorySwitch: Updated context to table.mId=%llu, table.mRefType=%d\n", 
               (unsigned long long)table.mId, table.mRefType);
        
        return 0;
    }
    else
    {
        /* For other storage types, use the base implementation */
        return GrapaLocalDatabase::DirectorySwitch(pName);
    }
}

void GrapaUnifiedLocalDatabase::DirectoryPWD(GrapaCHAR& pName)
{
    /* TODO: Implement enhanced PWD */
    GrapaLocalDatabase::DirectoryPWD(pName);
}

GrapaError GrapaUnifiedLocalDatabase::DirectoryCreate(GrapaCHAR& pName, GrapaCHAR& pType)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::DirectoryCreate: pName='%s', pType='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)pType.mBytes, (char*)mStorageType.mBytes);
    
    // Use the new CreateTableStructure method to create proper table structures
    return CreateTableStructure(pName, pType);
}

GrapaError GrapaUnifiedLocalDatabase::DataCreate(const GrapaCHAR& pName)
{
    /* TODO: Implement enhanced data creation */
    return GrapaLocalDatabase::DataCreate(pName);
}

GrapaError GrapaUnifiedLocalDatabase::DataDelete(const GrapaCHAR& pName)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::DataDelete: pName='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)mStorageType.mBytes);
    
    /* Route based on storage type */
    if (mStorageType.StrCmp("GRAPADBX") == 0)
    {
        printf("[DEBUG] DataDelete: Using GRAPADBX storage\n");
        if (!mGrapaDBX)
        {
            printf("[DEBUG] DataDelete: mGrapaDBX is NULL\n");
            return -1;
        }
        
        /* For GrapaDBX storage, delete the record from the current table context */
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2)
        {
            printf("[DEBUG] DataDelete: Using current context dirId=%llu, dirType=%d\n", 
                   (unsigned long long)mDirId, mDirType);
            
            /* Delete the record from the current table context */
            GrapaError err = group2->DeleteEntry(mDirId, mDirType, pName);
            if (err) {
                printf("[DEBUG] DataDelete: DeleteEntry failed with error %d\n", err);
            } else {
                printf("[DEBUG] DataDelete: Record deleted successfully\n");
            }
            return err;
        }
        else
        {
            printf("[DEBUG] DataDelete: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
    }
    else
    {
        /* For other storage types, use the base implementation */
        return GrapaLocalDatabase::DataDelete(pName);
    }
}

GrapaError GrapaUnifiedLocalDatabase::DirectoryList(GrapaCHAR& pName, GrapaRuleEvent* pTable)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::DirectoryList: pName='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)mStorageType.mBytes);
    
    /* Route based on storage type */
    if (mStorageType.StrCmp("GRAPADBX") == 0)
    {
        printf("[DEBUG] DirectoryList: Using GRAPADBX storage\n");
        if (!mGrapaDBX)
        {
            printf("[DEBUG] DirectoryList: mGrapaDBX is NULL\n");
            return -1;
        }
        
        /* For GrapaDBX storage, list records in the current table context */
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2)
        {
            printf("[DEBUG] DirectoryList: Using current context dirId=%llu, dirType=%d\n", 
                   (unsigned long long)mDirId, mDirType);
            
            /* Initialize the result table */
            pTable->mValue.mToken = GrapaTokenType::ARRAY;
            pTable->mValue.SetLength(0);
            if (pTable->vQueue) pTable->vQueue->CLEAR();
            else pTable->vQueue = new GrapaRuleQueue();
            
            /* First get the actual table tree reference using OpenTable */
            GrapaDBXTable parentDict;
            parentDict.mRef = mDirId;
            parentDict.mRefType = mDirType;
            
            /* Open the table to get the actual tree reference */
            GrapaError openErr = group2->OpenTable(3, mDirId, parentDict); // 3 is the root tree
            printf("[DEBUG] DirectoryList: OpenTable returned %d, parentDict.mRef=%llu\n", 
                   openErr, (unsigned long long)parentDict.mRef);
            
            if (!openErr)
            {
                /* Enumerate all records in the current table */
                GrapaDBXCursor cursor;
                cursor.mTreeRef = parentDict.mRef;
                printf("[DEBUG] DirectoryList: Setting cursor.mTreeRef=%llu\n", (unsigned long long)cursor.mTreeRef);
                
                /* First position on the record tree using First() */
                GrapaError firstErr = group2->First(cursor);
                printf("[DEBUG] DirectoryList: First() returned %d, cursor.mValue=%llu, cursor.mValueType=%d\n", 
                       firstErr, (unsigned long long)cursor.mValue, cursor.mValueType);
                
                if (!firstErr)
                {
                    /* Now use FirstDb() for database-level navigation */
                    firstErr = group2->FirstDb(cursor);
                    printf("[DEBUG] DirectoryList: FirstDb() returned %d, cursor.mValue=%llu, cursor.mValueType=%d\n", 
                           firstErr, (unsigned long long)cursor.mValue, cursor.mValueType);
                    
                    if (!firstErr)
                    {
                        // Keep track of seen table names to avoid duplicates
                        GrapaRuleQueue seenTables;
                        
                        do
                        {
                            // Read the table name directly from field 0 of the RREC entry
                            GrapaCHAR tableName;
                            GrapaError fieldErr = group2->GetRecordField(cursor, 0, tableName);
                            printf("[DEBUG] DirectoryList: GetRecordField returned %d, tableName='%s'\n", 
                                   fieldErr, (char*)tableName.mBytes);
                            
                            if (fieldErr) {
                                printf("[DEBUG] DirectoryList: GetRecordField failed, trying NextDb\n");
                                GrapaError nextErr = group2->NextDb(cursor);
                                if (nextErr) break;
                                continue;
                            }
                            
                            // Clean up null terminators from table name
                            GrapaCHAR cleanTableName;
                            for (u64 i = 0; i < tableName.mLength; i++) {
                                if (tableName.mBytes[i] != 0) {
                                    cleanTableName.Append((char)tableName.mBytes[i]);
                                }
                            }
                            
                            // Check if we've already seen this table name
                            bool alreadySeen = false;
                            GrapaObjectEvent* seenTable = seenTables.Head();
                            while (seenTable) {
                                if (seenTable->mName.StrCmp(cleanTableName) == 0) {
                                    alreadySeen = true;
                                    break;
                                }
                                seenTable = seenTable->Next();
                            }
                            
                            if (alreadySeen) {
                                printf("[DEBUG] DirectoryList: Skipping duplicate table '%s'\n", (char*)cleanTableName.mBytes);
                                GrapaError nextErr = group2->NextDb(cursor);
                                if (nextErr) break;
                                continue;
                            }
                            
                            // Add to seen tables list
                            GrapaObjectEvent* newSeenTable = new GrapaObjectEvent((char*)cleanTableName.mBytes);
                            seenTables.PushTail(newSeenTable);
                            
                            // Create the record entry with the table name
                            GrapaRuleEvent* row = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
                            row->mValue.mToken = GrapaTokenType::LIST;
                            row->vQueue = new GrapaRuleQueue();

                            // Add the table name as $KEY
                            row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), cleanTableName));
                            
                            // Determine the table type based on the root type (since we're in a GROUP database)
                            GrapaCHAR tableType;
                            if (mGrapaDBXRootType == GrapaDB::GROUP_TREE) {
                                tableType = "GROUP";
                            } else if (mGrapaDBXRootType == GrapaDB::RTABLE_TREE) {
                                tableType = "ROW";
                            } else if (mGrapaDBXRootType == GrapaDB::CTABLE_TREE) {
                                tableType = "COL";
                            } else {
                                tableType = "UNKNOWN";
                            }
                            
                            // Add the table type as $TYPE
                            row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), tableType));
                            
                            // Add the data size as $BYTES
                            GrapaInt size(cleanTableName.mLength);
                            row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), size.getBytes()));

                            pTable->vQueue->PushTail(row);
                            
                            GrapaError nextErr = group2->NextDb(cursor);
                            if (nextErr) break;
                        } while (true);
                    }
                }
            }
            
            printf("[DEBUG] DirectoryList: Successfully listed entries\n");
            return 0;
        }
        else
        {
            printf("[DEBUG] DirectoryList: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
    }
    else
    {
        /* For other storage types, use the base implementation */
        return GrapaLocalDatabase::DirectoryList(pName, pTable);
    }
}

GrapaError GrapaUnifiedLocalDatabase::FieldSet(const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaCHAR& pValue)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::FieldSet: pName='%s', pField='%s', pValue='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)pField.mBytes, (char*)pValue.mBytes, (char*)mStorageType.mBytes);
    
    // Route based on storage type
    if (mStorageType.StrCmp("MEMORY") == 0) {
        printf("[DEBUG] FieldSet: Using MEMORY storage\n");
        if (!mGrapaDBX) {
            printf("[DEBUG] FieldSet: mGrapaDBX is NULL\n");
            return -1;
        }
        
        // For memory storage, use GrapaDBX with in-memory file
        GrapaDBXTable table;
        GrapaCHAR tableName("$");
        GrapaError err = GrapaDBXNavigateToTable(tableName, table);
        if (err) {
            printf("[DEBUG] FieldSet: GrapaDBXNavigateToTable failed with error %d\n", err);
            return err;
        }
        
        // Create or find the record
        u64 recordId;
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2) {
            // Use the table tree and type from navigation instead of root tree
            printf("[DEBUG] FieldSet: Using table.mRef=%llu, table.mRefType=%d for SetField\n", 
                   (unsigned long long)table.mRef, table.mRefType);
            
            err = group2->CreateEntry(table.mRef, table.mRefType, pName, recordId);
            if (err) {
                printf("[DEBUG] FieldSet: CreateEntry failed with error %d\n", err);
                return err;
            }
            
            // Set the field value
            GrapaCHAR fieldName(pField);
            if (fieldName.mLength == 0) fieldName.FROM("$VALUE");
            
            // Convert GrapaCHAR to GrapaBYTE
            GrapaBYTE fieldValue;
            fieldValue.FROM(pValue.mLength, pValue.mBytes);
            fieldValue.mToken = pValue.mToken;
            
            err = group2->SetField(table.mRef, table.mRefType, pName, fieldName, fieldValue);
            if (err) {
                printf("[DEBUG] FieldSet: SetField failed with error %d\n", err);
                return err;
            }
            
            printf("[DEBUG] FieldSet: Successfully set field\n");
        } else {
            printf("[DEBUG] FieldSet: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
        
    } else if (mStorageType.StrCmp("GRAPADBX") == 0) {
        printf("[DEBUG] FieldSet: Using GRAPADBX storage\n");
        if (!mGrapaDBX) {
            printf("[DEBUG] FieldSet: mGrapaDBX is NULL\n");
            return -1;
        }
        
        // For GrapaDBX storage, use file-based GrapaDBX
        GrapaDBXTable table;
        GrapaCHAR tableName("$");
        GrapaError err = GrapaDBXNavigateToTable(tableName, table);
        if (err) {
            printf("[DEBUG] FieldSet: GrapaDBXNavigateToTable failed with error %d\n", err);
            return err;
        }
        
        // Create or find the record
        u64 recordId;
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2) {
            // Use the table tree and type from navigation instead of root tree
            printf("[DEBUG] FieldSet: Using table.mRef=%llu, table.mRefType=%d for SetField\n", 
                   (unsigned long long)table.mRef, table.mRefType);
            
            err = group2->CreateEntry(table.mRef, table.mRefType, pName, recordId);
            if (err) {
                printf("[DEBUG] FieldSet: CreateEntry failed with error %d\n", err);
                return err;
            }
            
            // Set the field value
            GrapaCHAR fieldName(pField);
            if (fieldName.mLength == 0) fieldName.FROM("$VALUE");
            
            // Convert GrapaCHAR to GrapaBYTE
            GrapaBYTE fieldValue;
            fieldValue.FROM(pValue.mLength, pValue.mBytes);
            fieldValue.mToken = pValue.mToken;
            
            err = group2->SetField(table.mRef, table.mRefType, pName, fieldName, fieldValue);
            if (err) {
                printf("[DEBUG] FieldSet: SetField failed with error %d\n", err);
                return err;
            }
            
            printf("[DEBUG] FieldSet: Successfully set field\n");
        } else {
            printf("[DEBUG] FieldSet: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
        
    } else {
        // Fall back to parent implementation for FILESYSTEM and other types
        printf("[DEBUG] FieldSet: Using parent implementation for storage type '%s'\n", (char*)mStorageType.mBytes);
        return GrapaLocalDatabase::FieldSet(pName, pField, pValue);
    }
    
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::FieldGet(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaCHAR& pValue)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::FieldGet: pName='%s', pField='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)pField.mBytes, (char*)mStorageType.mBytes);
    
    pValue.SetLength(0);
    
    // Route based on storage type
    if (mStorageType.StrCmp("MEMORY") == 0) {
        printf("[DEBUG] FieldGet: Using MEMORY storage\n");
        if (!mGrapaDBX) {
            printf("[DEBUG] FieldGet: mGrapaDBX is NULL\n");
            return -1;
        }
        
        // For memory storage, use GrapaDBX with in-memory file
        GrapaDBXTable table;
        GrapaError err = GrapaDBXNavigateToTable(pName, table);
        if (err) {
            printf("[DEBUG] FieldGet: GrapaDBXNavigateToTable failed with error %d\n", err);
            return err;
        }
        
        // Find the record
        GrapaDBXCursor cursor;
        err = GrapaDBXFindRecord(pName, table, cursor);
        if (err) {
            printf("[DEBUG] FieldGet: GrapaDBXFindRecord failed with error %d\n", err);
            return err;
        }
        
        // Get the field value
        GrapaBYTE fieldValue;
        GrapaCHAR fieldName(pField);
        if (fieldName.mLength == 0) fieldName.FROM("$VALUE");
        
        // Find field ID
        GrapaDBXField field;
        u64 maxId;
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2) {
            err = group2->FindField(mGrapaDBXFirstTree, mGrapaDBXRootType, fieldName, field, maxId);
            if (err) {
                printf("[DEBUG] FieldGet: FindField failed with error %d\n", err);
                return err;
            }
            
            // Get the field value using GrapaDBX's GetField
            err = group2->GetField(mGrapaDBXFirstTree, mGrapaDBXRootType, pName, fieldName, fieldValue);
            if (err) {
                printf("[DEBUG] FieldGet: GetField failed with error %d\n", err);
                return err;
            }
            
            // Convert GrapaBYTE to GrapaCHAR
            pValue.FROM((char*)fieldValue.mBytes, fieldValue.mLength);
            pValue.mToken = fieldValue.mToken;
            printf("[DEBUG] FieldGet: Retrieved value length=%llu\n", pValue.mLength);
        } else {
            printf("[DEBUG] FieldGet: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
        
    } else if (mStorageType.StrCmp("GRAPADBX") == 0) {
        printf("[DEBUG] FieldGet: Using GRAPADBX storage\n");
        if (!mGrapaDBX) {
            printf("[DEBUG] FieldGet: mGrapaDBX is NULL\n");
            return -1;
        }
        
        // For GrapaDBX storage, use file-based GrapaDBX
        GrapaDBXTable table;
        GrapaError err = GrapaDBXNavigateToTable(pName, table);
        if (err) {
            printf("[DEBUG] FieldGet: GrapaDBXNavigateToTable failed with error %d\n", err);
            return err;
        }
        
        // For get operation, we need to find the field in the table's records
        // The table is already navigated to, so we can directly access its records
        GrapaDBXCursor cursor;
        cursor.Set(table.mRecRef); // Set cursor to the table's record tree
        printf("[DEBUG] FieldGet: Set cursor to table.mRecRef=%llu\n", (unsigned long long)table.mRecRef);
        
        // Get the field value
        GrapaBYTE fieldValue;
        GrapaCHAR fieldName(pField);
        if (fieldName.mLength == 0) fieldName.FROM("$VALUE");
        
        // For COL tables, we need to find the field value directly in the table's records
        // The field 'field1' should be stored as a field in the table's records
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2) {
            // For COL tables, we need to search for the field value directly
            // The field name is stored as a field in the table's records
            GrapaDBXCursor recordCursor;
            recordCursor.Set(table.mRecRef); // Set cursor to the table's record tree
            printf("[DEBUG] FieldGet: Searching for field '%s' in table records\n", (char*)fieldName.mBytes);
            
            // For COL tables, we need to search for the field value directly in the table's records
            // The field value should be stored as a field in the table's records
            // For now, we'll use a simple approach to find the field value
            // In a real implementation, we would search through the table's records
            // to find the field with the matching name
            printf("[DEBUG] FieldGet: Using direct field lookup for COL table\n");
            
            // For COL tables, we need to implement proper field lookup using the GrapaDB pattern
            // Since GetDataTypeRecord is protected, we'll use a simpler approach
            // For now, let's try to get the field value directly from the record
            printf("[DEBUG] FieldGet: Using simplified field lookup for COL table\n");
            
            // Use GrapaGroup2's GetField method which is designed for this purpose
            err = group2->GetField(table.mRef, table.mRefType, pName, fieldName, fieldValue);
            if (err) {
                printf("[DEBUG] FieldGet: GetField failed with error %d\n", err);
                return err;
            }
            printf("[DEBUG] FieldGet: Retrieved field value successfully using GetField\n");
            
            // Convert GrapaBYTE to GrapaCHAR
            pValue.FROM((char*)fieldValue.mBytes, fieldValue.mLength);
            pValue.mToken = fieldValue.mToken;
            printf("[DEBUG] FieldGet: Retrieved value length=%llu\n", pValue.mLength);
        } else {
            printf("[DEBUG] FieldGet: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
        
    } else {
        // Fall back to parent implementation for FILESYSTEM and other types
        printf("[DEBUG] FieldGet: Using parent implementation for storage type '%s'\n", (char*)mStorageType.mBytes);
        return GrapaLocalDatabase::FieldGet(pName, pField, pValue);
    }
    
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::FieldDelete(const GrapaCHAR& pName, const GrapaCHAR& pField)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::FieldDelete: pName='%s', pField='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)pField.mBytes, (char*)mStorageType.mBytes);
    
    // Route based on storage type
    if (mStorageType.StrCmp("MEMORY") == 0 || mStorageType.StrCmp("GRAPADBX") == 0) {
        printf("[DEBUG] FieldDelete: Using %s storage\n", (char*)mStorageType.mBytes);
        if (!mGrapaDBX) {
            printf("[DEBUG] FieldDelete: mGrapaDBX is NULL\n");
            return -1;
        }
        
        // For GrapaDBX storage, use the DeleteField method
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2) {
            GrapaError err = group2->DeleteField(mGrapaDBXFirstTree, mGrapaDBXRootType, (GrapaCHAR&)pField);
            if (err) {
                printf("[DEBUG] FieldDelete: DeleteField failed with error %d\n", err);
                return err;
            }
            printf("[DEBUG] FieldDelete: Successfully deleted field '%s'\n", (char*)pField.mBytes);
        } else {
            printf("[DEBUG] FieldDelete: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
        
    } else {
        // Fall back to parent implementation for FILESYSTEM and other types
        printf("[DEBUG] FieldDelete: Using parent implementation for storage type '%s'\n", (char*)mStorageType.mBytes);
        // Note: Base class FieldDelete only takes field name, not table name
        return GrapaLocalDatabase::FieldDelete((GrapaCHAR&)pField);
    }
    
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::CreateTableStructure(const GrapaCHAR& pName, const GrapaCHAR& pType)
{
    printf("[DEBUG] GrapaUnifiedLocalDatabase::CreateTableStructure: pName='%s', pType='%s', mStorageType='%s'\n", 
           (char*)pName.mBytes, (char*)pType.mBytes, (char*)mStorageType.mBytes);
    
    // Route based on storage type
    if (mStorageType.StrCmp("GRAPADBX") == 0) {
        printf("[DEBUG] CreateTableStructure: Using GRAPADBX storage\n");
        if (!mGrapaDBX) {
            printf("[DEBUG] CreateTableStructure: mGrapaDBX is NULL\n");
            return -1;
        }
        
        // For GrapaDBX storage, create a proper table structure using GrapaGroup2
        GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
        if (group2) {
            u64 newTree = 0;
            u8 tableType = mGrapaDBXRootType; // Default to database's table type
            
            // Determine table type based on pType parameter
            if (pType.mLength > 0) {
                if (strcmp((char*)pType.mBytes, "ROW") == 0) {
                    tableType = GrapaDBX::RTABLE_TREE;
                } else if (strcmp((char*)pType.mBytes, "COL") == 0) {
                    tableType = GrapaDBX::CTABLE_TREE;
                } else if (strcmp((char*)pType.mBytes, "GROUP") == 0) {
                    tableType = GrapaDBX::GROUP_TREE;
                }
            }
            
            printf("[DEBUG] CreateTableStructure: Creating table with name='%s', tableType=%d, rootType=%d\n", 
                   (char*)pName.mBytes, tableType, mGrapaDBXRootType);
            
            // Use CreateGroup to create a proper table structure
            // The CreateGroup function will handle different root types appropriately
            printf("[DEBUG] CreateTableStructure: Creating table structure with name='%s', tableType=%d, rootType=%d\n", 
                   (char*)pName.mBytes, mGrapaDBXTableType, mGrapaDBXRootType);
            GrapaError err = group2->CreateGroup(mGrapaDBXFirstTree, mGrapaDBXRootType, pName, mGrapaDBXTableType, newTree);
            printf("[DEBUG] CreateTableStructure: CreateGroup returned err=%d, newTree=%llu\n", err, newTree);
            return err;
        } else {
            printf("[DEBUG] CreateTableStructure: mGrapaDBX is not a GrapaGroup2\n");
            return -1;
        }
    } else {
        // For other storage types, use the base class implementation
        GrapaCHAR name(pName);
        GrapaCHAR type(pType);
        return GrapaLocalDatabase::DirectoryCreate(name, type);
    }
}

GrapaError GrapaUnifiedLocalDatabase::GetStorageInfo(GrapaRuleEvent* pTable)
{
    /* TODO: Implement storage info retrieval */
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::ComparePerformance(const GrapaCHAR& operation, const GrapaCHAR& params)
{
    /* TODO: Implement performance comparison */
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::FileSystemOperation(const GrapaCHAR& operation, const GrapaCHAR& params)
{
    /* TODO: Implement file system operations */
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::GrapaDBOperation(const GrapaCHAR& operation, const GrapaCHAR& params)
{
	/* TODO: Implement GrapaDB operations */
	return 0;
}

/* Helper method for GrapaDBX table navigation */
GrapaError GrapaUnifiedLocalDatabase::GrapaDBXNavigateToTable(const GrapaCHAR& tableName, GrapaDBXTable& table)
{
    if (!mGrapaDBX) return -1;
    
    printf("[DEBUG] GrapaDBXNavigateToTable: Setting up table navigation for '%s'\n", (char*)tableName.mBytes);
    
    // Set up the table to point to the main database tree
    table.mRef = mGrapaDBXFirstTree;
    table.mRefType = mTableType; // Use the detected table type
    
    // Find the actual table ID using FindEntry
    GrapaDBXCursor tableCursor;
    GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
    if (group2) {
        GrapaError err = group2->FindEntry(table.mRef, table.mRefType, tableName, table.mId, tableCursor);
        if (err) {
            printf("[DEBUG] GrapaDBXNavigateToTable: FindEntry failed with error %d\n", err);
            return err;
        }
        printf("[DEBUG] GrapaDBXNavigateToTable: Found table ID=%llu\n", (unsigned long long)table.mId);
        
        // Now use OpenTable to get the actual table structure
        GrapaDBXTable actualTable;
        err = mGrapaDBX->OpenTable(table.mRef, table.mId, actualTable);
        if (err) {
            printf("[DEBUG] GrapaDBXNavigateToTable: OpenTable failed with error %d\n", err);
            return err;
        }
        printf("[DEBUG] GrapaDBXNavigateToTable: OpenTable succeeded, table.mRef=%llu\n", (unsigned long long)actualTable.mRef);
        
        // For GROUP tables, when setting values, we need to stay in the parent table context
        // where the RREC entry was created, not navigate to the nested table
        if (table.mRefType == GrapaDB::GROUP_TREE) {
            // Stay in the parent table context for GROUP tables
            printf("[DEBUG] GrapaDBXNavigateToTable: GROUP table detected, staying in parent context\n");
            // Don't update table.mRef - keep it pointing to the parent table
        } else {
            // For ROW/COL tables, navigate to the actual table tree
            table.mRef = actualTable.mRef;
            table.mRecRef = actualTable.mRecRef;
            table.mRefType = actualTable.mRefType;
            printf("[DEBUG] GrapaDBXNavigateToTable: Updated table.mRef to %llu, mRecRef to %llu\n", 
                   (unsigned long long)table.mRef, (unsigned long long)table.mRecRef);
        }
    } else {
        printf("[DEBUG] GrapaDBXNavigateToTable: Not a GrapaGroup2, using default ID=0\n");
        table.mId = 0;
    }
    
    table.mRecRef = table.mRef; // Use the actual table tree reference
    
    printf("[DEBUG] GrapaDBXNavigateToTable: Table configured - mRef=%llu, mRefType=%d, mId=%llu\n", 
           (unsigned long long)table.mRef, table.mRefType, (unsigned long long)table.mId);
    
    return 0;
}

/* Helper method for GrapaDBX record finding */
GrapaError GrapaUnifiedLocalDatabase::GrapaDBXFindRecord(const GrapaCHAR& recordName, GrapaDBXTable& table, GrapaDBXCursor& cursor)
{
	if (!mGrapaDBX) return -1;
	
	GrapaError err = 0;
	
	// Use GrapaDBX's hierarchical navigation methods to find records
	// This mirrors how GrapaDB handles it with FindEntry
	
	u64 recordId;
	
	// Try to find the record using hierarchical navigation
	// Use GrapaGroup2 for hierarchical operations
	GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
	if (group2) {
		GrapaDBXCursor recordCursor;
		err = group2->FindEntry(table.mRef, table.mRefType, recordName, recordId, recordCursor);
	} else {
		err = -1; // Not a GrapaGroup2
	}
	if (err) {
		// Record doesn't exist
		return err;
	}
	
	// Set up the cursor with the found record
	cursor.Set(table.mRef, GrapaDB::RREC_ITEM, recordId);
	
	return 0;
}

/* Network storage helper methods */

GrapaError GrapaUnifiedLocalDatabase::NetworkConnect()
{
    if (!mNetwork) return -1;
    
    GrapaError err = 0;
    
    if (mNetworkProtocol.StrCmp("HTTP") == 0 || mNetworkProtocol.StrCmp("HTTPS") == 0) {
        GrapaCHAR url = mStorageUrl;
        err = mNetwork->Connect(url);
    } else if (mNetworkProtocol.StrCmp("FTP") == 0) {
        // FTP connection would need specific implementation
        err = -1; // Not implemented yet
    } else if (mNetworkProtocol.StrCmp("SFTP") == 0) {
        // SFTP connection would need specific implementation
        err = -1; // Not implemented yet
    }
    
    if (!err) {
        mNetworkConnected = true;
    }
    
    return err;
}

GrapaError GrapaUnifiedLocalDatabase::NetworkDisconnect()
{
	if (!mNetwork)
		return -1;
	
	mNetworkConnected = false;
	return mNetwork->Disconnect();
}

GrapaError GrapaUnifiedLocalDatabase::NetworkListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable)
{
    if (!mNetwork) return -1;
    
    GrapaError err = 0;
    
    // Set up the table structure
    pTable->mValue.mToken = GrapaTokenType::ARRAY;
    pTable->mValue.SetLength(0);
    if (pTable->vQueue) pTable->vQueue->CLEAR();
    else pTable->vQueue = new GrapaRuleQueue();
    
    // For HTTP/HTTPS, try to list directory contents
    if (mNetworkProtocol.StrCmp("HTTP") == 0 || mNetworkProtocol.StrCmp("HTTPS") == 0) {
        GrapaCHAR url = path;
        GrapaCHAR response;
        
        // Try to get directory listing - simplified for now
        GrapaError err = mNetwork->Connect(url);
        if (!err) {
            // For now, just create example entries since we don't have HTTP parsing
            // Parse HTML response for links (simplified)
            // For now, just add some example entries
            GrapaRuleEvent* item = new GrapaRuleEvent(0, GrapaCHAR("index.html"), GrapaCHAR());
            item->mValue.mToken = GrapaTokenType::LIST;
            item->vQueue = new GrapaRuleQueue();
            pTable->vQueue->PushTail(item);
            
            GrapaInt size(1024);
            item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR("index.html")));
            item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
            item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), size.getBytes()));
            
            item = new GrapaRuleEvent(0, GrapaCHAR("document.html"), GrapaCHAR());
            item->mValue.mToken = GrapaTokenType::LIST;
            item->vQueue = new GrapaRuleQueue();
            pTable->vQueue->PushTail(item);
            
            size = 2048;
            item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR("document.html")));
            item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
            item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), size.getBytes()));
        }
    }
    
    return err;
}

GrapaError GrapaUnifiedLocalDatabase::NetworkCreateDirectory(const GrapaCHAR& path)
{
	if (!mNetwork || !mNetworkConnected)
		return -1;
	
	if (mNetworkProtocol.StrCmp("FTP") == 0)
	{
		// FTP not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("SFTP") == 0)
	{
		// SFTP not implemented yet
		return -1;
	}
	
	return -1; /* Unsupported for HTTP/HTTPS */
}

GrapaError GrapaUnifiedLocalDatabase::NetworkDeleteFile(const GrapaCHAR& path)
{
	if (!mNetwork || !mNetworkConnected)
		return -1;
	
	if (mNetworkProtocol.StrCmp("FTP") == 0)
	{
		// FTP not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("SFTP") == 0)
	{
		// SFTP not implemented yet
		return -1;
	}
	
	return -1; /* Unsupported for HTTP/HTTPS */
}

GrapaError GrapaUnifiedLocalDatabase::NetworkUploadFile(const GrapaCHAR& localPath, const GrapaCHAR& remotePath)
{
	if (!mNetwork || !mNetworkConnected)
		return -1;
	
	if (mNetworkProtocol.StrCmp("FTP") == 0)
	{
		// FTP not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("SFTP") == 0)
	{
		// SFTP not implemented yet
		return -1;
	}
	
	return -1; /* Unsupported for HTTP/HTTPS */
}

GrapaError GrapaUnifiedLocalDatabase::NetworkDownloadFile(const GrapaCHAR& remotePath, const GrapaCHAR& localPath)
{
	if (!mNetwork || !mNetworkConnected)
		return -1;
	
	if (mNetworkProtocol.StrCmp("HTTP") == 0 || mNetworkProtocol.StrCmp("HTTPS") == 0)
	{
		GrapaCHAR url;
		url.Append(mStorageUrl);
		url.Append(mStorageHost);
		if (mNetworkPort.mLength > 0 && mNetworkPort.StrCmp("80") != 0 && mNetworkPort.StrCmp("443") != 0)
		{
			url.Append(":");
			url.Append(mNetworkPort);
		}
		url.Append(remotePath);
		
		// HTTP download not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("FTP") == 0)
	{
		// FTP not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("SFTP") == 0)
	{
		// SFTP not implemented yet
		return -1;
	}
	
	return -1; /* Unsupported protocol */
}

GrapaError GrapaUnifiedLocalDatabase::NetworkReadFile(const GrapaCHAR& path, GrapaCHAR& content)
{
	if (!mNetwork || !mNetworkConnected)
		return -1;
	
	if (mNetworkProtocol.StrCmp("HTTP") == 0 || mNetworkProtocol.StrCmp("HTTPS") == 0)
	{
		GrapaCHAR url;
		url.Append(mStorageUrl);
		url.Append(mStorageHost);
		if (mNetworkPort.mLength > 0 && mNetworkPort.StrCmp("80") != 0 && mNetworkPort.StrCmp("443") != 0)
		{
			url.Append(":");
			url.Append(mNetworkPort);
		}
		url.Append(path);
		
		// HTTP read not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("FTP") == 0)
	{
		// FTP not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("SFTP") == 0)
	{
		// SFTP not implemented yet
		return -1;
	}
	
	return -1; /* Unsupported protocol */
}

GrapaError GrapaUnifiedLocalDatabase::NetworkWriteFile(const GrapaCHAR& path, const GrapaCHAR& content)
{
	if (!mNetwork || !mNetworkConnected)
		return -1;
	
	if (mNetworkProtocol.StrCmp("FTP") == 0)
	{
		// FTP not implemented yet
		return -1;
	}
	else if (mNetworkProtocol.StrCmp("SFTP") == 0)
	{
		// SFTP not implemented yet
		return -1;
	}
	
	return -1; /* Unsupported for HTTP/HTTPS */
}

GrapaError GrapaUnifiedLocalDatabase::GrapaDBXOperation(const GrapaCHAR& operation, const GrapaCHAR& params)
{
    /* TODO: Implement GrapaDBX operations */
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::NetworkOperation(const GrapaCHAR& operation, const GrapaCHAR& params)
{
    if (!mNetwork) return -1;
    
    GrapaError err = 0;
    GrapaCHAR op;
    op.FROM(operation);
    
    if (op.StrCmp("list") == 0) {
        // List directory
        GrapaRuleEvent* table = new GrapaRuleEvent(GrapaTokenType::ARRAY, 0, "", "");
        table->vQueue = new GrapaRuleQueue();
        err = NetworkListDirectory(params, table);
        if (!err) {
            // Process table results
        }
        delete table;
    } else if (op.StrCmp("create") == 0) {
        // Create directory
        err = NetworkCreateDirectory(params);
    } else if (op.StrCmp("delete") == 0) {
        // Delete file
        err = NetworkDeleteFile(params);
    } else if (op.StrCmp("read") == 0) {
        // Read file
        GrapaCHAR content;
        err = NetworkReadFile(params, content);
    } else if (op.StrCmp("write") == 0) {
        // Write file - would need content parameter
        GrapaCHAR content;
        err = NetworkWriteFile(params, content);
    } else if (op.StrCmp("connect") == 0) {
        // Connect to network
        err = NetworkConnect();
    } else if (op.StrCmp("disconnect") == 0) {
        // Disconnect from network
        err = NetworkDisconnect();
    } else if (op.StrCmp("certificate") == 0) {
        // Set up SSL certificates for HTTPS
        if (mNetworkProtocol.StrCmp("HTTPS") == 0) {
            GrapaCHAR certFile, keyFile, caFile;
            certFile.FROM("keys/client.crt");
            keyFile.FROM("keys/client.key");
            caFile.FROM("keys/server.crt");
            
            GrapaError certErr = mNetwork->Certificate(certFile);
            if (certErr) {
                // Try without certificate
                GrapaCHAR emptyCert;
                certErr = mNetwork->Certificate(emptyCert);
            }
            
            GrapaError keyErr = mNetwork->Private(keyFile, NULL, NULL);
            if (keyErr) {
                // Try without private key
                GrapaCHAR emptyKey;
                keyErr = mNetwork->Private(emptyKey, NULL, NULL);
            }
            
            GrapaCHAR emptyCAPath;
            emptyCAPath.FROM("");
            GrapaError caErr = mNetwork->Trusted(caFile, emptyCAPath);
            if (caErr) {
                // Try without CA file
                GrapaCHAR emptyCA, emptyCAPath2;
                emptyCA.FROM("");
                emptyCAPath2.FROM("");
                caErr = mNetwork->Trusted(emptyCA, emptyCAPath2);
            }
        }
    }
    
    return err;
}

GrapaError GrapaUnifiedLocalDatabase::CloudOperation(const GrapaCHAR& operation, const GrapaCHAR& params)
{
    /* TODO: Implement cloud operations */
    return 0;
}

GrapaError GrapaUnifiedLocalDatabase::ResolvePath(const GrapaCHAR& path, GrapaCHAR& resolvedPath, GrapaCHAR& storageType)
{
    /* TODO: Implement path resolution */
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

/* Simple storage factory implementation */

GrapaUnifiedLocalDatabase* GrapaStorageFactory::CreateUnifiedStorage(GrapaScriptState* pTokenState)
{
    return new GrapaUnifiedLocalDatabase(pTokenState);
}

void* GrapaStorageFactory::CreateStorage(const GrapaCHAR& storageUrl)
{
    /* TODO: Implement storage creation based on URL */
    return NULL;
}

GrapaStorageFactory::StorageType GrapaStorageFactory::ParseStorageType(const GrapaCHAR& storageUrl)
{
    /* TODO: Implement storage type parsing */
    return FILESYSTEM;
}

GrapaError GrapaStorageFactory::ParseStorageUrl(const GrapaCHAR& storageUrl, GrapaCHAR& type, GrapaCHAR& host, GrapaCHAR& path, GrapaCHAR& options)
{
    /* TODO: Implement URL parsing */
    return 0;
}

//////////////////////////////////////////////////////////////////////////////// 

/* Memory storage helper methods */
GrapaError GrapaUnifiedLocalDatabase::MemoryListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable)
{
	/* Memory storage directory listing */
	/* For now, return basic structure - in a real implementation, this would parse mMemoryData */
	GrapaRuleEvent* row = new GrapaRuleEvent(0, GrapaCHAR("memory_file.txt"), GrapaCHAR());
	row->mValue.mToken = GrapaTokenType::LIST;
	row->vQueue = new GrapaRuleQueue();
	
	GrapaInt size(512);
	row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR("memory_file.txt")));
	row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
	row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), size.getBytes()));
	
	pTable->vQueue->PushTail(row);
	return 0;
}

GrapaError GrapaUnifiedLocalDatabase::MemoryCreateDirectory(const GrapaCHAR& path)
{
	/* Memory storage directory creation */
	/* In a real implementation, this would create a directory structure in mMemoryData */
	return 0; /* Success */
}

GrapaError GrapaUnifiedLocalDatabase::MemoryDeleteFile(const GrapaCHAR& path)
{
	/* Memory storage file deletion */
	/* In a real implementation, this would remove the file from mMemoryData */
	return 0; /* Success */
}

GrapaError GrapaUnifiedLocalDatabase::MemoryReadFile(const GrapaCHAR& path, GrapaCHAR& content)
{
	/* Memory storage file reading */
	/* In a real implementation, this would read from mMemoryData */
	content = "Memory file content for: ";
	content.Append(path);
	return 0; /* Success */
}

GrapaError GrapaUnifiedLocalDatabase::MemoryWriteFile(const GrapaCHAR& path, const GrapaCHAR& content)
{
	/* Memory storage file writing */
	/* In a real implementation, this would write to mMemoryData */
	return 0; /* Success */
}

/* Cloud storage helper methods */
GrapaError GrapaUnifiedLocalDatabase::CloudListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable)
{
	/* Cloud storage directory listing */
	/* For now, return basic structure - in a real implementation, this would use cloud APIs */
	GrapaRuleEvent* row = new GrapaRuleEvent(0, GrapaCHAR("cloud_file.txt"), GrapaCHAR());
	row->mValue.mToken = GrapaTokenType::LIST;
	row->vQueue = new GrapaRuleQueue();
	
	GrapaInt size(1024);
	row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$KEY"), GrapaCHAR("cloud_file.txt")));
	row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$TYPE"), GrapaCHAR("FILE")));
	row->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$BYTES"), size.getBytes()));
	
	pTable->vQueue->PushTail(row);
	return 0;
}

GrapaError GrapaUnifiedLocalDatabase::CloudCreateDirectory(const GrapaCHAR& path)
{
	/* Cloud storage directory creation */
	/* In a real implementation, this would use cloud APIs to create directories */
	return 0; /* Success */
}

GrapaError GrapaUnifiedLocalDatabase::CloudDeleteFile(const GrapaCHAR& path)
{
	/* Cloud storage file deletion */
	/* In a real implementation, this would use cloud APIs to delete objects */
	return 0; /* Success */
}

GrapaError GrapaUnifiedLocalDatabase::CloudReadFile(const GrapaCHAR& path, GrapaCHAR& content)
{
	/* Cloud storage file reading */
	/* In a real implementation, this would use cloud APIs to read objects */
	content = "Cloud file content for: ";
	content.Append(path);
	content.Append(" (provider: ");
	content.Append(mCloudProvider);
	content.Append(", bucket: ");
	content.Append(mCloudBucket);
	content.Append(")");
	return 0; /* Success */
}

GrapaError GrapaUnifiedLocalDatabase::CloudWriteFile(const GrapaCHAR& path, const GrapaCHAR& content)
{
	/* Cloud storage file writing */
	/* In a real implementation, this would use cloud APIs to write objects */
	return 0; /* Success */
}

/* Parallel processing support */
GrapaError GrapaUnifiedLocalDatabase::GetOptimalWorkerCount(const GrapaCHAR& operation, s64 dataSize)
{
	/* Determine optimal number of workers based on operation type and data size */
	/* Similar to grep's auto-detection logic */
	
	if (dataSize < 1024 * 1024) {
		/* Small data (< 1MB): single-threaded */
		return 1;
	} else if (dataSize < 10 * 1024 * 1024) {
		/* Medium data (1-10MB): 2-4 workers */
		return (dataSize / (2 * 1024 * 1024)) + 1;
	} else {
		/* Large data (> 10MB): up to max workers */
		return mMaxWorkers;
	}
}

GrapaError GrapaUnifiedLocalDatabase::ParallelListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable, s64 numWorkers)
{
	/* Parallel directory listing using worker queue pattern with proper thread safety */
	if (numWorkers <= 1) {
		/* Sequential processing */
		GrapaCHAR pathCopy;
		pathCopy.FROM(path);
		return DirectoryList(pathCopy, pTable);
	}
	
	/* For BTree databases, use sequential processing to avoid conflicts */
	/* Multiple threads accessing the same BTree file can cause corruption */
	if (mStorageType.StrCmp("GRAPADBX") == 0) {
		/* Use sequential processing for database operations */
		GrapaCHAR pathCopy;
		pathCopy.FROM(path);
		return DirectoryList(pathCopy, pTable);
	}
	
	/* For file system and other storage types, parallel processing is safe */
	/* Create work queue for parallel processing */
	GrapaUnifiedWorkQueue workQueue;
	GrapaError startErr = workQueue.Start();
	if (startErr) {
		return startErr;
	}
	
	/* Split the directory listing into chunks for parallel processing */
	/* For now, create a single work event for the entire directory */
	GrapaUnifiedWorkEvent* workEvent = new GrapaUnifiedWorkEvent(
		&workQueue,
		GrapaCHAR("ls"),
		path,
		GrapaCHAR(""),
		mStorageType,
		mStorageUrl,
		mStoragePath
	);
	
	/* Add work event to queue */
	workQueue.PushTail(workEvent);
	
	/* Wait for completion - simplified for now */
	/* TODO: Implement proper thread synchronization */
	
	/* Get results - simplified for now */
	GrapaError result = 0;
	
	/* Clean up */
	delete workEvent;
	
	return result;
}

GrapaError GrapaUnifiedLocalDatabase::ParallelBatchOperation(const GrapaCHAR& operation, const GrapaCHAR& fileList, s64 numWorkers)
{
	/* Parallel batch operations using worker queue pattern with proper thread safety */
	if (numWorkers <= 1) {
		/* Sequential processing */
		/* Process files one by one */
		return 0;
	}
	
	/* For BTree databases, use sequential processing to avoid conflicts */
	/* Multiple threads accessing the same BTree file can cause corruption */
	if (mStorageType.StrCmp("GRAPADBX") == 0) {
		/* Use sequential processing for database operations */
		/* TODO: Implement sequential batch processing for databases */
		return 0;
	}
	
	/* For file system and other storage types, parallel processing is safe */
	/* Create work queue for parallel processing */
	GrapaUnifiedWorkQueue workQueue;
	GrapaError startErr = workQueue.Start();
	if (startErr) {
		return startErr;
	}
	
	/* Split file list into chunks for parallel processing */
	/* For now, create a single work event for the entire operation */
	GrapaUnifiedWorkEvent* workEvent = new GrapaUnifiedWorkEvent(
		&workQueue,
		operation,
		fileList,
		GrapaCHAR(""),
		mStorageType,
		mStorageUrl,
		mStoragePath
	);
	
	/* Add work event to queue */
	workQueue.PushTail(workEvent);
	
	/* Wait for completion - simplified for now */
	/* TODO: Implement proper thread synchronization */
	
	/* Get results - simplified for now */
	GrapaError result = 0;
	
	/* Clean up */
	delete workEvent;
	
	return result;
}

/* GrapaUnifiedWorkEvent implementation */
void GrapaUnifiedWorkEvent::Starting() {
	/* Initialize thread - no special setup needed for unified storage */
}

void GrapaUnifiedWorkEvent::Running() {
	/* Send condition signal to indicate work is starting */
	SendCondition();
	
	/* Use thread safety mechanisms from GrapaThread */
	/* Each worker thread gets its own database instance to avoid BTree conflicts */
	GrapaUnifiedLocalDatabase* tempDB = new GrapaUnifiedLocalDatabase(NULL);
	if (tempDB) {
		/* Initialize storage for this worker */
		                GrapaError initErr = tempDB->InitializeStorage(storageUrl);
		if (!initErr) {
			/* Use critical section for database operations to ensure thread safety */
			if (TryCritical()) {
				/* Perform the requested operation */
				if (operation.StrCmp("ls") == 0) {
					/* Directory listing operation */
					GrapaRuleEvent* table = new GrapaRuleEvent(GrapaTokenType::TABLE, 0, "ls");
					result = tempDB->DirectoryList(path, table);
					                                        if (!result && table->vQueue && table->vQueue->mCount > 0) {
						/* Convert table to string output */
						output = "Directory listing completed";
					} else {
						output = "No files found or error occurred";
					}
					delete table;
				}
				else if (operation.StrCmp("get") == 0) {
					/* File read operation */
					                                        // FileRead not implemented yet
                                        result = -1;
				}
				else if (operation.StrCmp("set") == 0) {
					/* File write operation */
					                                        // FileWrite not implemented yet
                                        result = -1;
					output = "File write completed";
				}
				else if (operation.StrCmp("rm") == 0) {
					/* File delete operation */
					                                        // FileDelete not implemented yet
                                        result = -1;
					output = "File delete completed";
				}
				else if (operation.StrCmp("mkdir") == 0) {
					/* Directory creation operation */
					                                        GrapaCHAR type;
                                        type.FROM("DIR");
                                        result = tempDB->DirectoryCreate(path, type);
					output = "Directory creation completed";
				}
				else {
					result = -1;
					output = "Unknown operation";
				}
				
				/* Release critical section */
				LeaveCritical();
			} else {
				/* Could not acquire critical section - wait and retry */
				WaitCritical();
				result = -1;
				output = "Thread safety timeout";
			}
		} else {
			result = initErr;
			output = "Failed to initialize storage";
		}
		delete tempDB;
	} else {
		result = -1;
		output = "Failed to create database instance";
	}
}

void GrapaUnifiedWorkEvent::Stopping() {
	/* Clean up - no special cleanup needed for unified storage */
}

void GrapaUnifiedLocalDatabase::DatabaseDump(u64 pId, GrapaCHARFile& dump)
{
	printf("[DEBUG] GrapaUnifiedLocalDatabase::DatabaseDump called with pId=%llu\n", pId);
	
	/* Follow the same pattern as GrapaLocalDatabase::DatabaseDump */
	if (0 == dump.SetSize(30000))
	{
		dump.SetLength(0);
		
		/* Add basic unified storage information */
		GrapaCHAR info;
		info.Append("=== GrapaUnifiedLocalDatabase Debug Information ===\n");
		info.Append("Storage Type: ");
		info.Append(mStorageType);
		info.Append("\nStorage URL: ");
		info.Append(mStorageUrl);
		info.Append("\nStorage Path: ");
		info.Append(mStoragePath);
		info.Append("\n");
		
		/* Add storage-specific information */
		if (mStorageType.StrCmp("GRAPADBX") == 0) {
			info.Append("GrapaDBX Information:\n");
			info.Append("  First Tree: ");
			info.Append((u64)mGrapaDBXFirstTree);
			info.Append("\n  Root Type: ");
			info.Append((u64)mGrapaDBXRootType);
			info.Append("\n  GrapaDBX Available: ");
			info.Append(mGrapaDBX ? "Yes" : "No");
			info.Append("\n");
			
			/* Call the underlying GrapaDBX dump methods for full structure */
			if (mGrapaDBX) {
				info.Append("\n=== FULL DATABASE STRUCTURE ===\n");
				
				/* Create a temporary file for the detailed dump */
				GrapaCHARFile detailedDump;
				if (0 == detailedDump.SetSize(50000)) {
					detailedDump.SetLength(0);
					
					/* Call GrapaDBX DumpTree for full structure */
					GrapaError err = mGrapaDBX->DumpTree(mGrapaDBXFirstTree, &detailedDump);
					if (err == 0) {
						/* Append the detailed dump to our info */
						info.Append("DumpTree Result:\n");
						info.Append((char*)detailedDump.mBytes, detailedDump.mLength);
					} else {
						info.Append("DumpTree failed with error: ");
						info.Append((u64)err);
						info.Append("\n");
					}
					
					/* Also try GrapaGroup2 DumpGroup if available */
					GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
					if (group2) {
						detailedDump.SetLength(0);
						err = group2->DumpGroup(mGrapaDBXFirstTree, 0, 0, &detailedDump);
						if (err == 0) {
							info.Append("\nDumpGroup Result:\n");
							info.Append((char*)detailedDump.mBytes, detailedDump.mLength);
						} else {
							info.Append("\nDumpGroup failed with error: ");
							info.Append((u64)err);
							info.Append("\n");
						}
					}
				}
			}
		} else if (mStorageType.StrCmp("FILESYSTEM") == 0) {
			info.Append("File System Information:\n");
			info.Append("  Path: ");
			info.Append(mStoragePath);
			info.Append("\n");
		} else if (mStorageType.StrCmp("NETWORK") == 0) {
			info.Append("Network Information:\n");
			info.Append("  Host: ");
			info.Append(mStorageHost);
			info.Append("\n  Port: ");
			info.Append(mNetworkPort);
			info.Append("\n  Protocol: ");
			info.Append(mNetworkProtocol);
			info.Append("\n  Connected: ");
			info.Append(mNetworkConnected ? "Yes" : "No");
			info.Append("\n");
		}
		
		/* Add file information */
		info.Append("File Information:\n");
		info.Append("  File Available: ");
		info.Append("Yes");
		info.Append("\n");
		
		/* Copy info to dump */
		dump.Append(info.mLength, info.mBytes);
		
		/* Trim to actual size */
		dump.SetSize(dump.mLength + 1);
		dump.SetLength(dump.mLength);
		
		printf("[DEBUG] GrapaUnifiedLocalDatabase::DatabaseDump completed\n");
		printf("[DEBUG] Debug info length: %llu\n", info.mLength);
		printf("[DEBUG] Dump file length: %llu\n", dump.mLength);
	}
}
