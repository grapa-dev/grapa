# Grapa Models Directory

This directory contains AI/ML models that are accessible by both the Grapa CLI tool and the grapapy Python extension.

## Current Models

### Qwen2.5-7B-Instruct
- **File**: `qwen2.5-7b-instruct-q5_k_m.gguf`
- **Size**: 5.1GB
- **Quantization**: Q5_K_M
- **Architecture**: Qwen2.5-7B-Instruct
- **Format**: GGUF (compatible with llama.cpp)

## Usage

### From Grapa CLI
```bash
# Load and use the model in Grapa scripts
./grapa -c "model = $file().get('models/qwen2.5-7b-instruct-q5_k_m.gguf');"
```

### From grapapy Python Extension
```python
import grapapy

# Load and use the model
g = grapapy.grapa()
result = g.eval("model = $file().get('models/qwen2.5-7b-instruct-q5_k_m.gguf');")
```

## Model Management

- **Adding new models**: Place GGUF format models directly in this directory
- **Model naming**: Use descriptive names that include model size and quantization
- **File organization**: Keep models in the root of this directory for easy access
- **Documentation**: Update this README when adding new models

## File Structure
```
models/
├── README.md
├── qwen2.5-7b-instruct-q5_k_m.gguf
└── [additional models...]
```

## Notes

- Models in this directory are accessible via relative paths from both CLI and Python
- The `models/` directory is included in the project structure for easy reference
- Large model files (>1GB) are typically not committed to version control
- Consider using Git LFS for large model files if version control is needed
