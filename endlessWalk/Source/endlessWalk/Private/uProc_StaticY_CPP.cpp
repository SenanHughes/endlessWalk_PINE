// Fill out your copyright notice in the Description page of Project Settings.


#include "uProc_StaticY_CPP.h"
#include "FastNoiseLite.h"

// Sets default values
AuProc_StaticY_CPP::AuProc_StaticY_CPP()
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
void AuProc_StaticY_CPP::BeginPlay()
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
			FVector NewPoint((i * PlaneDistance) - (SplinePoints * PlaneDistance) / 2, FMath::Sin(i * 0.1f) * 100.0f, 0.0f);  // Simple sinusoidal curve for demonstration
			PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local);
		}

		// Optionally, update the spline to ensure it's correct
		PathSpline->UpdateSpline();
	}
	//CreateRiverSpline();

	//GeneratePathMesh
	GenerateProcMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathNoiseBool, PathUVScale, PathMaterial);
	//GenerateRiverMesh
	//GenerateProcMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverNoiseBool, RiverUVScale, RiverMaterial);
}

// Called every frame
void AuProc_StaticY_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTerrainSpline();
}

void AuProc_StaticY_CPP::GenerateProcMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
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

	
	//MeshVertices.Empty();
	//MeshUVs.Empty();

	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));

		for (int32 y = 0; y < VertCount; y++)
		{			
			// Vertice Position & Noise
			if (MeshVertices.Num() != SplinePoints * VertCount)
			{
				offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
				MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
				if (NoiseRequired && (0 < y) && (y < VertCount - 1))
				{
					NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
					float Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
					MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
				}
				MeshVertices.Add(MeshVert);

				// MeshUVs
			
				/* {
					if (i == SplinePoints - 1)
					{
						MeshUVs[(i * VertCount) + y].Y = MeshUVs[((i * VertCount) + y) + VertCount].Y;
					}
				}
				else*/
				
				MeshVertU = MeshVert.X / MeshUVScale;
				MeshVertV = MeshVert.Y / MeshUVScale;

				MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
				UE_LOG(LogTemp, Warning, TEXT("current UV: %s"), *MeshUVs[(i * VertCount) + y].ToString());
			}
			else
			{
				int var = i * VertCount;
				if (i < SplinePoints - 1)
				{
					//float DeltaY = MeshVertices[var + y + (VertCount)].Y - MeshVertices[var + y].Y;
					float YUVWorldPosition = ProcMesh->GetComponentLocation().Y + MeshVertices[var + y].Y;
					float YNewUVWorldPosition = ProcMesh->GetComponentLocation().Y + MeshVertices[var + y + (VertCount)].Y;
					float XUVWorldPosition = ProcMesh->GetComponentLocation().X + MeshVertices[var + y].X;
					float XNewUVWorldPosition = ProcMesh->GetComponentLocation().X + MeshVertices[var + y + (VertCount)].X;
					float YWorldScale = ProcMesh->GetComponentScale().Y;

					float DeltaY = YNewUVWorldPosition - YUVWorldPosition;
					float DeltaX = XNewUVWorldPosition - XUVWorldPosition;

					UE_LOG(LogTemp, Warning, TEXT("DeltaY: %f"), DeltaY);


					MeshVertices[var + y].Y = MeshVertices[var + y + (VertCount)].Y;

					MeshVertU = MeshVertices[var + y].X / MeshUVScale;
					MeshVertV = MeshVertices[var + y].Y / MeshUVScale;

					MeshUVs[var + y].X = MeshVertU;
					MeshUVs[var + y].Y = MeshVertV;


					//UE_LOG(LogTemp, Warning, TEXT("current UV: %f"), MeshUVs[var + y].Y);
					//MeshUVs[var + y].Y = MeshUVs[var + y + (VertCount)].Y;
					//UE_LOG(LogTemp, Warning, TEXT("DeltaY: %f"), DeltaY);
					//MeshUVs[var + y].X -= (DeltaX);
					//MeshUVs[var + y].Y -= (DeltaY);
					UE_LOG(LogTemp, Warning, TEXT("current UV: %s"), *MeshUVs[var + y].ToString());
				}
				else
				{
					offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
					MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
					if (NoiseRequired && (0 < y) && (y < VertCount - 1))
					{
						NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
						float Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
						MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
					}
					MeshVertices[var + y].Y = MeshVert.Y;
					MeshVertices[var + y].Z = MeshVert.Z;
					MeshVertU = MeshVert.X / MeshUVScale;
					MeshVertV = MeshVert.Y / MeshUVScale;
					MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
				}
			}
			/*else
			{
				if (i < SplinePoints - 1)
				{
						
				}
				else
				{
					offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
					MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
					if (NoiseRequired && (0 < y) && (y < VertCount - 1))
					{
						NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
						float Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
						MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
					}
					MeshVertices[var + y] = MeshVert;

					//UVs
					MeshVertU = MeshVert.X / MeshUVScale;
					MeshVertV = MeshVert.Y / MeshUVScale;
					MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
				}
			}*/
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("MeshUVs: %d"), MeshUVs.Num());

	MeshTriangles.Empty();
	if (MeshTriangles.Num() < (((SplinePoints - 1) * (VertCount - 1)) * 6)) // times 6 because it's the amount of points per triangle (2 triangles per square)
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

