# Project Architecture

Technical overview of the benchmark suite's design and implementation.

## 🏗️ System Design

```
┌─────────────────────────────────────────────────────────┐
│                    build.py (Orchestrator)               │
│  - Build management                                      │
│  - Test execution                                        │
│  - Data collection                                       │
│  - Visualization                                         │
└──────────────┬───────────────────────────┬──────────────┘
               │                           │
       ┌───────▼────────┐          ┌──────▼───────┐
       │  Rust Binaries │          │ C++ Binaries │
       │  (10 tests)    │          │ (10 tests)   │
       └───────┬────────┘          └──────┬───────┘
               │                           │
               └───────────┬───────────────┘
                           │
                   ┌───────▼────────┐
                   │  Results JSON   │
                   └───────┬────────┘
                           │
                   ┌───────▼────────┐
                   │  PNG Graphs    │
                   └────────────────┘
```

## 📁 Directory Structure

```
tinker/
│
├── rust/                          # Rust implementation
│   ├── .cargo/
│   │   └── config.toml           # Compiler optimizations
│   ├── src/
│   │   ├── matrix_multiply.rs    # Parallelization tests
│   │   ├── parallel_quicksort.rs
│   │   ├── thread_pool.rs
│   │   ├── ray_tracer.rs         # Graphics tests
│   │   ├── mandelbrot.rs
│   │   ├── metal_compute.rs
│   │   ├── prime_sieve.rs        # Compute tests
│   │   ├── fft.rs
│   │   ├── sha256.rs
│   │   └── json_parse.rs         # Other tests
│   ├── Cargo.toml                # Dependencies
│   └── target/release/           # Compiled binaries (generated)
│
├── cpp/                          # C++ implementation
│   ├── src/
│   │   ├── matrix_multiply.cpp   # Mirror of Rust tests
│   │   ├── parallel_quicksort.cpp
│   │   ├── thread_pool.cpp
│   │   ├── ray_tracer.cpp
│   │   ├── mandelbrot.cpp
│   │   ├── metal_compute.mm      # Objective-C++ for Metal
│   │   ├── prime_sieve.cpp
│   │   ├── fft.cpp
│   │   ├── sha256.cpp
│   │   └── json_parse.cpp
│   ├── CMakeLists.txt            # Build configuration
│   └── build/                    # Build artifacts (generated)
│
├── results/                      # Output directory
│   ├── benchmark_results.json   # Raw timing data
│   ├── benchmark_comparison.png # Visual comparisons
│   ├── category_comparison.png
│   └── speedup_comparison.png
│
├── scripts/                      # (Reserved for future use)
│
├── build.py                      # Main orchestrator
├── quick_start.sh                # Setup automation
├── requirements.txt              # Python dependencies
│
└── Documentation/
    ├── README.md                # Project overview
    ├── GETTING_STARTED.md       # Quick start
    ├── USAGE.md                 # Detailed usage
    ├── BENCHMARKS.md            # Test descriptions
    ├── CONTRIBUTING.md          # Extension guide
    ├── ARCHITECTURE.md          # This file
    ├── PROJECT_SUMMARY.md       # Complete summary
    ├── LICENSE                  # MIT license
    └── .gitignore              # VCS configuration
```

## 🔄 Execution Flow

### Phase 1: Build
```
build.py
  │
  ├─→ build_rust()
  │     ├─→ cargo build --release
  │     └─→ Generates binaries in rust/target/release/
  │
  └─→ build_cpp()
        ├─→ mkdir cpp/build && cd cpp/build
        ├─→ cmake .. -DCMAKE_BUILD_TYPE=Release
        └─→ cmake --build .
```

### Phase 2: Execute
```
run_all_benchmarks()
  │
  └─→ For each benchmark:
        │
        ├─→ For run 1 to NUM_RUNS:
        │     │
        │     ├─→ run_benchmark("rust", benchmark)
        │     │     ├─→ Execute rust/target/release/{benchmark}
        │     │     ├─→ Capture stdout (time)
        │     │     └─→ Store in results["rust"][benchmark]
        │     │
        │     └─→ run_benchmark("cpp", benchmark)
        │           ├─→ Execute cpp/build/{benchmark}
        │           ├─→ Capture stdout (time)
        │           └─→ Store in results["cpp"][benchmark]
        │
        └─→ Return aggregated results
```

