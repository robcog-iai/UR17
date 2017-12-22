// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SlicingEditorStyle.h"

class FSlicingEditorCommands : public TCommands<FSlicingEditorCommands>
{
public:
	FSlicingEditorCommands(): TCommands<FSlicingEditorCommands>(
		TEXT("Slicing"), // Context name for fast lookup
		NSLOCTEXT("Contexts", "Slicing", "Slicing Plugin"), // Localized context name
		NAME_None, // Parent
		FSlicingEditorStyle::GetStyleSetName() // Icon Style Set
		){}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenPluginWindow;
	TSharedPtr<FUICommandInfo> CreateHandle;
	TSharedPtr<FUICommandInfo> CreateBlade;
	TSharedPtr<FUICommandInfo> CreateCuttingExitpoint;
	TSharedPtr<FUICommandInfo> ShowSlicingElements;
	TSharedPtr<FUICommandInfo> EnableDebugConsoleOutput;
	TSharedPtr<FUICommandInfo> EnableDebugShowPlane;
	TSharedPtr<FUICommandInfo> EnableDebugShowTrajectory;
	TSharedPtr<FUICommandInfo> ReplaceSockets;
};