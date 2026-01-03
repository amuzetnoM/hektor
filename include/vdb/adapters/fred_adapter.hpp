#pragma once
// ============================================================================
// VectorDB - FRED API Adapter
// Federal Reserve Economic Data integration
// ============================================================================

#include "data_adapter.hpp"
#include <chrono>

namespace vdb::adapters {

struct FREDConfig {
    std::string api_key;
    std::string base_url = "https://api.stlouisfed.org/fred";
    std::chrono::seconds timeout{30};
    size_t max_observations = 10000;
};

struct FREDSeries {
    std::string id;                    // e.g., "GDP", "UNRATE"
    std::string title;
    std::string units;
    std::string frequency;             // Daily, Monthly, Quarterly, Annual
    std::string seasonal_adjustment;
    std::chrono::system_clock::time_point last_updated;
    
    struct Observation {
        std::string date;
        float value;
        std::optional<std::string> notes;
    };
    std::vector<Observation> observations;
};

class FREDAdapter : public IDataAdapter {
public:
    explicit FREDAdapter(const FREDConfig& config);
    
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
    
    [[nodiscard]] std::string name() const override { return "FRED"; }
    [[nodiscard]] std::vector<DataFormat> supported_formats() const override {
        return {DataFormat::FRED};
    }
    
    // FRED-specific methods
    [[nodiscard]] Result<FREDSeries> fetch_series(
        const std::string& series_id,
        std::optional<std::string> start_date = std::nullopt,
        std::optional<std::string> end_date = std::nullopt
    );
    
    [[nodiscard]] Result<std::vector<std::string>> search_series(
        const std::string& query,
        size_t limit = 100
    );
    
    [[nodiscard]] Result<std::vector<std::string>> get_popular_series() const;

private:
    FREDConfig config_;
    
    [[nodiscard]] Result<std::string> http_get(const std::string& url) const;
    [[nodiscard]] NormalizedData series_to_normalized(const FREDSeries& series) const;
};

} // namespace vdb::adapters
