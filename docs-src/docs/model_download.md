# Model Download Guide

This guide explains how to download AI/ML models for use with the `$MODEL` data type in Grapa.

## Overview

**Note: Local model support has been removed from Grapa. The `$MODEL` type now only supports OpenAI cloud-based models.**

For OpenAI models, no downloading is required - you simply specify the model name when loading:

```grapa
/* Load OpenAI text generation model */
model = $MODEL();
result = model.load("gpt-4o", "openai");

/* Load OpenAI embedding model */
result = model.load("text-embedding-3-small", "openai-embedding");
```

## OpenAI Model Usage

OpenAI models are accessed via API calls and don't require local downloads. Simply use the model name and provide your API key:

```grapa
/* Generate text with OpenAI */
response = model.gen("Hello, how are you?", {
    "api_key": "sk-your-openai-api-key"
});

/* Create embeddings with OpenAI */
response = model.gen("The quick brown fox", {
    "api_key": "sk-your-openai-api-key"
});
```

## Legacy Local Model Information

The following information is kept for reference but local models are no longer supported:

## Download Methods

### Method 1: Using curl (Recommended)

The most reliable method for downloading models is using the `curl` command:

```bash
# Download a single-file model (Q3_K_M quantization)
curl -L -o models/qwen2.5-7b-instruct-q3_k_m.gguf \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q3_k_m.gguf"

# Download split files (Q4_K_M quantization)
curl -L -o models/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf"

curl -L -o models/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf"

# Download split files (Q5_K_M quantization - Higher Quality)
curl -L -o models/qwen2.5-7b-instruct-q5_k_m-00001-of-00002.gguf \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q5_k_m-00001-of-00002.gguf"

curl -L -o models/qwen2.5-7b-instruct-q5_k_m-00002-of-00002.gguf \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q5_k_m-00002-of-00002.gguf"
```

### Method 2: Using Hugging Face CLI

For authenticated downloads or easier management:

```bash
# Install Hugging Face CLI
pip install -U huggingface_hub

# Login to Hugging Face (optional, for private models)
huggingface-cli login

# Download model files
huggingface-cli download Qwen/Qwen2.5-7B-Instruct-GGUF \
  --include "qwen2.5-7b-instruct-q3_k_m.gguf" \
  --local-dir ./models \
  --local-dir-use-symlinks False
```

### Method 3: Using wget

Alternative to curl:

```bash
# Download single file
wget -O models/qwen2.5-7b-instruct-q3_k_m.gguf \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q3_k_m.gguf"
```

## Available Models

### Qwen2.5-7B-Instruct Models

The following models are available in the Qwen2.5-7B-Instruct-GGUF repository:

| Model | Size | Type | Download |
|-------|------|------|----------|
| `qwen2.5-7b-instruct-q2_k.gguf` | ~3.0 GB | Single file | ✅ Easy |
| `qwen2.5-7b-instruct-q3_k_m.gguf` | ~3.8 GB | Single file | ✅ Easy |
| `qwen2.5-7b-instruct-q4_k_m.gguf` | ~4.7 GB | Split (2 parts) | ⚠️ Requires merging |
| `qwen2.5-7b-instruct-q5_k_m.gguf` | ~5.4 GB | Split (2 parts) | ⚠️ Requires merging |
| `qwen2.5-7b-instruct-q6_k.gguf` | ~6.3 GB | Split (2 parts) | ⚠️ Requires merging |
| `qwen2.5-7b-instruct-q8_0.gguf` | ~8.0 GB | Split (3 parts) | ⚠️ Requires merging |

### Recommended Models

For most users, we recommend:

1. **Q3_K_M** - Good balance of quality and size (single file) - **Recommended for examples**
2. **Q5_K_M** - Higher quality, requires merging (2 parts) - **Recommended for production**
3. **Q4_K_M** - Better quality than Q3, requires merging (2 parts)
4. **Q2_K** - Smallest size, lower quality (single file)

**Note:** The examples in this documentation use Q3_K_M for simplicity, but you can substitute Q5_K_M for better quality if you're willing to handle the file merging process.

## Merging Split Files

When downloading split files, you need to merge them into a single file:

### Using cat (Linux/Mac)

```bash
# Merge Q4_K_M model
cat models/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf \
    models/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf \
    > models/qwen2.5-7b-instruct-q4_k_m.gguf

# Merge Q5_K_M model
cat models/qwen2.5-7b-instruct-q5_k_m-00001-of-00002.gguf \
    models/qwen2.5-7b-instruct-q5_k_m-00002-of-00002.gguf \
    > models/qwen2.5-7b-instruct-q5_k_m.gguf
```

### Using PowerShell (Windows)

```powershell
# Merge Q4_K_M model
Get-Content models/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf, 
              models/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf | 
    Set-Content models/qwen2.5-7b-instruct-q4_k_m.gguf
```

### Using Python

