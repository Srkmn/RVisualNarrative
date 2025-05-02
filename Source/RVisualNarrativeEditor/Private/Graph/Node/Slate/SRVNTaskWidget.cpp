#include "Graph/Node/Slate/SRVNTaskWidget.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPin.h"
#include "Graph/Node/RVNTaskNode.h"
#include "Graph/Node/RVNDecoratorItemFactory.h"
#include "Decorator/RVNDecorator.h"
#include "Decorator/Task/RVNTask.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Node/Slate/SRVNGraphPin.h"
#include "UEVersion.h"

void SRVNTaskWidget::Construct(const FArguments& InArgs, URVNTaskNode* InNode)
{
	GraphNode = InNode;
	OwnerTaskNodePtr = InNode;
	FirstDecoratorIndex = InNode->FirstDecoratorIndex;

	OwnerTaskNodePtr->OnAddCondition.BindLambda([this](URVNConditionBase* InCondition)
	{
		UpdateGraphNode();
	});

	OwnerTaskNodePtr->OnRemoveCondition.BindLambda([this](URVNConditionBase* InCondition)
	{
		UpdateGraphNode();
	});

	OwnerTaskNodePtr->OnExecutionOrderChanged.BindRaw(this, &SRVNTaskWidget::HandleExecutionOrderChanged);

	this->SetCursor(EMouseCursor::CardinalCross);

	UpdateGraphNode();
}

SRVNTaskWidget::~SRVNTaskWidget()
{
	if (OwnerTaskNodePtr.IsValid())
	{
		OwnerTaskNodePtr->OnExecutionOrderChanged.Unbind();

		OwnerTaskNodePtr->OnRemoveCondition.Unbind();

		OwnerTaskNodePtr->OnAddCondition.Unbind();
	}
}

void SRVNTaskWidget::UpdateGraphNode()
{
	InputPins.Reset();
	OutputPins.Reset();

	LeftNodeBox.Reset();
	RightNodeBox.Reset();

	ConditionList.Reset();
	TaskList.Reset();

	Decorators.Reset();

	const FLinearColor NodeBodyColor(0.2f, 0.2f, 0.2f, 1.f);
	const FLinearColor NodeBorderColor(0.6f, 0.1f, 0.1f, 1.0f);
	const FLinearColor NodeTitleColor(0.8f, 0.15f, 0.15f, 1.0f);

	ContentScale.Bind(this, &SGraphNode::GetContentScale);

	LeftNodeBox = SNew(SVerticalBox);
	RightNodeBox = SNew(SVerticalBox);

	CreatePinWidgets();

	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBorder)
			.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
				FAppStyle::Get().GetBrush
#else
				FEditorStyle::GetBrush
#endif
				("BTEditor.Graph.BTNode.Body"))
			.BorderBackgroundColor(NodeBodyColor)
			.Padding(0.0f)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
						FAppStyle::Get().GetBrush
#else
						FEditorStyle::GetBrush
#endif
						("BTEditor.Graph.BTNode.Body"))
					.BorderBackgroundColor(FLinearColor(0.4f, 0.05f, 0.05f, 1.0f))
					.Padding(FMargin(5.f, 12.f))
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Task Node")))
						.Font(
#if UE_APP_STYLE_GET_FONT_STYLE
							FAppStyle::Get().GetFontStyle
#else
							FEditorStyle::GetFontStyle
#endif
							("BoldFont"))
						.ColorAndOpacity(FLinearColor::White)
					]
				]

				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.Padding(FMargin(-5.f, 5.f, 0.f, 5.f))
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						LeftNodeBox.ToSharedRef()
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(5.0f)
					[
						SNew(SBorder)
						.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
							FAppStyle::Get().GetBrush
#else
							FEditorStyle::GetBrush
#endif
							("BTEditor.Graph.BTNode.Body"))
						.BorderBackgroundColor(FLinearColor::Black)
						[
							SNew(SVerticalBox)

							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SAssignNew(ConditionList, SVerticalBox)
							]

							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 5.0f, 0.0f, 0.0f)
							[
								SAssignNew(TaskList, SVerticalBox)
							]
						]
					]

					+ SHorizontalBox::Slot()
					.Padding(FMargin(5.f, 5.f, -5.f, 5.f))
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						RightNodeBox.ToSharedRef()
					]
				]
			]
		];

	CreateConditionsListWidget();
	CreateTaskWidget();
}

void SRVNTaskWidget::CreatePinWidgets()
{
	UEdGraphNode* CurGraphNode = GetNodeObj();
	if (!CurGraphNode)
	{
		return;
	}

	// 为每个Pin创建Widget  
	for (int32 PinIdx = 0; PinIdx < CurGraphNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* Pin = CurGraphNode->Pins[PinIdx];
		if (!Pin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SRVNGraphPin, Pin);
			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SRVNTaskWidget::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;

	if (bAdvancedParameter)
	{
		// 高级参数的处理  
		return;
	}

	if (PinToAdd->GetDirection() == EGPD_Input)
	{
		LeftNodeBox->AddSlot()
		           .AutoHeight()
		           .HAlign(HAlign_Left)
		           .VAlign(VAlign_Center)
		           .Padding(10.0f, 4.0f)
		[
			PinToAdd
		];
		InputPins.Add(PinToAdd);
	}
	else if (PinToAdd->GetDirection() == EGPD_Output)
	{
		RightNodeBox->AddSlot()
		            .AutoHeight()
		            .HAlign(HAlign_Right)
		            .VAlign(VAlign_Center)
		            .Padding(10.0f, 4.0f)
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
	}
}

void SRVNTaskWidget::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);

	OwnerTaskNodePtr->RequestUpdateExecutionOrder();
}

void SRVNTaskWidget::OnDecoratorSelected(URVNDecorator* SelectedDecorator)
{
	if (SelectedDecorator)
	{
		SelectedDecoratorPtr = SelectedDecorator;

		if (OwnerTaskNodePtr.IsValid())
		{
			OwnerTaskNodePtr->GetOwnerState()->OnSelectedDecorator(SelectedDecoratorPtr.Get());
		}
	}
}

void SRVNTaskWidget::OnDecoratorDeleted(URVNDecorator* DecoratorToDelete)
{
	if (DecoratorToDelete && OwnerTaskNodePtr.IsValid())
	{
		if (SelectedDecoratorPtr.Get() == DecoratorToDelete)
		{
			SelectedDecoratorPtr = nullptr;
		}

		if (const auto ConditionDecorator = Cast<URVNConditionBase>(DecoratorToDelete))
		{
			OwnerTaskNodePtr->RemoveCondition(ConditionDecorator);
		}
	}
}

int32 SRVNTaskWidget::HandleExecutionOrderChanged(int32 CurrentIndex)
{
	FirstDecoratorIndex = CurrentIndex;

	for (const TSharedPtr DecoratorWidget : Decorators)
	{
		if (DecoratorWidget.IsValid())
		{
			DecoratorWidget->SetItemIndex(CurrentIndex++);
		}
	}

	return CurrentIndex;
}

void SRVNTaskWidget::CreateConditionsListWidget()
{
	TArray<URVNConditionBase*> Conditions = GetConditionsList();

	if (!Conditions.IsEmpty())
	{
		for (URVNConditionBase* Condition : Conditions)
		{
			if (Condition)
			{
				bool bIsSelected = (SelectedDecoratorPtr.Get() == Condition);

				TSharedPtr<SRVNDecoratorItemBase> ConditionWidget =
					FRVNDecoratorItemFactory::CreateDecoratorItemWidget(
						OwnerTaskNodePtr.Get(),
						Condition,
						FirstDecoratorIndex++,
						bIsSelected,
						FOnDecoratorSelected::CreateSP(this, &SRVNTaskWidget::OnDecoratorSelected),
						FOnDecoratorDeleted::CreateSP(this, &SRVNTaskWidget::OnDecoratorDeleted)
					);

				ConditionList->AddSlot()
				             .AutoHeight()
				             .Padding(2.0f)
				[
					ConditionWidget.ToSharedRef()
				];

				Decorators.Add(ConditionWidget);
			}
		}
	}
}

void SRVNTaskWidget::CreateTaskWidget()
{
	URVNTaskBase* Task = GetTaskBase();

	if (Task)
	{
		bool bIsSelected = (SelectedDecoratorPtr.Get() == Task);

		TSharedPtr<SRVNDecoratorItemBase> TaskWidget =
			FRVNDecoratorItemFactory::CreateDecoratorItemWidget(
				OwnerTaskNodePtr.Get(),
				Task,
				FirstDecoratorIndex++,
				bIsSelected,
				FOnDecoratorSelected::CreateSP(this, &SRVNTaskWidget::OnDecoratorSelected),
				FOnDecoratorDeleted::CreateSP(this, &SRVNTaskWidget::OnDecoratorDeleted)
			);

		TaskList->AddSlot()
		        .AutoHeight()
		        .Padding(2.0f)
		[
			TaskWidget.ToSharedRef()
		];

		Decorators.Add(TaskWidget);
	}
}

TArray<URVNConditionBase*> SRVNTaskWidget::GetConditionsList() const
{
	if (OwnerTaskNodePtr.IsValid() && OwnerTaskNodePtr->GetOwnerTask() != nullptr)
	{
		return OwnerTaskNodePtr->GetOwnerTask()->GetConditions();
	}

	return TArray<URVNConditionBase*>();
}

URVNTaskBase* SRVNTaskWidget::GetTaskBase() const
{
	if (OwnerTaskNodePtr.IsValid())
	{
		return OwnerTaskNodePtr->GetOwnerTask();
	}

	return nullptr;
}
