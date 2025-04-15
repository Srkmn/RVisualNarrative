#include "Tab/Slate/SRVNBlackboardView.h"
#include "Styling/SlateBrush.h"
#include "Fonts/SlateFontInfo.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxDefs.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "EditorStyleSet.h"
#include "SGraphActionMenu.h"
#include "SGraphPalette.h"
#include "Styling/SlateIconFinder.h"
#include "Styling/CoreStyle.h"
#include "ScopedTransaction.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "RVNBlackboardAssetProvider.h"
#include "RVNBlackboardData.h"
#include "RVNBlackboardKeyType.h"
#include "UEVersion.h"

#define LOCTEXT_NAMESPACE "SRVNBlackboardView"

namespace EBlackboardSectionTitles
{
	enum Type
	{
		InheritedKeys = 1,
		Keys,
	};
}

FName FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId()
{
	static FName Type("FEdGraphSchemaAction_RVNBlackboardEntry");

	return Type;
}

FName FEdGraphSchemaAction_RVNBlackboardEntry::GetTypeId() const
{
	return StaticGetTypeId();
}

FEdGraphSchemaAction_RVNBlackboardEntry::FEdGraphSchemaAction_RVNBlackboardEntry(
	URVNBlackboardData* InBlackboardData, FRVNBlackboardEntry& InKey, bool bInIsInherited)
	: FEdGraphSchemaAction_Dummy()
	  , BlackboardData(InBlackboardData)
	  , Key(InKey)
	  , bIsInherited(bInIsInherited)
	  , bIsNew(false)
{
	check(BlackboardData);
	Update();
}

void FEdGraphSchemaAction_RVNBlackboardEntry::Update()
{
	UpdateSearchData(FText::FromName(Key.EntryName),
	                 FText::Format(
		                 LOCTEXT("BlackboardEntryFormat", "{0} '{1}'"),
		                 Key.KeyType ? Key.KeyType->GetClass()->GetDisplayNameText() : LOCTEXT("NullKeyDesc", "None"),
		                 FText::FromName(Key.EntryName)),
	                 (Key.EntryCategory.IsNone() ? FText() : FText::FromName(Key.EntryCategory)), FText());
	SectionID = bIsInherited ? EBlackboardSectionTitles::InheritedKeys : EBlackboardSectionTitles::Keys;
}

class SRVNBlackboardItem : public SGraphPaletteItem
{
	SLATE_BEGIN_ARGS(SRVNBlackboardItem)
		{
		}

		//SLATE_EVENT(FOnGetDisplayCurrentState, OnGetDisplayCurrentState)
		SLATE_EVENT(FOnBlackboardKeyChanged, OnBlackboardKeyChanged)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
	{
		//OnGetDisplayCurrentState = InArgs._OnGetDisplayCurrentState;
		OnBlackboardKeyChanged = InArgs._OnBlackboardKeyChanged;

		const FSlateFontInfo NameFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);

		check(InCreateData);
		check(InCreateData->Action.IsValid());

		TSharedPtr<FEdGraphSchemaAction> GraphAction = InCreateData->Action;
		check(GraphAction->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());
		TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> BlackboardEntryAction = StaticCastSharedPtr<
			FEdGraphSchemaAction_RVNBlackboardEntry>(GraphAction);

		ActionPtr = InCreateData->Action;

		FSlateBrush const* IconBrush =
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			(TEXT("NoBrush"));
		GetPaletteItemIcon(GraphAction, IconBrush);

		TSharedRef<SWidget> IconWidget = CreateIconWidget(GraphAction->GetTooltipDescription(), IconBrush,
		                                                  FLinearColor::White);
		TSharedRef<SWidget> NameSlotWidget = CreateTextSlotWidget(InCreateData, BlackboardEntryAction->bIsInherited);

		// Create the actual widget
		this->ChildSlot
		[
			SNew(SHorizontalBox)
			// Icon slot
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				IconWidget
			]
			// Name slot
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(3, 0)
			[
				NameSlotWidget
			]
		];
	}

