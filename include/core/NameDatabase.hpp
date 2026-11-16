#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>

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
    std::unordered_set<std::string> loadedFileHashes_;  // To detect re-loading same file
};

} // namespace ftree::core
