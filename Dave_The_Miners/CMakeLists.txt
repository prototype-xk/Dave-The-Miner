cmake_minimum_required(VERSION 3.15)
project(Dave_The_Miners)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Configuration SFML (utilisation du package fourni dans libs)
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.1.0/lib/cmake/SFML")
set(SFML_STATIC_LIBRARIES ON)
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# 2. LDtkLoader
add_subdirectory(libs/LDtkLoader-1.5.3.1)

# 3. Fichiers sources
file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 4. L'executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 5. Includes
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE libs)

# 6. Links
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    LDtkLoader::LDtkLoader
)

# 7. SFML en mode statique
target_compile_definitions(${PROJECT_NAME} PRIVATE SFML_STATIC)

# 8. Repertoire de travail Visual Studio = racine du projet
#    (les assets sont charges via "assets/..." relatif a la racine)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# 9. Copie automatique du dossier assets a cote de l'exe apres compilation
#    (indispensable pour lancer l'exe hors de Visual Studio)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copie des assets a cote de l'executable..."
)