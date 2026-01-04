# Vector Studio Documentation

Welcome to the Vector Studio documentation. This comprehensive guide covers everything from basic usage to advanced topics and API reference.

## Documentation Index

| Document | Description | Audience |
|----------|-------------|----------|
| [**USER_GUIDE.md**](USER_GUIDE.md) | Complete user guide from installation to advanced usage | All users |
| [**API_REFERENCE.md**](API_REFERENCE.md) | Detailed API documentation for all classes and functions | Developers |
| [**ARCHITECTURE.md**](ARCHITECTURE.md) | System design, data flow, component diagrams | Architects, contributors |
| [**DATA_INGESTION.md**](DATA_INGESTION.md) | Data ingestion module guide with adapters | Data engineers |
| [**GUIDE.md**](GUIDE.md) | Installation, usage tutorials, CLI reference | Beginners |
| [**MATH.md**](MATH.md) | Mathematical foundations, HNSW algorithm, distance metrics | ML engineers |
| [**AI_TRAINING.md**](AI_TRAINING.md) | Training custom models, fine-tuning, contrastive learning | AI researchers |
| [**MODELS.md**](MODELS.md) | Model specifications, benchmarks, integration | ML practitioners |

## Quick Links by Task

### Getting Started
- [Installation Guide](USER_GUIDE.md#installation)
- [Quick Start Tutorial](USER_GUIDE.md#getting-started)
- [First Database](USER_GUIDE.md#first-steps)

### Data Ingestion
- [Universal Data Adapters](DATA_INGESTION.md)
- [CSV Adapter](USER_GUIDE.md#csv-adapter)
- [JSON Adapter](USER_GUIDE.md#json-adapter)
- [FRED API Integration](USER_GUIDE.md#fred-api-adapter)

### Core Operations
- [Adding Vectors](USER_GUIDE.md#adding-vectors)
- [Searching](USER_GUIDE.md#searching)
- [Batch Operations](USER_GUIDE.md#batch-operations)

### API Reference
- [VectorDatabase API](API_REFERENCE.md#vectordatabase)
- [DataAdapterManager API](API_REFERENCE.md#dataadaptermanager)
- [TextEncoder API](API_REFERENCE.md#textencoder)
- [Python Bindings](API_REFERENCE.md#python-bindings-api)

### Advanced Topics
- [Performance Tuning](USER_GUIDE.md#performance-tuning)
- [Custom Adapters](USER_GUIDE.md#custom-adapters)
- [HNSW Configuration](USER_GUIDE.md#hnsw-index)
- [Distributed Deployment](USER_GUIDE.md#distributed-deployment)

## What is Vector Studio?

Vector Studio is a high-performance vector database and AI training platform designed for semantic search and machine learning applications. It provides:

- **Fast Similarity Search**: Sub-millisecond queries on millions of vectors
- **Universal Data Ingestion**: CSV, JSON, PDF, Excel, APIs (FRED, Yahoo Finance, Alpha Vantage, etc.)
- **Local Embedding Generation**: ONNX-based inference for text and images
- **Gold Standard Integration**: Seamless connection with Gold Standard journal system
- **AI Training Toolkit**: Tools for fine-tuning and training custom models
- **Python Bindings**: Easy integration with Python workflows

## Architecture Overview

```
┌────────────────────────────────────────────────────────────────────────────┐
│                              VECTOR STUDIO                                  │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  USER INTERFACES                                                          │
│  ┌─────────────┐   ┌─────────────┐   ┌─────────────┐   ┌─────────────┐   │
│  │   Python    │   │   C++ CLI   │   │  REST API   │   │    GUI      │   │
│  │   pyvdb     │   │     vdb     │   │  (planned)  │   │  (planned)  │   │
│  └──────┬──────┘   └──────┬──────┘   └──────┬──────┘   └──────┬──────┘   │
│         │                 │                 │                 │           │
│         └────────────────┬┴─────────────────┴─────────────────┘           │
│                          │                                                 │
│  CORE ENGINE (C++23)                                                      │
│  ┌───────────────────────┴───────────────────────────────────────────┐    │
│  │                                                                     │    │
│  │  DATA INGESTION              VECTOR OPS         STORAGE           │    │
│  │  ┌──────────────┐          ┌───────────┐    ┌────────────┐       │    │
│  │  │  Adapters    │          │   HNSW    │    │  Memory    │       │    │
│  │  │  • CSV       │  ──────→ │   Index   │    │  Mapped    │       │    │
│  │  │  • JSON      │          │           │    │  Storage   │       │    │
│  │  │  • PDF       │          │  O(log n) │    │            │       │    │
│  │  │  • Excel     │          │  Search   │    │  vectors   │       │    │
│  │  │  • APIs      │          │           │    │  .bin      │       │    │
│  │  │  • SQLite    │          │  99%+     │    │  index     │       │    │
│  │  └──────────────┘          │  Recall   │    │  .hnsw     │       │    │
│  │                            └───────────┘    └────────────┘       │    │
│  │                                                                     │    │
│  │  EMBEDDINGS                                                        │    │
│  │  ┌───────────────────────────────────────────────────────────┐    │    │
│  │  │  ONNX Runtime                                              │    │    │
│  │  │  • Text: MiniLM-L6 (384-dim)                              │    │    │
│  │  │  • Image: CLIP ViT (512-dim)                              │    │    │
│  │  │  • Local inference, no API calls                          │    │    │
│  │  └───────────────────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
└────────────────────────────────────────────────────────────────────────────┘
```
└────────────────────────────────────────────────────────────────────────────┘
```

## Getting Started

### 1. Installation

```powershell
cd vector_database
python scripts/setup.py --auto-install
python scripts/download_models.py
.\scripts\build.ps1
```

### 2. First Database

```python
import pyvdb

# Create database
db = pyvdb.create_gold_standard_db("./my_vectors")

# Add documents
db.add_text("Gold analysis for today", {
    "type": "Journal",
    "date": "2025-12-01"
})

# Search
results = db.search("gold outlook", k=5)
```

### 3. Learn More

- **Beginners**: Start with [GUIDE.md](GUIDE.md)
- **Developers**: Read [ARCHITECTURE.md](ARCHITECTURE.md)
- **Researchers**: Explore [MATH.md](MATH.md) and [AI_TRAINING.md](AI_TRAINING.md)
- **Model Selection**: See [MODELS.md](MODELS.md)

## Key Features

### 🚀 Performance

| Metric | Value |
|--------|-------|
| Query latency (1M vectors) | ~3ms |
| Insertion throughput | 10,000/sec |
| Recall@10 | 99%+ |
| Memory per vector | ~1.2 KB |

### 🧠 AI Capabilities

- **Text Embeddings**: MiniLM, MPNet, E5 models
- **Image Embeddings**: CLIP ViT (cross-modal search)
- **Custom Training**: Fine-tune or train from scratch
- **Local Inference**: No cloud dependency

### 🔧 Integration

- **Gold Standard**: Automatic journal ingestion
- **Python**: Full pyvdb bindings
- **CLI**: vdb command-line tool
- **Export**: Training data extraction

## Document Summaries

### ARCHITECTURE.md

Comprehensive system design document covering:
- Component diagrams and data flow
- Memory layout and file formats
- Threading model and concurrency
- API design patterns
- Performance characteristics

### GUIDE.md

Practical usage guide including:
- Step-by-step installation
- Building from source
- Python API tutorial
- CLI reference
- Configuration options
- Troubleshooting

### MATH.md

Mathematical foundations:
- Vector spaces and embeddings
- Distance metrics (cosine, Euclidean)
- HNSW algorithm analysis
- Complexity proofs
- Quantization techniques

### AI_TRAINING.md

Complete AI training guide:
- Pretrained model usage
- Fine-tuning strategies
- Training from scratch
- Contrastive learning
- Multi-modal training
- ONNX deployment

### MODELS.md

Model reference:
- Detailed specifications
- Benchmark results
- Model cards
- Custom integration
- Provider selection

## License

Vector Studio is released under the MIT License. See [../LICENSE](../LICENSE) for details.

## Contributing

See [CONTRIBUTING.md](../CONTRIBUTING.md) for contribution guidelines.
