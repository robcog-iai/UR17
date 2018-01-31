// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"

class FSlicingEditorActionCallbacks
{
public:
	/** Static mesh editor */
	static void ShowInstructions();

	static void OnEnableDebugConsoleOutput(bool* bButtonValue);
	static bool OnIsEnableDebugConsoleOutputEnabled(bool* bButtonValue);
	static void OnEnableDebugShowPlane(bool* bButtonValue);
	static bool OnIsEnableDebugShowPlaneEnabled(bool* bButtonValue);
	static void OnEnableDebugShowTrajectory(bool* bButtonValue);
	static bool OnIsEnableDebugShowTrajectoryEnabled(bool* bButtonValue);

	/** Level editor helper functions */
	//* Replaces the marked sockets of a selected StaticMeshComponent to Components that can be used for slicing
	static void FillSocketsWithComponents();
	//* Replaces the marked sockets of ALL StaticMeshComponents to Components that can be used for slicing
	static void ReplaceSocketsOfAllStaticMeshComponents();

private:
	/** Creates the components that fill the sockets */
	static void AddBoxComponent(UStaticMeshComponent* StaticMesh, UBoxComponent* BoxComponent, FName SocketName, FName CollisionProfileName, bool bGenerateOverlapEvents);
	static void AddHandleComponent(UStaticMeshComponent* StaticMesh);
	static void AddBladeComponent(UStaticMeshComponent* StaticMesh);
	static void AddCuttingExitpointComponent(UStaticMeshComponent* StaticMesh);
};