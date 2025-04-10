#include "AssetHelperFuncs.h"

TArray<FVector> AssetHelperFuncs::GetValidSpawnPoints(UProceduralMeshComponent* Mesh, USplineComponent* GuideSpline, int MeshVertCount, int VertMin, int VertMax)
{
	TArray<FVector> SpawnPoints;
	FProcMeshSection* Section = Mesh->GetProcMeshSection(0);

	for (int i = VertMin; i < VertMax; i++)
	{
		FProcMeshVertex& Vertex = Section->ProcVertexBuffer[i];
		FVector Normal = Vertex.Normal;
		float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Normal, FVector::UpVector)));

		if (SlopeAngle > 25.0f && Vertex.Position.Z != Section->SectionLocalBox.Min.Z)
		{
			FVector ClosestSplinePoint = GuideSpline->FindLocationClosestToWorldLocation(Vertex.Position, ESplineCoordinateSpace::Local);
			FVector RightVector = GuideSpline->GetRightVectorAtSplinePoint(ClosestSplinePoint.X, ESplineCoordinateSpace::Local);
			FVector ToPoint = Vertex.Position - ClosestSplinePoint;

			if (GuideSpline->GetName() == "PathSpline" && FVector::DotProduct(ToPoint, RightVector) < 0.0f)
			{
				SpawnPoints.Add(Vertex.Position);
			}
			else if (GuideSpline->GetName() == "MoundSpline" && FVector::DotProduct(ToPoint, RightVector) >= -100.0f)
			{
				SpawnPoints.Add(Vertex.Position);
			}
		}

	}
	return SpawnPoints;
}

bool AssetHelperFuncs::PointInTriangle2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, const FVector2D& C)
{
	float dX = P.X - C.X;
	float dY = P.Y - C.Y;
	float dX21 = C.X - B.X;
	float dY12 = B.Y - C.Y;
	float D = dY12 * (A.X - C.X) + dX21 * (A.Y - C.Y);
	float s = dY12 * dX + dX21 * dY;
	float t = (C.Y - A.Y) * dX + (A.X - C.X) * dY;
	if (D < 0) return (s <= 0) && (t <= 0) && (s + t >= D);
	return (s >= 0) && (t >= 0) && (s + t <= D);
}

FVector AssetHelperFuncs::InterpolateHeight(const FVector& Point, UProceduralMeshComponent* Mesh)
{
	const TArray<FProcMeshVertex>& Vertices = Mesh->GetProcMeshSection(0)->ProcVertexBuffer;
	const TArray<uint32>& Indices = Mesh->GetProcMeshSection(0)->ProcIndexBuffer;
	FVector InterpolatedPoint = Point;

	for (int32 i = 0; i < Indices.Num(); i += 3)
	{
		const FVector& V0 = Vertices[Indices[i]].Position;
		const FVector& V1 = Vertices[Indices[i + 1]].Position;
		const FVector& V2 = Vertices[Indices[i + 2]].Position;

		FVector2D A(V0.X, V0.Y);
		FVector2D B(V1.X, V1.Y);
		FVector2D C(V2.X, V2.Y);
		FVector2D P(Point.X, Point.Y);

		if (PointInTriangle2D(P, A, B, C))
		{
			// Barycentric interpolation
			FVector2D v0 = B - A;
			FVector2D v1 = C - A;
			FVector2D v2 = P - A;

			float d00 = FVector2D::DotProduct(v0, v0);
			float d01 = FVector2D::DotProduct(v0, v1);
			float d11 = FVector2D::DotProduct(v1, v1);
			float d20 = FVector2D::DotProduct(v2, v0);
			float d21 = FVector2D::DotProduct(v2, v1);

			float denom = d00 * d11 - d01 * d01;
			if (FMath::IsNearlyZero(denom)) continue;

			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0f - v - w;

			InterpolatedPoint.Z = u * V0.Z + v * V1.Z + w * V2.Z;
		}
	}
	return InterpolatedPoint; // Return zero vector if no valid point found
}