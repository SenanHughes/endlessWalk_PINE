// Fill out your copyright notice in the Description page of Project Settings.

#include "uProcTerrainGenerator_CPP.h"

// Sets default values
AuProcTerrainGenerator_CPP::AuProcTerrainGenerator_CPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the spline component
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	RootComponent = PathSpline;
	
	RiverSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RiverSpline"));
	RiverSpline->SetupAttachment(RootComponent);
	
	MoundSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoundSpline"));
	MoundSpline->SetupAttachment(RootComponent);
	
	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathMesh->SetupAttachment(RootComponent);

	RiverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));
	RiverMesh->SetupAttachment(RootComponent);

	MoundMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MoundMesh"));
	MoundMesh->SetupAttachment(RootComponent);

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	InstancedMesh->SetMobility(EComponentMobility::Movable);
	InstancedMesh->SetupAttachment(RootComponent);

	InstancedMesh2 = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh2"));
	InstancedMesh2->SetMobility(EComponentMobility::Movable);
	InstancedMesh2->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AuProcTerrainGenerator_CPP::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the spline is correctly set up
	if (PathSpline)
	{
		// Clear any previous spline points
		PathSpline->ClearSplinePoints();
		RiverSpline->ClearSplinePoints();
		MoundSpline->ClearSplinePoints();

		Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		Noise.SetFrequency(NoiseFrequency);

		for (int32 i = 0; i < SplinePoints; i++)
		{
			// Calculate position for each point
			FVector NewPoint((i * PlaneDistance) - (SplinePoints * PlaneDistance) / 2, FMath::Sin(i * (PI / ((SplinePoints / 2) - 1))) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		PathSpline->UpdateSpline();
	}

	//GeneratePathMesh
	GeneratePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathUVScale, PathMaterial, PathVertCount);
	

	RiverOffset = RiverWidth / 2;

	//GenerateRiverMesh
	GenerateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverUVScale, RiverMaterial, RiverVertCount);

	//GenerateMoundMesh
	GenerateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundUVs, MoundTriangles, MoundUVScale, MoundMaterial, MoundVertCount);
	
	SpawnProceduralAssets();
}

// Called every frame
void AuProcTerrainGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static FVector CharacterLocation = FVector::ZeroVector;
	static FVector MidPoint = FVector::ZeroVector;

	CharacterLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	MidPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints / 2, ESplineCoordinateSpace::Local);

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathWidth / 2)
	{
		PointCount += 1.0f;
		if (PointCount >= 40.0f)
		{
			ChangeCurve = true;
		}
		UpdateTerrainSpline();
	}
}

