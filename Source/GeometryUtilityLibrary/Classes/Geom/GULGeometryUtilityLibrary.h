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

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Convert Vector2D Array To Vector Array"))
    static void K2_ConvertVector2DArrayToVectorArray(TArray<FVector>& OutVectors, const TArray<FVector2D>& InVector2Ds, float ZPosition = 0.f);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Convert Vector2D Group To Vector Group"))
    static void K2_ConvertVector2DGroupToVectorGroup(FGULVectorGroup& OutVectorGroup, const FGULVector2DGroup& InVector2DGroup, float ZPosition = 0.f);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Convert Vector2D Groups To Vector Groups"))
    static void K2_ConvertVector2DGroupsToVectorGroups(TArray<FGULVectorGroup>& OutVectorGroups, const TArray<FGULVector2DGroup>& InVector2DGroups, float ZPosition = 0.f);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Points By Indices"))
    static void K2_GetPointsByIndices(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Index Range"))
    static void K2_GenerateIndexRange(TArray<int32>& OutIndices, int32 IndexStart = 0, int32 IndexCount = 1);

    inline static void ConvertVector2DArrayToVectorArray(TArray<FVector>& OutVectors, const TArray<FVector2D>& InVector2Ds, float ZPosition = 0.f);
    inline static void ConvertVector2DGroupToVectorGroup(FGULVectorGroup& OutVectorGroup, const FGULVector2DGroup& InVector2DGroup, float ZPosition = 0.f);
    inline static void ConvertVector2DGroupsToVectorGroups(TArray<FGULVectorGroup>& OutVectorGroups, const TArray<FGULVector2DGroup>& InVector2DGroups, float ZPosition = 0.f);

    inline static void GetPointsByIndices(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices);
    inline static void GetPointsByIndicesUnsafe(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices);

    inline static void GenerateIndexRange(TArray<int32>& OutIndices, int32 IndexStart = 0, int32 IndexCount = 1);

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

    FORCEINLINE_DEBUGGABLE static bool SegmentIntersection2DFast(
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

    FORCEINLINE_DEBUGGABLE static bool IsInsideBounds(
        const FVector2D& Point,
        const FBox2D& Bounds,
        EGULBox2DClip ClipCode
        );

    FORCEINLINE_DEBUGGABLE static FVector2D IntersectBounds(
        const FVector2D& Point0,
        const FVector2D& Point1,
        const FBox2D& Bounds,
        EGULBox2DClip ClipCode
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
    const FVector2D BA = SegmentA0 - SegmentB0;

    const float L = 1.f / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);
    const float S = (-VectorA.Y * BA.X + VectorA.X * BA.Y) * L;
    const float T = ( VectorB.X * BA.Y - VectorB.Y * BA.X) * L;

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

FORCEINLINE_DEBUGGABLE bool UGULGeometryUtility::SegmentIntersection2DFast(
    const FVector2D& SegmentA0,
    const FVector2D& SegmentA1,
    const FVector2D& SegmentB0,
    const FVector2D& SegmentB1
    )
{
    const FVector2D VectorA = SegmentA1 - SegmentA0;
    const FVector2D VectorB = SegmentB1 - SegmentB0;
    const FVector2D BA = SegmentA0 - SegmentB0;

    const float L = 1.f / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);
    const float S = (-VectorA.Y * BA.X + VectorA.X * BA.Y) * L;
    const float T = ( VectorB.X * BA.Y - VectorB.Y * BA.X) * L;

    return (S >= 0.f && S <= 1.f && T >= 0.f && T <= 1.f);
}

FORCEINLINE_DEBUGGABLE bool UGULGeometryUtility::IsInsideBounds(
    const FVector2D& Point,
    const FBox2D& Bounds,
    EGULBox2DClip ClipCode
    )
{
    switch (ClipCode)
    {
        case EGULBox2DClip::B2DCLIP_B: return (Point.Y < Bounds.Min.Y); break;
        case EGULBox2DClip::B2DCLIP_T: return (Point.Y > Bounds.Max.Y); break;
        case EGULBox2DClip::B2DCLIP_L: return (Point.X < Bounds.Min.X); break;
        case EGULBox2DClip::B2DCLIP_R: return (Point.X > Bounds.Max.X); break;
        default: return false; break;
    }
    return false;
}

FORCEINLINE_DEBUGGABLE FVector2D UGULGeometryUtility::IntersectBounds(
    const FVector2D& Point0,
    const FVector2D& Point1,
    const FBox2D& Bounds,
    EGULBox2DClip ClipCode
    )
{
    FVector2D P01 = Point1-Point0;

    switch (ClipCode)
    {
        case EGULBox2DClip::B2DCLIP_B:
            return FVector2D(
                Point0.X + P01.X * (Bounds.Min.Y - Point0.Y) / P01.Y,
                Bounds.Min.Y
                );
            break;

        case EGULBox2DClip::B2DCLIP_T:
            return FVector2D(
                Point0.X + P01.X * (Bounds.Max.Y - Point0.Y) / P01.Y,
                Bounds.Max.Y
                );
            break;

        case EGULBox2DClip::B2DCLIP_L:
            return FVector2D(
                Bounds.Min.X,
                Point0.Y + P01.Y * (Bounds.Min.X - Point0.X) / P01.X
                );
            break;

        case EGULBox2DClip::B2DCLIP_R:
            return FVector2D(
                Bounds.Max.X,
                Point0.Y + P01.Y * (Bounds.Max.X - Point0.X) / P01.X
                );
            break;

        default: return FVector2D(); break;
    }

    return FVector2D();
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

FORCEINLINE_DEBUGGABLE void UGULGeometryUtility::K2_ConvertVector2DArrayToVectorArray(TArray<FVector>& OutVectors, const TArray<FVector2D>& InVector2Ds, float ZPosition)
{
    OutVectors.Reset();
    ConvertVector2DArrayToVectorArray(OutVectors, InVector2Ds, ZPosition);
    OutVectors.Shrink();
}

FORCEINLINE_DEBUGGABLE void UGULGeometryUtility::K2_ConvertVector2DGroupToVectorGroup(FGULVectorGroup& OutVectorGroup, const FGULVector2DGroup& InVector2DGroup, float ZPosition)
{
    OutVectorGroup.Vectors.Reset();
    ConvertVector2DGroupToVectorGroup(OutVectorGroup, InVector2DGroup, ZPosition);
    OutVectorGroup.Vectors.Shrink();
}

FORCEINLINE_DEBUGGABLE void UGULGeometryUtility::K2_ConvertVector2DGroupsToVectorGroups(TArray<FGULVectorGroup>& OutVectorGroups, const TArray<FGULVector2DGroup>& InVector2DGroups, float ZPosition)
{
    OutVectorGroups.Reset();
    ConvertVector2DGroupsToVectorGroups(OutVectorGroups, InVector2DGroups, ZPosition);
    OutVectorGroups.Shrink();
}

FORCEINLINE_DEBUGGABLE void UGULGeometryUtility::K2_GetPointsByIndices(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices)
{
    OutPoints.Reset();
    GetPointsByIndices(OutPoints, InPoints, InIndices);
    OutPoints.Shrink();
}

FORCEINLINE_DEBUGGABLE void UGULGeometryUtility::K2_GenerateIndexRange(TArray<int32>& OutIndices, int32 IndexStart, int32 IndexCount)
{
    OutIndices.Reset();
    GenerateIndexRange(OutIndices, IndexStart, IndexCount);
    OutIndices.Shrink();
}

inline void UGULGeometryUtility::ConvertVector2DArrayToVectorArray(TArray<FVector>& OutVectors, const TArray<FVector2D>& InVector2Ds, float ZPosition)
{
    const int32 VectorOffset = OutVectors.Num();

    OutVectors.AddUninitialized(InVector2Ds.Num());

    for (int32 pi=0; pi<InVector2Ds.Num(); ++pi)
    {
        OutVectors[VectorOffset+pi] = FVector(InVector2Ds[pi], ZPosition);
    }
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

inline void UGULGeometryUtility::GetPointsByIndices(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices)
{
    OutPoints.Reserve(OutPoints.Num()+InIndices.Num());

    for (int32 i : InIndices)
    {
        if (InPoints.IsValidIndex(i))
        {
            OutPoints.Emplace(InPoints[i]);
        }
    }
}

inline void UGULGeometryUtility::GetPointsByIndicesUnsafe(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const TArray<int32>& InIndices)
{
    OutPoints.Reserve(OutPoints.Num()+InIndices.Num());

    for (int32 i : InIndices)
    {
        OutPoints.Emplace(InPoints[i]);
    }
}

inline void UGULGeometryUtility::GenerateIndexRange(TArray<int32>& OutIndices, int32 IndexStart, int32 IndexCount)
{
    if (IndexCount > 0)
    {
        const int32 OffsetIndex = OutIndices.Num();

        OutIndices.AddUninitialized(IndexCount);

        for (int32 i=0; i<IndexCount; ++i)
        {
            OutIndices[OffsetIndex+i] = IndexStart+i;
        }
    }
}