### Phase 3: Analysis
```
save_results(results)
  │
  ├─→ For each language/benchmark:
  │     ├─→ Calculate mean
  │     ├─→ Calculate min/max
  │     └─→ Calculate std deviation
  │
  └─→ Write to results/benchmark_results.json
```

### Phase 4: Visualization
```
plot_results()
  │
  ├─→ Load results JSON
  │
  ├─→ Generate benchmark_comparison.png
  │     └─→ Bar chart: Rust vs C++ for each test
  │
  ├─→ Generate category_comparison.png
  │     └─→ Bar chart: Performance by category
  │
  └─→ Generate speedup_comparison.png
        └─→ Horizontal bar: Relative performance
```

## 🎯 Benchmark Binary Protocol

Each benchmark executable follows this contract:

### Input
- **Arguments**: None (all parameters hardcoded)
- **Environment**: Standard process environment

### Output
- **stdout**: Single line with execution time in seconds (6 decimal places)
  ```
  1.234567
  ```
- **stderr**: Optional verification/debug info
  ```
  Checksum: 123456
  ```
- **Exit code**: 0 for success, non-zero for failure

### Example
```bash
$ ./matrix_multiply
0.523456
Checksum: 1234567890.0
```

The orchestrator parses stdout for timing and ignores stderr (except for logging).

## 🔧 Compilation Flags

### Rust (.cargo/config.toml)
```toml
[build]
rustflags = ["-C", "target-cpu=native"]

[target.aarch64-apple-darwin]
rustflags = ["-C", "target-cpu=native", "-C", "opt-level=3"]
```

### Rust (Cargo.toml)
```toml
[profile.release]
opt-level = 3              # Maximum optimizations
lto = true                 # Link-time optimization
codegen-units = 1          # Better optimization (slower compile)
panic = "abort"            # Smaller binary, faster unwind
```

### C++ (CMakeLists.txt)
```cmake
CMAKE_CXX_STANDARD 20
CMAKE_CXX_FLAGS "-O3 -march=native -std=c++20"
CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG"
```

## 🧩 Component Responsibilities

### build.py
- **Build management**: Invoke cargo/cmake
- **Test execution**: Run binaries, capture output
- **Timing collection**: Parse stdout times
- **Statistics**: Calculate mean, std dev, min/max
- **Persistence**: Save JSON results
- **Visualization**: Generate PNG graphs

### Benchmark Binaries
- **Algorithm implementation**: Core logic
- **Warmup**: Eliminate cold start effects
- **Timing**: Measure only relevant code
- **Verification**: Checksum to prevent optimization removal
- **Output**: Standard format for parsing

### Rust Implementations
- **Language**: Idiomatic Rust
- **Parallelism**: Rayon for data parallelism
- **Libraries**: serde_json, sha2, rustfft, metal-rs
- **Safety**: Leverages Rust's safety guarantees

### C++ Implementations
- **Language**: Modern C++20
- **Parallelism**: std::thread, manual pools
- **Libraries**: STL, Metal framework, custom implementations
- **Performance**: Manual memory management where beneficial

## 🔍 Data Flow

