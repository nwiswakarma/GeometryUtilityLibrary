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
#include "Poly/GULPolyTypes.h"
#include "GULPolyUtilityLibrary.generated.h"

USTRUCT(BlueprintType)
struct GEOMETRYUTILITYLIBRARY_API FGULPointAngleOutput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Angle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bOrientation;

    FORCEINLINE FString ToString() const
    {
        return FString::Printf(
            TEXT("(Index0: %d, Index1: %d, Index2: %d, Angle: %f, bOrientation: %d)"),
            Index0,
            Index1,
            Index2,
            Angle,
            bOrientation
            );
    }
};

UCLASS()
class GEOMETRYUTILITYLIBRARY_API UGULPolyUtilityLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    typedef TDoubleLinkedList<FVector2D>                        FPointList;
    typedef TDoubleLinkedList<FVector2D>::TIterator             FPointIterator;
    typedef TDoubleLinkedList<FVector2D>::TDoubleLinkedListNode FPointNode;
    typedef TSet<FPointNode*>                                   FPointNodeSet;

    FORCEINLINE static bool IsValidPointCountToCollapse(uint32 PointCount, bool bCircular)
    {
        return (!bCircular && PointCount > 2) || (bCircular && PointCount > 3);
    }

    static void CollapseOrMergePointNodeFromSet(FPointList& PointList, FPointNodeSet& PointNodeSet, FPointNode& PointNode, bool bCircular, TArray<FPointNode*>* RemovedNodeNeighbours = nullptr);
    static void GetAdjacentNodes(FPointNode*& PrevNode, FPointNode*& NextNode, FPointList& PointList, FPointNode& Node, bool bCircular, bool bSkipCircularEndPoints);

