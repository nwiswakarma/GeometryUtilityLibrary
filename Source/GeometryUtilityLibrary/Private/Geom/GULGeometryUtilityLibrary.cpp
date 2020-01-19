////////////////////////////////////////////////////////////////////////////////
//
// MIT License
// 
// Copyright (c) 2018-2019 Nuraga Wiswakarma
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////
// 

#include "Geom/GULGeometryUtilityLibrary.h"

void UGULGeometryUtility::TransformBox2DPoints(FGULBox2DPoints& OutPoints, const FTransform& Transform, const FGULBox2DPoints& InPoints)
{
    OutPoints.P0 = FVector2D(Transform.TransformPosition(FVector(InPoints.P0, 0)));
    OutPoints.P1 = FVector2D(Transform.TransformPosition(FVector(InPoints.P1, 0)));
    OutPoints.P2 = FVector2D(Transform.TransformPosition(FVector(InPoints.P2, 0)));
    OutPoints.P3 = FVector2D(Transform.TransformPosition(FVector(InPoints.P3, 0)));
}

void UGULGeometryUtility::TransformBoxVectors(FGULBoxVectors& OutVectors, const FTransform& Transform, const FGULBoxVectors& InVectors)
{
    OutVectors.V0 = Transform.TransformPosition(InVectors.V0);
    OutVectors.V1 = Transform.TransformPosition(InVectors.V1);
    OutVectors.V2 = Transform.TransformPosition(InVectors.V2);
    OutVectors.V3 = Transform.TransformPosition(InVectors.V3);

    OutVectors.V4 = Transform.TransformPosition(InVectors.V4);
    OutVectors.V5 = Transform.TransformPosition(InVectors.V5);
    OutVectors.V6 = Transform.TransformPosition(InVectors.V6);
    OutVectors.V7 = Transform.TransformPosition(InVectors.V7);
}

void UGULGeometryUtility::TransformBox(FGULOrientedBox& OutBox, const FGULOrientedBox& InBox, const FTransform& Transform)
{
    FVector Scale(Transform.GetScale3D());
    FQuat Rotation(Transform.GetRotation());

    OutBox.Center = Transform.TransformPosition(InBox.Center);
    OutBox.Extent = InBox.Extent * Scale;
    OutBox.Rotation = FRotator(InBox.Rotation.Quaternion() * Rotation);
}

void UGULGeometryUtility::AlignBox(TArray<FGULOrientedBox>& OutBoxes, TArray<FVector>& OutDeltas, const TArray<FGULOrientedBox>& InBoxes)
{
    AlignBoxToBottom(OutBoxes, OutDeltas, InBoxes);
}

void UGULGeometryUtility::AlignBoxToBottom(TArray<FGULOrientedBox>& OutBoxes, TArray<FVector>& OutDeltas, const TArray<FGULOrientedBox>& InBoxes)
{
    const int32 BoxCount = InBoxes.Num();

    OutBoxes.SetNumUninitialized(BoxCount);
    OutDeltas.SetNumUninitialized(BoxCount);

    float ZMin = BIG_NUMBER;

    for (const FGULOrientedBox& Box : InBoxes)
    {
        float BoxMinZ(Box.Center.Z-Box.Extent.Z);

        if (BoxMinZ < ZMin)
        {
            ZMin = BoxMinZ;
        }
    }

    for (int32 i=0; i<BoxCount; ++i)
    {
        const FGULOrientedBox& Box(InBoxes[i]);
        float BoxMinZ(Box.Center.Z-Box.Extent.Z);
        float ZDelta(ZMin-BoxMinZ);

        OutBoxes[i] = Box;
        OutBoxes[i].Center.Z += ZDelta;

        OutDeltas[i] = FVector(0, 0, ZDelta);
    }
}

struct FGULSegmentDistToSegment2D_Solver
{
	FGULSegmentDistToSegment2D_Solver(const FVector2D& InA1, const FVector2D& InB1, const FVector2D& InA2, const FVector2D& InB2):
		bLinesAreNearlyParallel(false),
		A1(InA1),
		A2(InA2),
		S1(InB1 - InA1),
		S2(InB2 - InA2),
		S3(InA1 - InA2)
	{
	}

	bool bLinesAreNearlyParallel;

	const FVector2D& A1;
	const FVector2D& A2;

	const FVector2D S1;
	const FVector2D S2;
	const FVector2D S3;

