// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralLevelGenerator_CPP.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProceduralLevelGenerator_CPP::AProceduralLevelGenerator_CPP()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the sphere trigger
	SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
	RootComponent = SphereTrigger;
	SphereTrigger->InitSphereRadius(40000.0f);
	SphereTrigger->SetMobility(EComponentMobility::Static);

	// Create the Path Components
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathSpline->SetupAttachment(RootComponent);
	PathSpline->SetCanEverAffectNavigation(false);

	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathMesh->SetupAttachment(RootComponent);
	PathMesh->SetCullDistance(9000.0f);
	PathMesh->SetCanEverAffectNavigation(false);
	PathMesh->bUseAsyncCooking = true;

	// Create the River Components
	RiverSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RiverSpline"));
	RiverSpline->SetupAttachment(RootComponent);
	RiverSpline->SetCanEverAffectNavigation(false);

	RiverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));
	RiverMesh->SetupAttachment(RootComponent);
	PathMesh->SetCullDistance(9000.0f);
	RiverMesh->SetCanEverAffectNavigation(false);
	RiverMesh->bUseAsyncCooking = true;

	// Create the Mound Components
	MoundSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoundSpline"));
	MoundSpline->SetupAttachment(RootComponent);
	MoundSpline->SetCanEverAffectNavigation(false);

	MoundMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MoundMesh"));
	MoundMesh->SetupAttachment(RootComponent);
	MoundMesh->SetCullDistance(9000.0f);
	MoundMesh->SetCanEverAffectNavigation(false);
	MoundMesh->bUseAsyncCooking = true;

	// Create the Wall Spline
	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	WallSpline->SetupAttachment(RootComponent);
	WallSpline->SetCanEverAffectNavigation(false);

	// Create the Plant Instances
	PlantInstance = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedMesh"));
	PlantInstance->SetMobility(EComponentMobility::Movable);
	PlantInstance->SetupAttachment(RootComponent);

	PlantInstance2 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedMesh2"));
	PlantInstance2->SetMobility(EComponentMobility::Movable);
	PlantInstance2->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProceduralLevelGenerator_CPP::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the spline is correctly set up
	if (PathSpline)
	{
		DynamicRiverMaterial = UMaterialInstanceDynamic::Create(RiverMaterial, this);

		// Clear any previous spline points
		PathSpline->ClearSplinePoints();
		WallSpline->ClearSplinePoints();
		RiverSpline->ClearSplinePoints();
		MoundSpline->ClearSplinePoints();

		Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		Noise.SetFrequency(NoiseFrequency);

		float AmplitudeScale = FMath::RandRange(1.0f, 2.5f);  // Randomly scale wave size
		float FrequencyScale = FMath::RandRange(0.8f, 1.2f);  // Random slight frequency variation

		for (int32 i = 0; i < SplinePoints; i++)
		{
			float SinWaveY = FMath::Sin((i - ((SplinePoints / 2) - 10)) * (PI / ((SplinePoints / 2) - 1)) * FrequencyScale) * (100.0f * AmplitudeScale);
			FVector NewPoint(
				((i - ((SplinePoints / 2) - 10)) * PlaneDistance),  // X position
				SinWaveY,  // Adjusted sinusoidal Y with randomness
				0.0f       // Keep Z the same (flat for now)
			);

			PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);
		}

		PathSpline->UpdateSpline();
	}

	// Set up the sphere trigger
	SphereCenter = SphereTrigger->GetComponentLocation();
	SphereRadius = SphereTrigger->GetScaledSphereRadius();
	DistanceToCenter = 0.0f;
	EdgeThreshold = SphereRadius * 0.85f;

	// Generate meshes
	GeneratePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathNormals, PathTangents, PathTriangles, PathMaterial, PathVertCount);
	//GenerateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverNormals, RiverTangents, RiverTriangles, DynamicRiverMaterial, RiverVertCount);
	//GenerateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundNormals, MoundTangents, MoundTriangles, MoundMaterial, MoundVertCount);



	SpawnWall(WallSpline);

	PlantInstance->SetStaticMesh(PlantMesh);
	SpawnAssetInstances(PlantInstance, GetValidSpawnPoints(PathMesh, PathSpline), PathMesh, 3, 6);


	// Clear any existing spline points
	MoundSpline->ClearSplinePoints();

	// Create a spatial grid to check for overlaps efficiently
	const float GridCellSize = 100.0f * 1.5f; // Grid cells should be at least as large as the step distance
	TMap<FIntVector, TArray<FVector>> Grid;

	// Generate the initial point at a random position within the circle
	FVector LastPoint = SphereCenter + FVector(FMath::RandRange(-SphereRadius, SphereRadius), FMath::RandRange(-SphereRadius, SphereRadius), 0.0f);

	// Ensure the first point is inside the circle
	if (FVector::Dist(LastPoint, SphereCenter) > SphereRadius)
	{
		LastPoint = SphereCenter + (LastPoint - SphereCenter).GetSafeNormal() * SphereRadius;
	}

	// Add the first point to the spline and the grid
	MoundSpline->AddSplinePoint(LastPoint, ESplineCoordinateSpace::Local, false);
	FIntVector GridKey = FIntVector(FMath::FloorToInt(LastPoint.X / GridCellSize), FMath::FloorToInt(LastPoint.Y / GridCellSize), 0);
	Grid.FindOrAdd(GridKey).Add(LastPoint);

	FVector CurrentDirection = FVector(1.f, 0.f, 0.f); // Initial direction, rightwards

	// Loop to generate the rest of the points
	for (int32 i = 1; i < 1000; i++)
	{
		FVector NewPoint;
		bool bValidPoint = false;

		// Try generating a valid point, retry if overlap occurs
		while (!bValidPoint)
		{
			// Calculate a random variation to simulate meandering
			float RandomAngleOffset = FMath::RandRange(-0.2f, 0.2f); // Adjust the range for tighter or looser turns
			FVector NewDirection = FRotationMatrix(FRotator(0.f, RandomAngleOffset, 0.f)).TransformVector(CurrentDirection);

			// Move the point along the current direction
			NewPoint = LastPoint + NewDirection * 100.0f;

			// Ensure the new point is within the circle
			if (FVector::Dist(NewPoint, SphereCenter) > SphereRadius)
			{
				NewPoint = SphereCenter + (NewPoint - SphereCenter).GetSafeNormal() * SphereRadius;
			}

			// Check if the new point is too close to any previous points using the spatial grid
			GridKey = FIntVector(FMath::FloorToInt(NewPoint.X / GridCellSize), FMath::FloorToInt(NewPoint.Y / GridCellSize), 0);
			bool bOverlapping = false;

			// Check neighboring grid cells for points that are too close
			for (int32 x = -1; x <= 1; x++)
			{
				for (int32 y = -1; y <= 1; y++)
				{
					FIntVector NeighborKey = GridKey + FIntVector(x, y, 0);
					if (Grid.Contains(NeighborKey))
					{
						for (const FVector& ExistingPoint : Grid[NeighborKey])
						{
							if (FVector::Dist(NewPoint, ExistingPoint) < 100.0f * 0.8f) // 80% of StepDistance for overlap check
							{
								bOverlapping = true;
								break;
							}
						}
					}
					if (bOverlapping) break;
				}
				if (bOverlapping) break;
			}

			// If no overlap, it's a valid point
			if (!bOverlapping)
			{
				bValidPoint = true;
			}
		}

		// Add the new point to the spline and the grid
		MoundSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);
		GridKey = FIntVector(FMath::FloorToInt(NewPoint.X / GridCellSize), FMath::FloorToInt(NewPoint.Y / GridCellSize), 0);
		Grid.FindOrAdd(GridKey).Add(NewPoint);

		// Update the direction for the next segment
		CurrentDirection = (NewPoint - LastPoint).GetSafeNormal();
		LastPoint = NewPoint;
	}

	// Update the spline to apply the generated points
	MoundSpline->UpdateSpline();
}

