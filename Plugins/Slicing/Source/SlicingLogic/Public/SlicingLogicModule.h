// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FSlicingLogicModule: public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Debug option booleans to be read  */
	bool bEnableDebugConsoleOutput = true;
	bool bEnableDebugShowPlane = false;
	bool bEnableDebugShowTrajectory = false;

	static void ConvertStaticToProceduralMeshComponent(UPrimitiveComponent* StaticMeshComponent);
};