	void Solve(FVector2D& OutP1, FVector2D& OutP2)
	{
		const float Dot11 = S1 | S1;
		const float Dot12 = S1 | S2;
		const float Dot13 = S1 | S3;
		const float Dot22 = S2 | S2;
		const float Dot23 = S2 | S3;

		const float D = Dot11*Dot22 - Dot12*Dot12;

		float D1 = D;
		float D2 = D;

		float N1;
		float N2;

		if (bLinesAreNearlyParallel || D < KINDA_SMALL_NUMBER)
		{
			// the lines are almost parallel
			N1 = 0.f;	// force using point A on segment S1
			D1 = 1.f;	// to prevent possible division by 0 later
			N2 = Dot23;
			D2 = Dot22;
		}
		else
		{
			// get the closest points on the infinite lines
			N1 = (Dot12*Dot23 - Dot22*Dot13);
			N2 = (Dot11*Dot23 - Dot12*Dot13);

			if (N1 < 0.f)
			{
				// t1 < 0.f => the s==0 edge is visible
				N1 = 0.f;
				N2 = Dot23;
				D2 = Dot22;
			}
			else if (N1 > D1)
			{
				// t1 > 1 => the t1==1 edge is visible
				N1 = D1;
				N2 = Dot23 + Dot12;
				D2 = Dot22;
			}
		}

		if (N2 < 0.f)
		{
			// t2 < 0 => the t2==0 edge is visible
			N2 = 0.f;

			// recompute t1 for this edge
			if (-Dot13 < 0.f)
			{
				N1 = 0.f;
			}
			else if (-Dot13 > Dot11)
			{
				N1 = D1;
			}
			else
			{
				N1 = -Dot13;
				D1 = Dot11;
			}
		}
		else if (N2 > D2)
		{
			// t2 > 1 => the t2=1 edge is visible
			N2 = D2;

			// recompute t1 for this edge
			if ((-Dot13 + Dot12) < 0.f)
			{
				N1 = 0.f;
			}
			else if ((-Dot13 + Dot12) > Dot11)
			{
				N1 = D1;
			}
			else
			{
				N1 = (-Dot13 + Dot12);
				D1 = Dot11;
			}
		}

		// finally do the division to get the points' location
		const float T1 = (FMath::Abs(N1) < KINDA_SMALL_NUMBER ? 0.f : N1 / D1);
		const float T2 = (FMath::Abs(N2) < KINDA_SMALL_NUMBER ? 0.f : N2 / D2);

		// return the closest points
		OutP1 = A1 + T1 * S1;
		OutP2 = A2 + T2 * S2;
	}
};

void UGULGeometryUtility::ShortestSegment2DBetweenSegment2DSafe(
    const FVector2D& A1,
    const FVector2D& B1,
    const FVector2D& A2,
    const FVector2D& B2,
    FVector2D& OutP1,
    FVector2D& OutP2
    )
{
	FGULSegmentDistToSegment2D_Solver Solver(A1, B1, A2, B2);

	const FVector2D S1_norm = Solver.S1.GetSafeNormal();
	const FVector2D S2_norm = Solver.S2.GetSafeNormal();

	const bool bS1IsPoint = S1_norm.IsZero();
	const bool bS2IsPoint = S2_norm.IsZero();

	if (bS1IsPoint && bS2IsPoint)
	{
		OutP1 = A1;
		OutP2 = A2;
	}
	else if (bS2IsPoint)
	{
		OutP1 = FMath::ClosestPointOnSegment2D(A2, A1, B1);
		OutP2 = A2;
	}
	else if (bS1IsPoint)
	{
		OutP1 = A1;
		OutP2 = FMath::ClosestPointOnSegment2D(A1, A2, B2);
	}
	else
	{
		const	float	Dot11_norm = S1_norm | S1_norm;	// always >= 0
		const	float	Dot22_norm = S2_norm | S2_norm;	// always >= 0
		const	float	Dot12_norm = S1_norm | S2_norm;
		const	float	D_norm	= Dot11_norm*Dot22_norm - Dot12_norm*Dot12_norm;	// always >= 0

		Solver.bLinesAreNearlyParallel = D_norm < KINDA_SMALL_NUMBER;
		Solver.Solve(OutP1, OutP2);
	}
}

void UGULGeometryUtility::ShortestSegment2DBetweenSegment2D(
    const FVector2D& A1,
    const FVector2D& B1,
    const FVector2D& A2,
    const FVector2D& B2,
    FVector2D& OutP1,
    FVector2D& OutP2
    )
{
	FGULSegmentDistToSegment2D_Solver Solver(A1, B1, A2, B2);
    Solver.Solve(OutP1, OutP2);
}
