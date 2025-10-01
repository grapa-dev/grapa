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
#include <ctime>  // For time() function
#include "GrapaSystem.h"
#include "GrapaState.h"

#include <vector>
#include <algorithm>  // For std::transform
#include <thread>
#include <fstream>
#include <cctype>

#include "llama.h"
#include "ggml.h"   // for ggml_log_level enum


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

    mMaxTokens = 20;  // Very low for testing
    mContextSize = 2048;  // Default context size

    mTemperature = 0.1f;
    mTopK = 20;
    mTopP = 0.7f;
    mRepeatPenalty = 1.3f;
    mSeed = -1;
    mVerbose = 0;     // Default to silent (no LLAMA.cpp output)

    // Initialize sampler to NULL
    mLlamaSampler = nullptr;

    // Initialize ONNX Runtime members
    mOnnxSession = nullptr;
    mOnnxEnv = nullptr;

    // Initialize context management
    mContextPreserved = false;
    mContextTokens.clear();
    mContextHistory.SetLength(0);
    mContextInitialized = false;

    // Set LLAMA.cpp logging callback to control verbosity
    llama_log_set(LogCallback, (void*)&mVerbose);

    float v;
    char buffer[128];
    GrapaCHAR s;

    // GrapaFloat d(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
    GrapaFloat d(false, 56, 4, 0);
    sprintf(buffer, "%f", mTemperature);
    d.FromString(buffer, 10);
    s = d.ToString(10);

    sprintf(buffer, "%f", 0.15f);
    d.FromString(buffer, 10);
    s = d.ToString(10);



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

        // Add verbose
        GrapaRuleEvent* verbose = new GrapaRuleEvent(0, GrapaCHAR("verbose"), GrapaInt(mVerbose).getBytes());
        vParams->vQueue->PushTail(verbose);

    }

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
    mMethod.SetNull();
}

GrapaError GrapaModel::Load(const GrapaCHAR& modelPath, const GrapaCHAR& method)
{
    GrapaError result = 0;

    if (mLoaded) {
        Unload();
    }
    // Reset model-specific parameters, preserve user preferences
    ResetModelSpecificParams();

    // Clear context cache when loading new model
    mContextPreserved = false;
    mContextTokens.clear();
    mContextHistory.SetLength(0);
    mContextInitialized = false;

    mModelPath.FROM(modelPath);
    mMethod.FROM(method);

    if (mModelPath.mLength == 0) {
        return 0; // Could not detect model path
    }

    // If method is not specified, try to auto-detect it
    if (method.mLength == 0) {
        mMethod = AutoDetectMethod(modelPath);
        if (mMethod.mLength == 0) {
            return -1; // Could not detect method
        }
    }
    else {
        mMethod.FROM(method);
    }

    if (mMethod.StrCmp("llama-local") == 0) {
        result = LoadLlama(modelPath);
    }
    else if (mMethod.StrCmp("openai") == 0) {
        result = LoadOpenAI(modelPath);
    }
    else if (mMethod.StrCmp("openai-embedding") == 0) {
        result = LoadOpenAI(modelPath); // Same as OpenAI but for embeddings
    }
    else {
        result = -1; // Unsupported method
    }

    if (result == 0) {
        mLoaded = true;
        SetModelDefaults();
    }

    return result;
}

GrapaCHAR GrapaModel::AutoDetectMethod(const GrapaCHAR& modelPath)
{
    GrapaCHAR result;
    std::string path((char*)modelPath.mBytes, modelPath.mLength);

    // First try file extension detection
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = path.substr(dotPos);

        // Convert to lowercase for case-insensitive comparison
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".gguf") {
            result.FROM("llama-local");
            return result;
        }
        else if (ext == ".pkl" || ext == ".joblib") {
            result.FROM("sklearn");
            return result;
        }

    }

    // If extension doesn't match, try magic bytes detection
    std::ifstream file(path, std::ios::binary);
    if (file) {
        // Zero-initialize header buffer to prevent false positives
        char header[8] = { 0 };

        // Read header and validate we got enough bytes
        file.read(header, 8);
        std::streamsize bytesRead = file.gcount();

        // Only proceed if we read enough bytes for the largest magic number (4 bytes)
        if (bytesRead >= 4) {
            // GGUF format (LLAMA.cpp) - requires 4 bytes
            if (bytesRead >= 4 && strncmp(header, "GGUF", 4) == 0) {
                result.FROM("llama-local");
                return result;
            }

            // Python pickle format (sklearn) - requires 2 bytes
            if (bytesRead >= 2 && strncmp(header, "PK", 2) == 0) {
                result.FROM("sklearn");
                return result;
            }
        }
    }

    // Could not detect backend
    return result;  // Empty result
}

