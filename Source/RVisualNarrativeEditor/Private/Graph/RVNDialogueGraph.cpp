#include "Graph/RVNDialogueGraph.h"
#include "SGraphPanel.h"
#include "RVNEditor.h"
#include "Decorator/RVNDecorator.h"
#include "Decorator/Task/RVNTask.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Graph/Node/Slate/SRVNStateNode.h"

URVNDialogueGraph::URVNDialogueGraph()
{
}

URVNStateNode* URVNDialogueGraph::CreateStateNode(ENodeType InType, const FVector2D& InPosition)
{
	check(RVNCompPtr)

	FGraphNodeCreator<URVNStateNode> NodeCreator(*this);
	URVNStateNode* NewNode = NodeCreator.CreateNode();

	auto& NewNodeData = RVNCompPtr->CreateNode(InPosition);
	NewNodeData.bIsSelector = InType == ENodeType::ENT_Select;

	NewNode->InitStateNode(NewNodeData, InType, this);
	NewNode->SetFlags(RF_Transactional);

	NodeCreator.Finalize();

	IdToNodeMap.Add(NewNode->GetNodeId(), NewNode);

	return NewNode;
}

URVNDecorator* URVNDialogueGraph::CreateDecorator(const UClass* InDecoratorClass) const
{
	check(RVNCompPtr)

	return RVNCompPtr->CreateDecorator(InDecoratorClass);
}

void URVNDialogueGraph::ProcessPasteNodes(const TArray<URVNStateNode*>& InStateNodes)
{
	TArray<URVNStateNode*> SourceNodes;
	SourceNodes.Reserve(InStateNodes.Num());

	for (auto StateNode : InStateNodes)
	{
		if (StateNode == nullptr)
		{
			continue;
		}

		if (const auto InputPin = StateNode->GetInputPin())
		{
			if (!InputPin->HasAnyConnections())
			{
				SourceNodes.Add(StateNode);
			}
		}
	}

	if (SourceNodes.IsEmpty())
	{
		return;
	}

	auto DFS = [&](auto& Self, URVNStateNode* CurNode) -> int32
	{
		if (CurNode == nullptr)
		{
			return INDEX_NONE;
		}

		TArray<URVNDecorator*> DecoratorsTemp;
		for (const auto Condition : CurNode->ConditionNodes)
		{
			DecoratorsTemp.Add(Cast<URVNDecorator>(Condition));
		}

		for (const auto Task : CurNode->TaskNodes)
		{
			DecoratorsTemp.Add(Cast<URVNDecorator>(Task));
		}

		CurNode->ConditionNodes.Empty();
		CurNode->TaskNodes.Empty();

		auto& NewNodeData = RVNCompPtr->CreateNode(FVector2D(CurNode->NodePosX, CurNode->NodePosY));

		NewNodeData.StateName = CurNode->StateName;
		NewNodeData.StateContent = CurNode->StateContent;
		NewNodeData.bIsPlayer = CurNode->bIsPlayer;
		NewNodeData.bIsSelector = CurNode->IsSelectNode();

		CurNode->SetNodeId(NewNodeData.NodeId);
		CurNode->PasteDecorator(DecoratorsTemp);

		IdToNodeMap.Add(NewNodeData.NodeId, CurNode);

		const auto Widget = GetNodeWidgetFromGuid(CurNode->NodeGuid);

		if (!Widget.IsValid())
		{
			return INDEX_NONE;
		}

		const auto StateWidget = StaticCastSharedPtr<SRVNStateWidget>(Widget);

		if (!StateWidget.IsValid())
		{
			return INDEX_NONE;
		}

		StateWidget->UpdateDelegate(CurNode);

		StateWidget->UpdateGraphNode();

		TArray<URVNStateNode*> NextStateNodes;

		if (const auto OutputPin = CurNode->GetOutputPin())
		{
			if (OutputPin->HasAnyConnections())
			{
				NextStateNodes.Reserve(OutputPin->LinkedTo.Num());

				for (const auto InputPin : OutputPin->LinkedTo)
				{
					if (const auto ChildNode = Cast<URVNStateNode>(InputPin->GetOwningNode()))
					{
						NextStateNodes.Add(ChildNode);
					}
				}
			}
		}

		NextStateNodes.Sort([](const URVNStateNode& A, const URVNStateNode& B) -> bool
		{
			if (!IsValid(&A) || !IsValid(&B))
			{
				return false;
			}

			const FVector2D PosA(A.NodePosX, A.NodePosY);
			const FVector2D PosB(B.NodePosX, B.NodePosY);

			return PosA.Y != PosB.Y ? PosA.Y < PosB.Y : PosA.X < PosB.X;
		});

		TArray<int32> NextStateNodeIds;
		NextStateNodeIds.Reserve(NextStateNodes.Num());

		for (auto ChildNode : NextStateNodes)
		{
			if (const int32 NextStateNodeId = Self(Self, ChildNode))
			{
				NextStateNodeIds.Add(NextStateNodeId);
			}
		}

		OnPinConnectionChanged(NewNodeData.NodeId, NextStateNodeIds);

		return NewNodeData.NodeId;
	};

	for (const auto SourceNode : SourceNodes)
	{
		DFS(DFS, SourceNode);
	}
}

bool URVNDialogueGraph::GetNodeData(int32 InNodeId, FRVNNodeData& OutData) const
{
	if (!RVNCompPtr)
	{
		return false;
	}

	return RVNCompPtr->GetNodeData(InNodeId, OutData);
}

void URVNDialogueGraph::OnStateNodeIdChanged(TMap<int32, int32> InIdMap)
{
	if (!RVNCompPtr)
	{
		return;
	}

	TMap<int32, URVNStateNode*> NewIdToNodeMap;

	for (const auto& Pair : InIdMap)
	{
		const int32 OldId = Pair.Key;
		const int32 NewId = Pair.Value;

		check(IdToNodeMap.Contains(OldId) && IdToNodeMap[OldId])

		IdToNodeMap[OldId]->SetNodeId(NewId);

		NewIdToNodeMap.Add(NewId, IdToNodeMap[OldId]);
	}

	IdToNodeMap = NewIdToNodeMap;
}

void URVNDialogueGraph::OnPinConnectionChanged(int32 InNodeId, const TArray<int32>& NextNodesId)
{
	if (!RVNCompPtr)
	{
		return;
	}

	RVNCompPtr->OnPinConnectionChanged(InNodeId, NextNodesId);
}

void URVNDialogueGraph::OnSpeakerNameChanged(int32 InNodeId, const FString& InNewName) const
{
	if (!RVNCompPtr)
	{
		return;
	}

	RVNCompPtr->OnSpeakerNameChanged(InNodeId, InNewName);
}

void URVNDialogueGraph::OnDialogueContentChanged(int32 InNodeId, const FString& InNewContent) const
{
	if (!RVNCompPtr)
	{
		return;
	}

	RVNCompPtr->OnDialogueContentChanged(InNodeId, InNewContent);
}

void URVNDialogueGraph::OnIsPlayerChanged(int32 InNodeId, bool bIsPlayer) const
{
	if (!RVNCompPtr)
	{
		return;
	}

	RVNCompPtr->OnIsPlayerChanged(InNodeId, bIsPlayer);
}

void URVNDialogueGraph::OnPositionChanged(int32 InNodeId, const FVector2D& InNewPosition)
{
	if (!RVNCompPtr)
	{
		return;
	}

	if (bIsTimerActive)
	{
		return;
	}

	if (RVNCompPtr)
	{
		RVNCompPtr->OnPositionChanged(InNodeId, InNewPosition);

		FRVNNodeData NodeData;
		RVNCompPtr->GetNodeData(InNodeId, NodeData);

		if (NodeData.bValid)
		{
			RVNCompPtr->RequestNodeSort([this](const TMap<int32, int32>& InSortCallback)
			{
				OnStateNodeIdChanged(InSortCallback);
			});
		}
	}
}

void URVNDialogueGraph::OnPropertyChanged(const FPropertyChangedEvent& InPropertyChangedEvent)
{
	for (int Index = 0; Index < InPropertyChangedEvent.GetNumObjectsBeingEdited(); ++Index)
	{
		const URVNStateNode* CurStateNode = Cast<URVNStateNode>(InPropertyChangedEvent.GetObjectBeingEdited(Index));

		if (!CurStateNode)
		{
			continue;
		}

		CurStateNode->OnPropertyEdited(InPropertyChangedEvent);
	}
}

TSharedPtr<SGraphNode> URVNDialogueGraph::GetNodeWidgetFromGuid(const FGuid InGuid) const
{
	return StaticCastSharedPtr<SGraphNode>(
		RVNEditorPtr.Pin()->GraphEditor->GetGraphPanel()->GetNodeWidgetFromGuid(InGuid));
}

void URVNDialogueGraph::SetSelectedDecorator(URVNDecorator* InDecorator)
{
	if (InDecorator == nullptr)
	{
		return;
	}

	if (!RVNEditorPtr.IsValid())
	{
		return;
	}

	const TSet Decorators = {Cast<UObject>(InDecorator)};

	RVNEditorPtr.Pin()->OnSelectedNodesChanged(Decorators);
}

void URVNDialogueGraph::RemoveStateNode(int32 InNodeId) const
{
	RVNCompPtr->RemoveNode(InNodeId);
}

void URVNDialogueGraph::AddCondition(int32 NodeId, URVNConditionBase* Condition) const
{
	RVNCompPtr->AddCondition(NodeId, Condition);
}

void URVNDialogueGraph::RemoveCondition(int32 NodeId, URVNConditionBase* Condition) const
{
	RVNCompPtr->RemoveCondition(NodeId, Condition);
}

void URVNDialogueGraph::AddTask(int32 NodeId, URVNTaskBase* Task) const
{
	RVNCompPtr->AddTask(NodeId, Task);
}

void URVNDialogueGraph::RemoveTask(int32 NodeId, URVNTaskBase* Task) const
{
	RVNCompPtr->RemoveTask(NodeId, Task);
}
