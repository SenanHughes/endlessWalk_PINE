// Fill out your copyright notice in the Description page of Project Settings.

#include "uProcTerrainGenerator_CPP.h"
#include "FastNoiseLite.h"

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
	
	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathMesh->SetupAttachment(RootComponent);

	RiverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));
	RiverMesh->SetupAttachment(RootComponent);
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

		for (int32 i = 0; i < SplinePoints; i++)
		{
			// Calculate position for each point
			FVector NewPoint((i * PlaneDistance) - (SplinePoints * PlaneDistance)/2, FMath::Sin(i * 0.1f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		PathSpline->UpdateSpline();
	}
	
	CreateRiverSpline();

	//GeneratePathMesh
	GenerateProcMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathNoiseBool, PathUVScale, PathMaterial);
	//GenerateRiverMesh
	GenerateProcMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverNoiseBool, RiverUVScale, RiverMaterial);
}

// Called every frame
void AuProcTerrainGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTerrainSpline();
}

void AuProcTerrainGenerator_CPP::GenerateProcMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices, 
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial)
{
	static FVector MeshVert;
	static FVector MeshLeftEdge;
	static FVector MeshSplinePoint;
	static FVector MeshSplineTangent;
	static FVector MeshEdgeVector;

	static float NoiseValue;
	static float offsetCalc;
	static float MeshVertU;
	static float MeshVertV;

	static FastNoiseLite Noise;

	Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise.SetFrequency(NoiseFrequency);

	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));

		for (int32 y = 0; y < VertCount; y++)
		{
			if (MeshVertices.Num() < SplinePoints * VertCount)
			{
				// Vertice Position & Noise
				
				offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
				MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
				if (NoiseRequired && (0 < y) && (y < VertCount - 1))
				{
					NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
					MeshVert.Z = NoiseValue * NoiseAmplitude;
				}
				MeshVertices.Add(MeshVert);

				// MeshUVs
				MeshVertU = MeshVert.X / MeshUVScale;
				MeshVertV = MeshVert.Y / MeshUVScale;
				MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
			}
			else
			{
				int var = i * VertCount;
				if (i < SplinePoints - 1)
				{
					MeshVertices[var + y] = MeshVertices[var + y + (VertCount)];
				}
				else
				{
					offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
					MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
					if (NoiseRequired && (0 < y) && (y < VertCount - 1))
					{
						NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
						MeshVert.Z = NoiseValue * NoiseAmplitude;
					}
					MeshVertices[var + y] = MeshVert;

					//UVs
					MeshVertU = MeshVert.X / MeshUVScale;
					MeshVertV = MeshVert.Y / MeshUVScale;
					MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
				}
			}
		}
	}
	MeshTriangles.Empty();
	if (MeshTriangles.Num() < (((SplinePoints - 1) * (VertCount-1)) * 6))
	{
		for (int i = 1; i < SplinePoints; i++)
		{
			for (int32 y = 0; y < VertCount - 1; y++)
			{
				int var = (i - 1) * VertCount;
				MeshTriangles.Add(var + y);
				MeshTriangles.Add(var + y + 1);
				MeshTriangles.Add(var + y + VertCount + 1);
				MeshTriangles.Add(var + y);
				MeshTriangles.Add(var + y + VertCount + 1);
				MeshTriangles.Add(var + y + VertCount);
			}
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

void AuProcTerrainGenerator_CPP::UpdateTerrainSpline()
{
	if (SplinePoints < 2)
	{
		return;
	}

	FVector CharacterLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector MidPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints / 2, ESplineCoordinateSpace::Local);

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathWidth / 2)
	{
		FVector LastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
		FVector SecondLastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 2, ESplineCoordinateSpace::Local);

		FVector Direction = LastPoint - SecondLastPoint;
		Direction.Normalize();
		
		static float CurrentCurveDirectionX = 0.0f;
		static float CurrentCurveDirectionY = 0.0f;

		static float CurveSpeedMultiplier = 0.1f;
		static float SharpnessMultiplier = 100.0f;
		static float FrequencyMultiplier = 0.005f;
		static float TimeElapsed = 0.0f;

		TimeElapsed += GetWorld()->GetDeltaSeconds(); // Track time progression

		// Every 3-6 seconds, randomise curve behavior
		if (FMath::Fmod(TimeElapsed, FMath::RandRange(6.0f, 10.0f)) < GetWorld()->GetDeltaSeconds())
		{
			CurveSpeedMultiplier = FMath::RandRange(0.5f, 1.0f); // Makes turns speed up/slow down
			SharpnessMultiplier = FMath::RandRange(100.0f, 200.0f); // Adjusts how sharp turns can be
			FrequencyMultiplier = FMath::RandRange(0.002f, 0.008f); // Adjusts how frequent turns occur
		}

		// Generate Perlin noise with the dynamically changing frequency
		float NoiseValueX = FMath::PerlinNoise1D(LastPoint.X * FrequencyMultiplier) * SharpnessMultiplier;
		float NoiseValueY = FMath::PerlinNoise1D(LastPoint.Y * FrequencyMultiplier) * SharpnessMultiplier;

		// === Detect If the Path is Too Straight (Both X & Y) ===
		float AverageDeviation = 0.0f;
		bool IsTooStraight = false;

		if (SplinePoints > CheckPoints)
		{
			float TotalDeviation = 0.0f;
			for (int32 i = SplinePoints - CheckPoints; i < SplinePoints - 1; i++)
			{
				FVector PrevPoint = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
				FVector NextPoint = PathSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

				// Measure deviation in both X and Y
				float XYDeviation = FVector(NextPoint - PrevPoint).Size2D(); // Only considers X and Y
				TotalDeviation += XYDeviation;
			}

			AverageDeviation = TotalDeviation / CheckPoints;
			UE_LOG(LogTemp, Warning, TEXT("Total Deviation: %f"), TotalDeviation);
			UE_LOG(LogTemp, Warning, TEXT("Average Deviation: %f"), AverageDeviation);

			// Define a "too straight" threshold (adjustable)
			IsTooStraight = AverageDeviation < DeviationThreshold; // If deviation is too small, force a turn
			UE_LOG(LogTemp, Warning, TEXT("Is Too Straight: %s"), IsTooStraight ? TEXT("True") : TEXT("False"));
		}

		if (IsTooStraight)
		{
			NoiseValueX += FMath::RandRange(-50.0f, 50.0f); // Strong forced curve in X
			NoiseValueY += FMath::RandRange(-50.0f, 50.0f); // Strong forced curve in Y
		}

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
			for (int i = 0; i < VertCount; i++)
			{
				PathUVs.RemoveAt(i);
			}
		}

		// Update spline with new points
		PathSpline->UpdateSpline();

		UpdateRiverSpline();

		GenerateProcMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathNoiseBool, PathUVScale, PathMaterial);
		//GenerateRiverMesh();
		GenerateProcMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverNoiseBool, RiverUVScale, RiverMaterial);
	}
}

