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
	float TotalDistance = 0.0f;

	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector PathSplinePoint = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector PathSplineTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector PathEdgeVector = FVector(-PathSplineTangent.Y, PathSplineTangent.X, 0.0f);

		FVector PathLeftEdge = PathSplinePoint - PathEdgeVector * (PathWidth / 2);
		FVector PathRightEdge = PathSplinePoint + PathEdgeVector * (PathWidth / 2);

		
		if (PathVertices.Num() < SplinePoints * 2)
		{
			PathVertices.Add(PathLeftEdge);
			PathVertices.Add(PathRightEdge);
		}
		else
		{
			int var = i * 2;
			if (i < SplinePoints - 1)
			{
				PathVertices[var] = PathVertices[var + 2];
				PathVertices[var + 1] = PathVertices[var + 3];
			}
			else
			{
				PathVertices[var] = PathLeftEdge;
				PathVertices[var + 1] = PathRightEdge;
			}
		}
		
		float PathLeftU = PathLeftEdge.X / PathUVScale;
		float PathLeftV = PathLeftEdge.Y / PathUVScale;
		float PathRightU = PathRightEdge.X / PathUVScale;
		float PathRightV = PathRightEdge.Y / PathUVScale;


		if (UVs.Num() < SplinePoints * 2)
		{
			UVs.Add(FVector2D(PathLeftU, PathLeftV));
			UVs.Add(FVector2D(PathRightU, PathRightV));
		}
	}

	Triangles.Empty();
	for (int i = 1; i < SplinePoints; i++)
	{
		int32 StartIndex = (i - 1) * 2;
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 2);
			
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
	float TotalDistance = 0.0f;

	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector RiverSplinePoint = RiverSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector RiverSplineTangent = RiverSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector RiverEdgeVector = FVector(-RiverSplineTangent.Y, RiverSplineTangent.X, 0.0f);

		FVector RiverLeftEdge = RiverSplinePoint - RiverEdgeVector * (RiverWidth/2);
		FVector RiverRightEdge = RiverSplinePoint + RiverEdgeVector * (RiverWidth/2);


		if (RiverVertices.Num() < SplinePoints * 2)
		{
			RiverVertices.Add(RiverLeftEdge);
			RiverVertices.Add(RiverRightEdge);
		}
		else
		{
			int var = i * 2;
			if (i < SplinePoints - 1)
			{
				RiverVertices[var] = RiverVertices[var + 2];
				RiverVertices[var + 1] = RiverVertices[var + 3];
			}
			else
			{
				RiverVertices[var] = RiverLeftEdge;
				RiverVertices[var + 1] = RiverRightEdge;
			}
		}

		float RiverLeftU = RiverLeftEdge.X / RiverUVScale;
		float RiverLeftV = RiverLeftEdge.Y / RiverUVScale;
		float RiverRightU = RiverRightEdge.X / RiverUVScale;
		float RiverRightV = RiverRightEdge.Y / RiverUVScale;


		if (UVs.Num() < SplinePoints * 2)
		{
			UVs.Add(FVector2D(RiverLeftU, RiverLeftV));
			UVs.Add(FVector2D(RiverRightU, RiverRightV));
		}
	}

	Triangles.Empty();
	for (int i = 1; i < SplinePoints; i++)
	{
		int32 StartIndex = (i - 1) * 2;
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 1);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex);
		Triangles.Add(StartIndex + 3);
		Triangles.Add(StartIndex + 2);

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

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathWidth/2)
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