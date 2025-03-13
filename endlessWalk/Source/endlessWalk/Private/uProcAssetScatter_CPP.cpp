// Fill out your copyright notice in the Description page of Project Settings.


#include "uProcAssetScatter_CPP.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"

AuProcAssetScatter_CPP::AuProcAssetScatter_CPP()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create Procedural Mesh Component
    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = ProceduralMesh;

    // Default values
    NumAssetsToSpawn = 10;
    SpawnAreaSize = FVector(1000, 1000, 0); // 1000x1000 area
}

void AuProcAssetScatter_CPP::BeginPlay()
{
    Super::BeginPlay();

    GenerateProceduralMesh();
    SpawnProceduralAssets();
}

void AuProcAssetScatter_CPP::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AuProcAssetScatter_CPP::GenerateProceduralMesh()
{
    // Dummy quad mesh (adjust as needed)
    TArray<FVector> Vertices = {
        FVector(-500, -500, 0), FVector(500, -500, 0),
        FVector(-500, 500, 0), FVector(500, 500, 0)
    };

    TArray<int32> Triangles = { 0, 1, 2, 2, 1, 3 };
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FProcMeshTangent> Tangents;
    TArray<FColor> VertexColors;

    ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

	ProceduralMesh->SetMaterial(0, Material);
}

void AuProcAssetScatter_CPP::SpawnProceduralAssets()
{
    if (!AssetToSpawn) {
        UE_LOG(LogTemp, Warning, TEXT("No asset assigned for spawning!"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    for (int32 i = 0; i < NumAssetsToSpawn; i++)
    {
        FVector RandomLocation = FVector(
            FMath::RandRange(-SpawnAreaSize.X / 2, SpawnAreaSize.X / 2),
            FMath::RandRange(-SpawnAreaSize.Y / 2, SpawnAreaSize.Y / 2),
            0 // Assuming assets spawn on the flat procedural mesh
        );

        AStaticMeshActor* SpawnedActor = GetWorld()->SpawnActor<AStaticMeshActor>(
            AStaticMeshActor::StaticClass(),
            RandomLocation,
            FRotator::ZeroRotator,
            SpawnParams
        );

        if (SpawnedActor)
        {
            SpawnedActor->GetStaticMeshComponent()->SetStaticMesh(AssetToSpawn);
            SpawnedActor->SetMobility(EComponentMobility::Movable);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Spawned %d assets"), NumAssetsToSpawn);
}
