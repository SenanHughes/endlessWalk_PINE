// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ATerrainGenerator::ATerrainGenerator()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create the spline component
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;

	// Initialise default values
	NoiseFrequency = 100.0f;
	NoiseAmplitude = 500.0f;
	MaxSplineLength = 10000;
	PlaneDistance = 100;
	CheckPoints = 2;

	// Load the terrain mesh
	//TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	//TerrainMesh->SetupAttachment(RootComponent);
	// Load the terrain material
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("Material'/Game/Materials/M_Terrain.M_Terrain'"));
	//if (Material.Succeeded())
	//{
	//	TerrainMaterial = Material.Object;
	//}
}

// Called when the game starts or when spawned
void ATerrainGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the spline is correctly set up
	if (Spline)
	{
		// Clear any previous spline points
		Spline->ClearSplinePoints();

		// Create spline points at the required distances (e.g., a 10,000-unit long spline)
		const int32 NumPoints = 100;  // Number of spline points
		const float SplineLength = 10000.0f;
		const float Step = SplineLength / (NumPoints);

		for (int32 i = 0; i < NumPoints; i++)
		{
			// Calculate position for each point
			FVector NewPoint(i * PlaneDistance, FMath::Sin(i * 0.1f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			Spline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		Spline->UpdateSpline();
	}

	// Initialise the terrain mesh
	//GenerateMeshFromSpline();
	InitialiseTerrainMesh();
}

// Called every frame
void ATerrainGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Recalculate the terrain planes
	UpdateTerrainMesh();
}

void ATerrainGenerator::InitialiseTerrainMesh()
{
	// Calculate how many planes we need based on spline length
	float TotalLength = 0.0f;
	TArray<FVector> SplinePoints;

	for (int32 i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		FVector SplinePoint = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		SplinePoints.Add(SplinePoint);

		if (i > 0)
		{
			TotalLength += FVector::Dist(SplinePoints[i], SplinePoints[i - 1]);
		}

		if (TotalLength > MaxSplineLength)
		{
			break;
		}
	}

	// Loop through the spline points and add terrain planes
	for (int32 i = 0; i < SplinePoints.Num(); i++)
	{
		FVector Position = SplinePoints[i];
		float Deformation = GenerateNoise(Position.X, Position.Y);  // Generate noise-based deformation
		AddTerrainPlane(i, Position, FVector(1.0f, 1.0f, 1.0f), Deformation);
	}
}


// Updates the terrain mesh based on the current spline
void ATerrainGenerator::UpdateTerrainMesh()
{
	if (Spline->GetNumberOfSplinePoints() < 10)
	{
		return;
	}

	// Get the middle terrain plane for reference
	//UStaticMeshComponent* MidPlane = TerrainPlanes[TerrainPlanes.Num() / 2];
	
	int32 NumPoints = Spline->GetNumberOfSplinePoints();
	
	// Get the player's position
	FVector CharacterLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector MidPoint = Spline->GetLocationAtSplinePoint(NumPoints / 2, ESplineCoordinateSpace::Local);

	// Check if the character has moved past the middle of the visible spline
	if (CharacterLocation.X >= MidPoint.X)
	{
		// Remove the oldest terrain piece
		TerrainPlanes[0]->DestroyComponent();
		TerrainPlanes.RemoveAt(0);

		// Get the last two spline points
		
		FVector LastPoint = Spline->GetLocationAtSplinePoint(NumPoints - 1, ESplineCoordinateSpace::Local);
		FVector SecondLastPoint = Spline->GetLocationAtSplinePoint(NumPoints - 2, ESplineCoordinateSpace::Local);

		// Calculate base movement direction
		FVector Direction = LastPoint - SecondLastPoint;
		Direction.Normalize();

		// === Dynamic Curve Variation ===
		static float CurrentCurveDirectionX = 0.0f; // Persistent curve for X
		static float CurrentCurveDirectionY = 0.0f; // Persistent curve for Y

		// Time-based random variation
		static float CurveSpeedMultiplier = 0.1f; // Controls curve speed
		static float SharpnessMultiplier = 100.0f; // Controls curve sharpness
		static float FrequencyMultiplier = 0.005f; // Controls frequency of bends
		static float TimeElapsed = 0.0f;

		TimeElapsed += GetWorld()->GetDeltaSeconds(); // Track time progression

		// Every 1-2 seconds, randomise curve behavior
		if (FMath::Fmod(TimeElapsed, FMath::RandRange(1.0f, 2.0f)) < GetWorld()->GetDeltaSeconds())
		{
			CurveSpeedMultiplier = FMath::RandRange(0.1f, 1.0f); // Makes turns speed up/slow down
			SharpnessMultiplier = FMath::RandRange(100.0f, 200.0f); // Adjusts how sharp turns can be
			FrequencyMultiplier = FMath::RandRange(0.002f, 0.008f); // Adjusts how frequent turns occur
		}

		// Generate Perlin noise with the dynamically changing frequency
		float NoiseValueX = FMath::PerlinNoise1D(LastPoint.X * FrequencyMultiplier) * SharpnessMultiplier;
		float NoiseValueY = FMath::PerlinNoise1D(LastPoint.Y * FrequencyMultiplier) * SharpnessMultiplier;

		// === Detect If the Path is Too Straight (Both X & Y) ===
		float AverageDeviation = 0.0f;
		bool IsTooStraight = false;

		if (NumPoints > CheckPoints)
		{
			float TotalDeviation = 0.0f;
			for (int32 i = NumPoints - CheckPoints; i < NumPoints - 1; i++)
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

		// === Apply a Forced Turn if Needed ===
		if (IsTooStraight)
		{
			UE_LOG(LogTemp, Warning, TEXT("Forcing a turn!"));
			NoiseValueX += FMath::RandRange(-50.0f, 50.0f); // Strong forced curve in X
			NoiseValueY += FMath::RandRange(-50.0f, 50.0f); // Strong forced curve in Y
		}

		// Smoothly adjust the curve
		CurrentCurveDirectionX = FMath::Lerp(CurrentCurveDirectionX, NoiseValueX, 0.1f * CurveSpeedMultiplier);
		CurrentCurveDirectionY = FMath::Lerp(CurrentCurveDirectionY, NoiseValueY, 0.1f * CurveSpeedMultiplier);

		// Apply movement on X and Y, keeping Z flat
		FVector NewPosition = LastPoint + (Direction * PlaneDistance);
		NewPosition.X += CurrentCurveDirectionX;
		NewPosition.Y += CurrentCurveDirectionY;
		NewPosition.Z = 0.0f; // Keep terrain flat

		// Add the new point to the spline
		Spline->AddSplinePoint(NewPosition, ESplineCoordinateSpace::Local);
		while (CalculateSplineLength() > MaxSplineLength)
		{
			// Remove first spline point to keep length constant
			Spline->RemoveSplinePoint(0);
		}

		// Update spline with new points
		Spline->UpdateSpline();

		// Generate procedural noise for terrain deformation (if needed)
		float Deformation = GenerateNoise(NewPosition.X, NewPosition.Y);

		// Add new terrain plane
		AddTerrainPlane(NumPoints - 1, NewPosition, FVector(1.0f, 1.0f, 1.0f), Deformation);
		//GenerateMeshFromSpline();
	}
}



float ATerrainGenerator::CalculateSplineLength()
{
	float TotalLength = 0.0f;

	for (int32 i = 1; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		FVector PointA = Spline->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::Local);
		FVector PointB = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);

		TotalLength += FVector::Dist(PointA, PointB);
	}

	return TotalLength;
}

