#include "Game/World.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

World::World() : m_playerSpawn(0.f, 0.f), m_levelSize(0, 0), m_gridSize(8) {}

bool World::load(const std::filesystem::path& assetDir) {
    // Chemins vers les fichiers exportes de LDtk
    std::filesystem::path ldtkPath = (assetDir / "Dave.ldtk").make_preferred();
    std::filesystem::path texturePath = (assetDir / "tileset.png").make_preferred();

    // 1. CHARGEMENT DU FICHIER PROJET (.ldtk)
    try {
        if (!std::filesystem::exists(ldtkPath)) throw std::runtime_error("Fichier .ldtk introuvable.");
        m_project.loadFromFile(ldtkPath.string());
        std::cout << "[LDtk] Projet charge avec succes." << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "[LDtk ERREUR] " << ex.what() << std::endl;
        return false;
    }

    if (!loadTexture(texturePath)) return false;

    // 2. SELECTION DU NIVEAU
    // LDtk peut avoir plusieurs niveaux. Ici on force le chargement de "Level_0".
    const auto& world = m_project.getWorld();
    const ldtk::Level& level = world.getLevel("Level_0");
    m_levelSize = { level.size.x, level.size.y };

    m_layers.clear();
    m_checkpoints.clear();

    // 3. PARCOURS DES COUCHES (LAYERS)
    // Chaque couche dans LDtk a un type (Entities, Tiles, IntGrid, etc.)
    for (const auto& layer : level.allLayers()) {
        
        // --- TYPE : ENTITIES (Entites) ---
        // Utilise pour les objets uniques : Joueur, Ennemis, Checkpoints, Portes.
        if (layer.getType() == ldtk::LayerType::Entities) {
            for (const auto& entity : layer.allEntities()) {
                if (entity.getName() == "Player_Spawn") {
                    // On recupere les coordonnees pixel (px) de l'entite
                    m_playerSpawn = { (float)entity.getPosition().x, (float)entity.getPosition().y };
                }
                else if (entity.getName() == "Checkpoint") {
                    m_checkpoints.push_back({ (float)entity.getPosition().x, (float)entity.getPosition().y });
                }
            }
        }

        // --- TYPE : INTGRID (Grille d'entiers) ---
        // Idéal pour les collisions. Chaque case a une valeur (ex: 1=Sol, 2=Piques).
        // On stocke ces valeurs dans un tableau (m_collisionGrid) pour les tester plus tard.
        if (layer.getType() == ldtk::LayerType::IntGrid && layer.getName() == "Colide") {
            m_gridSize = layer.getCellSize(); // Taille d'une case (ex: 8 pixels)
            auto gridSize = layer.getGridSize(); // Nombre de cases (ex: 128x128)
            m_collisionGrid.assign(gridSize.x * gridSize.y, 0);
            
            for (int y = 0; y < gridSize.y; ++y) {
                for (int x = 0; x < gridSize.x; ++x) {
                    const auto& val = layer.getIntGridVal(x, y);
                    if (val.value != ldtk::IntGridValue::None.value) {
                        m_collisionGrid[x + y * gridSize.x] = val.value;
                    }
                }
            }
        }

        // --- TYPE : TILES (Tuiles graphiques) ---
        // C'est ce qui est affiche a l'ecran. On cree des VertexArray SFML pour
        // dessiner toutes les tuiles d'un coup de maniere performante.
        if (layer.getType() == ldtk::LayerType::Tiles || layer.getType() == ldtk::LayerType::AutoLayer) {
            sf::VertexArray va(sf::PrimitiveType::Triangles);
            const auto& tiles = layer.allTiles();
            va.resize(tiles.size() * 6); // 2 triangles par tuile = 6 sommets

            int i = 0;
            for (const auto& tile : tiles) {
                auto v = tile.getVertices(); // Positions et coordonnees de texture de la tuile
                int idx = i * 6;
                va[idx + 0] = { {v[0].pos.x, v[0].pos.y}, sf::Color::White, {(float)v[0].tex.x, (float)v[0].tex.y} };
                va[idx + 1] = { {v[1].pos.x, v[1].pos.y}, sf::Color::White, {(float)v[1].tex.x, (float)v[1].tex.y} };
                va[idx + 2] = { {v[2].pos.x, v[2].pos.y}, sf::Color::White, {(float)v[2].tex.x, (float)v[2].tex.y} };
                va[idx + 3] = { {v[2].pos.x, v[2].pos.y}, sf::Color::White, {(float)v[2].tex.x, (float)v[2].tex.y} };
                va[idx + 4] = { {v[3].pos.x, v[3].pos.y}, sf::Color::White, {(float)v[3].tex.x, (float)v[3].tex.y} };
                va[idx + 5] = { {v[0].pos.x, v[0].pos.y}, sf::Color::White, {(float)v[0].tex.x, (float)v[0].tex.y} };
                i++;
            }
            m_layers.push_back({ layer.getName(), va });
        }
    }

    return true;
}

bool World::loadTexture(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (file) {
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (!m_tilesetTexture.loadFromMemory(buffer.data(), buffer.size())) return false;
        m_tilesetTexture.setSmooth(false); // Garde l'aspect Pixel Art (pas de flou)
        return true;
    }
    return false;
}

void World::draw(sf::RenderWindow& window) {
    sf::RenderStates states;
    states.texture = &m_tilesetTexture;
    for (const auto& l : m_layers) {
        // On dessine toutes les couches sauf "Colide" (qui est invisible, juste pour la physique)
        if (l.name != "Colide") window.draw(l.vertices, states);
    }
}

// --- FONCTIONS D'INTERROGATION DU MONDE ---

int World::getTileValue(sf::Vector2f pos) const {
    // Transforme une position pixel (ex: 45, 67) en coordonnee de grille (ex: 5, 8)
    int x = static_cast<int>(pos.x) / m_gridSize;
    int y = static_cast<int>(pos.y) / m_gridSize;
    int w = m_levelSize.x / m_gridSize;
    int h = m_levelSize.y / m_gridSize;
    
    // Verifie si on est hors des limites du niveau
    if (x < 0 || x >= w || y < 0 || y >= h) return 0;
    
    // Retourne la valeur stockee dans l'IntGrid (1, 2, 3...)
    return m_collisionGrid[x + y * w];
}

bool World::isSolid(sf::Vector2f pos) const { return getTileValue(pos) == 1; }
bool World::isDeath(sf::Vector2f pos) const { return getTileValue(pos) == 2; }
bool World::isOneWay(sf::Vector2f pos) const { return getTileValue(pos) == 3; }
