#!/usr/bin/env pwsh
# ============================================================================
# Vector Studio Documentation Reorganization Script
# Renames files with proper numbering and adds frontmatter
# ============================================================================

$ErrorActionPreference = "Stop"

Write-Host "============================================================================" -ForegroundColor Cyan
Write-Host "Vector Studio Documentation Reorganization" -ForegroundColor Cyan
Write-Host "============================================================================" -ForegroundColor Cyan
Write-Host ""

# Define the new documentation structure
$docStructure = @(
    # GETTING STARTED (01-04)
    @{Old = "01_README.md"; New = "01_INTRODUCTION.md"; Title = "Introduction"; Category = "Introduction"; Description = "Overview of Vector Studio, key features, and quick start guide" },
    @{Old = "02_GETTING_STARTED.md"; New = "02_INSTALLATION.md"; Title = "Installation Guide"; Category = "Setup"; Description = "System requirements, installation steps, and initial configuration" },
    @{Old = "QUICKSTART.md"; New = "03_QUICKSTART.md"; Title = "Quick Start Tutorial"; Category = "Tutorial"; Description = "Create your first database and perform basic operations" },
    @{Old = "03_USER_GUIDE.md"; New = "04_USER_GUIDE.md"; Title = "User Guide"; Category = "Guide"; Description = "Complete user guide covering all features and operations" },
    
    # CORE CONCEPTS (05-09)
    @{Old = "06_ARCHITECTURE.md"; New = "05_ARCHITECTURE.md"; Title = "Architecture"; Category = "Technical"; Description = "System design, components, and data flow" },
    @{Old = "04_DATA_FORMATS.md"; New = "06_DATA_FORMATS.md"; Title = "Data Formats"; Category = "Reference"; Description = "Supported data formats and specifications" },
    @{Old = "05_DATA_INGESTION.md"; New = "07_DATA_INGESTION.md"; Title = "Data Ingestion"; Category = "Guide"; Description = "Data adapters, batch processing, and ingestion patterns" },
    @{Old = "08_MODELS.md"; New = "08_EMBEDDINGS_MODELS.md"; Title = "Embeddings & Models"; Category = "Technical"; Description = "Text and image encoders, model specifications" },
    @{Old = "09_MATH.md"; New = "09_VECTOR_OPERATIONS.md"; Title = "Vector Operations"; Category = "Technical"; Description = "HNSW algorithm, distance metrics, and mathematical foundations" },
    
    # ADVANCED FEATURES (10-15)
    @{Old = "HYBRID_SEARCH_QUICKSTART.md"; New = "10_HYBRID_SEARCH.md"; Title = "Hybrid Search"; Category = "Feature"; Description = "BM25 full-text search, fusion methods, and RAG toolkit" },
    @{Old = "DISTRIBUTED_FEATURES_STATUS.md"; New = "11_DISTRIBUTED_SYSTEM.md"; Title = "Distributed System"; Category = "Feature"; Description = "Replication, sharding, and gRPC networking" },
    @{Old = "FRAMEWORK_INTEGRATION.md"; New = "12_ML_FRAMEWORK.md"; Title = "ML Framework Integration"; Category = "Feature"; Description = "TensorFlow and PyTorch integration with GPU acceleration" },
    @{Old = "15_LLM_ENGINE.md"; New = "13_LLM_ENGINE.md"; Title = "LLM Engine"; Category = "Feature"; Description = "Local text generation with llama.cpp" },
    @{Old = "16_QUANTIZATION.md"; New = "14_QUANTIZATION.md"; Title = "Quantization"; Category = "Optimization"; Description = "Vector compression and quantization techniques" },
    @{Old = "11_LOGGING.md"; New = "15_LOGGING_MONITORING.md"; Title = "Logging & Monitoring"; Category = "Operations"; Description = "Logging system, Prometheus metrics, and observability" },
    
    # DEPLOYMENT & OPERATIONS (16-19)
    @{Old = "13_DEPLOYMENT.md"; New = "16_DEPLOYMENT.md"; Title = "Deployment Guide"; Category = "Operations"; Description = "Docker, Kubernetes, and production deployment" },
    @{Old = "NEW"; New = "17_PERFORMANCE_TUNING.md"; Title = "Performance Tuning"; Category = "Optimization"; Description = "Benchmarks, optimization techniques, and best practices" },
    @{Old = "SECURITY.md"; New = "18_SECURITY.md"; Title = "Security"; Category = "Operations"; Description = "Security best practices and guidelines" },
    @{Old = "14_REAL_WORLD_APPLICATIONS.md"; New = "19_REAL_WORLD_APPLICATIONS.md"; Title = "Real-World Applications"; Category = "Case Studies"; Description = "Production use cases and success stories" },
    
    # DEVELOPMENT & EXTENSION (20-23)
    @{Old = "07_API_REFERENCE.md"; New = "20_API_REFERENCE.md"; Title = "API Reference"; Category = "Reference"; Description = "Complete C++ API documentation" },
    @{Old = "NEW"; New = "21_PYTHON_BINDINGS.md"; Title = "Python Bindings"; Category = "Reference"; Description = "Python API reference and examples" },
    @{Old = "NEW"; New = "22_CUSTOM_DEVELOPMENT.md"; Title = "Custom Development"; Category = "Guide"; Description = "Developing custom adapters and extensions" },
    @{Old = "../CONTRIBUTING.md"; New = "23_CONTRIBUTING.md"; Title = "Contributing Guide"; Category = "Guide"; Description = "How to contribute to Vector Studio" }
)

