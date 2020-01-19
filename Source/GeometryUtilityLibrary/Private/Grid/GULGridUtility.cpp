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

#include "Grid/GULGridUtility.h"
#include "Geom/GULGeometryUtilityLibrary.h"
#include "Poly/GULPolyUtilityLibrary.h"
#include "GeometryUtilityLibrary.h"

void UGULGridUtility::GridWalk(
    TArray<FIntPoint>& OutGridIds,
    const FVector2D& P0,
    const FVector2D& P1,
    int32 DimensionX,
    int32 DimensionY,
    bool bUniqueOutput,
    FGridWalkVisitCallback VisitCallback
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

    //UE_LOG(LogTemp,Warning, TEXT("ID0: %s"), *ID0.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("ID1: %s"), *ID1.ToString());

    //UE_LOG(LogTemp,Warning, TEXT("P0: %s"), *P0.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("P1: %s"), *P1.ToString());

    //UE_LOG(LogTemp,Warning, TEXT("SGN X: %d"), SgnX);
    //UE_LOG(LogTemp,Warning, TEXT("SGN Y: %d"), SgnY);

    while (ID0 != ID1)
    {
        FBox2D Bounds(ForceInitToZero);
        Bounds += FVector2D(ID0.X, ID0.Y) * DimF;
        Bounds += Bounds.Min + DimF;

        //UE_LOG(LogTemp,Warning, TEXT("ID: %s"), *ID0.ToString());
        //UE_LOG(LogTemp,Warning, TEXT("Bounds: %s"), *Bounds.ToString());

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

        //UE_LOG(LogTemp,Warning, TEXT("bIntersectX: %d"), bIntersectX);
        //UE_LOG(LogTemp,Warning, TEXT("SegX0: %s"), *SegX0.ToString());
        //UE_LOG(LogTemp,Warning, TEXT("SegX1: %s"), *SegX1.ToString());

        if (bIntersectX)
        {
            ID0.X += SgnX;

            if (bUniqueOutput)
            {
                OutGridIds.AddUnique(ID0);
            }
            else
            {
                OutGridIds.Emplace(ID0);
            }

            if (VisitCallback)
            {
                VisitCallback(ID0, P0, P1);
            }

            //UE_LOG(LogTemp,Warning, TEXT("INTERSECTX"));
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

        //UE_LOG(LogTemp,Warning, TEXT("bIntersectY: %d"), bIntersectY);
        //UE_LOG(LogTemp,Warning, TEXT("SegY0: %s"), *SegY0.ToString());
        //UE_LOG(LogTemp,Warning, TEXT("SegY1: %s"), *SegY1.ToString());

        if (bIntersectY)
        {
            ID0.Y += SgnY;

            if (bUniqueOutput)
            {
                OutGridIds.AddUnique(ID0);
            }
            else
            {
                OutGridIds.Emplace(ID0);
            }

            if (VisitCallback)
            {
                VisitCallback(ID0, P0, P1);
            }

            //UE_LOG(LogTemp,Warning, TEXT("INTERSECTY"));
            continue;
        }

        // Check no entry
        check(false);
    }
}

void UGULGridUtility::GenerateGridsFromPolyGroups(
    TArray<FIntPoint>& OutGridIds,
    const TArray<FGULVector2DGroup>& InPolys,
    int32 InDimensionX,
    int32 InDimensionY,
    bool bClosedPolygons,
    FGridWalkVisitCallback VisitCallback
    )
{
    if (InDimensionX < 1 || InDimensionY < 1)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GenerateGridsFromPolyGroups() ABORTED, INVALID DIMENSION"));
        return;
    }

    int32 PolyCount = InPolys.Num();

    if (PolyCount < 1)
    {
        return;
    }

    int32 TotalPointCount = 0;

    for (const FGULVector2DGroup& Poly : InPolys)
    {
        TotalPointCount += Poly.Points.Num();
    }

    OutGridIds.Reset(TotalPointCount);

    for (int32 PolyIt=0; PolyIt<InPolys.Num(); ++PolyIt)
    {
        const TArray<FVector2D>& InPoints(InPolys[PolyIt].Points);
        const int32 PointCount = InPoints.Num();

        // Generate grid data from line segments
        for (int32 i0=0, i1=1; i1<PointCount; ++i1)
        {
            const FVector2D& P0(InPoints[i0]);
            const FVector2D& P1(InPoints[i1]);

            FIntPoint ID0(GetGridId(P0, InDimensionX, InDimensionY));
            FIntPoint ID1(GetGridId(P1, InDimensionX, InDimensionY));

            OutGridIds.AddUnique(ID0);

            if (VisitCallback)
            {
                VisitCallback(ID0, P0, P1);
            }

            if (ID0 != ID1)
            {
                GridWalk(
                    OutGridIds,
                    P0,
                    P1,
                    InDimensionX,
                    InDimensionY,
                    true,
                    VisitCallback
                    );
            }

            i0 = i1;
        }

        // Generate grid data from last line segment (last to first points)
        if (bClosedPolygons && PointCount > 2)
        {
            int32 i0 = PointCount-1;
            int32 i1 = 0;

            const FVector2D& P0(InPoints[i0]);
            const FVector2D& P1(InPoints[i1]);

            FIntPoint ID0(GetGridId(P0, InDimensionX, InDimensionY));
            FIntPoint ID1(GetGridId(P1, InDimensionX, InDimensionY));

            OutGridIds.AddUnique(ID0);

            if (VisitCallback)
            {
                VisitCallback(ID0, P0, P1);
            }

            if (ID0 != ID1)
            {
                GridWalk(
                    OutGridIds,
                    P0,
                    P1,
                    InDimensionX,
                    InDimensionY,
                    true,
                    VisitCallback
                    );
            }
        }
    }

    OutGridIds.Shrink();
}

