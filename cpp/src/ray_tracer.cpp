#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>

const size_t WIDTH = 1920;
const size_t HEIGHT = 1080;
const size_t SAMPLES = 4;
const size_t NUM_THREADS = 8;

struct Vec3 {
    double x, y, z;
    
    Vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    
    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    Vec3 normalize() const {
        double len = length();
        return Vec3(x / len, y / len, z / len);
    }
    
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    
    Vec3 operator*(double scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
};

struct Sphere {
    Vec3 center;
    double radius;
    Vec3 color;
    
    bool intersect(const Vec3& origin, const Vec3& direction, double& t) const {
        Vec3 oc = origin - center;
        double a = direction.dot(direction);
        double b = 2.0 * oc.dot(direction);
        double c = oc.dot(oc) - radius * radius;
        double discriminant = b * b - 4.0 * a * c;
        
        if (discriminant < 0.0) {
            return false;
        }
        
        t = (-b - std::sqrt(discriminant)) / (2.0 * a);
        return t > 0.0;
    }
};

Vec3 trace_ray(const Vec3& origin, const Vec3& direction, const std::vector<Sphere>& spheres) {
    double closest_t = std::numeric_limits<double>::infinity();
    const Sphere* hit_sphere = nullptr;
    
    for (const auto& sphere : spheres) {
        double t;
        if (sphere.intersect(origin, direction, t) && t < closest_t) {
            closest_t = t;
            hit_sphere = &sphere;
        }
    }
    
    if (hit_sphere) {
        Vec3 hit_point = origin + direction * closest_t;
        Vec3 normal = (hit_point - hit_sphere->center).normalize();
        Vec3 light_dir = Vec3(1.0, 1.0, 1.0).normalize();
        double diffuse = std::max(0.0, normal.dot(light_dir));
        return hit_sphere->color * diffuse;
    }
    
    return Vec3(0.2, 0.3, 0.4); // Background color
}

void render_section(const std::vector<Sphere>& spheres, std::vector<Vec3>& image,
                   size_t start_row, size_t end_row) {
    for (size_t y = start_row; y < end_row; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            Vec3 color(0, 0, 0);
            for (size_t s = 0; s < SAMPLES; s++) {
                double u = static_cast<double>(x) / WIDTH - 0.5;
                double v = 0.5 - static_cast<double>(y) / HEIGHT;
                Vec3 origin(0, 0, 0);
                Vec3 direction = Vec3(u * 2.0, v * 2.0, -1.0).normalize();
                Vec3 sample_color = trace_ray(origin, direction, spheres);
                color = color + sample_color;
            }
            image[y * WIDTH + x] = color * (1.0 / SAMPLES);
        }
    }
}

int main() {
    std::vector<Sphere> spheres = {
        {Vec3(0, 0, -5), 1.0, Vec3(1, 0, 0)},
        {Vec3(2, 0, -6), 1.0, Vec3(0, 1, 0)},
        {Vec3(-2, 0, -6), 1.0, Vec3(0, 0, 1)},
        {Vec3(0, -1001, -5), 1000.0, Vec3(0.8, 0.8, 0.8)}
    };
    
    std::vector<Vec3> image(WIDTH * HEIGHT);
    
    // Warm-up
    render_section(spheres, image, 0, 10);
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    size_t rows_per_thread = HEIGHT / NUM_THREADS;
    
    for (size_t t = 0; t < NUM_THREADS; t++) {
        size_t start_row = t * rows_per_thread;
        size_t end_row = (t == NUM_THREADS - 1) ? HEIGHT : (t + 1) * rows_per_thread;
        threads.emplace_back(render_section, std::ref(spheres), std::ref(image), start_row, end_row);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    
    // Checksum
    double checksum = 0.0;
    for (size_t i = 0; i < 100; i++) {
        checksum += image[i].x + image[i].y + image[i].z;
    }
    
    std::cout << std::fixed << std::setprecision(6) << duration.count() << std::endl;
    std::cerr << "Checksum: " << checksum << std::endl;
    
    return 0;
}

