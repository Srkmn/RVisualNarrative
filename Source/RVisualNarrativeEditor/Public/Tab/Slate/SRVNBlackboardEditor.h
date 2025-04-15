#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SRVNBlackboardView.h"

DECLARE_DELEGATE_RetVal(bool, FOnIsBlackboardModeActive);

class SRVNBlackboardEditor : public SRVNBlackboardView
{
public:
	SLATE_BEGIN_ARGS(SRVNBlackboardEditor)
		{
		}

		SLATE_EVENT(FOnEntrySelected, OnEntrySelected)
		SLATE_EVENT(FOnBlackboardKeyChanged, OnBlackboardKeyChanged)
		SLATE_EVENT(FOnIsBlackboardModeActive, OnIsBlackboardModeActive)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList,
	               URVNBlackboardData* InBlackboardData);

	void HandleGraphActionChanged(URVNBlackboardData* InBlackboardData);

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	virtual void FillContextMenu(FMenuBuilder& MenuBuilder) const override;

	void FillToolbar(FToolBarBuilder& ToolbarBuilder) const;

	virtual TSharedPtr<FExtender> GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const override;

	void HandleDeleteEntry();

	void HandleRenameEntry();

	TSharedRef<SWidget> HandleCreateNewEntryMenu() const;

	void HandleKeyClassPicked(UClass* InClass);

	bool CanCreateNewEntry() const;

	bool CanDeleteEntry() const;

	bool CanRenameEntry() const;

private:
	FOnIsBlackboardModeActive OnIsBlackboardModeActive;
};
