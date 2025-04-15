#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "UEVersion.h"
#include "RVNEditorStyle.h"

class FRVNEditorCommands final : public TCommands<FRVNEditorCommands>
{
public:
	FRVNEditorCommands()
		: TCommands<FRVNEditorCommands>(
			TEXT("RVisualNarrativeEditor"),
			NSLOCTEXT("Contexts", "RVisualNarrativeEditor", "RVisualNarrativeEditor Plugin"),
			NAME_None,
			FRVNEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenEditorSettings;

	TSharedPtr<FUICommandInfo> OpenRuntimeSettings;

	TSharedPtr<FUICommandInfo> CompactNodesData;

	TSharedPtr<FUICommandInfo> NewBlackboard;
};

class FRVNBlackboardCommands final : public TCommands<FRVNBlackboardCommands>
{
public:
	FRVNBlackboardCommands()
		: TCommands<FRVNBlackboardCommands>(
			TEXT("BTEditor.Blackboard"),
			NSLOCTEXT("Contexts", "Blackboard", "Blackboard"),
			NAME_None,
#if UE_APP_STYLE_GET_STYLE_SET_NAME
			FAppStyle::Get().GetStyleSetName()
#else
			FEditorStyle::GetStyleSetName()
#endif
		)
	{
	}

	TSharedPtr<FUICommandInfo> DeleteEntry;

	virtual void RegisterCommands() override;
};