int32 UGULGridUtility::GroupGridsByDimension(
    TArray<FIntPoint>& OutGroupIds,
    TArray<FGULIntPointGroup>& OutGridIdGroups,
    const TArray<FIntPoint>& InGridIds,
    int32 GroupDimensionX,
    int32 GroupDimensionY
    )
{
    if (GroupDimensionX < 1 || GroupDimensionY < 1)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GroupGridsByDimension() ABORTED, INVALID GROUP DIMENSION"));
        return 0;
    }

    TMap< FIntPoint, TArray<FIntPoint> > GroupMap;

    for (const FIntPoint& GridId : InGridIds)
    {
        FIntPoint GroupId;
        GroupId.X = FMath::FloorToInt(static_cast<float>(GridId.X) / GroupDimensionX);
        GroupId.Y = FMath::FloorToInt(static_cast<float>(GridId.Y) / GroupDimensionY);

        TArray<FIntPoint>& GridIds(GroupMap.FindOrAdd(GroupId));
        GridIds.Emplace(GridId);
    }

    const int32 GroupCount = GroupMap.Num();

    OutGroupIds.Reserve(GroupCount);
    OutGridIdGroups.Reserve(GroupCount);

    for (auto& DataPair : GroupMap)
    {
        FIntPoint& GroupId(DataPair.Get<0>());
        TArray<FIntPoint>& GridIds(DataPair.Get<1>());

        OutGroupIds.Emplace(GroupId);

        OutGridIdGroups.AddDefaulted();
        OutGridIdGroups.Last().Points = MoveTemp(GridIds);
    }

    return GroupCount;
}

