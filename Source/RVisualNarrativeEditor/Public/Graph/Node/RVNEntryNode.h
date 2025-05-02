#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "RVNEntryNode.generated.h"

class URVNTaskNode;

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNEntryNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	void RequestUpdateExecutionOrder();

public:
	UPROPERTY()
	TArray<TObjectPtr<URVNTaskNode>> OutputConnection;
};
