# Vector Database Similarity Search

## Overview

Grapa provides comprehensive vector similarity search capabilities that can serve as a powerful alternative to dedicated vector databases like Pinecone. This guide covers the unified similarity search system, database integration, and advanced features that make Grapa competitive with specialized vector database solutions.

## Key Features

### Unified Similarity Algorithms
Grapa's `.similarity()` method supports multiple algorithms in a single, consistent API:

- **Levenshtein Distance** - Edit distance for string similarity
- **Damerau-Levenshtein Distance** - Edit distance with transposition support
- **Jaro-Winkler Similarity** - String similarity with prefix matching
- **Cosine Similarity** - Vector similarity with TF-IDF support
- **Jaccard Similarity** - Set-based similarity with configurable n-grams

### Advanced Options
- **TF-IDF Cosine Similarity** - Term frequency-inverse document frequency weighting
- **Word-based Jaccard** - Token-based similarity with configurable n-gram sizes
- **Case Sensitivity Control** - Configurable case handling
- **Corpus-based Analysis** - Context-aware similarity calculations

### Database Integration
- **In-memory Tables** - High-performance similarity search
- **$ID Optimization** - Faster lookups using integer IDs vs string keys
- **Metadata Filtering** - Object-based similarity with field matching
- **Schema Validation** - Proper database structure maintenance

## Grapa vs Pinecone Comparison

| Feature | Grapa `.similarity()` | Pinecone | Winner |
|---------|----------------------|----------|---------|
| **Similarity Algorithms** | 5+ algorithms (levenshtein, damerau, jaro, cosine, jaccard) | Primarily cosine | 🏆 **Grapa** |
| **Array Search** | ✅ Native support | ✅ Native support | 🤝 **Tie** |
| **Metadata Search** | ✅ Object field matching | ✅ Metadata filtering | 🤝 **Tie** |
| **Result Control** | ✅ top_n, threshold, sort, include_scores | ✅ top_k, score_threshold, include_metadata | 🤝 **Tie** |
| **Persistence** | ❌ In-memory only | ✅ Persistent database | 🏆 **Pinecone** |
| **Scale** | ❌ Memory limited | ✅ Billions of vectors | 🏆 **Pinecone** |
| **Performance** | ❌ O(n) for each search | ✅ Optimized indexing | 🏆 **Pinecone** |
| **Ease of Use** | ✅ Native language integration | ❌ Requires API calls | 🏆 **Grapa** |
| **Cost** | ✅ Free (local) | ❌ Pay-per-use | 🏆 **Grapa** |
| **Advanced Options** | ✅ TF-IDF, word-based jaccard, configurable parameters | ❌ Limited options | 🏆 **Grapa** |

## Use Case Recommendations

### Use Grapa `.similarity()` when:
- ✅ **Small to medium datasets** (< 1M items)
- ✅ **Prototyping and development**
- ✅ **Local/offline applications**
- ✅ **Complex similarity algorithms** needed
- ✅ **Native language integration** preferred
- ✅ **Cost-sensitive** applications
- ✅ **Real-time data** that changes frequently

### Use Pinecone when:
- ✅ **Large-scale production** (> 1M items)
- ✅ **High-performance requirements** (sub-second latency)
- ✅ **Persistent storage** needed
- ✅ **Cloud deployment**
- ✅ **Simple cosine similarity** sufficient
- ✅ **Budget available** for managed service

## Basic Usage

### String Similarity
```grapa
str1 = "hello world";
str2 = "hello there";

/* Test different similarity algorithms */
levenshtein_sim = str1.similarity(str2, "levenshtein");
cosine_sim = str1.similarity(str2, "cosine");
jaccard_sim = str1.similarity(str2, "jaccard");

("Levenshtein: " + levenshtein_sim.toString() + "\n").echo();
("Cosine: " + cosine_sim.toString() + "\n").echo();
("Jaccard: " + jaccard_sim.toString() + "\n").echo();
```

### Array Similarity Search
```grapa
documents = [
    "machine learning algorithms",
    "artificial intelligence research", 
    "deep learning neural networks",
    "computer vision applications",
    "natural language processing"
];

query = "machine learning";

/* Basic similarity search */
results = documents.similarity(query, "cosine", {
    "top_n": 3,
    "include_scores": true
});

/* Display results */
for i in results.range() {
    result = results[i];
    ("Score: " + result.similarity.toString() + " - " + result.item + "\n").echo();
}
```

