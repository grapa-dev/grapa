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

GrapaModel::GrapaModel(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pParams) { vScriptExec = pScriptExec; vNameSpace = pNameSpace; INIT(pParams); }
GrapaModel::~GrapaModel() { CLEAR(); }

void GrapaModel::SetRuleEvent(GrapaRuleEvent* pParams)
{
    if (pParams == NULL)
        return;

    vParams = pParams;
}

void GrapaModel::INIT(GrapaRuleEvent* pParams)
{
    mLoaded = false;
    mLlamaModel = nullptr;
    mLlamaContext = nullptr;

    mMaxTokens = 10;  // Very low for testing
    mContextSize = 0;

    mTemperature = 0.7f;
    mTopK = 40;
    mTopP = 0.9f;
    mRepeatPenalty = 1.1f;
    mSeed = -1;
    
    if (pParams)
    {
        vParams = pParams;

        vParams->CLEAR();
        vParams->mValue.mToken = GrapaTokenType::MODEL;
        vParams->vQueue = new GrapaRuleQueue();

        // Add max tokens
        GrapaRuleEvent* maxTokens = new GrapaRuleEvent(0, GrapaCHAR("max_tokens"), GrapaInt(mMaxTokens).getBytes());
        vParams->vQueue->PushTail(maxTokens);

        // Add context size
        GrapaRuleEvent* contextSize = new GrapaRuleEvent(0, GrapaCHAR("context_size"), GrapaInt(mContextSize).getBytes());
        vParams->vQueue->PushTail(contextSize);

        // Add temperature
        GrapaRuleEvent* temperature = new GrapaRuleEvent(0, GrapaCHAR("temperature"), GrapaFloat((double)mTemperature).getBytes());
        vParams->vQueue->PushTail(temperature);

        // Add top_k
        GrapaRuleEvent* topK = new GrapaRuleEvent(0, GrapaCHAR("top_k"), GrapaInt(mTopK).getBytes());
        vParams->vQueue->PushTail(topK);

        // Add top_p
        GrapaRuleEvent* topP = new GrapaRuleEvent(0, GrapaCHAR("top_p"), GrapaFloat(mTopP).getBytes());
        vParams->vQueue->PushTail(topP);

        // Add repeat penalty
        GrapaRuleEvent* repeatPenalty = new GrapaRuleEvent(0, GrapaCHAR("repeat_penalty"), GrapaFloat((double)mRepeatPenalty).getBytes());
        vParams->vQueue->PushTail(repeatPenalty);

        // Add seed
        GrapaRuleEvent* seed = new GrapaRuleEvent(0, GrapaCHAR("seed"), GrapaInt(mSeed).getBytes());
        vParams->vQueue->PushTail(seed);

    }
        
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
    
    mModelPath.FROM(modelPath);
    mBackend.FROM(backend);

    if (mBackend.StrCmp("llama") == 0) {
        result = LoadLlama(modelPath);
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
        SetModelDefaults();
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
    mLlamaModel = llama_load_model_from_file((char*)modelPath.mBytes, mLlamaModelParams);
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

GrapaError GrapaModel::Generate(const GrapaCHAR& prompt, GrapaCHAR& result, GrapaRuleEvent* callParams)
{
    if (!mLoaded) {
        return -1;
    }
    
    // Merge persistent + call-specific parameters
    GrapaRuleEvent* mergedParams = MergeParams(vParams, callParams);
    
    if (mBackend.StrCmp("llama") == 0) {
        GrapaError err = GenerateLlama(prompt, result, mergedParams);
        delete mergedParams;
        return err;
    }
    
    delete mergedParams;
    return -2; // Unsupported backend
}

GrapaError GrapaModel::GenerateLlama(const GrapaCHAR& prompt, GrapaCHAR& result, GrapaRuleEvent* mergedParams)
{
    if (!this->mLlamaContext) {
        return -1;
    }
    
    // Apply parameters to LLAMA context
    GrapaError paramResult = this->ApplyParamsToLlama(mergedParams);
    if (paramResult != 0) {
        return paramResult;
    }
    
    // Tokenize the prompt
    const struct llama_vocab* vocab = llama_model_get_vocab(this->mLlamaModel);
    std::vector<llama_token> tokens_list;
    tokens_list.resize(prompt.mLength + 1);
    int n_tokens = llama_tokenize(vocab, (char*)prompt.mBytes, (int32_t)prompt.mLength, tokens_list.data(), (int32_t)tokens_list.size(), true, false);
    if (n_tokens < 0) {
        tokens_list.resize(-n_tokens);
        n_tokens = llama_tokenize(vocab, (char*)prompt.mBytes, (int32_t)prompt.mLength, tokens_list.data(), (int32_t)tokens_list.size(), true, false);
    }
    tokens_list.resize(n_tokens);
    
    // Generate response
    result.SetLength(0);
    GrapaCHAR currentToken;
    
    // First, process the initial prompt tokens
    for (int i = 0; i < n_tokens; i++) {
        struct llama_batch batch = llama_batch_get_one(&tokens_list[i], 1);
        if (llama_decode(this->mLlamaContext, batch)) {
            return -2;
        }
    }
    
    // Now generate new tokens
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
                
        if (next_token == llama_vocab_eos(vocab)) {
            break;
        }
        
        // Convert token to string
        char token_str[256];
        int n_chars = llama_token_to_piece(vocab, next_token, token_str, sizeof(token_str), 0, false);
        if (n_chars > 0) {
            token_str[n_chars] = '\0';  // Ensure null termination
            currentToken.FROM(token_str, n_chars);
            result.Append(currentToken);
        } else {
        }
        
        // Add the new token to the sequence and decode it
        tokens_list.push_back(next_token);
        struct llama_batch batch = llama_batch_get_one(&next_token, 1);
        if (llama_decode(this->mLlamaContext, batch)) {
            return -2;
        }
    }
    
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
  
    return result;
}

GrapaError GrapaModel::SetParams(GrapaRuleEvent* params)
{
    if (params == NULL) 
		return -1;
    if (vParams == NULL)
        return -1;
    GrapaRuleEvent* current = params->vQueue ? params->vQueue->Head() : nullptr;
    while (current) {
        s64 index;
        GrapaRuleEvent* override = vParams->vQueue ? vParams->vQueue->Search(current->mName, index) : nullptr;
        if (override)
            override->mValue = current->mValue;
        else
            vParams->vQueue->PushTail(new GrapaRuleEvent(0, current->mName, current->mValue));
        current = current->Next();
    }
    return 0;
}

GrapaRuleEvent* GrapaModel::GetParams() const
{
    GrapaRuleEvent* result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::GOBJ;
	result->vQueue = vScriptExec->CopyQueue(vParams->vQueue);
    return result;
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

    GrapaRuleEvent* override;
    s64 index;
    override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
    if (override)
        override->mValue.FROM(GrapaInt(10).getBytes());
    override = vParams->vQueue ? vParams->vQueue->Search("context_size", index) : nullptr;
    if (override)
        override->mValue.FROM(GrapaInt(0).getBytes());

    // Preserve user preference parameters
    // mTemperature, mTopK, mTopP, mRepeatPenalty, mSeed stay the same
}

void GrapaModel::SetModelDefaults()
{
    // Set reasonable defaults for this specific model
    if (mBackend.StrCmp("llama") == 0) {
        // Get model info and set appropriate defaults
        int modelSize = GetModelSize();  // 7B, 13B, 70B, etc.
        GrapaRuleEvent* override;
        s64 index;

        if (modelSize <= 7) {
            override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaInt(10).getBytes());
            override = vParams->vQueue ? vParams->vQueue->Search("temperature", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaFloat((double)0.7f).getBytes());
        } else if (modelSize <= 13) {
            override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaInt(100).getBytes());
            override = vParams->vQueue ? vParams->vQueue->Search("temperature", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaFloat((double)0.6f).getBytes());
        } else {
            override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaInt(200).getBytes());
            override = vParams->vQueue ? vParams->vQueue->Search("temperature", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaFloat((double)0.5f).getBytes());
        }
    }
}

