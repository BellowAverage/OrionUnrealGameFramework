// Fill out your copyright notice in the Description page of Project Settings.

#include "OrionWeapon.h"
//#include "OrionChara.h"
#include "OrionProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AOrionWeapon::AOrionWeapon()
{
// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrionWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrionWeapon::SpawnOrionBulletActor(const FVector& TargetLocation, const FVector& MuzzleLocation)
{
    // Check if ProjectileClass is valid
    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ProjectileClass is null. Cannot spawn projectile."));
        return;
    }

    // Check if ProjectileClass is a subclass of AOrionProjectile
    if (!ProjectileClass->IsChildOf(AOrionProjectile::StaticClass()))
    {
        UE_LOG(LogTemp, Error, TEXT("ProjectileClass is not a subclass of AOrionProjectile."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("GetWorld() returned null. Cannot spawn projectile."));
        return;
    }

    FVector SpawnLocation = MuzzleLocation;

    // Calculate direction
    FVector DirectionToTarget = (TargetLocation - SpawnLocation).GetSafeNormal();

    // Offset to avoid collision with the weapon
    SpawnLocation += DirectionToTarget * 20.f;

    FRotator SpawnRotation = (DirectionToTarget + FVector(0.f, 0.f, SpawnZOffset)).Rotation();

    UE_LOG(LogTemp, Warning, TEXT("SpawnLocation: %s, SpawnRotation: %s"),
        *SpawnLocation.ToString(), *SpawnRotation.ToString());

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    // Spawn the projectile
    AOrionProjectile* SpawnedProjectile = World->SpawnActor<AOrionProjectile>(
        ProjectileClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    if (SpawnedProjectile)
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully spawned projectile: %s"), *SpawnedProjectile->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn projectile."));
        return;
    }

    // Set launch direction and velocity
    if (SpawnedProjectile && SpawnedProjectile->ProjectileMovement)
    {
        FVector LaunchDirection = DirectionToTarget; // Use the calculated direction
        LaunchDirection.Normalize();
        SpawnedProjectile->ProjectileMovement->Velocity = LaunchDirection * SpawnedProjectile->ProjectileMovement->InitialSpeed;
    }

    UE_LOG(LogTemp, Log, TEXT("FireProjectileForward: Spawned BP_OrionProjectile at %s"), *SpawnLocation.ToString());

    // Add Niagara Effect at Muzzle Location
    if (MuzzleFlashEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            MuzzleFlashEffect,
            MuzzleLocation,          // Spawn at the muzzle
            SpawnRotation           // Match the weapon's rotation
        );

        UE_LOG(LogTemp, Log, TEXT("Muzzle flash effect spawned at location: %s"), *MuzzleLocation.ToString());
    }

    if (!World) return;

    // Ensure particle system resource is loaded
    if (MuzzleParticleSystem)
    {
        // Spawn particle effect at specified location
        UGameplayStatics::SpawnEmitterAtLocation(
            World,            // World context
            MuzzleParticleSystem, // Particle effect resource
            MuzzleLocation,   // Spawn location
            SpawnRotation,    // Spawn rotation
            true              // Auto destroy
        );
    }
}


