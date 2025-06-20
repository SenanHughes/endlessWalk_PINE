// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnAssets_CPP.h"

// Sets default values for this component's properties
USpawnAssets_CPP::USpawnAssets_CPP()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USpawnAssets_CPP::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USpawnAssets_CPP::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USpawnAssets_CPP::SpawnWall(const FWallConfigData& WallConfigData, FWallDynamicData& WallDynamicData, const FSplineConfigData& SplineConfigData)
{
	if (!WallDynamicData.WallSpline || !WallConfigData.WallMesh1 || !WallConfigData.WallMesh2) return;

	const float SplineLength = WallDynamicData.WallSpline->GetSplineLength();

	while (WallDynamicData.EndDistance < SplineLength - 400.0f)
	{
		// Randomly select between the two meshes
		bool WallType = FMath::RandBool();
		UStaticMesh* SelectedMesh = (WallType) ? WallConfigData.WallMesh1 : WallConfigData.WallMesh2;
		float SelectedMeshLength = SelectedMesh->GetBounds().BoxExtent.X * 2;

		if (WallDynamicData.StartDistance == 0.0f)
		{
			WallDynamicData.SplinePointCount = FMath::RoundToInt(SelectedMeshLength / SplineConfigData.PlaneDistance);
		}

		WallDynamicData.StartDistance = WallDynamicData.EndDistance - 10.0f;
		WallDynamicData.EndDistance = WallDynamicData.StartDistance + SelectedMeshLength;

		WallDynamicData.SegmentStartLocation = WallDynamicData.WallSpline->GetLocationAtDistanceAlongSpline(WallDynamicData.StartDistance, ESplineCoordinateSpace::World);
		WallDynamicData.SegmentEndLocation = WallDynamicData.WallSpline->GetLocationAtDistanceAlongSpline(WallDynamicData.EndDistance, ESplineCoordinateSpace::World);
		FVector StartTangent = WallDynamicData.WallSpline->GetTangentAtDistanceAlongSpline(WallDynamicData.StartDistance, ESplineCoordinateSpace::World);
		FVector EndTangent = WallDynamicData.WallSpline->GetTangentAtDistanceAlongSpline(WallDynamicData.EndDistance, ESplineCoordinateSpace::World);

		FRotator Rotation = FRotator::ZeroRotator;
		if (FMath::RandBool())
		{
			Rotation.Yaw = 180.0f;
		}

		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetStaticMesh(SelectedMesh);
		SplineMesh->SetStartAndEnd(WallDynamicData.SegmentStartLocation, StartTangent, WallDynamicData.SegmentEndLocation, EndTangent);
		SplineMesh->AttachToComponent(WallDynamicData.WallSpline, FAttachmentTransformRules::KeepWorldTransform);
		SplineMesh->RegisterComponent();
		SplineMesh->SetForwardAxis(ESplineMeshAxis::X);
		WallDynamicData.SplineMeshComponents.Add(SplineMesh);
	}
}

void USpawnAssets_CPP::SpawnAssetInstances(UHierarchicalInstancedStaticMeshComponent* HISM, USplineComponent* GuideSpline, UProceduralMeshComponent* Mesh, int ClusterSizeMin, int ClusterSizeMax, TArray<FVector>& ValidSpawnPoints, int32& InstanceCount)
{
	if (!HISM) return;

	// Clear existing instances
	HISM->ClearInstances();
	for (const FVector& Point : ValidSpawnPoints)
	{
		float NoiseVal = FMath::PerlinNoise2D(FVector2D(Point.X, Point.Y) * 0.1f);
		NoiseVal = (NoiseVal + 1.0f) / 2.0f; // Normalize to [0, 1]
		if (NoiseVal > 0.4f)
		{
			bool OriginalPoint = false;

			int ClusterSize = FMath::RandRange(ClusterSizeMin, ClusterSizeMax); // Random cluster size
			for (int32 i = 0; i < ClusterSize; i++)
			{
				int32 AttemptCount = 0;
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
					AttemptCount++;
					if (SlopeAngle > 0.0f)
					{
						TooSteep = false; // Exit the loop if the slope is not too steep
					}
					else if (AttemptCount > 10 && !OriginalPoint)
					{
						// If too steep after 10 attempts, use the original point
						InterpolatedPoint = Point;
						OriginalPoint = true;
					}
					else
					{
						TooSteep = false;
						InterpolatedPoint.Z = -1000.0f;
					}
				}

				FRotator Rotation = InterpolatedPoint.GetSafeNormal().Rotation();

				HISM->AddInstance(FTransform(Rotation, InterpolatedPoint, FVector(ScaleFactor)));
			}
		}
	}
	// Update the instance count
	InstanceCount = HISM->GetInstanceCount();
}