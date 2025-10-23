# Usage Guide

## Installation Steps

### 1. Install Prerequisites

#### Rust
```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source ~/.cargo/env
```

#### CMake (if not installed)
```bash
brew install cmake
```

#### Python Dependencies
```bash
pip3 install -r requirements.txt
# or
pip3 install matplotlib numpy
```

### 2. Verify Installation

```bash
# Check Rust
rustc --version
cargo --version

# Check CMake
cmake --version

# Check Python
python3 --version
pip3 list | grep matplotlib
```

## Running Benchmarks

### Full Benchmark Suite
```bash
python3 build.py
```

### Build Only (No Benchmarks)
```bash
# Rust
cd rust
cargo build --release

# C++
cd cpp
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Run Individual Benchmarks

#### Rust
```bash
cd rust
cargo run --release --bin matrix_multiply
cargo run --release --bin ray_tracer
cargo run --release --bin metal_compute
# ... etc
```

#### C++
```bash
cd cpp/build
./matrix_multiply
./ray_tracer
./metal_compute
# ... etc
```

## Understanding Output

### Benchmark Output
Each benchmark prints:
- **stdout**: Execution time in seconds (e.g., `1.234567`)
- **stderr**: Additional info like checksums or verification

Example:
```
$ ./matrix_multiply
0.523456
Checksum: 123456789.0
```

### Results Files

#### benchmark_results.json
```json
{
  "rust": {
    "matrix_multiply": {
      "times": [0.52, 0.51, 0.53, 0.52, 0.51],
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

## Customization

### Change Number of Runs
Edit `build.py`:
```python
NUM_RUNS = 10  # Default is 5
```

### Modify Benchmark Sizes
Edit source files:

**Rust** (`rust/src/matrix_multiply.rs`):
```rust
const SIZE: usize = 2048;  // Change from 1024
```

**C++** (`cpp/src/matrix_multiply.cpp`):
```cpp
const size_t SIZE = 2048;  // Change from 1024
```

### Skip Specific Benchmarks
Edit `build.py`:
```python
BENCHMARKS = [
    "matrix_multiply",
    # "parallel_quicksort",  # Comment out to skip
    "thread_pool",
    # ...
]
```

## Troubleshooting

### Metal Not Found (C++)
If you see Metal-related errors:
```bash
# Verify Metal framework exists
ls /System/Library/Frameworks/Metal.framework

# Check Xcode Command Line Tools
xcode-select --install
```

### Rust Metal Crate Issues
```bash
cd rust
cargo clean
cargo build --release
```

### CMake Can't Find Clang
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Or specify compiler explicitly
export CC=clang
export CXX=clang++
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Python Module Not Found
```bash
pip3 install --upgrade matplotlib numpy
# or use virtual environment
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

### Benchmark Times Out
Increase timeout in `build.py`:
```python
result = subprocess.run(
    cmd,
    timeout=600  # Change from 300 to 600 seconds
)
```

## Performance Tips

### For Best Results

1. **Close unnecessary applications**
2. **Plug in to AC power** (prevents thermal throttling)
3. **Wait for system to cool** between runs
4. **Disable background tasks** (Time Machine, Spotlight, etc.)
5. **Run multiple times** and compare averages

### Monitoring Performance

```bash
# Check CPU temperature
sudo powermetrics --samplers smc -i1 -n1

# Watch CPU usage
top -o cpu

# Monitor GPU
sudo powermetrics --samplers gpu_power -i1000 -n1
```

## Advanced Usage

### Profile with Instruments
```bash
# Rust
cargo build --release
instruments -t "Time Profiler" rust/target/release/matrix_multiply

# C++
instruments -t "Time Profiler" cpp/build/matrix_multiply
```

### Benchmark with Hyperfine
```bash
brew install hyperfine

hyperfine './rust/target/release/matrix_multiply' './cpp/build/matrix_multiply'
```

### Export Results
```bash
# Run benchmarks
python3 build.py

# Results are in JSON format
cat results/benchmark_results.json | python3 -m json.tool
```

## Adding Custom Benchmarks

### 1. Create Rust Implementation
```bash
touch rust/src/my_benchmark.rs
```

Add to `rust/Cargo.toml`:
```toml
[[bin]]
name = "my_benchmark"
path = "src/my_benchmark.rs"
```

### 2. Create C++ Implementation
```bash
touch cpp/src/my_benchmark.cpp
```

Add to `cpp/CMakeLists.txt`:
```cmake
add_executable(my_benchmark src/my_benchmark.cpp)
target_link_libraries(my_benchmark pthread)
```

### 3. Update Build Script
Add to `build.py`:
```python
BENCHMARKS = [
    # ... existing benchmarks
    "my_benchmark",
]
```

### 4. Test
```bash
python3 build.py
```

## Interpreting Results

### Speedup Chart
- **> 1.0**: Rust is faster
- **< 1.0**: C++ is faster
- **≈ 1.0**: Approximately equal performance

### Category Comparison
Shows total time for each category:
- Lower bars = better performance
- Compare relative strengths of each language

### Individual Benchmarks
- Look at both average and standard deviation
- Lower std = more consistent performance
- Consider warmup effects

## FAQ

**Q: Why is the first run slower?**
A: Cold start effects. Both languages do warmup runs to mitigate this.

**Q: Can I run on Intel Macs?**
A: Yes, but remove Apple Silicon specific flags from build configs.

**Q: Why different results each run?**
A: Normal variation due to system state, thermal conditions, background processes.

**Q: Which language is "better"?**
A: Depends on use case. Both have trade-offs in performance, safety, and ecosystem.

**Q: Can I add GPU benchmarks other than Metal?**
A: Yes! Add CUDA/OpenCL/Vulkan implementations following the same pattern.

---

For more information, see [README.md](README.md)

