// ============================================================================
// VectorDB - Concurrent Stress Test
// Tests for race conditions and concurrent access patterns that could cause segfaults
// ============================================================================

#include <gtest/gtest.h>
#include "vdb/index.hpp"
#include "vdb/storage.hpp"
#include <thread>
#include <vector>
#include <random>
#include <atomic>

namespace vdb {
namespace test {

class ConcurrentStressTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir_ = fs::temp_directory_path() / "vdb_concurrent_test";
        fs::remove_all(test_dir_);
        fs::create_directories(test_dir_);
        
        // Initialize random vectors
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        
        for (int i = 0; i < 1000; ++i) {
            std::vector<float> vec(128);
            for (auto& v : vec) {
                v = dist(rng);
            }
            test_vectors_.emplace_back(vec);
        }
    }
    
    void TearDown() override {
        fs::remove_all(test_dir_);
    }
    
    fs::path test_dir_;
    std::vector<Vector> test_vectors_;
};

// Test concurrent HNSW insertions and searches
TEST_F(ConcurrentStressTest, ConcurrentHNSWInsertSearch) {
    HnswConfig config;
    config.dimension = 128;
    config.max_elements = 10000;
    config.M = 16;
    config.ef_construction = 200;
    config.ef_search = 50;
    
    HnswIndex index(config);
    
    std::atomic<bool> stop{false};
    std::atomic<int> insert_count{0};
    std::atomic<int> search_count{0};
    std::atomic<int> errors{0};
    
    // Writer threads - insert vectors
    auto writer = [&](int thread_id) {
        for (int i = 0; i < 100; ++i) {
            VectorId id = thread_id * 100 + i + 1;
            size_t vec_idx = (id - 1) % test_vectors_.size();
            
            auto result = index.add(id, test_vectors_[vec_idx]);
            if (result.has_value()) {
                insert_count++;
            } else {
                errors++;
            }
            
            // Small delay to allow interleaving
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    };
    
    // Reader threads - search for nearest neighbors
    auto reader = [&]() {
        while (!stop) {
            size_t vec_idx = search_count % test_vectors_.size();
            
            try {
                auto results = index.search(test_vectors_[vec_idx], 10);
                search_count++;
                
                // Verify results are valid
                for (const auto& res : results) {
                    if (res.id == 0) {
                        errors++;
                    }
                }
            } catch (...) {
                errors++;
            }
            
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    };
    
    // Start threads
    std::vector<std::thread> threads;
    
    // 4 writer threads
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(writer, i);
    }
    
    // 4 reader threads
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(reader);
    }
    
    // Wait for writers to finish
    for (int i = 0; i < 4; ++i) {
        threads[i].join();
    }
    
    // Stop readers
    stop = true;
    for (size_t i = 4; i < threads.size(); ++i) {
        threads[i].join();
    }
    
    // Verify no errors occurred
    EXPECT_EQ(errors.load(), 0) << "Detected " << errors << " errors during concurrent access";
    EXPECT_GT(insert_count.load(), 0);
    EXPECT_GT(search_count.load(), 0);
    
    std::cout << "Inserts: " << insert_count << ", Searches: " << search_count << std::endl;
}

// Test concurrent VectorStore operations with potential resize
TEST_F(ConcurrentStressTest, ConcurrentVectorStoreResize) {
    VectorStoreConfig config;
    config.path = test_dir_;
    config.dimension = 128;
    config.initial_capacity = 50;  // Small capacity to force resizes
    
    VectorStore store(config);
    ASSERT_TRUE(store.init().has_value());
    
    std::atomic<int> add_count{0};
    std::atomic<int> get_count{0};
    std::atomic<int> errors{0};
    
    // Writer threads - add vectors (will trigger resize)
    auto writer = [&](int thread_id) {
        for (int i = 0; i < 100; ++i) {
            VectorId id = thread_id * 1000 + i + 1;  // FIXED: Non-overlapping IDs
            size_t vec_idx = (id - 1) % test_vectors_.size();
            
            auto result = store.add(id, test_vectors_[vec_idx]);
            if (result.has_value()) {
                add_count++;
            } else {
                // Print error for debugging
                if (errors.load() < 5) {
                    std::cerr << "Add error for ID " << id << ": " << result.error().message << std::endl;
                }
                errors++;
            }
            
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    };
    
    // Reader threads - get vectors and immediately copy
    auto reader = [&]() {
        for (int i = 0; i < 200; ++i) {
            VectorId id = (i % 3000) + 1;  // FIXED: Match writer range
            
            auto result = store.get(id);
            if (result.has_value()) {
                // CRITICAL: Immediately copy the data - don't hold VectorView
                Vector copy(std::vector<Scalar>(result->begin(), result->end()));
                get_count++;
                
                // Verify the vector is valid
                if (copy.dim() != 128) {
                    errors++;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
    };
    
    // Start threads
    std::vector<std::thread> threads;
    
    // 3 writer threads
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(writer, i);
    }
    
    // 3 reader threads
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(reader);
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Verify no errors occurred
    EXPECT_EQ(errors.load(), 0) << "Detected " << errors << " errors during concurrent storage access";
    EXPECT_GT(add_count.load(), 0);
    
    std::cout << "Adds: " << add_count << ", Gets: " << get_count << std::endl;
}

} // namespace test
} // namespace vdb
