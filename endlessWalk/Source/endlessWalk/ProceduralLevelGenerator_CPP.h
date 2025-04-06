// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Components/FastNoiseLite.h"
//#include "Components/ProcMeshGeneration_CPP.h"
#include "Components/DataHelper.h"
#include "ProceduralLevelGenerator_CPP.generated.h"

UCLASS()
class ENDLESSWALK_API AProceduralLevelGenerator_CPP : public AActor
{
	GENERATED_BODY()

public:
	AProceduralLevelGenerator_CPP();

	//** Sphere Trigger */
	UPROPERTY()
	USphereComponent* SphereTrigger;

	//** Character Variables for Direction Containment */
	FVector CharacterLocation = FVector::ZeroVector;
	APawn* CharacterPawn;

	//** Data Helper Variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings")
	FPathData PathData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings")
	FRiverData RiverData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings")
	FMoundData MoundData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings")
	FSplineData SplineData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings")
	FWallData WallData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings")
	FNoiseData NoiseData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings")
	FPlantData PlantData;


	/** Static Mesh to Spawn
	UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Walls")
	UStaticMesh* WallMesh1 = nullptr;

	UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Walls")
	UStaticMesh* WallMesh2 = nullptr;

	UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Plants")
	UStaticMesh* PlantMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Plants")
	UStaticMesh* PlantMesh2 = nullptr;*/




	//UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Misc")
	//UStaticMesh* AssetToSpawn5 = nullptr;

	//UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Misc")
	//UStaticMesh* AssetToSpawn6 = nullptr;

	//UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Misc")
	//UStaticMesh* AssetToSpawn7 = nullptr;

	//UPROPERTY(EditAnywhere, Category = "03 ProceduralAssets | Misc")
	//UStaticMesh* AssetToSpawn8 = nullptr;


	//** Plant Asset Variables 
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PlantInstance = nullptr;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PlantInstance2 = nullptr;



	// Wall SplineMesh Variables
	TArray<USplineMeshComponent*> SplineMeshComponents;
	FVector SegmentStartLocation = FVector::ZeroVector;
	FVector SegmentEndLocation = FVector::ZeroVector;
	float MeshLength = 0.0f;
	float StartDistance = 0.0f;
	float EndDistance = 0.0f;
	int32 WallCounter = 0;
	int32 SplinePointCount = 0;


	// Spline Containment Variables
	FVector SphereCenter;
	float SphereRadius = 0.0f;
	float DistanceToCenter = 0.0f;
	float EdgeThreshold = 0.0f;
	float PointCount = 0;
	bool ChangeCurve = false;
	bool validPoint = false;

	float NoiseValue = 0.0f;
	float offsetCalc = 0.0f;
	float depthOffset = 0.0f;

	// Generic uProcMesh Variables
	FVector MeshVert = FVector::ZeroVector;
	FVector MeshLeftEdge = FVector::ZeroVector;
	FVector MeshSplinePoint = FVector::ZeroVector;
	FVector MeshSplineTangent = FVector::ZeroVector;
	FVector MeshEdgeVector = FVector::ZeroVector;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LimitMovementSmoothly(USplineComponent* GuideSpline, float MaxDistance, float PullSpeed);

	void UpdateTerrainSpline();

	void TriangleCalcs(TArray<FVector>& MeshVertices, TArray<int32>& MeshTriangles, int MeshVertCount, int i);

	void NormalCalcs(TArray<FVector>& MeshVertices, TArray<FVector>& MeshNormals, int MeshVertCount, int i);

	bool IsPointInsideSphere(FVector Point, FVector SphereCenter, float SphereRadius);

	void SplineRegulator(USplineComponent* GuideSpline);

	FVector PathMeshHelper(int y, int MeshWidth, FVector& MeshEdge);

	FVector MoundMeshHelper(int y, int MeshWidth, float& zValue, float zOffset, int i);

	void GeneratePathMesh(const FPathData& PathDataStruct);

	void UpdatePathMesh(const FPathData& PathDataStruct);

	void GenerateRiverMesh(const FRiverData& RiverDataStruct);

	void UpdateRiverMesh(const FRiverData& RiverDataStruct);

	void GenerateMoundMesh(const FMoundData& MoundDataStruct);

	void UpdateMoundMesh(const FMoundData& MoundDataStruct);

	void SpawnWall(USplineComponent* GuideSpline);

	void UpdateWall(USplineComponent* GuideSpline);

	TArray<FVector> GetValidSpawnPoints(UProceduralMeshComponent* Mesh, USplineComponent* GuideSpline);
	void SpawnAssetInstances(UHierarchicalInstancedStaticMeshComponent* HISM, const TArray<FVector>& SpawnPoints, UProceduralMeshComponent* Mesh, int ClusterSizeMin, int ClusterSizeMax);
	FVector InterpolateHeight(const FVector& Point, UProceduralMeshComponent* Mesh);
	bool PointInTriangle2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, const FVector2D& C);	
};
