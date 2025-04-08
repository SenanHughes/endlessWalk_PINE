// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataHelper.h"
#include "AssetHelperFuncs.h"
#include "UpdateAssets_CPP.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENDLESSWALK_API UUpdateAssets_CPP : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUpdateAssets_CPP();

	AssetHelperFuncs AssetHelper;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateWall(const FWallConfigData& WallConfigData, FWallDynamicData& WallDynamicData, const FSplineConfigData& SplineConfigData);

	void UpdateAssetInstances(UHierarchicalInstancedStaticMeshComponent* HISM, UHierarchicalInstancedStaticMeshComponent* HISM1, USplineComponent* GuideSpline, UProceduralMeshComponent* Mesh, int32 ClusterSizeMin, int32 ClusterSizeMax, TArray<FVector>& ValidSpawnPoints, int32 InstanceCount, int32& LogicalStart);
};
