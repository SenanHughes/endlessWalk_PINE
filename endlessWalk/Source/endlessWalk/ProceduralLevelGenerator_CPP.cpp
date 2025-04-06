// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralLevelGenerator_CPP.h"

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
	PathData.PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathData.PathSpline->SetupAttachment(RootComponent);
	PathData.PathSpline->SetCanEverAffectNavigation(false);
	
	PathData.PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathData.PathMesh->SetupAttachment(RootComponent);
	PathData.PathMesh->SetCullDistance(9000.0f);
	PathData.PathMesh->SetCanEverAffectNavigation(false);
	PathData.PathMesh->bUseAsyncCooking = true;

	// Create the River Components
	RiverData.RiverSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RiverSpline"));
	RiverData.RiverSpline->SetupAttachment(RootComponent);
	RiverData.RiverSpline->SetCanEverAffectNavigation(false);

	RiverData.RiverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));
	RiverData.RiverMesh->SetupAttachment(RootComponent);
	RiverData.RiverMesh->SetCullDistance(9000.0f);
	RiverData.RiverMesh->SetCanEverAffectNavigation(false);
	RiverData.RiverMesh->bUseAsyncCooking = true;

	// Create the Mound Components
	MoundData.MoundSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoundSpline"));
	MoundData.MoundSpline->SetupAttachment(RootComponent);
	MoundData.MoundSpline->SetCanEverAffectNavigation(false);

	MoundData.MoundMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MoundMesh"));
	MoundData.MoundMesh->SetupAttachment(RootComponent);
	MoundData.MoundMesh->SetCullDistance(9000.0f);
	MoundData.MoundMesh->SetCanEverAffectNavigation(false);
	MoundData.MoundMesh->bUseAsyncCooking = true;

	// Create the Wall Spline
	WallData.WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	WallData.WallSpline->SetupAttachment(RootComponent);
	WallData.WallSpline->SetCanEverAffectNavigation(false);

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
	if (PathData.PathSpline)
	{
		RiverData.DynamicRiverMaterial = UMaterialInstanceDynamic::Create(RiverData.RiverMaterial, this);

		// Clear any previous spline points
		PathData.PathSpline->ClearSplinePoints();
		WallData.WallSpline->ClearSplinePoints();
		RiverData.RiverSpline->ClearSplinePoints();
		MoundData.MoundSpline->ClearSplinePoints();

		NoiseData.Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		NoiseData.Noise.SetFrequency(NoiseData.NoiseFrequency);

		float AmplitudeScale = FMath::RandRange(1.0f, 2.5f);  // Randomly scale wave size
		float FrequencyScale = FMath::RandRange(0.8f, 1.2f);  // Random slight frequency variation

		for (int32 i = 0; i < SplineData.SplinePoints; i++)
		{
			float SinWaveY = FMath::Sin((i - ((SplineData.SplinePoints / 2) - 10)) * (PI / ((SplineData.SplinePoints / 2) - 1)) * FrequencyScale) * (100.0f * AmplitudeScale);
			FVector NewPoint(
				((i - ((SplineData.SplinePoints / 2) - 10)) * SplineData.PlaneDistance),  // X position
				SinWaveY,  // Adjusted sinusoidal Y with randomness
				0.0f       // Keep Z the same (flat for now)
			);

			PathData.PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);
		}

		PathData.PathSpline->UpdateSpline();
	}

	// Set up the sphere trigger
	SphereCenter = SphereTrigger->GetComponentLocation();
	SphereRadius = SphereTrigger->GetScaledSphereRadius();
	DistanceToCenter = 0.0f;
	EdgeThreshold = SphereRadius * 0.85f;

	// Generate meshes
	GeneratePathMesh(PathData);
	GenerateRiverMesh(RiverData);
	GenerateMoundMesh(MoundData);

	SpawnWall(WallData.WallSpline);

	PlantInstance->SetStaticMesh(PlantData.PlantMesh);
	SpawnAssetInstances(PlantInstance, GetValidSpawnPoints(PathData.PathMesh, PathData.PathSpline), PathData.PathMesh, 3, 6);
}

