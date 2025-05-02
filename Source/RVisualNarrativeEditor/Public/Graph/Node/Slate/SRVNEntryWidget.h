#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class URVNEntryNode;

class SRVNEntryWidget : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SRVNEntryWidget)
	{
	}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URVNEntryNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;
	
private:
	UPROPERTY()
	TObjectPtr<URVNEntryNode> OwnerEntryNodePtr;
};
