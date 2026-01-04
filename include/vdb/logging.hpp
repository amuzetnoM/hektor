#pragma once
// ============================================================================
// VectorDB - Comprehensive Logging and Anomaly Detection System
// ============================================================================

#include "core.hpp"
#include <fstream>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>

namespace vdb::logging {

namespace fs = std::filesystem;

// ============================================================================
// Log Levels
// ============================================================================

enum class LogLevel {
    DEBUG = 0,      // Detailed debug information
    INFO = 1,       // General informational messages
    WARN = 2,       // Warning messages
    ERROR = 3,      // Error messages
    CRITICAL = 4,   // Critical failures
    ANOMALY = 5     // Anomaly detection (separate track)
};

inline const char* log_level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        case LogLevel::ANOMALY: return "ANOMALY";
        default: return "UNKNOWN";
    }
}

// ============================================================================
// Logger Configuration
// ============================================================================

struct LoggerConfig {
    LogLevel min_level = LogLevel::INFO;           // Minimum log level to record
    bool log_to_console = true;                     // Output to stderr
    bool log_to_file = true;                        // Output to log file
    std::string log_file_path = "vectordb.log";     // Main log file
    std::string anomaly_log_path = "anomalies.log"; // Separate anomaly log
    size_t max_file_size = 10 * 1024 * 1024;       // 10MB before rotation
    size_t max_backup_files = 5;                    // Number of backup files
    bool include_timestamp = true;                  // Add timestamps to logs
    bool include_thread_id = true;                  // Add thread IDs
    bool include_source_location = true;            // Add file:line info
};

// ============================================================================
// Anomaly Types for Detection
// ============================================================================

enum class AnomalyType {
    UNKNOWN_FORMAT,          // Unrecognized data format
    PARSE_ERROR,             // Parsing failure
    DATA_CORRUPTION,         // Corrupted data detected
    UNEXPECTED_SIZE,         // Unusual data size
    INVALID_ENCODING,        // Character encoding issues
    SQL_INJECTION_ATTEMPT,   // Potential SQL injection
    MEMORY_ANOMALY,          // Unusual memory usage
    PERFORMANCE_DEGRADATION, // Unexpectedly slow operation
    MISSING_DATA,            // Expected data not found
    DUPLICATE_DATA,          // Unexpected duplicates
    SCHEMA_MISMATCH,         // Schema doesn't match expected
    CONNECTION_FAILURE,      // Network/DB connection issues
    SECURITY_VIOLATION,      // Security policy violation
    RESOURCE_EXHAUSTION,     // Resource limits exceeded
    CUSTOM                   // Application-defined anomaly
};

