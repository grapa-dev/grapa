// GrapaDatabaseTemplate.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaDatabaseTemplate_
#define _GrapaDatabaseTemplate_

#include "GrapaBtree.h"
#include "GrapaFile.h"
#include "GrapaDatabase.h"
#include "GrapaState.h"
#include "GrapaDB.h"
#include "GrapaDB2.h"
#include "GrapaNet.h"

/* Forward declarations */
class GrapaWorkQueue;
class GrapaWorkEvent;

/* GrapaUnifiedWorkEvent class for parallel unified storage operations using WorkQueue pattern */
class GrapaUnifiedWorkEvent : public GrapaWorkEvent
{
public:
	/* Input data */
	GrapaCHAR operation;      /* Operation type: "ls", "get", "set", "rm", etc. */
	GrapaCHAR path;           /* Path to operate on */
	GrapaCHAR data;           /* Data for set operations */
	GrapaCHAR storageType;    /* Storage type: "FILESYSTEM", "GRAPADB", etc. */
	GrapaCHAR storageUrl;     /* Storage URL */
	GrapaCHAR storagePath;    /* Storage path */
	
	/* Output data */
	GrapaError result;        /* Operation result */
	GrapaCHAR output;         /* Operation output */
	
	/* Constructor */
	GrapaUnifiedWorkEvent(GrapaWorkQueue* vQueue,
						  const GrapaCHAR& op,
						  const GrapaCHAR& p,
						  const GrapaCHAR& d,
						  const GrapaCHAR& type,
						  const GrapaCHAR& url,
						  const GrapaCHAR& spath)
		: operation(op), path(p), data(d), storageType(type), storageUrl(url), storagePath(spath), result(-1)
	{
		Set(vQueue);
	}
	
	/* Thread lifecycle methods (override from GrapaWorkEvent) */
	virtual void Starting() override;
	virtual void Running() override;
	virtual void Stopping() override;
};

/* GrapaUnifiedWorkQueue class for managing unified storage work events */
class GrapaUnifiedWorkQueue : public GrapaWorkQueue
{
public:
	virtual ~GrapaUnifiedWorkQueue() {
		GrapaUnifiedWorkQueue::CLEAR();
	}
	virtual void CLEAR()
	{
		GrapaUnifiedWorkEvent* e = (GrapaUnifiedWorkEvent*)PopHead();
		while (e)
		{
			e->CLEAR();
			delete e;
			e = (GrapaUnifiedWorkEvent*)PopHead();
		}
	}
};

class GrapaUnifiedLocalDatabase;

////////////////////////////////////////////////////////////////////////////////

/* Simple enhanced LocalDatabase for unified storage */
class GrapaUnifiedLocalDatabase : public GrapaLocalDatabase
{
private:
    GrapaCHAR mStorageType;      /* "FILESYSTEM", "GRAPADB", "GRAPADB2", "NETWORK", "MEMORY", "CLOUD" */
    GrapaCHAR mStorageUrl;       /* Full storage URL */
    GrapaCHAR mStorageHost;      /* Host/server for network/cloud */
    GrapaCHAR mStoragePath;      /* Path within storage */
    GrapaCHAR mStorageOptions;   /* Additional options */
    
    /* Storage-specific instances */
    	GrapaGroup* mGrapaDB;        /* GrapaGroup instance for database operations */
    u64 mGrapaDBFirstTree;       /* First tree reference for GrapaDB */
    u8 mGrapaDBRootType;         /* Root tree type for GrapaDB */
    
    GrapaGroup2* mGrapaDB2;      /* GrapaGroup2 instance for database operations */
            u64 mGrapaDB2FirstTree;      /* First tree reference for GrapaDB2 */
        u8 mGrapaDB2RootType;        /* Root tree type for GrapaDB2 */
    
    				/* Network storage members */
				GrapaNet* mNetwork;          /* Network connection for remote storage */
				GrapaCHAR mNetworkProtocol;  /* "HTTP", "HTTPS", "FTP", "SFTP" */
				GrapaCHAR mNetworkPort;      /* Port number for network connection */
				GrapaCHAR mNetworkAuth;      /* Authentication credentials */
				bool mNetworkConnected;      /* Connection status */
				
				/* Memory storage members */
				GrapaCHAR mMemoryData;       /* In-memory data storage */
				GrapaCHAR mMemoryPath;       /* Current path in memory storage */
				
