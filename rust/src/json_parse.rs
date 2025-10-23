use serde::{Deserialize, Serialize};
use serde_json;
use std::time::Instant;

#[derive(Serialize, Deserialize, Clone)]
struct Record {
    id: u32,
    name: String,
    email: String,
    age: u32,
    balance: f64,
    is_active: bool,
    tags: Vec<String>,
    metadata: Metadata,
}

#[derive(Serialize, Deserialize, Clone)]
struct Metadata {
    created_at: String,
    updated_at: String,
    version: u32,
}

fn generate_records(count: usize) -> Vec<Record> {
    (0..count)
        .map(|i| Record {
            id: i as u32,
            name: format!("User {}", i),
            email: format!("user{}@example.com", i),
            age: 20 + (i % 50) as u32,
            balance: (i as f64) * 123.456,
            is_active: i % 2 == 0,
            tags: vec![
                format!("tag{}", i % 10),
                format!("category{}", i % 5),
                "important".to_string(),
            ],
            metadata: Metadata {
                created_at: "2024-01-01T00:00:00Z".to_string(),
                updated_at: "2024-01-02T00:00:00Z".to_string(),
                version: (i % 100) as u32,
            },
        })
        .collect()
}

fn main() {
    let records = generate_records(10_000);
    
    // Serialize
    let json_string = serde_json::to_string(&records).expect("Failed to serialize");
    
    // Warm-up
    let _ = serde_json::from_str::<Vec<Record>>(&json_string).expect("Failed to parse");
    
    // Benchmark parse
    let start = Instant::now();
    let parsed: Vec<Record> = serde_json::from_str(&json_string).expect("Failed to parse");
    let parse_duration = start.elapsed();
    
    // Benchmark serialize
    let start = Instant::now();
    let serialized = serde_json::to_string(&parsed).expect("Failed to serialize");
    let serialize_duration = start.elapsed();
    
    let total_duration = parse_duration + serialize_duration;
    
    println!("{:.6}", total_duration.as_secs_f64());
    eprintln!("Parse: {:.6}s, Serialize: {:.6}s", parse_duration.as_secs_f64(), serialize_duration.as_secs_f64());
    eprintln!("Records: {}, JSON size: {} bytes", parsed.len(), serialized.len());
}

