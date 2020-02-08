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
#include "Async/ParallelFor.h"
#include "GeometryUtilityLibrary.h"
#include "GULMathLibrary.h"
#include "Poly/GULPolyUtilityLibrary.h"

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

    struct FGroupData
    {
        FIntPoint GroupId;
        TArray<FIntPoint> GridIds;
    };

    TMap< uint32, FGroupData > GroupMap;

    for (const FIntPoint& GridId : InGridIds)
    {
        FIntPoint GroupId;
        GroupId.X = FMath::FloorToInt(static_cast<float>(GridId.X) / GroupDimensionX);
        GroupId.Y = FMath::FloorToInt(static_cast<float>(GridId.Y) / GroupDimensionY);
        uint32 Hash = UGULMathLibrary::GetHash(GroupId);

        FGroupData* GroupData(GroupMap.Find(Hash));

        if (! GroupData)
        {
            GroupData = &GroupMap.Emplace(Hash);
            GroupData->GroupId = GroupId;
        }

        GroupData->GridIds.Emplace(GridId);
    }

    const int32 GroupCount = GroupMap.Num();

    OutGroupIds.Reserve(GroupCount);
    OutGridIdGroups.Reserve(GroupCount);

    for (auto& DataPair : GroupMap)
    {
        FGroupData& GroupData(DataPair.Get<1>());

        FIntPoint& GroupId(GroupData.GroupId);
        TArray<FIntPoint>& GridIds(GroupData.GridIds);

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
    const int32 Size = GetBoundsStride(BoundsMin, BoundsMax);
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
    const int32 Size = GetBoundsStride(BoundsMin, BoundsMax);

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

void UGULGridUtility::VisitPoints(
    const FIntPoint& BoundsMin,
    const FIntPoint& BoundsMax,
    const TArray<FIntPoint>& TargetPoints,
    TFunction<bool(int32, FIntPoint, int32)> VisitCallback
    )
{
    const int32 Size = GetBoundsStride(BoundsMin, BoundsMax);

    check(Size > 0);

    TQueue<FIntPoint> VisitQueue0;
    TQueue<FIntPoint> VisitQueue1;

    TSet<int32> VisitedSet;

    // Visit target points
    for (const FIntPoint& TargetPoint : TargetPoints)
    {
        int32 TargetIndex = GetGridIndex(TargetPoint, BoundsMin, Size);

        if (! IsOnBounds(TargetPoint, BoundsMin, BoundsMax) ||
            VisitedSet.Contains(TargetIndex))
        {
            continue;
        }

        if (! VisitCallback || VisitCallback(TargetIndex, TargetPoint, 0))
        {
            // Add point to the visited set
            VisitedSet.Emplace(TargetIndex);
            VisitQueue0.Enqueue(TargetPoint);
        }
    }

    const FIntPoint Offsets[4] = {
        FIntPoint(-1,  0), // W
        FIntPoint( 0, -1), // S
        FIntPoint( 1,  0), // E
        FIntPoint( 0,  1)  // N
        };

    TQueue<FIntPoint>* CurVisitQueue(&VisitQueue1);
    TQueue<FIntPoint>* NextVisitQueue(&VisitQueue0);
    int32 Iteration = 1;

    while (! NextVisitQueue->IsEmpty())
    {
        Swap(CurVisitQueue, NextVisitQueue);

        // Visit target point neighbours
        while (! CurVisitQueue->IsEmpty())
        {
            FIntPoint VisitPoint;
            CurVisitQueue->Dequeue(VisitPoint);

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

                if (! VisitCallback || VisitCallback(NeighbourIndex, NeighbourPoint, Iteration))
                {
                    // Add point to the visited set
                    VisitedSet.Emplace(NeighbourIndex);
                    NextVisitQueue->Enqueue(NeighbourPoint);
                }
            }
        }

        ++Iteration;
    }
}

bool UGULGridUtility::GridFillByPoint(
    TArray<FIntPoint>& OutPoints,
    const TArray<FIntPoint>& BoundaryPoints,
    const FIntPoint& FillTargetPoint
    )
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

    if (! IsOnBounds(FillTargetPoint, BoundsMin, BoundsMax))
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GridFillByPoint() ABORTED, OUT-OF-BOUND FILL TARGET POINT"));
        return false;
    }

    TSet<int32> BoundaryIndexSet;
    const int32 FillTargetIndex = GetGridIndex(FillTargetPoint, BoundsMin, Size);

    for (const FIntPoint& BoundaryPoint : BoundaryPoints)
    {
        int32 GridIndex = GetGridIndex(BoundaryPoint, BoundsMin, Size);
        BoundaryIndexSet.Emplace(GridIndex);
    }

    if (BoundaryIndexSet.Contains(FillTargetIndex))
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GridFillByPoint() ABORTED, FILL TARGET POINT IS A BOUNDARY POINT"));
        return false;
    }

    PointFill(
        OutPoints,
        BoundaryIndexSet,
        BoundsMin,
        BoundsMax,
        FillTargetPoint
        );
    
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

