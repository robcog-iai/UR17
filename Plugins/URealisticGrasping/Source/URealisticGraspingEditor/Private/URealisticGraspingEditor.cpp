// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "URealisticGraspingEditor.h"

#include "UGraspingEditorStyle.h"
#include "UGraspingEditorCommands.h"
#include "UGraspingEditorCallback.h"

#include "Engine.h"
#include "LevelEditor.h"
#include "ISkeletalMeshEditor.h"
#include "ISkeletalMeshEditorModule.h"
#include "ISkeletonEditor.h"
#include "ISkeletonEditorModule.h"

#define LOCTEXT_NAMESPACE "FURealisticGraspingEditorModule"

void FURealisticGraspingEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UGraspingEditorStyle::Initialize();
	UGraspingEditorStyle::ReloadTextures();

	//Initializes the drop down menu.
	InitializeUIButtons();

	//Creates the button in the menu bar fo the skeleton editor.
	CreateButton();
}

void FURealisticGraspingEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	//UGraspingEditorStyle::Shutdown();

	UGraspingEditorCommands::Unregister();
}

void FURealisticGraspingEditorModule::InitializeUIButtons()
{
	UGraspingEditorCommands::Register();

	PluginCommandList = MakeShareable(new FUICommandList);
	const UGraspingEditorCommands& Commands = UGraspingEditorCommands::Get();

	PluginCommandList->MapAction(
		Commands.CreateGraspingStyle,
		FExecuteAction::CreateStatic(&UGraspingEditorCallback::ShowInstructions),
		FCanExecuteAction()
	);

	PluginCommandList->MapAction(
		Commands.LoadGraspingStyle,
		FExecuteAction::CreateStatic(&UGraspingEditorCallback::ShowInstructions),
		FCanExecuteAction()
	);

	PluginCommandList->MapAction(
		Commands.SaveGraspingPosition,
		FExecuteAction::CreateStatic(&UGraspingEditorCallback::ShowInstructions),
		FCanExecuteAction()
	);

	PluginCommandList->MapAction(
		Commands.EditGraspingPosition,
		FExecuteAction::CreateStatic(&UGraspingEditorCallback::ShowInstructions),
		FCanExecuteAction()
	);
}

void FURealisticGraspingEditorModule::CreateButton()
{
	//Load the ISkeletalMeshEditorModule and add a new button to its menu bar.
	ISkeletalMeshEditorModule& SkeletalMeshEditorModule =
		FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>("SkeletonEditor");
	// Add toolbar entry
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		PluginCommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FURealisticGraspingEditorModule::AddOptions)
	);
	
	SkeletalMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}


TSharedRef<SWidget> FURealisticGraspingEditorModule::CreateOptionMenu()
{
	//Creates all of the drop down entries.
	FMenuBuilder Builder(false, PluginCommandList.ToSharedRef());

	const UGraspingEditorCommands& Commands = UGraspingEditorCommands::Get();
	Builder.BeginSection("GraspingOptions");
	{
		Builder.AddMenuEntry(Commands.CreateGraspingStyle);
		Builder.AddMenuEntry(Commands.LoadGraspingStyle);
		Builder.AddMenuEntry(Commands.SaveGraspingPosition);
		Builder.AddMenuEntry(Commands.EditGraspingPosition);
	}
	Builder.EndSection();

	return Builder.MakeWidget();
}

void FURealisticGraspingEditorModule::AddOptions(FToolBarBuilder & Builder)
{
	UGraspingEditorCommands GraspingEditorCommands;

	//Adds the drop down menu to the button.
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FURealisticGraspingEditorModule::CreateOptionMenu),
		LOCTEXT("GraspingDebugToolbar", "Grasping Options"),
		LOCTEXT("GraspingDebugToolbar_ToolTip", "Grasping plugin options"),
		FSlateIcon(UGraspingEditorStyle::GetStyleSetName(), "GraspingEditor.DebugOptionToolBar"),
		false
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FURealisticGraspingEditorModule, URealisticGrasping)