private:
	void GetPaletteItemIcon(TSharedPtr<FEdGraphSchemaAction> InGraphAction, FSlateBrush const*& OutIconBrush)
	{
		check(InGraphAction.IsValid());
		check(InGraphAction->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());
		TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> BlackboardEntryAction = StaticCastSharedPtr<
			FEdGraphSchemaAction_RVNBlackboardEntry>(InGraphAction);

		if (BlackboardEntryAction->Key.KeyType)
		{
			OutIconBrush = FSlateIconFinder::FindIconBrushForClass(BlackboardEntryAction->Key.KeyType->GetClass());
		}
	}

	virtual TSharedRef<SWidget> CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData,
	                                                 TAttribute<bool> bInIsReadOnly) override
	{
		check(InCreateData);

		TSharedPtr<SWidget> DisplayWidget;

		// Copy the mouse delegate binding if we want it
		if (InCreateData->bHandleMouseButtonDown)
		{
			MouseButtonDownDelegate = InCreateData->MouseButtonDownDelegate;
		}

		// If the creation data says read only, then it must be read only
		bIsReadOnly = InCreateData->bIsReadOnly || bInIsReadOnly.Get();

		InlineRenameWidget =
			SAssignNew(DisplayWidget, SInlineEditableTextBlock)
			.Text(this, &SRVNBlackboardItem::GetDisplayText)
			.HighlightText(InCreateData->HighlightText)
			.ToolTipText(this, &SRVNBlackboardItem::GetItemTooltip)
			.OnTextCommitted(this, &SRVNBlackboardItem::OnNameTextCommitted)
			.OnVerifyTextChanged(this, &SRVNBlackboardItem::OnNameTextVerifyChanged)
			.IsSelected(InCreateData->IsRowSelectedDelegate)
			.IsReadOnly(this, &SRVNBlackboardItem::IsReadOnly);

		InCreateData->OnRenameRequest->BindSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

		return DisplayWidget.ToSharedRef();
	}

	virtual FText GetItemTooltip() const override
	{
		return ActionPtr.Pin()->GetTooltipDescription();
	}

	virtual void OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit) override
	{
		check(ActionPtr.Pin()->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());

		const FString AsString = *NewText.ToString();

		if (AsString.Len() >= NAME_SIZE)
		{
			return;
		}

		TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> BlackboardEntryAction = StaticCastSharedPtr<
			FEdGraphSchemaAction_RVNBlackboardEntry>(ActionPtr.Pin());

		FName OldName = BlackboardEntryAction->Key.EntryName;
		FName NewName = FName(*AsString);
		if (NewName != OldName)
		{
			TArray<UObject*> ExternalBTAssetsWithKeyReferences;
			if (!BlackboardEntryAction->bIsNew && BlackboardEntryAction->BlackboardData)
			{
				// Preload behavior trees before we transact otherwise they will add objects to 
				// the transaction buffer whether we change them or not.
				// Blueprint regeneration does this in UEdGraphNode::CreatePin.
				LoadReferencerBehaviorTrees(*(BlackboardEntryAction->BlackboardData),
				                            ExternalBTAssetsWithKeyReferences);
			}

			const FScopedTransaction
				Transaction(LOCTEXT("BlackboardEntryRenameTransaction", "Rename Blackboard Entry"));
			BlackboardEntryAction->BlackboardData->SetFlags(RF_Transactional);
			BlackboardEntryAction->BlackboardData->Modify();
			BlackboardEntryAction->Key.EntryName = NewName;

			FProperty* KeysArrayProperty = FindFProperty<FProperty>(URVNBlackboardData::StaticClass(),
			                                                        GET_MEMBER_NAME_CHECKED(URVNBlackboardData, Keys));
			FProperty* NameProperty = FindFProperty<FProperty>(FRVNBlackboardEntry::StaticStruct(),
			                                                   GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, EntryName));
			FEditPropertyChain PropertyChain;
			PropertyChain.AddHead(KeysArrayProperty);
			PropertyChain.AddTail(NameProperty);
			PropertyChain.SetActiveMemberPropertyNode(KeysArrayProperty);
			PropertyChain.SetActivePropertyNode(NameProperty);

			BlackboardEntryAction->BlackboardData->PreEditChange(PropertyChain);

			BlackboardEntryAction->Update();

			OnBlackboardKeyChanged.ExecuteIfBound(BlackboardEntryAction->BlackboardData, &BlackboardEntryAction->Key);

			if (!BlackboardEntryAction->bIsNew)
			{
				UpdateExternalBlackboardKeyReferences(OldName, NewName, ExternalBTAssetsWithKeyReferences);
			}

			FPropertyChangedEvent PropertyChangedEvent(NameProperty, EPropertyChangeType::ValueSet);
			FPropertyChangedChainEvent PropertyChangedChainEvent(PropertyChain, PropertyChangedEvent);
			BlackboardEntryAction->BlackboardData->PostEditChangeChainProperty(PropertyChangedChainEvent);
		}

		BlackboardEntryAction->bIsNew = false;
	}

	void GetBlackboardOwnerClasses(TArray<const UClass*>& BlackboardOwnerClasses)
	{
		for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
		{
			UClass* Class = *ClassIt;
			if (Class->ImplementsInterface(URVNBlackboardAssetProvider::StaticClass()))
			{
				BlackboardOwnerClasses.Add(Class);
			}
		}
	}

	void LoadReferencerBehaviorTrees(const URVNBlackboardData& InBlackboardData,
	                                 TArray<UObject*>& OutExternalBTAssetsWithKeyReferences)
	{
		// Get classes and derived classes which implement UBlackboardAssetProvider.
		TArray<const UClass*> BlackboardOwnerClasses;
		GetBlackboardOwnerClasses(BlackboardOwnerClasses);

		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).
			Get();

		TArray<FName> ReferencerPackages;
		AssetRegistry.GetReferencers(InBlackboardData.GetOutermost()->GetFName(), ReferencerPackages,
		                             UE::AssetRegistry::EDependencyCategory::Package,
		                             UE::AssetRegistry::EDependencyQuery::Hard);

		if (ReferencerPackages.Num())
		{
			FScopedSlowTask SlowTask((float)ReferencerPackages.Num(),
			                         LOCTEXT("UpdatingBehaviorTrees", "Updating behavior trees"));
			SlowTask.MakeDialog();

			for (const FName& ReferencerPackage : ReferencerPackages)
			{
				TArray<FAssetData> Assets;
				AssetRegistry.GetAssetsByPackageName(ReferencerPackage, Assets);

				for (const FAssetData& Asset : Assets)
				{
					if (BlackboardOwnerClasses.Find(Asset.GetClass()) != INDEX_NONE)
					{
						SlowTask.EnterProgressFrame(1.0f, FText::Format(
							                            LOCTEXT("CheckingBehaviorTree", "Key renamed, loading {0}"),
							                            FText::FromName(Asset.AssetName)));

						UObject* AssetObject = Asset.GetAsset();
						const IRVNBlackboardAssetProvider* BlackboardProvider = Cast<const IRVNBlackboardAssetProvider>(
							AssetObject);
						if (BlackboardProvider && BlackboardProvider->GetBlackboardAsset() == &InBlackboardData)
						{
							OutExternalBTAssetsWithKeyReferences.Add(AssetObject);
						}
					}
				}
			}
		}
	}

