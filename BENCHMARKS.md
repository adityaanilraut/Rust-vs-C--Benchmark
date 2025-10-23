# Benchmark Descriptions

Detailed information about each benchmark test, including what it measures, implementation details, and expected performance characteristics.

---

## 🔄 Parallelization Benchmarks

### 1. Parallel Matrix Multiplication

**What it tests**: Multi-threaded computation and memory access patterns

**Implementation**:
- Matrix size: 1024×1024 (1,048,576 elements each)
- Operation: Standard matrix multiplication C = A × B
- Parallelization: Row-wise distribution across threads
- Rust: Uses `rayon` parallel iterators
- C++: Uses `std::thread` with manual work distribution

**Why it matters**: Matrix multiplication is fundamental in:
- Graphics/game engines (transformations)
- Machine learning (neural networks)
- Scientific computing (simulations)

**Performance factors**:
- Thread synchronization overhead
- Cache locality and false sharing
- SIMD auto-vectorization
- Memory bandwidth

**Expected**: Close performance, slight edge to language with better auto-vectorization

---

### 2. Parallel QuickSort

**What it tests**: Recursive parallelization and cache-efficient sorting

**Implementation**:
- Array size: 10,000,000 integers
- Algorithm: Parallel quicksort with sequential fallback at 10K elements
- Both implementations partition recursively and spawn threads for sub-arrays

**Why it matters**: Sorting is ubiquitous in:
- Database operations
- Search algorithms
- Data processing pipelines

**Performance factors**:
- Thread creation/destruction overhead
- Stack depth and recursion
- Cache efficiency during partitioning
- Branch prediction

**Expected**: Very close, with potential advantage to Rust's `rayon` work-stealing

---

### 3. Thread Pool Task Distribution

**What it tests**: Task scheduling, queue management, and synchronization

**Implementation**:
- 100,000 tasks distributed across 8 worker threads
- Each task performs 1000 iterations of computation
- Uses atomic counter for result aggregation
- Rust: Custom thread pool with mpsc channels
- C++: Custom thread pool with condition variables

**Why it matters**: Thread pools are core to:
- Web servers (request handling)
- Background job processing
- Event-driven systems

**Performance factors**:
- Lock contention
- Context switching
- Queue management efficiency
- Task granularity

**Expected**: Close performance, advantages to better synchronization primitives

---

## 🎨 Graphics Benchmarks

### 4. CPU Ray Tracer

**What it tests**: Floating-point intensive computation with parallelization

**Implementation**:
- Resolution: 1920×1080 pixels (2,073,600 pixels)
- 4 samples per pixel
- Scene: 4 spheres with diffuse shading
- Parallel ray generation across pixels

**Why it matters**: Ray tracing is used in:
- Real-time rendering engines
- Film production
- Architectural visualization

**Performance factors**:
- Floating-point performance
- SIMD vectorization
- Thread efficiency
- Cache behavior with random memory access

**Expected**: Close performance, potential SIMD optimizations matter

---

### 5. Mandelbrot Set Generator

**What it tests**: Complex number arithmetic with SIMD potential

**Implementation**:
- Resolution: 4096×4096 (16,777,216 points)
- Max iterations: 1000 per point
- Parallel computation across rows
- Region: -2.5 to 1.0 (real), -1.0 to 1.0 (imaginary)

**Why it matters**: Representative of:
- Scientific visualization
- Fractal generation
- Iterative numerical methods

**Performance factors**:
- Loop optimization
- SIMD utilization
- Branch prediction (early termination)
- Parallel efficiency

**Expected**: Very close, compiler optimizations crucial

---

### 6. Metal GPU Compute

**What it tests**: GPU compute shader performance on Apple Silicon

**Implementation**:
- Buffer size: 10,000,000 floats
- Operation: `result[i] = a[i] * b[i] + sin(a[i]) + cos(b[i])`
- Native Metal API calls in both languages
- Single kernel dispatch with 256 threads per threadgroup

**Why it matters**: GPU compute is essential for:
- Machine learning inference
- Image processing
- Physics simulations
- Cryptocurrency mining

**Performance factors**:
- API overhead (Objective-C vs Rust bindings)
- Memory transfer CPU↔GPU
- Kernel launch latency
- Unified memory advantage on Apple Silicon

**Expected**: Very close since both use same Metal API, slight overhead differences

