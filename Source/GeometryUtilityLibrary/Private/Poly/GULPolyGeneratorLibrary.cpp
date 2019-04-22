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

#include "Poly/GULPolyGeneratorLibrary.h"

void UGULPolyGeneratorLibrary::GeneratePolyDisplacementInternal(TArray<FVector2D>& OutPoints, FPolyList& PolyList, FBox2D& Bounds, const FGULPolyDisplacementParams& Params, FRandomStream& Rand)
{
    check(Params.IsValid());

    const int32 SubdivCount = Params.SubdivCount;
    const float SubdivLimit = Params.SubdivLimit;
    const float PolyScale = Params.Scale;

    const FVector2D& Size(Params.Size);
    const FVector2D  Extents = Size/2.f;

    // Subdivides poly edges
    for (int32 i=0; i<SubdivCount; ++i)
    {
        int32 SubdivPerformed = 0;
        FPolyNode* h = PolyList.GetHead();
        FPolyNode* n = h->GetNextNode();

        while (n)
        {
            const FRoughV2D& r0(n->GetPrevNode()->GetValue());
            const FRoughV2D& r1(n->GetValue());
            const FVector2D& v0(r0.Pos);
            const FVector2D& v1(r1.Pos);

            if (FVector2D::DistSquared(v0, v1) > SubdivLimit)
            {
                float mx = (v0.X+v1.X)*.5f;
                float my = (v0.Y+v1.Y)*.5f;
                float vx = -(v0.Y-v1.Y);
                float vy = v0.X-v1.X;
                float b = r1.Balance;
                float o = r1.MaxOffset;
                float d = (Rand.GetFraction()-b)*o;
                FRoughV2D pt(mx+d*vx, my+d*vy, b, o);
                PolyList.InsertNode(pt, n);
                Bounds += pt.Pos;
                ++SubdivPerformed;
            }

            n = n->GetNextNode();
        }

        if (SubdivPerformed < 1)
        {
            break;
        }
    }

    // Fit points to bounds & write output vertices
    {
        const FVector2D& Unit(FVector2D::UnitVector);
        FVector2D Offset = Unit-(Unit+Bounds.Min);
        FVector2D ScaledOffset = Extents * (1.f-PolyScale);
        float Scale = (Size/Bounds.GetSize()).GetMin() * PolyScale;

        OutPoints.Reset(PolyList.Num());

        for (const FRoughV2D& v : PolyList)
        {
            OutPoints.Emplace(ScaledOffset + (Offset+v.Pos)*Scale);
        }
    }
}

void UGULPolyGeneratorLibrary::GeneratePoly(TArray<FVector2D>& OutPoints, const FGULPolyGenerationParams& Params)
{
    // Invalid parameters, abort
    if (! Params.IsValid())
    {
        return;
    }

    typedef FPlatformMath FPM;

    FRandomStream Rand(Params.RandomSeed);

    const FVector2D& Size(Params.Size);
    const FVector2D  Extents = Size/2.f;

    const int32 SideCount = Params.Sides;
    const float Scale = Params.Scale;
    const float RandomScale = Params.RandomScale;
    const float PI2 = 2.f*PI;
    const float SideAngle = PI2/SideCount;

    float RandomSpread = Params.RandomSpread;
    float Angle = (Params.RandomAngle+(Params.Angle/360.f))*PI2;
    Angle += Rand.GetFraction()*Params.RandomAngle*PI2;

    // Generates random points
    for (int32 i=0; i<SideCount; ++i, Angle+=SideAngle)
    {
        float Spread = Rand.FRandRange(-.5f, .5f)*RandomSpread*SideAngle;
        float PointAngle = Angle + Spread;
        FVector2D pt(FPM::Cos(PointAngle), FPM::Sin(PointAngle));
        pt *= Scale-FMath::Max(0.f, (Rand.GetFraction()*RandomScale));
        pt  = pt*Extents + Extents;
        OutPoints.Emplace(pt);
    }
}

void UGULPolyGeneratorLibrary::GeneratePolyDisplacement(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InitialPoints, const FGULPolyDisplacementParams& Params)
{
    // Invalid Parameters, abort
    if (! Params.IsValid())
    {
        return;
    }

    typedef FPlatformMath FPM;

    FRandomStream Rand(Params.RandomSeed);
    const float DisplacementMin = Params.DisplacementRange.X;
    const float DisplacementMax = Params.DisplacementRange.Y;

    FPolyList PolyList;
    FBox2D Bounds(ForceInitToZero);

    // Generate poly list from initial points
    for (const FVector2D& Point : InitialPoints)
    {
        FRoughV2D pt(Point.X, Point.Y, Rand, DisplacementMin, DisplacementMax);
        Bounds += pt.Pos;
        PolyList.AddTail(pt);
    }
    PolyList.AddTail(PolyList.GetHead()->GetValue());

    // Generate poly displacement
    GeneratePolyDisplacementInternal(OutPoints, PolyList, Bounds, Params, Rand);
}

TArray<FVector2D> UGULPolyGeneratorLibrary::K2_GeneratePointOffsets(int32 Seed, const TArray<FVector2D>& Points, float PointRadius)
{
    FRandomStream Rand(Seed);
    FBox2D Bounds(ForceInitToZero);
    TArray<FVector2D> OutPoints(Points);

    for (FVector2D& Point : OutPoints)
    {
		FVector2D UnitRand;
		float L;

		do
		{
			// Check random vectors in the unit sphere so result is statistically uniform.
			UnitRand.X = Rand.GetFraction() * 2.f - 1.f;
			UnitRand.Y = Rand.GetFraction() * 2.f - 1.f;
			L = UnitRand.SizeSquared();
		}
		while (L > 1.f || L < KINDA_SMALL_NUMBER);

		Point += UnitRand.GetSafeNormal() * PointRadius;
    }

    return OutPoints;
}
