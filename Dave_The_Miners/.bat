@echo off
set BUILD_DIR=out\build\x64-Debug
set EXE_NAME=Dave_The_Miners.exe

echo --- DAVE THE MINER : BUILD & RUN ---

:: Build the project
cmake --build %BUILD_DIR% --target Dave_The_Miners
if %ERRORLEVEL% NEQ 0 (
    echo [ERREUR] La compilation a echoue.
    pause
    exit /b %ERRORLEVEL%
)

:: Run the game
echo [OK] Lancement de %EXE_NAME%...
start "" "%BUILD_DIR%\%EXE_NAME%"
