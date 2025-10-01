// GrapaModel.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaModel_
#define _GrapaModel_

#include "GrapaFloat.h"
#include "GrapaState.h"
#include "GrapaNetConnect.h"
#include "llama.h"  // LLAMA.cpp includes
#include <vector>   // For std::vector<llama_token>

// Forward declarations
class GrapaRuleEvent;

class GrapaModel : public GrapaCritical
{
public:
    // Model state
    bool mLoaded;
    GrapaCHAR mModelPath;
    GrapaCHAR mMethod;  // "llama-local", etc.
    
    // LLAMA.cpp specific
    struct llama_model* mLlamaModel;
    struct llama_context* mLlamaContext;
    struct llama_model_params mLlamaModelParams;
    struct llama_context_params mLlamaContextParams;
    struct llama_sampler* mLlamaSampler;  // Sampler chain for temperature-aware generation
    
    // Backend-optimized context management
    bool mContextPreserved;  // Whether context is preserved between calls
    std::vector<llama_token> mContextTokens;  // LLAMA.cpp: Persistent token context
    GrapaCHAR mContextHistory;  // Generic: Text-based conversation history
    
    // LLAMA.cpp persistent context optimization
    bool mContextInitialized;  // Whether the context has been initialized with tokens
    
    // OpenAI context management
    GrapaCHAR mOpenAIResponseId;  // OpenAI: Response ID for context continuity
    
    // ONNX Runtime specific
    void* mOnnxSession;  // ONNX Runtime session
    void* mOnnxEnv;      // ONNX Runtime environment
    
    // Generation parameters
    s32 mMaxTokens;
    float mTemperature;
    s32 mTopK;
    float mTopP;
    float mRepeatPenalty;
    s32 mSeed;
    s32 mContextSize;
    s32 mVerbose;     // Logging verbosity level (0=silent, 1=errors, 2=warnings, 3=info, 4=debug)
    
    // Persistent parameter management
    GrapaScriptExec* vScriptExec;
    GrapaNames* vNameSpace;
    GrapaRuleEvent* vParams;  // Grapa object of current parameters

    // Network for cloud models
    GrapaNetConnect mNet;
    
public:
    GrapaModel(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pParams=NULL);
    ~GrapaModel();
    void SetRuleEvent(GrapaRuleEvent* pParams);
    void INIT(GrapaRuleEvent* pParams);
    void CLEAR();
    
    // Core model operations
    GrapaError Load(const GrapaCHAR& modelPath, const GrapaCHAR& method);
    GrapaError Load(const GrapaCHAR& modelPath);  // Auto-detect method
    GrapaError Unload();
    bool IsLoaded() const;
    
    // Generation operations
    GrapaRuleEvent* Generate(const GrapaCHAR& prompt, GrapaRuleEvent* callParams);
    
    // Model information
    GrapaRuleEvent* GetModelInfo();
    
    // Parameter management
    GrapaError SetParams(GrapaRuleEvent* params);
    GrapaRuleEvent* GetParams() const;
    
    // Context management
    GrapaRuleEvent* GetContext() const;  // Returns $GOBJ with text, tokens, method, model
    GrapaError SetContext(GrapaRuleEvent* context);  // Accepts $GOBJ, $LIST, or $STR
    GrapaError SetContextFromText(const GrapaCHAR& text);  // Helper for text-based context
    
    // Backend-specific operations
    GrapaError LoadLlama(const GrapaCHAR& modelPath);
    GrapaError UnloadLlama();
    GrapaRuleEvent* GenerateLlama(const GrapaCHAR& prompt, GrapaRuleEvent* mergedParams);
    
    // OpenAI cloud model operations
    GrapaError LoadOpenAI(const GrapaCHAR& modelPath);
    GrapaError UnloadOpenAI();
    GrapaRuleEvent* GenerateOpenAI(const GrapaCHAR& prompt, GrapaRuleEvent* mergedParams);
    GrapaRuleEvent* EmbedOpenAI(const GrapaCHAR& text, GrapaRuleEvent* mergedParams);
    
    /*
    // ONNX Runtime operations
    GrapaError LoadOnnx(const GrapaCHAR& modelPath);
    GrapaError UnloadOnnx();
    GrapaRuleEvent* GenerateOnnx(const GrapaCHAR& prompt, GrapaRuleEvent* mergedParams);
    GrapaRuleEvent* EmbedOnnx(const GrapaCHAR& text, GrapaRuleEvent* mergedParams);
    */
    
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
    
    // Auto-detection
    GrapaCHAR AutoDetectMethod(const GrapaCHAR& modelPath);
    
    // Logging callback
    static void LogCallback(enum ggml_log_level level, const char * text, void * user_data);
};

#endif //_GrapaModel_

////////////////////////////////////////////////////////////////////////////////
