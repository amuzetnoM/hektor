#pragma once
// ============================================================================
// VectorDB - Apache Parquet Data Adapter
// Parse columnar Parquet files for vector ingestion
// ============================================================================

#include "data_adapter.hpp"
#include <string>
#include <vector>

namespace vdb::adapters {

// ============================================================================
// Parquet Configuration
// ============================================================================

struct ParquetConfig {
    bool row_based_chunks = true;        // True: one chunk per row, False: one chunk per column
    std::vector<std::string> columns;    // Specific columns to read (empty = all)
    size_t batch_size = 1000;            // Number of rows to read at once
    bool extract_schema = true;          // Include schema info in metadata
    bool include_column_names = true;    // Prefix column name in content
};

// ============================================================================
// Parquet Adapter
// ============================================================================

class ParquetAdapter : public IDataAdapter {
public:
    explicit ParquetAdapter(const ParquetConfig& config = {});
    ~ParquetAdapter() override = default;

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
    
    [[nodiscard]] std::string name() const override { return "Parquet"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override;

private:
    ParquetConfig config_;
    
    // Parse Parquet file
    Result<NormalizedData> parse_parquet_file(
        const fs::path& path,
        const ChunkConfig& chunk_config
    );
};

} // namespace vdb::adapters
