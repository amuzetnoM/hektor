#pragma once
// ============================================================================
// VectorDB - CSV Data Adapter
// ============================================================================

#include "data_adapter.hpp"
#include <sstream>

namespace vdb::adapters {

struct CSVConfig {
    char delimiter = ',';
    char quote = '"';
    bool has_header = true;
    bool skip_empty_rows = true;
    std::vector<std::string> column_names;  // Override header
};

class CSVAdapter : public IDataAdapter {
public:
    explicit CSVAdapter(const CSVConfig& config = {});
    
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
    
    [[nodiscard]] std::string name() const override { return "CSV"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override {
        return {DataFormat::CSV};
    }
    
private:
    CSVConfig config_;
    
    struct ParsedRow {
        std::vector<std::string> fields;
        size_t row_number;
    };
    
    [[nodiscard]] std::vector<ParsedRow> parse_csv_content(std::string_view content) const;
    [[nodiscard]] std::vector<std::string> parse_csv_line(std::string_view line) const;
};

} // namespace vdb::adapters
