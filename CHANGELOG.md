# Changelog

All notable changes to Vector Studio will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- **XML Adapter**: Full support for XML document parsing with DOM traversal, attribute extraction, and hierarchical metadata
- **Parquet Adapter**: Apache Parquet columnar format support (requires Apache Arrow library)
- **SQLite Adapter**: Complete SQLite database reading with multi-table support and custom SQL queries
- **pgvector Adapter**: PostgreSQL with pgvector extension integration for distributed vector storage
  - Bidirectional sync (read/write operations)
  - Native similarity search with distance operators
  - IVFFlat and HNSW index support
  - Batch insert operations
- Comprehensive documentation suite (ARCHITECTURE.md, GUIDE.md, MATH.md, AI_TRAINING.md, MODELS.md)
- Enhanced data format documentation with examples for all new formats
- Usage examples for XML, Parquet, SQLite, and pgvector adapters
- Automated setup scripts for Windows (setup.ps1) and Unix (setup.sh)
- Python virtual environment management with auto-activation
- Automatic Python installation via winget/brew/apt
- Git configuration files (.gitignore, .gitattributes)
- CONTRIBUTING.md with development guidelines
- CHANGELOG.md for version tracking

### Changed
- Updated DataAdapterManager to register new adapters (XML, Parquet, SQLite)
- Enhanced CMakeLists.txt with conditional compilation flags for SQLite3 and libpq
- Updated format detection to include .xml, .parquet, .db, .sqlite, .sqlite3 extensions
- Renamed project from "VectorDB" to "Vector Studio"
- Updated requirements.txt with latest package versions (December 2025)
- Enhanced build.ps1 with -AutoInstall and -CheckOnly flags
- Bumped documentation version to 2.0.0

### Technical Details
- XML parsing with simple DOM parser (production can use libxml2 or pugixml)
- SQLite integration with conditional compilation (HAVE_SQLITE3 flag)
- PostgreSQL pgvector integration with conditional compilation (HAVE_LIBPQ flag)
- Parquet placeholder implementation with Apache Arrow integration notes
- All new adapters follow IDataAdapter interface
- Type-aware data extraction for numerical features
- Batch processing support for database operations

### Fixed
- (None in this release)

---

## [0.1.0] - 2025-12-01

### Added
- Initial project structure with C++ core
- HNSW index implementation with SIMD optimization (AVX2/AVX-512)
- ONNX Runtime integration for local embeddings
- Text encoder support (all-MiniLM-L6-v2)
- Image encoder support (CLIP ViT-B/32)
- Memory-mapped vector storage
- Python bindings via pybind11
- CMake build system with Ninja support
- Basic CLI tool (vdb)
- Gold Standard integration for journal/chart ingestion

### Technical Details
- C++20 standard
- Cross-platform support (Windows, Linux)
- Thread-safe read/write operations
- Configurable HNSW parameters (M, ef_construction, ef_search)
- Metadata storage in JSONL format

---

## Version History Summary

| Version | Date | Highlights |
|---------|------|------------|
| 0.1.0 | 2025-12-01 | Initial release with core functionality |

---

## Upgrade Notes

### 0.1.0

First release - no upgrade path needed.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for how to contribute changes.
