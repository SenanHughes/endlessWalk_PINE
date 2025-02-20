// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "TerrainGenerator.generated.h"

// Forward Declarations
class USplineComponent;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class ENDLESSWALK_API ATerrainGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATerrainGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// The Spline to generate terrain along
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	USplineComponent* Spline;

	// The mesh to use for the terrain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	UStaticMesh* TerrainMesh;

	// Material to apply to terrain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	UMaterialInterface* TerrainMaterial;

	// Maximum length of the spline (10,000 units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int MaxSplineLength = 10000;

	// Frequency of terrain noise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseFrequency = 0.1f;

	// Amplitude of terrain noise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseAmplitude = 50.0f;

	// The distance at which to start adding new terrain planes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int PlaneDistance = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int CheckPoints = 2;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* TerrainMeshProc;

private:
	//Initialises the terrain mesh along the spline
	void InitialiseTerrainMesh();

	// Updates the terrain mesh along the spline based on the noise function
	void UpdateTerrainMesh();

	float CalculateSplineLength();

	// Generates a Perlin noise value for a given position along the terrain
	float GenerateNoise(float X, float Y);

	void GenerateMeshFromSpline();

	// Adds a new terrain plane at the correct position and deformation
	void AddTerrainPlane(int32 Index, FVector Position, FVector Scale, float Deformation);

	// List of terrain mesh components
	TArray<UStaticMeshComponent*> TerrainPlanes;

	// Spline length and index for plane management
	float CurrentSplineLength;
	int32 SplineIndex;
	bool booltest;
};
