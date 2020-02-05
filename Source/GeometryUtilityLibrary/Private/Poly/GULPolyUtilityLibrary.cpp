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
#include "Poly/GULPolyGridObject.h"
#include "Geom/GULGeometryUtilityLibrary.h"
#include "GULMathLibrary.h"

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
    FIntPoint pt = UGULMathLibrary::ScaleToIntPoint(Point);

    //returns 0 if false, +1 if true, -1 if pt ON polygon boundary
    int32 result = 0;
    int32 cnt = Poly.Num();
    if (cnt < 3) return false; //return 0;
    FIntPoint ip = UGULMathLibrary::ScaleToIntPoint(Poly[0]);
    for(int32 i = 1; i <= cnt; ++i)
    {
        FIntPoint ipNext = (i==cnt)
            ? UGULMathLibrary::ScaleToIntPoint(Poly[0])
            : UGULMathLibrary::ScaleToIntPoint(Poly[i]);
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

bool UGULPolyUtilityLibrary::IsPointOnPoly(const FVector2D& Point, const FGULIndexedPolyGroup& IndexGroup, const TArray<FGULVector2DGroup>& PolyGroups)
{
    if (! IndexGroup.IsValidIndexGroup(PolyGroups))
    {
        return false;
    }

    if (IsPointOnPoly(Point, PolyGroups[IndexGroup.OuterPolyIndex].Points))
    {
        if (IndexGroup.InnerPolyIndices.Num() > 0)
        {
            bool bIsWithinInnerPoly = false;

            for (int32 InnerPolyIndex : IndexGroup.InnerPolyIndices)
            {
                if (IsPointOnPoly(Point, PolyGroups[InnerPolyIndex].Points))
                {
                    bIsWithinInnerPoly = true;
                    break;
                }
            }

            if (! bIsWithinInnerPoly)
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }

    return false;
}

bool UGULPolyUtilityLibrary::IsPointOnPoly(const FVector2D& Point, const TArray<FGULIndexedPolyGroup>& IndexGroups, const TArray<FGULVector2DGroup>& PolyGroups)
{
    for (const FGULIndexedPolyGroup& IndexGroup : IndexGroups)
    {
        if (IsPointOnPoly(Point, IndexGroup, PolyGroups))
        {
            return true;
        }
    }

    return false;
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
        }
    }

    OutPoints.Shrink();
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


void UGULPolyUtilityLibrary::CollapsePointAngles(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& Points, bool bCircular, int32 MaxIteration, float AngleThreshold, float SignFilter)
{
    const int32 PointCount = Points.Num();

    if ((!bCircular && Points.Num() < 2) ||
        ( bCircular && Points.Num() < 3) ||
        MaxIteration < 1
        )
    {
        return;
    }

    const bool bFilterBySign = FMath::Abs(SignFilter) > KINDA_SMALL_NUMBER;
    const bool bFilterNegative = SignFilter < 0.f;

    // Generate point list

    FPointList PointList;

    for (const FVector2D& Point : Points)
    {
        PointList.AddTail(Point);
    }

    const bool bIntersectingEndPoints = bCircular && Points[0].Equals(Points.Last());

    // First collapse iteration, iterate the whole point list

    TArray<FPointNode*> RemoveCandidates;

    {
        TSet<FPointNode*> NodesToRemoveSet;
        FPointNode* Head = PointList.GetHead();
        FPointNode* Tail = PointList.GetTail();

        for (FPointIterator NodeIt(Head); NodeIt; ++NodeIt)
        {
            FPointNode* CurrNode = NodeIt.GetNode();
            FPointNode* PrevNode = CurrNode->GetPrevNode();
            FPointNode* NextNode = CurrNode->GetNextNode();

            GetAdjacentNodes(PrevNode, NextNode, PointList, *CurrNode, bCircular, bIntersectingEndPoints);

            if (! bCircular && (! PrevNode || ! NextNode))
            {
                continue;
            }

            check(CurrNode != nullptr);
            check(PrevNode != nullptr);
            check(NextNode != nullptr);

            const FVector2D& P0(PrevNode->GetValue());
            const FVector2D& P1(CurrNode->GetValue());
            const FVector2D& P2(NextNode->GetValue());

            bool bRemovePoint = IsPointAngleBelowThreshold(P0, P1, P2, AngleThreshold, bFilterBySign, bFilterNegative);

            if (bRemovePoint)
            {
                NodesToRemoveSet.Emplace(CurrNode);
            }
        }

        // Collapse or merge node and add affected node to the next iteration remove candidates

        // Get removal set copy since the set will be modified on collapse/merge operation
        TArray<FPointNode*> NodesToRemoveArr(NodesToRemoveSet.Array());

        for (FPointNode* CurrNode : NodesToRemoveArr)
        {
            check(CurrNode != nullptr);
            CollapseOrMergePointNodeFromSet(PointList, NodesToRemoveSet, *CurrNode, bCircular, &RemoveCandidates);
        }
    }

    // Collapse iteration

    for (int32 It=1; It<MaxIteration && IsValidPointCountToCollapse(PointList.Num(), bCircular) && (RemoveCandidates.Num() > 0); ++It)
    {
        // Generate removal candidates

        TSet<FPointNode*> NodesToRemoveSet;
        FPointNode* Head = PointList.GetHead();
        FPointNode* Tail = PointList.GetTail();

        for (FPointNode* CurrNode : RemoveCandidates)
        {
            FPointNode* PrevNode = CurrNode->GetPrevNode();
            FPointNode* NextNode = CurrNode->GetNextNode();

            GetAdjacentNodes(PrevNode, NextNode, PointList, *CurrNode, bCircular, bIntersectingEndPoints);

            if (! bCircular && (! PrevNode || ! NextNode))
            {
                continue;
            }

            check(CurrNode != nullptr);
            check(PrevNode != nullptr);
            check(NextNode != nullptr);

            const FVector2D& P0(PrevNode->GetValue());
            const FVector2D& P1(CurrNode->GetValue());
            const FVector2D& P2(NextNode->GetValue());

            bool bRemovePoint = IsPointAngleBelowThreshold(P0, P1, P2, AngleThreshold, bFilterBySign, bFilterNegative);

            if (bRemovePoint)
            {
                NodesToRemoveSet.Emplace(CurrNode);
            }
        }

        // No more nodes to remove, break
        if (NodesToRemoveSet.Num() < 1)
        {
            break;
        }

        // Collapse or merge node and add affected node to the next iteration remove candidates

        // Get removal set copy since the set will be modified on collapse/merge operation
        TArray<FPointNode*> NodesToRemoveArr(NodesToRemoveSet.Array());

        // Reset remove candidates
        RemoveCandidates.Reset();

        for (FPointNode* CurrNode : NodesToRemoveArr)
        {
            check(CurrNode != nullptr);
            CollapseOrMergePointNodeFromSet(PointList, NodesToRemoveSet, *CurrNode, bCircular, &RemoveCandidates);
        }
    }

    // Generate output points

    OutPoints.Reserve(PointList.Num());

    for (const FVector2D& Point : PointList)
    {
        OutPoints.Emplace(Point);
    }
}

void UGULPolyUtilityLibrary::CollapseOrMergePointNodeFromSet(FPointList& PointList, FPointNodeSet& PointNodeSet, FPointNode& PointNode, bool bCircular, TArray<FPointNode*>* RemovedNodeNeighbours)
{
    if (! PointNodeSet.Contains(&PointNode))
    {
        return;
    }

    FPointNode* NodePrev = PointNode.GetPrevNode();
    FPointNode* NodeNext = PointNode.GetNextNode();

    // Check for adjacent remove candidates
    const bool bRemovePrev = NodePrev && PointNodeSet.Contains(NodePrev);
    const bool bRemoveNext = NodeNext && PointNodeSet.Contains(NodeNext);

    // No adjacent remove candidate, remove the point
    if (! bRemovePrev && ! bRemoveNext)
    {
        PointNodeSet.Remove(&PointNode);
        PointList.RemoveNode(&PointNode, true);

        // Add removed node neighbours if required
        if (RemovedNodeNeighbours)
        {
            if (NodePrev)
            {
                RemovedNodeNeighbours->Emplace(NodePrev);
            }

            if (NodeNext)
            {
                RemovedNodeNeighbours->Emplace(NodeNext);
            }
        }
    }
    // Merge multiple adjacent remove candidates
    else
    {
        TArray<FPointNode*> RemoveCandidates;

        RemoveCandidates.Emplace(&PointNode);

        // Find previous nodes to remove
        while (NodePrev && PointNodeSet.Contains(NodePrev))
        {
            RemoveCandidates.Emplace(NodePrev);
            NodePrev = NodePrev->GetPrevNode();
        }

        // Find next nodes to remove
        while (NodeNext && PointNodeSet.Contains(NodeNext))
        {
            RemoveCandidates.Emplace(NodeNext);
            NodeNext = NodeNext->GetNextNode();
        }

        // Find nodes center as merge point

        FBox2D Bounds(ForceInitToZero);

        for (FPointNode* RemoveCandidate : RemoveCandidates)
        {
            Bounds += RemoveCandidate->GetValue();
        }

        // Insert node as merge point

        FPointNode* MergeNode = new FPointNode(Bounds.GetCenter());
        bool bMergeNodeValid = PointList.InsertNode(MergeNode, &PointNode);

        check(bMergeNodeValid);

        if (! bMergeNodeValid)
        {
            delete MergeNode;
            MergeNode = nullptr;
        }

        // Remove found nodes
        for (FPointNode* RemoveCandidate : RemoveCandidates)
        {
            PointNodeSet.Remove(RemoveCandidate);
            PointList.RemoveNode(RemoveCandidate, true);
        }

        // Add merged node if required
        if (RemovedNodeNeighbours && MergeNode)
        {
            RemovedNodeNeighbours->Emplace(MergeNode);
        }
    }
}

void UGULPolyUtilityLibrary::SubdividePolylines(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints)
{
    OutPoints.Reset(InPoints.Num() * 2);

    for (int32 i=1; i<InPoints.Num(); ++i)
    {
        const FVector2D& P0(InPoints[i-1]);
        const FVector2D& P1(InPoints[i]);

        OutPoints.Emplace(P0);
        OutPoints.Emplace(P0+(P1-P0)*.5f);
    }

    OutPoints.Emplace(InPoints.Last());
}

void UGULPolyUtilityLibrary::FixOrientations(TArray<FGULVector2DGroup>& InOutPolyGroups)
{
    for (FGULVector2DGroup& Poly : InOutPolyGroups)
    {
        if (! GetOrientation(Poly.Points))
        {
            Algo::Reverse(Poly.Points);
        }
    }
}

void UGULPolyUtilityLibrary::GroupPolyHierarchyEvenOdd(TArray<FGULIndexedPolyGroup>& OutIndexedPolyGroups, const TArray<FGULVector2DGroup>& PolyGroups)
{
    const int32 InPolyCount = PolyGroups.Num();

    // Generate candidate indices

    TArray<int32> CandidateIndices;
    CandidateIndices.SetNumUninitialized(InPolyCount);

    for (int32 PolyIt=0; PolyIt<InPolyCount; ++PolyIt)
    {
        // Only generate index for valid poly
        if (PolyGroups[PolyIt].Points.Num() >= 3)
        {
            CandidateIndices[PolyIt] = PolyIt;
        }
    }

    TArray<int32> OuterPolyIndices;
    TMap<int32, FGULIndexedPolyGroup> InitialGroupMap;

    for (int32 i=0; i<CandidateIndices.Num(); ++i)
    {
        int32 PolyIndex = CandidateIndices[i];
        const TArray<FVector2D>& Points(PolyGroups[PolyIndex].Points);

        // Poly orientation is positive, Assign as outer poly
        if (GetOrientation(Points))
        {
            OuterPolyIndices.Emplace(PolyIndex);
            continue;
        }

        FVector2D InnerPoint(Points[0]);

        // Find for bounding poly if any
        for (int32 j=0; j<CandidateIndices.Num(); ++j)
        {
            // Skip checking the same poly
            if (j == i)
            {
                continue;
            }

            int32 BoundingPolyIndex = CandidateIndices[j];
            const TArray<FVector2D>& BoundingPoints(PolyGroups[BoundingPolyIndex].Points);

            // Bounding poly found, map the connection
            if (IsPointOnPoly(InnerPoint, BoundingPoints))
            {
                FGULIndexedPolyGroup& PolyGroup(InitialGroupMap.FindOrAdd(BoundingPolyIndex));
                PolyGroup.OuterPolyIndex = BoundingPolyIndex;
                PolyGroup.InnerPolyIndices.Emplace(PolyIndex);
                break;
            }
        }
    }

    // Assign mapped inner poly to outer poly
    while (OuterPolyIndices.Num() > 0)
    {
        int32 OuterPolyIndex = OuterPolyIndices.Pop();

        // Create new outer poly group

        OutIndexedPolyGroups.AddDefaulted();
        FGULIndexedPolyGroup& OuterPolyGroup(OutIndexedPolyGroups.Last());

        OuterPolyGroup.OuterPolyIndex = OuterPolyIndex;

        // Find inner poly (outer poly holes)

        FGULIndexedPolyGroup* MappedOuterPoly(InitialGroupMap.Find(OuterPolyIndex));

        // Outer poly is not mapped, no inner poly, continue
        if (! MappedOuterPoly)
        {
            continue;
        }

        // Move mapped inner poly indices
        OuterPolyGroup.InnerPolyIndices = MoveTemp(MappedOuterPoly->InnerPolyIndices);

        // Remove mapped outer poly index
        InitialGroupMap.Remove(OuterPolyIndex);

        // Find inner poly within current mapped inner poly
        // and add them as new outer poly
        for (int32 InnerPolyIndex : OuterPolyGroup.InnerPolyIndices)
        {
            FGULIndexedPolyGroup* MappedInnerPoly(InitialGroupMap.Find(InnerPolyIndex));

            if (MappedInnerPoly)
            {
                OuterPolyIndices.Append(MappedInnerPoly->InnerPolyIndices);
                InitialGroupMap.Remove(InnerPolyIndex);
            }
        }
    }

    // Move inner poly to the innermost outer poly
    for (int32 pi0=0; pi0<OutIndexedPolyGroups.Num(); ++pi0)
    {
        FGULIndexedPolyGroup& IndexGroup0(OutIndexedPolyGroups[pi0]);
        int32 OuterIndex0 = IndexGroup0.OuterPolyIndex;

        const TArray<FVector2D>& OuterPoly0(PolyGroups[OuterIndex0].Points);
        const FVector2D& OuterPoint0(OuterPoly0[0]);

        for (int32 pi1=0; pi1<OutIndexedPolyGroups.Num(); ++pi1)
        {
            // Skip checking the same poly
            if (pi1 == pi0)
            {
                continue;
            }

            FGULIndexedPolyGroup& IndexGroup1(OutIndexedPolyGroups[pi1]);
            TArray<int32>& InnerIndices(IndexGroup1.InnerPolyIndices);
            int32 OuterIndex1 = IndexGroup1.OuterPolyIndex;

            // OuterPoly0 is not within OuterPoly1, skip
            if (! IsPointOnPoly(OuterPoint0, PolyGroups[OuterIndex1].Points))
            {
                continue;
            }

            // OuterPoly0 is within OuterPoly1, move any OuterPoly0 inner poly
            // that is within OuterPoly1 as OuterPoly1 inner poly

            InnerIndices.RemoveAll(
                [&PolyGroups, &IndexGroup0, &OuterPoly0](int32 InnerIndex)
                {
                    bool bMoveInner = IsPointOnPoly(
                        PolyGroups[InnerIndex].Points[0],
                        OuterPoly0
                        );

                    if (bMoveInner)
                    {
                        IndexGroup0.InnerPolyIndices.Emplace(InnerIndex);
                    }

                    return bMoveInner;
                } );
        }
    }
}

void UGULPolyUtilityLibrary::ConvertIndexedPolyGroupToVectorGroup(FGULVectorGroup& OutVectorGroup, const FGULIndexedPolyGroup& InIndexedPolyGroup, const TArray<FGULVector2DGroup>& InPolyGroups, float ZPosition)
{
    // Skip invalid index group
    if (! InIndexedPolyGroup.IsValidIndexGroup(InPolyGroups))
    {
        return;
    }

    UGULGeometryUtility::ConvertVector2DGroupToVectorGroup(
        OutVectorGroup,
        InPolyGroups[InIndexedPolyGroup.OuterPolyIndex],
        ZPosition
        );

    const TArray<int32>& InnerPolyIndices(InIndexedPolyGroup.InnerPolyIndices);

    for (int32 i=0; i<InnerPolyIndices.Num(); ++i)
    {
        UGULGeometryUtility::ConvertVector2DGroupToVectorGroup(
            OutVectorGroup,
            InPolyGroups[InnerPolyIndices[i]],
            ZPosition
            );
    }
}

void UGULPolyUtilityLibrary::ClipBounds(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const FBox2D& InBounds)
{
    OutPoints.Reset();

    if (InPoints.Num() < 3 ||
        InBounds.Min.X > InBounds.Max.X ||
        InBounds.Min.Y > InBounds.Max.Y)
    {
        return;
    }

    TArray<FVector2D> ClipPoints0;
    TArray<FVector2D> ClipPoints1(InPoints);

    // Ensure open poly (first point is different from the last)
    if (ClipPoints1[0].Equals(ClipPoints1.Last()))
    {
        ClipPoints1.Pop();
    }

    // Clip poly on each bounds side
    for (int32 s=0; s<4; ++s)
    {
        // Swap point containers
        ClipPoints0 = MoveTemp(ClipPoints1);
        ClipPoints1 = TArray<FVector2D>();

        // Abort clip if there is not sufficient points left

        const int32 PointCount = ClipPoints0.Num();

        if (PointCount < 3)
        {
            return;
        }

        ClipPoints1.Reserve(PointCount);

        // Clip poly on bounds side

        const EGULBox2DClip ClipCode = static_cast<EGULBox2DClip>(1 << s);

        FVector2D P0;
        FVector2D P1(ClipPoints0[PointCount-1]);

        bool T0;
        bool T1 = UGULGeometryUtility::IsInsideBounds(P1, InBounds, ClipCode);

        for (int32 i=0; i<PointCount; ++i)
        {
            P0 = P1;
            P1 = ClipPoints0[i];

            // Skip coincident edge
            if ((P1-P0).SizeSquared() < KINDA_SMALL_NUMBER)
            {
                continue;
            }

            T0 = T1;
            T1 = UGULGeometryUtility::IsInsideBounds(P1, InBounds, ClipCode);

            // Points are on different clip side, add intersection
            if (T0 != T1)
            {
                ClipPoints1.Emplace( UGULGeometryUtility::IntersectBounds(
                    P0,
                    P1,
                    InBounds,
                    ClipCode
                    ) );
            }

            // Point is within clip bounds, add point
            if (T1 == 0)
            {
                ClipPoints1.Emplace(P1);
            }
        }
    }

    // Filter duplicate points
    {
        ClipPoints0 = MoveTemp(ClipPoints1);
        ClipPoints1 = TArray<FVector2D>();

        const int32 PointCount = ClipPoints0.Num();

        if (PointCount < 3)
        {
            return;
        }

        ClipPoints1.Reserve(PointCount);

        FVector2D P0;
        FVector2D P1(ClipPoints0[PointCount-1]);

        for (int32 i=0; i<PointCount; ++i)
        {
            P0 = P1;
            P1 = ClipPoints0[i];

            // Skip coincident edge
            if ((P1-P0).SizeSquared() < KINDA_SMALL_NUMBER)
            {
                continue;
            }

            ClipPoints1.Emplace(P1);
        }
    }

    OutPoints = MoveTemp(ClipPoints1);
}

void UGULPolyUtilityLibrary::ClipBounds(TArray<FGULVector2DGroup>& OutPolyGroups, const TArray<FGULVector2DGroup>& InPolyGroups, const FBox2D& InBounds)
{
    OutPolyGroups.Reset();
    OutPolyGroups.SetNum(InPolyGroups.Num());

    for (int32 i=0; i<InPolyGroups.Num(); ++i)
    {
        ClipBounds(OutPolyGroups[i].Points, InPolyGroups[i].Points, InBounds);
    }
}