GrapaError GrapaModel::Unload()
{
    GrapaError result = 0;

    if (mLoaded) {
        if (mMethod.StrCmp("llama-local") == 0) {
            result = UnloadLlama();
        }
        else if (mMethod.StrCmp("openai") == 0) {
            result = UnloadOpenAI();
        }
        else if (mMethod.StrCmp("openai-embedding") == 0) {
            result = UnloadOpenAI();
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

    // Set LLAMA.cpp logging callback to control verbosity BEFORE loading
    llama_log_set(LogCallback, (void*)&mVerbose);

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

    // Initialize sampler chain for temperature-aware generation
    result = InitializeSampler();
    if (result != 0) {
        // Sampler initialization failed, but model is still usable with greedy sampling
        // We'll continue without the sampler
        result = 0;  // Reset to success since model is usable
    }

    return result;
}

GrapaError GrapaModel::UnloadLlama()
{
    // Clean up sampler first
    CleanupSampler();

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

GrapaError GrapaModel::LoadOpenAI(const GrapaCHAR& modelPath)
{
    // Unload first if already loaded
    if (mLoaded) {
        UnloadOpenAI();
    }

    // Store the model name (e.g., "gpt-5-nano")
    mModelPath.FROM(modelPath);

    // Initialize OpenAI response ID
    mOpenAIResponseId.SetNull();

    // Get client.crt from static library for SSL
    GrapaCHAR clientCert;
    if (gSystem && gSystem->mStaticLib) {
        s64 index;
        GrapaRuleEvent* certParam = gSystem->mStaticLib->Search("client.crt", index);
        if (certParam) {
            clientCert.FROM(certParam->mValue);
        }
    }

    GrapaCHAR url("https://api.openai.com");
    mNet.Set(url);
    // Set the certificate data in the mNet object (like in the .grc example)
    if (clientCert.mLength > 0) {
        GrapaRuleEvent* certObj = new GrapaRuleEvent();
        certObj->mValue.mToken = GrapaTokenType::STR;
        certObj->mValue.FROM(clientCert);
        mNet.Certificate(certObj);
        delete certObj;
    }

    // Connect to OpenAI API using the mNet object (keep-alive connection)
    GrapaError err = mNet.Connect(NULL);
    if (err != 0) {
        return -1; // Connection failed
    }

    // Verify connection is established
    if (!mNet.mNet.mConnected) {
        mNet.Disconnect();
        return -2; // Connection verification failed
    }

    mLoaded = true;

    return 0;
}

GrapaError GrapaModel::UnloadOpenAI()
{
    // Disconnect from OpenAI API
    if (mNet.mNet.mConnected) {
        mNet.Disconnect();
    }

    // Clear OpenAI context
    mOpenAIResponseId.SetNull();

    mLoaded = false;

    return 0;
}

GrapaRuleEvent* GrapaModel::Generate(const GrapaCHAR& prompt, GrapaRuleEvent* callParams)
{
    GrapaRuleEvent* result = NULL;

    if (!mLoaded) {
        return result;
    }

    GrapaRuleEvent* mergedParams = MergeParams(vParams, callParams);

    if (mMethod.StrCmp("llama-local") == 0) {
        result = GenerateLlama(prompt, mergedParams);
    }
    else if (mMethod.StrCmp("openai") == 0) {
        result = GenerateOpenAI(prompt, mergedParams);
    }
    else if (mMethod.StrCmp("openai-embedding") == 0) {
        result = EmbedOpenAI(prompt, mergedParams);
    }

    delete mergedParams;
    return result;
}

GrapaRuleEvent* GrapaModel::GenerateLlama(const GrapaCHAR& prompt, GrapaRuleEvent* mergedParams)
{
    GrapaRuleEvent* result = NULL;
    result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::GOBJ;
    result->vQueue = new GrapaRuleQueue();

    if (!this->mLlamaContext) {
        result->mValue.mToken = GrapaTokenType::ERR;
        result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaInt(-1).getBytes()));
        result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("message"),GrapaCHAR("Llama context not initialized")));
        return result;
    }

    // Apply parameters to LLAMA context
    GrapaError paramResult = this->ApplyParamsToLlama(mergedParams);
    if (paramResult != 0) {
        result->mValue.mToken = GrapaTokenType::ERR;
        result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaInt(-1).getBytes()));
        result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("message"),GrapaCHAR("Llama parameter application error")));
        return result;
    }

    // Early exit for empty prompts
    if (prompt.mLength == 0) {
        result->mValue.mToken = GrapaTokenType::ERR;
        result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("error"), GrapaInt(-1).getBytes()));
        result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("message"),GrapaCHAR("Llama prompt is empty")));
        return result;
    }

    // Backend-optimized context management
    const struct llama_vocab* vocab = llama_model_get_vocab(this->mLlamaModel);
    std::vector<llama_token> new_tokens;

    // Tokenize the new prompt
    size_t estimated_tokens = prompt.mLength / 4;  // Rough estimate: ~4 chars per token
    new_tokens.resize(estimated_tokens);

    int n_new_tokens = llama_tokenize(vocab, (char*)prompt.mBytes, (int32_t)prompt.mLength,
        new_tokens.data(), (int32_t)new_tokens.size(), true, false);
    if (n_new_tokens < 0) {
        new_tokens.resize(-n_new_tokens);
        n_new_tokens = llama_tokenize(vocab, (char*)prompt.mBytes, (int32_t)prompt.mLength,
            new_tokens.data(), (int32_t)new_tokens.size(), true, false);
    }
    new_tokens.resize(n_new_tokens);

    // Early exit if no tokens
    if (n_new_tokens == 0) {
        return result;
    }

    // For LLAMA.cpp: Use truly persistent context (Option 3 - most efficient)
    if (mContextPreserved && mContextInitialized) {
        // Context is already initialized - just append new tokens and process them
        mContextTokens.insert(mContextTokens.end(), new_tokens.begin(), new_tokens.end());

        // Process only the new tokens (LLAMA.cpp context preserves all previous state)
        struct llama_batch batch = llama_batch_get_one(new_tokens.data(), n_new_tokens);
        if (llama_decode(this->mLlamaContext, batch)) {
            return result;
        }
    }
    else {
        // First call - initialize context with all tokens
        mContextTokens = new_tokens;
        mContextPreserved = true;
        mContextInitialized = true;

        // Process all tokens to initialize the context
        struct llama_batch batch = llama_batch_get_one(mContextTokens.data(), mContextTokens.size());
        if (llama_decode(this->mLlamaContext, batch)) {
            return result;
        }
    }

    // Add text field
    GrapaRuleEvent* text = new GrapaRuleEvent(0, GrapaCHAR("text"), GrapaCHAR());
    result->vQueue->PushTail(text);

    // Add metadata field
    GrapaRuleEvent* metadata = new GrapaRuleEvent();
    metadata->mName.FROM("metadata");
    metadata->mValue.mToken = GrapaTokenType::GOBJ;
    metadata->vQueue = new GrapaRuleQueue();
    result->vQueue->PushTail(metadata);

    // Now generate new tokens using LLAMA.cpp sampler
    for (int i = 0; i < this->mMaxTokens; i++) {
        llama_token next_token;

        // Use LLAMA.cpp sampler if available, otherwise fall back to greedy
        if (mLlamaSampler) {
            // Use the sampler chain for temperature-aware generation
            next_token = llama_sampler_sample(mLlamaSampler, this->mLlamaContext, -1);
            llama_sampler_accept(mLlamaSampler, next_token);
        }
        else {
            // Fallback to greedy sampling if sampler is not available
            float* logits = llama_get_logits(this->mLlamaContext);
            int n_vocab = llama_vocab_n_tokens(vocab);

            next_token = 0;
            float max_logit = logits[0];
            for (int j = 1; j < n_vocab; j++) {
                if (logits[j] > max_logit) {
                    max_logit = logits[j];
                    next_token = j;
                }
            }
        }

        if (next_token == llama_vocab_eos(vocab)) {
            break;
        }

        // Convert token to string
        char token_str[256];
        int n_chars = llama_token_to_piece(vocab, next_token, token_str, sizeof(token_str), 0, false);
        if (n_chars > 0) {
            text->mValue.GrapaCHAR::Append(token_str, n_chars);
        }

        // Add the new token to persistent context and decode it
        mContextTokens.push_back(next_token);
        struct llama_batch next_batch = llama_batch_get_one(&next_token, 1);
        if (llama_decode(this->mLlamaContext, next_batch)) {
            return result;
        }
    }

    // Add metadata to result
    GrapaRuleEvent* prompt_tokens_item = new GrapaRuleEvent(0, GrapaCHAR("prompt_tokens"), GrapaInt(n_new_tokens).getBytes());
    metadata->vQueue->PushTail(prompt_tokens_item);

    GrapaRuleEvent* completion_tokens_item = new GrapaRuleEvent(0, GrapaCHAR("completion_tokens"), GrapaInt(mMaxTokens).getBytes());
    metadata->vQueue->PushTail(completion_tokens_item);

    GrapaRuleEvent* total_tokens_item = new GrapaRuleEvent(0, GrapaCHAR("total_tokens"), GrapaInt(n_new_tokens + mMaxTokens).getBytes());
    metadata->vQueue->PushTail(total_tokens_item);

    GrapaRuleEvent* finish_reason_item = new GrapaRuleEvent(0, GrapaCHAR("finish_reason"), GrapaCHAR("stop"));
    metadata->vQueue->PushTail(finish_reason_item);

    GrapaRuleEvent* model_path_item = new GrapaRuleEvent(0, GrapaCHAR("model_path"), mModelPath);
    metadata->vQueue->PushTail(model_path_item);

    GrapaRuleEvent* temperature_item = new GrapaRuleEvent(0, GrapaCHAR("temperature"), GrapaFloat(mTemperature).getBytes());
    metadata->vQueue->PushTail(temperature_item);

    GrapaRuleEvent* max_tokens_item = new GrapaRuleEvent(0, GrapaCHAR("max_tokens"), GrapaInt(mMaxTokens).getBytes());
    metadata->vQueue->PushTail(max_tokens_item);

    GrapaRuleEvent* context_length_item = new GrapaRuleEvent(0, GrapaCHAR("context_length"), GrapaInt(mContextTokens.size()).getBytes());
    metadata->vQueue->PushTail(context_length_item);

    GrapaRuleEvent* context_preserved_item = new GrapaRuleEvent(0, GrapaCHAR("context_preserved"), GrapaInt(mContextPreserved ? 1 : 0).getBytes());
    metadata->vQueue->PushTail(context_preserved_item);

    return result;
}

