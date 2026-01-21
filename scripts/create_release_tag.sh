#!/bin/bash
#
# Create v4.0.0 Release Tag
#
# This script creates and pushes the v4.0.0 release tag for Hektor
#

set -e

echo "======================================================================"
echo "  Hektor v4.0.0 Release Tag Creation"
echo "======================================================================"
echo ""

# Check if we're in the hektor repository
if [ ! -f "CMakeLists.txt" ] || ! grep -q "project(VectorDB" CMakeLists.txt; then
    echo "❌ Error: This script must be run from the hektor repository root"
    exit 1
fi

# Check current branch
CURRENT_BRANCH=$(git branch --show-current)
echo "📌 Current branch: $CURRENT_BRANCH"

# Check if on the CLI implementation branch
if [ "$CURRENT_BRANCH" != "copilot/build-exhaustive-cli" ]; then
    echo "⚠️  Warning: Not on copilot/build-exhaustive-cli branch"
    echo "   Are you sure you want to create the tag from this branch?"
    read -p "   Continue? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Check for uncommitted changes
if ! git diff-index --quiet HEAD --; then
    echo "❌ Error: You have uncommitted changes. Please commit or stash them first."
    exit 1
fi

# Verify version in CMakeLists.txt
VERSION=$(grep "project(VectorDB VERSION" CMakeLists.txt | sed 's/.*VERSION \([0-9.]*\).*/\1/')
echo "📦 Version in CMakeLists.txt: $VERSION"

if [ "$VERSION" != "4.0.0" ]; then
    echo "❌ Error: Version in CMakeLists.txt is not 4.0.0"
    echo "   Found: $VERSION"
    exit 1
fi

echo ""
echo "Creating annotated tag v4.0.0..."
echo ""

# Create annotated tag with release notes
git tag -a v4.0.0 -m "Release v4.0.0 - Security Patch Release

Patch Release - Hektor Vector Database

## Overview

Hektor v4.0.0 is a patch release that includes important dependency updates to address security vulnerabilities. This release maintains full backward compatibility with v3.0.0.

## Changes

### Security Updates
- Dependency Update: Upgraded tar package from 7.5.2 to 7.5.3 in UI dependencies
  - Fixes vulnerability related to absolute linkpath sanitization
  - Addresses security advisory for tar package

## Technical Details
- Changed Files: 1 (ui/package-lock.json)
- Lines Changed: 8 (4 additions, 4 deletions)
- Scope: UI/Frontend dependencies only

## Migration Guide
- No breaking changes - This is a drop-in replacement for v3.0.0
- No action required - simply update to v4.0.0
- All v3.0.0 features and APIs remain unchanged

## Dependencies
- Updated: tar 7.5.2 → 7.5.3 (UI dependency)

## Installation

git clone https://github.com/amuzetnoM/hektor.git
cd hektor
git checkout v4.0.0
cmake -B build -G Ninja
cmake --build build

Full release notes: docs/changelog/v4.0.0.md
"

if [ $? -eq 0 ]; then
    echo "✅ Tag v4.0.0 created successfully"
else
    echo "❌ Failed to create tag"
    exit 1
fi

echo ""
echo "Pushing tag to origin..."
echo ""

# Push the tag to remote
git push origin v4.0.0

if [ $? -eq 0 ]; then
    echo "✅ Tag v4.0.0 pushed successfully"
else
    echo "❌ Failed to push tag"
    echo "   You can manually push with: git push origin v4.0.0"
    exit 1
fi

echo ""
echo "======================================================================"
echo "  ✅ Release v4.0.0 Tagged Successfully!"
echo "======================================================================"
echo ""
echo "Next steps:"
echo "  1. Create GitHub Release from tag v4.0.0"
echo "  2. Attach release notes from docs/releases/v4.0.0.md"
echo "  3. Announce release to community"
echo ""
echo "Tag information:"
git show v4.0.0 --no-patch
echo ""
