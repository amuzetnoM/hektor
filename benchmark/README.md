# HEKTOR Benchmark Suite
> Comprehensive Stress Testing & Performance Benchmarking Framework  
> Version: 1.0.0  
> Date: 2026-01-24

---

## Overview

This directory contains the comprehensive benchmarking and stress testing framework for HEKTOR Vector Database. The suite follows the audit standards defined in `docs/.SCOPE/audit/` and provides meticulous, masterclass-level testing and reporting.

## Directory Structure

```
benchmark/
├── database/          # Database core benchmarks
│   ├── stress_tests/  # 1M+ vector stress tests
│   ├── hnsw/          # HNSW index benchmarks
│   ├── simd/          # SIMD optimization tests
│   ├── memory/        # Memory profiling
│   ├── distributed/   # Distributed system tests
│   └── latency/       # Latency and throughput tests
├── studio/            # Studio/UI benchmarks
│   ├── electron/      # Electron app integration tests
│   ├── native_addon/  # Native addon performance
│   ├── ui/            # UI component benchmarks
│   ├── e2e/           # End-to-end workflow tests
│   └── visualization/ # 3D rendering performance
├── reports/           # Generated benchmark reports
│   ├── database/      # Database test results
│   ├── studio/        # Studio test results
│   └── summary/       # Aggregate summaries
├── scripts/           # Execution and utility scripts
│   ├── run_all.sh     # Master execution script
│   ├── run_database.sh # Database benchmarks
│   ├── run_studio.sh  # Studio benchmarks
│   └── generate_report.py # Report generation
├── data/              # Test data and datasets
│   ├── vectors/       # Vector datasets (1K, 10K, 100K, 1M, 10M)
│   └── fixtures/      # Test fixtures
└── configs/           # Configuration files
    ├── database.yaml  # Database test configs
    └── studio.yaml    # Studio test configs
```

## Test Categories

### 1. Database Core Tests

#### 1.1 Vector Stress Tests
- **1M Vector Insertion**: Measure insertion throughput and latency
- **10M Vector Search**: Query performance at scale
- **Concurrent Operations**: Multi-threaded read/write stress
- **Memory Pressure**: Behavior under limited memory
- **Index Rebuilding**: Performance of index optimization

#### 1.2 HNSW Index Benchmarks
- **Construction Time**: Index building at various scales
- **Search Accuracy**: Recall@K metrics (K=1,5,10,50,100)
- **Parameter Tuning**: M and ef_construction optimization
- **Update Performance**: Insert/delete while searching

#### 1.3 SIMD Optimization Tests
- **Distance Computation**: AVX2 vs AVX-512 vs scalar
- **Batch Processing**: SIMD vectorization efficiency
- **CPU Feature Detection**: Fallback correctness

#### 1.4 Memory Profiling
- **Heap Usage**: Memory consumption patterns
- **Memory-Mapped I/O**: mmap efficiency
- **Cache Performance**: L1/L2/L3 cache hit rates
- **Memory Leaks**: Long-running leak detection

#### 1.5 Distributed System Tests
- **Replication Lag**: Async/sync replication latency
- **Shard Distribution**: Data distribution uniformity
- **Network Partitioning**: Split-brain handling
- **Failover Time**: Recovery time objectives

#### 1.6 Latency & Throughput
- **Query Latency**: p50, p95, p99, p99.9 percentiles
- **Insert Throughput**: Documents/second sustained
- **QPS Under Load**: Queries per second capacity
- **Mixed Workloads**: Read/write ratio testing

### 2. Studio/UI Tests

#### 2.1 Electron Integration
- **Startup Time**: Cold start and warm start
- **Memory Footprint**: RAM usage baseline and peak
- **IPC Performance**: Main/renderer communication latency
- **Native Module Loading**: Addon initialization time

#### 2.2 Native Addon Performance
- **C++ Bindings Overhead**: N-API call latency
- **Data Transfer**: Zero-copy verification
- **Callback Performance**: JS to C++ round-trip time
- **Thread Safety**: Concurrent addon calls

#### 2.3 UI Component Benchmarks
- **Render Performance**: 60fps/120fps targets
- **Component Mount Time**: Initial render latency
- **State Updates**: Re-render efficiency
- **Event Handling**: Input lag measurement

#### 2.4 End-to-End Workflows
- **Database Connection**: Connection establishment time
- **Query Execution**: Full query workflow latency
- **Result Visualization**: Data → display pipeline
- **Export Operations**: File generation performance

