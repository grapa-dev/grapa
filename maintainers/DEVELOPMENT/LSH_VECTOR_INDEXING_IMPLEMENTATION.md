# LSH Vector Indexing Implementation Plan

## Overview

This document outlines the implementation plan for Locality Sensitive Hashing (LSH) vector indexing in Grapa to address performance issues with similarity search on datasets larger than 20K records. The approach uses an in-memory hash index that maps LSH hashes to database record IDs, allowing vectors to remain in persistent storage while providing fast approximate similarity search.

## Problem Statement

Current Grapa similarity search performance degrades significantly above 20K records:
- **10K records**: ~85ms (excellent)
- **20K records**: ~1,482ms (acceptable)
- **25K records**: ~2,250ms (slow)
- **50K records**: ~9,067ms (very slow)
- **100K records**: ~40,968ms (unusable)

The current implementation performs O(n) linear search through all vectors, calculating similarity for each one individually.

## Proposed Solution: LSH with Database Integration

### Core Concept

- **In-Memory Index**: `{}.table()` with LSH hash as `$KEY` and database record `$ID` as `$VALUE`
- **Database Storage**: Vectors remain in the main database table
- **Lookup Process**: Hash query → Get candidate IDs → Fetch vectors from DB → Calculate similarity

### Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Query Vector  │───▶│   LSH Hash Gen   │───▶│  Hash Lookup    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                                         │
                                                         ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  Final Results  │◀───│ Similarity Calc  │◀───│ Candidate IDs   │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │                        │
                                ▼                        ▼
                       ┌──────────────────┐    ┌─────────────────┐
                       │  Vector Storage  │    │  LSH Index      │
                       │  (Database)      │    │  (In-Memory)    │
                       └──────────────────┘    └─────────────────┘
```

## Implementation Plan

### Phase 1: Core LSH Infrastructure

#### 1.1 LSH Hash Generation
**File**: `source/grapa/GrapaVector.cpp`
**Method**: `GrapaVector::GenerateLSHHash()`

```cpp
GrapaError GrapaVector::GenerateLSHHash(
    GrapaScriptExec* pScriptExec,
    GrapaNames* pNameSpace,
    GrapaCHAR& method,
    int num_hashes,
    int hash_size,
    GrapaCHAR& result_hash
) {
    // Generate multiple hash functions (random hyperplanes)
    std::vector<std::vector<float>> hash_functions;
    for (int i = 0; i < num_hashes; i++) {
        std::vector<float> hyperplane;
        for (u64 j = 0; j < mCounts[0]; j++) {
            hyperplane.push_back(random_normal());
        }
        hash_functions.push_back(hyperplane);
    }
    
    // Calculate hash bits
    std::string hash_bits;
    for (int i = 0; i < num_hashes; i++) {
        GrapaFloat dot_product = 0;
        for (u64 j = 0; j < mCounts[0]; j++) {
            GrapaVectorParam p(pScriptExec, mData, mBlock, j);
            dot_product = dot_product + (*p.aa * hash_functions[i][j]);
        }
        hash_bits += (dot_product >= 0) ? "1" : "0";
    }
    
    result_hash = GrapaCHAR(hash_bits.c_str());
    return 0;
}
```

#### 1.2 LSH Index Management
**File**: `source/grapa/GrapaLibRule.cpp`
**New Functions**:
- `build_lsh_index()`
- `search_with_lsh_index()`
- `update_lsh_index()`

### Phase 2: Database Integration

#### 2.1 Enhanced Similarity Search
**File**: `source/grapa/GrapaLibRule.cpp`
**Method**: `calculate_array_vector_similarity_with_lsh()`

```cpp
GrapaRuleEvent* calculate_array_vector_similarity_with_lsh(
    GrapaScriptExec* vScriptExec,
    GrapaNames* pNameSpace,
    GrapaRuleEvent* array,
    GrapaRuleEvent* query,
    GrapaCHAR& method,
    int top_n,
    double threshold,
    GrapaRuleEvent* lsh_index = NULL
) {
    if (lsh_index && lsh_index->vQueue) {
        return search_with_lsh_index(vScriptExec, pNameSpace, query, method, top_n, threshold, lsh_index);
    } else {
        return calculate_array_vector_similarity_original(vScriptExec, pNameSpace, array, query, method, top_n, threshold, "desc", true, true);
    }
}
```

#### 2.2 Database Table Extensions
**File**: `source/grapa/GrapaTable.cpp`
**New Methods**:
- `BuildSimilarityIndex()`
- `SimilaritySearch()`
- `UpdateSimilarityIndex()`

### Phase 3: Grapa Language Integration

#### 3.1 New Grapa Methods
```grapa
/* LSH Index Management */
vector_db.build_lsh_index(vector_field="vector", num_hashes=8, hash_size=4);
vector_db.similarity_search(query_vector, method="cosine", top_n=10, use_lsh=true);
vector_db.update_lsh_index(record_id, vector);

