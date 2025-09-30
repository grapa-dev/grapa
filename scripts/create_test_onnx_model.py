#!/usr/bin/env python3
"""
Create a simple test ONNX model for Grapa ONNX integration testing.
This creates a minimal embedding model for testing purposes.
"""

import os
import sys
import numpy as np
import torch
import torch.nn as nn
from pathlib import Path

class SimpleEmbeddingModel(nn.Module):
    """A simple embedding model for testing ONNX conversion."""
    
    def __init__(self, vocab_size=1000, embedding_dim=384, max_length=512):
        super().__init__()
        self.embedding = nn.Embedding(vocab_size, embedding_dim)
        self.pooler = nn.Linear(embedding_dim, embedding_dim)
        self.max_length = max_length
        
    def forward(self, input_ids, attention_mask):
        # Get embeddings
        embeddings = self.embedding(input_ids)
        
        # Apply attention mask
        masked_embeddings = embeddings * attention_mask.unsqueeze(-1)
        
        # Mean pooling
        pooled = masked_embeddings.sum(dim=1) / attention_mask.sum(dim=1, keepdim=True)
        
        # Apply final linear layer
        output = self.pooler(pooled)
        
        return output, pooled

def create_test_model():
    """Create a simple test model."""
    print("🔄 Creating test embedding model...")
    
    model = SimpleEmbeddingModel()
    model.eval()
    
    return model

def create_test_tokenizer():
    """Create a simple test tokenizer."""
    print("🔄 Creating test tokenizer...")
    
    # Simple word-based tokenizer
    vocab = {
        "<PAD>": 0,
        "<UNK>": 1,
        "<CLS>": 2,
        "<SEP>": 3,
    }
    
    # Add common words
    words = ["the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for", "of", "with", "by", "is", "are", "was", "were", "be", "been", "being", "have", "has", "had", "do", "does", "did", "will", "would", "could", "should", "may", "might", "can", "this", "that", "these", "those", "i", "you", "he", "she", "it", "we", "they", "me", "him", "her", "us", "them"]
    
    for i, word in enumerate(words):
        vocab[word] = i + 4
    
    def tokenize(text, max_length=512):
        """Simple tokenization."""
        words = text.lower().split()
        token_ids = [vocab.get(word, vocab["<UNK>"]) for word in words]
        
        # Truncate or pad
        if len(token_ids) > max_length:
            token_ids = token_ids[:max_length]
        else:
            token_ids.extend([vocab["<PAD>"]] * (max_length - len(token_ids)))
        
        attention_mask = [1 if token != vocab["<PAD>"] else 0 for token in token_ids]
        
        return token_ids, attention_mask
    
    return tokenize, vocab

def convert_to_onnx(model, output_dir):
    """Convert the model to ONNX format."""
    print("🔄 Converting model to ONNX format...")
    
    try:
        # Create dummy input
        input_ids = torch.tensor([[1, 2, 3, 4, 5, 0, 0, 0]], dtype=torch.long)
        attention_mask = torch.tensor([[1, 1, 1, 1, 1, 0, 0, 0]], dtype=torch.long)
        
        # Export to ONNX
        model_path = os.path.join(output_dir, "model.onnx")
        
        torch.onnx.export(
            model,
            (input_ids, attention_mask),
            model_path,
            export_params=True,
            opset_version=11,
            do_constant_folding=True,
            input_names=["input_ids", "attention_mask"],
            output_names=["embeddings", "pooled_output"],
            dynamic_axes={
                "input_ids": {0: "batch_size", 1: "sequence_length"},
                "attention_mask": {0: "batch_size", 1: "sequence_length"},
                "embeddings": {0: "batch_size"},
                "pooled_output": {0: "batch_size"}
            }
        )
        
        print(f"✅ ONNX model saved to {model_path}")
        return model_path
        
    except Exception as e:
        print(f"❌ Error converting to ONNX: {e}")
        return None

def create_test_script(output_dir, tokenizer_func, vocab):
    """Create a test script for the ONNX model."""
    test_script = os.path.join(output_dir, "test_onnx_model.py")
    
    test_code = f'''#!/usr/bin/env python3
"""
Test script for the ONNX embedding model.
"""

import numpy as np
from onnxruntime import InferenceSession

def test_onnx_model():
    """Test the ONNX model with sample text."""
    try:
        # Load ONNX model
        session = InferenceSession("model.onnx")
        
        # Simple tokenizer
        vocab = {vocab}
        
        def tokenize(text, max_length=512):
            words = text.lower().split()
            token_ids = [vocab.get(word, vocab["<UNK>"]) for word in words]
            
            if len(token_ids) > max_length:
                token_ids = token_ids[:max_length]
            else:
                token_ids.extend([vocab["<PAD>"]] * (max_length - len(token_ids)))
            
            attention_mask = [1 if token != vocab["<PAD>"] else 0 for token in token_ids]
            return token_ids, attention_mask
        
        # Test with sample text
        texts = [
            "This is a test sentence.",
            "Another example for testing.",
            "The quick brown fox jumps over the lazy dog."
        ]
        
        print("🧪 Testing ONNX model with sample texts...")
        
        for text in texts:
            # Tokenize input
            token_ids, attention_mask = tokenize(text)
            
            # Convert to numpy arrays
            input_ids = np.array([token_ids], dtype=np.int64)
            attention_mask = np.array([attention_mask], dtype=np.int64)
            
            # Run inference
            outputs = session.run(
                None,
                {{
                    "input_ids": input_ids,
                    "attention_mask": attention_mask
                }}
            )
            
            embeddings = outputs[0]
            pooled = outputs[1]
            
            print(f"✅ Text: '{{text}}'")
            print(f"   Embedding shape: {{embeddings.shape}}")
            print(f"   Embedding norm: {{np.linalg.norm(embeddings):.4f}}")
            print(f"   Pooled shape: {{pooled.shape}}")
            print(f"   Pooled norm: {{np.linalg.norm(pooled):.4f}}")
            print()
        
        print("🎉 ONNX model test completed successfully!")
        
    except Exception as e:
        print(f"❌ Error testing ONNX model: {{e}}")

if __name__ == "__main__":
    test_onnx_model()
'''
    
    with open(test_script, 'w') as f:
        f.write(test_code)
    
    os.chmod(test_script, 0o755)
    print(f"✅ Test script created: {test_script}")

def main():
    """Main function to create the test model."""
    print("🚀 Creating test ONNX embedding model...")
    
    # Set up paths
    models_dir = Path("models")
    output_dir = models_dir / "test-embedding-onnx"
    
    print(f"📁 Output directory: {output_dir}")
    os.makedirs(output_dir, exist_ok=True)
    
    # Create model and tokenizer
    model = create_test_model()
    tokenizer_func, vocab = create_test_tokenizer()
    
    # Convert to ONNX
    model_path = convert_to_onnx(model, str(output_dir))
    if model_path is None:
        return False
    
    # Create test script
    create_test_script(str(output_dir), tokenizer_func, vocab)
    
    print(f"\n🎉 Successfully created test ONNX embedding model!")
    print(f"📁 Model directory: {output_dir}")
    print(f"🔧 Test the model: python3 {output_dir}/test_onnx_model.py")
    print(f"📝 ONNX model: {model_path}")
    
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
