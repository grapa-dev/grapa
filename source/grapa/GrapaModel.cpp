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
#include "GrapaSystem.h"
#include "GrapaState.h"

#include <vector>
#include <thread>

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
        mUserParamsSet = that.mUserParamsSet;
        
        // Deep copy persistent params if they exist
        if (that.mPersistentParams) {
            mPersistentParams = new GrapaRuleEvent(*that.mPersistentParams);
        } else {
            mPersistentParams = nullptr;
        }
        
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
    mMaxTokens = 10;  // Very low for testing
    mTemperature = 0.7f;
    mTopK = 40;
    mTopP = 0.9f;
    mRepeatPenalty = 1.1f;
    mSeed = -1;
    mContextSize = 0;
    mPersistentParams = nullptr;
    mUserParamsSet = false;
    
    // Initialize LLAMA parameters
    mLlamaModelParams = llama_model_default_params();
    mLlamaContextParams = llama_context_default_params();
    //mLlamaContextParams.n_threads = std::thread::hardware_concurrency();
    //mLlamaContextParams.n_threads_batch = mLlamaContextParams.n_threads;
}

void GrapaModel::CLEAR()
{
    if (mLoaded) {
        Unload();
    }
    if (mPersistentParams) {
        delete mPersistentParams;
        mPersistentParams = nullptr;
    }
    mModelPath.SetNull();
    mBackend.SetNull();
}

