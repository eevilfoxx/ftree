#pragma once

#include "Person.hpp"
#include <vector>
#include <cstdint>
#include <algorithm>

namespace ftree::domain {

using FamilyId = uint32_t;
using PersonId = uint32_t;

// Data structure for family (used in World)
struct FamilyData {
    FamilyId id = 0;
    PersonId motherId = 0;
    PersonId fatherId = 0;
    std::vector<PersonId> childrenIds;
    std::string surname;
};

class Family {
public:
    // Constructors
    Family() = default;
    Family(FamilyId id, PersonId motherId, PersonId fatherId, const std::string& surname = "")
        : id_(id), motherId_(motherId), fatherId_(fatherId), surname_(surname) {}

    // Getters
    FamilyId getId() const { return id_; }
    PersonId getMotherId() const { return motherId_; }
    PersonId getFatherId() const { return fatherId_; }
    const std::vector<PersonId>& getChildrenIds() const { return childrenIds_; }
    const std::string& getSurname() const { return surname_; }

    // Add/remove child
    void addChild(PersonId childId) {
        if (childId != 0 && !hasChild(childId)
            && childId != motherId_ && childId != fatherId_) {
            childrenIds_.push_back(childId);
        }
    }

    bool removeChild(PersonId childId) {
        const auto oldSize = childrenIds_.size();
        childrenIds_.erase(
            std::remove(childrenIds_.begin(), childrenIds_.end(), childId),
            childrenIds_.end());
        return childrenIds_.size() != oldSize;
    }

    bool hasChild(PersonId childId) const {
        return std::find(childrenIds_.begin(), childrenIds_.end(), childId)
            != childrenIds_.end();
    }

    // Get all family members (parents + children), without duplicate IDs.
    std::vector<PersonId> getAllMembers() const {
        std::vector<PersonId> members;
        members.reserve(2 + childrenIds_.size());
        if (motherId_ != 0) members.push_back(motherId_);
        if (fatherId_ != 0 && fatherId_ != motherId_) members.push_back(fatherId_);
        for (PersonId childId : childrenIds_) {
            if (childId != 0
                && std::find(members.begin(), members.end(), childId) == members.end()) {
                members.push_back(childId);
            }
        }
        return members;
    }

    bool isMember(PersonId personId) const {
        if (personId == 0) return false;
        return personId == motherId_ || personId == fatherId_ || hasChild(personId);
    }

private:
    FamilyId id_ = 0;
    PersonId motherId_ = 0;
    PersonId fatherId_ = 0;
    std::vector<PersonId> childrenIds_;
    std::string surname_;
};

} // namespace ftree::domain
