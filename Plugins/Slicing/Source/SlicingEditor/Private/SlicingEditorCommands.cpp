// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingEditorCommands.h"

#define LOCTEXT_NAMESPACE "FSlicingEditorModule"

void FSlicingEditorCommands::RegisterCommands()
{
	// Register all the UI elements
	UI_COMMAND(CreateHandle, "Add Handle",
		"Generates a new box to signify the area used as the handle.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CreateBlade, "Add Blade",
		"Generates a new box to signify the area used as the blade.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CreateCuttingExitpoint, "Add Cutting Exitpoint",
		"Generates a new box to signify the area at which cutting is supposed to be aborted.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ShowSlicingElements, "Slicing",
		"Toggles display of the slicing elements in the Preview Pane.", EUserInterfaceActionType::ToggleButton, FInputChord());
	
	UI_COMMAND(EnableDebugConsoleOutput, "Enable Console Output",
		"Enable console debug output", EUserInterfaceActionType::ToggleButton, FInputChord(EKeys::PageDown));
	UI_COMMAND(EnableDebugShowPlane, "Enable Slicing Plane",
		"Enable slicing plane debug view", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(EnableDebugShowTrajectory, "Enable Slicing Trajectory",
		"Enable slicing trajectory debug view", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(ReplaceSockets, "Replace Sockets to Useable Components",
		"Replaces sockets created by the slicing plugin to Components that are able to detect collision", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE