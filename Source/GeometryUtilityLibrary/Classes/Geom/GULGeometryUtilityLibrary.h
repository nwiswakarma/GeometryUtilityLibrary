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
#include "GULGeometryUtilityLibrary.generated.h"

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULGeometryUtility : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    static void AlignBoxToBottom(TArray<FGULOrientedBox>& OutBoxes, TArray<FVector>& OutDeltas, const TArray<FGULOrientedBox>& InBoxes);

public:

    UFUNCTION(BlueprintCallable)
    static void TransformBox2DPoints(FGULBox2DPoints& OutPoints, const FTransform& Transform, const FGULBox2DPoints& InPoints);

    UFUNCTION(BlueprintCallable)
    static void TransformBoxVectors(FGULBoxVectors& OutVectors, const FTransform& Transform, const FGULBoxVectors& InVectors);

    UFUNCTION(BlueprintCallable)
    static void TransformBox(FGULOrientedBox& OutBox, const FGULOrientedBox& InBox, const FTransform& Transform);

    UFUNCTION(BlueprintCallable)
    static void AlignBox(TArray<FGULOrientedBox>& OutBoxes, TArray<FVector>& OutDeltas, const TArray<FGULOrientedBox>& InBoxes);

    UFUNCTION(BlueprintCallable)
    static void ConvertVector2DGroupToVectorGroup(FGULVectorGroup& OutVectorGroup, const FGULVector2DGroup& InVector2DGroup, float ZPosition = 0.f);

    UFUNCTION(BlueprintCallable)
    static void ConvertVector2DGroupsToVectorGroups(TArray<FGULVectorGroup>& OutVectorGroups, const TArray<FGULVector2DGroup>& InVector2DGroups, float ZPosition = 0.f);

    inline static bool SegmentIntersection2D(
        const FVector2D& SegmentA0,
        const FVector2D& SegmentA1,
        const FVector2D& SegmentB0,
        const FVector2D& SegmentB1,
        FVector2D& OutIntersectionPoint,
        float& OutT
        );

    FORCEINLINE_DEBUGGABLE static bool SegmentIntersection2D(
        const FVector2D& SegmentA0,
        const FVector2D& SegmentA1,
        const FVector2D& SegmentB0,
        const FVector2D& SegmentB1,
        FVector2D& OutIntersectionPoint
        );

    FORCEINLINE_DEBUGGABLE static bool SegmentIntersection2D(
        const FVector2D& SegmentA0,
        const FVector2D& SegmentA1,
        const FVector2D& SegmentB0,
        const FVector2D& SegmentB1
        );

    static void ShortestSegment2DBetweenSegment2DSafe(
        const FVector2D& A1,
        const FVector2D& B1,
        const FVector2D& A2,
        const FVector2D& B2,
        FVector2D& OutP1,
        FVector2D& OutP2
        );

    static void ShortestSegment2DBetweenSegment2D(
        const FVector2D& A1,
        const FVector2D& B1,
        const FVector2D& A2,
        const FVector2D& B2,
        FVector2D& OutP1,
        FVector2D& OutP2
        );

    FORCEINLINE static float PointDistToSegment2D(const FVector2D& Point, const FVector2D& StartPoint, const FVector2D& EndPoint);

    FORCEINLINE_DEBUGGABLE static bool IsPointOnBounds(const FVector2D& BoundsMin, const FVector2D& BoundsMax, const FVector2D& Point);
    FORCEINLINE_DEBUGGABLE static bool IsPointOnBounds(const FBox2D& Bounds, const FVector2D& Point);
};

// Inlined Functions

