// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingEditorModule.h"
#include "SlicingEditorStyle.h"
#include "SlicingEditorCommands.h"
#include "SlicingEditorActionCallbacks.h"
#include "SlicingLogicModule.h"

#include "LevelEditor.h"
#include "StaticMeshEditorModule.h"
#include "Engine.h"

#define LOCTEXT_NAMESPACE "FSlicingEditorModule"

// This code will execute after your module is loaded into memory; the exact timing is specified in the
// .uplugin file per-module
void FSlicingEditorModule::StartupModule()
{
	/* Initialize the appearance of the UI buttons */
	FSlicingEditorStyle::Initialize();
	FSlicingEditorStyle::ReloadTextures();
	
	InitializeUIButtons();
	CreateInstructionsButton();
	CreateDebugButtons();
}

// This function may be called during shutdown to clean up the module.
void FSlicingEditorModule::ShutdownModule()
{
	FSlicingEditorStyle::Shutdown();

	FSlicingEditorCommands::Unregister();
}

// Creates the menu-entries & buttons in the staticmesheditor
void FSlicingEditorModule::InitializeUIButtons()
{
	FSlicingEditorCommands::Register();

	PluginCommandList = MakeShareable(new FUICommandList);
	const FSlicingEditorCommands& Commands = FSlicingEditorCommands::Get();

	PluginCommandList->MapAction(
		Commands.ShowInstructions,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::ShowInstructions),
		FCanExecuteAction()
	);

	//* Needed for the debug option booleans
	FSlicingLogicModule& SlicingLogicModule =
		FModuleManager::Get().LoadModuleChecked<FSlicingLogicModule>("SlicingLogic");

	PluginCommandList->MapAction(
		Commands.EnableDebugConsoleOutput,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::OnEnableDebugConsoleOutput, &SlicingLogicModule.bEnableDebugConsoleOutput),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FSlicingEditorActionCallbacks::OnIsEnableDebugConsoleOutputEnabled, &SlicingLogicModule.bEnableDebugConsoleOutput)
	);
	PluginCommandList->MapAction(
		Commands.EnableDebugShowComponents,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::OnEnableDebugShowComponents, &SlicingLogicModule.bEnableDebugShowComponents),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FSlicingEditorActionCallbacks::OnIsEnableDebugShowComponentsEnabled, &SlicingLogicModule.bEnableDebugShowComponents)
	);
	PluginCommandList->MapAction(
		Commands.EnableDebugShowPlane,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::OnEnableDebugShowPlane, &SlicingLogicModule.bEnableDebugShowPlane),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FSlicingEditorActionCallbacks::OnIsEnableDebugShowPlaneEnabled, &SlicingLogicModule.bEnableDebugShowPlane)
	);
	PluginCommandList->MapAction(
		Commands.EnableDebugShowTrajectory,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::OnEnableDebugShowTrajectory, &SlicingLogicModule.bEnableDebugShowTrajectory),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FSlicingEditorActionCallbacks::OnIsEnableDebugShowTrajectoryEnabled, &SlicingLogicModule.bEnableDebugShowTrajectory)
	);
	PluginCommandList->MapAction(
		Commands.MakeCuttingObjects,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::MakeCuttingObjects)
	);
	PluginCommandList->MapAction(
		Commands.MakeCuttableObjects,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::MakeCuttableObjects)
	);
}

// Adds a toolbar button in the static mesh editor to instruct the user how to create the areas
// needed to make the current object be able to cut other objects
void FSlicingEditorModule::CreateInstructionsButton()
{
	IStaticMeshEditorModule& StaticMeshEditorModule =
		FModuleManager::Get().LoadModuleChecked<IStaticMeshEditorModule>("StaticMeshEditor");

	// Add toolbar entry
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Command",
		EExtensionHook::After,
		PluginCommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FSlicingEditorModule::AddInstructionsButton)
	);
	StaticMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

// Creates the buttons to enable debug options in the level editor toolbar
void FSlicingEditorModule::CreateDebugButtons()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Settings",
		EExtensionHook::After,
		PluginCommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FSlicingEditorModule::AddDebugOptions)
	);
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

// Creates the menu-entries for the dropdown-menu of the slicing toolbar menu in the level editor
TSharedRef<SWidget> FSlicingEditorModule::CreateDebugOptionMenu()
{
	FMenuBuilder Builder(false, PluginCommandList.ToSharedRef());

	const FSlicingEditorCommands& Commands = FSlicingEditorCommands::Get();
	Builder.BeginSection("SlicingDebugOptions");
	{
		Builder.AddMenuEntry(Commands.EnableDebugConsoleOutput);
		Builder.AddMenuEntry(Commands.EnableDebugShowComponents);
		Builder.AddMenuEntry(Commands.EnableDebugShowPlane);
		Builder.AddMenuEntry(Commands.EnableDebugShowTrajectory);
		Builder.AddMenuEntry(Commands.MakeCuttingObjects);
		Builder.AddMenuEntry(Commands.MakeCuttableObjects);
	}
	Builder.EndSection();

	return Builder.MakeWidget();
}

void FSlicingEditorModule::AddInstructionsButton(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(
		FSlicingEditorCommands::Get().ShowInstructions,
		NAME_None,
		LOCTEXT("SlicingInstructionsButton", "Show Slicing Instructions"),
		LOCTEXT("SlicingInstructionsButton_ToolTip", "Show Instructions for the creation of slicing objects"),
		FSlateIcon(FSlicingEditorStyle::GetStyleSetName(), "SlicingEditor.ShowInstructions")
	);
}

void FSlicingEditorModule::AddDebugOptions(FToolBarBuilder& Builder)
{
	FSlicingEditorCommands SlicingEditorCommands;
	
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FSlicingEditorModule::CreateDebugOptionMenu),
		LOCTEXT("SlicingDebugToolbar", "Slicing Options"),
		LOCTEXT("SlicingDebugToolbar_ToolTip", "Slicing plugin debug options"),
		FSlateIcon(FSlicingEditorStyle::GetStyleSetName(), "SlicingEditor.DebugOptionToolBar"),
		false
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSlicingEditorModule, Slicing)