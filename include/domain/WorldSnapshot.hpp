#pragma once

#include "domain/Person.hpp"
#include "domain/Family.hpp"

#include <vector>

namespace ftree::domain {

// A complete, in-memory representation of the mutable state owned by World.
// The vectors are sorted by ID when produced by World::createSnapshot().
// Their order is not semantically significant when restoring a World, except
// that FamilyData::childrenIds order is preserved exactly.
struct WorldSnapshot {
    std::vector<PersonData> people;
    std::vector<FamilyData> families;

    // Zero means that the corresponding ID space has been exhausted.
    // These counters must be persisted: deriving them from current records
    // would allow reuse of IDs after records have been removed.
    PersonId nextPersonId = 1;
    FamilyId nextFamilyId = 1;
};

} // namespace ftree::domain
