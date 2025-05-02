#include "RVNComponent.h"
#include "RVNDialogueManager.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNTask.h"

#define LOCTEXT_NAMESPACE "RVNComponent"

URVNComponent::URVNComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

URVNDialogueManager* URVNComponent::GetDialogueManager()
{
	if (DialogueManager == nullptr)
	{
		DialogueManager = NewObject<URVNDialogueManager>(this);

		DialogueManager->Initialize(this);
	}

	return DialogueManager.Get();
}

URVNBlackboardData* URVNComponent::GetBlackboardAsset() const
{
	return BlackboardData;
}

bool URVNComponent::GetNodeData(int32 NodeId, FRVNNodeData& OutNode)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return false;
	}

	OutNode = Nodes[Index];

	return true;
}

void URVNComponent::OnDialogueComplete()
{
	DialogueManager = nullptr;
}

#if WITH_EDITOR
void URVNComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName NAME_BlackboardData = GET_MEMBER_NAME_CHECKED(URVNComponent, BlackboardData);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == NAME_BlackboardData)
		{
			OnBlackboardDataChanged.ExecuteIfBound(BlackboardData);
		}
	}
}
#endif

bool URVNComponent::GetNodeIndex(int32 NodeId, int32& OutIndex) const
{
	const int32* IndexPtr = NodeIdToIndex.Find(NodeId);
	if (!IndexPtr || !Nodes.IsValidIndex(*IndexPtr) || !Nodes[*IndexPtr].bValid)
	{
		return false;
	}

	OutIndex = *IndexPtr;
	return true;
}

#if WITH_EDITOR
FRVNNodeData& URVNComponent::CreateNode(const FVector2d& InPosition)
{
	Modify();

	FRVNNodeData NewNode;
	NewNode.NodeId = Nodes.Num();
	NewNode.bIsPlayer = true;
	NewNode.StateName = "SpeakerName";
	NewNode.StateContent = "Edit Dialogue Content";
	NewNode.NodePosition = InPosition;
	NewNode.bValid = true;

	const int32 NewIndex = Nodes.Add(NewNode);
	NodeIdToIndex.Add(NewNode.NodeId, NewIndex);

	return Nodes[NewIndex];
}

URVNDecorator* URVNComponent::CreateDecorator(const UClass* InDecoratorClass, UObject* InOuter) const
{
	return NewObject<URVNDecorator>(InOuter == nullptr ? GetPackage() : InOuter, InDecoratorClass);
}

void URVNComponent::RemoveNode(int32 NodeId)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	InvalidateNode(Index);

	for (FRVNNodeData& Node : Nodes)
	{
		if (Node.bValid)
		{
			Node.NextNodesId.Remove(NodeId);
		}
	}

	NodeIdToIndex.Remove(NodeId);
}

void URVNComponent::ConnectNodes(int32 SourceNodeId, int32 TargetNodeId)
{
	int32 SourceIndex;
	if (!GetNodeIndex(SourceNodeId, SourceIndex))
	{
		return;
	}

	int32 TargetIndex;
	if (!GetNodeIndex(TargetNodeId, TargetIndex))
	{
		return;
	}

	Modify();

	if (!Nodes[SourceIndex].NextNodesId.Contains(TargetNodeId))
	{
		Nodes[SourceIndex].NextNodesId.Add(TargetNodeId);
	}
}

