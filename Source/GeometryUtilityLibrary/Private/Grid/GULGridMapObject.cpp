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

#include "Grid/GULGridMapObject.h"

UGULGridMapObject::FGrid::FGrid(UGULGridMapObject& InMap, FIntPoint InGridId)
    : Map(&InMap)
    , GridId(InGridId)
    , BlockOrigin(InMap.GetBlockOrigin(InGridId))
    , VoxelOrigin(InMap.GetVoxelOrigin(InGridId))
    , Dimension(InMap.GetGridDimension())
    , UnitSize(InMap.GetUnitSize())
    , GridType(EGULGridMapVoxelType::GMVT_SOLID)
{
}

UGULGridMapObject::FGrid::~FGrid()
{
}

void UGULGridMapObject::FGrid::SetGridType(EGULGridMapVoxelType InGridType)
{
    GridType = InGridType;
}

void UGULGridMapObject::FGrid::SetVoxelTypes(const TArray<FIntPoint>& Points, EGULGridVoxelType InVoxelType)
{
    if (! HasValidVoxels())
    {
        FillBlankVoxels();
    }

    for (const FIntPoint& Point : Points)
    {
        if (IsPointOnGrid(Point))
        {
            Voxels[GetIndex(Point)] = InVoxelType;
        }
    }
}

void UGULGridMapObject::FGrid::AddPolyGroups(const TArray<FGULVector2DGroup>& InPolyGroups)
{
    PolyGroups.Append(InPolyGroups);
}

void UGULGridMapObject::FGrid::FillBlankVoxels()
{
    Voxels.Reset(GetVoxelCount());
    Voxels.SetNumZeroed(GetVoxelCount());
}

UGULGridMapObject::UGULGridMapObject()
    : MaterialType(EGULGridMapMaterialType::GMMT_SINGLE)
{
}

void UGULGridMapObject::BeginDestroy()
{
	Super::BeginDestroy();

    ResetGrids();
}

void UGULGridMapObject::ResetGrids()
{
    // Delete existing grids
    for (FGrid* Grid : Grids)
    {
        if (Grid)
        {
            delete Grid;
        }
    }

    // Reset grid properties
    Grids.Empty();
    Origin = FIntPoint::ZeroValue;
    GridCount = FIntPoint::ZeroValue;
    GridDimension = 0;
}

void UGULGridMapObject::GenerateFromPoints(const TArray<FIntPoint>& InPoints, int32 InGridDimension, int32 InUnitSize)
{
    const int32 PointCount = InPoints.Num();

    if (PointCount < 0)
    {
        return;
    }

    if (InGridDimension < 1 || InUnitSize < 1)
    {
        return;
    }

    ResetGrids();

    // Find origin and grid count

    FBox2D Bounds(ForceInitToZero);

    for (const FIntPoint& Point : InPoints)
    {
        Bounds += FVector2D(Point.X, Point.Y);
    }

    FIntPoint BoundsMin;
    BoundsMin.X = FMath::RoundToInt(Bounds.Min.X);
    BoundsMin.Y = FMath::RoundToInt(Bounds.Min.Y);

    FIntPoint BoundsMax;
    BoundsMax.X = FMath::RoundToInt(Bounds.Max.X);
    BoundsMax.Y = FMath::RoundToInt(Bounds.Max.Y);

    Origin = BoundsMin;

    GridCount.X = (BoundsMax.X-BoundsMin.X)+1;
    GridCount.Y = (BoundsMax.Y-BoundsMin.Y)+1;

    GridDimension = InGridDimension;
    UnitSize = InUnitSize;

    UE_LOG(LogTemp,Warning, TEXT("Origin: %s, GridCount: %s, GridDimension: %d, UnitSize: %d"),
        *Origin.ToString(),
        *GridCount.ToString(),
        GridDimension,
        UnitSize
        );

    // Generate grids

    Grids.SetNumZeroed(GetGridCount());

    for (const FIntPoint& Point : InPoints)
    {
        int32 Index = GetGridIndex(Point);
        Grids[Index] = new FGrid(*this, Point);
    }
}
