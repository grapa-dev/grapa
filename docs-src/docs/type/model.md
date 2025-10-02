# $MODEL Data Type

The `$MODEL` data type provides AI/ML model integration capabilities in Grapa, supporting cloud-based OpenAI models for text generation and embeddings.

## Overview

The `$MODEL` type allows you to:
- Load and manage OpenAI models for text generation and embeddings
- Generate text using OpenAI models
- Create text embeddings using OpenAI models
- Configure API key and context storage
- Unload models to free memory
- Use cloud-based models with persistent context

## Basic Usage

```grapa
/* Create a new model instance */
model = $MODEL();

/* Load an OpenAI model */
result = model.load("gpt-4o", "openai");

/* Generate text with API key */
response = model.gen("Hello, how are you?", {
    "api_key": "sk-your-openai-api-key"
});

/* Get model information */
info = model.info();

/* Unload the model */
model.load();
```

## Methods

### `.load(modelname, method)`
Loads an OpenAI model.

**Parameters:**
- `modelname` (string): OpenAI model name (e.g., "gpt-4o", "gpt-3.5-turbo", "text-embedding-3-small")
- `method` (string): Method to use ("openai" for text generation, "openai-embedding" for embeddings)

**Returns:** Error code (0 for success)

**Example:**
```grapa
/* Load OpenAI text generation model */
result = model.load("gpt-4o", "openai");
if (result.type() != $ERR) {
    "Model loaded successfully!".echo();
} else {
    ("Failed to load model: " + result.str()).echo();
}

/* Load OpenAI embedding model */
result = model.load("text-embedding-3-small", "openai-embedding");
```

### `.gen(prompt, params)`
Generates text using the loaded model.

**Parameters:**
- `prompt` (string): Input text prompt
- `params` (object, optional): Generation parameters to override defaults

**Returns:** Generated text string

**Example:**
```grapa
/* Basic generation */
response = model.gen("What is artificial intelligence?");

/* Generation with store parameter */
response = model.gen("Explain machine learning", {
    "api_key": "sk-your-openai-api-key",
    "store": false
});

/* Generation with OpenAI API key (cloud model) */
response = model.gen("Hello, how are you?", {
    "api_key": "sk-your-openai-api-key"
});
```

### `.info()`
Returns information about the current model state.

**Returns:** Object containing model information:
- `loaded` (boolean): Whether the model is currently loaded
- `method` (string): The method being used (e.g., "openai", "openai-embedding")
- `model_path` (string): The model name or path

**Example:**
```grapa
info = model.info();
("Model loaded: " + info."loaded".str()).echo();
("Method: " + info."method".str()).echo();
("Model path: " + info."model_path".str()).echo();
```

### `.params()`
Returns current generation parameters.

**Returns:** Object containing all current parameters

**Example:**
```grapa
params = model.params();
("Current parameters: " + params.str()).echo();
```

### `.params(parameters)`
Sets generation parameters using a `$GOBJ` collection.

**Parameters:**
- `parameters` (`$GOBJ`): Collection containing parameter name-value pairs

**Example:**
```grapa
model.params({
    "api_key": "sk-your-openai-api-key",
    "store": true
});
```

### `.context()`
Gets the current context state of the model.

**Returns:** Object containing context information:
- `id` (string): Response ID for conversation continuity (OpenAI only)
- `method` (string): Method being used
- `model_path` (string): Model name or path

**Example:**
```grapa
context = model.context();
("Response ID: " + context."id".str()).echo();
("Method: " + context."method".str()).echo();
("Model path: " + context."model_path".str()).echo();
```

### `.context(contextData)`
Sets the context for the model.

**Parameters:**
- `contextData` (object): Context object containing an `id` field for conversation continuity

**Example:**
```grapa
/* Set context using response ID from previous conversation */
context = {
    "id": "response_12345",
    "method": "openai",
    "model_path": "gpt-4o"
};
model.context(context);
```

### `.load()`
Unloads the current model and frees memory by calling `.load()` with no parameters.

**Returns:** Error code (0 for success)

**Example:**
```grapa
result = model.load();
if (result.type() != $ERR) {
    "Model unloaded successfully".echo();
}
```

## Generation Parameters

**Note: Local model support has been removed. Only OpenAI parameters are supported.**

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `api_key` | string | ✅ | OpenAI API key for authentication |
| `store` | boolean | ❌ | Whether to store context for conversation continuity (default: true) |


## Method Support

Currently, the `$MODEL` type supports:

- **OpenAI**: For cloud-based models
  - Supports GPT-4, GPT-3.5-turbo, text-embedding-3-small, and other OpenAI models
  - Server-side context persistence
  - Requires API key authentication
  - Uses HTTPS with SSL certificate verification
  - Two methods: "openai" for text generation, "openai-embedding" for embeddings

## Performance Considerations

- **API Rate Limits**: OpenAI has rate limits based on your subscription tier
- **Network Latency**: Response time depends on network connection to OpenAI servers
- **Context Management**: Server-side context persistence reduces API calls
- **Cost**: Usage is billed per token, so consider prompt and response length

## Error Handling

Always check return values from `.load()` and `.unload()`:

```grapa
model = $MODEL();
/* Load OpenAI model */
result = model.load("gpt-4o", "openai");
if (result.type() != $ERR) {
    "Model loaded successfully".echo();
} else {
    ("Error loading model: " + result.str()).echo();
    return;
}

/* Use model safely */
response = model.gen("Hello", {
    "api_key": "sk-your-openai-api-key"
});
model.load(); /* Unload when done */
```

## Thread Safety

The `$MODEL` type is designed to be thread-safe, allowing multiple model instances to be used concurrently in different threads.

## OpenAI Usage Example

Here's a complete example of using OpenAI models:

```grapa
/* Load OpenAI model */
model = $MODEL();
result = model.load("gpt-4o", "openai");
if (result.type() != $ERR) {
    "OpenAI model loaded successfully!".echo();
} else {
    ("Failed to load OpenAI model: " + result.str()).echo();
    return;
}

/* Generate with API key */
response1 = model.gen("Hello, how are you?", {
    "api_key": "sk-your-openai-api-key"
});
response1.echo();

/* Continue conversation with context */
response2 = model.gen("What's the weather like?", {
    "api_key": "sk-your-openai-api-key"
});
response2.echo();

/* Disable context persistence */
response3 = model.gen("Tell me a joke", {
    "api_key": "sk-your-openai-api-key",
    "store": false
});
response3.echo();

/* Unload model */
model.load();
model = null;
```

## See Also

- [Model Examples](../examples/model_examples.grc) - Practical usage examples
- [Model Download Guide](../examples/model_download.md) - How to download models
- [API Reference](../api/model.md) - Complete API documentation