---

## 💪 Heavy Compute Benchmarks

### 7. Prime Number Sieve

**What it tests**: Memory-intensive algorithm with cache behavior

**Implementation**:
- Sieve of Eratosthenes up to 100,000,000
- Sequential algorithm (not parallelized)
- Large boolean array (100MB)

**Why it matters**: Similar patterns in:
- Cryptography
- Number theory applications
- Bit manipulation algorithms

**Performance factors**:
- Memory access patterns
- Cache efficiency (sequential access)
- Branch prediction
- Memory allocation strategy

**Expected**: Very close, memory bandwidth limited

---

### 8. FFT Computation

**What it tests**: Complex number arithmetic and recursion

**Implementation**:
- FFT size: 16,777,216 points (2^24)
- Algorithm: Cooley-Tukey radix-2 FFT
- Rust: Uses `rustfft` crate (highly optimized)
- C++: Custom recursive implementation
- Input: Sine wave combination (50Hz + 120Hz)

**Why it matters**: FFT is fundamental in:
- Audio processing
- Signal processing
- Telecommunications
- Image compression

**Performance factors**:
- Cache efficiency (butterfly operations)
- Complex number operations
- Memory access patterns (bit-reversal)
- Library optimization quality

**Expected**: Rust may have advantage due to optimized library

---

### 9. SHA-256 Hashing

**What it tests**: Bitwise operations and integer arithmetic

**Implementation**:
- Data size: 100,000,000 bytes (100MB)
- Processed in 1KB chunks
- Rust: Uses `sha2` crate
- C++: Custom SHA-256 implementation

**Why it matters**: Hashing is crucial in:
- Cryptography
- Data integrity verification
- Blockchain
- Password storage

**Performance factors**:
- Integer operations per cycle
- Instruction-level parallelism
- Cache efficiency
- SIMD potential for parallel lanes

**Expected**: Rust likely faster due to optimized library implementation

---

## 📦 Other Benchmarks

### 10. JSON Parsing & Serialization

**What it tests**: String processing, memory allocation, parsing efficiency

**Implementation**:
- 100,000 JSON records with nested objects
- Each record: 7 fields + nested metadata + array
- Rust: Uses `serde_json` (zero-copy where possible)
- C++: Custom simple parser (no external library)
- Measures parse + serialize round-trip

**Why it matters**: JSON is ubiquitous in:
- Web APIs
- Configuration files
- Data interchange
- NoSQL databases

**Performance factors**:
- Memory allocation strategy
- String handling
- Parsing algorithm efficiency
- Serialization optimizations

**Expected**: Rust significantly faster due to highly optimized `serde_json`

---

## 🎯 Performance Insights

### What Each Category Tests

**Parallelization**: 
- Thread management overhead
- Synchronization primitives efficiency
- Work distribution strategies

**Graphics**:
- Floating-point performance
- SIMD utilization
- GPU compute efficiency

**Heavy Compute**:
- Raw CPU performance
- Memory bandwidth
- Cache hierarchy efficiency

**Other**:
- Ecosystem library quality
- String/data structure handling
- Memory management

### Apple Silicon Specific Optimizations

Both implementations leverage:
- **ARM NEON** SIMD instructions
- **Native CPU features** (-march=native / target-cpu=native)
- **Metal API** for GPU compute
- **Unified memory** architecture benefits
- **High bandwidth** memory subsystem

### Real-World Relevance

These benchmarks represent:
- ✅ **Common patterns** in production code
- ✅ **Realistic data sizes** (not microbenchmarks)
- ✅ **Diverse workloads** (CPU, GPU, memory, I/O)
- ✅ **Modern hardware** utilization

---

## 📊 Interpreting Results

### When Rust Tends to Win
- JSON/serialization (excellent libraries)
- Memory safety overhead is minimal
- Zero-cost abstractions shine
- Iterator optimizations help

### When C++ Tends to Win
- Raw pointer manipulation
- Minimal abstraction overhead
- Mature optimization in compilers
- Fine-grained control

### When They're Close
- Compute-bound workloads
- SIMD-heavy operations
- GPU-bound tasks
- Memory bandwidth limited

### Variance Factors
- System thermal state
- Background processes
- Memory pressure
- Power management state

---

For detailed usage instructions, see [USAGE.md](USAGE.md)

