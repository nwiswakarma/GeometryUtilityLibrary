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
#include "GULPolyGridObject.generated.h"

class UGULBoundPolyGridObject;

UCLASS(BlueprintType)
class GEOMETRYUTILITYLIBRARY_API UGULPolyGridObject : public UObject
{
    GENERATED_BODY()

    struct FGridData
    {
        FIntPoint Id;
        TSet<int32> PointData;
    };

    TMap<FIntPoint, int32> GridMap;
    TArray<FGridData> GridData;

    FORCEINLINE FIntPoint GetGridId(const FVector2D& Point);
    void GridWalk(TArray<FIntPoint>& GridIds, const FVector2D& P0, const FVector2D& P1);
    void GenerateGridDataFromLineSegment(const FVector2D& P0, const FVector2D P1, int32 i0, int32 i1);

public:

    UPROPERTY(BlueprintReadOnly)
    int32 DimensionX;

    UPROPERTY(BlueprintReadOnly)
    int32 DimensionY;

    UFUNCTION(BlueprintCallable)
    void GenerateFromPolyPoints(const TArray<FVector2D>& InPoints, int32 InDimensionX, int32 InDimensionY);

    UFUNCTION(BlueprintCallable)
    void GenerateFromPolyPointIndices(const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices, int32 InDimensionX, int32 InDimensionY);

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE bool IsValidGrid(int32 Index) const;

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE int32 GetGridCount() const;

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE FIntPoint GetGridId(int32 Index) const;

    UFUNCTION(BlueprintCallable)
    FORCEINLINE_DEBUGGABLE int32 GetGridIndex(const FIntPoint& GridId) const;

    UFUNCTION(BlueprintCallable)
    void GetPointData(TArray<int32>& OutPointIndices, int32 Index);

    UFUNCTION(BlueprintCallable)
    UGULBoundPolyGridObject* GenerateBoundPolyGridObjectFromPointData(UObject* Outer, const TArray<FVector2D>& Points, int32 Index);

    //UFUNCTION(BlueprintCallable)
    //void GridFillByPoint(TArray<FIntPoint>& OutPoints, const FIntPoint& InPoint);
};

FORCEINLINE FIntPoint UGULPolyGridObject::GetGridId(const FVector2D& Point)
{
    return FIntPoint(
        FMath::FloorToInt(Point.X / DimensionX),
        FMath::FloorToInt(Point.Y / DimensionY)
        );
}

FORCEINLINE_DEBUGGABLE bool UGULPolyGridObject::IsValidGrid(int32 Index) const
{
    return GridData.IsValidIndex(Index);
}

FORCEINLINE_DEBUGGABLE int32 UGULPolyGridObject::GetGridCount() const
{
    return GridData.Num();
}

FORCEINLINE_DEBUGGABLE FIntPoint UGULPolyGridObject::GetGridId(int32 Index) const
{
    return Index < GetGridCount() ? GridData[Index].Id : FIntPoint::NoneValue;
}

FORCEINLINE_DEBUGGABLE int32 UGULPolyGridObject::GetGridIndex(const FIntPoint& GridId) const
{
    const int32* IndexPtr = GridMap.Find(GridId);
    return IndexPtr ? *IndexPtr : INDEX_NONE;
}