void AuProcTerrainGenerator_CPP::UpdateTerrainSpline()
{	
	FVector LastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	FVector SecondLastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 2, ESplineCoordinateSpace::Local);

	FVector Direction = LastPoint - SecondLastPoint;
	Direction.Normalize();
		
	static float CurrentCurveDirectionX = 0.0f;
	static float CurrentCurveDirectionY = 0.0f;

	static float CurveSpeedMultiplier = 0.1f;
	static float SharpnessMultiplier = 50.0f;
	static float FrequencyMultiplier = 0.003f;
	static float TimeElapsed = 0.0f;

	TimeElapsed += GetWorld()->GetDeltaSeconds(); // Track time progression

	// Every 3-6 seconds, randomise curve behavior
	if (ChangeCurve)
	{
		CurveSpeedMultiplier = FMath::RandRange(0.25f, 0.5f); // Makes turns speed up/slow down
		SharpnessMultiplier = FMath::RandRange(25.0f, 45.0f); // Adjusts how sharp turns can be
		FrequencyMultiplier = FMath::RandRange(0.002f, 0.005f); // Adjusts how frequent turns occur
		ChangeCurve = false;
		PointCount = 0;
	}

	// Generate Perlin noise with the dynamically changing frequency
	float NoiseValueX = FMath::PerlinNoise1D(LastPoint.X * FrequencyMultiplier) * SharpnessMultiplier;
	float NoiseValueY = FMath::PerlinNoise1D(LastPoint.Y * FrequencyMultiplier) * SharpnessMultiplier;

	// === Detect If the Path is Too Straight (Both X & Y) ===
	float AverageDeviation = 0.0f;
	bool IsTooStraight = false;

	CurrentCurveDirectionX = FMath::Lerp(CurrentCurveDirectionX, NoiseValueX, 0.1f * CurveSpeedMultiplier);
	CurrentCurveDirectionY = FMath::Lerp(CurrentCurveDirectionY, NoiseValueY, 0.1f * CurveSpeedMultiplier);

	FVector NewPoint = LastPoint + (Direction * PlaneDistance);
	NewPoint.X += CurrentCurveDirectionX;
	NewPoint.Y += CurrentCurveDirectionY;
	NewPoint.Z = 0.0f; // Keep terrain flat

	// Add the new point to the spline
	PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);

	while (PathSpline->GetNumberOfSplinePoints() > SplinePoints)
	{
		// Remove first spline point to keep length constant
		PathSpline->RemoveSplinePoint(0);
		PathVertices.RemoveAt(0, PathVertCount);
		PathUVs.RemoveAt(0, PathVertCount);
	}

	// Update spline with new points
	PathSpline->UpdateSpline();

	UpdatePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathUVScale, PathMaterial, PathVertCount);

	//GenerateRiverMesh();
	UpdateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverUVScale, RiverMaterial, RiverVertCount);

	//GenerateMoundMesh();
	UpdateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundUVs, MoundTriangles, MoundUVScale, MoundMaterial, MoundVertCount);

	UpdateProceduralAssets();
}


void AuProcTerrainGenerator_CPP::GeneratePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int PathVerts)
{
	/// split into two functions - generate mesh and update mesh
	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));
		FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverDepth * 2.0f);

		for (int32 y = 0; y < PathVertCount; y++)
		{
			offsetCalc = ((MeshWidth / 2.0f) * (y / ((PathVertCount - 1.0f) / 2.0f)));
			depthOffset = (RiverDepth * 2.0f) * (y / 2.0f);

			MeshVert = MeshEdge + (MeshEdgeVector * offsetCalc);

			// MeshUVs
			MeshVertU = MeshVert.X / MeshUVScale;
			MeshVertV = MeshVert.Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));

			NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
			Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

			if (depthOffset == RiverDepth)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Z += depthOffset;

				RiverSpline->AddSplinePoint(MeshVert, ESplineCoordinateSpace::Local);
			}
			else if (y < PathVertCount / 4)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Z += depthOffset;
			}
			else if (y == PathVertCount / 4)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
				MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
			}
			else if (y < PathVertCount - 1)
			{
				Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
				MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
			}
			else
			{
				MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
				MeshVert.Z = -RiverDepth * 1.1f;
			}
			MeshVertices.Add(MeshVert);
		}
	}

	for (int i = 1; i < SplinePoints; i++)
	{
		for (int32 y = 0; y < PathVertCount - 1; y++)
		{
			int var = (i - 1) * PathVertCount;
			MeshTriangles.Add(var + y);
			MeshTriangles.Add(var + y + 1);
			MeshTriangles.Add(var + y + PathVertCount + 1);
			MeshTriangles.Add(var + y);
			MeshTriangles.Add(var + y + PathVertCount + 1);
			MeshTriangles.Add(var + y + PathVertCount);
		}
	}

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}

