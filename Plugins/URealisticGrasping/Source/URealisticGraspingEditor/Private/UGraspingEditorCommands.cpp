// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UGraspingEditorCommands.h"

#define LOCTEXT_NAMESPACE "FGraspingEditorModule"

void UGraspingEditorCommands::RegisterCommands() {
	// Register all the UI elements

	UI_COMMAND(CreateGraspingStyle, "Create new grasping style",
		"Create new grasping style", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(LoadGraspingStyle, "Load grasping style",
		"Create new grasping style", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(SaveGraspingPosition, "Save hand position",
		"Save hand position", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(EditGraspingPosition, "Edit hand position",
		"Edit hand position", EUserInterfaceActionType::ToggleButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE