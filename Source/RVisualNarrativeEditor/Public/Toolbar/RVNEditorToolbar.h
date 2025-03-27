﻿#pragma once

#include "CoreMinimal.h"
#include "Slate/DebugCanvas.h"

class FRVNEditor;

class FRVNEditorToolbarBuilder : public TSharedFromThis<FRVNEditorToolbarBuilder>
{
public:
	FRVNEditorToolbarBuilder(TSharedPtr<FRVNEditor> InRVNEditor)
		: RVNEditorPtr(InRVNEditor)
	{
	}

	void FillDialogueToolbar(FExtender& InExtender);
	void FillEventToolbar(FExtender& InExtender);

	void FillModeSwitcherToolbar(FToolBarBuilder& InToolbarBuilder);

protected:
	TWeakPtr<FRVNEditor> RVNEditorPtr;
};