### Object Array Similarity (Metadata Search)
```grapa
user_profiles = [
    {"name": "Alice", "age": 30, "skills": ["python", "ml"], "location": "NYC"},
    {"name": "Bob", "age": 25, "skills": ["java", "web"], "location": "SF"},
    {"name": "Charlie", "age": 35, "skills": ["python", "ai"], "location": "NYC"}
];

query_profile = {"age": 30, "skills": ["python"], "location": "NYC"};

/* Object similarity search */
results = user_profiles.similarity(query_profile, "cosine", {
    "top_n": 2,
    "include_scores": true,
    "include_items": true
});

/* Display results */
for i in results.range() {
    result = results[i];
    ("Score: " + result.similarity.toString() + " - " + result.item.name + " (age: " + result.item.age.toString() + ")\n").echo();
}
```

## Advanced Features

### TF-IDF Cosine Similarity
```grapa
documents = [
    "machine learning algorithms",
    "artificial intelligence research", 
    "deep learning neural networks"
];

query = "machine learning";

/* Advanced cosine similarity with TF-IDF */
results = documents.similarity(query, "cosine", {
    "cosine_method": "tfidf",
    "corpus": documents,
    "case_sensitive": false,
    "top_n": 3,
    "include_scores": true
});
```

### Word-based Jaccard Similarity
```grapa
documents = [
    "the quick brown fox",
    "the fast brown dog",
    "a quick brown cat"
];

query = "the quick brown";

/* Word-based jaccard with configurable n-grams */
results = documents.similarity(query, "jaccard", {
    "jaccard_method": "word",
    "jaccard_n": 2,
    "case_sensitive": false,
    "top_n": 3,
    "include_scores": true
});
```

### Vector Mathematical Operations
```grapa
/* Create test vectors representing document embeddings */
vector1 = #[1.0, 0.0, 0.0, 0.0, 0.0]#;  /* machine learning - perfect match */
vector2 = #[0.8, 0.6, 0.0, 0.0, 0.0]#;  /* artificial intelligence - high similarity */
vector3 = #[0.6, 0.8, 0.0, 0.0, 0.0]#;  /* deep learning - medium similarity */
vector4 = #[0.0, 0.0, 0.0, 1.0, 0.0]#;  /* computer vision - low similarity */
vector5 = #[0.0, 0.0, 0.0, 0.0, 1.0]#;  /* natural language processing - no similarity */

vectors = [vector1, vector2, vector3, vector4, vector5];
query_vector = #[1.0, 0.0, 0.0, 0.0, 0.0]#;  /* machine learning query */

/* Vector similarity search */
results = vectors.similarity(query_vector, "cosine", {
    "top_n": 3,
    "include_scores": true,
    "include_items": true
});

/* Results will show realistic cosine similarity scores:
   - Score: 1.0 (perfect match)
   - Score: 0.8 (high similarity) 
   - Score: 0.6 (medium similarity)
*/
```

## Database Integration

### In-Memory Vector Database
```grapa
/* Create in-memory table for vector storage */
vector_db = {}.table();

/* Insert vector data with metadata */
vector_db.set("doc1", "id", "document_1");
vector_db.set("doc1", "content", "machine learning algorithms");
vector_db.set("doc1", "type", "article");
vector_db.set("doc1", "category", "tech");
vector_db.set("doc1", "author", "John");

vector_db.set("doc2", "id", "document_2");
vector_db.set("doc2", "content", "artificial intelligence research");
vector_db.set("doc2", "type", "article");
vector_db.set("doc2", "category", "science");
vector_db.set("doc2", "author", "Jane");
```

### Performance Optimization with $ID
```grapa
/* Get all records for performance testing */
all_records = vector_db.ls();

/* Use $ID for faster lookups */
for i in all_records.range() {
    record = all_records[i];
    record_id = record["$ID"];
    
    /* Fast field access using $ID */
    content = vector_db.getfield(record_id, "content");
    type = vector_db.getfield(record_id, "type");
    category = vector_db.getfield(record_id, "category");
}
```

### Database Filtering and Similarity Search
```grapa
/* Manual filtering using $ID for performance */
tech_documents = [];
for i in all_records.range() {
    record = all_records[i];
    record_id = record["$ID"];
    record_type = vector_db.getfield(record_id, "type");
    record_category = vector_db.getfield(record_id, "category");
    
    if (record_type == "article" && record_category == "tech") {
        content = vector_db.getfield(record_id, "content");
        tech_documents.push(content);
    }
}

/* Perform similarity search on filtered results */
query_text = "machine learning";
results = tech_documents.similarity(query_text, "cosine", {
    "top_n": 2,
    "include_scores": true,
    "include_items": true
});
```

