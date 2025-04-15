#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "EdGraph/EdGraphSchema.h"

class SGraphActionMenu;
struct FCreateWidgetForActionData;
struct FRVNBlackboardEntry;
class URVNBlackboardData;

DECLARE_DELEGATE_TwoParams(FOnEntrySelected, const FRVNBlackboardEntry*, bool);
DECLARE_DELEGATE_RetVal(bool, FOnGetDisplayCurrentState);
DECLARE_DELEGATE_TwoParams(FOnBlackboardKeyChanged, URVNBlackboardData*, FRVNBlackboardEntry* const);

class FEdGraphSchemaAction_RVNBlackboardEntry : public FEdGraphSchemaAction_Dummy
{
public:
	static FName StaticGetTypeId();
	virtual FName GetTypeId() const;

	FEdGraphSchemaAction_RVNBlackboardEntry(URVNBlackboardData* InBlackboardData, FRVNBlackboardEntry& InKey,
	                                        bool bInIsInherited);

	void Update();

	URVNBlackboardData* BlackboardData;

	FRVNBlackboardEntry& Key;

	bool bIsInherited;

	bool bIsNew;
};

class SRVNBlackboardView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRVNBlackboardView)
		{
			_IsReadOnly = true;
		}

		SLATE_EVENT(FOnEntrySelected, OnEntrySelected)
		SLATE_EVENT(FOnBlackboardKeyChanged, OnBlackboardKeyChanged)
		SLATE_ARGUMENT(bool, IsReadOnly)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList,
	               URVNBlackboardData* InBlackboardData);

	FRVNBlackboardEntry* GetSelectedEntry(bool& bOutIsInherited) const;

	int32 GetSelectedEntryIndex(bool& bOutIsInherited) const;

	void SetObject(URVNBlackboardData* InBlackboardData);

protected:
	TSharedRef<SWidget> HandleCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData);

	void HandleCollectAllActions(FGraphActionListBuilderBase& GraphActionListBuilder);

	FText HandleGetSectionTitle(int32 SectionID) const;

	void HandleActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedActions,
	                          ESelectInfo::Type InSelectionType) const;

	TSharedPtr<SWidget> HandleContextMenuOpening(TSharedRef<FUICommandList> ToolkitCommands) const;

	virtual void FillContextMenu(FMenuBuilder& MenuBuilder) const;

	virtual TSharedPtr<FExtender> GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const;

	void HandleUseCurrentValues();

	void HandleUseSavedValues();

	bool HasSelectedItems() const;

	TSharedPtr<FEdGraphSchemaAction_RVNBlackboardEntry> GetSelectedEntryInternal() const;

	bool HandleActionMatchesName(FEdGraphSchemaAction* InAction, const FName& InName) const;

protected:
	URVNBlackboardData* BlackboardData;

	TSharedPtr<SGraphActionMenu> GraphActionMenu;

	FOnEntrySelected OnEntrySelected;

	FOnBlackboardKeyChanged OnBlackboardKeyChanged;

	bool bShowCurrentState;
};
