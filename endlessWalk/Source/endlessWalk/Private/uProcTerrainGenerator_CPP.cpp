// Fill out your copyright notice in the Description page of Project Settings.

#include "uProcTerrainGenerator_CPP.h"
#include "DrawDebugHelpers.h"

// Sets default values
AuProcTerrainGenerator_CPP::AuProcTerrainGenerator_CPP()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the sphere trigger
	SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
	RootComponent = SphereTrigger;
	SphereTrigger->InitSphereRadius(40000.0f);

	// Create the
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathSpline->SetupAttachment(RootComponent);
	PathSpline->SetCanEverAffectNavigation(false);

	RiverSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RiverSpline"));
	RiverSpline->SetupAttachment(RootComponent);
	RiverSpline->SetCanEverAffectNavigation(false);

	MoundSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoundSpline"));
	MoundSpline->SetupAttachment(RootComponent);
	MoundSpline->SetCanEverAffectNavigation(false);

	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
	PathMesh->SetupAttachment(RootComponent);
	PathMesh->SetCanEverAffectNavigation(false);
	PathMesh->bUseAsyncCooking = true;
	PathMesh->ContainsPhysicsTriMeshData(true);

	RiverMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverMesh"));
	RiverMesh->SetupAttachment(RootComponent);
	RiverMesh->SetCanEverAffectNavigation(false);
	RiverMesh->bUseAsyncCooking = true;
	RiverMesh->ContainsPhysicsTriMeshData(true);

	MoundMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MoundMesh"));
	MoundMesh->SetupAttachment(RootComponent);
	MoundMesh->SetCanEverAffectNavigation(false);
	MoundMesh->bUseAsyncCooking = true;
	MoundMesh->ContainsPhysicsTriMeshData(true);

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

	if (RiverMaterial)
	{
		DynamicRiverMaterial = UMaterialInstanceDynamic::Create(RiverMaterial, this);
	}

	// Ensure the spline is correctly set up
	if (PathSpline)
	{
		// Clear any previous spline points
		PathSpline->ClearSplinePoints();
		RiverSpline->ClearSplinePoints();
		MoundSpline->ClearSplinePoints();

		Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		Noise.SetFrequency(NoiseFrequency);

		float AmplitudeScale = FMath::RandRange(1.0f, 2.5f);  // Randomly scale wave size
		float FrequencyScale = FMath::RandRange(0.8f, 1.2f);  // Random slight frequency variation

		for (int32 i = 0; i < SplinePoints; i++)
		{
			float SinWaveY = FMath::Sin((i - ((SplinePoints / 2) - 10)) * (PI / ((SplinePoints / 2) - 1)) * FrequencyScale) * (100.0f * AmplitudeScale);
			FVector NewPoint(
				((i - ((SplinePoints / 2) - 10)) * PlaneDistance),  // X position
				SinWaveY,  // Adjusted sinusoidal Y with randomness
				0.0f       // Keep Z the same (flat for now)
			);

			PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);
		}

		PathSpline->UpdateSpline();
	}

	// Set up the sphere trigger
	SphereCenter = SphereTrigger->GetComponentLocation();
	SphereRadius = SphereTrigger->GetScaledSphereRadius();
	DistanceToCenter = 0.0f;
	EdgeThreshold = SphereRadius * 0.85f;



	//GeneratePathMesh
	GeneratePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathUVScale, PathMaterial, PathVertCount);
	//FMeshGeneration::GeneratePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathUVScale, PathMaterial, PathVertCount, SplinePoints, RiverDepth, NoiseAmplitude, RiverSpline);

	//GenerateRiverMesh
	GenerateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverUVScale, DynamicRiverMaterial, RiverVertCount);

	//GenerateMoundMesh
	GenerateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundUVs, MoundTriangles, MoundUVScale, MoundMaterial, MoundVertCount);

	SpawnProceduralAssets();
}

