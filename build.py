#!/usr/bin/env python3
"""
Rust vs C++ Benchmark Runner
Builds, runs, and plots performance comparison graphs
"""

import subprocess
import json
import time
import os
import sys
from pathlib import Path
from typing import Dict, List, Tuple

# Configuration
NUM_RUNS = 5
BENCHMARKS = [
    "matrix_multiply",
    "parallel_quicksort", 
    "thread_pool",
    "ray_tracer",
    "mandelbrot",
    "metal_compute",
    "prime_sieve",
    "fft",
    "sha256",
    "json_parse"
]

CATEGORIES = {
    "Parallelization": ["matrix_multiply", "parallel_quicksort", "thread_pool"],
    "Graphics": ["ray_tracer", "mandelbrot", "metal_compute"],
    "Heavy Compute": ["prime_sieve", "fft", "sha256"],
    "Other": ["json_parse"]
}

def run_command(cmd: List[str], cwd: str = None) -> Tuple[bool, str, str]:
    """Run a command and return success status, stdout, stderr"""
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
            timeout=300
        )
        return result.returncode == 0, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return False, "", "Timeout"
    except Exception as e:
        return False, "", str(e)

def build_rust() -> bool:
    """Build all Rust benchmarks"""
    print("Building Rust benchmarks...")
    success, stdout, stderr = run_command(
        ["cargo", "build", "--release"],
        cwd="rust"
    )
    
    if not success:
        print(f"❌ Rust build failed:")
        print(stderr)
        return False
    
    print("✅ Rust build successful")
    return True

def build_cpp() -> bool:
    """Build all C++ benchmarks"""
    print("Building C++ benchmarks...")
    
    # Create build directory
    build_dir = Path("cpp/build")
    build_dir.mkdir(exist_ok=True)
    
    # Run CMake
    success, stdout, stderr = run_command(
        ["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"],
        cwd=str(build_dir)
    )
    
    if not success:
        print(f"❌ CMake configuration failed:")
        print(stderr)
        return False
    
    # Build
    success, stdout, stderr = run_command(
        ["cmake", "--build", ".", "--config", "Release"],
        cwd=str(build_dir)
    )
    
    if not success:
        print(f"❌ C++ build failed:")
        print(stderr)
        return False
    
    print("✅ C++ build successful")
    return True

def run_benchmark(lang: str, benchmark: str) -> float:
    """Run a single benchmark and return execution time"""
    if lang == "rust":
        executable = f"rust/target/release/{benchmark}"
    else:  # cpp
        executable = f"cpp/build/{benchmark}"
    
    success, stdout, stderr = run_command([executable])
    
    if not success:
        print(f"  ⚠️  {lang}/{benchmark} failed: {stderr}")
        return -1.0
    
    try:
        return float(stdout.strip())
    except ValueError:
        print(f"  ⚠️  {lang}/{benchmark} returned invalid output: {stdout}")
        return -1.0

def run_all_benchmarks() -> Dict:
    """Run all benchmarks multiple times and collect results"""
    results = {
        "rust": {},
        "cpp": {}
    }
    
    total_tests = len(BENCHMARKS) * 2 * NUM_RUNS
    current_test = 0
    
    for benchmark in BENCHMARKS:
        print(f"\n{'='*60}")
        print(f"Running: {benchmark}")
        print(f"{'='*60}")
        
        results["rust"][benchmark] = []
        results["cpp"][benchmark] = []
        
        for run in range(NUM_RUNS):
            current_test += 1
            
            # Run Rust
            print(f"[{current_test}/{total_tests}] Rust {benchmark} (run {run+1}/{NUM_RUNS})...", end=" ", flush=True)
            time_rust = run_benchmark("rust", benchmark)
            if time_rust >= 0:
                results["rust"][benchmark].append(time_rust)
                print(f"✓ {time_rust:.4f}s")
            else:
                print("✗ Failed")
            
            current_test += 1
            
            # Run C++
            print(f"[{current_test}/{total_tests}] C++  {benchmark} (run {run+1}/{NUM_RUNS})...", end=" ", flush=True)
            time_cpp = run_benchmark("cpp", benchmark)
            if time_cpp >= 0:
                results["cpp"][benchmark].append(time_cpp)
                print(f"✓ {time_cpp:.4f}s")
            else:
                print("✗ Failed")
    
    return results

