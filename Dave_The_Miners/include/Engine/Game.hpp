#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "Game/World.hpp"
#include "Game/player.hpp"

class Game {
public:
    Game();
    bool init();
    void run();

private:
    void processEvents();
    void update();
    void render();

    sf::RenderWindow m_window;
    sf::View m_gameView;
    World m_world;
    Player m_player;
    sf::Clock m_clock;
};

#endif // GAME_HPP
