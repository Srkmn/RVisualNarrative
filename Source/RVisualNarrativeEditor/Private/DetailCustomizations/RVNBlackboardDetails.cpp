#include "DetailCustomizations/RVNBlackboardDetails.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "RVNBlackboardData.h"
#include "UEVersion.h"

#define LOCTEXT_NAMESPACE "RVNBlackboardDataDetails"

TSharedRef<IDetailCustomization> FRVNBlackboardDataDetails::MakeInstance(
	FOnGetSelectedRVNBlackboardItemIndex InOnGetSelectedBlackboardItemIndex, URVNBlackboardData* InBlackboardData)
{
	return MakeShareable(new FRVNBlackboardDataDetails(InOnGetSelectedBlackboardItemIndex, InBlackboardData));
}

void FRVNBlackboardDataDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	DetailLayout.HideProperty(TEXT("Keys"));
	DetailLayout.HideProperty(TEXT("ParentKeys"));

	bool bIsInherited = false;

	int32 CurrentSelection = INDEX_NONE;

	if (OnGetSelectedBlackboardItemIndex.IsBound())
	{
		CurrentSelection = OnGetSelectedBlackboardItemIndex.Execute(bIsInherited);
	}

	if (CurrentSelection >= 0)
	{
		TSharedPtr<IPropertyHandle> KeysHandle = bIsInherited
			                                         ? DetailLayout.GetProperty(TEXT("ParentKeys"))
			                                         : DetailLayout.GetProperty(TEXT("Keys"));

		check(KeysHandle.IsValid());

		uint32 NumChildKeys = 0;
		KeysHandle->GetNumChildren(NumChildKeys);

		if ((uint32)CurrentSelection < NumChildKeys)
		{
			KeyHandle = KeysHandle->GetChildHandle((uint32)CurrentSelection);

			IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory("Key");
			TSharedPtr<IPropertyHandle> EntryNameProperty = KeyHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, EntryName));
			DetailCategoryBuilder.AddCustomRow(LOCTEXT("EntryNameLabel", "Entry Name"))
			                     .NameContent()
				[
					EntryNameProperty->CreatePropertyNameWidget()
				]
				.ValueContent()
				[
					SNew(SHorizontalBox)
					.IsEnabled(true)
					+ SHorizontalBox::Slot()
					[
						EntryNameProperty->CreatePropertyValueWidget()
					]
				];

			PopulateKeyCategories();

			const FText CategoryTooltip = LOCTEXT("BlackboardDataDetails_EditCategoryName_Tooltip",
			                                      "The category of the variable; editing this will place the variable into another category or create a new one.");
			TSharedPtr<IPropertyHandle> EntryCategoryProperty = KeyHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, EntryCategory));

			DetailCategoryBuilder.AddCustomRow(LOCTEXT("BlackboardDataDetails_EntryCategoryLabel", "Entry Category"))
			                     .NameContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("BlackboardDataDetails_EntryCategoryLabel", "Entry Category"))
					.ToolTipText(CategoryTooltip)
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ValueContent()
				[
					SAssignNew(KeyCategoryComboButton, SComboButton)
					.IsEnabled(EntryCategoryProperty.IsValid() && !EntryCategoryProperty->IsEditConst())
					.ContentPadding(FMargin(0, 0, 5, 0))
					.ButtonContent()
					[
						SNew(SBorder)
						.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
							FAppStyle::Get().GetBrush
#else
							FEditorStyle::GetBrush
#endif
							("NoBorder"))
						.Padding(FMargin(0, 0, 5, 0))
						[
							SNew(SEditableTextBox)
							.Text(this, &FRVNBlackboardDataDetails::OnGetKeyCategoryText)
							.OnTextCommitted(this, &FRVNBlackboardDataDetails::OnKeyCategoryTextCommitted)
							.ToolTipText(CategoryTooltip)
							.SelectAllTextWhenFocused(true)
							.RevertTextOnEscape(true)
							.Font(IDetailLayoutBuilder::GetDetailFont())
						]
					]
					.MenuContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.MaxHeight(400.0f)
						[
							SAssignNew(KeyCategoryListView, SListView<TSharedPtr<FText>>)
							.ListItemsSource(&KeyCategorySource)
							.OnGenerateRow(this, &FRVNBlackboardDataDetails::MakeKeyCategoryViewWidget)
							.OnSelectionChanged(this, &FRVNBlackboardDataDetails::OnKeyCategorySelectionChanged)
						]
					]
				];

			TSharedPtr<IPropertyHandle> EntryDescriptionHandle = KeyHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, EntryDescription));
			DetailCategoryBuilder.AddProperty(EntryDescriptionHandle);

			TSharedPtr<IPropertyHandle> KeyTypeProperty = KeyHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, KeyType));
			DetailCategoryBuilder.AddProperty(KeyTypeProperty);

			TSharedPtr<IPropertyHandle> bInstanceSyncedProperty = KeyHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, bInstanceSynced));
			DetailCategoryBuilder.AddProperty(bInstanceSyncedProperty);
		}
	}
}

