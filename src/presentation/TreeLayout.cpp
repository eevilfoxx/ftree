#include "presentation/TreeLayout.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ftree::presentation {
namespace {

constexpr float NODE_WIDTH = 150.0f;
constexpr float NODE_HEIGHT = 30.0f;

class DisjointSet {
public:
    void add(PersonId id) {
        parent_.emplace(id, id);
    }

    PersonId find(PersonId id) {
        auto it = parent_.find(id);

        if (it == parent_.end()) {
            return 0;
        }

        if (it->second != id) {
            it->second = find(it->second);
        }

        return it->second;
    }

    void unite(PersonId a, PersonId b) {
        PersonId rootA = find(a);
        PersonId rootB = find(b);

        if (rootA == 0 || rootB == 0 || rootA == rootB) {
            return;
        }

        if (rootA < rootB) {
            parent_[rootB] = rootA;
        } else {
            parent_[rootA] = rootB;
        }
    }

private:
    std::unordered_map<PersonId, PersonId> parent_;
};

struct Component {
    std::vector<PersonId> members;
    double parentPosition = 0.0;
    bool hasParentPosition = false;
    PersonId parentA = 0;
    PersonId parentB = 0;
    PersonId minId = 0;
};

} // namespace

std::vector<LayoutNode> calculateTreeLayout(
    const std::vector<PersonDisplay>& people,
    const std::vector<FamilyDisplay>& families,
    float horizontalSpacing,
    float verticalSpacing
) {
    if (!std::isfinite(horizontalSpacing) ||
        !std::isfinite(verticalSpacing) ||
        horizontalSpacing < 0.0f ||
        verticalSpacing < 0.0f) {
        return {};
    }

    std::unordered_map<PersonId, const PersonDisplay*> peopleById;
    peopleById.reserve(people.size());

    for (const auto& person : people) {
        if (person.id != 0) {
            peopleById.emplace(person.id, &person);
        }
    }

    if (peopleById.empty()) {
        return {};
    }

    std::map<int, std::vector<PersonId>> generations;

    for (const auto& [id, person] : peopleById) {
        generations[std::max(0, person->generationLevel)].push_back(id);
    }

    for (auto& [generation, ids] : generations) {
        (void)generation;
        std::sort(ids.begin(), ids.end());
    }

    // Объединяем партнёров в группы.
    DisjointSet disjointSet;

    for (const auto& [id, person] : peopleById) {
        (void)person;
        disjointSet.add(id);
    }

    for (const auto& family : families) {
        const PersonId mother = family.motherId;
        const PersonId father = family.fatherId;

        if (mother == 0 || father == 0 || mother == father) {
            continue;
        }

        const auto motherIt = peopleById.find(mother);
        const auto fatherIt = peopleById.find(father);

        if (motherIt == peopleById.end() ||
            fatherIt == peopleById.end()) {
            continue;
        }

        const int motherGeneration =
            std::max(0, motherIt->second->generationLevel);

        const int fatherGeneration =
            std::max(0, fatherIt->second->generationLevel);

        if (motherGeneration == fatherGeneration) {
            disjointSet.unite(mother, father);
        }
    }

    // Горизонтальные координаты уже размещённых поколений.
    std::unordered_map<PersonId, float> xPositions;

    std::vector<LayoutNode> result;
    result.reserve(peopleById.size());

    for (const auto& [generation, ids] : generations) {
        std::map<PersonId, Component> components;

        for (PersonId id : ids) {
            components[disjointSet.find(id)].members.push_back(id);
        }

        std::vector<Component> orderedComponents;
        orderedComponents.reserve(components.size());

        for (auto& [root, component] : components) {
            (void)root;

            std::sort(
                component.members.begin(),
                component.members.end()
            );

            component.minId = component.members.front();

            double parentPositionSum = 0.0;
            int knownParents = 0;

            for (PersonId id : component.members) {
                const auto* person = peopleById.at(id);

                PersonId mother = person->motherId;
                PersonId father = person->fatherId;

                if (mother > 0 && father > 0 && father < mother) {
                    std::swap(mother, father);
                }

                if (component.parentA == 0 &&
                    component.parentB == 0) {
                    component.parentA = mother;
                    component.parentB = father;
                }

                for (PersonId parentId : {mother, father}) {
                    const auto parentIt = xPositions.find(parentId);

                    if (parentId != 0 && parentIt != xPositions.end()) {
                        parentPositionSum += parentIt->second;
                        ++knownParents;
                    }
                }
            }

            if (knownParents > 0) {
                component.parentPosition =
                    parentPositionSum / knownParents;
                component.hasParentPosition = true;
            }

            orderedComponents.push_back(std::move(component));
        }

        std::sort(
            orderedComponents.begin(),
            orderedComponents.end(),
            [](const Component& a, const Component& b) {
                if (a.hasParentPosition != b.hasParentPosition) {
                    return a.hasParentPosition > b.hasParentPosition;
                }

                if (a.hasParentPosition &&
                    a.parentPosition != b.parentPosition) {
                    return a.parentPosition < b.parentPosition;
                }

                return std::tie(a.parentA, a.parentB, a.minId) <
                       std::tie(b.parentA, b.parentB, b.minId);
            }
        );

        float x = 0.0f;
        const float nodeStep = NODE_WIDTH + horizontalSpacing;

        for (const auto& component : orderedComponents) {
            for (PersonId id : component.members) {
                LayoutNode node;

                node.id = id;
                node.x = x;
                node.y = static_cast<float>(generation) *
                         (NODE_HEIGHT + verticalSpacing);
                node.width = NODE_WIDTH;
                node.height = NODE_HEIGHT;

                xPositions[id] = x;
                result.push_back(node);

                x += nodeStep;
            }

            x += horizontalSpacing;
        }
    }

    std::sort(
        result.begin(),
        result.end(),
        [](const LayoutNode& a, const LayoutNode& b) {
            return a.id < b.id;
        }
    );

    return result;
}

} // namespace ftree::presentation
