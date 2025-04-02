@echo off

:: Chemin vers UnrealEditor.exe
set UE_PATH="C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe"

:: Chemin vers ton fichier .uproject
set PROJECT_PATH="C:\Users\NOA\Documents\Unreal Projects\FirstGame\Blaster.uproject"

:: Lancer Unreal Engine avec -game et -log
%UE_PATH% %PROJECT_PATH% -game -log

:: Attendre que l'utilisateur appuie sur une touche avant de fermer la fenêtre
pause
