#pragma once
// ============================================================================
// VectorDB - PDF Data Adapter
// Text extraction, table detection, and metadata parsing
// ============================================================================

#include "data_adapter.hpp"

#ifdef HAVE_POPPLER
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#endif

namespace vdb::adapters {

struct PDFConfig {
    bool extract_tables = true;
    bool extract_images = false;
    bool preserve_layout = true;
    int start_page = 0;          // 0 = first page
    int end_page = -1;            // -1 = all pages
    bool extract_toc = true;      // Table of contents
};

class PDFAdapter : public IDataAdapter {
public:
    explicit PDFAdapter(const PDFConfig& config = {});
    
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
    
    [[nodiscard]] std::string name() const override { return "PDF"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override {
        return {DataFormat::PDF};
    }
    [[nodiscard]] bool supports_write() const override { return true; }
    
    /// Write NormalizedData to PDF file
    [[nodiscard]] Result<void> write(
        const NormalizedData& data,
        const fs::path& path
    ) override;
    
private:
    PDFConfig config_;
    
#ifdef HAVE_POPPLER
    [[nodiscard]] Result<NormalizedData> parse_pdf_document(
        poppler::document* doc,
        const ChunkConfig& config,
        const std::string& source_path
    );
    
    [[nodiscard]] std::string extract_page_text(poppler::page* page) const;
    [[nodiscard]] std::unordered_map<std::string, std::string> extract_metadata(
        poppler::document* doc
    ) const;
#endif
};

} // namespace vdb::adapters
