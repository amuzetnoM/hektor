// ============================================================================
// VectorDB - FRED API Adapter Implementation
// ============================================================================

#include "vdb/adapters/fred_adapter.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>

namespace vdb::adapters {

using json = nlohmann::json;

FREDAdapter::FREDAdapter(const FREDConfig& config)
    : config_(config) {
    if (config_.api_key.empty()) {
        throw std::runtime_error("FRED API key is required");
    }
}

bool FREDAdapter::can_handle(const fs::path& path) const {
    // FRED adapter doesn't handle files directly
    return false;
}

bool FREDAdapter::can_handle(std::string_view content) const {
    // Check if content looks like FRED JSON response
    if (content.empty()) return false;
    
    try {
        json j = json::parse(content);
        // FRED responses typically have "observations" or "seriess" fields
        return j.contains("observations") || j.contains("seriess");
    } catch (...) {
        return false;
    }
}

Result<NormalizedData> FREDAdapter::parse(
    const fs::path& path,
    const ChunkConfig& config
) {
    return std::unexpected(Error{ErrorCode::InvalidData, 
        "FRED adapter requires API calls, not file paths. Use fetch_series() instead."});
}

Result<NormalizedData> FREDAdapter::parse_content(
    std::string_view content,
    const ChunkConfig& config,
    std::string_view source_hint
) {
    try {
        json j = json::parse(content);
        
        // Parse FRED JSON response
        FREDSeries series;
        series.id = std::string(source_hint);
        
        if (j.contains("observations")) {
            for (const auto& obs : j["observations"]) {
                FREDSeries::Observation observation;
                observation.date = obs.value("date", "");
                
                std::string value_str = obs.value("value", ".");
                if (value_str != ".") {
                    observation.value = std::stof(value_str);
                } else {
                    observation.value = std::numeric_limits<float>::quiet_NaN();
                }
                
                series.observations.push_back(observation);
            }
        }
        
        return series_to_normalized(series);
        
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::InvalidData, 
            std::string("FRED parse error: ") + e.what()});
    }
}

Result<void> FREDAdapter::sanitize(NormalizedData& data) {
    // FRED data is already clean from API
    data.sanitized = true;
    return {};
}

Result<FREDSeries> FREDAdapter::fetch_series(
    const std::string& series_id,
    std::optional<std::string> start_date,
    std::optional<std::string> end_date
) {
    std::string url = config_.base_url + "/series/observations";
    url += "?series_id=" + series_id;
    url += "&api_key=" + config_.api_key;
    url += "&file_type=json";
    
    if (start_date) {
        url += "&observation_start=" + *start_date;
    }
    if (end_date) {
        url += "&observation_end=" + *end_date;
    }
    
    auto response = http_get(url);
    if (!response) {
        return std::unexpected(response.error());
    }
    
    try {
        json j = json::parse(*response);
        
        FREDSeries series;
        series.id = series_id;
        
        // Parse series metadata from a separate API call
        std::string meta_url = config_.base_url + "/series";
        meta_url += "?series_id=" + series_id;
        meta_url += "&api_key=" + config_.api_key;
        meta_url += "&file_type=json";
        
        auto meta_response = http_get(meta_url);
        if (meta_response) {
            try {
                json meta_j = json::parse(*meta_response);
                if (meta_j.contains("seriess") && !meta_j["seriess"].empty()) {
                    const auto& s = meta_j["seriess"][0];
                    series.title = s.value("title", "");
                    series.units = s.value("units", "");
                    series.frequency = s.value("frequency", "");
                    series.seasonal_adjustment = s.value("seasonal_adjustment", "");
                }
            } catch (...) {
                // Metadata is optional
            }
        }
        
        // Parse observations
        if (j.contains("observations")) {
            for (const auto& obs : j["observations"]) {
                FREDSeries::Observation observation;
                observation.date = obs.value("date", "");
                
                std::string value_str = obs.value("value", ".");
                if (value_str != ".") {
                    try {
                        observation.value = std::stof(value_str);
                    } catch (...) {
                        observation.value = std::numeric_limits<float>::quiet_NaN();
                    }
                } else {
                    observation.value = std::numeric_limits<float>::quiet_NaN();
                }
                
                series.observations.push_back(observation);
            }
        }
        
        return series;
        
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::InvalidData, 
            std::string("FRED parse error: ") + e.what()});
    }
}

