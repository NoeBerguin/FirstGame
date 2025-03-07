// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

//
// Declairing our own delegates for MUltiplayerMenu class to bind callback to 
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete,const TArray<FOnlineSessionSearchResult>&SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	public:

		UMultiplayerSessionsSubsystem();

		//To Handle session functionality. the menu class will call these
		void CreateSession(int32 NumPublicConnections, FString MatchType);

		void FindSessions (int32 MaxSearchResults);

		void JoinSession(const FOnlineSessionSearchResult & SessionResult);

		void DestroySession();

		void StartSession(); 

		// delegate for Multiplyer menu class
		FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
		FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
		FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
		FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
		FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

	protected: 

		//Internal callback for the delegate list
		void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
		void OnFindSessionComplete(bool bWasSuccessful);
		void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
		void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
		void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);


	private:
		IOnlineSessionPtr SessionInterface;
		TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
		TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

		//To add to delegate list
		//We will bind MultiplayerSessionsSubsystem internal callback to these

		FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
		FDelegateHandle CreateSessionCompleteDelegateHandle;

		FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
		FDelegateHandle FindSessionsCompleteDelegateHandle;

		FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
		FDelegateHandle JoinSessionCompleteDelegateHandle;

		FOnDestroySessionCompleteDelegate DestroySessionsCompleteDelegate;
		FDelegateHandle DestroySessionsCompleteDelegateHandle;

		FOnStartSessionCompleteDelegate StartSessionsCompleteDelegate;
		FDelegateHandle StartSessionsCompleteDelegateHandle;

		bool bCreateSessionOnDestroy{false};
		int32 LastNumPublicConnections;
		FString LastMatchType;

};
