#include "Tab/Slate/SRVNBlackboardEditor.h"
#include "Modules/ModuleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "EditorStyleSet.h"
#include "ClassViewerModule.h"
#include "SGraphActionMenu.h"
#include "ScopedTransaction.h"
#include "ClassViewerFilter.h"
#include "RVNBlackboardData.h"
#include "RVNBlackboardKeyType.h"
#include "RVNEditorCommands.h"
#include "Framework/Commands/GenericCommands.h"
#include "UEVersion.h"

#define LOCTEXT_NAMESPACE "SRVNBlackboardEditor"

void SRVNBlackboardEditor::Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList,
                                     URVNBlackboardData* InBlackboardData)
{
	OnEntrySelected = InArgs._OnEntrySelected;
	OnIsBlackboardModeActive = InArgs._OnIsBlackboardModeActive;
	InBlackboardData->OnBlackboardDataChanged.AddRaw(this, &SRVNBlackboardEditor::HandleGraphActionChanged);

	TSharedRef<FUICommandList> CommandList = MakeShareable(new FUICommandList);

	CommandList->MapAction(
		FRVNBlackboardCommands::Get().DeleteEntry,
		FExecuteAction::CreateSP(this, &SRVNBlackboardEditor::HandleDeleteEntry),
		FCanExecuteAction::CreateSP(this, &SRVNBlackboardEditor::CanDeleteEntry)
	);

	CommandList->MapAction(
		FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &SRVNBlackboardEditor::HandleRenameEntry),
		FCanExecuteAction::CreateSP(this, &SRVNBlackboardEditor::CanRenameEntry)
	);

	InCommandList->Append(CommandList);

	SRVNBlackboardView::Construct(
		SRVNBlackboardView::FArguments()
		.OnEntrySelected(InArgs._OnEntrySelected)
		.OnBlackboardKeyChanged(InArgs._OnBlackboardKeyChanged)
		.IsReadOnly(false),
		CommandList,
		InBlackboardData
	);
}

void SRVNBlackboardEditor::HandleGraphActionChanged(URVNBlackboardData* InBlackboardData)
{
	if (InBlackboardData != BlackboardData)
	{
		return;
	}

	GraphActionMenu->RefreshAllActions(true);
	if (InBlackboardData != nullptr)
	{
		OnBlackboardKeyChanged.ExecuteIfBound(InBlackboardData, nullptr);
	}
}

FReply SRVNBlackboardEditor::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::F2 && CanRenameEntry())
	{
		HandleRenameEntry();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SRVNBlackboardEditor::FillContextMenu(FMenuBuilder& MenuBuilder) const
{
	if (HasSelectedItems())
	{
		MenuBuilder.AddMenuEntry(FRVNBlackboardCommands::Get().DeleteEntry, NAME_None, LOCTEXT("Delete", "Delete"),
		                         LOCTEXT("Delete_Tooltip", "Deletes this blackboard entry."),
		                         FSlateIcon(FAppStyle::GetAppStyleSetName(), "MyBlueprint.DeleteEntry"));
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("Rename", "Rename"),
		                         LOCTEXT("Rename_Tooltip", "Renames this blackboard entry."));
	}
}

void SRVNBlackboardEditor::FillToolbar(FToolBarBuilder& ToolbarBuilder) const
{
	ToolbarBuilder.AddComboButton(
		FUIAction(
			FExecuteAction(),
			FCanExecuteAction::CreateSP(this, &SRVNBlackboardEditor::CanCreateNewEntry)
		),
		FOnGetContent::CreateSP(this, &SRVNBlackboardEditor::HandleCreateNewEntryMenu),
		LOCTEXT("New_Label", "New Key"),
		LOCTEXT("New_ToolTip", "Create a new blackboard entry"),
		FSlateIcon(
#if UE_APP_STYLE_GET_STYLE_SET_NAME
			FAppStyle::Get().GetStyleSetName()
#else
			FEditorStyle::GetStyleSetName()
#endif
			, "BTEditor.Blackboard.NewEntry")
	);
}

