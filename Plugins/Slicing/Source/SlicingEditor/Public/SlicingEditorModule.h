// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FSlicingEditorModule: public IModuleInterface
{
public:
	/**** IModuleInterface implementation ****/
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	TSharedPtr<class FUICommandList> PluginCommandList;

	/**** Creating the UI elements ****/
	void InitializeUIButtons();
	void CreateInstructionsButton();
	void CreateDebugButtons();

	/**** Needed to build the UI elements ****/
	TSharedRef<SWidget> CreateDebugOptionMenu();
	void AddInstructionsButton(FToolBarBuilder& Builder);
	void AddDebugOptions(FToolBarBuilder& Builder);
};