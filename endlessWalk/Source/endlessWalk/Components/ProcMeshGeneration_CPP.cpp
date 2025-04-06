// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcMeshGeneration_CPP.h"

// Sets default values for this component's properties
UProcMeshGeneration_CPP::UProcMeshGeneration_CPP()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UProcMeshGeneration_CPP::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UProcMeshGeneration_CPP::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UProcMeshGeneration_CPP::GeneratePathMesh(const FPathConfigData& PathConfigData, FPathDynamicData& PathDynamicData, const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData, 
												const FWallConfigData& WallConfigData, FWallDynamicData& WallDynamicData, const FNoiseConfigData& NoiseConfigData, FNoiseDynamicData& NoiseDynamicData, const FSplineConfigData SplineConfigData)
{
	PathDynamicData.DepthOffset = RiverConfigData.RiverDepth / (PathConfigData.PathVertCount / 4.0f);

	for (int32 i = 0; i < SplineConfigData.SplinePoints; i++)
	{
		MeshSplinePoint = PathDynamicData.PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = PathDynamicData.PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (PathConfigData.PathWidth * 0.5f));
		MeshLeftEdge.Z -= RiverConfigData.RiverDepth;

		for (int32 y = 0; y < PathConfigData.PathVertCount; y++)
		{
			MeshVert = MeshHelperFuncs.PathMeshHelper(y, SplineConfigData.SplinePoints, MeshSplineTangent, MeshLeftEdge, MeshEdgeVector, PathConfigData, PathDynamicData, RiverConfigData, RiverDynamicData, NoiseConfigData, WallDynamicData, NoiseDynamicData.Noise);

			PathDynamicData.PathVertices.Add(MeshVert);
			PathDynamicData.PathTangents.Add(FProcMeshTangent(MeshSplineTangent, false));
		}
	}

	for (int i = 0; i < PathDynamicData.PathVertices.Num(); i++)
	{
		if (i < (SplineConfigData.SplinePoints - 1) * (PathConfigData.PathVertCount - 1))
		{
			MeshHelperFuncs.TriangleCalcs(PathDynamicData.PathVertices, PathDynamicData.PathTriangles, PathConfigData.PathVertCount, i);
		}
		MeshHelperFuncs.NormalCalcs(PathDynamicData.PathVertices, PathDynamicData.PathNormals, PathConfigData.PathVertCount, i);
	}
	PathDynamicData.PathMesh->CreateMeshSection(0, PathDynamicData.PathVertices, PathDynamicData.PathTriangles, PathDynamicData.PathNormals, TArray<FVector2D>(), TArray<FColor>(), PathDynamicData.PathTangents, true);

	// Set the material
	if (PathConfigData.PathMaterial)
	{
		PathDynamicData.PathMesh->SetMaterial(0, PathConfigData.PathMaterial);
	}
}

void UProcMeshGeneration_CPP::GenerateRiverMesh(const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData, const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData,
												const FSplineConfigData SplineConfigData)
{
	for (int32 i = 0; i < SplineConfigData.SplinePoints; i++)
	{
		MeshSplinePoint = RiverDynamicData.RiverSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = RiverDynamicData.RiverSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

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

	}

	MoundDynamicData.MoundSpline->UpdateSpline();

	for (int i = 0; i < RiverDynamicData.RiverVertices.Num(); i++)
	{
		if (i < (SplineConfigData.SplinePoints - 1) * (RiverConfigData.RiverVertCount - 1))
		{
			MeshHelperFuncs.TriangleCalcs(RiverDynamicData.RiverVertices, RiverDynamicData.RiverTriangles, RiverConfigData.RiverVertCount, i);
		}
		MeshHelperFuncs.NormalCalcs(RiverDynamicData.RiverVertices, RiverDynamicData.RiverNormals, RiverConfigData.RiverVertCount, i);
	}
	RiverDynamicData.RiverMesh->CreateMeshSection_LinearColor(0, RiverDynamicData.RiverVertices, RiverDynamicData.RiverTriangles, RiverDynamicData.RiverNormals, TArray<FVector2D>(), RiverDynamicData.VertexColours, RiverDynamicData.RiverTangents, true, false);

	// Set the material
	if (RiverDynamicData.DynamicRiverMaterial)
	{
		RiverDynamicData.RiverMesh->SetMaterial(0, RiverDynamicData.DynamicRiverMaterial);
	}
}

void UProcMeshGeneration_CPP::GenerateMoundMesh(const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData, const FRiverConfigData& RiverConfigData, const FNoiseConfigData& NoiseConfigData, FNoiseDynamicData& NoiseDynamicData, 
													const FSplineConfigData SplineConfigData)
{
	MoundDynamicData.verts75 = MoundConfigData.MoundVertCount * 0.75f;
	MoundDynamicData.verts25 = MoundConfigData.MoundVertCount - MoundDynamicData.verts75;
	MoundDynamicData.zOffset75 = MoundConfigData.MoundHeight / MoundDynamicData.verts75 + 1;
	MoundDynamicData.zOffset25 = MoundDynamicData.zOffset75 / MoundDynamicData.verts25;

	for (int32 i = 0; i < SplineConfigData.SplinePoints; i++)
	{
		MeshSplinePoint = MoundDynamicData.MoundSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		MeshSplineTangent = MoundDynamicData.MoundSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

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
	}

	for (int i = 0; i < MoundDynamicData.MoundVertices.Num(); i++)
	{
		if (i < (SplineConfigData.SplinePoints - 1) * (MoundConfigData.MoundVertCount - 1))
		{
			MeshHelperFuncs.TriangleCalcs(MoundDynamicData.MoundVertices, MoundDynamicData.MoundTriangles, MoundConfigData.MoundVertCount, i);
		}
		MeshHelperFuncs.NormalCalcs(MoundDynamicData.MoundVertices, MoundDynamicData.MoundNormals, MoundConfigData.MoundVertCount, i);
	}

	MoundDynamicData.MoundMesh->CreateMeshSection(0, MoundDynamicData.MoundVertices, MoundDynamicData.MoundTriangles, MoundDynamicData.MoundNormals, TArray<FVector2D>(), TArray<FColor>(), MoundDynamicData.MoundTangents, true);

	// Set the material
	if (MoundConfigData.MoundMaterial)
	{
		MoundDynamicData.MoundMesh->SetMaterial(0, MoundConfigData.MoundMaterial);
	}
}