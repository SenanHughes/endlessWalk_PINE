// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateAssets_CPP.h"

// Sets default values for this component's properties
UUpdateAssets_CPP::UUpdateAssets_CPP()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUpdateAssets_CPP::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUpdateAssets_CPP::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UUpdateAssets_CPP::UpdateWall(const FWallConfigData& WallConfigData, FWallDynamicData& WallDynamicData, const FSplineConfigData& SplineConfigData)
{
	if (WallDynamicData.SplineMeshComponents.Num() == 0) return;

	// Randomly select between the two meshes
	bool WallType = FMath::RandBool();
	UStaticMesh* SelectedMesh = (WallType) ? WallConfigData.WallMesh1 : WallConfigData.WallMesh2;
	float SelectedMeshLength = WallDynamicData.SplineMeshComponents[WallDynamicData.LogicalStart]->GetStaticMesh()->GetBounds().BoxExtent.X * 2.0f;



	// Calculate the new start and end positions and tangents for the repositioned component

	WallDynamicData.StartDistance = WallDynamicData.WallSpline->GetDistanceAlongSplineAtLocation(WallDynamicData.SegmentEndLocation, ESplineCoordinateSpace::World) - 10.0f;
	WallDynamicData.EndDistance = WallDynamicData.StartDistance + SelectedMeshLength;

	WallDynamicData.SegmentStartLocation = WallDynamicData.WallSpline->GetLocationAtDistanceAlongSpline(WallDynamicData.StartDistance, ESplineCoordinateSpace::World);
	WallDynamicData.SegmentEndLocation = WallDynamicData.WallSpline->GetLocationAtDistanceAlongSpline(WallDynamicData.EndDistance, ESplineCoordinateSpace::World);
	FVector StartTangent = WallDynamicData.WallSpline->GetTangentAtDistanceAlongSpline(WallDynamicData.StartDistance, ESplineCoordinateSpace::World);
	FVector EndTangent = WallDynamicData.WallSpline->GetTangentAtDistanceAlongSpline(WallDynamicData.EndDistance, ESplineCoordinateSpace::World);

	// Add the removed component to the end of the array
	WallDynamicData.SplineMeshComponents[WallDynamicData.LogicalStart]->SetStaticMesh(SelectedMesh);
	WallDynamicData.SplineMeshComponents[WallDynamicData.LogicalStart]->SetStartAndEnd(WallDynamicData.SegmentStartLocation, StartTangent, WallDynamicData.SegmentEndLocation, EndTangent);
	WallDynamicData.SplineMeshComponents[WallDynamicData.LogicalStart]->UpdateMesh();
	WallDynamicData.LogicalStart++;
	if (WallDynamicData.LogicalStart >= WallDynamicData.SplineMeshComponents.Num())
	{
		WallDynamicData.LogicalStart = 0; // Reset to the beginning if we reach the end
	}

	float FirstMeshLength = WallDynamicData.SplineMeshComponents[WallDynamicData.LogicalStart]->GetStaticMesh()->GetBounds().BoxExtent.X * 2.0f;
	WallDynamicData.SplinePointCount = FMath::RoundToInt(SelectedMeshLength / SplineConfigData.PlaneDistance);
}

void UUpdateAssets_CPP::UpdateAssetInstances(UHierarchicalInstancedStaticMeshComponent* HISM, USplineComponent* GuideSpline, UProceduralMeshComponent* Mesh, int32 ClusterSizeMin, int32 ClusterSizeMax, TArray<FVector>& ValidSpawnPoints, int32 InstanceCount, int32& LogicalStart)
{
	if (!HISM || !GuideSpline || !Mesh) return;
	int ClusterSize = 0;
	for (int i = LogicalStart % InstanceCount; i < (LogicalStart % InstanceCount) + ClusterSizeMax; i++)
	{
		FTransform InstanceLocation;
		HISM->GetInstanceTransform(i, InstanceLocation, false);
		if (FVector::Dist(InstanceLocation.GetLocation(), GuideSpline->GetLocationAtSplinePoint(GuideSpline->GetNumberOfSplinePoints() / 2, ESplineCoordinateSpace::Local)) >
			FVector::Dist(GuideSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local), GuideSpline->GetLocationAtSplinePoint(GuideSpline->GetNumberOfSplinePoints() / 2, ESplineCoordinateSpace::Local)))
		{
			ClusterSize++;
		}
	}

	for (const FVector& Point : ValidSpawnPoints)
	{
		float NoiseVal = FMath::PerlinNoise2D(FVector2D(Point.X, Point.Y) * 0.1f);
		NoiseVal = (NoiseVal + 1.0f) / 2.0f; // Normalize to [0, 1]
		if (NoiseVal > 0.4f)
		{
			//int ClusterSize = FMath::RandRange(ClusterSizeMin, ClusterSizeMax); // Random cluster size
			for (int32 i = 0; i < ClusterSize; i++)
			{
				bool TooSteep = true;
				FVector InterpolatedPoint = FVector::ZeroVector;
				float ScaleFactor = 0.0f;
				// Generate random offset within a small radius around the point
				while (TooSteep)
				{
					FVector Offset = FVector(
						FMath::RandRange(-50.0f, 50.0f),
						FMath::RandRange(-50.0f, 50.0f),
						0.0f
					);

					FVector ClusterPoint = Point + Offset;
					ScaleFactor = FMath::Lerp(0.6f, 1.2f, FMath::RandRange(0.0f, 1.0f));

					// Interpolate the height based on the surrounding vertices
					InterpolatedPoint = AssetHelper.InterpolateHeight(ClusterPoint, Mesh);

					FVector Normal = (InterpolatedPoint - ClusterPoint).GetSafeNormal();
					float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Normal, FVector::UpVector)));
					if (SlopeAngle < 25.0f)
					{
						TooSteep = false; // Exit the loop if the slope is not too steep
					}
				}

				FRotator Rotation = InterpolatedPoint.GetSafeNormal().Rotation();

				int32 InstanceIndex = LogicalStart % InstanceCount;

				// Spawn the instance
				HISM->UpdateInstanceTransform(InstanceIndex, FTransform(Rotation, InterpolatedPoint, FVector(ScaleFactor)), false, true, false);

				// Increment the logical start index
				LogicalStart++;
				if (LogicalStart >= InstanceCount)
				{
					LogicalStart = 0; // Reset to the beginning if we reach the end
				}
			}
		}
	}
	// Update the HISM to reflect the changes
	HISM->MarkRenderStateDirty();
}