Result<std::vector<std::string>> FREDAdapter::search_series(
    const std::string& query,
    size_t limit
) {
    std::string url = config_.base_url + "/series/search";
    url += "?search_text=" + query;
    url += "&api_key=" + config_.api_key;
    url += "&file_type=json";
    url += "&limit=" + std::to_string(limit);
    
    auto response = http_get(url);
    if (!response) {
        return std::unexpected(response.error());
    }
    
    try {
        json j = json::parse(*response);
        std::vector<std::string> series_ids;
        
        if (j.contains("seriess")) {
            for (const auto& series : j["seriess"]) {
                series_ids.push_back(series.value("id", ""));
            }
        }
        
        return series_ids;
        
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::InvalidData, 
            std::string("FRED search error: ") + e.what()});
    }
}

Result<std::vector<std::string>> FREDAdapter::get_popular_series() const {
    // Popular economic indicators
    return std::vector<std::string>{
        "GDP",          // Gross Domestic Product
        "UNRATE",       // Unemployment Rate
        "CPIAUCSL",     // Consumer Price Index
        "FEDFUNDS",     // Federal Funds Rate
        "DGS10",        // 10-Year Treasury Rate
        "DEXUSEU",      // USD/EUR Exchange Rate
        "SP500",        // S&P 500
        "DCOILWTICO",   // WTI Oil Price
        "GOLDAMGBD228NLBM", // Gold Price
        "M2SL"          // M2 Money Supply
    };
}

Result<std::string> FREDAdapter::http_get(const std::string& url) const {
    // TODO: Implement actual HTTP client
    // For now, return error suggesting external library
    return std::unexpected(Error{ErrorCode::NotImplemented, 
        "HTTP client not implemented. Consider using libcurl or similar. URL: " + url});
    
    // Future implementation with libcurl:
    // CURL* curl = curl_easy_init();
    // if (curl) {
    //     std::string response;
    //     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    //     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    //     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    //     CURLcode res = curl_easy_perform(curl);
    //     curl_easy_cleanup(curl);
    //     if (res == CURLE_OK) return response;
    // }
}

NormalizedData FREDAdapter::series_to_normalized(const FREDSeries& series) const {
    NormalizedData result;
    result.format = DataFormat::FRED;
    result.source_path = "fred://" + series.id;
    
    result.global_metadata["series_id"] = series.id;
    result.global_metadata["title"] = series.title;
    result.global_metadata["units"] = series.units;
    result.global_metadata["frequency"] = series.frequency;
    result.global_metadata["seasonal_adjustment"] = series.seasonal_adjustment;
    result.global_metadata["num_observations"] = std::to_string(series.observations.size());
    
    // Create chunks from observations
    std::ostringstream content;
    std::vector<float> numerical_features;
    
    for (const auto& obs : series.observations) {
        content << obs.date << ": " << obs.value << "\n";
        if (!std::isnan(obs.value)) {
            numerical_features.push_back(obs.value);
        }
    }
    
    DataChunk chunk;
    chunk.content = content.str();
    chunk.metadata = result.global_metadata;
    chunk.numerical_features = numerical_features;
    chunk.chunk_index = 0;
    chunk.total_chunks = 1;
    chunk.source = "FRED:" + series.id;
    
    result.chunks.push_back(std::move(chunk));
    result.confidence = 1.0f;
    result.sanitized = true;
    
    return result;
}

} // namespace vdb::adapters
