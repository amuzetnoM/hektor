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
git tag -a v4.0.0 -m "Release v4.0.0 - Billion-scale Performance & Perceptual Quantization

Major Release - Hektor Vector Studio v4.0.0

## Highlights
- Billion-scale verified (1B+ vectors) with improved perceptual quantization and display-aware PQ profiles
- Comprehensive system snapshot and exhaustive analysis: docs/research/HEKTOR_ANALYSIS.md (v4.0.0)
- Observability: OpenTelemetry distributed tracing + eBPF integration and dashboards
- Packaging & Automation: improved build scripts and Docker images
- Documentation refresh and release draft at docs/release_draft/v4.0.0-draft.md

## Migration
- Backward compatible; no breaking API changes expected

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
echo "  2. Attach release notes from docs/changelog/v4.0.0.md"
echo "  3. Announce release to community"
echo ""
echo "Tag information:"
git show v4.0.0 --no-patch
echo ""
