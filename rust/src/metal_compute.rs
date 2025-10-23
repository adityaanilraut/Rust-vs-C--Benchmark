use metal::*;
use std::time::Instant;

const MATRIX_SIZE: usize = 2048;  // 2048x2048 matrix
const TOTAL_ELEMENTS: usize = MATRIX_SIZE * MATRIX_SIZE;
const ITERATIONS: usize = 10;

fn main() {
    // Initialize Metal
    let device = Device::system_default().expect("No Metal device found");
    
    let shader_source = r#"
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
    "#;
    
    let compile_options = CompileOptions::new();
    let library = device.new_library_with_source(shader_source, &compile_options)
        .expect("Failed to compile shader");
    
    let kernel = library.get_function("heavy_compute", None)
        .expect("Failed to get kernel function");
    
    let pipeline = device.new_compute_pipeline_state_with_function(&kernel)
        .expect("Failed to create pipeline");
    
    // Create input data - large matrices
    let input_a: Vec<f32> = (0..TOTAL_ELEMENTS).map(|i| (i as f32 * 0.001).sin()).collect();
    let input_b: Vec<f32> = (0..TOTAL_ELEMENTS).map(|i| (i as f32 * 0.002).cos()).collect();
    
    let buffer_a = device.new_buffer_with_data(
        input_a.as_ptr() as *const _,
        (TOTAL_ELEMENTS * std::mem::size_of::<f32>()) as u64,
        MTLResourceOptions::StorageModeShared,
    );
    
    let buffer_b = device.new_buffer_with_data(
        input_b.as_ptr() as *const _,
        (TOTAL_ELEMENTS * std::mem::size_of::<f32>()) as u64,
        MTLResourceOptions::StorageModeShared,
    );
    
    let buffer_result = device.new_buffer(
        (TOTAL_ELEMENTS * std::mem::size_of::<f32>()) as u64,
        MTLResourceOptions::StorageModeShared,
    );
    
    let matrix_size_u32 = MATRIX_SIZE as u32;
    let buffer_size = device.new_buffer_with_data(
        &matrix_size_u32 as *const u32 as *const _,
        std::mem::size_of::<u32>() as u64,
        MTLResourceOptions::StorageModeShared,
    );
    
    // Warm-up
    {
        let command_queue = device.new_command_queue();
        let command_buffer = command_queue.new_command_buffer();
        let encoder = command_buffer.new_compute_command_encoder();
        
        encoder.set_compute_pipeline_state(&pipeline);
        encoder.set_buffer(0, Some(&buffer_a), 0);
        encoder.set_buffer(1, Some(&buffer_b), 0);
        encoder.set_buffer(2, Some(&buffer_result), 0);
        encoder.set_buffer(3, Some(&buffer_size), 0);
        
        let grid_size = MTLSize::new(MATRIX_SIZE as u64, MATRIX_SIZE as u64, 1);
        let threadgroup_size = MTLSize::new(16, 16, 1);
        encoder.dispatch_threads(grid_size, threadgroup_size);
        encoder.end_encoding();
        
        command_buffer.commit();
        command_buffer.wait_until_completed();
    }
    
    // Benchmark - run multiple iterations
    let start = Instant::now();
    let command_queue = device.new_command_queue();
    
    for _ in 0..ITERATIONS {
        let command_buffer = command_queue.new_command_buffer();
        let encoder = command_buffer.new_compute_command_encoder();
        
        encoder.set_compute_pipeline_state(&pipeline);
        encoder.set_buffer(0, Some(&buffer_a), 0);
        encoder.set_buffer(1, Some(&buffer_b), 0);
        encoder.set_buffer(2, Some(&buffer_result), 0);
        encoder.set_buffer(3, Some(&buffer_size), 0);
        
        let grid_size = MTLSize::new(MATRIX_SIZE as u64, MATRIX_SIZE as u64, 1);
        let threadgroup_size = MTLSize::new(16, 16, 1);
        encoder.dispatch_threads(grid_size, threadgroup_size);
        encoder.end_encoding();
        
        command_buffer.commit();
        command_buffer.wait_until_completed();
    }
    
    let duration = start.elapsed();
    
    // Get result and checksum
    let result_ptr = buffer_result.contents() as *const f32;
    let result_slice = unsafe { std::slice::from_raw_parts(result_ptr, TOTAL_ELEMENTS) };
    let checksum: f32 = result_slice.iter().step_by(1000).sum();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Checksum: {}", checksum);
}

