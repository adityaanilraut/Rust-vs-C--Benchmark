#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <iomanip>

// Simple JSON generator/parser (avoiding external dependencies)
struct Metadata {
    std::string created_at;
    std::string updated_at;
    uint32_t version;
};

struct Record {
    uint32_t id;
    std::string name;
    std::string email;
    uint32_t age;
    double balance;
    bool is_active;
    std::vector<std::string> tags;
    Metadata metadata;
};

std::string escape_json_string(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        if (c == '"') oss << "\\\"";
        else if (c == '\\') oss << "\\\\";
        else oss << c;
    }
    return oss.str();
}

std::string serialize_record(const Record& r) {
    std::ostringstream oss;
    oss << "{\"id\":" << r.id << ",";
    oss << "\"name\":\"" << escape_json_string(r.name) << "\",";
    oss << "\"email\":\"" << escape_json_string(r.email) << "\",";
    oss << "\"age\":" << r.age << ",";
    oss << std::fixed << std::setprecision(3) << "\"balance\":" << r.balance << ",";
    oss << "\"is_active\":" << (r.is_active ? "true" : "false") << ",";
    oss << "\"tags\":[";
    for (size_t i = 0; i < r.tags.size(); i++) {
        if (i > 0) oss << ",";
        oss << "\"" << escape_json_string(r.tags[i]) << "\"";
    }
    oss << "],";
    oss << "\"metadata\":{";
    oss << "\"created_at\":\"" << escape_json_string(r.metadata.created_at) << "\",";
    oss << "\"updated_at\":\"" << escape_json_string(r.metadata.updated_at) << "\",";
    oss << "\"version\":" << r.metadata.version;
    oss << "}}";
    return oss.str();
}

std::string serialize_records(const std::vector<Record>& records) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < records.size(); i++) {
        if (i > 0) oss << ",";
        oss << serialize_record(records[i]);
    }
    oss << "]";
    return oss.str();
}

// Simple JSON parser
size_t find_next_field(const std::string& json, const std::string& field, size_t start) {
    std::string search = "\"" + field + "\":";
    return json.find(search, start);
}

std::string extract_string_value(const std::string& json, size_t pos) {
    size_t start = json.find('"', pos) + 1;
    size_t end = json.find('"', start);
    return json.substr(start, end - start);
}

int extract_int_value(const std::string& json, size_t pos) {
    size_t start = json.find(':', pos) + 1;
    return std::stoi(json.substr(start));
}

double extract_double_value(const std::string& json, size_t pos) {
    size_t start = json.find(':', pos) + 1;
    return std::stod(json.substr(start));
}

bool extract_bool_value(const std::string& json, size_t pos) {
    size_t start = json.find(':', pos) + 1;
    return json.substr(start, 4) == "true";
}

std::vector<Record> parse_records(const std::string& json) {
    std::vector<Record> records;
    size_t pos = 1; // Start after '['
    const size_t len = json.length();
    
    while (pos < len) {
        // Skip whitespace and commas
        while (pos < len && (json[pos] == ' ' || json[pos] == ',' || json[pos] == '\n')) pos++;
        
        // Check for end of array
        if (pos >= len || json[pos] == ']') break;
        
        // Find the record bounds more efficiently
        if (json[pos] != '{') break;
        
        size_t record_start = pos;
        size_t record_end = json.find('}', record_start);
        if (record_end == std::string::npos) break;
        
        // Find next nested object end if metadata exists
        size_t nested_end = json.find('}', record_end + 1);
        if (nested_end != std::string::npos && nested_end < record_end + 10) {
            record_end = nested_end;
        }
        
        Record r;
        
        // Parse fields within the record bounds
        size_t id_pos = json.find("\"id\":", record_start);
        if (id_pos != std::string::npos && id_pos < record_end) {
            r.id = extract_int_value(json, id_pos);
        }
        
        size_t name_pos = json.find("\"name\":", record_start);
        if (name_pos != std::string::npos && name_pos < record_end) {
            r.name = extract_string_value(json, name_pos);
        }
        
        size_t email_pos = json.find("\"email\":", record_start);
        if (email_pos != std::string::npos && email_pos < record_end) {
            r.email = extract_string_value(json, email_pos);
        }
        
        size_t age_pos = json.find("\"age\":", record_start);
        if (age_pos != std::string::npos && age_pos < record_end) {
            r.age = extract_int_value(json, age_pos);
        }
        
        size_t balance_pos = json.find("\"balance\":", record_start);
        if (balance_pos != std::string::npos && balance_pos < record_end) {
            r.balance = extract_double_value(json, balance_pos);
        }
        
        size_t active_pos = json.find("\"is_active\":", record_start);
        if (active_pos != std::string::npos && active_pos < record_end) {
            r.is_active = extract_bool_value(json, active_pos);
        }
        
        records.push_back(r);
        pos = record_end + 1;
    }
    
    return records;
}

std::vector<Record> generate_records(size_t count) {
    std::vector<Record> records;
    records.reserve(count);
    
    for (size_t i = 0; i < count; i++) {
        Record r;
        r.id = static_cast<uint32_t>(i);
        r.name = "User " + std::to_string(i);
        r.email = "user" + std::to_string(i) + "@example.com";
        r.age = 20 + (i % 50);
        r.balance = static_cast<double>(i) * 123.456;
        r.is_active = (i % 2 == 0);
        r.tags = {
            "tag" + std::to_string(i % 10),
            "category" + std::to_string(i % 5),
            "important"
        };
        r.metadata.created_at = "2024-01-01T00:00:00Z";
        r.metadata.updated_at = "2024-01-02T00:00:00Z";
        r.metadata.version = i % 100;
        
        records.push_back(r);
    }
    
    return records;
}

int main() {
    auto records = generate_records(10'000);
    
    // Serialize
    std::string json_string = serialize_records(records);
    
    // Warm-up
    auto warmup = parse_records(json_string);
    
    // Benchmark parse
    auto start = std::chrono::high_resolution_clock::now();
    auto parsed = parse_records(json_string);
    auto parse_end = std::chrono::high_resolution_clock::now();
    
    // Benchmark serialize
    auto serialize_start = std::chrono::high_resolution_clock::now();
    auto serialized = serialize_records(parsed);
    auto serialize_end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> parse_duration = parse_end - start;
    std::chrono::duration<double> serialize_duration = serialize_end - serialize_start;
    std::chrono::duration<double> total_duration = parse_duration + serialize_duration;
    
    std::cout << std::fixed << std::setprecision(6) << total_duration.count() << std::endl;
    std::cerr << "Parse: " << parse_duration.count() << "s, Serialize: " << serialize_duration.count() << "s" << std::endl;
    std::cerr << "Records: " << parsed.size() << ", JSON size: " << serialized.length() << " bytes" << std::endl;
    
    return 0;
}

