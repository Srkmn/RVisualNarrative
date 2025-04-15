#include "RVNEditorCommands.h"

#define LOCTEXT_NAMESPACE "FRVNEditorCommands"

void FRVNEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenEditorSettings, "Open Editor Settings", "Open RVNEditor Settings Action",
	           EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(OpenRuntimeSettings, "Open Runtime Settings", "Open RVNRuntime Settings Action",
	           EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CompactNodesData, "Compact Nodes Data", "Compact RVNComponent Nodes Data",
	           EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(NewBlackboard, "Create Blackboard", "Create a new Blackboard Data Asset",
	           EUserInterfaceActionType::Button, FInputChord());
}

void FRVNBlackboardCommands::RegisterCommands()
{
	UI_COMMAND(DeleteEntry, "Delete", "Delete this blackboard entry",
	           EUserInterfaceActionType::Button, FInputChord(EKeys::Platform_Delete));
}

#undef LOCTEXT_NAMESPACE
