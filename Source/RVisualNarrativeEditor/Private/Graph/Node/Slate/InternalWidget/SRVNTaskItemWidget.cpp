#include "Graph/Node/Slate/InternalWidget/SRVNTaskItemWidget.h"

void SRVNTaskItemWidget::Construct(const FArguments& InArgs)
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

FSlateColor SRVNTaskItemWidget::GetBackgroundColor() const
{
	return FLinearColor(0.3f, 0.2f, 0.4f, 0.8f);
}

FSlateColor SRVNTaskItemWidget::GetBorderColor() const
{
	return bIsSelected ? FLinearColor(0.4f, 0.05f, 0.05f, 1.0f) : GetBackgroundColor();
}