GrapaRuleEvent* GrapaModel::GenerateOpenAI(const GrapaCHAR& prompt, GrapaRuleEvent* mergedParams)
{
    GrapaRuleEvent* result = NULL;

    s64 index;

    // Early exit for empty prompts
    if (prompt.mLength == 0) {
        return result;
    }

    // Extract authorization and store parameters
    GrapaCHAR authToken;
    bool storeContext = true; // Default to true for backward compatibility
    if (mergedParams && mergedParams->vQueue) {
        GrapaRuleEvent* authParam = mergedParams->vQueue->Search("api_key", index);
        while (authParam && authParam->mValue.mToken == GrapaTokenType::PTR && authParam->vRulePointer) authParam = authParam->vRulePointer;
        if (authParam && authParam->mValue.mToken == GrapaTokenType::STR) {
            authToken.FROM(authParam->mValue);
        }

        // Check for store parameter (optional)
        GrapaRuleEvent* storeParam = mergedParams->vQueue->Search("store", index);
        while (storeParam && storeParam->mValue.mToken == GrapaTokenType::PTR && storeParam->vRulePointer) storeParam = storeParam->vRulePointer;
        if (storeParam && storeParam->mValue.mToken == GrapaTokenType::BOOL) {
            storeContext = storeParam->mValue.mLength == 0 || storeParam->mValue.mBytes[0] != 0;
        }
    }

    if (authToken.mLength == 0) {
        return result;
    }

    // Check if connection is still active
    if (!mNet.mNet.mConnected) {
        return result;
    }

    // Prepare headers
    GrapaCHAR headers;
    headers.FROM("Host: api.openai.com\r\n");
    headers.Append("Authorization: Bearer ");
    headers.Append(authToken);
    headers.Append("\r\nContent-Type: application/json\r\nConnection: keep-alive\r\n");

    // Prepare request body
    GrapaCHAR requestBody;
    requestBody.FROM("{");
    requestBody.Append("\"model\": \"");
    requestBody.Append(mModelPath);
    requestBody.Append("\",");
    requestBody.Append("\"input\": \"");

    // Escape the prompt for JSON
    GrapaCHAR escapedPrompt = prompt;
    escapedPrompt.Replace("\"", "\\\"");
    escapedPrompt.Replace("\n", "\\n");
    escapedPrompt.Replace("\r", "\\r");
    escapedPrompt.Replace("\t", "\\t");
    requestBody.Append(escapedPrompt);
    requestBody.Append("\",");
    requestBody.Append("\"store\": ");
    requestBody.Append(storeContext ? "true" : "false");

    // Add previous_response_id if we have context
    if (mOpenAIResponseId.mLength > 0) {
        requestBody.Append(",");
        requestBody.Append("\"previous_response_id\": \"");
        requestBody.Append(mOpenAIResponseId);
        requestBody.Append("\"");
    }

    requestBody.Append("}");

    // Send request using the existing connection
    GrapaCHAR headerStr;
    headerStr.Append("POST /v1/responses HTTP/1.1\r\n");
    headerStr.Append(headers);
    headerStr.Append("Content-Length: ");
    headerStr.Append(GrapaInt(requestBody.mLength).ToString());
    headerStr.Append("\r\n");
    headerStr.Append("\r\n");
    headerStr.Append(requestBody);

    GrapaError err = mNet.mNet.Send(headerStr);
    if (err != 0) {
        return result;
    }

    mNet.HttpRead(vScriptExec);
    GrapaRuleEvent* message = mNet.HttpMessage(vScriptExec, vNameSpace);
    if (message && message->vQueue)
    {
        // Extract response ID for context management
        GrapaRuleEvent* body = message->vQueue->Search("body", index);
        if (body)
        {
            result = body;
            message->vQueue->PopEvent(body);
        }
    }
    if (message)
    {
        message->CLEAR();
        delete message;
        message = NULL;
    }

    return result;
}

