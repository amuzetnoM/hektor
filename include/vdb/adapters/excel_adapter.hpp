#pragma once
// ============================================================================
// VectorDB - Excel Data Adapter
// Multi-sheet reading/writing with formula support
// ============================================================================

#include "data_adapter.hpp"

#ifdef HAVE_XLSXWRITER
#include <xlsxwriter.h>
#endif

namespace vdb::adapters {

struct ExcelConfig {
    std::vector<std::string> sheet_names;  // Empty = all sheets
    bool has_header = true;
    bool skip_empty_rows = true;
    bool evaluate_formulas = false;         // Evaluate formulas (slow)
    bool extract_charts = false;            // Extract chart metadata
    int start_row = 0;
    int start_col = 0;
};

class ExcelAdapter : public IDataAdapter {
public:
    explicit ExcelAdapter(const ExcelConfig& config = {});
    
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
    
    // Write support
    [[nodiscard]] Result<void> write(
        const fs::path& path,
        const NormalizedData& data
    );
    
    [[nodiscard]] std::string name() const override { return "Excel"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override {
        return {DataFormat::Excel};
    }
    
private:
    ExcelConfig config_;
    
    struct CellValue {
        enum class Type { Empty, String, Number, Boolean, Error };
        Type type = Type::Empty;
        std::string string_value;
        double number_value = 0.0;
        bool bool_value = false;
    };
    
    struct Sheet {
        std::string name;
        std::vector<std::vector<CellValue>> rows;
        std::vector<std::string> headers;
    };
    
#ifdef HAVE_XLSXWRITER
    [[nodiscard]] Result<std::vector<Sheet>> parse_excel_file(const fs::path& path);
    [[nodiscard]] Result<void> write_excel_file(
        const fs::path& path,
        const std::vector<Sheet>& sheets
    );
#endif
};

} // namespace vdb::adapters
