// NameManager.hpp
#ifndef NAME_MANAGER_HPP
#define NAME_MANAGER_HPP

#include <vector>
#include <string>
#include <memory>
#include "entities/Gender.hpp"
#include "core/RandomEngine.hpp"

class NameManager {
public:
    static NameManager& getInstance();
    
    void loadNames(const std::string& maleNamesFile, const std::string& femaleNamesFile);
    std::string getRandomName(Gender gender);

    void loadSurnames(const std::string& surnamesFile);
    std::string getRandomSurname();
    
private:
    NameManager() = default;
    NameManager(const NameManager&) = delete;
    NameManager& operator=(const NameManager&) = delete;
    
    std::vector<std::string> maleNames;
    std::vector<std::string> femaleNames;
    std::vector<std::string> surnames;
};

#endif