#### 2.5 3D Visualization
- **Three.js Performance**: FPS under various vector counts
- **WebGL Shader Compilation**: Initial load time
- **Camera Movement**: Interaction responsiveness
- **Large Dataset Rendering**: LOD and culling efficiency

## Reporting Standards

All reports follow the audit standards in `docs/.SCOPE/audit/` and include:

1. **Executive Summary**
   - Key findings and metrics
   - Pass/fail criteria
   - Critical issues identified

2. **Detailed Metrics**
   - Raw performance numbers
   - Statistical analysis (mean, median, std dev)
   - Percentile distributions
   - Comparison with baselines

3. **Visualizations**
   - Performance graphs
   - Distribution histograms
   - Time series plots
   - Comparison charts

4. **Recommendations**
   - Performance bottlenecks
   - Optimization opportunities
   - Configuration tuning suggestions

5. **Reproducibility**
   - Exact test conditions
   - Hardware specifications
   - Software versions
   - Configuration files

## Quick Start

### Run All Benchmarks
```bash
cd benchmark
./scripts/run_all.sh
```

### Run Database Benchmarks Only
```bash
./scripts/run_database.sh
```

### Run Studio Benchmarks Only
```bash
./scripts/run_studio.sh
```

### Generate Reports
```bash
./scripts/generate_report.py --format html --output reports/
```

## Requirements

### For Database Benchmarks
- CMake 3.20+
- C++23 compiler (GCC 13+, Clang 16+, MSVC 19.33+)
- Python 3.10+
- 16GB+ RAM (32GB recommended for 10M+ vector tests)
- 50GB+ free disk space

### For Studio Benchmarks
- Node.js 18+
- npm 9+
- Electron 28+
- Graphics card with WebGL 2.0 support

## Performance Baselines

### Database (Intel Core i7-12700H, 32GB RAM, NVMe SSD)

| Scale | Insert (docs/sec) | Search p50 | Search p99 | Recall@10 |
|-------|------------------|------------|------------|-----------|
| 100K  | 125              | 1.2 ms     | 2.8 ms     | 98.5%     |
| 1M    | 125              | 2.1 ms     | 4.8 ms     | 98.1%     |
| 10M   | 83               | 4.3 ms     | 9.2 ms     | 97.5%     |

### Studio (Same Hardware)

| Metric | Target | Acceptable | Critical |
|--------|--------|------------|----------|
| Startup Time | <2s | <3s | >5s |
| Memory Usage | <500MB | <800MB | >1GB |
| UI FPS | 60fps | 45fps | <30fps |
| Query Latency | <100ms | <200ms | >500ms |

## Configuration

### Database Test Configuration
Edit `configs/database.yaml`:
```yaml
vector_counts: [1000, 10000, 100000, 1000000, 10000000]
dimensions: [128, 384, 512, 768, 1024]
metrics: [cosine, l2, dot_product]
hnsw_m: [8, 16, 32, 64]
hnsw_ef_construction: [100, 200, 400]
hnsw_ef_search: [50, 100, 200, 400]
```

### Studio Test Configuration
Edit `configs/studio.yaml`:
```yaml
vector_counts: [100, 1000, 10000, 100000]
test_duration_sec: 60
target_fps: [60, 120]
enable_3d: true
enable_profiling: true
```

## CI/CD Integration

The benchmark suite can be integrated into CI/CD pipelines:

```yaml
# .github/workflows/benchmark.yml
- name: Run Benchmarks
  run: |
    cd benchmark
    ./scripts/run_all.sh --ci-mode
    ./scripts/generate_report.py --format json
```

## Continuous Monitoring

Set up performance regression detection:

```bash
# Compare with baseline
./scripts/compare_with_baseline.py \
  --baseline reports/baseline.json \
  --current reports/latest.json \
  --threshold 10  # 10% regression threshold
```

## Contributing

When adding new benchmarks:

1. Follow the naming convention: `test_<category>_<name>.cpp` or `.py`
2. Include configuration in `configs/`
3. Update this README with test description
4. Ensure reproducibility with fixed seeds
5. Document hardware requirements
6. Add baseline metrics

## Support

For issues or questions:
- Create an issue in the repository
- Reference the audit standards in `docs/.SCOPE/audit/`
- Include full benchmark logs and system information

---

**Audit Compliance**: This benchmark suite adheres to the standards defined in:
- `docs/.SCOPE/audit/quality/QUALITY_AUDIT.md`
- `docs/.SCOPE/audit/quality/TEST_COVERAGE_REPORT.md`
- `docs/.SCOPE/audit/reports/STANDARDS_ADHERENCE.md`

**Last Updated**: 2026-01-24  
**Maintainer**: HEKTOR Engineering Team