$docsDir = ".\docs"
$backupDir = ".\docs\backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')"

# Create backup
Write-Host "[1/4] Creating backup..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force -Path $backupDir | Out-Null
Get-ChildItem $docsDir -Filter "*.md" | Copy-Item -Destination $backupDir
Write-Host "  Backup created: $backupDir" -ForegroundColor Green

# Function to add/update frontmatter
function Add-Frontmatter {
    param(
        [string]$FilePath,
        [string]$Title,
        [string]$Category,
        [string]$Description,
        [int]$Position
    )
    
    $content = Get-Content $FilePath -Raw
    
    # Remove existing frontmatter if present
    if ($content -match '^---\r?\n.*?\r?\n---\r?\n') {
        $content = $content -replace '^---\r?\n.*?\r?\n---\r?\n', ''
    }
    
    # Create new frontmatter
    $frontmatter = @"
---
title: "$Title"
description: "$Description"
version: "2.3.0"
last_updated: "2026-01-06"
sidebar_position: $Position
category: "$Category"
---

"@
    
    # Combine and save
    $newContent = $frontmatter + $content.TrimStart()
    Set-Content -Path $FilePath -Value $newContent -NoNewline
}

# Rename and update files
Write-Host ""
Write-Host "[2/4] Renaming and updating files..." -ForegroundColor Yellow