GrapaRuleEvent* GrapaModel::EmbedOpenAI(const GrapaCHAR& text, GrapaRuleEvent* mergedParams)
{
    GrapaRuleEvent* result = NULL;

    s64 index;

    // Early exit for empty text
    if (text.mLength == 0) {
        return result;
    }

    // Extract authorization token
    GrapaCHAR authToken;
    if (mergedParams && mergedParams->vQueue) {
        GrapaRuleEvent* authParam = mergedParams->vQueue->Search("api_key", index);
        while (authParam && authParam->mValue.mToken == GrapaTokenType::PTR && authParam->vRulePointer) authParam = authParam->vRulePointer;
        if (authParam && authParam->mValue.mToken == GrapaTokenType::STR) {
            authToken.FROM(authParam->mValue);
        }
    }

    if (authToken.mLength == 0) {
        return result;
    }

    // Check if connection is still active
    if (!mNet.mNet.mConnected) {
        return result;
    }

    // Prepare headers
    GrapaCHAR headers;
    headers.FROM("Host: api.openai.com\r\n");
    headers.Append("Authorization: Bearer ");
    headers.Append(authToken);
    headers.Append("\r\nContent-Type: application/json\r\nConnection: keep-alive\r\n");

    // Prepare request body for embeddings
    GrapaCHAR requestBody;
    requestBody.FROM("{");
    requestBody.Append("\"model\": \"");
    requestBody.Append(mModelPath);
    requestBody.Append("\",");
    requestBody.Append("\"input\": \"");

    // Escape the text for JSON
    GrapaCHAR escapedText = text;
    escapedText.Replace("\"", "\\\"");
    escapedText.Replace("\n", "\\n");
    escapedText.Replace("\r", "\\r");
    escapedText.Replace("\t", "\\t");
    requestBody.Append(escapedText);
    requestBody.Append("\"");
    requestBody.Append("}");

    // Send request using the existing connection
    GrapaCHAR headerStr;
    headerStr.Append("POST /v1/embeddings HTTP/1.1\r\n");
    headerStr.Append(headers);
    headerStr.Append("Content-Length: ");
    headerStr.Append(GrapaInt(requestBody.mLength).ToString());
    headerStr.Append("\r\n");
    headerStr.Append("\r\n");
    headerStr.Append(requestBody);

    GrapaError err = mNet.mNet.Send(headerStr);
    if (err != 0)
        return result;

    err = mNet.HttpRead(vScriptExec);
    if (err != 0)
        return result;

    GrapaRuleEvent* message = mNet.HttpMessage(vScriptExec, vNameSpace);
    if (message && message->vQueue)
    {
        s64 index;
        // Parse the JSON response to extract embedding
        GrapaRuleEvent* data = message->vQueue->Search("body", index);
        if (data)
        {
            result = data;
            message->vQueue->PopEvent(data);
        }
    }
    if (message)
    {
        message->CLEAR();
        delete message;
        message = NULL;
    }

    return result;
}

