#!/bin/bash

# Bas Veeg Arc 3D - Build Script for Linux
# Usage: ./build.sh [clean|debug|release]

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
BUILD_TYPE="Release"

# Parse arguments
if [ "$1" == "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "Clean complete!"
    exit 0
elif [ "$1" == "debug" ]; then
    BUILD_TYPE="Debug"
elif [ "$1" == "release" ]; then
    BUILD_TYPE="Release"
fi

echo "================================"
echo "Bas Veeg Arc 3D Build Script"
echo "================================"
echo "Build Type: $BUILD_TYPE"
echo "Project Dir: $PROJECT_DIR"
echo "Build Dir: $BUILD_DIR"
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Check for dependencies
echo "Checking dependencies..."
MISSING_DEPS=0

if ! pkg-config --exists OGRE; then
    echo "  [MISSING] Ogre3D not found!"
    MISSING_DEPS=1
else
    echo "  [OK] Ogre3D found: $(pkg-config --modversion OGRE)"
fi

if ! pkg-config --exists bullet; then
    echo "  [MISSING] Bullet Physics not found!"
    MISSING_DEPS=1
else
    echo "  [OK] Bullet Physics found"
fi

if ! pkg-config --exists openal; then
    echo "  [MISSING] OpenAL not found!"
    MISSING_DEPS=1
else
    echo "  [OK] OpenAL found"
fi

if [ $MISSING_DEPS -eq 1 ]; then
    echo ""
    echo "ERROR: Missing dependencies! Please install them first."
    echo "On Gentoo: emerge -av dev-games/ogre sci-physics/bullet media-libs/openal net-libs/enet"
    exit 1
fi

echo ""
echo "All dependencies found!"
echo ""

# Configure with CMake
echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE "$PROJECT_DIR"

# Build
echo ""
echo "Building project..."
CORES=$(nproc)
echo "Using $CORES cores..."
make -j$CORES

echo ""
echo "================================"
echo "Build complete!"
echo "================================"
echo "Binary location: $BUILD_DIR/bin/BasVeegArc3D"
echo ""
echo "To run the game:"
echo "  cd $BUILD_DIR"
echo "  ./bin/BasVeegArc3D"
echo ""
