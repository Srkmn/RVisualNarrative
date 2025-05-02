#pragma once

#include "CoreMinimal.h"
#include "Decorator/Task/RVNTask.h"
#include "EdGraph/EdGraphNode.h"
#include "RVNStateNode.generated.h"

class URVNStateGraph;class URVNDecorator;
class URVNTaskBase;
class URVNConditionBase;
class URVNDialogueGraph;
class SRVNStateWidget;
class URVNConditionNode;
class URVNTaskNode;
struct FRVNNodeData;

UENUM()
enum class ENodeType : uint8
{
	ENT_Entry,
	ENT_Dialogue,
	ENT_Select
};

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNStateNode : public UEdGraphNode
{
	GENERATED_BODY()

	DECLARE_DELEGATE(FOnNodeIdChanged);
	DECLARE_DELEGATE_OneParam(FOnAddCondition, URVNConditionBase*);
	DECLARE_DELEGATE_OneParam(FOnRemoveCondition, URVNConditionBase*);

public:
	URVNStateNode();

	void InitStateNode(FRVNNodeData& InData, ENodeType InType, TObjectPtr<URVNDialogueGraph> InGraph);

	// 数据访问接口
	FORCEINLINE TSharedPtr<SGraphNode> GetStateWidget() const;
	FORCEINLINE void SetDialogText(const FString& NewText);
	FORCEINLINE FString GetSpeakerName() const { return StateName; }
	FORCEINLINE FString GetDialogText() const { return StateContent; }
	FORCEINLINE bool IsEntryNode() const { return GetNodeType() == ENodeType::ENT_Entry; }
	FORCEINLINE bool IsDialogueNode() const { return GetNodeType() == ENodeType::ENT_Dialogue; }
	FORCEINLINE bool IsSelectNode() const { return GetNodeType() == ENodeType::ENT_Select; }
	FORCEINLINE ENodeType GetNodeType() const { return NodeType; }
	FORCEINLINE const TArray<URVNConditionBase*>& GetConditionNodes() const { return ConditionNodes; }
	FORCEINLINE TArray<URVNConditionBase*>& GetConditionNodes() { return ConditionNodes; }

	// 插槽节点管理
	void ProcessPasteStateNodes(FRVNNodeData& InNodeData);
	void PasteDecorator(const TArray<URVNDecorator*>& InDecorator);
	void PasteDecorator(URVNDecorator* InDecorator);
	void OnSelectedDecorator(URVNDecorator* InDecorator);
	void AddDecorator(URVNDecorator* InDecorator);
	void AddCondition(URVNConditionBase* InCondition);
	void AddTask(URVNTaskBase* InTask);
	void AddTaskReference(URVNTaskBase* InTask);
	void RemoveDecorator(URVNDecorator* InDecorator);
	void RemoveCondition(URVNConditionBase* InCondition);
	void RemoveTask(URVNTaskBase* InTask);
	void OnReorderTaskNodes(const TArray<URVNTaskBase*>& InTaskNodes);

	// Index
	int32 GetNodeId() const;
	void SetNodeId(const int32 NewId);

public:
	// UEdGraphNode interface  
	virtual void AllocateDefaultPins() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	UEdGraphPin* GetInputPin() const;
	UEdGraphPin* GetOutputPin() const;
	// End of UEdGraphNode interface

	// Changed From Detail
	void OnPropertyEdited(const FPropertyChangedEvent& InPropertyChangedEvent) const;
	void OnSpeakerNameChanged() const;
	void OnDialogueContentChanged() const;
	void OnIsPlayerChanged() const;
	void OnPositionChanged();
	// End of Detail Interface

	const URVNDialogueGraph* GetDialogueGraphConst() const;
	URVNDialogueGraph* GetDialogueGraph();

public:
	// State Graph
	TObjectPtr<URVNStateGraph> GetStateGraph();

	void OpenStateGraph();

public:
	UPROPERTY(VisibleAnywhere, Category = "Dialogue")
	int32 NodeId;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType == ENodeType::ENT_Dialogue"), Category = "Dialogue")
	FString StateName;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType != ENodeType::ENT_Entry"), Category = "Dialogue")
	FString StateContent;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType == ENodeType::ENT_Dialogue"), Category = "Dialogue")
	bool bIsPlayer;

	UPROPERTY(VisibleAnywhere, meta = (EditCondition = "NodeType != ENodeType::ENT_Entry"), Category = "Dialogue")
	TArray<URVNConditionBase*> ConditionNodes;

	UPROPERTY(VisibleAnywhere, Category = "Internal")
	ENodeType NodeType;

public:
	FOnNodeIdChanged OnNodeIdChangedCallback;
	FOnAddCondition OnAddConditionCallback;
	FOnRemoveCondition OnRemoveConditionCallback;

private:
	UPROPERTY()
	TWeakObjectPtr<URVNDialogueGraph> RVNDialogueGraph;

	UPROPERTY()
	TObjectPtr<URVNStateGraph> StateGraph;

	UPROPERTY()
	TArray<URVNStateNode*> InputConnections;

	UPROPERTY()
	TArray<URVNStateNode*> OutputConnections;
};
