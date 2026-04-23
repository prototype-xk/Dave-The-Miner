cmake_minimum_required(VERSION 3.15)
project(Dave_The_Miners)

# --- 1. CONFIGURATION DU COMPILATEUR ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 2. CONFIGURATION SFML 3.1.0 ---
set(SFML_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.1.0")
include_directories("${SFML_PATH}/include")
link_directories("${SFML_PATH}/lib")

# --- 3. CONFIGURATION LDTKLOADER ---
add_subdirectory(libs/LDtkLoader-1.5.3.1)

# --- 4. CONFIGURATION NLOHMANN JSON ---
# Puisque json.hpp est dans libs/nlohmann/json.hpp, 
# on ajoute "libs" aux chemins de recherche.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs")

# --- 5. RECHERCHE AUTOMATIQUE DES FICHIERS ---
file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

# --- 6. CRÉATION DE L'EXÉCUTABLE ---
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# --- 7. GESTION DES CHEMINS D'INCLUSION PROJET ---
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

# --- 8. LIEN AVEC LES BIBLIOTHÈQUES ---
target_link_libraries(${PROJECT_NAME} PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system 
    LDtkLoader::LDtkLoader
)