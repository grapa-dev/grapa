# $MODEL API Reference

Complete API reference for the `$MODEL` data type in Grapa.

## Constructor

### `$MODEL()`
Creates a new model instance.

**Returns:** New `$MODEL` instance

**Example:**
```grapa
model = $MODEL();
```

## Methods

### `.load(path, backend)`

Loads a model from the specified file path.

**Parameters:**
- `path` (string): Path to the model file (GGUF format)
- `backend` (string, optional): Backend to use ("llama" is currently the only supported backend)

**Returns:** 
- `0` - Success
- `-1` - Model context creation failed
- `-2` - Model loading failed
- Other negative values - Various error conditions

**Example:**
```grapa
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf", "llama");
if (result == 0) {
    "Model loaded successfully".echo();
}
```

**Error Codes:**
- `-1`: Failed to create LLAMA context (usually memory or file issues)
- `-2`: Failed to load model (file not found, invalid format, etc.)

### `.gen(prompt, params)`

Generates text using the loaded model.

**Parameters:**
- `prompt` (string): Input text prompt for generation
- `params` (object, optional): Generation parameters to override defaults

**Returns:** Generated text string

**Example:**
```grapa
/* Basic generation */
response = model.gen("Hello, how are you?");

/* Generation with custom parameters */
response = model.gen("Explain AI", {
    "temperature": 0.8,
    "max_tokens": 100
});
```

**Generation Parameters:**
- `temperature` (float): Controls randomness (0.0 = deterministic, 1.0+ = more random)
- `max_tokens` (int): Maximum number of tokens to generate
- `top_k` (int): Number of top tokens to consider
- `top_p` (float): Nucleus sampling threshold
- `repeat_penalty` (float): Penalty for repeating tokens
- `seed` (int): Random seed (-1 for random)
- `context_size` (int): Context window size
- `verbose` (int): Logging verbosity level

### `.info()`

Returns information about the current model state.

**Returns:** Object containing:
- `loaded` (boolean): Whether the model is currently loaded
- `backend` (string): Backend being used ("llama")
- `path` (string): Path to the loaded model file

**Example:**
```grapa
info = model.info();
("Model loaded: " + info.loaded.str()).echo();
("Backend: " + info.backend.str()).echo();
("Path: " + info.path.str()).echo();
```

### `.params()`

Returns current generation parameters.

**Returns:** Object containing all current parameters:
- `temperature` (float): Current temperature setting
- `max_tokens` (int): Current max tokens setting
- `top_k` (int): Current top_k setting
- `top_p` (float): Current top_p setting
- `repeat_penalty` (float): Current repeat penalty setting
- `seed` (int): Current seed setting
- `context_size` (int): Current context size setting
- `verbose` (int): Current verbose level setting

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

**Returns:** None

**Example:**
```grapa
model.params({
    "temperature": 0.7,
    "max_tokens": 50,
    "verbose": 2
});
```

**Supported Parameters:**
- `"temperature"` (float): 0.0 to 2.0+
- `"max_tokens"` (int): 1 to 4096+
- `"top_k"` (int): 1 to 100+
- `"top_p"` (float): 0.0 to 1.0
- `"repeat_penalty"` (float): 0.0 to 2.0+
- `"seed"` (int): -1 for random, or any integer
- `"context_size"` (int): 512 to 131072+
- `"verbose"` (int): 0-4 (0=silent, 1=errors, 2=warnings, 3=info, 4=debug)

### `.echo(text)`

Echoes text to output stream.

**Parameters:**
- `text` (string): Text to echo

**Returns:** None

**Example:**
```grapa
model.echo("Model is ready for generation");
```

### `.unload()`

Unloads the current model and frees memory.

**Returns:**
- `0` - Success
- `-1` - Error during unload

**Example:**
```grapa
result = model.unload();
if (result == 0) {
    "Model unloaded successfully".echo();
}
```

## Default Parameters

When a new `$MODEL` instance is created, the following default parameters are set:

