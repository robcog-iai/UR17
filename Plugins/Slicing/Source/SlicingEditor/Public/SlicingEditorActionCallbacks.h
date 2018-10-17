// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"

#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"

class FSlicingEditorActionCallbacks
{
public:
	/**** Static mesh editor ****/
	static void ShowInstructions();

	static void OnEnableDebugConsoleOutput(bool* bButtonValue);
	static bool OnIsEnableDebugConsoleOutputEnabled(bool* bButtonValue);
	static void OnEnableDebugShowComponents(bool* bButtonValue);
	static bool OnIsEnableDebugShowComponentsEnabled(bool* bButtonValue);
	static void OnEnableDebugShowPlane(bool* bButtonValue);
	static bool OnIsEnableDebugShowPlaneEnabled(bool* bButtonValue);
	static void OnEnableDebugShowTrajectory(bool* bButtonValue);
	static bool OnIsEnableDebugShowTrajectoryEnabled(bool* bButtonValue);

	/**** Level editor helper functions ****/
	//* Adds components needed to make the object be able to cut other objects (proper sockets are needed)
	static void MakeCuttingObjects();
	//* Adds/changes properties to make the object be able to get cut by other objects
	static void MakeCuttableObjects();

private:
	static TArray<AStaticMeshActor*> GetSelectedStaticMeshActors();

	/**** Creates the components that fill the sockets ****/
	static void AddBoxComponent(UStaticMeshComponent* StaticMesh, UBoxComponent* BoxComponent, FName SocketName,
		FName CollisionProfileName, bool bGenerateOverlapEvents);
	static void AddHandleComponent(UStaticMeshComponent* StaticMesh);
	static void AddBladeComponent(UStaticMeshComponent* StaticMesh);
	static void AddTipComponent(UStaticMeshComponent* StaticMesh);
	static void AddDebugVisualComponent(AStaticMeshActor* StaticMesh);
};