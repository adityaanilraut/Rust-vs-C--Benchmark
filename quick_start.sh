#!/bin/bash
# Quick Start Script for Rust vs C++ Benchmark Suite

set -e

echo "=================================="
echo "Rust vs C++ Benchmark Quick Start"
echo "=================================="
echo ""

# Check prerequisites
echo "Checking prerequisites..."

# Check Rust
if ! command -v rustc &> /dev/null; then
    echo "❌ Rust not found. Installing..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source ~/.cargo/env
else
    echo "✅ Rust: $(rustc --version)"
fi

# Check CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found."
    echo "Please install CMake: brew install cmake"
    exit 1
else
    echo "✅ CMake: $(cmake --version | head -n1)"
fi

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "❌ Python 3 not found. Please install Python 3."
    exit 1
else
    echo "✅ Python: $(python3 --version)"
fi

# Install Python dependencies
echo ""
echo "Installing Python dependencies..."
pip3 install -q matplotlib numpy 2>/dev/null || pip3 install --user matplotlib numpy

echo ""
echo "✅ All prerequisites satisfied!"
echo ""
echo "Starting benchmark suite..."
echo ""

# Run benchmarks
python3 build.py

echo ""
echo "=================================="
echo "✅ Benchmark Complete!"
echo "=================================="

