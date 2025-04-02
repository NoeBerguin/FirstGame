// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "MultiplayerSessionData.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UMultiplayerSessionData : public UObject
{
	GENERATED_BODY()

public:
	UMultiplayerSessionData();

    void Initialize(FString InCreatorName, FString InNumberOfPlayers, const FOnlineSessionSearchResult& Session, class UMultiplayerSessionsSubsystem * InMultiplayerSessionsSubsystem);
	void JoinSession(); 
	class FOnlineSessionSearchResult SessionResult;


private:

	FString CreatorName;
	FString NumberOfPlayers;
	UMultiplayerSessionsSubsystem * MultiplayerSessionsSubsystem;

public:

	FORCEINLINE FString GetCreatorName() const {return CreatorName;} 
	FORCEINLINE FString GetNumberOfPlayer() const {return NumberOfPlayers;} 

};
