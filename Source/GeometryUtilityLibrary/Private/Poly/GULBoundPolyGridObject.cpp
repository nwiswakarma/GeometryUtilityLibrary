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

#include "Poly/GULBoundPolyGridObject.h"
#include "Geom/GULGeometryUtilityLibrary.h"

bool UGULBoundPolyGridObject::GeneratePolyFromPointIndices(const FIntPoint& InOrigin, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices)
{
    bool bGenerateSuccess = false;
    const int32 PointCount = InPoints.Num();

    //UE_LOG(LogTemp,Warning, TEXT("GENERATE"));

    if (PointCount < 2)
    {
        return bGenerateSuccess;
    }

    TSet<int32> IndexSet(InIndices);

    while (IndexSet.Num() > 0)
    {
        int32 i1 = *IndexSet.CreateConstIterator();
        IndexSet.Remove(i1);

        //UE_LOG(LogTemp,Warning, TEXT("i1: %d"), i1);

        if (! InPoints.IsValidIndex(i1))
        {
            continue;
        }

        TDoubleLinkedList<FVector2D> PointList;
        PointList.AddHead(InPoints[i1]);

        // Find head connection
        {
            int32 It = i1;
            bool bHasConnection = false;
            do
            {
                int32 i0 = (It>0) ? It-1 : PointCount-1;
                bHasConnection = false;

                if (IndexSet.Contains(i0))
                {
                    PointList.AddHead(InPoints[i0]);
                    IndexSet.Remove(i0);
                    bHasConnection = true;
                    //UE_LOG(LogTemp,Warning, TEXT("TAIL"));
                }

                It = i0;
            }
            while (bHasConnection);
        }

        // Find tail connection
        {
            int32 It = i1;
            bool bHasConnection = false;
            do
            {
                int32 i2 = (It+1) % PointCount;
                bHasConnection = false;

                if (IndexSet.Contains(i2))
                {
                    PointList.AddTail(InPoints[i2]);
                    IndexSet.Remove(i2);
                    bHasConnection = true;
                    //UE_LOG(LogTemp,Warning, TEXT("HEAD"));
                }

                It = i2;
            }
            while (bHasConnection);
        }

        // Assign segment point list
        if (PointList.Num() > 1)
        {
            auto& SegmentPoints(Segments[Segments.AddDefaulted()].Points);
            SegmentPoints.Reserve(PointList.Num());

            for (const FVector2D& Point : PointList)
            {
                SegmentPoints.Emplace(Point);
            }

            //UE_LOG(LogTemp,Warning, TEXT("SegmentPointList.Num(): %d"), SegmentPointList.Num());

            bGenerateSuccess = true;
        }
    }

    //UE_LOG(LogTemp,Warning, TEXT("bHasConnection: %d"), Segments.Num());

    if (bGenerateSuccess)
    {
        Origin = InOrigin;
        Origin.X *= DimensionX;
        Origin.Y *= DimensionY;
    }

    return bGenerateSuccess;
}

