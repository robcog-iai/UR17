// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"

class FSlicingEditorActionCallbacks
{
public:
	/** Static mesh editor */
	static void CreateBlade();
	static void CreateCuttingExitpoint();
	static void ShowSlicingElements();

	/** Level editor debug options */
	static void OnEnableDebugConsoleOutput(bool* bButtonValue);
	static bool OnIsEnableDebugConsoleOutputEnabled(bool* bButtonValue);
	static void OnEnableDebugShowPlane(bool* bButtonValue);
	static bool OnIsEnableDebugShowPlaneEnabled(bool* bButtonValue);
	static void OnEnableDebugShowTrajectory(bool* bButtonValue);
	static bool OnIsEnableDebugShowTrajectoryEnabled(bool* bButtonValue);
	static void ReplaceSocketsWithUseableComponents();
};

