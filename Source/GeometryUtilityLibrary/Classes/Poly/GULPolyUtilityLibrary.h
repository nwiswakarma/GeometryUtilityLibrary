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
#include "GULPolyUtilityLibrary.generated.h"

// Float/Integer conversion constants, copied over from ajclipperplugin

#define FGULCONST_INT8_SCALE     100000000.f
#define FGULCONST_INT8_SCALE_INV 0.00000001f

#define FGULCONST_INT4_SCALE     10000.f
#define FGULCONST_INT4_SCALE_INV 0.0001f

#define FGULCONST_INT3_SCALE     1000.f
#define FGULCONST_INT3_SCALE_INV 0.001f

#define FGULCONST_INT2_SCALE     100.f
#define FGULCONST_INT2_SCALE_INV 0.01f

#define FGULCONST_INT_SCALE     FGULCONST_INT3_SCALE
#define FGULCONST_INT_SCALE_INV FGULCONST_INT3_SCALE_INV

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULPolyUtilityLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    FORCEINLINE static int32 ScaleToInt32(float v)
    {
        return v < 0.f
            ? static_cast<int32>(v*FGULCONST_INT_SCALE - .5f)
            : static_cast<int32>(v*FGULCONST_INT_SCALE + .5f);
    }

    FORCEINLINE static float ScaleToFloat(int32 v)
    {
        return static_cast<float>(v) * FGULCONST_INT_SCALE_INV;
    }

    FORCEINLINE static FIntPoint ScaleToIntPoint(float X, float Y)
    {
        return FIntPoint(ScaleToInt32(X), ScaleToInt32(Y));
    }

    FORCEINLINE static FIntPoint ScaleToIntPoint(const FVector2D& v)
    {
        return ScaleToIntPoint(v.X, v.Y);
    }

    FORCEINLINE static void ScaleToIntPoint(const TArray<FVector2D>& Vectors, TArray<FIntPoint>& Points)
    {
        Points.Reset(Vectors.Num());
        for (const FVector2D& Vector : Vectors)
        {
            Points.Emplace(ScaleToIntPoint(Vector));
        }
    }

    FORCEINLINE static FVector2D ScaleToVector2D(int32 X, int32 Y)
    {
        return FVector2D(ScaleToFloat(X), ScaleToFloat(Y));
    }

    FORCEINLINE static FVector2D ScaleToVector2D(const FIntPoint& pt)
    {
        return ScaleToVector2D(pt.X, pt.Y);
    }

    FORCEINLINE static void ScaleToVector2D(const TArray<FIntPoint>& Points, TArray<FVector2D>& Vectors)
    {
        Vectors.Reset(Points.Num());
        for (const FIntPoint& Point : Points)
        {
            Vectors.Emplace(ScaleToFloat(Point.X), ScaleToFloat(Point.Y));
        }
    }

public:

    UFUNCTION(BlueprintCallable)
    static float GetArea(const TArray<FVector2D>& Points);

    UFUNCTION(BlueprintCallable)
    static bool GetOrientation(const TArray<FVector2D>& Points);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Fit Points"))
    static TArray<FVector2D> K2_FitPoints(const TArray<FVector2D>& Points, FVector2D Dimension, float FitScale = 1.f);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Flip Points"))
    static TArray<FVector2D> K2_FlipPoints(const TArray<FVector2D>& Points, FVector2D Dimension);

    UFUNCTION(BlueprintCallable)
    static void ReversePoints(const TArray<FVector2D>& InPoints, TArray<FVector2D>& OutPoints);

    static void FitPoints(TArray<FVector2D>& Points, const FVector2D& Dimension, float FitScale = 1.f);

    static void FlipPoints(TArray<FVector2D>& Points, const FVector2D& Dimension);

    static bool IsPointInPoly(const FVector2D& Point, const TArray<FVector2D>& Poly);

    static bool IsPointOnTri(float px, float py, float tpx0, float tpy0, float tpx1, float tpy1, float tpx2, float tpy2);

    FORCEINLINE static bool IsPointOnTri(
        const FVector2D& Point,
        const FVector2D& TriPoint0,
        const FVector2D& TriPoint1,
        const FVector2D& TriPoint2
        )
    {
        return IsPointOnTri(
            Point.X,
            Point.Y,
            TriPoint0.X,
            TriPoint0.Y,
            TriPoint1.X,
            TriPoint1.Y,
            TriPoint2.X,
            TriPoint2.Y
            );
    }

    FORCEINLINE static bool IsPointOnTri(
        const FVector& Point,
        const FVector& TriPoint0,
        const FVector& TriPoint1,
        const FVector& TriPoint2
        )
    {
        return IsPointOnTri(
            Point.X,
            Point.Y,
            TriPoint0.X,
            TriPoint0.Y,
            TriPoint1.X,
            TriPoint1.Y,
            TriPoint2.X,
            TriPoint2.Y
            );
    }
};