/* Direct LSH Operations */
hash = vector.lsh_hash("cosine", num_hashes=8, hash_size=4);
similarity = hash1.similarity(hash2, "hamming");
```

#### 3.2 Configuration Options
```grapa
lsh_config = {
    "num_hashes": 8,           /* More hashes = more accurate, slower */
    "hash_size": 4,            /* Hash size in bits */
    "similarity_threshold": 0.5, /* Hash similarity threshold */
    "candidate_limit": 1000,   /* Max candidates to check */
    "method": "cosine"         /* Similarity method */
};
```

## Performance Projections

| Dataset Size | Index Size | Search Time | Memory Usage | Improvement |
|--------------|------------|-------------|--------------|-------------|
| 10K records | ~80KB | ~10ms | ~100KB | 8.5x faster |
| 100K records | ~800KB | ~50ms | ~1MB | 180x faster |
| 1M records | ~8MB | ~200ms | ~10MB | 45x faster |
| 10M records | ~80MB | ~1s | ~100MB | 40x faster |

## Implementation Details

### LSH Algorithm Selection

**Cosine Similarity LSH**:
- Uses random hyperplanes to partition vector space
- Hash function: `h(v) = sign(v · r)` where `r` is random vector
- Multiple hash functions create multiple partitions
- Similar vectors likely to hash to same buckets

### Hash Collision Handling

```cpp
// Multiple hash functions reduce false positives
std::vector<std::string> GenerateMultipleHashes(GrapaVector* vector, int num_hashes) {
    std::vector<std::string> hashes;
    for (int i = 0; i < num_hashes; i++) {
        GrapaCHAR hash;
        vector->GenerateLSHHash(pScriptExec, pNameSpace, method, 1, hash_size, hash);
        hashes.push_back(hash.Str());
    }
    return hashes;
}
```

### Candidate Selection Strategy

1. **Hash Similarity**: Use Hamming distance between query hash and index hashes
2. **Threshold Filtering**: Only consider hashes with similarity > threshold
3. **Candidate Limiting**: Limit number of candidates to prevent performance degradation
4. **Exact Similarity**: Calculate exact similarity only for candidates

### Memory Management

- **Index Size**: `num_records × hash_size × num_hashes` bits
- **Compression**: Use bit-packed storage for hash strings
- **Cleanup**: Automatic cleanup of unused hash entries
- **Persistence**: Optional persistence of LSH index to disk

## Testing Strategy

### Unit Tests
- LSH hash generation consistency
- Hash similarity calculation accuracy
- Index building and searching
- Memory usage validation

### Performance Tests
- Benchmark against current implementation
- Test with various dataset sizes (1K, 10K, 100K, 1M records)
- Measure accuracy vs speed trade-offs
- Memory usage profiling

### Integration Tests
- Database integration
- Incremental updates
- Error handling
- Configuration validation

## Risk Assessment

### Technical Risks
- **Hash Collisions**: May miss similar vectors
- **Memory Usage**: Index size grows with dataset
- **Accuracy Trade-off**: Approximate search may miss some results
- **Complexity**: Adds significant complexity to codebase

### Mitigation Strategies
- **Tunable Parameters**: Allow users to adjust accuracy vs speed
- **Fallback Mechanism**: Use current implementation if LSH fails
- **Comprehensive Testing**: Extensive testing with real-world data
- **Documentation**: Clear documentation of limitations and trade-offs

## Success Criteria

### Performance Targets
- **100K records**: < 100ms search time
- **1M records**: < 500ms search time
- **10M records**: < 2s search time
- **Memory usage**: < 1GB for 10M records

### Accuracy Targets
- **Recall**: > 95% for top-10 results
- **Precision**: > 90% for top-10 results
- **Configurable**: Users can tune accuracy vs speed

### Usability Targets
- **Backward Compatible**: Existing code continues to work
- **Easy Configuration**: Simple configuration options
- **Clear Documentation**: Comprehensive usage examples
- **Error Handling**: Graceful degradation on errors

## Future Enhancements

### Phase 4: Advanced Optimizations
- **Multi-level Indexing**: Hierarchical LSH for very large datasets
- **GPU Acceleration**: CUDA/OpenCL for hash generation
- **Distributed Indexing**: Multi-machine LSH index
- **Adaptive Parameters**: Automatic parameter tuning

### Phase 5: Additional Algorithms
- **Product Quantization**: For memory-constrained environments
- **Annoy Integration**: Facebook's approximate nearest neighbors
- **Faiss Integration**: Meta's similarity search library
- **Custom Distance Functions**: User-defined similarity metrics

## Conclusion

The LSH vector indexing implementation will significantly improve Grapa's similarity search performance for large datasets while maintaining the advantages of local control and algorithm variety. The phased approach allows for incremental implementation and testing, reducing risk while delivering immediate benefits.

The key innovation is using Grapa's existing database infrastructure with `$ID` as the bridge between in-memory hash indexes and persistent vector storage, making the solution both scalable and memory-efficient.
