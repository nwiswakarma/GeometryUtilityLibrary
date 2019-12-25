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
#include "GULPolySplitUtility.generated.h"

enum class EGULLineSide : uint8
{
    O,  // On
    L,  // Left
    R   // Right
};

class FGULPolySplitter
{
public:

    struct FLine
    {
        FLine() = default;

        FLine(const FVector2D InP0, const FVector2D InP1)
            : P0(InP0)
            , P1(InP1)
        {
        }

        FVector2D P0;
        FVector2D P1;
    };

    struct FPolyEdge
    {
        FPolyEdge(const FVector2D &startPos, EGULLineSide side)
            : StartPos(startPos)
            , StartSide(side)
            , Next(nullptr)
            , Prev(nullptr)
            , DistOnLine(0.0f)
            , IsSrcEdge(false)
            , IsDstEdge(false)
            , Visited(false)
        {
        }

        FVector2D StartPos;     // start position on edge
        EGULLineSide StartSide; // start position's side of split line
        FPolyEdge* Next;        // next polygon in linked list
        FPolyEdge* Prev;        // previous polygon in linked list
        float DistOnLine;       // distance relative to first point on split line
        bool IsSrcEdge;         // for visualization
        bool IsDstEdge;         // for visualization
        bool Visited;           // for collecting split polygons
    };

public:

    void Split(TArray<FGULVector2DGroup>& OutPolys, const TArray<FVector2D>& InPoly, const FVector2D& InLineP0, const FVector2D& InLineP1);

private:

    void SplitEdges(const TArray<FVector2D>& Poly, const FLine& line);
    void SortEdges(const FLine &line);
    void SplitPolygon();
    void CollectPolys(TArray<FGULVector2DGroup>& OutPolys);
    void VerifyCycles() const;
    void CreateBridge(FPolyEdge *srcEdge, FPolyEdge *dstEdge);

    FORCEINLINE static float PointDistance(const FVector2D& pt0, const FVector2D& pt1);
    FORCEINLINE static float CalcSignedDistance(const FLine& line, const FVector2D& p);

public:

    TDoubleLinkedList<FPolyEdge> SplitPoly;
    TArray<FPolyEdge*> EdgesOnLine;

    FORCEINLINE static EGULLineSide GetSideOfLine(const FVector2D& LP0, const FVector2D& LP1, const FVector2D& pt, const float Threshold = KINDA_SMALL_NUMBER);
    FORCEINLINE static EGULLineSide GetSideOfLine(const FLine& line, const FVector2D& pt, const float Threshold = KINDA_SMALL_NUMBER);
    FORCEINLINE static FString GetLineSideString(EGULLineSide LineSide);
};

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULPolySplitUtility : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Split Poly With Line"))
    static void Split(TArray<FGULVector2DGroup>& OutPolys, const TArray<FVector2D>& InPoly, const FVector2D& InLineP0, const FVector2D& InLineP1);

    UFUNCTION(BlueprintCallable)
    static bool SplitPolyWithPolylines(TArray<FGULVector2DGroup>& PolysR, TArray<FGULVector2DGroup>& PolysL, const TArray<FVector2D>& InPolyPoints, const TArray<FVector2D>& InSplitPoints);
};

FORCEINLINE EGULLineSide FGULPolySplitter::GetSideOfLine(const FVector2D& LP0, const FVector2D& LP1, const FVector2D& pt, const float Threshold)
{
    const float d = (pt.X-LP0.X)*(LP1.Y-LP0.Y)-(pt.Y-LP0.Y)*(LP1.X-LP0.X); // Cross
    return (d > Threshold ? EGULLineSide::R : (d < -Threshold ? EGULLineSide::L : EGULLineSide::O));
}

FORCEINLINE EGULLineSide FGULPolySplitter::GetSideOfLine(const FLine& line, const FVector2D& pt, const float Threshold)
{
    return GetSideOfLine(line.P0, line.P1, pt, Threshold);
}

FORCEINLINE float FGULPolySplitter::PointDistance(const FVector2D& pt0, const FVector2D& pt1)
{
    return (pt0-pt1).Size();
}

FORCEINLINE float FGULPolySplitter::CalcSignedDistance(const FLine& line, const FVector2D& p)
{
    // Scalar projection on line. in case of co-linear
    // vectors this is equal to the signed distance (squared)
    return (p.X-line.P0.X)*(line.P1.X-line.P0.X)+(p.Y-line.P0.Y)*(line.P1.Y-line.P0.Y); // Dot
}

FORCEINLINE FString FGULPolySplitter::GetLineSideString(EGULLineSide LineSide)
{
    FString Name;

    switch (LineSide)
    {
        case EGULLineSide::O:
            Name = TEXT("EGULLineSide::O");
            break;
        case EGULLineSide::L:
            Name = TEXT("EGULLineSide::L");
            break;
        case EGULLineSide::R:
            Name = TEXT("EGULLineSide::R");
            break;
        default:
            Name = TEXT("UNKNOWN");
            break;
    };

    return Name;
}
