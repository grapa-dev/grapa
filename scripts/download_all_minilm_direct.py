#!/usr/bin/env python3
"""
Download all-MiniLM-L6-v2 model files directly from Hugging Face and convert to ONNX.
This bypasses the transformers library cache issues.
"""

import os
import sys
import urllib.request
import json
import tempfile
from pathlib import Path

def download_file(url, output_path):
    """Download a file from URL to output path."""
    print(f"📥 Downloading {os.path.basename(output_path)}...")
    try:
        urllib.request.urlretrieve(url, output_path)
        print(f"✅ Downloaded {os.path.basename(output_path)}")
        return True
    except Exception as e:
        print(f"❌ Error downloading {os.path.basename(output_path)}: {e}")
        return False

def download_model_files():
    """Download the model files directly from Hugging Face."""
    print("🔄 Downloading all-MiniLM-L6-v2 model files...")
    
    base_url = "https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2/resolve/main/"
    
    files_to_download = [
        "config.json",
        "tokenizer_config.json", 
        "vocab.txt",
        "tokenizer.json",
        "special_tokens_map.json",
        "model.safetensors"
    ]
    
    # Create temporary directory
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_path = Path(temp_dir)
        
        # Download all files
        for filename in files_to_download:
            url = base_url + filename
            output_path = temp_path / filename
            
            if not download_file(url, str(output_path)):
                return None, None
        
        # Load the model using transformers
        try:
            from transformers import AutoTokenizer, AutoModel
            
            print("🔄 Loading model from downloaded files...")
            tokenizer = AutoTokenizer.from_pretrained(str(temp_path))
            model = AutoModel.from_pretrained(str(temp_path))
            
            return tokenizer, model
            
        except Exception as e:
            print(f"❌ Error loading model: {e}")
            return None, None

def convert_to_onnx(tokenizer, model, output_dir):
    """Convert the model to ONNX format."""
    print("🔄 Converting model to ONNX format...")
    
    try:
        import torch
        import onnx
        from onnxruntime import InferenceSession
        
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
        
        # Save tokenizer
        tokenizer_path = os.path.join(output_dir, "tokenizer")
        tokenizer.save_pretrained(tokenizer_path)
        print(f"✅ Tokenizer saved to {tokenizer_path}")
        
        # Convert model to ONNX
        model_path = os.path.join(output_dir, "model.onnx")
        
        # Create dummy input for tracing
        dummy_input = tokenizer("This is a test sentence", return_tensors="pt", padding=True, truncation=True)
        
        # Export to ONNX
        torch.onnx.export(
            model,
            (dummy_input["input_ids"], dummy_input["attention_mask"]),
            model_path,
            export_params=True,
            opset_version=11,
            do_constant_folding=True,
            input_names=["input_ids", "attention_mask"],
            output_names=["last_hidden_state", "pooler_output"],
            dynamic_axes={
                "input_ids": {0: "batch_size", 1: "sequence_length"},
                "attention_mask": {0: "batch_size", 1: "sequence_length"},
                "last_hidden_state": {0: "batch_size", 1: "sequence_length"},
                "pooler_output": {0: "batch_size"}
            }
        )
        
        print(f"✅ ONNX model saved to {model_path}")
        
        # Verify the ONNX model
        onnx_model = onnx.load(model_path)
        onnx.checker.check_model(onnx_model)
        print("✅ ONNX model validation passed")
        
        # Test the ONNX model
        session = InferenceSession(model_path)
        print("✅ ONNX model can be loaded successfully")
        
        return model_path, tokenizer_path
        
    except Exception as e:
        print(f"❌ Error converting to ONNX: {e}")
        return None, None

def create_test_script(output_dir):
    """Create a test script to verify the ONNX model works."""
    test_script = os.path.join(output_dir, "test_onnx_model.py")
    
    test_code = '''#!/usr/bin/env python3
"""
Test script for the ONNX all-MiniLM-L6-v2 model.
"""

import numpy as np
from transformers import AutoTokenizer
from onnxruntime import InferenceSession

def test_onnx_model():
    """Test the ONNX model with sample text."""
    try:
        # Load tokenizer and ONNX model
        tokenizer = AutoTokenizer.from_pretrained("tokenizer")
        session = InferenceSession("model.onnx")
        
        # Test with sample text
        texts = [
            "This is a test sentence.",
            "Another example for testing.",
            "The quick brown fox jumps over the lazy dog."
        ]
        
        print("🧪 Testing ONNX model with sample texts...")
        
        for text in texts:
            # Tokenize input
            inputs = tokenizer(text, return_tensors="np", padding=True, truncation=True, max_length=512)
            
            # Run inference
            outputs = session.run(
                None,
                {
                    "input_ids": inputs["input_ids"],
                    "attention_mask": inputs["attention_mask"]
                }
            )
            
            # Extract embeddings (mean pooling)
            last_hidden_state = outputs[0]
            attention_mask = inputs["attention_mask"]
            
            # Mean pooling
            embeddings = np.mean(last_hidden_state * attention_mask[:, :, np.newaxis], axis=1)
            
            print(f"✅ Text: '{text}'")
            print(f"   Embedding shape: {embeddings.shape}")
            print(f"   Embedding norm: {np.linalg.norm(embeddings):.4f}")
            print()
        
        print("🎉 ONNX model test completed successfully!")
        
    except Exception as e:
        print(f"❌ Error testing ONNX model: {e}")

if __name__ == "__main__":
    test_onnx_model()
'''
    
    with open(test_script, 'w') as f:
        f.write(test_code)
    
    os.chmod(test_script, 0o755)
    print(f"✅ Test script created: {test_script}")

def main():
    """Main function to download and convert the model."""
    print("🚀 Starting all-MiniLM-L6-v2 ONNX conversion (direct download)...")
    
    # Check dependencies
    try:
        import torch
        import transformers
        import onnx
        import onnxruntime
        print("✅ All required dependencies are available")
    except ImportError as e:
        print(f"❌ Missing dependency: {e}")
        print("Please install required packages:")
        print("pip install torch transformers onnx onnxruntime")
        return False
    
    # Set up paths
    models_dir = Path("models")
    output_dir = models_dir / "all-MiniLM-L6-v2-onnx"
    
    print(f"📁 Output directory: {output_dir}")
    
    # Download model files
    tokenizer, model = download_model_files()
    if tokenizer is None or model is None:
        return False
    
    # Convert to ONNX
    model_path, tokenizer_path = convert_to_onnx(tokenizer, model, str(output_dir))
    if model_path is None or tokenizer_path is None:
        return False
    
    # Create test script
    create_test_script(str(output_dir))
    
    print(f"\n🎉 Successfully converted all-MiniLM-L6-v2 to ONNX format!")
    print(f"📁 Model directory: {output_dir}")
    print(f"🔧 Test the model: python3 {output_dir}/test_onnx_model.py")
    print(f"📝 ONNX model: {model_path}")
    print(f"📝 Tokenizer: {tokenizer_path}")
    
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