void URVNComponent::OnPinConnectionChanged(int32 InNodeId, const TArray<int32>& NextNodesId)
{
	int32 Index;
	if (!GetNodeIndex(InNodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].NextNodesId = NextNodesId;
}

void URVNComponent::OnSpeakerNameChanged(int32 InNodeId, const FString& InNewName)
{
	int32 Index;
	if (!GetNodeIndex(InNodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].StateName = InNewName;
}

void URVNComponent::OnDialogueContentChanged(int32 InNodeId, const FString& InNewContent)
{
	int32 Index;
	if (!GetNodeIndex(InNodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].StateContent = InNewContent;
}

void URVNComponent::OnIsPlayerChanged(int32 InNodeId, bool bIsPlayer)
{
	int32 Index;
	if (!GetNodeIndex(InNodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].bIsPlayer = bIsPlayer;
}

void URVNComponent::OnPositionChanged(int32 InNodeId, const FVector2D& InNewPosition)
{
	int32 Index;
	if (!GetNodeIndex(InNodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].NodePosition = InNewPosition;
}

void URVNComponent::RequestNodeSort(TFunction<void(const TMap<int32, int32>&)> InSortCallback)
{
	check(!Nodes.IsEmpty());

	Modify();

	TSet<int32> VisitedNodes;
	TArray<int32> SortedOrder;
	TMap<int32, int32> OldToNewId;

	auto DFS = [&](auto& Self, int32 OldNodeId)
	{
		if (VisitedNodes.Contains(OldNodeId))
		{
			return;
		}

		VisitedNodes.Add(OldNodeId);

		// First, add the current node to the sorting sequence 
		const int32 NewNodeId = SortedOrder.Num();
		SortedOrder.Add(OldNodeId);
		OldToNewId.Add(OldNodeId, NewNodeId);

		// Obtain parallel ordered child nodes  
		int32 NodeIndex;
		check(GetNodeIndex(OldNodeId, NodeIndex));
		TArray<int32> SortedNextNodes = Nodes[NodeIndex].NextNodesId;
		SortNodesByPosition(SortedNextNodes);

		// Reprocess all child nodes  
		for (int32 NextNodeId : SortedNextNodes)
		{
			if (!VisitedNodes.Contains(NextNodeId))
			{
				Self(Self, NextNodeId);
			}
		}
	};

	DFS(DFS, 0);

	// Get unreachable nodes  
	TArray<int32> UnreachableNodeIds;
	for (const FRVNNodeData& Node : Nodes)
	{
		if (!VisitedNodes.Contains(Node.NodeId) && Node.bValid)
		{
			UnreachableNodeIds.Add(Node.NodeId);
		}
	}

	// Sort unreachable nodes by original ID
	UnreachableNodeIds.Sort();

	// Find nodes in unreachable nodes that do not have a parent node  
	TArray<int32> RootNodes;
	for (int32 NodeId : UnreachableNodeIds)
	{
		int32 ParentIndex;
		if (!GetParentNodeIndex(NodeId, ParentIndex))
		{
			RootNodes.Add(NodeId);
		}
	}

	for (int32 RootNodeId : RootNodes)
	{
		DFS(DFS, RootNodeId);
	}

	// Update NodeIdToIndex mapping  
	TMap<int32, int32> NewIdToIndex;
	for (int32 i = 0; i < SortedOrder.Num(); ++i)
	{
		int32 Index;
		check(GetNodeIndex(SortedOrder[i], Index));
		NewIdToIndex.Add(i, Index);
	}
	NodeIdToIndex = NewIdToIndex;

	// Update the IDs and NextNodesId of all nodes 
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		const int32* NewId = OldToNewId.Find(Nodes[i].NodeId);
		if (NewId)
		{
			Nodes[i].NodeId = *NewId;

			for (int32& NextNodeId : Nodes[i].NextNodesId)
			{
				if (const int32* NewNextId = OldToNewId.Find(NextNodeId))
				{
					NextNodeId = *NewNextId;
				}
			}
		}
	}

	InSortCallback(OldToNewId);
}

void URVNComponent::CompactNodesData()
{
	Modify();

	TArray<FRVNNodeData> CompactedNodes;

	for (const auto& Node : Nodes)
	{
		if (Node.bValid)
		{
			CompactedNodes.Add(Node);
		}
	}

	CompactedNodes.Sort([](const FRVNNodeData& A, const FRVNNodeData& B)
	{
		return A.NodeId < B.NodeId;
	});

	TMap<int32, int32> NewNodeIdToIndex;

	for (int32 Index = 0; Index < CompactedNodes.Num(); ++Index)
	{
		NewNodeIdToIndex.Add(CompactedNodes[Index].NodeId, Index);
	}

	Nodes = CompactedNodes;

	NodeIdToIndex = NewNodeIdToIndex;
}

void URVNComponent::AddCondition(int32 NodeId, URVNConditionBase* Condition)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].Conditions.AddUnique(Condition);
}

void URVNComponent::RemoveCondition(int32 NodeId, URVNConditionBase* Condition)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].Conditions.Remove(Condition);
}

void URVNComponent::AddTask(int32 NodeId, URVNTaskBase* Task)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].Tasks.AddUnique(Task);
}

void URVNComponent::AddTaskReference(int32 NodeId, URVNTaskBase* Task)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].TaskReferences.AddUnique(Task);
}

void URVNComponent::RemoveTask(int32 NodeId, URVNTaskBase* Task)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].Tasks.Remove(Task);
}

void URVNComponent::RemoveTaskReference(int32 NodeId, URVNTaskBase* Task)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].TaskReferences.Remove(Task);
}

void URVNComponent::OnReorderTaskNodes(int32 NodeId, const TArray<URVNTaskBase*>& InTaskNodes)
{
	int32 Index;
	if (!GetNodeIndex(NodeId, Index))
	{
		return;
	}

	Modify();

	Nodes[Index].Tasks = InTaskNodes;
}

void URVNComponent::InvalidateNode(int32 Index)
{
	if (!Nodes.IsValidIndex(Index))
	{
		return;
	}

	NodeIdToIndex.Remove(Nodes[Index].NodeId);

	Nodes[Index].NodeId = INDEX_NONE;
	Nodes[Index].bValid = false;
	Nodes[Index].StateName.Empty();
	Nodes[Index].StateContent.Empty();
	Nodes[Index].NextNodesId.Empty();
	Nodes[Index].Conditions.Empty();
	Nodes[Index].Tasks.Empty();
	Nodes[Index].TaskReferences.Empty();
}