// Called every frame
void AProceduralLevelGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CharacterPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	CharacterLocation = CharacterPawn->GetActorLocation();
	FVector MidPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints / 2, ESplineCoordinateSpace::Local);

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathWidth / 2)
	{
		PointCount += 1.0f;
		if (PointCount >= 40.0f)
		{
			ChangeCurve = true;
		}
		UpdateTerrainSpline();

		WallCounter += 1;
		if (WallCounter >= SplinePointCount)
		{
			UpdateWall(WallSpline);
			WallCounter = 0.0f;
		}
	}

	LimitMovementSmoothly(PathSpline, PathWidth * 0.2f, 5.0f);
}


TArray<FVector> AProceduralLevelGenerator_CPP::GetValidSpawnPoints(UProceduralMeshComponent* Mesh, USplineComponent* GuideSpline)
{
	TArray<FVector> SpawnPoints;
	FProcMeshSection* Section = Mesh->GetProcMeshSection(0);

	for (const FProcMeshVertex& Vertex : Section->ProcVertexBuffer)
	{
		FVector Normal = Vertex.Normal;
		float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Normal, FVector::UpVector)));

		if (SlopeAngle > 25.0f && Vertex.Position.Z > -RiverDepth) // Only spawn on relatively flat surfaces
		{
			FVector ClosestSplinePoint = GuideSpline->FindLocationClosestToWorldLocation(Vertex.Position, ESplineCoordinateSpace::World);
			FVector RightVector = GuideSpline->GetRightVectorAtSplinePoint(ClosestSplinePoint.X, ESplineCoordinateSpace::World);
			FVector ToPoint = Vertex.Position - ClosestSplinePoint;

			if (FVector::DotProduct(ToPoint, RightVector) < 0.0f)
			{
				SpawnPoints.Add(Vertex.Position);
			}
		}
	}
	return SpawnPoints;
}

