﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RVNComponent.generated.h"

class URVNDecorator;
class URVNConditionBase;
class URVNTaskBase;
class URVNDialogueManager;

USTRUCT(BlueprintType)
struct RVISUALNARRATIVE_API FRVNNodeData
{
	GENERATED_BODY()

	/** The editable or viewable portion */
	UPROPERTY(BlueprintReadOnly, Category = "State Node")
	int32 NodeId;

	UPROPERTY(BlueprintReadOnly, Category = "State Node")
	bool bIsPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "State Node")
	FString StateName;

	UPROPERTY(BlueprintReadOnly, Category = "State Node")
	FString StateContent;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSelector;

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> NextNodesId;

	/** Internal properties */
	UPROPERTY()
	bool bValid;

	UPROPERTY()
	FVector2D NodePosition;

	UPROPERTY()
	TArray<URVNConditionBase*> Conditions;

	UPROPERTY()
	TArray<URVNTaskBase*> Tasks;

	FRVNNodeData()
		: NodeId(INDEX_NONE)
		  , bIsPlayer(true)
		  , bIsSelector(false)
		  , bValid(false)
		  , NodePosition(FVector2D::ZeroVector)
	{
	}

	bool operator<(const FRVNNodeData& Other) const
	{
		return NodePosition.Y != Other.NodePosition.Y
			       ? NodePosition.Y < Other.NodePosition.Y
			       : NodePosition.X < Other.NodePosition.X;
	}

	FRVNNodeData& operator +=(const FRVNNodeData& Other)
	{
		UScriptStruct* ScriptStruct = StaticStruct();

		const auto StructSize = ScriptStruct->GetStructureSize() ? ScriptStruct->GetStructureSize() : 1;

		const auto DefaultStructure = reinterpret_cast<FRVNNodeData*>(static_cast<uint8*>(
			FMemory::Malloc(StructSize, ScriptStruct->GetMinAlignment())));

		ON_SCOPE_EXIT
		{
			ScriptStruct->DestroyStruct(DefaultStructure);
			FMemory::Free(DefaultStructure);
		};

		ScriptStruct->InitializeStruct(DefaultStructure);

		for (TFieldIterator<FProperty> PropIt(ScriptStruct); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;

			const void* DefaultValue = Property->ContainerPtrToValuePtr<void>(DefaultStructure);
			const void* SourceValue = Property->ContainerPtrToValuePtr<void>(&Other);
			void* TargetValue = Property->ContainerPtrToValuePtr<void>(this);

			if (!Property->Identical(DefaultValue, SourceValue))
			{
				Property->CopyCompleteValue(TargetValue, SourceValue);
			}
		}

		return *this;
	}
};

UCLASS(BlueprintType)
class RVISUALNARRATIVE_API URVNComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URVNComponent();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	URVNDialogueManager* GetDialogueManager();

	bool GetNodeData(int32 NodeId, FRVNNodeData& OutNode);

private:
	bool GetNodeIndex(int32 NodeId, int32& OutIndex) const;

	UFUNCTION()
	void OnDialogueComplete();

#if WITH_EDITOR

public:
	FRVNNodeData& CreateNode(const FVector2d& InPosition);

	URVNDecorator* CreateDecorator(const UClass* InDecoratorClass) const;

	void RemoveNode(int32 NodeId);

	void ConnectNodes(int32 SourceNodeId, int32 TargetNodeId);

	void OnPinConnectionChanged(int32 InNodeId, const TArray<int32>& NextNodesId);

	void OnSpeakerNameChanged(int32 InNodeId, const FString& InNewName);

	void OnDialogueContentChanged(int32 InNodeId, const FString& InNewContent);

	void OnIsPlayerChanged(int32 InNodeId, bool bIsPlayer);

	void OnPositionChanged(int32 InNodeId, const FVector2D& InNewPosition);

	void RequestNodeSort(TFunction<void(const TMap<int32, int32>&)> InSortCallback);

	void CompactNodesData();

	void DebugPrintNodeTree();

	UFUNCTION()
	void AddCondition(int32 NodeId, URVNConditionBase* Condition);

	UFUNCTION()
	void RemoveCondition(int32 NodeId, URVNConditionBase* Condition);

	UFUNCTION()
	void AddTask(int32 NodeId, URVNTaskBase* Task);

	UFUNCTION()
	void RemoveTask(int32 NodeId, URVNTaskBase* Task);

private:
	void InvalidateNode(int32 Index);

	void SortNodesByPosition(TArray<int32>& NodeIds);

	bool GetParentNodeIndex(int32 InNodeId, int32& OutIndex);
#endif

public:
	TObjectPtr<URVNDialogueManager> DialogueManager;

	UPROPERTY()
	TArray<FRVNNodeData> Nodes;

private:
	UPROPERTY()
	TMap<int32, int32> NodeIdToIndex;
};
