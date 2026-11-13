#ifndef MEMORY_FAMILY_REPOSITORY_HPP
#define MEMORY_FAMILY_REPOSITORY_HPP

#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>
#include "IFamilyRepository.hpp"

class MemoryFamilyRepository : public IFamilyRepository {
public:
    std::shared_ptr<Family> getById(int familyId) override {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = families.find(familyId);
        if (it != families.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    void add(std::shared_ptr<Family> family) override {
        std::lock_guard<std::mutex> lock(mutex);
        families[family->getId()] = family;
    }
    
    void remove(int familyId) override {
        std::lock_guard<std::mutex> lock(mutex);
        families.erase(familyId);
    }
    
    void update(std::shared_ptr<Family> family) override {
        std::lock_guard<std::mutex> lock(mutex);
        families[family->getId()] = family;
    }
    
    std::vector<std::shared_ptr<Family>> getAll() override {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<Family>> result;
        for (const auto& pair : families) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    std::vector<std::shared_ptr<Family>> find(const std::function<bool(const Family&)>& predicate) override {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<Family>> result;
        for (const auto& pair : families) {
            if (predicate(*pair.second)) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
    
private:
    std::unordered_map<int, std::shared_ptr<Family>> families;
    std::mutex mutex;
};

#endif