```python
# Python script to merge files
def merge_gguf_files(part_files, output_file):
    with open(output_file, 'wb') as outfile:
        for part_file in part_files:
            with open(part_file, 'rb') as infile:
                outfile.write(infile.read())

# Merge Q4_K_M model
merge_gguf_files([
    'models/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf',
    'models/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf'
], 'models/qwen2.5-7b-instruct-q4_k_m.gguf')
```

## Directory Structure

Organize your models in a dedicated directory:

```
your-project/
├── models/
│   ├── qwen2.5-7b-instruct-q3_k_m.gguf
│   ├── qwen2.5-7b-instruct-q4_k_m.gguf
│   └── other-models/
└── your-grapa-scripts/
    └── model_usage.grc
```

## Download Scripts

### Automated Download Script

Create a download script for easy model management:

```bash
#!/bin/bash
# download_models.sh

MODELS_DIR="models"
mkdir -p "$MODELS_DIR"

echo "Downloading Qwen2.5-7B-Instruct models..."

# Download Q3_K_M (single file, recommended)
echo "Downloading Q3_K_M model..."
curl -L -o "$MODELS_DIR/qwen2.5-7b-instruct-q3_k_m.gguf" \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q3_k_m.gguf"

# Download Q4_K_M parts
echo "Downloading Q4_K_M model parts..."
curl -L -o "$MODELS_DIR/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf" \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf"

curl -L -o "$MODELS_DIR/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf" \
  "https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf"

# Merge Q4_K_M
echo "Merging Q4_K_M model..."
cat "$MODELS_DIR/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf" \
    "$MODELS_DIR/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf" \
    > "$MODELS_DIR/qwen2.5-7b-instruct-q4_k_m.gguf"

# Clean up split files
rm "$MODELS_DIR/qwen2.5-7b-instruct-q4_k_m-00001-of-00002.gguf"
rm "$MODELS_DIR/qwen2.5-7b-instruct-q4_k_m-00002-of-00002.gguf"

echo "Download complete!"
```

### Grapa Download Script

You can also create a Grapa script to download models:

```grapa
/* download_models.grc - Download models using Grapa */

"Starting model download...".echo();

/* Create models directory */
models_dir = "models";
if (!$file(models_dir).exists()) {
    $file(models_dir).mkdir();
    ("Created directory: " + models_dir).echo();
}

/* Download Q3_K_M model (single file) */
"Downloading Q3_K_M model...".echo();
download_cmd = "curl -L -o " + models_dir + "/qwen2.5-7b-instruct-q3_k_m.gguf " +
               "\"https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q3_k_m.gguf\"";

result = $system(download_cmd);
if (result.type() != $ERR) {
    "Q3_K_M model downloaded successfully!".echo();
} else {
    ("Download failed with error: " + result.str()).echo();
}

"Download process complete!".echo();
```

## Verification

After downloading, verify your models:

```grapa
/* verify_models.grc - Verify downloaded models */

"Verifying downloaded models...".echo();

models_dir = "models";
model_files = [
    "qwen2.5-7b-instruct-q3_k_m.gguf",
    "qwen2.5-7b-instruct-q4_k_m.gguf"
];

for (i = 0; i < model_files.len(); i++) {
    model_path = models_dir + "/" + model_files[i];
    if ($file(model_path).exists()) {
        file_size = $file(model_path).size();
        ("✅ " + model_files[i] + " - " + file_size.str() + " bytes").echo();
        
        /* Test loading the model (auto-detect method) */
        test_model = $MODEL();
        test_model.params({"verbose": 0});
        load_result = test_model.load(model_path);
        if (load_result.type() != $ERR) {
            "   Model loads successfully".echo();
            test_model.load();
        } else {
            ("   ⚠️ Model load failed: " + load_result.str()).echo();
        }
    } else {
        ("❌ " + model_files[i] + " - Not found").echo();
    }
}
```

## Troubleshooting

### Common Issues

1. **Download fails with 404 error**
   - Check the URL is correct
   - Verify the model file exists in the repository

2. **Download fails with authentication error**
   - Some models require Hugging Face authentication
   - Use `huggingface-cli login` to authenticate

3. **Merge fails**
   - Ensure all parts are downloaded completely
   - Check file sizes match expected values

4. **Model won't load**
   - Verify the file is not corrupted
   - Check you have sufficient memory
   - Ensure the file is a valid GGUF format

### File Size Verification

Expected file sizes for Qwen2.5-7B-Instruct models:

| Model | Expected Size |
|-------|---------------|
| Q2_K | ~3.0 GB |
| Q3_K_M | ~3.8 GB |
| Q4_K_M | ~4.7 GB |
| Q5_K_M | ~5.4 GB |
| Q6_K | ~6.3 GB |
| Q8_0 | ~8.0 GB |

## See Also

- [Model Examples](examples/model_examples.grc) - How to use downloaded models
- [$MODEL Data Type](../type/model.md) - Complete type reference
- [API Reference](../api/model.md) - Complete API documentation
