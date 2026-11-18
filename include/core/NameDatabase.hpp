#pragma once

#include "Random.hpp"

#include <cstddef>
#include <string>
#include <vector>


namespace ftree::core {

struct NameDatabaseSnapshot {
    std::vector<std::string> maleNames;
    std::vector<std::string> femaleNames;
    std::vector<std::string> surnames;
};

class NameDatabase {
public:
    NameDatabase();
    
    // Load names from files
    void loadMaleNames(const std::string& filename);
    void loadFemaleNames(const std::string& filename);
    void loadSurnames(const std::string& filename);
    
    // Get random name by gender
    std::string getRandomName(bool isMale, Random& random) const;
    std::string getRandomSurname(Random& random) const;
    
    // Check if already loaded
    bool areNamesLoaded() const;
    bool areSurnamesLoaded() const;

    NameDatabaseSnapshot createSnapshot() const;
    void restoreSnapshot(const NameDatabaseSnapshot& snapshot);
    
private:
    std::vector<std::string> maleNames_;
    std::vector<std::string> femaleNames_;
    std::vector<std::string> surnames_;

public:
    std::size_t maleNameCount() const noexcept {
        return maleNames_.size();
    }

    std::size_t femaleNameCount() const noexcept {
        return femaleNames_.size();
    }

    std::size_t surnameCount() const noexcept {
        return surnames_.size();
    }
};

} // namespace ftree::core
