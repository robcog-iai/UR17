// Copyright 2018, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class UProceduralMeshComponent;

class FSlicingLogicModule: public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Debug option booleans to be read  */
	bool bEnableDebugConsoleOutput = true;
	bool bEnableDebugShowComponents = false;
	bool bEnableDebugShowPlane = false;
	bool bEnableDebugShowTrajectory = false;

	static void ConvertStaticToProceduralMeshComponent(UPrimitiveComponent* StaticMeshComponent);
	static void ConvertProceduralComponentToStaticMeshActor(UProceduralMeshComponent* ProceduralMeshComponent);
};