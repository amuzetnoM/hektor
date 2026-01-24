# HEKTOR Benchmark Suite - Usage Guide

## Quick Start

### Run All Benchmarks
```bash
cd benchmark
./scripts/run_all.sh
```

This will:
1. Run all database benchmarks
2. Run all studio benchmarks (if available)
3. Generate comprehensive HTML and JSON reports
4. Save results to `reports/run_<timestamp>/`

### Run Database Benchmarks Only
```bash
./scripts/run_database.sh
```

### Run Studio Benchmarks Only
```bash
./scripts/run_studio.sh
```

## Individual Test Execution

### 1M Vector Stress Test
```bash
# Python version (recommended)
python3 benchmark/database/stress_tests/test_1m_vectors.py \
    --vectors 1000000 \
    --dimension 512 \
    --threads 8 \
    --duration 300 \
    --output results.json

# C++ version (requires compilation)
cd benchmark/database/stress_tests
g++ -std=c++23 -O3 -pthread test_1m_vectors.cpp -o test_1m_vectors
./test_1m_vectors --vectors 1000000 --dimension 512
```

### HNSW Performance Benchmark
```bash
python3 benchmark/database/hnsw/test_hnsw_performance.py \
    --vectors "1000,10000,100000,1000000" \
    --dimensions "384,512,768" \
    --output hnsw_results.json
```

### Latency & Throughput Benchmark
```bash
python3 benchmark/database/latency/test_latency_throughput.py \
    --vectors 100000 \
    --dimension 512 \
    --queries 10000 \
    --output latency_results.json
```

## Configuration

### Database Configuration
Edit `configs/database.yaml` to customize:
- Vector counts to test
- Dimensions
- HNSW parameters
- Stress test parameters
- Memory profiling settings

### Studio Configuration
Edit `configs/studio.yaml` to customize:
- UI rendering tests
- Electron integration tests
- 3D visualization parameters
- Performance thresholds

## Report Generation

### Generate HTML Report
```bash
python3 scripts/generate_report.py \
    --report-dir reports/run_<timestamp> \
    --output reports/run_<timestamp>/summary \
    --format html
```

### Generate JSON Report
```bash
python3 scripts/generate_report.py \
    --report-dir reports/run_<timestamp> \
    --output reports/run_<timestamp>/summary \
    --format json
```

### Generate Both
```bash
python3 scripts/generate_report.py \
    --report-dir reports/run_<timestamp> \
    --output reports/run_<timestamp>/summary \
    --format html,json
```

## CI/CD Integration

### GitHub Actions Example
```yaml
name: Benchmarks

on:
  push:
    branches: [ main ]
  schedule:
    - cron: '0 0 * * 0'  # Weekly

jobs:
  benchmark:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.11'
      
      - name: Install Dependencies
        run: |
          pip install numpy psutil
          # Install pyvdb if available
      
      - name: Run Benchmarks
        run: |
          cd benchmark
          ./scripts/run_all.sh --ci-mode
      
      - name: Upload Results
        uses: actions/upload-artifact@v3
        with:
          name: benchmark-results
          path: benchmark/reports/
```

## Performance Baselines

### Expected Performance (Intel Core i7-12700H, 32GB RAM)

**1M Vector Stress Test:**
- Throughput: 10,000+ ops/sec
- Mean latency: <3ms
- p99 latency: <10ms

**HNSW Search (1M vectors, 512D):**
- p50 latency: 2-3ms
- p99 latency: 5-8ms
- Recall@10: >98%

**Latency Distribution:**
- p50: <2ms
- p95: <5ms
- p99: <10ms
- p99.9: <20ms

**Sustained Load:**
- 1,000 QPS: <5ms mean latency
- 5,000 QPS: <10ms mean latency
- 10,000 QPS: <20ms mean latency

## Troubleshooting

### pyvdb Not Available
If `pyvdb` is not installed, benchmarks will run in simulation mode:
```bash
# Install pyvdb
pip install hektor-vdb

# Or build from source
cd /path/to/hektor
pip install -e .
```

### Memory Issues
For large-scale tests (10M+ vectors), ensure sufficient RAM:
```bash
# Reduce vector count
python3 test_1m_vectors.py --vectors 100000

# Or increase swap space
sudo fallocate -l 16G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

### Permission Errors
Ensure scripts are executable:
```bash
chmod +x scripts/*.sh
```

## Advanced Usage

### Custom Test Configuration
Create a custom config file:
```yaml
# my_config.yaml
vector_counts: [1000, 5000, 10000]
dimensions: [512]
hnsw:
  m_values: [16, 32]
  ef_construction: [200]
  ef_search: [100]
```

Then reference it in your tests.

### Profiling
Enable detailed profiling:
```bash
# CPU profiling
python3 -m cProfile -o profile.stats test_1m_vectors.py

# Memory profiling
python3 -m memory_profiler test_1m_vectors.py

# Generate flame graph
pip install py-spy
py-spy record -o profile.svg -- python3 test_1m_vectors.py
```

### Distributed Testing
For distributed benchmarks:
1. Set up multiple HEKTOR nodes
2. Configure `configs/database.yaml` with distributed settings
3. Run distributed tests (requires implementation)

## Support

### Common Issues

**Issue:** Tests run but show no real database operations
**Solution:** Install pyvdb: `pip install hektor-vdb`

**Issue:** Out of memory during large tests
**Solution:** Reduce `--vectors` parameter or increase system RAM

**Issue:** Reports not generating
**Solution:** Check that all result JSON files are present in report directory

### Getting Help
- Check logs in `reports/run_<timestamp>/execution.log`
- Review audit standards in `docs/.SCOPE/audit/`
- Open an issue on GitHub with full error logs

## Contributing

When adding new benchmarks:
1. Follow naming convention: `test_<category>_<name>.{cpp,py}`
2. Add configuration to `configs/`
3. Update `scripts/run_database.sh` or `scripts/run_studio.sh`
4. Document expected baselines
5. Ensure reproducibility with fixed seeds

---

**Last Updated:** 2026-01-24  
**Version:** 1.0.0  
**Maintainer:** HEKTOR Engineering Team
