#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

class World;

class Player {
public:
    Player();
    void init(sf::Vector2f spawnPos);
    void update(const World& world);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const { return m_shape.getPosition(); }

private:
    sf::RectangleShape m_shape;
    sf::Vector2f m_velocity;
    sf::Vector2f m_spawnPos;
    bool m_onGround;

    const float m_speed = 100.f;
    const float m_jumpForce = -200.f;
    const float m_gravity = 700.f;
};

#endif // PLAYER_HPP
