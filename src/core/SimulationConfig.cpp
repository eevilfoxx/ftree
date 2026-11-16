#include "core/SimulationConfig.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace ftree::core {

SimulationConfig SimulationConfig::loadFromFile(const std::string& filename) {
    SimulationConfig config;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return config;  // Return defaults
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Parse key=value
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            std::cerr << "Warning: Invalid line " << lineNum << " in config file: " << line << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        
        try {
            if (key == "min_founder_age") config.min_founder_age = std::stoi(value);
            else if (key == "max_founder_age") config.max_founder_age = std::stoi(value);
            else if (key == "mortality_a") config.mortality_a = std::stod(value);
            else if (key == "mortality_b") config.mortality_b = std::stod(value);
            else if (key == "base_conception_prob") config.base_conception_prob = std::stod(value);
            else if (key == "min_marriage_age") config.min_marriage_age = std::stoi(value);
            else if (key == "age_diff_factor") config.age_diff_factor = std::stod(value);
            else if (key == "prob_triple") config.prob_triple = std::stod(value);
            else if (key == "prob_twin") config.prob_twin = std::stod(value);
            else if (key == "max_person_age") config.max_person_age = std::stoi(value);
            else if (key == "male_names_file") config.male_names_file = value;
            else if (key == "female_names_file") config.female_names_file = value;
            else if (key == "surnames_file") config.surnames_file = value;
            else std::cerr << "Warning: Unknown config key: " << key << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid value for key '" << key << "': " << e.what() << " (line " << lineNum << ")" << std::endl;
        }
    }
    
    config.validate();
    return config;
}

double SimulationConfig::getDouble(const std::string& key, double defaultValue) const {
    // In a full implementation, would store values and return them
    // For now return default
    (void)key; (void)defaultValue;
    return defaultValue;
}

int SimulationConfig::getInt(const std::string& key, int defaultValue) const {
    (void)key; (void)defaultValue;
    return defaultValue;
}

std::string SimulationConfig::getString(const std::string& key, const std::string& defaultValue) const {
    (void)key; (void)defaultValue;
    return defaultValue;
}

bool SimulationConfig::has(const std::string& key) const {
    // Check if key is known
    return false;  // Simplified
}

std::vector<std::string> SimulationConfig::getKeys() const {
    return {"min_founder_age", "max_founder_age", "mortality_a", "mortality_b",
            "base_conception_prob", "min_marriage_age", "age_diff_factor",
            "prob_triple", "prob_twin", "max_person_age",
            "male_names_file", "female_names_file", "surnames_file"};
}

void SimulationConfig::validate() const {
    if (min_founder_age < 0) throw std::runtime_error("min_founder_age must be non-negative");
    if (max_founder_age < min_founder_age) throw std::runtime_error("max_founder_age must be >= min_founder_age");
    if (mortality_a < 0) throw std::runtime_error("mortality_a must be non-negative");
    if (mortality_b < 0) throw std::runtime_error("mortality_b must be non-negative");
    if (base_conception_prob < 0 || base_conception_prob > 1) throw std::runtime_error("base_conception_prob must be in [0,1]");
    if (min_marriage_age < 0) throw std::runtime_error("min_marriage_age must be non-negative");
    if (age_diff_factor < 0) throw std::runtime_error("age_diff_factor must be non-negative");
    if (prob_triple < 0 || prob_triple > 1) throw std::runtime_error("prob_triple must be in [0,1]");
    if (prob_twin < 0 || prob_twin > 1) throw std::runtime_error("prob_twin must be in [0,1]");
    if (max_person_age < 0) throw std::runtime_error("max_person_age must be non-negative");
}
} // namespace ftree::core
