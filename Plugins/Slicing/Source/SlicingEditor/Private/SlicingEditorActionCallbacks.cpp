// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingEditorActionCallbacks.h"
#include "SlicingComponent.h"

#include "Core.h"
#include "Editor.h"
#include "Engine/Selection.h"

#include "Components/BoxComponent.h"

#define LOCTEXT_NAMESPACE "FSlicingEditorModule"

void FSlicingEditorActionCallbacks::ShowInstructions()
{
	const FText InstructionsTitle =
		LOCTEXT("SlicingInstructions_Title", "Instructions to edit an object to be able to slice");
	const FText InstructionsText = FText::FromString(
		FString("To alter an object to be able to slice other objects, ") +
		FString("the user needs to create multiple Sockets on the position - and with the size of - ") +
		FString("certain areas needed for cutting objects. These sockets need specific names to specify ") +
		FString("the area they are used for.\n\nCutting Blade: ") + USlicingComponent::SocketBladeName.ToString() +
		FString("\nCutting Handle: ") + USlicingComponent::SocketHandleName.ToString() +
		FString("\nCutting Exitpoint: ") + USlicingComponent::SocketCuttingExitpointName.ToString());

	// Display the popup-window
	FMessageDialog* Instructions = new FMessageDialog();
	Instructions->Debugf(InstructionsText, &InstructionsTitle);
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

			float BoxScale = 3.5;

			UBoxComponent* HandleBox = NewObject<UBoxComponent>(Mesh, USlicingComponent::SocketHandleName);
			HandleBox->RegisterComponent();
			HandleBox->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, USlicingComponent::SocketHandleName);
			HandleBox->SetWorldLocation(Mesh->GetSocketLocation(USlicingComponent::SocketHandleName));
			FVector TempScale = Mesh->GetSocketTransform(USlicingComponent::SocketHandleName).GetScale3D();
			HandleBox->SetBoxExtent(FVector(BoxScale, BoxScale, BoxScale));
			HandleBox->SetCollisionProfileName(FName("BlockAll"));
			HandleBox->bGenerateOverlapEvents = false;
			
			USlicingComponent* BladeBox = NewObject<USlicingComponent>(Mesh, USlicingComponent::SocketBladeName);
			BladeBox->RegisterComponent();
			BladeBox->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, USlicingComponent::SocketBladeName);
			BladeBox->SetWorldLocation(Mesh->GetSocketLocation(USlicingComponent::SocketBladeName));
			TempScale = Mesh->GetSocketTransform(USlicingComponent::SocketBladeName).GetScale3D();
			BladeBox->SetBoxExtent(FVector(BoxScale, BoxScale, BoxScale));
			BladeBox->bGenerateOverlapEvents = true;
			BladeBox->SetCollisionProfileName(FName("OverlapAll"));
			BladeBox->bMultiBodyOverlap = false;

			UBoxComponent* CuttingExitpointBox = NewObject<UBoxComponent>(Mesh, USlicingComponent::SocketCuttingExitpointName);
			CuttingExitpointBox->RegisterComponent();
			CuttingExitpointBox->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, USlicingComponent::SocketCuttingExitpointName);
			CuttingExitpointBox->SetWorldLocation(Mesh->GetSocketLocation(USlicingComponent::SocketCuttingExitpointName));
			TempScale = Mesh->GetSocketTransform(USlicingComponent::SocketCuttingExitpointName).GetScale3D();
			CuttingExitpointBox->SetBoxExtent(FVector(BoxScale, BoxScale, BoxScale));
			CuttingExitpointBox->SetCollisionProfileName(FName("OverlapAll"));
			CuttingExitpointBox->bGenerateOverlapEvents = false;
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