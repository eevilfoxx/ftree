#pragma once

#include "Person.hpp"
#include "Family.hpp"
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <limits>
#include <algorithm>

namespace ftree::domain {

using PersonId = uint32_t;
using FamilyId = uint32_t;

class World {
public:
    World() = default;

    // Create operations
    PersonId createPerson(const std::string& name, Gender gender, int age = 0) {
        const PersonId newId = allocatePersonId();
        PersonData data;
        data.id = newId;
        data.name = name;
        data.gender = gender;
        data.age = age;
        data.alive = true;
        data.pregnant = false;
        data.motherId = 0;
        data.fatherId = 0;
        data.familyId = 0;
        people_[newId] = data;
        return newId;
    }

    FamilyId createFamily(PersonId motherId, PersonId fatherId, const std::string& surname = "") {
        const auto motherIt = people_.find(motherId);
        const auto fatherIt = people_.find(fatherId);
        if (motherIt == people_.end() || fatherIt == people_.end()) {
            throw std::invalid_argument("Cannot create a family with an unknown parent");
        }
        if (motherId == fatherId) {
            throw std::invalid_argument("A person cannot be both parents");
        }
        if (motherIt->second.gender != Gender::Female
            || fatherIt->second.gender != Gender::Male) {
            throw std::invalid_argument("Family parents must be female mother and male father");
        }
        if (!motherIt->second.alive || !fatherIt->second.alive) {
            throw std::invalid_argument("Cannot create a family with a deceased parent");
        }
        if (motherIt->second.familyId != 0 || fatherIt->second.familyId != 0) {
            throw std::invalid_argument("A parent already belongs to a family");
        }

        const FamilyId newId = allocateFamilyId();
        families_.emplace(newId, FamilyData{newId, motherId, fatherId, {}, surname});
        motherIt->second.familyId = newId;
        fatherIt->second.familyId = newId;
        return newId;
    }

    // Search by ID
    std::optional<PersonData> getPerson(PersonId id) const {
        auto it = people_.find(id);
        if (it == people_.end()) return std::nullopt;
        return it->second;
    }

    std::optional<FamilyData> getFamily(FamilyId id) const {
        auto it = families_.find(id);
        if (it == families_.end()) return std::nullopt;
        return it->second;
    }

    // Reference access (for tests/internals)
    PersonData& getPersonRef(PersonId id) { return people_.at(id); }
    const PersonData& getPersonRef(PersonId id) const { return people_.at(id); }
    FamilyData& getFamilyRef(FamilyId id) { return families_.at(id); }
    const FamilyData& getFamilyRef(FamilyId id) const { return families_.at(id); }

    // Collection access
    const std::unordered_map<PersonId, PersonData>& getAllPeople() const { return people_; }
    const std::unordered_map<FamilyId, FamilyData>& getAllFamilies() const { return families_; }

    // Living people
    std::vector<PersonId> getLivingPeople() const {
        std::vector<PersonId> living;
        for (const auto& [id, data] : people_) {
            if (data.alive) living.push_back(id);
        }
        return living;
    }

    // Existence check
    bool personExists(PersonId id) const { return people_.find(id) != people_.end(); }
    bool familyExists(FamilyId id) const { return families_.find(id) != families_.end(); }

    // Get all people including dead (for genealogy)
    const std::unordered_map<PersonId, PersonData>& getAllPeopleIncludingDead() const { return people_; }

    // Family members - compute directly from FamilyData
    std::vector<PersonId> getFamilyMembers(FamilyId familyId) const {
        if (!familyExists(familyId)) return {};
        auto& data = families_.at(familyId);
        std::vector<PersonId> members;
        const auto appendUnique = [&members](PersonId personId) {
            if (personId != 0
                && std::find(members.begin(), members.end(), personId) == members.end()) {
                members.push_back(personId);
            }
        };

        appendUnique(data.motherId);
        appendUnique(data.fatherId);
        for (PersonId childId : data.childrenIds) {
            appendUnique(childId);
        }
        return members;
    }

    // Delete/archive operations
    void killPerson(PersonId id) {
        if (!personExists(id)) return;
        people_[id].alive = false;
        people_[id].pregnant = false;
    }

    void archivePerson(PersonId id) { killPerson(id); }

    void dissolveFamily(FamilyId id) {
        if (!familyExists(id)) return;
        auto& data = families_.at(id);
        const auto clearFamilyLink = [this, id](PersonId personId) {
            const auto personIt = people_.find(personId);
            if (personIt != people_.end() && personIt->second.familyId == id) {
                personIt->second.familyId = 0;
            }
        };

        clearFamilyLink(data.motherId);
        clearFamilyLink(data.fatherId);
        for (PersonId childId : data.childrenIds) {
            clearFamilyLink(childId);
        }
        data.childrenIds.clear();
        families_.erase(id);
    }

private:
    PersonId allocatePersonId() {
        if (nextPersonId_ == 0) {
            throw std::overflow_error("Person ID space exhausted");
        }

        const PersonId id = nextPersonId_;
        nextPersonId_ = id == std::numeric_limits<PersonId>::max() ? 0 : id + 1;
        return id;
    }

    FamilyId allocateFamilyId() {
        if (nextFamilyId_ == 0) {
            throw std::overflow_error("Family ID space exhausted");
        }

        const FamilyId id = nextFamilyId_;
        nextFamilyId_ = id == std::numeric_limits<FamilyId>::max() ? 0 : id + 1;
        return id;
    }

    PersonId nextPersonId_ = 1;
    FamilyId nextFamilyId_ = 1;
    std::unordered_map<PersonId, PersonData> people_;
    std::unordered_map<FamilyId, FamilyData> families_;
};  // closes class World

}  // closes namespace ftree::domain
