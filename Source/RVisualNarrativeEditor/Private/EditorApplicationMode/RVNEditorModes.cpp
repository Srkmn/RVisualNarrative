#include "EditorApplicationMode/RVNEditorModes.h"

#include "BlueprintEditorTabs.h"
#include "RVNEditor.h"
#include "SBlueprintEditorToolbar.h"
#include "Tab/RVNEditorTabFactories.h"
#include "Tab/RVNEditorTabsID.h"
#include "Toolbar/RVNEditorToolbar.h"

#define LOCTEXT_NAMESPACE "RVNEditorMode"

const FName FRVNEditorApplicationModes::RVNEditorDialogueMode("RVNEditorDialogueMode");
const FName FRVNEditorApplicationModes::RVNEditorEventMode("RVNEditorEventMode");
const FName FRVNEditorApplicationModes::RVNEditorBlackboardMode("RVNEditorBlackboardMode");

FRVNDialogueMode::FRVNDialogueMode(TSharedPtr<FRVNEditor> InEditor)
	: FApplicationMode(FRVNEditorApplicationModes::RVNEditorDialogueMode, FRVNEditorApplicationModes::GetLocalizedMode),
	  RVNEditor(InEditor)
{
	RVNDialogueTabFactories.RegisterFactory(MakeShared<FDialogueNodeListSummoner>(RVNEditor.Pin()));

	RVNDialogueTabFactories.RegisterFactory(MakeShared<FRVNDialogueDetailsSummoner>(RVNEditor.Pin()));

	RVNDialogueTabFactories.RegisterFactory(MakeShared<FRVNBlackboardViewerSummoner>(RVNEditor.Pin()));

	TabLayout = FTabManager::NewLayout("RVNEditor_DialogueMode_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			                             ->Split
			                             (
				                             FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewStack()
					                                                       ->SetSizeCoefficient(0.2f)
					                                                       ->AddTab(
						                                                       FRVNEditorTabsID::DialogueNewNodeList,
						                                                       ETabState::OpenedTab)
				                                                       )

				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewStack()
					                                                       ->SetSizeCoefficient(0.55f)
					                                                       ->AddTab("Document", ETabState::ClosedTab)
				                                                       )

				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewSplitter()->SetOrientation(
						                                                       Orient_Vertical)
					                                                       ->SetSizeCoefficient(0.25f)
					                                                       ->Split
					                                                       (
						                                                       FTabManager::NewStack()
						                                                       ->SetSizeCoefficient(0.6f)
						                                                       ->AddTab
						                                                       (
							                                                       FRVNEditorTabsID::DialogueNodeDetails,
							                                                       ETabState::OpenedTab
						                                                       )
					                                                       )
					                                                       ->Split(
						                                                       FTabManager::NewStack()
						                                                       ->SetSizeCoefficient(0.4f)
						                                                       ->AddTab
						                                                       (
							                                                       FRVNEditorTabsID::BlackboardViewer,
							                                                       ETabState::OpenedTab
						                                                       )
					                                                       )
				                                                       )
			                             )
		);

	RVNEditor.Pin()->GetRVNToolbarBuilder()->FillDialogueToolbar(*ToolbarExtender);

	if (UToolMenu* Toolbar = RVNEditor.Pin()->RegisterModeToolbarIfUnregistered(GetModeName()))
	{
		RVNEditor.Pin()->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
		RVNEditor.Pin()->GetToolbarBuilder()->AddDebuggingToolbar(Toolbar);
	}
}

void FRVNDialogueMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	if (!RVNEditor.IsValid())
	{
		return;
	}

	const auto RVNEditorPtr = RVNEditor.Pin();

	RVNEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	RVNEditorPtr->PushTabFactories(RVNDialogueTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FRVNDialogueMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();

	check(RVNEditor.IsValid())

	RVNEditor.Pin()->SaveEditedObjectState();
}

void FRVNDialogueMode::PostActivateMode()
{
	if (!RVNEditor.IsValid())
	{
		return;
	}

	const auto RVNEditorPtr = RVNEditor.Pin();

	RVNEditorPtr->RestoreRVNEditor();

	FApplicationMode::PostActivateMode();
}