// Called every frame
void AProceduralLevelGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CharacterPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	CharacterLocation = CharacterPawn->GetActorLocation();
	FVector MidPoint = PathData.PathSpline->GetLocationAtSplinePoint(SplineData.SplinePoints / 2, ESplineCoordinateSpace::Local);

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathData.PathWidth / 2)
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
			UpdateWall(WallData.WallSpline);
			WallCounter = 0.0f;
		}
	}

	LimitMovementSmoothly(PathData.PathSpline, PathData.PathWidth * 0.2f, 5.0f);
}


TArray<FVector> AProceduralLevelGenerator_CPP::GetValidSpawnPoints(UProceduralMeshComponent* Mesh, USplineComponent* GuideSpline)
{
	TArray<FVector> SpawnPoints;
	FProcMeshSection* Section = Mesh->GetProcMeshSection(0);

	for (const FProcMeshVertex& Vertex : Section->ProcVertexBuffer)
	{
		FVector Normal = Vertex.Normal;
		float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Normal, FVector::UpVector)));

		if (SlopeAngle > 25.0f && Vertex.Position.Z > -RiverData.RiverDepth) // Only spawn on relatively flat surfaces
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
	FVector LastPoint = PathData.PathSpline->GetLocationAtSplinePoint(SplineData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	FVector SecondLastPoint = PathData.PathSpline->GetLocationAtSplinePoint(SplineData.SplinePoints - 2, ESplineCoordinateSpace::Local);

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
		NoiseData.SharpnessMultiplier = FMath::RandRange(35.0f, 45.0f); // Adjusts how sharp turns can be
		NoiseData.FrequencyMultiplier = FMath::RandRange(0.002f, 0.005f); // Adjusts how frequent turns occur
		ChangeCurve = false;
		PointCount = 0;
	}

	// Generate Perlin noise with the dynamically changing frequency
	float NoiseValueX = FMath::PerlinNoise1D(LastPoint.X * NoiseData.FrequencyMultiplier) * NoiseData.SharpnessMultiplier;
	float NoiseValueY = FMath::PerlinNoise1D(LastPoint.Y * NoiseData.FrequencyMultiplier) * NoiseData.SharpnessMultiplier;

	CurrentCurveDirectionX = FMath::Lerp(CurrentCurveDirectionX, NoiseValueX, 0.1f * CurveSpeedMultiplier);
	CurrentCurveDirectionY = FMath::Lerp(CurrentCurveDirectionY, NoiseValueY, 0.1f * CurveSpeedMultiplier);

	NewPoint = LastPoint + (Direction * SplineData.PlaneDistance);
	NewPoint.X += CurrentCurveDirectionX;
	NewPoint.Y += CurrentCurveDirectionY;
	NewPoint.Z = 0.0f; // Keep terrain flat

	DistanceToCenter = FVector::Dist(NewPoint, SphereCenter);


	// If the spline is getting too close to the edge, steer it back inward
	if (DistanceToCenter >= EdgeThreshold)
	{
		FVector ToCenter = (SphereCenter - NewPoint).GetSafeNormal(); // Vector pointing inward
		float InfluenceFactor = (DistanceToCenter - EdgeThreshold) / (SphereRadius - EdgeThreshold); // Smooth transition
		FVector SteeringAdjustment = ToCenter * InfluenceFactor * NoiseData.SharpnessMultiplier * 0.25f; // Scale influence

		NewPoint += SteeringAdjustment; // Apply gentle inward force
	}

	// Add the new point to the spline
	PathData.PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);

	// Remove first spline point to keep length constant
	PathData.PathSpline->RemoveSplinePoint(0, false);
	PathData.PathVertices.RemoveAt(0, PathData.PathVertCount);
	PathData.PathNormals.RemoveAt(0, PathData.PathVertCount);
	PathData.PathTangents.RemoveAt(0, PathData.PathVertCount);

	// Update spline with new points
	PathData.PathSpline->UpdateSpline();

	UpdatePathMesh(PathData);
	UpdateRiverMesh(RiverData);
	UpdateMoundMesh(MoundData);
}



