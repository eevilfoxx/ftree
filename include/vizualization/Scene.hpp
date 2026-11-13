#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "managers/FamilyManager.hpp"
#include "managers/PopulationManager.hpp"
#include "entities/repos/IPersonRepository.hpp"
#include <unordered_map>
#include <vector>
#include <memory>


class Scene {
public:
    Scene(std::shared_ptr<FamilyManager> familyManager,
          std::shared_ptr<PopulationManager> populationManager,
          std::shared_ptr<IPersonRepository> personRepository);
    
    void update();
    void draw(sf::RenderWindow& window, const sf::View& view);
    
    // Взаимодействие с объектами сцены
    int getPersonAtPosition(const sf::Vector2f& worldPos, float zoomLevel) const;
    std::optional<sf::Vector2f> getPersonPosition(int personId) const;
    
    void setSelectedPerson(int personId) { selectedPersonId_ = personId; }
    void setHoveredPerson(int personId) { hoveredPersonId_ = personId; }

    int getHoveredPersonId() {return hoveredPersonId_;}
    int getSelectedPersonId() {return selectedPersonId_;}

    bool loadFont(const std::string& fontPath);
    
private:
    std::shared_ptr<FamilyManager> familyManager_;
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<IPersonRepository> personRepository_;
    
    std::unordered_map<int, sf::Vector2f> personPositions_;
    int selectedPersonId_ = -1;
    int hoveredPersonId_ = -1;
    
    // Настройки отрисовки
    float nodeRadius_ = 20.0f;
    float verticalSpacing_ = 100.0f;
    float horizontalSpacing_ = 80.0f;
    
    sf::Color maleColor_ = sf::Color(100, 150, 255);
    sf::Color femaleColor_ = sf::Color(255, 150, 200);
    sf::Color deceasedColor_ = sf::Color(150, 150, 150);

    struct TreeNode {
    std::shared_ptr<Person> person;
    float x = 0;
    float y = 0;
    float width = 0;
    int height = 0;
    bool visited = false;
    std::vector<std::shared_ptr<TreeNode>> children;
};
        
   const float nodeWidth_ = 120.0f;
    const float collisionMargin_ = 20.0f; // Минимальное расстояние между узлами
    
    // Шрифт для текста
    sf::Font font_;
    bool fontLoaded_ = false;
    // Методы отрисовки
    void calculateLayout(const std::vector<std::shared_ptr<Family>>& families);
    void drawFamilyTree(const std::shared_ptr<Family>& family, float x, float y, int generation);
    void drawFamilyConnections(const std::vector<std::shared_ptr<Family>>& families, sf::RenderWindow& window);
    void drawPerson(const std::shared_ptr<Person>& person, const sf::Vector2f& position, sf::RenderWindow& window);
    void drawMarriageConnection(const sf::Vector2f& pos1, const sf::Vector2f& pos2, sf::RenderWindow& window);
    void drawParentChildConnection(const sf::Vector2f& parentPos, const sf::Vector2f& childPos, sf::RenderWindow& window);
    void drawSiblingConnections(const std::vector<std::shared_ptr<Person>>& children, sf::RenderWindow& window);
    void calculateTreeLayout();
    std::shared_ptr<Person> findRoot(std::shared_ptr<Person> person);
    float layoutSubtree(std::shared_ptr<Person> person, float x, float y, std::unordered_set<int> &visited);
    void calculateInitialPositions(const std::shared_ptr<TreeNode> &node, float x, float y, float levelHeight);
    void checkAndResolveCollisions(const std::shared_ptr<TreeNode>& node);
    void savePositions(const std::shared_ptr<TreeNode>& node);
    void processOrphans(const std::vector<std::shared_ptr<Person>>& allPersons, float startX, float startY);
    void shiftAllDescendants(const std::shared_ptr<TreeNode>& node, float amount);
    void shiftSubtree(const std::shared_ptr<TreeNode>& parent, size_t startIndex, float amount);

    int getGenerationLevel(std::shared_ptr<Person> person,
                       std::unordered_map<int, int>& memo);

    void buildLevels(
    const std::vector<std::shared_ptr<Person>>& allPersons,
    std::unordered_map<int, std::vector<std::shared_ptr<Person>>>& levels,
    std::unordered_map<int, int>& memo
    );

    void sortLevelByParents(std::vector<std::shared_ptr<Person>>& persons);

    std::shared_ptr<TreeNode> buildTree(const std::shared_ptr<Person>& person);
    void calculateTreeSize(std::shared_ptr<TreeNode> node);
    void layoutTree(std::shared_ptr<TreeNode> node, float x, float y, float levelHeight);
    float getTreeWidth(std::shared_ptr<TreeNode> node);
    void saveTreePositions(std::shared_ptr<TreeNode> node);
};