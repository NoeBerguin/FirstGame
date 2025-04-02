// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionData.h"

UMultiplayerSessionData::UMultiplayerSessionData()
    : Super()
{
   
}

void UMultiplayerSessionData::Initialize(FString InCreatorName, FString InNumberOfPlayers, const FOnlineSessionSearchResult& Session, UMultiplayerSessionsSubsystem * InMultiplayerSessionsSubsystem)
{
    CreatorName = InCreatorName;
    NumberOfPlayers = InNumberOfPlayers;
    SessionResult = Session;
    MultiplayerSessionsSubsystem = InMultiplayerSessionsSubsystem;
}

void UMultiplayerSessionData::JoinSession()
{
    // if(MultiplayerSessionsSubsystem && SessionResult)
    // {
    //    UE_LOG(LogTemp, Log, TEXT("Adresse du pointeur = %p, Adresse de l'objet = %p"), SessionResult, &*SessionResult);
    //     // MultiplayerSessionsSubsystem->JoinSession(*SessionResult);
    // }
}

