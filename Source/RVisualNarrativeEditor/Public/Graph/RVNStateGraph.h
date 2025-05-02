#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "RVNStateGraph.generated.h"

class URVNTaskNode;
class URVNTaskBase;
class URVNDecorator;
class URVNStateNode;
class URVNEntryNode;

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNStateGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	URVNStateGraph();

	~URVNStateGraph();

public:
	URVNDecorator* CreateDecorator(const UClass* InDecoratorClass, UObject* InOuter = nullptr) const;

	URVNEntryNode* CreateEntryNode(const FVector2D& InPosition);

	void ProcessPasteTaskNodes(const TArray<URVNTaskNode*>& InNodes = TArray<URVNTaskNode*>());

	void CreateTaskNode(const FVector2D& InPosition, URVNTaskBase* InTask);

	void DeleteTaskNode(URVNTaskNode* InNode);

	void OnPinConnectionChanged();

	void ReorderTaskNodes();

public:
	UPROPERTY()
	TObjectPtr<URVNStateNode> OwnerStateNode;

	UPROPERTY()
	TObjectPtr<URVNEntryNode> Entry;

private:
	int32 LastRequestFrame;

	FDelegateHandle OnEndFrameDelegateHandle;
};
