// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingEditorCommands.h"

#define LOCTEXT_NAMESPACE "FSlicingEditorModule"

void FSlicingEditorCommands::RegisterCommands()
{
	// Register all the UI elements
	UI_COMMAND(ShowInstructions, "Show Instructions",
		"Show Instructions", EUserInterfaceActionType::ToggleButton, FInputChord());
	
	UI_COMMAND(EnableDebugConsoleOutput, "Enable Console Output",
		"Enable console debug output", EUserInterfaceActionType::ToggleButton, FInputChord(EKeys::PageDown));
	UI_COMMAND(EnableDebugShowPlane, "Enable Slicing Plane",
		"Enable slicing plane debug view", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(EnableDebugShowTrajectory, "Enable Slicing Trajectory",
		"Enable slicing trajectory debug view", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(ReplaceSelectedSockets, "Replace Sockets on Selected Object",
		"Replaces the sockets of the selected Object to Components that are able to detect collision", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ReplaceAllSockets, "Replace Sockets of all StaticMeshComponents",
		"Replaces sockets of all StaticMeshComponents to Components that are able to detect collision", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE