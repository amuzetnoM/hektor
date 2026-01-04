# Data Ingestion Module - Implementation Plan & Documentation

## Overview
Universal data ingestion system with intelligent type detection, normalization, and support for multiple data sources including APIs like FRED (Federal Reserve Economic Data).

## Architecture

### Core Components

1. **DataAdapterManager** - Central orchestrator
   - Auto-detects data formats
   - Routes to appropriate adapters
   - Manages batch processing
   - Pluggable adapter system

2. **IDataAdapter Interface** - Base for all adapters
   - Type detection (`can_handle()`)
   - Parsing (`parse()`, `parse_content()`)
   - Sanitization (`sanitize()`)
   - Format specification

3. **NormalizedData** - Unified output format
   - Chunks with metadata
   - Numerical features
   - Quality metrics
   - Warning/error tracking

## Implemented Adapters

### 1. CSV Adapter ✅
- **Features:**
  - Configurable delimiter, quote character
  - Header detection and custom columns
  - Row-level chunking
  - Automatic numerical feature extraction
  
- **Use Cases:**
  - Financial data (stock prices, economic indicators)
  - Tabular datasets
  - Spreadsheet exports

- **Example:**
```cpp
CSVConfig config;
config.delimiter = ',';
config.has_header = true;

CSVAdapter adapter(config);
auto result = adapter.parse("data.csv");
```

### 2. JSON Adapter ✅
- **Features:**
  - Handles objects, arrays, and JSONL
  - Nested structure flattening
  - Automatic metadata extraction
  - Type-aware parsing

- **Use Cases:**
  - API responses
  - Configuration files
  - Structured logs
  - NoSQL exports

- **Example:**
```cpp
JSONAdapter adapter;
auto result = adapter.parse("data.json");
// Automatically handles single objects or arrays
```

### 3. FRED API Adapter ✅
- **Features:**
  - Direct API integration
  - Series search and discovery
  - Time series data handling
  - Popular indicators support

- **Use Cases:**
  - Economic indicators (GDP, inflation, unemployment)
  - Interest rates
  - Exchange rates
  - Market indices

- **Example:**
```cpp
FREDConfig config;
config.api_key = "your_api_key";

FREDAdapter adapter(config);
auto series = adapter.fetch_series("GDP", "2020-01-01", "2024-01-01");
```

- **Popular Series:**
  - GDP - Gross Domestic Product
  - UNRATE - Unemployment Rate
  - CPIAUCSL - Consumer Price Index
  - FEDFUNDS - Federal Funds Rate
  - DGS10 - 10-Year Treasury Rate
  - SP500 - S&P 500 Index
  - GOLDAMGBD228NLBM - Gold Price

- **Note:** HTTP client implementation pending (libcurl recommended)

## Chunking Strategies

### Fixed Size
- Split by character count
- Good for uniform processing
- Configurable overlap for context

### Paragraph
- Split by double newlines
- Preserves semantic boundaries
- Default for documents

### Semantic (Planned)
- Split by similarity
- Requires embeddings
- Best quality, higher cost

### Sentence (Planned)
- Split by sentence boundaries
- Preserves complete thoughts
- Good for QA systems

## Usage Examples

### Basic Auto-Detection
```cpp
#include "vdb/adapters/data_adapter.hpp"

using namespace vdb::adapters;

DataAdapterManager manager;

// Auto-detect and parse
auto result = manager.auto_parse("data.csv");
if (result) {
    for (const auto& chunk : result->chunks) {
        std::cout << chunk.content << "\n";
    }
}
```

### Batch Processing
```cpp
std::vector<fs::path> files = {
    "data1.csv",
    "data2.json",
    "data3.txt"
};

ChunkConfig config;
config.strategy = ChunkStrategy::Paragraph;
config.max_chunk_size = 1000;

auto results = manager.parse_batch(files, config);
```

### Custom Adapter Registration
```cpp
class MyAdapter : public IDataAdapter {
    // Implement interface methods
};

manager.register_adapter(std::make_unique<MyAdapter>());
```

### FRED Integration
```cpp
FREDConfig fred_config;
fred_config.api_key = std::getenv("FRED_API_KEY");

auto fred_adapter = std::make_unique<FREDAdapter>(fred_config);
manager.register_adapter(std::move(fred_adapter));

// Now can search and fetch economic data
auto gdp_series = fred_adapter->fetch_series("GDP");
auto normalized = fred_adapter->series_to_normalized(*gdp_series);
```

