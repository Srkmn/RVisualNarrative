#include "Graph/Node/Slate/SRVNStateWidget.h"
#include "SGraphPin.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Node/Slate/SRVNStatePin.h"
#include "Graph/Node/Slate/InternalWidget/SRVNStateInternal_Decorator.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Brushes/SlateColorBrush.h"
#include "UEVersion.h"

class SRVNNodeIndex : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRVNNodeIndex)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, int32 InNodeId)
	{
		const FSlateBrush* IndexBrush =
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			(TEXT("BTEditor.Graph.BTNode.Index"));

		const FSlateFontInfo FontInfo =
#if UE_APP_STYLE_GET_FONT_STYLE
			FAppStyle::Get().GetFontStyle
#else
			FEditorStyle::GetFontStyle
#endif
			("BTEditor.Graph.BTNode.IndexText");

		ChildSlot
		[
			SNew(SBox)
			.WidthOverride(IndexBrush->ImageSize.X)
			.HeightOverride(IndexBrush->ImageSize.Y)
			[
				SNew(SBorder)
				.BorderImage(IndexBrush)
				.BorderBackgroundColor(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f))
				.Padding(FMargin(0, 0, 0.2, 1.6))
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SAssignNew(NodeIdText, STextBlock)
					.Text(FText::AsNumber(InNodeId))
					.Font(FontInfo)
					.ColorAndOpacity(FLinearColor::Black)
					.Justification(ETextJustify::Center)
				]
			]
		];
	}

public:
	TSharedPtr<STextBlock> NodeIdText;

private:
	FSlateColor GetColor() const
	{
		return FLinearColor(0.1f, 0.1f, 0.1f, 0.8f);
	}
};

void SRVNStateWidget::Construct(const FArguments& InArgs, URVNStateNode* InNode)
{
	UpdateDelegate(InNode);

	SetCursor(EMouseCursor::CardinalCross);

	bIsDialogSlotExpanded = false;
	bIsNodeSlot1Expanded = false;

	UpdateGraphNode();
}

void SRVNStateWidget::UpdateDelegate(URVNStateNode* InNode)
{
	StateNode = InNode;
	GraphNode = InNode;

	StateNode->OnNodeIdChangedCallback.BindRaw(this, &SRVNStateWidget::HandleNodeIdChanged);
	StateNode->OnAddConditionCallback.BindRaw(this, &SRVNStateWidget::HandleAddCondition);
	StateNode->OnRemoveConditionCallback.BindRaw(this, &SRVNStateWidget::HandleRemoveCondition);
}

void SRVNStateWidget::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	ContentScale.Bind(this, &SGraphNode::GetContentScale);

	NodeIndexOverlay = SNew(SRVNNodeIndex, StateNode->GetNodeId());

	auto BackgroundColor = FLinearColor(1.f, 0.3f, 0.3f, 1.0f);

	switch (StateNode->GetNodeType())
	{
	case ENodeType::ENT_Entry:
		{
			BackgroundColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
			break;
		}

	case ENodeType::ENT_Select:
		{
			BackgroundColor = FLinearColor(0.3f, 1.f, 0.3f, 1.0f);
			break;
		}

	default:
		{
			break;
		}
	}

	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("Graph.Node.Body"))
			.BorderBackgroundColor(BackgroundColor)
			.Padding(10.0f)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					CreateHeaderWidget()
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					[
						SAssignNew(LeftNodeBox, SVerticalBox)
					]
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					CreateNodeContent()
				]
			]
		];

	CreatePinWidgets();
}

void SRVNStateWidget::CreatePinWidgets()
{
	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		if (!Pin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SRVNStatePin, Pin);
			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SRVNStateWidget::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();

	if (PinObj && PinObj->Direction == EGPD_Input)
	{
		LeftNodeBox->AddSlot()
		           .HAlign(HAlign_Center)
		           .VAlign(VAlign_Center)
		           .Padding(0.0f)
		           .AutoHeight()
		[
			PinToAdd
		];
		InputPins.Add(PinToAdd);
	}
	else
	{
		RightNodeBox->AddSlot()
		            .HAlign(HAlign_Center)
		            .VAlign(VAlign_Center)
		            .Padding(0.0f)
		            .AutoHeight()
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
	}
}