				/* Cloud storage members */
				GrapaCHAR mCloudProvider;    /* "S3", "GS", "AZURE" */
				GrapaCHAR mCloudBucket;      /* Cloud storage bucket/container */
				GrapaCHAR mCloudRegion;      /* Cloud storage region */
				GrapaCHAR mCloudCredentials; /* Cloud storage credentials */
				
				/* Parallel processing members */
				s64 mMaxWorkers;             /* Maximum number of workers for parallel operations */
				
				/* Thread safety - using existing GrapaThread mechanisms */
    
public:
    				            GrapaUnifiedLocalDatabase(GrapaScriptState* pTokenState) : GrapaLocalDatabase(pTokenState), mGrapaDB(NULL), mGrapaDBFirstTree(0), mGrapaDBRootType(0), mGrapaDB2(NULL), mGrapaDB2FirstTree(0), mGrapaDB2RootType(0), mNetwork(NULL), mNetworkConnected(false), mMaxWorkers(16) {}
        virtual ~GrapaUnifiedLocalDatabase() {
            if (mGrapaDB) { mGrapaDB->CloseFile(); delete mGrapaDB; mGrapaDB = NULL; }
            if (mGrapaDB2) { mGrapaDB2->CloseFile(); delete mGrapaDB2; mGrapaDB2 = NULL; }
            if (mNetwork) { delete mNetwork; mNetwork = NULL; }
        }
    
    /* Storage type detection and initialization */
    virtual GrapaError InitializeStorage(const GrapaCHAR& storageUrl);
    virtual GrapaError ParseStorageUrl(const GrapaCHAR& storageUrl);
    virtual GrapaError SwitchStorageType(const GrapaCHAR& newStorageType);
    
    /* Enhanced navigation that spans storage types */
    virtual GrapaError DirectorySwitchSingle(GrapaCHAR& pName) override;
    virtual GrapaError DirectorySwitch(const GrapaCHAR& pName) override;
    virtual void DirectoryPWD(GrapaCHAR& pName) override;
    
    /* Enhanced operations that work across storage types */
    virtual GrapaError DirectoryCreate(GrapaCHAR& pName, GrapaCHAR& pType) override;
    virtual GrapaError DataCreate(const GrapaCHAR& pName) override;
    virtual GrapaError DataDelete(const GrapaCHAR& pName) override;
    virtual GrapaError FieldSet(const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaCHAR& pValue) override;
    virtual GrapaError FieldGet(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaCHAR& pValue) override;
    
    /* Storage type information */
    virtual GrapaCHAR GetStorageType() { return mStorageType; }
    virtual GrapaCHAR GetStorageUrl() { return mStorageUrl; }
    virtual GrapaError GetStorageInfo(GrapaRuleEvent* pTable);
    
    /* Storage-specific accessors */
    	virtual GrapaGroup* GetGrapaDB() { return mGrapaDB; }
    virtual u64 GetGrapaDBFirstTree() { return mGrapaDBFirstTree; }
    virtual u8 GetGrapaDBRootType() { return mGrapaDBRootType; }
    
    virtual GrapaGroup2* GetGrapaDB2() { return mGrapaDB2; }
    virtual u64 GetGrapaDB2FirstTree() { return mGrapaDB2FirstTree; }
    virtual u8 GetGrapaDB2RootType() { return mGrapaDB2RootType; }
    
    /* Network storage accessors */
    virtual GrapaNet* GetNetwork() { return mNetwork; }
    virtual bool IsNetworkConnected() { return mNetworkConnected; }
    virtual GrapaCHAR GetNetworkProtocol() { return mNetworkProtocol; }
    virtual GrapaCHAR GetNetworkHost() { return mStorageHost; }
    virtual GrapaCHAR GetNetworkPort() { return mNetworkPort; }
    
    /* Performance comparison */
    virtual GrapaError ComparePerformance(const GrapaCHAR& operation, const GrapaCHAR& params);
    
    /* GrapaDB-specific helper methods - moved to public for library access */
    virtual GrapaError GrapaDBNavigateToTable(const GrapaCHAR& tableName, GrapaDBTable& table);
    virtual GrapaError GrapaDBFindRecord(const GrapaCHAR& recordName, GrapaDBTable& table, GrapaCursor& cursor);
    
    /* GrapaDB2-specific helper methods - moved to public for library access */
    	virtual GrapaError GrapaDB2NavigateToTable(const GrapaCHAR& tableName, GrapaDB2Table& table);
	virtual GrapaError GrapaDB2FindRecord(const GrapaCHAR& recordName, GrapaDB2Table& table, GrapaDB2Cursor& cursor);
    
