#!/bin/bash

# Exit on errors
set -e

# Define paths
BUILD_DIR="tests/csrc/build"
SOURCE_DIR="tests/csrc"

# Clean the build directory if it exists, then recreate it
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning the previous build directory..."
    rm -rf $BUILD_DIR
fi
mkdir -p $BUILD_DIR

# Run cmake to configure the project, pointing to the source directory
echo "Configuring the project with CMake..."
cmake -S $SOURCE_DIR -B $BUILD_DIR

# Build the project using cmake
echo "Building the project with CMake..."
cmake --build $BUILD_DIR --target run_tests

# Run the compiled tests
echo "Running the tests..."
$BUILD_DIR/bin/run_tests

