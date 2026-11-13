#include "entities/Family.hpp"
#include <algorithm>

#include <managers/NameManager.hpp>

Family::Family(int id, std::shared_ptr<Person> mother, std::shared_ptr<Person> father)
    : id(id), mother(mother), father(father), 
      surname(NameManager::getInstance().getRandomSurname()) {}

std::vector<std::shared_ptr<Person>> Family::getChildren() const {
    std::vector<std::shared_ptr<Person>> result;
    for (const auto& childWeak : children) {
        if (auto child = childWeak.lock()) {
            result.push_back(child);
        }
    }
    return result;
}

void Family::addChild(std::shared_ptr<Person> child) {
    if (child) {
        children.push_back(child);
        child->setFamily(shared_from_this());
    }
}

void Family::removeChild(std::shared_ptr<Person> child) {
    if (!child) return;
    
    auto it = std::remove_if(children.begin(), children.end(),
        [&child](const std::weak_ptr<Person>& weakChild) {
            auto sharedChild = weakChild.lock();
            return !sharedChild || sharedChild == child;
        });
    
    children.erase(it, children.end());
    
    if (child->getFamily().get() == this) {
        child->setFamily(nullptr);
    }
}

bool Family::hasChild(std::shared_ptr<Person> child) const {
    if (!child) return false;
    
    for (const auto& weakChild : children) {
        if (auto sharedChild = weakChild.lock()) {
            if (sharedChild == child) {
                return true;
            }
        }
    }
    
    return false;
}