#define GET_STRUCT_NAME_CHECKED(StructName) \
		((void)sizeof(StructName), TEXT(#StructName))

	void UpdateExternalBlackboardKeyReferences(const FName& OldKey, const FName& NewKey,
	                                           const TArray<UObject*>& InExternalBTAssetsWithKeyReferences) const
	{
		for (const UObject* Asset : InExternalBTAssetsWithKeyReferences)
		{
			// search all subobjects of this package for FBlackboardKeySelector structs and update as necessary
			TArray<UObject*> Objects;
			GetObjectsWithOuter(Asset->GetOutermost(), Objects);
			for (const auto& SubObject : Objects)
			{
				for (TFieldIterator<FStructProperty> It(SubObject->GetClass()); It; ++It)
				{
					/*if (It->GetCPPType(NULL, CPPF_None).Contains(GET_STRUCT_NAME_CHECKED(FBlackboardKeySelector)))
					{
						FBlackboardKeySelector* PropertyValue = (FBlackboardKeySelector*)(It->ContainerPtrToValuePtr<
							uint8>(SubObject));
						if (PropertyValue && PropertyValue->SelectedKeyName == OldKey)
						{
							SubObject->Modify();
							PropertyValue->SelectedKeyName = NewKey;
						}
					}*/
				}
			}
		}
	}

	virtual bool OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage) override
	{
		check(ActionPtr.Pin()->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());
		TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> BlackboardEntryAction = StaticCastSharedPtr<
			FEdGraphSchemaAction_RVNBlackboardEntry>(ActionPtr.Pin());

		const FString NewTextAsString = InNewText.ToString();

		// check for duplicate keys
		for (const auto& Key : BlackboardEntryAction->BlackboardData->Keys)
		{
			if (&BlackboardEntryAction->Key != &Key && Key.EntryName.ToString() == NewTextAsString)
			{
				OutErrorMessage = LOCTEXT("DuplicateKeyWarning", "A key of this name already exists.");
				return false;
			}
		}

		for (const auto& Key : BlackboardEntryAction->BlackboardData->ParentKeys)
		{
			if (&BlackboardEntryAction->Key != &Key && Key.EntryName.ToString() == NewTextAsString)
			{
				OutErrorMessage = LOCTEXT("DuplicateParentKeyWarning", "An inherited key of this name already exists.");
				return false;
			}
		}

		return true;
	}

	bool IsReadOnly() const
	{
		return bIsReadOnly;
	}

private:
	/** Delegate used to determine whether the BT debugger displaying the current state */
	//FOnGetDisplayCurrentState OnGetDisplayCurrentState;

	/** Delegate for when a blackboard key changes (added, removed, renamed) */
	FOnBlackboardKeyChanged OnBlackboardKeyChanged;

	/** Read-only flag */
	bool bIsReadOnly;
};

