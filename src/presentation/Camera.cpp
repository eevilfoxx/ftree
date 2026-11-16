#include "presentation/Camera.hpp"

namespace ftree::presentation {

Camera::Camera(sf::RenderWindow& window) : window_(window) {
    view_ = window.getDefaultView();
    center_ = view_.getCenter();
}

void Camera::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPanning_ = true;
            lastMousePos_ = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPanning_ = false;
        }
    } else if (event.type == sf::Event::MouseMoved) {
        if (isPanning_) {
            sf::Vector2i delta = sf::Vector2i(event.mouseMove.x, event.mouseMove.y) - lastMousePos_;
            // Convert pixel delta to world delta
            sf::Vector2f worldDelta = window_.mapPixelToCoords(delta, view_) - 
                                      window_.mapPixelToCoords(sf::Vector2i(0, 0), view_);
            center_ -= worldDelta;
            lastMousePos_ = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
        }
    } else if (event.type == sf::Event::MouseWheelScrolled) {
        float scroll = event.mouseWheel.scroll.delta;
        if (scroll > 0) {
            zoomLevel_ *= 1.1f;
        } else {
            zoomLevel_ /= 1.1f;
        }
        zoomLevel_ = std::clamp(zoomLevel_, minZoom_, maxZoom_);
    } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
            reset();
        }
    }
}

void Camera::update() {
    view_.setCenter(center_);
    view_.setZoom(zoomLevel_);
}

void Camera::reset() {
    center_ = sf::Vector2f(0.f, 0.f);
    zoomLevel_ = 1.0f;
}

sf::Vector2f Camera::getWorldPos(sf::Vector2i pixelPos) const {
    return window_.mapPixelToCoords(pixelPos, view_);
}
