// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingEditorActionCallbacks.h"

#include "IStaticMeshEditor.h"
#include "StaticMeshEditorActions.h"
#include "StaticMeshEditorModule.h"
#include "Editor.h"
#include "Components/BoxComponent.h"
#include "Engine/Selection.h"
#include "SlicingEditorLogicBox.h"

#define LOCTEXT_NAMESPACE "FSlicingEditorModule"

void FSlicingEditorActionCallbacks::ShowInstructions()
{
	UE_LOG(LogTemp, Warning, TEXT("TOGGLED SLICING ELEMENTS SHOWN"));
}

void FSlicingEditorActionCallbacks::OnEnableDebugConsoleOutput(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FSlicingEditorActionCallbacks::OnIsEnableDebugConsoleOutputEnabled(bool* bButtonValue)
{
	return *bButtonValue;
}

void FSlicingEditorActionCallbacks::OnEnableDebugShowPlane(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FSlicingEditorActionCallbacks::OnIsEnableDebugShowPlaneEnabled(bool* bButtonValue)
{
	return *bButtonValue;
}

void FSlicingEditorActionCallbacks::OnEnableDebugShowTrajectory(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FSlicingEditorActionCallbacks::OnIsEnableDebugShowTrajectoryEnabled(bool* bButtonValue)
{
	return *bButtonValue;
}

void FSlicingEditorActionCallbacks::ReplaceSocketsWithComponents()
{
	USelection* Selection = GEditor->GetSelectedComponents();
	UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Selection->GetSelectedObject(0));
	if (Mesh != NULL && Mesh != nullptr)
	{
		if (Mesh->ComponentHasTag(FName("Knife")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Has Sockets = Success"));

			UBoxComponent* HandleBox = NewObject<UBoxComponent>(Mesh,FName("Handle"));
			HandleBox->RegisterComponent();
			HandleBox->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Handle"));
			HandleBox->SetWorldLocation(Mesh->GetSocketLocation("Handle"));
			HandleBox->SetBoxExtent(Mesh->GetSocketTransform(FName("Handle")).GetScale3D());

			SlicingEditorLogicBox* BladeBox = NewObject<SlicingEditorLogicBox>(Mesh, FName("BladeBox"));
			BladeBox->RegisterComponent();
			BladeBox->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BladeBox"));
			BladeBox->SetWorldLocation(Mesh->GetSocketLocation("BladeBox"));
			BladeBox->SetBoxExtent(Mesh->GetSocketTransform(FName("BladeBox")).GetScale3D());

			UBoxComponent* CuttingExitpointBox = NewObject<UBoxComponent>();
			CuttingExitpointBox->RegisterComponent();
			CuttingExitpointBox->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("CuttingExitpointBox"));
			CuttingExitpointBox->SetWorldLocation(Mesh->GetSocketLocation("CuttingExitpointBox"));
			CuttingExitpointBox->SetBoxExtent(Mesh->GetSocketTransform(FName("CuttingExitpointBox")).GetScale3D());
		} 
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Has Sockets = FAILURE"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Error in Box Creation"));
	}
}

void FSlicingEditorActionCallbacks::ReplaceSocketsOfAllStaticMeshComponents()
{
	// To be implemented in the future
}

#undef LOCTEXT_NAMESPACE