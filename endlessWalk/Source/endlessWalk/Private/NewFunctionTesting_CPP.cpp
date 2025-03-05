// Fill out your copyright notice in the Description page of Project Settings.


#include "NewFunctionTesting_CPP.h"

// Sets default values
ANewFunctionTesting_CPP::ANewFunctionTesting_CPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	RootComponent = PathSpline;

	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ANewFunctionTesting_CPP::BeginPlay()
{
	Super::BeginPlay();

	if (PathSpline)
	{
		// Clear any previous spline points
		PathSpline->ClearSplinePoints();

		for (int32 i = 0; i < 10; i++)
		{
			// Calculate position for each point
			FVector NewPoint((i * 100) - (10 * 100) / 2, FMath::Sin(i * 0.4f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		PathSpline->UpdateSpline();

		GenerateProcMesh();
	}
	
}

// Called every frame
void ANewFunctionTesting_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float TimeElapsed = 0.0f;

	TimeElapsed += GetWorld()->GetDeltaSeconds(); // Track time progression

	if (FMath::Fmod(TimeElapsed, 5.0f) < GetWorld()->GetDeltaSeconds())
	{
		for (int32 i = 0; i < PathSpline->GetNumberOfSplinePoints(); i++)
		{
			if (i < 10 - 1)
			{
				FVector nextPoint = PathSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
				FVector currentPoint = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				currentPoint.Y = nextPoint.Y;
				PathSpline->SetLocationAtSplinePoint(i, currentPoint, ESplineCoordinateSpace::World);
				UE_LOG(LogTemp, Warning, TEXT("splinePointLoc: %s"), *currentPoint.ToString());
			}
			else
			{
				FVector LastPoint = PathSpline->GetLocationAtSplinePoint(10 - 1, ESplineCoordinateSpace::World);
				PathSpline->SetLocationAtSplinePoint(10 - 1, LastPoint, ESplineCoordinateSpace::World);
				UE_LOG(LogTemp, Warning, TEXT("LastsplinePointLoc: %s"), *LastPoint.ToString());
			}
		}

		// Update spline with new points
		PathSpline->UpdateSpline();

		GenerateProcMesh();
	}
}

void ANewFunctionTesting_CPP::GenerateProcMesh()
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

	for (int32 i = 0; i < 10; i++)
	{
		MeshSplinePoint = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (400 / 2));

		for (int32 y = 0; y < 6; y++)
		{
			if (PathVertices.Num() < 10 * 6)
			{
				// Vertice Position & Noise

				offsetCalc = ((400 / 2.0f) * (y / ((6 - 1.0f) / 2.0f)));
				MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
				PathVertices.Add(MeshVert);

				// MeshUVs
				MeshVertU = MeshVert.X / 1000;
				MeshVertV = MeshVert.Y / 1000;
				PathUVs.Add(FVector2D(MeshVertU, MeshVertV));
			}
			else
			{
				int var = i * 6;
				if (i < 9 )
				{
					PathVertices[var + y].Y = PathVertices[var + y + (6)].Y;
					//UE_LOG(LogTemp, Warning, TEXT("nextPathVert Y: %f"), PathVertices[var + y + 6].Y);
				}
				else
				{
					/*offsetCalc = ((400 / 2.0f) * (y / ((6 - 1.0f) / 2.0f)));
					MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
					PathVertices[var + y] = MeshVert;*/

					//UVs
					/*MeshVertU = MeshVert.X / 1000;
					MeshVertV = MeshVert.Y / 1000;
					PathUVs.Add(FVector2D(MeshVertU, MeshVertV));*/
				}
			}
		}
	}

	PathTriangles.Empty();
	if (PathTriangles.Num() < (((10 - 1) * (6 - 1)) * 6))
	{
		for (int i = 1; i < 10; i++)
		{
			for (int32 y = 0; y < 6 - 1; y++)
			{
				int var = (i - 1) * 6;
				PathTriangles.Add(var + y);
				PathTriangles.Add(var + y + 1);
				PathTriangles.Add(var + y + 6 + 1);
				PathTriangles.Add(var + y);
				PathTriangles.Add(var + y + 6 + 1);
				PathTriangles.Add(var + y + 6);
			}
		}
	}

	PathMesh->CreateMeshSection(0, PathVertices, PathTriangles, TArray<FVector>(), PathUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	PathMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
