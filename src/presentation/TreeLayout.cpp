#include "presentation/TreeLayout.hpp"
#include <cmath>
#include <algorithm>

namespace ftree::presentation {

LayoutNode calculateLayout(
    const PersonDisplay& person,
    const FamilyDisplay& family,
    int generation,
    float nodeWidth,
    float hSpacing,
    float vSpacing,
    const std::vector<LayoutNode>& parentSiblings
) {
    LayoutNode node;
    node.id = person.id;
    node.width = nodeWidth;
    node.height = 30;  // fixed height for text
    
    // Position based on generation
    node.x = generation * hSpacing;
    node.y = generation * vSpacing;
    
    // Add children if any
    // In a real implementation, we'd iterate through family.childrenIds
    // and calculate their layouts recursively
    
    return node;
}

std::vector<LayoutNode> calculateFamilyLayout(
    const FamilyDisplay& family,
    const std::vector<presentation::PersonDisplay>& people,
    float hSpacing,
    float vSpacing
) {
    std::vector<LayoutNode> result;
    
    // Calculate layout for parents
    // Mother
    if (family.motherId > 0) {
        auto it = std::find_if(people.begin(), people.end(),
            [&](const presentation::PersonDisplay& p) { return p.id == family.motherId; });
        if (it != people.end()) {
            LayoutNode motherNode = calculateLayout(*it, family, 0, 150.f, vSpacing);
            result.push_back(motherNode);
        }
    }
    
    // Father
    if (family.fatherId > 0) {
        auto it = std::find_if(people.begin(), people.end(),
            [&](const presentation::PersonDisplay& p) { return p.id == family.fatherId; });
        if (it != people.end()) {
            LayoutNode fatherNode = calculateLayout(*it, family, 0, 150.f, vSpacing);
            result.push_back(fatherNode);
        }
    }
    
    // Children
    for (PersonId childId : family.childrenIds) {
        auto it = std::find_if(people.begin(), people.end(),
            [&](const presentation::PersonDisplay& p) { return p.id == childId; });
        if (it != people.end()) {
            // Children are in generation 1 more than parents
            LayoutNode childNode = calculateLayout(*it, family, 1, hSpacing, vSpacing);
            result.push_back(childNode);
        }
    }
    
    return result;
}

int getGenerationLevel(
    PersonId id,
    const std::vector<presentation::PersonDisplay>& people
) {
    // Simple generation calculation based on depth
    // In a full implementation, would trace parent links
    return 0;
}

bool checkHorizontalOverlap(
    const LayoutNode& a,
    const LayoutNode& b,
    float nodeWidth
) {
    // Simple overlap check
    float aRight = a.x + nodeWidth;
    float bRight = b.x + nodeWidth;
    
    return (a.x < bRight && aRight > b.x);
}
}