FText FRVNBlackboardDataDetails::OnGetKeyCategoryText() const
{
	FName PropertyCategoryText;

	check(KeyHandle.IsValid())
	TSharedPtr<IPropertyHandle> EntryCategoryProperty = KeyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, EntryCategory));
	if (EntryCategoryProperty.IsValid())
	{
		EntryCategoryProperty->GetValue(PropertyCategoryText);
	}
	return FText::FromName(PropertyCategoryText);
}

void FRVNBlackboardDataDetails::OnKeyCategoryTextCommitted(const FText& InNewText, ETextCommit::Type InTextCommit)
{
	check(KeyHandle.IsValid())
	if (InTextCommit == ETextCommit::OnEnter || InTextCommit == ETextCommit::OnUserMovedFocus)
	{
		TSharedPtr<IPropertyHandle> EntryCategoryProperty = KeyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, EntryCategory));
		if (EntryCategoryProperty.IsValid())
		{
			EntryCategoryProperty->SetValue(FName(*InNewText.ToString()));
		}
		PopulateKeyCategories();
	}
}

void FRVNBlackboardDataDetails::OnKeyCategorySelectionChanged(TSharedPtr<FText> ProposedSelection, ESelectInfo::Type)
{
	check(KeyHandle.IsValid());
	if (ProposedSelection.IsValid())
	{
		FText NewCategory = *ProposedSelection.Get();
		TSharedPtr<IPropertyHandle> EntryCategoryProperty = KeyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, EntryCategory));
		if (EntryCategoryProperty.IsValid())
		{
			EntryCategoryProperty->SetValue(FName(*NewCategory.ToString()));
		}

		check(KeyCategoryListView.IsValid());
		check(KeyCategoryComboButton.IsValid());

		KeyCategoryListView->ClearSelection();
		KeyCategoryComboButton->SetIsOpen(false);
	}
}

TSharedRef<ITableRow> FRVNBlackboardDataDetails::MakeKeyCategoryViewWidget(TSharedPtr<FText> Item,
                                                                           const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(*Item.Get())
		];
}

void FRVNBlackboardDataDetails::PopulateKeyCategories()
{
	KeyCategorySource.Reset();
	KeyCategorySource.Add(MakeShareable(new FText(LOCTEXT("None", "None"))));
	if (!BlackboardData.IsValid())
	{
		return;
	}

	TArray<FRVNBlackboardEntry> AllKeys;
	AllKeys.Append(BlackboardData->ParentKeys);
	AllKeys.Append(BlackboardData->Keys);

	TArray<FName> UniqueCategories;
	for (const FRVNBlackboardEntry& Entry : AllKeys)
	{
		if (!Entry.EntryCategory.IsNone())
		{
			UniqueCategories.AddUnique(Entry.EntryCategory);
		}
	}

	for (const FName& Category : UniqueCategories)
	{
		KeyCategorySource.Add(MakeShareable(new FText(FText::FromName(Category))));
	}
}

#undef LOCTEXT_NAMESPACE
