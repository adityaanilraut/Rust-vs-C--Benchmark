use rustfft::{FftPlanner, num_complex::Complex};
use std::time::Instant;

const SIZE: usize = 16_777_216; // 2^24

fn main() {
    let mut planner = FftPlanner::<f64>::new();
    let fft = planner.plan_fft_forward(SIZE);
    
    // Generate input signal
    let mut buffer: Vec<Complex<f64>> = (0..SIZE)
        .map(|i| {
            let t = i as f64 / SIZE as f64;
            let signal = (2.0 * std::f64::consts::PI * 50.0 * t).sin()
                       + (2.0 * std::f64::consts::PI * 120.0 * t).sin();
            Complex::new(signal, 0.0)
        })
        .collect();
    
    // Warm-up
    let mut warmup = buffer.clone();
    fft.process(&mut warmup);
    
    // Benchmark
    let start = Instant::now();
    fft.process(&mut buffer);
    let duration = start.elapsed();
    
    // Checksum
    let checksum: f64 = buffer.iter().take(1000).map(|c| c.norm()).sum();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Checksum: {}", checksum);
}

