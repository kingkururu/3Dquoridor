#pragma once

#include <SFML/Graphics.hpp>
#include "../test-logging/log.hpp" 


class GameWindow{
public: 
    GameWindow( unsigned int screenWidth, unsigned int screenHeight, std::string gameTitle, unsigned int boardRate );
    sf::RenderWindow& getWindow() { return window; } 
    ~GameWindow() = default;

    explicit operator bool() const {
        return window.getSize().x > 0 && window.getSize().y > 0;
    }

private:
    sf::RenderWindow window;
};

class GameView{
public:
    GameView(sf::FloatRect viewRect);
    const sf::View& getView() const { return view; }
    ~GameView() = default;

    void setSceneView(const sf::View& NewView) { view = NewView; }
    explicit operator bool() const {
        return view.getSize().x > 0 && view.getSize().y > 0; // Checks if the view has a valid size
    }

private:
    sf::View view; 

};

