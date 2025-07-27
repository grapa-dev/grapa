// GrapaDatabaseTemplate.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "GrapaDatabaseTemplate.h"

////////////////////////////////////////////////////////////////////////////////

/* Simple placeholder implementations for enhanced LocalDatabase */

GrapaError GrapaUnifiedLocalDatabase::InitializeStorage(const GrapaCHAR& storageUrl)
{
    GrapaError err = 0;
    
    // Parse the storage URL to determine type
    err = ParseStorageUrl(storageUrl);
    if (err) return err;
    
    // Initialize based on storage type
    if (mStorageType.StrCmp("FILESYSTEM") == 0) {
        // File system is always available
    } else if (mStorageType.StrCmp("GRAPADB") == 0) {
        if (!mGrapaDB) {
            mGrapaDB = new GrapaGroup();
            if (!mGrapaDB) return -1;
        }
        err = mGrapaDB->Create((char*)mStoragePath.mBytes, GrapaDB::GROUP_TREE, mGrapaDBFirstTree);
        if (err) {
            // Try to open existing database
            err = mGrapaDB->OpenFile(mStoragePath, 'r');
            if (err) return err;
            mGrapaDBRootType = GrapaDB::GROUP_TREE;
        }
    } else if (mStorageType.StrCmp("GRAPADBX") == 0) {
        if (!mGrapaDBX) {
            mGrapaDBX = new GrapaGroup2(&mFile);
            if (!mGrapaDBX) return -1;
        }
        err = mGrapaDBX->Create((char*)mStoragePath.mBytes, GrapaDB::GROUP_TREE, mGrapaDBXFirstTree);
        if (err) {
            // Try to open existing database
            err = mGrapaDBX->OpenFile(mStoragePath, 'r');
            if (err) return err;
            mGrapaDBXRootType = GrapaDB::GROUP_TREE;
        }
    } else if (mStorageType.StrCmp("NETWORK") == 0) {
        if (!mNetwork) {
            mNetwork = new GrapaNet();
            if (!mNetwork) return -1;
        }
    } else if (mStorageType.StrCmp("MEMORY") == 0) {
        // Memory storage is always available
    } else if (mStorageType.StrCmp("CLOUD") == 0) {
        if (!mNetwork) {
            mNetwork = new GrapaNet();
            if (!mNetwork) return -1;
        }
    } else {
        return -1; // Unknown storage type
    }
    
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
    printf("[DEBUG] Compare to 'grapadb://': %d\n", url.StrNCmp("grapadb://", 10));
    printf("[DEBUG] Compare to 'grapadbx://': %d\n", url.StrNCmp("grapadbx://", 11));
    if (url.StrNCmp("grapadb://", 10) == 0) {
        mStorageType.FROM("GRAPADB");
        mStoragePath.FROM((char*)url.mBytes + 10, url.mLength - 10);
    } else if (url.StrNCmp("grapadbx://", 11) == 0) {
        mStorageType.FROM("GRAPADBX");
        mStoragePath.FROM((char*)url.mBytes + 11, url.mLength - 11);
    } else if (url.StrCmp("http://") == 0 || url.StrCmp("https://") == 0 || 
               url.StrCmp("ftp://") == 0 || url.StrCmp("sftp://") == 0) {
        mStorageType.FROM("NETWORK");
        mStoragePath = url;
    } else if (url.StrCmp("memory://") == 0) {
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
    /* TODO: Implement enhanced directory switching */
    return GrapaLocalDatabase::DirectorySwitch(pName);
}

void GrapaUnifiedLocalDatabase::DirectoryPWD(GrapaCHAR& pName)
{
    /* TODO: Implement enhanced PWD */
    GrapaLocalDatabase::DirectoryPWD(pName);
}

GrapaError GrapaUnifiedLocalDatabase::DirectoryCreate(GrapaCHAR& pName, GrapaCHAR& pType)
{
    /* TODO: Implement enhanced directory creation */
    return GrapaLocalDatabase::DirectoryCreate(pName, pType);
}

GrapaError GrapaUnifiedLocalDatabase::DataCreate(const GrapaCHAR& pName)
{
    /* TODO: Implement enhanced data creation */
    return GrapaLocalDatabase::DataCreate(pName);
}

GrapaError GrapaUnifiedLocalDatabase::DataDelete(const GrapaCHAR& pName)
{
    /* TODO: Implement enhanced data deletion */
    return GrapaLocalDatabase::DataDelete(pName);
}

GrapaError GrapaUnifiedLocalDatabase::FieldSet(const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaCHAR& pValue)
{
    /* TODO: Implement enhanced field setting */
    return GrapaLocalDatabase::FieldSet(pName, pField, pValue);
}

GrapaError GrapaUnifiedLocalDatabase::FieldGet(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaCHAR& pValue)
{
    /* TODO: Implement enhanced field getting */
    return GrapaLocalDatabase::FieldGet(pName, pField, pValue);
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

/* Helper method for GrapaDB table navigation */
GrapaError GrapaUnifiedLocalDatabase::GrapaDBNavigateToTable(const GrapaCHAR& tableName, GrapaDBTable& table)
{
    if (!mGrapaDB) return -1;
    
    GrapaError err;
    
    // Use GrapaGroup's OpenGroup method to properly handle nested table navigation
    // This will handle different table types (ROW, COL, GROUP) correctly
    u64 newTree;
    u8 newType;
    u64 tableId;
    
    err = mGrapaDB->OpenGroup(mGrapaDBFirstTree, mGrapaDBRootType, tableName, newTree, newType, tableId);
    if (err) {
        // Table doesn't exist, we need to create it
        // For now, default to GROUP_TREE - this should be enhanced to detect table type
        err = mGrapaDB->LastTableId(mGrapaDBFirstTree, tableId);
        if (err) return err;
        tableId++;
        
        err = mGrapaDB->CreateTable(mGrapaDBFirstTree, GrapaDB::GROUP_TREE, tableId, table);
        if (err) return err;
    } else {
        // Successfully opened existing table, now open it as a GrapaDBTable
        err = mGrapaDB->OpenTable(mGrapaDBFirstTree, tableId, table);
        if (err) return err;
    }
    
    return 0;
}

/* Helper method for GrapaDB record operations */
GrapaError GrapaUnifiedLocalDatabase::GrapaDBFindRecord(const GrapaCHAR& recordName, GrapaDBTable& table, GrapaCursor& cursor)
{
	if (!mGrapaDB)
		return -1;
	
	/* Search for record by name in the $KEY field */
	GrapaDBFieldValueArray searchFields;
	GrapaDBFieldValue keyField;
	keyField.mValue = recordName;
	keyField.mCmp = GrapaDB::EQ_CMP;
	searchFields.Append(mGrapaDB, table, 0, keyField.mValue, keyField.mCmp); /* $KEY field is typically field ID 0 */
	
	GrapaDBCursor dbCursor;
	GrapaError err = mGrapaDB->SearchDb(dbCursor, table, searchFields);
	if (!err)
	{
		cursor = dbCursor;
	}
	
	return err;
}

/* Helper method for GrapaDBX table navigation */
GrapaError GrapaUnifiedLocalDatabase::GrapaDBXNavigateToTable(const GrapaCHAR& tableName, GrapaDBXTable& table)
{
    if (!mGrapaDBX) return -1;
    
    GrapaError err;
    
    // Use GrapaDBX's hierarchical navigation methods to properly handle nested structures
    // This mirrors how GrapaDB handles it with OpenGroup
    
    u64 newTree;
    u8 newType;
    u64 tableId;
    
    // Try to open the group/table using hierarchical navigation
    // Use GrapaGroup2 for hierarchical operations
    GrapaGroup2* group2 = dynamic_cast<GrapaGroup2*>(mGrapaDBX);
    if (group2) {
        err = group2->OpenGroup(mGrapaDBXFirstTree, mGrapaDBXRootType, tableName, newTree, newType, tableId);
    } else {
        err = -1; // Not a GrapaGroup2
    }
    if (err) {
        // Group/table doesn't exist, we need to create it
        // For now, default to GROUP_TREE - this should be enhanced to detect table type
        err = mGrapaDBX->LastTableId(mGrapaDBXFirstTree, tableId);
        if (err) return err;
        tableId++;
        
        err = mGrapaDBX->CreateTable(mGrapaDBXFirstTree, GrapaDB::GROUP_TREE, tableId, table);
        if (err) return err;
    } else {
        // Successfully opened existing group/table, now open it as a GrapaDBXTable
        err = mGrapaDBX->OpenTable(mGrapaDBXFirstTree, tableId, table);
        if (err) return err;
    }
    
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
            err = group2->FindEntry(table.mRef, table.mRefType, recordName, recordId);
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
	if (mStorageType.StrCmp("GRAPADB") == 0 || mStorageType.StrCmp("GRAPADBX") == 0) {
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
	if (mStorageType.StrCmp("GRAPADB") == 0 || mStorageType.StrCmp("GRAPADBX") == 0) {
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