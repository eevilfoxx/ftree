#include "visualization/Scene.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

Scene::Scene(std::shared_ptr<FamilyManager> familyManager,
             std::shared_ptr<PopulationManager> populationManager,
             std::shared_ptr<IPersonRepository> personRepository)
    : familyManager_(familyManager), populationManager_(populationManager), 
      personRepository_(personRepository) {}

void Scene::update() {
    calculateTreeLayout();
}

void Scene::draw(sf::RenderWindow& window, const sf::View& /*view*/) {
    auto families = familyManager_->getAllFamilies();
    drawFamilyConnections(families, window);
    
    // Рисуем всех персонажей
    for (const auto& [id, pos] : personPositions_) {
        auto person = personRepository_->getById(id);
        if (person) {
            drawPerson(person, pos, window);
            
            // Подсветка выбранного человека
            if (selectedPersonId_ == id) {
                sf::CircleShape highlight(nodeRadius_ * 1.2f);
                highlight.setPosition(pos.x - nodeRadius_ * 1.2f, pos.y - nodeRadius_ * 1.2f);
                highlight.setFillColor(sf::Color::Transparent);
                highlight.setOutlineThickness(3.0f);
                highlight.setOutlineColor(sf::Color::Yellow);
                window.draw(highlight);
            }
            
            // Подсветка при наведении
            if (hoveredPersonId_ == id) {
                sf::CircleShape hoverEffect(nodeRadius_ * 1.1f);
                hoverEffect.setPosition(pos.x - nodeRadius_ * 1.1f, pos.y - nodeRadius_ * 1.1f);
                hoverEffect.setFillColor(sf::Color(255, 255, 0, 50));
                window.draw(hoverEffect);
            }
        }
    }
}

void Scene::calculateTreeLayout() {
    personPositions_.clear();

    auto allPersons = personRepository_->getAll();

    std::unordered_map<int, int> levelMemo;
    std::unordered_map<int, std::vector<std::shared_ptr<Person>>> levels;

    buildLevels(allPersons, levels, levelMemo);

    float startX = 100.f;
    float startY = 100.f;

    // ВАЖНО: уровни надо обходить по порядку
    std::vector<int> sortedLevels;
    for (auto& kv : levels)
        sortedLevels.push_back(kv.first);

    std::sort(sortedLevels.begin(), sortedLevels.end());

    for (int level : sortedLevels) {
        auto& persons = levels[level];

        float y = startY + level * verticalSpacing_;

        // 🔥 сначала сортируем относительно родителей
        sortLevelByParents(persons);

        // 🔥 центрируем уровень
        float startLevelX = startX;

        for (size_t i = 0; i < persons.size(); ++i) {
            float x = startLevelX + i * horizontalSpacing_;

            personPositions_[persons[i]->getUuid()] = {x, y};
        }
    }
}

void Scene::drawFamilyConnections(const std::vector<std::shared_ptr<Family>>& families, sf::RenderWindow& window) {
    // Рисуем брачные связи
    for (const auto& family : families) {
        if (!family) continue;
        
        auto father = family->getFather();
        auto mother = family->getMother();
        
        // Рисуем связь между мужем и женой
        if (father && mother) {
            auto fatherPos = getPersonPosition(father->getUuid());
            auto motherPos = getPersonPosition(mother->getUuid());
            
            if (fatherPos.has_value() && motherPos.has_value()) {
                drawMarriageConnection(fatherPos.value(), motherPos.value(), window);
            }
        }
    }
    
    // Рисуем родительско-детские связи
    for (const auto& [id, pos] : personPositions_) {
        auto person = personRepository_->getById(id);
        if (!person) continue;
        
        // Связи с детьми
        auto family = person->getFamily();
        if (family) {
            auto children = family->getChildren();
            for (const auto& child : children) {
                if (!child) continue;
                
                auto childPos = getPersonPosition(child->getUuid());
                if (childPos.has_value()) {
                    drawParentChildConnection(pos, childPos.value(), window);
                }
            }
        }
        
        // Связи с родителями
        std::vector<std::shared_ptr<Person>> parents = {person->getMother(), person->getFather()};
        for (const auto& parent : parents) {
            if (!parent) continue;
            
            auto parentPos = getPersonPosition(parent->getUuid());
            if (parentPos.has_value()) {
                drawParentChildConnection(parentPos.value(), pos, window);
            }
        }
    }
    
    // Рисуем связи между братьями и сестрами
    for (const auto& family : families) {
        if (!family) continue;
        
        auto children = family->getChildren();
        drawSiblingConnections(children, window);
    }
}

