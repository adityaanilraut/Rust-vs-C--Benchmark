#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cmath>

const size_t MATRIX_SIZE = 2048;  // 2048x2048 matrix
const size_t TOTAL_ELEMENTS = MATRIX_SIZE * MATRIX_SIZE;
const size_t ITERATIONS = 10;

int main() {
    @autoreleasepool {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            std::cerr << "Metal is not supported on this device" << std::endl;
            return 1;
        }
        
        NSString* shaderSource = @R"(
            #include <metal_stdlib>
            using namespace metal;
            
            // Heavy compute shader: matrix element-wise operations with trigonometric functions
            kernel void heavy_compute(device const float* a [[buffer(0)]],
                                     device const float* b [[buffer(1)]],
                                     device float* result [[buffer(2)]],
                                     constant uint& matrix_size [[buffer(3)]],
                                     uint2 gid [[thread_position_in_grid]])
            {
                uint idx = gid.y * matrix_size + gid.x;
                
                // Perform multiple heavy computations
                float val_a = a[idx];
                float val_b = b[idx];
                
                float sum = 0.0;
                for (int i = 0; i < 50; i++) {
                    float t = val_a * float(i) * 0.01 + val_b;
                    sum += sin(t) * cos(val_b * float(i) * 0.01) + 
                           tan(val_a * 0.1) * exp(-val_b * 0.001) +
                           sqrt(abs(t)) * log(abs(t) + 1.0);
                }
                
                // Additional matrix-style computation
                float accumulator = 0.0;
                uint row = gid.y;
                for (uint col = 0; col < matrix_size; col++) {
                    uint idx2 = row * matrix_size + col;
                    accumulator += a[idx2] * b[idx2] * 0.0001;
                }
                
                result[idx] = sum + accumulator;
            }
        )";
        
        NSError* error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:shaderSource options:nil error:&error];
        if (!library) {
            std::cerr << "Failed to compile shader: " << [[error localizedDescription] UTF8String] << std::endl;
            return 1;
        }
        
        id<MTLFunction> kernel = [library newFunctionWithName:@"heavy_compute"];
        id<MTLComputePipelineState> pipeline = [device newComputePipelineStateWithFunction:kernel error:&error];
        if (!pipeline) {
            std::cerr << "Failed to create pipeline: " << [[error localizedDescription] UTF8String] << std::endl;
            return 1;
        }
        
        // Create input data - large matrices
        std::vector<float> input_a(TOTAL_ELEMENTS);
        std::vector<float> input_b(TOTAL_ELEMENTS);
        
        for (size_t i = 0; i < TOTAL_ELEMENTS; i++) {
            input_a[i] = std::sin(static_cast<float>(i) * 0.001f);
            input_b[i] = std::cos(static_cast<float>(i) * 0.002f);
        }
        
        id<MTLBuffer> buffer_a = [device newBufferWithBytes:input_a.data()
                                                     length:TOTAL_ELEMENTS * sizeof(float)
                                                    options:MTLResourceStorageModeShared];
        
        id<MTLBuffer> buffer_b = [device newBufferWithBytes:input_b.data()
                                                     length:TOTAL_ELEMENTS * sizeof(float)
                                                    options:MTLResourceStorageModeShared];
        
        id<MTLBuffer> buffer_result = [device newBufferWithLength:TOTAL_ELEMENTS * sizeof(float)
                                                          options:MTLResourceStorageModeShared];
        
        uint32_t matrix_size_val = static_cast<uint32_t>(MATRIX_SIZE);
        id<MTLBuffer> buffer_size = [device newBufferWithBytes:&matrix_size_val
                                                        length:sizeof(uint32_t)
                                                       options:MTLResourceStorageModeShared];
        
        id<MTLCommandQueue> commandQueue = [device newCommandQueue];
        
        // Warm-up
        {
            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
            
            [encoder setComputePipelineState:pipeline];
            [encoder setBuffer:buffer_a offset:0 atIndex:0];
            [encoder setBuffer:buffer_b offset:0 atIndex:1];
            [encoder setBuffer:buffer_result offset:0 atIndex:2];
            [encoder setBuffer:buffer_size offset:0 atIndex:3];
            
            MTLSize gridSize = MTLSizeMake(MATRIX_SIZE, MATRIX_SIZE, 1);
            MTLSize threadgroupSize = MTLSizeMake(16, 16, 1);
            [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];
            [encoder endEncoding];
            
            [commandBuffer commit];
            [commandBuffer waitUntilCompleted];
        }
        
        // Benchmark - run multiple iterations
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t iter = 0; iter < ITERATIONS; iter++) {
            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
            
            [encoder setComputePipelineState:pipeline];
            [encoder setBuffer:buffer_a offset:0 atIndex:0];
            [encoder setBuffer:buffer_b offset:0 atIndex:1];
            [encoder setBuffer:buffer_result offset:0 atIndex:2];
            [encoder setBuffer:buffer_size offset:0 atIndex:3];
            
            MTLSize gridSize = MTLSizeMake(MATRIX_SIZE, MATRIX_SIZE, 1);
            MTLSize threadgroupSize = MTLSizeMake(16, 16, 1);
            [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];
            [encoder endEncoding];
            
            [commandBuffer commit];
            [commandBuffer waitUntilCompleted];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        
        // Get result and checksum
        float* result_ptr = static_cast<float*>([buffer_result contents]);
        float checksum = 0.0f;
        for (size_t i = 0; i < TOTAL_ELEMENTS; i += 1000) {
            checksum += result_ptr[i];
        }
        
        std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
        std::cerr << "Checksum: " << checksum << std::endl;
    }
    
    return 0;
}

