cmake_minimum_required(VERSION 3.15)
project(Dave_The_Miners)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Configuration SFML (Utilisation du package fourni dans libs)
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.1.0/lib/cmake/SFML")
set(SFML_STATIC_LIBRARIES ON) # On utilise les libs statiques (.lib sans DLL)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# 2. LDtkLoader
add_subdirectory(libs/LDtkLoader-1.5.3.1)

# 3. Définition du chemin des assets pour le code C++
add_definitions(-DASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# 4. Fichiers sources
file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 5. L'exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 6. Includes
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE libs) # Permet d'inclure <nlohmann/json.hpp>

# 7. Links
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    LDtkLoader::LDtkLoader
)

# Important pour SFML en mode statique
target_compile_definitions(${PROJECT_NAME} PRIVATE SFML_STATIC)

# 8. FIX : Répertoire de travail pour Visual Studio (Pour charger les assets)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")