#pragma once

#include "Graph/Node/Slate/InternalWidget/SRVNDecoratorItemBase.h"

class URVNTaskBase;

class SRVNTaskItemWidget : public SRVNDecoratorItemBase
{
public:
	SLATE_BEGIN_ARGS(SRVNTaskItemWidget)
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

protected:
	virtual FSlateColor GetBackgroundColor() const override;
	virtual FSlateColor GetBorderColor() const override;
};