TSharedPtr<FExtender> SRVNBlackboardEditor::GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const
{
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Debugging", EExtensionHook::Before, ToolkitCommands,
	                                     FToolBarExtensionDelegate::CreateSP(this, &SRVNBlackboardEditor::FillToolbar));

	return ToolbarExtender;
}

void SRVNBlackboardEditor::HandleDeleteEntry()
{
	if (BlackboardData == nullptr)
	{
		return;
	}

	{
		bool bIsInherited = false;
		FRVNBlackboardEntry* BlackboardEntry = GetSelectedEntry(bIsInherited);
		if (BlackboardEntry != nullptr && !bIsInherited)
		{
			const FScopedTransaction
				Transaction(LOCTEXT("BlackboardEntryDeleteTransaction", "Delete Blackboard Entry"));
			BlackboardData->SetFlags(RF_Transactional);
			BlackboardData->Modify();

			FProperty* KeysProperty = FindFProperty<FProperty>(URVNBlackboardData::StaticClass(),
			                                                   GET_MEMBER_NAME_CHECKED(URVNBlackboardData, Keys));
			BlackboardData->PreEditChange(KeysProperty);

			for (int32 ItemIndex = 0; ItemIndex < BlackboardData->Keys.Num(); ItemIndex++)
			{
				if (BlackboardEntry == &BlackboardData->Keys[ItemIndex])
				{
					BlackboardData->Keys.RemoveAt(ItemIndex);
					break;
				}
			}

			HandleGraphActionChanged(BlackboardData);

			if (OnEntrySelected.IsBound())
			{
				OnEntrySelected.Execute(nullptr, false);
			}

			FPropertyChangedEvent PropertyChangedEvent(KeysProperty, EPropertyChangeType::ArrayRemove);
			BlackboardData->PostEditChangeProperty(PropertyChangedEvent);
		}
	}
}

void SRVNBlackboardEditor::HandleRenameEntry()
{
	GraphActionMenu->OnRequestRenameOnActionNode();
}

bool SRVNBlackboardEditor::CanDeleteEntry() const
{
	const bool bModeActive = OnIsBlackboardModeActive.IsBound() && OnIsBlackboardModeActive.Execute();

	if (bModeActive)
	{
		bool bIsInherited = false;
		FRVNBlackboardEntry* BlackboardEntry = GetSelectedEntry(bIsInherited);
		if (BlackboardEntry != nullptr)
		{
			return !bIsInherited;
		}
	}

	return false;
}

bool SRVNBlackboardEditor::CanRenameEntry() const
{
	const bool bModeActive = OnIsBlackboardModeActive.IsBound() && OnIsBlackboardModeActive.Execute();

	if (bModeActive)
	{
		bool bIsInherited = false;
		FRVNBlackboardEntry* BlackboardEntry = GetSelectedEntry(bIsInherited);
		if (BlackboardEntry != nullptr)
		{
			return !bIsInherited;
		}
	}

	return false;
}

class FRVNBlackboardEntryClassFilter : public IClassViewerFilter
{
public:
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass,
	                            TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		if (InClass != nullptr)
		{
			return !InClass->HasAnyClassFlags(CLASS_Abstract | CLASS_HideDropDown) &&
				InClass->HasAnyClassFlags(CLASS_EditInlineNew) &&
				InClass->IsChildOf(URVNBlackboardKeyType::StaticClass());
		}
		return false;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
	                                    const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData,
	                                    TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return InUnloadedClassData->IsChildOf(URVNBlackboardKeyType::StaticClass());
	}
};


