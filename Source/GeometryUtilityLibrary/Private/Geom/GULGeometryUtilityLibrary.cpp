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
    int32 Seed,
    const FGULGeometryRadialSplatterParameters& RadialConfig,
    const FGULGeometryTransformParameters& GeometryTransform,
    TArray<FGULGeometrySplatterInstance>& GeometryInstances
    )
{
    FRandomStream Rand(Seed);

    const int32 InstanceCount = FMath::Max(RadialConfig.InstanceCount, 3);
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
