#pragma once

#define TRACE_LENGTH 80000.f

#define  CUSTUM_DEPTH_PURPLE 250
#define  CUSTUM_DEPTH_BLUE 251
#define  CUSTUM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
    EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
    EWT_Pistol UMETA(DisplayName = "Pistol"),
    EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
    EWT_Shotgun UMETA(DisplayName = "Shotgun"),
    EWT_SniperRifle UMETA(DisplayName = "SniperRifle"),
    EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
    EWT_MAX UMETA(DisplayName = "DefaultMAX")
};