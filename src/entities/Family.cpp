#include "entities/Family.hpp"
#include <algorithm>
#include <utility>
#include <stdexcept>

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
    if (!child || hasChild(child)
        || child == getMother() || child == getFather()) {
        return;
    }

    auto self = weak_from_this().lock();
    if (!self) {
        throw std::logic_error("Family must be owned by std::shared_ptr before adding children");
    }

    if (auto previousFamily = child->getFamily()) {
        if (previousFamily.get() != this) {
            previousFamily->removeChild(child);
        }
    }

    // Discard expired entries while keeping the family's live children.
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [](const std::weak_ptr<Person>& weakChild) {
                return weakChild.expired();
            }),
        children.end());

    children.emplace_back(child);
    child->setFamily(std::move(self));
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