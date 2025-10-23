use sha2::{Sha256, Digest};
use std::time::Instant;

const DATA_SIZE: usize = 100_000_000;
const CHUNK_SIZE: usize = 1024;

fn main() {
    // Generate data
    let data: Vec<u8> = (0..DATA_SIZE).map(|i| (i % 256) as u8).collect();
    
    // Warm-up
    {
        let mut hasher = Sha256::new();
        hasher.update(&data[..1_000_000]);
        let _ = hasher.finalize();
    }
    
    // Benchmark
    let start = Instant::now();
    
    let mut hasher = Sha256::new();
    for chunk in data.chunks(CHUNK_SIZE) {
        hasher.update(chunk);
    }
    let result = hasher.finalize();
    
    let duration = start.elapsed();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Hash: {:x}", result);
}

