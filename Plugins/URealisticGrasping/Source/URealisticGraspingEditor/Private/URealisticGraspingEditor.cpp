// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "URealisticGraspingEditor.h"

#include "UGraspingEditorStyle.h"
#include "UGraspingEditorCommands.h"
#include "UGraspingEditorCallback.h"
#include "PersonaModule.h"
#include "IPersonaPreviewScene.h"
#include "IPersonaToolkit.h"
#include "Engine.h"
#include "LevelEditor.h"
#include "ISkeletalMeshEditor.h"
#include "ISkeletalMeshEditorModule.h"
#include "ISkeletonEditor.h"
#include "Editor/UnrealEd/Classes/Animation/DebugSkelMeshComponent.h"
#include "ISkeletonEditorModule.h"
#define WIN32_LEAN_AND_MEAN
#include "Editor/AnimGraph/Classes/AnimPreviewInstance.h"

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

	//Loads the Persona module and binds an action to its OnPreviewSceneCreatedDelegate
	FPersonaModule& PersonaModule =
		FModuleManager::Get().LoadModuleChecked<FPersonaModule>("Persona");

	OnPreviewSceneCreatedDelegate = PersonaModule.OnPreviewSceneCreated().AddLambda([this](const TSharedRef<IPersonaPreviewScene>& param) { this->OnPreviewCreation(param); });
}

void FURealisticGraspingEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UGraspingEditorCommands::Unregister();
}

void FURealisticGraspingEditorModule::InitializeUIButtons()
{
	UGraspingEditorCommands::Register();
	EditorCallback = UGraspingEditorCallback();
	EditorCallback.ReadFingerTypes();
	PluginCommandListCreateSection = MakeShareable(new FUICommandList);
	PluginCommandListEditSection = MakeShareable(new FUICommandList);
	const UGraspingEditorCommands& Commands = UGraspingEditorCommands::Get();

	//Commands for the "Create grasp" button
	PluginCommandListCreateSection->MapAction(
		Commands.CreateGraspingStyle,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::ShowSaveGraspingStyleWindow),
		FCanExecuteAction()
	);

	PluginCommandListCreateSection->MapAction(
		Commands.SaveGraspingPosition,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::SaveBoneDatasAsEpisode),
		FCanExecuteAction()
	);

	PluginCommandListCreateSection->MapAction(
		Commands.DiscardNewGraspingStyle,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::DiscardAllEpisodes),
		FCanExecuteAction()
	);

	PluginCommandListCreateSection->MapAction(
		Commands.ShowCreateHelp,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::ShowCreateHelp),
		FCanExecuteAction()
	);

	//Commands for the "Edit grasp" button
	PluginCommandListEditSection->MapAction(
		Commands.LoadGraspingStyle,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::EditLoadedGraspingStyle),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.EditGraspingPosition,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::ShowEpisodeEditWindow),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.ShowNextEpisode,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::ShowNextEpisode),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.ShowPreviousEpisode,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::ShowPreviousEpisode),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.ShowEditHelp,
		FExecuteAction::CreateRaw(this, &FURealisticGraspingEditorModule::ShowEditHelp),
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
	//Creates the "Create grasp" button
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		PluginCommandListCreateSection,
		FToolBarExtensionDelegate::CreateRaw(this, &FURealisticGraspingEditorModule::AddCreateOptions)
	);
	//Creates the "Edit grasp" button
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		PluginCommandListEditSection,
		FToolBarExtensionDelegate::CreateRaw(this, &FURealisticGraspingEditorModule::AddEditOptions)
	);
	
	SkeletalMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}


TSharedRef<SWidget> FURealisticGraspingEditorModule::CreateOptionMenu()
{
	//Creates all of the drop down entries of the "Create grasp" button.
	FMenuBuilder CreateBuilder(false, PluginCommandListCreateSection.ToSharedRef());
	
	const UGraspingEditorCommands& Commands = UGraspingEditorCommands::Get();
	CreateBuilder.BeginSection("Create grasp");
	{
		CreateBuilder.AddMenuEntry(Commands.CreateGraspingStyle);
		CreateBuilder.AddMenuEntry(Commands.SaveGraspingPosition);
		CreateBuilder.AddMenuEntry(Commands.DiscardNewGraspingStyle);
		CreateBuilder.AddMenuEntry(Commands.ShowCreateHelp);
	}
	CreateBuilder.EndSection();

	return CreateBuilder.MakeWidget();
}

TSharedRef<SWidget> FURealisticGraspingEditorModule::EditOptionMenu()
{
	//Creates all of the drop down entries of the "Edit grasp" button.
	FMenuBuilder EditBuilder(false, PluginCommandListEditSection.ToSharedRef());
	const UGraspingEditorCommands& Commands = UGraspingEditorCommands::Get();
	EditBuilder.BeginSection("Edit grasps");
	{
		EditBuilder.AddMenuEntry(Commands.LoadGraspingStyle);
		EditBuilder.AddMenuEntry(Commands.EditGraspingPosition);
		EditBuilder.AddMenuEntry(Commands.ShowNextEpisode);
		EditBuilder.AddMenuEntry(Commands.ShowPreviousEpisode);
		EditBuilder.AddMenuEntry(Commands.ShowEditHelp);
	}
	EditBuilder.EndSection();

	return EditBuilder.MakeWidget();
}