## Performance Optimization

### $ID vs $KEY Performance
```grapa
/* Benchmark $KEY lookup performance */
start_time = $TIME().utc();
for i in (1000).range() {
    key = "doc" + ((i % 3) + 1).toString();
    content = vector_db.getfield(key, "content");
}
key_lookup_time = $TIME().utc();
key_duration = key_lookup_time.ms() - start_time.ms();

/* Benchmark $ID lookup performance */
start_time = $TIME().utc();
for i in (1000).range() {
    record_id = all_records[(i % 3)]["$ID"];
    content = vector_db.getfield(record_id, "content");
}
id_lookup_time = $TIME().utc();
id_duration = id_lookup_time.ms() - start_time.ms();

/* Calculate performance improvement */
improvement = key_duration / id_duration;
("Performance improvement: " + improvement.toString() + "x faster with $ID\n").echo();
```

### Large Dataset Performance
```grapa
/* Create larger dataset for performance testing */
large_dataset = [];
for i in (1000).range() {
    large_dataset.push("document " + i.toString() + " about machine learning and artificial intelligence");
}

query = "machine learning";

/* Benchmark similarity search performance */
start_time = $TIME().utc();
results = large_dataset.similarity(query, "cosine", {
    "top_n": 10,
    "include_scores": true
});
end_time = $TIME().utc();
duration = end_time.ms() - start_time.ms();

("Dataset size: " + large_dataset.len().toString() + " documents\n").echo();
("Search time: " + duration.toString() + "ms\n").echo();
("Results found: " + results.len().toString() + "\n").echo();
```

## Error Handling and Edge Cases

### Empty Array Handling
```grapa
empty_array = [];
empty_results = empty_array.similarity("test", "cosine");
("Empty array similarity: " + empty_results.len().toString() + " results\n").echo();
```

### Single Item Arrays
```grapa
single_array = ["single item"];
single_results = single_array.similarity("single item", "cosine");
("Single item similarity: " + single_results.len().toString() + " results\n").echo();
```

### Threshold Filtering
```grapa
results = documents.similarity(query, "cosine", {
    "top_n": 10,
    "threshold": 0.5,
    "include_scores": true
});
("Results above 0.5 threshold: " + results.len().toString() + "\n").echo();
```

## Best Practices

### 1. Use $ID for Database Operations
- **$ID lookups** are significantly faster than $KEY lookups
- **Integer comparison** vs string comparison performance
- **Better cache locality** for bulk operations

### 2. Optimize Similarity Algorithm Selection
- **Cosine similarity** for text documents and vectors
- **Jaccard similarity** for set-based data
- **Levenshtein/Damerau** for edit distance requirements
- **Jaro-Winkler** for name matching and fuzzy string search

### 3. Leverage Advanced Options
- **TF-IDF** for document similarity with corpus context
- **Word-based jaccard** for token-based similarity
- **Case sensitivity** control for your use case
- **Configurable n-grams** for jaccard similarity

### 4. Database Schema Design
- **Separate fields** for metadata instead of complex objects
- **Index-friendly** field structures
- **$ID-based** primary access patterns
- **Consistent field naming** conventions

### 5. Performance Monitoring
- **Benchmark** similarity search performance
- **Monitor** memory usage with large datasets
- **Profile** database operations
- **Optimize** based on actual usage patterns

## Future Enhancements

### Planned Features
- **Persistent database** integration for large-scale storage
- **Enhanced database querying** with multi-field search
- **Index optimization** for very large datasets
- **Distributed similarity** search across multiple databases
- **Real-time indexing** for dynamic data

### Implementation Roadmap
1. **Phase 1**: Enhanced database methods (`.search()`, `.filter()`)
2. **Phase 2**: Optimized indexing and query planning
3. **Phase 3**: Persistent storage integration
4. **Phase 4**: Advanced similarity algorithms and optimizations

## Conclusion

Grapa's vector database similarity search provides a powerful, cost-effective alternative to dedicated vector databases like Pinecone. With its unified API, advanced similarity algorithms, and native language integration, Grapa offers significant advantages for many use cases while maintaining competitive performance for small to medium-scale applications.

The system's flexibility, combined with its comprehensive similarity algorithms and database integration capabilities, makes it an excellent choice for prototyping, development, and production applications where cost, simplicity, and algorithmic diversity are important factors.
