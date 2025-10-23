# Contributing Guide

Thank you for your interest in improving the Rust vs C++ Benchmark Suite! This guide will help you add new benchmarks or improve existing ones.

## 🎯 Goals

This benchmark suite aims to:
1. Provide **fair, apples-to-apples** comparisons
2. Use **realistic workloads** (not artificial microbenchmarks)
3. Showcase **Apple Silicon** optimizations
4. Demonstrate **modern best practices** in both languages
5. Remain **educational and accessible**

## 📝 Adding a New Benchmark

### Step 1: Design the Benchmark

Ask yourself:
- ✅ **What does it measure?** (computation, I/O, memory, parallelism?)
- ✅ **Is it realistic?** (does it represent real-world code?)
- ✅ **Is it fair?** (can both languages implement it similarly?)
- ✅ **Is it measurable?** (runs 0.1-10 seconds ideally)

### Step 2: Implement in Rust

Create `rust/src/my_benchmark.rs`:

```rust
use std::time::Instant;

// Your benchmark implementation
fn my_algorithm() -> ResultType {
    // Implementation here
}

fn main() {
    // Warm-up (important!)
    let _ = my_algorithm();
    
    // Benchmark
    let start = Instant::now();
    let result = my_algorithm();
    let duration = start.elapsed();
    
    // Output time to stdout (REQUIRED FORMAT)
    println!("{:.6}", duration.as_secs_f64());
    
    // Optional: verification info to stderr
    eprintln!("Checksum: {:?}", result);
}
```

**Key requirements**:
- ✅ Output time in seconds to stdout (6 decimal places)
- ✅ Include warmup run
- ✅ Print verification/checksum to stderr
- ✅ Use release-optimized algorithms

### Step 3: Implement in C++

Create `cpp/src/my_benchmark.cpp`:

```cpp
#include <iostream>
#include <chrono>
#include <iomanip>

// Your benchmark implementation
auto my_algorithm() {
    // Implementation here
}

int main() {
    // Warm-up
    auto warmup = my_algorithm();
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    auto result = my_algorithm();
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    
    // Output time to stdout (REQUIRED FORMAT)
    std::cout << std::fixed << std::setprecision(6) 
              << duration.count() << std::endl;
    
    // Optional: verification to stderr
    std::cerr << "Checksum: " << result << std::endl;
    
    return 0;
}
```

**Key requirements**:
- ✅ Match Rust implementation exactly
- ✅ Same data structures and algorithms
- ✅ Same output format
- ✅ Include warmup run

### Step 4: Update Build Files

**Rust** - Add to `rust/Cargo.toml`:
```toml
[[bin]]
name = "my_benchmark"
path = "src/my_benchmark.rs"
```

**C++** - Add to `cpp/CMakeLists.txt`:
```cmake
add_executable(my_benchmark src/my_benchmark.cpp)
target_link_libraries(my_benchmark pthread)  # if needed
```

### Step 5: Update Runner Script

Add to `build.py`:
```python
BENCHMARKS = [
    # ... existing benchmarks
    "my_benchmark",
]

# If adding a new category:
CATEGORIES = {
    # ... existing categories
    "My Category": ["my_benchmark"],
}
```

### Step 6: Test

```bash
# Build
cd rust && cargo build --release && cd ..
cd cpp && mkdir -p build && cd build && cmake .. && make && cd ../..

# Test individually
./rust/target/release/my_benchmark
./cpp/build/my_benchmark

# Run full suite
python3 build.py
```

### Step 7: Document

Add to `BENCHMARKS.md`:
```markdown
### X. My Benchmark

**What it tests**: Brief description

**Implementation**:
- Key parameters
- Algorithm details
- Rust vs C++ differences (if any)

**Why it matters**: Real-world applications

**Performance factors**:
- Factor 1
- Factor 2

**Expected**: What you expect to see
```

## 🔧 Best Practices

### Algorithm Implementation

✅ **DO**:
- Use idiomatic code for each language
- Leverage standard libraries where appropriate
- Optimize for the hardware (SIMD, cache, etc.)
- Make implementations as similar as possible
- Comment any unavoidable differences

❌ **DON'T**:
- Artificially handicap one language
- Use debug builds or unoptimized code
- Include I/O in timing (unless testing I/O)
- Make one implementation obviously worse

### Data Sizes

Choose sizes that:
- ✅ Run in 0.1-10 seconds (sweet spot)
- ✅ Exceed L1/L2 cache (test memory subsystem)
- ✅ Allow multiple runs for statistics
- ✅ Are realistic (not toy problems)

### Parallelization

If using threads:
- ✅ Use same thread count in both languages
- ✅ Account for thread creation overhead
- ✅ Test on machines with 4-16 cores
- ✅ Consider CPU affinity if relevant

### Apple Silicon Optimizations

Enable these where relevant:
```rust
// Rust: in .cargo/config.toml
[build]
rustflags = ["-C", "target-cpu=native"]

#[cfg(target_arch = "aarch64")]
use std::arch::aarch64::*;
```

```cpp
// C++: in CMakeLists.txt or flags
-march=native -O3

#ifdef __aarch64__
#include <arm_neon.h>
#endif
```

## 🐛 Improving Existing Benchmarks

### Found a Bug?
1. Verify it affects both implementations
2. Create an issue describing the problem
3. Submit a fix with verification

### Optimization Opportunity?
1. Ensure it's fair (can apply to both languages)
2. Document the optimization
3. Update both implementations
4. Measure the improvement

### Better Algorithm?
1. Discuss in an issue first
2. Ensure it's still representative
3. Update both implementations simultaneously
4. Update documentation

## 📊 Benchmark Quality Checklist

Before submitting:

- [ ] Both implementations produce identical results
- [ ] Both use same algorithms and data structures
- [ ] Both run in reasonable time (0.1-10s)
- [ ] Output format matches (time to stdout)
- [ ] Warmup runs included
- [ ] Verification/checksum to stderr
- [ ] Builds without warnings in both languages
- [ ] Added to `build.py` BENCHMARKS list
- [ ] Documented in `BENCHMARKS.md`
- [ ] Tested on Apple Silicon
- [ ] Graphs generate correctly

## 🎨 Code Style

### Rust
- Follow `rustfmt` defaults
- Use `cargo clippy` and fix warnings
- Prefer iterators over loops where idiomatic
- Use descriptive variable names

### C++
- Modern C++20 style
- RAII for resource management
- Use standard library over raw pointers
- Consistent with existing benchmarks

## 🚀 Advanced Contributions

### GPU Benchmarks
- Use Metal for macOS
- Consider CUDA/OpenCL for cross-platform
- Measure kernel time separately from setup
- Document GPU model dependencies

### SIMD Optimizations
- Provide both scalar and SIMD versions
- Use portable SIMD where possible
- Document instruction sets (NEON, AVX, etc.)
- Benchmark on multiple CPU architectures

### Memory Benchmarks
- Test different allocation patterns
- Consider NUMA effects (if relevant)
- Measure cache behavior
- Profile with Instruments/perf

## 📞 Getting Help

- **Questions**: Open a discussion
- **Bugs**: File an issue
- **Ideas**: Start a discussion first
- **PRs**: Include rationale and measurements

## 🏆 Recognition

Contributors will be acknowledged in:
- README.md contributors section
- Release notes
- Benchmark documentation

---

Thank you for helping make this benchmark suite better! 🎉

