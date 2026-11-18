#pragma once

#include <string>
#include <stdexcept>
#include <vector>

namespace ftree::core {

struct SimulationConfig {
    // Founder ages
    int min_founder_age = 20;
    int max_founder_age = 40;
    
    // Mortality parameters
    double mortality_a = 0.001;
    double mortality_b = 0.0693;
    
    // Conception
    double base_conception_prob = 0.3;
    
    // Marriage
    int min_marriage_age = 18;
    double age_diff_factor = 0.1;
    
    // Birth probabilities
    double prob_triple = 0.01;
    double prob_twin = 0.09;
    
    // Person limits
    int max_person_age = 100;
    
    // File paths
    std::string male_names_file = "male_names.txt";
    std::string female_names_file = "female_names.txt";
    std::string surnames_file = "surnames.txt";
    
    // Load from file format: key=value
    static SimulationConfig loadFromFile(const std::string& filename);
    
    // Get with default
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // All keys
    std::vector<std::string> getKeys() const;
    
    // Validate values
    void validate() const;
};

} // namespace ftree::core
