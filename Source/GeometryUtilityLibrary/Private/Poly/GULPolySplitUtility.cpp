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

#include "Poly/GULPolySplitUtility.h"

void FGULPolySplitter::Split(TArray<FGULVector2DGroup>& OutPolys, const TArray<FVector2D>& InPoly, const FVector2D& InLineP0, const FVector2D& InLineP1)
{
    const FLine Line(InLineP0, InLineP1);
    SplitEdges(InPoly, Line);
    SortEdges(Line);
    SplitPolygon();
    CollectPolys(OutPolys);
}

void FGULPolySplitter::SplitEdges(const TArray<FVector2D>& Poly, const FLine& line)
{
    SplitPoly.Empty();
    EdgesOnLine.Reset();

    for (int32 i=0; i<Poly.Num(); i++)
    {
        const FLine edge(Poly[i], Poly[(i+1)%Poly.Num()]);
        const EGULLineSide edgeStartSide = GetSideOfLine(line, edge.P0);
        const EGULLineSide edgeEndSide = GetSideOfLine(line, edge.P1);

        SplitPoly.AddTail(FPolyEdge(Poly[i], edgeStartSide));

        if (edgeStartSide == EGULLineSide::O)
        {
            EdgesOnLine.Emplace(&SplitPoly.GetTail()->GetValue());
        }
        else
        if (edgeStartSide != edgeEndSide && edgeEndSide != EGULLineSide::O)
        {
            FVector2D Intersection;
            bool bHasIntersection = UGULGeometryUtility::SegmentIntersection2D(
                edge.P0,
                edge.P1,
                line.P0,
                line.P1,
                Intersection
                );
            check(bHasIntersection);
            SplitPoly.AddTail(FPolyEdge(Intersection, EGULLineSide::O));
            EdgesOnLine.Emplace(&SplitPoly.GetTail()->GetValue());
        }
    }

    auto NodeIt = SplitPoly.GetHead();
    while (NodeIt)
    {
        NodeIt->GetValue().Prev = (NodeIt == SplitPoly.GetHead())
            ? &SplitPoly.GetTail()->GetValue()
            : &NodeIt->GetPrevNode()->GetValue();

        NodeIt->GetValue().Next = (NodeIt == SplitPoly.GetTail())
            ? &SplitPoly.GetHead()->GetValue()
            : &NodeIt->GetNextNode()->GetValue();

        NodeIt = NodeIt->GetNextNode();
    }
}

void FGULPolySplitter::SortEdges(const FLine &line)
{
    // sort edges by start position relative to
    // the start position of the split line
    EdgesOnLine.Sort( [&](const FPolyEdge& e0, const FPolyEdge& e1)
    {
        // it's important to take the signed distance here,
        // because it can happen that the split line starts/ends
        // inside the polygon. in that case intersection points
        // can fall on both sides of the split line and taking
        // an unsigned distance metric will result in wrongly
        // ordered points in EdgesOnLine.
        return CalcSignedDistance(line, e0.StartPos) < CalcSignedDistance(line, e1.StartPos);
    } );

    // compute distance between each edge's start
    // position and the first edge's start position
    for (int32 i=1; i<EdgesOnLine.Num(); ++i)
    {
        EdgesOnLine[i]->DistOnLine = PointDistance(EdgesOnLine[i]->StartPos, EdgesOnLine[0]->StartPos);
    }
}

