#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Types/SlateEnums.h"
#include "Widgets/Views/SListView.h"

class IDetailLayoutBuilder;
class IPropertyHandle;
class ITableRow;
class STableViewBase;
class SComboButton;
class URVNBlackboardData;

DECLARE_DELEGATE_RetVal_OneParam(int32, FOnGetSelectedRVNBlackboardItemIndex, bool& /* bIsInherited */);

class FRVNBlackboardDataDetails : public IDetailCustomization
{
public:
	FRVNBlackboardDataDetails(FOnGetSelectedRVNBlackboardItemIndex InOnGetSelectedBlackboardItemIndex,
	                          URVNBlackboardData* InBlackboardData)
		: OnGetSelectedBlackboardItemIndex(InOnGetSelectedBlackboardItemIndex)
		  , BlackboardData(InBlackboardData)
	{
	}

	static TSharedRef<IDetailCustomization> MakeInstance(
		FOnGetSelectedRVNBlackboardItemIndex InOnGetSelectedBlackboardItemIndex, URVNBlackboardData* InBlackboardData);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

private:
	FText OnGetKeyCategoryText() const;
	void OnKeyCategoryTextCommitted(const FText& InNewText, ETextCommit::Type InTextCommit);
	void OnKeyCategorySelectionChanged(TSharedPtr<FText> ProposedSelection, ESelectInfo::Type /*SelectInfo*/);
	TSharedRef<ITableRow> MakeKeyCategoryViewWidget(TSharedPtr<FText> Item,
	                                                const TSharedRef<STableViewBase>& OwnerTable);
	void PopulateKeyCategories();

private:
	FOnGetSelectedRVNBlackboardItemIndex OnGetSelectedBlackboardItemIndex;
	TSharedPtr<IPropertyHandle> KeyHandle;
	TSharedPtr<SListView<TSharedPtr<FText>>> KeyCategoryListView;
	TSharedPtr<SComboButton> KeyCategoryComboButton;
	TArray<TSharedPtr<FText>> KeyCategorySource;
	TWeakObjectPtr<URVNBlackboardData> BlackboardData;
};