// Called every frame
void AuProcTerrainGenerator_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CharacterPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	CharacterLocation = CharacterPawn->GetActorLocation();
	FVector MidPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints / 2, ESplineCoordinateSpace::Local);

	if (FVector(CharacterLocation - MidPoint).Size2D() <= PathWidth / 2)
	{
		PointCount += 1.0f;
		if (PointCount >= 40.0f)
		{
			ChangeCurve = true;
		}
		UpdateTerrainSpline();
	}

	LimitMovementSmoothly(PathSpline, PathWidth * 0.2f, 5.0f);
	UpdateMaterialPanner(DeltaTime);
}

void AuProcTerrainGenerator_CPP::UpdateMaterialPanner(float DeltaTime)
{
	if (DynamicRiverMaterial)
	{
		TargetPannerDirection = FVector::ZeroVector;

		for (int i = (SplinePoints / 2) - 5; i < (SplinePoints / 2) + 5; i++)
		{
			FVector CurrentPos = RiverSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			FVector NextPos = RiverSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

			TargetPannerDirection += (NextPos - CurrentPos).GetSafeNormal();
		}

		TargetPannerDirection = TargetPannerDirection.GetSafeNormal(); // Normalize the direction

		if (CurrentPannerDirection.IsNearlyZero())
		{
			CurrentPannerDirection = TargetPannerDirection;
		}

		CurrentPannerDirection = FMath::VInterpTo(CurrentPannerDirection, TargetPannerDirection, DeltaTime, TimeLimit);

		FVector UpdatedDirection = CurrentPannerDirection * 0.1f;
		FVector PannerDirection2 = FVector(CurrentPannerDirection.X * CurrentPannerDirection.Y, -(CurrentPannerDirection.X * CurrentPannerDirection.Y), 0.0f) * 0.05f;
	
		// Set the "PannerDirection" parameter in the material
		DynamicRiverMaterial->SetDoubleVectorParameterValue(FName("PannerDirection"), FLinearColor(UpdatedDirection.X, UpdatedDirection.Y, 0.0f, 0.0f));
		//DynamicRiverMaterial->SetDoubleVectorParameterValue(FName("PannerDirection2"), FLinearColor(PannerDirection2.X, PannerDirection2.Y, 0.0f, 0.0f));
	}

	/*if (DynamicRiverMaterial && RiverSpline)
	{
		int32 ClosestIndex = RiverSpline->FindInputKeyClosestToWorldLocation(CharacterLocation);

		TargetPannerDirection = RiverSpline->GetTangentAtSplinePoint(ClosestIndex, ESplineCoordinateSpace::World).GetSafeNormal();

		if (CurrentPannerDirection == FVector::ZeroVector)
		{
			CurrentPannerDirection = TargetPannerDirection;
		}

		int32 NextIndex = FMath::Clamp(ClosestIndex + 1, 0, RiverSpline->GetNumberOfSplinePoints() - 1);
		FVector NextTangent = RiverSpline->GetTangentAtSplinePoint(NextIndex, ESplineCoordinateSpace::World).GetSafeNormal();

		float CurveFactor = FMath::Clamp(FVector::DotProduct(TargetPannerDirection, NextTangent), 0.5f, 1.0f);

		CurrentPannerDirection = FMath::VInterpTo(CurrentPannerDirection, TargetPannerDirection, DeltaTime, TimeLimit);

		CurrentPannerDirection *= (0.5f);
		UE_LOG(LogTemp, Warning, TEXT("UpdatedDirection: %s"), *CurrentPannerDirection.ToString());

		DrawDebugLine(GetWorld(), CharacterLocation, CharacterLocation + CurrentPannerDirection * 1000.0f, FColor::Green, false, 0.1f, 0, 1.0f);

		DynamicRiverMaterial->SetDoubleVectorParameterValue(FName("PannerDirection"), FLinearColor(CurrentPannerDirection.X, CurrentPannerDirection.Y, 0.0f, 0.0f));
	}*/

	/*if (DynamicRiverMaterial && RiverSpline)
	{
		FVector AverageDirection = FVector::ZeroVector;

		for (int i = (SplinePoints / 2) - 1; i < (SplinePoints / 2) + 9; i++)
		{
			FVector CurrentPos = RiverSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			FVector NextPos = RiverSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

			FVector Direction = (NextPos - CurrentPos).GetSafeNormal();
			AverageDirection += Direction;  // Sum up the direction vectors
		}

		// Average the directions
		AverageDirection = AverageDirection / 10.0f;

		// Normalize the average direction to get the final rotation direction
		FVector TargetDirection = AverageDirection.GetSafeNormal();

		// Calculate the previous spline point for direction comparison
		FVector PreviousPos = RiverSpline->GetLocationAtSplinePoint((SplinePoints / 2) - 2, ESplineCoordinateSpace::World);
		FVector CurrentPos = RiverSpline->GetLocationAtSplinePoint((SplinePoints / 2) - 1, ESplineCoordinateSpace::World);
		FVector CurrentDirection = (CurrentPos - PreviousPos).GetSafeNormal();

		// Calculate the angle between the current direction and target direction
		float DotProduct = FVector::DotProduct(CurrentDirection, TargetDirection);
		float Angle = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f)); // Angle in radians
		float AngleInDegrees = FMath::RadiansToDegrees(Angle);

		// Normalize the angle to a value between 0 and 1 (based on 180 degrees max)
		float MaxRotation = 360.0f;
		float TargetNormalisedRotation = FMath::Clamp(AngleInDegrees / MaxRotation, 0.0f, 1.0f);

		// Smoothly interpolate the normalized rotation value over time
		CurrentRotation = FMath::FInterpTo(CurrentRotation, TargetNormalisedRotation, DeltaTime, 5.0f); // Adjust the speed of interpolation with the last parameter (higher = faster)

		DynamicRiverMaterial->SetScalarParameterValue(FName("RotationAngle"), CurrentRotation);
		UE_LOG(LogTemp, Warning, TEXT("CurrentRotation: %f, Angle: %f"), CurrentRotation, Angle);
	}*/
}

void AuProcTerrainGenerator_CPP::LimitMovementSmoothly(USplineComponent* GuideSpline, float MaxDistance, float PullSpeed)
{
	float ClosestDistance = GuideSpline->FindInputKeyClosestToWorldLocation(CharacterLocation);
	FVector ClosestPoint = GuideSpline->GetLocationAtSplineInputKey(ClosestDistance, ESplineCoordinateSpace::World);

	float DistanceToSpline = FVector::Dist(CharacterLocation, ClosestPoint);

	if (DistanceToSpline > MaxDistance)
	{
		FVector DirectionToSpline = (ClosestPoint - CharacterLocation).GetSafeNormal();
		FVector TargetPosition = ClosestPoint - DirectionToSpline * MaxDistance;

		// Move smoothly towards the allowed range
		FVector NewPosition = FMath::VInterpTo(CharacterLocation, TargetPosition, GetWorld()->GetDeltaSeconds(), PullSpeed);
		CharacterPawn->SetActorLocation(NewPosition);
	}
}

bool AuProcTerrainGenerator_CPP::IsPointInsideSphere(FVector Point, FVector CircleCenter, float CircleRadius)
{
	float DistanceSquared = FVector::DistSquared(Point, CircleCenter);
	return DistanceSquared <= FMath::Square(CircleRadius);
}

