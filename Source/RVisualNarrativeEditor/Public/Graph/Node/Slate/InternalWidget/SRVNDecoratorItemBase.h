#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class URVNDecorator;
class URVNTaskNode;

DECLARE_DELEGATE_OneParam(FOnDecoratorSelected, URVNDecorator*);
DECLARE_DELEGATE_OneParam(FOnDecoratorDeleted, URVNDecorator*);

class SRVNDecoratorItemBase : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRVNDecoratorItemBase)
			: _Decorator(nullptr)
			  , _IsSelected(false)
		{
		}

		SLATE_ARGUMENT(URVNTaskNode*, OwnerTaskNode)
		SLATE_ARGUMENT(URVNDecorator*, Decorator)
		SLATE_ARGUMENT(int32, FirstItemIndex)
		SLATE_ARGUMENT(bool, IsSelected)
		SLATE_EVENT(FOnDecoratorSelected, OnDecoratorSelected)
		SLATE_EVENT(FOnDecoratorDeleted, OnDecoratorDeleted)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void SetIsSelected(bool bInIsSelected);
	virtual bool IsSelected() const { return bIsSelected; }
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

	URVNDecorator* GetDecorator() const { return DecoratorPtr.Get(); }

	FORCEINLINE int32 GetItemIndex() const { return ItemIndex; }

	FORCEINLINE void SetItemIndex(int32 InItemIndex) { ItemIndex = InItemIndex; }

protected:
	TObjectPtr<URVNTaskNode> OwnerTaskNodePtr;

	TObjectPtr<URVNDecorator> DecoratorPtr;

	FOnDecoratorSelected OnDecoratorSelectedDelegate;
	FOnDecoratorDeleted OnDecoratorDeletedDelegate;

	bool bIsSelected;
	int32 ItemIndex;

	virtual FSlateColor GetBackgroundColor() const;
	virtual FSlateColor GetBorderColor() const;
	virtual FText GetItemText() const;
	virtual const FSlateBrush* GetDecoratorIconBrush() const;
};
