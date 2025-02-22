// Fill out your copyright notice in the Description page of Project Settings.


#include "uProcTerrainGenerator_CPP.h"

// Sets default values
AuProcTerrainGenerator_CPP::AuProcTerrainGenerator_CPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the spline component
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;

	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AuProcTerrainGenerator_CPP::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay, Spline Points times 2: %d"), SplinePoints * 2);

	// Ensure the spline is correctly set up
	if (Spline)
	{
		// Clear any previous spline points
		Spline->ClearSplinePoints();

		for (int32 i = 0; i < SplinePoints; i++)
		{
			// Calculate position for each point
			FVector NewPoint(i * PlaneDistance, FMath::Sin(i * 0.1f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			Spline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		Spline->UpdateSpline();
	}
	
	InitialisePathMesh();
}

// Called every frame
void AuProcTerrainGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTerrainSpline();
}

void AuProcTerrainGenerator_CPP::InitialisePathMesh()
{
	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector SplinePoint = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector SplineTangent = Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector EdgeVector = FVector(-SplineTangent.Y, SplineTangent.X, 0.0f);

		FVector LeftEdge = SplinePoint - EdgeVector * PathWidth;
		FVector RightEdge = SplinePoint + EdgeVector * PathWidth;

		
		if (Vertices.Num() < SplinePoints * 2)
		{
			Vertices.Add(LeftEdge);
			Vertices.Add(RightEdge);
		}
		else if (i < SplinePoints - 1)
		{
			int var = i * 2;
			Vertices[var] = Vertices[var + 2];
			Vertices[var + 1] = Vertices[var + 3];
			UE_LOG(LogTemp, Warning, TEXT("Vertices var and var+1: %s, %s"), *Vertices[var].ToString(), *Vertices[var + 1].ToString());
		}
		else if (i < SplinePoints)
		{
			Vertices[i*2] = LeftEdge;
			Vertices[i*2 + 1] = RightEdge;
			UE_LOG(LogTemp, Warning, TEXT("Vertices i*2 and i*2+1: %s, %s"), *Vertices[i * 2].ToString(), *Vertices[i * 2 + 1].ToString());
		}

		float SplineLength = CalculateSplineLength();
		float Distance = (i / (float)SplinePoints - 1) * SplineLength;
		float U = (Distance / SplineLength) * 20;
		
		if (UVs.Num() < SplinePoints * 2)
		{
			UVs.Add(FVector2D(U, 0.0f));
			UVs.Add(FVector2D(U, 1.0f));
		}
		else if (i < SplinePoints - 1)
		{
			int var = i * 2;
			UVs[var] = UVs[var + 2];
			UVs[var + 1] = UVs[var + 3];
			UE_LOG(LogTemp, Warning, TEXT("UVs var and var+1: %s, %s"), *UVs[var].ToString(), *UVs[var + 1].ToString());
		}
		else if (i < SplinePoints)
		{
			UVs[i*2] = FVector2D(U, 0.0f);
			UVs[i*2 + 1] = FVector2D(U, 1.0f);
		}

		if (Triangles.Num() == (SplinePoints - 1) * 6)
		{
			Triangles.Empty();
		}
		else if (Triangles.Num() < ((SplinePoints - 1) * 6))
		{
			if (i > 0)
			{
				int32 StartIndex = (i - 1) * 2;
				Triangles.Add(StartIndex);
				Triangles.Add(StartIndex + 1);
				Triangles.Add(StartIndex + 3);
				Triangles.Add(StartIndex);
				Triangles.Add(StartIndex + 3);
				Triangles.Add(StartIndex + 2);
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Vertices: %d, Triangles: %d, UVs: %d"), Vertices.Num(), Triangles.Num(), UVs.Num());
	}

	PathMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	
	PathMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (AssignedMaterial)
	{
		PathMesh->SetMaterial(0, AssignedMaterial);
	}
}

void AuProcTerrainGenerator_CPP::UpdateTerrainSpline()
{
	if (SplinePoints < 2)
	{
		return;
	}

	FVector CharacterLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector MidPoint = Spline->GetLocationAtSplinePoint(SplinePoints / 2, ESplineCoordinateSpace::Local);

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathWidth)
	{
		FVector LastPoint = Spline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
		FVector SecondLastPoint = Spline->GetLocationAtSplinePoint(SplinePoints - 2, ESplineCoordinateSpace::Local);
		UE_LOG(LogTemp, Warning, TEXT("Last point: %s"), *LastPoint.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Second last point: %s"), *SecondLastPoint.ToString());

		FVector Direction = LastPoint - SecondLastPoint;
		Direction.Normalize();
		UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());
		
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
				FVector PrevPoint = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
				FVector NextPoint = Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

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
		Spline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		while (Spline->GetNumberOfSplinePoints() > SplinePoints)
		{
			// Remove first spline point to keep length constant
			Spline->RemoveSplinePoint(0);
		}

		// Update spline with new points
		Spline->UpdateSpline();

		UE_LOG(LogTemp, Warning, TEXT("Spline points: %d"), Spline->GetNumberOfSplinePoints());
		UE_LOG(LogTemp, Warning, TEXT("New point: %s"), *NewPoint.ToString());

		InitialisePathMesh();
	}
}

float AuProcTerrainGenerator_CPP::CalculateSplineLength()
{
	float TotalLength = 0.0f;
	for (int32 i = 1; i <= SplinePoints; i++)
	{
		FVector PointA = Spline->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::Local);
		FVector PointB = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		TotalLength += FVector::Dist(PointA, PointB);
	}
	return TotalLength;
}