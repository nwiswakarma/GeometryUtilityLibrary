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
#include "GULTypes.h"
#include "GULPolyTypes.generated.h"

union GEOMETRYUTILITYLIBRARY_API FGULEdgeIndexPair
{
    uint64 IndexPacked;
    struct { uint32 MinIndex, MaxIndex; };

    FGULEdgeIndexPair() = default;

    FGULEdgeIndexPair(uint64 InIndexPacked)
        : IndexPacked(InIndexPacked)
    {
    }

    FGULEdgeIndexPair(uint32 InMinIndex, uint32 InMaxIndex)
        : MinIndex(InMinIndex)
        , MaxIndex(InMaxIndex)
    {
    }

    FORCEINLINE uint32 GetMinIndex() const
    {
        return MinIndex;
    }

    FORCEINLINE uint32 GetMaxIndex() const
    {
        return MaxIndex;
    }
};

template <> struct TIsPODType<FGULEdgeIndexPair> { enum { Value = true }; };

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULIndexedPolyGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 OuterPolyIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> InnerPolyIndices;

    inline bool IsValidIndexGroup(const TArray<FGULVector2DGroup>& PolyGroups) const
    {
        if (PolyGroups.IsValidIndex(OuterPolyIndex))
        {
            bool bIsValidIndexGroup = true;

            for (int32 InnerPolyIndex : InnerPolyIndices)
            {
                if (! PolyGroups.IsValidIndex(InnerPolyIndex))
                {
                    bIsValidIndexGroup = false;
                    break;
                }
            }

            return bIsValidIndexGroup;
        }

        return false;
    }

    FORCEINLINE const FGULVector2DGroup& GetOuter(const TArray<FGULVector2DGroup>& PolyGroups) const
    {
        return PolyGroups[OuterPolyIndex];
    }

    FORCEINLINE FGULVector2DGroup& GetOuter(TArray<FGULVector2DGroup>& PolyGroups) const
    {
        return PolyGroups[OuterPolyIndex];
    }

    FORCEINLINE const FGULVector2DGroup& GetInner(const TArray<FGULVector2DGroup>& PolyGroups, int32 Index) const
    {
        return PolyGroups[InnerPolyIndices[Index]];
    }

    FORCEINLINE FGULVector2DGroup& GetInner(TArray<FGULVector2DGroup>& PolyGroups, int32 Index) const
    {
        return PolyGroups[InnerPolyIndices[Index]];
    }

    FORCEINLINE int32 GetInnerNum() const
    {
        return InnerPolyIndices.Num();
    }
};
