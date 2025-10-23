#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <chrono>
#include <iomanip>

using Complex = std::complex<double>;
const double PI = 3.14159265358979323846;

void fft(std::vector<Complex>& x) {
    size_t N = x.size();
    if (N <= 1) return;
    
    // Divide
    std::vector<Complex> even(N / 2);
    std::vector<Complex> odd(N / 2);
    for (size_t i = 0; i < N / 2; i++) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }
    
    // Conquer
    fft(even);
    fft(odd);
    
    // Combine
    for (size_t k = 0; k < N / 2; k++) {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

int main() {
    const size_t SIZE = 16'777'216; // 2^24
    
    // Generate input signal
    std::vector<Complex> buffer(SIZE);
    for (size_t i = 0; i < SIZE; i++) {
        double t = static_cast<double>(i) / SIZE;
        double signal = std::sin(2.0 * PI * 50.0 * t) + std::sin(2.0 * PI * 120.0 * t);
        buffer[i] = Complex(signal, 0.0);
    }
    
    // Warm-up with smaller size
    std::vector<Complex> warmup(1024);
    for (size_t i = 0; i < 1024; i++) {
        warmup[i] = Complex(static_cast<double>(i), 0.0);
    }
    fft(warmup);
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    fft(buffer);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    
    // Checksum
    double checksum = 0.0;
    for (size_t i = 0; i < 1000; i++) {
        checksum += std::abs(buffer[i]);
    }
    
    std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
    std::cerr << "Checksum: " << checksum << std::endl;
    
    return 0;
}

