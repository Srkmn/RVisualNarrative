#pragma once

#include "BlueprintEditor.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "Blueprint/RVNAssetBlueprint.h"
#include "RVNComponent.h"

struct FRVNBlackboardEntry;
class SRVNBlackboardEditor;
class SRVNBlackboardView;
class URVNComponent;
class URVNAssetBlueprint;
class FDocumentTracker;
class FRVNEditorToolbar;
class URVNDialogueGraph;
class FRVNEditorToolbarBuilder;
class IDetailsView;
class SRVNNodeList;

class FRVNEditor : public FBlueprintEditor
{
public:
	FRVNEditor();

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	void InitRVNEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost,
	                   UBlueprint* InBlueprint);

	void RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager);

	void RestoreRVNEditor();
	void RestoreEventMode();
	void RestoreBlackboardMode();

	void BindCommonCommands();

	void CreateDialogueGraph();
	void CreateEventGraph();
	void CreateGraphCommandList();

	void CreateNewBlackboard();
	bool CanCreateNewBlackboard() const;
	void DeleteSelectedNodesDG();
	void CopySelectedNodesDG();
	void CutSelectedNodesDG();
	void PasteNodesDG();
	bool CanDeleteNodesDG() const;
	bool CanCopyNodesDG() const;
	bool CanCutNodesDG() const;
	bool CanDuplicateNodesDG() const;
	virtual void PasteNodesHere(UEdGraph* DestinationGraph, const FVector2D& GraphLocation) override;
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	void HandleUndoTransactionDG(const FTransaction* Transaction);

	void OpenDocument(UEdGraph* InGraph, FDocumentTracker::EOpenDocumentCause InOpenCause);
	void CloseDocument(UEdGraph* InGraph);

	virtual void OnClose() override;

	void SaveEditedObjectState();

	bool CanAccessDialogueMode() const;
	bool CanAccessEventMode() const;
	bool CanAccessBlackboardMode() const;

	virtual void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor) override;
	void OnSelectedNodesChanged(const TSet<UObject*>& NewSelection);
	void OnNodeDoubleClicked(UEdGraphNode* Node);

public:
	URVNComponent* GetRVNComponent();
	URVNAssetBlueprint* GetRVNEditorBlueprint();
	URVNDialogueGraph* GetRVNDialogueGraph();
	TSharedPtr<FRVNEditorToolbarBuilder> GetRVNToolbarBuilder();
	TSharedPtr<FDocumentTracker> GetDocumentManager();
	URVNBlackboardData* GetBlackboardData();

	TSharedRef<SWidget> SpawnDetailsView();
	TSharedRef<SWidget> SpawnNodeListView();
	TSharedRef<SGraphEditor> CreateRVNGraphEditorWidget(UEdGraph* InGraph);
	TSharedRef<SWidget> SpawnBlackboardView();
	TSharedRef<SWidget> SpawnBlackboardEditor();
	TSharedRef<SWidget> SpawnBlackboardDetails();

	void InitializeBlackboard(URVNBlackboardData* InBlackboardData);
	void HandleBlackboardEntrySelected(const FRVNBlackboardEntry* BlackboardEntry, bool bIsInherited);
	void HandleBlackboardKeyChanged(URVNBlackboardData* InBlackboardData, FRVNBlackboardEntry* const InKey);
	bool HandleIsBlackboardModeActive() const;
	int32 HandleGetSelectedBlackboardItemIndex(bool& bOutIsInherited);

protected:
	virtual void Compile() override;

	virtual FName GetToolkitFName() const override;

	virtual FText GetBaseToolkitName() const override;

	virtual FString GetWorldCentricTabPrefix() const override;

	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual void SaveAsset_Execute() override;

private:
	void CreateInternalWidgets();

	void RefreshRVNEditor(bool bNewlyCreated);

	void ClearDetailsView();

public:
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<FUICommandList> GraphEditorCommandsRef;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SRVNNodeList> NodeListView;
	TSharedPtr<IDetailsView> BlackboardDetailsView;
	TSharedPtr<SRVNBlackboardView> BlackboardView;
	TSharedPtr<SRVNBlackboardEditor> BlackboardEditor;

private:
	TSharedPtr<FDocumentTracker> DocumentManager;
	TWeakPtr<FDocumentTabFactory> GraphEditorTabFactoryPtr;
	TSharedPtr<FRVNEditorToolbarBuilder> RVNToolbarBuilder;

	TObjectPtr<UEdGraph> DialogueGraphPtr;
	TObjectPtr<UEdGraph> EventGraphPtr;

	TObjectPtr<URVNBlackboardData> BlackboardData;
};
