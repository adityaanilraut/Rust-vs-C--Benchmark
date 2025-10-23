# Getting Started - Quick Guide

New to benchmarking or want the fastest path to results? This guide is for you!

## ⚡ 30-Second Start

```bash
cd tinker
./quick_start.sh
```

That's it! The script will check dependencies and run everything.

## 🎯 What Happens?

1. **Checks prerequisites** (Rust, CMake, Python)
2. **Builds all benchmarks** (Rust + C++)
3. **Runs each test 5 times**
4. **Generates graphs**
5. **Shows summary**

## 📊 What You'll See

### During Execution
```
==================================
Rust vs C++ Benchmark Suite
==================================

Building Rust benchmarks...
✅ Rust build successful

Building C++ benchmarks...
✅ C++ build successful

============================================================
Running: matrix_multiply
============================================================
[1/100] Rust matrix_multiply (run 1/5)... ✓ 0.5234s
[2/100] C++  matrix_multiply (run 1/5)... ✓ 0.5456s
...
```

### Final Summary
```
================================================================================
BENCHMARK SUMMARY
================================================================================
Benchmark                 Rust (avg)      C++ (avg)       Winner     Speedup
--------------------------------------------------------------------------------
matrix_multiply              0.5234s        0.5456s       Rust       1.04x
parallel_quicksort           1.2345s        1.2890s       Rust       1.04x
...
--------------------------------------------------------------------------------
Total: Rust wins: 6, C++ wins: 4
================================================================================
```

### Generated Files
```
results/
├── benchmark_results.json      # Raw data
├── benchmark_comparison.png    # Main chart
├── category_comparison.png     # By category
└── speedup_comparison.png      # Relative performance
```

## 🔧 Prerequisites

### Required
- **macOS** with Apple Silicon (M1/M2/M3/M4)
- **Rust** - Install: `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
- **CMake** - Install: `brew install cmake`
- **Python 3** - Already on macOS, or: `brew install python3`

### Check If Installed
```bash
rustc --version    # Should show: rustc 1.x.x
cmake --version    # Should show: cmake version 3.x
python3 --version  # Should show: Python 3.x
```

## 🚀 Manual Run (Alternative)

If you prefer manual control:

```bash
# Install Python dependencies
pip3 install matplotlib numpy

# Run benchmarks
python3 build.py
```

## 📈 View Results

### Open Graphs
```bash
open results/benchmark_comparison.png
open results/category_comparison.png
open results/speedup_comparison.png
```

### View Raw Data
```bash
cat results/benchmark_results.json | python3 -m json.tool
```

## ⏱️ How Long Does It Take?

- **Build time**: 2-5 minutes (first time only)
- **Benchmark time**: 5-15 minutes (depends on your Mac)
- **Total**: ~10-20 minutes for everything

## 🎓 Understanding Results

### The Graphs

**1. Benchmark Comparison** (bar chart)
- Red bars = Rust
- Blue bars = C++
- Shorter = faster = better

**2. Category Comparison**
- Shows performance by test category
- Parallelization, Graphics, Heavy Compute, Other

**3. Speedup Chart**
- Horizontal bars
- >1.0 = Rust faster
- <1.0 = C++ faster

### The Numbers

Example output:
```
matrix_multiply    0.5234s    0.5456s    Rust    1.04x
```

Means:
- Rust took 0.5234 seconds (average of 5 runs)
- C++ took 0.5456 seconds (average of 5 runs)
- Rust was winner (faster)
- Rust was 1.04× faster (4% speed improvement)

## 🐛 Common Issues

### "Rust not found"
```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source ~/.cargo/env
```

### "CMake not found"
```bash
brew install cmake
```

### "matplotlib not found"
```bash
pip3 install matplotlib numpy
# or
pip3 install --user matplotlib numpy
```

### "Permission denied"
```bash
chmod +x quick_start.sh
chmod +x build.py
```

### Metal/GPU errors
- Make sure you're on Apple Silicon (M1/M2/M3/M4)
- Install Xcode Command Line Tools: `xcode-select --install`

## 📱 System Requirements

### Minimum
- macOS 11.0+ (Big Sur)
- Apple Silicon Mac (M1 or newer)
- 8 GB RAM
- 2 GB free disk space

### Recommended
- macOS 13.0+ (Ventura)
- M2 or newer
- 16 GB RAM
- Plugged into power (prevents throttling)

## 🎯 Next Steps

After running:

1. **Explore graphs** in `results/` folder
2. **Read detailed explanations** in `BENCHMARKS.md`
3. **Try modifying** benchmark parameters
4. **Add your own** benchmarks (see `CONTRIBUTING.md`)

## 📚 Learn More

- **README.md** - Full project overview
- **USAGE.md** - Detailed usage guide
- **BENCHMARKS.md** - What each test does
- **CONTRIBUTING.md** - How to extend

## ❓ FAQ

**Q: Do I need to understand Rust or C++?**
A: No! Just run the benchmarks and view results.

**Q: Can I run just one benchmark?**
A: Yes! See `USAGE.md` for individual test instructions.

**Q: Will this slow down my Mac?**
A: It will use CPU heavily for 10-15 minutes, but won't harm anything.

**Q: Can I run this on Intel Mac?**
A: Mostly yes, but Metal GPU tests require Apple Silicon.

**Q: How accurate are the results?**
A: Very accurate - multiple runs, statistical analysis, warm-up phases.

**Q: Why does Rust/C++ win certain tests?**
A: Different strengths - read `BENCHMARKS.md` for details.

## 🎉 Success!

If you see the summary table and graphs are generated, congratulations! You've successfully benchmarked Rust vs C++ on your Mac.

---

**Need Help?** Check `USAGE.md` for troubleshooting or open an issue.

