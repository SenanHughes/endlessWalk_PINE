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

	// Create the
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathSpline->SetupAttachment(RootComponent);
	PathSpline->SetCanEverAffectNavigation(false);

	RiverSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RiverSpline"));
	RiverSpline->SetupAttachment(RootComponent);
	RiverSpline->SetCanEverAffectNavigation(false);

	MoundSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoundSpline"));
	MoundSpline->SetupAttachment(RootComponent);
	MoundSpline->SetCanEverAffectNavigation(false);

	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathMesh->SetupAttachment(RootComponent);
	PathMesh->SetCanEverAffectNavigation(false);
	PathMesh->bUseAsyncCooking = true;
	PathMesh->ContainsPhysicsTriMeshData(true);

	RiverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));
	RiverMesh->SetupAttachment(RootComponent);
	RiverMesh->SetCanEverAffectNavigation(false);
	RiverMesh->bUseAsyncCooking = true;
	RiverMesh->ContainsPhysicsTriMeshData(true);

	MoundMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MoundMesh"));
	MoundMesh->SetupAttachment(RootComponent);
	MoundMesh->SetCanEverAffectNavigation(false);
	MoundMesh->bUseAsyncCooking = true;
	MoundMesh->ContainsPhysicsTriMeshData(true);

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	InstancedMesh->SetMobility(EComponentMobility::Movable);
	InstancedMesh->SetupAttachment(RootComponent);

	InstancedMesh2 = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh2"));
	InstancedMesh2->SetMobility(EComponentMobility::Movable);
	InstancedMesh2->SetupAttachment(RootComponent);
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

	//GeneratePathMesh
	//FMeshGeneration::GeneratePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathUVScale, PathMaterial, PathVertCount, SplinePoints, RiverDepth, NoiseAmplitude, RiverSpline);
	GeneratePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathNormals, PathTangents, PathTriangles, PathMaterial, PathVertCount);

	//GenerateRiverMesh();
	GenerateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverNormals, RiverTangents, RiverTriangles, DynamicRiverMaterial, RiverVertCount);

	//GenerateMoundMesh();
	GenerateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundNormals, MoundTangents, MoundTriangles, MoundMaterial, MoundVertCount);

	SpawnProceduralAssets();
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


	}

	LimitMovementSmoothly(PathSpline, PathWidth * 0.2f, 5.0f);
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
	if (ChangeCurve)
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
	}

	// Add the new point to the spline
	PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);

	// Remove first spline point to keep length constant
	PathSpline->RemoveSplinePoint(0, false);
	PathVertices.RemoveAt(0, PathVertCount);
	if (NewPathVerts.Num() > 9)
	{
		NewPathVerts.RemoveAt(0, PathVertCount);
	}
	PathNormals.RemoveAt(0, PathVertCount);
	PathTangents.RemoveAt(0, PathVertCount);

	// Update spline with new points
	PathSpline->UpdateSpline();

	UpdatePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathNormals, PathTangents, PathTriangles, PathMaterial, PathVertCount);

	//GenerateRiverMesh();
	UpdateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverNormals, RiverTangents, RiverTriangles, DynamicRiverMaterial, RiverVertCount);

	//GenerateMoundMesh();
	UpdateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundNormals, MoundTangents, MoundTriangles, MoundMaterial, MoundVertCount);

	UpdateProceduralAssets();
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
	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, MeshNormals, TArray<FVector2D>(), TArray<FColor>(), MeshTangents, true);
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
		else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
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
	else if (y > PathVertCount / 4 && y < PathVertCount - (PathVertCount / 4))
	{
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
	}
	else if (y == PathVertCount - (PathVertCount / 4) - 1)
	{ 
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

	for (int i = 0; i < RiverVerts; i++)
	{
		NormalCalcs(MeshVertices, MeshNormals, RiverVerts, (MeshVertices.Num() - RiverVerts) + i);
	}
	ProcMesh->CreateMeshSection_LinearColor(0, MeshVertices, MeshTriangles, MeshNormals, TArray<FVector2D>(), VertexColours, MeshTangents, false, false);
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

		float zValue = zHeight;
		int verts75 = MoundVerts * 0.75f;
		int verts25 = MoundVerts - verts75;
		float zOffset75 = zHeight / verts75 + 1;
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
	SplineRegulator(GuideSpline);

	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	//// Might need for later - if left turns too sharp
	/*float MeshDist = FVector::Dist(GuideSpline->GetLocationAtSplinePoint(SplinePoints - 2, ESplineCoordinateSpace::Local), MeshSplinePoint);
	UE_LOG(LogTemp, Warning, TEXT("Distance between new point and mound spline last point is %f"), MeshDist);

	UE_LOG(LogTemp, Warning, TEXT("Distance between new point and mound spline last point is %f"), FVector::Dist(MoundSplinePoint, MoundSpline->GetLocationAtSplinePoint(MoundSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local)));
	if (FVector::Dist(MoundSplinePoint, MoundSpline->GetLocationAtSplinePoint(MoundSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local)) < 50.0f)
	{
		//MoundSplinePoint += (MeshEdgeVector * 50.0f);
		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth * .4f));
		MoundSplinePoint = MeshLeftEdge - (MeshEdgeVector * (MoundWidth / 2));
		UE_LOG(LogTemp, Warning, TEXT("NEW Distance between new point and mound spline last point is %f"), FVector::Dist(MoundSplinePoint, MoundSpline->GetLocationAtSplinePoint(MoundSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local)));
	}*/

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth / 2.0f);

	float zValue = zHeight;
	int verts75 = MoundVerts * 0.75f;
	int verts25 = MoundVerts - verts75;
	float zOffset75 = zHeight / verts75 + 1;
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

	for (int i = 0; i < MoundVerts; i++)
	{
		NormalCalcs(MeshVertices, MeshNormals, MoundVerts, (MeshVertices.Num() - MoundVerts) + i);
	}
	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, MeshNormals, TArray<FVector2D>(), TArray<FColor>(), MeshTangents, true);
}

