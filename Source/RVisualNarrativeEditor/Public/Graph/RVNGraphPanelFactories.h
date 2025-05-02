#pragma once
#include "EdGraphUtilities.h"

class SGraphNode;

struct FRVNGraphNodeFactory : FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* InNode) const override;
};