int32 UGULGridUtility::GroupGridsByDimensionAndBounds(
    TArray<FIntPoint>& OutGroupIds,
    TArray<FGULIntPointGroup>& OutGridIdGroups,
    const TArray<FIntPoint>& InGridIds,
    FIntPoint GroupBoundsMin,
    FIntPoint GroupBoundsMax,
    int32 GroupDimensionX,
    int32 GroupDimensionY
    )
{
    if (GroupDimensionX < 1 || GroupDimensionY < 1)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GroupGridsByDimensionAndBounds() ABORTED, INVALID GROUP DIMENSION"));
        return 0;
    }
    else
    if (GroupBoundsMin == GroupBoundsMax)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GroupGridsByDimensionAndBounds() ABORTED, INVALID GROUP BOUNDS"));
        return 0;
    }

    TMap< FIntPoint, TArray<FIntPoint> > GroupMap;

    for (const FIntPoint& GridId : InGridIds)
    {
        FIntPoint GroupId;
        GroupId.X = FMath::FloorToInt(static_cast<float>(GridId.X) / GroupDimensionX);
        GroupId.Y = FMath::FloorToInt(static_cast<float>(GridId.Y) / GroupDimensionY);

        if (IsOnBounds(GroupId, GroupBoundsMin, GroupBoundsMax))
        {
            TArray<FIntPoint>& GridIds(GroupMap.FindOrAdd(GroupId));
            GridIds.Emplace(GridId);
        }
    }

    const int32 GroupCount = GroupMap.Num();

    OutGroupIds.Reserve(GroupCount);
    OutGridIdGroups.Reserve(GroupCount);

    for (auto& DataPair : GroupMap)
    {
        FIntPoint& GroupId(DataPair.Get<0>());
        TArray<FIntPoint>& GridIds(DataPair.Get<1>());

        OutGroupIds.Emplace(GroupId);

        OutGridIdGroups.AddDefaulted();
        OutGridIdGroups.Last().Points = MoveTemp(GridIds);
    }

    return GroupCount;
}

void UGULGridUtility::PointFill(
    TArray<FIntPoint>& OutPoints,
    const TSet<int32>& BoundaryIndexSet,
    const FIntPoint& BoundsMin,
    const FIntPoint& BoundsMax,
    const FIntPoint& FillTargetPoint,
    TSet<int32>* VisitedSet
    )
{
    const int32 Size = GetSquaredSize(BoundsMin, BoundsMax);
    const int32 FillTargetIndex = GetGridIndex(FillTargetPoint, BoundsMin, Size);

    check(Size > 0);

    OutPoints.Reset(FMath::Max(1, (Size*Size) / 4));
    OutPoints.Emplace(FillTargetPoint);

    TQueue<FIntPoint> VisitQueue;
    TSet<int32> LocalVisitedSet;

    if (! VisitedSet)
    {
        VisitedSet = &LocalVisitedSet;
    }

    // Visit starting index
    VisitQueue.Enqueue(FillTargetPoint);
    VisitedSet->Emplace(FillTargetIndex);

    const FIntPoint Offsets[4] = {
        FIntPoint(-1,  0), // W
        FIntPoint( 0, -1), // S
        FIntPoint( 1,  0), // E
        FIntPoint( 0,  1)  // N
        };

    // Visit cells in queue
    while (! VisitQueue.IsEmpty())
    {
        FIntPoint VisitPoint;
        VisitQueue.Dequeue(VisitPoint);

        for (int32 i=0; i<4; ++i)
        {
            FIntPoint NeighbourPoint(VisitPoint+Offsets[i]);
            int32 NeighbourIndex = GetGridIndex(NeighbourPoint, BoundsMin, Size);

            if (! IsOnBounds(NeighbourPoint, BoundsMin, BoundsMax) ||
                BoundaryIndexSet.Contains(NeighbourIndex) ||
                VisitedSet->Contains(NeighbourIndex))
            {
                continue;
            }

            // Add point to the visited set
            VisitedSet->Emplace(NeighbourIndex);
            VisitQueue.Enqueue(NeighbourPoint);

            OutPoints.Emplace(NeighbourPoint);
        }
    }

    OutPoints.Shrink();
}

void UGULGridUtility::PointFillMulti(
    TArray<FIntPoint>& OutPoints,
    const FIntPoint& BoundsMin,
    const FIntPoint& BoundsMax,
    const TArray<FIntPoint>& TargetPoints,
    TFunction<bool(int32, FIntPoint)> VisitCallback
    )
{
    const int32 Size = GetSquaredSize(BoundsMin, BoundsMax);

    check(Size > 0);

    TQueue<FIntPoint> VisitQueue;
    TSet<int32> VisitedSet;

    // Visit target points
    for (const FIntPoint& TargetPoint : TargetPoints)
    {
        VisitQueue.Enqueue(TargetPoint);
        VisitedSet.Emplace(GetGridIndex(TargetPoint, BoundsMin, Size));
    }

    const FIntPoint Offsets[4] = {
        FIntPoint(-1,  0), // W
        FIntPoint( 0, -1), // S
        FIntPoint( 1,  0), // E
        FIntPoint( 0,  1)  // N
        };

    // Visit target point neighbours
    while (! VisitQueue.IsEmpty())
    {
        FIntPoint VisitPoint;
        VisitQueue.Dequeue(VisitPoint);

        for (int32 i=0; i<4; ++i)
        {
            FIntPoint NeighbourPoint(VisitPoint+Offsets[i]);
            int32 NeighbourIndex = GetGridIndex(NeighbourPoint, BoundsMin, Size);

            // Skip visited or out-of-bounds points
            if (! IsOnBounds(NeighbourPoint, BoundsMin, BoundsMax) ||
                VisitedSet.Contains(NeighbourIndex))
            {
                continue;
            }

            if (! VisitCallback || VisitCallback(NeighbourIndex, NeighbourPoint))
            {
                // Add point to the visited set
                VisitedSet.Emplace(NeighbourIndex);
                VisitQueue.Enqueue(NeighbourPoint);

                OutPoints.Emplace(NeighbourPoint);
            }
        }
    }
}

bool UGULGridUtility::GridFillByPoint(TArray<FIntPoint>& OutPoints, const TArray<FIntPoint>& BoundaryPoints, const FIntPoint& FillTargetPoint)
{
    if (BoundaryPoints.Num() < 1)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GridFillByPoint() ABORTED, EMPTY BOUNDARY POINTS"));
        return false;
    }

    FIntPoint BoundsMin;
    FIntPoint BoundsMax;
    int32 Size;

    GenerateBoundaryData(BoundsMin, BoundsMax, Size, BoundaryPoints);

    //UE_LOG(LogTemp,Warning, TEXT("BoundsMin: %s"), *BoundsMin.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("BoundsMax: %s"), *BoundsMax.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("Size: %d"), Size);

    if (! IsOnBounds(FillTargetPoint, BoundsMin, BoundsMax))
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GridFillByPoint() ABORTED, OUT-OF-BOUND FILL TARGET POINT"));
        return false;
    }

    TSet<int32> BoundaryIndexSet;
    const int32 FillTargetIndex = GetGridIndex(FillTargetPoint, BoundsMin, Size);

    //UE_LOG(LogTemp,Warning, TEXT("FillTargetPoint: %s %d"), *FillTargetPoint.ToString(), FillTargetIndex);

    for (const FIntPoint& BoundaryPoint : BoundaryPoints)
    {
        int32 GridIndex = GetGridIndex(BoundaryPoint, BoundsMin, Size);
        BoundaryIndexSet.Emplace(GridIndex);

        //UE_LOG(LogTemp,Warning, TEXT("BoundaryPoint: %s %d"), *BoundaryPoint.ToString(), GridIndex);
    }

    if (BoundaryIndexSet.Contains(FillTargetIndex))
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GridFillByPoint() ABORTED, FILL TARGET POINT IS A BOUNDARY POINT"));
        return false;
    }

    PointFill(OutPoints, BoundaryIndexSet, BoundsMin, BoundsMax, FillTargetPoint);
    
    return true;
}

bool UGULGridUtility::GridFillBoundsByPoints(
    TArray<FIntPoint>& OutPoints,
    const TArray<FIntPoint>& InTargetPoints,
    FIntPoint BoundsMin,
    FIntPoint BoundsMax,
    TFunction<bool(int32, FIntPoint)> FilterCallback
    )
{
    FBox2D Bounds(ForceInitToZero);
    Bounds += FVector2D(BoundsMin.X, BoundsMin.Y);
    Bounds += FVector2D(BoundsMax.X, BoundsMax.Y);

    BoundsMin.X = FMath::RoundToInt(Bounds.Min.X);
    BoundsMin.Y = FMath::RoundToInt(Bounds.Min.Y);

    BoundsMax.X = FMath::RoundToInt(Bounds.Max.X);
    BoundsMax.Y = FMath::RoundToInt(Bounds.Max.Y);

    TArray<FIntPoint> TargetPoints = InTargetPoints.FilterByPredicate(
    [BoundsMin, BoundsMax](const FIntPoint& TargetPoint)
    {
        return IsOnBounds(TargetPoint, BoundsMin, BoundsMax);
    } );

    if (TargetPoints.Num() < 1)
    {
        return false;
    }

    PointFillMulti(
        OutPoints,
        BoundsMin,
        BoundsMax,
        TargetPoints,
        FilterCallback
        );
    
    return true;
}

