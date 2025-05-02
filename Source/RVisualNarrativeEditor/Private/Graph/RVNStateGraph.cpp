#include "Graph/RVNStateGraph.h"
#include "Graph/RVNDialogueGraph.h"
#include "Graph/Node/RVNEntryNode.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Node/RVNTaskNode.h"

struct FTaskSort
{
	FORCEINLINE bool operator()(const URVNTaskNode& A,
	                            const URVNTaskNode& B) const
	{
		return A.NodePosY != B.NodePosY ? A.NodePosY < B.NodePosY : A.NodePosX < B.NodePosX;
	}
};

URVNStateGraph::URVNStateGraph()
{
}

URVNStateGraph::~URVNStateGraph()
{
	FCoreDelegates::OnEndFrame.Remove(OnEndFrameDelegateHandle);
}

URVNDecorator* URVNStateGraph::CreateDecorator(const UClass* InDecoratorClass, UObject* InOuter) const
{
	if (OwnerStateNode == nullptr || OwnerStateNode->GetDialogueGraph() == nullptr)
	{
		return nullptr;
	}

	return OwnerStateNode->GetDialogueGraph()->CreateDecorator(InDecoratorClass, InOuter);
}

URVNEntryNode* URVNStateGraph::CreateEntryNode(const FVector2D& InPosition)
{
	FGraphNodeCreator<URVNEntryNode> NodeCreator(*this);

	URVNEntryNode* NewNode = NodeCreator.CreateNode();

	NewNode->NodePosX = InPosition.X;
	NewNode->NodePosY = InPosition.Y;

	NodeCreator.Finalize();

	return NewNode;
}

void URVNStateGraph::ProcessPasteTaskNodes(const TArray<URVNTaskNode*>& InNodes)
{
	TMap<URVNTaskNode*, URVNTaskBase*> NodeToTaskMap;

	auto DFS = [&](auto& Self, URVNTaskNode* CurrentNode) -> URVNTaskBase* {
		if (!IsValid(CurrentNode))
		{
			return nullptr;
		}

		if (const auto ExistingTask = NodeToTaskMap.Find(CurrentNode))
		{
			return *ExistingTask;
		}

		const auto CloneTask = CurrentNode->GetOwnerTask()->PasteTaskNoChildren();

		NodeToTaskMap.Add(CurrentNode, CloneTask);

		OwnerStateNode->AddTaskReference(CloneTask);

		TArray<URVNTaskNode*> NextNodes;

		if (const auto Pin = CurrentNode->GetOutputPin())
		{
			for (const auto Connection : Pin->LinkedTo)
			{
				if (const auto NextTaskNode = Cast<URVNTaskNode>(Connection->GetOwningNode()))
				{
					NextNodes.Add(NextTaskNode);
				}
			}
		}

		NextNodes.Sort(FTaskSort());

		TArray<TObjectPtr<URVNTaskBase>> NextTasks;

		for (const auto& NextNode : NextNodes)
		{
			NextTasks.Add(TObjectPtr<URVNTaskBase>(Self(Self, NextNode)));
		}

		CloneTask->SetChildren(NextTasks);

		CurrentNode->SetOwnerTask(CloneTask);

		return CloneTask;
	};

	TArray<URVNTaskNode*> AllTaskNodes = InNodes;
	TArray<URVNTaskNode*> RootTaskNodes;
	TArray<URVNTaskNode*> NormalTaskNodes;

	if (InNodes.IsEmpty())
	{
		RootTaskNodes.Append(Entry->OutputConnection);

		for (const auto& Node : Nodes)
		{
			if (const auto TaskNode = Cast<URVNTaskNode>(Node))
			{
				AllTaskNodes.Add(TaskNode);
			}
		}
	}

	for (const auto& TaskNode : AllTaskNodes)
	{
		if (!TaskNode->GetInputPin()->HasAnyConnections())
		{
			RootTaskNodes.Add(TaskNode);
		}
		else
		{
			NormalTaskNodes.Add(TaskNode);
		}
	}

	for (const auto& RootTaskNode : RootTaskNodes)
	{
		const auto NewTask = DFS(DFS, RootTaskNode);

		if (Entry->OutputConnection.Contains(RootTaskNode))
		{
			OwnerStateNode->AddDecorator(NewTask);
		}
	}

	// Nodes that cannot be reached through the root node, such as circular references
	for (const auto& TaskNode : NormalTaskNodes)
	{
		DFS(DFS, TaskNode);
	}

	NotifyGraphChanged();
}

