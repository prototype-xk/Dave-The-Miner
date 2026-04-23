cmake_minimum_required(VERSION 3.15)
project(Dave_The_Miners)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Chemins des Libs (Relatifs à la racine, donc ok pour tout le monde)
set(SFML_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.1.0")
include_directories("${SFML_PATH}/include")
link_directories("${SFML_PATH}/lib")

add_subdirectory(libs/LDtkLoader-1.5.3.1)

set(JSON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann_json")
include_directories("${JSON_PATH}/include")

# 2. Définition du chemin des assets pour le code C++
add_definitions(-DASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# 3. Fichiers sources
file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 4. L'exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 5. Includes
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

# 6. Links
target_link_libraries(${PROJECT_NAME} PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system 
    LDtkLoader::LDtkLoader
)

# 7. FIX : Répertoire de travail pour Visual Studio (Pour charger les assets)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# 8. FIX : Copie auto des DLL pour les collègues
if(WIN32)
    file(GLOB SFML_DLLS "${SFML_PATH}/bin/*.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_DLLS} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()