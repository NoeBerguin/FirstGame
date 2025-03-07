// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerMenu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"


void UMultiplayerMenu::MenuSetup(int32 NumOfPublicConnections ,FString TypeOfMatch,FString LobbyPath)
{
    PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
    NumPublicConnections = NumOfPublicConnections;
    MatchType = TypeOfMatch;
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld * World = GetWorld();
    if(World){
        APlayerController * PlayerController = World->GetFirstPlayerController();
        if(PlayerController){
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }

    UGameInstance * GameInstance = GetGameInstance();
    if(GameInstance){
       MultiplayerSessionsSubsystem =  GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }

    if(MultiplayerSessionsSubsystem){
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
        MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
        MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
    }
}

void UMultiplayerMenu::HostButtonClicked()
{ 
    HostButton->SetIsEnabled(false);
    if(MultiplayerSessionsSubsystem){
        MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
    }
}

void UMultiplayerMenu::JoinButtonClicked()
{
    JoinButton->SetIsEnabled(false);
    if(MultiplayerSessionsSubsystem){
        MultiplayerSessionsSubsystem->FindSessions(10000);
    }

}

void UMultiplayerMenu::MenuTearDown()
{
    RemoveFromParent();
    UWorld * World = GetWorld();
    if(World){
        APlayerController * PlayerController = World->GetFirstPlayerController();
        if(PlayerController){
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }
}

bool UMultiplayerMenu::Initialize()
{
    if(!Super::Initialize()){
        return false;
    }

    if(HostButton){
        HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
    }

    if(JoinButton){
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
    }


    return true;
}

void UMultiplayerMenu::NativeDestruct()
{
    MenuTearDown();
    Super::NativeDestruct();
}

void UMultiplayerMenu::OnCreateSession(bool bWasSuccessful)
{
    if(bWasSuccessful){
        if(GEngine){
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Yellow,
                FString(TEXT("Session created successfully!"))
            );
        }
        UWorld * World = GetWorld();
        if(World){
            World->ServerTravel(PathToLobby);
        }
    }
    else{
        if(GEngine){
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Red,
                FString(TEXT("Failed creating session !!"))
            );
        }
         HostButton->SetIsEnabled(true);
    }
}

void UMultiplayerMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionResults, bool bWasSuccessful)
{
    if(MultiplayerSessionsSubsystem == nullptr){
        return;
    }
    for(auto Result : SessionResults){
        FString SettingsValue;
        Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
        if(SettingsValue == MatchType){
            MultiplayerSessionsSubsystem->JoinSession(Result);
            return;
        }
    }
    if(!bWasSuccessful || SessionResults.Num() == 0){
        JoinButton->SetIsEnabled(true);
    }
}

void UMultiplayerMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem * Subsystem = IOnlineSubsystem::Get();
    if(Subsystem){
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if(SessionInterface.IsValid()){
            FString Address;
            SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

            APlayerController * PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if(PlayerController){
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
        }
    }

    if(Result != EOnJoinSessionCompleteResult::Success){
        JoinButton->SetIsEnabled(true);
    }
}

void UMultiplayerMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMultiplayerMenu::OnStartSession(bool bWasSuccessful)
{
}
