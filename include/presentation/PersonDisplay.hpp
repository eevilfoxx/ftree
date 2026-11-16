#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace ftree::presentation {

using PersonId = uint32_t;
using FamilyId = uint32_t;

struct PersonDisplay {
    PersonId id;
    std::string name;
    std::string displayName;  // name + age combo
    bool isMale;
    bool isAlive;
    bool isPregnant;
    bool isDeceased;
    PersonId motherId;
    PersonId fatherId;
    FamilyId familyId;
    int generationLevel;  // for layout purposes
    int depth;  // depth in tree
};

struct FamilyDisplay {
    FamilyId id;
    PersonId motherId;
    PersonId fatherId;
    std::vector<PersonId> childrenIds;
    std::string surname;
    int generationLevel;
    int memberCount;  // mother + father + children count
};

} // namespace ftree::presentation
