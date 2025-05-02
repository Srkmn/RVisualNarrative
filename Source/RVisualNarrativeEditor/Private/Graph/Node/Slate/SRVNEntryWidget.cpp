#include "Graph/Node/Slate/SRVNEntryWidget.h"
#include "SGraphPin.h"
#include "Graph/Node/RVNEntryNode.h"
#include "Graph/Node/Slate/SRVNEntryPin.h"
#include "UEVersion.h"

void SRVNEntryWidget::Construct(const FArguments& InArgs, URVNEntryNode* InNode)
{
	GraphNode = InNode;
	OwnerEntryNodePtr = InNode;

	UpdateGraphNode();
}

void SRVNEntryWidget::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	ContentScale.Bind(this, &SGraphNode::GetContentScale);

	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
				FAppStyle::Get().GetBrush
#else
				FEditorStyle::GetBrush
#endif
				("Graph.Node.Body")
			)
			.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.7f))
			.Padding(FMargin(16, 8, 8, 8))
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.FillWidth(1.0f)
				.Padding(FMargin(0, 0, 12, 0))
				[
					SNew(STextBlock)
					.Text(FText::FromString("Entry"))
					.Font(
#if UE_APP_STYLE_GET_FONT_STYLE
						FAppStyle::Get().GetFontStyle
#else
						FEditorStyle::GetFontStyle
#endif
						("BoldFont")
					)
					.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f))
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.Padding(FMargin(0, 10))
				.AutoWidth()
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]
			]
		];

	CreatePinWidgets();
}

void SRVNEntryWidget::CreatePinWidgets()
{
	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		if (!Pin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SRVNEntryPin, Pin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SRVNEntryWidget::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	if (const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
		PinObj && PinObj->Direction == EGPD_Output)
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

void SRVNEntryWidget::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);

	OwnerEntryNodePtr->RequestUpdateExecutionOrder();
}