/*void SRVNBlackboardView::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (BlackboardData != nullptr)
	{
		Collector.AddReferencedObject(BlackboardData);
	}
}*/

void SRVNBlackboardView::Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList,
                                   URVNBlackboardData* InBlackboardData)
{
	OnEntrySelected = InArgs._OnEntrySelected;
	OnBlackboardKeyChanged = InArgs._OnBlackboardKeyChanged;

	BlackboardData = InBlackboardData;

	TSharedRef<FUICommandList> CommandList = MakeShareable(new FUICommandList);

	InCommandList->Append(CommandList);

	FToolBarBuilder ToolbarBuilder(CommandList, FMultiBoxCustomization::None, GetToolbarExtender(InCommandList));

	ToolbarBuilder.BeginSection(TEXT("Debugging"));
	ToolbarBuilder.EndSection();

	ChildSlot
	[
		SNew(SBorder)
		.Padding(4.0f)
		.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 4.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					ToolbarBuilder.MakeWidget()
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SAssignNew(GraphActionMenu, SGraphActionMenu, InArgs._IsReadOnly)
				.OnCreateWidgetForAction(this, &SRVNBlackboardView::HandleCreateWidgetForAction)
				.OnCollectAllActions(this, &SRVNBlackboardView::HandleCollectAllActions)
				.OnGetSectionTitle(this, &SRVNBlackboardView::HandleGetSectionTitle)
				.OnActionSelected(this, &SRVNBlackboardView::HandleActionSelected)
				.OnContextMenuOpening(this, &SRVNBlackboardView::HandleContextMenuOpening, InCommandList)
				.OnActionMatchesName(this, &SRVNBlackboardView::HandleActionMatchesName)
				.AlphaSortItems(GetDefault<UEditorPerProjectUserSettings>()->bDisplayBlackboardKeysInAlphabeticalOrder)
				.AutoExpandActionMenu(true)
			]
		]
	];
}

TSharedRef<SWidget> SRVNBlackboardView::HandleCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
	return SNew(SRVNBlackboardItem, InCreateData)
			//.OnGetDisplayCurrentState(this, &SRVNBlackboardView::IsUsingCurrentValues)
			.OnBlackboardKeyChanged(OnBlackboardKeyChanged);
}

void SRVNBlackboardView::HandleCollectAllActions(FGraphActionListBuilderBase& GraphActionListBuilder)
{
	if (BlackboardData != nullptr)
	{
		for (auto& ParentKey : BlackboardData->ParentKeys)
		{
			GraphActionListBuilder.AddAction(
				MakeShareable(new FEdGraphSchemaAction_RVNBlackboardEntry(BlackboardData, ParentKey, true)));
		}

		for (auto& Key : BlackboardData->Keys)
		{
			GraphActionListBuilder.AddAction(
				MakeShareable(new FEdGraphSchemaAction_RVNBlackboardEntry(BlackboardData, Key, false)));
		}
	}
}

