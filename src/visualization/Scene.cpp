#include "visualization/Scene.hpp"

#include "presentation/TreeLayout.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <utility>

namespace ftree::visualization {

void Scene::setData(
    std::vector<presentation::PersonDisplay> people,
    std::vector<presentation::FamilyDisplay> families
) {
    people_ = std::move(people);
    families_ = std::move(families);

    calculateTreeLayout();

    // Не оставляем выбор и наведение на уже отсутствующих людей.
    const auto personExists = [this](int id) {
        if (id < 0) {
            return false;
        }

        return std::any_of(
            people_.begin(),
            people_.end(),
            [id](const presentation::PersonDisplay& person) {
                return person.id == static_cast<presentation::PersonId>(id);
            }
        );
    };

    if (!personExists(selectedPersonId_)) {
        selectedPersonId_ = -1;
    }

    if (!personExists(hoveredPersonId_)) {
        hoveredPersonId_ = -1;
    }
}

void Scene::update() {
    // Scene не изменяет предметную модель.
    // Данные и расположение обновляются через setData().
}

void Scene::calculateTreeLayout() {
    personPositions_.clear();

    const auto layout = presentation::calculateTreeLayout(
        people_,
        families_
    );

    personPositions_.reserve(layout.size());

    for (const auto& node : layout) {
        // TreeLayout возвращает координаты левого верхнего угла
        // прямоугольника узла. В Scene храним центр узла.
        personPositions_.emplace(
            node.id,
            sf::Vector2f(
                node.x + node.width / 2.0f,
                node.y + node.height / 2.0f
            )
        );
    }
}

void Scene::draw(
    sf::RenderWindow& window,
    const sf::View& view
) {
    (void)view;

    drawFamilyConnections(window);

    for (const auto& person : people_) {
        const auto positionIt = personPositions_.find(person.id);

        if (positionIt == personPositions_.end()) {
            continue;
        }

        const sf::Vector2f& position = positionIt->second;

        if (selectedPersonId_ == static_cast<int>(person.id)) {
            drawSelection(position, window);
        }

        if (hoveredPersonId_ == static_cast<int>(person.id)) {
            drawHover(position, window);
        }

        drawPerson(person, position, window);
    }
}

void Scene::drawFamilyConnections(sf::RenderWindow& window) {
    // Брачные связи.
    for (const auto& family : families_) {
        if (family.motherId == 0 || family.fatherId == 0) {
            continue;
        }

        const auto motherIt = personPositions_.find(family.motherId);
        const auto fatherIt = personPositions_.find(family.fatherId);

        if (motherIt == personPositions_.end() ||
            fatherIt == personPositions_.end()) {
            continue;
        }

        drawMarriageConnection(
            motherIt->second,
            fatherIt->second,
            window
        );
    }

    // Родительско-детские связи.
    // Источник происхождения — motherId и fatherId самого человека.
    // Поэтому каждая связь рисуется только один раз.
    for (const auto& person : people_) {
        const auto childIt = personPositions_.find(person.id);

        if (childIt == personPositions_.end()) {
            continue;
        }

        const sf::Vector2f& childPosition = childIt->second;

        for (presentation::PersonId parentId : {
                 person.motherId,
                 person.fatherId
             }) {
            if (parentId == 0 || parentId == person.id) {
                continue;
            }

            const auto parentIt = personPositions_.find(parentId);

            if (parentIt == personPositions_.end()) {
                continue;
            }

            drawParentChildConnection(
                parentIt->second,
                childPosition,
                window
            );
        }
    }

    // Связи между братьями и сёстрами.
    for (const auto& family : families_) {
        drawSiblingConnections(family, window);
    }
}

void Scene::drawMarriageConnection(
    const sf::Vector2f& pos1,
    const sf::Vector2f& pos2,
    sf::RenderWindow& window
) const {
    const sf::Color color(150, 150, 150, 200);

    if (std::abs(pos1.x - pos2.x) > 100.0f) {
        sf::VertexArray curve(sf::LineStrip, 3);

        curve[0].position = pos1;
        curve[1].position = sf::Vector2f(
            (pos1.x + pos2.x) / 2.0f,
            (pos1.y + pos2.y) / 2.0f - 20.0f
        );
        curve[2].position = pos2;

        for (std::size_t i = 0; i < curve.getVertexCount(); ++i) {
            curve[i].color = color;
        }

        window.draw(curve);
    } else {
        sf::VertexArray line(sf::LineStrip, 2);

        line[0].position = pos1;
        line[1].position = pos2;

        line[0].color = color;
        line[1].color = color;

        window.draw(line);
    }
}

void Scene::drawParentChildConnection(
    const sf::Vector2f& parentPos,
    const sf::Vector2f& childPos,
    sf::RenderWindow& window
) const {
    sf::VertexArray line(sf::Lines, 2);

    line[0].position = parentPos;
    line[1].position = childPos;

    const sf::Color color(100, 100, 100, 150);

    line[0].color = color;
    line[1].color = color;

    window.draw(line);
}

void Scene::drawSiblingConnections(
    const presentation::FamilyDisplay& family,
    sf::RenderWindow& window
) const {
    if (family.childrenIds.size() < 2) {
        return;
    }

    std::vector<sf::Vector2f> childrenPositions;
    childrenPositions.reserve(family.childrenIds.size());

    std::unordered_set<presentation::PersonId> processedIds;

    for (presentation::PersonId childId : family.childrenIds) {
        if (childId == 0 || !processedIds.insert(childId).second) {
            continue;
        }

        const auto positionIt = personPositions_.find(childId);

        if (positionIt == personPositions_.end()) {
            continue;
        }

        childrenPositions.push_back(positionIt->second);
    }

    if (childrenPositions.size() < 2) {
        return;
    }

    std::sort(
        childrenPositions.begin(),
        childrenPositions.end(),
        [](const sf::Vector2f& a, const sf::Vector2f& b) {
            return a.x < b.x;
        }
    );

    const float minX = childrenPositions.front().x;
    const float maxX = childrenPositions.back().x;

    float averageY = 0.0f;

    for (const auto& position : childrenPositions) {
        averageY += position.y;
    }

    averageY /= static_cast<float>(childrenPositions.size());

    const sf::Color color(200, 200, 100, 100);

    sf::VertexArray siblingLine(sf::Lines, 2);

    siblingLine[0].position = sf::Vector2f(minX, averageY);
    siblingLine[1].position = sf::Vector2f(maxX, averageY);

    siblingLine[0].color = color;
    siblingLine[1].color = color;

    window.draw(siblingLine);

    for (const auto& position : childrenPositions) {
        sf::VertexArray connection(sf::Lines, 2);

        connection[0].position = sf::Vector2f(position.x, averageY);
        connection[1].position = position;

        connection[0].color = color;
        connection[1].color = color;

        window.draw(connection);
    }
}

void Scene::drawPerson(
    const presentation::PersonDisplay& person,
    const sf::Vector2f& position,
    sf::RenderWindow& window
) const {
    sf::CircleShape shape(nodeRadius_);

    shape.setPosition(
        position.x - nodeRadius_,
        position.y - nodeRadius_
    );

    if (!person.isAlive || person.isDeceased) {
        shape.setFillColor(deceasedColor_);
    } else {
        shape.setFillColor(
            person.isMale ? maleColor_ : femaleColor_
        );
    }

    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::Black);