void UGULGridUtility::VisitPointsByPredicate(
    const TArray<FIntPoint>& InTargetPoints,
    FIntPoint BoundsMin,
    FIntPoint BoundsMax,
    TFunction<bool(int32, FIntPoint, int32)> VisitCallback
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
        return;
    }

    VisitPoints(
        BoundsMin,
        BoundsMax,
        TargetPoints,
        VisitCallback
        );
}

bool UGULGridUtility::GenerateGridAndEdgeGroupsIntersections(
    TMap<int32, FGULVector2DGroup>& IntersectionMap,
    TArray<FVector2D>& IntersectEdges,
    const TArray<FGULVector2DGroup>& InEdgeGroups,
    const FIntPoint& GridBoundsMin,
    const FIntPoint& GridBoundsMax,
    const FIntPoint& GridId,
    int32 GridIndex,
    float IntersectRadius
    )
{
    check((GridBoundsMax-GridBoundsMin).SizeSquared() > KINDA_SMALL_NUMBER);

    float Radius = FMath::Abs(IntersectRadius);
    float RadiusSq = Radius*Radius;

    FBox2D Bounds(ForceInitToZero);
    Bounds += FVector2D(GridBoundsMin);
    Bounds += FVector2D(GridBoundsMax);
    Bounds = Bounds.ExpandBy(Radius);

    FVector2D SegA[4] = {
        FVector2D(Bounds.Min.X, Bounds.Min.Y),
        FVector2D(Bounds.Max.X, Bounds.Min.Y),
        FVector2D(Bounds.Max.X, Bounds.Max.Y),
        FVector2D(Bounds.Min.X, Bounds.Max.Y)
        };

    int32 LastIntersectEdgeCount = IntersectEdges.Num();

    for (const FGULVector2DGroup& EdgeGroup : InEdgeGroups)
    {
        const TArray<FVector2D>& Points(EdgeGroup.Points);
        const int32 PointCount = Points.Num();

        for (int32 pi=0; pi<(PointCount-1); ++pi)
        {
            int32 i0 = pi;
            int32 i1 = pi+1;

            const FVector2D& SegB0(Points[i0]);
            const FVector2D& SegB1(Points[i1]);

            const bool bValidSegB = (SegB1-SegB0).SizeSquared() > KINDA_SMALL_NUMBER;

            bool bHasIntersection = false;

            // Edge segments have non-zero length
            if (bValidSegB)
            {
                FVector2D ClosestP0;
                FVector2D ClosestP1;

                for (int32 i=0; i<4; ++i)
                {
                    bHasIntersection = UGULGeometryUtility::SegmentIntersection2D(
                        SegA[i      ],
                        SegA[(i+1)%4],
                        SegB0,
                        SegB1
                        );

                    if (bHasIntersection)
                    {
                        break;
                    }
                }

                // No intersection found, check whether
                // either of the points is inside grid bounds
                if (! bHasIntersection &&
                    (Bounds.IsInside(SegB0) || Bounds.IsInside(SegB1)))
                {
                    bHasIntersection = true;
                }
            }
            // Edge segment have nearly zero length
            else
            {
                FVector2D ClosestP = Bounds.GetClosestPointTo(SegB0);
                float DistSq = (ClosestP-SegB0).SizeSquared();

                if (DistSq <= RadiusSq || Bounds.IsInside(SegB0))
                {
                    bHasIntersection = true;
                }
            }

            if (bHasIntersection)
            {
                IntersectEdges.Emplace(SegB0);
                IntersectEdges.Emplace(SegB1);
            }
        }
    }

    bool bHasIntersection = IntersectEdges.Num() > LastIntersectEdgeCount;

    return bHasIntersection;
}

