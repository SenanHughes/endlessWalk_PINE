// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralLevelGenerator_CPP.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProceduralLevelGenerator_CPP::AProceduralLevelGenerator_CPP(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the sphere trigger
	SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
	RootComponent = SphereTrigger;
	SphereTrigger->InitSphereRadius(40000.0f);
	SphereTrigger->SetMobility(EComponentMobility::Static);

	// Create the Path Components
	PathDynamicData.PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathDynamicData.PathSpline->SetupAttachment(RootComponent);
	PathDynamicData.PathSpline->SetCanEverAffectNavigation(false);
	
	PathDynamicData.PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathDynamicData.PathMesh->SetupAttachment(RootComponent);
	PathDynamicData.PathMesh->SetCullDistance(9000.0f);
	PathDynamicData.PathMesh->SetCanEverAffectNavigation(false);
	PathDynamicData.PathMesh->bUseAsyncCooking = true;

	// Create the River Components
	RiverDynamicData.RiverSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RiverSpline"));
	RiverDynamicData.RiverSpline->SetupAttachment(RootComponent);
	RiverDynamicData.RiverSpline->SetCanEverAffectNavigation(false);

	RiverDynamicData.RiverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));
	RiverDynamicData.RiverMesh->SetupAttachment(RootComponent);
	RiverDynamicData.RiverMesh->SetCullDistance(9000.0f);
	RiverDynamicData.RiverMesh->SetCanEverAffectNavigation(false);
	RiverDynamicData.RiverMesh->bUseAsyncCooking = true;

	// Create the Mound Components
	MoundDynamicData.MoundSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoundSpline"));
	MoundDynamicData.MoundSpline->SetupAttachment(RootComponent);
	MoundDynamicData.MoundSpline->SetCanEverAffectNavigation(false);

	MoundDynamicData.MoundMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MoundMesh"));
	MoundDynamicData.MoundMesh->SetupAttachment(RootComponent);
	MoundDynamicData.MoundMesh->SetCullDistance(9000.0f);
	MoundDynamicData.MoundMesh->SetCanEverAffectNavigation(false);
	MoundDynamicData.MoundMesh->bUseAsyncCooking = true;

	// Create the Wall Spline
	WallDynamicData.WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	WallDynamicData.WallSpline->SetupAttachment(RootComponent);
	WallDynamicData.WallSpline->SetCanEverAffectNavigation(false);

	// Create the Plant Instances
	PlantDynamicData.PlantInstance = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedMesh"));
	PlantDynamicData.PlantInstance->SetMobility(EComponentMobility::Movable);
	PlantDynamicData.PlantInstance->SetupAttachment(RootComponent);
	PlantDynamicData.PlantInstance1 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedMesh1"));
	PlantDynamicData.PlantInstance1->SetMobility(EComponentMobility::Movable);
	PlantDynamicData.PlantInstance1->SetupAttachment(RootComponent);

	PlantDynamicData1.PlantInstance = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedMesh2"));
	PlantDynamicData1.PlantInstance->SetMobility(EComponentMobility::Movable);
	PlantDynamicData1.PlantInstance->SetupAttachment(RootComponent);
	PlantDynamicData1.PlantInstance1 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedMesh3"));
	PlantDynamicData1.PlantInstance1->SetMobility(EComponentMobility::Movable);
	PlantDynamicData1.PlantInstance1->SetupAttachment(RootComponent);

	MeshGeneration = ObjectInitializer.CreateDefaultSubobject<UProcMeshGeneration_CPP>(this, TEXT("MeshGeneration"));
	MeshGeneration->RegisterComponent();

	MeshUpdates = ObjectInitializer.CreateDefaultSubobject<UProcMeshUpdates_CPP>(this, TEXT("MeshUpdates"));
	MeshUpdates->RegisterComponent();

	SpawnAssets = ObjectInitializer.CreateDefaultSubobject<USpawnAssets_CPP>(this, TEXT("SpawnAssets"));
	SpawnAssets->RegisterComponent();

	UpdateAssets = ObjectInitializer.CreateDefaultSubobject<UUpdateAssets_CPP>(this, TEXT("UpdateAssets"));
	UpdateAssets->RegisterComponent();
}

