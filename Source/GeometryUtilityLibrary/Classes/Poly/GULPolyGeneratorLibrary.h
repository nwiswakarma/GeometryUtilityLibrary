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
#include "GULPolyGeneratorLibrary.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="Poly Generation Params"))
struct GEOMETRYUTILITYLIBRARY_API FGULPolyGenerationParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 RandomSeed = 1337;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Size;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 Sides = 3;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Scale = .985f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float RandomScale = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Angle = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float RandomAngle = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float RandomSpread = 0.f;

    FORCEINLINE bool IsValid() const
    {
        return Sides >= 3 && Size.GetMin() > KINDA_SMALL_NUMBER;
    }
};

USTRUCT(BlueprintType, meta=(DisplayName="Poly Displacement Params"))
struct GEOMETRYUTILITYLIBRARY_API FGULPolyDisplacementParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 RandomSeed = 1337;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D Size;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector2D DisplacementRange;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 SubdivCount = 3;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float SubdivLimit = .1f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Scale = .985f;

    FORCEINLINE bool HasValidSubdivision() const
    {
        return SubdivCount >= 0
            && SubdivLimit > 0.f;
    }

    FORCEINLINE bool HasValidSize() const
    {
        return Size.GetMin() > 0.f
            && Scale > 0.f;
    }
};

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULPolyGeneratorLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    struct FRoughV2D
    {
        FVector2D Pos;
        float Balance;
        float MaxOffset;

        FORCEINLINE FRoughV2D(float x, float y, float b, float m)
            : Pos(x, y)
            , Balance(b)
            , MaxOffset(m)
        {
        }

        FORCEINLINE FRoughV2D(float x, float y, FRandomStream& Rand, float rmin, float rmax)
            : Pos(x, y)
        {
            GenerateBalance(Rand, rmin, rmax);
        }

        FORCEINLINE FRoughV2D(const FVector2D& p, FRandomStream& Rand, float rmin, float rmax)
            : Pos(p)
        {
            GenerateBalance(Rand, rmin, rmax);
        }

        void GenerateBalance(FRandomStream& Rand, float rmin, float rmax)
        {
            Balance = Rand.GetFraction();
            MaxOffset = Rand.GetFraction() * Rand.FRandRange(rmin, rmax);
        }
    };

    typedef TDoubleLinkedList<FRoughV2D>     FPolyList;
    typedef FPolyList::TDoubleLinkedListNode FPolyNode;

    static void GeneratePolyDisplacementInternal(
        TArray<FVector2D>& OutPoints,
        FPolyList& PolyList,
        FBox2D& Bounds,
        const FGULPolyDisplacementParams& Params,
        FRandomStream& Rand,
        bool bFitToBounds
        );

public:

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Poly"))
    static void GeneratePoly(TArray<FVector2D>& OutPoints, const FGULPolyGenerationParams& Params);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Poly Displacement", AutoCreateRefTerm="Params"))
    static void GeneratePolyDisplacement(
        TArray<FVector2D>& OutPoints,
        const TArray<FVector2D>& InitialPoints,
        const FGULPolyDisplacementParams& Params,
        bool bFitToBounds = true
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Generate Point Offsets"))
    static TArray<FVector2D> K2_GeneratePointOffsets(int32 Seed, const TArray<FVector2D>& Points, float PointRadius);
};
