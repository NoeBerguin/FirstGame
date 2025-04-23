// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Blaster/BlasterTypes/Team.h"
#include "RailWay.generated.h"

USTRUCT(BlueprintType)
struct FBeamConnection
{
	GENERATED_BODY()

	UPROPERTY()
	class USplineComponent* Spline = nullptr;

	UPROPERTY()
	class UNiagaraComponent* NiagaraComponent = nullptr;
};

UCLASS()
class BLASTER_API ARailWay : public AActor
{
	GENERATED_BODY()
	
public:	
	ARailWay();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Spline")
	class UStaticMesh * Mesh;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Spline")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis; 

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent * OverlappedComponent,
		AActor * OtherActor,
		UPrimitiveComponent * OtherComp,
		int32 OtherCompBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent * OverlappedComponent,
		AActor * OtherActor,
		UPrimitiveComponent * OtherComp,
		int32 OtherCompBodyIndex
	);

private: 

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MovementSpeed = 100.f;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    float CurrentDistance = 0.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool bLoopMovement = false;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    UStaticMeshComponent* MovingMesh;

	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bOrientRotationToMovement = true;

	void UpdateMeshTransform();

	FTransform InitialMeshTransform;

	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bReverseDirection;

	bool bIsMoving = false;

	UPROPERTY(VisibleAnywhere, Category = "Event")
	class USphereComponent * AreaSphere; 

	UPROPERTY(EditAnywhere, Category = "Niagara")
    float BeamWidth = 5.0f;

    // Remplacez les paramètres de debug par des paramètres Niagara
	UPROPERTY(EditAnywhere, Category = "Niagara")
	class UNiagaraSystem* BeamEffect;

	UPROPERTY()
    TMap< class ABlasterCharacter*, FBeamConnection> BeamConnections;
    
    UPROPERTY(EditAnywhere, Category = "Debug")
    float CurveHeight = 100.0f;

	void DestroyConnectionForCharacter(ABlasterCharacter* BlasterCharacter);
	void UpdateSplineForCharacter();
	void CreateConnectionForCharacter(ABlasterCharacter* BlasterCharacter);

	UPROPERTY(EditAnywhere, Category = "Event")
	ETeam Team = ETeam::ET_BlueTeam;

	int32 TeamPlayerAround = 0;

public:	
	

};
