#pragma once
// ============================================================================
// VectorDB - XML Data Adapter
// Parse XML documents and extract structured data
// ============================================================================

#include "data_adapter.hpp"
#include <string>
#include <vector>

namespace vdb::adapters {

// ============================================================================
// XML Configuration
// ============================================================================

struct XMLConfig {
    bool extract_text_content = true;    // Extract all text nodes
    bool extract_attributes = true;      // Extract element attributes
    bool preserve_hierarchy = true;      // Maintain XML structure in metadata
    std::string root_element = "";       // Root element to start extraction (empty = document root)
    std::vector<std::string> text_elements; // Specific elements to extract text from (empty = all)
    bool remove_cdata = false;           // Strip CDATA sections
    bool flatten_namespaces = true;      // Remove namespace prefixes
};

// ============================================================================
// XML Adapter
// ============================================================================

class XMLAdapter : public IDataAdapter {
public:
    explicit XMLAdapter(const XMLConfig& config = {});
    ~XMLAdapter() override = default;

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
    
    [[nodiscard]] std::string name() const override { return "XML"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override;
    [[nodiscard]] bool supports_write() const override { return true; }
    
    /// Write NormalizedData to XML file
    [[nodiscard]] Result<void> write(
        const NormalizedData& data,
        const fs::path& path
    ) override;

private:
    XMLConfig config_;
    
    // Parse XML string and extract data
    Result<NormalizedData> parse_xml_string(
        std::string_view xml_content,
        const ChunkConfig& chunk_config,
        std::string_view source_path
    );
};

} // namespace vdb::adapters
