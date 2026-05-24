#include "Engine/Game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "--- DAVE THE MINER : VERSION FINALE STABLE ---" << std::endl;

    Game game;
    if (game.init()) {
        game.run();
    }
    else {
        std::cerr << "[ERREUR] L'initialisation du jeu a echoue." << std::endl;
        return 1;
    }

    return 0;
}