# INDEX

![](https://img.shields.io/badge/Version-2.3.0-4B0082?style=for-the-badge&labelColor=0D1117&logo=git&logoColor=white)
![](https://img.shields.io/badge/Last_Update-2026--01--06-4B0082?style=for-the-badge&labelColor=0D1117&logo=calendar&logoColor=white)
![](https://img.shields.io/github/last-commit/amuzetnoM/vector_studio?style=for-the-badge&labelColor=0D1117&logo=github&logoColor=white)

## OVERVIEW

Vector Studio is a production-grade vector database with hybrid search, distributed features, and comprehensive ML framework integration. Built in C++23 with Python bindings, it combines cutting-edge vector search with traditional full-text search for superior retrieval accuracy.

**Current Version:** v2.3.0

### ✪ Key Capabilities

- **Hybrid Search**: BM25 + vector search with 5 fusion algorithms (RRF, WeightedSum, CombSUM, CombMNZ, Borda)
- **Distributed System**: Replication, sharding, and gRPC networking for horizontal scaling
- **ML Framework Integration**: TensorFlow and PyTorch C++ API with GPU acceleration
- **High Performance**: Sub-millisecond queries on millions of vectors with HNSW indexing
- **Universal Data Ingestion**: 10+ data adapters (CSV, JSON, PDF, Excel, XML, Parquet, SQLite, pgvector)
- **LLM Engine**: Local text generation with llama.cpp integration
- **RAG Toolkit**: Complete retrieval-augmented generation pipeline

---

## CORE DOCUMENTATION

### Getting Started
| Document | Description |
|----------|-------------|
| [Introduction](docs/01_INTRODUCTION.md) | System overview, key features, and quick start guide |
| [Installation](docs/02_INSTALLATION.md) | System requirements, installation steps, and initial configuration |
| [Quick Start](docs/03_QUICKSTART.md) | Create your first database and perform basic operations |
| [User Guide](docs/04_USER_GUIDE.md) | Complete user guide covering all features and operations |

### System Architecture
| Document | Description |
|----------|-------------|
| [Architecture](docs/05_ARCHITECTURE.md) | System design, components, and data flow |
| [Data Formats](docs/06_DATA_FORMATS.md) | Supported data formats and specifications |
| [Data Ingestion](docs/07_DATA_INGESTION.md) | Data adapters, batch processing, and ingestion patterns |
| [Embeddings & Models](docs/08_EMBEDDINGS_MODELS.md) | Text and image encoders, model specifications |
| [Vector Operations](docs/09_VECTOR_OPERATIONS.md) | HNSW algorithm, distance metrics, and mathematical foundations |

### Advanced Features
| Document | Description |
|----------|-------------|
| [Hybrid Search](docs/10_HYBRID_SEARCH.md) | BM25 full-text search, fusion methods, and RAG toolkit |
| [Distributed System](docs/11_DISTRIBUTED_SYSTEM.md) | Replication, sharding, and gRPC networking |
| [ML Framework Integration](docs/12_ML_FRAMEWORK.md) | TensorFlow and PyTorch integration with GPU acceleration |
| [LLM Engine](docs/13_LLM_ENGINE.md) | Local text generation with llama.cpp |
| [Quantization](docs/14_QUANTIZATION.md) | Vector compression and quantization techniques |
| [Logging & Monitoring](docs/15_LOGGING_MONITORING.md) | Logging system, Prometheus metrics, and observability |

### Deployment & Operations
| Document | Description |
|----------|-------------|
| [Deployment Guide](docs/16_DEPLOYMENT.md) | Docker, Kubernetes, and production deployment |
| [Performance Tuning](docs/17_PERFORMANCE_TUNING.md) | Benchmarks, optimization techniques, and best practices |
| [Security](docs/18_SECURITY.md) | Security best practices and guidelines |
| [Real-World Applications](docs/19_REAL_WORLD_APPLICATIONS.md) | Production use cases and success stories |

### Development & Extension
| Document | Description |
|----------|-------------|
| [API Reference](docs/20_API_REFERENCE.md) | Complete C++ API documentation |
| [Python Bindings](docs/21_PYTHON_BINDINGS.md) | Python API reference and examples |
| [Custom Development](docs/22_CUSTOM_DEVELOPMENT.md) | Developing custom adapters and extensions |
| [Contributing Guide](docs/23_CONTRIBUTING.md) | How to contribute to Vector Studio |

### Documentation Meta
| Document | Description |
|----------|-------------|
| [Documentation Index](docs/00_INDEX.md) | Comprehensive documentation navigation hub |
| [Changelog](CHANGELOG.md) | Complete version history and release notes |
| [Release Notes v2.2.0](RELEASE_NOTES_v2.2.0.md) | Hybrid search release details |
| [New Features](docs/NEW_FEATURES.md) | What's new in Vector Studio v2.0+ |

---

## MODULE DIRECTORIES

### Core Library
| Module | Description | Documentation |
|--------|-------------|---------------|
| **src/core** | Vector operations, distance metrics, thread pool | Core module |
| **src/index** | HNSW and flat index implementations | Core module |
| **src/storage** | Memory-mapped storage and metadata management | Core module |
| **src/database.cpp** | Main database implementation | Core module |

### Data Ingestion & Adapters
| Module | Description | Documentation |
|--------|-------------|---------------|
| **src/adapters** | Data adapters (CSV, JSON, PDF, Excel, XML, Parquet, SQLite, pgvector) | [Data Ingestion](docs/07_DATA_INGESTION.md) |
| **src/ingest** | Markdown parser and Gold Standard ingestion | Core module |

### Hybrid Search & RAG
| Module | Description | Documentation |
|--------|-------------|---------------|
| **src/hybrid** | BM25 engine and hybrid search with fusion methods | [Hybrid Search](docs/10_HYBRID_SEARCH.md) |
| **src/framework** | RAG engine and framework adapters | [RAG Toolkit](docs/10_HYBRID_SEARCH.md#rag-toolkit) |

### ML & Embeddings
| Module | Description | Documentation |
|--------|-------------|---------------|
| **src/embeddings** | ONNX Runtime text and image encoders | [Embeddings](docs/08_EMBEDDINGS_MODELS.md) |
| **src/llm** | llama.cpp LLM engine integration | [LLM Engine](docs/13_LLM_ENGINE.md) |
| **src/framework** | TensorFlow and PyTorch embedders | [ML Framework](docs/12_ML_FRAMEWORK.md) |

### Distributed System (🆕 v2.3.0)
| Module | Description | Documentation |
|--------|-------------|---------------|
| **src/distributed** | Replication and sharding managers | [Distributed System](docs/11_DISTRIBUTED_SYSTEM.md) |
| **src/grpc** | gRPC service definitions and implementations | [Distributed System](docs/11_DISTRIBUTED_SYSTEM.md) |
| **proto** | Protocol Buffer definitions | Core module |

### Quantization & Optimization
| Module | Description | Documentation |
|--------|-------------|---------------|
| **src/quantization** | Product and scalar quantization | [Quantization](docs/14_QUANTIZATION.md) |

### Python Bindings
| Module | Description | Documentation |
|--------|-------------|---------------|
| **bindings/python** | pybind11 Python bindings | [Python Bindings](docs/21_PYTHON_BINDINGS.md) |

### Testing & Examples
| Module | Description | Documentation |
|--------|-------------|---------------|
| **tests** | Comprehensive test suites (50+ tests) | Core module |
| **examples** | Python usage examples | [Examples](examples/) |

### Build & Deployment
| Module | Description | Documentation |
|--------|-------------|---------------|
| **scripts** | Build scripts, setup utilities, model downloads | [Installation](docs/02_INSTALLATION.md) |
| **deployment** | Docker configurations and deployment scripts | [Deployment](docs/16_DEPLOYMENT.md) |
| **vcpkg** | Dependency management with vcpkg | [Installation](docs/02_INSTALLATION.md) |

---

## QUICK REFERENCE

### Essential Links
- **Full README**: [INDEX.md](INDEX.md)
- **Documentation Hub**: [docs/00_INDEX.md](docs/00_INDEX.md)
- **Changelog**: [CHANGELOG.md](CHANGELOG.md)
- **License**: [LICENSE](LICENSE)
- **ML Framework Setup**: [ML_FRAMEWORK_SETUP_STATUS.md](ML_FRAMEWORK_SETUP_STATUS.md)
- **v2.3.0 PR**: [V2.3.0_PR_DESCRIPTION.md](V2.3.0_PR_DESCRIPTION.md)

### Key Features
- **Hybrid Search**: BM25 + vector with 5 fusion algorithms
- **Distributed**: Replication, sharding, gRPC networking
- **ML Frameworks**: TensorFlow and PyTorch C++ integration
- **10+ Data Adapters**: CSV, JSON, PDF, Excel, XML, Parquet, SQLite, pgvector
- **LLM Engine**: Local inference with llama.cpp
- **RAG Toolkit**: Complete retrieval-augmented generation
- **Quantization**: 4-32x compression
- **High Performance**: Sub-ms queries, 10,000+ QPS

### Quick Start Commands

```bash
# Install dependencies (Windows)
python scripts/setup.py --auto-install
python scripts/download_models.py

# Build C++ library
.\scripts\build.ps1

# Install ML framework dependencies
.\scripts\setup_ml_framework.ps1

# Build with all features
cmake -B build -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DVDB_USE_TENSORFLOW=ON `
  -DVDB_USE_PYTORCH=ON `
  -DVDB_USE_GRPC=ON `
  -DVDB_BUILD_DISTRIBUTED=ON `
  -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake

cmake --build build -j

# Install Python bindings
cd build && pip install .
```

### Python Quick Start

```python
import pyvdb

# Create database
db = pyvdb.VectorDatabase("./my_vectors", 384)

# Add documents
db.add_text("Vector databases enable semantic search", {
    "category": "technology",
    "date": "2026-01-06"
})

# Vector search
results = db.search("semantic retrieval", k=5)

# Hybrid search (v2.2.0+)
from pyvdb import BM25Engine, HybridSearchConfig, FusionMethod

bm25 = BM25Engine()
bm25.add_document(1, "machine learning and AI")

config = HybridSearchConfig()
config.fusion = FusionMethod.RRF

results = db.hybrid_search("AI and ML", bm25, config, k=10)
```

---

## SYSTEM ARCHITECTURE

```
┌─────────────────────────────────────────────────────────────────┐
│                    VECTOR STUDIO v2.3.0                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  CLIENT INTERFACES                                              │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐       │
│  │  Python  │  │  C++ CLI │  │   gRPC   │  │  Docker  │       │
│  │  pyvdb   │  │   vdb    │  │  Server  │  │  Image   │       │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘       │
│       └─────────────┴─────────────┴─────────────┘              │
│                          │                                       │
│  CORE ENGINE (C++23)                                            │
│  ┌──────────────────────┴──────────────────────────────────┐   │
│  │                                                           │   │
│  │  SEARCH LAYER                                            │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │   │
│  │  │   Vector    │  │    BM25     │  │   Hybrid    │     │   │
│  │  │   Search    │  │  Full-Text  │  │   Fusion    │     │   │
│  │  │   (HNSW)    │  │   Search    │  │  (5 algos)  │     │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘     │   │
│  │                                                           │   │
│  │  DATA LAYER                                              │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │   │
│  │  │  Adapters   │  │  Embeddings │  │   Storage   │     │   │
│  │  │  (10+)      │  │  TF/PyTorch │  │   (mmap)    │     │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘     │   │
│  │                                                           │   │
│  │  DISTRIBUTED LAYER                                       │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │   │
│  │  │ Replication │  │   Sharding  │  │    gRPC     │     │   │
│  │  │  Manager    │  │   Manager   │  │  Networking │     │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘     │   │
│  └───────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

---

## PERFORMANCE BENCHMARKS

### Vector Search
| Metric | Value |
|--------|-------|
| Query latency (1M vectors) | ~3ms (p99) |
| Query latency (10M vectors) | ~8ms (p99) |
| Recall@10 | 99%+ with HNSW |
| Throughput (single node) | 10,000+ QPS |
| Memory per vector | ~1.2 KB |

### Hybrid Search
| Metric | Value |
|--------|-------|
| Accuracy improvement | +15-20% over vector-only |
| Recall improvement | +10-15% |
| Latency (1M documents) | <10ms (p99) |

### Distributed System
| Metric | Value |
|--------|-------|
| 4-shard cluster throughput | 40,000+ QPS |
| Replication lag (async) | <100ms |
| Replication lag (sync) | <5ms |
| Failover time | <5 seconds |
| Availability (4-node) | 99.99% |

---

## TECHNOLOGY STACK

### Core
- **Language**: C++23
- **Build System**: CMake 3.20+ + Ninja
- **Dependencies**: vcpkg

### ML & AI
- **Embeddings**: ONNX Runtime, TensorFlow C API, LibTorch
- **LLM**: llama.cpp
- **Models**: MiniLM, MPNet, E5, CLIP

### Distributed
- **Networking**: gRPC v1.71.0 + Protobuf v5.29.5
- **Serialization**: Protocol Buffers
- **Service Discovery**: Built-in

### Observability
- **Metrics**: Prometheus C++ client
- **Tracing**: OpenTelemetry (planned)
- **Logging**: Custom multi-level system

### Data
- **Formats**: CSV, JSON, PDF, Excel, XML, Parquet, SQLite, pgvector
- **Storage**: Memory-mapped files
- **Compression**: Product/Scalar quantization

---

## DEPLOYMENT OPTIONS

### Local Development

```bash
# Build and run
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/vdb_cli
```

### Docker Container

```bash
# Pull latest image
docker pull ghcr.io/amuzetnom/vector_studio:latest

# Run container
docker run -p 8080:8080 -v ./data:/data ghcr.io/amuzetnom/vector_studio:latest

# Docker Compose
docker-compose up -d
```

### Python Package

```bash
# Install from build
cd build && pip install .

# Use in Python
import pyvdb
db = pyvdb.VectorDatabase("./vectors", 384)
```

---

## TESTING

### Run Tests

```bash
# Build with tests
cmake -B build -G Ninja -DVDB_BUILD_TESTS=ON
cmake --build build

# Run all tests
cd build && ctest --output-on-failure

# Run specific test suite
./build/vdb_tests --gtest_filter=HybridSearch.*
```

### Test Coverage
- **50+ comprehensive test suites**
- **Unit tests**: Core functionality
- **Integration tests**: End-to-end workflows
- **Performance tests**: Benchmarking

---

## ROADMAP

### v2.3.0 (Current)
- ✅ Hybrid search with BM25
- ✅ Distributed system (replication, sharding)
- ✅ ML framework integration (TensorFlow, PyTorch)
- ✅ Comprehensive documentation reorganization

### v2.4.0 (Q2 2026)
- [ ] GraphQL API
- [ ] WebSocket support
- [ ] Advanced RAG features
- [ ] Multi-modal search

### v3.0.0 (Q3 2026)
- [ ] Cloud-native deployment
- [ ] Kubernetes operators
- [ ] Real-time collaboration
- [ ] Advanced analytics

---

## SUPPORT & COMMUNITY

- **Documentation**: Browse [docs/](docs/) directory for detailed guides
- **Issues**: Report bugs via [GitHub Issues](https://github.com/amuzetnoM/vector_studio/issues)
- **Discussions**: Join [GitHub Discussions](https://github.com/amuzetnoM/vector_studio/discussions)
- **Changelog**: See [CHANGELOG.md](CHANGELOG.md) for version history
- **License**: MIT License - See [LICENSE](LICENSE)

---

## DISCLAIMER

⚠️ **Production Use**

This software is provided "as is" without warranty of any kind. While Vector Studio is designed for production use, always:

- Test thoroughly in your environment
- Review security settings
- Monitor performance metrics
- Keep backups of your data
- Stay updated with releases

**The authors and contributors of Vector Studio are not responsible for any data loss or system failures.**

---

## LICENSE

MIT License © 2024-2026 Vector Studio Contributors

See [LICENSE](LICENSE) file for complete details.

---

**Built with focus on performance, scalability, and production readiness. 🚀**
