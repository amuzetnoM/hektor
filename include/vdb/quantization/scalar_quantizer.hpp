#pragma once
// ============================================================================
// VectorDB - Scalar Quantization (simpler alternative to PQ)
// ============================================================================

#include "../core.hpp"
#include <vector>
#include <span>

namespace vdb {
namespace quantization {

struct ScalarQuantizerConfig {
    Dim dimension = UNIFIED_DIM;
    bool per_dimension = true;  // Per-dimension vs global min/max
};

class ScalarQuantizer {
public:
    explicit ScalarQuantizer(const ScalarQuantizerConfig& config = {});
    
    // Training
    [[nodiscard]] Result<void> train(std::span<const Vector> training_data);
    [[nodiscard]] bool is_trained() const { return trained_; }
    
    // Encoding/Decoding
    [[nodiscard]] Result<std::vector<uint8_t>> encode(VectorView vector) const;
    [[nodiscard]] Result<Vector> decode(std::span<const uint8_t> codes) const;
    
    // Distance computation
    [[nodiscard]] Distance compute_distance(VectorView query,
        std::span<const uint8_t> codes) const;
    
    // Stats
    [[nodiscard]] size_t code_size() const { return config_.dimension; }
    [[nodiscard]] float compression_ratio() const {
        return static_cast<float>(config_.dimension * sizeof(Scalar)) / code_size();
    }
    
    // Persistence
    [[nodiscard]] Result<void> save(std::string_view path) const;
    [[nodiscard]] static Result<ScalarQuantizer> load(std::string_view path);

private:
    ScalarQuantizerConfig config_;
    bool trained_;
    std::vector<Scalar> min_values_;
    std::vector<Scalar> max_values_;
    std::vector<Scalar> scales_;
    std::vector<Scalar> offsets_;
};

}} // namespace vdb::quantization
