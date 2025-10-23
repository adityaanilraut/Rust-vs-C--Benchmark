use std::time::Instant;

const LIMIT: usize = 100_000_000;

fn sieve_of_eratosthenes(limit: usize) -> Vec<usize> {
    let mut is_prime = vec![true; limit + 1];
    is_prime[0] = false;
    is_prime[1] = false;
    
    let sqrt_limit = (limit as f64).sqrt() as usize;
    
    for i in 2..=sqrt_limit {
        if is_prime[i] {
            let mut j = i * i;
            while j <= limit {
                is_prime[j] = false;
                j += i;
            }
        }
    }
    
    is_prime.iter()
        .enumerate()
        .filter(|(_, &is_p)| is_p)
        .map(|(i, _)| i)
        .collect()
}

fn main() {
    // Warm-up with smaller limit
    let _ = sieve_of_eratosthenes(1_000_000);
    
    // Benchmark
    let start = Instant::now();
    let primes = sieve_of_eratosthenes(LIMIT);
    let duration = start.elapsed();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Number of primes: {}", primes.len());
}

