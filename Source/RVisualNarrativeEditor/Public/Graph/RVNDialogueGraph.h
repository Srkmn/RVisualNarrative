#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "RVNComponent.h"
#include "Graph/Node/RVNStateNode.h"
#include "RVNClassInfo.h"
#include "RVNDialogueGraph.generated.h"

class URVNComponent;
class FRVNEditor;
class URVNStateNode;
struct FRVNNodeData;

USTRUCT()
struct FRVNPasteNodeData
{
	GENERATED_BODY()

	UPROPERTY()
	FString StateName;

	UPROPERTY()
	FString StateContent;

	UPROPERTY()
	bool bIsPlayer;

	UPROPERTY()
	TArray<FRVNClassInfo> ConditionNode;

	UPROPERTY()
	TArray<FRVNClassInfo> TaskNode;

	UPROPERTY()
	ENodeType NodeType;

	UPROPERTY()
	FVector2D Position;
};

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNDialogueGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	URVNDialogueGraph();

	URVNStateNode* CreateStateNode(ENodeType InType, const FVector2D& InPosition);

	URVNDecorator* CreateDecorator(const UClass* InDecoratorClass, UObject* InOuter = nullptr) const;

	void ProcessPasteStateNodes(const TArray<URVNStateNode*>& InStateNodes);

	bool GetNodeData(int32 InNodeId, FRVNNodeData& OutData) const;

	void OnStateNodeIdChanged(TMap<int32, int32> InIdMap);

	void OnPinConnectionChanged(int32 InNodeId, const TArray<int32>& NextNodesId);

	void OnSpeakerNameChanged(int32 InNodeId, const FString& InNewName) const;

	void OnDialogueContentChanged(int32 InNodeId, const FString& InNewContent) const;

	void OnIsPlayerChanged(int32 InNodeId, bool bIsPlayer) const;

	void OnPositionChanged(int32 InNodeId, const FVector2D& InNewPosition);

	void OnPropertyChanged(const FPropertyChangedEvent& InPropertyChangedEvent);

	TSharedPtr<SGraphNode> GetNodeWidgetFromGuid(const FGuid InGuid) const;

	void SetSelectedDecorator(URVNDecorator* InDecorator);

	void RemoveStateNode(int32 InNodeId) const;

	void AddCondition(int32 NodeId, URVNConditionBase* Condition) const;

	void RemoveCondition(int32 NodeId, URVNConditionBase* Condition) const;

	void AddTask(int32 NodeId, URVNTaskBase* Task) const;

	void RemoveTask(int32 NodeId, URVNTaskBase* Task) const;

	void RefreshTasks(int32 InNodeId, const TArray<URVNTaskBase*>& InTaskNodes) const;

public:
	UPROPERTY()
	TObjectPtr<URVNComponent> RVNCompPtr;

	TWeakPtr<FRVNEditor> RVNEditorPtr;

	UPROPERTY()
	TObjectPtr<UEdGraphNode> Entry;

	UPROPERTY()
	TMap<int32, URVNStateNode*> IdToNodeMap;

private:
	FTimerHandle ThrottleTimerHandle;
	bool bIsTimerActive = false;
};