inline const char* anomaly_type_to_string(AnomalyType type) {
    switch (type) {
        case AnomalyType::UNKNOWN_FORMAT: return "UNKNOWN_FORMAT";
        case AnomalyType::PARSE_ERROR: return "PARSE_ERROR";
        case AnomalyType::DATA_CORRUPTION: return "DATA_CORRUPTION";
        case AnomalyType::UNEXPECTED_SIZE: return "UNEXPECTED_SIZE";
        case AnomalyType::INVALID_ENCODING: return "INVALID_ENCODING";
        case AnomalyType::SQL_INJECTION_ATTEMPT: return "SQL_INJECTION_ATTEMPT";
        case AnomalyType::MEMORY_ANOMALY: return "MEMORY_ANOMALY";
        case AnomalyType::PERFORMANCE_DEGRADATION: return "PERFORMANCE_DEGRADATION";
        case AnomalyType::MISSING_DATA: return "MISSING_DATA";
        case AnomalyType::DUPLICATE_DATA: return "DUPLICATE_DATA";
        case AnomalyType::SCHEMA_MISMATCH: return "SCHEMA_MISMATCH";
        case AnomalyType::CONNECTION_FAILURE: return "CONNECTION_FAILURE";
        case AnomalyType::SECURITY_VIOLATION: return "SECURITY_VIOLATION";
        case AnomalyType::RESOURCE_EXHAUSTION: return "RESOURCE_EXHAUSTION";
        case AnomalyType::CUSTOM: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

// ============================================================================
// Logger Class (Thread-Safe Singleton)
// ============================================================================

class Logger {
public:
    // Get singleton instance
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    // Initialize logger with configuration
    void initialize(const LoggerConfig& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        
        // Open log files if file logging is enabled
        if (config_.log_to_file) {
            log_file_.open(config_.log_file_path, std::ios::app);
            anomaly_file_.open(config_.anomaly_log_path, std::ios::app);
        }
        
        initialized_ = true;
    }

    // Log a message at specified level
    void log(LogLevel level, std::string_view message, 
             std::string_view file = "", int line = 0) {
        if (!initialized_) {
            initialize(LoggerConfig{});
        }
        
        if (level < config_.min_level) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string formatted = format_message(level, message, file, line);
        
        // Console output
        if (config_.log_to_console) {
            std::cerr << formatted << std::endl;
        }
        
        // File output
        if (config_.log_to_file && log_file_.is_open()) {
            log_file_ << formatted << std::endl;
            log_file_.flush();
            
            // Check for rotation
            check_and_rotate_log();
        }
    }

    // Log an anomaly with specific type
    void log_anomaly(AnomalyType type, std::string_view message,
                     std::string_view file = "", int line = 0) {
        if (!initialized_) {
            initialize(LoggerConfig{});
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::stringstream ss;
        ss << "[" << anomaly_type_to_string(type) << "] " << message;
        
        std::string formatted = format_message(LogLevel::ANOMALY, ss.str(), file, line);
        
        // Always log anomalies to console
        std::cerr << "\033[1;31m" << formatted << "\033[0m" << std::endl; // Red color
        
        // Log to both main log and anomaly log
        if (config_.log_to_file) {
            if (log_file_.is_open()) {
                log_file_ << formatted << std::endl;
                log_file_.flush();
            }
            
            if (anomaly_file_.is_open()) {
                anomaly_file_ << formatted << std::endl;
                anomaly_file_.flush();
                check_and_rotate_anomaly_log();
            }
        }
    }

    // Convenience methods
    void debug(std::string_view msg, std::string_view file = "", int line = 0) {
        log(LogLevel::DEBUG, msg, file, line);
    }
    
    void info(std::string_view msg, std::string_view file = "", int line = 0) {
        log(LogLevel::INFO, msg, file, line);
    }
    
    void warn(std::string_view msg, std::string_view file = "", int line = 0) {
        log(LogLevel::WARN, msg, file, line);
    }
    
    void error(std::string_view msg, std::string_view file = "", int line = 0) {
        log(LogLevel::ERROR, msg, file, line);
    }
    
    void critical(std::string_view msg, std::string_view file = "", int line = 0) {
        log(LogLevel::CRITICAL, msg, file, line);
    }

    // Get current configuration
    const LoggerConfig& config() const { return config_; }

    // Flush all logs
    void flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (log_file_.is_open()) log_file_.flush();
        if (anomaly_file_.is_open()) anomaly_file_.flush();
    }

    ~Logger() {
        flush();
        if (log_file_.is_open()) log_file_.close();
        if (anomaly_file_.is_open()) anomaly_file_.close();
    }

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string format_message(LogLevel level, std::string_view message,
                               std::string_view file, int line) {
        std::stringstream ss;
        
        // Timestamp
        if (config_.include_timestamp) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            ss << "." << std::setfill('0') << std::setw(3) << ms.count() << " ";
        }
        
        // Thread ID
        if (config_.include_thread_id) {
            ss << "[" << std::this_thread::get_id() << "] ";
        }
        
        // Log level
        ss << "[" << log_level_to_string(level) << "] ";
        
        // Source location
        if (config_.include_source_location && !file.empty()) {
            ss << file << ":" << line << " - ";
        }
        
        // Message
        ss << message;
        
        return ss.str();
    }

    void check_and_rotate_log() {
        if (!log_file_.is_open()) return;
        
        log_file_.seekp(0, std::ios::end);
        size_t size = log_file_.tellp();
        
        if (size >= config_.max_file_size) {
            rotate_file(config_.log_file_path, log_file_);
        }
    }

    void check_and_rotate_anomaly_log() {
        if (!anomaly_file_.is_open()) return;
        
        anomaly_file_.seekp(0, std::ios::end);
        size_t size = anomaly_file_.tellp();
        
        if (size >= config_.max_file_size) {
            rotate_file(config_.anomaly_log_path, anomaly_file_);
        }
    }

    void rotate_file(const std::string& base_path, std::ofstream& file) {
        file.close();
        
        // Rotate existing backups
        for (int i = config_.max_backup_files - 1; i >= 1; --i) {
            std::string old_name = base_path + "." + std::to_string(i);
            std::string new_name = base_path + "." + std::to_string(i + 1);
            
            if (fs::exists(old_name)) {
                if (i == config_.max_backup_files - 1) {
                    fs::remove(new_name);
                }
                fs::rename(old_name, new_name);
            }
        }
        
        // Rename current log to .1
        if (fs::exists(base_path)) {
            fs::rename(base_path, base_path + ".1");
        }
        
        // Open new log file
        file.open(base_path, std::ios::app);
    }

    LoggerConfig config_;
    std::ofstream log_file_;
    std::ofstream anomaly_file_;
    std::mutex mutex_;
    bool initialized_ = false;
};

// ============================================================================
// Convenience Macros
// ============================================================================

#define LOG_DEBUG(msg) vdb::logging::Logger::instance().debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) vdb::logging::Logger::instance().info(msg, __FILE__, __LINE__)
#define LOG_WARN(msg) vdb::logging::Logger::instance().warn(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) vdb::logging::Logger::instance().error(msg, __FILE__, __LINE__)
#define LOG_CRITICAL(msg) vdb::logging::Logger::instance().critical(msg, __FILE__, __LINE__)
#define LOG_ANOMALY(type, msg) vdb::logging::Logger::instance().log_anomaly(type, msg, __FILE__, __LINE__)

} // namespace vdb::logging