void FURealisticGraspingEditorModule::AddCreateOptions(FToolBarBuilder & Builder)
{
	UGraspingEditorCommands GraspingEditorCommands;

	//Adds the drop down menu to the button.
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FURealisticGraspingEditorModule::CreateOptionMenu),
		LOCTEXT("Create", "Create grasp"),
		LOCTEXT("Create_Tooltip", "Options to create a grasping style"),
		FSlateIcon(UGraspingEditorStyle::GetStyleSetName(), "GraspingEditor.DebugOptionToolBar"),
		false
	);
}

void FURealisticGraspingEditorModule::AddEditOptions(FToolBarBuilder & Builder)
{
	UGraspingEditorCommands GraspingEditorCommands;

	//Adds the drop down menu to the button.
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FURealisticGraspingEditorModule::EditOptionMenu),
		LOCTEXT("Edit", "Edit grasp"),
		LOCTEXT("Edit_Tooltip", "Options to edit an existing grasping style"),
		FSlateIcon(UGraspingEditorStyle::GetStyleSetName(), "GraspingEditor.DebugOptionToolBar"),
		false
	);
}


void FURealisticGraspingEditorModule::OnPreviewCreation(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	//When a preview scene is created get its DebugMeshComponent and also reset the EditorCallBack
	TSharedRef<IPersonaToolkit> PersonaToolKitRef = InPreviewScene.Get().GetPersonaToolkit();
	DebugMeshComponent = PersonaToolKitRef.Get().GetPreviewMeshComponent();
	EditorCallback.Reset();
}

void FURealisticGraspingEditorModule::ShowEpisodeEditWindow()
{
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TMap<FString, FTransform> StartingBoneRotations;
	//Get all of the current bone rotations and save them
	for (FName BoneName : BoneNames) 
	{
		int Index = DebugMeshComponent->GetBoneIndex(BoneName);
		FTransform BoneTransform = DebugMeshComponent->GetBoneTransform(Index);
		StartingBoneRotations.Add(BoneName.ToString(), BoneTransform);
	}
	EditorCallback.SetStartingBoneRotations(StartingBoneRotations);
	EditorCallback.ShowEpisodeEditWindow();
}

void FURealisticGraspingEditorModule::WriteEpisodesToFile()
{
	//Writes all currently recorded episodes to a .ini file
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.WriteEpisodesToFile();
}

void FURealisticGraspingEditorModule::ShowSaveGraspingStyleWindow()
{
	//Shows a window where you can enter a new for a newly created grasping stlye
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.ShowSaveGraspingStyleWindow();

}

void FURealisticGraspingEditorModule::SaveBoneDatasAsEpisode()
{
	//Saves the current mesh position as an episode
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.SaveBoneDatasAsEpisode();
}

void FURealisticGraspingEditorModule::EditLoadedGraspingStyle()
{
	//Overwrites the loaded step for a grasping stlye with the currently displayed hand position
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.EditLoadedGraspingStyle();
}

void FURealisticGraspingEditorModule::ShowCreateHelp()
{
	//Shows help for the "Create grasp" button
	FString Message = "Create new grasping style out of saved episodes :\nCreates a new grasping style with the saved episodes. \nYou need to have at least 2 episodes to create a new grasp. \nThe new grasp will be saved in the GraspAnimations folder of your project.\n\nSave current hand position as episode :\nSaves the hand position currently displayed as an episode. \n\nDiscard all saved episodes :\nDiscards all your previously saved episodes.";
	EditorCallback.ShowInstructions(Message);
}

void FURealisticGraspingEditorModule::ShowEditHelp()
{
	//Shows help for the "Edit grasp" button
	FString Message = "Overwrite loaded Episode :\nAfter you load in a grasp this button will replace the Episode currently loaded with the hand position currently displayed.\n\nLoad grasping style :\nLoads a grasping at a specific Episode. The name of the grasp has to exist in the GraspAnimations folder and the grasp also has to have the specific episode.\n\nShow next episode :\nShows the next episode depending on the currently displayed episode. This action will change the episode to overwrite you set in Load grasping style.\n\nShow previous episode :\nShows the previous episode depending on the currently displayed episode. This action will change the episode to overwrite you set in Load grasping style.\n\n";
	EditorCallback.ShowInstructions(Message);
}

void FURealisticGraspingEditorModule::DiscardAllEpisodes()
{
	//Discards all currently recorded episodes
	EditorCallback.DiscardAllEpisodes();
}

void FURealisticGraspingEditorModule::ShowNextEpisode()
{
	//Shows the next episode depending on the one currently displayed
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.ShowEpisode(true);
}

void FURealisticGraspingEditorModule::ShowPreviousEpisode()
{
	//Shows the previous episode depending on the one currently displayed
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.ShowEpisode(false);
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FURealisticGraspingEditorModule, URealisticGrasping)