GrapaRuleEvent* GrapaModel::MergeParams(GrapaRuleEvent* persistent, GrapaRuleEvent* callSpecific)
{
    GrapaRuleEvent* result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::GOBJ;
    result->vQueue = vScriptExec->CopyQueue(persistent->vQueue);
    GrapaRuleEvent* current = callSpecific->vQueue ? callSpecific->vQueue->Head() : nullptr;
    while (current) {
        s64 index;
        GrapaRuleEvent* override = result->vQueue ? result->vQueue->Search(current->mName, index) : nullptr;
        if (override)
            override->mValue = current->mValue;
        else
            result->vQueue->PushTail(new GrapaRuleEvent(0, current->mName, current->mValue));
        current = current->Next();
    }
    return result;
}

GrapaError GrapaModel::ApplyParamsToLlama(GrapaRuleEvent* params)
{
    if (params->mValue.mToken != GrapaTokenType::GOBJ)
        return -1;
    if (params->vQueue == NULL)
        return 0;
    params = params->vQueue->Head();
    while (params)
    {
        if (params->mName.StrCmp("max_tokens") == 0) {
            if (params->mValue.mToken == GrapaTokenType::INT) {
                mMaxTokens = GrapaInt(params->mValue).LongValue();
            }
        }
        else if (params->mName.StrCmp("context_size") == 0) {
            if (params->mValue.mToken == GrapaTokenType::INT) {
                mContextSize = GrapaInt(params->mValue).LongValue();
            }
        }
        else if (params->mName.StrCmp("temperature") == 0) {
            if (params->mValue.mToken == GrapaTokenType::FLOAT) {
                mTemperature = GrapaFloat(params->mValue).ToDouble();
            }
        }
        else if (params->mName.StrCmp("top_k") == 0) {
            if (params->mValue.mToken == GrapaTokenType::INT) {
                mTopK = GrapaInt(params->mValue).LongValue();
            }
        }
        else if (params->mName.StrCmp("top_p") == 0) {
            if (params->mValue.mToken == GrapaTokenType::FLOAT) {
                mTopP = GrapaFloat(params->mValue).ToDouble();
            }
        }
        else if (params->mName.StrCmp("repeat_penalty") == 0) {
            if (params->mValue.mToken == GrapaTokenType::FLOAT) {
                mRepeatPenalty = GrapaFloat(params->mValue).ToDouble();
            }
        }
        else if (params->mName.StrCmp("seed") == 0) {
            if (params->mValue.mToken == GrapaTokenType::INT) {
                mSeed = GrapaInt(params->mValue).LongValue();
            }
        }

        params = params->Next();
    }
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
