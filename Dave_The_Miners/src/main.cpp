#include <SFML/Graphics.hpp>
#include <LDtkLoader/Project.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;

struct LayerData {
    std::string name;
    sf::VertexArray vertices;
};

int main(int argc, char* argv[]) {
    std::cout << "--- DAVE THE MINER : VERSION FINALE STABLE ---" << std::endl;

    // 1. CONFIGURATION DES CHEMINS
    fs::path asset_dir = fs::path(ASSETS_PATH);
    fs::path ldtk_path = (asset_dir / "Dave.ldtk").make_preferred();
    fs::path texture_path = (asset_dir / "tileset.png").make_preferred();

    // 2. CHARGEMENT LDTK
    ldtk::Project ldtk_project;
    try {
        if (!fs::exists(ldtk_path)) throw std::runtime_error("Fichier .ldtk introuvable.");
        ldtk_project.loadFromFile(ldtk_path.string());
        std::cout << "[OK] Projet LDtk charge." << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "[ERREUR FATALE] " << ex.what() << std::endl;
        return 1;
    }

    // 3. CHARGEMENT TEXTURE (METHODE BINAIRE ANTI-CRASH)
    sf::Texture tileset_texture;
    std::ifstream file(texture_path, std::ios::binary);
    if (file) {
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (!tileset_texture.loadFromMemory(buffer.data(), buffer.size())) {
            std::cerr << "[ERREUR] SFML n'a pas pu decoder l'image." << std::endl;
            return 1;
        }
        tileset_texture.setSmooth(false); // Garde le cote Pixel Art net
        std::cout << "[OK] Texture chargee via buffer memoire." << std::endl;
    }
    else {
        std::cerr << "[ERREUR] Impossible d'ouvrir tileset.png" << std::endl;
        return 1;
    }

    // 4. SELECTION DU NIVEAU
    const auto& world = ldtk_project.getWorld();
    const ldtk::Level* level_ptr = nullptr;
    try {
        level_ptr = &world.getLevel("Level");
    }
    catch (...) {
        std::cout << "[WARN] 'Level' non trouve. Niveaux dispos : ";
        for (auto& l : world.allLevels()) std::cout << l.name << " ";
        std::cout << "\nUtilisation du premier niveau par defaut." << std::endl;
        level_ptr = &world.allLevels()[0];
    }
    const auto& level = *level_ptr;

    // 5. CONSTRUCTION DES CALQUES ET SPAWN
    std::vector<LayerData> layers_to_render;
    sf::Vector2f player_spawn(0.f, 0.f);

    for (const auto& layer : level.allLayers()) {
        if (layer.getType() == ldtk::LayerType::Entities) {
            for (const auto& entity : layer.allEntities()) {
                if (entity.getName() == "Player_Spawn") {
                    player_spawn = { (float)entity.getPosition().x, (float)entity.getPosition().y };
                }
            }
        }

        if (layer.getType() == ldtk::LayerType::Tiles || layer.getType() == ldtk::LayerType::AutoLayer) {
            sf::VertexArray va(sf::PrimitiveType::Triangles);
            const auto& tiles = layer.allTiles();
            va.resize(tiles.size() * 6);

            int i = 0;
            for (const auto& tile : tiles) {
                auto v = tile.getVertices();
                int idx = i * 6;
                va[idx + 0] = { {v[0].pos.x, v[0].pos.y}, sf::Color::White, {(float)v[0].tex.x, (float)v[0].tex.y} };
                va[idx + 1] = { {v[1].pos.x, v[1].pos.y}, sf::Color::White, {(float)v[1].tex.x, (float)v[1].tex.y} };
                va[idx + 2] = { {v[2].pos.x, v[2].pos.y}, sf::Color::White, {(float)v[2].tex.x, (float)v[2].tex.y} };
                va[idx + 3] = { {v[2].pos.x, v[2].pos.y}, sf::Color::White, {(float)v[2].tex.x, (float)v[2].tex.y} };
                va[idx + 4] = { {v[3].pos.x, v[3].pos.y}, sf::Color::White, {(float)v[3].tex.x, (float)v[3].tex.y} };
                va[idx + 5] = { {v[0].pos.x, v[0].pos.y}, sf::Color::White, {(float)v[0].tex.x, (float)v[0].tex.y} };
                i++;
            }
            layers_to_render.push_back({ layer.getName(), va });
        }
    }

    // 6. FENETRE ET VUE (POUR LA TAILLE)
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Dave The Miner");
    window.setFramerateLimit(60);

    // La vue fait la taille du niveau pour un zoom parfait sans deformer
    sf::View gameView;
    gameView.setSize({ (float)level.size.x, (float)level.size.y });
    gameView.setCenter({ (float)level.size.x / 2.f, (float)level.size.y / 2.f });

    // Dave (le rectangle rouge)
    sf::RectangleShape dave(sf::Vector2f(12.f, 12.f)); // Taille un peu plus petite que la tuile
    dave.setFillColor(sf::Color::Red);
    dave.setOrigin({ 6.f, 6.f });
    dave.setPosition(player_spawn);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        window.clear(sf::Color(30, 30, 30));

        // On applique la vue avant de dessiner
        window.setView(gameView);

        sf::RenderStates states;
        states.texture = &tileset_texture;

        for (const auto& l : layers_to_render) {
            if (l.name != "Colide") window.draw(l.vertices, states);
        }

        window.draw(dave);

        window.display();
    }

    return 0;
}