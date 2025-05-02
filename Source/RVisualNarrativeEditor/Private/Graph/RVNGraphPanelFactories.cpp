#include "Graph/RVNGraphPanelFactories.h"
#include "EdGraph/EdGraphNode.h"
#include "Graph/Node/RVNEntryNode.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Node/RVNTaskNode.h"
#include "Graph/Node/Slate/SRVNEntryWidget.h"
#include "Graph/Node/Slate/SRVNStateWidget.h"
#include "Graph/Node/Slate/SRVNTaskWidget.h"

TSharedPtr<SGraphNode> FRVNGraphNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
	if (URVNStateNode* StateNode = Cast<URVNStateNode>(InNode))
	{
		return SNew(SRVNStateWidget, StateNode);
	}

	if (URVNEntryNode* EntryNode = Cast<URVNEntryNode>(InNode))
	{
		return SNew(SRVNEntryWidget, EntryNode);
	}

	if (URVNTaskNode* TaskNode = Cast<URVNTaskNode>(InNode))
	{
		return SNew(SRVNTaskWidget, TaskNode);
	}

	return nullptr;
}