int Scene::getGenerationLevel(std::shared_ptr<Person> person,
                             std::unordered_map<int, int>& memo) {
    if (!person) return 0;

    int id = person->getUuid();

    auto it = memo.find(id);
    if (it != memo.end())
        return it->second;

    int level = 0;

    auto mother = person->getMother();
    auto father = person->getFather();

    if (mother || father) {
        int parentLevel = 0;

        if (mother)
            parentLevel = std::max(parentLevel, getGenerationLevel(mother, memo));

        if (father)
            parentLevel = std::max(parentLevel, getGenerationLevel(father, memo));

        level = parentLevel + 1;
    }

    memo[id] = level;
    return level;
}

void Scene::buildLevels(
    const std::vector<std::shared_ptr<Person>>& allPersons,
    std::unordered_map<int, std::vector<std::shared_ptr<Person>>>& levels,
    std::unordered_map<int, int>& memo) {

    for (auto& person : allPersons) {
        if (!person) continue;

        int level = getGenerationLevel(person, memo);
        levels[level].push_back(person);
    }
}

void Scene::sortLevelByParents(std::vector<std::shared_ptr<Person>>& persons) {
    std::sort(persons.begin(), persons.end(),
        [&](const std::shared_ptr<Person>& a,
            const std::shared_ptr<Person>& b) {

            float ax = 0;
            float bx = 0;
            int ac = 0;
            int bc = 0;

            for (auto parent : {a->getMother(), a->getFather()}) {
                if (parent) {
                    auto it = personPositions_.find(parent->getUuid());
                    if (it != personPositions_.end()) {
                        ax += it->second.x;
                        ac++;
                    }
                }
            }

            for (auto parent : {b->getMother(), b->getFather()}) {
                if (parent) {
                    auto it = personPositions_.find(parent->getUuid());
                    if (it != personPositions_.end()) {
                        bx += it->second.x;
                        bc++;
                    }
                }
            }

            if (ac > 0) ax /= ac;
            if (bc > 0) bx /= bc;

            return ax < bx;
        });
}

void Scene::drawMarriageConnection(const sf::Vector2f& pos1, const sf::Vector2f& pos2, sf::RenderWindow& window) {
    sf::VertexArray line(sf::LineStrip, 2);
    line[0].position = pos1;
    line[1].position = pos2;
    line[0].color = sf::Color(150, 150, 150, 200);
    line[1].color = sf::Color(150, 150, 150, 200);
    
    if (std::abs(pos1.x - pos2.x) > 100) {
        sf::VertexArray curve(sf::LineStrip, 3);
        curve[0].position = pos1;
        curve[1].position = sf::Vector2f((pos1.x + pos2.x) / 2, (pos1.y + pos2.y) / 2 - 20);
        curve[2].position = pos2;
        curve[0].color = sf::Color(150, 150, 150, 180);
        curve[1].color = sf::Color(150, 150, 150, 180);
        curve[2].color = sf::Color(150, 150, 150, 180);
        window.draw(curve);
    } else {
        window.draw(line);
    }
}

void Scene::drawParentChildConnection(const sf::Vector2f& parentPos, const sf::Vector2f& childPos, sf::RenderWindow& window) {
    // Упрощенная версия - прямая линия
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = parentPos;
    line[1].position = childPos;
    line[0].color = sf::Color(100, 100, 100, 150);
    line[1].color = sf::Color(100, 100, 100, 150);
    window.draw(line);
}