public:

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Area"))
    static float K2_GetArea(const TArray<FVector2D>& Points);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Orientation"))
    static bool K2_GetOrientation(const TArray<FVector2D>& Points);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Is Point On Poly"))
    static bool K2_IsPointOnPoly(const FVector2D& Point, const TArray<FVector2D>& Poly);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Points Bounds"))
    static FBox2D K2_GetPointsBounds(const TArray<FVector2D>& Points);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Fit Points"))
    static TArray<FVector2D> K2_FitPoints(const TArray<FVector2D>& Points, FVector2D Dimension, float FitScale = 1.f);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Flip Points"))
    static TArray<FVector2D> K2_FlipPoints(const TArray<FVector2D>& Points, FVector2D Dimension);

    UFUNCTION(BlueprintCallable)
    static void ReversePoints(const TArray<FVector2D>& InPoints, TArray<FVector2D>& OutPoints);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Find Points By Angle"))
    static void K2_FindPointsByAngle(TArray<FGULPointAngleOutput>& OutPoints, const TArray<FVector2D>& Points, float AngleThreshold = 0.f);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Point Angle Vectors"))
    static bool K2_GetPointAngleVectors(FVector2D& Point0, FVector2D& Point1, FVector2D& Point2, const FGULPointAngleOutput& PointAngle, const TArray<FVector2D>& Points, bool bMidPointExtents);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Collapse Point Angles"))
    static void K2_CollapsePointAngles(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& Points, bool bCircular, int32 MaxIteration = 1, float AngleThreshold = 0.f, float SignFilter = 0.f);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Subdivide Polylines"))
    static void K2_SubdividePolylines(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Clip Polylines To Bounds"))
    static void K2_ClipBounds(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const FBox2D& InBounds);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Fix Poly Groups Orientations"))
    static void K2_FixOrientations(TArray<FGULVector2DGroup>& OutPolyGroups, const TArray<FGULVector2DGroup>& InPolyGroups);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Group Poly Hierarchy (Even Odd)"))
    static void K2_GroupPolyHierarchyEvenOdd(TArray<FGULIndexedPolyGroup>& OutIndexedPolyGroups, const TArray<FGULVector2DGroup>& PolyGroups);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Convert Indexed Poly Groups To Vector Groups"))
    static void K2_ConvertIndexedPolyGroupToVectorGroup(
        FGULVectorGroup& OutVectorGroup,
        const FGULIndexedPolyGroup& InIndexedPolyGroup,
        const TArray<FGULVector2DGroup>& InPolyGroups,
        float ZPosition = 0.f
        );

    // Points Utility

    inline static FBox2D GetPointsBounds(const TArray<FVector2D>& Points);

    static void FitPoints(TArray<FVector2D>& Points, const FVector2D& Dimension, float FitScale = 1.f);

    static void FitPointsWithinBounds(TArray<FVector2D>& Points, const FBox2D& FitBounds, float FitScale = 1.f);

    static void FlipPoints(TArray<FVector2D>& Points, const FVector2D& Dimension);

    // Find Point On Poly

    static bool IsPointOnPoly(
        const FVector2D& Point,
        const TArray<FVector2D>& Poly
        );

    static bool IsPointOnPoly(
        const FVector2D& Point,
        const FGULIndexedPolyGroup& IndexGroup,
        const TArray<FGULVector2DGroup>& PolyGroups
        );

    static bool IsPointOnPoly(
        const FVector2D& Point,
        const TArray<FGULIndexedPolyGroup>& IndexGroups,
        const TArray<FGULVector2DGroup>& PolyGroups
        );

    inline static bool IsPointOnTri(
        float px,
        float py,
        float tpx0,
        float tpy0,
        float tpx1,
        float tpy1,
        float tpx2,
        float tpy2
        );

    FORCEINLINE static bool IsPointOnTri(
        const FVector2D& Point,
        const FVector2D& TriPoint0,
        const FVector2D& TriPoint1,
        const FVector2D& TriPoint2
        );

    FORCEINLINE static bool IsPointOnTri(
        const FVector& Point,
        const FVector& TriPoint0,
        const FVector& TriPoint1,
        const FVector& TriPoint2
        );

    // Find Area and Orientation

    inline static float GetArea(const TArray<FVector2D>& Points);

    FORCEINLINE static float GetArea(
        const FVector2D& Point0,
        const FVector2D& Point1,
        const FVector2D& Point2
        );

    FORCEINLINE static bool GetOrientation(const TArray<FVector2D>& Points);

    FORCEINLINE static bool GetOrientation(
        const FVector2D& Point0,
        const FVector2D& Point1,
        const FVector2D& Point2
        );

    // Find Points

    static void FindPointsByAngle(TArray<FGULPointAngleOutput>& OutPoints, const TArray<FVector2D>& Points, float AngleThreshold = 0.f);
    static bool GetPointAngleVectors(FVector2D& Point0, FVector2D& Point1, FVector2D& Point2, const FGULPointAngleOutput& PointAngle, const TArray<FVector2D>& Points, bool bMidPointExtents);
    static void CollapsePointAngles(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& Points, bool bCircular, int32 MaxIteration = 1, float AngleThreshold = 0.f, float SignFilter = 0.f);
    static bool IsPointAngleBelowThreshold(const FVector2D& P0, const FVector2D& P1, const FVector2D& P2, float AngleThreshold, bool bFilterBySign, bool bFilterNegative);

    // Polylines

    static void SubdividePolylines(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints);
    FORCEINLINE static void SubdividePolylines(TArray<FVector2D>& Points);

    static void SubdividePolylinesWithinLength(
        TArray<FVector2D>& OutPoints,
        const TArray<FVector2D>& InPoints,
        float SubdivisionLength = 10.f,
        bool bClosePolygon = false,
        bool bClosePolygonOutput = false
        );

    // Poly Grouping

    static void FixOrientations(TArray<FGULVector2DGroup>& InOutPolyGroups);

    static void GroupPolyHierarchyEvenOdd(
        TArray<FGULIndexedPolyGroup>& OutIndexedPolyGroups,
        const TArray<FGULVector2DGroup>& PolyGroups
        );

    static void ConvertIndexedPolyGroupToVectorGroup(
        FGULVectorGroup& OutVectorGroup,
        const FGULIndexedPolyGroup& InIndexedPolyGroup,
        const TArray<FGULVector2DGroup>& InPolyGroups,
        float ZPosition
        );

    // Poly Clip

    static void ClipBounds(
        TArray<FVector2D>& OutPoints,
        const TArray<FVector2D>& InPoints,
        const FBox2D& InBounds
        );

    static void ClipBounds(
        TArray<FGULVector2DGroup>& OutPolyGroups,
        const TArray<FGULVector2DGroup>& InPolyGroups,
        const FBox2D& InBounds
        );
};

