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

    std::filesystem::path assetDir = std::filesystem::path(ASSETS_PATH);
    
    if (!m_world.load(assetDir)) {
        return false;
    }

    // Setup View
    // 1. Setup Player first
    m_player.init(m_world.getPlayerSpawn());

    // 2. Setup View based on player position
    m_gameView.setSize({ 320.f, 180.f });
    m_gameView.setCenter(m_player.getPosition());

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
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
    }
}

void Game::update() {
    m_player.update(m_world);

    // Camera follow player with clamping
    sf::Vector2f playerPos = m_player.getPosition();
    sf::Vector2f viewSize = m_gameView.getSize();
    sf::Vector2i levelSize = m_world.getLevelSize();

    float centerX = std::max(viewSize.x / 2.f, std::min(playerPos.x, static_cast<float>(levelSize.x) - viewSize.x / 2.f));
    float centerY = std::max(viewSize.y / 2.f, std::min(playerPos.y, static_cast<float>(levelSize.y) - viewSize.y / 2.f));

    m_gameView.setCenter({ centerX, centerY });
}

void Game::render() {
    m_window.clear(sf::Color(30, 30, 30));

    m_window.setView(m_gameView);

    m_world.draw(m_window);
    m_player.draw(m_window);

    m_window.display();
}