void AProceduralLevelGenerator_CPP::GeneratePathMesh(const FPathData& PathDataStruct)
{
	depthOffset = RiverData.RiverDepth / (PathData.PathVertCount / 4.0f);

	for (int32 i = 0; i < SplineData.SplinePoints; i++)
	{
		MeshSplinePoint = PathData.PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = PathData.PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (PathData.PathWidth * 0.5f));
		FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverData.RiverDepth);

		for (int32 y = 0; y < PathData.PathVertCount; y++)
		{
			MeshVert = PathMeshHelper(y, PathData.PathWidth, MeshEdge);

			PathData.PathVertices.Add(MeshVert);
			PathData.PathTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
		}
	}

	for (int i = 0; i < PathData.PathVertices.Num(); i++)
	{
		if (i < (SplineData.SplinePoints - 1) * (PathData.PathVertCount - 1))
		{
			TriangleCalcs(PathData.PathVertices, PathData.PathTriangles, PathData.PathVertCount, i);
		}
		NormalCalcs(PathData.PathVertices, PathData.PathNormals, PathData.PathVertCount, i);
	}
	PathData.PathMesh->CreateMeshSection(0, PathData.PathVertices, PathData.PathTriangles, PathData.PathNormals, TArray<FVector2D>(), TArray<FColor>(), PathData.PathTangents, true);

	// Set the material
	if (PathData.PathMaterial)
	{
		PathData.PathMesh->SetMaterial(0, PathData.PathMaterial);
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

void AProceduralLevelGenerator_CPP::UpdatePathMesh(const FPathData& PathDataStruct)
{
	MeshSplinePoint = PathData.PathSpline->GetLocationAtSplinePoint(SplineData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = PathData.PathSpline->GetTangentAtSplinePoint(SplineData.SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (PathData.PathWidth * 0.5f));
	FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverData.RiverDepth);

	for (int32 y = 0; y < PathData.PathVertCount; y++)
	{
		MeshVert = PathMeshHelper(y, PathData.PathWidth, MeshEdge);

		PathData.PathVertices.Add(MeshVert);
		PathData.PathTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
	}

	for (int i = 0; i < PathData.PathVertCount; i++)
	{
		NormalCalcs(PathData.PathVertices, PathData.PathNormals, PathData.PathVertCount, (PathData.PathVertices.Num() - PathData.PathVertCount) + i);
	}
	PathData.PathMesh->UpdateMeshSection(0, PathData.PathVertices, PathData.PathNormals, TArray<FVector2D>(), TArray<FColor>(), PathData.PathTangents);
}

FVector AProceduralLevelGenerator_CPP::PathMeshHelper(int y, int MeshWidth, FVector& MeshEdge)
{
	offsetCalc = ((MeshWidth / 2.0f) * (y / ((PathData.PathVertCount - 1.0f) / 2.0f)));

	MeshVert = MeshEdge + (MeshEdgeVector * offsetCalc);

	NoiseValue = NoiseData.Noise.GetNoise(MeshVert.X, MeshVert.Y);
	NoiseData.Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

	if (y == 0)
	{
		FVector RiverSplinePoint = MeshVert - (MeshEdgeVector * (RiverData.RiverWidth / 2));
		RiverData.RiverSpline->AddSplinePoint(RiverSplinePoint, ESplineCoordinateSpace::Local, false);

		while (RiverData.RiverSpline->GetNumberOfSplinePoints() > SplineData.SplinePoints)
		{
			// Remove first spline point to keep length constant
			RiverData.RiverSpline->RemoveSplinePoint(0, false);
			RiverData.RiverVertices.RemoveAt(0, RiverData.RiverVertCount);
			RiverData.VertexColours.RemoveAt(0, RiverData.RiverVertCount);
			RiverData.RiverNormals.RemoveAt(0, RiverData.RiverVertCount);
			RiverData.RiverTangents.RemoveAt(0, RiverData.RiverVertCount);
		}
	}
	else if (y < PathData.PathVertCount / 4)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, NoiseData.Falloff);
		float SmoothFactor = FMath::Pow(0.85f + 0.15f * FMath::Cos((y / (float)PathData.PathVertCount) * PI), 2.5f);

		if (MeshSplineTangent.X - MeshSplineTangent.Y > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (NoiseData.NoiseAmplitude);
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (30.0f) * SmoothFactor;
		}
		else if (MeshSplineTangent.Y - MeshSplineTangent.X > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (30.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (NoiseData.NoiseAmplitude);
		}
		else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (20.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (20.0f) * SmoothFactor;
		}
		MeshVert.Z += (depthOffset * y);
	}
	else if (y == PathData.PathVertCount / 4)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, NoiseData.Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;
		NoiseData.Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * NoiseData.Falloff) * NoiseData.NoiseAmplitude;
	}
	else if (y > PathData.PathVertCount / 4 && y < PathData.PathVertCount - (PathData.PathVertCount / 4) - 1)
	{
		NoiseData.Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * NoiseData.Falloff) * NoiseData.NoiseAmplitude;
	}
	else if (y == PathData.PathVertCount - (PathData.PathVertCount / 4) - 1)
	{ 
		WallData.WallSpline->AddSplinePoint(MeshVert + FVector(0.0f, 0.0f, RiverData.RiverDepth * 0.9f), ESplineCoordinateSpace::Local, false);

		while (WallData.WallSpline->GetNumberOfSplinePoints() > SplineData.SplinePoints)
		{
			WallData.WallSpline->RemoveSplinePoint(0, false);
		}

		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, NoiseData.Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;
		NoiseData.Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * NoiseData.Falloff) * NoiseData.NoiseAmplitude;
	}
	else
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, NoiseData.Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;
		MeshVert.Z += (depthOffset * ((PathData.PathVertCount - 1) - y));
	}
	RiverData.RiverSpline->UpdateSpline();
	WallData.WallSpline->UpdateSpline();
	return MeshVert;
}