void AProceduralLevelGenerator_CPP::SpawnAssetInstances(UHierarchicalInstancedStaticMeshComponent* HISM, const TArray<FVector>& SpawnPoints, UProceduralMeshComponent* Mesh, int ClusterSizeMin, int ClusterSizeMax)
{
  if (!HISM) return;

	// Clear existing instances
	HISM->ClearInstances();

	for (const FVector& Point : SpawnPoints)
	{
		float NoiseVal = FMath::PerlinNoise2D(FVector2D(Point.X, Point.Y) * 0.1f);
		NoiseVal = (NoiseVal + 1.0f) / 2.0f; // Normalize to [0, 1]
		if (NoiseVal > 0.4f)
			{
				
			int ClusterSize = FMath::RandRange(ClusterSizeMin, ClusterSizeMax); // Random cluster size
			for (int32 i = 0; i < ClusterSize; i++)
			{
				// Generate random offset within a small radius around the point
				FVector Offset = FVector(
					FMath::RandRange(-50.0f, 50.0f),
					FMath::RandRange(-50.0f, 50.0f),
					0.0f
				);

				FVector ClusterPoint = Point + Offset;
				float ScaleFactor = FMath::Lerp(0.6f, 1.2f, FMath::RandRange(0.0f,1.0f));

				// Interpolate the height based on the surrounding vertices
				FVector InterpolatedPoint = InterpolateHeight(ClusterPoint, Mesh);

				FVector Normal = (InterpolatedPoint - ClusterPoint).GetSafeNormal();
				float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Normal, FVector::UpVector)));
				if (SlopeAngle > 25.0f) continue; // Skip steep slopes

				FRotator Rotation = InterpolatedPoint.GetSafeNormal().Rotation();

				// Spawn the instance
				HISM->AddInstance(FTransform(Rotation, InterpolatedPoint, FVector(ScaleFactor)));
			}
		}
	}

}

bool AProceduralLevelGenerator_CPP::PointInTriangle2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, const FVector2D& C)
{
	float dX = P.X - C.X;
	float dY = P.Y - C.Y;
	float dX21 = C.X - B.X;
	float dY12 = B.Y - C.Y;
	float D = dY12 * (A.X - C.X) + dX21 * (A.Y - C.Y);
	float s = dY12 * dX + dX21 * dY;
	float t = (C.Y - A.Y) * dX + (A.X - C.X) * dY;
	if (D < 0) return (s <= 0) && (t <= 0) && (s + t >= D);
	return (s >= 0) && (t >= 0) && (s + t <= D);
}