```
Source Code (.rs, .cpp)
      │
      ├─→ Cargo.toml / CMakeLists.txt
      │
      ▼
Build System (cargo / cmake)
      │
      ├─→ Compiler Optimizations
      │     ├─→ -O3
      │     ├─→ -march=native
      │     ├─→ LTO
      │     └─→ target-cpu=native
      │
      ▼
Binary Executables
      │
      ├─→ Standard Input: None
      │
      ▼
Execution (subprocess)
      │
      ├─→ stdout: "0.123456"
      ├─→ stderr: "Checksum: X"
      └─→ exit code: 0
      │
      ▼
Python Parser
      │
      ├─→ Extract float from stdout
      ├─→ Store in results dict
      └─→ Repeat NUM_RUNS times
      │
      ▼
Statistics Calculation
      │
      ├─→ Mean, Min, Max, StdDev
      └─→ Per benchmark, per language
      │
      ▼
JSON Serialization
      │
      └─→ results/benchmark_results.json
      │
      ▼
Matplotlib Visualization
      │
      ├─→ Bar charts
      ├─→ Category aggregations
      └─→ Speedup calculations
      │
      ▼
PNG Files (300 DPI)
      │
      ├─→ benchmark_comparison.png
      ├─→ category_comparison.png
      └─→ speedup_comparison.png
```

## 🎨 Design Patterns

### Rust
- **Iterator patterns**: Extensive use of iterator chains
- **Ownership**: Zero-copy where possible
- **Error handling**: Result types throughout
- **Parallelism**: Rayon's data parallelism model

### C++
- **RAII**: Automatic resource management
- **Smart pointers**: Minimal raw pointers
- **Templates**: Generic programming where beneficial
- **Modern STL**: std::thread, std::mutex, etc.

### Python Orchestrator
- **Subprocess management**: Safe command execution
- **Data structures**: Dict/list for flexible data handling
- **Visualization**: Matplotlib for publication-quality graphs
- **Error handling**: Try/except with informative messages

## 🔐 Safety & Reliability

### Compile-time Checks
- Rust: Ownership, borrowing, lifetimes
- C++: Type safety, const correctness

### Runtime Protections
- Timeout: 300s per benchmark (configurable)
- Exit code checking: Detect failures
- Output validation: Parse errors caught
- Exception handling: Graceful degradation

### Verification
- Checksums: Prevent dead code elimination
- Warmup runs: Consistent measurements
- Multiple runs: Statistical confidence
- Identical algorithms: Fair comparison

## 📊 Performance Considerations

### Benchmark Design
- **Size selection**: Exceeds L2 cache to test memory
- **Iteration counts**: Balance accuracy vs runtime
- **Warmup**: Eliminate JIT/cache effects
- **Verification**: Minimal overhead

### Apple Silicon Specific
- **Unified memory**: Shared CPU/GPU memory
- **Metal**: Native GPU API
- **ARM NEON**: SIMD instructions
- **Efficiency cores**: May affect parallel tests

### Measurement Accuracy
- **High-resolution timer**: std::chrono / Instant
- **Process isolation**: Subprocess per run
- **Multiple runs**: Reduce variance
- **Statistics**: Mean ± std dev

## 🔮 Extensibility

### Adding New Benchmarks
1. Create source files (rust/src/*.rs, cpp/src/*.cpp)
2. Update build configs (Cargo.toml, CMakeLists.txt)
3. Add to BENCHMARKS list in build.py
4. Optional: Add new category in CATEGORIES
5. Document in BENCHMARKS.md

### Modifying Parameters
- Edit constants in source files
- Rebuild and re-run
- Compare against previous results

### Cross-platform Support
- Abstract Metal dependencies
- Add platform detection
- Support alternative GPU APIs (CUDA, OpenCL)
- Adjust compiler flags per platform

## 📝 File Formats

### benchmark_results.json
```json
{
  "rust": {
    "benchmark_name": {
      "times": [0.51, 0.52, 0.51, 0.53, 0.52],
      "stats": {
        "mean": 0.518,
        "min": 0.51,
        "max": 0.53,
        "std": 0.008
      }
    }
  },
  "cpp": { ... }
}
```

## 🎯 Design Goals Achieved

✅ **Fair comparison**: Identical algorithms, both optimized
✅ **Automated**: One command runs everything
✅ **Comprehensive**: 10 diverse, realistic tests
✅ **Visual**: Publication-quality graphs
✅ **Documented**: Extensive guides
✅ **Extensible**: Easy to add new tests
✅ **Reproducible**: Multiple runs, statistics
✅ **Platform-optimized**: Apple Silicon specific

---

For implementation details, see source code comments in respective files.

