#pragma once

#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

class FRVNEditor;

struct FRVNEditorSummoner : FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);

public:
	FRVNEditorSummoner(TSharedPtr<FRVNEditor> InRVNEditorPtr,
	                   FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);

	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;

	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;

protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;

	virtual TSharedRef<SWidget>
	CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;

	virtual const FSlateBrush*
	GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;

protected:
	TWeakPtr<FRVNEditor> RVNEditorPtr;

	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};

struct FDialogueNodeListSummoner : FWorkflowTabFactory
{
public:
	FDialogueNodeListSummoner(TSharedPtr<FRVNEditor> InRVNEditor);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FRVNEditor> RVNEditorPtr;
};

struct FRVNDialogueDetailsSummoner : FWorkflowTabFactory
{
public:
	FRVNDialogueDetailsSummoner(TSharedPtr<FRVNEditor> InRVNEditorPtr);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<FRVNEditor> RVNEditorPtr;
};

struct FRVNBlackboardViewerSummoner : FWorkflowTabFactory
{
public:
	FRVNBlackboardViewerSummoner(TSharedPtr<FRVNEditor> InRVNEditorPtr);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<FRVNEditor> RVNEditorPtr;
};

struct FRVNBlackboardEditorSummoner : FWorkflowTabFactory
{
public:
	FRVNBlackboardEditorSummoner(TSharedPtr<FRVNEditor> InRVNEditorPtr);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<FRVNEditor> RVNEditorPtr;
};

struct FRVNBlackboardDetailsSummoner : FWorkflowTabFactory
{
public:
	FRVNBlackboardDetailsSummoner(TSharedPtr<FRVNEditor> InRVNEditorPtr);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<FRVNEditor> RVNEditorPtr;
};
