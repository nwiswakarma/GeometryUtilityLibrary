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
#include "GULTypes.h"
#include "GULGridMapObject.generated.h"

enum class EGULGridMapVoxelType : uint8
{
    GMVT_SOLID = 0,
    GMVT_MASKED = 1
};

enum class EGULGridMapMaterialType : uint8
{
    GMMT_SINGLE = 0,
    GMMT_DOUBLE = 1,
    GMMT_TRIPLE = 2
};

enum class EGULGridVoxelType : uint8
{
    GVT_BLANK = 0,
    GVT_SOLID = 1,
    GVT_POLY = 2
};

UCLASS(BlueprintType)
class GEOMETRYUTILITYLIBRARY_API UGULGridMapObject : public UObject
{
    GENERATED_BODY()

    struct FGrid
    {
        UGULGridMapObject* const Map;
        const FIntPoint GridId;
        const FIntPoint BlockOrigin;
        const FIntPoint VoxelOrigin;
        const int32 Dimension;
        const int32 UnitSize;
        EGULGridMapVoxelType GridType;
        EGULGridMapMaterialType MaterialType;

        TArray<EGULGridVoxelType> Voxels;
        TArray<FColor> Material;
        TArray<FGULVector2DGroup> PolyGroups;

        FGrid(UGULGridMapObject& InMap, FIntPoint InGridId);
        ~FGrid();

        FORCEINLINE bool IsPointOnGrid(const FIntPoint& Point) const
        {
            return
                Point.X >= BlockOrigin.X &&
                Point.Y >= BlockOrigin.Y &&
                Point.X < BlockOrigin.X+Dimension &&
                Point.Y < BlockOrigin.Y+Dimension;
        }

        FORCEINLINE int32 GetVoxelCount() const
        {
            return Dimension*Dimension;
        }

        FORCEINLINE int32 GetIndex(const FIntPoint& Point) const
        {
            return (Point.X-BlockOrigin.X) + (Point.Y-BlockOrigin.Y)*Dimension;
        }

        FORCEINLINE bool HasValidVoxels() const
        {
            return Voxels.Num() == GetVoxelCount();
        }

        void SetGridType(EGULGridMapVoxelType InGridType);
        void SetVoxelTypes(const TArray<FIntPoint>& Points, EGULGridVoxelType InVoxelType);
        void AddPolyGroups(const TArray<FGULVector2DGroup>& InPolyGroups);
        void FillBlankVoxels();
    };

    TArray<FGrid*> Grids;

    FIntPoint Origin;
    FIntPoint GridCount;
    int32 GridDimension;
    int32 UnitSize;
    EGULGridMapMaterialType MaterialType;

    FORCEINLINE int32 GetGridIndex(FIntPoint Id) const;

public:

    UGULGridMapObject();

    UFUNCTION(BlueprintCallable)
    void GenerateFromPoints(const TArray<FIntPoint>& InPoints, int32 InGridDimension = 10, int32 InUnitSize = 1);

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE FIntPoint GetGridCountXY() const;

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE int32 GetGridCount() const;

    void ResetGrids();
    FORCEINLINE bool IsValidGridId(const FIntPoint& Id) const;
    FORCEINLINE int32 GetGridDimension() const;
    FORCEINLINE int32 GetUnitSize() const;
    FORCEINLINE FIntPoint GetBlockOrigin(const FIntPoint& Id) const;
    FORCEINLINE FIntPoint GetVoxelOrigin(const FIntPoint& Id) const;

    virtual void BeginDestroy() override;
};

FORCEINLINE_DEBUGGABLE int32 UGULGridMapObject::GetGridIndex(FIntPoint Id) const
{
    check(GridCount.X > 0);
    return (Id.X-Origin.X) + (Id.Y-Origin.Y)*GridCount.X;
}

FORCEINLINE_DEBUGGABLE FIntPoint UGULGridMapObject::GetGridCountXY() const
{
    return FIntPoint(GridCount.X, GridCount.Y);
}

FORCEINLINE_DEBUGGABLE int32 UGULGridMapObject::GetGridCount() const
{
    return GridCount.X + GridCount.Y*GridCount.X;
}

FORCEINLINE bool UGULGridMapObject::IsValidGridId(const FIntPoint& Id) const
{
    return Id.X >= Origin.X
        && Id.Y >= Origin.Y
        && Id.X < (Origin.X+GridCount.X)
        && Id.Y < (Origin.Y+GridCount.Y);
}

FORCEINLINE int32 UGULGridMapObject::GetGridDimension() const
{
    return GridDimension;
}

FORCEINLINE int32 UGULGridMapObject::GetUnitSize() const
{
    return UnitSize;
}

FORCEINLINE FIntPoint UGULGridMapObject::GetBlockOrigin(const FIntPoint& Id) const
{
    return FIntPoint(Id.X*GridDimension, Id.Y*GridDimension);
}

FORCEINLINE FIntPoint UGULGridMapObject::GetVoxelOrigin(const FIntPoint& Id) const
{
    return FIntPoint(Id.X*GridDimension*UnitSize, Id.Y*GridDimension*UnitSize);
}
