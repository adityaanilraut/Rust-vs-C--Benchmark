cmake_minimum_required(VERSION 3.20)
project(cpp_benchmarks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -std=c++20")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# Find Metal framework
find_library(METAL_LIBRARY Metal)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(METALKIT_LIBRARY MetalKit)

# Create executables for each benchmark
add_executable(matrix_multiply src/matrix_multiply.cpp)
target_link_libraries(matrix_multiply pthread)

add_executable(parallel_quicksort src/parallel_quicksort.cpp)
target_link_libraries(parallel_quicksort pthread)

add_executable(thread_pool src/thread_pool.cpp)
target_link_libraries(thread_pool pthread)

add_executable(ray_tracer src/ray_tracer.cpp)
target_link_libraries(ray_tracer pthread)

add_executable(mandelbrot src/mandelbrot.cpp)
target_link_libraries(mandelbrot pthread)

add_executable(metal_compute src/metal_compute.mm)
set_source_files_properties(src/metal_compute.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
target_link_libraries(metal_compute ${METAL_LIBRARY} ${FOUNDATION_LIBRARY} ${METALKIT_LIBRARY})

add_executable(prime_sieve src/prime_sieve.cpp)

add_executable(fft src/fft.cpp)
target_link_libraries(fft pthread)

add_executable(sha256 src/sha256.cpp)

add_executable(json_parse src/json_parse.cpp)

