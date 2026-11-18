
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ftree::presentation {

using FamilyId = uint32_t;
using PersonId = uint32_t;

struct FamilyDisplay {
    FamilyId id = 0;

    PersonId motherId = 0;
    PersonId fatherId = 0;

    std::vector<PersonId> childrenIds;

    std::string surname;

    int generationLevel = 0;
    int memberCount = 0;
};

} // namespace ftree::presentation
