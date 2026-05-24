#include "Game/player.hpp"
#include "Game/World.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <algorithm>

// Hitbox (peut différer de la frame visuelle)
static constexpr float HW = 3.5f;
static constexpr float HH = 3.5f;

// ---------------------------------------------------------------
Player::Player() : m_sprite(m_texture) {}

bool Player::loadTexture(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    std::vector<char> buf((std::istreambuf_iterator<char>(file)), {});
    if (!m_texture.loadFromMemory(buf.data(), buf.size())) return false;
    m_texture.setSmooth(false);

    // Origine centrée horizontalement, collée en bas verticalement
    // → le sprite s'aligne sur la hitbox naturellement
    m_sprite.setOrigin({ FRAME_W / 2.f, FRAME_H * 0.75}); // centré sur la hitbox
    m_sprite.setScale({ 0.25f, 0.25f });

    return true;
}

void Player::init(sf::Vector2f spawnPos) {
    m_spawnPos = spawnPos;
    m_velocity = { 0.f, 0.f };
    m_onGround = false;
    m_isDead = false;
    m_deadTimer = 0.f;
    m_lastDir = 1;
    m_sprite.setPosition(spawnPos);
    setAnimation(ROW_IDLE_FACE, 2);
}

// ---------------------------------------------------------------
void Player::setAnimation(int row, int frameCount, float fps) {
    // Évite de redémarrer la même animation
    if (m_anim.row == row && m_anim.frameCount == frameCount) return;

    m_anim.row = row;
    m_anim.frameCount = frameCount;
    m_anim.fps = fps;
    m_anim.curFrame = 0;
    m_anim.timer = 0.f;
}

void Player::updateAnimation(float dt) {
    m_anim.timer += dt;
    if (m_anim.timer >= 1.f / m_anim.fps) {
        m_anim.timer = 0.f;
        m_anim.curFrame = (m_anim.curFrame + 1) % m_anim.frameCount;
    }
    m_sprite.setTextureRect(sf::IntRect(
        { m_anim.curFrame * FRAME_W, m_anim.row * FRAME_H },
        { FRAME_W, FRAME_H }
    ));
}

// ---------------------------------------------------------------
bool Player::collidesAt(const World& world, sf::Vector2f p) const {
    return world.isSolid({ p.x - HW, p.y - HH })
        || world.isSolid({ p.x + HW, p.y - HH })
        || world.isSolid({ p.x - HW, p.y + HH })
        || world.isSolid({ p.x + HW, p.y + HH })
        || world.isSolid({ p.x,      p.y - HH })
        || world.isSolid({ p.x,      p.y + HH })
        || world.isSolid({ p.x - HW, p.y })
        || world.isSolid({ p.x + HW, p.y });
}

