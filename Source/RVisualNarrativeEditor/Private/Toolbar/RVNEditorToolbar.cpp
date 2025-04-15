#include "Toolbar/RVNEditorToolbar.h"
#include "RVNEditor.h"
#include "EditorApplicationMode/RVNEditorModes.h"
#include "WorkflowOrientedApp/SModeWidget.h"
#include "EditorStyleSet.h"
#include "RVNEditorCommands.h"
#include "UEVersion.h"

#define LOCTEXT_NAMESPACE "RVNEditorToolbar"

void FRVNEditorToolbarBuilder::FillDialogueToolbar(FExtender& InExtender)
{
	InExtender.AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		RVNEditorPtr.Pin()->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FRVNEditorToolbarBuilder::AddBlackboardToolbar));

	InExtender.AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		RVNEditorPtr.Pin()->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FRVNEditorToolbarBuilder::FillModeSwitcherToolbar));
}

void FRVNEditorToolbarBuilder::FillEventToolbar(FExtender& InExtender)
{
	InExtender.AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		RVNEditorPtr.Pin()->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FRVNEditorToolbarBuilder::FillModeSwitcherToolbar));
}

void FRVNEditorToolbarBuilder::FillBlackboardToolbar(FExtender& InExtender)
{
	InExtender.AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		RVNEditorPtr.Pin()->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FRVNEditorToolbarBuilder::FillModeSwitcherToolbar));
}

void FRVNEditorToolbarBuilder::AddBlackboardToolbar(FToolBarBuilder& InToolbarBuilder)
{
	InToolbarBuilder.BeginSection("Blackboard");
	{
		InToolbarBuilder.AddToolBarButton(FRVNEditorCommands::Get().NewBlackboard,
		                                  NAME_None,
		                                  TAttribute<FText>(),
		                                  TAttribute<FText>(),
		                                  FSlateIcon(
#if UE_APP_STYLE_GET_STYLE_SET_NAME
			                                  FAppStyle::Get().GetStyleSetName()
#else
			                                  FEditorStyle::GetStyleSetName()
#endif
			                                  , "BTEditor.Common.NewBlackboard"));
	}
	InToolbarBuilder.EndSection();
}

void FRVNEditorToolbarBuilder::FillModeSwitcherToolbar(FToolBarBuilder& InToolbarBuilder)
{
	check(RVNEditorPtr.IsValid())
	const auto RVNEditor = RVNEditorPtr.Pin().Get();

	TAttribute<FName> GetActiveMode(RVNEditor, &FRVNEditor::GetCurrentMode);
	FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(RVNEditor, &FRVNEditor::SetCurrentMode);

	RVNEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

	RVNEditor->AddToolbarWidget(
		SNew(SModeWidget,
		     FRVNEditorApplicationModes::GetLocalizedMode(FRVNEditorApplicationModes::RVNEditorDialogueMode),
		     FRVNEditorApplicationModes::RVNEditorDialogueMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(RVNEditor, &FRVNEditor::CanAccessDialogueMode)
		.ToolTipText(LOCTEXT("DialogueModeButtonTooltip", "Switch to conversation mode"))
		.IconImage(
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			("BTEditor.SwitchToBehaviorTreeMode"))
	);

	RVNEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(5.0f, 1.0f)));

	RVNEditor->AddToolbarWidget(
		SNew(SModeWidget, FRVNEditorApplicationModes::GetLocalizedMode(FRVNEditorApplicationModes::RVNEditorEventMode),
		     FRVNEditorApplicationModes::RVNEditorEventMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(RVNEditor, &FRVNEditor::CanAccessEventMode)
		.ToolTipText(LOCTEXT("EventModeButtonTooltip", "Switch to event mode"))
		.IconImage(
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			("GraphEditor.EventGraph_24x"))
	);

	RVNEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(5.0f, 1.0f)));

	RVNEditor->AddToolbarWidget(
		SNew(SModeWidget,
		     FRVNEditorApplicationModes::GetLocalizedMode(FRVNEditorApplicationModes::RVNEditorBlackboardMode),
		     FRVNEditorApplicationModes::RVNEditorBlackboardMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(RVNEditor, &FRVNEditor::CanAccessBlackboardMode)
		.ToolTipText(LOCTEXT("BlackboardModeButtonTooltip", "Switch to blackboard mode"))
		.IconImage(
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			("BTEditor.SwitchToBlackboardMode"))
	);

	RVNEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));
}

#undef LOCTEXT_NAMESPACE
