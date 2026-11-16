#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace ftree::presentation {

using FamilyId = uint32_t;
using PersonId = uint32_t;

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
