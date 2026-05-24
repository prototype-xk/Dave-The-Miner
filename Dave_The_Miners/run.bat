@echo off
set BUILD_DIR=out\build\x64-Debug
set EXE_NAME=Dave_The_Miners.exe

echo --- DAVE THE MINER : BUILD ^& RUN ---

:: 1. Configuration CMake si absente ou corrompue (on verifie CMakeCache, plus fiable que .sln)
if not exist "%BUILD_DIR%\CMakeCache.txt" (
    echo [INFO] Configuration CMake introuvable. Lancement de la configuration...
    cmake -B "%BUILD_DIR%" -S . -A x64
    if %ERRORLEVEL% NEQ 0 (
        echo [ERREUR] La configuration de CMake a echoue.
        echo [CONSEIL] Verifiez que Visual Studio est installe avec le support C++,
        echo          et que les libs SFML sont presentes dans libs/SFML-3.1.0/
        pause
        exit /b %ERRORLEVEL%
    )
)

:: 2. Compilation
echo [INFO] Compilation en cours...
cmake --build "%BUILD_DIR%" --target Dave_The_Miners --config Debug
if %ERRORLEVEL% NEQ 0 (
    echo [ERREUR] La compilation a echoue. Consultez les messages ci-dessus.
    pause
    exit /b %ERRORLEVEL%
)

:: 3. Lancement (Visual Studio place l'exe dans Debug/, d'autres generateurs a la racine)
set FINAL_EXE=%BUILD_DIR%\Debug\%EXE_NAME%
if not exist "%FINAL_EXE%" (
    set FINAL_EXE=%BUILD_DIR%\%EXE_NAME%
)

if exist "%FINAL_EXE%" (
    echo [OK] Lancement de %EXE_NAME%...
    start "" "%FINAL_EXE%"
) else (
    echo [ERREUR] Executable introuvable apres compilation.
    echo [CONSEIL] Verifiez le nom du projet dans CMakeLists.txt.
    pause
)