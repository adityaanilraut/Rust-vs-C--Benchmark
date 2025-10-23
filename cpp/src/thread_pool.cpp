#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <iomanip>
#include <atomic>

const size_t NUM_TASKS = 100'000;
const size_t NUM_WORKERS = 8;

class ThreadPool {
public:
    ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; i++) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        
                        if (stop && tasks.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    
    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }
    
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

uint64_t heavy_computation(size_t n) {
    uint64_t result = 0;
    for (size_t i = 0; i < 1000; i++) {
        result += static_cast<uint64_t>(n) * i;
    }
    return result;
}

int main() {
    std::atomic<uint64_t> counter(0);
    
    // Warm-up
    {
        ThreadPool pool(NUM_WORKERS);
        std::atomic<uint64_t> counter_warmup(0);
        for (size_t i = 0; i < 100; i++) {
            pool.enqueue([i, &counter_warmup] {
                uint64_t result = heavy_computation(i);
                counter_warmup += result;
            });
        }
    }
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    {
        ThreadPool pool(NUM_WORKERS);
        
        for (size_t i = 0; i < NUM_TASKS; i++) {
            pool.enqueue([i, &counter] {
                uint64_t result = heavy_computation(i);
                counter += result;
            });
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    
    std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
    std::cerr << "Final count: " << counter.load() << std::endl;
    
    return 0;
}

