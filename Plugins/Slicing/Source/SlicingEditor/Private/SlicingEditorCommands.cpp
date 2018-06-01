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
	UI_COMMAND(EnableDebugShowComponents, "Enable Showing Components",
		"Enable components debug view", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(EnableDebugShowPlane, "Enable Slicing Plane",
		"Enable slicing plane debug view", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(EnableDebugShowTrajectory, "Enable Slicing Trajectory",
		"Enable slicing trajectory debug view", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(MakeCuttingObjects, "Make Selected StaticMeshActors Be Able to Cut",
		"Adds components to the sockets of the selected StaticMeshActor, to make it be able to cut other objects", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(MakeCuttableObjects, "Make Selected StaticMeshActors Cuttable",
		"Adds the necessary properties to the selected SteticMeshActors, so they can be cut", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE