#include "visualization/SFMLVisualizer.hpp"

#include "visualization/VisualizationConfig.hpp"

#include <utility>

namespace ftree::visualization {

SFMLVisualizer::SFMLVisualizer(unsigned int width, unsigned int height)
    : window_(sf::VideoMode(width, height), "Family Tree"),
      camera_(window_),
      ui_(window_)
{
    window_.setFramerateLimit(VisualizationConfig::FRAMERATE_LIMIT);

    // Prefer the bundled font. The second path is kept as a fallback
    // for installations that use the original project assets.
    if (!scene_.loadFont("assets/font.otf")) {
        scene_.loadFont("assets/arial.ttf");
    }

    if (!ui_.loadFont("assets/font.otf")) {
        ui_.loadFont("assets/arial.ttf");
    }
}

void SFMLVisualizer::setData(
    std::vector<presentation::PersonDisplay> people,
    std::vector<presentation::FamilyDisplay> families)
{
    people_ = std::move(people);
    families_ = std::move(families);
    scene_.setData(people_, families_);
}

void SFMLVisualizer::setState(
    const application::VisualizationState& state)
{
    state_ = state;
}

bool SFMLVisualizer::isOpen() const
{
    return window_.isOpen();
}

void SFMLVisualizer::processEvents()
{
    sf::Event event{};

    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
            continue;
        }

        camera_.handleEvent(event);

        if (event.type == sf::Event::Resized) {
            ui_.handleResize(event);
        }

        if (event.type == sf::Event::KeyPressed) {
            handleKeyboardEvents(event);
        }

        if (event.type == sf::Event::MouseMoved) {
            const auto worldPosition = camera_.getWorldPos(
                {event.mouseMove.x, event.mouseMove.y}
            );
            scene_.setHoveredPerson(
                scene_.getPersonAtPosition(worldPosition)
            );
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            const auto worldPosition = camera_.getWorldPos(
                {event.mouseButton.x, event.mouseButton.y}
            );
            scene_.setSelectedPerson(
                scene_.getPersonAtPosition(worldPosition)
            );
        }
    }
}

void SFMLVisualizer::handleKeyboardEvents(const sf::Event& event)
{
    switch (event.key.code) {
    case sf::Keyboard::Escape:
        window_.close();
        break;

    case sf::Keyboard::R:
        camera_.reset();
        break;

    case sf::Keyboard::C:
        showControls_ = !showControls_;
        ui_.setShowControls(showControls_);
        break;

    case sf::Keyboard::Space:
        commands_.push_back(application::VisualizationCommand::PauseResume);
        break;

    case sf::Keyboard::Right:
        commands_.push_back(application::VisualizationCommand::NextYear);
        break;

    case sf::Keyboard::A:
        commands_.push_back(application::VisualizationCommand::ToggleAutoAdvance);
        break;

    case sf::Keyboard::Up:
        commands_.push_back(application::VisualizationCommand::IncreaseSpeed);
        break;

    case sf::Keyboard::Down:
        commands_.push_back(application::VisualizationCommand::DecreaseSpeed);
        break;

    case sf::Keyboard::Comma:
        camera_.zoom(1.0f / 1.1f);
        break;

    case sf::Keyboard::Period:
        camera_.zoom(1.1f);
        break;

    case sf::Keyboard::H:
        scene_.setSelectedPerson(scene_.getHoveredPersonId());
        break;

    default:
        break;
    }
}

std::vector<application::VisualizationCommand>
SFMLVisualizer::consumeCommands()
{
    std::vector<application::VisualizationCommand> result;
    result.swap(commands_);
    return result;
}

void SFMLVisualizer::render()
{
    window_.clear(sf::Color(245, 245, 245));

    camera_.update();
    scene_.draw(window_, camera_.getView());

    window_.setView(ui_.getView());
    ui_.drawInfoPanel(
        people_,
        families_,
        scene_.getSelectedPersonId()
    );
    ui_.drawControlsHint();
    ui_.drawSimulationStatus(state_);

    window_.display();
}

} // namespace ftree::visualization
