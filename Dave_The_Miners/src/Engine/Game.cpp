#include "Engine/Game.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>

Game::Game() : m_window(sf::VideoMode({ 1280u, 720u }), "Dave The Miner") {
    m_window.setFramerateLimit(60);
}

bool Game::init() {
    std::cout << "--- DAVE THE MINER : INITIALISATION ---" << std::endl;

    // Chemin relatif a l'executable : fonctionne sur tous les PC
    std::filesystem::path assetDir = std::filesystem::current_path() / "assets";

    if (!m_world.load(assetDir)) {
        return false;
    }

    if (!m_player.loadTexture(assetDir / "spritesheet.png")) {
        std::cerr << "Impossible de charger la spritesheet joueur" << std::endl;
        return false;
    }

    m_player.init(m_world.getPlayerSpawn());

    m_gameView.setSize({ 320.f, 180.f });
    m_gameView.setCenter(m_player.getPosition());

    m_clock.restart();
    return true;
}

void Game::run() {
    while (m_window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            m_window.close();
    }
}

void Game::update() {
    float dt = m_clock.restart().asSeconds();
    // Plafond de securite : evite un gros saut si la fenetre etait en pause
    if (dt > 0.05f) dt = 0.05f;

    m_player.update(m_world, dt);

    // Camera qui suit le joueur avec clamping aux bords du niveau
    sf::Vector2f playerPos = m_player.getPosition();
    sf::Vector2f viewSize = m_gameView.getSize();
    sf::Vector2i levelSize = m_world.getLevelSize();

    float cx = std::clamp(playerPos.x, viewSize.x / 2.f, (float)levelSize.x - viewSize.x / 2.f);
    float cy = std::clamp(playerPos.y, viewSize.y / 2.f, (float)levelSize.y - viewSize.y / 2.f);
    m_gameView.setCenter({ cx, cy });
}

void Game::render() {
    m_window.clear(sf::Color(30, 30, 30));
    m_window.setView(m_gameView);
    m_world.draw(m_window);
    m_player.draw(m_window);
    m_window.display();
}