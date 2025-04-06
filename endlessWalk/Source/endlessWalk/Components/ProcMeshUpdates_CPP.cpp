// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcMeshUpdates_CPP.h"

// Sets default values for this component's properties
UProcMeshUpdates_CPP::UProcMeshUpdates_CPP()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UProcMeshUpdates_CPP::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProcMeshUpdates_CPP::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UProcMeshUpdates_CPP::UpdatePathMesh(const FPathConfigData& PathConfigData, FPathDynamicData& PathDynamicData, const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData,
	const FWallConfigData& WallConfigData, FWallDynamicData& WallDynamicData, const FNoiseConfigData& NoiseConfigData, FNoiseDynamicData& NoiseDynamicData, const FSplineConfigData SplineConfigData)
{
	MeshSplinePoint = PathDynamicData.PathSpline->GetLocationAtSplinePoint(SplineConfigData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = PathDynamicData.PathSpline->GetTangentAtSplinePoint(SplineConfigData.SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (PathConfigData.PathWidth * 0.5f));
	FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverConfigData.RiverDepth);

	for (int32 y = 0; y < PathConfigData.PathVertCount; y++)
	{
		MeshVert = MeshHelperFuncs.PathMeshHelper(y, SplineConfigData.SplinePoints, MeshSplineTangent, MeshEdge, MeshEdgeVector, PathConfigData, PathDynamicData, RiverConfigData, RiverDynamicData, NoiseConfigData, WallDynamicData, NoiseDynamicData.Noise);

		PathDynamicData.PathVertices.Add(MeshVert);
		PathDynamicData.PathTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
	}

	for (int i = 0; i < PathConfigData.PathVertCount; i++)
	{
		MeshHelperFuncs.NormalCalcs(PathDynamicData.PathVertices, PathDynamicData.PathNormals, PathConfigData.PathVertCount, (PathDynamicData.PathVertices.Num() - PathConfigData.PathVertCount) + i);
	}
	PathDynamicData.PathMesh->UpdateMeshSection(0, PathDynamicData.PathVertices, PathDynamicData.PathNormals, TArray<FVector2D>(), TArray<FColor>(), PathDynamicData.PathTangents);
}

void UProcMeshUpdates_CPP::UpdateRiverMesh(const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData, const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData, const FSplineConfigData SplineConfigData)
{
	MeshSplinePoint = RiverDynamicData.RiverSpline->GetLocationAtSplinePoint(SplineConfigData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = RiverDynamicData.RiverSpline->GetTangentAtSplinePoint(SplineConfigData.SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	FProcMeshTangent MeshTangent = FProcMeshTangent(MeshSplineTangent, false);
	FVector TangentX = MeshTangent.TangentX.GetSafeNormal();
	if (TangentX.X < 0.0f)
	{
		TangentX.X = -TangentX.X;
	}
	if (TangentX.Y < 0.0f)
	{
		TangentX.Y = -TangentX.Y;
	}
	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (RiverConfigData.RiverWidth / 1.8f));

	FVector MoundSplinePoint = (MeshSplinePoint - (MeshEdgeVector * (RiverConfigData.RiverWidth / 2.0f))) - (MeshEdgeVector * (MoundConfigData.MoundWidth / 2));
	MoundDynamicData.MoundSpline->AddSplinePoint(MoundSplinePoint, ESplineCoordinateSpace::Local, false);

	while (MoundDynamicData.MoundSpline->GetNumberOfSplinePoints() > SplineConfigData.SplinePoints)
	{
		// Remove first spline point to keep length constant
		MoundDynamicData.MoundSpline->RemoveSplinePoint(0, true);
		MoundDynamicData.MoundVertices.RemoveAt(0, MoundConfigData.MoundVertCount);
		MoundDynamicData.MoundNormals.RemoveAt(0, MoundConfigData.MoundVertCount);
		MoundDynamicData.MoundTangents.RemoveAt(0, MoundConfigData.MoundVertCount);
	}

	for (int32 y = 0; y < RiverConfigData.RiverVertCount; y++)
	{
		float offsetCalc = FVector::Dist(MeshLeftEdge, MeshSplinePoint) / ((RiverConfigData.RiverVertCount - 1.0f) / 2.0f) * y;
		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);

		MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
		MeshVert.Z += 10.0f;

		RiverDynamicData.RiverVertices.Add(MeshVert);

		RiverDynamicData.RiverTangents.Add(MeshTangent);
		RiverDynamicData.VertexColours.Add(FLinearColor(TangentX.X, TangentX.Y, 0.5f, 1.0f));
	}

	for (int i = 0; i < RiverConfigData.RiverVertCount; i++)
	{
		MeshHelperFuncs.NormalCalcs(RiverDynamicData.RiverVertices, RiverDynamicData.RiverNormals, RiverConfigData.RiverVertCount, (RiverDynamicData.RiverVertices.Num() - RiverConfigData.RiverVertCount) + i);
	}
	RiverDynamicData.RiverMesh->UpdateMeshSection_LinearColor(0, RiverDynamicData.RiverVertices, RiverDynamicData.RiverNormals, TArray<FVector2D>(), RiverDynamicData.VertexColours, RiverDynamicData.RiverTangents);
}

void UProcMeshUpdates_CPP::UpdateMoundMesh(const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData, const FRiverConfigData& RiverConfigData, const FNoiseConfigData& NoiseConfigData, FNoiseDynamicData& NoiseDynamicData, 
											const FSplineConfigData SplineConfigData)
{
	MeshHelperFuncs.SplineDistanceRegulating(MoundDynamicData.MoundSpline, SplineConfigData.SplinePoints);

	MeshSplinePoint = MoundDynamicData.MoundSpline->GetLocationAtSplinePoint(SplineConfigData.SplinePoints - 1, ESplineCoordinateSpace::Local);
	MeshSplineTangent = MoundDynamicData.MoundSpline->GetTangentAtSplinePoint(SplineConfigData.SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * MoundConfigData.MoundWidth / 2.0f);

	float zValue = MoundConfigData.MoundHeight;

	for (int32 y = 0; y < MoundConfigData.MoundVertCount; y++)
	{
		if (y < MoundConfigData.MoundVertCount * 0.25f)
		{
			MeshVert = MeshHelperFuncs.MoundMeshHelper(y, SplineConfigData.SplinePoints, MeshSplineTangent, MeshLeftEdge, MeshEdgeVector, MoundConfigData, MoundDynamicData, NoiseConfigData, NoiseDynamicData.Noise, zValue);
			zValue -= MoundDynamicData.zOffset25;
		}
		else
		{
			MeshVert = MeshHelperFuncs.MoundMeshHelper(y, SplineConfigData.SplinePoints, MeshSplineTangent, MeshLeftEdge, MeshEdgeVector, MoundConfigData, MoundDynamicData, NoiseConfigData, NoiseDynamicData.Noise, zValue);
			zValue -= MoundDynamicData.zOffset75;
		}
		MoundDynamicData.MoundVertices.Add(MeshVert);
		MoundDynamicData.MoundTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
	}

	for (int i = 0; i < MoundConfigData.MoundVertCount; i++)
	{
		MeshHelperFuncs.NormalCalcs(MoundDynamicData.MoundVertices, MoundDynamicData.MoundNormals, MoundConfigData.MoundVertCount, (MoundDynamicData.MoundVertices.Num() - MoundConfigData.MoundVertCount) + i);
	}
	MoundDynamicData.MoundMesh->UpdateMeshSection(0, MoundDynamicData.MoundVertices, MoundDynamicData.MoundNormals, TArray<FVector2D>(), TArray<FColor>(), MoundDynamicData.MoundTangents);
}