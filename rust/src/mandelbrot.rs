use rayon::prelude::*;
use std::time::Instant;

const WIDTH: usize = 4096;
const HEIGHT: usize = 4096;
const MAX_ITER: u32 = 1000;

fn mandelbrot_point(cx: f64, cy: f64) -> u32 {
    let mut x = 0.0;
    let mut y = 0.0;
    let mut iteration = 0;
    
    while x * x + y * y <= 4.0 && iteration < MAX_ITER {
        let xtemp = x * x - y * y + cx;
        y = 2.0 * x * y + cy;
        x = xtemp;
        iteration += 1;
    }
    
    iteration
}

fn compute_mandelbrot() -> Vec<u32> {
    let min_re = -2.5;
    let max_re = 1.0;
    let min_im = -1.0;
    let max_im = 1.0;
    
    (0..HEIGHT).into_par_iter().flat_map(|y| {
        (0..WIDTH).into_par_iter().map(move |x| {
            let cx = min_re + (x as f64 / WIDTH as f64) * (max_re - min_re);
            let cy = min_im + (y as f64 / HEIGHT as f64) * (max_im - min_im);
            mandelbrot_point(cx, cy)
        }).collect::<Vec<_>>()
    }).collect()
}

fn main() {
    // Warm-up
    let _ = compute_mandelbrot();
    
    // Benchmark
    let start = Instant::now();
    let result = compute_mandelbrot();
    let duration = start.elapsed();
    
    // Checksum
    let checksum: u64 = result.iter().take(1000).map(|&x| x as u64).sum();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Checksum: {}", checksum);
}