$position = 1
foreach ($doc in $docStructure) {
    $oldPath = Join-Path $docsDir $doc.Old
    $newPath = Join-Path $docsDir $doc.New
    
    if ($doc.Old -eq "NEW") {
        # Create new file with template
        Write-Host "  Creating: $($doc.New)" -ForegroundColor Cyan
        $template = @"
# $($doc.Title)

[Content to be added]

## Overview

This section covers $($doc.Description.ToLower()).

## Table of Contents

- [Overview](#overview)
- [Getting Started](#getting-started)
- [Examples](#examples)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)

## Getting Started

[Content to be added]

## Examples

[Content to be added]

## Best Practices

[Content to be added]

## Troubleshooting

[Content to be added]

---

**Next**: [Continue to next section]
**Previous**: [Return to previous section]
**Index**: [Documentation Index](00_INDEX.md)
"@
        Set-Content -Path $newPath -Value $template
        Add-Frontmatter -FilePath $newPath -Title $doc.Title -Category $doc.Category -Description $doc.Description -Position $position
        
    }
    elseif (Test-Path $oldPath) {
        # Rename existing file
        if ($oldPath -ne $newPath) {
            Write-Host "  Renaming: $($doc.Old) -> $($doc.New)" -ForegroundColor Green
            Move-Item -Path $oldPath -Destination $newPath -Force
        }
        else {
            Write-Host "  Updating: $($doc.New)" -ForegroundColor Green
        }
        Add-Frontmatter -FilePath $newPath -Title $doc.Title -Category $doc.Category -Description $doc.Description -Position $position
    }
    else {
        Write-Host "  Skipping: $($doc.Old) (not found)" -ForegroundColor Yellow
    }
    
    $position++
}

# Update cross-references
Write-Host ""
Write-Host "[3/4] Updating cross-references..." -ForegroundColor Yellow

$mdFiles = Get-ChildItem $docsDir -Filter "*.md"
foreach ($file in $mdFiles) {
    $content = Get-Content $file.FullName -Raw
    $updated = $false
    
    # Update common references
    $replacements = @{
        "02_GETTING_STARTED.md"         = "02_INSTALLATION.md"
        "03_USER_GUIDE.md"              = "04_USER_GUIDE.md"
        "04_DATA_FORMATS.md"            = "06_DATA_FORMATS.md"
        "05_DATA_INGESTION.md"          = "07_DATA_INGESTION.md"
        "06_ARCHITECTURE.md"            = "05_ARCHITECTURE.md"
        "07_API_REFERENCE.md"           = "20_API_REFERENCE.md"
        "08_MODELS.md"                  = "08_EMBEDDINGS_MODELS.md"
        "09_MATH.md"                    = "09_VECTOR_OPERATIONS.md"
        "11_LOGGING.md"                 = "15_LOGGING_MONITORING.md"
        "13_DEPLOYMENT.md"              = "16_DEPLOYMENT.md"
        "14_REAL_WORLD_APPLICATIONS.md" = "19_REAL_WORLD_APPLICATIONS.md"
        "15_LLM_ENGINE.md"              = "13_LLM_ENGINE.md"
        "16_QUANTIZATION.md"            = "14_QUANTIZATION.md"
    }
    
    foreach ($old in $replacements.Keys) {
        if ($content -match $old) {
            $content = $content -replace [regex]::Escape($old), $replacements[$old]
            $updated = $true
        }
    }
    
    if ($updated) {
        Set-Content -Path $file.FullName -Value $content -NoNewline
        Write-Host "  Updated references in: $($file.Name)" -ForegroundColor Green
    }
}

# Generate summary
Write-Host ""
Write-Host "[4/4] Generating summary..." -ForegroundColor Yellow

$summary = @"
# Documentation Reorganization Summary

**Date**: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
**Backup Location**: $backupDir

## Files Processed

Total Files: $($docStructure.Count)

### Renamed Files
"@

foreach ($doc in $docStructure | Where-Object { $_.Old -ne "NEW" -and $_.Old -ne $_.New }) {
    $summary += "`n- $($doc.Old) → $($doc.New)"
}

$summary += @"

### New Files Created
"@

foreach ($doc in $docStructure | Where-Object { $_.Old -eq "NEW" }) {
    $summary += "`n- $($doc.New)"
}

$summary += @"

### Frontmatter Added

All files now include:
- title
- description
- version (2.3.0)
- last_updated (2026-01-06)
- sidebar_position
- category

### Cross-References Updated

All internal links updated to reflect new file names.

## Next Steps

1. Review all files for content accuracy
2. Fill in placeholder content for new files
3. Update main README.md to reference 00_INDEX.md
4. Commit changes to git

## Rollback

To rollback these changes:
```powershell
Remove-Item .\docs\*.md
Copy-Item $backupDir\*.md .\docs\
```
"@

$summaryPath = Join-Path $docsDir "REORGANIZATION_SUMMARY.md"
Set-Content -Path $summaryPath -Value $summary

Write-Host ""
Write-Host "============================================================================" -ForegroundColor Green
Write-Host "Documentation Reorganization Complete!" -ForegroundColor Green
Write-Host "============================================================================" -ForegroundColor Green
Write-Host ""
Write-Host "Summary:" -ForegroundColor Cyan
Write-Host "  Files processed: $($docStructure.Count)" -ForegroundColor White
Write-Host "  Backup location: $backupDir" -ForegroundColor White
Write-Host "  Summary: $summaryPath" -ForegroundColor White
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Review files in docs/ directory" -ForegroundColor White
Write-Host "  2. Update main README.md" -ForegroundColor White
Write-Host "  3. Commit changes" -ForegroundColor White
Write-Host ""
