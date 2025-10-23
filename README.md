# Rust vs C++ Benchmark Suite

A comprehensive performance comparison between Rust and C++ on Apple Silicon, featuring 10 different benchmark tests across parallelization, graphics, heavy compute, and other domains.

## 🎯 Overview

This project benchmarks Rust and C++ implementations of identical algorithms, optimized for Apple Silicon (M-series chips). Each benchmark is run multiple times, and results are visualized with detailed performance graphs.

## 📊 Benchmark Categories

### Parallelization (3 tests)
1. **Parallel Matrix Multiplication** - 1024×1024 matrix multiplication using multi-threading
2. **Parallel QuickSort** - Sorting 10M integers with parallel quicksort
3. **Thread Pool Task Distribution** - 100K tasks distributed across worker threads

### Graphics (3 tests)
4. **CPU Ray Tracer** - Software ray tracing at 1920×1080 with 4 samples per pixel
5. **Mandelbrot Set Generator** - 4096×4096 Mandelbrot set computation with SIMD
6. **GPU Compute (Metal)** - Metal compute shader processing 10M elements

### Heavy Compute (3 tests)
7. **Prime Number Sieve** - Sieve of Eratosthenes up to 100M
8. **FFT Computation** - Fast Fourier Transform on 16M samples (2^24)
9. **SHA-256 Hashing** - Cryptographic hashing of 100MB data

### Other (1 test)
10. **JSON Parsing & Serialization** - Parse and serialize 100K JSON records

## 🚀 Quick Start

### Prerequisites

- **Rust** (latest stable): `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
- **C++ compiler** (Clang/LLVM with C++20 support)
- **CMake** (3.20+): `brew install cmake`
- **Python 3** with matplotlib: `pip3 install matplotlib numpy`
- **macOS** with Apple Silicon (M1/M2/M3/M4)

### Installation

```bash
# Clone or download the project
cd tinker

# The build script will handle everything
python3 build.py
```

## 📈 Running Benchmarks

Simply run:

```bash
python3 build.py
```

This will:
1. Build all Rust benchmarks with release optimizations
2. Build all C++ benchmarks with CMake
3. Run each benchmark 5 times for statistical accuracy
4. Generate performance comparison graphs
5. Save results to `results/` directory

## 🔧 Apple Silicon Optimizations

### Rust
- Target CPU set to `native` for ARM-specific optimizations
- Link-time optimization (LTO) enabled
- Uses `rayon` for efficient parallelization
- Metal API via `metal-rs` for GPU compute
- ARM NEON intrinsics available via `std::arch::aarch64`

### C++
- Compiled with `-march=native -O3` for maximum performance
- Metal framework for native GPU acceleration
- ARM NEON intrinsics via `arm_neon.h`
- C++20 features for modern code

## 📁 Project Structure

```
.
├── rust/                   # Rust implementations
│   ├── src/               # Benchmark source files
│   ├── Cargo.toml         # Dependencies and build config
│   └── .cargo/config.toml # Compiler flags
├── cpp/                    # C++ implementations
│   ├── src/               # Benchmark source files
│   └── CMakeLists.txt     # Build configuration
├── results/               # Generated results and graphs
│   ├── benchmark_results.json
│   ├── benchmark_comparison.png
│   ├── category_comparison.png
│   └── speedup_comparison.png
├── build.py               # Main build and benchmark script
└── README.md              # This file
```

## 📊 Output Files

After running benchmarks, you'll find:

- **`results/benchmark_results.json`** - Raw timing data with statistics
- **`results/benchmark_comparison.png`** - Bar chart comparing each test
- **`results/category_comparison.png`** - Performance by category
- **`results/speedup_comparison.png`** - Relative speedup visualization

## 🎨 Customization

### Adjust Number of Runs

Edit `build.py`:
```python
NUM_RUNS = 5  # Change to desired number
```

### Modify Benchmark Parameters

Edit individual benchmark files in `rust/src/` or `cpp/src/` to adjust:
- Matrix sizes
- Array lengths
- Iteration counts
- Sample counts

### Add New Benchmarks

1. Add Rust implementation in `rust/src/your_benchmark.rs`
2. Add C++ implementation in `cpp/src/your_benchmark.cpp`
3. Update `rust/Cargo.toml` to add the binary
4. Update `cpp/CMakeLists.txt` to add the executable
5. Add benchmark name to `BENCHMARKS` list in `build.py`

## 🔬 Methodology

- Each benchmark outputs execution time to stdout in seconds
- Warm-up runs prevent cold start penalties
- Multiple runs (default: 5) ensure statistical significance
- Identical algorithms and data sizes for fair comparison
- Both languages compiled with maximum optimizations

## 📝 Notes

- **Metal benchmarks** require macOS with Metal support
- Some benchmarks (FFT, SHA-256) may take several seconds
- First run may be slower due to system initialization
- Results vary based on system load and thermal conditions
- Best run on AC power with minimal background processes

## 🤝 Contributing

Feel free to:
- Add new benchmark categories
- Optimize existing implementations
- Report issues or inconsistencies
- Suggest improvements

## 📜 License

This project is provided as-is for educational and benchmarking purposes.

## 🎯 Goals

This benchmark suite aims to:
- Provide fair, real-world performance comparisons
- Showcase Apple Silicon optimization techniques
- Demonstrate parallel computing patterns
- Compare language ecosystems and tooling
- Help developers make informed language choices

---

**Happy Benchmarking! 🚀**

