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
	NoiseFrequency = 0.1f;
	NoiseAmplitude = 50.0f;
	MaxSplineLength = 10000.0f;
	CurrentSplineLength = 0.0f;
	SplineIndex = 0;
	booltest = false;
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
		const int32 NumPoints = 50;  // Number of spline points
		const float SplineLength = 10000.0f;
		const float Step = SplineLength / (NumPoints);

		for (int32 i = 0; i < NumPoints; i++)
		{
			// Calculate position for each point
			FVector NewPoint(i * Step, FMath::Sin(i * 0.1f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			Spline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		Spline->UpdateSpline();
	}

	// Initialise the terrain mesh
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

		UE_LOG(LogTemp, Warning, TEXT("Position: %s"), *Position.ToString());
	}
}


// Updates the terrain mesh based on the current spline
void ATerrainGenerator::UpdateTerrainMesh()
{

	//FVector LastSplinePointPosition = Spline->GetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local);
	//LastSplinePointPosition = LastSplinePointPosition + FVector(0.0f, 0.005f, 0.0f);
	//Spline->SetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1, LastSplinePointPosition, ESplineCoordinateSpace::Local);
	
	UStaticMeshComponent* LastPlane = TerrainPlanes.Last();
	UE_LOG(LogTemp, Warning, TEXT("LastPlane: %s"), *LastPlane->GetComponentLocation().ToString());

	//LastPlane->SetRelativeTransform(FTransform(LastSplinePointPosition));


	int32 NumPoints = Spline->GetNumberOfSplinePoints();

	// Calculate the total length of the spline
	float TotalLength = CalculateSplineLength();

	// Get the current location of the character (or camera)
	FVector CharacterLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	// Calculate the halfway point along the spline
	float HalfwayPoint = TotalLength / 2.0f;



	// Check if the character is at or beyond the halfway point (5,000 units in this case)
	if (CharacterLocation.X >= LastPlane->GetComponentLocation().X)
	{
		TerrainPlanes[0]->DestroyComponent();  // Destroy the first plane
		TerrainPlanes.RemoveAt(0); // Remove from the array

		FVector LastPoint = Spline->GetLocationAtSplinePoint(NumPoints - 1, ESplineCoordinateSpace::Local);
		FVector SecondLastPoint = Spline->GetLocationAtSplinePoint(NumPoints - 2, ESplineCoordinateSpace::Local);

		FVector Direction = LastPoint - SecondLastPoint;
		UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());
		float Distance = Direction.Size();
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);

		// Normalize the direction vector
		Direction.Normalize();
		UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());
		
		// Add a new point at the same distance in the same direction
		FVector NewPosition = LastPoint + Direction * Distance;
		UE_LOG(LogTemp, Warning, TEXT("NumPoints: %d"), NumPoints);
		// Add the new point to the spline
		Spline->AddSplinePoint(NewPosition, ESplineCoordinateSpace::Local);

		Spline->RemoveSplinePoint(0); // Remove the first spline point

		// Update the spline to reflect the new point
		Spline->UpdateSpline();

		int32 NumPoints2 = Spline->GetNumberOfSplinePoints();
		UE_LOG(LogTemp, Warning, TEXT("NumPoints2: %d"), NumPoints2);

		//FVector NewPosition = Spline->GetLocationAtSplinePoint((10000.0f / 50.0f) + 1, ESplineCoordinateSpace::Local); // Next spline point
		float Deformation = GenerateNoise(NewPosition.X, NewPosition.Y);

		UE_LOG(LogTemp, Warning, TEXT("NewPosition: %s"), *NewPosition.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Deformation: %f"), Deformation);
		UE_LOG(LogTemp, Warning, TEXT("LastPoint: %s"), *LastPoint.ToString());
		UE_LOG(LogTemp, Warning, TEXT("SecondLastPoint: %s"), *SecondLastPoint.ToString());

		// add the new terrain plane
		AddTerrainPlane(NumPoints2 - 1, NewPosition, FVector(1.0f, 1.0f, 1.0f), Deformation);
		booltest = true;
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

void ATerrainGenerator::AddTerrainPlane(int32 Index, FVector Position, FVector Scale, float Deformation)
{
	UE_LOG(LogTemp, Warning, TEXT("Index: %d"), Index);
	UE_LOG(LogTemp, Warning, TEXT("Position: %s"), *Position.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Def: %f"), Deformation);

	float Distance = 0.0f;
	// Make sure to calculate the distance between this point and the next spline point for correct scaling
	FVector NextSplinePoint = Spline->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local);
	if (Index + 1 <= Spline->GetNumberOfSplinePoints()) // Ensure it's not the last point
	{
		// Calculate the distance to the next point
		if (Index < 49)
		{
			Distance = FVector::Dist(Position, NextSplinePoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Last Point"));
			Distance = 200.0f;
		}
		// Create a new mesh component for the terrain plane
		UStaticMeshComponent* NewPlane = NewObject<UStaticMeshComponent>(this);
		NewPlane->SetStaticMesh(TerrainMesh);
		NewPlane->SetMaterial(0, TerrainMaterial);

		// Set the terrain plane's position
		NewPlane->SetRelativeLocation(Position);

		// Set the scale of the terrain plane based on the distance between the points
		NewPlane->SetRelativeScale3D(FVector(Distance / 100.0f, 1.0f, 1.0f)); // Adjust the X-scale based on the distance between points
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance/100.0f);
		// Apply noise-based deformation
		NewPlane->SetRelativeLocation(Position + FVector(0.0f, 0.0f, Deformation)); // Apply deformation in Z-axis

		// Register the component
		NewPlane->RegisterComponent();

		// Log to verify plane creation
		UE_LOG(LogTemp, Warning, TEXT("Added Terrain Plane at %s with Distance: %f"), *Position.ToString(), Distance);

		// Store the new plane for later management
		TerrainPlanes.Add(NewPlane);

		//NewPlane->SetVisibility(true);
		//NewPlane->SetHiddenInGame(false);
	}
}

