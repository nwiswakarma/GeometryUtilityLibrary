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
#include "GULTypes.generated.h"

enum EGULBox2DClip
{
   B2DCLIP_B = 1, // Bottom
   B2DCLIP_T = 2, // Top
   B2DCLIP_L = 4, // Left
   B2DCLIP_R = 8  // Right
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULIntGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> Values;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULIntPointGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FIntPoint> Points;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULVector2DGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector2D> Points;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULVectorGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector> Vectors;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULBox2DPoints
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D P0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D P1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D P2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D P3;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULBoxVectors
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector V7;
};

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULOrientedBox
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Center;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Extent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation;
};

template <> struct TIsPODType<FGULOrientedBox> { enum { Value = true }; };