// Called when the game starts or when spawned
void AProceduralLevelGenerator_CPP::BeginPlay()
{
	Super::BeginPlay();



	// Ensure the spline is correctly set up
	if (PathDynamicData.PathSpline)
	{
		RiverDynamicData.DynamicRiverMaterial = UMaterialInstanceDynamic::Create(RiverConfigData.RiverMaterial, this);

		// Clear any previous spline points
		PathDynamicData.PathSpline->ClearSplinePoints();
		WallDynamicData.WallSpline->ClearSplinePoints();
		RiverDynamicData.RiverSpline->ClearSplinePoints();
		MoundDynamicData.MoundSpline->ClearSplinePoints();

		NoiseDynamicData.Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		NoiseDynamicData.Noise.SetFrequency(NoiseConfigData.NoiseFrequency);

		float AmplitudeScale = FMath::RandRange(1.0f, 2.5f);  // Randomly scale wave size
		float FrequencyScale = FMath::RandRange(0.8f, 1.2f);  // Random slight frequency variation

		for (int32 i = 0; i < SplineConfigData.SplinePoints; i++)
		{
			float SinWaveY = FMath::Sin((i - ((SplineConfigData.SplinePoints / 2) - 10)) * (PI / ((SplineConfigData.SplinePoints / 2) - 1)) * FrequencyScale) * (100.0f * AmplitudeScale);
			FVector NewPoint(
				((i - ((SplineConfigData.SplinePoints / 2) - 10)) * SplineConfigData.PlaneDistance),  // X position
				SinWaveY,  // Adjusted sinusoidal Y with randomness
				0.0f       // Keep Z the same (flat for now)
			);

			PathDynamicData.PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);
		}

		PathDynamicData.PathSpline->UpdateSpline();
	}

	// Set up the sphere trigger
	SphereCenter = SphereTrigger->GetComponentLocation();
	SphereRadius = SphereTrigger->GetScaledSphereRadius();
	DistanceToCenter = 0.0f;
	EdgeThreshold = SphereRadius * 0.75f;

	// Spawn Procedural Meshes
	MeshGeneration->GeneratePathMesh(PathConfigData, PathDynamicData, RiverConfigData, RiverDynamicData, WallConfigData, WallDynamicData, NoiseConfigData, NoiseDynamicData, SplineConfigData);
	MeshGeneration->GenerateRiverMesh(RiverConfigData, RiverDynamicData, MoundConfigData, MoundDynamicData, SplineConfigData);
	MeshGeneration->GenerateMoundMesh(MoundConfigData, MoundDynamicData, RiverConfigData, NoiseConfigData, NoiseDynamicData, SplineConfigData);


	// Spawn the wall
	SpawnAssets->SpawnWall(WallConfigData, WallDynamicData, SplineConfigData);

	// Spawn the plants
	TArray<UStaticMesh*> PlantMeshes = {
		PlantConfigData.PlantMesh,
		PlantConfigData.PlantMesh1,
		PlantConfigData.PlantMesh2,
		PlantConfigData.PlantMesh3
	};

	for (int32 i = PlantMeshes.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		PlantMeshes.Swap(i, j);
	}

	PlantDynamicData.PlantInstance->SetStaticMesh(PlantMeshes[0]);
	PlantDynamicData1.PlantInstance->SetStaticMesh(PlantMeshes[1]);

	/**for (int32 i = PlantMeshes.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		PlantMeshes.Swap(i, j);
	}

	PlantDynamicData1.PlantInstance->SetStaticMesh(PlantMeshes[0]);
	PlantDynamicData1.PlantInstance1->SetStaticMesh(PlantMeshes[1]);*/


	PlantDynamicData.ValidSpawnPoints = AssetHelper.GetValidSpawnPoints(PathDynamicData.PathMesh, PathDynamicData.PathSpline, PathConfigData.PathVertCount, 0, PathDynamicData.PathVertices.Num());
	PlantDynamicData1.ValidSpawnPoints = AssetHelper.GetValidSpawnPoints(MoundDynamicData.MoundMesh, MoundDynamicData.MoundSpline, MoundConfigData.MoundVertCount, 0, MoundDynamicData.MoundVertices.Num());


	SpawnAssets->SpawnAssetInstances(PlantDynamicData.PlantInstance, PlantDynamicData.PlantInstance1, MoundDynamicData.MoundSpline, MoundDynamicData.MoundMesh, 2, 5, PlantDynamicData.ValidSpawnPoints, PlantDynamicData.InstanceCount);
	SpawnAssets->SpawnAssetInstances(PlantDynamicData1.PlantInstance, PlantDynamicData1.PlantInstance1, MoundDynamicData.MoundSpline, MoundDynamicData.MoundMesh, 4, 7, PlantDynamicData1.ValidSpawnPoints, PlantDynamicData1.InstanceCount);

	// Spawn the rocks
	/**RockDynamicData.RockInstance->SetStaticMesh(RockConfigData.RockMesh);
	RockDynamicData.RockInstance1->SetStaticMesh(RockConfigData.RockMesh1);
	RockDynamicData.RockInstance2->SetStaticMesh(RockConfigData.RockMesh2);
	RockDynamicData.RockInstance3->SetStaticMesh(RockConfigData.RockMesh3);


	RockDynamicData.ValidSpawnPoints = AssetHelper.GetValidSpawnPoints(PathDynamicData.PathMesh, PathDynamicData.PathSpline, PathConfigData.PathVertCount, 0, PathDynamicData.PathVertices.Num(), RockDynamicData.RockInstance);
	SpawnAssets->SpawnAssetInstances(RockDynamicData.RockInstance, PathDynamicData.PathSpline, PathDynamicData.PathMesh, 3, 6, RockDynamicData.ValidSpawnPoints, RockDynamicData.InstanceCount);*/
}