FORCEINLINE_DEBUGGABLE float UGULPolyUtilityLibrary::K2_GetArea(const TArray<FVector2D>& Points)
{
    return GetArea(Points);
}

FORCEINLINE_DEBUGGABLE bool UGULPolyUtilityLibrary::K2_GetOrientation(const TArray<FVector2D>& Points)
{
    return GetOrientation(Points);
}

FORCEINLINE_DEBUGGABLE bool UGULPolyUtilityLibrary::K2_IsPointOnPoly(const FVector2D& Point, const TArray<FVector2D>& Poly)
{
    return IsPointOnPoly(Point, Poly);
}

FORCEINLINE_DEBUGGABLE FBox2D UGULPolyUtilityLibrary::K2_GetPointsBounds(const TArray<FVector2D>& Points)
{
    return GetPointsBounds(Points);
}

FORCEINLINE_DEBUGGABLE void UGULPolyUtilityLibrary::K2_FindPointsByAngle(TArray<FGULPointAngleOutput>& OutPoints, const TArray<FVector2D>& Points, float AngleThreshold)
{
    FindPointsByAngle(OutPoints, Points, AngleThreshold);
}

FORCEINLINE_DEBUGGABLE bool UGULPolyUtilityLibrary::K2_GetPointAngleVectors(FVector2D& Point0, FVector2D& Point1, FVector2D& Point2, const FGULPointAngleOutput& PointAngle, const TArray<FVector2D>& Points, bool bMidPointExtents)
{
    return GetPointAngleVectors(Point0, Point1, Point2, PointAngle, Points, bMidPointExtents);
}

FORCEINLINE_DEBUGGABLE void UGULPolyUtilityLibrary::K2_CollapsePointAngles(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& Points, bool bCircular, int32 MaxIteration, float AngleThreshold, float SignFilter)
{
    CollapsePointAngles(OutPoints, Points, bCircular, MaxIteration, AngleThreshold, SignFilter);
}

FORCEINLINE_DEBUGGABLE void UGULPolyUtilityLibrary::K2_SubdividePolylines(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints)
{
    SubdividePolylines(OutPoints, InPoints);
}

FORCEINLINE_DEBUGGABLE void UGULPolyUtilityLibrary::K2_ClipBounds(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, const FBox2D& InBounds)
{
    ClipBounds(OutPoints, InPoints, InBounds);
}

FORCEINLINE_DEBUGGABLE void UGULPolyUtilityLibrary::K2_FixOrientations(TArray<FGULVector2DGroup>& OutPolyGroups, const TArray<FGULVector2DGroup>& InPolyGroups)
{
    OutPolyGroups = InPolyGroups;
    FixOrientations(OutPolyGroups);
}

FORCEINLINE_DEBUGGABLE void UGULPolyUtilityLibrary::K2_GroupPolyHierarchyEvenOdd(TArray<FGULIndexedPolyGroup>& OutIndexedPolyGroups, const TArray<FGULVector2DGroup>& PolyGroups)
{
    GroupPolyHierarchyEvenOdd(OutIndexedPolyGroups, PolyGroups);
}

FORCEINLINE_DEBUGGABLE void UGULPolyUtilityLibrary::K2_ConvertIndexedPolyGroupToVectorGroup(
    FGULVectorGroup& OutVectorGroup,
    const FGULIndexedPolyGroup& InIndexedPolyGroup,
    const TArray<FGULVector2DGroup>& InPolyGroups,
    float ZPosition
    )
{
    OutVectorGroup.Vectors.Reset();

    ConvertIndexedPolyGroupToVectorGroup(
        OutVectorGroup, 
        InIndexedPolyGroup, 
        InPolyGroups,
        ZPosition
        );
}

// Points Utility

inline FBox2D UGULPolyUtilityLibrary::GetPointsBounds(const TArray<FVector2D>& Points)
{
    FBox2D Bounds(ForceInitToZero);

    for (const FVector2D& Point : Points)
    {
        Bounds += Point;
    }

    return Bounds;
}

// Find Point On Poly

