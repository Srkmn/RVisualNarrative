#include "Graph/Node/RVNEntryNode.h"

#include "Graph/RVNStateGraph.h"
#include "Graph/Node/RVNTaskNode.h"

void URVNEntryNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, "EntryPin", "Out");
}

void URVNEntryNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	OutputConnection.Empty();

	if (UEdGraphPin* OutputPin = FindPin(TEXT("Out")))
	{
		for (UEdGraphPin* LinkedTo : OutputPin->LinkedTo)
		{
			if (URVNTaskNode* StateNode = Cast<URVNTaskNode>(LinkedTo->GetOwningNode()))
			{
				OutputConnection.Add(StateNode);
			}
		}
	}

	RequestUpdateExecutionOrder();
}

void URVNEntryNode::RequestUpdateExecutionOrder()
{
	if (const auto StateGraph = Cast<URVNStateGraph>(GetGraph()))
	{
		StateGraph->OnPinConnectionChanged();
	}
}
