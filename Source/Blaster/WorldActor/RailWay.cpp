#include "RailWay.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMesh.h" 
#include "UObject/ConstructorHelpers.h" 
#include "Components/SphereComponent.h"
#include "Components/SphereComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "DrawDebugHelpers.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraDataInterfaceSpline.h"
#include "NiagaraParameterCollection.h"
#include "NiagaraSystemInstance.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "NiagaraComponent.h"


ARailWay::ARailWay()
{
	PrimaryActorTick.bCanEverTick = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>("RailWaySpline",true);
	if (SplineComponent)
    {
        SetRootComponent(SplineComponent);
        SplineComponent->SetIsReplicated(true);
    } 

	MovingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovingMesh"));
	if (MovingMesh) MovingMesh->SetupAttachment(RootComponent);

    AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
    AreaSphere->SetupAttachment(MovingMesh);
    AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AreaSphere->SetCollisionObjectType(ECC_WorldDynamic);
    AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    AreaSphere->SetGenerateOverlapEvents(true);
}

void ARailWay::BeginPlay()
{
	Super::BeginPlay();

    if(AreaSphere)
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ARailWay::OnSphereOverlap);
    	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ARailWay::OnSphereEndOverlap);
	}

	if (SplineComponent && MovingMesh)
    {
		InitialMeshTransform = MovingMesh->GetRelativeTransform();
        CurrentDistance = 0.0f;
        UpdateMeshTransform();
    }	
	
}

void ARailWay::UpdateMeshTransform()
{
    if (!SplineComponent || !MovingMesh) return;

    const float EffectiveDistance = bReverseDirection ? 
        (SplineComponent->GetSplineLength() - CurrentDistance) : 
        CurrentDistance;

    const FTransform SplineTransform = SplineComponent->GetTransformAtDistanceAlongSpline(
        EffectiveDistance, 
        ESplineCoordinateSpace::World,
        true
    );

    FTransform NewTransform = InitialMeshTransform * SplineTransform;
    
    if (bOrientRotationToMovement)
    {
        FRotator SplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(
            EffectiveDistance, 
            ESplineCoordinateSpace::World
        );
        
        if (bReverseDirection)
        {
            SplineRotation.Yaw += 180.0f;
            SplineRotation.Normalize();
        }
        
        NewTransform.SetRotation(SplineRotation.Quaternion());
    }
    MovingMesh->SetWorldTransform(NewTransform);
}

void ARailWay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsMoving && SplineComponent && MovingMesh)
    {
        CurrentDistance += MovementSpeed * DeltaTime * (bReverseDirection ? -1.0f : 1.0f);
        const float SplineLength = SplineComponent->GetSplineLength();  
        if ((!bReverseDirection && CurrentDistance > SplineLength) || 
            (bReverseDirection && CurrentDistance < 0.0f))
        {
            if (bLoopMovement)
            {
                CurrentDistance = bReverseDirection ? SplineLength : 0.0f;
            }
            else
            {
                CurrentDistance = bReverseDirection ? 0.0f : SplineLength;
                bIsMoving = false;
            }
        }
        
        UpdateMeshTransform();
    }

    UpdateSplineForCharacter();

}

void ARailWay::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if(!Mesh || !SplineComponent || !HasAuthority()) return;

    FBox MeshBounds = Mesh->GetBoundingBox();
    const float MeshLength = MeshBounds.GetSize().X;
    const float SegmentLength = MeshLength * 0.98f; // 2% de chevauchement

    const float SplineLength = SplineComponent->GetSplineLength();
    const int32 NumSegments = FMath::CeilToInt(SplineLength / SegmentLength);
    const float TangentScale = SplineLength / NumSegments; // Ajustement dynamique

    TArray<UActorComponent*> OldComponents;
    GetComponents(USplineMeshComponent::StaticClass(), OldComponents);
    for (UActorComponent* Component : OldComponents)
    {
        Component->DestroyComponent();
    }

    for (int32 i = 0; i < NumSegments; ++i)
    {
        const float StartDistance = i * SegmentLength;
        const float EndDistance = FMath::Min(StartDistance + SegmentLength, SplineLength);

        USplineMeshComponent* NewRail = NewObject<USplineMeshComponent>(this);
        NewRail->RegisterComponent();
        NewRail->SetStaticMesh(Mesh);
        NewRail->SetMobility(EComponentMobility::Movable);
        NewRail->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

        FVector StartPos = SplineComponent->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local);
        FVector StartTangent = SplineComponent->GetTangentAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local).GetSafeNormal() * TangentScale;
        FVector EndPos = SplineComponent->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local);
        FVector EndTangent = SplineComponent->GetTangentAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local).GetSafeNormal() * TangentScale;

        NewRail->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
        NewRail->SetForwardAxis(ESplineMeshAxis::X); // Adaptez selon votre mesh
        NewRail->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        NewRail->SetCollisionResponseToAllChannels(ECR_Block);
    }
}

void ARailWay::UpdateSplineForCharacter()
{
    for (auto& Pair : BeamConnections)
    {
        ABlasterCharacter* Character = Pair.Key;
        FBeamConnection Connection = Pair.Value;
        
        if (Character && Connection.Spline && Connection.NiagaraComponent)
        {
            if (!Character) return;
            
            BeamConnections[Character].Spline->ClearSplinePoints();
            BeamConnections[Character].Spline->AddSplinePoint(MovingMesh->GetComponentLocation(), ESplineCoordinateSpace::World);
            
            FVector MidPoint = (MovingMesh->GetComponentLocation() + Character->GetActorLocation()) * 0.5f;
            MidPoint.Z += CurveHeight;
            BeamConnections[Character].Spline->AddSplinePoint(MidPoint, ESplineCoordinateSpace::World);
            
            BeamConnections[Character].Spline->AddSplinePoint(Character->GetActorLocation(), ESplineCoordinateSpace::World);
            
            for (int32 i = 0; i < BeamConnections[Character].Spline->GetNumberOfSplinePoints(); i++)
            {
                BeamConnections[Character].Spline->SetSplinePointType(i, ESplinePointType::Curve);
            }
        }
    }
}