// Generates Perlin noise for terrain deformation
float ATerrainGenerator::GenerateNoise(float X, float Y)
{
	// Generate Perlin noise (use your noise algorithm here)
	return FMath::PerlinNoise2D(FVector2D(X * NoiseFrequency, Y * NoiseFrequency)) * NoiseAmplitude;
}

//void ATerrainGenerator::GenerateMeshFromSpline()
//{
//	TArray<FVector> Vertices;
//	TArray<int32> Triangles;
//	TArray<FVector2D> UVs;
//
//	int32 NumPoints = Spline->GetNumberOfSplinePoints();
//	float Width = 500.0f;  // Adjust width of the path
//
//	for (int32 i = 0; i < NumPoints; i++)
//	{
//		FVector SplinePoint = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
//		FVector Tangent = Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();
//		FVector RightVector = FVector(-Tangent.Y, Tangent.X, 0.0f);  // Perpendicular to tangent
//
//		// Create left and right edges of the path
//		FVector LeftEdge = SplinePoint - RightVector * Width;
//		FVector RightEdge = SplinePoint + RightVector * Width;
//
//		Vertices.Add(LeftEdge);
//		Vertices.Add(RightEdge);
//
//		// UV mapping
//		float U = (float)i / (float)NumPoints;
//		UVs.Add(FVector2D(U, 0.0f));
//		UVs.Add(FVector2D(U, 1.0f));
//
//		// Create triangles (quads between spline points)
//		if (i > 0)
//		{
//			int32 StartIndex = (i - 1) * 2;
//			Triangles.Add(StartIndex);
//			Triangles.Add(StartIndex + 1);
//			Triangles.Add(StartIndex + 3);
//
//			Triangles.Add(StartIndex);
//			Triangles.Add(StartIndex + 3);
//			Triangles.Add(StartIndex + 2);
//		}
//	}
//
//	// Debugging - check array sizes
//	UE_LOG(LogTemp, Warning, TEXT("Vertices: %d, Triangles: %d, UVs: %d"),
//		Vertices.Num(), Triangles.Num(), UVs.Num());
//
//	// Ensure valid mesh data
//	if (Vertices.Num() == 0 || Triangles.Num() == 0)
//	{
//		UE_LOG(LogTemp, Error, TEXT("Error: No vertices or triangles!"));
//		return;
//	}
//
//	// Check if TerrainMesh is valid before creating the mesh
//	if (!TerrainMesh)
//	{
//		UE_LOG(LogTemp, Error, TEXT("TerrainMesh is nullptr!"));
//		return;
//	}

	//check(TerrainMesh);
	// Update procedural mesh
	//TerrainMesh->CreateMeshSection_LinearColor(
		//0, Vertices, Triangles, TArray<FVector>(), UVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true
	//);
