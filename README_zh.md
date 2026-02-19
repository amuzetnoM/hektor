# ⬜️ HEKTOR

> **向量工作室** <br>
> *高性能向量数据库引擎* <br>
> 语义搜索与AI训练平台

[English](README.md) | **中文**

高性能 C++ 向量数据库，支持 SIMD 优化的相似度搜索和本地 ONNX 嵌入推理。专为百万级向量亚毫秒查询而设计。

---

<p align="center">

[![Version](https://img.shields.io/badge/版本-4.0.0-blue?style=for-the-badge&logo=semver&logoColor=white)](#)
[![C++](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![ONNX](https://img.shields.io/badge/ONNX-Runtime-005CED?style=for-the-badge&logo=onnx&logoColor=white)](https://onnxruntime.ai/)
[![Python](https://img.shields.io/badge/Python-3.10--3.13-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://www.python.org/)
[![SIMD](https://img.shields.io/badge/SIMD-AVX2%2FAVX--512-FF6600?style=for-the-badge&logo=intel&logoColor=white)](#性能)
[![Latency](https://img.shields.io/badge/延迟-%3C3ms-blueviolet?style=for-the-badge)](#性能)
[![License](https://img.shields.io/badge/许可证-MIT-green?style=for-the-badge)](LICENSE)

</p>

---

## 为什么选择 HEKTOR？

| 对比项 | HEKTOR | Milvus | Qdrant | Weaviate |
|--------|--------|--------|--------|----------|
| **语言** | C++23 | Go/C++ | Rust | Go |
| **单机延迟** | <3ms | ~10ms | ~5ms | ~8ms |
| **混合搜索** | BM25 + 向量（5种融合算法） | 有限 | 稀疏向量 | BM25 |
| **依赖** | 零外部依赖 | Etcd + MinIO + Pulsar | 无 | 无 |
| **部署** | 单二进制 / pip install | 分布式集群 | 单节点/集群 | 单节点/集群 |
| **本地嵌入** | ONNX 内置 | 需外部服务 | 需外部服务 | 内置（有限） |
| **十亿级支持** | ✅ 96.8% 召回率 @ 8.5ms | ✅ | ✅ | ✅ |

**HEKTOR 适合：** 边缘计算、机器人本地推理、嵌入式AI、需要轻量高性能向量检索的场景。

---

## 核心特性

- 🚀 **SIMD 加速** — AVX2/AVX-512 优化的余弦相似度和欧氏距离计算
- 🔍 **HNSW 索引** — O(log n) 近似最近邻搜索
- 🔀 **混合搜索** — BM25 全文检索 + 向量搜索，支持 RRF、加权求和、CombSUM、CombMNZ、Borda 五种融合算法
- 🌐 **分布式系统** — 异步/同步/半同步复制，哈希/范围/一致性哈希分片，gRPC 网络
- 🤖 **ML框架集成** — TensorFlow C++ API 和 PyTorch (LibTorch)，GPU 加速
- 📝 **本地嵌入** — 内置 ONNX Runtime 推理，无需 API 调用
- 🖼️ **跨模态搜索** — 文本与图像统一 512 维空间
- 💾 **内存映射存储** — mmap 零拷贝向量访问
- 📊 **RAG 引擎** — 完整 RAG 流水线，5种分块策略，LangChain/LlamaIndex 适配器
- 📈 **可观测性** — Prometheus 指标、OpenTelemetry 分布式追踪、eBPF 剖析
- 🐍 **Python 绑定** — 基于 pybind11 的高级 Python API

---

## 快速开始

### 通过 pip 安装（最简单）

```bash
# 基础安装
pip install hektor-vdb

# 包含 ML 依赖
pip install hektor-vdb[ml]
```

> 💡 国内用户可使用清华源加速：`pip install hektor-vdb -i https://pypi.tuna.tsinghua.edu.cn/simple`

### 从源码构建

```bash
git clone https://github.com/amuzetnoM/hektor.git
cd hektor

# 自动构建（推荐）
./build-hektor.sh

# 或手动构建
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

**系统要求：**
- Python 3.10+
- CMake 3.20+
- C++23 编译器（GCC 13+、Clang 16+、MSVC 2022 17.3+）

### 第一个向量数据库

```python
import pyvdb

# 创建数据库
db = pyvdb.create_database("./my_vectors")

# 添加文档
doc_id = db.add_text("HEKTOR 是一个高性能向量数据库", {
    "type": "技术文档",
    "date": "2026-02-19",
    "language": "zh"
})

# 语义搜索
results = db.search("向量检索性能", k=5)
for r in results:
    print(f"{r.score:.4f}: {r.text[:50]}")
```

---

## 架构

```
+-----------------------------------------------------------------------------+
|                              HEKTOR 向量工作室                                |
+-----------------------------------------------------------------------------+
|                                                                             |
|  +---------------+   +---------------+   +--------------------------+       |
|  |  文本编码器    |   |  图像编码器    |   |      数据摄取引擎         |       |
|  |  MiniLM-L6-v2 |   |  CLIP ViT-B32 |   |  JSON | CSV | PDF | SQL  |       |
|  +-------+-------+   +-------+-------+   +------------+-----------+         |
|          |                   |                        |                     |
|          +--------+----------+-----------+------------+                     |
|                   |                                                         |
|          +--------v--------+                                                |
|          |   维度投影层     |   384维 → 512维 统一空间                        |
|          +--------+--------+                                                |
|                   |                                                         |
|  +----------------v----------------+   +--------------------------+         |
|  |          HNSW 索引              |   |      元数据存储           |         |
|  |  M=16, ef_construction=200      |   |   JSONL，支持过滤         |         |
|  +----------------+----------------+   +------------+-------------+         |
|                   |                                 |                       |
|  +----------------v---------------------------------v-------------+         |
|  |                    内存映射存储层                                |         |
|  |          vectors.bin | index.hnsw | metadata.jsonl             |         |
|  +----------------------------------------------------------------+         |
+-----------------------------------------------------------------------------+
```

---

## 性能

### 基准测试

*Intel Core i7-12700H，32GB 内存，NVMe SSD*

| 操作 | 数据规模 | 耗时 | 吞吐量 |
|------|---------|------|--------|
| 添加文本 | 1 条文档 | 8 ms | 125/秒 |
| 添加图像 | 1 张图片 | 55 ms | 18/秒 |
| 搜索 (k=10) | 10万向量 | 2.1 ms | 476 QPS |
| 搜索 (k=10) | 100万向量 | 2.9 ms | 345 QPS |
| 搜索 (k=10) | 1000万向量 | 4.3 ms | 233 QPS |
| 搜索 (k=10) | 1亿向量 | 6.8 ms | 147 QPS |
| 批量导入 | 1000 文档 | 12 秒 | 83/秒 |

### 十亿级性能

| 指标 | 数值 | 配置 |
|------|------|------|
| **总向量数** | **10亿** | 10节点集群 |
| **查询延迟 (p99)** | **8.5 ms** | 分片模式 |
| **Recall@10** | **96.8%** | 大规模保持 |
| **吞吐量** | **85,000 QPS** | 分布式 |

### 内存占用

| 组件 | 每向量大小 |
|------|-----------|
| 向量存储 (512维 float32) | 2,048 字节 |
| HNSW 索引 | ~200 字节 |
| 元数据（平均） | ~100 字节 |
| **总计** | **~2.4 KB** |

---

## 混合搜索

HEKTOR 独特的混合搜索将 BM25 全文检索与向量语义搜索结合，提供5种融合算法：

```python
import pyvdb

db = pyvdb.create_database("./hybrid_db")

# 添加文档
db.add_text("深度学习在自然语言处理中的应用")
db.add_text("Transformer 架构改变了 NLP 领域")
db.add_text("BERT 和 GPT 是两种主流预训练模型")

# 混合搜索 — 同时利用关键词匹配和语义理解
results = db.hybrid_search(
    query="预训练语言模型",
    k=5,
    fusion="rrf",        # RRF 融合（推荐）
    bm25_weight=0.4,     # BM25 权重
    vector_weight=0.6    # 向量权重
)
```

**融合算法对比：**

| 算法 | 适用场景 | 优势 |
|------|---------|------|
| **RRF** | 通用（推荐默认） | 对分数尺度不敏感 |
| **加权求和** | 已知最优权重 | 可调节精确控制 |
| **CombSUM** | 多路召回 | 多信号聚合 |
| **CombMNZ** | 高精度需求 | 奖励多路命中 |
| **Borda** | 排序公平性 | 基于排名，忽略分数差异 |

---

## 分布式部署

```python
from pyvdb.distributed import ClusterConfig, ShardingStrategy

config = ClusterConfig(
    nodes=["node1:9090", "node2:9090", "node3:9090"],
    replication_mode="semi-sync",      # 半同步复制
    sharding=ShardingStrategy.CONSISTENT_HASH,
    replication_factor=2
)

cluster = pyvdb.create_cluster(config)
```

### 支持的部署模式

| 模式 | 描述 | 适用场景 |
|------|------|---------|
| **单机** | 单进程，单文件 | 开发、嵌入式、边缘设备 |
| **主从复制** | 一主多从，支持异步/同步/半同步 | 高可用读场景 |
| **分片集群** | 数据水平分片，多节点 | 十亿级数据 |

---

## 机器人与边缘计算

HEKTOR 专为资源受限的边缘环境优化：

```python
# 轻量配置 — 适合机器人/嵌入式设备
config = pyvdb.DatabaseConfig()
config.dimension = 384           # 使用较小维度节省内存
config.max_elements = 100_000    # 限制容量
config.hnsw_m = 8                # 减少连接数
config.hnsw_ef_search = 30       # 降低搜索精度换取速度

db = pyvdb.create_database("./robot_memory", config)

# 机器人场景：存储环境观测
db.add_text("厨房桌子上有两个红色苹果", {"location": "kitchen", "timestamp": "2026-02-19T15:00:00"})
db.add_text("客厅沙发旁边有一个遥控器", {"location": "living_room", "timestamp": "2026-02-19T15:01:00"})

# 查询：我在哪里见过苹果？
results = db.search("苹果在什么位置", k=3)
```

**边缘设备性能参考：**

| 设备 | 搜索延迟 (10万向量) | 内存占用 |
|------|---------------------|---------|
| x86 服务器 (AVX-512) | 2.1 ms | ~240 MB |
| ARM 开发板 (树莓派5) | ~15 ms | ~240 MB |
| NVIDIA Jetson | ~8 ms | ~240 MB |

---

## 嵌入模型

| 模型 | 类型 | 维度 | 大小 | 延迟 (CPU) |
|------|------|------|------|-----------|
| all-MiniLM-L6-v2 | 文本 | 384 | 23 MB | ~5 ms |
| CLIP ViT-B/32 | 图像 | 512 | 340 MB | ~50 ms |

```bash
# 下载模型
python scripts/download_models.py
```

---

## 配置

```python
config = pyvdb.DatabaseConfig()
config.dimension = 512                        # 向量维度
config.metric = pyvdb.DistanceMetric.Cosine   # 距离函数
config.hnsw_m = 16                            # HNSW 每节点连接数
config.hnsw_ef_construction = 200             # 构建质量
config.hnsw_ef_search = 50                    # 搜索质量
config.max_elements = 1_000_000               # 最大容量
```

| 环境变量 | 说明 | 默认值 |
|---------|------|--------|
| `VDB_MODELS_DIR` | ONNX 模型目录 | `~/.cache/vector_studio/models` |
| `VDB_LOG_LEVEL` | 日志级别 | `INFO` |
| `VDB_NUM_THREADS` | 线程池大小（0=自动） | `0` |
| `VDB_SIMD` | SIMD 级别 | `auto` |

---

## 文档

完整文档位于 `docs/` 目录（英文）：

| 文档 | 说明 |
|------|------|
| [安装指南](docs/02_INSTALLATION.md) | 系统要求、安装步骤 |
| [快速入门](docs/03_QUICKSTART.md) | Docker/Kubernetes 部署 |
| [用户手册](docs/04_USER_GUIDE.md) | 完整功能指南 |
| [架构设计](docs/05_ARCHITECTURE.md) | 系统设计与组件图 |
| [混合搜索](docs/10_HYBRID_SEARCH.md) | BM25 + 向量融合详解 |
| [分布式系统](docs/11_DISTRIBUTED_SYSTEM.md) | 复制、分片、gRPC |
| [ML框架集成](docs/12_ML_FRAMEWORK.md) | TensorFlow/PyTorch C++ |
| [性能调优](docs/17_PERFORMANCE_TUNING.md) | 基准测试与优化 |
| [API 参考](docs/20_API_REFERENCE.md) | 完整 C++ API 文档 |
| [Python 绑定](docs/21_PYTHON_BINDINGS.md) | Python API 参考 |

---

## 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解如何参与开发。

```bash
# 克隆仓库
git clone https://github.com/amuzetnoM/hektor.git
cd hektor

# 构建开发版本
./build-hektor.sh --dev --test

# 运行测试
cd build && ctest --output-on-failure
pytest tests/ -v
```

---

## 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。

---

## 致谢

- [HNSW 论文](https://arxiv.org/abs/1603.09320) — Malkov 和 Yashunin
- [Sentence-Transformers](https://www.sbert.net/) — MiniLM 模型
- [OpenAI CLIP](https://openai.com/research/clip) — 视觉-语言模型
- [ONNX Runtime](https://onnxruntime.ai/) — 跨平台推理引擎

---

<p align="center">
<b>HEKTOR</b> — 为 AI 代理、机器人和边缘计算构建的向量搜索引擎<br>
<a href="https://github.com/amuzetnoM/hektor">GitHub</a> · <a href="https://pypi.org/project/hektor-vdb/">PyPI</a> · <a href="https://artifactvirtual.com">Artifact Virtual</a>
</p>
