# Model Usage Guide

This comprehensive guide covers all the different ways to use the `$MODEL` data type in Grapa, from basic usage to advanced patterns and real-world applications.

## Table of Contents

1. [Basic Usage Patterns](#basic-usage-patterns)
2. [Context Management](#context-management)
3. [Cross-CLI Context Persistence](#cross-cli-context-persistence)
4. [Multiple Model Instances](#multiple-model-instances)
5. [Performance Optimization](#performance-optimization)
6. [Real-world Use Cases](#real-world-use-cases)
7. [Consistent Metadata Structure](#consistent-metadata-structure)
8. [Best Practices](#best-practices)

## Basic Usage Patterns

### Simple Text Generation

The most basic pattern for using `$MODEL`:

```grapa
/* Create and load a model */
model = $MODEL();
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

if (result.type() != $ERR) {
    /* Generate text */
    response = model.gen("Hello, how are you?");
    
    /* Response is a $GOBJ with 'text' and 'metadata' fields */
    ("Response type: " + response.type().str() + "\n").echo();
    ("Response keys: " + response.keys().str() + "\n").echo();
    
    /* Access the generated text */
    text = response."text";
    ("Generated text: " + text.str() + "\n").echo();
    
    /* Access metadata */
    metadata = response."metadata";
    ("Model path: " + metadata."model_path".str() + "\n").echo();
    ("Total tokens: " + metadata."total_tokens".str() + "\n").echo();
    
    /* Clean up */
    model.load();
} else {
    ("Failed to load model: " + result.str() + "\n").echo();
}
```

### Parameter Configuration

Configure generation parameters before or after loading:

```grapa
model = $MODEL();

/* Configure parameters before loading */
model.params({
    "temperature": 0.8,
    "max_tokens": 100,
    "verbose": 0
});

model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Generate with configured parameters */
response = model.gen("Write a short story");
("Story: " + response.str() + "\n").echo();

model.load();
```

### Dynamic Parameter Override

Override parameters for specific generation calls:

```grapa
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Conservative generation */
conservative = model.gen("Explain AI", {
    "temperature": 0.3,
    "max_tokens": 50
});

/* Creative generation */
creative = model.gen("Write a poem", {
    "temperature": 1.2,
    "max_tokens": 150
});

("Conservative: " + conservative.str() + "\n").echo();
("Creative: " + creative.str() + "\n").echo();

model.load();
```

## Context Management

### Basic Context Usage

Context allows the model to maintain conversation history and state:

```grapa
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Set initial context */
model.context("You are a helpful assistant. Previous conversation: User asked about AI.");

/* Generate with context */
response1 = model.gen("What is machine learning?");
("Response 1: " + response1.str() + "\n").echo();

/* Continue conversation with preserved context */
response2 = model.gen("Can you explain that in simpler terms?");
("Response 2: " + response2.str() + "\n").echo();

model.load();
```

### Efficient Token Context

For better performance, use token context instead of text:

```grapa
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Set context using tokens (more efficient) */
token_context = [1, 2, 3, 4, 5];  /* Example token IDs */
model.context(token_context);

/* Generate with token context */
response = model.gen("Continue the conversation");
("Response: " + response.str() + "\n").echo();

model.load();
```

### Context Transfer Between Models

Transfer context from one model instance to another:

```grapa
/* Create two model instances */
model1 = $MODEL();
model2 = $MODEL();

/* Load the same model in both instances */
model1.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
model2.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Set up context in first model */
model1.context("Previous conversation: User asked about quantum computing.");

/* Generate response in first model */
response1 = model1.gen("What is quantum computing?");
("Model 1 Response: " + response1.str() + "\n").echo();

/* Get context from first model */
context_data = model1.context();
("Context from model1: " + context_data."text".str() + "\n").echo();

/* Transfer context to second model */
model2.context(context_data);

/* Continue conversation in second model */
response2 = model2.gen("Can you give me a simple example?");
("Model 2 Response: " + response2.str() + "\n").echo();

/* Clean up */
model1.load();
model2.load();
```

## Cross-CLI Context Persistence

### Save Context to File

Save context to persist across CLI sessions:

```grapa
/* Save context function */
save_context = op(model, filename) {
    context = model.context();
    
    /* Create context data structure */
    context_data = {
        "text": context."text",
        "tokens": context."tokens",
        "method": context."method",
        "model": context."model",
        "timestamp": $TIME().utc().str()
    };
    
    /* Save to file */
    $file(filename).set(context_data.str());
    ("Context saved to: " + filename + "\n").echo();
};

/* Load context function */
load_context = op(model, filename) {
    if ($file(filename).exists()) {
        context_str = $file(filename).get();
        /* Parse and set context - convert back to $GOBJ */
        context_data = op()(context_str)();
        model.context(context_data);
        ("Context loaded from: " + filename + "\n").echo();
        return true;
    } else {
        ("No context file found: " + filename + "\n").echo();
        return false;
    };
};

/* Usage example */
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Try to load existing context */
if (!load_context(model, "conversation_context.txt")) {
    /* No existing context, start fresh */
    model.context("You are a helpful assistant.");
};

/* Have a conversation */
response = model.gen("Hello, I'd like to learn about machine learning.");

/* Access the generated text and metadata */
text = response."text";
metadata = response."metadata";

("Response: " + text.str() + "\n").echo();
("Model: " + metadata."model_path".str() + "\n").echo();
("Tokens used: " + metadata."total_tokens".str() + "\n").echo();

/* Save context for next session */
save_context(model, "conversation_context.txt");

model.load();
```

### Token-Based Context Persistence

For maximum efficiency, save and load token context:

```grapa
/* Save text-only context function (alternative approach) */
save_text_context = op(model, filename) {
    context = model.context();
    if (context."text".len() > 0) {
        /* Save just the text */
        $file(filename).set(context."text".str());
        ("Text context saved: " + context."text".len().str() + " characters\n").echo();
    } else {
        ("No text context to save\n").echo();
    };
};

/* Load text-only context function */
load_text_context = op(model, filename) {
    if ($file(filename).exists()) {
        text_str = $file(filename).get();
        /* Set context from text string */
        model.context(text_str);
        ("Text context loaded from: " + filename + "\n").echo();
        return true;
    } else {
        ("No text context file found: " + filename + "\n").echo();
        return false;
    };
};

/* Save token context function */
save_token_context = op(model, filename) {
    context = model.context();
    if (context."tokens".len() > 0) {
        /* Save just the tokens */
        $file(filename).set(context."tokens".str());
        ("Token context saved: " + context."tokens".len().str() + " tokens\n").echo();
    } else {
        ("No token context to save\n").echo();
    };
};

/* Load token context function */
load_token_context = op(model, filename) {
    if ($file(filename).exists()) {
        token_str = $file(filename).get();
        /* Parse token list and set context - convert back to $LIST */
        token_list = op()(token_str)();
        model.context(token_list);
        ("Token context loaded from: " + filename + "\n").echo();
        return true;
    } else {
        ("No token context file found: " + filename + "\n").echo();
        return false;
    };
};

/* Usage example - try text context first, then token context */
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Try to load existing text context first (simpler approach) */
if (!load_text_context(model, "text_context.txt")) {
    /* Try token context if text context not found */
    if (!load_token_context(model, "token_context.txt")) {
        /* No existing context, start with text */
        model.context("You are a helpful assistant.");
    };
};

/* Continue conversation */
response = model.gen("What are the benefits of using token context?");
("Response: " + response.str() + "\n").echo();

/* Save both text and token context for next session */
save_text_context(model, "text_context.txt");
save_token_context(model, "token_context.txt");

model.load();
```

## Multiple Model Instances

### Concurrent Model Usage

Use multiple model instances with different configurations:

```grapa
/* Create multiple model instances */
conservative_model = $MODEL();
creative_model = $MODEL();

/* Configure different parameters */
conservative_model.params({
    "temperature": 0.3,
    "max_tokens": 50,
    "verbose": 0
});

creative_model.params({
    "temperature": 1.2,
    "max_tokens": 200,
    "verbose": 0
});

/* Load the same model in both instances */
conservative_model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
creative_model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Generate with different styles */
prompt = "Write a story about space exploration";

conservative_story = conservative_model.gen(prompt);
creative_story = creative_model.gen(prompt);

("Conservative Story: " + conservative_story.str() + "\n").echo();
("Creative Story: " + creative_story.str() + "\n").echo();

/* Clean up */
conservative_model.load();
creative_model.load();
```

### Model Comparison

Compare responses from different models or configurations:

```grapa
/* Compare different temperature settings */
compare_temperatures = op(prompt) {
    results = {};
    
    temperatures = [0.1, 0.5, 0.8, 1.2];
    
    for (i = 0; i < temperatures.len(); i++) {
        temp = temperatures[i];
        
        model = $MODEL();
        model.params({"temperature": temp, "max_tokens": 100});
        model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
        
        response = model.gen(prompt);
        results[temp.str()] = response;
        
        model.load();
    };
    
    return results;
};

/* Compare responses */
prompt = "Explain the concept of artificial intelligence";
results = compare_temperatures(prompt);

("Temperature Comparison for: " + prompt + "\n").echo();
for (temp in results) {
    ("Temperature " + temp + ": " + results[temp].str() + "\n").echo();
};
```

## Performance Optimization

### Efficient Model Usage

Optimize model usage for better performance:

```grapa
/* Optimize for speed */
fast_model = $MODEL();
fast_model.params({
    "context_size": 1024,    /* Smaller context */
    "max_tokens": 50,        /* Shorter responses */
    "verbose": 0             /* No logging */
});

fast_model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Batch multiple generations */
prompts = ["What is AI?", "Explain ML", "Define deep learning"];
responses = [];

for (i = 0; i < prompts.len(); i++) {
    response = fast_model.gen(prompts[i]);
    responses.append(response);
    ("Response " + (i+1).str() + ": " + response.str() + "\n").echo();
};

fast_model.load();
```

### Memory Management

Proper memory management for large models:

```grapa
/* Memory-efficient model usage */
use_model_efficiently = op(prompt) {
    model = $MODEL();
    
    /* Load model */
    result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
    if (result.type() != $ERR) {
        /* Generate response */
        response = model.gen(prompt);
        
        /* Always clean up */
        model.load();
        
        return response;
    } else {
        model.load();
        return "Error loading model";
    };
};

/* Use the function */
response = use_model_efficiently("What is machine learning?");
("Response: " + response.str() + "\n").echo();
```

## Real-world Use Cases

### Chat Bot Implementation

```grapa
/* Simple chat bot */
chatbot = $MODEL();
chatbot.params({
    "temperature": 0.7,
    "max_tokens": 200,
    "verbose": 0
});

chatbot.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Set up chatbot personality */
chatbot.context("You are a helpful, friendly AI assistant. Be concise but informative.");

/* Simulate chat loop */
user_messages = [
    "Hello, what can you help me with?",
    "Tell me about machine learning",
    "What are the main types of ML?",
    "Thanks for the explanation!"
];

for (i = 0; i < user_messages.len(); i++) {
    user_msg = user_messages[i];
    ("User: " + user_msg + "\n").echo();
    
    response = chatbot.gen(user_msg);
    ("Bot: " + response.str() + "\n").echo();
};

chatbot.load();
```

### Content Generation Pipeline

```grapa
/* Content generation with different styles */
content_pipeline = op(topic) {
    results = {};
    
    /* Technical writing */
    technical_model = $MODEL();
    technical_model.params({"temperature": 0.3, "max_tokens": 150});
    technical_model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
    
    technical_content = technical_model.gen("Write a technical explanation of " + topic);
    results["technical"] = technical_content;
    technical_model.load();
    
    /* Creative writing */
    creative_model = $MODEL();
    creative_model.params({"temperature": 1.1, "max_tokens": 200});
    creative_model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
    
    creative_content = creative_model.gen("Write a creative story about " + topic);
    results["creative"] = creative_content;
    creative_model.load();
    
    return results;
};

/* Generate content */
topic = "artificial intelligence";
content = content_pipeline(topic);

("Technical Content: " + content["technical"].str() + "\n").echo();
("Creative Content: " + content["creative"].str() + "\n").echo();
```

### Context-Aware Assistant

```grapa
/* Context-aware assistant with memory */
assistant = $MODEL();
assistant.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

/* Load previous context if available */
context_file = "assistant_context.txt";
if ($file(context_file).exists()) {
    context_data = $file(context_file).get();
    assistant.context(context_data);
    ("Previous context loaded\n").echo();
} else {
    assistant.context("You are a helpful assistant with a good memory.");
};

/* Simulate conversation with memory */
conversations = [
    "My name is John and I'm interested in learning Python.",
    "What are the best resources for learning Python?",
    "What about data science libraries in Python?",
    "Can you recommend some projects to practice?"
];

for (i = 0; i < conversations.len(); i++) {
    user_msg = conversations[i];
    ("User: " + user_msg + "\n").echo();
    
    response = assistant.gen(user_msg);
    ("Assistant: " + response.str() + "\n").echo();
};

/* Save context for next session */
current_context = assistant.context();
$file(context_file).set(current_context."text".str());
("Context saved for next session at " + $TIME().utc().str() + "\n").echo();

assistant.load();
```

## Consistent Metadata Structure

All model types now return structured results with consistent metadata fields:

### Llama Models

```grapa
/* Llama models return $GOBJ with 'text' and 'metadata' */
result = model.gen("Your prompt here");
text = result."text";                    /* Generated text */
metadata = result."metadata";            /* Processing metadata */

/* Available metadata fields */
("Model path: " + metadata."model_path".str() + "\n").echo();
("Prompt tokens: " + metadata."prompt_tokens".str() + "\n").echo();
("Completion tokens: " + metadata."completion_tokens".str() + "\n").echo();
("Total tokens: " + metadata."total_tokens".str() + "\n").echo();
("Temperature: " + metadata."temperature".str() + "\n").echo();
("Max tokens: " + metadata."max_tokens".str() + "\n").echo();
("Context length: " + metadata."context_length".str() + "\n").echo();
```


### OpenAI Models

```grapa
/* OpenAI models return the full JSON response */
result = model.gen("Your prompt here");
/* Access the parsed JSON structure */
response_data = op()(result.str())();
text = response_data."output"[0]."content"[0]."text";
usage = response_data."usage";
model_name = response_data."model";
```

### Model Information

```grapa
/* Get model information for any loaded model */
info = model.info();
("Loaded: " + info."loaded".str() + "\n").echo();
("Method: " + info."method".str() + "\n").echo();
("Model path: " + info."model_path".str() + "\n").echo();

/* Llama-specific information */
if (info."method".str() == "llama") {
    ("Model size: " + info."model_size_bytes".str() + " bytes\n").echo();
    ("Parameters: " + info."n_params".str() + "\n").echo();
    ("Description: " + info."model_description".str() + "\n").echo();
}
```

## Best Practices

### 1. Always Check Load Results

```grapa
model = $MODEL();
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");

if (result.type() != $ERR) {
    /* Model loaded successfully */
    response = model.gen("Hello");
    
    /* Access structured response */
    text = response."text";
    metadata = response."metadata";
    
    ("Response: " + text.str() + "\n").echo();
    ("Model: " + metadata."model_path".str() + "\n").echo();
    ("Context length: " + metadata."context_length".str() + "\n").echo();
} else {
    ("Error loading model: " + result.str() + "\n").echo();
    return;
}

model.load();
```

### 2. Use Appropriate Parameters

```grapa
/* For factual responses */
model.params({
    "temperature": 0.3,
    "max_tokens": 100
});

/* For creative responses */
model.params({
    "temperature": 1.0,
    "max_tokens": 200
});

/* For debugging */
model.params({
    "verbose": 4
});
```

### 3. Manage Memory Efficiently

```grapa
/* Always clean up models */
model = $MODEL();
model.load("large_model.gguf");

/* Use model... */

model.load();  /* Free memory */
```

### 4. Use Context Appropriately

```grapa
/* For conversation continuity */
model.context("Previous conversation: User asked about AI.");

/* For role-playing */
model.context("You are a helpful coding tutor.");

/* For specific tasks */
model.context("You are an expert in machine learning.");
```

### 5. Handle Errors Gracefully

```grapa
/* Option 1: Using .iferr() for fallback */
safe_generate = op(model, prompt) {
    response = model.gen(prompt).iferr("Generation failed");
    if (response.len() == 0) {
        return "Empty response generated";
    };
    return response;
};

/* Option 2: Using try/catch for detailed error handling */
safe_generate_detailed = op(model, prompt) {
    try response = model.gen(prompt);
    catch (true) return "Generation error occurred";
    default return "Unknown error occurred";:
    
    if (response.len() == 0) {
        return "Empty response generated";
    };
    return response;
};

/* Option 3: Using explicit error checking */
safe_generate_explicit = op(model, prompt) {
    response = model.gen(prompt);
    if (response.type() == $ERR) {
        return "Generation error: " + response.str();
    }
    if (response.len() == 0) {
        return "Empty response generated";
    };
    return response;
};
```

## Grapa Integration Patterns

### LLAMA + Grapa Interpolation

A powerful pattern is to have LLAMA generate responses with Grapa interpolation placeholders, then use `.interpolate()` to evaluate them:

```grapa
/* Set up model to use interpolation placeholders */
model = $MODEL();
model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
model.context("You are a helpful math tutor. When doing calculations, you MUST use ${expression} placeholders that can be evaluated by Grapa. For example, instead of writing '15 * 23 = 345', write '15 * 23 = ${15 * 23}'. Always show the calculation step by step using this format.");

/* Important: Define variables for literal text that looks like placeholders */
expression = "${expression}";  /* Prevents interpolation errors on literal text */

/* Math calculations */
math_response = model.gen("What is 15 * 23 + 45? Please show your work step by step using ${expression} format for all calculations.");
/* LLAMA: "The calculation is ${15 * 23 + 45} which equals ${15 * 23 + 45}" */
result = math_response.interpolate();
/* Result: "The calculation is 390 which equals 390" */

/* Data processing */
data_response = model.gen("What's the average of [10, 20, 30, 40, 50]?");
/* LLAMA: "The average is ${[10, 20, 30, 40, 50].reduce(op(a,b){a+b}) / 5}" */
avg_result = data_response.interpolate();
/* Result: "The average is 30" */

/* File operations */
file_response = model.gen("How many lines are in my data.txt file?");
/* LLAMA: "The file has ${$file('data.txt').get().split('\n').len()} lines" */
line_count = file_response.interpolate();
/* Result: "The file has 42 lines" */

/* With context variables */
user_name = "Alice";
user_age = 30;
personal_response = model.gen("Tell me about this user");
/* LLAMA: "User ${user_name} is ${user_age} years old, born in ${2024 - user_age}" */
personal_result = personal_response.interpolate();
/* Result: "User Alice is 30 years old, born in 1994" */
```

**Important Notes:**
- **Pre-define literal variables**: If LLAMA generates text like `"use ${expression} format"`, define `expression = "${expression}";` to prevent interpolation errors
- **Interpolation scope**: Variables in `${}` must be defined in the current scope before calling `.interpolate()`
- **Error handling**: Undefined variables in placeholders will result in `{"error":-1}`

**Benefits:**
- **Simple**: LLAMA just uses `${}` syntax
- **Flexible**: Interpolate when/where you want
- **Safe**: Programmer controls evaluation
- **Powerful**: Full Grapa capabilities in responses


## OpenAI Integration

### OpenAI Text Generation

Grapa supports OpenAI's GPT models for text generation. The `model.gen()` method returns the complete JSON response from OpenAI, giving you access to all metadata and additional information:

```grapa
/* OpenAI text generation */
model = $MODEL();
model.load('gpt-4o', 'openai');

/* Generate with API key */
result = model.gen('The quick brown fox jumps over the lazy dog', {
    'api_key': 'your-openai-api-key'
});

/* Access the full JSON response */
("Full response: " + result.str() + "\n").echo();

/* Extract specific information from the JSON */
response_data = op()(result.str())();
if (response_data."output".len() > 0) {
    text_content = response_data."output"[0]."content"[0]."text";
    ("Generated text: " + text_content + "\n").echo();
    
    /* Access usage information */
    usage = response_data."usage";
    ("Tokens used: " + usage."total_tokens".str() + "\n").echo();
};

model.load();
```

### OpenAI Embeddings

For embedding generation, Grapa returns the complete OpenAI embedding response:

```grapa
/* OpenAI embeddings */
model = $MODEL();
model.load('text-embedding-3-small', 'openai-embedding');

/* Generate embeddings */
result = model.gen('The quick brown fox jumps over the lazy dog', {
    'api_key': 'your-openai-api-key'
});

/* Access the full JSON response */
("Full response: " + result.str() + "\n").echo();

/* Extract embedding data */
response_data = op()(result.str())();
if (response_data."data".len() > 0) {
    embedding = response_data."data"[0]."embedding";
    ("Embedding dimension: " + embedding.len().str() + "\n").echo();
    ("First 5 values: " + embedding[0:5].str() + "\n").echo();
    
    /* Access model and usage information */
    ("Model used: " + response_data."model" + "\n").echo();
    usage = response_data."usage";
    ("Total tokens: " + usage."total_tokens".str() + "\n").echo();
};

model.load();
```

### Working with OpenAI JSON Responses

The JSON responses contain rich metadata that you can extract and use:

```grapa
/* Extract information from OpenAI responses */
extract_response_info = op(response_json) {
    data = op()(response_json)();
    info = {};
    
    /* For text generation responses */
    if (data."output".len() > 0) {
        output = data."output"[0];
        info["text"] = output."content"[0]."text";
        info["role"] = output."role";
        info["status"] = output."status";
    };
    
    /* For embedding responses */
    if (data."data".len() > 0) {
        embedding_data = data."data"[0];
        info["embedding"] = embedding_data."embedding";
        info["index"] = embedding_data."index";
    };
    
    /* Common metadata */
    info["model"] = data."model";
    info["usage"] = data."usage";
    info["created_at"] = data."created_at";
    
    return info;
};

/* Usage example */
model = $MODEL();
model.load('gpt-4o', 'openai');

result = model.gen('Explain machine learning', {
    'api_key': 'your-openai-api-key'
});

info = extract_response_info(result.str());
("Generated text: " + info["text"] + "\n").echo();
("Model: " + info["model"] + "\n").echo();
("Tokens used: " + info["usage"]."total_tokens".str() + "\n").echo();

model.load();
```

### OpenAI Response Structure

The JSON responses follow OpenAI's API format:

**Text Generation Response:**
```json
{
  "id": "resp_...",
  "object": "response",
  "created_at": 1759248115,
  "status": "completed",
  "model": "gpt-4o-2024-08-06",
  "output": [
    {
      "id": "msg_...",
      "type": "message",
      "status": "completed",
      "content": [
        {
          "type": "output_text",
          "text": "Generated text here"
        }
      ],
      "role": "assistant"
    }
  ],
  "usage": {
    "input_tokens": 16,
    "output_tokens": 32,
    "total_tokens": 48
  }
}
```

**Embedding Response:**
```json
{
  "object": "list",
  "data": [
    {
      "object": "embedding",
      "index": 0,
      "embedding": [0.123, -0.456, ...]
    }
  ],
  "model": "text-embedding-3-small",
  "usage": {
    "prompt_tokens": 9,
    "total_tokens": 9
  }
}
```

## See Also

- [$MODEL Data Type](type/model.md) - Complete type reference
- [Model API Reference](api/model.md) - Complete API documentation
- [Model Examples](examples/model_examples.grc) - Working code examples
- [Model Download Guide](model_download.md) - How to download models
