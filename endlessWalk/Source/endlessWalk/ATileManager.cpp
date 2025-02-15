// Fill out your copyright notice in the Description page of Project Settings.


#include "ATileManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATileManager::ATileManager()
{
    PrimaryActorTick.bCanEverTick = true;
    SpawnThreshold = 1000.0f;  // Adjust according to your tile size
    LastSpawnLocation = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void ATileManager::BeginPlay()
{
    Super::BeginPlay();

    // Get the player actor (assumes player pawn is present)
    PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    // Spawn the initial tile at our starting location
    if (TileBlueprint)
    {
        LastSpawnLocation = GetActorLocation();
        SpawnTile(LastSpawnLocation);
    }
}

// Called every frame
void ATileManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerActor)
    {
        FVector PlayerLocation = PlayerActor->GetActorLocation();

        // Check if the player has moved beyond the threshold from the last spawn location
        if (FVector::Dist(PlayerLocation, LastSpawnLocation) > SpawnThreshold)
        {
            // Calculate a new spawn location ahead of the player
            FVector Direction = (PlayerLocation - LastSpawnLocation).GetSafeNormal();
            FVector NewSpawnLocation = LastSpawnLocation + Direction * SpawnThreshold;

            SpawnTile(NewSpawnLocation);
            LastSpawnLocation = NewSpawnLocation;
        }
    }
}

void ATileManager::SpawnTile(const FVector& SpawnLocation)
{
    if (TileBlueprint)
    {
        // Spawn the tile at the given location with no rotation
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(TileBlueprint, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    }
}