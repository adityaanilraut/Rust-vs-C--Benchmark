use std::sync::{Arc, Mutex, mpsc};
use std::thread;
use std::time::Instant;

const NUM_TASKS: usize = 100_000;
const NUM_WORKERS: usize = 8;

struct ThreadPool {
    workers: Vec<thread::JoinHandle<()>>,
    sender: Option<mpsc::Sender<Task>>,
}

type Task = Box<dyn FnOnce() + Send + 'static>;

impl ThreadPool {
    fn new(size: usize) -> ThreadPool {
        let (sender, receiver) = mpsc::channel::<Task>();
        let receiver = Arc::new(Mutex::new(receiver));
        let mut workers = Vec::with_capacity(size);
        
        for _ in 0..size {
            let receiver = Arc::clone(&receiver);
            let handle = thread::spawn(move || {
                loop {
                    let task = receiver.lock().unwrap().recv();
                    match task {
                        Ok(task) => task(),
                        Err(_) => break,
                    }
                }
            });
            workers.push(handle);
        }
        
        ThreadPool { workers, sender: Some(sender) }
    }
    
    fn execute<F>(&self, f: F)
    where
        F: FnOnce() + Send + 'static,
    {
        self.sender.as_ref().unwrap().send(Box::new(f)).unwrap();
    }
}

impl Drop for ThreadPool {
    fn drop(&mut self) {
        // Drop the sender to close the channel
        // This will cause recv() to return Err and workers to exit
        drop(self.sender.take());
        
        // Now join all workers
        while let Some(worker) = self.workers.pop() {
            worker.join().unwrap();
        }
    }
}

fn heavy_computation(n: usize) -> u64 {
    let mut result = 0u64;
    for i in 0..1000 {
        result = result.wrapping_add((n as u64).wrapping_mul(i));
    }
    result
}

fn main() {
    let counter = Arc::new(Mutex::new(0u64));
    
    // Warm-up
    {
        let pool = ThreadPool::new(NUM_WORKERS);
        let counter_warmup = Arc::new(Mutex::new(0u64));
        for i in 0..100 {
            let counter_clone = Arc::clone(&counter_warmup);
            pool.execute(move || {
                let result = heavy_computation(i);
                let mut count = counter_clone.lock().unwrap();
                *count = count.wrapping_add(result);
            });
        }
    }
    
    // Benchmark
    let start = Instant::now();
    {
        let pool = ThreadPool::new(NUM_WORKERS);
        
        for i in 0..NUM_TASKS {
            let counter_clone = Arc::clone(&counter);
            pool.execute(move || {
                let result = heavy_computation(i);
                let mut count = counter_clone.lock().unwrap();
                *count = count.wrapping_add(result);
            });
        }
    }
    let duration = start.elapsed();
    
    let final_count = *counter.lock().unwrap();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Final count: {}", final_count);
}

