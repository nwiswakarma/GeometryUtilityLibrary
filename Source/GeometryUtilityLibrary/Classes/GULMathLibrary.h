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
#include "GULMathLibrary.generated.h"

// Float/Integer conversion constants

#define FGULCONST_PRECISION8_SCALE     100000000.f
#define FGULCONST_PRECISION8_SCALE_INV 0.00000001f

#define FGULCONST_PRECISION4_SCALE     10000.f
#define FGULCONST_PRECISION4_SCALE_INV 0.0001f

#define FGULCONST_PRECISION3_SCALE     1000.f
#define FGULCONST_PRECISION3_SCALE_INV 0.001f

#define FGULCONST_PRECISION2_SCALE     100.f
#define FGULCONST_PRECISION2_SCALE_INV 0.01f

#define FGULCONST_PRECISION_SCALE     FGULCONST_PRECISION3_SCALE
#define FGULCONST_PRECISION_SCALE_INV FGULCONST_PRECISION3_SCALE_INV

#define FGULCONST_INT8_SCALE 100000000
#define FGULCONST_INT4_SCALE 10000.f
#define FGULCONST_INT3_SCALE 1000.f
#define FGULCONST_INT2_SCALE 100.f

#define FGULCONST_INT_SCALE FGULCONST_INT3_SCALE

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULMathLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    // Type Scale

    FORCEINLINE static int32 ScaleInt(int32 v, int32 Scale = FGULCONST_INT_SCALE)
    {
        return v * FGULCONST_INT_SCALE;
    }

    FORCEINLINE static FIntPoint ScaleInt(const FIntPoint& Point, int32 Scale = FGULCONST_INT_SCALE)
    {
        return Point * FGULCONST_INT_SCALE;
    }

    // 1D Conversions

    FORCEINLINE static int32 ScaleToInt(float v, float Scale = FGULCONST_PRECISION_SCALE)
    {
        return FMath::RoundToInt(v*Scale);
    }

    FORCEINLINE static float ScaleToFloat(int32 v, float Scale = FGULCONST_PRECISION_SCALE_INV)
    {
        return static_cast<float>(v) * Scale;
    }

    // 2D Conversions (float -> integer)

    FORCEINLINE static FIntPoint ScaleToIntPoint(float X, float Y, float Scale = FGULCONST_PRECISION_SCALE)
    {
        return FIntPoint(ScaleToInt(X, Scale), ScaleToInt(Y, Scale));
    }

    FORCEINLINE static FIntPoint ScaleToIntPoint(const FVector2D& Point, float Scale = FGULCONST_PRECISION_SCALE)
    {
        return ScaleToIntPoint(Point.X, Point.Y);
    }

    FORCEINLINE static void ScaleToIntPoint(TArray<FIntPoint>& Points, const TArray<FVector2D>& Vectors, float Scale = FGULCONST_PRECISION_SCALE)
    {
        Points.SetNumUninitialized(Vectors.Num());

        for (int32 i=0; i<Vectors.Num(); ++i)
        {
            Points[i] = ScaleToIntPoint(Vectors[i], Scale);
        }
    }

    // 2D Conversions (integer -> float)

    FORCEINLINE static FVector2D ScaleToVector2D(int32 X, int32 Y, float Scale = FGULCONST_PRECISION_SCALE_INV)
    {
        return FVector2D(ScaleToFloat(X, Scale), ScaleToFloat(Y, Scale));
    }

    FORCEINLINE static FVector2D ScaleToVector2D(const FIntPoint& Point, float Scale = FGULCONST_PRECISION_SCALE_INV)
    {
        return ScaleToVector2D(Point.X, Point.Y, Scale);
    }

    FORCEINLINE static void ScaleToVector2D(TArray<FVector2D>& Vectors, const TArray<FIntPoint>& Points, float Scale = FGULCONST_PRECISION_SCALE_INV)
    {
        Vectors.SetNumUninitialized(Points.Num());

        for (int32 i=0; i<Points.Num(); ++i)
        {
            Vectors[i] = ScaleToVector2D(Points[i], Scale);
        }
    }

    // Precision scaling (default precision scale)

    FORCEINLINE static float ScalePrecision(float f)
    {
        return ScaleToFloat(ScaleToInt(f, FGULCONST_PRECISION_SCALE), FGULCONST_PRECISION_SCALE_INV);
    }

    FORCEINLINE static FVector2D ScalePrecision(const FVector2D& Point)
    {
        return FVector2D(ScalePrecision(Point.X), ScalePrecision(Point.Y));
    }

    // Hash Utility

    FORCEINLINE static uint32 GetHash(const FIntPoint& Point)
    {
        int64 X = Point.X;
        int64 Y = Point.Y;
        return static_cast<int32>((0x65d200ce55b19ad8L * X + 0x4f2162926e40c299L * Y + 0x162dd799029970f8L) >> 32);
    }

    FORCEINLINE static uint32 GetHash(const FVector2D& Point)
    {
        return GetHash(ScaleToIntPoint(Point));
    }
};
