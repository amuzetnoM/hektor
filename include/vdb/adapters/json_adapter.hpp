#pragma once
// ============================================================================
// VectorDB - JSON Data Adapter
// ============================================================================

#include "data_adapter.hpp"
#include <nlohmann/json.hpp>

namespace vdb::adapters {

using json = nlohmann::json;

enum class JSONStructure {
    Object,         // Single JSON object
    Array,          // Array of objects
    Lines,          // JSONL - one object per line
    Nested,         // Deeply nested structure
};

class JSONAdapter : public IDataAdapter {
public:
    JSONAdapter() = default;
    
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
    
    [[nodiscard]] std::string name() const override { return "JSON"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override {
        return {DataFormat::JSON, DataFormat::API_JSON};
    }
    
private:
    [[nodiscard]] JSONStructure detect_structure(const json& j) const;
    [[nodiscard]] std::vector<DataChunk> process_json(const json& j) const;
    [[nodiscard]] std::string flatten_json_to_text(const json& j) const;
    void extract_metadata(const json& j, std::unordered_map<std::string, std::string>& meta) const;
};

} // namespace vdb::adapters
