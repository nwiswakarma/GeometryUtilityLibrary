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

#include "Geom/GULGeometryUtilityLibrary.h"

void UGULGeometryUtility::TransformBox2DPoints(FGULBox2DPoints& OutPoints, const FTransform& Transform, const FGULBox2DPoints& InPoints)
{
    OutPoints.P0 = FVector2D(Transform.TransformPosition(FVector(InPoints.P0, 0)));
    OutPoints.P1 = FVector2D(Transform.TransformPosition(FVector(InPoints.P1, 0)));
    OutPoints.P2 = FVector2D(Transform.TransformPosition(FVector(InPoints.P2, 0)));
    OutPoints.P3 = FVector2D(Transform.TransformPosition(FVector(InPoints.P3, 0)));
}

void UGULGeometryUtility::TransformBoxVectors(FGULBoxVectors& OutVectors, const FTransform& Transform, const FGULBoxVectors& InVectors)
{
    OutVectors.V0 = Transform.TransformPosition(InVectors.V0);
    OutVectors.V1 = Transform.TransformPosition(InVectors.V1);
    OutVectors.V2 = Transform.TransformPosition(InVectors.V2);
    OutVectors.V3 = Transform.TransformPosition(InVectors.V3);

    OutVectors.V4 = Transform.TransformPosition(InVectors.V4);
    OutVectors.V5 = Transform.TransformPosition(InVectors.V5);
    OutVectors.V6 = Transform.TransformPosition(InVectors.V6);
    OutVectors.V7 = Transform.TransformPosition(InVectors.V7);
}

void UGULGeometryUtility::TransformBox(FGULOrientedBox& OutBox, const FGULOrientedBox& InBox, const FTransform& Transform)
{
    FVector Scale(Transform.GetScale3D());
    FQuat Rotation(Transform.GetRotation());

    OutBox.Center = Transform.TransformPosition(InBox.Center);
    OutBox.Extent = InBox.Extent * Scale;
    OutBox.Rotation = FRotator(InBox.Rotation.Quaternion() * Rotation);
}

void UGULGeometryUtility::AlignBox(TArray<FGULOrientedBox>& OutBoxes, TArray<FVector>& OutDeltas, const TArray<FGULOrientedBox>& InBoxes)
{
    AlignBoxToBottom(OutBoxes, OutDeltas, InBoxes);
}

void UGULGeometryUtility::AlignBoxToBottom(TArray<FGULOrientedBox>& OutBoxes, TArray<FVector>& OutDeltas, const TArray<FGULOrientedBox>& InBoxes)
{
    const int32 BoxCount = InBoxes.Num();

    OutBoxes.SetNumUninitialized(BoxCount);
    OutDeltas.SetNumUninitialized(BoxCount);

    float ZMin = BIG_NUMBER;

    for (const FGULOrientedBox& Box : InBoxes)
    {
        float BoxMinZ(Box.Center.Z-Box.Extent.Z);

        if (BoxMinZ < ZMin)
        {
            ZMin = BoxMinZ;
        }
    }

    for (int32 i=0; i<BoxCount; ++i)
    {
        const FGULOrientedBox& Box(InBoxes[i]);
        float BoxMinZ(Box.Center.Z-Box.Extent.Z);
        float ZDelta(ZMin-BoxMinZ);

        OutBoxes[i] = Box;
        OutBoxes[i].Center.Z += ZDelta;

        OutDeltas[i] = FVector(0, 0, ZDelta);
    }
}
