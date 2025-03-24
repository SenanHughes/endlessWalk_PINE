#include "MeshGeneration.h"
#include "Components/SplineComponent.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"

void FMeshGeneration::GenerateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, bool& MeshInitiated, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount)
{
	/// split into two functions - generate mesh and update mesh
	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		FVector MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));

		for (int32 y = 0; y < VertCount; y++)
		{
			float offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
			FVector MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			MeshVertices.Add(MeshVert);

			// MeshUVs
			float MeshVertU = MeshVert.X / MeshUVScale;
			float MeshVertV = MeshVert.Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
		}
	}

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

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}

	MeshInitiated = true;
}

void FMeshGeneration::UpdateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount)
{
	FVector MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	FVector MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	FVector MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	FVector MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));

	for (int32 y = 0; y < VertCount; y++)
	{
		float offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
		FVector MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
		MeshVertices.Add(MeshVert);

		//UVs
		float MeshVertU = MeshVert.X / MeshUVScale;
		float MeshVertV = MeshVert.Y / MeshUVScale;
		MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
	}
	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}



void FMeshGeneration::GeneratePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, bool& MeshInitiated, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount, int RiverOffset)
{
	FastNoiseLite Noise;
	Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise.SetFrequency(NoiseFrequency);

	/// split into two functions - generate mesh and update mesh
	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector MeshVert = FVector::ZeroVector;
		FVector MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		FVector MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));
		FVector MeshRiverEdge = MeshLeftEdge - FVector(-MeshSplineTangent.Y * 50.0f, MeshSplineTangent.X * 50.0f, RiverOffset);

		for (int32 y = 0; y < VertCount; y++)
		{
			float offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
			if (y < 2)
			{
				MeshVert = MeshRiverEdge + (MeshEdgeVector * offsetCalc);
				MeshVert.Z = MeshVert.Z + RiverOffset * (y / 2);
			}
			else
			{
				MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			}
			//MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			if (NoiseRequired && (2 < y) && (y < VertCount - 1))
			{
				float NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
				float Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
				MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
			}
			MeshVertices.Add(MeshVert);

			// MeshUVs
			float MeshVertU = MeshVert.X / MeshUVScale;
			float MeshVertV = MeshVert.Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
		}
	}

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

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}

	MeshInitiated = true;
}

void FMeshGeneration::UpdatePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount, int RiverOffset)
{
	FastNoiseLite Noise;
	Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise.SetFrequency(NoiseFrequency);

	FVector MeshVert = FVector::ZeroVector;

	FVector MeshSplinePoint = GuideSpline->GetLocationAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local);
	FVector MeshSplineTangent = GuideSpline->GetTangentAtSplinePoint(SplinePoints - 1, ESplineCoordinateSpace::Local).GetSafeNormal();

	FVector MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

	FVector MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));
	FVector MeshRiverEdge = MeshLeftEdge - FVector(-MeshSplineTangent.Y * 50.0f, MeshSplineTangent.X * 50.0f, RiverOffset);

	for (int32 y = 0; y < VertCount; y++)
	{
		float offsetCalc = ((MeshWidth / 2.0f) * (y / ((VertCount - 1.0f) / 2.0f)));
		if (y < 2)
			{
				MeshVert = MeshRiverEdge + (MeshEdgeVector * offsetCalc);
				MeshVert.Z = MeshVert.Z + RiverOffset * (y / 2);
			}
			else
			{
				MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			}
			//MeshVert = MeshLeftEdge + (MeshEdgeVector * offsetCalc);
			if (NoiseRequired && (2 < y) && (y < VertCount - 1))
			{
			float NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
			float Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
			MeshVert.Z = (NoiseValue * Falloff) * NoiseAmplitude;
		}
		MeshVertices.Add(MeshVert);

		//UVs
		float MeshVertU = MeshVert.X / MeshUVScale;
		float MeshVertV = MeshVert.Y / MeshUVScale;
		MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));
	}
	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}