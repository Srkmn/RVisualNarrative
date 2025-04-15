#pragma once
#include "BlueprintEditorModes.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

#define LOCTEXT_NAMESPACE "RVNEditorMode"

struct FRVNEditorApplicationModes
{
	static const FName RVNEditorDialogueMode;
	static const FName RVNEditorEventMode;
	static const FName RVNEditorBlackboardMode;

	static FText GetLocalizedMode(const FName InMode)
	{
		static TMap<FName, FText> LocModes;

		if (LocModes.Num() == 0)
		{
			LocModes.Add(RVNEditorDialogueMode, LOCTEXT("RVNDialogueName",
			                                            "Dialogue Graph"));

			LocModes.Add(RVNEditorEventMode, LOCTEXT("RVNEventName",
			                                         "Event Graph"));

			LocModes.Add(RVNEditorBlackboardMode, LOCTEXT("RVNBlackboardName",
			                                              "Blackboard"));
		}

		check(InMode != NAME_None);
		const FText* OutDesc = LocModes.Find(InMode);
		check(OutDesc);
		return *OutDesc;
	}
};
#undef LOCTEXT_NAMESPACE

class FRVNEditor;

class FRVNDialogueMode : public FApplicationMode
{
public:
	FRVNDialogueMode(TSharedPtr<FRVNEditor> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;

	virtual void PreDeactivateMode() override;

	virtual void PostActivateMode() override;

protected:
	TWeakPtr<FRVNEditor> RVNEditor;

	FWorkflowAllowedTabSet RVNDialogueTabFactories;
};


class FRVNEventMode : public FBlueprintEditorApplicationMode
{
public:
	FRVNEventMode(TSharedPtr<FRVNEditor> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;

	virtual void PostActivateMode() override;

protected:
	TWeakPtr<FRVNEditor> RVNEditor;

	FWorkflowAllowedTabSet RVNEventTabFactories;
};

class FRVNBlackboardEditorApplicationMode : public FApplicationMode
{
public:
	FRVNBlackboardEditorApplicationMode(TSharedPtr<FRVNEditor> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;

	virtual void PostActivateMode() override;

protected:
	TWeakPtr<FRVNEditor> RVNEditor;

	FWorkflowAllowedTabSet BlackboardTabFactories;
};
