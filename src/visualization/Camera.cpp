#include "visualization/Camera.hpp"

#include <algorithm>

namespace ftree::visualization {

Camera::Camera(sf::RenderWindow& window)
    : window_(window),
      view_(window.getDefaultView()),
      center_(view_.getCenter()),
      baseSize_(view_.getSize())
{
}

void Camera::handleEvent(const sf::Event& event)
{
    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPanning_ = true;
            lastMousePos_ = {event.mouseButton.x, event.mouseButton.y};
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPanning_ = false;
        }
        break;

    case sf::Event::MouseMoved:
        if (isPanning_) {
            const sf::Vector2i currentPos(event.mouseMove.x, event.mouseMove.y);
            const sf::Vector2f oldWorld = window_.mapPixelToCoords(lastMousePos_, view_);
            const sf::Vector2f newWorld = window_.mapPixelToCoords(currentPos, view_);
            center_ -= newWorld - oldWorld;
            lastMousePos_ = currentPos;
        }
        break;

    case sf::Event::MouseWheelScrolled:
        if (event.mouseWheelScroll.delta > 0.0f) {
            zoom(1.1f);
        } else if (event.mouseWheelScroll.delta < 0.0f) {
            zoom(1.0f / 1.1f);
        }
        break;

    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::R) {
            reset();
        }
        break;

    case sf::Event::Resized:
        baseSize_ = {
            static_cast<float>(event.size.width),
            static_cast<float>(event.size.height)
        };
        break;

    default:
        break;
    }
}

void Camera::update()
{
    view_.setSize(baseSize_.x / zoomLevel_, baseSize_.y / zoomLevel_);
    view_.setCenter(center_);
    window_.setView(view_);
}

void Camera::reset()
{
    center_ = {0.0f, 0.0f};
    zoomLevel_ = 1.0f;
}

void Camera::zoom(float factor)
{
    if (factor <= 0.0f) {
        return;
    }

    zoomLevel_ = std::clamp(zoomLevel_ * factor, minZoom_, maxZoom_);
}

sf::Vector2f Camera::getWorldPos(const sf::Vector2i& pixelPos) const
{
    return window_.mapPixelToCoords(pixelPos, view_);
}

} // namespace ftree::visualization
