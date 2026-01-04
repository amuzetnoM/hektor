#pragma once
// ============================================================================
// VectorDB - pgvector PostgreSQL Adapter
// Bidirectional sync with PostgreSQL pgvector extension
// ============================================================================

#include "data_adapter.hpp"
#include <string>
#include <vector>

namespace vdb::adapters {

// ============================================================================
// pgvector Configuration
// ============================================================================

struct PgvectorConfig {
    std::string host = "localhost";
    int port = 5432;
    std::string database;
    std::string user;
    std::string password;
    std::string table;                       // Table with vector column
    std::string vector_column = "embedding"; // Name of vector column
    std::string content_column = "content";  // Name of text content column
    std::vector<std::string> metadata_columns; // Additional columns to extract
    size_t batch_size = 1000;                // Rows to fetch at once
    bool create_table_if_missing = false;    // Auto-create table
};

// ============================================================================
// pgvector Adapter
// ============================================================================

class PgvectorAdapter : public IDataAdapter {
public:
    explicit PgvectorAdapter(const PgvectorConfig& config);
    ~PgvectorAdapter() override = default;

    // IDataAdapter interface
    [[nodiscard]] bool can_handle(const fs::path& path) const override;
    [[nodiscard]] bool can_handle(std::string_view content) const override;
    
    [[nodiscard]] Result<NormalizedData> parse(
        const fs::path& path,
        const ChunkConfig& config = {}
    ) override;
    
    [[nodiscard]] Result<NormalizedData> parse_content(
        std::string_view content,
        const ChunkConfig& config = {},
        std::string_view source_hint = ""
    ) override;
    
    [[nodiscard]] Result<void> sanitize(NormalizedData& data) override;
    
    [[nodiscard]] std::string name() const override { return "pgvector"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override;
    
    // Additional methods for bidirectional sync
    
    /// Connect to PostgreSQL database
    [[nodiscard]] Result<void> connect();
    
    /// Disconnect from database
    void disconnect();
    
    /// Check if pgvector extension is installed
    [[nodiscard]] Result<bool> is_pgvector_available();
    
    /// Create table with vector column
    [[nodiscard]] Result<void> create_table(size_t vector_dimension);
    
    /// Insert vectors into database
    [[nodiscard]] Result<size_t> insert_vectors(
        const std::vector<std::vector<float>>& vectors,
        const std::vector<std::string>& contents,
        const std::vector<std::unordered_map<std::string, std::string>>& metadata
    );
    
    /// Query similar vectors
    [[nodiscard]] Result<std::vector<DataChunk>> query_similar(
        const std::vector<float>& query_vector,
        size_t k = 10,
        float distance_threshold = -1.0f
    );

private:
    PgvectorConfig config_;
    void* connection_ = nullptr; // PGconn* when libpq is available
    
    // Parse data from pgvector database
    Result<NormalizedData> parse_pgvector_db(const ChunkConfig& chunk_config);
};

} // namespace vdb::adapters
