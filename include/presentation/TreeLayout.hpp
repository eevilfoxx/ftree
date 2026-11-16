#pragma once

#include "presentation/PersonDisplay.hpp"
#include "presentation/FamilyDisplay.hpp"
#include <vector>
#include <map>
#include <optional>

namespace ftree::presentation {

// Layout result for a node
struct LayoutNode {
    PersonId id;
    float x = 0.f;
    float y = 0.f;
    float width = 0.f;
    int height = 0;
    bool hasCollision = false;
    std::vector<LayoutNode> children;
};

// Calculate layout for a person and their descendants
LayoutNode calculateLayout(
    const PersonDisplay& person,
    const FamilyDisplay& family,
    int generation,
    float nodeWidth,
    float hSpacing,
    float vSpacing,
    const std::vector<LayoutNode>& parentSiblings = {}
);

// Calculate layouts for all people in a family
std::vector<LayoutNode> calculateFamilyLayout(
    const FamilyDisplay& family,
    const std::vector<PersonDisplay>& people,
    float hSpacing = 80.f,
    float vSpacing = 100.f
);

// Get the generation level for a person
int getGenerationLevel(
    PersonId id,
    const std::vector<PersonDisplay>& people
);

// Check if two nodes overlap horizontally
bool checkHorizontalOverlap(
    const LayoutNode& a,
    const LayoutNode& b,
    float nodeWidth
);

} // namespace ftree::presentation
