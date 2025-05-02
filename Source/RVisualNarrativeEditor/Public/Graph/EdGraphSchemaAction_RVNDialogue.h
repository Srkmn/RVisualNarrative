#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchemaAction_RVNDialogue.generated.h"

USTRUCT()
struct RVISUALNARRATIVEEDITOR_API FEdGraphSchemaAction_Dialogue_State : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FEdGraphSchemaAction_Dialogue_State()
	{
	};

	FEdGraphSchemaAction_Dialogue_State(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
	                                    bool bSelectNewNode = true) override;
};

USTRUCT()
struct RVISUALNARRATIVEEDITOR_API FEdGraphSchemaAction_Dialogue_Selector : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FEdGraphSchemaAction_Dialogue_Selector()
	{
	};

	FEdGraphSchemaAction_Dialogue_Selector(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
	                                    bool bSelectNewNode = true) override;
};