FVector AProceduralLevelGenerator_CPP::InterpolateHeight(const FVector& Point, UProceduralMeshComponent* Mesh)
{
	const TArray<FProcMeshVertex>& Vertices = Mesh->GetProcMeshSection(0)->ProcVertexBuffer;
	const TArray<uint32>& Indices = Mesh->GetProcMeshSection(0)->ProcIndexBuffer;
	FVector InterpolatedPoint = Point;

	for (int32 i = 0; i < Indices.Num(); i += 3)
	{
		const FVector& V0 = Vertices[Indices[i]].Position;
		const FVector& V1 = Vertices[Indices[i + 1]].Position;
		const FVector& V2 = Vertices[Indices[i + 2]].Position;

		FVector2D A(V0.X, V0.Y);
		FVector2D B(V1.X, V1.Y);
		FVector2D C(V2.X, V2.Y);
		FVector2D P(Point.X, Point.Y);

		if (PointInTriangle2D(P, A, B, C))
		{
			// Barycentric interpolation
			FVector2D v0 = B - A;
			FVector2D v1 = C - A;
			FVector2D v2 = P - A;

			float d00 = FVector2D::DotProduct(v0, v0);
			float d01 = FVector2D::DotProduct(v0, v1);
			float d11 = FVector2D::DotProduct(v1, v1);
			float d20 = FVector2D::DotProduct(v2, v0);
			float d21 = FVector2D::DotProduct(v2, v1);

			float denom = d00 * d11 - d01 * d01;
			if (FMath::IsNearlyZero(denom)) continue;

			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0f - v - w;

			InterpolatedPoint.Z = u * V0.Z + v * V1.Z + w * V2.Z;
		}
	}

	return InterpolatedPoint; // Return zero vector if no valid point found
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
	FVector LastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	FVector SecondLastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 2, ESplineCoordinateSpace::Local);

	FVector NewPoint = FVector::ZeroVector;

	FVector Direction = LastPoint - SecondLastPoint;
	Direction.Normalize();

	static float CurrentCurveDirectionX = 0.0f;
	static float CurrentCurveDirectionY = 0.0f;

	static float CurveSpeedMultiplier = 0.1f;


	// Every 3-6 seconds, randomise curve behavior
	if (ChangeCurve && !AvoidingEdge)
	{
		CurveSpeedMultiplier = FMath::RandRange(0.25f, 0.5f); // Makes turns speed up/slow down
		SharpnessMultiplier = FMath::RandRange(35.0f, 45.0f); // Adjusts how sharp turns can be
		FrequencyMultiplier = FMath::RandRange(0.002f, 0.005f); // Adjusts how frequent turns occur
		ChangeCurve = false;
		PointCount = 0;
	}

	// Generate Perlin noise with the dynamically changing frequency
	float NoiseValueX = FMath::PerlinNoise1D(LastPoint.X * FrequencyMultiplier) * SharpnessMultiplier;
	float NoiseValueY = FMath::PerlinNoise1D(LastPoint.Y * FrequencyMultiplier) * SharpnessMultiplier;

	CurrentCurveDirectionX = FMath::Lerp(CurrentCurveDirectionX, NoiseValueX, 0.1f * CurveSpeedMultiplier);
	CurrentCurveDirectionY = FMath::Lerp(CurrentCurveDirectionY, NoiseValueY, 0.1f * CurveSpeedMultiplier);

	NewPoint = LastPoint + (Direction * PlaneDistance);
	NewPoint.X += CurrentCurveDirectionX;
	NewPoint.Y += CurrentCurveDirectionY;
	NewPoint.Z = 0.0f; // Keep terrain flat

	DistanceToCenter = FVector::Dist(NewPoint, SphereCenter);


	// If the spline is getting too close to the edge, steer it back inward
	if (DistanceToCenter >= EdgeThreshold)
	{
		FVector ToCenter = (SphereCenter - NewPoint).GetSafeNormal(); // Vector pointing inward
		float InfluenceFactor = (DistanceToCenter - EdgeThreshold) / (SphereRadius - EdgeThreshold); // Smooth transition
		FVector SteeringAdjustment = ToCenter * InfluenceFactor * SharpnessMultiplier * 0.25f; // Scale influence

		NewPoint += SteeringAdjustment; // Apply gentle inward force
		AvoidingEdge = true; // Set flag to indicate we're avoiding the edge;
	}
	else
	{
		AvoidingEdge = false; // Reset flag when not avoiding the edge
	}

	// Add the new point to the spline
	PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);

	// Remove first spline point to keep length constant
	PathSpline->RemoveSplinePoint(0, false);
	PathVertices.RemoveAt(0, PathVertCount);
	PathNormals.RemoveAt(0, PathVertCount);
	PathTangents.RemoveAt(0, PathVertCount);

	// Update spline with new points
	PathSpline->UpdateSpline();

	UpdatePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathNormals, PathTangents, PathTriangles, PathMaterial, PathVertCount);
	UpdateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverNormals, RiverTangents, RiverTriangles, DynamicRiverMaterial, RiverVertCount);
	UpdateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundNormals, MoundTangents, MoundTriangles, MoundMaterial, MoundVertCount);
}



void AProceduralLevelGenerator_CPP::GeneratePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, UMaterialInterface* MeshMaterial, int PathVerts)
{
	depthOffset = RiverDepth / (PathVerts / 4.0f);

	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth * 0.5f));
		FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverDepth);

		for (int32 y = 0; y < PathVertCount; y++)
		{
			MeshVert = PathMeshHelper(y, MeshWidth, MeshEdge);

			MeshVertices.Add(MeshVert);
			MeshTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
		}
	}

	for (int i = 0; i < MeshVertices.Num(); i++)
	{
		if (i < (SplinePoints - 1) * (PathVerts - 1))
		{
			TriangleCalcs(MeshVertices, MeshTriangles, PathVerts, i);
		}
		NormalCalcs(MeshVertices, MeshNormals, PathVerts, i);
	}
	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, MeshNormals, TArray<FVector2D>(), TArray<FColor>(), MeshTangents, true);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}

void AProceduralLevelGenerator_CPP::TriangleCalcs(TArray<FVector>& MeshVertices, TArray<int32>& MeshTriangles, int MeshVertCount, int i)
{
	int Row = i / (MeshVertCount - 1);
	int Col = i % (MeshVertCount - 1);

	// Compute the four vertex indices
	int A = (Row * MeshVertCount) + Col;
	int B = A + 1;
	int C = A + MeshVertCount;
	int D = C + 1;

	// First triangle (Counter-Clockwise)
	MeshTriangles.Add(A);
	MeshTriangles.Add(B);
	MeshTriangles.Add(C);

	// Second triangle (Counter-Clockwise)
	MeshTriangles.Add(C);
	MeshTriangles.Add(B);
	MeshTriangles.Add(D);
}

