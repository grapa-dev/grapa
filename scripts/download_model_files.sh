#!/bin/bash
# Download all-MiniLM-L6-v2 model files directly

echo "🚀 Downloading all-MiniLM-L6-v2 model files..."

# Create directory
mkdir -p models/all-MiniLM-L6-v2-onnx
cd models/all-MiniLM-L6-v2-onnx

# Base URL
BASE_URL="https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2/resolve/main/"

# Download files
echo "📥 Downloading config.json..."
curl -L -o config.json "${BASE_URL}config.json"

echo "📥 Downloading tokenizer_config.json..."
curl -L -o tokenizer_config.json "${BASE_URL}tokenizer_config.json"

echo "📥 Downloading vocab.txt..."
curl -L -o vocab.txt "${BASE_URL}vocab.txt"

echo "📥 Downloading tokenizer.json..."
curl -L -o tokenizer.json "${BASE_URL}tokenizer.json"

echo "📥 Downloading special_tokens_map.json..."
curl -L -o special_tokens_map.json "${BASE_URL}special_tokens_map.json"

echo "📥 Downloading model.safetensors..."
curl -L -o model.safetensors "${BASE_URL}model.safetensors"

echo "✅ All files downloaded to models/all-MiniLM-L6-v2-onnx/"
echo "📁 Files:"
ls -la

cd ../..
