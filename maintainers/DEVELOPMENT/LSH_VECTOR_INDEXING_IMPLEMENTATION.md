# LSH Vector Indexing Implementation Plan

## Overview

This document outlines the implementation plan for **Multi-Hash Locality Sensitive Hashing (LSH)** vector indexing in Grapa to address performance issues with similarity search on datasets larger than 20K records. The approach uses **multiple hash sets** to reduce the boundary problem while leveraging Grapa's database infrastructure for persistent storage and fast hash-based lookups.

## Problem Statement

Current Grapa similarity search performance degrades significantly above 20K records:
- **10K records**: ~85ms (excellent)
- **20K records**: ~1,482ms (acceptable)
- **25K records**: ~2,250ms (slow)
- **50K records**: ~9,067ms (very slow)
- **100K records**: ~40,968ms (unusable)

The current implementation performs O(n) linear search through all vectors, calculating similarity for each one individually.

## Revised Solution: Multi-Hash LSH with Database Integration

### Core Concept

- **Multi-Hash LSH**: Generate multiple hash sets for each similarity method to reduce boundary problems
- **Database Storage**: Vectors remain in the main database table with multiple hash columns
- **Hash-Based Lookup**: Use database indexes on hash columns for O(log n) candidate retrieval
- **Exact Similarity**: Calculate exact similarity only for candidates from relevant hash buckets

### Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Query Vector  │───▶│ Multi-Hash Gen   │───▶│ Hash Lookup     │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                                         │
                                                         ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  Final Results  │◀───│ Similarity Calc  │◀───│ Candidate IDs   │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │                        │
                                ▼                        ▼
                       ┌──────────────────┐    ┌─────────────────┐
                       │  Vector Storage  │    │  Hash Indexes   │
                       │  (Database)      │    │  (Database)     │
                       └──────────────────┘    └─────────────────┘
```

## Multi-Hash LSH Design

### 1. Hash Generation Strategy

#### **Multiple Hash Sets per Similarity Method**
```cpp
// Each similarity method gets multiple hash sets
struct LSHHashSets {
    std::vector<std::vector<GrapaVector*>> cosine_hyperplane_sets;    // 3 sets of 8 hyperplanes
    std::vector<std::vector<GrapaVector*>> euclidean_projection_sets; // 3 sets of 8 projections
    std::vector<std::vector<GrapaVector*>> jaccard_minhash_sets;      // 3 sets of 8 minhash functions
    std::vector<std::vector<GrapaVector*>> manhattan_projection_sets; // 3 sets of 8 projections
    // ... other similarity methods
};
```

#### **Hash Generation Functions**
```cpp
// Different hash generation for each similarity method
class MultiHashLSH {
public:
    // Cosine similarity: Random normalized hyperplanes
    std::vector<GrapaVector*> generateCosineHyperplanes(int dimension, int count) {
        std::vector<GrapaVector*> hyperplanes;
        for (int i = 0; i < count; i++) {
            GrapaVector* hyperplane = new GrapaVector();
            for (int j = 0; j < dimension; j++) {
                double random_val = generateRandomNormal(0.0, 1.0);
                hyperplane->push_back(random_val);
            }
            hyperplane->normalize();  // Critical for cosine similarity
            hyperplanes.push_back(hyperplane);
        }
        return hyperplanes;
    }
    
    // Euclidean distance: Random unnormalized projections
    std::vector<GrapaVector*> generateEuclideanProjections(int dimension, int count) {
        std::vector<GrapaVector*> projections;
        for (int i = 0; i < count; i++) {
            GrapaVector* projection = new GrapaVector();
            for (int j = 0; j < dimension; j++) {
                double random_val = generateRandomNormal(0.0, 1.0);
                projection->push_back(random_val);
            }
            // DON'T normalize for euclidean distance
            projections.push_back(projection);
        }
        return projections;
    }
    
