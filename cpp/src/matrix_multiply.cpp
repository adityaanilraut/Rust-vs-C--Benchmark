#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

const size_t SIZE = 1024;
const size_t NUM_THREADS = 8;

void matrix_multiply_row(const std::vector<std::vector<double>>& a,
                         const std::vector<std::vector<double>>& b,
                         std::vector<std::vector<double>>& result,
                         size_t start_row, size_t end_row) {
    for (size_t i = start_row; i < end_row; i++) {
        for (size_t j = 0; j < SIZE; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < SIZE; k++) {
                sum += a[i][k] * b[k][j];
            }
            result[i][j] = sum;
        }
    }
}

void matrix_multiply_parallel(const std::vector<std::vector<double>>& a,
                              const std::vector<std::vector<double>>& b,
                              std::vector<std::vector<double>>& result) {
    std::vector<std::thread> threads;
    size_t rows_per_thread = SIZE / NUM_THREADS;
    
    for (size_t t = 0; t < NUM_THREADS; t++) {
        size_t start_row = t * rows_per_thread;
        size_t end_row = (t == NUM_THREADS - 1) ? SIZE : (t + 1) * rows_per_thread;
        threads.emplace_back(matrix_multiply_row, std::ref(a), std::ref(b), std::ref(result), start_row, end_row);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    // Initialize matrices
    std::vector<std::vector<double>> a(SIZE, std::vector<double>(SIZE));
    std::vector<std::vector<double>> b(SIZE, std::vector<double>(SIZE));
    std::vector<std::vector<double>> result(SIZE, std::vector<double>(SIZE, 0.0));
    
    for (size_t i = 0; i < SIZE; i++) {
        for (size_t j = 0; j < SIZE; j++) {
            a[i][j] = static_cast<double>(i + j);
            b[i][j] = static_cast<double>(i * j);
        }
    }
    
    // Warm-up
    matrix_multiply_parallel(a, b, result);
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    matrix_multiply_parallel(a, b, result);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    
    // Checksum
    double checksum = 0.0;
    for (size_t j = 0; j < SIZE; j++) {
        checksum += result[0][j];
    }
    
    std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
    std::cerr << "Checksum: " << checksum << std::endl;
    
    return 0;
}