void FGULPolySplitter::SplitPolygon()
{
    FPolyEdge* useSrc = nullptr;

    for (int32 i=0; i<EdgesOnLine.Num(); i++)
    {
        // Find source
        FPolyEdge* srcEdge = useSrc;
        useSrc = nullptr;

        for (; !srcEdge && i<EdgesOnLine.Num(); i++)
        {
            FPolyEdge* curEdge = EdgesOnLine[i];
            const auto curSide = curEdge->StartSide;
            const auto prevSide = curEdge->Prev->StartSide;
            const auto nextSide = curEdge->Next->StartSide;
            //assert(curSide == EGULLineSide::O);
            check(curSide == EGULLineSide::O);

            if ((prevSide == EGULLineSide::L && nextSide == EGULLineSide::R) ||
                (prevSide == EGULLineSide::L && nextSide == EGULLineSide::O && curEdge->Next->DistOnLine < curEdge->DistOnLine) ||
                (prevSide == EGULLineSide::O && nextSide == EGULLineSide::R && curEdge->Prev->DistOnLine < curEdge->DistOnLine))
            {
                srcEdge = curEdge;
                srcEdge->IsSrcEdge = true;
            }
        }

        // Find destination
        FPolyEdge* dstEdge = nullptr;

        for (; !dstEdge && i<EdgesOnLine.Num(); )
        {
            FPolyEdge* curEdge = EdgesOnLine[i];
            const auto curSide = curEdge->StartSide;
            const auto prevSide = curEdge->Prev->StartSide;
            const auto nextSide = curEdge->Next->StartSide;
            //assert(curSide == EGULLineSide::O);
            check(curSide == EGULLineSide::O);

            if ((prevSide == EGULLineSide::R && nextSide == EGULLineSide::L) ||
                (prevSide == EGULLineSide::O && nextSide == EGULLineSide::L) ||
                (prevSide == EGULLineSide::R && nextSide == EGULLineSide::O) ||
                (prevSide == EGULLineSide::R && nextSide == EGULLineSide::R) ||
                (prevSide == EGULLineSide::L && nextSide == EGULLineSide::L))
            {
                dstEdge = curEdge;
                dstEdge->IsDstEdge = true;
            }
            else
            {
                ++i;
            }
        }

        // bridge source and destination
        if (srcEdge && dstEdge)
        {
            CreateBridge(srcEdge, dstEdge);
            VerifyCycles();

            // is it a configuration in which a vertex
            // needs to be reused as source vertex?
            if (srcEdge->Prev->Prev->StartSide == EGULLineSide::L)
            {
                useSrc = srcEdge->Prev;
                useSrc->IsSrcEdge = true;
            }
            else
            if (dstEdge->Next->StartSide == EGULLineSide::R)
            {
                useSrc = dstEdge;
                useSrc->IsSrcEdge = true;
            }
        }
    }
}

void FGULPolySplitter::CreateBridge(FPolyEdge* srcEdge, FPolyEdge* dstEdge)
{
    SplitPoly.AddTail(*srcEdge);
    FPolyEdge* a = &SplitPoly.GetTail()->GetValue();
    SplitPoly.AddTail(*dstEdge);
    FPolyEdge* b = &SplitPoly.GetTail()->GetValue();
    a->Next = dstEdge;
    a->Prev = srcEdge->Prev;
    b->Next = srcEdge;
    b->Prev = dstEdge->Prev;
    srcEdge->Prev->Next = a;
    srcEdge->Prev = b;
    dstEdge->Prev->Next = b;
    dstEdge->Prev = a;
}

void FGULPolySplitter::VerifyCycles() const
{
    for (const FPolyEdge& edge : SplitPoly)
    {
        const FPolyEdge* curSide = &edge;
        int32 count = 0;

        do
        {
            check(count < SplitPoly.Num());
            curSide = curSide->Next;
            count++;
        }
        while (curSide != &edge);
    }
}

void FGULPolySplitter::CollectPolys(TArray<FGULVector2DGroup>& OutPolys)
{
    OutPolys.Reset();

    for (FPolyEdge& e : SplitPoly)
    {
        if (! e.Visited)
        {
            OutPolys.AddDefaulted();

            //QPolygonF splitPoly;
            TArray<FVector2D>& splitPoly(OutPolys.Last().Points);
            FPolyEdge* curSide = &e;

            do
            {
                curSide->Visited = true;
                splitPoly.Emplace(curSide->StartPos);
                curSide = curSide->Next;
            }
            while (curSide != &e);
        }
    }
}

