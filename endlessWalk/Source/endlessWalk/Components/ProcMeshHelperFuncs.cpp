#include "ProcMeshHelperFuncs.h"

void ProcMeshHelperFuncs::TriangleCalcs(TArray<FVector>& MeshVertices, TArray<int32>& MeshTriangles, int MeshVertCount, int i)
{
	int Row = i / (MeshVertCount - 1);
	int Col = i % (MeshVertCount - 1);

	// Compute the four vertex indices
	int A = (Row * MeshVertCount) + Col;
	int B = A + 1;
	int C = A + MeshVertCount;
	int D = C + 1;

	// First triangle (Counter-Clockwise)
	MeshTriangles.Add(A);
	MeshTriangles.Add(B);
	MeshTriangles.Add(C);

	// Second triangle (Counter-Clockwise)
	MeshTriangles.Add(C);
	MeshTriangles.Add(B);
	MeshTriangles.Add(D);
}

void ProcMeshHelperFuncs::NormalCalcs(TArray<FVector>& MeshVertices, TArray<FVector>& MeshNormals, int MeshVertCount, int i)
{
	FVector CurrentVertex = MeshVertices[i];

	bool bHasForwardPoint = i + MeshVertCount < MeshVertices.Num();
	bool bHasBackwardPoint = i - MeshVertCount >= 0;
	bool bHasRightPoint = (i + 1) % MeshVertCount != 0;
	bool bHasLeftPoint = i % MeshVertCount != 0;

	FVector NormalSum = FVector::ZeroVector;
	int NormalCount = 0;

	if (bHasForwardPoint && bHasRightPoint)
	{
		NormalSum += FVector::CrossProduct(MeshVertices[i + MeshVertCount] - CurrentVertex, MeshVertices[i + 1] - CurrentVertex);
		NormalCount++;
	}
	if (bHasForwardPoint && bHasLeftPoint)
	{
		NormalSum += FVector::CrossProduct(MeshVertices[i + MeshVertCount] - CurrentVertex, CurrentVertex - MeshVertices[i - 1]);
		NormalCount++;
	}
	if (bHasBackwardPoint && bHasRightPoint)
	{
		NormalSum += FVector::CrossProduct(CurrentVertex - MeshVertices[i - MeshVertCount], MeshVertices[i + 1] - CurrentVertex);
		NormalCount++;
	}
	if (bHasBackwardPoint && bHasLeftPoint)
	{
		NormalSum += FVector::CrossProduct(CurrentVertex - MeshVertices[i - MeshVertCount], CurrentVertex - MeshVertices[i - 1]);
		NormalCount++;
	}

	if (NormalCount > 0)
	{
		MeshNormals.Add(NormalSum.GetSafeNormal());
	}
	else
	{
		MeshNormals.Add(FVector::UpVector);
	}
}

void ProcMeshHelperFuncs::SplineDistanceRegulating(USplineComponent* GuideSpline, int SplinePoints)
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