FRVNEventMode::FRVNEventMode(TSharedPtr<FRVNEditor> InEditor)
	: FBlueprintEditorApplicationMode(InEditor, FRVNEditorApplicationModes::RVNEditorEventMode,
	                                  FRVNEditorApplicationModes::GetLocalizedMode, false, false),
	  RVNEditor(InEditor)
{
	TabLayout =
		FTabManager::NewLayout("Standalone_BlueprintEditor_Layout_v17")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			                             ->Split
			                             (
				                             FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewSplitter()->SetOrientation(
						                                                       Orient_Vertical)
					                                                       ->SetSizeCoefficient(0.15f)
					                                                       ->Split
					                                                       (
						                                                       FTabManager::NewStack()->
						                                                       SetSizeCoefficient(1.f)
						                                                       ->AddTab(
							                                                       FBlueprintEditorTabs::MyBlueprintID,
							                                                       ETabState::OpenedTab)
					                                                       )
				                                                       )

				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewSplitter()->SetOrientation(
						                                                       Orient_Vertical)
					                                                       ->SetSizeCoefficient(0.70f)
					                                                       ->Split
					                                                       (
						                                                       FTabManager::NewStack()
						                                                       ->SetSizeCoefficient(0.80f)
						                                                       ->AddTab(
							                                                       "Document", ETabState::ClosedTab)
					                                                       )
					                                                       ->Split
					                                                       (
						                                                       FTabManager::NewStack()
						                                                       ->SetSizeCoefficient(0.20f)
						                                                       ->AddTab(
							                                                       FBlueprintEditorTabs::CompilerResultsID,
							                                                       ETabState::ClosedTab)
						                                                       ->AddTab(
							                                                       FBlueprintEditorTabs::FindResultsID,
							                                                       ETabState::ClosedTab)
						                                                       ->AddTab(
							                                                       FBlueprintEditorTabs::BookmarksID,
							                                                       ETabState::ClosedTab)
					                                                       )
				                                                       )
				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewSplitter()->SetOrientation(
						                                                       Orient_Vertical)
					                                                       ->SetSizeCoefficient(0.15f)
					                                                       ->Split
					                                                       (
						                                                       FTabManager::NewStack()->
						                                                       SetSizeCoefficient(1.f)
						                                                       ->AddTab(FBlueprintEditorTabs::DetailsID,
							                                                       ETabState::OpenedTab)
						                                                       ->AddTab(FBlueprintEditorTabs::PaletteID,
							                                                       ETabState::ClosedTab)
						                                                       ->AddTab(
							                                                       FBlueprintEditorTabs::DefaultEditorID,
							                                                       ETabState::ClosedTab)
					                                                       )
				                                                       )
			                             )
		);

	RVNEditor.Pin()->GetRVNToolbarBuilder()->FillEventToolbar(*ToolbarExtender);

	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorModule.OnRegisterTabsForEditor().Broadcast(RVNEventTabFactories,
	                                                          FRVNEditorApplicationModes::RVNEditorEventMode,
	                                                          RVNEditor.Pin());

	LayoutExtender = MakeShared<FLayoutExtender>();
	BlueprintEditorModule.OnRegisterLayoutExtensions().Broadcast(*LayoutExtender);

	if (UToolMenu* Toolbar = RVNEditor.Pin()->RegisterModeToolbarIfUnregistered(GetModeName()))
	{
		RVNEditor.Pin()->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
		RVNEditor.Pin()->GetToolbarBuilder()->AddBlueprintGlobalOptionsToolbar(Toolbar);
		RVNEditor.Pin()->GetToolbarBuilder()->AddDebuggingToolbar(Toolbar);
		RVNEditor.Pin()->GetToolbarBuilder()->AddScriptingToolbar(Toolbar);
		RVNEditor.Pin()->GetToolbarBuilder()->AddNewToolbar(Toolbar);
	}
}

void FRVNEventMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	if (!RVNEditor.IsValid())
	{
		return;
	}

	const auto RVNEditorPtr = RVNEditor.Pin();

	RVNEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	RVNEditorPtr->PushTabFactories(CoreTabFactories);
	RVNEditorPtr->PushTabFactories(BlueprintEditorOnlyTabFactories);
	RVNEditorPtr->PushTabFactories(BlueprintEditorTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FRVNEventMode::PostActivateMode()
{
	if (!RVNEditor.IsValid())
	{
		return;
	}

	const auto RVNEditorPtr = RVNEditor.Pin();

	RVNEditorPtr->RestoreEditedObjectState();
	RVNEditorPtr->SetupViewForBlueprintEditingMode();

	RVNEditorPtr->RestoreEventMode();

	FApplicationMode::PostActivateMode();
}

FRVNBlackboardEditorApplicationMode::FRVNBlackboardEditorApplicationMode(TSharedPtr<FRVNEditor> InEditor):
	FApplicationMode(FRVNEditorApplicationModes::RVNEditorBlackboardMode, FRVNEditorApplicationModes::GetLocalizedMode),
	RVNEditor(InEditor)
{
	BlackboardTabFactories.RegisterFactory(MakeShared<FRVNBlackboardEditorSummoner>(RVNEditor.Pin()));
	BlackboardTabFactories.RegisterFactory(MakeShared<FRVNBlackboardDetailsSummoner>(RVNEditor.Pin()));

	TabLayout = FTabManager::NewLayout("RVNEditor_BlackboardMode_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			                             ->Split
			                             (
				                             FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewStack()
					                                                       ->AddTab(FRVNEditorTabsID::BlackboardEditor,
						                                                       ETabState::OpenedTab)
				                                                       )
				                                                       ->Split
				                                                       (
					                                                       FTabManager::NewStack()
					                                                       ->AddTab(FRVNEditorTabsID::BlackboardDetails,
						                                                       ETabState::OpenedTab)
				                                                       )
			                             )
		);

	RVNEditor.Pin()->GetRVNToolbarBuilder()->FillBlackboardToolbar(*ToolbarExtender);
}

void FRVNBlackboardEditorApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	if (!RVNEditor.IsValid())
	{
		return;
	}

	const auto RVNEditorPtr = RVNEditor.Pin();

	RVNEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	RVNEditorPtr->PushTabFactories(BlackboardTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FRVNBlackboardEditorApplicationMode::PostActivateMode()
{
	if (!RVNEditor.IsValid())
	{
		return;
	}

	const auto RVNEditorPtr = RVNEditor.Pin();

	RVNEditorPtr->RestoreBlackboardMode();

	FApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE
