#include "Graph/Node/Slate/InternalWidget/SRVNConditionItemWidget.h"

void SRVNConditionItemWidget::Construct(const FArguments& InArgs)
{
	SRVNDecoratorItemBase::Construct(
		SRVNDecoratorItemBase::FArguments()
		.OwnerTaskNode(InArgs._OwnerTaskNode)
		.Decorator(InArgs._Decorator)
		.FirstItemIndex(InArgs._FirstItemIndex)
		.IsSelected(InArgs._IsSelected)
		.OnDecoratorSelected(InArgs._OnDecoratorSelected)
		.OnDecoratorDeleted(InArgs._OnDecoratorDeleted)
	);
}

FSlateColor SRVNConditionItemWidget::GetBackgroundColor() const
{
	return FLinearColor(0.15f, 0.6f, 0.15f, 0.7f);
}

FSlateColor SRVNConditionItemWidget::GetBorderColor() const
{
	return bIsSelected ? FLinearColor(0.4f, 0.05f, 0.05f, 1.0f) : GetBackgroundColor();
}
