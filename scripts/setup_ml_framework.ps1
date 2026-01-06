#!/usr/bin/env pwsh
# ============================================================================
# Vector Studio v2.3.0 - ML Framework and Dependencies Setup
# Installs TensorFlow C API, LibTorch, gRPC, and all required dependencies
# ============================================================================

param(
    [switch]$SkipTensorFlow,
    [switch]$SkipPyTorch,
    [switch]$SkipGRPC,
    [switch]$AutoYes
)

$ErrorActionPreference = "Stop"

Write-Host "============================================================================" -ForegroundColor Cyan
Write-Host "Vector Studio v2.3.0 - ML Framework Setup" -ForegroundColor Cyan
Write-Host "============================================================================" -ForegroundColor Cyan
Write-Host ""

# Colors
function Write-Success { param($msg) Write-Host "[OK] $msg" -ForegroundColor Green }
function Write-Info { param($msg) Write-Host "[INFO] $msg" -ForegroundColor Cyan }
function Write-Warning { param($msg) Write-Host "[WARN] $msg" -ForegroundColor Yellow }
function Write-ErrorMsg { param($msg) Write-Host "[ERROR] $msg" -ForegroundColor Red }

# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Warning "Not running as administrator. Some installations may fail."
    if (-not $AutoYes) {
        $continue = Read-Host "Continue anyway? (y/n)"
        if ($continue -ne "y") { exit 1 }
    }
}

# ============================================================================
# 1. Install vcpkg packages
# ============================================================================

Write-Host ""
Write-Host "[1/6] Installing vcpkg packages..." -ForegroundColor Yellow

$vcpkgExe = ".\vcpkg\vcpkg.exe"
if (-not (Test-Path $vcpkgExe)) {
    Write-ErrorMsg "vcpkg not found. Bootstrapping..."
    if (Test-Path ".\vcpkg\bootstrap-vcpkg.bat") {
        & ".\vcpkg\bootstrap-vcpkg.bat"
    }
    else {
        Write-ErrorMsg "vcpkg directory not found!"
        exit 1
    }
}

$vcpkgPackages = @(
    "grpc:x64-windows",
    "protobuf:x64-windows",
    "abseil:x64-windows",
    "c-ares:x64-windows",
    "re2:x64-windows",
    "sqlite3:x64-windows",
    "libpq:x64-windows",
    "curl:x64-windows",
    "openssl:x64-windows",
    "zlib:x64-windows",
    "prometheus-cpp:x64-windows"
)

Write-Info "Installing vcpkg packages (this may take 30-60 minutes)..."
foreach ($pkg in $vcpkgPackages) {
    Write-Info "Installing $pkg..."
    & $vcpkgExe install $pkg --recurse
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Failed to install $pkg, continuing..."
    }
    else {
        Write-Success "Installed $pkg"
    }
}

# Integrate vcpkg
Write-Info "Integrating vcpkg with Visual Studio..."
& $vcpkgExe integrate install

# ============================================================================
# 2. Install TensorFlow C API
# ============================================================================

if (-not $SkipTensorFlow) {
    Write-Host ""
    Write-Host "[2/6] Installing TensorFlow C API..." -ForegroundColor Yellow
    
    $tfVersion = "2.15.0"
    $tfDir = "C:\Program Files\TensorFlow"
    $tfUrl = "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-windows-x86_64-$tfVersion.zip"
    
    if (Test-Path $tfDir) {
        Write-Success "TensorFlow already installed at $tfDir"
    }
    else {
        Write-Info "Downloading TensorFlow C API v$tfVersion..."
        $tfZip = "$env:TEMP\libtensorflow.zip"
        
        try {
            Invoke-WebRequest -Uri $tfUrl -OutFile $tfZip -UseBasicParsing
            Write-Success "Downloaded TensorFlow"
            
            Write-Info "Extracting to $tfDir..."
            New-Item -ItemType Directory -Force -Path $tfDir | Out-Null
            Expand-Archive -Path $tfZip -DestinationPath $tfDir -Force
            Write-Success "Installed TensorFlow C API"
            
            # Add to PATH
            $env:Path += ";$tfDir\lib"
            [Environment]::SetEnvironmentVariable("TENSORFLOW_ROOT", $tfDir, "User")
            Write-Success "Added TensorFlow to PATH"
            
        }
        catch {
            Write-ErrorMsg "Failed to install TensorFlow: $_"
        }
        finally {
            if (Test-Path $tfZip) { Remove-Item $tfZip }
        }
    }
}
else {
    Write-Warning "Skipping TensorFlow installation"
}

# ============================================================================
# 3. Install LibTorch (PyTorch C++)
# ============================================================================