void UGULPolySplitUtility::Split(TArray<FGULVector2DGroup>& OutPolys, const TArray<FVector2D>& InPoly, const FVector2D& InLineP0, const FVector2D& InLineP1)
{
    FGULPolySplitter Splitter;
    Splitter.Split(OutPolys, InPoly, InLineP0, InLineP1);
}

bool UGULPolySplitUtility::SplitPolyWithPolylines(TArray<FGULVector2DGroup>& PolysR, TArray<FGULVector2DGroup>& PolysL, const TArray<FVector2D>& InPolyPoints, const TArray<FVector2D>& InSplitPoints)
{
#if 0
    if (InPolyPoints.Num() < 3)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULPolySplitUtility::SplitPolyWithPolylines() ABORTED, POLY POINTS < 3"));
        return false;
    }

    if (InSplitPoints.Num() < 2)
    {
        UE_LOG(LogGUL,Warning, TEXT("UGULPolySplitUtility::SplitPolyWithPolylines() ABORTED, SPLIT POINTS < 2"));
        return false;
    }

    //struct FSplitNode
    //{
    //    int32 SplitLineIndex;
    //    int32 PolyLineIndex;
    //    FVector2D Intersection;
    //};

    //TArray<FSplitNode> SplitNodes;

    const bool bPolyFirstLastPointEquals = InPolyPoints[0].Equals(InPolyPoints.Last());
    const int32 PolyPointCount = InPolyPoints.Num();
    const int32 PolyEdgeCount = bPolyFirstLastPointEquals ? PolyPointCount-1 : PolyPointCount;
    const int32 SplitPointCount = InSplitPoints.Num();
    const int32 SplitLineCount = SplitPointCount-1;

    enum EPolyNodeType
    {
        SOURCE  = 0,
        SPLIT   = 1,
        MERGE   = 2,
        SNAP    = 4
    };

    struct FPolyNode
    {
        EPolyNodeType NodeType;
        FVector2D StartPoint;
        int32 SplitPointIndex;
        FPolyNode* NextNode;
        FPolyNode* PrevNode;
    };

    // Generate poly nodes

    typedef TDoubleLinkedList<FPolyNode>     FPolyList;
    typedef FPolyList::TDoubleLinkedListNode FPolyListNode;

    TDoubleLinkedList<FPolyNode> PolyNodeList;

    for (const FVector2D& PolyPoint : InPolyPoints)
    {
        FPolyListNode* TailNode(PolyNodeList.GetTail());

        FPolyNode PolyNode;
        PolyNode.NodeType = EPolyNodeType::SOURCE;
        PolyNode.StartPoint = PolyPoint;
        PolyNode.SplitPointIndex = -1;

        PolyNodeList.AddTail(PolyNode);

        if (PolyNodeList.Num() > 0)
        {
            FPolyNode& CurrNode(PolyNodeList.GetTail()->GetValue());
            FPolyNode& PrevNode(TailNode->GetValue());

            CurrNode.PrevNode = &PrevNode;
            PrevNode.NextNode = &CurrNode;
        }
    }

    // Connects first to last node and vice versa
    {
        FPolyNode& HeadNode(PolyNodeList.GetHead()->GetValue());
        FPolyNode& TailNode(PolyNodeList.GetTail()->GetValue());

        HeadNode.PrevNode = &TailNode;
        TailNode.NextNode = &HeadNode;
    }

    UE_LOG(LogTemp,Warning, TEXT("SPLIT"));

    for (int32 li=0; li<SplitLineCount; ++li)
    {
        const int32 li0 = li;
        const int32 li1 = li+1;

        const FVector2D& LP0(InSplitPoints[li0]);
        const FVector2D& LP1(InSplitPoints[li1]);

        TArray< TPair<FPolyListNode*, FPolyNode> > NewPolyNodes;

        FPolyListNode* NodeIt = PolyNodeList.GetHead();

        //for (int32 pi=0; pi<PolyEdgeCount; ++pi)
        while (NodeIt)
        {
            //const int32 pi0 = pi;
            //const int32 pi1 = (pi+1) % PolyPointCount;

            //const FVector2D& PP0(InPolyPoints[pi0]);
            //const FVector2D& PP1(InPolyPoints[pi1]);

            FPolyNode& PolyNode(NodeIt->GetValue());

            const FVector2D& PP0(PolyNode.StartPoint);
            const FVector2D& PP1(PolyNode.NextNode->StartPoint);

            const EGULLineSide EdgeSide0 = FGULPolySplitter::GetSideOfLine(LP0, LP1, PP0, KINDA_SMALL_NUMBER);
            const EGULLineSide EdgeSide1 = FGULPolySplitter::GetSideOfLine(LP0, LP1, PP1, KINDA_SMALL_NUMBER);

            const bool bPolyPointOnSplitLine0 = EdgeSide0 == EGULLineSide::O;
            const bool bPolyPointOnSplitLine1 = EdgeSide1 == EGULLineSide::O;

            // Any poly segment point is on split line
            if (bPolyPointOnSplitLine0 || bPolyPointOnSplitLine1)
            {
                const bool bSplitPointOnPoly0 = UGULGeometryUtility::IsPointOnBounds(PP0, PP1, LP0);
                const bool bSplitPointOnPoly1 = UGULGeometryUtility::IsPointOnBounds(PP0, PP1, LP1);

                // Both poly segment point is on split line
                if (bPolyPointOnSplitLine0 && bPolyPointOnSplitLine1)
                {
                    // Split segment is on poly segment
                    if (bSplitPointOnPoly0 && bSplitPointOnPoly1)
                    {
                        // Add both split points as new poly node

                        FPolyNode NewPolyNode0;
                        NewPolyNode0.NodeType = EPolyNodeType::MERGE;
                        NewPolyNode0.StartPoint = PP0;
                        NewPolyNode0.SplitPointIndex = li0;
                        NewPolyNode0.PrevNode = &PolyNode;
                        NewPolyNode0.NextNode = PolyNode.NextNode;

                        FPolyNode NewPolyNode1;
                        NewPolyNode1.NodeType = EPolyNodeType::MERGE;
                        NewPolyNode1.StartPoint = PP1;
                        NewPolyNode1.SplitPointIndex = li0;
                        NewPolyNode1.PrevNode = &PolyNode;
                        NewPolyNode1.NextNode = PolyNode.NextNode;

                        NewPolyNodes.Emplace(NodeIt, NewPolyNode0);
                        NewPolyNodes.Emplace(NodeIt, NewPolyNode1);

                        UE_LOG(LogTemp,Warning, TEXT("Add both split points as new poly node"));
                    }
                    // First split segment point is on poly segment
                    else
                    if (bSplitPointOnPoly0)
                    {
                        // Add first split point as new poly node
                        UE_LOG(LogTemp,Warning, TEXT("Add first split point as new poly node"));
                    }
                    // Second split segment point is on poly segment
                    else
                    if (bSplitPointOnPoly1)
                    {
                        // Add second split point as new poly node
                        UE_LOG(LogTemp,Warning, TEXT("Add second split point as new poly node"));
                    }
                }
                // One poly segment point is on split line
                else
                {
                    const bool bPolyPointOnSplitSegment0 = (bPolyPointOnSplitLine0 && bSplitPointOnPoly0);
                    const bool bPolyPointOnSplitSegment1 = (bPolyPointOnSplitLine1 && bSplitPointOnPoly1);

                    // One of the poly segment points is on split segment
                    if (bPolyPointOnSplitSegment0 || bPolyPointOnSplitSegment1)
                    {
                        const FVector2D PolyPointOnSegment = bPolyPointOnSplitSegment0 ? PP0 : PP1;

                        if (PolyPointOnSegment.Equals(LP0))
                        {
                            // Snap poly segment point to first split segment
                            UE_LOG(LogTemp,Warning, TEXT("Snap poly segment point to first split segment"));
                        }
                        else
                        if (PolyPointOnSegment.Equals(LP1))
                        {
                            // Snap poly segment point to second split segment
                            UE_LOG(LogTemp,Warning, TEXT("Snap poly segment point to second split segment"));
                        }
                        else
                        {
                            // Snap poly segment point to split segment
                            UE_LOG(LogTemp,Warning, TEXT("Snap poly segment point to split segment"));
                        }
                    }
                }
            }
            // Both poly segment points are not on split line
            else
            {
                // Poly and split segments intersection

                FVector2D Intersect;
                float IntersectT;

                bool bIntersect = UGULGeometryUtility::SegmentIntersection2D(
                    LP0,
                    LP1,
                    PP0,
                    PP1,
                    Intersect,
                    IntersectT
                    );

                if (bIntersect)
                {
                    // Add segment intersection point as new poly node

                    FPolyNode NewPolyNode;
                    NewPolyNode.NodeType = EPolyNodeType::SPLIT;
                    NewPolyNode.StartPoint = Intersect;
                    NewPolyNode.SplitPointIndex = li0;
                    NewPolyNode.PrevNode = &PolyNode;
                    NewPolyNode.NextNode = PolyNode.NextNode;

                    NewPolyNodes.Emplace(NodeIt, NewPolyNode);

                    UE_LOG(LogTemp,Warning, TEXT("Add segment intersection point as new poly node"));
                }
            }

            NodeIt = NodeIt->GetNextNode();

#if 0
            FVector2D Intersect;
            float IntersectT;

            // Polyline segment and poly segment intersection

            bool bIntersect = UGULGeometryUtility::SegmentIntersection2D(
                LP0,
                LP1,
                PP0,
                PP1,
                Intersect,
                IntersectT
                );

            if (bIntersect)
            {
                FSplitNode SplitNode = { li, pi, Intersect };
                SplitNodes.Emplace(SplitNode);
            }

            // Polyline segment and poly point intersection

            float DistToPoint = UGULGeometryUtility::PointDistToSegment2D(PP0, LP0, LP1);

            if (DistToPoint < KINDA_SMALL_NUMBER)
            {
                FSplitNode SplitNode = { li, pi, PP0 };
                SplitNodes.Emplace(SplitNode);
            }

            UE_LOG(LogTemp,Warning, TEXT("bIntersect: %d, T: %f, DistToPoint: %d, Side0: %s, Side1: %s"),
                bIntersect,
                bIntersect ? IntersectT : -1.f,
                DistToPoint < KINDA_SMALL_NUMBER,
                *FGULPolySplitter::GetLineSideString(EdgeSide0),
                *FGULPolySplitter::GetLineSideString(EdgeSide1)
                );
#endif
        }

        // Register new poly node

        for (const auto& NewPolyNodeEntry : NewPolyNodes)
        {
            FPolyListNode* ListNode = NewPolyNodeEntry.Get<0>();
            FPolyListNode* NewListNode;

            if (ListNode == PolyNodeList.GetTail())
            {
                PolyNodeList.AddTail(NewPolyNodeEntry.Get<1>());
                NewListNode = PolyNodeList.GetTail();
            }
            else
            {
                PolyNodeList.InsertNode(NewPolyNodeEntry.Get<1>(), ListNode->GetNextNode());
                NewListNode = ListNode->GetNextNode();
            }

            FPolyNode& NewPolyNode(NewListNode->GetValue());
            NewPolyNode.PrevNode->NextNode = &NewPolyNode;
            NewPolyNode.NextNode->PrevNode = &NewPolyNode;
        }
    }
#endif

    return false;
}
