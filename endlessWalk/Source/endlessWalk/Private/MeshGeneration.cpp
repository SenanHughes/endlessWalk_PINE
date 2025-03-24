#include "MeshGeneration.h"



//FMeshGeneration::GeneratePathMesh(PathSpline, PathMesh, PathWidth, PathVertices, PathUVs, PathTriangles, PathUVScale, PathMaterial, PathVertCount, SplinePoints, RiverDepth, NoiseAmplitude, RiverSpline);

void FMeshGeneration::GeneratePathMesh(USplineComponent* PathSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
	TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int PathVertCount, int SplinePoints, float RiverDepth, float NoiseAmplitude, USplineComponent* RiverSpline)
{
	FastNoiseLite Noise;
	/// split into two functions - generate mesh and update mesh
	for (int32 i = 0; i < SplinePoints; i++)
	{
		FVector MeshSplinePoint = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector MeshSplineTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local).GetSafeNormal();

		FVector MeshEdgeVector = FVector(-MeshSplineTangent.Y, MeshSplineTangent.X, 0.0f);

		FVector MeshLeftEdge = MeshSplinePoint - (MeshEdgeVector * (MeshWidth / 2));
		FVector MeshEdge = MeshLeftEdge - FVector(0.0f, 0.0f, RiverDepth * 2.0f);

		for (int32 y = 0; y < PathVertCount; y++)
		{
			float offsetCalc = ((MeshWidth / 2.0f) * (y / ((PathVertCount - 1.0f) / 2.0f)));
			float depthOffset = (RiverDepth * 2.0f) * (y / 2.0f);

			FVector MeshVert = MeshEdge + (MeshEdgeVector * offsetCalc);

			// MeshUVs
			float MeshVertU = MeshVert.X / MeshUVScale;
			float MeshVertV = MeshVert.Y / MeshUVScale;
			MeshUVs.Add(FVector2D(MeshVertU, MeshVertV));

			float NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
			float Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

			if (depthOffset == RiverDepth)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Z += depthOffset;

				RiverSpline->AddSplinePoint(MeshVert, ESplineCoordinateSpace::Local);
			}
			else if (y < PathVertCount / 4)
			{
				float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
				MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseAmplitude;
				MeshVert.Z += depthOffset;
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
			MeshVertices.Add(MeshVert);
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

	ProcMesh->CreateMeshSection(0, MeshVertices, MeshTriangles, TArray<FVector>(), MeshUVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set the material
	if (MeshMaterial)
	{
		ProcMesh->SetMaterial(0, MeshMaterial);
	}
}