void UGULBoundPolyGridObject::SnapPointsToSegmentPoints(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, float SnapRange)
{
    const int32 PointCount = InPoints.Num();

    if (DimensionX < 1 || DimensionY < 1 || SnapRange < SMALL_NUMBER)
    {
        return;
    }

    const FBox2D Bounds(GetBounds());
    TArray<FVector2D> BoundPoints;
    GetBoundingPoints(BoundPoints);

    TArray<FVector2D> SnapPoints;

    for (const FPointGroup& SegmentPointGroup : Segments)
    {
        const TArray<FVector2D>& SegmentPoints(SegmentPointGroup.Points);
        const int32 SegmentPointCount = SegmentPoints.Num();

        for (int32 i=0; i<SegmentPointCount; ++i)
        {
            int32 i0 = i;
            int32 i1 = (i+1) % SegmentPointCount;

            const FVector2D& P0(SegmentPoints[i0]);
            const FVector2D& P1(SegmentPoints[i1]);

            bool bInsideBounds0 = Bounds.IsInside(P0);
            bool bInsideBounds1 = Bounds.IsInside(P1);

            SnapPoints.Emplace(P0);
            SnapPoints.Emplace(P1);

            if (!bInsideBounds0 && !bInsideBounds1)
            {
                FVector2D Intersect;
                bool bIntersect = false;

                // Intersect X0, Y0 -> X0 Y1
                {
                    bIntersect = UGULGeometryUtility::SegmentIntersection2D(
                        P0,
                        P1,
                        BoundPoints[0],
                        BoundPoints[1],
                        Intersect
                        );
                    if (bIntersect)
                    {
                        SnapPoints.Emplace(Intersect);
                    }
                }

                // Intersect X1, Y0 -> X1 Y1
                {
                    bIntersect = UGULGeometryUtility::SegmentIntersection2D(
                        P0,
                        P1,
                        BoundPoints[2],
                        BoundPoints[3],
                        Intersect
                        );
                    if (bIntersect)
                    {
                        SnapPoints.Emplace(Intersect);
                    }
                }

                // Intersect X0, Y0 -> X1 Y0
                {
                    bIntersect = UGULGeometryUtility::SegmentIntersection2D(
                        P0,
                        P1,
                        BoundPoints[0],
                        BoundPoints[2],
                        Intersect
                        );
                    if (bIntersect)
                    {
                        SnapPoints.Emplace(Intersect);
                    }
                }

                // Intersect X0, Y1 -> X1 Y1
                {
                    bIntersect = UGULGeometryUtility::SegmentIntersection2D(
                        P0,
                        P1,
                        BoundPoints[1],
                        BoundPoints[3],
                        Intersect
                        );
                    if (bIntersect)
                    {
                        SnapPoints.Emplace(Intersect);
                    }
                }
            }
            else
            if (!bInsideBounds0 || !bInsideBounds1)
            {
                const float DX = P1.X-P0.X;
                const float DY = P1.Y-P0.Y;
                int32 SgnX = (DX>0.f) ? 1 : -1;
                int32 SgnY = (DY>0.f) ? 1 : -1;
                FVector2D Intersect;

                // Flip intersect direction if P0 is outside bounds
                if (! bInsideBounds0)
                {
                    SgnX *= -1;
                    SgnY *= -1;
                }

                // Find X-Axis intersection

                FVector2D SegX0;
                FVector2D SegX1;

                if (SgnX > 0)
                {
                    SegX0 = BoundPoints[0x2 | 0x0];
                    SegX1 = BoundPoints[0x2 | 0x1];
                }
                else
                {
                    SegX0 = BoundPoints[0x0 | 0x0];
                    SegX1 = BoundPoints[0x0 | 0x1];
                }

                bool bIntersectX = UGULGeometryUtility::SegmentIntersection2D(
                    P0,
                    P1,
                    SegX0,
                    SegX1,
                    Intersect
                    );

                if (bIntersectX)
                {
                    SnapPoints.Emplace(Intersect);
                    continue;
                }

                // Find Y-Axis intersection

                FVector2D SegY0;
                FVector2D SegY1;

                if (SgnY > 0)
                {
                    SegY0 = BoundPoints[0x0 | 0x1];
                    SegY1 = BoundPoints[0x2 | 0x1];
                }                                  
                else                               
                {                                  
                    SegY0 = BoundPoints[0x0 | 0x0];
                    SegY1 = BoundPoints[0x2 | 0x0];
                }

                bool bIntersectY = UGULGeometryUtility::SegmentIntersection2D(
                    P0,
                    P1,
                    SegY0,
                    SegY1,
                    Intersect
                    );

                if (bIntersectY)
                {
                    SnapPoints.Emplace(Intersect);
                    continue;
                }

                // Check no entry
                check(false);
            }
        }
    }

    SnapPoints.Append(BoundPoints);
    OutPoints = InPoints;

    //UE_LOG(LogTemp,Warning, TEXT("SNAP POINTS: %d"), OutPoints.Num());

    //for (int32 i=0; i<PointCount; ++i)
    //{
    //    UE_LOG(LogTemp,Warning, TEXT("Point[%d]: %s"), i, *OutPoints[i].ToString());
    //}

    int32 SnapCount = 0;

    for (FVector2D& Point : OutPoints)
    {
        for (const FVector2D& SnapPoint : SnapPoints)
        {
            if (Point.Equals(SnapPoint, SnapRange))
            {
                Point = SnapPoint;
                ++SnapCount;
                break;
            }
        }
    }

    //UE_LOG(LogTemp,Warning, TEXT("SNAP COUNT: %d"), SnapCount);

    //for (int32 i=0; i<PointCount; ++i)
    //{
    //    UE_LOG(LogTemp,Warning, TEXT("Point[%d]: %s"), i, *OutPoints[i].ToString());
    //}
}