bool UGULGridUtility::GenerateIsolatedPointGroups(TArray<FGULIntPointGroup>& OutPointGroups, const TArray<FIntPoint>& BoundaryPoints)
{
    if (BoundaryPoints.Num() < 1)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GenerateIsolatedPointGroups() ABORTED, EMPTY BOUNDARY POINTS"));
        return false;
    }

    FIntPoint BoundsMin;
    FIntPoint BoundsMax;
    int32 Size;

    GenerateBoundaryData(BoundsMin, BoundsMax, Size, BoundaryPoints);

    //UE_LOG(LogTemp,Warning, TEXT("BoundsMin: %s"), *BoundsMin.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("BoundsMax: %s"), *BoundsMax.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("Size: %d"), Size);

    TSet<int32> BoundaryIndexSet;
    GenerateIndexSet(BoundaryIndexSet, BoundaryPoints, BoundsMin, Size);

    TSet<int32> VisitedIndexSet;

    const FIntPoint Offsets[4] = {
        FIntPoint(-1,  0), // W
        FIntPoint( 0, -1), // S
        FIntPoint( 1,  0), // E
        FIntPoint( 0,  1)  // N
        };

    for (const FIntPoint& BoundaryPoint : BoundaryPoints)
    {
        for (int32 i=0; i<4; ++i)
        {
            FIntPoint NeighbourPoint(BoundaryPoint+Offsets[i]);
            int32 NeighbourIndex = GetGridIndex(NeighbourPoint, BoundsMin, Size);

            if (! IsOnBounds(NeighbourPoint, BoundsMin, BoundsMax) ||
                BoundaryIndexSet.Contains(NeighbourIndex) ||
                VisitedIndexSet.Contains(NeighbourIndex))
            {
                continue;
            }

            TArray<FIntPoint> IsolatedPoints;

            PointFill(
                IsolatedPoints,
                BoundaryIndexSet,
                BoundsMin,
                BoundsMax,
                NeighbourPoint,
                &VisitedIndexSet
                );

            if (IsolatedPoints.Num() > 0)
            {
                OutPointGroups.AddDefaulted();
                OutPointGroups.Last().Points = MoveTemp(IsolatedPoints);
            }
        }
    }
    
    return true;
}

