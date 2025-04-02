// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MultiplayerSessionListEntry.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UMultiplayerSessionListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ConnectButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CreatorNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NumberOfPlayerText;

	class UMultiplayerSessionData * MultiplayerSessionData;

	UFUNCTION()
	void ConnectButtonClicked();
	
	
};
