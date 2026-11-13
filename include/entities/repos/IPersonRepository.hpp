// IPersonRepository.hpp
#ifndef IPERSON_REPOSITORY_HPP
#define IPERSON_REPOSITORY_HPP

#include <memory>
#include <vector>
#include <functional>
#include <entities/Person.hpp>

class IPersonRepository {
public:
    virtual ~IPersonRepository() = default;
    
    virtual std::shared_ptr<Person> getById(int uuid) = 0;
    virtual void add(std::shared_ptr<Person> person) = 0;
    virtual void remove(int uuid) = 0;
    virtual void update(std::shared_ptr<Person> person) = 0;
    
    virtual std::vector<std::shared_ptr<Person>> getAll() = 0;
    virtual std::vector<std::shared_ptr<Person>> find(const std::function<bool(const Person&)>& predicate) = 0;
    virtual std::vector<std::shared_ptr<Person>> getByAgeRange(int minAge, int maxAge) = 0;
    virtual std::vector<std::shared_ptr<Person>> getByGender(Gender gender) = 0;
    virtual std::vector<std::shared_ptr<Person>> getLiving() = 0;
};

#endif