// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"


// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	SetRootComponent(MeshComponent);
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBox"));
	AreaBox->SetupAttachment(RootComponent);
	AreaBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidget"));
	InfoWidget->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	InitialRotation = GetActorRotation();

	if(AreaBox){
		AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnBoxOverlap);
		AreaBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnBoxEndOverlap);
	}
	if(InfoWidget){
		InfoWidget->SetVisibility(false);
	}
	
}

void ADoor::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherCompBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	ABlasterCharacter * BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter){
		BlasterCharacter->SetOverlappingDoor(this);
	}
	InfoWidget->SetVisibility(true);
}

void ADoor::OnBoxEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherCompBodyIndex)
{
	ABlasterCharacter * BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter){
		BlasterCharacter->SetOverlappingDoor(nullptr);
	}
	InfoWidget->SetVisibility(false);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OpenCloseDoor(DeltaTime);
}

void ADoor::Open(ABlasterCharacter * PlayerCharacter)
{
	bIsOpen = true;
	MouvementDirection = IsPlayerInFront(PlayerCharacter) ? int8(-1) : int8(1);
}

void ADoor::Close(ABlasterCharacter * PlayerCharacter)
{
	bIsOpen = false;
}

void ADoor::OpenCloseDoor(float DeltaTime)
{
	if (MeshComponent)
    {
        // Déterminer l'angle cible en fonction de l'orientation initiale
        float TargetYaw = InitialRotation.Yaw + (bIsOpen ? MouvementDirection * 90.0f : 0.0f);
        
        // Obtenir la rotation actuelle
        FRotator CurrentRotation = GetActorRotation();
        
        // Interpoler vers la rotation cible
        FRotator TargetRotation = FRotator(CurrentRotation.Pitch, TargetYaw, CurrentRotation.Roll);
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 2.0f);
        
        // Appliquer la nouvelle rotation
        SetActorRotation(NewRotation);
    }
}

bool ADoor::IsPlayerInFront(ABlasterCharacter * PlayerCharacter)
{
	if (!PlayerCharacter) return false;

    // Obtenir la rotation locale de la porte (indépendante de la rotation globale)
    FRotator DoorRotation = MeshComponent->GetRelativeRotation();

    // Calculer le Right Vector local à partir de la rotation locale
    FVector DoorRight = FRotationMatrix(DoorRotation).GetUnitAxis(EAxis::Y);

    // Calculer le vecteur entre la porte et le joueur
    FVector PlayerToDoor = GetActorLocation() - PlayerCharacter->GetActorLocation();

    // Calcul du produit scalaire
    float DotProduct = FVector::DotProduct(DoorRight, PlayerToDoor);

    // Log pour voir si le joueur est devant ou derrière
    UE_LOG(LogTemp, Warning, TEXT("IsPlayerInFront (Local Y-Axis): %s"), DotProduct < 0 ? TEXT("true") : TEXT("false"));

    // Retourne true si le joueur est devant (DotProduct < 0)
    return DotProduct < 0;
}
