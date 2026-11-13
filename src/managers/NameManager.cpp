#include "managers/NameManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

NameManager& NameManager::getInstance() {
    static NameManager instance;
    return instance;
}

void NameManager::loadNames(const std::string& maleNamesFile, const std::string& femaleNamesFile) {
    std::ifstream maleFile(maleNamesFile);
    if (maleFile.is_open()) {
        std::string name;
        while (std::getline(maleFile, name)) {
            if (!name.empty()) {
                maleNames.push_back(name);
            }
        }
        maleFile.close();
        std::cout << "Loaded " << maleNames.size() << " male names" << std::endl;
    } else {
        std::cerr << "Warning: Could not open male names file: " << maleNamesFile << std::endl;
    }
    
    std::ifstream femaleFile(femaleNamesFile);
    if (femaleFile.is_open()) {
        std::string name;
        while (std::getline(femaleFile, name)) {
            if (!name.empty()) {
                femaleNames.push_back(name);
            }
        }
        femaleFile.close();
        std::cout << "Loaded " << femaleNames.size() << " female names" << std::endl;
    } else {
        std::cerr << "Warning: Could not open female names file: " << femaleNamesFile << std::endl;
    }
}

void NameManager::loadSurnames(const std::string& surnamesFile) {
    std::ifstream file(surnamesFile);
    if (file.is_open()) {
        std::string surname;
        while (std::getline(file, surname)) {
            if (!surname.empty()) {
                surnames.push_back(surname);
            }
        }
        file.close();
        std::cout << "Loaded " << surnames.size() << " surnames" << std::endl;
    } else {
        std::cerr << "Warning: Could not open surnames file: " << surnamesFile << std::endl;
    }
}

std::string NameManager::getRandomName(Gender gender) {
    const std::vector<std::string>* names = nullptr;
    
    if (gender == Gender::Male) {
        names = &maleNames;
    } else {
        names = &femaleNames;
    }
    
    if (names && !names->empty()) {
        
        int dist = RandomEngine::getInstance().randomInt(0, names->size() - 1);
        return (*names)[dist];
    }
    
    return (gender == Gender::Male) ? "Unknown_Male" : "Unknown_Female";
}


std::string NameManager::getRandomSurname() {
    if (!surnames.empty()) {
        int dist = RandomEngine::getInstance().randomInt(0, surnames.size() - 1);
        return surnames[dist];
    }
    return "Unknown_Surname";
}