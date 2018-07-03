// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Runtime/SlateCore/Public/Widgets/SWidget.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

class FURealisticGraspingEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void InitializeUIButtons();
	void CreateButton();

	TSharedRef<SWidget> CreateOptionMenu();
	void AddOptions(FToolBarBuilder& Builder);
	TSharedPtr<class FUICommandList> PluginCommandList;
};