GrapaRuleEvent* GrapaModel::GetModelInfo()
{
    GrapaRuleEvent* result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::GOBJ;
    result->vQueue = new GrapaRuleQueue();

    // Add loaded status
    GrapaRuleEvent* loaded = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "loaded", mLoaded ? "\1" : "");
    result->vQueue->PushTail(loaded);

    // Add method
    GrapaRuleEvent* method = new GrapaRuleEvent(0, GrapaCHAR("method"), mMethod);
    result->vQueue->PushTail(method);

    // Add model path
    GrapaRuleEvent* path = new GrapaRuleEvent(0, GrapaCHAR("model_path"), mModelPath);
    result->vQueue->PushTail(path);


    // Note: Configuration parameters (temperature, top_k, etc.) are available via .params()
    // This method focuses on model metadata and status information

    // Add method-specific information if model is loaded
    if (mLoaded) {
        // LLAMA.cpp specific information
        if (const_cast<GrapaModel*>(this)->mMethod.StrCmp("llama-local") == 0 && mLlamaModel) {
            // Model size in bytes
            uint64_t model_size = llama_model_size(mLlamaModel);
            GrapaRuleEvent* model_size_info = new GrapaRuleEvent(0, GrapaCHAR("model_size_bytes"), GrapaInt((s64)model_size).getBytes());
            result->vQueue->PushTail(model_size_info);

            // Number of parameters
            uint64_t n_params = llama_model_n_params(mLlamaModel);
            GrapaRuleEvent* n_params_info = new GrapaRuleEvent(0, GrapaCHAR("n_params"), GrapaInt((s64)n_params).getBytes());
            result->vQueue->PushTail(n_params_info);

            // Model description
            char desc_buf[256];
            int desc_len = llama_model_desc(mLlamaModel, desc_buf, sizeof(desc_buf));
            if (desc_len > 0) {
                GrapaRuleEvent* model_desc = new GrapaRuleEvent(0, GrapaCHAR("model_description"), GrapaCHAR(desc_buf, desc_len));
                result->vQueue->PushTail(model_desc);
            }

            // Model capabilities
            bool has_encoder = llama_model_has_encoder(mLlamaModel);
            GrapaRuleEvent* encoder_info = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "has_encoder", has_encoder ? "\1" : "");
            result->vQueue->PushTail(encoder_info);

            bool has_decoder = llama_model_has_decoder(mLlamaModel);
            GrapaRuleEvent* decoder_info = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "has_decoder", has_decoder ? "\1" : "");
            result->vQueue->PushTail(decoder_info);

            bool is_recurrent = llama_model_is_recurrent(mLlamaModel);
            GrapaRuleEvent* recurrent_info = new GrapaRuleEvent(GrapaTokenType::BOOL, 0, "is_recurrent", is_recurrent ? "\1" : "");
            result->vQueue->PushTail(recurrent_info);

            // Vocabulary size (if context is available)
            if (mLlamaContext) {
                // Use the context to get vocabulary size - skip for now as API is unclear
                // int32_t vocab_size = llama_n_vocab(mLlamaContext);
                // if (vocab_size > 0) {
                //     GrapaRuleEvent* vocab_info = new GrapaRuleEvent(0, GrapaCHAR("vocab_size"), GrapaInt(vocab_size).getBytes());
                //     result->vQueue->PushTail(vocab_info);
                // }
            }
        }

    }

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

    // Check if vParams is null to prevent crash
    if (!vParams) {
        result->vQueue = new GrapaRuleQueue();  // Create empty queue
        return result;
    }

    result->vQueue = vScriptExec->CopyQueue(vParams->vQueue);
    return result;
}

