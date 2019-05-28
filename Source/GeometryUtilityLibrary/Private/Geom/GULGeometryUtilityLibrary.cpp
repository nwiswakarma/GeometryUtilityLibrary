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

#include "Geom/GULGeometryUtilityLibrary.h"

void UGULGeometryUtility::GenerateRadialSplatter(
    FRandomStream& Rand,
    const FGULGeometryRadialSplatterParameters& RadialConfig,
    const FGULGeometryTransformParameters& GeometryTransform,
    TArray<FGULGeometrySplatterInstance>& GeometryInstances
    )
{
    int32 InstanceCount = FMath::Max(RadialConfig.InstanceCount, 1);
    int32 InstanceCountMax = RadialConfig.InstanceCountMax;

    if (InstanceCount < InstanceCountMax)
    {
        InstanceCount = Rand.RandRange(InstanceCount, InstanceCountMax);
    }

    const float UnitAngle = (2.f * PI) / InstanceCount;

    // Ring angle random (calculate once)
    float RingAngleRandom = PI*RadialConfig.RingAngleRandom;
    RingAngleRandom *= Rand.GetFraction() * 2.f - 1.f;

    // Reserve geometry instance container space
    GeometryInstances.Reserve(InstanceCount);

    for (int32 i=0; i<InstanceCount; ++i)
    {
        // Radial Spread

        float RadialSpread = RadialConfig.Spread;
        RadialSpread *= .5f * UnitAngle * (Rand.GetFraction()*2.f-1.f);

        // Ring Angle

        float RingAngle = PI*RadialConfig.RingAngle;

        RingAngle += i*UnitAngle + RingAngleRandom;
        RingAngle += RadialSpread;

        // Ring Radius

        float RadiusRandom = RadialConfig.RadiusRandom;
        float Radius = RadialConfig.Radius;

        RadiusRandom *= Radius * Rand.GetFraction();
        Radius -= RadiusRandom;

        // Geom Offset

        FVector2D RadiusOffset(ForceInitToZero);
        RadiusOffset.X = FMath::Cos(RingAngle);
        RadiusOffset.Y = FMath::Sin(RingAngle);
        RadiusOffset *= Radius;

        FVector2D Offset = RadialConfig.Offset + RadiusOffset;

        // Geom Angle

        float AngleRandom = PI*GeometryTransform.AngleRandom;
        float Angle = PI*GeometryTransform.Angle;

        AngleRandom *= Rand.GetFraction() * 2.f - 1.f;
        Angle += RingAngle + AngleRandom;

        // Geom Size

        FVector2D SizeRandom = GeometryTransform.SizeRandom;
        FVector2D Size = GeometryTransform.Size;

        SizeRandom.X *= Rand.GetFraction();
        SizeRandom.Y *= Rand.GetFraction();
        SizeRandom *= Size;

        Size -= SizeRandom;

        // Geom Scale

        float ScaleRandom = GeometryTransform.ScaleRandom;
        float Scale = GeometryTransform.Scale;

        ScaleRandom *= Scale * Rand.GetFraction();
        Scale -= ScaleRandom;

        // Geom Value

        float ValueRandom = GeometryTransform.ValueRandom;
        float Value = GeometryTransform.Value;

        ValueRandom *= Value * Rand.GetFraction();
        Value -= ValueRandom;

        GeometryInstances.Emplace(
            Offset,
            Size,
            Scale,
            Angle,
            Value
            );
    }
}

void UGULGeometryUtility::K2_GenerateRadialSplatter(
    int32 Seed,
    const FGULGeometryRadialSplatterParameters& RadialConfig,
    const FGULGeometryTransformParameters& GeometryTransform,
    TArray<FGULGeometrySplatterInstance>& GeometryInstances
    )
{
    FRandomStream Rand(Seed);
    GenerateRadialSplatter(
        Rand,
        RadialConfig,
        GeometryTransform,
        GeometryInstances
        );
}

void UGULGeometryUtility::GenerateRadialSplatterPoly(
    int32 Seed,
    int32 Sides,
    int32 SidesMax,
    const FGULGeometryRadialSplatterParameters& RadialConfig,
    const FGULGeometryTransformParameters& GeometryTransform,
    TArray<FGULPolyGeometryInstance>& Polys
    )
{
    FRandomStream Rand(Seed);

    TArray<FGULGeometrySplatterInstance> GeometryInstances;
    GenerateRadialSplatter(
        Rand,
        RadialConfig,
        GeometryTransform,
        GeometryInstances
        );

    Sides = FMath::Max(3, Sides);

    Polys.SetNumUninitialized(GeometryInstances.Num());

    for (int32 i=0; i<Polys.Num(); ++i)
    {
        const FGULGeometrySplatterInstance& Geom(GeometryInstances[i]);
        FGULPolyGeometryInstance Poly;

        Poly.Origin = Geom.Position;
        Poly.Size = Geom.Size;
        Poly.Scale = Geom.Scale;
        Poly.Angle = Geom.Angle;
        Poly.Value = Geom.Value;

        Poly.Sides = (Sides < SidesMax)
            ? Rand.RandRange(Sides, SidesMax)
            : Sides;

        Polys[i] = Poly;
    }
}

void UGULGeometryUtility::GenerateRadialSplatterQuad(
    int32 Seed,
    const FGULGeometryRadialSplatterParameters& RadialConfig,
    const FGULGeometryTransformParameters& GeometryTransform,
    TArray<FGULQuadGeometryInstance>& Quads
    )
{
    FRandomStream Rand(Seed);

    TArray<FGULGeometrySplatterInstance> GeometryInstances;
    GenerateRadialSplatter(
        Rand,
        RadialConfig,
        GeometryTransform,
        GeometryInstances
        );

    Quads.SetNumUninitialized(GeometryInstances.Num());

    for (int32 i=0; i<Quads.Num(); ++i)
    {
        const FGULGeometrySplatterInstance& Geom(GeometryInstances[i]);
        FGULQuadGeometryInstance Quad;

        Quad.Origin = Geom.Position;
        Quad.Size = Geom.Size;
        Quad.Scale = Geom.Scale;
        Quad.Angle = Geom.Angle;
        Quad.Value = Geom.Value;

        Quads[i] = Quad;
    }
}

bool UGULGeometryUtility::SegmentIntersection2D(
    const FVector2D& SegmentA0,
    const FVector2D& SegmentA1,
    const FVector2D& SegmentB0,
    const FVector2D& SegmentB1,
    FVector2D& OutIntersectionPoint
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
    }

    return bIntersects;
}
