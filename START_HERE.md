# 🚀 START HERE

Welcome to the **Rust vs C++ Benchmark Suite** for Apple Silicon!

## ⚡ Quick Start (30 seconds)

```bash
./quick_start.sh
```

That's it! This will check dependencies, build everything, run all benchmarks, and generate comparison graphs.

## 📊 What This Project Does

Compares **Rust** and **C++** performance across **10 different benchmark tests**:

### Test Categories
- **Parallelization**: Matrix multiplication, sorting, thread pools
- **Graphics**: Ray tracing, Mandelbrot sets, GPU compute
- **Heavy Compute**: Prime numbers, FFT, SHA-256 hashing
- **Other**: JSON parsing and serialization

### Features
✅ Fully automated build and test system  
✅ Apple Silicon optimized (M1/M2/M3/M4)  
✅ Multiple runs for statistical accuracy  
✅ Beautiful comparison graphs  
✅ Comprehensive documentation  

## 📁 Important Files

| File | Purpose |
|------|---------|
| **quick_start.sh** | One-command setup and run |
| **build.py** | Main benchmark orchestrator |
| **GETTING_STARTED.md** | Beginner's guide |
| **README.md** | Full project overview |
| **BENCHMARKS.md** | What each test does |

## 🎯 What Happens When You Run

1. ✅ Checks if Rust, CMake, Python are installed
2. 🔨 Builds 10 Rust benchmarks (optimized)
3. 🔨 Builds 10 C++ benchmarks (optimized)
4. ⚡ Runs each test 5 times
5. 📊 Generates comparison graphs
6. 📋 Shows summary table

**Total time**: 10-20 minutes (depending on your Mac)

## 📈 Results You'll Get

After running, check the `results/` folder:

```
results/
├── benchmark_results.json      # Raw timing data
├── benchmark_comparison.png    # Main comparison chart
├── category_comparison.png     # By category
└── speedup_comparison.png      # Rust vs C++ relative speed
```

## 🔧 Requirements

- **macOS** with Apple Silicon (M1/M2/M3/M4)
- **Rust** - [Install here](https://rustup.rs)
- **CMake** - `brew install cmake`
- **Python 3** - Already on macOS

## 📚 Documentation Guide

New to this? Read in this order:

1. **START_HERE.md** ← You are here!
2. **GETTING_STARTED.md** - Step-by-step guide
3. **README.md** - Full overview
4. **BENCHMARKS.md** - Test details
5. **USAGE.md** - Advanced usage

Want to extend? Read:
- **CONTRIBUTING.md** - How to add benchmarks
- **ARCHITECTURE.md** - Technical design

## 🎓 What You'll Learn

- Performance comparison: Rust vs C++
- Parallel programming patterns
- GPU compute with Metal
- Algorithm optimization techniques
- Apple Silicon features

## ❓ Common Questions

**Q: Do I need to know Rust or C++?**  
A: No! Just run the benchmarks and view the graphs.

**Q: How long does it take?**  
A: First run: ~10-20 minutes. Subsequent runs are faster.

**Q: Is this accurate?**  
A: Yes! Multiple runs, warm-up phases, and statistical analysis ensure accuracy.

**Q: Can I modify tests?**  
A: Absolutely! See CONTRIBUTING.md for how to add or modify benchmarks.

**Q: Why these specific tests?**  
A: They represent real-world workloads in different domains. See BENCHMARKS.md for details.

## 🚀 Let's Go!

Ready? Run this:

```bash
./quick_start.sh
```

Or if you prefer manual control:

```bash
pip3 install matplotlib numpy
python3 build.py
```

## 🎉 After Running

1. Open the graphs in `results/` folder
2. Check the console for summary table
3. Explore the source code in `rust/src/` and `cpp/src/`
4. Read BENCHMARKS.md to understand what each test measures

## 🆘 Need Help?

- **Setup issues?** → Check GETTING_STARTED.md
- **Understanding tests?** → Read BENCHMARKS.md
- **Want to customize?** → See USAGE.md
- **Adding features?** → Check CONTRIBUTING.md

## 🌟 Project Highlights

✨ **20 benchmark implementations** (10 Rust + 10 C++)  
✨ **Fully automated** - one command does everything  
✨ **Apple Silicon optimized** - Metal GPU, ARM NEON, native targeting  
✨ **Fair comparison** - identical algorithms, both optimized  
✨ **Beautiful graphs** - publication-quality visualizations  
✨ **Extensively documented** - 8 guide files, 2000+ lines  

---

**Ready to see which language wins on your Mac? Let's benchmark!** 🏁

```bash
./quick_start.sh
```

