# Vector Studio v2.3.0 - ML Framework Setup Status

## 🔄 Installation Progress

### Current Status: IN PROGRESS ⏳

**Started**: 2026-01-06 20:18  
**Estimated Completion**: 30-60 minutes  
**Current Package**: c-ares (4/11)

### vcpkg Packages Being Installed

1. ✅ vcpkg-cmake
2. ✅ vcpkg-cmake-config  
3. ✅ abseil
4. 🔄 c-ares (IN PROGRESS)
5. ⏳ grpc
6. ⏳ protobuf
7. ⏳ re2
8. ⏳ openssl
9. ⏳ zlib
10. ⏳ utf8-range
11. ⏳ prometheus-cpp

### Large Downloads Pending

- **TensorFlow C API**: ~150MB (v2.15.0)
- **LibTorch (PyTorch C++)**: ~2GB (v2.1.2 CPU or CUDA)
- **Python ML packages**: ~1GB total

---

## 📦 What's Being Installed

### 1. C++ Dependencies (vcpkg)

#### gRPC Stack
- **grpc**: High-performance RPC framework
- **protobuf**: Protocol Buffers for serialization
- **abseil**: Google's C++ common libraries
- **c-ares**: Asynchronous DNS resolver
- **re2**: Regular expression library

#### Supporting Libraries
- **openssl**: TLS/SSL support
- **zlib**: Compression
- **curl**: HTTP client
- **sqlite3**: Database
- **libpq**: PostgreSQL client
- **prometheus-cpp**: Metrics export

### 2. TensorFlow C API

**Version**: 2.15.0  
**Size**: ~150MB  
**Install Location**: `C:\Program Files\TensorFlow`

**Features**:
- SavedModel loading
- Frozen graph support
- TensorFlow Lite
- CPU inference (GPU version available separately)

**Environment Variables**:
- `TENSORFLOW_ROOT`: C:\Program Files\TensorFlow
- Added to PATH: C:\Program Files\TensorFlow\lib

### 3. LibTorch (PyTorch C++)

**Version**: 2.1.2  
**Size**: ~2GB (CPU) or ~2.5GB (CUDA 12.1)  
**Install Location**: `C:\Program Files\LibTorch`

**Auto-Detection**:
- Checks for NVIDIA GPU (nvidia-smi)
- Installs CUDA version if GPU detected
- Falls back to CPU version otherwise

**Features**:
- TorchScript loading
- ONNX conversion
- GPU acceleration (if CUDA available)
- Mixed precision (FP16/FP32)

**Environment Variables**:
- `TORCH_ROOT`: C:\Program Files\LibTorch
- `Torch_DIR`: C:\Program Files\LibTorch\share\cmake\Torch
- Added to PATH: C:\Program Files\LibTorch\lib

### 4. Python ML Packages

**Packages Being Installed**:
```
tensorflow >= 2.15.0
torch >= 2.1.0
torchvision
transformers >= 4.35.0
sentence-transformers >= 2.2.0
onnx >= 1.15.0
onnxruntime >= 1.16.0
grpcio >= 1.60.0
grpcio-tools >= 1.60.0
protobuf >= 4.25.0
prometheus-client >= 0.19.0
```

---

## 🏗️ CMakeLists.txt Updates

### New Build Options Added

```cmake
option(VDB_USE_TENSORFLOW "Enable TensorFlow C++ API" OFF)
option(VDB_USE_PYTORCH "Enable PyTorch C++ API (LibTorch)" OFF)
option(VDB_USE_GRPC "Enable gRPC for distributed networking" OFF)
option(VDB_BUILD_DISTRIBUTED "Build distributed features" OFF)
option(VDB_USE_PROMETHEUS "Enable Prometheus metrics export" OFF)
```

### Dependency Detection

✅ **gRPC Detection**:
- Finds gRPC and Protobuf via CONFIG mode
- Locates grpc_cpp_plugin for code generation
- Auto-disables if not found

✅ **Prometheus Detection**:
- Finds prometheus-cpp via CONFIG mode
- Gracefully disables if not available

✅ **TensorFlow Detection**:
- Custom FindTensorFlow.cmake module
- Checks TENSORFLOW_ROOT environment variable
- Links TensorFlow::TensorFlow target

✅ **PyTorch Detection**:
- Uses Torch CONFIG package
- Checks Torch_DIR and TORCH_ROOT
- Links Torch::Torch target

---

## 🚀 Build Commands

### Full ML Framework Build

```bash
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DVDB_USE_TENSORFLOW=ON \
  -DVDB_USE_PYTORCH=ON \
  -DVDB_USE_GRPC=ON \
  -DVDB_BUILD_DISTRIBUTED=ON \
  -DVDB_USE_PROMETHEUS=ON \
  -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake

cmake --build build -j
```

### Minimal Build (No ML)

```bash
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake

cmake --build build -j
```

### Python Only Build

```bash
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DVDB_BUILD_PYTHON=ON \
  -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake

cmake --build build -j
cd build && pip install .
```

---

## 📋 Post-Installation Checklist

### After Script Completes

