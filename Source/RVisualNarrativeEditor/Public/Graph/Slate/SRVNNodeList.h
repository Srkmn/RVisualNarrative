#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "GraphEditorDragDropAction.h"

class FRVNEditor;
class URVNDecorator;
class URVNStateNode;
class URVNDialogueGraph;

struct FRVNClassInfo;

struct FRVNNodeCategory
{
	FString CategoryName;
	TArray<TSharedPtr<FRVNClassInfo>> Nodes;
	bool bIsExpanded;

	FRVNNodeCategory(const FString& InName)
		: CategoryName(InName), bIsExpanded(true)
	{
	}
};

class FRVNNodeDragDropOp : public FGraphEditorDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FRVNNodeDragDropOp, FGraphEditorDragDropAction)

	UClass* DraggedNodeClass;

	TSharedPtr<SBorder> DecoratorWidget;

	TWeakObjectPtr<URVNStateNode> OverlappingNode;

	TWeakPtr<FRVNEditor> RVNEditorPtr;

	static TSharedRef<FRVNNodeDragDropOp> New(UClass* InNodeClass, TWeakPtr<FRVNEditor> InGraph);

	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override;

	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;

	virtual FCursorReply OnCursorQuery() override;
};

class SRVNNodeList : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SRVNNodeList)
		{
		}

		SLATE_ARGUMENT(FRVNEditor*, RVNEditor)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	void OnSearchTextChanged(const FText& NewText);

	void CollectNodes();

	void RefreshList();

	TSharedRef<SWidget> CreateCategoryWidget(TSharedPtr<FRVNNodeCategory> Category);
	TSharedRef<SWidget> CreateNodesForCategory(TSharedPtr<FRVNNodeCategory> Category);
	TSharedRef<SWidget> CreateNodeWidget(TSharedPtr<FRVNClassInfo> NodeInfo);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	TSharedPtr<FRVNClassInfo> GetNodeUnderMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

private:
	TWeakPtr<FRVNEditor> RVNEditorPtr;

	TMap<FString, TSharedPtr<FRVNNodeCategory>> NodeCategories;
	TArray<TSharedPtr<FRVNNodeCategory>> FilteredCategories;
	TSharedPtr<FRVNClassInfo> HoveredNode;
	TMap<TSharedPtr<FRVNClassInfo>, TSharedRef<SWidget>> NodeWidgets;

	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<SVerticalBox> CategoryList;
};
