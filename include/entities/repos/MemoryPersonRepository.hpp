#ifndef MEMORY_PERSON_REPOSITORY_HPP
#define MEMORY_PERSON_REPOSITORY_HPP

#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>
#include "IPersonRepository.hpp"

class MemoryPersonRepository : public IPersonRepository {
public:
    std::shared_ptr<Person> getById(int uuid) override {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = persons.find(uuid);
        if (it != persons.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    void add(std::shared_ptr<Person> person) override {
        std::lock_guard<std::mutex> lock(mutex);
        persons[person->getUuid()] = person;
    }
    
    void remove(int uuid) override {
        std::lock_guard<std::mutex> lock(mutex);
        persons.erase(uuid);
    }
    
    void update(std::shared_ptr<Person> person) override {
        std::lock_guard<std::mutex> lock(mutex);
        if (person) {
        persons[person->getUuid()] = person;
    }
    }
    
    std::vector<std::shared_ptr<Person>> getAll() override {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<Person>> result;
        for (const auto& pair : persons) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    std::vector<std::shared_ptr<Person>> find(const std::function<bool(const Person&)>& predicate) override {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<Person>> result;
        for (const auto& pair : persons) {
            if (predicate(*pair.second)) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
    
    std::vector<std::shared_ptr<Person>> getByAgeRange(int minAge, int maxAge) override {
        return find([minAge, maxAge](const Person& person) {
            return person.getAge() >= minAge && person.getAge() <= maxAge;
        });
    }
    
    std::vector<std::shared_ptr<Person>> getByGender(Gender gender) override {
        return find([gender](const Person& person) {
            return person.getGender() == gender;
        });
    }
    
    std::vector<std::shared_ptr<Person>> getLiving() override {
        return find([](const Person& person) {
            return person.isAlive();
        });
    }
    
private:
    std::unordered_map<int, std::shared_ptr<Person>> persons;
    std::mutex mutex;
};

#endif