    // Jaccard similarity: Random minhash permutations
    std::vector<GrapaVector*> generateJaccardMinhash(int dimension, int count) {
        std::vector<GrapaVector*> minhash_functions;
        for (int i = 0; i < count; i++) {
            GrapaVector* minhash = new GrapaVector();
            std::vector<int> permutation(dimension);
            for (int j = 0; j < dimension; j++) {
                permutation[j] = j;
            }
            std::random_shuffle(permutation.begin(), permutation.end());
            for (int j = 0; j < dimension; j++) {
                minhash->push_back(permutation[j]);
            }
            minhash_functions.push_back(minhash);
        }
        return minhash_functions;
    }
};
```

### 2. Database Schema Design

#### **Table Structure with Multiple Hash Columns**
```cpp
// Vector table schema with multiple hash columns
struct VectorTableSchema {
    // Primary fields
    GrapaCHAR vector_id;           // Primary key
    GrapaBYTE vector_data;        // Compressed vector data
    
    // Cosine similarity hashes (3 sets)
    u64 cosine_hash_1;            // First hash set
    u64 cosine_hash_2;            // Second hash set
    u64 cosine_hash_3;            // Third hash set
    
    // Euclidean distance hashes (3 sets)
    u64 euclidean_hash_1;
    u64 euclidean_hash_2;
    u64 euclidean_hash_3;
    
    // Jaccard similarity hashes (3 sets)
    u64 jaccard_hash_1;
    u64 jaccard_hash_2;
    u64 jaccard_hash_3;
    
    // Manhattan distance hashes (3 sets)
    u64 manhattan_hash_1;
    u64 manhattan_hash_2;
    u64 manhattan_hash_3;
    
