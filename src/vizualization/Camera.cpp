#include "vizualization/Camera.hpp"

Camera::Camera(sf::RenderWindow& window) 
    : window_(window), view_(window.getDefaultView()), zoomLevel_(1.0f) {}

void Camera::handleMouseEvents(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left) {
                isDragging_ = true;
                lastMousePos_ = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }
            break;
            
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left) {
                isDragging_ = false;
            }
            break;
            
        case sf::Event::MouseMoved:
            if (isDragging_) {
                handleMouseDrag(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            }
            break;
            
        case sf::Event::MouseWheelScrolled:
            handleMouseWheel(event.mouseWheelScroll.delta);
            break;
            
        case sf::Event::Resized:
            view_.setSize(event.size.width, event.size.height);
            break;
    }
}

void Camera::handleKeyboardEvents(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::N:
        break;

        case sf::Keyboard::M:
        break;

    }
}

void Camera::handleMouseDrag(const sf::Vector2i& currentPos) {
    sf::Vector2i delta = currentPos - lastMousePos_;
    sf::Vector2f worldDelta = window_.mapPixelToCoords(delta, view_) - 
                             window_.mapPixelToCoords(sf::Vector2i(0, 0), view_);
    
    offset_ -= worldDelta;
    lastMousePos_ = currentPos;
}

void Camera::handleMouseWheel(float delta) {
    if (delta > 0) {
        zoomLevel_ *= 1.1f;
    } else {
        zoomLevel_ /= 1.1f;
    }
    zoomLevel_ = std::clamp(zoomLevel_, 0.1f, 5.0f);
}

void Camera::zoom(float delta) {
    if (delta > 0) {
        zoomLevel_ *= 1.1f;
    } else {
        zoomLevel_ /= 1.1f;
    }
    zoomLevel_ = std::clamp(zoomLevel_, 0.1f, 5.0f);
}

void Camera::update() {
    view_ = window_.getDefaultView();
    view_.setCenter(offset_);
    view_.zoom(zoomLevel_);
}

void Camera::reset() {
    offset_ = sf::Vector2f(0, 0);
    zoomLevel_ = 1.0f;
}

sf::Vector2f Camera::getWorldPos(const sf::Vector2i& pixelPos) const {
    return window_.mapPixelToCoords(pixelPos, view_);
}