void URVNStateGraph::CreateTaskNode(const FVector2D& InPosition, URVNTaskBase* InTask)
{
	Modify();
	SetFlags(RF_Transactional);

	FGraphNodeCreator<URVNTaskNode> NodeCreator(*this);

	URVNTaskNode* NewNode = NodeCreator.CreateNode();

	NewNode->NodePosX = InPosition.X;
	NewNode->NodePosY = InPosition.Y;

	NewNode->InitTaskNode(OwnerStateNode, InTask);
	NewNode->SetFlags(RF_Transactional);

	NodeCreator.Finalize();

	NotifyGraphChanged();
}

void URVNStateGraph::DeleteTaskNode(URVNTaskNode* InNode)
{
	if (!IsValid(InNode))
	{
		return;
	}

	if (!IsValid(InNode->GetOwnerState()))
	{
		return;
	}

	Modify();
	SetFlags(RF_Transactional);

	InNode->GetOwnerState()->RemoveDecorator(InNode->GetOwnerTask());
}

void URVNStateGraph::OnPinConnectionChanged()
{
	if (LastRequestFrame == GFrameNumber)
	{
		return;
	}

	LastRequestFrame = GFrameNumber;

	OnEndFrameDelegateHandle = FCoreDelegates::OnEndFrame.AddUObject(this, &URVNStateGraph::ReorderTaskNodes);
}

void URVNStateGraph::ReorderTaskNodes()
{
	check(IsValid(Entry))

	FCoreDelegates::OnEndFrame.Remove(OnEndFrameDelegateHandle);

	TSet<URVNTaskNode*> VisitedNodes;
	int32 CurrentOrder = 0;

	auto DFS = [&](auto& Self, URVNTaskNode* CurrentNode)
	{
		if (!IsValid(CurrentNode) || VisitedNodes.Contains(CurrentNode))
		{
			return;
		}

		VisitedNodes.Add(CurrentNode);

		CurrentOrder = CurrentNode->ChangeExecutionOrder(CurrentOrder);

		TArray<URVNTaskNode*> NextNodes;

		for (auto& Pin : CurrentNode->Pins)
		{
			if (Pin->Direction == EGPD_Output)
			{
				for (const auto Connection : Pin->LinkedTo)
				{
					if (const auto NextTaskNode = Cast<URVNTaskNode>(Connection->GetOwningNode()))
					{
						NextNodes.Add(NextTaskNode);
					}
				}
			}
		}

		if (!NextNodes.IsEmpty())
		{
			NextNodes.Sort(FTaskSort());
		}
		else
		{
			return;
		}

		TArray<URVNTaskBase*> NextTasks;

		for (auto& NextNode : NextNodes)
		{
			Self(Self, NextNode);

			NextTasks.Add(NextNode->GetOwnerTask());
		}

		CurrentNode->GetOwnerTask()->GetChildren() = NextTasks;
	};

	TArray<URVNTaskNode*> RootTaskNodes;

	for (const auto RootTaskNode : Entry->OutputConnection)
	{
		RootTaskNodes.Add(RootTaskNode);
	}

	RootTaskNodes.Sort(FTaskSort());

	TArray<URVNTaskBase*> RootTasks;

	for (const auto RootTaskNode : RootTaskNodes)
	{
		RootTasks.Add(RootTaskNode->GetOwnerTask());

		DFS(DFS, RootTaskNode);
	}

	OwnerStateNode->OnReorderTaskNodes(RootTasks);
}
