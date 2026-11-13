// FamilyManager.hpp
#ifndef FAMILY_MANAGER_HPP
#define FAMILY_MANAGER_HPP

#include <memory>
#include <vector>
#include <entities/repos/IFamilyRepository.hpp>
#include <entities/repos/IPersonRepository.hpp>
#include <core/EventDispatcher.hpp>

class FamilyManager {
public:
    FamilyManager(std::shared_ptr<IFamilyRepository> familyRepo,
                  std::shared_ptr<IPersonRepository> personRepo,
                  std::shared_ptr<EventDispatcher> eventDispatcher);
    
    std::shared_ptr<Family> createFamily(std::shared_ptr<Person> mother, std::shared_ptr<Person> father);
    void dissolveFamily(int familyId);
    void addChildToFamily(int familyId, std::shared_ptr<Person> child);
    void removeChildFromFamily(int familyId, int childId);
    
    std::vector<std::shared_ptr<Family>> getAllFamilies();
    std::vector<std::shared_ptr<Person>> getFamilyMembers(int familyId);
    
private:
    std::shared_ptr<IFamilyRepository> familyRepository;
    std::shared_ptr<IPersonRepository> personRepository;
    std::shared_ptr<EventDispatcher> eventDispatcher;
};

#endif