void AProceduralLevelGenerator_CPP::NormalCalcs(TArray<FVector>& MeshVertices, TArray<FVector>& MeshNormals, int MeshVertCount, int i)
{
	FVector CurrentVertex = MeshVertices[i];

	bool bHasForwardPoint = i + MeshVertCount < MeshVertices.Num();
	bool bHasBackwardPoint = i - MeshVertCount >= 0;
	bool bHasRightPoint = (i + 1) % MeshVertCount != 0;
	bool bHasLeftPoint = i % MeshVertCount != 0;

	FVector NormalSum = FVector::ZeroVector;
	int NormalCount = 0;

	if (bHasForwardPoint && bHasRightPoint)
	{
		NormalSum += FVector::CrossProduct(MeshVertices[i + MeshVertCount] - CurrentVertex, MeshVertices[i + 1] - CurrentVertex);
		NormalCount++;
	}
	if (bHasForwardPoint && bHasLeftPoint)
	{
		NormalSum += FVector::CrossProduct(MeshVertices[i + MeshVertCount] - CurrentVertex, CurrentVertex - MeshVertices[i - 1]);
		NormalCount++;
	}
	if (bHasBackwardPoint && bHasRightPoint)
	{
		NormalSum += FVector::CrossProduct(CurrentVertex - MeshVertices[i - MeshVertCount], MeshVertices[i + 1] - CurrentVertex);
		NormalCount++;
	}
	if (bHasBackwardPoint && bHasLeftPoint)
	{
		NormalSum += FVector::CrossProduct(CurrentVertex - MeshVertices[i - MeshVertCount], CurrentVertex - MeshVertices[i - 1]);
		NormalCount++;
	}

	if (NormalCount > 0)
	{
		MeshNormals.Add(NormalSum.GetSafeNormal());
	}
	else
	{
		MeshNormals.Add(FVector::UpVector);
	}
}

void AProceduralLevelGenerator_CPP::UpdatePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, UMaterialInterface* MeshMaterial, int PathVerts)
{
	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth * 0.5f));
	FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverDepth);

	for (int32 y = 0; y < PathVerts; y++)
	{
		MeshVert = PathMeshHelper(y, MeshWidth, MeshEdge);

		MeshVertices.Add(MeshVert);
		MeshTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
	}

	for (int i = 0; i < PathVerts; i++)
	{
		NormalCalcs(MeshVertices, MeshNormals, PathVerts, (MeshVertices.Num() - PathVerts) + i);
	}
	ProcMesh->UpdateMeshSection(0, MeshVertices, MeshNormals, TArray<FVector2D>(), TArray<FColor>(), MeshTangents); 
}

FVector AProceduralLevelGenerator_CPP::PathMeshHelper(int y, int MeshWidth, FVector& MeshEdge)
{
	offsetCalc = ((MeshWidth / 2.0f) * (y / ((PathVertCount - 1.0f) / 2.0f)));

	MeshVert = MeshEdge + (MeshEdgeVector * offsetCalc);

	NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
	Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

	if (y == 0)
	{
		FVector RiverSplinePoint = MeshVert - (MeshEdgeVector * (RiverWidth / 2));
		RiverSpline->AddSplinePoint(RiverSplinePoint, ESplineCoordinateSpace::Local, false);

		while (RiverSpline->GetNumberOfSplinePoints() > SplinePoints)
		{
			// Remove first spline point to keep length constant
			RiverSpline->RemoveSplinePoint(0, false);
			RiverVertices.RemoveAt(0, RiverVertCount);
			VertexColours.RemoveAt(0, RiverVertCount);
			RiverNormals.RemoveAt(0, RiverVertCount);
			RiverTangents.RemoveAt(0, RiverVertCount);
		}
	}
	else if (y < PathVertCount / 4)
	{
		/**/float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		float SmoothFactor = FMath::Pow(0.85f + 0.15f * FMath::Cos((y / (float)MoundVertCount) * PI), 2.5f);

		if (MeshSplineTangent.X - MeshSplineTangent.Y > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (NoiseAmplitude);
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (30.0f) * SmoothFactor;
		}
		else if (MeshSplineTangent.Y - MeshSplineTangent.X > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (30.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (NoiseAmplitude);
		}
		else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (20.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (20.0f) * SmoothFactor;
		}
		MeshVert.Z += (depthOffset * y);
	}
	else if (y == PathVertCount / 4)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
	}
	else if (y > PathVertCount / 4 && y < PathVertCount - (PathVertCount / 4) - 1)
	{
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
	}
	else if (y == PathVertCount - (PathVertCount / 4) - 1)
	{ 
		WallSpline->AddSplinePoint(MeshVert + FVector(0.0f, 0.0f, RiverDepth * 0.9f), ESplineCoordinateSpace::Local, false);

		while (WallSpline->GetNumberOfSplinePoints() > SplinePoints)
		{
			WallSpline->RemoveSplinePoint(0, false);
		}

		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
	}
	else
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Z += (depthOffset * ((PathVertCount - 1) - y));
	}
	RiverSpline->UpdateSpline();
	WallSpline->UpdateSpline();
	return MeshVert;
}

