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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GULTypes.h"
#include "GULGridUtility.generated.h"

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULGridUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    static void PointFill(
        TArray<FIntPoint>& OutPoints,
        const TSet<int32>& BoundaryIndexSet,
        const FIntPoint& BoundsMin,
        const FIntPoint& BoundsMax,
        const FIntPoint& FillTargetPoint,
        TSet<int32>* VisitedSet = nullptr
        );

    inline static void GenerateBoundaryData(
        FIntPoint& BoundsMin,
        FIntPoint& BoundsMax,
        int32& Size,
        const TArray<FIntPoint>& BoundaryPoints
        );

    inline static void GenerateIndexSet(
        TSet<int32>& OutIndexSet,
        const TArray<FIntPoint>& InPoints,
        const FIntPoint& Origin,
        int32 Stride
        );

    FORCEINLINE static int32 GetSquaredSize(const FIntPoint& BoundsMin, const FIntPoint& BoundsMax);

public:

    UFUNCTION(BlueprintCallable)
    static void GridWalk(TArray<FIntPoint>& OutGridIds, const FVector2D& P0, const FVector2D& P1, int32 DimensionX, int32 DimensionY, bool bUniqueOutput = false);

    UFUNCTION(BlueprintCallable)
    static bool GridFillByPoint(TArray<FIntPoint>& OutPoints, const TArray<FIntPoint>& BoundaryPoints, const FIntPoint& FillTargetPoint);

    UFUNCTION(BlueprintCallable)
    static bool GenerateIsolatedPointGroups(TArray<FGULIntPointGroup>& OutPointGroups, const TArray<FIntPoint>& BoundaryPoints);

    UFUNCTION(BlueprintCallable)
    static bool GenerateIsolatedPointGroupsWithinBounds(
        TArray<FGULIntPointGroup>& OutPointGroups,
        const TArray<FIntPoint>& BoundaryPoints,
        FIntPoint BoundsMin,
        FIntPoint BoundsMax
        );

    UFUNCTION(BlueprintCallable)
    static void GenerateIsolatedGridsOnPoly(
        TArray<FIntPoint>& GridIds,
        const TArray<FIntPoint>& InBoundaryPoints,
        const TArray<FGULVector2DGroup>& InPolyGroups,
        int32 GridSize,
        FIntPoint BoundsMin,
        FIntPoint BoundsMax
        );

    FORCEINLINE static bool IsOnBounds(const FIntPoint& Point, const FIntPoint& BoundsMin, const FIntPoint& BoundsMax);
    FORCEINLINE static FIntPoint GetGridId(const FVector2D& Point, int32 DimensionX, int32 DimensionY);
    FORCEINLINE static int32 GetGridIndex(const FIntPoint& Point, const FIntPoint& Origin, int32 Stride);
};

FORCEINLINE FIntPoint UGULGridUtility::GetGridId(const FVector2D& Point, int32 DimensionX, int32 DimensionY)
{
    check(DimensionX > 0 && DimensionY > 0);
    return FIntPoint(
        FMath::FloorToInt(Point.X / DimensionX),
        FMath::FloorToInt(Point.Y / DimensionY)
        );
}

FORCEINLINE int32 UGULGridUtility::GetGridIndex(const FIntPoint& Point, const FIntPoint& Origin, int32 Stride)
{
    return (Point.X-Origin.X) + (Point.Y-Origin.Y)*Stride;
}

FORCEINLINE bool UGULGridUtility::IsOnBounds(const FIntPoint& Point, const FIntPoint& BoundsMin, const FIntPoint& BoundsMax)
{
    return 
        Point.X >= BoundsMin.X && Point.Y >= BoundsMin.Y &&
        Point.X <= BoundsMax.X && Point.Y <= BoundsMax.Y;
}

FORCEINLINE int32 UGULGridUtility::GetSquaredSize(const FIntPoint& BoundsMin, const FIntPoint& BoundsMax)
{
    FIntPoint Extent = BoundsMax-BoundsMin;
    return FMath::Max(Extent.X, Extent.Y)+1;
}

inline void UGULGridUtility::GenerateBoundaryData(
    FIntPoint& BoundsMin,
    FIntPoint& BoundsMax,
    int32& Size,
    const TArray<FIntPoint>& BoundaryPoints
    )
{
    FBox2D Bounds(ForceInitToZero);

    for (const FIntPoint& BoundaryPoint : BoundaryPoints)
    {
        Bounds += FVector2D(BoundaryPoint.X, BoundaryPoint.Y);
    }

    BoundsMin.X = FMath::RoundToInt(Bounds.Min.X);
    BoundsMin.Y = FMath::RoundToInt(Bounds.Min.Y);

    BoundsMax.X = FMath::RoundToInt(Bounds.Max.X);
    BoundsMax.Y = FMath::RoundToInt(Bounds.Max.Y);

    Size = GetSquaredSize(BoundsMin, BoundsMax);
}

inline void UGULGridUtility::GenerateIndexSet(TSet<int32>& OutIndexSet, const TArray<FIntPoint>& InPoints, const FIntPoint& Origin, int32 Stride)
{
    for (const FIntPoint& Point : InPoints)
    {
        OutIndexSet.Emplace(GetGridIndex(Point, Origin, Stride));
    }
}
