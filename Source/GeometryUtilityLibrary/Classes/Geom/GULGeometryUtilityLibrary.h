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
#include "Geom/GULGeometryInstanceTypes.h"
#include "GULGeometryUtilityLibrary.generated.h"

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULGeometryTileSplatterParameters
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Offset;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Dimension;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Scale = 1.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float ScaleRandom = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 InstanceCountX = 3;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 InstanceCountXMax = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 InstanceCountY = 3;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 InstanceCountYMax = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float InstanceMask = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float PositionRandom = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Angle = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float AngleRandom = 0.f;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULGeometryRadialSplatterParameters
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Offset;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 InstanceCount = 3;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 InstanceCountMax = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Radius = 0.5f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float RadiusRandom = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Spread = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float RingAngle = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float RingAngleLimit = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float RingAngleRandom = 0.f;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULGeometryTransformParameters
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Size = FVector2D::UnitVector;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D SizeRandom = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Scale = 1.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float ScaleRandom = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Angle = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float AngleRandom = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Value = 1.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float ValueRandom = 0.f;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULGeometrySplatterInstance
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Position = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Size = FVector2D::UnitVector;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Scale = 1.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Angle = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Value = 1.f;

    FGULGeometrySplatterInstance() = default;

    FGULGeometrySplatterInstance(
        FVector2D InPosition,
        FVector2D InSize,
        float InScale,
        float InAngle,
        float InValue
        )
        : Position(InPosition)
        , Size(InSize)
        , Scale(InScale)
        , Angle(InAngle)
        , Value(InValue)
    {
    }
};

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULGeometryUtility : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    static void GenerateTileSplatter(
        FRandomStream& Rand,
        const FGULGeometryTileSplatterParameters& TileConfig,
        const FGULGeometryTransformParameters& GeometryTransform,
        TArray<FGULGeometrySplatterInstance>& GeometryInstances
        );

    static void GenerateRadialSplatter(
        FRandomStream& Rand,
        const FGULGeometryRadialSplatterParameters& RadialConfig,
        const FGULGeometryTransformParameters& GeometryTransform,
        TArray<FGULGeometrySplatterInstance>& GeometryInstances
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Tile Splatter", AutoCreateRefTerm="TileConfig,GeometryTransform"))
    static void K2_GenerateTileSplatter(
        int32 Seed,
        const FGULGeometryTileSplatterParameters& TileConfig,
        const FGULGeometryTransformParameters& GeometryTransform,
        TArray<FGULGeometrySplatterInstance>& GeometryInstances
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Radial Splatter", AutoCreateRefTerm="RadialConfig,GeometryTransform"))
    static void K2_GenerateRadialSplatter(
        int32 Seed,
        const FGULGeometryRadialSplatterParameters& RadialConfig,
        const FGULGeometryTransformParameters& GeometryTransform,
        TArray<FGULGeometrySplatterInstance>& GeometryInstances
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Radial Splatter (Poly)", AutoCreateRefTerm="RadialConfig,GeometryTransform"))
    static void GenerateRadialSplatterPoly(
        int32 Seed,
        int32 Sides,
        int32 SidesMax,
        const FGULGeometryRadialSplatterParameters& RadialConfig,
        const FGULGeometryTransformParameters& GeometryTransform,
        TArray<FGULPolyGeometryInstance>& Polys
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Radial Splatter (Quad)", AutoCreateRefTerm="RadialConfig,GeometryTransform"))
    static void GenerateRadialSplatterQuad(
        int32 Seed,
        const FGULGeometryRadialSplatterParameters& RadialConfig,
        const FGULGeometryTransformParameters& GeometryTransform,
        TArray<FGULQuadGeometryInstance>& Quads
        );

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