// Called every frame
void AProceduralLevelGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CharacterPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	CharacterLocation = CharacterPawn->GetActorLocation();
	FVector MidPoint = PathDynamicData.PathSpline->GetLocationAtSplinePoint(SplineConfigData.SplinePoints / 2, ESplineCoordinateSpace::Local);

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathConfigData.PathWidth / 2)
	{
		PointCount += 1.0f;
		if (PointCount >= 40.0f)
		{
			ChangeCurve = true;
		}
		UpdateTerrainSpline();

		PlantDynamicData.ValidSpawnPoints = AssetHelper.GetValidSpawnPoints(PathDynamicData.PathMesh, PathDynamicData.PathSpline, PathConfigData.PathVertCount,	(PathDynamicData.PathVertices.Num() - (PathConfigData.PathVertCount + 1)), PathDynamicData.PathVertices.Num());
		UpdateAssets->UpdateAssetInstances(PlantDynamicData.PlantInstance,PlantDynamicData.PlantInstance1, PathDynamicData.PathSpline, PathDynamicData.PathMesh, 3, 6, PlantDynamicData.ValidSpawnPoints, PlantDynamicData.InstanceCount, PlantDynamicData.LogicalStart);

		PlantDynamicData1.ValidSpawnPoints = AssetHelper.GetValidSpawnPoints(MoundDynamicData.MoundMesh, MoundDynamicData.MoundSpline, MoundConfigData.MoundVertCount,	(MoundDynamicData.MoundVertices.Num() - (MoundConfigData.MoundVertCount + 1)), MoundDynamicData.MoundVertices.Num());
		UpdateAssets->UpdateAssetInstances(PlantDynamicData1.PlantInstance, PlantDynamicData1.PlantInstance1, MoundDynamicData.MoundSpline, MoundDynamicData.MoundMesh, 3, 6, PlantDynamicData1.ValidSpawnPoints, PlantDynamicData1.InstanceCount, PlantDynamicData1.LogicalStart);

		WallDynamicData.WallCounter += 1;
		if (WallDynamicData.WallCounter >= WallDynamicData.SplinePointCount)
		{
			UpdateAssets->UpdateWall(WallConfigData, WallDynamicData, SplineConfigData);
			WallDynamicData.WallCounter = 0.0f;
		}
	}

	LimitMovementSmoothly(PathDynamicData.PathSpline, PathConfigData.PathWidth * 0.2f, 5.0f);
}


void AProceduralLevelGenerator_CPP::LimitMovementSmoothly(USplineComponent* GuideSpline, float MaxDistance, float PullSpeed)
{
	float ClosestDistance = GuideSpline->FindInputKeyClosestToWorldLocation(CharacterLocation);
	FVector ClosestPoint = GuideSpline->GetLocationAtSplineInputKey(ClosestDistance, ESplineCoordinateSpace::World);

	float DistanceToSpline = FVector::Dist(CharacterLocation, ClosestPoint);

	if (DistanceToSpline > MaxDistance)
	{
		FVector DirectionToSpline = (ClosestPoint - CharacterLocation).GetSafeNormal();
		FVector TargetPosition = ClosestPoint - DirectionToSpline * MaxDistance;

		// Move smoothly towards the allowed range
		FVector NewPosition = FMath::VInterpTo(CharacterLocation, TargetPosition, GetWorld()->GetDeltaSeconds(), PullSpeed);
		CharacterPawn->SetActorLocation(NewPosition);
	}
}

bool AProceduralLevelGenerator_CPP::IsPointInsideSphere(FVector Point, FVector CircleCenter, float CircleRadius)
{
	float DistanceSquared = FVector::DistSquared(Point, CircleCenter);
	return DistanceSquared <= FMath::Square(CircleRadius);
}

