// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void ALobbyGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
    if(NumberOfPlayers == 1){
        UWorld * World = GetWorld();
        if(World){
            // Log pour confirmer que la carte a été trouvée
            FString MapPath = "/Game/Maps/BlasterMap?listen";
            if (FPaths::FileExists(FPaths::ConvertRelativePathToFull(MapPath))) {
                UE_LOG(LogTemp, Warning, TEXT("La carte %s a été trouvée."), *MapPath);
            } else {
                UE_LOG(LogTemp, Error, TEXT("La carte %s est introuvable."), *MapPath);
            }
            
            bUseSeamlessTravel = true;
            World->ServerTravel(MapPath);
        } else {
            UE_LOG(LogTemp, Error, TEXT("Le monde est nul. Impossible de lancer ServerTravel."));
        }
    }
}