FText SRVNBlackboardView::HandleGetSectionTitle(int32 SectionID) const
{
	switch (SectionID)
	{
	case EBlackboardSectionTitles::InheritedKeys:
		return LOCTEXT("InheritedKeysSectionLabel", "Inherited Keys");
	case EBlackboardSectionTitles::Keys:
		return LOCTEXT("KeysSectionLabel", "Keys");
	}

	return FText();
}

void SRVNBlackboardView::HandleActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedActions,
                                              ESelectInfo::Type InSelectionType) const
{
	if (InSelectionType == ESelectInfo::OnMouseClick || InSelectionType == ESelectInfo::OnKeyPress || SelectedActions.
		Num() == 0)
	{
		if (SelectedActions.Num() > 0)
		{
			check(SelectedActions[0]->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());
			TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> BlackboardEntry = StaticCastSharedPtr<
				FEdGraphSchemaAction_RVNBlackboardEntry>(SelectedActions[0]);
			OnEntrySelected.ExecuteIfBound(&BlackboardEntry->Key, BlackboardEntry->bIsInherited);
		}
	}
}

TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> SRVNBlackboardView::GetSelectedEntryInternal() const
{
	TArray<TSharedPtr<FEdGraphSchemaAction>> SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);

	if (SelectedActions.Num() > 0)
	{
		check(SelectedActions[0]->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());
		return StaticCastSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry>(SelectedActions[0]);
	}

	return TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry>();
}

int32 SRVNBlackboardView::GetSelectedEntryIndex(bool& bOutIsInherited) const
{
	TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> Entry = GetSelectedEntryInternal();
	if (Entry.IsValid())
	{
		bOutIsInherited = Entry->bIsInherited;
		FRVNBlackboardEntry* BlackboardEntry = &Entry->Key;

		// check to see what entry index we are using
		TArray<FRVNBlackboardEntry>& EntryArray = bOutIsInherited ? BlackboardData->ParentKeys : BlackboardData->Keys;
		for (int32 Index = 0; Index < EntryArray.Num(); Index++)
		{
			if (BlackboardEntry == &EntryArray[Index])
			{
				return Index;
			}
		}
	}

	return INDEX_NONE;
}

FRVNBlackboardEntry* SRVNBlackboardView::GetSelectedEntry(bool& bOutIsInherited) const
{
	TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> Entry = GetSelectedEntryInternal();
	if (Entry.IsValid())
	{
		bOutIsInherited = Entry->bIsInherited;
		return &Entry->Key;
	}

	return nullptr;
}

void SRVNBlackboardView::SetObject(URVNBlackboardData* InBlackboardData)
{
	BlackboardData = InBlackboardData;
	GraphActionMenu->RefreshAllActions(true);
}

TSharedPtr<SWidget> SRVNBlackboardView::HandleContextMenuOpening(TSharedRef<FUICommandList> ToolkitCommands) const
{
	FMenuBuilder MenuBuilder(/* bInShouldCloseWindowAfterMenuSelection =*/true, ToolkitCommands);

	FillContextMenu(MenuBuilder);

	return MenuBuilder.MakeWidget();
}

void SRVNBlackboardView::FillContextMenu(FMenuBuilder& MenuBuilder) const
{
}

TSharedPtr<FExtender> SRVNBlackboardView::GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const
{
	return TSharedPtr<FExtender>();
}

void SRVNBlackboardView::HandleUseCurrentValues()
{
	bShowCurrentState = true;
}

void SRVNBlackboardView::HandleUseSavedValues()
{
	bShowCurrentState = false;
}

bool SRVNBlackboardView::HasSelectedItems() const
{
	bool bIsInherited = false;
	return GetSelectedEntry(bIsInherited) != nullptr;
}

bool SRVNBlackboardView::HandleActionMatchesName(FEdGraphSchemaAction* InAction, const FName& InName) const
{
	check(InAction->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());
	FEdGraphSchemaAction_RVNBlackboardEntry* BlackboardEntryAction = static_cast<FEdGraphSchemaAction_RVNBlackboardEntry
		*>(InAction);
	return BlackboardEntryAction->Key.EntryName == InName;
}

#undef LOCTEXT_NAMESPACE