void AuProcTerrainGenerator_CPP::CreateRiverSpline()
{
	if (RiverSpline)
	{
		RiverSpline->ClearSplinePoints();
		// Loop through each point in the original spline
		for (int32 i = 0; i < PathSpline->GetNumberOfSplinePoints(); i++)
		{
			// Get the location of the current spline point
			FVector SplinePointLocation = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);

			// Get the tangent (direction) at this point and normalize it
			FVector SplineTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

			// Define the UpVector (usually the Z-axis, but you can modify this if needed)
			FVector UpVector = FVector(0, 0, 1); // Z-axis as up

			// Calculate the perpendicular vector using the cross product
			FVector PerpendicularVector = FVector::CrossProduct(SplineTangent, UpVector).GetSafeNormal();

			// Apply the offset by adding the perpendicular vector to the original location
			FVector OffsetLocation = SplinePointLocation + PerpendicularVector * RiverOffset;
			OffsetLocation.Z = -RiverDepth;
			// Add the offset location to the new array of spline points
			RiverSpline->AddSplinePoint(OffsetLocation, ESplineCoordinateSpace::World);
		}
		RiverSpline->UpdateSpline();
	}
}

void AuProcTerrainGenerator_CPP::UpdateRiverSpline()
{
	// Get the location of the current spline point
	FVector SplinePointLocation = PathSpline->GetLocationAtSplinePoint(PathSpline->GetNumberOfSplinePoints(), ESplineCoordinateSpace::Local);

	// Get the tangent (direction) at this point and normalize it
	FVector SplineTangent = PathSpline->GetTangentAtSplinePoint(PathSpline->GetNumberOfSplinePoints(), ESplineCoordinateSpace::Local).GetSafeNormal();

	// Define the UpVector (usually the Z-axis, but you can modify this if needed)
	FVector UpVector = FVector(0, 0, 1); // Z-axis as up

	// Calculate the perpendicular vector using the cross product
	FVector PerpendicularVector = FVector::CrossProduct(SplineTangent, UpVector).GetSafeNormal();

	// Apply the offset by adding the perpendicular vector to the original location
	FVector OffsetLocation = SplinePointLocation + PerpendicularVector * RiverOffset;
	OffsetLocation.Z = -RiverDepth;

	// Add the offset location to the new array of spline points
	RiverSpline->AddSplinePoint(OffsetLocation, ESplineCoordinateSpace::Local);
	while (RiverSpline->GetNumberOfSplinePoints() > SplinePoints)
	{
		// Remove first spline point to keep length constant
		RiverSpline->RemoveSplinePoint(0);
		for (int i = 0; i < VertCount; i++)
		{
			RiverUVs.RemoveAt(i);
		}
	}

	RiverSpline->UpdateSpline();
}