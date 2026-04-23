@echo off
set BUILD_DIR=out\build\x64-Debug
set EXE_NAME=Dave_The_Miners.exe

echo --- DAVE THE MINER : BUILD ^& RUN ---

:: 1. Configuration si le dossier build n'existe pas
if not exist "%BUILD_DIR%" (
    echo [INFO] Le dossier de build n'existe pas. Configuration de CMake...
    cmake -B "%BUILD_DIR%" -S .
    if %ERRORLEVEL% NEQ 0 (
        echo [ERREUR] La configuration de CMake a echoue.
        pause
        exit /b %ERRORLEVEL%
    )
)

:: 2. Compilation
echo [INFO] Compilation en cours...
cmake --build "%BUILD_DIR%" --target Dave_The_Miners --config Debug
if %ERRORLEVEL% NEQ 0 (
    echo [ERREUR] La compilation a echoue.
    pause
    exit /b %ERRORLEVEL%
)

:: 3. Lancement
:: Note : Les generateurs multi-config (comme Visual Studio) placent l'executable dans un sous-dossier (Debug/Release)
set FINAL_EXE_PATH=%BUILD_DIR%\Debug\%EXE_NAME%
if not exist "%FINAL_EXE_PATH%" (
    set FINAL_EXE_PATH=%BUILD_DIR%\%EXE_NAME%
)

if exist "%FINAL_EXE_PATH%" (
    echo [OK] Lancement de %EXE_NAME%...
    start "" "%FINAL_EXE_PATH%"
) else (
    echo [ERREUR] Executable introuvable : %FINAL_EXE_PATH%
    pause
)