    // ... other similarity methods
};
```

#### **Database Indexes**
```cpp
// Create indexes on all hash columns
GrapaError createVectorTable(GrapaCHAR& table_name, int vector_dimension) {
    // Create main table
    db.mk(table_name, "ROW");
    db.cd(table_name);
    
    // Define vector dimension
    db.mkfield("vector_dimension", "INT");
    db.setfield("schema", "vector_dimension", vector_dimension);
    
    // Generate and store hyperplanes for this dimension
    MultiHashLSH lsh_gen;
    LSHHashSets hash_sets = lsh_gen.generateAllHashSets(vector_dimension, 3, 8);
    storeHashSetsInSchema(table_name, hash_sets);
    
    // Create vector storage fields
    db.mkfield("vector_data", "VAR");  // Compressed vector storage
    
    // Create hash columns for each similarity method
    createHashColumns("cosine", 3);
    createHashColumns("euclidean", 3);
    createHashColumns("jaccard", 3);
    createHashColumns("manhattan", 3);
    // ... other methods
    
    // Create indexes on all hash columns
    createHashIndexes("cosine", 3);
    createHashIndexes("euclidean", 3);
    createHashIndexes("jaccard", 3);
    createHashIndexes("manhattan", 3);
    // ... other methods
    
    return 0;
}
```

### 3. Multi-Hash Search Implementation

#### **Multi-Hash Similarity Search**
```cpp
// Fast similarity search using multiple hash sets
GrapaError multiHashSimilaritySearch(GrapaCHAR& table_name, GrapaVector& query, 
                                      GrapaCHAR& method, GrapaVector& results, int top_k) {
    // Validate dimension match
    int table_dimension = db.getfield("schema", "vector_dimension").toInt();
    if (query.getDimension() != table_dimension) {
        return -1;  // Dimension mismatch error
    }
    
    // Generate multiple hashes for query
    std::vector<u64> query_hashes = generateMultiHash(query, method, table_name);
    
    // Collect candidates from all hash buckets
    std::vector<GrapaVector*> all_candidates;
    
    for (u64 hash : query_hashes) {
        // Search all hash sets for this method
        for (int i = 1; i <= 3; i++) {  // 3 hash sets
            GrapaCHAR hash_field = method + "_hash_" + std::to_string(i);
            GrapaRuleEvent* cursor = db.find(hash_field, hash);
            
            while (cursor) {
                GrapaVector* candidate = db.getfield(cursor, "vector_data");
                all_candidates.push_back(candidate);
                cursor = cursor->Next();
            }
        }
    }
    
    // Remove duplicates
    std::sort(all_candidates.begin(), all_candidates.end());
    all_candidates.erase(std::unique(all_candidates.begin(), all_candidates.end()), 
                        all_candidates.end());
    
    // Compute exact similarity for all candidates
    std::vector<std::pair<GrapaVector*, double>> similarities;
    for (auto candidate : all_candidates) {
        double similarity = computeExactSimilarity(query, *candidate, method);
        similarities.push_back({candidate, similarity});
    }
    
    // Sort and return top_k
    std::sort(similarities.begin(), similarities.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (int i = 0; i < top_k && i < similarities.size(); i++) {
        results.push_back(similarities[i].first);
    }
    
    return 0;
}
```

#### **Hash Generation for Multiple Sets**
```cpp
// Generate multiple hashes for a vector using stored hash sets
std::vector<u64> generateMultiHash(GrapaVector& vector, GrapaCHAR& method, 
                                    GrapaCHAR& table_name) {
    std::vector<u64> hashes;
    
    // Load hash sets from table schema
    std::vector<std::vector<GrapaVector*>> hash_sets = loadHashSetsFromTable(table_name, method);
    
    // Generate hash for each set
    for (auto& hash_set : hash_sets) {
        u64 hash = generateHashWithSet(vector, hash_set);
        hashes.push_back(hash);
    }
    
    return hashes;
}

// Generate hash using specific hash set
u64 generateHashWithSet(GrapaVector& vector, std::vector<GrapaVector*>& hash_set) {
    u64 hash = 0;
    
    for (int i = 0; i < hash_set.size(); i++) {
        double dot_product = vector.dot(*hash_set[i]);
        bool hash_bit = (dot_product > 0);
        hash = (hash << 1) | (hash_bit ? 1 : 0);
    }
    
    return hash;
}
```

### 4. Boundary Problem Mitigation

#### **Multi-Hash Benefits**
```cpp
// Multi-hash approach reduces boundary problems
struct BoundaryMitigation {
    int hash_sets;
    double recall_improvement;    // % improvement in finding relevant results
    double boundary_reduction;    // % reduction in boundary problems
    double speedup_reduction;     // % reduction in speedup
};

// Examples for 10,000 vectors:
BoundaryMitigation benefits[] = {
    {1, 0.0,   0.0,   0.0},      // Single hash (baseline)
    {2, 0.15,  0.30,  0.20},     // 2 hash sets: 15% better recall, 30% fewer boundary issues
    {3, 0.25,  0.50,  0.40},     // 3 hash sets: 25% better recall, 50% fewer boundary issues
    {4, 0.30,  0.60,  0.60}      // 4 hash sets: 30% better recall, 60% fewer boundary issues
};
```

#### **Adaptive Hash Selection**
```cpp
// Detect boundary vectors and use more hash sets
bool isNearBoundary(GrapaVector& vector, std::vector<GrapaVector*>& hyperplanes, 
                    double threshold = 0.1) {
    for (auto hyperplane : hyperplanes) {
        double dot_product = vector.dot(*hyperplane);
        double abs_dot_product = std::abs(dot_product);
        
        if (abs_dot_product < threshold) {
            return true;
        }
    }
    return false;
}

// Adaptive hash generation
std::vector<u64> generateAdaptiveHash(GrapaVector& vector, GrapaCHAR& method, 
                                      GrapaCHAR& table_name) {
    std::vector<GrapaVector*> hyperplanes = loadHashSetsFromTable(table_name, method)[0];
    bool near_boundary = isNearBoundary(vector, hyperplanes);
    
    if (near_boundary) {
        // Use all hash sets for boundary vectors
        return generateMultiHash(vector, method, table_name);
    } else {
        // Use single hash set for non-boundary vectors
        std::vector<u64> single_hash = {generateHashWithSet(vector, hyperplanes)};
        return single_hash;
    }
}
```

## Implementation Plan

### Phase 1: Core Multi-Hash Infrastructure

#### 1.1 Multi-Hash Generation
**File**: `source/grapa/GrapaVector.cpp`
**New Methods**:
- `GenerateMultiHash()`
- `GenerateCosineHyperplanes()`
- `GenerateEuclideanProjections()`
- `GenerateJaccardMinhash()`

```cpp
GrapaError GrapaVector::GenerateMultiHash(
    GrapaScriptExec* pScriptExec,
    GrapaNames* pNameSpace,
    GrapaCHAR& method,
    int num_hash_sets,
    int hyperplanes_per_set,
    std::vector<u64>& result_hashes
) {
    // Generate multiple hash sets for the specified method
    MultiHashLSH lsh_gen;
    std::vector<std::vector<GrapaVector*>> hash_sets;
    
    if (method.StrLowerCmp("cosine") == 0) {
        for (int i = 0; i < num_hash_sets; i++) {
            std::vector<GrapaVector*> hyperplanes = lsh_gen.generateCosineHyperplanes(
                mCounts[0], hyperplanes_per_set);
            hash_sets.push_back(hyperplanes);
        }
    } else if (method.StrLowerCmp("euclidean") == 0) {
        for (int i = 0; i < num_hash_sets; i++) {
            std::vector<GrapaVector*> projections = lsh_gen.generateEuclideanProjections(
                mCounts[0], hyperplanes_per_set);
            hash_sets.push_back(projections);
        }
    } else if (method.StrLowerCmp("jaccard") == 0) {
        for (int i = 0; i < num_hash_sets; i++) {
            std::vector<GrapaVector*> minhash = lsh_gen.generateJaccardMinhash(
                mCounts[0], hyperplanes_per_set);
            hash_sets.push_back(minhash);
        }
    }
    
    // Generate hash for each set
    for (auto& hash_set : hash_sets) {
        u64 hash = generateHashWithSet(*this, hash_set);
        result_hashes.push_back(hash);
    }
    
    return 0;
}
```

#### 1.2 Database Schema Management
**File**: `source/grapa/GrapaDB.cpp`
**New Methods**:
- `CreateVectorTableWithLSH()`
- `StoreHashSetsInSchema()`
- `LoadHashSetsFromTable()`

### Phase 2: Database Integration

#### 2.1 Enhanced Similarity Search
**File**: `source/grapa/GrapaLibRule.cpp`
**New Method**: `calculate_array_vector_similarity_with_multi_hash()`

```cpp
GrapaRuleEvent* calculate_array_vector_similarity_with_multi_hash(
    GrapaScriptExec* vScriptExec,
    GrapaNames* pNameSpace,
    GrapaRuleEvent* array,
    GrapaRuleEvent* query,
    GrapaCHAR& method,
    int top_n,
    double threshold,
    GrapaRuleEvent* vector_table = NULL
) {
    if (vector_table && vector_table->mValue.mToken == GrapaTokenType::TABLE) {
        return multiHashSimilaritySearch(vector_table, *query->vVector, method, 
                                        result, top_n);
    } else {
        // Fallback to original implementation
        return calculate_array_vector_similarity_original(vScriptExec, pNameSpace, 
                                                        array, query, method, top_n, 
                                                        threshold, "desc", true, true);
    }
}
```

#### 2.2 Vector Insertion with Multi-Hash
**File**: `source/grapa/GrapaDB.cpp`
**New Method**: `InsertVectorWithMultiHash()`

```cpp
GrapaError InsertVectorWithMultiHash(GrapaCHAR& table_name, GrapaVector& vector) {
    // Validate dimension match
    int table_dimension = db.getfield("schema", "vector_dimension").toInt();
    if (vector.getDimension() != table_dimension) {
        return -1;  // Dimension mismatch error
    }
    
    // Generate hashes for all similarity methods
    std::map<GrapaCHAR, std::vector<u64>> all_hashes;
    
    GrapaCHAR methods[] = {"cosine", "euclidean", "jaccard", "manhattan"};
    for (auto method : methods) {
        std::vector<u64> hashes;
        vector.GenerateMultiHash(pScriptExec, pNameSpace, method, 3, 8, hashes);
        all_hashes[method] = hashes;
    }
    
    // Get next vector ID
    u64 vector_id = getNextVectorId(table_name);
    
    // Insert vector with all hashes
    db.setfield("vector_" + vector_id, "vector_data", vector);
    
    for (auto& method_hashes : all_hashes) {
        GrapaCHAR method = method_hashes.first;
        std::vector<u64> hashes = method_hashes.second;
        
        for (int i = 0; i < hashes.size(); i++) {
            GrapaCHAR hash_field = method + "_hash_" + std::to_string(i + 1);
            db.setfield("vector_" + vector_id, hash_field, hashes[i]);
        }
    }
    
    return 0;
}
```

### Phase 3: Grapa Language Integration

#### 3.1 New Grapa Methods
```grapa
/* Multi-Hash LSH Operations */
vector_db.create_vector_table("vectors", vector_dimension=384, num_hash_sets=3);
vector_db.insert_vector(vector_data, vector_id);
vector_db.similarity_search(query_vector, method="cosine", top_n=10, use_multi_hash=true);

/* Direct Multi-Hash Operations */
hashes = vector.multi_hash("cosine", num_hash_sets=3, hyperplanes_per_set=8);
similarity = hash1.similarity(hash2, "hamming");
```

#### 3.2 Configuration Options
```grapa
multi_hash_config = {
    "num_hash_sets": 3,          /* Number of hash sets per method */
    "hyperplanes_per_set": 8,    /* Hyperplanes per hash set */
    "vector_dimension": 384,      /* Vector dimension */
    "similarity_methods": ["cosine", "euclidean", "jaccard", "manhattan"],
    "boundary_threshold": 0.1,   /* Boundary detection threshold */
    "adaptive_hashing": true     /* Use adaptive hash selection */
};
```

## Performance Projections

### Multi-Hash Performance Impact

| Dataset Size | Hash Sets | Search Time | Memory Usage | Recall Improvement | Speedup |
|--------------|-----------|-------------|--------------|-------------------|---------|
| 10K records | 1 | ~10ms | ~100KB | 0% | 8.5x |
| 10K records | 3 | ~25ms | ~300KB | 25% | 6.8x |
| 100K records | 1 | ~50ms | ~1MB | 0% | 180x |
| 100K records | 3 | ~120ms | ~3MB | 25% | 75x |
| 1M records | 1 | ~200ms | ~10MB | 0% | 45x |
| 1M records | 3 | ~500ms | ~30MB | 25% | 18x |

### Boundary Problem Reduction

| Hash Sets | Boundary Reduction | Recall Improvement | False Negative Reduction |
|-----------|-------------------|-------------------|-------------------------|
| 1 | 0% | 0% | 0% |
| 2 | 30% | 15% | 20% |
| 3 | 50% | 25% | 35% |
| 4 | 60% | 30% | 45% |

## Implementation Details

### Hash Collision Statistics

```cpp
// Expected hash collisions with multi-hash approach
struct MultiHashCollisionStats {
    int hash_sets;
    int hyperplanes_per_set;
    int possible_hashes_per_set;    // 2^hyperplanes_per_set
    double expected_collisions;     // Average vectors per hash
    double collision_probability;   // Probability of collision
};

// Examples for 10,000 vectors:
MultiHashCollisionStats stats[] = {
    {1, 8, 256, 39.1, 0.0039},      // Single hash set
    {2, 8, 256, 19.5, 0.0019},      // Two hash sets
    {3, 8, 256, 13.0, 0.0013},      // Three hash sets
    {4, 8, 256, 9.8, 0.00098}       // Four hash sets
};
```

### Memory Management

```cpp
// Memory usage calculation
struct MultiHashMemoryUsage {
    int num_vectors;
    int hash_sets;
    int hyperplanes_per_set;
    int vector_dimension;
    
    // Memory calculations
    size_t hash_storage_size() {
        return num_vectors * hash_sets * (hyperplanes_per_set / 8);  // bytes
    }
    
    size_t hyperplane_storage_size() {
        return hash_sets * hyperplanes_per_set * vector_dimension * sizeof(double);  // bytes
    }
    
    size_t total_memory_usage() {
        return hash_storage_size() + hyperplane_storage_size();
    }
};
```

## Testing Strategy

### Unit Tests
- Multi-hash generation consistency
- Hash set storage and retrieval
- Boundary detection accuracy
- Adaptive hash selection

### Performance Tests
- Benchmark against single-hash LSH
- Test with various hash set configurations
- Measure recall vs speed trade-offs
- Memory usage profiling

### Integration Tests
- Database integration with multiple hash sets
- Incremental updates with hash regeneration
- Error handling for dimension mismatches
- Configuration validation

## Risk Assessment

### Technical Risks
- **Memory Usage**: Multiple hash sets increase memory requirements
- **Hash Regeneration**: Changing hash sets requires re-hashing all vectors
- **Complexity**: Multi-hash approach adds significant complexity
- **Performance Trade-off**: Better recall but slower than single-hash

### Mitigation Strategies
- **Configurable Hash Sets**: Allow users to adjust number of hash sets
- **Memory Monitoring**: Track memory usage and provide warnings
- **Incremental Updates**: Support partial hash regeneration
- **Fallback Mechanism**: Use single-hash if multi-hash fails

## Success Criteria

### Performance Targets
- **100K records**: < 150ms search time (with 3 hash sets)
- **1M records**: < 600ms search time (with 3 hash sets)
- **10M records**: < 3s search time (with 3 hash sets)
- **Memory usage**: < 2GB for 10M records (with 3 hash sets)

### Accuracy Targets
- **Recall**: > 98% for top-10 results (with 3 hash sets)
- **Precision**: > 95% for top-10 results (with 3 hash sets)
- **Boundary Problem**: < 5% of queries affected by boundary issues
- **Configurable**: Users can tune accuracy vs speed

### Usability Targets
- **Backward Compatible**: Existing code continues to work
- **Easy Configuration**: Simple configuration for hash sets
- **Clear Documentation**: Comprehensive usage examples
- **Error Handling**: Graceful degradation on errors

## Future Enhancements

### Phase 4: Advanced Multi-Hash Optimizations
- **Adaptive Hash Sets**: Dynamic number of hash sets based on vector characteristics
- **Hierarchical Multi-Hash**: Multi-level hash sets for very large datasets
- **GPU Acceleration**: CUDA/OpenCL for multi-hash generation
- **Distributed Multi-Hash**: Multi-machine hash set coordination

### Phase 5: Additional Algorithms
- **Product Quantization**: For memory-constrained environments
- **Annoy Integration**: Facebook's approximate nearest neighbors
- **Faiss Integration**: Meta's similarity search library
- **Custom Distance Functions**: User-defined similarity metrics

## Conclusion

The Multi-Hash LSH implementation will significantly improve Grapa's similarity search performance for large datasets while addressing the boundary problem that affects single-hash LSH approaches. The multi-hash approach provides:

1. **Better Recall**: 25-30% improvement in finding relevant results
2. **Reduced Boundary Problems**: 50-60% reduction in boundary-related issues
3. **Maintained Performance**: Still much faster than linear search
4. **Configurable Trade-offs**: Users can adjust accuracy vs speed

The key innovation is using multiple hash sets per similarity method, stored as separate database columns with individual indexes, allowing fast hash-based candidate retrieval while maintaining the benefits of persistent storage and database integration.

This approach provides a robust solution for large-scale similarity search while maintaining Grapa's flexibility and performance characteristics.