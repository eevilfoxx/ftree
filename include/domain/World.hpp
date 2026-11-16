#pragma once

#include "Person.hpp"
#include "Family.hpp"
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>
#include <cstdint>
#include <stdexcept>

namespace ftree::domain {

using PersonId = uint32_t;
using FamilyId = uint32_t;

class World {
public:
    World() = default;

    // Create operations
    PersonId createPerson(const std::string& name, Gender gender, int age = 0) {
        PersonId newId = 1;
        while (personExists(newId)) { newId++; }
        PersonData data;
        data.id = newId;
        data.name = name;
        data.gender = gender;
        data.age = age;
        data.alive = true;
        data.pregnant = (gender == Gender::Female);
        data.motherId = 0;
        data.fatherId = 0;
        data.familyId = 0;
        people_[newId] = data;
        return newId;
    }

    FamilyId createFamily(PersonId motherId, PersonId fatherId, const std::string& surname = "") {
        FamilyId newId = 1;
        while (familyExists(newId)) { newId++; }
        // Use FamilyData from Family.hpp
        families_.emplace(newId, FamilyData{newId, motherId, fatherId, {}, surname});
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
        if (data.motherId != 0) members.push_back(data.motherId);
        if (data.fatherId != 0) members.push_back(data.fatherId);
        members.insert(members.end(), data.childrenIds.begin(), data.childrenIds.end());
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
        if (data.motherId != 0) people_[data.motherId].familyId = 0;
        if (data.fatherId != 0) people_[data.fatherId].familyId = 0;
        data.childrenIds.clear();
    }

private:
    std::unordered_map<PersonId, PersonData> people_;
    std::unordered_map<FamilyId, FamilyData> families_;
};  // closes class World

}  // closes namespace ftree::domain
