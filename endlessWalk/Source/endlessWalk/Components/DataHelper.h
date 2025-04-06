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
struct FPathData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* PathMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PathVertCount = 12;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PathWidth = 600;
	UPROPERTY()
	USplineComponent* PathSpline;
	UPROPERTY()
	UProceduralMeshComponent* PathMesh;
	TArray<FVector> PathVertices = TArray<FVector>();
	TArray<FVector> PathNormals = TArray<FVector>();
	TArray<FProcMeshTangent> PathTangents = TArray<FProcMeshTangent>();
	TArray<int32> PathTriangles = TArray<int32>();
};

USTRUCT(BlueprintType)
struct FRiverData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* RiverMaterial = nullptr;
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicRiverMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RiverVertCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int RiverWidth = 600;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int RiverDepth = 100;
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
struct FMoundData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* MoundMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MoundVertCount = 12;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MoundWidth = 600;
	UPROPERTY()
	USplineComponent* MoundSpline;
	UPROPERTY()
	UProceduralMeshComponent* MoundMesh;
	TArray<FVector> MoundVertices = TArray<FVector>();
	TArray<FVector> MoundNormals = TArray<FVector>();
	TArray<FProcMeshTangent> MoundTangents = TArray<FProcMeshTangent>();
	TArray<int32> MoundTriangles = TArray<int32>();
	float MoundHeight = 600.0f;
};

USTRUCT(BlueprintType)
struct FWallData
{
	GENERATED_BODY()

	UPROPERTY()
	USplineComponent* WallSpline;
	UPROPERTY(EditAnywhere)
	UStaticMesh* WallMesh1 = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* WallMesh2 = nullptr;
};

USTRUCT(BlueprintType)
struct FPlantData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMesh* PlantMesh = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMesh* PlantMesh2 = nullptr;
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PlantInstance = nullptr;
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PlantInstance2 = nullptr;
};

USTRUCT(BlueprintType)
struct FNoiseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float NoiseFrequency = 0.1f;
	UPROPERTY(EditAnywhere)
	float NoiseAmplitude = 15.0f;
	float Falloff = 0.0f;
	float SharpnessMultiplier = 50.0f;
	float FrequencyMultiplier = 0.003f;

	FastNoiseLite Noise;
};

USTRUCT(BlueprintType)
struct FSplineData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int SplinePoints = 200;
	UPROPERTY(EditAnywhere)
	int PlaneDistance = 100;
};