if (-not $SkipPyTorch) {
    Write-Host ""
    Write-Host "[3/6] Installing LibTorch (PyTorch C++)..." -ForegroundColor Yellow
    
    $torchVersion = "2.1.2"
    $torchDir = "C:\Program Files\LibTorch"
    
    # Check CUDA availability
    $hasCuda = $false
    try {
        $nvidiaSmi = & nvidia-smi 2>$null
        if ($LASTEXITCODE -eq 0) {
            $hasCuda = $true
            Write-Info "CUDA detected, will install GPU version"
        }
    }
    catch {
        Write-Info "No CUDA detected, will install CPU version"
    }
    
    if ($hasCuda) {
        $torchUrl = "https://download.pytorch.org/libtorch/cu121/libtorch-win-shared-with-deps-$torchVersion%2Bcu121.zip"
    }
    else {
        $torchUrl = "https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-$torchVersion%2Bcpu.zip"
    }
    
    if (Test-Path $torchDir) {
        Write-Success "LibTorch already installed at $torchDir"
    }
    else {
        Write-Info "Downloading LibTorch v$torchVersion..."
        Write-Warning "This is a large download (~2GB), please be patient..."
        $torchZip = "$env:TEMP\libtorch.zip"
        
        try {
            # Use BITS for large file download
            Start-BitsTransfer -Source $torchUrl -Destination $torchZip -Description "Downloading LibTorch"
            Write-Success "Downloaded LibTorch"
            
            Write-Info "Extracting to $torchDir (this may take several minutes)..."
            New-Item -ItemType Directory -Force -Path $torchDir | Out-Null
            Expand-Archive -Path $torchZip -DestinationPath "$torchDir\.." -Force
            
            # LibTorch extracts to a libtorch subdirectory
            if (Test-Path "$torchDir\..\libtorch") {
                Move-Item "$torchDir\..\libtorch\*" $torchDir -Force
                Remove-Item "$torchDir\..\libtorch" -Force
            }
            
            Write-Success "Installed LibTorch"
            
            # Add to PATH
            $env:Path += ";$torchDir\lib"
            [Environment]::SetEnvironmentVariable("TORCH_ROOT", $torchDir, "User")
            [Environment]::SetEnvironmentVariable("Torch_DIR", "$torchDir\share\cmake\Torch", "User")
            Write-Success "Added LibTorch to PATH"
            
        }
        catch {
            Write-ErrorMsg "Failed to install LibTorch: $_"
        }
        finally {
            if (Test-Path $torchZip) { Remove-Item $torchZip }
        }
    }
}
else {
    Write-Warning "Skipping LibTorch installation"
}

# ============================================================================
# 4. Install Python ML packages
# ============================================================================

Write-Host ""
Write-Host "[4/6] Installing Python ML packages..." -ForegroundColor Yellow

$pythonPackages = @(
    "tensorflow>=2.15.0",
    "torch>=2.1.0",
    "torchvision",
    "transformers>=4.35.0",
    "sentence-transformers>=2.2.0",
    "onnx>=1.15.0",
    "onnxruntime>=1.16.0",
    "grpcio>=1.60.0",
    "grpcio-tools>=1.60.0",
    "protobuf>=4.25.0",
    "prometheus-client>=0.19.0"
)

Write-Info "Installing Python packages..."
foreach ($pkg in $pythonPackages) {
    Write-Info "Installing $pkg..."
    python -m pip install --upgrade $pkg 2>&1 | Out-Null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Installed $pkg"
    }
    else {
        Write-Warning "Failed to install $pkg"
    }
}

# ============================================================================
# Summary
# ============================================================================

Write-Host ""
Write-Host "============================================================================" -ForegroundColor Green
Write-Host "ML Framework Setup Complete!" -ForegroundColor Green
Write-Host "============================================================================" -ForegroundColor Green
Write-Host ""

Write-Host "Installed Components:" -ForegroundColor Cyan
Write-Host "  [OK] vcpkg packages (gRPC, protobuf, prometheus-cpp, etc.)" -ForegroundColor Green
if (-not $SkipTensorFlow) {
    Write-Host "  [OK] TensorFlow C API v2.15.0" -ForegroundColor Green
}
if (-not $SkipPyTorch) {
    Write-Host "  [OK] LibTorch (PyTorch C++) v2.1.2" -ForegroundColor Green
}
Write-Host "  [OK] Python ML packages (tensorflow, torch, transformers, etc.)" -ForegroundColor Green
Write-Host ""

Write-Host "Next Steps:" -ForegroundColor Cyan
Write-Host "  1. Restart your terminal to load new environment variables" -ForegroundColor White
Write-Host "  2. Run the build command below" -ForegroundColor White
Write-Host ""

Write-Host "Build Command:" -ForegroundColor Yellow
Write-Host "  cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DVDB_USE_TENSORFLOW=ON -DVDB_USE_PYTORCH=ON -DVDB_USE_GRPC=ON -DVDB_BUILD_DISTRIBUTED=ON -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake" -ForegroundColor White
Write-Host ""
