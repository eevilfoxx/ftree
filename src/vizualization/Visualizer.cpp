#include "vizualization/Visualizer.hpp"

Visualizer::Visualizer(std::shared_ptr<FamilyManager> familyManager,
                      std::shared_ptr<PopulationManager> populationManager,
                      std::shared_ptr<IPersonRepository> personRepository,
                      unsigned int width, unsigned int height)
    : window_(sf::VideoMode(width, height), "Family Tree", sf::Style::Default)
    , camera_(window_)
    , scene_(familyManager, populationManager, personRepository)
    , ui_(window_), familyManager_(familyManager), populationManager_(populationManager), personRepository_(personRepository)  {
    
    // Загрузка шрифта для UI
    if (!ui_.loadFont("../assets/font.otf")) {
        std::cerr << "Warning: Using default font\n";
    }
    
    if (!scene_.loadFont("../assets/font.otf")) {
        std::cerr << "Warning: Using default font\n";
    }
    window_.setFramerateLimit(60);

}

void Visualizer::update() {
    scene_.update();
}

void Visualizer::render() {
    window_.clear(sf::Color(240, 240, 240));
    
    // Обновляем камеру
    camera_.update();
    
    // Рендеринг сцены с видом камеры
    window_.setView(camera_.getView());
    scene_.draw(window_, camera_.getView());
    
    // Рендеринг UI с фиксированным видом
    window_.setView(ui_.getView());
    ui_.drawInfoPanel(populationManager_, familyManager_, scene_.getSelectedPersonId(), personRepository_);
    ui_.drawSimulationStatus(simulationState_);
    
    if (showControls_) {
        ui_.drawControlsHint();
    }
    
    window_.display();
}

void Visualizer::handleEvents() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        camera_.handleMouseEvents(event);
        camera_.handleKeyboardEvents(event);
        
        switch (event.type) {
            case sf::Event::Closed:
                window_.close();
                break;
                
            case sf::Event::Resized:
                ui_.handleResize(event);
                break;
                
            case sf::Event::KeyPressed:
                handleKeyboardEvents(event);
                break;
                
            case sf::Event::MouseMoved: {
                sf::Vector2f worldPos = camera_.getWorldPos(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                int hoveredId = scene_.getPersonAtPosition(worldPos);
                scene_.setHoveredPerson(hoveredId);
                break;
            }
                
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f worldPos = camera_.getWorldPos(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    int clickedId = scene_.getPersonAtPosition(worldPos);
                    scene_.setSelectedPerson(clickedId);
                }
                break;
                
            default:
                break;
        }
    }
}

void Visualizer::handleKeyboardEvents(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::Escape:
            window_.close();
            break;
        case sf::Keyboard::R:
            camera_.reset();
            break;
        case sf::Keyboard::C:
            showControls_ = !showControls_;
            break;
        case sf::Keyboard::H:
            if (scene_.getHoveredPersonId() != -1) {
                scene_.setSelectedPerson(scene_.getHoveredPersonId());
            }
            break;
        case sf::Keyboard::Comma:
            camera_.zoom(1);
            break;
        case sf::Keyboard::Period:
            camera_.zoom(0);
            break;
        default:
            break;
    }
}

void Visualizer::setSimulationState(const SimulationState& state) {
    simulationState_ = state;
}