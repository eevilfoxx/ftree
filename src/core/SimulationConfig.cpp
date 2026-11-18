#include "core/SimulationConfig.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <cmath>

namespace ftree::core {
namespace {

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

bool isKnownKey(const std::string& key) {
    static const std::vector<std::string> keys{
        "min_founder_age", "max_founder_age",
        "mortality_a", "mortality_b", "base_conception_prob",
        "min_marriage_age", "age_diff_factor",
        "prob_triple", "prob_twin", "max_person_age",
        "male_names_file", "female_names_file", "surnames_file"
    };

    for (const auto& knownKey : keys) {
        if (knownKey == key) {
            return true;
        }
    }
    return false;
}

template <typename Number>
Number parseNumber(const std::string& value) {
    std::size_t parsedCharacters = 0;
    Number result;

    if constexpr (std::is_same<Number, int>::value) {
        result = std::stoi(value, &parsedCharacters);
    } else {
        result = std::stod(value, &parsedCharacters);
    }

    if (parsedCharacters != value.size()) {
        throw std::invalid_argument("unexpected characters after numeric value");
    }
    return result;
}

} // namespace

SimulationConfig SimulationConfig::loadFromFile(const std::string& filename) {
    SimulationConfig config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << '\n';
        return config;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        const std::string cleanedLine = trim(line);

        if (cleanedLine.empty() || cleanedLine.front() == '#') {
            continue;
        }

        const auto separator = cleanedLine.find('=');
        if (separator == std::string::npos) {
            std::cerr << "Warning: Invalid line " << lineNumber
                      << " in config file: " << cleanedLine << '\n';
            continue;
        }

        const std::string key = trim(cleanedLine.substr(0, separator));
        const std::string value = trim(cleanedLine.substr(separator + 1));

        try {
            if (key == "min_founder_age") config.min_founder_age = parseNumber<int>(value);
            else if (key == "max_founder_age") config.max_founder_age = parseNumber<int>(value);
            else if (key == "mortality_a") config.mortality_a = parseNumber<double>(value);
            else if (key == "mortality_b") config.mortality_b = parseNumber<double>(value);
            else if (key == "base_conception_prob") config.base_conception_prob = parseNumber<double>(value);
            else if (key == "min_marriage_age") config.min_marriage_age = parseNumber<int>(value);
            else if (key == "age_diff_factor") config.age_diff_factor = parseNumber<double>(value);
            else if (key == "prob_triple") config.prob_triple = parseNumber<double>(value);
            else if (key == "prob_twin") config.prob_twin = parseNumber<double>(value);
            else if (key == "max_person_age") config.max_person_age = parseNumber<int>(value);
            else if (key == "male_names_file") config.male_names_file = value;
            else if (key == "female_names_file") config.female_names_file = value;
            else if (key == "surnames_file") config.surnames_file = value;
            else std::cerr << "Warning: Unknown config key '" << key
                           << "' on line " << lineNumber << '\n';
        } catch (const std::exception& error) {
            std::cerr << "Warning: Invalid value for key '" << key
                      << "' on line " << lineNumber << ": "
                      << error.what() << '\n';
        }
    }

    config.validate();
    return config;
}

double SimulationConfig::getDouble(const std::string& key, double defaultValue) const {
    if (key == "mortality_a") return mortality_a;
    if (key == "mortality_b") return mortality_b;
    if (key == "base_conception_prob") return base_conception_prob;
    if (key == "age_diff_factor") return age_diff_factor;
    if (key == "prob_triple") return prob_triple;
    if (key == "prob_twin") return prob_twin;
    return defaultValue;
}

int SimulationConfig::getInt(const std::string& key, int defaultValue) const {
    if (key == "min_founder_age") return min_founder_age;
    if (key == "max_founder_age") return max_founder_age;
    if (key == "min_marriage_age") return min_marriage_age;
    if (key == "max_person_age") return max_person_age;
    return defaultValue;
}

std::string SimulationConfig::getString(const std::string& key,
                                        const std::string& defaultValue) const {
    if (key == "male_names_file") return male_names_file;
    if (key == "female_names_file") return female_names_file;
    if (key == "surnames_file") return surnames_file;
    return defaultValue;
}

bool SimulationConfig::has(const std::string& key) const {
    return isKnownKey(key);
}

std::vector<std::string> SimulationConfig::getKeys() const {
    return {
        "min_founder_age", "max_founder_age",
        "mortality_a", "mortality_b", "base_conception_prob",
        "min_marriage_age", "age_diff_factor",
        "prob_triple", "prob_twin", "max_person_age",
        "male_names_file", "female_names_file", "surnames_file"
    };
}

void SimulationConfig::validate() const {
    if (min_founder_age < 0) {
        throw std::runtime_error("min_founder_age must be non-negative");
    }
    if (max_founder_age < min_founder_age) {
        throw std::runtime_error("max_founder_age must be >= min_founder_age");
    }
    if (!std::isfinite(mortality_a) || !std::isfinite(mortality_b)
        || mortality_a < 0.0 || mortality_b < 0.0) {
        throw std::runtime_error("mortality parameters must be finite and non-negative");
    }
    if (!std::isfinite(base_conception_prob)
        || base_conception_prob < 0.0 || base_conception_prob > 1.0) {
        throw std::runtime_error("base_conception_prob must be in [0,1]");
    }
    if (min_marriage_age < 0) {
        throw std::runtime_error("min_marriage_age must be non-negative");
    }
    if (!std::isfinite(age_diff_factor) || age_diff_factor < 0.0) {
        throw std::runtime_error("age_diff_factor must be non-negative");
    }
    if (!std::isfinite(prob_triple) || !std::isfinite(prob_twin)
        || prob_triple < 0.0 || prob_triple > 1.0
        || prob_twin < 0.0 || prob_twin > 1.0
        || prob_triple + prob_twin > 1.0) {
        throw std::runtime_error("birth probabilities must be in [0,1] and their sum must not exceed 1");
    }
    if (max_person_age < 0) {
        throw std::runtime_error("max_person_age must be non-negative");
    }
    if (min_marriage_age > max_person_age) {
        throw std::runtime_error("min_marriage_age must not exceed max_person_age");
    }
    if (max_founder_age > max_person_age) {
        throw std::runtime_error("max_founder_age must not exceed max_person_age");
    }
}

} // namespace ftree::core