| Parameter | Default Value | Description |
|-----------|---------------|-------------|
| `temperature` | 0.7 | Moderate creativity |
| `max_tokens` | 10 | Short responses (good for testing) |
| `top_k` | 40 | Balanced token selection |
| `top_p` | 0.9 | Nucleus sampling threshold |
| `repeat_penalty` | 1.1 | Slight penalty for repetition |
| `seed` | -1 | Random seed |
| `context_size` | 2048 | Standard context window |
| `verbose` | 0 | Silent operation |

## Error Handling

### Common Error Scenarios

1. **Model Loading Errors:**
   ```grapa
   result = model.load("nonexistent.gguf");
   if (result != 0) {
       ("Load failed: " + result.str()).echo();
       /* Handle error appropriately */
   }
   ```

2. **Generation Errors:**
   ```grapa
   try {
       response = model.gen("Hello");
       if (response.len() == 0) {
           "Warning: Empty response".echo();
       }
   } catch (error) {
       ("Generation error: " + error.str()).echo();
   }
   ```

3. **Parameter Validation:**
   ```grapa
   /* Validate parameter ranges */
   if (temperature >= 0.0 && temperature <= 2.0) {
       model.params({"temperature": temperature});
   } else {
       "Invalid temperature value".echo();
   }
   ```

## Thread Safety

The `$MODEL` type is designed to be thread-safe:

- Multiple model instances can be used concurrently
- Each instance maintains its own state
- No shared global state between instances

**Example:**
```grapa
/* Safe to use multiple instances */
model1 = $MODEL();
model2 = $MODEL();

model1.load("model.gguf");
model2.load("model.gguf");

/* These can run concurrently */
response1 = model1.gen("Hello");
response2 = model2.gen("Hi");
```

## Memory Management

### Best Practices

1. **Always unload models when done:**
   ```grapa
   model = $MODEL();
   model.load("large_model.gguf");
   
   /* Use model... */
   
   model.unload();  /* Free memory */
   ```

2. **Check available memory before loading large models:**
   ```grapa
   /* Large models (7B+ parameters) require significant RAM */
   model = $MODEL();
   result = model.load("qwen2.5-7b-instruct-q3_k_m.gguf");
   if (result != 0) {
       "Insufficient memory for model".echo();
   }
   ```

3. **Use appropriate context sizes:**
   ```grapa
   /* Smaller context for memory-constrained environments */
   model.params({"context_size": 1024});
   ```

## Performance Considerations

### Optimization Tips

1. **Set appropriate parameters for your use case:**
   ```grapa
   /* For fast, short responses */
   model.params({
       "max_tokens": 20,
       "temperature": 0.3
   });
   
   /* For creative, longer responses */
   model.params({
       "max_tokens": 200,
       "temperature": 1.0
   });
   ```

2. **Disable verbose output for production:**
   ```grapa
   model.params({"verbose": 0});  /* Silent operation */
   ```

3. **Use smaller models for faster inference:**
   ```grapa
   /* Q2_K or Q3_K_M for speed */
   model.load("qwen2.5-7b-instruct-q2_k.gguf");
   ```

## Backend-Specific Information

### LLAMA.cpp Backend

The current implementation uses LLAMA.cpp as the backend:

- **Supported formats:** GGUF
- **Quantization levels:** Q2_K, Q3_K_M, Q4_K_M, Q5_K_M, Q6_K, Q8_0, FP16
- **Hardware acceleration:** CPU-optimized
- **Thread safety:** Yes

### Future Backends

The `$MODEL` type is designed to support multiple backends:

- **ONNX:** For cross-platform inference
- **TensorFlow:** For TensorFlow models
- **PyTorch:** For PyTorch models (via Python integration)

## See Also

- [$MODEL Data Type](../type/model.md) - Complete type reference
- [Model Examples](../examples/model_examples.md) - Practical usage examples
- [Model Download Guide](../examples/model_download.md) - How to download models