def calculate_statistics(times: List[float]) -> Dict:
    """Calculate mean, min, max, std from list of times"""
    if not times:
        return {"mean": 0, "min": 0, "max": 0, "std": 0}
    
    import statistics
    return {
        "mean": statistics.mean(times),
        "min": min(times),
        "max": max(times),
        "std": statistics.stdev(times) if len(times) > 1 else 0
    }

def save_results(results: Dict, filename: str = "results/benchmark_results.json"):
    """Save results to JSON file"""
    Path("results").mkdir(exist_ok=True)
    
    # Calculate statistics
    processed = {
        "rust": {},
        "cpp": {}
    }
    
    for lang in ["rust", "cpp"]:
        for benchmark, times in results[lang].items():
            processed[lang][benchmark] = {
                "times": times,
                "stats": calculate_statistics(times)
            }
    
    with open(filename, "w") as f:
        json.dump(processed, f, indent=2)
    
    print(f"\n✅ Results saved to {filename}")

def print_summary(results: Dict):
    """Print summary of results"""
    print("\n" + "="*80)
    print("BENCHMARK SUMMARY")
    print("="*80)
    print(f"{'Benchmark':<25} {'Rust (avg)':<15} {'C++ (avg)':<15} {'Winner':<10} {'Speedup':<10}")
    print("-"*80)
    
    rust_wins = 0
    cpp_wins = 0
    
    for benchmark in BENCHMARKS:
        rust_times = results["rust"].get(benchmark, [])
        cpp_times = results["cpp"].get(benchmark, [])
        
        if not rust_times or not cpp_times:
            continue
        
        rust_avg = sum(rust_times) / len(rust_times)
        cpp_avg = sum(cpp_times) / len(cpp_times)
        
        if rust_avg < cpp_avg:
            winner = "Rust"
            speedup = cpp_avg / rust_avg
            rust_wins += 1
        else:
            winner = "C++"
            speedup = rust_avg / cpp_avg
            cpp_wins += 1
        
        print(f"{benchmark:<25} {rust_avg:>10.4f}s    {cpp_avg:>10.4f}s    {winner:<10} {speedup:>6.2f}x")
    
    print("-"*80)
    print(f"Total: Rust wins: {rust_wins}, C++ wins: {cpp_wins}")
    print("="*80)