inline bool UGULGeometryUtility::SegmentIntersection2D(
    const FVector2D& SegmentA0,
    const FVector2D& SegmentA1,
    const FVector2D& SegmentB0,
    const FVector2D& SegmentB1,
    FVector2D& OutIntersectionPoint,
    float& OutT
    )
{
    const FVector2D VectorA = SegmentA1 - SegmentA0;
    const FVector2D VectorB = SegmentB1 - SegmentB0;

    const float S = (-VectorA.Y * (SegmentA0.X - SegmentB0.X) + VectorA.X * (SegmentA0.Y - SegmentB0.Y)) / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);
    const float T = ( VectorB.X * (SegmentA0.Y - SegmentB0.Y) - VectorB.Y * (SegmentA0.X - SegmentB0.X)) / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);

    const bool bIntersects = (S >= 0 && S <= 1 && T >= 0 && T <= 1);

    if (bIntersects)
    {
        OutIntersectionPoint.X = SegmentA0.X + (T * VectorA.X);
        OutIntersectionPoint.Y = SegmentA0.Y + (T * VectorA.Y);
        OutT = T;
    }

    return bIntersects;
}

FORCEINLINE_DEBUGGABLE bool UGULGeometryUtility::SegmentIntersection2D(
    const FVector2D& SegmentA0,
    const FVector2D& SegmentA1,
    const FVector2D& SegmentB0,
    const FVector2D& SegmentB1,
    FVector2D& OutIntersectionPoint
    )
{
    float OutT;
    return SegmentIntersection2D(
        SegmentA0,
        SegmentA1,
        SegmentB0,
        SegmentB1,
        OutIntersectionPoint,
        OutT
        );
}

FORCEINLINE_DEBUGGABLE bool UGULGeometryUtility::SegmentIntersection2D(
    const FVector2D& SegmentA0,
    const FVector2D& SegmentA1,
    const FVector2D& SegmentB0,
    const FVector2D& SegmentB1
    )
{
    FVector2D OutIntersectionPoint;
    float OutT;
    return SegmentIntersection2D(
        SegmentA0,
        SegmentA1,
        SegmentB0,
        SegmentB1,
        OutIntersectionPoint,
        OutT
        );
}

FORCEINLINE float UGULGeometryUtility::PointDistToSegment2D(const FVector2D& Point, const FVector2D& StartPoint, const FVector2D& EndPoint)
{
    const FVector2D ClosestPoint(FMath::ClosestPointOnSegment2D(Point, StartPoint, EndPoint));
    return (Point-ClosestPoint).Size();
}

FORCEINLINE_DEBUGGABLE bool UGULGeometryUtility::IsPointOnBounds(const FVector2D& BoundsMin, const FVector2D& BoundsMax, const FVector2D& Point)
{
    if (Point.X < BoundsMin.X || Point.Y < BoundsMin.Y ||
        Point.X > BoundsMax.Y || Point.Y > BoundsMax.Y)
    {
        return false;
    }
    return true;
}

FORCEINLINE_DEBUGGABLE bool UGULGeometryUtility::IsPointOnBounds(const FBox2D& Bounds, const FVector2D& Point)
{
    return IsPointOnBounds(Bounds.Min, Bounds.Max, Point);
}

inline void UGULGeometryUtility::ConvertVector2DGroupToVectorGroup(FGULVectorGroup& OutVectorGroup, const FGULVector2DGroup& InVector2DGroup, float ZPosition)
{
    const TArray<FVector2D>& Points(InVector2DGroup.Points);
    TArray<FVector>& Vectors(OutVectorGroup.Vectors);

    const int32 VectorOffset = Vectors.Num();

    Vectors.AddUninitialized(Points.Num());

    for (int32 pi=0; pi<Points.Num(); ++pi)
    {
        Vectors[VectorOffset+pi] = FVector(Points[pi], ZPosition);
    }
}

inline void UGULGeometryUtility::ConvertVector2DGroupsToVectorGroups(TArray<FGULVectorGroup>& OutVectorGroups, const TArray<FGULVector2DGroup>& InVector2DGroups, float ZPosition)
{
    const int32 GroupOffset = OutVectorGroups.Num();

    OutVectorGroups.AddDefaulted(InVector2DGroups.Num());

    for (int32 i=0; i<InVector2DGroups.Num(); ++i)
    {
        ConvertVector2DGroupToVectorGroup(
            OutVectorGroups[GroupOffset+i],
            InVector2DGroups[i],
            ZPosition
            );
    }
}
