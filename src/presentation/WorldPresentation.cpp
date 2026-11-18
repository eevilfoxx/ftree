
#include "presentation/WorldPresentation.hpp"

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ftree::presentation {
namespace {

// Converts domain data into presentation data.
// The function does not modify the domain model.
PersonDisplay makePersonDisplay(
    const domain::PersonData& person
) {
    PersonDisplay result;

    result.id = person.id;
    result.name = person.name;
    result.age = person.age;

    result.displayName =
        person.name + " " + std::to_string(person.age) + " yrs";

    result.isMale =
        person.gender == domain::Gender::Male;

    result.isAlive = person.alive;
    result.isPregnant = person.pregnant;
    result.isDeceased = !person.alive;

    result.motherId = person.motherId;
    result.fatherId = person.fatherId;
    result.familyId = person.familyId;

    return result;
}

// Calculates generation levels from parent-child relationships.
//
// A person without known parents belongs to generation 0.
// If one or both parents are known, the person's generation is
// one greater than the highest known parent generation.
//
// The state map prevents infinite recursion if malformed data
// contains cyclic parent references.
std::unordered_map<domain::PersonId, int> calculateGenerationLevels(
    const domain::World& world
) {
    const auto& people = world.getAllPeopleIncludingDead();

    std::unordered_map<domain::PersonId, int> generations;
    std::unordered_map<domain::PersonId, unsigned char> states;

    generations.reserve(people.size());
    states.reserve(people.size());

    // 0 = not visited
    // 1 = currently calculating
    // 2 = calculated
    std::function<int(domain::PersonId)> calculateGeneration;

    calculateGeneration = [&](domain::PersonId personId) -> int {
        const auto personIt = people.find(personId);

        if (personId == 0 || personIt == people.end()) {
            return 0;
        }

        const auto stateIt = states.find(personId);

        if (stateIt != states.end()) {
            if (stateIt->second == 2) {
                return generations.at(personId);
            }

            if (stateIt->second == 1) {
                // A cycle was encountered.
                // Treat this reference as a root to stop recursion.
                return 0;
            }
        }

        states[personId] = 1;

        const domain::PersonData& person = personIt->second;

        int parentGeneration = -1;

        if (person.motherId != 0 &&
            people.find(person.motherId) != people.end()) {
            parentGeneration = std::max(
                parentGeneration,
                calculateGeneration(person.motherId)
            );
        }

        if (person.fatherId != 0 &&
            people.find(person.fatherId) != people.end()) {
            parentGeneration = std::max(
                parentGeneration,
                calculateGeneration(person.fatherId)
            );
        }

        const int generation =
            parentGeneration < 0 ? 0 : parentGeneration + 1;

        generations[personId] = generation;
        states[personId] = 2;

        return generation;
    };

    for (const auto& [personId, person] : people) {
        (void)person;
        calculateGeneration(personId);
    }

    return generations;
}

} // namespace

std::vector<PersonDisplay> WorldPresentation::getAllPeople(
    const domain::World& world
) {
    const auto& people = world.getAllPeopleIncludingDead();

    const auto generations = calculateGenerationLevels(world);

    std::vector<PersonDisplay> result;
    result.reserve(people.size());

    for (const auto& [id, person] : people) {
        (void)id;

        PersonDisplay display = makePersonDisplay(person);

        const auto generationIt = generations.find(person.id);

        if (generationIt != generations.end()) {
            display.generationLevel = generationIt->second;
        }

        result.push_back(std::move(display));
    }

    std::sort(
        result.begin(),
        result.end(),
        [](const PersonDisplay& first, const PersonDisplay& second) {
            return first.id < second.id;
        }
    );

    return result;
}

std::vector<FamilyDisplay> WorldPresentation::getAllFamilies(
    const domain::World& world
) {
    const auto& families = world.getAllFamilies();

    const auto generations = calculateGenerationLevels(world);

    std::vector<FamilyDisplay> result;
    result.reserve(families.size());

    for (const auto& [id, family] : families) {
        (void)id;

        FamilyDisplay display;

        display.id = family.id;
        display.motherId = family.motherId;
        display.fatherId = family.fatherId;
        display.childrenIds = family.childrenIds;
        display.surname = family.surname;

        std::sort(
            display.childrenIds.begin(),
            display.childrenIds.end()
        );

        display.childrenIds.erase(
            std::unique(
                display.childrenIds.begin(),
                display.childrenIds.end()
            ),
            display.childrenIds.end()
        );

        std::unordered_set<domain::PersonId> members;

        if (display.motherId != 0 &&
            world.personExists(display.motherId)) {
            members.insert(display.motherId);
        }

        if (display.fatherId != 0 &&
            world.personExists(display.fatherId)) {
            members.insert(display.fatherId);
        }

        int parentGeneration = -1;

        const auto motherGeneration =
            generations.find(display.motherId);

        if (motherGeneration != generations.end()) {
            parentGeneration = std::max(
                parentGeneration,
                motherGeneration->second
            );
        }

        const auto fatherGeneration =
            generations.find(display.fatherId);

        if (fatherGeneration != generations.end()) {
            parentGeneration = std::max(
                parentGeneration,
                fatherGeneration->second
            );
        }

        for (domain::PersonId childId : display.childrenIds) {
            if (world.personExists(childId)) {
                members.insert(childId);
            }
        }

        display.memberCount = static_cast<int>(members.size());

        display.generationLevel =
            parentGeneration < 0 ? 0 : parentGeneration;

        result.push_back(std::move(display));
    }

    std::sort(
        result.begin(),
        result.end(),
        [](const FamilyDisplay& first, const FamilyDisplay& second) {
            return first.id < second.id;
        }
    );

    return result;
}

std::vector<domain::PersonId>
WorldPresentation::getLivingPeopleIds(
    const domain::World& world
) {
    auto result = world.getLivingPeople();

    std::sort(result.begin(), result.end());

    return result;
}

std::vector<domain::PersonId>
WorldPresentation::getFamilyMembers(
    const domain::World& world,
    domain::FamilyId familyId
) {
    auto result = world.getFamilyMembers(familyId);

    std::sort(result.begin(), result.end());

    return result;
}

PersonDisplay WorldPresentation::getPersonData(
    const domain::World& world,
    domain::PersonId id
) {
    const auto person = world.getPerson(id);

    if (!person.has_value()) {
        throw std::out_of_range(
            "Person with ID " + std::to_string(id) +
            " does not exist"
        );
    }

    PersonDisplay result = makePersonDisplay(person.value());

    const auto generations = calculateGenerationLevels(world);
    const auto generationIt = generations.find(id);

    if (generationIt != generations.end()) {
        result.generationLevel = generationIt->second;
    }

    return result;
}

} // namespace ftree::presentation
