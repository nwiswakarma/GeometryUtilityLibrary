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

//inline bool UGULGeometryUtility::ShortestSegment2DBetweenSegment2D(
//    const FVector2D& SegmentA0,
//    const FVector2D& SegmentA1,
//    const FVector2D& SegmentB0,
//    const FVector2D& SegmentB1,
//    FVector2D& ClosestA,
//    FVector2D& ClosestB
//    )
//{
//   FVector2D p13,p43,p21;
//
//   //p13 = p1 - p3;
//   //p43 = p4 - p3;
//   p13 = SegmentA0 - SegmentB0;
//   p43 = SegmentB1 - SegmentB0;
//
//   //if (ABS(p43.x) < EPS && ABS(p43.y) < EPS)
//   if (p43.IsNearlyZero())
//      return false;
//
//   //p21 = p2 - p1;
//   p21 = SegmentA1 - SegmentA0;
//
//   //if (ABS(p21.x) < EPS && ABS(p21.y) < EPS)
//   if (p21.IsNearlyZero())
//      return false;
//
//   float d1343 = p13.X * p43.X + p13.Y * p43.Y;
//   float d4321 = p43.X * p21.X + p43.Y * p21.Y;
//   float d1321 = p13.X * p21.X + p13.Y * p21.Y;
//   float d4343 = p43.X * p43.X + p43.Y * p43.Y;
//   float d2121 = p21.X * p21.X + p21.Y * p21.Y;
//
//   float denom = d2121 * d4343 - d4321 * d4321;
//   //if (ABS(denom) < EPS)
//   if (FMath::Abs(denom) < KINDA_SMALL_NUMBER)
//      return false;
//   float numer = d1343 * d4321 - d1321 * d4343;
//
//   //*mua = numer / denom;
//   //*mub = (d1343 + d4321 * (*mua)) / d4343;
//   float mua = numer / denom;
//   float mub = (d1343 + d4321 * mua) / d4343;
//
//   //pa->x = p1.x + *mua * p21.x;
//   //pa->y = p1.y + *mua * p21.y;
//   //pb->x = p3.x + *mub * p43.x;
//   //pb->y = p3.y + *mub * p43.y;
//   ClosestA = SegmentA0 + mua * p21;
//   ClosestB = SegmentB0 + mub * p43;
//
//   return true;
//}

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
