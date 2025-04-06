// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "dataHelper.h"
#include "ProcMeshHelperFuncs.h"
#include "ProcMeshUpdates_CPP.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENDLESSWALK_API UProcMeshUpdates_CPP : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProcMeshUpdates_CPP();

	// Generic uProcMesh Variables
	FVector MeshVert = FVector::ZeroVector;
	FVector MeshLeftEdge = FVector::ZeroVector;
	FVector MeshSplinePoint = FVector::ZeroVector;
	FVector MeshSplineTangent = FVector::ZeroVector;
	FVector MeshEdgeVector = FVector::ZeroVector;

	ProcMeshHelperFuncs MeshHelperFuncs;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdatePathMesh(const FPathConfigData& PathConfigData, FPathDynamicData& PathDynamicData, const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData,
		const FWallConfigData& WallConfigData, FWallDynamicData& WallDynamicData, const FNoiseConfigData& NoiseConfigData, FNoiseDynamicData& NoiseDynamicData, const FSplineConfigData SplineConfigData);

	void UpdateRiverMesh(const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData, const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData, const FSplineConfigData SplineConfigData);

	void UpdateMoundMesh(const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData, const FRiverConfigData& RiverConfigData, const FNoiseConfigData& NoiseConfigData, FNoiseDynamicData& NoiseDynamicData, const FSplineConfigData SplineConfigData);
};