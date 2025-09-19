// GrapaModel.cpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#include "GrapaModel.h"

extern GrapaSystem* gSystem;

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaModel::GrapaModel() { INIT(); }
GrapaModel::GrapaModel(const GrapaModel& that) { INIT(); *this = that; }
GrapaModel::~GrapaModel() { CLEAR(); }

GrapaModel& GrapaModel::operator=(const GrapaModel& that)
{
    if (this != &that) {
        CLEAR();
        mLoaded = that.mLoaded;
        mModelPath = that.mModelPath;
        mBackend = that.mBackend;
        mMaxTokens = that.mMaxTokens;
        mTemperature = that.mTemperature;
        mTopK = that.mTopK;
        mTopP = that.mTopP;
        mRepeatPenalty = that.mRepeatPenalty;
        mSeed = that.mSeed;
        mContextSize = that.mContextSize;
        mPersistentParams = that.mPersistentParams;
        mUserParamsSet = that.mUserParamsSet;
        
        // Note: Don't copy LLAMA contexts - they need to be recreated
        if (mLoaded && mBackend.StrCmp("llama") == 0) {
            LoadLlama(mModelPath);
        }
    }
    return *this;
}

void GrapaModel::INIT()
{
    mLoaded = false;
    mLlamaModel = nullptr;
    mLlamaContext = nullptr;
    mMaxTokens = 512;
    mTemperature = 0.7f;
    mTopK = 40;
    mTopP = 0.9f;
    mRepeatPenalty = 1.1f;
    mSeed = -1;
    mContextSize = 0;
    mUserParamsSet = false;
    
    // Initialize LLAMA parameters
    mLlamaModelParams = llama_model_default_params();
    mLlamaContextParams = llama_context_default_params();
}

void GrapaModel::CLEAR()
{
    if (mLoaded) {
        Unload();
    }
    mModelPath.SetNull();
    mBackend.SetNull();
}

GrapaError GrapaModel::Load(const GrapaCHAR& modelPath, const GrapaCHAR& backend)
{
    GrapaError result = 0;
    
    if (mLoaded) {
        Unload();
    }
    
    // Reset model-specific parameters, preserve user preferences
    ResetModelSpecificParams();
    
    mModelPath = modelPath;
    mBackend = backend;
    
    if (backend.StrCmp("llama") == 0) {
        result = LoadLlama(modelPath);
    }
    else if (backend.StrCmp("onnx") == 0) {
        result = LoadOnnx(modelPath);
    }
    else if (backend.StrCmp("tensorflow") == 0) {
        result = LoadTensorFlow(modelPath);
    }
    else {
        result = -1; // Unsupported backend
    }
    
    if (result == 0) {
        mLoaded = true;
        
        // Apply model-appropriate defaults if no user params set
        if (!mUserParamsSet) {
            SetModelDefaults();
        }
    }
    
    return result;
}

GrapaError GrapaModel::Unload()
{
    GrapaError result = 0;
    
    if (mLoaded) {
        if (mBackend.StrCmp("llama") == 0) {
            result = UnloadLlama();
        }
        // Add other backend cleanup here
    }
    
    mLoaded = false;
    return result;
}

bool GrapaModel::IsLoaded() const
{
    return mLoaded;
}

GrapaError GrapaModel::LoadLlama(const GrapaCHAR& modelPath)
{
    GrapaError result = 0;
    
    // Load the model
    mLlamaModel = llama_load_model_from_file(modelPath.mBytes, mLlamaModelParams);
    if (!mLlamaModel) {
        return -1;
    }
    
    // Create context
    mLlamaContext = llama_new_context_with_model(mLlamaModel, mLlamaContextParams);
    if (!mLlamaContext) {
        llama_free_model(mLlamaModel);
        mLlamaModel = nullptr;
        return -2;
    }
    
    return result;
}

GrapaError GrapaModel::UnloadLlama()
{
    if (mLlamaContext) {
        llama_free(mLlamaContext);
        mLlamaContext = nullptr;
    }
    if (mLlamaModel) {
        llama_free_model(mLlamaModel);
        mLlamaModel = nullptr;
    }
    return 0;
}

GrapaError GrapaModel::Generate(const GrapaCHAR& prompt, GrapaCHAR& result, const GrapaCHAR& callParams)
{
    if (!mLoaded) {
        return -1;
    }
    
    // Merge persistent + call-specific parameters
    GrapaCHAR mergedParams = MergeParams(mPersistentParams, callParams);
    
    if (mBackend.StrCmp("llama") == 0) {
        return GenerateLlama(prompt, result, mergedParams);
    }
    
    return -2; // Unsupported backend
}

GrapaError GrapaModel::GenerateLlama(const GrapaCHAR& prompt, GrapaCHAR& result, const GrapaCHAR& mergedParams)
{
    if (!mLlamaContext) {
        return -1;
    }
    
    // Apply parameters to LLAMA context
    GrapaError paramResult = ApplyParamsToLlama(mergedParams);
    if (paramResult != 0) {
        return paramResult;
    }
    
    // Tokenize the prompt
    std::vector<llama_token> tokens_list;
    tokens_list.resize(prompt.mLength + 1);
    int n_tokens = llama_tokenize(mLlamaModel, prompt.mBytes, prompt.mLength, tokens_list.data(), tokens_list.size(), true, false);
    if (n_tokens < 0) {
        tokens_list.resize(-n_tokens);
        n_tokens = llama_tokenize(mLlamaModel, prompt.mBytes, prompt.mLength, tokens_list.data(), tokens_list.size(), true, false);
    }
    tokens_list.resize(n_tokens);
    
    // Generate response
    result.SetNull();
    GrapaCHAR currentToken;
    
    for (int i = 0; i < mMaxTokens; i++) {
        // Evaluate the model
        if (llama_decode(mLlamaContext, llama_batch_get_one(&tokens_list[i], 1, i, 0))) {
            return -2;
        }
        
        // Get next token
        llama_token next_token = llama_sample_token(mLlamaContext, nullptr);
        if (next_token == llama_token_eos(mLlamaModel)) {
            break;
        }
        
        // Convert token to string
        char token_str[256];
        int n_chars = llama_token_to_piece(mLlamaModel, next_token, token_str, sizeof(token_str), false);
        if (n_chars > 0) {
            currentToken.FROM(token_str, n_chars);
            result += currentToken;
        }
        
        tokens_list.push_back(next_token);
    }
    
    return 0;
}