// void ARailWay::CreateConnectionForCharacter(ABlasterCharacter* BlasterCharacter)
// {
//     if (!Mesh || !AreaSphere || !BlasterCharacter || BeamConnections.Contains(BlasterCharacter)) return;
//     //if (!Mesh || !BlasterCharacter || BeamConnections.Contains(BlasterCharacter)) return;


//     UE_LOG(LogTemp, Warning, TEXT("Creating beam for character: %s"), *BlasterCharacter->GetName());
    
//     FBeamConnection NewConnection;
    
//     // 1. Création de la spline avec nom unique
//     FName UniqueSplineName = *FString::Printf(TEXT("Spline_%s"), *BlasterCharacter->GetName());
//     NewConnection.Spline = NewObject<USplineComponent>(this, USplineComponent::StaticClass(), UniqueSplineName);
//     NewConnection.Spline->SetupAttachment(MovingMesh);
//     NewConnection.Spline->RegisterComponent();
//     NewConnection.Spline->SetRelativeLocation(FVector::ZeroVector);

//     // 2. Création du Niagara Component avec système unique
//     if (BeamEffect)
//     {

//         NewConnection.NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
//             BeamEffect,  // Utilise la copie unique
//             NewConnection.Spline,
//             NAME_None,
//             FVector::ZeroVector,
//             FRotator::ZeroRotator,
//             EAttachLocation::KeepRelativeOffset,
//             true
//         );
//         NewConnection.NiagaraComponent->SetNiagaraVariableObject(TEXT("SplineObject"),  NewConnection.Spline);
// 		NewConnection.NiagaraComponent->ReinitializeSystem();
//     }
    
//     BeamConnections.Add(BlasterCharacter, NewConnection);
// }

void ARailWay::CreateConnectionForCharacter(ABlasterCharacter* BlasterCharacter)
{
    if (!Mesh || !AreaSphere || !BlasterCharacter || BeamConnections.Contains(BlasterCharacter)) 
        return;

    UE_LOG(LogTemp, Warning, TEXT("Creating beam for character: %s"), *BlasterCharacter->GetName());
    
    FBeamConnection NewConnection;
    
    // 1. Création de la spline avec nom unique
    FName UniqueSplineName = *FString::Printf(TEXT("Spline_%s"), *BlasterCharacter->GetName());
    NewConnection.Spline = NewObject<USplineComponent>(this, USplineComponent::StaticClass(), UniqueSplineName);
    
    // Correction: Utilisation sécurisée de l'attachement
    if (NewConnection.Spline)
    {
        if (MovingMesh && MovingMesh->IsRegistered())
        {
            // Si MovingMesh est déjà enregistré, on utilise AttachToComponent
            NewConnection.Spline->AttachToComponent(
                MovingMesh, 
                FAttachmentTransformRules::SnapToTargetNotIncludingScale
            );
        }
        else
        {
            // Sinon, SetupAttachment est autorisé
            NewConnection.Spline->SetupAttachment(MovingMesh);
        }
        
        NewConnection.Spline->RegisterComponent();
        NewConnection.Spline->SetRelativeLocation(FVector::ZeroVector);
    }

    // 2. Création du Niagara Component avec système unique
    if (BeamEffect && NewConnection.Spline)
    {
        NewConnection.NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            BeamEffect,
            NewConnection.Spline,  // Attachement à la spline
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true
        );
        
        if (NewConnection.NiagaraComponent)
        {
            NewConnection.NiagaraComponent->SetNiagaraVariableObject(TEXT("SplineObject"), NewConnection.Spline);
            NewConnection.NiagaraComponent->ReinitializeSystem();
        }
    }
    
    BeamConnections.Add(BlasterCharacter, NewConnection);
}


void ARailWay::DestroyConnectionForCharacter(ABlasterCharacter* BlasterCharacter)
{
    if (!BlasterCharacter || !BeamConnections.Contains(BlasterCharacter)) return;
    FBeamConnection& Connection = BeamConnections[BlasterCharacter];
    if (Connection.Spline)
    {
        Connection.Spline->DestroyComponent();
    }
    if (Connection.NiagaraComponent)
    {
        Connection.NiagaraComponent->DestroyComponent();
    }
    BeamConnections.Remove(BlasterCharacter);
}

void ARailWay::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter && !BeamConnections.Contains(BlasterCharacter))
	{
        ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(BlasterCharacter->GetPlayerState());
        if (BlasterPlayerState)
        {
            if(BlasterPlayerState->GetTeam() == Team)
            {
		        CreateConnectionForCharacter(BlasterCharacter);
                TeamPlayerAround = FMath::Clamp(TeamPlayerAround + 1, 0, 5);
            }
        }
	}
    bIsMoving = TeamPlayerAround > 0;
}

void ARailWay::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
        ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(BlasterCharacter->GetPlayerState());
        if (BlasterPlayerState)
        {
            if(BlasterPlayerState->GetTeam() == Team)
            {
                DestroyConnectionForCharacter(BlasterCharacter);
                TeamPlayerAround = FMath::Clamp(TeamPlayerAround - 1, 0, 5);	
            }
        }
	}
    bIsMoving = TeamPlayerAround > 0;
}

