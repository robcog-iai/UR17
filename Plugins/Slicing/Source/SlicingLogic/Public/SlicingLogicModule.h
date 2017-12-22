// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

#include "UObject/ObjectMacros.h"

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

	// 0 = CreateNewSectionForCap (default), 1 = No Cap, 2 = UseLastSectionForCap
	int NewSectionOptions = 0;

	bool bDebugPrintText = false;
	bool bDebugShowPlane = false;
	bool bDebugShowWay = false;

	void SetMaterialForSection(UMaterialInterface* InputTexture);

private:
	void TestingThisShit();

	UMaterialInterface* MaterialReferenceNewSection = NULL;

	void Debug(UPrimitiveComponent* InputBladeComponent);
	void ConvertToProceduralMeshComponent(UPrimitiveComponent* ReferencedComponent);
	void CutGivenComponent(UPrimitiveComponent* ReferencedComponent, FVector PlanePosition, FVector PlaneNormal);
};