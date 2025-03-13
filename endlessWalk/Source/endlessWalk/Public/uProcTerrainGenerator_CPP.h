// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/SplineComponent.h"
#include "FastNoiseLite.h"
#include "Engine/StaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "uProcTerrainGenerator_CPP.generated.h"

// Forward Declarations
class USplineComponent;
class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class ENDLESSWALK_API AuProcTerrainGenerator_CPP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AuProcTerrainGenerator_CPP();

	// The Spline to generate terrain along
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	USplineComponent* PathSpline;

	// The Spline to generate river along
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "River")
	USplineComponent* RiverSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* PathMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* RiverMaterial = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	UProceduralMeshComponent* PathMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	UProceduralMeshComponent* RiverMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "River")
	int RiverOffset = 400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "River")
	int RiverDepth = 100;

	// Maximum length of the spline (10,000 units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int MaxSplineLength = 10000;

	// Spline Steps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int SplinePoints = 100;

	//Triangle horizontal vert count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int VertCount = 9;

	// Required noise check
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	bool PathNoiseBool = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	bool RiverNoiseBool = false;

	// Frequency of terrain noise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseFrequency = 0.1f;

	// Amplitude of terrain noise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseAmplitude = 15.0f;

	// The distance at which to start adding new terrain planes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int PlaneDistance = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int CheckPoints = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	float DeviationThreshold = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	int PathWidth = 400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "River")
	int RiverWidth = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int PathUVScale = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "River")
	int RiverUVScale = 1000;



	/** Static Mesh to Spawn */
	UPROPERTY(EditAnywhere, Category = "Procedural")
	UStaticMesh* AssetToSpawn;

	UPROPERTY(EditAnywhere, Category = "Procedural")
	UStaticMesh* AssetToSpawn2;

	UPROPERTY(VisibleAnywhere, Category = "Procedural")
	UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(VisibleAnywhere, Category = "Procedural")
	UInstancedStaticMeshComponent* InstancedMesh2;

	/** Number of assets to spawn */
	UPROPERTY(EditAnywhere, Category = "Procedural", meta = (ClampMin = "1", UIMin = "1"))
	int32 NumAssetsToSpawn;




	TArray<FVector> RiverSplinePoints;

	TArray<FVector> PathVertices;
	TArray<FVector2D> PathUVs;
	TArray<int32> PathTriangles;

	TArray<FVector> RiverVertices;
	TArray<FVector2D> RiverUVs;
	TArray<int32> RiverTriangles;

	bool PathMeshInitiated = false;
	bool RiverMeshInitiated = false;

	FVector MeshVert;
	FVector MeshLeftEdge;
	FVector MeshSplinePoint;
	FVector MeshSplineTangent;
	FVector MeshEdgeVector;

	float NoiseValue;
	float offsetCalc;
	float MeshVertU;
	float MeshVertV;

	FastNoiseLite Noise;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GenerateProcMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector> &MeshVertices, 
		TArray<FVector2D> &MeshUVs, TArray<int32> &MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, bool& MeshInitiated);

	void UpdateProcMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial);
	
	void UpdateTerrainSpline();
	void CreateRiverSpline();
	void UpdateRiverSpline();

	void SpawnProceduralAssets();
};
