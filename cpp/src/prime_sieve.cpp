#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <iomanip>

const size_t LIMIT = 100'000'000;

std::vector<size_t> sieve_of_eratosthenes(size_t limit) {
    std::vector<bool> is_prime(limit + 1, true);
    is_prime[0] = false;
    is_prime[1] = false;
    
    size_t sqrt_limit = static_cast<size_t>(std::sqrt(limit));
    
    for (size_t i = 2; i <= sqrt_limit; i++) {
        if (is_prime[i]) {
            for (size_t j = i * i; j <= limit; j += i) {
                is_prime[j] = false;
            }
        }
    }
    
    std::vector<size_t> primes;
    for (size_t i = 0; i <= limit; i++) {
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }
    
    return primes;
}

int main() {
    // Warm-up with smaller limit
    auto warmup = sieve_of_eratosthenes(1'000'000);
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    auto primes = sieve_of_eratosthenes(LIMIT);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    
    std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
    std::cerr << "Number of primes: " << primes.size() << std::endl;
    
    return 0;
}