void URVNComponent::SortNodesByPosition(TArray<int32>& NodeIds)
{
	NodeIds.Sort([this](const int32 A, const int32 B)
	{
		FRVNNodeData NodeA, NodeB;
		if (!GetNodeData(A, NodeA) || !GetNodeData(B, NodeB))
			return false;

		return NodeA < NodeB;
	});
}

bool URVNComponent::GetParentNodeIndex(int32 InNodeId, int32& OutIndex)
{
	for (int32 Index = 0; Index < Nodes.Num(); ++Index)
	{
		if (!Nodes[Index].bValid)
		{
			continue;
		}

		if (Nodes[Index].NextNodesId.Contains(InNodeId))
		{
			OutIndex = Index;
			return true;
		}
	}

	return false;
}

void URVNComponent::DebugPrintNodeTree()
{
	if (Nodes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No nodes in component!"));
		return;
	}

	TQueue<int32> NodeQueue;
	TSet<int32> VisitedNodeIds;
	TMap<int32, int32> NodeLevels;
	FString FullOutput;

	FRVNNodeData EntryNode;
	if (!GetNodeData(Nodes[0].NodeId, EntryNode))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Entry Node data!"));
		return;
	}

	NodeQueue.Enqueue(EntryNode.NodeId);
	NodeLevels.Add(EntryNode.NodeId, 0);

	while (!NodeQueue.IsEmpty())
	{
		int32 CurrentNodeId;
		NodeQueue.Dequeue(CurrentNodeId);

		if (VisitedNodeIds.Contains(CurrentNodeId))
		{
			continue;
		}

		VisitedNodeIds.Add(CurrentNodeId);

		FRVNNodeData CurrentNode;
		if (!GetNodeData(CurrentNodeId, CurrentNode))
		{
			continue;
		}

		const int32 CurrentLevel = NodeLevels[CurrentNodeId];

		FString Indent;
		for (int32 i = 0; i < CurrentLevel; ++i)
		{
			Indent += TEXT("    ");
		}

		FullOutput += FString::Printf(TEXT("\n%s[Node %d] {\n"), *Indent, CurrentNodeId);
		FullOutput += FString::Printf(TEXT("%s    NodeId: %d\n"), *Indent, CurrentNode.NodeId);
		FullOutput += FString::Printf(
			TEXT("%s    Valid: %s\n"), *Indent, CurrentNode.bValid ? TEXT("True") : TEXT("False"));
		FullOutput += FString::Printf(
			TEXT("%s    StateName: %s\n"), *Indent, *CurrentNode.StateName);
		FullOutput += FString::Printf(
			TEXT("%s    StateContent: %s\n"), *Indent, *CurrentNode.StateContent);
		FullOutput += FString::Printf(
			TEXT("%s    IsSelector: %s\n"), *Indent, CurrentNode.bIsSelector ? TEXT("True") : TEXT("False"));
		FullOutput += FString::Printf(
			TEXT("%s    IsPlayer: %s\n"), *Indent, CurrentNode.bIsPlayer ? TEXT("True") : TEXT("False"));

		FullOutput += FString::Printf(TEXT("%s    NextNodes: ["), *Indent);
		for (int32 i = 0; i < CurrentNode.NextNodesId.Num(); ++i)
		{
			FullOutput += FString::Printf(TEXT("%d"), CurrentNode.NextNodesId[i]);
			if (i < CurrentNode.NextNodesId.Num() - 1)
			{
				FullOutput += TEXT(", ");
			}
		}
		FullOutput += TEXT("]\n");

		FullOutput += FString::Printf(TEXT("%s    Tasks: [\n"), *Indent);
		for (const auto& Task : CurrentNode.Tasks)
		{
			FullOutput += FString::Printf(TEXT("%s        %s\n"), *Indent, *Task->GetName());
		}
		FullOutput += FString::Printf(TEXT("%s    ]\n"), *Indent);

		FullOutput += FString::Printf(TEXT("%s    Conditions: [\n"), *Indent);
		for (const auto& Condition : CurrentNode.Conditions)
		{
			FullOutput += FString::Printf(TEXT("%s        %s\n"), *Indent, *Condition->GetName());
		}
		FullOutput += FString::Printf(TEXT("%s    ]\n"), *Indent);

		FullOutput += FString::Printf(TEXT("%s}\n"), *Indent);

		for (int32 NextNodeId : CurrentNode.NextNodesId)
		{
			NodeQueue.Enqueue(NextNodeId);
			NodeLevels.Add(NextNodeId, CurrentLevel + 1);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RVN Node Tree:%s"), *FullOutput);
}

#endif

#undef LOCTEXT_NAMESPACE
