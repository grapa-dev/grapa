// GrapaModel.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaModel_
#define _GrapaModel_

#include "GrapaFloat.h"
#include "GrapaState.h"
#include "llama.h"  // LLAMA.cpp includes

// Forward declarations
class GrapaRuleEvent;

class GrapaModel : public GrapaCritical
{
public:
    // Model state
    bool mLoaded;
    GrapaCHAR mModelPath;
    GrapaCHAR mBackend;  // "llama", "onnx", etc.
    
    // LLAMA.cpp specific
    struct llama_model* mLlamaModel;
    struct llama_context* mLlamaContext;
    struct llama_model_params mLlamaModelParams;
    struct llama_context_params mLlamaContextParams;
    struct llama_sampler* mLlamaSampler;  // Sampler chain for temperature-aware generation
    
    // Generation parameters
    s32 mMaxTokens;
    float mTemperature;
    s32 mTopK;
    float mTopP;
    float mRepeatPenalty;
    s32 mSeed;
    s32 mContextSize;
    s32 mVerbose;     // Logging verbosity level (0=silent, 1=errors, 2=warnings, 3=info, 4=debug)
    
    // Performance optimization buffers
    GrapaCHAR mTempToken;        // Reuse token buffer
    char mTokenBuffer[256];      // Reuse token string buffer
    
    // Persistent parameter management
    GrapaScriptExec* vScriptExec;
    GrapaNames* vNameSpace;
    GrapaRuleEvent* vParams;  // Grapa object of current parameters
    
public:
    GrapaModel(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pParams=NULL);
    ~GrapaModel();
    void SetRuleEvent(GrapaRuleEvent* pParams);
    void INIT(GrapaRuleEvent* pParams);
    void CLEAR();
    
    // Core model operations
    GrapaError Load(const GrapaCHAR& modelPath, const GrapaCHAR& backend);
    GrapaError Unload();
    bool IsLoaded() const;
    
    // Generation operations
    GrapaError Generate(const GrapaCHAR& prompt, GrapaCHAR& result, GrapaRuleEvent* callParams);
    GrapaError GenerateStream(const GrapaCHAR& prompt, GrapaCHAR& result, GrapaRuleEvent* callParams);
    
    // Model information
    GrapaRuleEvent* GetModelInfo() const;
    
    // Parameter management
    GrapaError SetParams(GrapaRuleEvent* params);
    GrapaRuleEvent* GetParams() const;
    
    // Backend-specific operations
    GrapaError LoadLlama(const GrapaCHAR& modelPath);
    GrapaError UnloadLlama();
    GrapaError GenerateLlama(const GrapaCHAR& prompt, GrapaCHAR& result, GrapaRuleEvent* mergedParams);
    
    // Future backends
    GrapaError LoadOnnx(const GrapaCHAR& modelPath);
    GrapaError LoadTensorFlow(const GrapaCHAR& modelPath);
    
private:
    GrapaError InitializeLlama();
    GrapaError InitializeSampler();  // Initialize LLAMA.cpp sampler chain
    void CleanupSampler();           // Clean up sampler resources
    
    // Parameter management
    void ResetModelSpecificParams();
    void SetModelDefaults();
    GrapaRuleEvent* MergeParams(GrapaRuleEvent* persistent, GrapaRuleEvent* callSpecific);
    GrapaError ApplyParamsToLlama(GrapaRuleEvent* params);
    int GetModelSize();  // Estimate model size for parameter defaults
    
    // Logging callback
    static void LogCallback(enum ggml_log_level level, const char * text, void * user_data);
};

#endif //_GrapaModel_

////////////////////////////////////////////////////////////////////////////////