void AuProcTerrainGenerator_CPP::UpdatePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int PathVerts)
{
	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));
	FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverDepth * 2.0f);

	for (int32 y = 0; y < PathVertCount; y++)
	{
		offsetCalc = ((MeshWidth / 2.0f) * (y / ((PathVertCount - 1.0f) / 2.0f)));
		depthOffset = (RiverDepth * 2.0f) * (y / 2.0f);

		MeshVert = MeshEdge + (MeshEdgeVector * offsetCalc);

		// MeshUVs
		MeshVertU = MeshVert.X / MeshUVScale;
		MeshVertV = MeshVert.Y / MeshUVScale;
		MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));

		NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
		Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

		if (depthOffset == RiverDepth)
		{
			float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
			MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Z += depthOffset;

			RiverSpline->AddSplinePoint(MeshVert, ESplineCoordinateSpace::Local);

			while (RiverSpline->GetNumberOfSplinePoints() > SplinePoints)
			{
				// Remove first spline point to keep length constant
				RiverSpline->RemoveSplinePoint(0, true);
				RiverVertices.RemoveAt(0, RiverVertCount);
				RiverUVs.RemoveAt(0, RiverVertCount);
			}
		}
		else if (y < PathVertCount / 4)
		{
			float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
			MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Z += depthOffset;
		}
		else if (y == PathVertCount / 4)
		{
			float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
			MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
			MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
		}
		else if (y < PathVertCount - 1)
		{
			Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
			MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
		}
		else
		{
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			MeshVert.Z = -RiverDepth * 1.1f;
		}
		MeshVertices.Add(MeshVert);
	}
	ProcMesh->UpdateMeshSection(0, MeshVertices, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>());
}

void AuProcTerrainGenerator_CPP::GenerateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int RiverVerts)
{
	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth);
		MoundSpline->AddSplinePoint(MeshLeftEdge, ESplineCoordinateSpace::Local);

		for (int32 y = 0; y < RiverVertCount; y++)
		{
			offsetCalc = ((MeshWidth / 2.0f) * (y / ((RiverVertCount - 1.0f) / 2.0f)));
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			MeshVertices.Add(MeshVert);

			// MeshUVs
			MeshVertU = MeshVert.X / MeshUVScale;
			MeshVertV = MeshVert.Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
		}
	}

	for (int i = 1; i < SplinePoints; i++)
	{
		for (int32 y = 0; y < RiverVertCount - 1; y++)
		{
			int var = (i - 1) * RiverVertCount;
			MeshTriangles.Add(var + y);
			MeshTriangles.Add(var + y + 1);
			MeshTriangles.Add(var + y + RiverVertCount + 1);
			MeshTriangles.Add(var + y);
			MeshTriangles.Add(var + y + RiverVertCount + 1);
			MeshTriangles.Add(var + y + RiverVertCount);
		}
	}

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	
	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}

void AuProcTerrainGenerator_CPP::UpdateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int RiverVerts)
{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
		MeshSplineTangent = PathSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth);
		MoundSpline->AddSplinePoint(MeshLeftEdge, ESplineCoordinateSpace::Local);

		while (MoundSpline->GetNumberOfSplinePoints() > SplinePoints)
		{
			// Remove first spline point to keep length constant
			MoundSpline->RemoveSplinePoint(0, true);
			MoundVertices.RemoveAt(0, MoundVertCount);
			MoundUVs.RemoveAt(0, MoundVertCount);
		}

		for (int32 y = 0; y < RiverVertCount; y++)
		{
			offsetCalc = ((MeshWidth / 2.0f) * (y / ((RiverVertCount - 1.0f) / 2.0f)));
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			MeshVertices.Add(MeshVert);

			//UVs
			MeshVertU = MeshVert.X / MeshUVScale;
			MeshVertV = MeshVert.Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
		}
		ProcMesh->UpdateMeshSection(0, MeshVertices, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>());
}