void AProceduralLevelGenerator_CPP::GenerateRiverMesh(const FRiverData& RiverDStruct)
{
	for (int32 i = 0; i < SplineData.SplinePoints; i++)
	{
		MeshSplinePoint = RiverData.RiverSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = RiverData.RiverSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

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

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (RiverData.RiverWidth / 1.8f));
		FVector MoundSplinePoint = (MeshSplinePoint - (MeshEdgeVector * (RiverData.RiverWidth / 2.0f))) - (MeshEdgeVector * (MoundData.MoundWidth / 2));
		MoundData.MoundSpline->AddSplinePoint(MoundSplinePoint, ESplineCoordinateSpace::Local, false);

		for (int32 y = 0; y < RiverData.RiverVertCount; y++)
		{
			offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((RiverData.RiverVertCount - 1.0f) / 2.0f) * y;
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);

			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			MeshVert.Z += 10.0f;

			RiverData.RiverVertices.Add(MeshVert);

			RiverData.RiverTangents.Add(MeshTangent);
			RiverData.VertexColours.Add(FLinearColor(TangentX.X, TangentX.Y, 0.5f, 1.0f));
		}

	}

	MoundData.MoundSpline->UpdateSpline();

	for (int i = 0; i < RiverData.RiverVertices.Num(); i++)
	{
		if (i < (SplineData.SplinePoints - 1) * (RiverData.RiverVertCount - 1))
		{
			TriangleCalcs(RiverData.RiverVertices, RiverData.RiverTriangles, RiverData.RiverVertCount, i);
		}
		NormalCalcs(RiverData.RiverVertices, RiverData.RiverNormals, RiverData.RiverVertCount, i);
	}
	RiverData.RiverMesh->CreateMeshSection_LinearColor(0, RiverData.RiverVertices, RiverData.RiverTriangles, RiverData.RiverNormals, TArray<FVector2D>(), RiverData.VertexColours, RiverData.RiverTangents, true, false);

	// Set the material
	if (RiverData.RiverMaterial)
	{
		RiverData.RiverMesh->SetMaterial(0, RiverData.RiverMaterial);
	}
}