TArray<FOverlayWidgetInfo> SRVNStateWidget::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets;

	check(NodeIndexOverlay.IsValid());

	FVector2D Origin(-10.0f, -10.0f);

	FOverlayWidgetInfo Overlay(NodeIndexOverlay);
	Overlay.OverlayOffset = FVector2D(Origin);

	Widgets.Add(Overlay);

	return Widgets;
}

void SRVNStateWidget::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);

	StateNode->OnPositionChanged();
}

TSharedRef<SWidget> SRVNStateWidget::CreateHeaderWidget()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(5, 0, 0, 0))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SAssignNew(NameText, STextBlock)
				.Text_Lambda([this] { return FText::FromString(StateNode->GetSpeakerName()); })
				.ColorAndOpacity(FLinearColor::White)
			]
		];
}

TSharedRef<SWidget> SRVNStateWidget::CreateNodeContent()
{
	TextBoxStyle = FAppStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
	TextBoxStyle.BackgroundImageNormal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.1f));
	TextBoxStyle.ForegroundColor = FSlateColor(FLinearColor::White);

	auto Widget = SNew(SVerticalBox);

	if (!StateNode->IsEntryNode())
	{
		Widget->AddSlot()
		      .AutoHeight()
		      .Padding(0, 2)
		[
			CreateDialogSlot()
		];

		Widget->AddSlot()
		      .AutoHeight()
		      .Padding(0, 2)
		[
			CreateNodeSlot(NodeSlot1Box,
			               bIsNodeSlot1Expanded,
			               FText::FromString("Condition Slot"),
			               FOnClicked::CreateSP(this, &SRVNStateWidget::OnNodeSlot1ExpandClicked))
		];

		for (const auto& Condition : StateNode->GetConditionNodes())
		{
			if (Condition == nullptr)
			{
				continue;
			}

			HandleAddCondition(Condition);
		}
	}

	Widget->AddSlot()
	      .AutoHeight()
	      .Padding(0, 2)
	[
		CreateStateGraphSlot()
	];

	OnConditionSlotExpandedChanged(false);

	return Widget;
}

TSharedRef<SWidget> SRVNStateWidget::CreateDialogSlot()
{
	TextBoxStyle = FAppStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
	TextBoxStyle.BackgroundImageNormal.TintColor = FSlateColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.6f));
	TextBoxStyle.ForegroundColor = FSlateColor(FLinearColor::White);

	constexpr float MinHeight = 60.0f;
	constexpr float MaxHeight = 200.0f;

	return SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Graph.Node.Body"))
		.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.6f))
		.Padding(5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(DialogTextBox, SBox)
					.MaxDesiredHeight_Lambda([this, MinHeight, MaxHeight]() -> float
					{
						if (DialogTextEditor.IsValid())
						{
							const FVector2D ContentSize = DialogTextEditor->GetDesiredSize();
							if (bIsDialogSlotExpanded)
							{
								return FMath::Min(ContentSize.Y, MaxHeight);
							}
						}
						return MinHeight;
					})
					[
						SAssignNew(DialogTextEditor, SMultiLineEditableTextBox)
						.Style(&TextBoxStyle)
						.Text_Lambda([this] { return FText::FromString(StateNode->GetDialogText()); })
						.OnTextChanged(this, &SRVNStateWidget::OnDialogTextChanged)
						.WrapTextAt(400.0f)
						.AllowMultiLine(true)
						.SelectAllTextWhenFocused(false)
						.ClearKeyboardFocusOnCommit(false)
						.SelectAllTextOnCommit(false)
					]
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Top)
			.Padding(FMargin(4, 0, 0, 0))
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "NoBorder")
				.OnClicked(this, &SRVNStateWidget::OnDialogSlotExpandClicked)
				.ContentPadding(FMargin(2, 2))
				[
					SNew(SImage)
					.Image_Lambda([this]()
					{
						return FAppStyle::Get().GetBrush(bIsDialogSlotExpanded
							                                 ? "TreeArrow_Expanded"
							                                 : "TreeArrow_Collapsed");
					})
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
				]
			]
		];
}

