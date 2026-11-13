// PregnancyService.hpp
#ifndef PREGNANCY_SERVICE_HPP
#define PREGNANCY_SERVICE_HPP

#include "managers/FamilyManager.hpp"
#include "entities/repos/IPersonRepository.hpp"
#include "core/EventDispatcher.hpp"

class PregnancyService {
public:
    PregnancyService(std::shared_ptr<FamilyManager> familyManager,
                     std::shared_ptr<IPersonRepository> personRepository,
                     std::shared_ptr<EventDispatcher> eventDispatcher);
    void processPregnancies();
    
private:
    std::shared_ptr<FamilyManager> familyManager_;
    std::shared_ptr<IPersonRepository> personRepository_;
    std::shared_ptr<EventDispatcher> eventDispatcher_;
};

#endif