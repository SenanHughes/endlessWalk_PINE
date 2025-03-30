// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "FastNoiseLite.h"
#include "Engine/StaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ProceduralLevelGenerator_CPP.generated.h"

UCLASS()
class ENDLESSWALK_API AProceduralLevelGenerator_CPP : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProceduralLevelGenerator_CPP();

	// The Spline to generate terrain along
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	USplineComponent* PathSpline;

	// The Spline to generate river along
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "River")
	USplineComponent* RiverSpline;

	// The Spline to generate mounds along
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mound")
	USplineComponent* MoundSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* PathMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* RiverMaterial = nullptr;

	UPROPERTY();
	UMaterialInstanceDynamic* DynamicRiverMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* MoundMaterial = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	UProceduralMeshComponent* PathMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	UProceduralMeshComponent* RiverMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	UProceduralMeshComponent* MoundMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boundary")
	USphereComponent* SphereTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "River")
	int RiverDepth = 100;

	// Spline Steps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int SplinePoints = 200;

	//Triangle horizontal vert count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int PathVertCount = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "River")
	int RiverVertCount = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mound")
	int MoundVertCount = 9;

	// Frequency of terrain noise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseFrequency = 0.1f;

	// Amplitude of terrain noise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseAmplitude = 15.0f;

	// The distance at which to start adding new terrain planes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int PlaneDistance = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	int PathWidth = 600;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "River")
	int RiverWidth = 600;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mound")
	int MoundWidth = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int PathUVScale = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "River")
	int RiverUVScale = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mound")
	int MoundUVScale = 1000;


	/** Static Mesh to Spawn */
	UPROPERTY(EditAnywhere, Category = "Procedural")
	UStaticMesh* AssetToSpawn = nullptr;

	UPROPERTY(EditAnywhere, Category = "Procedural")
	UStaticMesh* AssetToSpawn2 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Procedural")
	UInstancedStaticMeshComponent* InstancedMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Procedural")
	UInstancedStaticMeshComponent* InstancedMesh2 = nullptr;

	/** Number of assets to spawn */
	UPROPERTY(EditAnywhere, Category = "Procedural", meta = (ClampMin = "1", UIMin = "1"))
	int32 NumAssetsToSpawn;

	bool bInitialised = false;

	bool validPoint = false;
	FVector SphereCenter;
	float SphereRadius = 0.0f;
	float DistanceToCenter = 0.0f;
	float EdgeThreshold = 0.0f;

	TArray<FVector> PathVertices = TArray<FVector>();
	TArray<FVector> PathNormals = TArray<FVector>();
	TArray<FProcMeshTangent> PathTangents = TArray<FProcMeshTangent>();
	TArray<FVector> NewPathVerts = TArray<FVector>();
	TArray<int32> PathTriangles = TArray<int32>();

	TArray<FVector> RiverVertices = TArray<FVector>();
	TArray<FVector> RiverNormals = TArray<FVector>();
	TArray<FProcMeshTangent> RiverTangents = TArray<FProcMeshTangent>();
	TArray<FVector> RiverTangentsArray = TArray<FVector>();
	TArray<int32> RiverTriangles = TArray<int32>();
	TArray<FVector> NoisyRiverEdge = TArray<FVector>();

	TArray<FVector> MoundVertices = TArray<FVector>();
	TArray<FVector> MoundNormals = TArray<FVector>();
	TArray<FProcMeshTangent> MoundTangents = TArray<FProcMeshTangent>();
	TArray<FVector> NewMoundVerts = TArray<FVector>();
	TArray<int32> MoundTriangles = TArray<int32>();
	TArray<FVector> NoisyMoundEdge = TArray<FVector>();


	FVector MeshVert = FVector::ZeroVector;
	FVector MeshPathEdge = FVector::ZeroVector;
	FVector MeshLeftEdge = FVector::ZeroVector;
	FVector MeshSplinePoint = FVector::ZeroVector;
	FVector MeshSplineTangent = FVector::ZeroVector;
	FVector MeshEdgeVector = FVector::ZeroVector;

	float NoiseValue = 0.0f;
	float offsetCalc = 0.0f;
	float depthOffset = 0.0f;
	float MeshVertU = 0.0f;
	float MeshVertV = 0.0f;

	TArray<FVector> PlacedLocations;
	FVector SpawnLocation = FVector::ZeroVector;

	int32 AttemptCount = 0;
	int32 MaxAttempts = 10;
	FVector OverlapCheck = FVector::ZeroVector;
	FVector InstanceScale = FVector::ZeroVector;
	UInstancedStaticMeshComponent* Instance;
	bool bValidLocation = false;
	float randCheck = 0.0f;

	bool ChangeCurve = false;
	float PointCount = 0;

	FastNoiseLite Noise;
	float Falloff = 0.0f;
	float zHeight = 600.0f;

	float SharpnessMultiplier = 50.0f;
	float FrequencyMultiplier = 0.003f;

	FVector CharacterLocation = FVector::ZeroVector;
	APawn* CharacterPawn;

	float CurrentRotation = 0.0f;

	FVector TargetPannerDirection = FVector::ZeroVector;
	FVector CurrentPannerDirection = FVector::ZeroVector;
	float TimeLimit = 0.5f;

	TArray<int32> PathMeshTriangles;
	TArray<int32> RiverMeshTriangles;
	TArray<int32> MoundMeshTriangles;

	float TimeValue;
	TArray<FLinearColor> VertexColours;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateMaterialPanner(float DeltaTime);

	void LimitMovementSmoothly(USplineComponent* GuideSpline, float MaxDistance, float PullSpeed);

	void UpdateTerrainSpline();

	void TriangleCalcs(TArray<FVector>& MeshVertices, TArray<int32>& MeshTriangles, int MeshVertCount, int i);

	void NormalCalcs(TArray<FVector>& MeshVertices, TArray<FVector>& MeshNormals, int MeshVertCount, int i);

	bool IsPointInsideSphere(FVector Point, FVector SphereCenter, float SphereRadius);

	void SplineRegulator(USplineComponent* GuideSpline);

	void GeneratePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int PathVertCount);

	FVector PathMeshHelper(int y, int MeshWidth, FVector& MeshEdge);

	FVector MoundMeshHelper(int y, int MeshWidth, float& zValue, float zOffset, int i);

	void UpdatePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int PathVertCount);

	void GenerateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int RiverVertCount);

	void UpdateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int RiverVertCount);

	void GenerateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int MoundVertCount);

	void UpdateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int MoundVertCount);

	void SpawnProceduralAssets();
	void UpdateProceduralAssets();
	UTexture2D* GenerateFlowMapTexture();
};
