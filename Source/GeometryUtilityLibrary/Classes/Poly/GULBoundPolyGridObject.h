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
#include "GULBoundPolyGridObject.generated.h"

UCLASS(BlueprintType)
class GEOMETRYUTILITYLIBRARY_API UGULBoundPolyGridObject : public UObject
{
    GENERATED_BODY()

    struct FPointGroup
    {
        TArray<FVector2D> Points;
    };

    TArray<FPointGroup> Segments;
    TArray<FPointGroup> Polys;

public:

    UPROPERTY(BlueprintReadOnly)
    FIntPoint Origin;

    UPROPERTY(BlueprintReadOnly)
    int32 DimensionX;

    UPROPERTY(BlueprintReadOnly)
    int32 DimensionY;

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE bool IsValidGrid() const;

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE FBox2D GetBounds() const;

    UFUNCTION(BlueprintCallable)
    bool GeneratePolyFromPointIndices(const FIntPoint& InOrigin, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices);

    UFUNCTION(BlueprintCallable)
    void SnapPointsToSegmentPoints(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, float SnapRange = 0.01f);

    inline void GetBoundingPoints(TArray<FVector2D>& OutPoints) const;
    inline void GetBoundingPoly(TArray<FVector2D>& OutPoints) const;

    void UnitGridWalk(TArray<FIntPoint>& GridIds, const FVector2D& P0, const FVector2D& P1);
};

FORCEINLINE_DEBUGGABLE bool UGULBoundPolyGridObject::IsValidGrid() const
{
    return DimensionX > 0 && DimensionY > 0;
}

FORCEINLINE_DEBUGGABLE FBox2D UGULBoundPolyGridObject::GetBounds() const
{
    FVector2D BoundsMin(Origin.X, Origin.Y);
    FVector2D BoundsMax(BoundsMin.X+DimensionX, BoundsMin.Y+DimensionY);
    return FBox2D(BoundsMin, BoundsMax);
}

inline void UGULBoundPolyGridObject::GetBoundingPoints(TArray<FVector2D>& OutPoints) const
{
    FVector2D BoundsMin(Origin.X, Origin.Y);
    FVector2D BoundsMax(BoundsMin.X+DimensionX, BoundsMin.Y+DimensionY);
    OutPoints.Reset(4);
    OutPoints.Emplace(BoundsMin.X, BoundsMin.Y);
    OutPoints.Emplace(BoundsMin.X, BoundsMax.Y);
    OutPoints.Emplace(BoundsMax.X, BoundsMin.Y);
    OutPoints.Emplace(BoundsMax.X, BoundsMax.Y);
}

inline void UGULBoundPolyGridObject::GetBoundingPoly(TArray<FVector2D>& OutPoints) const
{
    FVector2D BoundsMin(Origin.X, Origin.Y);
    FVector2D BoundsMax(BoundsMin.X+DimensionX, BoundsMin.Y+DimensionY);
    OutPoints.Reset(4);
    OutPoints.Emplace(BoundsMin.X, BoundsMin.Y);
    OutPoints.Emplace(BoundsMin.X, BoundsMax.Y);
    OutPoints.Emplace(BoundsMax.X, BoundsMax.Y);
    OutPoints.Emplace(BoundsMax.X, BoundsMin.Y);
}
