#pragma once

#include "Person.hpp"
#include <vector>
#include <cstdint>

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
    Family(FamilyId id, PersonId motherId, PersonId fatherId, const std::string& surname = "");

    // Getters
    FamilyId getId() const { return id_; }
    PersonId getMotherId() const { return motherId_; }
    PersonId getFatherId() const { return fatherId_; }
    const std::vector<PersonId>& getChildrenIds() const { return childrenIds_; }
    const std::string& getSurname() const { return surname_; }

    // Add/remove child
    void addChild(PersonId childId);
    bool removeChild(PersonId childId);
    bool hasChild(PersonId childId) const;

    // Get all family members (parents + children)
    std::vector<PersonId> getAllMembers() const;

    // Check if person is a member
    bool isMember(PersonId personId) const;

private:
    FamilyId id_ = 0;
    PersonId motherId_ = 0;
    PersonId fatherId_ = 0;
    std::vector<PersonId> childrenIds_;
    std::string surname_;
};

} // namespace ftree::domain
