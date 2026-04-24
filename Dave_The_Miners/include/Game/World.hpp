#ifndef WORLD_HPP
#define WORLD_HPP

#include <SFML/Graphics.hpp>
#include <LDtkLoader/Project.hpp>
#include <vector>
#include <string>
#include <filesystem>

struct LayerData {
    std::string name;
    sf::VertexArray vertices;
};

class World {
public:
    World();
    bool load(const std::filesystem::path& assetDir);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPlayerSpawn() const { return m_playerSpawn; }
    sf::Vector2i getLevelSize() const { return m_levelSize; }
    const sf::Texture& getTilesetTexture() const { return m_tilesetTexture; }

    // Collision checks
    int getTileValue(sf::Vector2f pos) const;
    bool isSolid(sf::Vector2f pos) const;
    bool isDeath(sf::Vector2f pos) const;
    bool isOneWay(sf::Vector2f pos) const;
    int getGridSize() const { return m_gridSize; }
    const std::vector<sf::Vector2f>& getCheckpoints() const { return m_checkpoints; }
private:
    ldtk::Project m_project;
    sf::Texture m_tilesetTexture;
    std::vector<LayerData> m_layers;
    sf::Vector2f m_playerSpawn;
    sf::Vector2i m_levelSize;
    int m_gridSize;
    std::vector<int> m_collisionGrid; // Stores IntGrid values
    std::vector<sf::Vector2f> m_checkpoints;

    bool loadTexture(const std::filesystem::path& path);
};

#endif // WORLD_HPP