// Context management
GrapaRuleEvent* GrapaModel::GetContext() const
{
    GrapaRuleEvent* result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::GOBJ;
    result->vQueue = new GrapaRuleQueue();

    if (const_cast<GrapaModel*>(this)->mMethod.StrCmp("openai") == 0) {
        // For OpenAI method, return response ID as context
        GrapaRuleEvent* responseId = new GrapaRuleEvent(0, GrapaCHAR("id"), mOpenAIResponseId);
        result->vQueue->PushTail(responseId);

        // Add method
        GrapaRuleEvent* method = new GrapaRuleEvent(0, GrapaCHAR("method"), mMethod);
        result->vQueue->PushTail(method);

        // Add model
        GrapaRuleEvent* model = new GrapaRuleEvent(0, GrapaCHAR("model"), mModelPath);
        result->vQueue->PushTail(model);

        return result;
    }

    // Add text context (convert tokens back to text for human readability)
    GrapaCHAR textContext;
    if (!mContextTokens.empty() && const_cast<GrapaModel*>(this)->mMethod.StrCmp("llama-local") == 0 && mLlamaModel) {
        const struct llama_vocab* vocab = llama_model_get_vocab(mLlamaModel);
        for (size_t i = 0; i < mContextTokens.size(); i++) {
            char token_str[256];
            int n_chars = llama_token_to_piece(vocab, mContextTokens[i], token_str, sizeof(token_str), 0, false);
            if (n_chars > 0) {
                textContext.Append(token_str, n_chars);
            }
        }
    }
    GrapaRuleEvent* text = new GrapaRuleEvent(0, GrapaCHAR("text"), textContext);
    result->vQueue->PushTail(text);

    // Add tokens as $LIST of $INT values
    GrapaRuleEvent* tokens = new GrapaRuleEvent();
    tokens->mValue.mToken = GrapaTokenType::LIST;
    tokens->vQueue = new GrapaRuleQueue();
    for (size_t i = 0; i < mContextTokens.size(); i++) {
        GrapaRuleEvent* token = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaInt((s64)mContextTokens[i]).getBytes());
        tokens->vQueue->PushTail(token);
    }
    GrapaRuleEvent* tokensField = new GrapaRuleEvent(0, GrapaCHAR("tokens"), GrapaCHAR(""));
    tokensField->mValue.mToken = GrapaTokenType::LIST;
    tokensField->vQueue = tokens->vQueue;
    result->vQueue->PushTail(tokensField);

    // Add method (backend type)
    GrapaRuleEvent* method = new GrapaRuleEvent(0, GrapaCHAR("method"), const_cast<GrapaModel*>(this)->mMethod);
    result->vQueue->PushTail(method);

    // Add model (filename only, no path)
    GrapaCHAR modelName = const_cast<GrapaModel*>(this)->mModelPath;
    // Extract filename from path
    std::string path((char*)modelName.mBytes, modelName.mLength);
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        path = path.substr(lastSlash + 1);
    }
    GrapaRuleEvent* model = new GrapaRuleEvent(0, GrapaCHAR("model"), GrapaCHAR(path.c_str(), path.length()));
    result->vQueue->PushTail(model);

    return result;
}

GrapaError GrapaModel::SetContext(GrapaRuleEvent* context)
{
    if (!context) {
        return -1; // Invalid context format
    }

    if (!mLoaded) {
        return -2; // Model not loaded
    }

    if (mMethod.StrCmp("openai") == 0) {
        // For OpenAI method, extract response ID from context
        if (context->mValue.mToken == GrapaTokenType::GOBJ && context->vQueue) {
            s64 index;
            GrapaRuleEvent* responseId = context->vQueue->Search("id", index);
            if (responseId && responseId->mValue.mToken == GrapaTokenType::STR) {
                mOpenAIResponseId.FROM(responseId->mValue);
                return 0; // Success
            }
        }
        return -1; // Invalid context format for OpenAI
    }

    // Clear existing context for other methods
    mContextTokens.clear();
    mContextPreserved = false;
    mContextInitialized = false;

    if (context->mValue.mToken == GrapaTokenType::GOBJ && context->vQueue) {
        // Handle $GOBJ input - check for tokens first (most efficient)
        s64 index;
        GrapaRuleEvent* tokens = context->vQueue->Search("tokens", index);
        if (tokens && tokens->mValue.mToken == GrapaTokenType::LIST && tokens->vQueue) {
            // Use tokens if available
            GrapaRuleEvent* item = tokens->vQueue->Head();
            while (item) {
                if (item->mValue.mToken == GrapaTokenType::INT) {
                    s64 token_id = GrapaInt(item->mValue).LongValue();
                    mContextTokens.push_back((llama_token)token_id);
                }
                item = item->Next();
            }
        }
        else {
            // Fall back to text
            GrapaRuleEvent* text = context->vQueue->Search("text", index);
            if (text && text->mValue.mToken == GrapaTokenType::STR) {
                GrapaCHAR textStr = text->mValue;
                return SetContextFromText(textStr);
            }
        }
    }
    else if (context->mValue.mToken == GrapaTokenType::LIST && context->vQueue) {
        // Handle $LIST input (current behavior)
        GrapaRuleEvent* item = context->vQueue->Head();
        while (item) {
            if (item->mValue.mToken == GrapaTokenType::INT) {
                s64 token_id = GrapaInt(item->mValue).LongValue();
                mContextTokens.push_back((llama_token)token_id);
            }
            item = item->Next();
        }
    }
    else if (context->mValue.mToken == GrapaTokenType::STR) {
        // Handle $STR input (new behavior)
        GrapaCHAR textStr = context->mValue;
        return SetContextFromText(textStr);
    }
    else {
        return -1; // Invalid context format
    }

    // Rebuild the LLAMA.cpp context from tokens
    if (!mContextTokens.empty() && mMethod.StrCmp("llama-local") == 0) {
        // Process all tokens to rebuild the context
        struct llama_batch batch = llama_batch_get_one(mContextTokens.data(), (int32_t)mContextTokens.size());
        if (llama_decode(mLlamaContext, batch)) {
            return -3; // Failed to rebuild context
        }
        mContextPreserved = true;
        mContextInitialized = true;
    }

    return 0; // Success
}