void UGULGridUtility::GenerateGridsFromPolyGroups(
    TArray<FIntPoint>& OutGridIds,
    const TArray<FGULVector2DGroup>& InPolys,
    int32 InGridSizeX,
    int32 InGridSizeY,
    bool bClosedPolygons,
    int32 GridSizePerSegment
    )
{
    if (InGridSizeX < 1 || InGridSizeY < 1)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULGridUtility::GenerateGridsFromPolyGroups() ABORTED, INVALID GRID SIZE"));
        return;
    }

    const int32 PolyCount = InPolys.Num();
    const FVector2D GridSize(InGridSizeX, InGridSizeY);
    const float SparseLength = (GridSize*FMath::Max(1,GridSizePerSegment)).Size();

    if (PolyCount < 1)
    {
        return;
    }

    TArray<FIntPoint> GridIds;

    for (int32 PolyIt=0; PolyIt<PolyCount; ++PolyIt)
    {
        const TArray<FVector2D>& PolyPoints(InPolys[PolyIt].Points);

        if (PolyPoints.Num() < 2)
        {
            return;
        }

        TArray<FVector2D> SparsePoints;
        UGULPolyUtilityLibrary::SubdividePolylinesWithinLength(
            SparsePoints,
            PolyPoints,
            SparseLength,
            bClosedPolygons,
            true
            );

        const int32 PointCount = SparsePoints.Num();
        const int32 EdgeCount = PointCount-1;

        // Grid walk poly line segments

        TArray< TArray<FIntPoint> > GridIdGroups;
        GridIdGroups.SetNum(EdgeCount);

        // Generate grid data from line segments
        ParallelFor(EdgeCount, [&](int32 EdgeIndex)
        {
            const FVector2D& P0(SparsePoints[EdgeIndex  ]);
            const FVector2D& P1(SparsePoints[EdgeIndex+1]);

            FIntPoint ID0(GetGridId(P0, InGridSizeX, InGridSizeY));
            FIntPoint ID1(GetGridId(P1, InGridSizeX, InGridSizeY));

            TArray<FIntPoint>& GridIdGroup(GridIdGroups[EdgeIndex]);

            GridIdGroup.Emplace(ID0);

            if (ID0 != ID1)
            {
                GridWalk(
                    GridIdGroup,
                    P0,
                    P1,
                    InGridSizeX,
                    InGridSizeY
                    );
            }
        } );

        // Gather grid id sets union
        for (int32 i=0; i<EdgeCount; ++i)
        {
            GridIds.Append(MoveTemp(GridIdGroups[i]));
        }

        // Generate grid data from last line segment (last to first points)
        if (bClosedPolygons && PointCount > 2)
        {
            int32 i0 = PointCount-1;
            int32 i1 = 0;

            const FVector2D& P0(SparsePoints[i0]);
            const FVector2D& P1(SparsePoints[i1]);

            // Close poly if not already
            if (! P0.Equals(P1))
            {
                FIntPoint ID0(GetGridId(P0, InGridSizeX, InGridSizeY));
                FIntPoint ID1(GetGridId(P1, InGridSizeX, InGridSizeY));

                GridIds.Emplace(ID0);

                if (ID0 != ID1)
                {
                    GridWalk(
                        GridIds,
                        P0,
                        P1,
                        InGridSizeX,
                        InGridSizeY
                        );
                }
            }
        }
    }

    // Assign unique ids as output

    OutGridIds.Append(TSet<FIntPoint>(MoveTemp(GridIds)).Array());
}

bool UGULGridUtility::GenerateIsolatedPointGroups(
    TArray<FGULIntPointGroup>& OutPointGroups,
    const TArray<FIntPoint>& BoundaryPoints
    )
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

bool UGULGridUtility::GenerateIsolatedPointGroupsWithinBounds(
    TArray<FGULIntPointGroup>& OutPointGroups,
    const TArray<FIntPoint>& BoundaryPoints,
    FIntPoint BoundsMin,
    FIntPoint BoundsMax
    )
{
    FBox2D Bounds(ForceInitToZero);
    Bounds += FVector2D(BoundsMin.X, BoundsMin.Y);
    Bounds += FVector2D(BoundsMax.X, BoundsMax.Y);

    BoundsMin.X = FMath::RoundToInt(Bounds.Min.X);
    BoundsMin.Y = FMath::RoundToInt(Bounds.Min.Y);

    BoundsMax.X = FMath::RoundToInt(Bounds.Max.X);
    BoundsMax.Y = FMath::RoundToInt(Bounds.Max.Y);

    int32 Size = GetBoundsStride(BoundsMin, BoundsMax);

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
            if (UGULPolyUtilityLibrary::IsPointOnPoly(
                    ReferencePoint,
                    PolyGroup.Points
                    ) )
            {
                GridIds.Append(PointGroup.Points);
                break;
            }
        }
    }
}

void UGULGridUtility::GenerateIsolatedGridsOnPoly(
    TArray<FIntPoint>& GridIds,
    const TArray<FIntPoint>& InBoundaryPoints,
    const TArray<FGULIndexedPolyGroup>& InIndexGroups,
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

        if (UGULPolyUtilityLibrary::IsPointOnPoly(
                ReferencePoint,
                InIndexGroups,
                InPolyGroups
                ) )
        {
            GridIds.Append(PointGroup.Points);
        }
    }
}
