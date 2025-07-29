// GrapaDatabaseTemplate.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaDatabaseTemplate_
#define _GrapaDatabaseTemplate_

#include "GrapaBtree.h"
#include "GrapaFile.h"
#include "GrapaDatabase.h"
#include "GrapaState.h"
#include "GrapaDBX.h"
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
    GrapaCHAR mStorageType;      /* "FILESYSTEM", "GRAPADBX", "NETWORK", "MEMORY", "CLOUD" */
    GrapaCHAR mStorageUrl;       /* Full storage URL */
    GrapaCHAR mStorageHost;      /* Host/server for network/cloud */
    GrapaCHAR mStoragePath;      /* Path within storage */
    GrapaCHAR mStorageOptions;   /* Additional options */
        
    GrapaGroup2* mGrapaDBX;      /* GrapaGroup2 instance for database operations */
    u64 mGrapaDBXFirstTree;      /* First tree reference for GrapaDBX */
    u8 mGrapaDBXRootType;        /* Root tree type for GrapaDBX */
    u8 mTableType;               /* Table type for unified operations (GROUP_TREE, RTABLE_TREE, CTABLE_TREE) */
    u8 mGrapaDBXTableType;       /* Table type for tables created within GrapaDBX groups */
    
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
    				            GrapaUnifiedLocalDatabase(GrapaScriptState* pTokenState) : GrapaLocalDatabase(pTokenState), mGrapaDBXFirstTree(0), mGrapaDBXRootType(0), mGrapaDBXTableType(0), mGrapaDBX(NULL), mNetwork(NULL), mNetworkConnected(false), mMaxWorkers(16) {}
        virtual ~GrapaUnifiedLocalDatabase() {
            if (mGrapaDBX) { mGrapaDBX->CloseFile(); delete mGrapaDBX; mGrapaDBX = NULL; }
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
	virtual GrapaError DirectoryList(GrapaCHAR& pName, GrapaRuleEvent* pTable) override;
    virtual GrapaError FieldSet(const GrapaCHAR& pName, const GrapaCHAR& pField, const GrapaCHAR& pValue) override;
    virtual GrapaError FieldGet(const GrapaCHAR& pName, const GrapaCHAR& pField, GrapaCHAR& pValue) override;
    virtual GrapaError FieldDelete(const GrapaCHAR& pName, const GrapaCHAR& pField);
    virtual GrapaError CreateTableStructure(const GrapaCHAR& pName, const GrapaCHAR& pType);
    
    /* Storage type information */
    virtual GrapaCHAR GetStorageType() { return mStorageType; }
    virtual GrapaCHAR GetStorageUrl() { return mStorageUrl; }
    virtual GrapaError GetStorageInfo(GrapaRuleEvent* pTable);
    virtual void SetTableType(u8 tableType) { mTableType = tableType; }
    virtual u8 GetTableType() { return mTableType; }
    
    virtual GrapaGroup2* GetGrapaDBX() { return mGrapaDBX; }
    virtual u64 GetGrapaDBXFirstTree() { return mGrapaDBXFirstTree; }
    virtual u8 GetGrapaDBXRootType() { return mGrapaDBXRootType; }
    
    /* Network storage accessors */
    virtual GrapaNet* GetNetwork() { return mNetwork; }
    virtual bool IsNetworkConnected() { return mNetworkConnected; }
    virtual GrapaCHAR GetNetworkProtocol() { return mNetworkProtocol; }
    virtual GrapaCHAR GetNetworkHost() { return mStorageHost; }
    virtual GrapaCHAR GetNetworkPort() { return mNetworkPort; }
    
    /* Performance comparison */
    virtual GrapaError ComparePerformance(const GrapaCHAR& operation, const GrapaCHAR& params);
        
    /* GrapaDBX-specific helper methods - moved to public for library access */
    	        virtual GrapaError GrapaDBXNavigateToTable(const GrapaCHAR& tableName, GrapaDBXTable& table);
        virtual GrapaError GrapaDBXFindRecord(const GrapaCHAR& recordName, GrapaDBXTable& table, GrapaDBXCursor& cursor);
    
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
    
    /* Debug and diagnostic methods */
    virtual void DatabaseDump(u64 pId, GrapaCHARFile& dump) override;
    
protected:
    /* Storage type-specific implementations */
    virtual GrapaError FileSystemOperation(const GrapaCHAR& operation, const GrapaCHAR& params);
    virtual GrapaError GrapaDBOperation(const GrapaCHAR& operation, const GrapaCHAR& params);
    virtual GrapaError GrapaDBXOperation(const GrapaCHAR& operation, const GrapaCHAR& params);
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
        GRAPADBX = 1,
        NETWORK = 2,
        MEMORY = 3,
        CLOUD = 4
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
