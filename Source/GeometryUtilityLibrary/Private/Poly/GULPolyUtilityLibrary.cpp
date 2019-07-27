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

#include "Poly/GULPolyUtilityLibrary.h"

TArray<FVector2D> UGULPolyUtilityLibrary::K2_FitPoints(const TArray<FVector2D>& Points, FVector2D Dimension, float FitScale)
{
    // Invalid dimension, abort
    if (Dimension.IsNearlyZero())
    {
        return Points;
    }

    TArray<FVector2D> OutPoints(Points);

    FitPoints(OutPoints, Dimension, FitScale);

    return OutPoints;
}

TArray<FVector2D> UGULPolyUtilityLibrary::K2_FlipPoints(const TArray<FVector2D>& Points, FVector2D Dimension)
{
    TArray<FVector2D> OutPoints(Points);
    FlipPoints(OutPoints, Dimension);
    return OutPoints;
}

void UGULPolyUtilityLibrary::ReversePoints(const TArray<FVector2D>& InPoints, TArray<FVector2D>& OutPoints)
{
    const int32 PointCount = InPoints.Num();
    OutPoints.Reset(PointCount);
    for (int32 i=PointCount-1; i>=0; --i)
    {
        OutPoints.Emplace(InPoints[i]);
    }
}

void UGULPolyUtilityLibrary::FitPoints(TArray<FVector2D>& Points, const FVector2D& Dimension, float FitScale)
{
    // Generate point bounds

    FBox2D Bounds(ForceInitToZero);

    for (const FVector2D& Point : Points)
    {
        Bounds += Point;
    }

    // Scale points

    const FVector2D& Unit(FVector2D::UnitVector);
    const FVector2D& Size(Dimension);
    const FVector2D  Extents = Size/2.f;

    const FVector2D Offset = Unit-(Unit+Bounds.Min);
    const FVector2D ScaledOffset = (1.f-FitScale) * Extents;
    const float Scale = (Size/Bounds.GetSize()).GetMin() * FitScale;

    for (FVector2D& Point : Points)
    {
        Point = ScaledOffset + (Offset+Point)*Scale;
    }
}

void UGULPolyUtilityLibrary::FitPointsWithinBounds(TArray<FVector2D>& Points, const FBox2D& FitBounds, float FitScale)
{
    // Generate point bounds

    FBox2D Bounds(ForceInitToZero);

    for (const FVector2D& Point : Points)
    {
        Bounds += Point;
    }

    // Scale points

    const FVector2D& Unit(FVector2D::UnitVector);
    const FVector2D& Size(FitBounds.GetSize());
    const FVector2D  Extents = Size/2.f;

    const FVector2D Offset = Unit-(Unit+Bounds.Min);
    const FVector2D ScaledOffset = (1.f-FitScale) * Extents;
    const float Scale = (Size/Bounds.GetSize()).GetMin() * FitScale;

    const FVector2D& BoundsOffset(FitBounds.Min);

    for (FVector2D& Point : Points)
    {
        Point = BoundsOffset + ScaledOffset + (Offset+Point)*Scale;
    }
}

void UGULPolyUtilityLibrary::FlipPoints(TArray<FVector2D>& Points, const FVector2D& Dimension)
{
    for (FVector2D& Point : Points)
    {
        Point = Dimension - Point;
    }
}

bool UGULPolyUtilityLibrary::IsPointOnPoly(const FVector2D& Point, const TArray<FVector2D>& Poly)
{
    TArray<FIntPoint> IntPoly;
    FIntPoint pt = ScaleToIntPoint(Point);

    ScaleToIntPoint(Poly, IntPoly);

    //returns 0 if false, +1 if true, -1 if pt ON polygon boundary
    int32 result = 0;
    int32 cnt = IntPoly.Num();
    if (cnt < 3) return false; //return 0;
    FIntPoint ip = IntPoly[0];
    for(int32 i = 1; i <= cnt; ++i)
    {
        FIntPoint ipNext = (i == cnt ? IntPoly[0] : IntPoly[i]);
        if (ipNext.Y == pt.Y)
        {
            if ((ipNext.X == pt.X) || (ip.Y == pt.Y && 
              ((ipNext.X > pt.X) == (ip.X < pt.X)))) return true; //return -1;
        }
        if ((ip.Y < pt.Y) != (ipNext.Y < pt.Y))
        {
            if (ip.X >= pt.X)
            {
                if (ipNext.X > pt.X) result = 1 - result;
                else
                {
                    float d = (float)(ip.X - pt.X) * (ipNext.Y - pt.Y) - 
                      (float)(ipNext.X - pt.X) * (ip.Y - pt.Y);
                    if (!d) return true; //return -1;
                    if ((d > 0) == (ipNext.Y > ip.Y)) result = 1 - result;
                }
            }
            else
            {
                if (ipNext.X > pt.X)
                {
                    float d = (float)(ip.X - pt.X) * (ipNext.Y - pt.Y) - 
                      (float)(ipNext.X - pt.X) * (ip.Y - pt.Y);
                    if (!d) return true; //return -1;
                    if ((d > 0) == (ipNext.Y > ip.Y)) result = 1 - result;
                }
            }
        }
        ip = ipNext;
    }

    return (result != 0);
}

void UGULPolyUtilityLibrary::FindPointsByAngle(TArray<FGULPointAngleOutput>& OutPoints, const TArray<FVector2D>& Points, float AngleThreshold)
{
    const int32 PointCount = Points.Num();

    if (Points.Num() < 3)
    {
        return;
    }

    OutPoints.Reset(PointCount);

    for (int32 i=1; i<PointCount; ++i)
    {
        int32 i0 = i-1;
        int32 i1 = i%PointCount;
        int32 i2 = (i+1)%PointCount;

        const FVector2D& P0(Points[i0]);
        const FVector2D& P1(Points[i1]);
        const FVector2D& P2(Points[i2]);

        FVector2D N01(P1-P0);
        FVector2D N12(P2-P1);

        N01.Normalize();
        N12.Normalize();

        const float Dot = N01 | N12;

        if (Dot < AngleThreshold)
        {
            FGULPointAngleOutput Output;
            Output.Index0 = i0;
            Output.Index1 = i1;
            Output.Index2 = i2;
            Output.Angle = Dot;
            Output.bOrientation = (FVector2D(-N01.Y, N01.X) | N12) >= 0.f;
            OutPoints.Emplace(Output);
            //UE_LOG(LogTemp,Warning, TEXT("%s"), *Output.ToString());
        }
    }

    OutPoints.Shrink();

    //UE_LOG(LogTemp,Warning, TEXT("OutPoints.Num(): %d"), OutPoints.Num());
}

bool UGULPolyUtilityLibrary::GetPointAngleVectors(FVector2D& Point0, FVector2D& Point1, FVector2D& Point2, const FGULPointAngleOutput& PointAngle, const TArray<FVector2D>& Points, bool bMidPointExtents)
{
    if (Points.IsValidIndex(PointAngle.Index0) &&
        Points.IsValidIndex(PointAngle.Index1) &&
        Points.IsValidIndex(PointAngle.Index2))
    {
        Point0 = Points[PointAngle.Index0];
        Point1 = Points[PointAngle.Index1];
        Point2 = Points[PointAngle.Index2];

        if (bMidPointExtents)
        {
            Point0 = Point0+(Point1-Point0)*.5f;
            Point2 = Point1+(Point2-Point1)*.5f;
        }

        return true;
    }

    return false;
}
