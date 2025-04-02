// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPickup.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/BuffComponent.h"


void AShieldPickup::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherCompBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherCompBodyIndex, bFromSweep, SweepResult);
    ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
    if(BlasterCharacter)
    {
        UBuffComponent* Buff = Cast<UBuffComponent>(BlasterCharacter->GetBuff());
        if(Buff)
        {
            Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
        }
    }
    Destroy();
}

