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
model.unload();
```

## Methods

### `.load(path, backend)`
Loads a model from the specified file path.

**Parameters:**
- `path` (string): Path to the model file (GGUF format)
- `backend` (string, optional): Backend to use ("llama" is currently the only supported backend)

**Returns:** Error code (0 for success)

**Example:**
```grapa
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf", "llama");
if (result == 0) {
    "Model loaded successfully!".echo();
} else {
    ("Failed to load model: " + result.str()).echo();
}
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
("Model loaded: " + info.loaded.str()).echo();
("Backend: " + info.backend.str()).echo();
("Path: " + info.path.str()).echo();
```

### `.params()`
Returns current generation parameters.

**Returns:** Object containing all current parameters

**Example:**
```grapa
params = model.params();
("Current temperature: " + params.temperature.str()).echo();
("Max tokens: " + params.max_tokens.str()).echo();
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

### `.echo(text)`
Echoes text to output (useful for debugging and status messages).

**Parameters:**
- `text` (string): Text to echo

**Example:**
```grapa
model.echo("Model is ready for generation");
```

### `.unload()`
Unloads the current model and frees memory.

**Returns:** Error code (0 for success)

**Example:**
```grapa
result = model.unload();
if (result == 0) {
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

## Backend Support

Currently, the `$MODEL` type supports:

- **LLAMA.cpp**: For GGUF format models
  - Supports various quantization levels (Q2_K, Q3_K_M, Q4_K_M, Q5_K_M, Q6_K, Q8_0, FP16)
  - Optimized for CPU inference
  - Thread-safe operations

## Model Formats

The `$MODEL` type currently supports:

- **GGUF**: The standard format for LLAMA.cpp models
  - Efficient quantization
  - Cross-platform compatibility
  - Metadata inclusion

## Performance Considerations

- **Memory Usage**: Large models (7B+ parameters) require significant RAM
- **Loading Time**: Initial model loading can take several seconds
- **Generation Speed**: Depends on model size and hardware capabilities
- **Context Size**: Larger context windows use more memory

## Error Handling

Always check return values from `.load()` and `.unload()`:

```grapa
model = $MODEL();
result = model.load("model.gguf");
if (result != 0) {
    ("Error loading model: " + result.str()).echo();
    return;
}

/* Use model safely */
response = model.gen("Hello");
```

## Thread Safety

The `$MODEL` type is designed to be thread-safe, allowing multiple model instances to be used concurrently in different threads.

## See Also

- [Model Examples](../examples/model_examples.md) - Practical usage examples
- [Model Download Guide](../examples/model_download.md) - How to download models
- [API Reference](../api/model.md) - Complete API documentation
