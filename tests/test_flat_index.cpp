// ============================================================================
// VectorDB Tests - FlatIndex
// ============================================================================

#include <gtest/gtest.h>
#include "vdb/index.hpp"
#include <filesystem>
#include <random>
#include <thread>

namespace vdb::test
{

    class FlatIndexTest : public ::testing::Test
    {
    protected:
        static constexpr Dim DIM = 64;
        static constexpr size_t NUM_VECTORS = 100;

        void SetUp() override
        {
            // Generate random normalized vectors
            std::mt19937 gen(42);
            std::normal_distribution<float> dist(0.0f, 1.0f);

            for (size_t i = 0; i < NUM_VECTORS; ++i)
            {
                Vector v(DIM);
                float norm = 0.0f;
                for (Dim d = 0; d < DIM; ++d)
                {
                    v[d] = dist(gen);
                    norm += v[d] * v[d];
                }
                norm = std::sqrt(norm);
                for (Dim d = 0; d < DIM; ++d)
                {
                    v[d] /= norm;
                }
                vectors_.push_back(std::move(v));
            }
        }

        void TearDown() override
        {
            // Clean up any test files
            if (std::filesystem::exists(test_file_path_))
            {
                std::filesystem::remove(test_file_path_);
            }
        }

        std::string get_temp_path()
        {
            auto temp_dir = std::filesystem::temp_directory_path();
            auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
            test_file_path_ = (temp_dir / ("test_flat_index_" + std::to_string(tid) + ".vdb")).string();
            return test_file_path_;
        }

        std::vector<Vector> vectors_;
        std::string test_file_path_;
    };

    TEST_F(FlatIndexTest, Construction)
    {
        FlatIndex index(DIM);
        EXPECT_EQ(index.dimension(), DIM);
        EXPECT_EQ(index.size(), 0);
    }

    TEST_F(FlatIndexTest, AddAndSearch)
    {
        FlatIndex index(DIM);

        for (size_t i = 0; i < 10; ++i)
        {
            auto result = index.add(i + 1, vectors_[i]);
            EXPECT_TRUE(result.has_value());
        }

        EXPECT_EQ(index.size(), 10);

        // Search for exact match
        auto results = index.search(vectors_[0], 1);
        EXPECT_EQ(results.size(), 1);
        EXPECT_EQ(results[0].id, 1);
        EXPECT_NEAR(results[0].distance, 0.0f, 1e-5f);
    }

    TEST_F(FlatIndexTest, SaveAndLoad)
    {
        FlatIndex index(DIM);

        // Add some vectors
        for (size_t i = 0; i < 20; ++i)
        {
            index.add(i + 100, vectors_[i]);
        }

        std::string path = get_temp_path();

        // Save
        auto save_result = index.save(path);
        EXPECT_TRUE(save_result.has_value()) << save_result.error().message;
        EXPECT_TRUE(std::filesystem::exists(path));

        // Load
        auto load_result = FlatIndex::load(path);
        EXPECT_TRUE(load_result.has_value()) << load_result.error().message;

        FlatIndex &loaded = load_result.value();
        EXPECT_EQ(loaded.dimension(), DIM);
        EXPECT_EQ(loaded.size(), 20);

        // Verify vectors are preserved
        for (size_t i = 0; i < 20; ++i)
        {
            EXPECT_TRUE(loaded.contains(i + 100));
            auto vec_opt = loaded.get_vector(i + 100);
            EXPECT_TRUE(vec_opt.has_value());

            const Vector &original = vectors_[i];
            const Vector &loaded_vec = vec_opt.value();

            for (Dim d = 0; d < DIM; ++d)
            {
                EXPECT_NEAR(original[d], loaded_vec[d], 1e-6f);
            }
        }

        // Verify search still works
        auto results = loaded.search(vectors_[0], 1);
        EXPECT_EQ(results.size(), 1);
        EXPECT_EQ(results[0].id, 100);
    }

    TEST_F(FlatIndexTest, GetVector)
    {
        FlatIndex index(DIM);
        index.add(42, vectors_[0]);

        auto vec_opt = index.get_vector(42);
        EXPECT_TRUE(vec_opt.has_value());

        for (Dim d = 0; d < DIM; ++d)
        {
            EXPECT_NEAR(vectors_[0][d], vec_opt.value()[d], 1e-6f);
        }

        // Non-existent vector
        auto missing = index.get_vector(999);
        EXPECT_FALSE(missing.has_value());
    }

    TEST_F(FlatIndexTest, LoadInvalidFile)
    {
        std::string path = get_temp_path();

        // Create invalid file
        std::ofstream file(path, std::ios::binary);
        uint32_t garbage = 0xDEADBEEF;
        file.write(reinterpret_cast<const char *>(&garbage), sizeof(garbage));
        file.close();

        auto result = FlatIndex::load(path);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error().code, ErrorCode::IndexCorrupted);
    }

    TEST_F(FlatIndexTest, LoadNonExistentFile)
    {
        auto result = FlatIndex::load("/nonexistent/path/file.vdb");
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error().code, ErrorCode::IoError);
    }

    TEST_F(FlatIndexTest, DuplicateIdFails)
    {
        FlatIndex index(DIM);

        auto result1 = index.add(1, vectors_[0]);
        EXPECT_TRUE(result1.has_value());

        auto result2 = index.add(1, vectors_[1]);
        EXPECT_FALSE(result2.has_value());
        EXPECT_EQ(result2.error().code, ErrorCode::InvalidVectorId);
    }

    TEST_F(FlatIndexTest, DimensionMismatchFails)
    {
        FlatIndex index(DIM);

        Vector wrong_dim(DIM + 10);
        for (size_t i = 0; i < DIM + 10; ++i)
        {
            wrong_dim[i] = 1.0f;
        }

        auto result = index.add(1, wrong_dim);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error().code, ErrorCode::InvalidDimension);
    }

} // namespace vdb::test
