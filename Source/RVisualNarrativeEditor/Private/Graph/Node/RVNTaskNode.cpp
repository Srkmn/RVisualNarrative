#include "Graph/Node/RVNTaskNode.h"
#include "Graph/RVNDialogueGraph.h"
#include "Graph/RVNStateGraph.h"
#include "Graph/Node/RVNStateNode.h"

URVNTaskNode::URVNTaskNode()
	: FirstDecoratorIndex(-1)
{
}

URVNTaskNode::~URVNTaskNode()
{
}

void URVNTaskNode::InitTaskNode(URVNStateNode* InOwnerState, URVNTaskBase* InOwnerTask)
{
	OwnerState = InOwnerState;
	OwnerTask = InOwnerTask;
}

void URVNTaskNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "StatePin", "In");

	CreatePin(EGPD_Output, "StatePin", "Out");
}

void URVNTaskNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	RequestUpdateExecutionOrder();
}

UEdGraphPin* URVNTaskNode::GetInputPin() const
{
	return FindPin(TEXT("In"));
}

UEdGraphPin* URVNTaskNode::GetOutputPin() const
{
	return FindPin(TEXT("Out"));
}

URVNStateNode* URVNTaskNode::GetOwnerState()
{
	return OwnerState.Get();
}

URVNTaskBase* URVNTaskNode::GetOwnerTask()
{
	return OwnerTask.Get();
}

void URVNTaskNode::SetOwnerTask(URVNTaskBase* InTask)
{
	OwnerTask = InTask;
}

TSharedPtr<SGraphNode> URVNTaskNode::GetTaskWidget() const
{
	return StaticCastSharedPtr<SGraphNode>(OwnerState->GetDialogueGraph()->GetNodeWidgetFromGuid(NodeGuid));
}

void URVNTaskNode::AddCondition(URVNConditionBase* InCondition)
{
	if (InCondition == nullptr)
	{
		return;
	}

	OwnerTask->AddCondition(InCondition);

	OnAddCondition.ExecuteIfBound(InCondition);

	if (const auto StateGraph = Cast<URVNStateGraph>(GetGraph()))
	{
		StateGraph->ReorderTaskNodes();
	}
}

void URVNTaskNode::RemoveCondition(URVNConditionBase* InCondition)
{
	if (InCondition == nullptr)
	{
		return;
	}

	OwnerTask->RemoveCondition(InCondition);

	OnRemoveCondition.ExecuteIfBound(InCondition);

	if (const auto StateGraph = Cast<URVNStateGraph>(GetGraph()))
	{
		StateGraph->ReorderTaskNodes();
	}
}

void URVNTaskNode::RequestUpdateExecutionOrder()
{
	if (const auto StateGraph = Cast<URVNStateGraph>(GetGraph()))
	{
		StateGraph->OnPinConnectionChanged();
	}
}

int32 URVNTaskNode::ChangeExecutionOrder(int32 FirstIndex)
{
	FirstDecoratorIndex = FirstIndex;

	if (OnExecutionOrderChanged.IsBound())
	{
		return OnExecutionOrderChanged.Execute(FirstDecoratorIndex);
	}

	return FirstDecoratorIndex;
}