void AuProc_StaticY_CPP::UpdateTerrainSpline()
{
	if (SplinePoints < 2)
	{
		return;
	}
	

	static float CurrentCurveDirectionX = 0.0f;
	static float CurrentCurveDirectionY = 0.0f;

	static float CurveSpeedMultiplier = 0.1f;
	static float SharpnessMultiplier = 100.0f;
	static float FrequencyMultiplier = 0.005f;

	static float TimeElapsed = 0.0f;
	TimeElapsed += GetWorld()->GetDeltaSeconds(); // Track time progression
	

	FVector CharacterLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	if (FMath::Fmod(TimeElapsed, 1.0f) < GetWorld()->GetDeltaSeconds())
	{
		FVector LastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
		
		CurveSpeedMultiplier = FMath::RandRange(0.5f, 1.0f); // Makes turns speed up/slow down
		SharpnessMultiplier = FMath::RandRange(100.0f, 200.0f); // Adjusts how sharp turns can be
		FrequencyMultiplier = FMath::RandRange(0.002f, 0.008f); // Adjusts how frequent turns occur

		// Generate Perlin noise with the dynamically changing frequency
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
				float YDeviation = NextPoint.Y - PrevPoint.Y; // Only considers X
				TotalDeviation += YDeviation;
			}

			AverageDeviation = TotalDeviation / CheckPoints;

			// Define a "too straight" threshold (adjustable)
			IsTooStraight = AverageDeviation < DeviationThreshold; // If deviation is too small, force a turn
		}

		if (IsTooStraight)
		{
			NoiseValueY += FMath::RandRange(-50.0f, 50.0f); // Strong forced curve in X
		}

		CurrentCurveDirectionY = FMath::Lerp(CurrentCurveDirectionY, NoiseValueY, 0.1f * CurveSpeedMultiplier);

		LastPoint.Y += CurrentCurveDirectionY;

		for (int32 i = 0; i < PathSpline->GetNumberOfSplinePoints(); i++)
		{
			if (i < SplinePoints - 1)
			{
				if (i == 0)
				{
					for (int j = 0; j < VertCount; j++)
					{
						PathUVs.RemoveAt(j);
					}
				}
				FVector nextPoint = PathSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
				FVector currentPoint = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
				currentPoint.Y = nextPoint.Y;
				PathSpline->SetLocationAtSplinePoint(i, currentPoint, ESplineCoordinateSpace::Local);
			}
			else
			{
				PathSpline->SetLocationAtSplinePoint(SplinePoints - 1, LastPoint, ESplineCoordinateSpace::Local);
			}
		}

		// Update spline with new points
		PathSpline->UpdateSpline();

		//UpdateRiverSpline();

		GenerateProcMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathNoiseBool, PathUVScale, PathMaterial);
		//GenerateRiverMesh();
		//GenerateProcMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverNoiseBool, RiverUVScale, RiverMaterial);
	}
}

void AuProc_StaticY_CPP::CreateRiverSpline()
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

void AuProc_StaticY_CPP::UpdateRiverSpline()
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

