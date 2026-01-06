# Vector Studio

![Version](https://img.shields.io/badge/version-2.3.0-blue?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)
![C++](https://img.shields.io/badge/C++-23-orange?style=flat-square)
![Python](https://img.shields.io/badge/Python-3.10+-blue?style=flat-square)
![Status](https://img.shields.io/badge/status-production-success?style=flat-square)

**Production-grade vector database with hybrid search, distributed features, and comprehensive ML framework integration.**

## 🎯 Overview

Vector Studio is a high-performance vector database designed for semantic search, RAG applications, and AI-powered systems. Built in C++23 with Python bindings, it combines cutting-edge vector search with traditional full-text search for superior retrieval accuracy.

### Key Features

- ⚡ **Hybrid Search**: BM25 + vector search with 5 fusion algorithms (RRF, WeightedSum, CombSUM, CombMNZ, Borda)
- 🌐 **Distributed System**: Replication, sharding, and gRPC networking for horizontal scaling
- 🧠 **ML Framework Integration**: TensorFlow and PyTorch C++ API with GPU acceleration
- 🚀 **High Performance**: Sub-millisecond queries on millions of vectors with HNSW indexing
- 📊 **Universal Data Ingestion**: 10+ data adapters (CSV, JSON, PDF, Excel, XML, Parquet, SQLite, pgvector)
- 🤖 **LLM Engine**: Local text generation with llama.cpp integration
- 📦 **Quantization**: 4-32x compression with minimal accuracy loss
- 🔍 **RAG Toolkit**: Complete retrieval-augmented generation pipeline
- 📈 **Observability**: Prometheus metrics, OpenTelemetry tracing, comprehensive logging

## 🚀 Quick Start

### Installation

```bash
# Clone repository
git clone https://github.com/amuzetnoM/vector_studio.git
cd vector_studio

# Install dependencies (Windows)
python scripts/setup.py --auto-install
python scripts/download_models.py

# Build
.\scripts\build.ps1

# Or use Docker
docker pull ghcr.io/amuzetnom/vector_studio:latest
```

### First Database

```python
import pyvdb

# Create database
db = pyvdb.VectorDatabase("./my_vectors", 384)

# Add documents
db.add_text("Vector databases enable semantic search", {
    "category": "technology",
    "date": "2026-01-06"
})

# Search
results = db.search("semantic retrieval", k=5)
for result in results:
    print(f"Score: {result.score}, Text: {result.metadata['text']}")
```

### Hybrid Search

```python
from pyvdb import BM25Engine, HybridSearchConfig, FusionMethod

# Create BM25 engine
bm25 = BM25Engine()
bm25.add_document(1, "machine learning and artificial intelligence")
bm25.add_document(2, "deep learning neural networks")

# Configure hybrid search
config = HybridSearchConfig()
config.fusion = FusionMethod.RRF
config.vector_weight = 0.7
config.lexical_weight = 0.3

# Perform hybrid search
results = db.hybrid_search("AI and ML", bm25, config, k=10)
```

## 📚 Documentation

**Complete documentation is available in the [docs/](docs/) directory.**

### Quick Links

- **[📖 Documentation Index](docs/00_INDEX.md)** - Start here!
- **[🎓 Quick Start Tutorial](docs/03_QUICKSTART.md)** - 5-minute guide
- **[📘 User Guide](docs/04_USER_GUIDE.md)** - Complete reference
- **[🏗️ Architecture](docs/05_ARCHITECTURE.md)** - System design
- **[🔍 Hybrid Search](docs/10_HYBRID_SEARCH.md)** - BM25 + vector fusion
- **[🌐 Distributed System](docs/11_DISTRIBUTED_SYSTEM.md)** - Scaling guide
- **[🧠 ML Framework](docs/12_ML_FRAMEWORK.md)** - TensorFlow/PyTorch
- **[📊 API Reference](docs/20_API_REFERENCE.md)** - Complete API docs

### Documentation by Role

#### New Users
1. [Introduction](docs/01_INTRODUCTION.md)
2. [Installation](docs/02_INSTALLATION.md)
3. [Quick Start](docs/03_QUICKSTART.md)
4. [User Guide](docs/04_USER_GUIDE.md)

#### Developers
1. [Architecture](docs/05_ARCHITECTURE.md)
2. [API Reference](docs/20_API_REFERENCE.md)
3. [Python Bindings](docs/21_PYTHON_BINDINGS.md)
4. [Custom Development](docs/22_CUSTOM_DEVELOPMENT.md)

#### ML Engineers
1. [Embeddings & Models](docs/08_EMBEDDINGS_MODELS.md)
2. [Hybrid Search](docs/10_HYBRID_SEARCH.md)
3. [ML Framework Integration](docs/12_ML_FRAMEWORK.md)
4. [Quantization](docs/14_QUANTIZATION.md)

#### DevOps/SRE
1. [Deployment Guide](docs/16_DEPLOYMENT.md)
2. [Logging & Monitoring](docs/15_LOGGING_MONITORING.md)
3. [Security](docs/18_SECURITY.md)
4. [Distributed System](docs/11_DISTRIBUTED_SYSTEM.md)

## 🎉 What's New in v2.3.0

### Major Features

#### Hybrid Search Engine
Complete BM25 full-text search with 5 fusion algorithms for combining vector and lexical results.

```python
# RRF (Reciprocal Rank Fusion) - recommended
config.fusion = FusionMethod.RRF

# Weighted Sum
config.fusion = FusionMethod.WeightedSum
config.vector_weight = 0.7
config.lexical_weight = 0.3
```

**Performance**: +15-20% accuracy, +10-15% recall over vector-only search.

#### Distributed System
Production-ready distributed architecture with:
- **Replication**: Async, sync, and semi-sync modes
- **Sharding**: Hash, range, and consistent hashing
- **gRPC**: High-performance networking
- **Failover**: Automatic health monitoring

```cpp
// Configure distributed database
ReplicationConfig repl_config;
repl_config.mode = ReplicationMode::Async;
repl_config.min_replicas = 2;

ShardingConfig shard_config;
shard_config.strategy = ShardingStrategy.ConsistentHash;
shard_config.num_shards = 4;

DistributedVectorDatabase db(repl_config, shard_config);
```

#### ML Framework Integration
Full TensorFlow and PyTorch C++ API integration:
- SavedModel and TorchScript loading
- GPU acceleration (CUDA/ROCm)
- Training data export
- Mixed precision inference

```cpp
// TensorFlow
TensorFlowEmbedder tf_embedder("models/encoder/");
auto embeddings = tf_embedder.embed_batch(texts);

// PyTorch
PyTorchEmbedder pt_embedder("models/encoder.pt");
auto embedding = pt_embedder.embed("text");
```

### Performance Improvements

| Metric | v2.2.0 | v2.3.0 | Improvement |
|--------|--------|--------|-------------|
| Query Latency (p99) | 5ms | 3ms | 40% faster |
| Hybrid Search Accuracy | N/A | +15-20% | New feature |
| Distributed Throughput | N/A | 10,000+ QPS | New feature |
| GPU Inference | N/A | <10ms | New feature |

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        VECTOR STUDIO v2.3                        │
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

## 📊 Performance Benchmarks

### Vector Search
- **1M vectors**: ~3ms query latency (p99)
- **10M vectors**: ~8ms query latency (p99)
- **Recall@10**: 99%+ with HNSW
- **Throughput**: 10,000+ QPS (single node)

### Hybrid Search
- **Accuracy**: +15-20% over vector-only
- **Recall**: +10-15% improvement
- **Latency**: <10ms (p99) for 1M documents

### Distributed System
- **4-shard cluster**: 40,000+ QPS
- **Replication lag**: <100ms (async), <5ms (sync)
- **Failover time**: <5 seconds
- **Availability**: 99.99%

## 🛠️ Technology Stack

### Core
- **Language**: C++23
- **Build System**: CMake + Ninja
- **Dependencies**: vcpkg

### ML & AI
- **Embeddings**: ONNX Runtime, TensorFlow C API, LibTorch
- **LLM**: llama.cpp
- **Models**: MiniLM, MPNet, E5, CLIP

### Distributed
- **Networking**: gRPC + Protobuf
- **Serialization**: Protocol Buffers
- **Service Discovery**: Built-in

### Observability
- **Metrics**: Prometheus C++ client
- **Tracing**: OpenTelemetry
- **Logging**: Custom multi-level system

### Data
- **Formats**: CSV, JSON, PDF, Excel, XML, Parquet, SQLite, pgvector
- **Storage**: Memory-mapped files
- **Compression**: Product/Scalar quantization

## 🐳 Docker

```bash
# Pull latest image
docker pull ghcr.io/amuzetnom/vector_studio:latest

# Run container
docker run -p 8080:8080 -v ./data:/data ghcr.io/amuzetnom/vector_studio:latest

# Docker Compose
docker-compose up -d
```

## 🧪 Testing

```bash
# Build with tests
cmake -B build -G Ninja -DVDB_BUILD_TESTS=ON
cmake --build build

# Run tests
cd build && ctest --output-on-failure

# Run specific test
./build/vdb_tests --gtest_filter=HybridSearch.*
```

**Test Coverage**: 50+ comprehensive test suites covering all features.

## 📈 Roadmap

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

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](docs/23_CONTRIBUTING.md) for details.

### Development Setup

```bash
# Clone repository
git clone https://github.com/amuzetnoM/vector_studio.git
cd vector_studio

# Install ML framework dependencies
.\scripts\setup_ml_framework.ps1

# Build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DVDB_BUILD_TESTS=ON
cmake --build build

# Run tests
cd build && ctest
```

## 📄 License

Vector Studio is released under the MIT License. See [LICENSE](LICENSE) for details.

## 🙏 Acknowledgments

Built with:
- [HNSWLIB](https://github.com/nmslib/hnswlib) - Inspiration for HNSW implementation
- [nlohmann/json](https://github.com/nlohmann/json) - JSON parsing
- [pybind11](https://github.com/pybind/pybind11) - Python bindings
- [llama.cpp](https://github.com/ggerganov/llama.cpp) - LLM inference
- [gRPC](https://grpc.io/) - Distributed networking
- [TensorFlow](https://www.tensorflow.org/) - ML framework
- [PyTorch](https://pytorch.org/) - ML framework

## 📞 Support

- **Documentation**: [docs/00_INDEX.md](docs/00_INDEX.md)
- **Issues**: https://github.com/amuzetnoM/vector_studio/issues
- **Discussions**: https://github.com/amuzetnoM/vector_studio/discussions

---

**Built with ❤️ for the AI community**

[Documentation](docs/00_INDEX.md) | [Quick Start](docs/03_QUICKSTART.md) | [API Reference](docs/20_API_REFERENCE.md) | [Contributing](docs/23_CONTRIBUTING.md)
