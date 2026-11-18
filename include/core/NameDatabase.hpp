#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ftree::core {

class NameDatabase {
public:
    NameDatabase();
    
    // Load names from files
    void loadMaleNames(const std::string& filename);
    void loadFemaleNames(const std::string& filename);
    void loadSurnames(const std::string& filename);
    
    // Get random name by gender
    std::string getRandomName(bool isMale) const;
    std::string getRandomSurname() const;
    
    // Check if already loaded
    bool areNamesLoaded() const;
    bool areSurnamesLoaded() const;
    
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
