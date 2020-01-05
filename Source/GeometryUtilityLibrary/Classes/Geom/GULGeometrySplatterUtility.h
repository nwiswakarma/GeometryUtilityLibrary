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
#include "Geom/GULGeometryInstanceTypes.h"
#include "Geom/GULGeometrySplatterTypes.h"
#include "GULGeometrySplatterUtility.generated.h"

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULGeometrySplatterUtility : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    inline static FTransform GetSplatterInstanceTransform(const FGULGeometrySplatterInstance& SplatterInstance);

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

    UFUNCTION(BlueprintCallable)
    static void SplatterInstanceTransformBox2DPoints(
        FGULBox2DPoints& OutPoints,
        const FGULGeometrySplatterInstance& SplatterInstance,
        const FGULBox2DPoints& InPoints
        );

    UFUNCTION(BlueprintCallable)
    static void SplatterInstanceTransformBoxVectors(
        FGULBoxVectors& OutVectors,
        const FGULGeometrySplatterInstance& SplatterInstance,
        const FGULBoxVectors& InVectors
        );

    UFUNCTION(BlueprintCallable)
    static void SplatterInstanceTransformOrientedBox(
        FGULOrientedBox& OutBox,
        const FGULOrientedBox& InBox,
        const FGULGeometrySplatterInstance& SplatterInstance
        );
};

inline FTransform UGULGeometrySplatterUtility::GetSplatterInstanceTransform(const FGULGeometrySplatterInstance& SplatterInstance)
{
    FVector Translation(SplatterInstance.Position, 0);
    FQuat Rotation(FVector::UpVector, SplatterInstance.Angle);
    FVector Scale(SplatterInstance.Scale);
    return FTransform(Rotation, Translation, Scale);
}

FORCEINLINE_DEBUGGABLE void UGULGeometrySplatterUtility::K2_GenerateTileSplatter(
    int32 Seed,
    const FGULGeometryTileSplatterParameters& TileConfig,
    const FGULGeometryTransformParameters& GeometryTransform,
    TArray<FGULGeometrySplatterInstance>& GeometryInstances
    )
{
    FRandomStream Rand(Seed);
    GenerateTileSplatter(
        Rand,
        TileConfig,
        GeometryTransform,
        GeometryInstances
        );
}

FORCEINLINE_DEBUGGABLE void UGULGeometrySplatterUtility::K2_GenerateRadialSplatter(
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