GrapaCHAR GrapaModel::GetModelInfo() const
{
    GrapaCHAR result = "{";
    result += "\"loaded\":";
    result += mLoaded ? "true" : "false";
    result += ",\"backend\":\"";
    result += mBackend;
    result += "\",\"path\":\"";
    result += mModelPath;
    result += "\",\"max_tokens\":";
    result += GrapaInt(mMaxTokens).ToString();
    result += ",\"context_size\":";
    result += GrapaInt(mContextSize).ToString();
    result += ",\"temperature\":";
    result += GrapaFloat(mTemperature).ToString();
    result += ",\"top_k\":";
    result += GrapaInt(mTopK).ToString();
    result += ",\"top_p\":";
    result += GrapaFloat(mTopP).ToString();
    result += ",\"repeat_penalty\":";
    result += GrapaFloat(mRepeatPenalty).ToString();
    result += ",\"seed\":";
    result += GrapaInt(mSeed).ToString();
    result += "}";
    return result;
}

GrapaCHAR GrapaModel::GetBackend() const
{
    return mBackend;
}

GrapaCHAR GrapaModel::GetModelPath() const
{
    return mModelPath;
}

GrapaError GrapaModel::SetParams(const GrapaCHAR& params)
{
    // Parse JSON-like parameters
    GrapaError result = ParseParams(params);
    if (result == 0) {
        mPersistentParams = params;
        mUserParamsSet = true;
    }
    return result;
}

GrapaCHAR GrapaModel::GetParams() const
{
    GrapaCHAR result = "{";
    result += "\"max_tokens\":";
    result += GrapaInt(mMaxTokens).ToString();
    result += ",\"temperature\":";
    result += GrapaFloat(mTemperature).ToString();
    result += ",\"top_k\":";
    result += GrapaInt(mTopK).ToString();
    result += ",\"top_p\":";
    result += GrapaFloat(mTopP).ToString();
    result += ",\"repeat_penalty\":";
    result += GrapaFloat(mRepeatPenalty).ToString();
    result += ",\"seed\":";
    result += GrapaInt(mSeed).ToString();
    result += "}";
    return result;
}

GrapaError GrapaModel::ParseParams(const GrapaCHAR& params)
{
    // Simple parameter parsing - you might want to use a proper JSON parser
    // For now, just set defaults
    return 0;
}

// Future backend implementations
GrapaError GrapaModel::LoadOnnx(const GrapaCHAR& modelPath)
{
    // TODO: Implement ONNX Runtime backend
    return -1;
}

GrapaError GrapaModel::LoadTensorFlow(const GrapaCHAR& modelPath)
{
    // TODO: Implement TensorFlow backend
    return -1;
}

void GrapaModel::ResetModelSpecificParams()
{
    // Reset model-dependent parameters
    mMaxTokens = 512;  // Will be adjusted based on model
    mContextSize = 0;  // Will be set from model
    
    // Preserve user preference parameters
    // mTemperature, mTopK, mTopP, mRepeatPenalty, mSeed stay the same
}

void GrapaModel::SetModelDefaults()
{
    // Set reasonable defaults for this specific model
    if (mBackend.StrCmp("llama") == 0) {
        // Get model info and set appropriate defaults
        int modelSize = GetModelSize();  // 7B, 13B, 70B, etc.
        
        if (modelSize <= 7) {
            mMaxTokens = 2048;
            mTemperature = 0.7f;
        } else if (modelSize <= 13) {
            mMaxTokens = 4096;
            mTemperature = 0.6f;
        } else {
            mMaxTokens = 8192;
            mTemperature = 0.5f;
        }
    }
}

GrapaCHAR GrapaModel::MergeParams(const GrapaCHAR& persistent, const GrapaCHAR& callSpecific)
{
    // Simple merge - call-specific overrides persistent
    // In a full implementation, you'd parse both JSON objects and merge them
    if (callSpecific.mLength > 0) {
        return callSpecific;  // For now, just use call-specific if provided
    }
    return persistent;
}

GrapaError GrapaModel::ApplyParamsToLlama(const GrapaCHAR& params)
{
    // Apply parameters to LLAMA context
    // This would parse the JSON params and apply them to the context
    // For now, just return success
    return 0;
}

int GrapaModel::GetModelSize()
{
    // Estimate model size from filename or model metadata
    // This is a simple heuristic - in practice you'd get this from the model file
    if (mModelPath.StrCmp("7b") >= 0) return 7;
    if (mModelPath.StrCmp("13b") >= 0) return 13;
    if (mModelPath.StrCmp("30b") >= 0) return 30;
    if (mModelPath.StrCmp("70b") >= 0) return 70;
    return 7;  // Default to 7B
}

////////////////////////////////////////////////////////////////////////////////
