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

    if (pParams)
    {
        vParams = pParams;

        vParams->CLEAR();
        vParams->mValue.mToken = GrapaTokenType::MODEL;
        vParams->vQueue = new GrapaRuleQueue();

    }
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

    mModelPath.FROM(modelPath);
    mMethod.FROM(method);

    if (mModelPath.mLength == 0) {
        return 0; // Could not detect model path
    }

    mMethod.FROM(method);

    if (mMethod.StrCmp("openai") == 0) {
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

GrapaError GrapaModel::Unload()
{
    GrapaError result = 0;

    if (mLoaded) {
        if (mMethod.StrCmp("openai") == 0) {
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

    if (mMethod.StrCmp("openai") == 0) {
        result = GenerateOpenAI(prompt, mergedParams);
    }
    else if (mMethod.StrCmp("openai-embedding") == 0) {
        result = EmbedOpenAI(prompt, mergedParams);
    }

    delete mergedParams;
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

        // Add model path
        GrapaRuleEvent* modelPath = new GrapaRuleEvent(0, GrapaCHAR("model_path"), mModelPath);
        result->vQueue->PushTail(modelPath);

        return result;
    }

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

    return 0; // Success
}

GrapaError GrapaModel::SetContextFromText(const GrapaCHAR& text)
{
    if (!mLoaded) {
        return -1; // Model not loaded or not LLAMA backend
    }

    return 0; // Success
}

void GrapaModel::ResetModelSpecificParams()
{
    // Reset model-dependent parameters
    if (!vParams) {
        return;  // No parameters to reset
    }

    // Preserve user preference parameters
    // mTemperature, mTopK, mTopP, mRepeatPenalty, mSeed stay the same
}

void GrapaModel::SetModelDefaults()
{
    // Set reasonable defaults for this specific model
    if (!vParams) {
        return;  // No parameters to set defaults for
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



////////////////////////////////////////////////////////////////////////////////
