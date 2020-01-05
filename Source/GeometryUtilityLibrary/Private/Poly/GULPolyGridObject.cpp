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

#include "Poly/GULPolyGridObject.h"
#include "Poly/GULBoundPolyGridObject.h"
#include "Geom/GULGeometryUtilityLibrary.h"

void UGULPolyGridObject::GridWalk(TArray<FIntPoint>& GridIds, const FVector2D& P0, const FVector2D& P1)
{
    const float DX = P1.X-P0.X;
    const float DY = P1.Y-P0.Y;
    const int32 SgnX = (DX>0.f) ? 1 : -1;
    const int32 SgnY = (DY>0.f) ? 1 : -1;

    FIntPoint ID0(GetGridId(P0));
    FIntPoint ID1(GetGridId(P1));
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
            GridIds.Emplace(ID0);
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
            GridIds.Emplace(ID0);
            //UE_LOG(LogTemp,Warning, TEXT("INTERSECTY"));
            continue;
        }

        // Check no entry
        check(false);
    }
}

void UGULPolyGridObject::GetPointData(TArray<int32>& OutPointIndices, int32 Index)
{
    if (IsValidGrid(Index))
    {
        OutPointIndices = GridData[Index].PointData.Array();
    }
}

void UGULPolyGridObject::GenerateGridDataFromLineSegment(const FVector2D& P0, const FVector2D P1, int32 i0, int32 i1)
{
    check(DimensionX > 0);
    check(DimensionY > 0);

    FIntPoint ID0(GetGridId(P0));
    FIntPoint ID1(GetGridId(P1));

    {
        int32* GridId = GridMap.Find(ID0);

        if (! GridId)
        {
            GridId = &GridMap.Emplace(ID0, GridData.AddDefaulted());
            GridData[*GridId].Id = ID0;
        }

        GridData[*GridId].PointData.Emplace(i0);
        GridData[*GridId].PointData.Emplace(i1);
    }

    if (ID0 != ID1)
    {
        TArray<FIntPoint> CoveredGrids;

        GridWalk(CoveredGrids, P0, P1);

        for (const FIntPoint& CoveredId : CoveredGrids)
        {
            int32* GridId = GridMap.Find(CoveredId);

            if (! GridId)
            {
                GridId = &GridMap.Emplace(CoveredId, GridData.AddDefaulted());
                GridData[*GridId].Id = CoveredId;
            }

            GridData[*GridId].PointData.Emplace(i0);
            GridData[*GridId].PointData.Emplace(i1);
        }
    }
}

void UGULPolyGridObject::GenerateFromPolyPoints(const TArray<FVector2D>& InPoints, int32 InDimensionX, int32 InDimensionY)
{
    if (InDimensionX < 1 || InDimensionY < 1)
    {
        return;
    }

    DimensionX = InDimensionX;
    DimensionY = InDimensionY;

    int32 PointCount = InPoints.Num();

    if (PointCount < 2)
    {
        return;
    }

    GridMap.Reset();
    GridMap.Reserve(PointCount);

    GridData.Reset(PointCount);

    // Generate grid data from line segments
    for (int32 i0=0, i1=1; i1<PointCount; ++i1)
    {
        const FVector2D& P0(InPoints[i0]);
        const FVector2D& P1(InPoints[i1]);

        GenerateGridDataFromLineSegment(P0, P1, i0, i1);

        i0 = i1;
    }

    // Generate grid data from last line segment (last to first points)
    if (PointCount > 2)
    {
        int32 i0 = PointCount-1;
        int32 i1 = 0;

        const FVector2D& P0(InPoints[i0]);
        const FVector2D& P1(InPoints[i1]);

        if (! P0.Equals(P1))
        {
            GenerateGridDataFromLineSegment(P0, P1, i0, i1);
        }
    }

    GridMap.Shrink();
    GridData.Shrink();
}

void UGULPolyGridObject::GenerateFromPolys(const TArray<FGULVector2DGroup>& InPolys, int32 InDimensionX, int32 InDimensionY)
{
    if (InDimensionX < 1 || InDimensionY < 1)
    {
        return;
    }

    DimensionX = InDimensionX;
    DimensionY = InDimensionY;

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

    GridMap.Reset();
    GridMap.Reserve(TotalPointCount);

    GridData.Reset(TotalPointCount);

    for (int32 PolyIt=0; PolyIt<InPolys.Num(); ++PolyIt)
    {
        const TArray<FVector2D>& InPoints(InPolys[PolyIt].Points);
        const int32 PointCount = InPoints.Num();

        // Generate grid data from line segments
        for (int32 i0=0, i1=1; i1<PointCount; ++i1)
        {
            const FVector2D& P0(InPoints[i0]);
            const FVector2D& P1(InPoints[i1]);

            GenerateGridDataFromLineSegment(P0, P1, i0, i1);

            i0 = i1;
        }

        // Generate grid data from last line segment (last to first points)
        if (PointCount > 2)
        {
            int32 i0 = PointCount-1;
            int32 i1 = 0;

            const FVector2D& P0(InPoints[i0]);
            const FVector2D& P1(InPoints[i1]);

            if (! P0.Equals(P1))
            {
                GenerateGridDataFromLineSegment(P0, P1, i0, i1);
            }
        }
    }

    GridMap.Shrink();
    GridData.Shrink();
}