void AProceduralLevelGenerator_CPP::UpdateTerrainSpline()
{
	FVector LastPoint = PathDynamicData.PathSpline->GetLocationAtSplinePoint(SplineConfigData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	FVector SecondLastPoint = PathDynamicData.PathSpline->GetLocationAtSplinePoint(SplineConfigData.SplinePoints - 2, ESplineCoordinateSpace::Local);

	FVector NewPoint = FVector::ZeroVector;

	FVector Direction = LastPoint - SecondLastPoint;
	Direction.Normalize();

	static float CurrentCurveDirectionX = 0.0f;
	static float CurrentCurveDirectionY = 0.0f;

	static float CurveSpeedMultiplier = 0.1f;


	// Every 3-6 seconds, randomise curve behavior
	if (ChangeCurve)
	{
		CurveSpeedMultiplier = FMath::RandRange(0.25f, 0.5f); // Makes turns speed up/slow down
		NoiseDynamicData.SharpnessMultiplier = FMath::RandRange(35.0f, 45.0f); // Adjusts how sharp turns can be
		NoiseDynamicData.FrequencyMultiplier = FMath::RandRange(0.002f, 0.005f); // Adjusts how frequent turns occur
		ChangeCurve = false;
		PointCount = 0;
	}

	// Generate Perlin noise with the dynamically changing frequency
	float NoiseValueX = FMath::PerlinNoise1D(LastPoint.X * NoiseDynamicData.FrequencyMultiplier) * NoiseDynamicData.SharpnessMultiplier;
	float NoiseValueY = FMath::PerlinNoise1D(LastPoint.Y * NoiseDynamicData.FrequencyMultiplier) * NoiseDynamicData.SharpnessMultiplier;

	CurrentCurveDirectionX = FMath::Lerp(CurrentCurveDirectionX, NoiseValueX, 0.1f * CurveSpeedMultiplier);
	CurrentCurveDirectionY = FMath::Lerp(CurrentCurveDirectionY, NoiseValueY, 0.1f * CurveSpeedMultiplier);

	NewPoint = LastPoint + (Direction * SplineConfigData.PlaneDistance);
	NewPoint.X += CurrentCurveDirectionX;
	NewPoint.Y += CurrentCurveDirectionY;
	NewPoint.Z = 0.0f; // Keep terrain flat

	DistanceToCenter = FVector::Dist(NewPoint, SphereCenter);


	// If the spline is getting too close to the edge, steer it back inward
	if (DistanceToCenter >= EdgeThreshold)
	{
		FVector ToCenter = (SphereCenter - NewPoint).GetSafeNormal(); // Vector pointing inward
		float InfluenceFactor = (DistanceToCenter - EdgeThreshold) / (SphereRadius - EdgeThreshold); // Smooth transition
		FVector SteeringAdjustment = ToCenter * InfluenceFactor * NoiseDynamicData.SharpnessMultiplier * 0.4f; // Scale influence

		NewPoint += SteeringAdjustment; // Apply gentle inward force
	}

	FVector NearestPoint = PathDynamicData.PathSpline->FindLocationClosestToWorldLocation(NewPoint, ESplineCoordinateSpace::Local);

	float DistanceToNearest = FVector::Dist(NearestPoint, NewPoint);
	FVector ToNearest = (NearestPoint - NewPoint).GetSafeNormal();

	// Dot product: 1 = same direction, 0 = perpendicular, -1 = opposite direction
	float Alignment = FVector::DotProduct(Direction, ToNearest);

	// Optional threshold: adjust as needed (e.g. 0.3 allows for ~70 degrees cone in front)
	const float MinForwardAlignment = 0.4f;
	FVector RepelDirection;
	// Check both distance and alignment
	if (DistanceToNearest < SplineConfigData.MinDistanceToPreviousPoints && Alignment > MinForwardAlignment)
	{
		// Repel only if point is in the forward arc
		RepelDirection = (NewPoint - NearestPoint).GetSafeNormal();
		float PushStrength = FMath::Clamp((SplineConfigData.MinDistanceToPreviousPoints - DistanceToNearest) / SplineConfigData.MinDistanceToPreviousPoints, 0.0f, 1.0f);
		NewPoint += RepelDirection * PushStrength * 100.0f;
	}
		

	// Add the new point to the spline
	PathDynamicData.PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);

	// Remove first spline point to keep length constant
	PathDynamicData.PathSpline->RemoveSplinePoint(0, false);
	PathDynamicData.PathVertices.RemoveAt(0, PathConfigData.PathVertCount);
	PathDynamicData.PathNormals.RemoveAt(0, PathConfigData.PathVertCount);
	PathDynamicData.PathTangents.RemoveAt(0, PathConfigData.PathVertCount);

	// Update spline with new points
	PathDynamicData.PathSpline->UpdateSpline();

	MeshUpdates->UpdatePathMesh(PathConfigData, PathDynamicData, RiverConfigData, RiverDynamicData, WallConfigData, WallDynamicData, NoiseConfigData, NoiseDynamicData, SplineConfigData);
	MeshUpdates->UpdateRiverMesh(RiverConfigData, RiverDynamicData, MoundConfigData, MoundDynamicData, SplineConfigData);
	MeshUpdates->UpdateMoundMesh(MoundConfigData, MoundDynamicData, RiverConfigData, NoiseConfigData, NoiseDynamicData, SplineConfigData);
}