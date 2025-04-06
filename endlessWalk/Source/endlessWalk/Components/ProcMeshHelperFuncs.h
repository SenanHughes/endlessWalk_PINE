#pragma once

#include "CoreMinimal.h"
#include "DataHelper.h"

class ProcMeshHelperFuncs
{
public:

	void TriangleCalcs(TArray<FVector>& MeshVertices, TArray<int32>& MeshTriangles, int MeshVertCount, int i);

	void NormalCalcs(TArray<FVector>& MeshVertices, TArray<FVector>& MeshNormals, int MeshVertCount, int i);

	void SplineDistanceRegulating(USplineComponent* GuideSpline, int SplinePoints);

	FVector PathMeshHelper(int y, int SplinePoints, FVector MeshSplineTangent, FVector MeshEdge, FVector MeshEdgeVect, const FPathConfigData& PathConfigData, FPathDynamicData& PathDynamicData,
								const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData, const FNoiseConfigData& NoiseConfigData, FWallDynamicData& WallDynamicData, FastNoiseLite Noise);

	FVector MoundMeshHelper(int y, int SplinePoints, FVector MeshSplineTangent, FVector MeshLeftEdge, FVector MeshEdgeVect, const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData,
		const FNoiseConfigData& NoiseConfigData, FastNoiseLite Noise, float zValue);
};
