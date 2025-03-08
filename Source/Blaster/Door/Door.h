// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class BLASTER_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	UFUNCTION()
	virtual void OnBoxOverlap(
		UPrimitiveComponent * OverlappedComponent,
		AActor * OtherActor,
		UPrimitiveComponent * OtherComp,
		int32 OtherCompBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);

	UFUNCTION()
	virtual void OnBoxEndOverlap(
		UPrimitiveComponent * OverlappedComponent,
		AActor * OtherActor,
		UPrimitiveComponent * OtherComp,
		int32 OtherCompBodyIndex
	);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Open(class ABlasterCharacter* PlayerCharacter);
	void Close(ABlasterCharacter* PlayerCharacter);

	FORCEINLINE bool GetDoorIsOpen() {return bIsOpen;}

private: 

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent * InfoWidget;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class UBoxComponent* AreaBox;

	void OpenCloseDoor(float DeltaTime);

	UPROPERTY(EditAnywhere)
	float SpeedRotation = 1.f;

	bool IsPlayerInFront(ABlasterCharacter* PlayerCharacter);
	
	int8 MouvementDirection = 1;

private:

	bool bIsOpen = false; 
	FRotator InitialRotation;

};