FVector ProcMeshHelperFuncs::PathMeshHelper(int y, int SplinePoints, FVector MeshSplineTangent, FVector MeshLeftEdge, FVector MeshEdgeVect, const FPathConfigData& PathConfigData, FPathDynamicData& PathDynamicData,
										const FRiverConfigData& RiverConfigData, FRiverDynamicData& RiverDynamicData, const FNoiseConfigData& NoiseConfigData, FWallDynamicData& WallDynamicData, FastNoiseLite Noise)
{
	float offsetCalc = ((PathConfigData.PathWidth / 2.0f) * (y / ((PathConfigData.PathVertCount - 1.0f) / 2.0f)));

	FVector MeshVert = MeshLeftEdge + (MeshEdgeVect * offsetCalc);

	float NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
	float Falloff = FMath::Lerp(0.5f, 1.3f, (NoiseValue + 1.0f) / 2.0f);

	if (y == 0)
	{
		FVector RiverSplinePoint = MeshVert - (MeshEdgeVect * (RiverConfigData.RiverWidth / 2));
		RiverDynamicData.RiverSpline->AddSplinePoint(RiverSplinePoint, ESplineCoordinateSpace::Local, false);

		while (RiverDynamicData.RiverSpline->GetNumberOfSplinePoints() > SplinePoints)
		{
			//Remove first spline point to keep length constant
			RiverDynamicData.RiverSpline->RemoveSplinePoint(0, false);
			RiverDynamicData.RiverVertices.RemoveAt(0, RiverConfigData.RiverVertCount);
			RiverDynamicData.VertexColours.RemoveAt(0, RiverConfigData.RiverVertCount);
			RiverDynamicData.RiverNormals.RemoveAt(0, RiverConfigData.RiverVertCount);
			RiverDynamicData.RiverTangents.RemoveAt(0, RiverConfigData.RiverVertCount);
		}
	}
	else if (y < PathConfigData.PathVertCount / 4)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		float SmoothFactor = FMath::Pow(0.85f + 0.15f * FMath::Cos((y / (float)PathConfigData.PathVertCount) * PI), 2.5f);

		if (MeshSplineTangent.X - MeshSplineTangent.Y > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (NoiseConfigData.NoiseAmplitude);
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (30.0f) * SmoothFactor;
		}
		else if (MeshSplineTangent.Y - MeshSplineTangent.X > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (30.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (NoiseConfigData.NoiseAmplitude);
		}
		else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (20.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (20.0f) * SmoothFactor;
		}
		MeshVert.Z += (PathDynamicData.DepthOffset * y);
	}
	else if (y == PathConfigData.PathVertCount / 4)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * Falloff) * NoiseConfigData.NoiseAmplitude;
	}
	else if (y > PathConfigData.PathVertCount / 4 && y < PathConfigData.PathVertCount - (PathConfigData.PathVertCount / 4) - 1)
	{
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * Falloff) * NoiseConfigData.NoiseAmplitude;
	}
	else if (y == PathConfigData.PathVertCount - (PathConfigData.PathVertCount / 4) - 1)
	{
		WallDynamicData.WallSpline->AddSplinePoint(MeshVert + FVector(0.0f, 0.0f, RiverConfigData.RiverDepth * 0.9f), ESplineCoordinateSpace::Local, false);

		while (WallDynamicData.WallSpline->GetNumberOfSplinePoints() > SplinePoints)
		{
			WallDynamicData.WallSpline->RemoveSplinePoint(0, false);
		}

		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;
		Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 2.0f);
		MeshVert.Z = (NoiseValue * Falloff) * NoiseConfigData.NoiseAmplitude;
	}
	else
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;
		MeshVert.Z += (PathDynamicData.DepthOffset * ((PathConfigData.PathVertCount - 1) - y));
	}
	RiverDynamicData.RiverSpline->UpdateSpline();
	WallDynamicData.WallSpline->UpdateSpline();

	return MeshVert;
}

FVector ProcMeshHelperFuncs::MoundMeshHelper(int y, int SplinePoints, FVector MeshSplineTangent, FVector MeshLeftEdge, FVector MeshEdgeVect, const FMoundConfigData& MoundConfigData, FMoundDynamicData& MoundDynamicData,
											const FNoiseConfigData& NoiseConfigData, FastNoiseLite Noise, float zValue)
{
	float offsetCalc = ((MoundConfigData.MoundWidth / 2.0f) * (y / ((MoundConfigData.MoundVertCount - 1.0f) / 2.0f)));
	FVector MeshVert = MeshLeftEdge + (MeshEdgeVect * offsetCalc) + FVector(0.0f, 0.0f, zValue);

	float NoiseValue = Noise.GetNoise(MeshVert.X, MeshVert.Y);
	float Falloff = 1.0f - FMath::Pow(FMath::Abs(NoiseValue), 1.25f);

	if (y == 0)
	{
		MeshVert.Z -= 600.0f;
	}
	else if (y == 1)
	{
		MeshVert.Z -= 30.0f;
	}
	else if (y < (MoundConfigData.MoundVertCount - 2) * 0.25f)
	{
		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundConfigData.MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * Falloff) * 30.0f * SmoothFactor;
	}
	else if (y == (MoundConfigData.MoundVertCount - 2) * 0.25f)
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		MeshVert.X += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;
		MeshVert.Y += (NoiseValue * XYNoiseFactor) * NoiseConfigData.NoiseAmplitude;

		float SmoothFactor = 0.5f + 0.5f * FMath::Cos((y / (float)MoundConfigData.MoundVertCount) * PI);
		MeshVert.Z += (NoiseValue * Falloff) * NoiseConfigData.NoiseAmplitude * SmoothFactor;
	}
	else
	{
		float XYNoiseFactor = FMath::Lerp(0.3f, 1.0f, Falloff);
		float SmoothFactor = FMath::Pow(0.85f + 0.15f * FMath::Cos((y / (float)MoundConfigData.MoundVertCount) * PI), 2.5f);

		if (MeshSplineTangent.X - MeshSplineTangent.Y > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (NoiseConfigData.NoiseAmplitude);
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (50.0f) * SmoothFactor;
		}
		else if (MeshSplineTangent.Y - MeshSplineTangent.X > 0.2f)
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (50.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (NoiseConfigData.NoiseAmplitude);
		}
		else
		{
			MeshVert.X += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
			MeshVert.Y += (NoiseValue * XYNoiseFactor) * (40.0f) * SmoothFactor;
		}
	}
	return MeshVert;
}