GrapaError GrapaModel::Load(const GrapaCHAR& modelPath, const GrapaCHAR& backend)
{
    GrapaError result = 0;
    
    printf("GrapaModel::Load called\n");
    printf("modelPath: %s\n", (char*)modelPath.mBytes);
    printf("backend: %s\n", (char*)backend.mBytes);
    
    if (mLoaded) {
        Unload();
    }
    
    // Reset model-specific parameters, preserve user preferences
    ResetModelSpecificParams();
    
    mModelPath = modelPath;
    mBackend = backend;
    
    if (mBackend.StrCmp("llama") == 0) {
        printf("Loading llama model...\n");
        result = LoadLlama(modelPath);
        printf("LoadLlama returned: %d\n", result);
    }
    else if (mBackend.StrCmp("onnx") == 0) {
        result = LoadOnnx(modelPath);
    }
    else if (mBackend.StrCmp("tensorflow") == 0) {
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
    
    printf("GrapaModel::LoadLlama called\n");
    printf("Loading model from: %s\n", (char*)modelPath.mBytes);
    
    // Load the model
    mLlamaModel = llama_load_model_from_file((char*)modelPath.mBytes, mLlamaModelParams);
    if (!mLlamaModel) {
        printf("llama_load_model_from_file failed\n");
        return -1;
    }
    printf("Model loaded successfully\n");
    
    // Create context
    mLlamaContext = llama_new_context_with_model(mLlamaModel, mLlamaContextParams);
    if (!mLlamaContext) {
        printf("llama_new_context_with_model failed\n");
        llama_free_model(mLlamaModel);
        mLlamaModel = nullptr;
        return -2;
    }
    printf("Context created successfully\n");
    
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

GrapaError GrapaModel::Generate(const GrapaCHAR& prompt, GrapaCHAR& result, GrapaRuleEvent* callParams)
{
    printf("GrapaModel::Generate called\n");
    printf("mLoaded: %d\n", mLoaded);
    printf("mBackend: %s\n", (char*)mBackend.mBytes);
    
    if (!mLoaded) {
        printf("Model not loaded, returning -1\n");
        return -1;
    }
    
    // Merge persistent + call-specific parameters
    GrapaRuleEvent* mergedParams = MergeParams(mPersistentParams, callParams);
    
    if (mBackend.StrCmp("llama") == 0) {
        printf("Backend is llama, calling GenerateLlama\n");
        GrapaError err = GenerateLlama(prompt, result, mergedParams);
        if (mergedParams && mergedParams != callParams) {
            delete mergedParams;
        }
        printf("GenerateLlama returned: %d\n", err);
        return err;
    }
    
    printf("Backend not llama, returning -2\n");
    if (mergedParams && mergedParams != callParams) {
        delete mergedParams;
    }
    return -2; // Unsupported backend
}

GrapaError GrapaModel::GenerateLlama(const GrapaCHAR& prompt, GrapaCHAR& result, GrapaRuleEvent* mergedParams)
{
    printf("GrapaModel::GenerateLlama called\n");
    printf("mLlamaContext: %p\n", this->mLlamaContext);
    
    if (!this->mLlamaContext) {
        printf("mLlamaContext is null, returning -1\n");
        return -1;
    }
    
    // Apply parameters to LLAMA context
    GrapaError paramResult = this->ApplyParamsToLlama(mergedParams);
    if (paramResult != 0) {
        printf("ApplyParamsToLlama failed: %d\n", paramResult);
        return paramResult;
    }
    
    // Tokenize the prompt
    const struct llama_vocab* vocab = llama_model_get_vocab(this->mLlamaModel);
    std::vector<llama_token> tokens_list;
    tokens_list.resize(prompt.mLength + 1);
    printf("Tokenizing prompt: %s\n", (char*)prompt.mBytes);
    int n_tokens = llama_tokenize(vocab, (char*)prompt.mBytes, (int32_t)prompt.mLength, tokens_list.data(), (int32_t)tokens_list.size(), true, false);
    if (n_tokens < 0) {
        tokens_list.resize(-n_tokens);
        n_tokens = llama_tokenize(vocab, (char*)prompt.mBytes, (int32_t)prompt.mLength, tokens_list.data(), (int32_t)tokens_list.size(), true, false);
    }
    tokens_list.resize(n_tokens);
    printf("n_tokens is %d\n", n_tokens);
    
    // Generate response
    result.SetLength(0);
    GrapaCHAR currentToken;
    
    // First, process the initial prompt tokens
    printf("Processing initial prompt tokens: %d\n", n_tokens);
    for (int i = 0; i < n_tokens; i++) {
        struct llama_batch batch = llama_batch_get_one(&tokens_list[i], 1);
        printf("Processing prompt token %d: %d\n", i, tokens_list[i]);
        if (llama_decode(this->mLlamaContext, batch)) {
            printf("llama_decode failed on prompt token %d\n", i);
            return -2;
        }
    }
    
    // Now generate new tokens
    printf("Starting generation loop (max tokens: %d)...\n", this->mMaxTokens);
    for (int i = 0; i < this->mMaxTokens; i++) {
        // Get next token using simple greedy sampling
        float* logits = llama_get_logits(this->mLlamaContext);
        int n_vocab = llama_vocab_n_tokens(vocab);
        
        // Find the token with highest probability (greedy sampling)
        llama_token next_token = 0;
        float max_logit = logits[0];
        for (int j = 1; j < n_vocab; j++) {
            if (logits[j] > max_logit) {
                max_logit = logits[j];
                next_token = j;
            }
        }
        
        printf("Generated token %d/%d: %d\n", i+1, this->mMaxTokens, next_token);
        
        if (next_token == llama_vocab_eos(vocab)) {
            printf("EOS token reached, stopping generation\n");
            break;
        }
        
        // Convert token to string
        char token_str[256];
        int n_chars = llama_token_to_piece(vocab, next_token, token_str, sizeof(token_str), 0, false);
        if (n_chars > 0) {
            token_str[n_chars] = '\0';  // Ensure null termination
            currentToken.FROM(token_str, n_chars);
            result.Append(currentToken);
            printf("Token %d: ID=%d, Text='%s' (len=%d)\n", i+1, next_token, token_str, n_chars);
        } else {
            printf("Token %d: ID=%d, Text='<empty>' (n_chars=%d)\n", i+1, next_token, n_chars);
        }
        
        // Add the new token to the sequence and decode it
        tokens_list.push_back(next_token);
        struct llama_batch batch = llama_batch_get_one(&next_token, 1);
        printf("Decoding generated token %d\n", i);
        if (llama_decode(this->mLlamaContext, batch)) {
            printf("llama_decode failed on generated token %d\n", i);
            return -2;
        }
    }
    
    printf("Final generated text: '%s' (length: %d)\n", (char*)result.mBytes, result.mLength);
    return 0;
}

GrapaRuleEvent* GrapaModel::GetModelInfo() const
{
    GrapaRuleEvent* result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::GOBJ;
    result->vQueue = new GrapaRuleQueue();
    
    // Add loaded status
    GrapaCHAR isLoaded;
    isLoaded.SetBool(mLoaded);
    isLoaded.mToken = GrapaTokenType::BOOL;
    GrapaRuleEvent* loaded = new GrapaRuleEvent(0, GrapaCHAR("loaded"), isLoaded);
    result->vQueue->PushTail(loaded);

    // Add backend
    GrapaRuleEvent* backend = new GrapaRuleEvent(0, GrapaCHAR("backend"), mBackend);
    result->vQueue->PushTail(backend);
    
    // Add model path
    GrapaRuleEvent* path = new GrapaRuleEvent(0, GrapaCHAR("path"), mModelPath);
    result->vQueue->PushTail(path);
    
    // Add max tokens
    GrapaRuleEvent* maxTokens = new GrapaRuleEvent(0, GrapaCHAR("max_tokens"), GrapaInt(mMaxTokens).getBytes());
    result->vQueue->PushTail(maxTokens);
    
    // Add context size
    GrapaRuleEvent* contextSize = new GrapaRuleEvent(0, GrapaCHAR("context_size"), GrapaInt(mContextSize).getBytes());
    result->vQueue->PushTail(contextSize);
    
    // Add temperature
    GrapaRuleEvent* temperature = new GrapaRuleEvent(0, GrapaCHAR("temperature"), GrapaFloat((double)mTemperature).getBytes());
    result->vQueue->PushTail(temperature);
    
    // Add top_k
    GrapaRuleEvent* topK = new GrapaRuleEvent(0, GrapaCHAR("top_k"), GrapaInt(mTopK).getBytes());
    result->vQueue->PushTail(topK);
    
    // Add top_p
    GrapaRuleEvent* topP = new GrapaRuleEvent(0, GrapaCHAR("top_p"), GrapaFloat(mTopP).getBytes());
    result->vQueue->PushTail(topP);
    
    // Add repeat penalty
    GrapaRuleEvent* repeatPenalty = new GrapaRuleEvent(0, GrapaCHAR("repeat_penalty"), GrapaFloat((double)mRepeatPenalty).getBytes());
    result->vQueue->PushTail(repeatPenalty);
    
    // Add seed
    GrapaRuleEvent* seed = new GrapaRuleEvent(0, GrapaCHAR("seed"), GrapaInt(mSeed).getBytes());
    result->vQueue->PushTail(seed);
    
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

GrapaError GrapaModel::SetParams(GrapaRuleEvent* params)
{
    // Parse Grapa object parameters
    GrapaError result = ParseParams(params);
    if (result == 0) {
        mPersistentParams = params;
        mUserParamsSet = true;
    }
    return result;
}

GrapaRuleEvent* GrapaModel::GetParams() const
{
    GrapaRuleEvent* result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::GOBJ;
    result->vQueue = new GrapaRuleQueue();

    // Add max tokens
    GrapaRuleEvent* maxTokens = new GrapaRuleEvent(0, GrapaCHAR("max_tokens"), GrapaInt(mMaxTokens).getBytes());
    result->vQueue->PushTail(maxTokens);
    
    // Add temperature
    GrapaRuleEvent* temperature = new GrapaRuleEvent(0, GrapaCHAR("temperature"), GrapaFloat((double)mTemperature).getBytes());
    result->vQueue->PushTail(temperature);
    
    // Add top_k
    GrapaRuleEvent* topK = new GrapaRuleEvent(0, GrapaCHAR("top_k"), GrapaInt(mTopK).getBytes());
    result->vQueue->PushTail(topK);
    
    // Add top_p
    GrapaRuleEvent* topP = new GrapaRuleEvent(0, GrapaCHAR("top_p"), GrapaFloat((double)mTopP).getBytes());
    result->vQueue->PushTail(topP);
    
    // Add repeat penalty
    GrapaRuleEvent* repeatPenalty = new GrapaRuleEvent(0, GrapaCHAR("repeat_penalty"), GrapaFloat((double)mRepeatPenalty).getBytes());
    result->vQueue->PushTail(repeatPenalty);
    
    // Add seed
    GrapaRuleEvent* seed = new GrapaRuleEvent(0, GrapaCHAR("seed"), GrapaInt(mSeed).getBytes());
    result->vQueue->PushTail(seed);
    
    return result;
}

GrapaError GrapaModel::ParseParams(GrapaRuleEvent* params)
{
    // TODO: Implement proper parameter parsing
    // For now, just return success
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
    mMaxTokens = 10;  // Very low for testing - will be adjusted based on model
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
            mMaxTokens = 10;  // Very low for testing
            mTemperature = 0.7f;
        } else if (modelSize <= 13) {
            mMaxTokens = 100;  // Much lower for testing
            mTemperature = 0.6f;
        } else {
            mMaxTokens = 200;  // Much lower for testing
            mTemperature = 0.5f;
        }
    }
}

GrapaRuleEvent* GrapaModel::MergeParams(GrapaRuleEvent* persistent, GrapaRuleEvent* callSpecific)
{
    // Simple merge - call-specific overrides persistent
    if (callSpecific) {
        return callSpecific;  // For now, just use call-specific if provided
    }
    return persistent;
}

GrapaError GrapaModel::ApplyParamsToLlama(GrapaRuleEvent* params)
{
    // Apply parameters to LLAMA context
    // This would parse the Grapa object params and apply them to the context
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
