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
#include "GULGeometrySplatterTypes.generated.h"

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
