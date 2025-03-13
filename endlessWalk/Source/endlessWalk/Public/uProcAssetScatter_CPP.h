// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "uProcAssetScatter_CPP.generated.h"

UCLASS()
class ENDLESSWALK_API AuProcAssetScatter_CPP : public AActor
{
    GENERATED_BODY()

public:
    AuProcAssetScatter_CPP();

    /** Procedural Mesh */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Procedural")
    UProceduralMeshComponent* ProceduralMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* Material = nullptr;

    /** Static Mesh to Spawn */
    UPROPERTY(EditAnywhere, Category = "Procedural")
    UStaticMesh* AssetToSpawn;

    /** Number of assets to spawn */
    UPROPERTY(EditAnywhere, Category = "Procedural", meta = (ClampMin = "1", UIMin = "1"))
    int32 NumAssetsToSpawn;

    /** Spawning area size */
    UPROPERTY(EditAnywhere, Category = "Procedural")
    FVector SpawnAreaSize;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    void GenerateProceduralMesh();
    void SpawnProceduralAssets();
};
