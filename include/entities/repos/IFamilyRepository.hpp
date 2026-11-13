// IFamilyRepository.hpp
#ifndef IFAMILY_REPOSITORY_HPP
#define IFAMILY_REPOSITORY_HPP

#include <memory>
#include <vector>
#include <functional>
#include <entities/Family.hpp>

class IFamilyRepository {
public:
    virtual ~IFamilyRepository() = default;
    
    virtual std::shared_ptr<Family> getById(int familyId) = 0;
    virtual void add(std::shared_ptr<Family> family) = 0;
    virtual void remove(int familyId) = 0;
    virtual void update(std::shared_ptr<Family> family) = 0;
    
    virtual std::vector<std::shared_ptr<Family>> getAll() = 0;
    virtual std::vector<std::shared_ptr<Family>> find(const std::function<bool(const Family&)>& predicate) = 0;
};

#endif