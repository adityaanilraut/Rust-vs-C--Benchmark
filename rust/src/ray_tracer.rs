use rayon::prelude::*;
use std::sync::Arc;
use std::time::Instant;

const WIDTH: usize = 1920;
const HEIGHT: usize = 1080;
const SAMPLES: usize = 4;

#[derive(Clone, Copy)]
struct Vec3 {
    x: f64,
    y: f64,
    z: f64,
}

impl Vec3 {
    fn new(x: f64, y: f64, z: f64) -> Self {
        Vec3 { x, y, z }
    }
    
    fn dot(&self, other: &Vec3) -> f64 {
        self.x * other.x + self.y * other.y + self.z * other.z
    }
    
    fn length(&self) -> f64 {
        (self.x * self.x + self.y * self.y + self.z * self.z).sqrt()
    }
    
    fn normalize(&self) -> Vec3 {
        let len = self.length();
        Vec3::new(self.x / len, self.y / len, self.z / len)
    }
    
    fn sub(&self, other: &Vec3) -> Vec3 {
        Vec3::new(self.x - other.x, self.y - other.y, self.z - other.z)
    }
    
    fn add(&self, other: &Vec3) -> Vec3 {
        Vec3::new(self.x + other.x, self.y + other.y, self.z + other.z)
    }
    
    fn mul(&self, scalar: f64) -> Vec3 {
        Vec3::new(self.x * scalar, self.y * scalar, self.z * scalar)
    }
}

struct Sphere {
    center: Vec3,
    radius: f64,
    color: Vec3,
}

impl Sphere {
    fn intersect(&self, origin: &Vec3, direction: &Vec3) -> Option<f64> {
        let oc = origin.sub(&self.center);
        let a = direction.dot(direction);
        let b = 2.0 * oc.dot(direction);
        let c = oc.dot(&oc) - self.radius * self.radius;
        let discriminant = b * b - 4.0 * a * c;
        
        if discriminant < 0.0 {
            None
        } else {
            let t = (-b - discriminant.sqrt()) / (2.0 * a);
            if t > 0.0 {
                Some(t)
            } else {
                None
            }
        }
    }
}

fn trace_ray(origin: &Vec3, direction: &Vec3, spheres: &[Sphere]) -> Vec3 {
    let mut closest_t = f64::INFINITY;
    let mut hit_sphere: Option<&Sphere> = None;
    
    for sphere in spheres {
        if let Some(t) = sphere.intersect(origin, direction) {
            if t < closest_t {
                closest_t = t;
                hit_sphere = Some(sphere);
            }
        }
    }
    
    if let Some(sphere) = hit_sphere {
        let hit_point = origin.add(&direction.mul(closest_t));
        let normal = hit_point.sub(&sphere.center).normalize();
        let light_dir = Vec3::new(1.0, 1.0, 1.0).normalize();
        let diffuse = normal.dot(&light_dir).max(0.0);
        sphere.color.mul(diffuse)
    } else {
        Vec3::new(0.2, 0.3, 0.4) // Background color
    }
}

fn main() {
    let spheres = Arc::new(vec![
        Sphere { center: Vec3::new(0.0, 0.0, -5.0), radius: 1.0, color: Vec3::new(1.0, 0.0, 0.0) },
        Sphere { center: Vec3::new(2.0, 0.0, -6.0), radius: 1.0, color: Vec3::new(0.0, 1.0, 0.0) },
        Sphere { center: Vec3::new(-2.0, 0.0, -6.0), radius: 1.0, color: Vec3::new(0.0, 0.0, 1.0) },
        Sphere { center: Vec3::new(0.0, -1001.0, -5.0), radius: 1000.0, color: Vec3::new(0.8, 0.8, 0.8) },
    ]);
    
    // Warm-up
    let spheres_warmup = Arc::clone(&spheres);
    let _: Vec<_> = (0..100).into_par_iter().map(move |_| {
        let origin = Vec3::new(0.0, 0.0, 0.0);
        let direction = Vec3::new(0.0, 0.0, -1.0).normalize();
        trace_ray(&origin, &direction, &spheres_warmup)
    }).collect();
    
    // Benchmark
    let start = Instant::now();
    
    let spheres_render = Arc::clone(&spheres);
    let image: Vec<Vec3> = (0..HEIGHT).into_par_iter().flat_map(|y| {
        let spheres_row = Arc::clone(&spheres_render);
        (0..WIDTH).into_par_iter().map(move |x| {
            let mut color = Vec3::new(0.0, 0.0, 0.0);
            for _ in 0..SAMPLES {
                let u = (x as f64) / (WIDTH as f64) - 0.5;
                let v = 0.5 - (y as f64) / (HEIGHT as f64);
                let origin = Vec3::new(0.0, 0.0, 0.0);
                let direction = Vec3::new(u * 2.0, v * 2.0, -1.0).normalize();
                let sample_color = trace_ray(&origin, &direction, &spheres_row);
                color = color.add(&sample_color);
            }
            color.mul(1.0 / SAMPLES as f64)
        }).collect::<Vec<_>>()
    }).collect();
    
    let duration = start.elapsed();
    
    // Checksum
    let checksum: f64 = image.iter().take(100).map(|c| c.x + c.y + c.z).sum();
    
    println!("{:.6}", duration.as_secs_f64());
    eprintln!("Checksum: {}", checksum);
}