void AuProcTerrainGenerator_CPP::UpdateTerrainSpline()
{
	FVector LastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	FVector SecondLastPoint = PathSpline->GetLocationAtSplinePoint(SplinePoints - 2, ESplineCoordinateSpace::Local);

	FVector NewPoint = FVector::ZeroVector;

	FVector Direction = LastPoint - SecondLastPoint;
	Direction.Normalize();

	static float CurrentCurveDirectionX = 0.0f;
	static float CurrentCurveDirectionY = 0.0f;

	static float CurveSpeedMultiplier = 0.1f;


	// Every 3-6 seconds, randomise curve behavior
	if (ChangeCurve)
	{
		CurveSpeedMultiplier = FMath::RandRange(0.25f, 0.5f); // Makes turns speed up/slow down
		SharpnessMultiplier = FMath::RandRange(35.0f, 45.0f); // Adjusts how sharp turns can be
		FrequencyMultiplier = FMath::RandRange(0.002f, 0.005f); // Adjusts how frequent turns occur
		ChangeCurve = false;
		PointCount = 0;
	}

	// Generate Perlin noise with the dynamically changing frequency
	float NoiseValueX = FMath::PerlinNoise1D(LastPoint.X * FrequencyMultiplier) * SharpnessMultiplier;
	float NoiseValueY = FMath::PerlinNoise1D(LastPoint.Y * FrequencyMultiplier) * SharpnessMultiplier;

	CurrentCurveDirectionX = FMath::Lerp(CurrentCurveDirectionX, NoiseValueX, 0.1f * CurveSpeedMultiplier);
	CurrentCurveDirectionY = FMath::Lerp(CurrentCurveDirectionY, NoiseValueY, 0.1f * CurveSpeedMultiplier);

	NewPoint = LastPoint + (Direction * PlaneDistance);
	NewPoint.X += CurrentCurveDirectionX;
	NewPoint.Y += CurrentCurveDirectionY;
	NewPoint.Z = 0.0f; // Keep terrain flat

	DistanceToCenter = FVector::Dist(NewPoint, SphereCenter);


	// If the spline is getting too close to the edge, steer it back inward
	if (DistanceToCenter >= EdgeThreshold)
	{
		FVector ToCenter = (SphereCenter - NewPoint).GetSafeNormal(); // Vector pointing inward
		float InfluenceFactor = (DistanceToCenter - EdgeThreshold) / (SphereRadius - EdgeThreshold); // Smooth transition
		FVector SteeringAdjustment = ToCenter * InfluenceFactor * SharpnessMultiplier * 0.25f; // Scale influence

		NewPoint += SteeringAdjustment; // Apply gentle inward force
	}

	// Add the new point to the spline
	PathSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::Local, false);

	// Remove first spline point to keep length constant
	PathSpline->RemoveSplinePoint(0, false);
	PathVertices.RemoveAt(0, PathVertCount);
	PathUVs.RemoveAt(0, PathVertCount);

	// Update spline with new points
	PathSpline->UpdateSpline();

	UpdatePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathUVScale, PathMaterial, PathVertCount);

	//GenerateRiverMesh();
	UpdateRiverMesh(RiverSpline, RiverMesh, RiverWidth, RiverVertices, RiverUVs, RiverTriangles, RiverUVScale, DynamicRiverMaterial, RiverVertCount);

	//GenerateMoundMesh();
	UpdateMoundMesh(MoundSpline, MoundMesh, MoundWidth, MoundVertices, MoundUVs, MoundTriangles, MoundUVScale, MoundMaterial, MoundVertCount);

	UpdateProceduralAssets();
}


void AuProcTerrainGenerator_CPP::GeneratePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int PathVerts)
{
	depthOffset = RiverDepth / (PathVertCount / 4.0f);

	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth * (5.0f / 8.0f)));
		FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverDepth);

		for (int32 y = 0; y < PathVertCount; y++)
		{
			MeshVert = PathMeshHelper(y, MeshWidth, MeshEdge);

			MeshVertices.Add(MeshVert);

			// MeshUVs
			MeshVertU = MeshVert.X / MeshUVScale;
			MeshVertV = MeshVert.Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
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

	// Create the mesh
	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

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

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth * (5.0f / 8.0f)));
	FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverDepth);

	for (int32 y = 0; y < PathVertCount; y++)
	{
		MeshVert = PathMeshHelper(y, MeshWidth, MeshEdge);

		MeshVertices.Add(MeshVert);

		// MeshUVs
		MeshVertU = MeshVert.X / MeshUVScale;
		MeshVertV = MeshVert.Y / MeshUVScale;
		MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
	}

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