def plot_results(results_file: str = "results/benchmark_results.json"):
    """Generate comparison plots"""
    try:
        import matplotlib.pyplot as plt
        import numpy as np
    except ImportError:
        print("\n⚠️  matplotlib not installed. Install with: pip install matplotlib numpy")
        print("Skipping plot generation.")
        return
    
    with open(results_file, "r") as f:
        data = json.load(f)
    
    # Plot 1: Individual benchmark comparison
    fig, ax = plt.subplots(figsize=(14, 8))
    
    benchmarks = BENCHMARKS
    rust_means = [data["rust"][b]["stats"]["mean"] for b in benchmarks if b in data["rust"]]
    cpp_means = [data["cpp"][b]["stats"]["mean"] for b in benchmarks if b in data["cpp"]]
    
    x = np.arange(len(benchmarks))
    width = 0.35
    
    bars1 = ax.bar(x - width/2, rust_means, width, label='Rust', color='#CE422B')
    bars2 = ax.bar(x + width/2, cpp_means, width, label='C++', color='#00599C')
    
    ax.set_xlabel('Benchmark', fontsize=12, fontweight='bold')
    ax.set_ylabel('Time (seconds)', fontsize=12, fontweight='bold')
    ax.set_title('Rust vs C++ Performance Comparison (Apple Silicon)', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(benchmarks, rotation=45, ha='right')
    ax.legend(fontsize=11)
    ax.grid(axis='y', alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('results/benchmark_comparison.png', dpi=300, bbox_inches='tight')
    print("✅ Saved: results/benchmark_comparison.png")
    
    # Plot 2: Category comparison
    fig, ax = plt.subplots(figsize=(12, 6))
    
    categories = list(CATEGORIES.keys())
    rust_category_times = []
    cpp_category_times = []
    
    for category, benches in CATEGORIES.items():
        rust_total = sum(data["rust"][b]["stats"]["mean"] for b in benches if b in data["rust"])
        cpp_total = sum(data["cpp"][b]["stats"]["mean"] for b in benches if b in data["cpp"])
        rust_category_times.append(rust_total)
        cpp_category_times.append(cpp_total)
    
    x = np.arange(len(categories))
    width = 0.35
    
    bars1 = ax.bar(x - width/2, rust_category_times, width, label='Rust', color='#CE422B')
    bars2 = ax.bar(x + width/2, cpp_category_times, width, label='C++', color='#00599C')
    
    ax.set_xlabel('Category', fontsize=12, fontweight='bold')
    ax.set_ylabel('Total Time (seconds)', fontsize=12, fontweight='bold')
    ax.set_title('Performance by Category (Apple Silicon)', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(categories)
    ax.legend(fontsize=11)
    ax.grid(axis='y', alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('results/category_comparison.png', dpi=300, bbox_inches='tight')
    print("✅ Saved: results/category_comparison.png")
    
    # Plot 3: Speedup chart
    fig, ax = plt.subplots(figsize=(14, 8))
    
    speedups = []
    colors = []
    for b in benchmarks:
        if b in data["rust"] and b in data["cpp"]:
            rust_mean = data["rust"][b]["stats"]["mean"]
            cpp_mean = data["cpp"][b]["stats"]["mean"]
            speedup = cpp_mean / rust_mean
            speedups.append(speedup)
            colors.append('#CE422B' if speedup > 1 else '#00599C')
    
    bars = ax.barh(benchmarks, speedups, color=colors)
    ax.axvline(x=1, color='black', linestyle='--', linewidth=1, alpha=0.5)
    ax.set_xlabel('Speedup (C++ time / Rust time)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Benchmark', fontsize=12, fontweight='bold')
    ax.set_title('Rust vs C++ Speedup (>1 = Rust faster, <1 = C++ faster)', fontsize=14, fontweight='bold')
    ax.grid(axis='x', alpha=0.3)
    
    # Add value labels on bars
    for i, (bar, speedup) in enumerate(zip(bars, speedups)):
        width = bar.get_width()
        label_x = width + 0.05 if width > 1 else width - 0.05
        ha = 'left' if width > 1 else 'right'
        ax.text(label_x, bar.get_y() + bar.get_height()/2, f'{speedup:.2f}x',
                ha=ha, va='center', fontsize=9, fontweight='bold')
    
    plt.tight_layout()
    plt.savefig('results/speedup_comparison.png', dpi=300, bbox_inches='tight')
    print("✅ Saved: results/speedup_comparison.png")
    
    plt.close('all')

def main():
    """Main execution"""
    print("="*80)
    print("Rust vs C++ Benchmark Suite (Apple Silicon Optimized)")
    print("="*80)
    
    # Build
    if not build_rust():
        print("\n❌ Failed to build Rust benchmarks")
        sys.exit(1)
    
    if not build_cpp():
        print("\n❌ Failed to build C++ benchmarks")
        sys.exit(1)
    
    print("\n✅ All builds successful!")
    
    # Run benchmarks
    print(f"\nRunning benchmarks ({NUM_RUNS} runs each)...")
    results = run_all_benchmarks()
    
    # Save results
    save_results(results)
    
    # Print summary
    print_summary(results)
    
    # Generate plots
    print("\nGenerating plots...")
    plot_results()
    
    print("\n" + "="*80)
    print("✅ Benchmark complete!")
    print("="*80)
    print("\nResults saved in:")
    print("  - results/benchmark_results.json")
    print("  - results/benchmark_comparison.png")
    print("  - results/category_comparison.png")
    print("  - results/speedup_comparison.png")

if __name__ == "__main__":
    main()