void AProceduralLevelGenerator_CPP::UpdateRiverMesh(const FRiverData& RiverDataStruct)
{
	MeshSplinePoint = RiverData.RiverSpline->GetLocationAtSplinePoint(SplineData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = RiverData.RiverSpline->GetTangentAtSplinePoint(SplineData.SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

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

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (RiverData.RiverWidth / 1.8f));

	FVector MoundSplinePoint = (MeshSplinePoint - (MeshEdgeVector * (RiverData.RiverWidth / 2.0f))) - (MeshEdgeVector * (MoundData.MoundWidth / 2));
	MoundData.MoundSpline->AddSplinePoint(MoundSplinePoint, ESplineCoordinateSpace::Local, false);

	while (MoundData.MoundSpline->GetNumberOfSplinePoints() > SplineData.SplinePoints)
	{
		// Remove first spline point to keep length constant
		MoundData.MoundSpline->RemoveSplinePoint(0, true);
		MoundData.MoundVertices.RemoveAt(0, MoundData.MoundVertCount);
		MoundData.MoundNormals.RemoveAt(0, MoundData.MoundVertCount);
		MoundData.MoundTangents.RemoveAt(0, MoundData.MoundVertCount);
	}

	for (int32 y = 0; y < RiverData.RiverVertCount; y++)
	{
		offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((RiverData.RiverVertCount - 1.0f) / 2.0f) * y;
		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
		
		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
		MeshVert.Z += 10.0f;

		RiverData.RiverVertices.Add(MeshVert);

		RiverData.RiverTangents.Add(MeshTangent);
		RiverData.VertexColours.Add(FLinearColor(TangentX.X, TangentX.Y, 0.5f, 1.0f));
	}

	for (int i = 0; i < RiverData.RiverVertCount; i++)
	{
		NormalCalcs(RiverData.RiverVertices, RiverData.RiverNormals, RiverData.RiverVertCount, (RiverData.RiverVertices.Num() - RiverData.RiverVertCount) + i);
	}
	RiverData.RiverMesh->UpdateMeshSection_LinearColor(0, RiverData.RiverVertices, RiverData.RiverNormals, TArray<FVector2D>(), RiverData.VertexColours, RiverData.RiverTangents);
}

void AProceduralLevelGenerator_CPP::SplineRegulator(USplineComponent* GuideSpline)
{
	TArray<FVector> SplineArray;

	float TotalLength = GuideSpline->GetSplineLength(); // Get full spline length
	float TargetSpacing = TotalLength / (SplineData.SplinePoints - 1); // Keep equal length

	for (int i = 0; i < SplineData.SplinePoints; i++)
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

void AProceduralLevelGenerator_CPP::GenerateMoundMesh(const FMoundData& MoundDataStruct)
{
	for (int32 i = 0; i < SplineData.SplinePoints; i++)
	{
		MeshSplinePoint = MoundData.MoundSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = MoundData.MoundSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MoundData.MoundWidth / 2.0f);

		float zValue = MoundData.MoundHeight;
		int verts75 = MoundData.MoundVertCount * 0.75f;
		int verts25 = MoundData.MoundVertCount - verts75;
		float zOffset75 = MoundData.MoundHeight / verts75 + 1;
		float zOffset25 = zOffset75 / verts25;

		for (int32 y = 0; y < MoundData.MoundVertCount; y++)
		{
			if (y < MoundData.MoundVertCount * 0.25f)
			{
				MeshVert = MoundMeshHelper(y,RiverData.RiverWidth, zValue, zOffset25, SplineData.SplinePoints - 1);
			}
			else
			{
				MeshVert = MoundMeshHelper(y,RiverData.RiverWidth, zValue, zOffset75, SplineData.SplinePoints - 1);
			}

			MoundData.MoundVertices.Add(MeshVert);
			MoundData.MoundTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
		}
	}

	for (int i = 0; i < MoundData.MoundVertices.Num(); i++)
	{
		if (i < (SplineData.SplinePoints - 1) * (MoundData.MoundVertCount - 1))
		{
			TriangleCalcs(MoundData.MoundVertices, MoundData.MoundTriangles, MoundData.MoundVertCount, i);
		}
		NormalCalcs(MoundData.MoundVertices, MoundData.MoundNormals, MoundData.MoundVertCount, i);
	}

	MoundData.MoundMesh->CreateMeshSection(0, MoundData.MoundVertices, MoundData.MoundTriangles, MoundData.MoundNormals, TArray<FVector2D>(), TArray<FColor>(), MoundData.MoundTangents, true);

	// Set the material
	if (MoundData.MoundMaterial)
	{
		MoundData.MoundMesh->SetMaterial(0, MoundData.MoundMaterial);
	}
}

void AProceduralLevelGenerator_CPP::UpdateMoundMesh(const FMoundData& MoundDataStruct)
{
	SplineRegulator(MoundData.MoundSpline);

	MeshSplinePoint = MoundData.MoundSpline->GetLocationAtSplinePoint(SplineData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = MoundData.MoundSpline->GetTangentAtSplinePoint(SplineData.SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MoundData.MoundWidth / 2.0f);

	float zValue = MoundData.MoundHeight;
	int verts75 = MoundData.MoundVertCount * 0.75f;
	int verts25 = MoundData.MoundVertCount - verts75;
	float zOffset75 = MoundData.MoundHeight / verts75 + 1;
	float zOffset25 = zOffset75 / verts25;

	for (int32 y = 0; y < MoundData.MoundVertCount; y++)
	{
		if (y < MoundData.MoundVertCount * 0.25f)
		{
			MeshVert = MoundMeshHelper(y, MoundData.MoundWidth, zValue, zOffset25, SplineData.SplinePoints - 1);
		}
		else
		{
			MeshVert = MoundMeshHelper(y, MoundData.MoundWidth, zValue, zOffset75, SplineData.SplinePoints - 1);
		}
		MoundData.MoundVertices.Add(MeshVert);
		MoundData.MoundTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
	}

	for (int i = 0; i < MoundData.MoundVertCount; i++)
	{
		NormalCalcs(MoundData.MoundVertices, MoundData.MoundNormals, MoundData.MoundVertCount, (MoundData.MoundVertices.Num() - MoundData.MoundVertCount) + i);
	}
	MoundData.MoundMesh->UpdateMeshSection(0, MoundData.MoundVertices, MoundData.MoundNormals, TArray<FVector2D>(), TArray<FColor>(), MoundData.MoundTangents);
}

FVector AProceduralLevelGenerator_CPP::MoundMeshHelper(int y, int MeshWidth, float& zValue, float zOffset, int i)
{
	offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((MoundData.MoundVertCount - 1.0f) / 2.0f) * y;
	MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc) + FVector(0.0f, 0.0f, zValue);

	NoiseValue = NoiseData.Noise.GetNoise(MeshVert.X, MeshVert.Y);
	NoiseData.Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 1.25f);

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
	else if (y < (MoundData.MoundVertCount - 2) * 0.25f)
	{
		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundData.MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * NoiseData.Falloff) * 30.0f * SmoothFactor;

		zValue -= zOffset;
	}
	else if (y == (MoundData.MoundVertCount - 2) * 0.25f)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, NoiseData.Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseData.NoiseAmplitude;

		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundData.MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * NoiseData.Falloff) * NoiseData.NoiseAmplitude * SmoothFactor;

		zValue -= zOffset;
	}
	else 
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, NoiseData.Falloff);
		float SmoothFactor = FMath::Pow(0.85f + 0.15f * FMath::Cos((y / (float)MoundData.MoundVertCount) * PI), 2.5f);

		if (MeshSplineTangent.X - MeshSplineTangent.Y > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (NoiseData.NoiseAmplitude);
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (50.0f) * SmoothFactor;
		}
		else if (MeshSplineTangent.Y - MeshSplineTangent.X > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (50.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (NoiseData.NoiseAmplitude);
		}
		else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
		}

		zValue -= zOffset;
	}
	return MeshVert;
}

