#pragma once

#include "Person.hpp"
#include "Family.hpp"
#include "WorldSnapshot.hpp"
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

    WorldSnapshot createSnapshot() const {
        WorldSnapshot snapshot;
        snapshot.nextPersonId = nextPersonId_;
        snapshot.nextFamilyId = nextFamilyId_;
        snapshot.people.reserve(people_.size());
        for (const auto& entry : people_) {
            if (entry.first != entry.second.id) {
                throw std::logic_error("World person map key does not match PersonData::id");
            }
            snapshot.people.push_back(entry.second);
        }
        std::sort(snapshot.people.begin(), snapshot.people.end(),
            [](const PersonData& a, const PersonData& b) { return a.id < b.id; });

        snapshot.families.reserve(families_.size());
        for (const auto& entry : families_) {
            if (entry.first != entry.second.id) {
                throw std::logic_error("World family map key does not match FamilyData::id");
            }
            snapshot.families.push_back(entry.second);
        }
        std::sort(snapshot.families.begin(), snapshot.families.end(),
            [](const FamilyData& a, const FamilyData& b) { return a.id < b.id; });
        return snapshot;
    }

    static World fromSnapshot(const WorldSnapshot& snapshot) {
        World restored;
        PersonId maxPersonId = 0;
        FamilyId maxFamilyId = 0;

        for (const auto& person : snapshot.people) {
            if (person.id == 0 || person.age < 0
                || (person.gender != Gender::Male && person.gender != Gender::Female)) {
                throw std::invalid_argument("WorldSnapshot contains an invalid person ID, gender, or age");
            }
            if (person.pregnant && (!person.alive || person.gender != Gender::Female)) {
                throw std::invalid_argument("WorldSnapshot contains an invalid pregnancy state");
            }
            if (!restored.people_.emplace(person.id, person).second) {
                throw std::invalid_argument("WorldSnapshot contains duplicate person IDs");
            }
            maxPersonId = std::max(maxPersonId, person.id);
        }

        for (const auto& family : snapshot.families) {
            if (family.id == 0 || family.motherId == 0 || family.fatherId == 0
                || family.motherId == family.fatherId) {
                throw std::invalid_argument("WorldSnapshot contains an invalid family");
            }
            if (!restored.families_.emplace(family.id, family).second) {
                throw std::invalid_argument("WorldSnapshot contains duplicate family IDs");
            }
            maxFamilyId = std::max(maxFamilyId, family.id);
        }

        if (snapshot.nextPersonId != 0 && snapshot.nextPersonId <= maxPersonId) {
            throw std::invalid_argument("WorldSnapshot nextPersonId would reuse an existing ID");
        }
        if (snapshot.nextFamilyId != 0 && snapshot.nextFamilyId <= maxFamilyId) {
            throw std::invalid_argument("WorldSnapshot nextFamilyId would reuse an existing ID");
        }

        for (const auto& [id, person] : restored.people_) {
            (void)id;
            const auto requirePerson = [&restored](PersonId relatedId, const char* relation) {
                if (relatedId != 0 && restored.people_.find(relatedId) == restored.people_.end()) {
                    throw std::invalid_argument(std::string("WorldSnapshot has missing ") + relation);
                }
            };
            requirePerson(person.motherId, "mother");
            requirePerson(person.fatherId, "father");
            if (person.motherId == person.id || person.fatherId == person.id
                || (person.motherId != 0 && person.motherId == person.fatherId)) {
                throw std::invalid_argument("WorldSnapshot contains an invalid parent relationship");
            }
            if (person.motherId != 0
                && restored.people_.at(person.motherId).gender != Gender::Female) {
                throw std::invalid_argument("WorldSnapshot mother must be female");
            }
            if (person.fatherId != 0
                && restored.people_.at(person.fatherId).gender != Gender::Male) {
                throw std::invalid_argument("WorldSnapshot father must be male");
            }
            if (person.familyId != 0) {
                const auto family = restored.families_.find(person.familyId);
                if (family == restored.families_.end()) {
                    throw std::invalid_argument("WorldSnapshot person references a missing family");
                }
                if (family->second.motherId != person.id && family->second.fatherId != person.id) {
                    throw std::invalid_argument("WorldSnapshot person family link does not point to a partner family");
                }
            }
        }

        for (const auto& [id, family] : restored.families_) {
            const auto mother = restored.people_.find(family.motherId);
            const auto father = restored.people_.find(family.fatherId);
            if (mother == restored.people_.end() || father == restored.people_.end()) {
                throw std::invalid_argument("WorldSnapshot family references a missing parent");
            }
            if (mother->second.gender != Gender::Female || father->second.gender != Gender::Male) {
                throw std::invalid_argument("WorldSnapshot family has parents with invalid genders");
            }
            if (mother->second.familyId != id || father->second.familyId != id) {
                throw std::invalid_argument("WorldSnapshot family and parent family links disagree");
            }
            std::unordered_map<PersonId, bool> seenChildren;
            for (PersonId childId : family.childrenIds) {
                const auto child = restored.people_.find(childId);
                if (child == restored.people_.end() || childId == family.motherId
                    || childId == family.fatherId || !seenChildren.emplace(childId, true).second) {
                    throw std::invalid_argument("WorldSnapshot contains an invalid family child");
                }
                if (child->second.motherId != family.motherId
                    || child->second.fatherId != family.fatherId) {
                    throw std::invalid_argument("WorldSnapshot child parent links disagree with family");
                }
            }
        }

        restored.nextPersonId_ = snapshot.nextPersonId;
        restored.nextFamilyId_ = snapshot.nextFamilyId;
        return restored;
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