//}

void ATerrainGenerator::AddTerrainPlane(int32 Index, FVector Position, FVector Scale, float Deformation)
{
	if (Index >= Spline->GetNumberOfSplinePoints())
	{
		return;
	}

	// Get the tangent direction at this spline point
	FVector Tangent = Spline->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::Local);
	Tangent.Normalize();

	// Convert the tangent vector into a rotation
	FRotator PlaneRotation = Tangent.Rotation();

	float Distance = 0.0f;
	FVector PrevSplinePoint, NextSplinePoint;

	// Ensure we're not at the first or last point
	if (Index > 0)
	{
		PrevSplinePoint = Spline->GetLocationAtSplinePoint(Index - 1, ESplineCoordinateSpace::Local);
		Distance = FVector::Dist(Position, PrevSplinePoint);
	}
	else if (Index < Spline->GetNumberOfSplinePoints() - 1)
	{
		NextSplinePoint = Spline->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local);
		Distance = FVector::Dist(Position, NextSplinePoint);
	}

	// Create a new mesh component for the terrain plane
	UStaticMeshComponent* NewPlane = NewObject<UStaticMeshComponent>(this);
	NewPlane->SetStaticMesh(TerrainMesh);
	NewPlane->SetMaterial(0, TerrainMaterial);

	// Set position, rotation, and scale
	NewPlane->SetRelativeLocation(Position);
	NewPlane->SetRelativeRotation(PlaneRotation);
	NewPlane->SetRelativeScale3D(FVector(Distance / 100.0f, 10.0f, 1.0f));

	// Register the component
	NewPlane->RegisterComponent();

	// Store the new plane for later management
	TerrainPlanes.Add(NewPlane);

	UE_LOG(LogTemp, Warning, TEXT("Plane Location: %s, Rotation: %s"), *Position.ToString(), *PlaneRotation.ToString());
}