void AProceduralLevelGenerator_CPP::GenerateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, UMaterialInterface* MeshMaterial, int RiverVerts)
{
	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FProcMeshTangent MeshTangent = FProcMeshTangent(MeshSplineTangent, false);
		FVector TangentX = MeshTangent.TangentX.GetSafeNormal();
		if (TangentX.X < 0.0f)
		{
			TangentX.X = -TangentX.X;
		}
		if (TangentX.Y < 0.0f)
		{
			TangentX.Y = -TangentX.Y;
		}

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 1.8f));
		FVector MoundSplinePoint = (MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2.0f))) - (MeshEdgeVector * (MoundWidth / 2));
		MoundSpline->AddSplinePoint(MoundSplinePoint, ESplineCoordinateSpace::Local, false);

		for (int32 y = 0; y < RiverVertCount; y++)
		{
			offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((RiverVertCount - 1.0f) / 2.0f) * y;
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);

			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			MeshVert.Z += 10.0f;

			MeshVertices.Add(MeshVert);

			MeshTangents.Add(MeshTangent);
			VertexColours.Add(FLinearColor(TangentX.X, TangentX.Y, 0.5f, 1.0f));
		}

	}

	MoundSpline->UpdateSpline();

	for (int i = 0; i < MeshVertices.Num(); i++)
	{
		if (i < (SplinePoints - 1) * (RiverVerts - 1))
		{
			TriangleCalcs(MeshVertices, MeshTriangles, RiverVerts, i);
		}
		NormalCalcs(MeshVertices, MeshNormals, RiverVerts, i);
	}
	ProcMesh->CreateMeshSection_LinearColor(0, MeshVertices, MeshTriangles, MeshNormals, TArray<FVector2D>(), VertexColours, MeshTangents, true, false);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}

void AProceduralLevelGenerator_CPP::UpdateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, UMaterialInterface* MeshMaterial, int RiverVerts)
{
	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	FProcMeshTangent MeshTangent = FProcMeshTangent(MeshSplineTangent, false);
	FVector TangentX = MeshTangent.TangentX.GetSafeNormal();
	if (TangentX.X < 0.0f)
	{
		TangentX.X = -TangentX.X;
	}
	if (TangentX.Y < 0.0f)
	{
		TangentX.Y = -TangentX.Y;
	}
	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 1.8f));

	FVector MoundSplinePoint = (MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2.0f))) - (MeshEdgeVector * (MoundWidth / 2));
	MoundSpline->AddSplinePoint(MoundSplinePoint, ESplineCoordinateSpace::Local, false);

	while (MoundSpline->GetNumberOfSplinePoints() > SplinePoints)
	{
		// Remove first spline point to keep length constant
		MoundSpline->RemoveSplinePoint(0, true);
		MoundVertices.RemoveAt(0, MoundVertCount);
		MoundNormals.RemoveAt(0, MoundVertCount);
		MoundTangents.RemoveAt(0, MoundVertCount);
	}

	for (int32 y = 0; y < RiverVertCount; y++)
	{
		offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((RiverVertCount - 1.0f) / 2.0f) * y;
		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
		
		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
		MeshVert.Z += 10.0f;

		MeshVertices.Add(MeshVert);

		MeshTangents.Add(MeshTangent);
		VertexColours.Add(FLinearColor(TangentX.X, TangentX.Y, 0.5f, 1.0f));
	}

	if (AvoidingEdge)
	{
		GuideSpline->UpdateSpline();
		MeshTangents.Empty();
		MeshNormals.Empty();
		for (int32 i = 0; i < SplinePoints; i++)
		{
			MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

			MeshTangent = FProcMeshTangent(MeshSplineTangent, false);
			TangentX = MeshTangent.TangentX.GetSafeNormal();
			if (TangentX.X < 0.0f)
			{
				TangentX.X = -TangentX.X;
			}
			if (TangentX.Y < 0.0f)
			{
				TangentX.Y = -TangentX.Y;
			}
			for (int32 y = 0; y < RiverVertCount; y++)
			{
				MeshTangents.Add(MeshTangent);
				VertexColours.Add(FLinearColor(TangentX.X, TangentX.Y, 0.5f, 1.0f));

				NormalCalcs(MeshVertices, MeshNormals, RiverVerts, (i * RiverVerts) + y);
			}
		}
	}
	else
	{
		for (int i = 0; i < RiverVerts; i++)
		{
			NormalCalcs(MeshVertices, MeshNormals, RiverVerts, (MeshVertices.Num() - RiverVerts) + i);
		}
	}

	ProcMesh->UpdateMeshSection_LinearColor(0, MeshVertices, MeshNormals, TArray<FVector2D>(), VertexColours, MeshTangents);
	//ProcMesh->CreateMeshSection_LinearColor(0, MeshVertices, MeshTriangles, MeshNormals, TArray<FVector2D>(), VertexColours, MeshTangents, true, false);
}

