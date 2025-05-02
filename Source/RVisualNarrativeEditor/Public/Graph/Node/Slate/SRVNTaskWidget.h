#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "Graph/Node/Slate/InternalWidget/SRVNDecoratorItemBase.h"

class URVNTaskNode;
class URVNDecorator;
class URVNTaskBase;
class URVNConditionBase;

class SRVNTaskWidget : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SRVNTaskWidget)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URVNTaskNode* InNode);
	~SRVNTaskWidget();

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;

private:
	void OnDecoratorSelected(URVNDecorator* SelectedDecorator);
	void OnDecoratorDeleted(URVNDecorator* DecoratorToDelete);
	int32 HandleExecutionOrderChanged(int32 CurrentIndex);

	FDelegateHandle OnDecoratorSelectedHandle;
	FDelegateHandle OnDecoratorDeletedHandle;
	FDelegateHandle OnExecutionOrderChangedHandle;

private:
	TWeakObjectPtr<URVNTaskNode> OwnerTaskNodePtr;
	TWeakObjectPtr<URVNDecorator> SelectedDecoratorPtr;

	TSharedPtr<SVerticalBox> ConditionList;
	TSharedPtr<SVerticalBox> TaskList;

	TArray<TSharedPtr<SRVNDecoratorItemBase>> Decorators;

	int32 FirstDecoratorIndex;

	// 创建条件列表和任务区域  
	void CreateConditionsListWidget();
	void CreateTaskWidget();

	// 获取条件列表和任务  
	TArray<URVNConditionBase*> GetConditionsList() const;
	URVNTaskBase* GetTaskBase() const;
};
