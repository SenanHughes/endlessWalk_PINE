// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/SplineComponent.h"
#include "uProcTerrainGenerator_CPP.generated.h"

// Forward Declarations
class USplineComponent;

UCLASS()
class ENDLESSWALK_API AuProcTerrainGenerator_CPP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AuProcTerrainGenerator_CPP();

	// The Spline to generate terrain along
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	USplineComponent* Spline;

	// Maximum length of the spline (10,000 units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int MaxSplineLength = 10000;

	// Spline Steps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int SplinePoints = 100;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	int PathWidth = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int UVScale = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* AssignedMaterial;

	TArray<FVector> Vertices;
	TArray<FVector2D> UVs;
	TArray<int32> Triangles;
	float DynamicSplineLength;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	UProceduralMeshComponent* PathMesh;

	bool initialiseUVs = false;
	
	void GeneratePathMesh();
	void UpdateTerrainSpline();


};