bool UGULGridUtility::GenerateIsolatedPointGroupsWithinBounds(TArray<FGULIntPointGroup>& OutPointGroups, const TArray<FIntPoint>& BoundaryPoints, FIntPoint BoundsMin, FIntPoint BoundsMax)
{
    FBox2D Bounds(ForceInitToZero);
    Bounds += FVector2D(BoundsMin.X, BoundsMin.Y);
    Bounds += FVector2D(BoundsMax.X, BoundsMax.Y);

    BoundsMin.X = FMath::RoundToInt(Bounds.Min.X);
    BoundsMin.Y = FMath::RoundToInt(Bounds.Min.Y);

    BoundsMax.X = FMath::RoundToInt(Bounds.Max.X);
    BoundsMax.Y = FMath::RoundToInt(Bounds.Max.Y);

    int32 Size = GetSquaredSize(BoundsMin, BoundsMax);

    //UE_LOG(LogTemp,Warning, TEXT("BoundsMin: %s, BoundsMax: %s, Size: %d"), *BoundsMin.ToString(), *BoundsMax.ToString(), Size);

    TArray<FIntPoint> ValidBoundaryPoints;
    ValidBoundaryPoints.Reserve(BoundaryPoints.Num());

    for (const FIntPoint& Point : BoundaryPoints)
    {
        if (IsOnBounds(Point, BoundsMin, BoundsMax))
        {
            ValidBoundaryPoints.Emplace(Point);
        }
    }

    if (ValidBoundaryPoints.Num() < 1)
    {
        OutPointGroups.AddDefaulted();
        TArray<FIntPoint>& OutPoints(OutPointGroups.Last().Points);

        for (int32 y=BoundsMin.Y; y<=BoundsMax.Y; ++y)
        for (int32 x=BoundsMin.X; x<=BoundsMax.X; ++x)
        {
            OutPoints.Emplace(x, y);
        }

        return true;
    }

    //UE_LOG(LogTemp,Warning, TEXT("BoundsMin: %s"), *BoundsMin.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("BoundsMax: %s"), *BoundsMax.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("Size: %d"), Size);

    TSet<int32> BoundaryIndexSet;
    GenerateIndexSet(BoundaryIndexSet, ValidBoundaryPoints, BoundsMin, Size);

    TSet<int32> VisitedIndexSet;

    const FIntPoint Offsets[4] = {
        FIntPoint(-1,  0), // W
        FIntPoint( 0, -1), // S
        FIntPoint( 1,  0), // E
        FIntPoint( 0,  1)  // N
        };

    for (const FIntPoint& BoundaryPoint : ValidBoundaryPoints)
    {
        for (int32 i=0; i<4; ++i)
        {
            FIntPoint NeighbourPoint(BoundaryPoint+Offsets[i]);
            int32 NeighbourIndex = GetGridIndex(NeighbourPoint, BoundsMin, Size);

            if (! IsOnBounds(NeighbourPoint, BoundsMin, BoundsMax) ||
                BoundaryIndexSet.Contains(NeighbourIndex) ||
                VisitedIndexSet.Contains(NeighbourIndex))
            {
                continue;
            }

            TArray<FIntPoint> IsolatedPoints;

            PointFill(
                IsolatedPoints,
                BoundaryIndexSet,
                BoundsMin,
                BoundsMax,
                NeighbourPoint,
                &VisitedIndexSet
                );

            if (IsolatedPoints.Num() > 0)
            {
                OutPointGroups.AddDefaulted();
                OutPointGroups.Last().Points = MoveTemp(IsolatedPoints);
            }
        }
    }
    
    return true;
}

void UGULGridUtility::GenerateIsolatedGridsOnPoly(
    TArray<FIntPoint>& GridIds,
    const TArray<FIntPoint>& InBoundaryPoints,
    const TArray<FGULVector2DGroup>& InPolyGroups,
    int32 GridSize,
    FIntPoint BoundsMin,
    FIntPoint BoundsMax
    )
{
    GridIds.Reset();

    TArray<FGULIntPointGroup> IsolatedPointGroups;

    if (BoundsMin.X < BoundsMax.X && BoundsMin.Y < BoundsMax.Y)
    {
        UGULGridUtility::GenerateIsolatedPointGroupsWithinBounds(
            IsolatedPointGroups,
            InBoundaryPoints,
            BoundsMin,
            BoundsMax
            );
    }
    else
    {
        UGULGridUtility::GenerateIsolatedPointGroups(
            IsolatedPointGroups,
            InBoundaryPoints
            );
    }

    for (FGULIntPointGroup& PointGroup : IsolatedPointGroups)
    {
        if (PointGroup.Points.Num() < 1)
        {
            continue;
        }

        FVector2D ReferencePoint;
        ReferencePoint.X = PointGroup.Points[0].X*GridSize;
        ReferencePoint.Y = PointGroup.Points[0].Y*GridSize;
        ReferencePoint.X += static_cast<float>(GridSize) * .5f;
        ReferencePoint.Y += static_cast<float>(GridSize) * .5f;

        for (const FGULVector2DGroup& PolyGroup : InPolyGroups)
        {
            if (UGULPolyUtilityLibrary::IsPointOnPoly(ReferencePoint, PolyGroup.Points))
            {
                GridIds.Append(PointGroup.Points);
                break;
            }
        }
    }
}
