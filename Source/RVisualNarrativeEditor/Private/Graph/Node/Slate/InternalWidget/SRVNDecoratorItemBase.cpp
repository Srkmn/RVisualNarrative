#include "Graph/Node/Slate/InternalWidget/SRVNDecoratorItemBase.h"
#include "Framework/Application/SlateApplication.h"
#include "Decorator/RVNDecorator.h"
#include "UEVersion.h"

void SRVNDecoratorItemBase::Construct(const FArguments& InArgs)
{
	OwnerTaskNodePtr = InArgs._OwnerTaskNode;
	DecoratorPtr = InArgs._Decorator;
	ItemIndex = InArgs._FirstItemIndex;
	bIsSelected = InArgs._IsSelected;
	OnDecoratorSelectedDelegate = InArgs._OnDecoratorSelected;
	OnDecoratorDeletedDelegate = InArgs._OnDecoratorDeleted;

	const FLinearColor TextColor(0.9f, 0.9f, 0.9f, 1.0f);

	ChildSlot
	[
		SNew(SOverlay)

		+ SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor_Raw(this, &SRVNDecoratorItemBase::GetBorderColor)
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor_Raw(this, &SRVNDecoratorItemBase::GetBackgroundColor)
				.Padding(FMargin(5.0f, 15.f))
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0.0f, 0.0f, 8.0f, 0.0f)
					[
						SNew(SImage)
						.Image(this, &SRVNDecoratorItemBase::GetDecoratorIconBrush)
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(STextBlock)
						.Text(this, &SRVNDecoratorItemBase::GetItemText)
						.ColorAndOpacity(TextColor)
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
					]
				]
			]
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0.f, -10.0f, -20.0f, 0.f))
		[
			SNew(SBox)
			.WidthOverride(24.0f)
			.HeightOverride(24.0f)
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("BTEditor.Graph.BTNode.Index"))
				.BorderBackgroundColor(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() { return FText::AsNumber(GetItemIndex()); })
					.ColorAndOpacity(TextColor)
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				]
			]
		]
	];
}

FReply SRVNDecoratorItemBase::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

FReply SRVNDecoratorItemBase::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	SetIsSelected(true);

	return FReply::Handled();
}

void SRVNDecoratorItemBase::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	SetIsSelected(false);
}

FReply SRVNDecoratorItemBase::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsSelected && InKeyEvent.GetKey() == EKeys::Delete)
	{
		if (DecoratorPtr != nullptr && OnDecoratorDeletedDelegate.IsBound())
		{
			OnDecoratorDeletedDelegate.Execute(DecoratorPtr.Get());

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

void SRVNDecoratorItemBase::SetIsSelected(bool bInIsSelected)
{
	if (bIsSelected != bInIsSelected)
	{
		bIsSelected = bInIsSelected;

		if (bIsSelected)
		{
			OnDecoratorSelectedDelegate.Execute(DecoratorPtr.Get());
		}
	}
}

FSlateColor SRVNDecoratorItemBase::GetBackgroundColor() const
{
	return FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

FSlateColor SRVNDecoratorItemBase::GetBorderColor() const
{
	return bIsSelected ? FLinearColor(0.4f, 0.05f, 0.05f, 1.0f) : GetBackgroundColor();
}

FText SRVNDecoratorItemBase::GetItemText() const
{
	if (DecoratorPtr != nullptr)
	{
		return FText::FromString(DecoratorPtr->GetName());
	}

	return FText::FromString(TEXT("Null Decorator"));
}

const FSlateBrush* SRVNDecoratorItemBase::GetDecoratorIconBrush() const
{
	if (DecoratorPtr != nullptr)
	{
		FString IconName = DecoratorPtr->GetNodeIconName();

		const FSlateBrush* Brush =
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			(*IconName);

		if (Brush && Brush->GetResourceName().ToString() != TEXT("Default"))
		{
			return Brush;
		}
	}

	return
#if UE_APP_STYLE_GET_BRUSH
		FAppStyle::Get().GetBrush
#else
		FEditorStyle::GetBrush
#endif
		("ClassIcon.Default");
}
