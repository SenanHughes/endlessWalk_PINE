// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/DataHelper.h"
#include "Components/MeshHelperFuncs_CPP.h"
#include "Components/ProcMeshGeneration_CPP.h"
#include "Components/SpawnAssets_CPP.h"
#include "Components/ProcMeshUpdates_CPP.h"
#include "Components/UpdateAssets_CPP.h"
#include "ProceduralLevelGenerator_CPP.generated.h"

UCLASS()
class ENDLESSWALK_API AProceduralLevelGenerator_CPP : public AActor
{
	GENERATED_BODY()

public:
	AProceduralLevelGenerator_CPP(const FObjectInitializer& ObjectInitializer);

	//** Sphere Trigger */
	UPROPERTY()
	USphereComponent* SphereTrigger;

	//** Character Variables for Direction Containment */
	FVector CharacterLocation = FVector::ZeroVector;
	APawn* CharacterPawn;

	//** Data Helper Variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings", meta = (DisplayName = "Spline Parameters"))
	FSplineConfigData SplineConfigData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings", meta = (DisplayName = "Noise Parameters"))
	FNoiseConfigData NoiseConfigData;
	FNoiseDynamicData NoiseDynamicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings", meta = (DisplayName = "Path Parameters"))
	FPathConfigData PathConfigData;
	FPathDynamicData PathDynamicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings", meta = (DisplayName = "River Parameters"))
	FRiverConfigData RiverConfigData;
	FRiverDynamicData RiverDynamicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings", meta = (DisplayName = "Mound Parameters"))
	FMoundConfigData MoundConfigData;
	FMoundDynamicData MoundDynamicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings", meta = (DisplayName = "Wall Parameters"))
	FWallConfigData WallConfigData;
	FWallDynamicData WallDynamicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Settings", meta = (DisplayName = "Plant Parameters"))
	FPlantConfigData PlantConfigData;

	FPlantDynamicData PlantDynamicData;
	FPlantDynamicData PlantDynamicData1;

	// Spline Containment Variables
	FVector SphereCenter;
	float SphereRadius = 0.0f;
	float DistanceToCenter = 0.0f;
	float EdgeThreshold = 0.0f;
	float PointCount = 0;
	bool ChangeCurve = false;
	bool validPoint = false;
	bool AvoidingEdge = false;

	float NoiseValue = 0.0f;
	float offsetCalc = 0.0f;
	float depthOffset = 0.0f;

	AssetHelperFuncs AssetHelper;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsPointInsideSphere(FVector Point, FVector SphereCenter, float SphereRadius);

	void LimitMovementSmoothly(USplineComponent* GuideSpline, float MaxDistance, float PullSpeed);

	void UpdateTerrainSpline();

	UProcMeshGeneration_CPP* MeshGeneration;
	UProcMeshUpdates_CPP* MeshUpdates;

	USpawnAssets_CPP* SpawnAssets;
	UUpdateAssets_CPP* UpdateAssets;
};
