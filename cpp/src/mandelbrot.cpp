#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

const size_t WIDTH = 4096;
const size_t HEIGHT = 4096;
const uint32_t MAX_ITER = 1000;
const size_t NUM_THREADS = 8;

uint32_t mandelbrot_point(double cx, double cy) {
    double x = 0.0;
    double y = 0.0;
    uint32_t iteration = 0;
    
    while (x * x + y * y <= 4.0 && iteration < MAX_ITER) {
        double xtemp = x * x - y * y + cx;
        y = 2.0 * x * y + cy;
        x = xtemp;
        iteration++;
    }
    
    return iteration;
}

void compute_section(std::vector<uint32_t>& result, size_t start_row, size_t end_row) {
    const double min_re = -2.5;
    const double max_re = 1.0;
    const double min_im = -1.0;
    const double max_im = 1.0;
    
    for (size_t y = start_row; y < end_row; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            double cx = min_re + (static_cast<double>(x) / WIDTH) * (max_re - min_re);
            double cy = min_im + (static_cast<double>(y) / HEIGHT) * (max_im - min_im);
            result[y * WIDTH + x] = mandelbrot_point(cx, cy);
        }
    }
}

int main() {
    std::vector<uint32_t> result(WIDTH * HEIGHT);
    
    // Warm-up
    compute_section(result, 0, 10);
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    size_t rows_per_thread = HEIGHT / NUM_THREADS;
    
    for (size_t t = 0; t < NUM_THREADS; t++) {
        size_t start_row = t * rows_per_thread;
        size_t end_row = (t == NUM_THREADS - 1) ? HEIGHT : (t + 1) * rows_per_thread;
        threads.emplace_back(compute_section, std::ref(result), start_row, end_row);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    
    // Checksum
    uint64_t checksum = 0;
    for (size_t i = 0; i < 1000; i++) {
        checksum += result[i];
    }
    
    std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
    std::cerr << "Checksum: " << checksum << std::endl;
    
    return 0;
}

