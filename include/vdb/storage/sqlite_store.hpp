#pragma once

#include "vdb/core.hpp"
#include <sqlite3.h>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <chrono>

namespace vdb {
namespace storage {

// Configuration for SQLite store
struct SqliteConfig {
    std::string db_path = ":memory:";  // Default to in-memory
    bool enable_cache = true;
    size_t cache_size_mb = 100;
    size_t max_cache_entries = 10000;
    std::chrono::seconds cache_ttl{3600};  // 1 hour default
    bool enable_wal = true;  // Write-Ahead Logging for better concurrency
    int busy_timeout_ms = 5000;
};

// Cached query result
struct CachedResult {
    std::string data;
    std::chrono::system_clock::time_point timestamp;
    size_t access_count = 0;
    
    auto is_expired(std::chrono::seconds ttl) const -> bool {
        auto now = std::chrono::system_clock::now();
        return (now - timestamp) > ttl;
    }
};

// Metadata entry for documents/vectors
struct DocumentMetadata {
    std::string id;
    std::string source;
    std::string content_type;
    std::string title;
    std::string author;
    std::string date;
    std::unordered_map<std::string, std::string> custom_fields;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
};

// SQLite store for metadata, caching, and configuration
class SqliteStore {
public:
    explicit SqliteStore(const SqliteConfig& config = SqliteConfig{});
    ~SqliteStore();

    // Disable copy
    SqliteStore(const SqliteStore&) = delete;
    auto operator=(const SqliteStore&) -> SqliteStore& = delete;

    // Enable move
    SqliteStore(SqliteStore&& other) noexcept;
    auto operator=(SqliteStore&& other) noexcept -> SqliteStore&;

    // Initialize database schema
    [[nodiscard]] auto init() -> Result<void>;

    // Close database connection
    auto close() -> void;

    // Metadata operations
    [[nodiscard]] auto store_metadata(const DocumentMetadata& metadata) -> Result<void>;
    [[nodiscard]] auto get_metadata(const std::string& id) -> Result<DocumentMetadata>;
    [[nodiscard]] auto update_metadata(const DocumentMetadata& metadata) -> Result<void>;
    [[nodiscard]] auto delete_metadata(const std::string& id) -> Result<void>;
    [[nodiscard]] auto list_metadata(size_t limit = 100, size_t offset = 0) 
        -> Result<std::vector<DocumentMetadata>>;

    // Cache operations
    [[nodiscard]] auto cache_put(const std::string& key, const std::string& value) -> Result<void>;
    [[nodiscard]] auto cache_get(const std::string& key) -> Result<std::string>;
    [[nodiscard]] auto cache_delete(const std::string& key) -> Result<void>;
    [[nodiscard]] auto cache_clear() -> Result<void>;
    [[nodiscard]] auto cache_size() -> Result<size_t>;

    // Configuration operations
    [[nodiscard]] auto config_set(const std::string& key, const std::string& value) -> Result<void>;
    [[nodiscard]] auto config_get(const std::string& key) -> Result<std::string>;
    [[nodiscard]] auto config_delete(const std::string& key) -> Result<void>;
    [[nodiscard]] auto config_list() -> Result<std::unordered_map<std::string, std::string>>;

    // Statistics
    [[nodiscard]] auto get_stats() -> Result<std::unordered_map<std::string, size_t>>;

    // Maintenance
    [[nodiscard]] auto vacuum() -> Result<void>;
    [[nodiscard]] auto evict_expired_cache() -> Result<size_t>;

private:
    auto create_tables() -> Result<void>;
    auto exec_sql(const std::string& sql) -> Result<void>;
    auto begin_transaction() -> Result<void>;
    auto commit_transaction() -> Result<void>;
    auto rollback_transaction() -> Result<void>;

    SqliteConfig config_;
    sqlite3* db_ = nullptr;
    std::mutex mutex_;
    bool initialized_ = false;
};

} // namespace storage
} // namespace vdb
