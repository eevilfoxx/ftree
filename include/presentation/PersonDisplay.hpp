
#pragma once

#include <cstdint>
#include <string>

namespace ftree::presentation {

using PersonId = uint32_t;
using FamilyId = uint32_t;

struct PersonDisplay {
    PersonId id = 0;

    std::string name;
    std::string displayName;

    int age = 0;

    bool isMale = false;
    bool isAlive = true;
    bool isPregnant = false;
    bool isDeceased = false;

    PersonId motherId = 0;
    PersonId fatherId = 0;
    FamilyId familyId = 0;

    int generationLevel = 0;
    int depth = 0;
};

} // namespace ftree::presentation