    window.draw(shape);

    if (!fontLoaded_) {
        return;
    }

    // Символ пола.
    sf::Text symbol;

    symbol.setFont(font_);
    symbol.setString(person.isMale ? "M" : "F");
    symbol.setCharacterSize(16);
    symbol.setFillColor(sf::Color::White);
    symbol.setStyle(sf::Text::Bold);

    const auto symbolBounds = symbol.getLocalBounds();

    symbol.setOrigin(
        symbolBounds.left + symbolBounds.width / 2.0f,
        symbolBounds.top + symbolBounds.height / 2.0f
    );

    symbol.setPosition(position);

    window.draw(symbol);

    // Информация о человеке.
    std::string info = person.name +
        "\n" + std::to_string(person.age) + " yrs";

    if (!person.isAlive || person.isDeceased) {
        info += "\nDeceased";
    } else if (person.isPregnant && !person.isMale) {
        info += "\nPregnant";
    }

    sf::Text infoText;

    infoText.setFont(font_);
    infoText.setString(info);
    infoText.setCharacterSize(12);
    infoText.setFillColor(sf::Color::Black);

    const auto textBounds = infoText.getLocalBounds();

    const float textX = position.x - textBounds.width / 2.0f;
    const float textY = position.y + nodeRadius_ + 5.0f;

    sf::RectangleShape textBackground(
        sf::Vector2f(
            textBounds.width + 10.0f,
            textBounds.height + 5.0f
        )
    );

    textBackground.setFillColor(sf::Color(255, 255, 255, 230));
    textBackground.setPosition(textX - 5.0f, textY - 2.0f);

    window.draw(textBackground);

    infoText.setPosition(textX, textY);

    window.draw(infoText);
}

void Scene::drawSelection(
    const sf::Vector2f& position,
    sf::RenderWindow& window
) const {
    const float radius = nodeRadius_ * 1.2f;

    sf::CircleShape highlight(radius);

    highlight.setPosition(
        position.x - radius,
        position.y - radius
    );

    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineThickness(3.0f);
    highlight.setOutlineColor(sf::Color::Yellow);

    window.draw(highlight);
}

void Scene::drawHover(
    const sf::Vector2f& position,
    sf::RenderWindow& window
) const {
    const float radius = nodeRadius_ * 1.1f;

    sf::CircleShape hoverEffect(radius);

    hoverEffect.setPosition(
        position.x - radius,
        position.y - radius
    );

    hoverEffect.setFillColor(sf::Color(255, 255, 0, 50));

    window.draw(hoverEffect);
}

int Scene::getPersonAtPosition(
    const sf::Vector2f& worldPos
) const {
    for (const auto& [id, position] : personPositions_) {
        const float dx = worldPos.x - position.x;
        const float dy = worldPos.y - position.y;

        const float hitRadius = nodeRadius_ * 2.0f;

        if (dx * dx + dy * dy <= hitRadius * hitRadius) {
            return static_cast<int>(id);
        }
    }

    return -1;
}

std::optional<sf::Vector2f> Scene::getPersonPosition(
    int personId
) const {
    if (personId < 0) {
        return std::nullopt;
    }

    const auto it = personPositions_.find(
        static_cast<presentation::PersonId>(personId)
    );

    if (it == personPositions_.end()) {
        return std::nullopt;
    }

    return it->second;
}

bool Scene::loadFont(const std::string& fontPath) {
    if (font_.loadFromFile(fontPath)) {
        fontLoaded_ = true;
        return true;
    }

    std::cerr
        << "Failed to load font from: "
        << fontPath
        << std::endl;

    fontLoaded_ = false;
    return false;
}

} // namespace ftree::visualization