TSharedRef<SWidget> SRVNStateWidget::CreateNodeSlot(
	TSharedPtr<SVerticalBox>& OutSlotBox,
	bool& bIsExpanded,
	const FText& SlotTitle,
	FOnClicked ExpandCallback)
{
	constexpr float CollapsedHeight = 45.0f;
	constexpr float ExpandedMinHeight = 100.0f;

	return SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Graph.Node.Body"))
		.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.6f))
		.Padding(5)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(SlotTitle)
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(4, 0, 0, 0))
				[
					SNew(SButton)
					.ButtonStyle(FAppStyle::Get(), "NoBorder")
					.OnClicked(ExpandCallback)
					[
						SNew(SImage)
						.Image_Lambda([&bIsExpanded]()
						{
							return FAppStyle::Get().
								GetBrush(bIsExpanded ? "TreeArrow_Expanded" : "TreeArrow_Collapsed");
						})
						.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 5, 0, 0)
			[
				SNew(SBox)
				.MinDesiredHeight_Lambda([&bIsExpanded, CollapsedHeight, ExpandedMinHeight]
				{
					return bIsExpanded ? ExpandedMinHeight : CollapsedHeight;
				})
				.MaxDesiredHeight_Lambda([&bIsExpanded, CollapsedHeight, ExpandedMinHeight]
				{
					return bIsExpanded ? FOptionalSize() : CollapsedHeight;
				})
				[
					SNew(SBorder)
					.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
						FAppStyle::Get().GetBrush
#else
						FEditorStyle::GetBrush
#endif
						("Graph.StateNode.Body"))
					.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 0.3f))
					.Padding(2)
					[
						SNew(SBorder)
						.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
							FAppStyle::Get().GetBrush
#else
							FEditorStyle::GetBrush
#endif
							("Graph.StateNode.Body"))
						.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 1.f))
						.Padding(3)
						[
							SAssignNew(OutSlotBox, SVerticalBox)
						]
					]
				]
			]
		];
}

TSharedRef<SWidget> SRVNStateWidget::CreateStateGraphSlot()
{
	static FButtonStyle CustomButtonStyle = FButtonStyle()
	                                        .SetNormal(FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f)))
	                                        .SetHovered(FSlateColorBrush(FLinearColor(0.2f, 0.2f, 0.4f, 0.3f)))
	                                        .SetPressed(FSlateColorBrush(FLinearColor(0.2f, 0.2f, 0.5f, 0.4f)))
	                                        .SetNormalPadding(FMargin(0))
	                                        .SetPressedPadding(FMargin(0));

	return SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Graph.Node.Body"))
		.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.6f))
		.Padding(5)
		[
			SNew(SButton)
			.ButtonStyle(&CustomButtonStyle)
			.OnClicked(this, &SRVNStateWidget::OnOpenStateGraphClicked)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.ContentPadding(FMargin(0))
			[
				SNew(SBorder)
				.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
                    FAppStyle::Get().GetBrush
#else
					FEditorStyle::GetBrush
#endif
					("Graph.CollapsedNode.Body")
				)
				.Padding(FMargin(20, 14))
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					[
						SNew(SImage)
						.Image(
#if UE_APP_STYLE_GET_BRUSH
                            FAppStyle::Get().GetBrush("Graph.CollapsedNode.BodyColorSpill")  
#else
							FEditorStyle::GetBrush("Graph.CollapsedNode.BodyColorSpill")
#endif
						)
						.ColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f, 0.7f))
					]
					+ SOverlay::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.Padding(0)
						[
							SNew(STextBlock)
							.Text(FText::FromString("State Machine"))
							.Font(
#if UE_APP_STYLE_GET_FONT_STYLE
                                FAppStyle::Get().GetFontStyle("NormalFont")  
#else
								FEditorStyle::GetFontStyle("NormalFont")
#endif
							)
							.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.6f))
							.Justification(ETextJustify::Center)
						]
					]
				]
			]
		];
}

