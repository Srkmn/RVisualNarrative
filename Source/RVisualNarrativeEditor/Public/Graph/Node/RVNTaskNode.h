#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "RVNTaskNode.generated.h"

class URVNConditionBase;
class URVNStateNode;
class URVNTaskBase;

DECLARE_DELEGATE_RetVal_OneParam(int32, FOnExecutionOrderChanged, int32);

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNTaskNode : public UEdGraphNode
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(FOnAddCondition, URVNConditionBase*);
	DECLARE_DELEGATE_OneParam(FOnRemoveCondition, URVNConditionBase*);

public:
	URVNTaskNode();

	~URVNTaskNode();

	void InitTaskNode(URVNStateNode* InOwnerState, URVNTaskBase* InOwnerTask);

	virtual void AllocateDefaultPins() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	UEdGraphPin* GetInputPin() const;

	UEdGraphPin* GetOutputPin() const;

	URVNStateNode* GetOwnerState();

	URVNTaskBase* GetOwnerTask();

	void SetOwnerTask(URVNTaskBase* InTask);

	TSharedPtr<SGraphNode> GetTaskWidget() const;

	void AddCondition(URVNConditionBase* InCondition);

	void RemoveCondition(URVNConditionBase* InCondition);

	void RequestUpdateExecutionOrder();

	int32 ChangeExecutionOrder(int32 FirstIndex);

public:
	FOnAddCondition OnAddCondition;
	FOnRemoveCondition OnRemoveCondition;
	FOnExecutionOrderChanged OnExecutionOrderChanged;

	UPROPERTY()
	int32 FirstDecoratorIndex;

private:
	UPROPERTY()
	TWeakObjectPtr<URVNStateNode> OwnerState;

	UPROPERTY()
	TWeakObjectPtr<URVNTaskBase> OwnerTask;
};