- [ ] **Restart Terminal**: Load new environment variables
- [ ] **Verify TensorFlow**: Check `$env:TENSORFLOW_ROOT`
- [ ] **Verify LibTorch**: Check `$env:TORCH_ROOT`
- [ ] **Test vcpkg**: Run `.\vcpkg\vcpkg.exe list`
- [ ] **Test Python**: Run `python -c "import tensorflow, torch; print('OK')"`

### Build Verification

- [ ] **Configure**: Run cmake configure command
- [ ] **Check Output**: Verify all packages found
- [ ] **Build**: Run cmake build
- [ ] **Test**: Run `ctest` if tests enabled
- [ ] **Install Python**: `cd build && pip install .`

---

## 🎯 Expected Results

### CMake Configure Output

```
-- gRPC found - Distributed networking enabled
  gRPC version: 1.71.0
  Protobuf version: 5.29.5
  gRPC C++ plugin: C:/workspace/vector_database/vcpkg/installed/x64-windows/tools/grpc/grpc_cpp_plugin.exe

-- Prometheus C++ found - Metrics export enabled

-- TensorFlow found - ML embeddings enabled
  Include: C:/Program Files/TensorFlow/include
  Library: C:/Program Files/TensorFlow/lib/tensorflow.lib

-- LibTorch found - PyTorch embeddings enabled
  Include: C:/Program Files/LibTorch/include
  Libraries: torch;torch_cpu;c10

VectorDB Configuration:
  Build Type:         Release
  C++ Standard:       23
  AVX2:               ON
  GPU Support:        OFF
  Python Bindings:    ON
  Tests:              ON

Optional Features:
  llama.cpp:          ON
  ONNX Runtime:       OFF
  TensorFlow:         ON
  PyTorch (LibTorch): ON
  gRPC:               ON
  Distributed:        ON
  Prometheus:         ON
```

### Build Success Indicators

- ✅ All source files compile without errors
- ✅ gRPC services generated from .proto files
- ✅ TensorFlow and PyTorch libraries linked
- ✅ Python module `pyvdb` builds successfully
- ✅ All tests pass (if enabled)

---

## 🔧 Troubleshooting

### If TensorFlow Not Found

```powershell
# Set environment variable
[Environment]::SetEnvironmentVariable("TENSORFLOW_ROOT", "C:\Program Files\TensorFlow", "User")

# Restart terminal and retry
```

### If LibTorch Not Found

```powershell
# Set environment variables
[Environment]::SetEnvironmentVariable("TORCH_ROOT", "C:\Program Files\LibTorch", "User")
[Environment]::SetEnvironmentVariable("Torch_DIR", "C:\Program Files\LibTorch\share\cmake\Torch", "User")

# Restart terminal and retry
```

### If gRPC Not Found

```powershell
# Verify vcpkg installation
.\vcpkg\vcpkg.exe list | Select-String "grpc"

# If not installed, run:
.\vcpkg\vcpkg.exe install grpc:x64-windows protobuf:x64-windows
```

### If Build Fails

1. Check CMake output for missing dependencies
2. Verify all environment variables are set
3. Ensure vcpkg toolchain file is specified
4. Try clean build: `rm -r build; cmake -B build ...`

---

## 📊 Disk Space Requirements

- **vcpkg packages**: ~2GB
- **TensorFlow C API**: ~150MB
- **LibTorch CPU**: ~2GB
- **LibTorch CUDA**: ~2.5GB
- **Python packages**: ~1GB
- **Build artifacts**: ~500MB

**Total**: ~5-6GB (CPU) or ~6-7GB (CUDA)

---

## ⏱️ Time Estimates

- **vcpkg installation**: 30-60 minutes
- **TensorFlow download**: 2-5 minutes
- **LibTorch download**: 10-20 minutes (2GB)
- **Python packages**: 5-10 minutes
- **CMake configure**: 1-2 minutes
- **Build (first time)**: 10-15 minutes
- **Build (incremental)**: 1-3 minutes

**Total First-Time Setup**: ~1-2 hours

---

## 🎉 Success Criteria

### Installation Complete When:

1. ✅ All vcpkg packages installed
2. ✅ TensorFlow C API extracted to Program Files
3. ✅ LibTorch extracted to Program Files
4. ✅ Python packages installed successfully
5. ✅ Environment variables set
6. ✅ CMake finds all dependencies
7. ✅ Build completes without errors
8. ✅ Tests pass (if enabled)
9. ✅ Python module imports successfully

### Verification Commands

```powershell
# Check installations
ls "C:\Program Files\TensorFlow"
ls "C:\Program Files\LibTorch"
.\vcpkg\vcpkg.exe list

# Check Python
python -c "import tensorflow as tf; print(f'TF: {tf.__version__}')"
python -c "import torch; print(f'PyTorch: {torch.__version__}')"
python -c "import grpc; print('gRPC: OK')"

# Check C++ build
cmake -B build -G Ninja -DVDB_USE_TENSORFLOW=ON -DVDB_USE_PYTORCH=ON -DVDB_USE_GRPC=ON -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build
```

---

**Status**: Installation in progress...  
**Next Update**: Check after 30 minutes or when script completes
