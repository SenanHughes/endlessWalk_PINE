#pragma once

#include "CoreMinimal.h"
#include "DataHelper.h"

class AssetHelperFuncs
{
public:

	TArray<FVector> GetValidSpawnPoints(UProceduralMeshComponent* Mesh, USplineComponent* GuideSpline, int MeshVertCount, int VertMin, int VertMax);

	bool PointInTriangle2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, const FVector2D& C);

	FVector InterpolateHeight(const FVector& Point, UProceduralMeshComponent* Mesh);
};