FReply SRVNStateWidget::OnDialogSlotExpandClicked()
{
	bIsDialogSlotExpanded = !bIsDialogSlotExpanded;

	if (DialogTextBox.IsValid() && DialogTextEditor.IsValid())
	{
		const FVector2D ContentSize = DialogTextEditor->GetDesiredSize();
		const float MinHeight = 60.0f;
		const float MaxHeight = 200.0f;

		if (ContentSize.Y > MinHeight)
		{
			float NewHeight = bIsDialogSlotExpanded ? FMath::Min(ContentSize.Y, MaxHeight) : MinHeight;

			DialogTextBox->SetMaxDesiredHeight(NewHeight);
		}

		GetParentWidget()->Invalidate(EInvalidateWidget::Layout);
	}

	return FReply::Handled();
}

FReply SRVNStateWidget::OnNodeSlot1ExpandClicked()
{
	OnConditionSlotExpandedChanged(!bIsNodeSlot1Expanded);

	return FReply::Handled();
}

FReply SRVNStateWidget::OnOpenStateGraphClicked()
{
	if (StateNode.IsValid())
	{
		StateNode->OpenStateGraph();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SRVNStateWidget::OnConditionSlotExpandedChanged(bool bIsExpanded)
{
	if (bIsNodeSlot1Expanded == bIsExpanded)
	{
		return;
	}

	bIsNodeSlot1Expanded = !bIsNodeSlot1Expanded;

	if (NodeSlot1Box.IsValid())
	{
		auto ToggleVisibility = [](TSharedPtr<SVerticalBox> Box, bool bExpanded)
		{
			for (int32 i = 1; i < Box->GetChildren()->Num(); ++i)
			{
				Box->GetChildren()->GetChildAt(i)->SetVisibility(
					bExpanded ? EVisibility::Visible : EVisibility::Collapsed
				);
			}
		};

		ToggleVisibility(NodeSlot1Box, bIsNodeSlot1Expanded);
	}
}

void SRVNStateWidget::OnDialogTextChanged(const FText& NewText)
{
	if (StateNode.IsValid())
	{
		StateNode->SetDialogText(NewText.ToString());

		if (DialogTextBox.IsValid() && DialogTextEditor.IsValid())
		{
			const FVector2D ContentSize = DialogTextEditor->GetDesiredSize();
			const float MinHeight = 60.0f;
			const float MaxHeight = 200.0f;

			if (bIsDialogSlotExpanded)
			{
				float NewHeight = FMath::Min(ContentSize.Y, MaxHeight);
				DialogTextBox->SetMaxDesiredHeight(NewHeight);
			}
			else
			{
				DialogTextBox->SetMaxDesiredHeight(MinHeight);
			}

			GetParentWidget()->Invalidate(EInvalidateWidget::Layout);
		}
	}
}

void SRVNStateWidget::HandleNodeIdChanged()
{
	NodeIndexOverlay->NodeIdText->SetText(TAttribute(FText::AsNumber(StateNode->GetNodeId())));
}

void SRVNStateWidget::HandleAddCondition(URVNConditionBase* ConditionInfo)
{
	if (!NodeSlot1Box.IsValid())
		return;

	TSharedRef<SWidget> NewItemWidget = CreateSlotItemWidget(ConditionInfo);

	const auto ConditionWidget = StaticCastSharedRef<SRVNStateInternal_Decorator>(NewItemWidget);

	ConditionWidget->OnSelectedDecorator.BindUObject(StateNode.Get(), &URVNStateNode::OnSelectedDecorator);
	ConditionWidget->OnDeleteRequested.BindUObject(StateNode.Get(), &URVNStateNode::RemoveDecorator);

	ConditionSlotItems.Add(ConditionInfo, NewItemWidget);

	NodeSlot1Box->AddSlot()
	            .AutoHeight()
	            .Padding(0.f, 5.f, 0.f, 0.f)
	[
		NewItemWidget
	];

	OnConditionSlotExpandedChanged(true);
}

void SRVNStateWidget::HandleRemoveCondition(URVNConditionBase* ConditionInfo)
{
	if (!NodeSlot1Box.IsValid())
		return;

	NodeSlot1Box->RemoveSlot(ConditionSlotItems[ConditionInfo].ToSharedRef());
}

TSharedRef<SWidget> SRVNStateWidget::CreateSlotItemWidget(URVNDecorator* InItem)
{
	return SNew(SRVNStateInternal_Decorator)
		.DecoratorPtr(InItem)
		.OuterNode(SharedThis(this));
}
