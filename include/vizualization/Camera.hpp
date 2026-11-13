#pragma once
#include <SFML/Graphics.hpp>

class Camera {
public:
    Camera(sf::RenderWindow& window);
    
    void handleMouseEvents(const sf::Event& event);
    void handleKeyboardEvents(const sf::Event& event);
    void update();
    void reset();
    void zoom(float delta);
        
    const sf::View& getView() const { return view_; }
    float getZoomLevel() const { return zoomLevel_; }
    sf::Vector2f getWorldPos(const sf::Vector2i& pixelPos) const;
    
private:
    sf::RenderWindow& window_;
    sf::View view_;
    sf::Vector2f offset_;
    float zoomLevel_;
    
    bool isDragging_ = false;
    sf::Vector2i lastMousePos_;
    
    void handleMouseDrag(const sf::Vector2i& currentPos);
    void handleMouseWheel(float delta);
};