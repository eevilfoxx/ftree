#pragma once

#include "presentation/PersonDisplay.hpp"
#include "presentation/FamilyDisplay.hpp"

#include <vector>

namespace ftree::presentation {

struct LayoutNode {
    PersonId id = 0;

    float x = 0.0f;
    float y = 0.0f;

    float width = 150.0f;
    float height = 30.0f;
};

// Рассчитывает расположение всех людей генеалогического дерева.
// Координаты вычисляются на основании generationLevel
// и родственных связей.
std::vector<LayoutNode> calculateTreeLayout(
    const std::vector<PersonDisplay>& people,
    const std::vector<FamilyDisplay>& families,
    float horizontalSpacing = 80.0f,
    float verticalSpacing = 100.0f
);

} // namespace ftree::presentation
