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

float UGULPolyUtilityLibrary::GetArea(const TArray<FVector2D>& Points)
{
    int32 PointCount = Points.Num();
    
    if (PointCount < 3)
    {
        return 0.f;
    }
    
    float a = 0;
    
    for (int32 i=0, j=PointCount-1; i<PointCount; ++i)
    {
        a += (Points[j].X + Points[i].X) * (Points[j].Y - Points[i].Y);
        j = i;
    }
    
    return -a * 0.5;
}

bool UGULPolyUtilityLibrary::GetOrientation(const TArray<FVector2D>& Points)
{
    return GetArea(Points) >= 0.f;
}

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

void UGULPolyUtilityLibrary::FlipPoints(TArray<FVector2D>& Points, const FVector2D& Dimension)
{
    for (FVector2D& Point : Points)
    {
        Point = Dimension - Point;
    }
}