void AProceduralLevelGenerator_CPP::SpawnWall(USplineComponent* GuideSpline)
{
	if (!GuideSpline || !WallData.WallMesh1 || !WallData.WallMesh2) return;

	const float SplineLength = GuideSpline->GetSplineLength();
	int32 NumSegments = FMath::FloorToInt(SplineLength / MeshLength);

	while (EndDistance < SplineLength)
	{
		// Randomly select between the two meshes
		bool WallType = FMath::RandBool();
		UStaticMesh* SelectedMesh = (WallType) ? WallData.WallMesh1 : WallData.WallMesh2;
		//UMaterialInterface* SelectedMaterial = (WallType) ? WallMaterial1 : WallMaterial2;
		float SelectedMeshLength = SelectedMesh->GetBounds().BoxExtent.X * 2;

		if (StartDistance == 0.0f)
		{
			SplinePointCount = FMath::FloorToInt(SelectedMeshLength / SplineData.PlaneDistance);
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
	UStaticMesh* SelectedMesh = (WallType) ? WallData.WallMesh1 : WallData.WallMesh2;
	//UMaterialInterface* SelectedMaterial = (WallType) ? WallMaterial1 : WallMaterial2;
	float SelectedMeshLength = SelectedMesh->GetBounds().BoxExtent.X * 2;


	SplinePointCount = FMath::FloorToInt(SelectedMeshLength / SplineData.PlaneDistance);

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