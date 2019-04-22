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

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULPolyUtilityLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

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
};