void AProceduralLevelGenerator_CPP::SplineRegulator(USplineComponent* GuideSpline)
{
	TArray<FVector> SplineArray;

	float TotalLength = GuideSpline->GetSplineLength(); // Get full spline length
	float TargetSpacing = TotalLength / (SplinePoints - 1); // Keep equal length

	for (int i = 0; i < SplinePoints; i++)
	{
		// Find correct position along base spline
		float DistanceAlongSpline = i * TargetSpacing;

		FVector SplineLocation = GuideSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);
		//GuideSpline->SetLocationAtSplinePoint(i, SplineLocation, ESplineCoordinateSpace::Local, true);
		SplineArray.Add(SplineLocation);
	}

	GuideSpline->ClearSplinePoints();
	for (int i = 0; i < SplineArray.Num(); i++)
	{
		GuideSpline->AddSplinePoint(SplineArray[i], ESplineCoordinateSpace::Local, false);
	}
	GuideSpline->UpdateSpline();
}

void AProceduralLevelGenerator_CPP::GenerateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, UMaterialInterface* MeshMaterial, int MoundVerts)
{
	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth / 2.0f);

		float zValue = MoundHeight;
		int verts75 = MoundVerts * 0.75f;
		int verts25 = MoundVerts - verts75;
		float zOffset75 = MoundHeight / verts75 + 1;
		float zOffset25 = zOffset75 / verts25;

		for (int32 y = 0; y < MoundVertCount; y++)
		{
			if (y < MoundVerts * 0.25f)
			{
				MeshVert = MoundMeshHelper(y, MeshWidth, zValue, zOffset25, SplinePoints - 1);
			}
			else
			{
				MeshVert = MoundMeshHelper(y, MeshWidth, zValue, zOffset75, SplinePoints - 1);
			}

			MeshVertices.Add(MeshVert);
			MeshTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
		}
	}

	for (int i = 0; i < MeshVertices.Num(); i++)
	{
		if (i < (SplinePoints - 1) * (MoundVerts - 1))
		{
			TriangleCalcs(MeshVertices, MeshTriangles, MoundVerts, i);
		}
		NormalCalcs(MeshVertices, MeshNormals, MoundVerts, i);
	}

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, MeshNormals, TArray<FVector2D>(), TArray<FColor>(), MeshTangents, true);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}

void AProceduralLevelGenerator_CPP::UpdateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector>& MeshNormals, TArray<FProcMeshTangent>& MeshTangents, TArray<int32>& MeshTriangles, UMaterialInterface* MeshMaterial, int MoundVerts)
{
	//SplineRegulator(GuideSpline);

	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth / 2.0f);

	float zValue = MoundHeight;
	int verts75 = MoundVerts * 0.75f;
	int verts25 = MoundVerts - verts75;
	float zOffset75 = MoundHeight / verts75 + 1;
	float zOffset25 = zOffset75 / verts25;

	for (int32 y = 0; y < MoundVerts; y++)
	{
		if (y < MoundVerts * 0.25f)
		{
			MeshVert = MoundMeshHelper(y, MeshWidth, zValue, zOffset25, SplinePoints - 1);
		}
		else
		{
			MeshVert = MoundMeshHelper(y, MeshWidth, zValue, zOffset75, SplinePoints - 1);
		}
		MeshVertices.Add(MeshVert);
		MeshTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
	}

	for (int i = 0; i < MoundVerts; i++)
	{
		NormalCalcs(MeshVertices, MeshNormals, MoundVerts, (MeshVertices.Num() - MoundVerts) + i);
	}
	ProcMesh->UpdateMeshSection(0, MeshVertices, MeshNormals, TArray<FVector2D>(), TArray<FColor>(), MeshTangents);
}

