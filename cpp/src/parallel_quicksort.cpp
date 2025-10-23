#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>

const size_t ARRAY_SIZE = 10'000'000;
const size_t THRESHOLD = 10'000;

size_t partition(std::vector<int>& arr, size_t low, size_t high) {
    int pivot = arr[high];
    size_t i = low;
    
    for (size_t j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            std::swap(arr[i], arr[j]);
            i++;
        }
    }
    
    std::swap(arr[i], arr[high]);
    return i;
}

void parallel_quicksort(std::vector<int>& arr, size_t low, size_t high) {
    if (low < high) {
        size_t len = high - low + 1;
        
        if (len <= THRESHOLD) {
            std::sort(arr.begin() + low, arr.begin() + high + 1);
            return;
        }
        
        size_t pi = partition(arr, low, high);
        
        if (pi > 0) {
            std::thread left_thread(parallel_quicksort, std::ref(arr), low, pi - 1);
            if (pi + 1 <= high) {
                parallel_quicksort(arr, pi + 1, high);
            }
            left_thread.join();
        } else {
            if (pi + 1 <= high) {
                parallel_quicksort(arr, pi + 1, high);
            }
        }
    }
}

int main() {
    // Generate data
    std::vector<int> data(ARRAY_SIZE);
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        data[i] = static_cast<int>((i * 1103515245 + 12345) % 2147483648);
    }
    
    // Warm-up
    std::vector<int> warmup = data;
    parallel_quicksort(warmup, 0, warmup.size() - 1);
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    parallel_quicksort(data, 0, data.size() - 1);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    
    // Verify sort
    bool is_sorted = std::is_sorted(data.begin(), data.end());
    
    std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
    std::cerr << "Sorted: " << std::boolalpha << is_sorted << std::endl;
    
    return 0;
}