    /* Parallel processing support - moved to public for library access */
    virtual GrapaError GetOptimalWorkerCount(const GrapaCHAR& operation, s64 dataSize);
    virtual GrapaError ParallelListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable, s64 numWorkers);
    virtual GrapaError ParallelBatchOperation(const GrapaCHAR& operation, const GrapaCHAR& fileList, s64 numWorkers);
    
    /* Network storage helper methods - moved to public for library access */
    virtual GrapaError NetworkConnect();
    virtual GrapaError NetworkDisconnect();
    virtual GrapaError NetworkListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable);
    virtual GrapaError NetworkCreateDirectory(const GrapaCHAR& path);
    virtual GrapaError NetworkDeleteFile(const GrapaCHAR& path);
    virtual GrapaError NetworkUploadFile(const GrapaCHAR& localPath, const GrapaCHAR& remotePath);
    virtual GrapaError NetworkDownloadFile(const GrapaCHAR& remotePath, const GrapaCHAR& localPath);
    virtual GrapaError NetworkReadFile(const GrapaCHAR& path, GrapaCHAR& content);
    virtual GrapaError NetworkWriteFile(const GrapaCHAR& path, const GrapaCHAR& content);
    
    /* Memory storage helper methods - moved to public for library access */
    virtual GrapaError MemoryListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable);
    virtual GrapaError MemoryCreateDirectory(const GrapaCHAR& path);
    virtual GrapaError MemoryDeleteFile(const GrapaCHAR& path);
    virtual GrapaError MemoryReadFile(const GrapaCHAR& path, GrapaCHAR& content);
    virtual GrapaError MemoryWriteFile(const GrapaCHAR& path, const GrapaCHAR& content);
    
protected:
    /* Storage type-specific implementations */
    virtual GrapaError FileSystemOperation(const GrapaCHAR& operation, const GrapaCHAR& params);
    virtual GrapaError GrapaDBOperation(const GrapaCHAR& operation, const GrapaCHAR& params);
    virtual GrapaError GrapaDB2Operation(const GrapaCHAR& operation, const GrapaCHAR& params);
    virtual GrapaError NetworkOperation(const GrapaCHAR& operation, const GrapaCHAR& params);
    virtual GrapaError CloudOperation(const GrapaCHAR& operation, const GrapaCHAR& params);
    
    /* Path resolution across storage types */
    virtual GrapaError ResolvePath(const GrapaCHAR& path, GrapaCHAR& resolvedPath, GrapaCHAR& storageType);
				
				/* Cloud storage helper methods */
				virtual GrapaError CloudListDirectory(const GrapaCHAR& path, GrapaRuleEvent* pTable);
				virtual GrapaError CloudCreateDirectory(const GrapaCHAR& path);
				virtual GrapaError CloudDeleteFile(const GrapaCHAR& path);
				virtual GrapaError CloudReadFile(const GrapaCHAR& path, GrapaCHAR& content);
				virtual GrapaError CloudWriteFile(const GrapaCHAR& path, const GrapaCHAR& content);
				
				/* Thread safety - using existing GrapaThread mechanisms */
				/* No additional methods needed - GrapaWorkEvent inherits from GrapaThread */
};

////////////////////////////////////////////////////////////////////////////////

/* Simple storage factory */
class GrapaStorageFactory
{
public:
    enum StorageType {
        FILESYSTEM = 0,
        GRAPADB = 1,
        GRAPADB2 = 2,
        NETWORK = 3,
        MEMORY = 4,
        CLOUD = 5
    };
    
    /* Factory methods for different storage types */
    static GrapaUnifiedLocalDatabase* CreateUnifiedStorage(GrapaScriptState* pTokenState);
    
    /* Unified factory method that parses storage URLs */
    static void* CreateStorage(const GrapaCHAR& storageUrl);
    
    /* Parse storage URL to determine type */
    static StorageType ParseStorageType(const GrapaCHAR& storageUrl);
    
    /* Extract connection parameters from URL */
    static GrapaError ParseStorageUrl(const GrapaCHAR& storageUrl, GrapaCHAR& type, GrapaCHAR& host, GrapaCHAR& path, GrapaCHAR& options);
};

////////////////////////////////////////////////////////////////////////////////

#endif //_GrapaDatabaseTemplate_ 