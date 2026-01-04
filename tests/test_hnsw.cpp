// ============================================================================
// VectorDB Tests - HNSW Index
// ============================================================================

#include <gtest/gtest.h>
#include "vdb/index.hpp"
#include <random>

namespace vdb::test {

class HNSWTest : public ::testing::Test {
protected:
    static constexpr Dim DIM = 128;
    static constexpr size_t NUM_VECTORS = 1000;
    
    void SetUp() override {
        // Generate random vectors
        std::mt19937 gen(42);
        std::normal_distribution<float> dist(0.0f, 1.0f);
        
        for (size_t i = 0; i < NUM_VECTORS; ++i) {
            Vector v(DIM);
            float norm = 0.0f;
            for (Dim d = 0; d < DIM; ++d) {
                v[d] = dist(gen);
                norm += v[d] * v[d];
            }
            norm = std::sqrt(norm);
            for (Dim d = 0; d < DIM; ++d) {
                v[d] /= norm;
            }
            vectors_.push_back(std::move(v));
        }
    }
    
    std::vector<Vector> vectors_;
};

TEST_F(HNSWTest, Construction) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = NUM_VECTORS;
    
    HnswIndex index(config);
    EXPECT_EQ(index.dimension(), DIM);
    EXPECT_EQ(index.size(), 0);
}

TEST_F(HNSWTest, AddSingleVector) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = 100;
    
    HnswIndex index(config);
    auto result = index.add(1, vectors_[0]);
    
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(index.size(), 1);
    EXPECT_TRUE(index.contains(1));
}

TEST_F(HNSWTest, AddMultipleVectors) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = NUM_VECTORS;
    
    HnswIndex index(config);
    
    for (size_t i = 0; i < 100; ++i) {
        auto result = index.add(i + 1, vectors_[i]);
        EXPECT_TRUE(result.has_value());
    }
    
    EXPECT_EQ(index.size(), 100);
}

TEST_F(HNSWTest, SearchReturnsClosest) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = NUM_VECTORS;
    
    HnswIndex index(config);
    
    // Add first 100 vectors
    for (size_t i = 0; i < 100; ++i) {
        ASSERT_TRUE(index.add(i + 1, vectors_[i]).has_value());
    }
    
    // Search for the first vector
    auto results = index.search(vectors_[0], 1);
    
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].id, 1);  // Should find itself
    EXPECT_NEAR(results[0].distance, 0.0f, 1e-5f);
}

TEST_F(HNSWTest, SearchReturnsKResults) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = NUM_VECTORS;
    
    HnswIndex index(config);
    
    for (size_t i = 0; i < 100; ++i) {
        ASSERT_TRUE(index.add(i + 1, vectors_[i]).has_value());
    }
    
    auto results = index.search(vectors_[0], 10);
    
    EXPECT_EQ(results.size(), 10);
    
    // Results should be sorted by distance
    for (size_t i = 1; i < results.size(); ++i) {
        EXPECT_LE(results[i-1].distance, results[i].distance);
    }
}

TEST_F(HNSWTest, RemoveVector) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = NUM_VECTORS;
    
    HnswIndex index(config);
    
    // Add vectors
    for (size_t i = 0; i < 50; ++i) {
        ASSERT_TRUE(index.add(i + 1, vectors_[i]).has_value());
    }
    
    EXPECT_EQ(index.size(), 50);
    EXPECT_TRUE(index.contains(10));
    
    // Remove a vector
    auto remove_result = index.remove(10);
    EXPECT_TRUE(remove_result.has_value());
    EXPECT_EQ(index.size(), 49);
    EXPECT_FALSE(index.contains(10));
    
    // Search should not return removed vector
    auto results = index.search(vectors_[9], 50);
    for (const auto& result : results) {
        EXPECT_NE(result.id, 10);
    }
}

TEST_F(HNSWTest, RemoveNonExistentVector) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = NUM_VECTORS;
    
    HnswIndex index(config);
    
    for (size_t i = 0; i < 10; ++i) {
        ASSERT_TRUE(index.add(i + 1, vectors_[i]).has_value());
    }
    
    // Try to remove non-existent vector
    auto result = index.remove(999);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::VectorNotFound);
}

TEST_F(HNSWTest, ResizeIndex) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = 50;
    
    HnswIndex index(config);
    
    // Add vectors up to capacity
    for (size_t i = 0; i < 50; ++i) {
        ASSERT_TRUE(index.add(i + 1, vectors_[i]).has_value());
    }
    
    EXPECT_EQ(index.size(), 50);
    EXPECT_EQ(index.capacity(), 50);
    
    // Resize to larger capacity
    auto resize_result = index.resize(100);
    EXPECT_TRUE(resize_result.has_value());
    EXPECT_EQ(index.capacity(), 100);
    
    // Should be able to add more vectors
    for (size_t i = 50; i < 60; ++i) {
        auto result = index.add(i + 1, vectors_[i]);
        EXPECT_TRUE(result.has_value());
    }
    
    EXPECT_EQ(index.size(), 60);
}

TEST_F(HNSWTest, ResizeToSmallerCapacityFails) {
    HnswConfig config;
    config.dimension = DIM;
    config.max_elements = 100;
    
    HnswIndex index(config);
    
    for (size_t i = 0; i < 50; ++i) {
        ASSERT_TRUE(index.add(i + 1, vectors_[i]).has_value());
    }
    
    // Try to resize to smaller than current size
    auto result = index.resize(30);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::InvalidInput);
}

} // namespace vdb::test
