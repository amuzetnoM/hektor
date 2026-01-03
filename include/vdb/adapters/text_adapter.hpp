#pragma once
// ============================================================================
// VectorDB - Plain Text Data Adapter
// Encoding detection and smart text parsing
// ============================================================================

#include "data_adapter.hpp"

#ifdef HAVE_ICU
#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>
#endif

namespace vdb::adapters {

struct TextConfig {
    bool auto_detect_encoding = true;
    std::string default_encoding = "UTF-8";
    bool detect_language = false;
    bool detect_markdown = true;     // Detect if text is actually markdown
    bool detect_code = true;          // Detect code blocks
    bool preserve_whitespace = false;
};

class TextAdapter : public IDataAdapter {
public:
    explicit TextAdapter(const TextConfig& config = {});
    
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
    
    [[nodiscard]] std::string name() const override { return "PlainText"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override {
        return {DataFormat::PlainText, DataFormat::TXT, DataFormat::Markdown};
    }
    
private:
    TextConfig config_;
    
    [[nodiscard]] Result<std::string> detect_and_convert_encoding(
        std::string_view content
    ) const;
    
    [[nodiscard]] bool looks_like_markdown(std::string_view content) const;
    [[nodiscard]] bool looks_like_code(std::string_view content) const;
    
#ifdef HAVE_ICU
    [[nodiscard]] std::string detect_encoding_icu(std::string_view content) const;
    [[nodiscard]] Result<std::string> convert_encoding(
        std::string_view content,
        const std::string& from_encoding,
        const std::string& to_encoding = "UTF-8"
    ) const;
#endif
};

} // namespace vdb::adapters
