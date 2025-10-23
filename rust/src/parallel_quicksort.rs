use rayon;
use std::time::Instant;

const ARRAY_SIZE: usize = 10_000_000;
const THRESHOLD: usize = 10_000;

fn parallel_quicksort(arr: &mut [i32]) {
    if arr.len() <= THRESHOLD {
        arr.sort_unstable();
        return;
    }
    
    if arr.len() <= 1 {
        return;
    }
    
    let pivot_idx = partition(arr);
    let (left, right) = arr.split_at_mut(pivot_idx);
    
    rayon::join(
        || parallel_quicksort(left),
        || parallel_quicksort(&mut right[1..])
    );
}

fn partition(arr: &mut [i32]) -> usize {
    let len = arr.len();
    let pivot = arr[len - 1];
    let mut i = 0;
    
    for j in 0..len - 1 {
        if arr[j] <= pivot {
            arr.swap(i, j);
            i += 1;
        }
    }
    
    arr.swap(i, len - 1);
    i
}

fn main() {
    // Generate random data
    let mut data: Vec<i32> = (0..ARRAY_SIZE)
        .map(|i| ((i * 1103515245 + 12345) % 2147483648) as i32)
        .collect();
    
    // Warm-up
    let mut warmup = data.clone();
    parallel_quicksort(&mut warmup);
    
    // Benchmark
    let start = Instant::now();
    parallel_quicksort(&mut data);
    let duration = start.elapsed();
    
    // Verify sort
    let is_sorted = data.windows(2).all(|w| w[0] <= w[1]);
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Sorted: {}", is_sorted);
}

