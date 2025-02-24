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
			FVector NewPoint(i * PlaneDistance, FMath::Sin(i * 0.1f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		PathSpline->UpdateSpline();
	}
	
	CreateRiverSpline();

	GeneratePathMesh();
	GenerateRiverMesh();
}

// Called every frame
void AuProcTerrainGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTerrainSpline();
}

void AuProcTerrainGenerator_CPP::GeneratePathMesh()
{
	UVs.Empty();

	FastNoiseLite Noise;
	Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise.SetFrequency(NoiseFrequency);


	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector PathSplinePoint = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector PathSplineTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector PathEdgeVector = FVector(-PathSplineTangent.Y, PathSplineTangent.X, 0.0f);

		FVector PathLeftEdge = PathSplinePoint - PathEdgeVector * (PathWidth / 2);
		FVector PathLeft3Quarter = PathSplinePoint - PathEdgeVector * ((PathWidth / 2) * 0.75f);
		FVector PathLeft2Quarter = PathSplinePoint - PathEdgeVector * ((PathWidth / 2) * 0.5f);
		FVector PathLeft1Quarter = PathSplinePoint - PathEdgeVector * ((PathWidth / 2) * 0.25f);
		FVector PathRight1Quarter = PathSplinePoint + PathEdgeVector * ((PathWidth / 2) * 0.25f);
		FVector PathRight2Quarter = PathSplinePoint + PathEdgeVector * ((PathWidth / 2) * 0.5f);
		FVector PathRight3Quarter = PathSplinePoint + PathEdgeVector * ((PathWidth / 2) * 0.75f);
		FVector PathRightEdge = PathSplinePoint + PathEdgeVector * (PathWidth / 2);
		
		if (PathVertices.Num() < SplinePoints * 9)
		{
			PathVertices.Add(PathLeftEdge);
			PathVertices.Add(PathLeft3Quarter);
			PathVertices.Add(PathLeft2Quarter);
			PathVertices.Add(PathLeft1Quarter);
			PathVertices.Add(PathSplinePoint);
			PathVertices.Add(PathRight1Quarter);
			PathVertices.Add(PathRight2Quarter);
			PathVertices.Add(PathRight3Quarter);
			PathVertices.Add(PathRightEdge);
		}
		else
		{
			int var = i * 9;
			if (i < SplinePoints - 1)
			{
				PathVertices[var] = PathVertices[var + 9];
				PathVertices[var + 1] = PathVertices[var + 10];
				PathVertices[var + 2] = PathVertices[var + 11];
				PathVertices[var + 3] = PathVertices[var + 12];
				PathVertices[var + 4] = PathVertices[var + 13];
				PathVertices[var + 5] = PathVertices[var + 14];
				PathVertices[var + 6] = PathVertices[var + 15];
				PathVertices[var + 7] = PathVertices[var + 16];
				PathVertices[var + 8] = PathVertices[var + 17];
			}
			else
			{
				PathVertices[var] = PathLeftEdge;
				PathVertices[var + 1] = PathLeft3Quarter;
				PathVertices[var + 2] = PathLeft2Quarter;
				PathVertices[var + 3] = PathLeft1Quarter;
				PathVertices[var + 4] = PathSplinePoint;
				PathVertices[var + 5] = PathRight1Quarter;
				PathVertices[var + 6] = PathRight2Quarter;
				PathVertices[var + 7] = PathRight3Quarter;
				PathVertices[var + 8] = PathRightEdge;
			}
		}

		for (int32 x = i * 9; x < (i * 9) + 9; x++)
		{
			float NoiseValue = Noise.GetNoise(PathVertices[x].X, PathVertices[x].Y);
			PathVertices[x].Z = NoiseValue * NoiseAmplitude;
		}
		
		float PathLeftU = PathLeftEdge.X / PathUVScale;
		float PathLeftV = PathLeftEdge.Y / PathUVScale;
		float PathLeft3QU = PathLeft3Quarter.X / PathUVScale;
		float PathLeft3QV = PathLeft3Quarter.Y / PathUVScale;
		float PathLeft2QU = PathLeft2Quarter.X / PathUVScale;
		float PathLeft2QV = PathLeft2Quarter.Y / PathUVScale;
		float PathLeft1QU = PathLeft1Quarter.X / PathUVScale;
		float PathLeft1QV = PathLeft1Quarter.Y / PathUVScale;
		float PathCenterU = PathSplinePoint.X / PathUVScale;
		float PathCenterV = PathSplinePoint.Y / PathUVScale;
		float PathRight1QU = PathRight1Quarter.X / PathUVScale;
		float PathRight1QV = PathRight1Quarter.Y / PathUVScale;
		float PathRight2QU = PathRight2Quarter.X / PathUVScale;
		float PathRight2QV = PathRight2Quarter.Y / PathUVScale;
		float PathRight3QU = PathRight3Quarter.X / PathUVScale;
		float PathRight3QV = PathRight3Quarter.Y / PathUVScale;
		float PathRightU = PathRightEdge.X / PathUVScale;
		float PathRightV = PathRightEdge.Y / PathUVScale;


		if (UVs.Num() < SplinePoints * 9)
		{
			UVs.Add(FVector2D(PathLeftU, PathLeftV));
			UVs.Add(FVector2D(PathLeft3QU, PathLeft3QV));
			UVs.Add(FVector2D(PathLeft2QU, PathLeft2QV));
			UVs.Add(FVector2D(PathLeft1QU, PathLeft1QV));
			UVs.Add(FVector2D(PathCenterU, PathCenterV));
			UVs.Add(FVector2D(PathRight1QU, PathRight1QV));
			UVs.Add(FVector2D(PathRight2QU, PathRight2QV));
			UVs.Add(FVector2D(PathRight3QU, PathRight3QV));
			UVs.Add(FVector2D(PathRightU, PathRightV));
		}
	}

	Triangles.Empty();
	for (int i = 1; i < SplinePoints; i++)
	{
		int32 StartIndex = (i - 1) * 9;
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 10);
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 10);
		Triangles.Add(StartIndex + 9);

		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 2);
		Triangles.Add(StartIndex + 11);
		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 11);
		Triangles.Add(StartIndex + 10);

		Triangles.Add(StartIndex + 2);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 12);
		Triangles.Add(StartIndex + 2);
		Triangles.Add(StartIndex + 12);
		Triangles.Add(StartIndex + 11);

		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 4);
		Triangles.Add(StartIndex + 13);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 13);
		Triangles.Add(StartIndex + 12);

		Triangles.Add(StartIndex + 4);
		Triangles.Add(StartIndex + 5);
		Triangles.Add(StartIndex + 14);
		Triangles.Add(StartIndex + 4);
		Triangles.Add(StartIndex + 14);
		Triangles.Add(StartIndex + 13);

		Triangles.Add(StartIndex + 5);
		Triangles.Add(StartIndex + 6);
		Triangles.Add(StartIndex + 15);
		Triangles.Add(StartIndex + 5);
		Triangles.Add(StartIndex + 15);
		Triangles.Add(StartIndex + 14);

		Triangles.Add(StartIndex + 6);
		Triangles.Add(StartIndex + 7);
		Triangles.Add(StartIndex + 16);
		Triangles.Add(StartIndex + 6);
		Triangles.Add(StartIndex + 16);
		Triangles.Add(StartIndex + 15);

		Triangles.Add(StartIndex + 7);
		Triangles.Add(StartIndex + 8);
		Triangles.Add(StartIndex + 17);
		Triangles.Add(StartIndex + 7);
		Triangles.Add(StartIndex + 17);
		Triangles.Add(StartIndex + 16);
			
	}

	PathMesh->CreateMeshSection(0, PathVertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	
	PathMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (PathMaterial)
	{
		PathMesh->SetMaterial(0, PathMaterial);
	}
}

