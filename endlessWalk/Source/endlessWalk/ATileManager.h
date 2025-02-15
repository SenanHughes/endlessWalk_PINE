// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATileManager.generated.h"

UCLASS()
class ENDLESSWALK_API ATileManager : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ATileManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Function to spawn a tile
    void SpawnTile(const FVector& SpawnLocation);

    // Blueprint tile to spawn (greybox tile)
    UPROPERTY(EditAnywhere, Category = "Tile Spawning")
    TSubclassOf<AActor> TileBlueprint;

    // Distance threshold for spawning a new tile
    UPROPERTY(EditAnywhere, Category = "Tile Spawning")
    float SpawnThreshold;

    // Store the last spawn location for reference
    FVector LastSpawnLocation;

    // Reference to the player
    UPROPERTY()
    AActor* PlayerActor;
};