inline bool UGULPolyUtilityLibrary::IsPointOnTri(
    float px,
    float py,
    float tpx0,
    float tpy0,
    float tpx1,
    float tpy1,
    float tpx2,
    float tpy2
    )
{
    float dX = px-tpx2;
    float dY = py-tpy2;
    float dX21 = tpx2-tpx1;
    float dY12 = tpy1-tpy2;
    float D = dY12*(tpx0-tpx2) + dX21*(tpy0-tpy2);
    float s = dY12*dX + dX21*dY;
    float t = (tpy2-tpy0)*dX + (tpx0-tpx2)*dY;
    return (D<0.f)
        ? (s<=0.f && t<=0.f && s+t>=D)
        : (s>=0.f && t>=0.f && s+t<=D);
}

FORCEINLINE bool UGULPolyUtilityLibrary::IsPointOnTri(
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

FORCEINLINE bool UGULPolyUtilityLibrary::IsPointOnTri(
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

// Find Area and Orientation

inline float UGULPolyUtilityLibrary::GetArea(const TArray<FVector2D>& Points)
{
    int32 PointCount = Points.Num();
    
    if (PointCount < 3)
    {
        return 0.f;
    }
    
    float a = 0.f;
    
    for (int32 i=0, j=PointCount-1; i<PointCount; ++i)
    {
        a += (Points[i].X + Points[j].X) * (Points[i].Y - Points[j].Y);
        j = i;
    }
    
    return a * 0.5f;
}

FORCEINLINE float UGULPolyUtilityLibrary::GetArea(
    const FVector2D& Point0,
    const FVector2D& Point1,
    const FVector2D& Point2
    )
{
    float a = 0.f;
    a += (Point0.X + Point2.X) * (Point0.Y - Point2.Y);
    a += (Point1.X + Point0.X) * (Point1.Y - Point0.Y);
    a += (Point2.X + Point1.X) * (Point2.Y - Point1.Y);
    return a * 0.5;
}

FORCEINLINE bool UGULPolyUtilityLibrary::GetOrientation(const TArray<FVector2D>& Points)
{
    return GetArea(Points) >= 0.f;
}

FORCEINLINE bool UGULPolyUtilityLibrary::GetOrientation(
    const FVector2D& Point0,
    const FVector2D& Point1,
    const FVector2D& Point2
    )
{
    return GetArea(Point0, Point1, Point2) >= 0.f;
}

inline bool UGULPolyUtilityLibrary::IsPointAngleBelowThreshold(const FVector2D& P0, const FVector2D& P1, const FVector2D& P2, float AngleThreshold, bool bFilterBySign, bool bFilterNegative)
{
    FVector2D N01(P1-P0);
    FVector2D N12(P2-P1);

    N01.Normalize();
    N12.Normalize();

    const float Dot = N01 | N12;

    if (Dot < AngleThreshold)
    {
        bool bRemovePoint = true;

        if (bFilterBySign)
        {
            FVector2D N01Ortho(-N01.Y, N01.X);
            bRemovePoint = ((N01Ortho | N12) < 0.f) == bFilterNegative;
        }

        return bRemovePoint;
    }

    return false;
}

inline void UGULPolyUtilityLibrary::GetAdjacentNodes(FPointNode*& PrevNode, FPointNode*& NextNode, FPointList& PointList, FPointNode& Node, bool bCircular, bool bSkipCircularEndPoints)
{
    PrevNode = Node.GetPrevNode();
    NextNode = Node.GetNextNode();

    if (bCircular && (! PrevNode || ! NextNode))
    {
        check(PrevNode || NextNode);

        // Circular make sure no node is invalid
        if (bCircular)
        {
            // Invalid previous node, set prev node to tail node
            if (! PrevNode)
            {
                // Ensure current node is head
                check(&Node == PointList.GetHead());

                if (bSkipCircularEndPoints)
                {
                    PrevNode = PointList.GetTail()->GetPrevNode();
                }
                else
                {
                    PrevNode = PointList.GetTail();
                }
            }

            // Invalid next node, set next node to head node
            if (! NextNode)
            {
                // Ensure current node is tail
                check(&Node == PointList.GetTail());

                if (bSkipCircularEndPoints)
                {
                    NextNode = PointList.GetHead()->GetNextNode();
                }
                else
                {
                    NextNode = PointList.GetHead();
                }
            }
        }
    }
}

FORCEINLINE void UGULPolyUtilityLibrary::SubdividePolylines(TArray<FVector2D>& Points)
{
    TArray<FVector2D> InPoints(Points);
    SubdividePolylines(Points, InPoints);
}