void Player::resolveCollisions(const World& world, float dt) {
    const int gs = world.getGridSize();
    sf::Vector2f pos = m_sprite.getPosition();

    // ===== AXE X =====
    pos.x += m_velocity.x * dt;
    if (m_velocity.x > 0.f) {
        if (world.isSolid({ pos.x + HW, pos.y - HH + 1.f })
            || world.isSolid({ pos.x + HW, pos.y + HH - 1.f })) {
            pos.x = static_cast<float>((int)(pos.x + HW) / gs * gs) - HW - 0.01f;
            m_velocity.x = 0.f;
        }
    }
    else if (m_velocity.x < 0.f) {
        if (world.isSolid({ pos.x - HW, pos.y - HH + 1.f })
            || world.isSolid({ pos.x - HW, pos.y + HH - 1.f })) {
            pos.x = static_cast<float>(((int)(pos.x - HW) / gs + 1) * gs) + HW + 0.01f;
            m_velocity.x = 0.f;
        }
    }

    // ===== AXE Y =====
    pos.y += m_velocity.y * dt;
    m_onGround = false;

    if (m_velocity.y > 0.f) {
        bool hitSolid = world.isSolid({ pos.x - HW + 1.f, pos.y + HH })
            || world.isSolid({ pos.x + HW - 1.f, pos.y + HH })
            || world.isSolid({ pos.x,             pos.y + HH });

        float prevBottom = m_sprite.getPosition().y + HH;
        float tileTop = std::floor((pos.y + HH) / gs) * gs;
        bool hitOneWay = (prevBottom <= tileTop)
            && (world.isOneWay({ pos.x - HW + 1.f, pos.y + HH })
                || world.isOneWay({ pos.x + HW - 1.f, pos.y + HH }));

        if (hitSolid || hitOneWay) {
            pos.y = static_cast<float>((int)(pos.y + HH) / gs * gs) - HH - 0.01f;
            m_velocity.y = 0.f;
            m_onGround = true;
        }
    }
    else if (m_velocity.y < 0.f) {
        if (world.isSolid({ pos.x - HW + 1.f, pos.y - HH })
            || world.isSolid({ pos.x + HW - 1.f, pos.y - HH })
            || world.isSolid({ pos.x,             pos.y - HH })) {
            pos.y = static_cast<float>(((int)(pos.y - HH) / gs + 1) * gs) + HH + 0.01f;
            m_velocity.y = 0.f;
        }
    }

    m_sprite.setPosition(pos);
}

// ---------------------------------------------------------------
void Player::update(const World& world, float dt) {

    // --- Mort / respawn ---
    if (m_isDead) {
        setAnimation(ROW_DEAD, 6, 8.f);
        updateAnimation(dt);
        m_deadTimer -= dt;
        if (m_deadTimer <= 0.f) init(m_spawnPos);
        return;
    }

    // --- Inputs ---
    bool goLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
    bool goRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    bool jump = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);

    m_velocity.x = 0.f;
    if (goLeft) { m_velocity.x = -MOVE_SPEED; m_lastDir = -1; }
    if (goRight) { m_velocity.x = +MOVE_SPEED; m_lastDir = 1; }

    if (m_onGround && jump) {
        m_velocity.y = JUMP_FORCE;
        m_onGround = false;
    }

    // --- Gravité ---
    m_velocity.y = std::min(m_velocity.y + GRAVITY * dt, MAX_FALL);

    // --- Physique ---
    resolveCollisions(world, dt);

    // --- Choix de l'animation ---
    if (!m_onGround) {
        // En l'air
        if (m_lastDir < 0) setAnimation(ROW_JUMP_LEFT, 5, 10.f);
        else                setAnimation(ROW_JUMP_RIGHT, 5, 10.f);
    }
    else if (goLeft) {
        setAnimation(ROW_RUN_LEFT, 8, 12.f);
    }
    else if (goRight) {
        setAnimation(ROW_RUN_RIGHT, 8, 12.f);
    }
    else {
        // Idle : conserve la direction du dernier mouvement
        if (m_lastDir < 0) setAnimation(ROW_IDLE_LEFT, 2, 4.f);
        else if (m_lastDir > 0) setAnimation(ROW_IDLE_RIGHT, 2, 4.f);
        else                setAnimation(ROW_IDLE_FACE, 2, 4.f);
    }

    updateAnimation(dt);

    // --- Mort ---
    if (world.isDeath(m_sprite.getPosition())) {
        m_isDead = true;
        m_deadTimer = RESPAWN_TIME;
        return;
    }

    // --- Checkpoints ---
    for (const auto& cp : world.getCheckpoints()) {
        sf::Vector2f d = m_sprite.getPosition() - cp;
        if (std::abs(d.x) < 8.f && std::abs(d.y) < 8.f)
            m_spawnPos = cp;
    }
}

void Player::draw(sf::RenderWindow& window) {
    sf::Vector2f pos = m_sprite.getPosition(); // position physique (hitbox)

    m_sprite.setPosition(pos); // déjà là, mais le sprite suit pile la hitbox
    window.draw(m_sprite);
}