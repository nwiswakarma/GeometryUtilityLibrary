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

    static void FitPoints(TArray<FVector2D>& Points, const FVector2D& Dimension, float FitScale = 1.f);

    static void FitPointsWithinBounds(TArray<FVector2D>& Points, const FBox2D& FitBounds, float FitScale = 1.f);

    static void FlipPoints(TArray<FVector2D>& Points, const FVector2D& Dimension);

    // Find Point On Poly

    static bool IsPointOnPoly(const FVector2D& Point, const TArray<FVector2D>& Poly);

    inline static bool IsPointOnTri(float px, float py, float tpx0, float tpy0, float tpx1, float tpy1, float tpx2, float tpy2)
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

    // Find Area and Orientation

    inline static float GetArea(const TArray<FVector2D>& Points)
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

    FORCEINLINE static float GetArea(const FVector2D& Point0, const FVector2D& Point1, const FVector2D& Point2)
    {
        float a = 0.f;
        a += (Point0.X + Point2.X) * (Point0.Y - Point2.Y);
        a += (Point1.X + Point0.X) * (Point1.Y - Point0.Y);
        a += (Point2.X + Point1.X) * (Point2.Y - Point1.Y);
        return a * 0.5;
    }

    FORCEINLINE static bool GetOrientation(const TArray<FVector2D>& Points)
    {
        return GetArea(Points) >= 0.f;
    }

    FORCEINLINE static bool GetOrientation(const FVector2D& Point0, const FVector2D& Point1, const FVector2D& Point2)
    {
        return GetArea(Point0, Point1, Point2) >= 0.f;
    }

    // Find Points

    static void FindPointsByAngle(TArray<FGULPointAngleOutput>& OutPoints, const TArray<FVector2D>& Points, float AngleThreshold = 0.f);
    static bool GetPointAngleVectors(FVector2D& Point0, FVector2D& Point1, FVector2D& Point2, const FGULPointAngleOutput& PointAngle, const TArray<FVector2D>& Points, bool bMidPointExtents);
    static void CollapsePointAngles(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& Points, bool bCircular, int32 MaxIteration = 1, float AngleThreshold = 0.f, float SignFilter = 0.f);
    static bool IsPointAngleBelowThreshold(const FVector2D& P0, const FVector2D& P1, const FVector2D& P2, float AngleThreshold, bool bFilterBySign, bool bFilterNegative);

    // Polylines

    static void SubdividePolylines(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints);
};

FORCEINLINE_DEBUGGABLE float UGULPolyUtilityLibrary::K2_GetArea(const TArray<FVector2D>& Points)
{
    return GetArea(Points);
}

FORCEINLINE_DEBUGGABLE bool UGULPolyUtilityLibrary::K2_GetOrientation(const TArray<FVector2D>& Points)
{
    return GetOrientation(Points);
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
