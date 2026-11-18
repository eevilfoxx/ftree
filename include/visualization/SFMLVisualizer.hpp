#pragma once

#include "application/IVisualizer.hpp"
#include "visualization/Camera.hpp"
#include "visualization/Scene.hpp"
#include "visualization/UI.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

namespace ftree::visualization {

class SFMLVisualizer final : public application::IVisualizer {
public:
    SFMLVisualizer(unsigned int width, unsigned int height);

    void setData(
        std::vector<presentation::PersonDisplay> people,
        std::vector<presentation::FamilyDisplay> families
    ) override;

    void setState(
        const application::VisualizationState& state
    ) override;

    bool isOpen() const override;
    void processEvents() override;
    std::vector<application::VisualizationCommand> consumeCommands() override;
    void render() override;

private:
    sf::RenderWindow window_;
    Camera camera_;
    Scene scene_;
    UI ui_;

    application::VisualizationState state_;
    std::vector<presentation::PersonDisplay> people_;
    std::vector<presentation::FamilyDisplay> families_;
    std::vector<application::VisualizationCommand> commands_;

    bool showControls_ = true;

    void handleKeyboardEvents(const sf::Event& event);
};

} // namespace ftree::visualization
