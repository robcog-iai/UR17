// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingEditorModule.h"
#include "SlicingEditorStyle.h"
#include "SlicingEditorCommands.h"
#include "SlicingEditorActionCallbacks.h"
#include "SlicingLogicModule.h"

#include "LevelEditor.h"
#include "Editor.h"
#include "IStaticMeshEditor.h"
#include "StaticMeshEditorActions.h"
#include "StaticMeshEditorModule.h"

#include "Engine.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "FSlicingEditorModule"

//* This code will execute after your module is loaded into memory; the exact timing is specified in the
// .uplugin file per-module
void FSlicingEditorModule::StartupModule()
{
	/** Initialize the appearance of the UI buttons */
	FSlicingEditorStyle::Initialize();
	FSlicingEditorStyle::ReloadTextures();
	
	InitializeUIButtons();
	AddUIButtons();
	CreateDebugButtons();

	FAssetEditorManager::Get().OnAssetOpenedInEditor().AddRaw(this, &FSlicingEditorModule::HandleAsset);
}

void FSlicingEditorModule::HandleAsset(UObject * Asset, IAssetEditorInstance *Editor)
{
	Mesh = Asset;
	Editore = Editor;
}

//* This function may be called during shutdown to clean up the module.
void FSlicingEditorModule::ShutdownModule()
{
	FSlicingEditorStyle::Shutdown();

	FSlicingEditorCommands::Unregister();
}

//* Creates the menu-entries & buttons in the staticmesheditor
void FSlicingEditorModule::InitializeUIButtons()
{
	FSlicingEditorCommands::Register();

	PluginCommandList = MakeShareable(new FUICommandList);
	const FSlicingEditorCommands& Commands = FSlicingEditorCommands::Get();

	PluginCommandList->MapAction(
		Commands.CreateHandle,
		FExecuteAction::CreateRaw(this, &FSlicingEditorModule::CreateHandle)
	);
	PluginCommandList->MapAction(
		Commands.CreateBlade,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::CreateBlade)
	);
	PluginCommandList->MapAction(
		Commands.CreateCuttingExitpoint,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::CreateCuttingExitpoint)
	);

	PluginCommandList->MapAction(
		Commands.ShowSlicingElements,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::ShowSlicingElements),
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
		Commands.ReplaceSockets,
		FExecuteAction::CreateStatic(&FSlicingEditorActionCallbacks::ReplaceSocketsWithUseableComponents)
	);
}

//* Adds the neccessary menu-entries & buttons to the staticmesheditor to configure slicable objects
void FSlicingEditorModule::AddUIButtons()
{
	IStaticMeshEditorModule& StaticMeshEditorModule =
		FModuleManager::Get().LoadModuleChecked<IStaticMeshEditorModule>("StaticMeshEditor");

	// Add menubar entry
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuBarExtension(
		"Collision",
		EExtensionHook::After,
		PluginCommandList,
		FMenuBarExtensionDelegate::CreateRaw(this, &FSlicingEditorModule::AddSlicingMenuBar)
	);
	StaticMeshEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

	// Add toolbar entry
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Command",
		EExtensionHook::After,
		PluginCommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FSlicingEditorModule::AddSlicingToolbar)
	);
	StaticMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

//* Creates the buttons to enable debug options in the level editor toolbar
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

void FSlicingEditorModule::CreateHandle()
{
	UE_LOG(LogTemp, Warning, TEXT("CREATED HANDLE"));
	UBoxComponent* Handle = NewObject<UBoxComponent>();
	Handle->RegisterComponent();

	if (!Mesh)
		return;

	check(Editore->GetEditorName() == "StaticMeshEditor");
	auto * Tempy = StaticCast<IStaticMeshEditor *>(Editore);
	{
		UStaticMesh* WorkingStaticMesh = Tempy->GetStaticMesh();

		UStaticMeshSocket* Sock = NewObject<UStaticMeshSocket>();
		Sock->SocketName = FName("Handle - Test");
		WorkingStaticMesh->Sockets.Emplace(Sock);
		Tempy->SetSelectedSocket(Sock);

	}

	//RefreshViewport();
}

//* Refreshes the viewport so that newly added objects are shown immediately
void FSlicingEditorModule::RefreshViewport()
{
	StaticMeshEditorViewport = MakeShareable((FViewport*)GEditor->GetActiveViewport());

	StaticMeshEditorViewport->Invalidate();
}

//* Creates the menu-entries for the dropdown-menu of the slicing menubar
void FSlicingEditorModule::CreateSlicingMenu(FMenuBuilder& Builder)
{
	const FSlicingEditorCommands& Commands = FSlicingEditorCommands::Get();
	
	Builder.BeginSection("SlicingAddSlicingElements");
	{
		Builder.AddMenuEntry(Commands.CreateHandle);
		Builder.AddMenuEntry(Commands.CreateBlade);
		Builder.AddMenuEntry(Commands.CreateCuttingExitpoint);
	}
	Builder.EndSection();
}

//* Creates the menu-entries for the dropdown-menu of the slicing toolbar menu
TSharedRef<SWidget> FSlicingEditorModule::CreateDebugOptionMenu()
{
	FMenuBuilder Builder(false, PluginCommandList.ToSharedRef());

	const FSlicingEditorCommands& Commands = FSlicingEditorCommands::Get();
	Builder.BeginSection("SlicingDebugOptions");
	{
		Builder.AddMenuEntry(Commands.EnableDebugConsoleOutput);
		Builder.AddMenuEntry(Commands.EnableDebugShowPlane);
		Builder.AddMenuEntry(Commands.EnableDebugShowTrajectory);
		Builder.AddMenuEntry(Commands.ReplaceSockets);
	}
	Builder.EndSection();

	return Builder.MakeWidget();
}

void FSlicingEditorModule::AddSlicingMenuBar(FMenuBarBuilder& Builder)
{
	Builder.AddPullDownMenu(
		LOCTEXT("SlicingPluginMenu", "Slicing"),
		LOCTEXT("SlicingPluginMenu_ToolTip", "Opens a menu with commands for creating the elements needed to make the static mesh be able to slice."),
		FNewMenuDelegate::CreateRaw(this, &FSlicingEditorModule::CreateSlicingMenu),
		"Slicing"
	);
}

void FSlicingEditorModule::AddSlicingToolbar(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FSlicingEditorCommands::Get().ShowSlicingElements);
}

void FSlicingEditorModule::AddDebugOptions(FToolBarBuilder& Builder)
{
	FSlicingEditorCommands SlicingEditorCommands;
	
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FSlicingEditorModule::CreateDebugOptionMenu),
		LOCTEXT("SlicingDebugToolbar", "Slicing"),
		LOCTEXT("SlicingDebugToolbar_ToolTip", "Slicing plugin debug options"),
		FSlateIcon(FSlicingEditorStyle::GetStyleSetName(), "SlicingEditor.DebugOptionToolBar"),
		false
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSlicingEditorModule, Slicing)