void AuProcTerrainGenerator_CPP::GenerateRiverMesh()
{
	UVs.Empty();

	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector RiverSplinePoint = RiverSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector RiverSplineTangent = RiverSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector RiverEdgeVector = FVector(-RiverSplineTangent.Y, RiverSplineTangent.X, 0.0f);

		FVector RiverLeftEdge = RiverSplinePoint - RiverEdgeVector * (RiverWidth/2);
		FVector RiverLeft3Quarter = RiverSplinePoint - RiverEdgeVector * ((RiverWidth / 2) * 0.75f);
		FVector RiverLeft2Quarter = RiverSplinePoint - RiverEdgeVector * ((RiverWidth / 2) * 0.5f);
		FVector RiverLeft1Quarter = RiverSplinePoint - RiverEdgeVector * ((RiverWidth / 2) * 0.25f);
		FVector RiverRight1Quarter = RiverSplinePoint + RiverEdgeVector * ((RiverWidth / 2) * 0.25f);
		FVector RiverRight2Quarter = RiverSplinePoint + RiverEdgeVector * ((RiverWidth / 2) * 0.5f);
		FVector RiverRight3Quarter = RiverSplinePoint + RiverEdgeVector * ((RiverWidth / 2) * 0.75f);
		FVector RiverRightEdge = RiverSplinePoint + RiverEdgeVector * (RiverWidth/2);


		if (RiverVertices.Num() < SplinePoints * 9)
		{
			RiverVertices.Add(RiverLeftEdge);
			RiverVertices.Add(RiverLeft3Quarter);
			RiverVertices.Add(RiverLeft2Quarter);
			RiverVertices.Add(RiverLeft1Quarter);
			RiverVertices.Add(RiverSplinePoint);
			RiverVertices.Add(RiverRight1Quarter);
			RiverVertices.Add(RiverRight2Quarter);
			RiverVertices.Add(RiverRight3Quarter);
			RiverVertices.Add(RiverRightEdge);
		}
		else
		{
			int var = i * 9;
			if (i < SplinePoints - 1)
			{
				RiverVertices[var] = RiverVertices[var + 9];
				RiverVertices[var + 1] = RiverVertices[var + 10];
				RiverVertices[var + 2] = RiverVertices[var + 11];
				RiverVertices[var + 3] = RiverVertices[var + 12];
				RiverVertices[var + 4] = RiverVertices[var + 13];
				RiverVertices[var + 5] = RiverVertices[var + 14];
				RiverVertices[var + 6] = RiverVertices[var + 15];
				RiverVertices[var + 7] = RiverVertices[var + 16];
				RiverVertices[var + 8] = RiverVertices[var + 17];
			}
			else
			{
				RiverVertices[var] = RiverLeftEdge;
				RiverVertices[var + 1] = RiverLeft3Quarter;
				RiverVertices[var + 2] = RiverLeft2Quarter;
				RiverVertices[var + 3] = RiverLeft1Quarter;
				RiverVertices[var + 4] = RiverSplinePoint;
				RiverVertices[var + 5] = RiverRight1Quarter;
				RiverVertices[var + 6] = RiverRight2Quarter;
				RiverVertices[var + 7] = RiverRight3Quarter;
				RiverVertices[var + 8] = RiverRightEdge;
			}
		}

		float RiverLeftU = RiverLeftEdge.X / RiverUVScale;
		float RiverLeftV = RiverLeftEdge.Y / RiverUVScale;
		float RiverLeft3QU = RiverLeft3Quarter.X / RiverUVScale;
		float RiverLeft3QV = RiverLeft3Quarter.Y / RiverUVScale;
		float RiverLeft2QU = RiverLeft2Quarter.X / RiverUVScale;
		float RiverLeft2QV = RiverLeft2Quarter.Y / RiverUVScale;
		float RiverLeft1QU = RiverLeft1Quarter.X / RiverUVScale;
		float RiverLeft1QV = RiverLeft1Quarter.Y / RiverUVScale;
		float RiverCenterU = RiverSplinePoint.X / RiverUVScale;
		float RiverCenterV = RiverSplinePoint.Y / RiverUVScale;
		float RiverRight1QU = RiverRight1Quarter.X / RiverUVScale;
		float RiverRight1QV = RiverRight1Quarter.Y / RiverUVScale;
		float RiverRight2QU = RiverRight2Quarter.X / RiverUVScale;
		float RiverRight2QV = RiverRight2Quarter.Y / RiverUVScale;
		float RiverRight3QU = RiverRight3Quarter.X / RiverUVScale;
		float RiverRight3QV = RiverRight3Quarter.Y / RiverUVScale;
		float RiverRightU = RiverRightEdge.X / RiverUVScale;
		float RiverRightV = RiverRightEdge.Y / RiverUVScale;


		if (UVs.Num() < SplinePoints * 9)
		{
			UVs.Add(FVector2D(RiverLeftU, RiverLeftV));
			UVs.Add(FVector2D(RiverLeft3QU, RiverLeft3QV));
			UVs.Add(FVector2D(RiverLeft2QU, RiverLeft2QV));
			UVs.Add(FVector2D(RiverLeft1QU, RiverLeft1QV));
			UVs.Add(FVector2D(RiverCenterU, RiverCenterV));
			UVs.Add(FVector2D(RiverRight1QU, RiverRight1QV));
			UVs.Add(FVector2D(RiverRight2QU, RiverRight2QV));
			UVs.Add(FVector2D(RiverRight3QU, RiverRight3QV));
			UVs.Add(FVector2D(RiverRightU, RiverRightV));
		}
	}

	Triangles.Empty();
	for (int i = 1; i < SplinePoints; i++)
	{
		int32 StartIndex = (i - 1) * 9;
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 10);
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 10);
		Triangles.Add(StartIndex + 9);

		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 2);
		Triangles.Add(StartIndex + 11);
		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 11);
		Triangles.Add(StartIndex + 10);

		Triangles.Add(StartIndex + 2);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 12);
		Triangles.Add(StartIndex + 2);
		Triangles.Add(StartIndex + 12);
		Triangles.Add(StartIndex + 11);

		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 4);
		Triangles.Add(StartIndex + 13);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 13);
		Triangles.Add(StartIndex + 12);

		Triangles.Add(StartIndex + 4);
		Triangles.Add(StartIndex + 5);
		Triangles.Add(StartIndex + 14);
		Triangles.Add(StartIndex + 4);
		Triangles.Add(StartIndex + 14);
		Triangles.Add(StartIndex + 13);

		Triangles.Add(StartIndex + 5);
		Triangles.Add(StartIndex + 6);
		Triangles.Add(StartIndex + 15);
		Triangles.Add(StartIndex + 5);
		Triangles.Add(StartIndex + 15);
		Triangles.Add(StartIndex + 14);

		Triangles.Add(StartIndex + 6);
		Triangles.Add(StartIndex + 7);
		Triangles.Add(StartIndex + 16);
		Triangles.Add(StartIndex + 6);
		Triangles.Add(StartIndex + 16);
		Triangles.Add(StartIndex + 15);

		Triangles.Add(StartIndex + 7);
		Triangles.Add(StartIndex + 8);
		Triangles.Add(StartIndex + 17);
		Triangles.Add(StartIndex + 7);
		Triangles.Add(StartIndex + 17);
		Triangles.Add(StartIndex + 16);
	}

	RiverMesh->CreateMeshSection(0, RiverVertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	RiverMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (RiverMaterial)
	{
		RiverMesh->SetMaterial(0, RiverMaterial);
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

		// Every 1-2 seconds, randomise curve behavior
		if (FMath::Fmod(TimeElapsed, FMath::RandRange(3.0f, 6.0f)) < GetWorld()->GetDeltaSeconds())
		{
			CurveSpeedMultiplier = FMath::RandRange(0.1f, 0.6f); // Makes turns speed up/slow down
			SharpnessMultiplier = FMath::RandRange(75.0f, 125.0f); // Adjusts how sharp turns can be
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

			// Define a "too straight" threshold (adjustable)
			IsTooStraight = AverageDeviation < 200.0f; // If deviation is too small, force a turn
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
		}

		// Update spline with new points
		PathSpline->UpdateSpline();

		UpdateRiverSpline();

		GeneratePathMesh();
		GenerateRiverMesh();
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
	}

	RiverSpline->UpdateSpline();
}