TSharedRef<SWidget> SRVNBlackboardEditor::HandleCreateNewEntryMenu() const
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;
	Options.ClassFilters.Add(MakeShareable(new FRVNBlackboardEntryClassFilter));

	FOnClassPicked OnPicked(FOnClassPicked::CreateRaw(const_cast<SRVNBlackboardEditor*>(this),
	                                                  &SRVNBlackboardEditor::HandleKeyClassPicked));

	TSharedRef<SEditableTextBox> FilterTextBox = GraphActionMenu->GetFilterTextBox();
	FilterTextBox->SetText(FText());

	return
			SNew(SBox)
			.HeightOverride(240.0f)
			.WidthOverride(200.0f)
			[
				FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(
					Options, OnPicked)
			];
}

void SRVNBlackboardEditor::HandleKeyClassPicked(UClass* InClass)
{
	if (BlackboardData == nullptr)
	{
		return;
	}

	FSlateApplication::Get().DismissAllMenus();

	check(InClass);
	check(InClass->IsChildOf(URVNBlackboardKeyType::StaticClass()));

	const FScopedTransaction Transaction(LOCTEXT("BlackboardEntryAddTransaction", "Add Blackboard Entry"));
	BlackboardData->SetFlags(RF_Transactional);
	BlackboardData->Modify();

	FProperty* KeysProperty = FindFProperty<FProperty>(URVNBlackboardData::StaticClass(),
	                                                   GET_MEMBER_NAME_CHECKED(URVNBlackboardData, Keys));
	BlackboardData->PreEditChange(KeysProperty);

	FString NewKeyName = InClass->GetDisplayNameText().ToString();
	NewKeyName = NewKeyName.Replace(TEXT(" "), TEXT(""));
	NewKeyName += TEXT("Key");

	int32 IndexSuffix = -1;
	auto DuplicateFunction = [&](const FRVNBlackboardEntry& Key)
	{
		if (Key.EntryName.ToString() == NewKeyName)
		{
			IndexSuffix = FMath::Max(0, IndexSuffix);
		}
		if (Key.EntryName.ToString().StartsWith(NewKeyName))
		{
			const FString ExistingSuffix = Key.EntryName.ToString().RightChop(NewKeyName.Len());
			if (ExistingSuffix.IsNumeric())
			{
				IndexSuffix = FMath::Max(FCString::Atoi(*ExistingSuffix) + 1, IndexSuffix);
			}
		}
	};

	for (const auto& Key : BlackboardData->Keys)
	{
		DuplicateFunction(Key);
	}

	for (const auto& ParentKey : BlackboardData->ParentKeys)
	{
		DuplicateFunction(ParentKey);
	}

	if (IndexSuffix != -1)
	{
		NewKeyName += FString::Printf(TEXT("%d"), IndexSuffix);
	}

	FRVNBlackboardEntry Entry;
	Entry.EntryName = FName(*NewKeyName);
	Entry.KeyType = NewObject<URVNBlackboardKeyType>(BlackboardData, InClass);

	BlackboardData->Keys.Add(Entry);

	GraphActionMenu->RefreshAllActions(true);
	OnBlackboardKeyChanged.ExecuteIfBound(BlackboardData, &BlackboardData->Keys.Last());

	GraphActionMenu->SelectItemByName(Entry.EntryName, ESelectInfo::OnMouseClick);

	TArray<TSharedPtr<FEdGraphSchemaAction>> SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);

	check(SelectedActions.Num() == 1);
	check(SelectedActions[0]->GetTypeId() == FEdGraphSchemaAction_RVNBlackboardEntry::StaticGetTypeId());

	TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> BlackboardEntryAction = StaticCastSharedPtr<
		FEdGraphSchemaAction_RVNBlackboardEntry>(SelectedActions[0]);
	BlackboardEntryAction->bIsNew = true;

	GraphActionMenu->OnRequestRenameOnActionNode();

	FPropertyChangedEvent PropertyChangedEvent(KeysProperty, EPropertyChangeType::ArrayAdd);
	BlackboardData->PostEditChangeProperty(PropertyChangedEvent);
}

bool SRVNBlackboardEditor::CanCreateNewEntry() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
