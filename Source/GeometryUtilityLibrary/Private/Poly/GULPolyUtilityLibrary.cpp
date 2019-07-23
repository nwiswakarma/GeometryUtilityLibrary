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

bool UGULPolyUtilityLibrary::IsPointInPoly(const FVector2D& Point, const TArray<FVector2D>& Poly)
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
