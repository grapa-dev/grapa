// GrapaModel.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaModel_
#define _GrapaModel_

#include "GrapaFloat.h"
#include "GrapaState.h"
#include "GrapaNetConnect.h"

// Forward declarations
class GrapaRuleEvent;

class GrapaModel : public GrapaCritical
{
public:
    // Model state
    bool mLoaded;
    GrapaCHAR mModelPath;
    GrapaCHAR mMethod;  // "openai", etc.
    
    // OpenAI context management
    GrapaCHAR mOpenAIResponseId;  // OpenAI: Response ID for context continuity

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
    
    // OpenAI cloud model operations
    GrapaError LoadOpenAI(const GrapaCHAR& modelPath);
    GrapaError UnloadOpenAI();
    GrapaRuleEvent* GenerateOpenAI(const GrapaCHAR& prompt, GrapaRuleEvent* mergedParams);
    GrapaRuleEvent* EmbedOpenAI(const GrapaCHAR& text, GrapaRuleEvent* mergedParams);
    
private:
    // Parameter management
    void ResetModelSpecificParams();
    void SetModelDefaults();
    GrapaRuleEvent* MergeParams(GrapaRuleEvent* persistent, GrapaRuleEvent* callSpecific);

};

#endif //_GrapaModel_

////////////////////////////////////////////////////////////////////////////////
