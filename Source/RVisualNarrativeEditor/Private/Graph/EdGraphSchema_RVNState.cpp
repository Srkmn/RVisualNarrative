#include "Graph/EdGraphSchema_RVNState.h"
#include "Graph/RVNStateGraph.h"
#include "Graph/Node/RVNEntryNode.h"
#include "Graph/Node/RVNTaskNode.h"

void UEdGraphSchema_RVNState::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	URVNStateGraph* CurrentGraph = CastChecked<URVNStateGraph>(&Graph);

	if (CurrentGraph == nullptr)
	{
		return;
	}

	const auto Location = CurrentGraph->GetGoodPlaceForNewNode();
	const auto EntryNode = CurrentGraph->CreateEntryNode(Location);

	SetNodeMetaData(EntryNode, FNodeMetadata::DefaultGraphNode);

	CurrentGraph->Entry = EntryNode;
}

bool UEdGraphSchema_RVNState::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return false;
	}

	const auto NodeA = A->GetOwningNode();
	const auto NodeB = B->GetOwningNode();

	bool bEntryA = NodeA->IsA(URVNEntryNode::StaticClass());
	bool bEntryB = NodeB->IsA(URVNEntryNode::StaticClass());

	if (A->Direction == B->Direction)
	{
		if (!bEntryA && !bEntryB)
		{
			const auto TaskA = CastChecked<URVNTaskNode>(NodeA);
			const auto TaskB = CastChecked<URVNTaskNode>(NodeB);

			if (A->Direction == EGPD_Input)
			{
				A = TaskA->GetOutputPin();
			}
			else
			{
				B = TaskB->GetInputPin();
			}

			return UEdGraphSchema::TryCreateConnection(A, B);
		}

		if (bEntryB)
		{
			A = CastChecked<URVNTaskNode>(NodeA)->GetInputPin();
		}

		if (bEntryA)
		{
			B = CastChecked<URVNTaskNode>(NodeB)->GetInputPin();
		}
	}
	else if (A->Direction == EGPD_Input)
	{
		A = CastChecked<URVNTaskNode>(NodeB)->GetOutputPin();

		B = CastChecked<URVNTaskNode>(NodeA)->GetInputPin();
	}

	return UEdGraphSchema::TryCreateConnection(A, B);
}

const FPinConnectionResponse UEdGraphSchema_RVNState::CanCreateConnection(const UEdGraphPin* A,
                                                                          const UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect to self"));
	}

	const auto NodeA = A->GetOwningNode();
	const auto NodeB = B->GetOwningNode();

	bool bEntryA = NodeA->IsA(URVNEntryNode::StaticClass());
	bool bEntryB = NodeB->IsA(URVNEntryNode::StaticClass());
	bool bTaskA = NodeA->IsA(URVNTaskNode::StaticClass());
	bool bTaskB = NodeB->IsA(URVNTaskNode::StaticClass());

	if (bEntryA)
	{
		if (A->Direction == EGPD_Output && B->Direction == EGPD_Input && bTaskB)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Entry Can Connect To Task"));
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,TEXT("Entry Can Only Connect To Task"));
	}

	if (bEntryB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,TEXT("Can't Connect To Entry"));
	}

	if (A->Direction == EGPD_Output && B->Direction == EGPD_Input && bTaskA && bTaskB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Task Can Connect To Task"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid connection"));
}
