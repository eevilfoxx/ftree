#include "core/Config.hpp"
#include <managers/NameManager.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

void Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Пропускаем комментарии и пустые строки
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            values[key] = value;
        }
    }
    
    // Загружаем имена после загрузки конфига
    // // TODO: загружать не через конфиги а через ассеты
    auto& nameManager = NameManager::getInstance();
    std::string maleNamesFile = getString("male_names_file", "male_names.txt");
    std::string femaleNamesFile = getString("female_names_file", "female_names.txt");
    std::string surnamesFile = getString("surnames_file", "surnames.txt");
    
    nameManager.loadNames(maleNamesFile, femaleNamesFile);
    nameManager.loadSurnames(surnamesFile);
}

void Config::set(const std::string& key, const std::string& value) {
    values[key] = value;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::stod(it->second);
        } catch (const std::exception&) {
            std::cerr << "Warning: Invalid double value for key: " << key << std::endl;
        }
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception&) {
            std::cerr << "Warning: Invalid int value for key: " << key << std::endl;
        }
    }
    return defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = values.find(key);
    if (it != values.end()) {
        return it->second;
    }
    return defaultValue;
}