void AuProcTerrainGenerator_CPP::GenerateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int MoundVerts)
{
	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth);
		float zValue = zHeight;
		float valueting = (MoundVertCount - 1.0f) - (MoundVertCount / 3.0f);
		float zOffset = zHeight / valueting;

		for (int32 y = 0; y < MoundVertCount; y++)
		{
			offsetCalc = ((MeshWidth / 2.0f) * (y / ((MoundVertCount - 1.0f) / 2.0f)));

			// MeshUVs
			MeshVertU = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).X / MeshUVScale;
			MeshVertV = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
			
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc) + FVector(0.0f, 0.0f, zValue);

			NoiseValue = (Noise.GetNoise(MeshVert.X, MeshVert.Y) + Noise.GetNoise(MeshVert.X + 10, MeshVert.Y + 10)) / 2.0f;
			Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
			
			if (y < MoundVertCount / 3.0f)
			{
				float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
				MeshVert.Z += (NoiseValue * Falloff) * NoiseAmplitude * SmoothFactor;

				zValue -= (zOffset / (MoundVertCount / 3.0f));
			}
			else if (y == MoundVertCount / 3.0f)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude; 
				
				float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
				MeshVert.Z += (NoiseValue * Falloff) * NoiseAmplitude * SmoothFactor;

				zValue -= zOffset;
			}
			else if (y >= MoundVertCount / 3.0f && y < MoundVertCount - 1)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

				zValue -= zOffset;
			}
			else
			{
				MeshVert = MeshSplinePoint;
			}

			MeshVertices.Add(MeshVert);
		}
	}

	for (int i = 1; i < SplinePoints; i++)
	{
		for (int32 y = 0; y < MoundVertCount - 1; y++)
		{
			int var = (i - 1) * MoundVertCount;
			MeshTriangles.Add(var + y);
			MeshTriangles.Add(var + y + 1);
			MeshTriangles.Add(var + y + MoundVertCount + 1);
			MeshTriangles.Add(var + y);
			MeshTriangles.Add(var + y + MoundVertCount + 1);
			MeshTriangles.Add(var + y + MoundVertCount);
		}
	}

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}

void AuProcTerrainGenerator_CPP::UpdateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int MoundVerts)
{
	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = PathSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth);
	float zValue = zHeight;
	float valueting = (MoundVertCount - 1.0f) - (MoundVertCount / 3.0f);
	float zOffset = zHeight / valueting;

	for (int32 y = 0; y < MoundVertCount; y++)
	{
		offsetCalc = ((MeshWidth / 2.0f) * (y / ((MoundVertCount - 1.0f) / 2.0f)));

		// MeshUVs
		MeshVertU = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).X / MeshUVScale;
		MeshVertV = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).Y / MeshUVScale;
		MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));

		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc) + FVector(0.0f, 0.0f, zValue);

		NoiseValue = (Noise.GetNoise(MeshVert.X, MeshVert.Y) + Noise.GetNoise(MeshVert.X + 10, MeshVert.Y + 10)) / 2.0f;
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);

		if (y < MoundVertCount / 3.0f)
		{
			float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
			MeshVert.Z += (NoiseValue * Falloff) * NoiseAmplitude * SmoothFactor;

			zValue -= (zOffset / (MoundVertCount / 3.0f));
		}
		else if (y == MoundVertCount / 3.0f)
		{
			float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
			MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

			float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
			MeshVert.Z += (NoiseValue * Falloff) * NoiseAmplitude * SmoothFactor;

			zValue -= zOffset;
		}
		else if (y >= MoundVertCount / 3.0f && y < MoundVertCount - 1)
		{
			float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
			MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

			zValue -= zOffset;
		}
		else
		{
			MeshVert = MeshSplinePoint;
		}

		MeshVertices.Add(MeshVert);
	}
	ProcMesh->UpdateMeshSection(0, MeshVertices, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>());
}

void AuProcTerrainGenerator_CPP::SpawnProceduralAssets()
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
				FVector SplineLocation = PathSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
				FVector SplineRightVector = PathSpline->GetRightVectorAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

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

void AuProcTerrainGenerator_CPP::UpdateProceduralAssets()
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
			FVector SplineLocation = PathSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
			FVector SplineRightVector = PathSpline->GetRightVectorAtSplinePoint(DistanceAlongSpline, ESplineCoordinateSpace::World);

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