FVector AuProcTerrainGenerator_CPP::PathMeshHelper(int y, int MeshWidth, FVector& MeshEdge)
{
	offsetCalc = ((MeshWidth / 2.0f) * (y / ((PathVertCount - 1.0f) / 2.0f)));

	MeshVert = MeshEdge + (MeshEdgeVector * offsetCalc);

	NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
	Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

	if (y == 0)
	{
		FVector RiverSplinePoint = MeshVert - (MeshEdgeVector * (RiverWidth / 2));
		RiverSpline->AddSplinePoint(RiverSplinePoint, ESplineCoordinateSpace::Local, false);

		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

		NoisyRiverEdge.Add(MeshVert);

		while (RiverSpline->GetNumberOfSplinePoints() > SplinePoints)
		{
			// Remove first spline point to keep length constant
			RiverSpline->RemoveSplinePoint(0, false);
			NoisyRiverEdge.RemoveAt(0);
			RiverVertices.RemoveAt(0, RiverVertCount);
			RiverUVs.RemoveAt(0, RiverVertCount);
		}
	}
	else if (y < PathVertCount / 4)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Z += (depthOffset * y);
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

	RiverSpline->UpdateSpline();

	return MeshVert;
}

void AuProcTerrainGenerator_CPP::GenerateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int RiverVerts)
{
	//SplineRegulator(GuideSpline);

	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2.0f));
		FVector MoundSplinePoint = MeshLeftEdge - (MeshEdgeVector * (MoundWidth / 2));
		MoundSpline->AddSplinePoint(MoundSplinePoint, ESplineCoordinateSpace::Local, false);

		for (int32 y = 0; y < RiverVertCount; y++)
		{
			offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((RiverVertCount - 1.0f) / 2.0f) * y;
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);

			NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
			Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

			if (y == 0)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

				NoisyMoundEdge.Add(MeshVert);
			}
			else if (y < RiverVertCount - 1)
			{
				MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
				MeshVert.Z += 10.0f;
			}
			else
			{
				MeshVert = NoisyRiverEdge[i];
			}

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

	MoundSpline->UpdateSpline();

	// Create the mesh
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
	//SplineRegulator(GuideSpline);

	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2.0f));
	FVector MoundSplinePoint = MeshLeftEdge - (MeshEdgeVector * (MoundWidth / 2));
	
	MoundSpline->AddSplinePoint(MoundSplinePoint, ESplineCoordinateSpace::Local, false);

	while (MoundSpline->GetNumberOfSplinePoints() > SplinePoints)
	{
		// Remove first spline point to keep length constant
		MoundSpline->RemoveSplinePoint(0, true);
		NoisyMoundEdge.RemoveAt(0);
		MoundVertices.RemoveAt(0, MoundVertCount);
		MoundUVs.RemoveAt(0, MoundVertCount);
	}

	for (int32 y = 0; y < RiverVertCount; y++)
	{
		offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((RiverVertCount - 1.0f) / 2.0f) * y;
		//offsetCalc = ((MeshWidth / 2.0f) * (y / ((RiverVertCount - 1.0f) / 2.0f)));
		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);

		NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
		Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

		if (y == 0)
		{
			float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
			MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

			NoisyMoundEdge.Add(MeshVert);
		}
		else if (y < RiverVertCount - 1)
		{
			MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			MeshVert.Z += 10.0f;
		}
		else
		{
			MeshVert = NoisyRiverEdge[SplinePoints - 1];
		}
		MeshVertices.Add(MeshVert);

		//UVs
		MeshVertU = MeshVert.X / MeshUVScale;
		MeshVertV = MeshVert.Y / MeshUVScale;
		MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
	}

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

}

void AuProcTerrainGenerator_CPP::SplineRegulator(USplineComponent* GuideSpline)
{
	TArray<FVector> SplineArray;

	float TotalLength = GuideSpline->GetSplineLength(); // Get full spline length
	float TargetSpacing = TotalLength / (SplinePoints - 1); // Keep equal length

	for (int i = 0; i < SplinePoints; i++)
	{
		// Find correct position along base spline
		float DistanceAlongSpline = i * TargetSpacing;

		FVector SplineLocation = GuideSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);
		//GuideSpline->SetLocationAtSplinePoint(i, SplineLocation, ESplineCoordinateSpace::Local, true);
		SplineArray.Add(SplineLocation);
	}

	GuideSpline->ClearSplinePoints();
	for (int i = 0; i < SplineArray.Num(); i++)
	{
		GuideSpline->AddSplinePoint(SplineArray[i], ESplineCoordinateSpace::Local, false);
	}
	GuideSpline->UpdateSpline();
}