FVector AProceduralLevelGenerator_CPP::MoundMeshHelper(int y, int MeshWidth, float& zValue, float zOffset, int i)
{
	offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((MoundVertCount - 1.0f) / 2.0f) * y;
	MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc) + FVector(0.0f, 0.0f, zValue);

	NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
	Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 1.25f);

	if (y < MoundVertCount * 0.25f)
	{
		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * Falloff) * 30.0f * SmoothFactor;

		zValue -= zOffset;
	}
	else if (y == MoundVertCount * 0.25f)
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
		else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
		}

		zValue -= zOffset;
	}
	return MeshVert;
}

void AProceduralLevelGenerator_CPP::SpawnProceduralAssets()
{
	InstancedMesh->SetStaticMesh(AssetToSpawn);
	InstancedMesh2->SetStaticMesh(AssetToSpawn2);

	for (int32 i = 0; i < SplinePoints - 1; i++)
	{
		float StartDistance = PathSpline->GetDistanceAlongSplineAtSplinePoint(i);
		float EndDistance = PathSpline->GetDistanceAlongSplineAtSplinePoint(i + 1);
		float SegmentLength = EndDistance - StartDistance;
		PlacedLocations.Empty();

		for (int32 j = 0; j < NumAssetsToSpawn; j++)
		{
			bValidLocation = false;
			AttemptCount = 0;

			randCheck = rand() % 2;
			InstanceScale = FVector(FMath::FRandRange(0.2f, 0.5f));

			if (randCheck == 0)
			{
				OverlapCheck = AssetToSpawn->GetBoundingBox().GetSize() * InstanceScale;
				Instance = InstancedMesh;
			}
			else
			{
				OverlapCheck = AssetToSpawn2->GetBoundingBox().GetSize() * InstanceScale;;
				Instance = InstancedMesh2;
			}
			while (!bValidLocation && AttemptCount < MaxAttempts)
			{
				float DistanceAlongSpline = StartDistance + FMath::FRandRange(SegmentLength * 0.25f, SegmentLength * 0.75f);
				FVector SplineLocation = PathSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);
				FVector SplineRightVector = PathSpline->GetRightVectorAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);

				// Random offset within MaxOffsetFromSpline
				float LateralOffset = FMath::FRandRange(-(PathWidth / 2.0f), (PathWidth / 2.0f));
				SpawnLocation = SplineLocation + (SplineRightVector * LateralOffset);

				// Adjust height using line trace to prevent floating/buried objects
				FHitResult Hit;
				FVector TraceStart = SpawnLocation + FVector(0, 0, 200);
				FVector TraceEnd = SpawnLocation - FVector(0, 0, 500);
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);

				if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
				{
					SpawnLocation.Z = Hit.ImpactPoint.Z;
				}
				for (const FVector& ExistingLocation : PlacedLocations)
				{
					if (FVector::Dist(SpawnLocation, ExistingLocation) > OverlapCheck.X && FVector::Dist(SpawnLocation, ExistingLocation) > OverlapCheck.Y)
					{
						bValidLocation = true;
					}
				}
				if (PlacedLocations.Num() == 0)
				{
					bValidLocation = true;
				}
				AttemptCount++;
			}
			if (bValidLocation)
			{
				PlacedLocations.Add(SpawnLocation);  // Save location to prevent future overlaps
				Instance->AddInstance(FTransform(FRotator::ZeroRotator, SpawnLocation, InstanceScale));
				Instance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

void AProceduralLevelGenerator_CPP::UpdateProceduralAssets()
{
	float StartDistance = PathSpline->GetDistanceAlongSplineAtSplinePoint(SplinePoints - 2);
	float EndDistance = PathSpline->GetDistanceAlongSplineAtSplinePoint(SplinePoints - 1);
	float SegmentLength = EndDistance - StartDistance;
	PlacedLocations.Empty();

	for (int32 j = 0; j < NumAssetsToSpawn; j++)
	{
		bValidLocation = false;
		AttemptCount = 0;

		randCheck = rand() % 2;
		InstanceScale = FVector(FMath::FRandRange(0.2f, 0.5f));

		if (randCheck == 0)
		{
			OverlapCheck = AssetToSpawn->GetBoundingBox().GetSize() * InstanceScale;
			Instance = InstancedMesh;
		}
		else
		{
			OverlapCheck = AssetToSpawn2->GetBoundingBox().GetSize() * InstanceScale;;
			Instance = InstancedMesh2;
		}
		while (!bValidLocation && AttemptCount < MaxAttempts)
		{
			float DistanceAlongSpline = StartDistance + FMath::FRandRange(SegmentLength * 0.25f, SegmentLength * 0.75f);
			FVector SplineLocation = PathSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);
			FVector SplineRightVector = PathSpline->GetRightVectorAtSplinePoint(DistanceAlongSpline, ESplineCoordinateSpace::Local);

			// Random offset within MaxOffsetFromSpline
			float LateralOffset = FMath::FRandRange(-(PathWidth / 2.0f), (PathWidth / 2.0f));
			SpawnLocation = SplineLocation + (SplineRightVector * LateralOffset);

			// Adjust height using line trace to prevent floating/buried objects
			FHitResult Hit;
			FVector TraceStart = SpawnLocation + FVector(0, 0, 200);
			FVector TraceEnd = SpawnLocation - FVector(0, 0, 500);
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);

			if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
			{
				SpawnLocation.Z = Hit.ImpactPoint.Z;
			}
			for (const FVector& ExistingLocation : PlacedLocations)
			{
				if (FVector::Dist(SpawnLocation, ExistingLocation) > OverlapCheck.X && FVector::Dist(SpawnLocation, ExistingLocation) > OverlapCheck.Y)
				{
					bValidLocation = true;
				}
			}
			if (PlacedLocations.Num() == 0)
			{
				bValidLocation = true;
			}
			AttemptCount++;
		}
		if (bValidLocation)
		{
			PlacedLocations.Add(SpawnLocation);  // Save location to prevent future overlaps
			Instance->AddInstance(FTransform(FRotator::ZeroRotator, SpawnLocation, InstanceScale));
			Instance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Instance->RemoveInstance(0);
		}
	}
}