void UGULBoundPolyGridObject::UnitGridWalk(TArray<FIntPoint>& GridIds, const FVector2D& InP0, const FVector2D& InP1)
{
    // Invalid grid, abort
    if (! IsValidGrid())
    {
        return;
    }

    FBox2D GridBounds(GetBounds());
    FVector2D P0(InP0);
    FVector2D P1(InP1);

    if (UGULGeometryUtility::IsPointOnBounds(GridBounds, P0))
    {
        P0 = GridBounds.GetClosestPointTo(InP0);
    }

    if (UGULGeometryUtility::IsPointOnBounds(GridBounds, P1))
    {
        P1 = GridBounds.GetClosestPointTo(InP1);
    }

    const float DX = P1.X-P0.X;
    const float DY = P1.Y-P0.Y;
    int32 SgnX = (DX>0.f) ? 1 : -1;
    int32 SgnY = (DY>0.f) ? 1 : -1;

    //if (! GridBounds.IsInside(InP0))
    //{
    //    SgnX *= -1;
    //    SgnY *= -1;
    //}

    FIntPoint IDMin(Origin);
    FIntPoint IDMax(Origin.X+DimensionX-1, Origin.Y+DimensionY-1);

    FIntPoint ID0(FMath::FloorToInt(P0.X), FMath::FloorToInt(P0.Y));
    FIntPoint ID1(FMath::FloorToInt(P1.X), FMath::FloorToInt(P1.Y));

    ID0.X = FMath::Clamp(ID0.X, IDMin.X, IDMax.X);
    ID0.Y = FMath::Clamp(ID0.Y, IDMin.Y, IDMax.Y);

    ID1.X = FMath::Clamp(ID1.X, IDMin.X, IDMax.X);
    ID1.Y = FMath::Clamp(ID1.Y, IDMin.Y, IDMax.Y);

    //UE_LOG(LogTemp,Warning, TEXT("GridBounds: %s"), *GridBounds.ToString());

    //UE_LOG(LogTemp,Warning, TEXT("GridBounds.IsInside(InP0): %d"), GridBounds.IsInside(InP0));
    //UE_LOG(LogTemp,Warning, TEXT("GridBounds.IsInside(InP1): %d"), GridBounds.IsInside(InP1));
    //UE_LOG(LogTemp,Warning, TEXT("UGULGeometryUtility::IsPointOnBounds(GridBounds, InP0): %d"), UGULGeometryUtility::IsPointOnBounds(GridBounds, InP0));
    //UE_LOG(LogTemp,Warning, TEXT("UGULGeometryUtility::IsPointOnBounds(GridBounds, InP1): %d"), UGULGeometryUtility::IsPointOnBounds(GridBounds, InP1));

    //UE_LOG(LogTemp,Warning, TEXT("InP0: %s"), *InP0.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("InP1: %s"), *InP1.ToString());

    //UE_LOG(LogTemp,Warning, TEXT("P0: %s"), *P0.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("P1: %s"), *P1.ToString());

    //UE_LOG(LogTemp,Warning, TEXT("IDMin: %s"), *IDMin.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("IDMax: %s"), *IDMax.ToString());

    //UE_LOG(LogTemp,Warning, TEXT("ID0: %s"), *ID0.ToString());
    //UE_LOG(LogTemp,Warning, TEXT("ID1: %s"), *ID1.ToString());

    //UE_LOG(LogTemp,Warning, TEXT("SGN X: %d"), SgnX);
    //UE_LOG(LogTemp,Warning, TEXT("SGN Y: %d"), SgnY);

    while (ID0 != ID1)
    {
        FBox2D Bounds(ForceInitToZero);
        Bounds += FVector2D(ID0.X, ID0.Y);
        Bounds += Bounds.Min + 1.f;

        //UE_LOG(LogTemp,Warning, TEXT("ID: %s"), *ID0.ToString());
        //UE_LOG(LogTemp,Warning, TEXT("Bounds: %s"), *Bounds.ToString());

        // Find X-Axis intersection

        FVector2D SegX0;
        FVector2D SegX1;

        if (SgnX > 0)
        {
            SegX0 = FVector2D(Bounds.Max.X, Bounds.Min.Y-KINDA_SMALL_NUMBER);
            SegX1 = FVector2D(Bounds.Max.X, Bounds.Max.Y+KINDA_SMALL_NUMBER);
        }                                                                  
        else                                                               
        {                                                                  
            SegX0 = FVector2D(Bounds.Min.X, Bounds.Min.Y-KINDA_SMALL_NUMBER);
            SegX1 = FVector2D(Bounds.Min.X, Bounds.Max.Y+KINDA_SMALL_NUMBER);
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
            GridIds.AddUnique(ID0);
            //UE_LOG(LogTemp,Warning, TEXT("INTERSECTX"));
            continue;
        }

        // Find Y-Axis intersection

        FVector2D SegY0;
        FVector2D SegY1;

        if (SgnY > 0)
        {
            SegY0 = FVector2D(Bounds.Min.X-KINDA_SMALL_NUMBER, Bounds.Max.Y);
            SegY1 = FVector2D(Bounds.Max.X+KINDA_SMALL_NUMBER, Bounds.Max.Y);
        }
        else
        {
            SegY0 = FVector2D(Bounds.Min.X-KINDA_SMALL_NUMBER, Bounds.Min.Y);
            SegY1 = FVector2D(Bounds.Max.X+KINDA_SMALL_NUMBER, Bounds.Min.Y);
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
            GridIds.AddUnique(ID0);
            //UE_LOG(LogTemp,Warning, TEXT("INTERSECTY"));
            continue;
        }

        // Check no entry
        check(false);
    }
}