void AuProcTerrainGenerator_CPP::GenerateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int MoundVerts)
{
	//SplineRegulator(GuideSpline);

	for (int32 i = 0; i < SplinePoints; i++)
	{
		MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth / 2.0f);
		float zValue = zHeight;
		int valueting = MoundVertCount * 0.75f;
		float zOffset = zHeight / valueting;

		for (int32 y = 0; y < MoundVertCount; y++)
		{

			// MeshUVs
			MeshVertU = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).X / MeshUVScale;
			MeshVertV = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));

			MeshVert = MoundMeshHelper(y, MeshWidth, zValue, zOffset, i);

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

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}

void AuProcTerrainGenerator_CPP::UpdateMoundMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int MoundVerts)
{
	SplineRegulator(GuideSpline);

	MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();


	//// Might need for later - if left turns too sharp
	/*float MeshDist = FVector::Dist(GuideSpline->GetLocationAtSplinePoint(SplinePoints - 2, ESplineCoordinateSpace::Local), MeshSplinePoint);
	UE_LOG(LogTemp, Warning, TEXT("Distance between new point and mound spline last point is %f"), MeshDist);
	
	UE_LOG(LogTemp, Warning, TEXT("Distance between new point and mound spline last point is %f"), FVector::Dist(MoundSplinePoint, MoundSpline->GetLocationAtSplinePoint(MoundSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local)));
	if (FVector::Dist(MoundSplinePoint, MoundSpline->GetLocationAtSplinePoint(MoundSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local)) < 50.0f)
	{
		//MoundSplinePoint += (MeshEdgeVector * 50.0f);
		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth * .4f));
		MoundSplinePoint = MeshLeftEdge - (MeshEdgeVector * (MoundWidth / 2));
		UE_LOG(LogTemp, Warning, TEXT("NEW Distance between new point and mound spline last point is %f"), FVector::Dist(MoundSplinePoint, MoundSpline->GetLocationAtSplinePoint(MoundSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local)));
	}*/

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MeshWidth / 2.0f);

	float zValue = zHeight;
	int valueting = MoundVertCount * 0.75f;
	float zOffset = zHeight / valueting;

	for (int32 y = 0; y < MoundVertCount; y++)
	{
		MeshVert = MoundMeshHelper(y, MeshWidth, zValue, zOffset, SplinePoints - 1);

		// MeshUVs
		MeshVertU = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).X / MeshUVScale;
		MeshVertV = (MeshLeftEdge + (MeshEdgeVector * offsetCalc)).Y / MeshUVScale;

		MeshVertices.Add(MeshVert);
		MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
	}
	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

FVector AuProcTerrainGenerator_CPP::MoundMeshHelper(int y, int MeshWidth, float& zValue, float zOffset, int i)
{
	offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((MoundVertCount - 1.0f) / 1.75f) * y;
	MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc) + FVector(0.0f, 0.0f, zValue);

	NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
	Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 1.25f);

	if (y < MoundVertCount * 0.25f)
	{
		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * Falloff) * NoiseAmplitude * SmoothFactor;

		zValue -= (zOffset / (MoundVertCount * 0.25f));
	}
	else if (y == MoundVertCount * 0.25f)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;

		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * Falloff) * NoiseAmplitude * SmoothFactor;

		zValue -= zOffset;
	}
	else if (y > MoundVertCount * 0.25f && y < MoundVertCount - 1)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * (NoiseAmplitude);
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * (NoiseAmplitude);

		zValue -= zOffset;
	}
	else
	{
		MeshVert = NoisyMoundEdge[i];
	}
	return MeshVert;
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