void UGULPolyGridObject::GenerateFromPolyPointIndices(const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices, int32 InDimensionX, int32 InDimensionY)
{
    if (InDimensionX < 1 || InDimensionY < 1)
    {
        return;
    }

    DimensionX = InDimensionX;
    DimensionY = InDimensionY;

    int32 PointCount = InPoints.Num();
    int32 IndexCount = InIndices.Num();

    if (PointCount < 2)
    {
        return;
    }

    GridMap.Reset();
    GridMap.Reserve(PointCount);

    GridData.Reset(PointCount);

    // Generate grid data from line segments
    for (int32 i0 : InIndices)
    {
        if (InPoints.IsValidIndex(i0))
        {
            int32 i1 = (i0+1) % PointCount;

            const FVector2D& P0(InPoints[i0]);
            const FVector2D& P1(InPoints[i1]);

            GenerateGridDataFromLineSegment(P0, P1, i0, i1);
        }
    }

    GridMap.Shrink();
    GridData.Shrink();
}

UGULBoundPolyGridObject* UGULPolyGridObject::GenerateBoundPolyGridObjectFromPointData(UObject* Outer, const TArray<FVector2D>& Points, int32 Index)
{
    UGULBoundPolyGridObject* GridObject = nullptr;

    if (IsValidGrid(Index))
    {
        const FGridData& SrcGridData(GridData[Index]);
        UObject* OuterObject = IsValid(Outer) ? Outer : this;
        UGULBoundPolyGridObject* NewGridObject = NewObject<UGULBoundPolyGridObject>(OuterObject);

        if (IsValid(NewGridObject))
        {
            NewGridObject->DimensionX = DimensionX;
            NewGridObject->DimensionY = DimensionY;

            bool bGenerateSuccess = NewGridObject->GeneratePolyFromPointIndices(SrcGridData.Id, Points, SrcGridData.PointData.Array());

            if (bGenerateSuccess)
            {
                GridObject = NewGridObject;
            }
        }
    }

    return GridObject;
}

void UGULPolyGridObject::GetPointSet(TArray<int32>& PointSet, int32 Index) const
{
    PointSet.Reset();

    if (IsValidGrid(Index))
    {
        PointSet = GridData[Index].PointData.Array();
    }
}

int32 UGULPolyGridObject::GatherGridDataByDimension(
    TArray<FIntPoint>& GroupIds,
    TArray<FGULIntPointGroup>& GridIdGroups,
    TArray<FGULIntGroup>& PointDataGroups,
    int32 GroupDimensionX,
    int32 GroupDimensionY
    )
{
    if (GroupDimensionX < 1 || GroupDimensionY < 1)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULPolyGridObject::GatherGridDataByDimension() ABORTED, INVALID DIMENSION"));
        return 0;
    }

    TMap<FIntPoint, FGridDataGroup> GroupMap;

    for (const auto& DataPair : GridMap)
    {
        const FIntPoint& GridId(DataPair.Get<0>());
        const TSet<int32>& PointData(GridData[DataPair.Get<1>()].PointData);

        FIntPoint GroupId;
        GroupId.X = (GridId.X / GroupDimensionX);
        GroupId.Y = (GridId.Y / GroupDimensionY);

        if (GridId.X < 0 && (GridId.X % GroupDimensionX) != 0)
        {
            GroupId.X -= 1;
        }

        if (GridId.Y < 0 && (GridId.Y % GroupDimensionY) != 0)
        {
            GroupId.Y -= 1;
        }

        FGridDataGroup& GroupData(GroupMap.FindOrAdd(GroupId));
        GroupData.PointData = GroupData.PointData.Union(PointData);
        GroupData.GridIds.Emplace(GridId);
    }

    const int32 GroupCount = GroupMap.Num();

    GroupIds.Reserve(GroupCount);
    GridIdGroups.Reserve(GroupCount);
    PointDataGroups.Reserve(GroupCount);

    for (auto& DataPair : GroupMap)
    {
        FIntPoint& GroupId(DataPair.Get<0>());
        FGridDataGroup& GroupData(DataPair.Get<1>());

        GroupIds.Emplace(GroupId);

        GridIdGroups.AddDefaulted();
        GridIdGroups.Last().Points = MoveTemp(GroupData.GridIds);

        PointDataGroups.AddDefaulted();
        PointDataGroups.Last().Values = GroupData.PointData.Array();
    }

    return GroupCount;
}
