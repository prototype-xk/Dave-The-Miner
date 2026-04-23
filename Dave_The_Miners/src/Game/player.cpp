#include "Game/player.hpp"
#include "Game/World.hpp"

Player::Player() : m_velocity(0.f, 0.f), m_onGround(false) {
    m_shape.setSize({ 8.f, 8.f });
    m_shape.setFillColor(sf::Color::Green);
    m_shape.setOrigin({ 4.f, 4.f });
}

void Player::init(sf::Vector2f spawnPos) {
    // On place le joueur a la position trouvee dans LDtk (Player_Spawn)
    m_shape.setPosition(spawnPos);
}

void Player::update(const World& world) {
    /*
      C'est ici que vous ecrirez votre logique de jeu.
      
      COMMENT UTILISER LE MONDE (WORLD) :
      ----------------------------------
      1. Pour savoir si le joueur touche un mur (IntGrid valeur 1) :
         if (world.isSolid(m_shape.getPosition())) { ... }
         
      2. Pour savoir si le joueur est sur une zone de mort (IntGrid valeur 2) :
         if (world.isDeath(m_shape.getPosition())) { ... }
         
      3. Pour recuperer tous les checkpoints du niveau :
         const auto& checkpoints = world.getCheckpoints();
    */
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}