GrapaError GrapaModel::SetContextFromText(const GrapaCHAR& text)
{
    if (!mLoaded || mMethod.StrCmp("llama-local") != 0 || !mLlamaModel) {
        return -1; // Model not loaded or not LLAMA backend
    }

    // Tokenize the text
    const struct llama_vocab* vocab = llama_model_get_vocab(mLlamaModel);
    std::vector<llama_token> new_tokens;

    size_t estimated_tokens = text.mLength / 4;  // Rough estimate: ~4 chars per token
    new_tokens.resize(estimated_tokens);

    int n_tokens = llama_tokenize(vocab, (char*)text.mBytes, (int32_t)text.mLength,
        new_tokens.data(), (int32_t)new_tokens.size(), true, false);
    if (n_tokens < 0) {
        new_tokens.resize(-n_tokens);
        n_tokens = llama_tokenize(vocab, (char*)text.mBytes, (int32_t)text.mLength,
            new_tokens.data(), (int32_t)new_tokens.size(), true, false);
    }
    new_tokens.resize(n_tokens);

    // Set the context tokens
    mContextTokens = new_tokens;

    // Rebuild the LLAMA.cpp context from tokens
    if (!mContextTokens.empty()) {
        struct llama_batch batch = llama_batch_get_one(mContextTokens.data(), (int32_t)mContextTokens.size());
        if (llama_decode(mLlamaContext, batch)) {
            return -2; // Failed to rebuild context
        }
        mContextPreserved = true;
        mContextInitialized = true;
    }

    return 0; // Success
}

void GrapaModel::ResetModelSpecificParams()
{
    // Reset model-dependent parameters
    if (!vParams) {
        return;  // No parameters to reset
    }

    mMaxTokens = 20;  // Very low for testing
    mContextSize = 2048;  // Default context size

    GrapaRuleEvent* override;
    s64 index;
    override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
    if (override)
        override->mValue.FROM(GrapaInt(mMaxTokens).getBytes());
    override = vParams->vQueue ? vParams->vQueue->Search("context_size", index) : nullptr;
    if (override)
        override->mValue.FROM(GrapaInt(mContextSize).getBytes());

    // Preserve user preference parameters
    // mTemperature, mTopK, mTopP, mRepeatPenalty, mSeed stay the same
}

void GrapaModel::SetModelDefaults()
{
    // Set reasonable defaults for this specific model
    if (!vParams) {
        return;  // No parameters to set defaults for
    }

    if (mMethod.StrCmp("llama-local") == 0) {
        // Get model info and set appropriate defaults
        int modelSize = GetModelSize();  // 7B, 13B, 70B, etc.
        GrapaRuleEvent* override;
        s64 index;

        if (modelSize <= 7) {
            override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaInt(20).getBytes());
            override = vParams->vQueue ? vParams->vQueue->Search("temperature", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaFloat((double)0.1f).getBytes());
        }
        else if (modelSize <= 13) {
            override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaInt(100).getBytes());
            override = vParams->vQueue ? vParams->vQueue->Search("temperature", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaFloat((double)0.1f).getBytes());
        }
        else {
            override = vParams->vQueue ? vParams->vQueue->Search("max_tokens", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaInt(200).getBytes());
            override = vParams->vQueue ? vParams->vQueue->Search("temperature", index) : nullptr;
            if (override)
                override->mValue.FROM(GrapaFloat((double)0.1f).getBytes());
        }
    }

}

GrapaRuleEvent* GrapaModel::MergeParams(GrapaRuleEvent* persistent, GrapaRuleEvent* callSpecific)
{
    GrapaRuleEvent* result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::GOBJ;

    // Check if persistent is null to prevent crash
    if (!persistent) {
        result->vQueue = new GrapaRuleQueue();  // Create empty queue
        return result;
    }

    result->vQueue = vScriptExec->CopyQueue(persistent->vQueue);

    // Check if callSpecific is null to prevent crash
    if (!callSpecific) {
        return result;
    }

    GrapaRuleEvent* current = callSpecific->vQueue ? callSpecific->vQueue->Head() : nullptr;
    while (current) {
        s64 index;
        GrapaRuleEvent* override = result->vQueue ? result->vQueue->Search(current->mName, index) : nullptr;
        if (override)
            vScriptExec->AssignValue(vNameSpace, override, current, NULL);
        else
            result->vQueue->PushTail(vScriptExec->CopyItem(current));
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

    // Track if any sampling-related parameters changed
    bool samplerParamsChanged = false;

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
                samplerParamsChanged = true;  // Temperature affects sampler
            }
        }
        else if (params->mName.StrCmp("top_k") == 0) {
            if (params->mValue.mToken == GrapaTokenType::INT) {
                mTopK = GrapaInt(params->mValue).LongValue();
                samplerParamsChanged = true;  // Top-K affects sampler
            }
        }
        else if (params->mName.StrCmp("top_p") == 0) {
            if (params->mValue.mToken == GrapaTokenType::FLOAT) {
                mTopP = GrapaFloat(params->mValue).ToDouble();
                samplerParamsChanged = true;  // Top-P affects sampler
            }
        }
        else if (params->mName.StrCmp("repeat_penalty") == 0) {
            if (params->mValue.mToken == GrapaTokenType::FLOAT) {
                mRepeatPenalty = GrapaFloat(params->mValue).ToDouble();
                // Note: repeat_penalty not currently used in sampler, but could be added
            }
        }
        else if (params->mName.StrCmp("seed") == 0) {
            if (params->mValue.mToken == GrapaTokenType::INT) {
                mSeed = GrapaInt(params->mValue).LongValue();
                samplerParamsChanged = true;  // Seed affects sampler
            }
        }
        else if (params->mName.StrCmp("verbose") == 0) {
            if (params->mValue.mToken == GrapaTokenType::INT) {
                mVerbose = GrapaInt(params->mValue).LongValue();
                // Apply logging callback immediately with new verbose level
                llama_log_set(LogCallback, (void*)&mVerbose);
                // verbose does NOT affect sampler - no need to reinitialize
            }
        }

        params = params->Next();
    }

    // Only reinitialize sampler if sampling-related parameters changed and model is loaded
    if (samplerParamsChanged && mLlamaContext && mLlamaSampler) {
        InitializeSampler();  // This will clean up and recreate the sampler with new parameters
    }

    return 0;
}

