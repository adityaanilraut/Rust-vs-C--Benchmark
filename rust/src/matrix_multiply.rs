use rayon::prelude::*;
use std::time::Instant;

const SIZE: usize = 1024;

fn matrix_multiply_parallel(a: &Vec<Vec<f64>>, b: &Vec<Vec<f64>>) -> Vec<Vec<f64>> {
    let mut result = vec![vec![0.0; SIZE]; SIZE];
    
    result.par_iter_mut().enumerate().for_each(|(i, row)| {
        for j in 0..SIZE {
            let mut sum = 0.0;
            for k in 0..SIZE {
                sum += a[i][k] * b[k][j];
            }
            row[j] = sum;
        }
    });
    
    result
}

fn main() {
    // Initialize matrices
    let a: Vec<Vec<f64>> = (0..SIZE)
        .map(|i| (0..SIZE).map(|j| (i + j) as f64).collect())
        .collect();
    
    let b: Vec<Vec<f64>> = (0..SIZE)
        .map(|i| (0..SIZE).map(|j| (i * j) as f64).collect())
        .collect();
    
    // Warm-up
    let _ = matrix_multiply_parallel(&a, &b);
    
    // Benchmark
    let start = Instant::now();
    let result = matrix_multiply_parallel(&a, &b);
    let duration = start.elapsed();
    
    // Prevent optimization
    let checksum: f64 = result[0].iter().sum();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Checksum: {}", checksum);
}

