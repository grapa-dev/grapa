// GrapaModel.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaModel_
#define _GrapaModel_

#include "GrapaFloat.h"
#include "GrapaState.h"
#include "llama.h"  // LLAMA.cpp includes

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
    
    // Generation parameters
    s32 mMaxTokens;
    f32 mTemperature;
    s32 mTopK;
    f32 mTopP;
    f32 mRepeatPenalty;
    s32 mSeed;
    s32 mContextSize;
    
    // Persistent parameter management
    GrapaCHAR mPersistentParams;  // JSON string of current parameters
    bool mUserParamsSet;          // Whether user has explicitly set parameters
    
public:
    GrapaModel();
    GrapaModel(const GrapaModel& that);
    ~GrapaModel();
    GrapaModel& operator=(const GrapaModel& that);
    
    // Core model operations
    GrapaError Load(const GrapaCHAR& modelPath, const GrapaCHAR& backend = "llama");
    GrapaError Unload();
    bool IsLoaded() const;
    
    // Generation operations
    GrapaError Generate(const GrapaCHAR& prompt, GrapaCHAR& result, const GrapaCHAR& callParams = "");
    GrapaError GenerateStream(const GrapaCHAR& prompt, GrapaCHAR& result, const GrapaCHAR& callParams = "");
    
    // Model information
    GrapaCHAR GetModelInfo() const;
    GrapaCHAR GetBackend() const;
    GrapaCHAR GetModelPath() const;
    
    // Parameter management
    GrapaError SetParams(const GrapaCHAR& params);
    GrapaCHAR GetParams() const;
    
    // Backend-specific operations
    GrapaError LoadLlama(const GrapaCHAR& modelPath);
    GrapaError UnloadLlama();
    GrapaError GenerateLlama(const GrapaCHAR& prompt, GrapaCHAR& result, const GrapaCHAR& mergedParams = "");
    
    // Future backends
    GrapaError LoadOnnx(const GrapaCHAR& modelPath);
    GrapaError LoadTensorFlow(const GrapaCHAR& modelPath);
    
private:
    void INIT();
    void CLEAR();
    GrapaError ParseParams(const GrapaCHAR& params);
    GrapaError InitializeLlama();
    
    // Parameter management
    void ResetModelSpecificParams();
    void SetModelDefaults();
    GrapaCHAR MergeParams(const GrapaCHAR& persistent, const GrapaCHAR& callSpecific);
    GrapaError ApplyParamsToLlama(const GrapaCHAR& params);
    int GetModelSize();  // Estimate model size for parameter defaults
};

#endif //_GrapaModel_

////////////////////////////////////////////////////////////////////////////////