## Integration with VectorDatabase

### Ingesting Normalized Data
```cpp
VectorDatabase db(config);
DataAdapterManager adapter_manager;

auto data = adapter_manager.auto_parse("financial_data.csv");
if (data) {
    for (const auto& chunk : data->chunks) {
        // Generate embedding
        auto embedding = text_encoder->encode(chunk.content);
        
        // Create metadata
        Metadata meta;
        meta.content = chunk.content;
        for (const auto& [key, value] : chunk.metadata) {
            // Store metadata fields
        }
        
        // Add to database
        db.add_vector(embedding->view(), meta);
    }
}
```

## Planned Adapters

### High Priority
1. **PDF Adapter**
   - Page extraction
   - Table detection
   - OCR support
   - Metadata preservation

2. **Plain Text Adapter**
   - Encoding detection
   - Smart chunking
   - Format cleanup

3. **Markdown Adapter** (enhance existing)
   - Header hierarchy
   - Code block handling
   - Link extraction

### Medium Priority
4. **Excel Adapter**
   - Multi-sheet support
   - Formula evaluation
   - Chart metadata

5. **HTML/Web Adapter**
   - DOM parsing
   - Content extraction
   - Link following

6. **SQL/Database Adapter**
   - Direct DB connection
   - Query results
   - Table schemas

### Integration Adapters
7. **Yahoo Finance**
   - Stock quotes
   - Historical data
   - Company info

8. **Alpha Vantage**
   - Technical indicators
   - Cryptocurrency
   - Forex

9. **REST API Generic**
   - Configurable endpoints
   - Authentication
   - Rate limiting

## Configuration

### Global Settings
```cpp
// CMakeLists.txt additions
target_sources(vdb_core PRIVATE
    src/adapters/data_adapter.cpp
    src/adapters/csv_adapter.cpp
    src/adapters/json_adapter.cpp
    src/adapters/fred_adapter.cpp
)
```

### Required Dependencies
- nlohmann/json (already present)
- libcurl (for HTTP adapters) - TODO
- zlib (already present)

## Testing

### Unit Tests Needed
1. CSV parsing with various delimiters
2. JSON structure detection
3. Chunking strategies
4. Metadata extraction
5. Error handling

### Integration Tests
1. End-to-end ingestion pipeline
2. Batch processing performance
3. FRED API mocking
4. Format detection accuracy

## Performance Considerations

### Optimizations
- Lazy loading for large files
- Streaming parsers for memory efficiency
- Parallel batch processing (TODO)
- Caching for repeated files

### Benchmarks
- Target: 10MB/sec for CSV
- Target: 5MB/sec for JSON
- Target: 100 chunks/sec processing

## Next Steps

### Immediate (Week 1)
1. ✅ Core adapter framework
2. ✅ CSV adapter
3. ✅ JSON adapter  
4. ✅ FRED adapter
5. Add unit tests
6. Implement HTTP client for FRED

### Short Term (Week 2)
1. PDF adapter (using poppler or similar)
2. Enhanced markdown adapter
3. Plain text adapter with encoding detection
4. Batch processing parallelization

### Medium Term (Week 3-4)
1. Excel adapter
2. HTML/web adapter
3. SQL database adapter
4. Yahoo Finance integration
5. Query cache for adapters

## Security & Validation

### Input Validation
- File size limits
- Content sanitization
- Malicious input detection
- Type validation

### API Security
- API key management
- Rate limiting
- Request timeout
- SSL/TLS for HTTPS

## Error Handling

All adapters return `Result<T>` types:
- `std::expected<T, Error>` for operations that can fail
- Detailed error messages
- Error code categories (IoError, InvalidData, etc.)
- Graceful degradation

## Documentation

API documentation available in header files:
- `/include/vdb/adapters/data_adapter.hpp` - Core interfaces
- `/include/vdb/adapters/csv_adapter.hpp` - CSV specific
- `/include/vdb/adapters/json_adapter.hpp` - JSON specific
- `/include/vdb/adapters/fred_adapter.hpp` - FRED API specific

## Contributing

To add a new adapter:
1. Inherit from `IDataAdapter`
2. Implement required virtual methods
3. Add to CMakeLists.txt
4. Register in `DataAdapterManager` constructor
5. Write unit tests

---

**Status:** Phase 1 Complete (Core + CSV + JSON + FRED)  
**Next:** Testing, HTTP client, PDF adapter  
**Version:** 1.0.0-alpha
