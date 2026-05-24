#pragma once
#include <SFML/Graphics.hpp>

class World;

class Player {
public:
    Player();
    void init(sf::Vector2f spawnPos);
    bool loadTexture(const std::filesystem::path& path);
    void update(const World& world, float dt);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }

private:
    bool collidesAt(const World& world, sf::Vector2f pos) const;
    void resolveCollisions(const World& world, float dt);
    void updateAnimation(float dt);
    void setAnimation(int row, int frameCount, float fps = 10.f);

    // Physique
    sf::Vector2f m_velocity;
    sf::Vector2f m_spawnPos;
    bool  m_onGround = false;
    bool  m_isDead = false;
    float m_deadTimer = 0.f;
    int   m_lastDir = 1; // 1=droite, -1=gauche

    // Rendu
    sf::Texture m_texture;
    sf::Sprite  m_sprite;

    // Animation
    struct AnimState {
        int   row = 0;
        int   frameCount = 1;
        int   curFrame = 0;
        float fps = 10.f;
        float timer = 0.f;
    } m_anim;

    // Taille d'une frame sur la spritesheet (à ajuster selon votre sheet)
    static constexpr int FRAME_W = 64;
    static constexpr int FRAME_H = 64;

    // Rows spritesheet
    static constexpr int ROW_DEAD = 20;
    static constexpr int ROW_JUMP_LEFT = 27;
    static constexpr int ROW_JUMP_RIGHT = 29;
    static constexpr int ROW_RUN_LEFT = 39;
    static constexpr int ROW_RUN_RIGHT = 41;
    static constexpr int ROW_IDLE_FACE = 24;
    static constexpr int ROW_IDLE_LEFT = 23;
    static constexpr int ROW_IDLE_RIGHT = 25;

    // Physique
    static constexpr float GRAVITY = 800.f;
    static constexpr float MOVE_SPEED = 80.f;
    static constexpr float JUMP_FORCE = -280.f;
    static constexpr float MAX_FALL = 400.f;
    static constexpr float RESPAWN_TIME = 1.f;
};