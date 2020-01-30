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

#include "PDS/GULPoissonDiscSampler.h"

FGULPoissonDiscSampler::FGULPoissonDiscSampler(FBox2D InBounds, float InRadius, int32 InKValue)
{
    SetConfig(InBounds, InRadius, InKValue);
}

void FGULPoissonDiscSampler::SetConfig(FBox2D InBounds, float InRadius, int32 InKValue)
{
    KValue = InKValue;
    Bounds = InBounds;

    if (Bounds.bIsValid && Bounds.GetArea() > KINDA_SMALL_NUMBER)
    {
        FVector2D BoundsSize = Bounds.GetSize();
        PointRadius = InRadius * BoundsSize.Y;

        if (BoundsSize.X > BoundsSize.Y)
        {
            PointRadius *= (BoundsSize.X / BoundsSize.Y);
        }
    }
    else
    {
        Bounds = FBox2D(ForceInitToZero);
    }
}

void FGULPoissonDiscSampler::GeneratePoints(TArray<FVector2D>& OutPoints, FRandomStream& Rand)
{
    check(HasValidConfig());

    RadiusSq = PointRadius * PointRadius;
    CellSize = PointRadius * UE_INV_SQRT_2;
    CellSizeInv = 1.f/CellSize;

    const float KInv = 1.f/static_cast<float>(KValue);

    FVector2D GridSize(Bounds.GetSize());
    DimX = FMath::CeilToInt(GridSize.X * CellSizeInv);
    DimY = FMath::CeilToInt(GridSize.Y * CellSizeInv);

    // Initialize grid point sample indices with invalid indices
    Grid.SetNumUninitialized(DimX * DimY);
    FMemory::Memset(Grid.GetData(), 0xFF, Grid.Num()*Grid.GetTypeSize());

    TArray<int32> Queue;
    TArray<FVector2D> Points;

    // Pick the first sample within bounds
    {
        FVector2D InitialPoint = Bounds.Min + Rand.GetFraction()*GridSize;

        int32 PointIndex = Points.Emplace(InitialPoint);

        Queue.Emplace(PointIndex);
        SetSample(GetCell(InitialPoint), PointIndex);
    }

    // Pick a random existing sample from the queue
    while (Queue.Num() > 0)
    {
        const int32 i = Rand.RandHelper(Queue.Num());
        const int32 ParentIndex = Queue[i];
        const FVector2D& ParentPoint(Points[ParentIndex]);

        const float RandFrac = Rand.GetFraction();
        bool bHasNewPoint = false;

        // Make a new candidate
        for (int32 j=0; j<KValue; ++j)
        {
            const float a = 2.f * PI * (RandFrac + static_cast<float>(j)*KInv);
            const float r = PointRadius + SMALL_NUMBER;

            FVector2D Point;
            Point.X = ParentPoint.X + r * FMath::Cos(a);
            Point.Y = ParentPoint.Y + r * FMath::Sin(a);

            // Accept candidates that are inside the allowed extent
            // and farther than 2 * radius to all existing samples
            if (Bounds.Min.X <= Point.X && Point.X < Bounds.Max.X &&
                Bounds.Min.Y <= Point.Y && Point.Y < Bounds.Max.Y &&
                IsWithinValidPointRadius(Point, Points)
                )
            {
                int32 PointIndex = Points.Emplace(Point);

                Queue.Emplace(PointIndex);
                SetSample(GetCell(Point), PointIndex);

                bHasNewPoint = true;

                break;
            }
        }

        // If none of k candidates were accepted, remove it from the queue
        if (! bHasNewPoint)
        {
            Queue.RemoveAtSwap(i, 1, false);
        }
    }

    OutPoints = MoveTemp(Points);
}

bool FGULPoissonDiscSampler::IsWithinValidPointRadius(const FVector2D& Point, const TArray<FVector2D>& Points) const
{
    const FIntPoint Cell(GetCell(Point));
    const int32 x0 = FMath::Max(Cell.X-2, 0);
    const int32 y0 = FMath::Max(Cell.Y-2, 0);
    const int32 x1 = FMath::Min(Cell.X+3, DimX);
    const int32 y1 = FMath::Min(Cell.Y+3, DimY);

    for (int32 y=y0; y < y1; ++y)
    {
        int32 o = y * DimX;

        for (int32 x=x0; x < x1; ++x)
        {
            const int32 PointIndex = Grid[o+x];

            if (Points.IsValidIndex(PointIndex))
            {
                const FVector2D& s(Points[PointIndex]);

                // Point is within another point radius, invalid point
                if ((Point-s).SizeSquared() < RadiusSq)
                {
                    return false;
                }
            }
        }
    }

    return true;
}
