#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"

class USplineComponent;
class UProceduralMeshComponent;
class UMaterialInterface;

class FMeshGeneration
{
public:

	static void GenerateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, bool& MeshInitiated, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount);

	static void UpdateRiverMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount);

	static void GeneratePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, bool& MeshInitiated, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount, int RiverOffset);

	static void UpdatePathMesh(USplineComponent* GuideSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, bool NoiseRequired, int MeshUVScale, UMaterialInterface* MeshMaterial, float NoiseFrequency, float NoiseAmplitude, int SplinePoints, int VertCount, int RiverOffset);
};