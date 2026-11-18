#include "core/NameDatabase.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <codecvt>

namespace ftree::core {

NameDatabase::NameDatabase() {}

void NameDatabase::loadMaleNames(const std::string& filename) {
    // Check if same file was loaded before (by content hash)
    // For simplicity, just check if file exists and append new names
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open male names file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            // Avoid duplicates
            if (std::find(maleNames_.begin(), maleNames_.end(), line) == maleNames_.end()) {
                maleNames_.push_back(line);
            }
        }
    }
    file.close();
}

void NameDatabase::loadFemaleNames(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open female names file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            if (std::find(femaleNames_.begin(), femaleNames_.end(), line) == femaleNames_.end()) {
                femaleNames_.push_back(line);
            }
        }
    }
    file.close();
}

void NameDatabase::loadSurnames(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open surnames file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            if (std::find(surnames_.begin(), surnames_.end(), line) == surnames_.end()) {
                surnames_.push_back(line);
            }
        }
    }
    file.close();
}

std::string NameDatabase::getRandomName(
    bool isMale,
    Random& random
) const {
    const std::vector<std::string>& names =
        isMale ? maleNames_ : femaleNames_;

    if (names.empty()) {
        return "Unknown";
    }

    const int index = random.randomInt(
        0,
        static_cast<int>(names.size()) - 1
    );

    return names[index];
}

std::string NameDatabase::getRandomSurname(Random& random) const {
    if (surnames_.empty()) {
        return "Unknown";
    }

    const int index = random.randomInt(
        0,
        static_cast<int>(surnames_.size()) - 1
    );

    return surnames_[index];
}

bool NameDatabase::areNamesLoaded() const {
    return !maleNames_.empty() || !femaleNames_.empty();;
}

bool NameDatabase::areSurnamesLoaded() const {
    return !surnames_.empty();
}

NameDatabaseSnapshot NameDatabase::createSnapshot() const {
    return {maleNames_, femaleNames_, surnames_};
}

void NameDatabase::restoreSnapshot(
    const NameDatabaseSnapshot& snapshot) {
    maleNames_ = snapshot.maleNames;
    femaleNames_ = snapshot.femaleNames;
    surnames_ = snapshot.surnames;
}

} // namespace ftree::core