void GrapaModel::LogCallback(enum ggml_log_level level, const char* text, void* user_data)
{
    if (user_data == nullptr) {
        // If no user data, output to stderr (default behavior)
        fprintf(stderr, "%s", text);
        return;
    }

    s32* verbose_level = (s32*)user_data;

    // Map verbose levels to ggml_log_level
    // verbose=0: silent (no output)
    // verbose=1: errors only
    // verbose=2: warnings and errors
    // verbose=3: info, warnings, and errors
    // verbose=4: debug, info, warnings, and errors

    bool should_output = false;

    switch (*verbose_level) {
    case 0: // Silent
        should_output = false;
        break;
    case 1: // Errors only
        should_output = (level == GGML_LOG_LEVEL_ERROR);
        break;
    case 2: // Warnings and errors
        should_output = (level == GGML_LOG_LEVEL_WARN || level == GGML_LOG_LEVEL_ERROR);
        break;
    case 3: // Info, warnings, and errors
        should_output = (level == GGML_LOG_LEVEL_INFO || level == GGML_LOG_LEVEL_WARN || level == GGML_LOG_LEVEL_ERROR);
        break;
    case 4: // Debug and above
        should_output = (level >= GGML_LOG_LEVEL_DEBUG);
        break;
    default: // Default to silent for invalid levels
        should_output = false;
        break;
    }

    if (should_output) {
        // Output the message to stderr
        fprintf(stderr, "%s", text);
    }
}

int GrapaModel::GetModelSize()
{
    // Estimate model size from filename or model metadata
    // This is a simple heuristic - in practice you'd get this from the model file
    // Check for model size indicators in the path (case-insensitive)
    std::string path((char*)mModelPath.mBytes, mModelPath.mLength);
    std::transform(path.begin(), path.end(), path.begin(), ::tolower);

    if (path.find("70b") != std::string::npos) return 70;
    if (path.find("30b") != std::string::npos) return 30;
    if (path.find("13b") != std::string::npos) return 13;
    if (path.find("7b") != std::string::npos) return 7;

    return 7;  // Default to 7B
}

GrapaError GrapaModel::InitializeSampler()
{
    if (!mLlamaContext) {
        return -1;  // Context not loaded
    }

    // Clean up existing sampler
    CleanupSampler();

    // Create sampler chain with default parameters
    auto sparams = llama_sampler_chain_default_params();
    mLlamaSampler = llama_sampler_chain_init(sparams);

    if (!mLlamaSampler) {
        return -2;  // Failed to create sampler
    }

    // Add samplers to the chain based on current parameters
    // Top-K sampling
    if (mTopK > 0) {
        struct llama_sampler* top_k_sampler = llama_sampler_init_top_k(mTopK);
        if (top_k_sampler) {
            llama_sampler_chain_add(mLlamaSampler, top_k_sampler);
        }
    }

    // Top-P sampling
    if (mTopP > 0.0f && mTopP < 1.0f) {
        struct llama_sampler* top_p_sampler = llama_sampler_init_top_p(mTopP, 1);
        if (top_p_sampler) {
            llama_sampler_chain_add(mLlamaSampler, top_p_sampler);
        }
    }

    // Temperature sampling
    if (mTemperature > 0.0f) {
        struct llama_sampler* temp_sampler = llama_sampler_init_temp(mTemperature);
        if (temp_sampler) {
            llama_sampler_chain_add(mLlamaSampler, temp_sampler);
        }
    }

    // End with distribution sampler (greedy if temperature <= 0, otherwise random)
    struct llama_sampler* dist_sampler;
    if (mTemperature <= 0.0f) {
        dist_sampler = llama_sampler_init_greedy();
    }
    else {
        // Use seed if provided, otherwise use random seed
        uint32_t seed = (mSeed >= 0) ? (uint32_t)mSeed : (uint32_t)time(nullptr);
        dist_sampler = llama_sampler_init_dist(seed);
    }

    if (dist_sampler) {
        llama_sampler_chain_add(mLlamaSampler, dist_sampler);
    }

    return 0;  // Success
}

void GrapaModel::CleanupSampler()
{
    if (mLlamaSampler) {
        llama_sampler_free(mLlamaSampler);
        mLlamaSampler = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
