# Model Examples

This page provides practical examples of using the `$MODEL` data type for AI/ML model integration in Grapa.

**Note:** The examples in this guide use `qwen2.5-7b-instruct-q3_k_m.gguf` for simplicity (single file download). For higher quality results, you can substitute `qwen2.5-7b-instruct-q5_k_m.gguf` (requires downloading and merging 2 files). See the [Model Download Guide](model_download.md) for instructions on downloading both options.

## Basic Model Usage

### Simple Text Generation

```grapa
/* Basic model loading and text generation */
model = $MODEL();
"Loading model...".echo();

result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
if (result == 0) {
    "Model loaded successfully!".echo();
    
    /* Generate a simple response */
    response = model.gen("Hello, how are you?");
    ("Response: " + response.str()).echo();
} else {
    ("Failed to load model: " + result.str()).echo();
}

/* Clean up */
model.unload();
```

### Model Information

```grapa
/* Get detailed model information */
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

info = model.info();
("Model loaded: " + info.loaded.str()).echo();
("Backend: " + info.backend.str()).echo();
("Model path: " + info.path.str()).echo();

/* Display current parameters */
params = model.params();
("Current parameters:").echo();
("  Temperature: " + params.temperature.str()).echo();
("  Max tokens: " + params.max_tokens.str()).echo();
("  Context size: " + params.context_size.str()).echo();
```

## Parameter Configuration

### Setting Generation Parameters

```grapa
/* Configure model parameters before loading */
model = $MODEL();

/* Set generation parameters */
model.params({
    "temperature": 0.8,      /* More creative responses */
    "max_tokens": 100,       /* Longer responses */
    "top_k": 50,             /* More diverse token selection */
    "verbose": 1             /* Show errors only */
});

"Parameters configured".echo();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Generate with configured parameters */
response = model.gen("Write a short story about a robot");
("Story: " + response.str()).echo();
```

### Dynamic Parameter Override

```grapa
/* Override parameters for specific generation calls */
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Conservative generation */
conservative_response = model.gen("Explain quantum computing", {
    "temperature": 0.3,
    "max_tokens": 50
});

/* Creative generation */
creative_response = model.gen("Write a poem about space", {
    "temperature": 1.2,
    "max_tokens": 150
});

("Conservative: " + conservative_response.str()).echo();
("Creative: " + creative_response.str()).echo();
```

## Verbose Control

### Debugging with Verbose Output

```grapa
/* Enable verbose output for debugging */
model = $MODEL();
model.params({"verbose": 4});  /* Maximum verbosity */

"Loading model with debug output...".echo();
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

if (result == 0) {
    "Model loaded, generating with debug info...".echo();
    response = model.gen("What is machine learning?");
    ("Response: " + response.str()).echo();
} else {
    ("Load failed with error: " + result.str()).echo();
}
```

### Silent Operation

```grapa
/* Silent operation for production use */
model = $MODEL();
model.params({"verbose": 0});  /* No output */

model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
response = model.gen("Hello");
("Response: " + response.str()).echo();
```

## Multiple Model Instances

### Concurrent Model Usage

```grapa
/* Create multiple model instances */
model1 = $MODEL();
model2 = $MODEL();

/* Configure different parameters for each */
model1.params({"temperature": 0.5});
model2.params({"temperature": 1.0});

/* Load the same model in both instances */
model1.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
model2.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Generate with different styles */
conservative = model1.gen("Explain AI");
creative = model2.gen("Explain AI");

("Conservative: " + conservative.str()).echo();
("Creative: " + creative.str()).echo();

/* Clean up both models */
model1.unload();
model2.unload();
```

## Error Handling

### Robust Error Handling

```grapa
/* Comprehensive error handling example */
model = $MODEL();

/* Try to load a model */
result = model.load("nonexistent/model.gguf");
if (result != 0) {
    ("Error loading model: " + result.str()).echo();
    ("This could be due to:").echo();
    ("  - File not found").echo();
    ("  - Invalid model format").echo();
    ("  - Insufficient memory").echo();
    return;
}

/* Try to generate text */
try {
    response = model.gen("Hello");
    if (response.len() == 0) {
        "Warning: Empty response generated".echo();
    } else {
        ("Generated: " + response.str()).echo();
    }
} catch (error) {
    ("Generation error: " + error.str()).echo();
}

/* Always clean up */
unload_result = model.unload();
if (unload_result != 0) {
    ("Warning: Error unloading model: " + unload_result.str()).echo();
}
```

## Practical Applications

### Chat Bot Implementation

```grapa
/* Simple chat bot using $MODEL */
chatbot = $MODEL();
chatbot.params({
    "temperature": 0.7,
    "max_tokens": 200,
    "verbose": 0
});

"Loading chatbot model...".echo();
result = chatbot.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

if (result == 0) {
    "Chatbot ready! Type 'quit' to exit".echo();
    
    /* Simulate chat loop */
    user_input = "Hello, what can you help me with?";
    while (user_input != "quit") {
        response = chatbot.gen(user_input);
        ("Bot: " + response.str()).echo();
        
        /* In a real implementation, you would get user input here */
        user_input = "quit";  /* Exit after first response for demo */
    }
} else {
    ("Failed to load chatbot: " + result.str()).echo();
}

chatbot.unload();
```

### Content Generation

```grapa
/* Content generation with different styles */
content_generator = $MODEL();
content_generator.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Technical writing */
technical = content_generator.gen("Explain how neural networks work", {
    "temperature": 0.3,
    "max_tokens": 150
});

/* Creative writing */
creative = content_generator.gen("Write a creative story about AI", {
    "temperature": 1.1,
    "max_tokens": 200
});

("Technical: " + technical.str()).echo();
("Creative: " + creative.str()).echo();

content_generator.unload();
```

## Performance Optimization

### Efficient Model Usage

```grapa
/* Optimize model usage for better performance */
model = $MODEL();

/* Set optimal parameters for your use case */
model.params({
    "context_size": 1024,    /* Smaller context for faster processing */
    "max_tokens": 50,        /* Limit response length */
    "verbose": 0             /* Disable logging for speed */
});

"Loading optimized model...".echo();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Batch multiple generations */
prompts = ["What is AI?", "Explain ML", "Define deep learning"];
responses = [];

for (i = 0; i < prompts.len(); i++) {
    response = model.gen(prompts[i]);
    responses.append(response);
    ("Response " + (i+1).str() + ": " + response.str()).echo();
}

model.unload();
```

## See Also

- [$MODEL Data Type](../type/model.md) - Complete type reference
- [Model Download Guide](model_download.md) - How to download models
- [API Reference](../api/model.md) - Complete API documentation
