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

#pragma once

#include "CoreMinimal.h"

class GEOMETRYUTILITYLIBRARY_API FGULPoissonDiscSampler
{
    float PointRadius;
    FBox2D Bounds;
    int32 KValue;

    float RadiusSq;
    float CellSize;
    float CellSizeInv;

    int32 DimX;
    int32 DimY;

    TArray<int32> Grid;

    FORCEINLINE int32 GetIndex(int32 X, int32 Y) const
    {
        return X + Y*DimX;
    }

    FORCEINLINE FIntPoint GetCell(float X, float Y) const
    {
        return FIntPoint(
            FMath::FloorToInt((X-Bounds.Min.X) * CellSizeInv),
            FMath::FloorToInt((Y-Bounds.Min.Y) * CellSizeInv)
            );
    }

    FORCEINLINE FIntPoint GetCell(const FVector2D& Point) const
    {
        return GetCell(Point.X, Point.Y);
    }

    FORCEINLINE void SetSample(const FIntPoint& GridIndex, int32 CellIndex)
    {
        SetSample(GridIndex.X, GridIndex.Y, CellIndex);
    }

    FORCEINLINE void SetSample(int32 X, int32 Y, int32 CellIndex)
    {
        check(X >= 0 && X < DimX && Y >= 0 && Y < DimY);
        Grid[GetIndex(X, Y)] = CellIndex;
    }

    bool IsWithinValidPointRadius(const FVector2D& Point, const TArray<FVector2D>& Points) const;

public:

    FGULPoissonDiscSampler() = default;

    FGULPoissonDiscSampler(FBox2D InBounds, float InRadius = 1.f, int32 InKValue = 25);

    void SetConfig(FBox2D InBounds, float InRadius = 1.f, int32 InKValue = 25);

    FORCEINLINE bool HasValidConfig() const
    {
        return Bounds.bIsValid
            && Bounds.GetArea() > KINDA_SMALL_NUMBER
            && PointRadius > 0.f;
    }

    void GeneratePoints(TArray<FVector2D>& OutPoints, FRandomStream& Rand);

    inline void GeneratePoints(TArray<FVector2D>& OutPoints, int32 RandomSeed)
    {
        FRandomStream Rand(RandomSeed);
        GeneratePoints(OutPoints, Rand);
    }
};
