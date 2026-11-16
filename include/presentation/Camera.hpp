#pragma once

#include <SFML/Graphics.hpp>

namespace ftree::presentation {

// Camera handles view transformations (zoom, pan) independently of domain
class Camera {
public:
    Camera(sf::RenderWindow& window);
    
    // Handle mouse events
    void handleEvent(const sf::Event& event);
    
    // Handle keyboard events
    void handleKeyEvent(sf::Keyboard::Key key, bool pressed);
    
    // Update camera view
    void update();
    
    // Reset camera
    void reset();
    
    // Get the current view
    sf::View getView() const { return view_; }
    
    // Get zoom level
    float getZoomLevel() const { return zoomLevel_; }
    
    // Get world position from pixel
    sf::Vector2f getWorldPos(sf::Vector2i pixelPos) const;
    
    // Check if panning
    bool isPanning() const { return isPanning_; }
    
private:
    sf::RenderWindow& window_;
    sf::View view_;
    sf::Vector2f center_;
    float zoomLevel_ = 1.0f;
    float minZoom_ = 0.1f;
    float maxZoom_ = 5.0f;
    
    bool isPanning_ = false;
    sf::Vector2i lastMousePos_;
    sf::Vector2f offset_;
};
