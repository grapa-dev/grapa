#!/usr/bin/env python3
"""
Convert the downloaded all-MiniLM-L6-v2 model to ONNX format.
"""

import os
import sys
from pathlib import Path

def convert_model_to_onnx():
    """Convert the downloaded model to ONNX format."""
    print("🔄 Converting all-MiniLM-L6-v2 to ONNX format...")
    
    try:
        from transformers import AutoTokenizer, AutoModel
        import torch
        import onnx
        from onnxruntime import InferenceSession
        
        # Set up paths
        model_dir = "models/all-MiniLM-L6-v2-onnx"
        output_dir = model_dir
        
        print(f"📁 Loading model from: {model_dir}")
        
        # Load tokenizer and model
        print("📥 Loading tokenizer...")
        tokenizer = AutoTokenizer.from_pretrained(model_dir)
        
        print("📥 Loading model...")
        model = AutoModel.from_pretrained(model_dir)
        
        # Convert to ONNX
        model_path = os.path.join(output_dir, "model.onnx")
        
        print("🔄 Converting to ONNX...")
        
        # Create dummy input for tracing
        dummy_input = tokenizer("This is a test sentence", return_tensors="pt", padding=True, truncation=True)
        
        # Export to ONNX
        torch.onnx.export(
            model,
            (dummy_input["input_ids"], dummy_input["attention_mask"]),
            model_path,
            export_params=True,
            opset_version=14,  # Updated to support scaled_dot_product_attention
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
        
        return model_path
        
    except Exception as e:
        print(f"❌ Error converting to ONNX: {e}")
        return None

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
        tokenizer = AutoTokenizer.from_pretrained(".")
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
    """Main function to convert the model."""
    print("🚀 Converting all-MiniLM-L6-v2 to ONNX format...")
    
    # Check if model files exist
    model_dir = "models/all-MiniLM-L6-v2-onnx"
    required_files = ["config.json", "model.safetensors", "tokenizer.json"]
    
    for file in required_files:
        if not os.path.exists(os.path.join(model_dir, file)):
            print(f"❌ Missing required file: {file}")
            print("Please run ./scripts/download_model_files.sh first")
            return False
    
    # Convert to ONNX
    model_path = convert_model_to_onnx()
    if model_path is None:
        return False
    
    # Create test script
    create_test_script(model_dir)
    
    print(f"\n🎉 Successfully converted all-MiniLM-L6-v2 to ONNX format!")
    print(f"📁 Model directory: {model_dir}")
    print(f"🔧 Test the model: python3 {model_dir}/test_onnx_model.py")
    print(f"📝 ONNX model: {model_path}")
    
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
