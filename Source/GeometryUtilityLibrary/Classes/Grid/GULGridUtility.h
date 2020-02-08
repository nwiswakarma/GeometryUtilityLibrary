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
#include "Geom/GULGeometryUtilityLibrary.h"
#include "Poly/GULPolyTypes.h"
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

    static void PointFillMulti(
        TArray<FIntPoint>& OutPoints,
        const FIntPoint& BoundsMin,
        const FIntPoint& BoundsMax,
        const TArray<FIntPoint>& TargetPoints,
        TFunction<bool(int32, FIntPoint)> VisitCallback = nullptr
        );

    static void VisitPoints(
        const FIntPoint& BoundsMin,
        const FIntPoint& BoundsMax,
        const TArray<FIntPoint>& TargetPoints,
        TFunction<bool(int32, FIntPoint, int32)> VisitCallback = nullptr
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

    FORCEINLINE static int32 GetBoundsStride(const FIntPoint& BoundsMin, const FIntPoint& BoundsMax);

public:

    template<typename FGridContainer>
    static void GridWalk(
        FGridContainer& OutGridIds,
        const FVector2D& P0,
        const FVector2D& P1,
        int32 DimensionX,
        int32 DimensionY
        )
    {
        if (DimensionX < 1 || DimensionY < 1)
        {
            return;
        }

        const float DX = P1.X-P0.X;
        const float DY = P1.Y-P0.Y;
        const int32 SgnX = (DX>0.f) ? 1 : -1;
        const int32 SgnY = (DY>0.f) ? 1 : -1;

        FIntPoint ID0(GetGridId(P0, DimensionX, DimensionY));
        FIntPoint ID1(GetGridId(P1, DimensionX, DimensionY));
        FVector2D DimF(DimensionX, DimensionY);

        while (ID0 != ID1)
        {
            FBox2D Bounds(ForceInitToZero);
            Bounds += FVector2D(ID0.X, ID0.Y) * DimF;
            Bounds += Bounds.Min + DimF;

            // Find X-Axis intersection

            FVector2D SegX0;
            FVector2D SegX1;

            if (SgnX > 0)
            {
                SegX0 = FVector2D(Bounds.Max.X, Bounds.Min.Y);
                SegX1 = FVector2D(Bounds.Max.X, Bounds.Max.Y);
            }
            else
            {
                SegX0 = FVector2D(Bounds.Min.X, Bounds.Min.Y);
                SegX1 = FVector2D(Bounds.Min.X, Bounds.Max.Y);
            }

            bool bIntersectX = UGULGeometryUtility::SegmentIntersection2D(
                P0,
                P1,
                SegX0,
                SegX1
                );

            if (bIntersectX)
            {
                ID0.X += SgnX;
                OutGridIds.Emplace(ID0);
                continue;
            }

            // Find Y-Axis intersection

            FVector2D SegY0;
            FVector2D SegY1;

            if (SgnY > 0)
            {
                SegY0 = FVector2D(Bounds.Min.X, Bounds.Max.Y);
                SegY1 = FVector2D(Bounds.Max.X, Bounds.Max.Y);
            }
            else
            {
                SegY0 = FVector2D(Bounds.Min.X, Bounds.Min.Y);
                SegY1 = FVector2D(Bounds.Max.X, Bounds.Min.Y);
            }

            bool bIntersectY = UGULGeometryUtility::SegmentIntersection2D(
                P0,
                P1,
                SegY0,
                SegY1
                );

            if (bIntersectY)
            {
                ID0.Y += SgnY;
                OutGridIds.Emplace(ID0);
                continue;
            }

            // Fallback check if segment intersection checks
            // missed the correct final voxel
            if ((ID0+FIntPoint(-SgnX, 0    )) == ID1 ||
                (ID0+FIntPoint(    0, -SgnY)) == ID1)
            {
                ID0 = ID1;
                OutGridIds.Emplace(ID0);
                continue;
            }

            // Check no entry
            check(false);
        }
    }

    static bool GridFillBoundsByPoints(
        TArray<FIntPoint>& OutPoints,
        const TArray<FIntPoint>& InTargetPoints,
        FIntPoint BoundsMin,
        FIntPoint BoundsMax,
        TFunction<bool(int32, FIntPoint)> FilterCallback = nullptr
        );

    static void VisitPointsByPredicate(
        const TArray<FIntPoint>& InTargetPoints,
        FIntPoint BoundsMin,
        FIntPoint BoundsMax,
        TFunction<bool(int32, FIntPoint, int32)> VisitCallback
        );

    static bool GenerateGridAndEdgeGroupsIntersections(
        TMap<int32, FGULVector2DGroup>& IntersectionMap,
        TArray<FVector2D>& IntersectEdges,
        const TArray<FGULVector2DGroup>& InEdgeGroups,
        const FIntPoint& GridBoundsMin,
        const FIntPoint& GridBoundsMax,
        const FIntPoint& GridId,
        int32 GridIndex,
        float IntersectRadius = KINDA_SMALL_NUMBER
        );

    static void GenerateGridsFromPolyGroups(
        TArray<FIntPoint>& OutGridIds,
        const TArray<FGULVector2DGroup>& InPolys,
        int32 InGridSizeX,
        int32 InGridSizeY,
        bool bClosedPolygons = true,
        int32 GridSizePerSegment = 10
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Grid Walk"))
    static void K2_GridWalk(
        TArray<FIntPoint>& OutGridIds,
        const FVector2D& P0,
        const FVector2D& P1,
        int32 DimensionX,
        int32 DimensionY,
        bool bUniqueOutput = false
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Grids From Poly Groups"))
    static void K2_GenerateGridsFromPolyGroups(
        TArray<FIntPoint>& OutGridIds,
        const TArray<FGULVector2DGroup>& InPolys,
        int32 InDimensionX,
        int32 InDimensionY,
        bool bClosedPolygons = true
        );

    UFUNCTION(BlueprintCallable)
    static int32 GroupGridsByDimension(
        TArray<FIntPoint>& OutGroupIds,
        TArray<FGULIntPointGroup>& OutGridIdGroups,
        const TArray<FIntPoint>& InGridIds,
        int32 GroupDimensionX,
        int32 GroupDimensionY
        );

    UFUNCTION(BlueprintCallable)
    static int32 GroupGridsByDimensionAndBounds(
        TArray<FIntPoint>& OutGroupIds,
        TArray<FGULIntPointGroup>& OutGridIdGroups,
        const TArray<FIntPoint>& InGridIds,
        FIntPoint GroupBoundsMin,
        FIntPoint GroupBoundsMax,
        int32 GroupDimensionX,
        int32 GroupDimensionY
        );

    UFUNCTION(BlueprintCallable)
    static bool GridFillByPoint(
        TArray<FIntPoint>& OutPoints,
        const TArray<FIntPoint>& BoundaryPoints,
        const FIntPoint& FillTargetPoint
        );

    UFUNCTION(BlueprintCallable)
    static bool GenerateIsolatedPointGroups(
        TArray<FGULIntPointGroup>& OutPointGroups,
        const TArray<FIntPoint>& BoundaryPoints
        );

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

    static void GenerateIsolatedGridsOnPoly(
        TArray<FIntPoint>& GridIds,
        const TArray<FIntPoint>& InBoundaryPoints,
        const TArray<FGULIndexedPolyGroup>& InIndexGroups,
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

FORCEINLINE int32 UGULGridUtility::GetBoundsStride(const FIntPoint& BoundsMin, const FIntPoint& BoundsMax)
{
    return (BoundsMax.X-BoundsMin.X)+1;
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

    Size = GetBoundsStride(BoundsMin, BoundsMax);
}

inline void UGULGridUtility::GenerateIndexSet(TSet<int32>& OutIndexSet, const TArray<FIntPoint>& InPoints, const FIntPoint& Origin, int32 Stride)
{
    for (const FIntPoint& Point : InPoints)
    {
        OutIndexSet.Emplace(GetGridIndex(Point, Origin, Stride));
    }
}

FORCEINLINE_DEBUGGABLE void UGULGridUtility::K2_GridWalk(
    TArray<FIntPoint>& OutGridIds,
    const FVector2D& P0,
    const FVector2D& P1,
    int32 DimensionX,
    int32 DimensionY,
    bool bUniqueOutput
    )
{
    if (bUniqueOutput)
    {
        TArray<FIntPoint> GridIds;
        GridWalk(
            GridIds,
            P0,
            P1,
            DimensionX,
            DimensionY
            );
        OutGridIds = TSet<FIntPoint>(GridIds).Array();
    }
    else
    {
        OutGridIds.Reset();
        GridWalk(
            OutGridIds,
            P0,
            P1,
            DimensionX,
            DimensionY
            );
        OutGridIds.Shrink();
    }
}

FORCEINLINE_DEBUGGABLE void UGULGridUtility::K2_GenerateGridsFromPolyGroups(
    TArray<FIntPoint>& OutGridIds,
    const TArray<FGULVector2DGroup>& InPolys,
    int32 InDimensionX,
    int32 InDimensionY,
    bool bClosedPolygons
    )
{
    GenerateGridsFromPolyGroups(
        OutGridIds,
        InPolys,
        InDimensionX,
        InDimensionY,
        bClosedPolygons
        );
}
