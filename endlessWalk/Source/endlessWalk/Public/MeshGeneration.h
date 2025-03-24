#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/SplineComponent.h"
#include "uProcTerrainGenerator_CPP.h"

class USplineComponent;
class UProceduralMeshComponent;
class UMaterialInterface;

class FMeshGeneration
{
public:
	static void GeneratePathMesh(USplineComponent* PathSpline, UProceduralMeshComponent* ProcMesh, int MeshWidth, TArray<FVector>& MeshVertices,
		TArray<FVector2D>& MeshUVs, TArray<int32>& MeshTriangles, int MeshUVScale, UMaterialInterface* MeshMaterial, int PathVertCount, int SplinePoints, float RiverDepth, float NoiseAmplitude, USplineComponent* RiverSpline);
};