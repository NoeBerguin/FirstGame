// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"



UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(Display = "Initial State"),
	EWS_Equipped UMETA(Display = "Equipped"),
	EWS_EquippedSecondary UMETA(Display = "Equipped Secondary"),
	EWS_Dropped UMETA(Display = "Dropped"),

	EWS_Max UMETA(Display = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EFireType : uint8 
{
	EFT_HitScan UMETA(Display = "Hit Scan Weapon"),
	EFT_Projectile UMETA(Display = "Projectile Weapon"),
	EFT_Shotgun UMETA(Display = "Shotgun Weapon"),

	EFT_Max UMETA(Display = "DefaultMAX"),
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override; 
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();
	virtual void OnEquipped(); 
	virtual void OnDropped(); 
	virtual void OnEquippedSecondary();

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

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;

	UFUNCTION()
 	void OnPingTooHigh(bool bPingTooHigh);

private: 

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent * WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent * AreaSphere; 

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent * PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset * FireAnimation; 

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	// Zoom FOV while aiming 
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	void SpendRound();

	// number of unprocessed request from the server for ammo
	int32 Sequence = 0;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;


public:

	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent * GetAreaSphere() const {return AreaSphere;}
	FORCEINLINE USkeletalMeshComponent * GetWeaponMesh() const{return WeaponMesh;}
	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterpSpeed;}
	bool IsEmpty();
	bool IsFull();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType;}
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity;}
	FORCEINLINE float GetDamage() const { return Damage; }

public:
	// Textures for the crosshairs
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D * CrosshairsCenter; 

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D * CrosshairsLeft; 

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D * CrosshairsRight; 

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D * CrosshairsTop; 

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D * CrosshairsBottom; 

	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = 0.15f; 

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;
	
	UPROPERTY(EditAnywhere)
	class USoundCue * EquipSound;

	/*
	* Enable or disable custom depth
	*/

	void EnableCustomDepth(bool bEnable); 

	bool bDestroyWeapon = false; 

	UPROPERTY(EditAnywhere)
	EFireType FireType; 

	/*
	* Trace end with scatter
	*/

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

	FVector TraceEndWithScatter(const FVector& HitTarget);
};
