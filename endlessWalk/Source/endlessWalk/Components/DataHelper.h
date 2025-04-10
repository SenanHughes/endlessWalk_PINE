#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseLite.h"
#include "DataHelper.generated.h"


USTRUCT(BlueprintType)
struct FSplineConfigData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int SplinePoints = 200;
	UPROPERTY(EditAnywhere)
	int PlaneDistance = 100;
	UPROPERTY()
	int MinDistanceToPreviousPoints = 1000.0f;
};

USTRUCT(BlueprintType)
struct FNoiseConfigData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float NoiseFrequency = 0.1f;
	UPROPERTY(EditAnywhere)
	float NoiseAmplitude = 15.0f;
};

struct FNoiseDynamicData
{
	FastNoiseLite Noise;
	float Falloff = 0.0f;
	float SharpnessMultiplier = 50.0f;
	float FrequencyMultiplier = 0.003f;
};

USTRUCT(BlueprintType)
struct FPathConfigData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* PathMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* PathMaterial1 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* PathMaterial2 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PathVertCount = 12;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PathWidth = 600;
};

struct FPathDynamicData
{
	UPROPERTY()
	USplineComponent* PathSpline;
	UPROPERTY()
	UProceduralMeshComponent* PathMesh;
	TArray<FVector> PathVertices = TArray<FVector>();
	TArray<FVector> PathNormals = TArray<FVector>();
	TArray<FProcMeshTangent> PathTangents = TArray<FProcMeshTangent>();
	TArray<int32> PathTriangles = TArray<int32>();
	int DepthOffset = 0;
};

USTRUCT(BlueprintType)
struct FRiverConfigData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* RiverMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RiverVertCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int RiverWidth = 600;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int RiverDepth = 100;
};

struct FRiverDynamicData
{
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicRiverMaterial = nullptr;
	UPROPERTY()
	USplineComponent* RiverSpline;
	UPROPERTY()
	UProceduralMeshComponent* RiverMesh;
	TArray<FVector> RiverVertices = TArray<FVector>();
	TArray<FVector> RiverNormals = TArray<FVector>();
	TArray<FProcMeshTangent> RiverTangents = TArray<FProcMeshTangent>();
	TArray<int32> RiverTriangles = TArray<int32>();
	TArray<FLinearColor> VertexColours = TArray<FLinearColor>();
};

USTRUCT(BlueprintType)
struct FMoundConfigData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* MoundMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* MoundMaterial1 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* MoundMaterial2 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MoundVertCount = 12;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MoundWidth = 600;
	float MoundHeight = 600.0f;
};

struct FMoundDynamicData
{
	UPROPERTY()
	USplineComponent* MoundSpline;
	UPROPERTY()
	UProceduralMeshComponent* MoundMesh;
	TArray<FVector> MoundVertices = TArray<FVector>();
	TArray<FVector2D> MoundUVs = TArray<FVector2D>();
	TArray<FVector> MoundNormals = TArray<FVector>();
	TArray<FProcMeshTangent> MoundTangents = TArray<FProcMeshTangent>();
	TArray<int32> MoundTriangles = TArray<int32>();

	int verts75 = 0;
	int verts25 = 0;
	float zOffset75 = 0.0f;
	float zOffset25 = 0.0f;
};

USTRUCT(BlueprintType)
struct FWallConfigData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	UStaticMesh* WallMesh1 = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* WallMesh2 = nullptr;
};

struct FWallDynamicData
{
	UPROPERTY()
	USplineComponent* WallSpline;
	TArray<USplineMeshComponent*> SplineMeshComponents;
	FVector SegmentStartLocation = FVector::ZeroVector;
	FVector SegmentEndLocation = FVector::ZeroVector;
	float MeshLength = 0.0f;
	float StartDistance = 0.0f;
	float EndDistance = 0.0f;
	int32 WallCounter = 0;
	int32 SplinePointCount = 0;
	int32 LogicalStart = 0;
};
	
USTRUCT(BlueprintType)
struct FPlantConfigData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMesh* PlantMesh = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMesh* PlantMesh1 = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMesh* PlantMesh2 = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMesh* PlantMesh3 = nullptr;

};

struct FPlantDynamicData
{
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PlantInstance = nullptr;
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PlantInstance1 = nullptr;
	TArray<FVector> ValidSpawnPoints;
	int32 InstanceCount;
	int32 LogicalStart;
};
	
USTRUCT(BlueprintType)
struct FRockConfigData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMesh* RockMesh = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMesh* RockMesh1 = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMesh* RockMesh2 = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMesh* RockMesh3 = nullptr;
};

struct FRockDynamicData
{
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* RockInstance = nullptr;
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* RockInstance1 = nullptr;
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* RockInstance2 = nullptr;
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* RockInstance3 = nullptr;
	TArray<FVector> ValidSpawnPoints;
	int32 InstanceCount;
	int32 LogicalStart;
};