FVector AProceduralLevelGenerator_CPP::MoundMeshHelper(int y, int MeshWidth, float& zValue, float zOffset, int i)
{
	offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((MoundVertCount - 1.0f) / 2.0f) * y;
	MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc) + FVector(0.0f, 0.0f, zValue);

	NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
	Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 1.25f);

	if (y == 0)
	{
		MeshVert.Z -= 600.0f;
		zValue -= zOffset;
	}
	else if (y == 1)
	{
		MeshVert.Z -= 30.0f;
		zValue -= zOffset;
	}
	else if (y < (MoundVertCount - 2) * 0.25f)
	{
		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * Falloff) * 30.0f * SmoothFactor;

		zValue -= zOffset;
	}
	else if (y == (MoundVertCount - 2) * 0.25f)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * Falloff) * NoiseAmplitude * SmoothFactor;

		zValue -= zOffset;
	}
	else 
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		float SmoothFactor = FMath::Pow(0.85f + 0.15f * FMath::Cos((y / (float)MoundVertCount) * PI), 2.5f);

		if (MeshSplineTangent.X - MeshSplineTangent.Y > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (NoiseAmplitude);
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (50.0f) * SmoothFactor;
		}
		else if (MeshSplineTangent.Y - MeshSplineTangent.X > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (50.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (NoiseAmplitude);
		}
		/**else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
		}*/

		zValue -= zOffset;
	}
	return MeshVert;
}

void AProceduralLevelGenerator_CPP::SpawnWall(USplineComponent* GuideSpline)
{
	if (!GuideSpline || !WallMesh1 || !WallMesh2) return;

	const float SplineLength = GuideSpline->GetSplineLength();
	int32 NumSegments = FMath::FloorToInt(SplineLength / MeshLength);

	while (EndDistance < SplineLength)
	{
		// Randomly select between the two meshes
		bool WallType = FMath::RandBool();
		UStaticMesh* SelectedMesh = (WallType) ? WallMesh1 : WallMesh2;
		float SelectedMeshLength = SelectedMesh->GetBounds().BoxExtent.X * 2;

		if (StartDistance == 0.0f)
		{
			SplinePointCount = FMath::FloorToInt(SelectedMeshLength / PlaneDistance);
		}

		StartDistance = EndDistance - 10.0f;
		EndDistance = StartDistance + SelectedMeshLength;

		SegmentStartLocation = GuideSpline->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::World);
		SegmentEndLocation = GuideSpline->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::World);
		FVector StartTangent = GuideSpline->GetTangentAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::World);
		FVector EndTangent = GuideSpline->GetTangentAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::World);

		FRotator Rotation = FRotator::ZeroRotator;
		if (FMath::RandBool())
		{
			Rotation.Yaw = 180.0f;
		}

		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetStaticMesh(SelectedMesh);
		//SplineMesh->SetMaterial(0, SelectedMaterial);
		SplineMesh->SetStartAndEnd(SegmentStartLocation, StartTangent, SegmentEndLocation, EndTangent);
		SplineMesh->AttachToComponent(GuideSpline, FAttachmentTransformRules::KeepWorldTransform);
		SplineMesh->RegisterComponent();
		SplineMesh->SetForwardAxis(ESplineMeshAxis::X);
		SplineMeshComponents.Add(SplineMesh);
	}
}

void AProceduralLevelGenerator_CPP::UpdateWall(USplineComponent* GuideSpline)
{
	if (SplineMeshComponents.Num() == 0) return;

	// Remove the component at index 0
	USplineMeshComponent* FirstSplineMesh = SplineMeshComponents[0];
	SplineMeshComponents.RemoveAt(0);

	// Randomly select between the two meshes
	bool WallType = FMath::RandBool();
	UStaticMesh* SelectedMesh = (WallType) ? WallMesh1 : WallMesh2;
	//UMaterialInterface* SelectedMaterial = (WallType) ? WallMaterial1 : WallMaterial2;
	float SelectedMeshLength = SelectedMesh->GetBounds().BoxExtent.X * 2;


	SplinePointCount = FMath::FloorToInt(SelectedMeshLength / PlaneDistance);

	// Calculate the new start and end positions and tangents for the repositioned component

	StartDistance = GuideSpline->GetDistanceAlongSplineAtLocation(SegmentEndLocation, ESplineCoordinateSpace::World) - 10.0f;
	EndDistance = StartDistance + SelectedMeshLength;

	SegmentStartLocation = GuideSpline->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::World);
	SegmentEndLocation = GuideSpline->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::World);
	FVector StartTangent = GuideSpline->GetTangentAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::World);
	FVector EndTangent = GuideSpline->GetTangentAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::World);

	// Update the repositioned component with the new start and end positions and tangents
	FirstSplineMesh->SetStaticMesh(SelectedMesh);
	//FirstSplineMesh->SetMaterial(0, SelectedMaterial);
	FirstSplineMesh->SetStartAndEnd(SegmentStartLocation, StartTangent, SegmentEndLocation, EndTangent);
	FirstSplineMesh->UpdateMesh();

	// Add the removed component to the end of the array
	SplineMeshComponents.Add(FirstSplineMesh);
}