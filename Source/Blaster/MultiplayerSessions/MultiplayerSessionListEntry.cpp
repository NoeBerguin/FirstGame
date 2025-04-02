// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionListEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MultiplayerSessionData.h"
#include "MultiplayerSessionsSubsystem.h"

bool UMultiplayerSessionListEntry::Initialize()
{
	if(!Super::Initialize()){
        return false;
    }

    if(ConnectButton){
        ConnectButton->OnClicked.AddDynamic(this, &ThisClass::ConnectButtonClicked);
    }

    return true;
}

void UMultiplayerSessionListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    if (UMultiplayerSessionData* SessionData = Cast<UMultiplayerSessionData>(ListItemObject))
	{
		MultiplayerSessionData = SessionData; 
		if (CreatorNameText)
		{
			CreatorNameText->SetText(FText::FromString(SessionData->GetCreatorName()));
		}

		if (NumberOfPlayerText)
		{
			NumberOfPlayerText->SetText(FText::FromString(SessionData->GetNumberOfPlayer()));
		}
	}
}

void UMultiplayerSessionListEntry::ConnectButtonClicked()
{
	UGameInstance * GameInstance = GetGameInstance();
    if(GameInstance){
		UMultiplayerSessionsSubsystem * MultiplayerSessionsSubsystem =  GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if(MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->JoinSession(MultiplayerSessionData->SessionResult);
		}
    }
}