void Scene::drawSiblingConnections(const std::vector<std::shared_ptr<Person>>& children, sf::RenderWindow& window) {
    if (children.size() < 2) return;
    
    std::vector<std::pair<sf::Vector2f, std::shared_ptr<Person>>> sortedChildren;
    for (const auto& child : children) {
        if (!child) continue;
        auto pos = getPersonPosition(child->getUuid());
        if (pos.has_value()) {
            sortedChildren.emplace_back(pos.value(), child);
        }
    }
    
    std::sort(sortedChildren.begin(), sortedChildren.end(), 
              [](const auto& a, const auto& b) { return a.first.x < b.first.x; });
    
    if (sortedChildren.size() > 1) {
        float minX = sortedChildren.front().first.x;
        float maxX = sortedChildren.back().first.x;
        float avgY = 0;
        
        for (const auto& [pos, child] : sortedChildren) {
            avgY += pos.y;
        }
        avgY /= sortedChildren.size();
        
        sf::VertexArray siblingLine(sf::Lines, 2);
        siblingLine[0].position = sf::Vector2f(minX, avgY);
        siblingLine[1].position = sf::Vector2f(maxX, avgY);
        siblingLine[0].color = sf::Color(200, 200, 100, 100);
        siblingLine[1].color = sf::Color(200, 200, 100, 100);
        window.draw(siblingLine);
        
        for (const auto& [pos, child] : sortedChildren) {
            sf::VertexArray connection(sf::Lines, 2);
            connection[0].position = sf::Vector2f(pos.x, avgY);
            connection[1].position = pos;
            connection[0].color = sf::Color(200, 200, 100, 100);
            connection[1].color = sf::Color(200, 200, 100, 100);
            window.draw(connection);
        }
    }
}

void Scene::drawPerson(const std::shared_ptr<Person>& person, const sf::Vector2f& position, sf::RenderWindow& window) {
    if (!person) return;
    
    // Основной круг персоны
    sf::CircleShape shape(nodeRadius_);
    shape.setPosition(position.x - nodeRadius_, position.y - nodeRadius_);
    
    if (person->isAlive()) {
        shape.setFillColor(person->getGender() == Gender::Male ? maleColor_ : femaleColor_);
    } else {
        shape.setFillColor(deceasedColor_);
    }
    
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::Black);
    window.draw(shape);
    
    // Символ пола - только если шрифт загружен
    if (fontLoaded_) {
        sf::Text symbol;
        symbol.setFont(font_);
        symbol.setString(person->getGender() == Gender::Male ? "M" : "F"); // Используем простые символы
        symbol.setCharacterSize(16);
        symbol.setFillColor(sf::Color::White);
        symbol.setStyle(sf::Text::Bold);
        
        auto bounds = symbol.getLocalBounds();
        symbol.setPosition(position.x - bounds.width / 2, position.y - bounds.height / 2 - 5);
        window.draw(symbol);
    }
    
    // Информация о персоне - только если шрифт загружен
    if (fontLoaded_) {
        sf::Text infoText;
        infoText.setFont(font_);
        infoText.setCharacterSize(12);
        infoText.setFillColor(sf::Color::Black);
        
        std::string info = person->getName() + "\n" + std::to_string(person->getAge()) + " yrs";
        if (!person->isAlive()) {
            info += "\nDeceased";
        } else if (person->isPregnant() && person->getGender() == Gender::Female) {
            info += "\nPregnant";
        }
        
        infoText.setString(info);
        
        auto bounds = infoText.getLocalBounds();
        infoText.setPosition(position.x - bounds.width / 2, position.y + nodeRadius_ + 5);
        
        sf::RectangleShape textBackground(sf::Vector2f(bounds.width + 10, bounds.height + 5));
        textBackground.setFillColor(sf::Color(255, 255, 255, 230));
        textBackground.setPosition(position.x - bounds.width / 2 - 5, position.y + nodeRadius_ + 3);
        window.draw(textBackground);
        
        window.draw(infoText);
    }
}

int Scene::getPersonAtPosition(const sf::Vector2f& worldPos) const {
    for (const auto& [id, pos] : personPositions_) {
        const float dx = worldPos.x - pos.x;
        const float dy = worldPos.y - pos.y;
        const float hitRadius = nodeRadius_ * 2.0f;
        if (dx * dx + dy * dy <= hitRadius * hitRadius) {
            return id;
        }
    }
    return -1;
}

std::optional<sf::Vector2f> Scene::getPersonPosition(int personId) const {
    auto it = personPositions_.find(personId);
    if (it != personPositions_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool Scene::loadFont(const std::string& fontPath) {
    if (font_.loadFromFile(fontPath)) {
        fontLoaded_ = true;
        return true;
    }
    
    std::cerr << "Failed to load font from: " << fontPath << std::endl;
    fontLoaded_ = false;
    return false;
}