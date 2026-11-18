#pragma once

#include <SFML/Graphics.hpp>

namespace ftree::visualization {

class Camera {
public:
    explicit Camera(sf::RenderWindow& window);

    void handleEvent(const sf::Event& event);
    void update();
    void reset();
    void zoom(float factor);

    const sf::View& getView() const noexcept { return view_; }
    float getZoomLevel() const noexcept { return zoomLevel_; }
    sf::Vector2f getWorldPos(const sf::Vector2i& pixelPos) const;

private:
    sf::RenderWindow& window_;
    sf::View view_;
    sf::Vector2f center_;
    sf::Vector2f baseSize_;
    float zoomLevel_ = 1.0f;
    float minZoom_ = 0.1f;
    float maxZoom_ = 5.0f;
    bool isPanning_ = false;
    sf::Vector2i lastMousePos_;
};

} // namespace ftree::visualization
