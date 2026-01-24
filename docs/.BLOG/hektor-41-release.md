# Hektor 4.1: The Studio Era Begins

*January 24, 2026*

---

Two days after shipping 4.0, we're already at 4.1. Not because 4.0 was broken — it wasn't. But because something extraordinary happened: **Hektor Studio** is ready.

## What is Hektor Studio?

Hektor started as a C++ library. Then it grew Python bindings. Then a REST API. But something was missing: **a visual interface for the people who think in pictures, not code.**

Studio is an Electron + React application that wraps the entire Hektor ecosystem:

- **Visual Index Management**: Create, configure, and monitor HNSW indices
- **Interactive Search**: Type queries, see results, visualize embeddings
- **Real-time Metrics**: Watch your indices perform in real-time
- **Perceptual Quantization Curves**: See exactly how your data is being compressed

## The Technical Journey

Building Studio required solving a problem I hadn't anticipated: **bridging C++23 with Node.js 22**.

### The Node 22 Challenge

Node 22 ships with a newer V8 engine that conflicts with C++23's `std::expected`. Both define similar types, and the linker doesn't know which to choose.

Our solution:
1. **cmake-js** instead of node-gyp (full CMake support)
2. **Namespace isolation** in our native addon headers
3. **Conditional compilation** based on Node.js version detection

The result: a native addon that runs at full speed, with zero JavaScript overhead for hot paths.

### The Native Addon Architecture

```
┌─────────────────────────────────────────────────┐
│                 Hektor Studio                    │
│              (Electron + React)                  │
├─────────────────────────────────────────────────┤
│                 Native Addon                     │
│   ┌─────────┬─────────┬─────────┬─────────┐     │
│   │ BM25    │ Hybrid  │ PQ/HDR  │ Index   │     │
│   │ Engine  │ Search  │ Quant   │ Ops     │     │
│   └─────────┴─────────┴─────────┴─────────┘     │
├─────────────────────────────────────────────────┤
│              libvdb_core.lib                     │
│           (Full C++23 Engine)                    │
└─────────────────────────────────────────────────┘
```

Every search, every quantization operation, every index build happens in native code. The JavaScript layer is just UI.

## Perceptual Quantization Goes Visual

The crown jewel of 4.1 is **visual PQ curve editing**.

Perceptual Quantization isn't just compression — it's intelligent compression that preserves the information humans (and models) care about most. In 4.0, you configured it via JSON:

```json
{
  "pq_enabled": true,
  "curve": "st2084",
  "num_subquantizers": 8
}
```

In 4.1 Studio, you **see the curve**:

- Interactive ST 2084 (Dolby PQ) visualization
- Real-time preview of quantization effects
- Side-by-side comparison with linear quantization
- Export to any display profile: SDR, HDR10, Dolby Vision, HLG

## New Features in 4.1

### Hybrid Search Fusion Methods

We added four new fusion algorithms beyond RRF:

| Method | Description | Best For |
|--------|-------------|----------|
| **WeightedSum** | Simple linear combination | When you know your weights |
| **CombSUM** | Sum of normalized scores | Balanced results |
| **CombMNZ** | CombSUM × overlap count | Precision-focused |
| **Borda** | Rank-based voting | Robustness to outliers |

### BM25 Improvements

- **Stemming** support (Porter, Snowball)
- **Stop word** filtering
- **Custom tokenizers** for domain-specific text

### Studio-Specific

- **Embedding Preview**: See your vectors as 2D/3D projections
- **Query History**: Track and replay searches
- **Index Comparison**: A/B test different configurations

## The Numbers

Performance remains identical to 4.0 for the core engine. Studio adds:

| Metric | Value |
|--------|-------|
| Addon Build Time | 45 seconds |
| UI Render Time | <16ms (60fps) |
| Memory Overhead | ~50MB for Electron |
| Native Call Latency | <0.1ms |

## Installation

### From Source (Recommended for Developers)

```bash
git clone https://github.com/artifactvirtual/hektor.git
cd hektor
./build-hektor.sh
```

### Via pip

```bash
pip install hektor-vdb
```

### Studio

```bash
cd studio/hektor-app
npm install
npm run build:native
npm run dev
```

## What's Next

4.1 is a bridge release. It proves that a C++ vector database can have a beautiful visual interface without sacrificing performance.

For 4.2:
- **GPU-accelerated embedding generation** via ONNX Runtime
- **Distributed Studio** — manage clusters from one UI
- **Plugin architecture** for custom visualizations

For 5.0:
- **Neural quantization** — learned compression
- **Streaming search** — results as they're found
- **Multi-modal indices** — text, image, audio in one index

## The Bigger Picture

Vector databases are infrastructure. They're supposed to be invisible. But that doesn't mean they have to be ugly.

Studio is our bet that developers deserve better tools. Not just powerful — beautiful. Not just fast — intuitive.

We're just getting started.

---

```bash
pip install hektor-vdb
```

*See you in 4.2.*

— *Ali A. Shakil, ARTIFACT VIRTUAL*
