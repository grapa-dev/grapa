# $MODEL Data Type

The `$MODEL` data type provides AI/ML model integration capabilities in Grapa, currently supporting LLAMA.cpp-based models for text generation.

## Overview

The `$MODEL` type allows you to:
- Load and manage AI/ML models (currently GGUF format)
- Generate text using loaded models
- Configure generation parameters
- Control model verbosity and logging
- Unload models to free memory

## Basic Usage

```grapa
/* Create a new model instance */
model = $MODEL();

/* Load a model file */
result = model.load("path/to/model.gguf");

/* Generate text */
response = model.gen("Hello, how are you?");

/* Get model information */
info = model.info();

/* Unload the model */
model.load();
```

## Methods

### `.load(modelpath, method)`
Loads a model from the specified file path.

**Parameters:**
- `path` (string): Path to the model file (GGUF format)
- `method` (string, optional): Method to use ("llama" is currently the only supported method). If not specified, the method will be auto-detected from the file extension or magic bytes.

**Returns:** Error code (0 for success)

**Example:**
```grapa
/* Auto-detect method from file extension */
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
if (result.type() != $ERR) {
    "Model loaded successfully!".echo();
} else {
    ("Failed to load model: " + result.str()).echo();
}

/* Explicitly specify method */
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf", "llama");
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

/* Generation with custom parameters */
response = model.gen("Explain machine learning", {
    "temperature": 0.8,
    "max_tokens": 100
});
```

### `.info()`
Returns information about the current model state.

**Returns:** Object containing model information

**Example:**
```grapa
info = model.info();
("Model loaded: " + info."loaded".str()).echo();
("Method: " + info."method".str()).echo();
("Model path: " + info."model_path".str()).echo();
("Model name: " + info."model".str()).echo();
```

### `.params()`
Returns current generation parameters.

**Returns:** Object containing all current parameters

**Example:**
```grapa
params = model.params();
("Current temperature: " + params."temperature".str()).echo();
("Max tokens: " + params."max_tokens".str()).echo();
```

### `.params(parameters)`
Sets generation parameters using a `$GOBJ` collection.

**Parameters:**
- `parameters` (`$GOBJ`): Collection containing parameter name-value pairs

**Example:**
```grapa
model.params({
    "temperature": 0.7,
    "max_tokens": 50,
    "verbose": 2
});
```

### `.context()`
Gets the current context state of the model.

**Returns:** Object containing context information:
- `text` (string): Current text context
- `tokens` (list): Current token context (for efficient method processing)
- `method` (string): Method being used
- `model` (string): Model filename

**Example:**
```grapa
context = model.context();
("Context text: " + context."text".str()).echo();
("Context tokens: " + context."tokens".len().str() + " tokens").echo();
```

### `.context(contextData)`
Sets the context for the model.

**Parameters:**
- `contextData` (string, list, or object): Context data to set
  - If string: Sets text context
  - If list: Sets token context (for efficient processing)
  - If object: Can contain both "text" and "tokens" fields

**Example:**
```grapa
/* Set text context */
model.context("Previous conversation: Hello, how are you?");

/* Set token context (more efficient) */
tokenList = [1, 2, 3, 4, 5];
model.context(tokenList);

/* Set both text and tokens */
model.context({
    "text": "Previous conversation",
    "tokens": [1, 2, 3, 4, 5]
});
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

The following parameters can be set using `.params()`:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `temperature` | float | 0.7 | Controls randomness (0.0 = deterministic, 1.0+ = more random) |
| `max_tokens` | int | 10 | Maximum number of tokens to generate |
| `top_k` | int | 40 | Number of top tokens to consider |
| `top_p` | float | 0.9 | Nucleus sampling threshold |
| `repeat_penalty` | float | 1.1 | Penalty for repeating tokens |
| `seed` | int | -1 | Random seed (-1 for random) |
| `context_size` | int | 2048 | Context window size |
| `verbose` | int | 0 | Logging verbosity (0=silent, 1=errors, 2=warnings, 3=info, 4=debug) |

## Method Support

Currently, the `$MODEL` type supports:

- **LLAMA.cpp**: For GGUF format models
  - Supports various quantization levels (Q2_K, Q3_K_M, Q4_K_M, Q5_K_M, Q6_K, Q8_0, FP16)
  - Optimized for CPU inference
  - Thread-safe operations
  - Auto-detected from `.gguf` file extension or "GGUF" magic bytes

## Model Formats

The `$MODEL` type currently supports:

- **GGUF**: The standard format for LLAMA.cpp models
  - Efficient quantization
  - Cross-platform compatibility
  - Metadata inclusion
  - Auto-detected by file extension (`.gguf`) or magic bytes ("GGUF")

## Performance Considerations

- **Memory Usage**: Large models (7B+ parameters) require significant RAM
- **Loading Time**: Initial model loading can take several seconds
- **Generation Speed**: Depends on model size and hardware capabilities
- **Context Size**: Larger context windows use more memory

## Error Handling

Always check return values from `.load()` and `.unload()`:

```grapa
model = $MODEL();
/* Auto-detect method from file extension */
result = model.load("model.gguf");
if (result != 0) {
    ("Error loading model: " + result.str()).echo();
    return;
}

/* Use model safely */
response = model.gen("Hello");
model.load(); /* Unload when done */
```

## Thread Safety

The `$MODEL` type is designed to be thread-safe, allowing multiple model instances to be used concurrently in different threads.

## See Also

- [Model Examples](../examples/model_examples.grc) - Practical usage examples
- [Model Download Guide](../examples/model_download.md) - How to download models
- [API Reference](../api/model.md) - Complete API documentation
