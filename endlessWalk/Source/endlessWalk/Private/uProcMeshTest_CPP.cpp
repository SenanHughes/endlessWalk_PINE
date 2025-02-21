// Fill out your copyright notice in the Description page of Project Settings.


#include "uProcMeshTest_CPP.h"
#include "Components/SplineComponent.h"

// Sets default values
AuProcMeshTest_CPP::AuProcMeshTest_CPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Create the spline component
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;
}

// Called when the game starts or when spawned
void AuProcMeshTest_CPP::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the spline is correctly set up
	if (Spline)
	{
		// Clear any previous spline points
		Spline->ClearSplinePoints();

		// Create spline points at the required distances (e.g., a 10,000-unit long spline)
		const int32 NumPoints = 100;  // Number of spline points
		const float SplineLength = 1000.0f;

		for (int32 i = 0; i < NumPoints; i++)
		{
			// Calculate position for each point
			FVector NewPoint(i * 100, FMath::Sin(i * 0.1f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			Spline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		Spline->UpdateSpline();
	}

	GenerateSquareMesh();

}

void AuProcMeshTest_CPP::GenerateSquareMesh()
{
	TArray<FVector> Vertices;
	TArray<FVector2D> UVs;
	TArray<int32> Triangles;

	for (int32 i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		FVector SplinePoint = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector SplineTangent = Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector EdgeVector = FVector(-SplineTangent.Y, SplineTangent.X, 0.0f);

		FVector LeftEdge = SplinePoint - EdgeVector * 200;
		FVector RightEdge = SplinePoint + EdgeVector * 200;

		Vertices.Add(LeftEdge);
		Vertices.Add(RightEdge);

		float U = (float)i / (float)Spline->GetNumberOfSplinePoints();
		UVs.Add(FVector2D(U, 0.0f));
		UVs.Add(FVector2D(U, 1.0f));

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

	/*Vertices.Add(FVector(0, 900, 0));
	Vertices.Add(FVector(500, 900, 0));
	Vertices.Add(FVector(0, 0, 500));
	Vertices.Add(FVector(500, 0, 500));*/

	/*TArray<int32> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(3);
	Triangles.Add(2);

	TArray<FVector> Normals;
	for (int i = 0; i < Vertices.Num(); i++)
	{
		Normals.Add(FVector(0, 0, 1));
	}

	TArray<FVector2D> UVs;
	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(0, 1));
	UVs.Add(FVector2D(1, 1));

	TArray<FProcMeshTangent> Tangents;
	for (int i = 0; i < Vertices.Num(); i++)
	{
		Tangents.Add(FProcMeshTangent(1, 0, 0));
	}

	TArray<FLinearColor> VertexColors;
	for (int i = 0; i < Vertices.Num(); i++)
	{
		VertexColors.Add(FLinearColor(1, 1, 1, 1));
	}*/

	MeshComponent->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}


// Called every frame
void AuProcMeshTest_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

