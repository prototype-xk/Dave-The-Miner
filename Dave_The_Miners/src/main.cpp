#include <SFML/Graphics.hpp>
#include <LDtkLoader/Project.hpp>
#include <iostream>
#include <optional>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

int main() {
    ldtk::Project ldtk_project;

    // 1. Définition des chemins
    fs::path exe_path = fs::current_path();
    fs::path ldtk_path = exe_path / "assets" / "level.ldtk";
    fs::path texture_path = exe_path / "assets" / "tileset.png";

    // --- CHARGEMENT DU FICHIER LDTK ---
    try {
        ldtk_project.loadFromFile(ldtk_path.string());
        std::cout << "1. LDtk : Fichier charge avec succes !" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Erreur LDtk : " << ex.what() << std::endl;
        return 1;
    }

    // --- EXTRACTION DES DONNEES ---
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel("Level");
    const auto& layer = level.getLayer("Ground");
    const auto& tiles_vector = layer.allTiles();

    // --- CHARGEMENT DE LA TEXTURE (MÉTHODE MÉMOIRE FORCEE) ---
    sf::Texture tileset_texture;

    std::ifstream file(texture_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier : " << texture_path.string() << std::endl;
        return 1;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);

    if (file.read(buffer.data(), size)) {
        if (!tileset_texture.loadFromMemory(buffer.data(), size)) {
            std::cerr << "Erreur SFML : Le fichier est lu mais le format image est invalide." << std::endl;
            return 1;
        }
    }
    std::cout << "2. SFML : Texture chargee de la memoire avec succes !" << std::endl;

    // --- PREPARATION DU RENDU (TRIANGLES) ---
    sf::VertexArray tilemap(sf::PrimitiveType::Triangles);
    tilemap.resize(tiles_vector.size() * 6);

    int i = 0;
    for (const auto& tile : tiles_vector) {
        auto vertices = tile.getVertices();

        // Positions des 2 triangles formant une tuile
        tilemap[i * 6 + 0].position = sf::Vector2f(vertices[0].pos.x, vertices[0].pos.y);
        tilemap[i * 6 + 1].position = sf::Vector2f(vertices[1].pos.x, vertices[1].pos.y);
        tilemap[i * 6 + 2].position = sf::Vector2f(vertices[2].pos.x, vertices[2].pos.y);

        tilemap[i * 6 + 3].position = sf::Vector2f(vertices[2].pos.x, vertices[2].pos.y);
        tilemap[i * 6 + 4].position = sf::Vector2f(vertices[3].pos.x, vertices[3].pos.y);
        tilemap[i * 6 + 5].position = sf::Vector2f(vertices[0].pos.x, vertices[0].pos.y);

        // Coordonnées de texture
        tilemap[i * 6 + 0].texCoords = sf::Vector2f((float)vertices[0].tex.x, (float)vertices[0].tex.y);
        tilemap[i * 6 + 1].texCoords = sf::Vector2f((float)vertices[1].tex.x, (float)vertices[1].tex.y);
        tilemap[i * 6 + 2].texCoords = sf::Vector2f((float)vertices[2].tex.x, (float)vertices[2].tex.y);

        tilemap[i * 6 + 3].texCoords = sf::Vector2f((float)vertices[2].tex.x, (float)vertices[2].tex.y);
        tilemap[i * 6 + 4].texCoords = sf::Vector2f((float)vertices[3].tex.x, (float)vertices[3].tex.y);
        tilemap[i * 6 + 5].texCoords = sf::Vector2f((float)vertices[0].tex.x, (float)vertices[0].tex.y);

        i++;
    }

    // --- FENETRE ---
    sf::RenderWindow window(sf::VideoMode({ (unsigned int)level.size.x * 4, (unsigned int)level.size.y * 4 }), "Dave The Miners - Map OK");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color(50, 50, 50));

        sf::RenderStates states;
        states.transform.scale({ 4.f, 4.f }); // Zoom x4
        states.texture = &tileset_texture;

        window.draw(tilemap, states);
        window.display();
    }

    return 0;
}