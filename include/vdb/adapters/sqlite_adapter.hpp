#pragma once
// ============================================================================
// VectorDB - SQLite Database Adapter
// Parse SQLite database files and extract data
// ============================================================================

#include "data_adapter.hpp"
#include <string>
#include <vector>

namespace vdb::adapters {

// ============================================================================
// SQLite Configuration
// ============================================================================

struct SQLiteConfig {
    std::vector<std::string> tables;     // Specific tables to query (empty = all)
    std::string query;                   // Custom SQL query (overrides tables)
    bool include_schema = true;          // Include table schema in metadata
    bool row_based_chunks = true;        // True: one chunk per row, False: one chunk per table
    size_t batch_size = 1000;            // Number of rows to fetch at once
    bool extract_column_names = true;    // Include column names in content
};

// ============================================================================
// SQLite Adapter
// ============================================================================

class SQLiteAdapter : public IDataAdapter {
public:
    explicit SQLiteAdapter(const SQLiteConfig& config = {});
    ~SQLiteAdapter() override = default;

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
    
    [[nodiscard]] std::string name() const override { return "SQLite"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override;

private:
    SQLiteConfig config_;
    
    // Parse SQLite database
    Result<NormalizedData> parse_sqlite_db(
        const fs::path& path,
        const ChunkConfig& chunk_config
    );
    
    // Helper to extract table names
    Result<std::vector<std::string>> get_table_names(void* db);
    
    // Helper to extract data from a table
    Result<std::vector<DataChunk>> extract_table_data(
        void* db,
        const std::string& table_name,
        size_t& chunk_offset
    );
};

} // namespace vdb::adapters
