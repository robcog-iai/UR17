// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingEditorActionCallbacks.h"
#include "SlicingComponent.h"
#include "SlicingBladeComponent.h"
#include "SlicingTipComponent.h"
#include "SlicingDebugVisualComponent.h"

#include "Core.h"
#include "Editor.h"

#include "Components/BoxComponent.h"
#include "Engine/Selection.h"
#include "Engine/StaticMeshActor.h"

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
		FString("\nCutting Tip: ") + USlicingComponent::SocketTipName.ToString());

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

void FSlicingEditorActionCallbacks::OnEnableDebugShowComponents(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FSlicingEditorActionCallbacks::OnIsEnableDebugShowComponentsEnabled(bool* bButtonValue)
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

void FSlicingEditorActionCallbacks::MakeCuttingObjects()
{
	// Create the BoxComponents for every selected StaticMeshActor
	for (AStaticMeshActor* StaticMeshActor : FSlicingEditorActionCallbacks::GetSelectedStaticMeshActors())
	{
		UStaticMeshComponent* StaticMesh = StaticMeshActor->GetStaticMeshComponent();

		FSlicingEditorActionCallbacks::AddHandleComponent(StaticMesh);
		FSlicingEditorActionCallbacks::AddBladeComponent(StaticMesh);
		FSlicingEditorActionCallbacks::AddTipComponent(StaticMesh);
		FSlicingEditorActionCallbacks::AddDebugVisualComponent(StaticMeshActor);
	}
}

void FSlicingEditorActionCallbacks::MakeCuttableObjects()
{
	for (AStaticMeshActor* StaticMeshActor : FSlicingEditorActionCallbacks::GetSelectedStaticMeshActors())
	{
		UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();

		// Make the actor identifiable to the cutting object
		if (!StaticMeshComponent->ComponentTags.Contains(USlicingComponent::TagCuttable))
		{
			StaticMeshComponent->ComponentTags.Add(USlicingComponent::TagCuttable);
		}
		
		// Let the cutting object go through the actor
		StaticMeshActor->GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	}
}

TArray<AStaticMeshActor*> FSlicingEditorActionCallbacks::GetSelectedStaticMeshActors()
{
	TArray<AStaticMeshActor*> SelectedStaticMeshActors;
	GEditor->GetSelectedActors()->GetSelectedObjects<AStaticMeshActor>(SelectedStaticMeshActors);

	if (SelectedStaticMeshActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: No StaticMeshActor was selected."));
		return TArray<AStaticMeshActor*>();
	}

	return SelectedStaticMeshActors;
}

void FSlicingEditorActionCallbacks::AddBoxComponent(UStaticMeshComponent* StaticMesh, UBoxComponent* BoxComponent, FName SocketName, FName CollisionProfileName, bool bGenerateOverlapEvents)
{
	float SocketToBoxScale = 3.5;

	// Makes the added component editable in the editor and saveable in the level
	BoxComponent->GetAttachmentRootActor()->AddInstanceComponent(BoxComponent);

	BoxComponent->RegisterComponent();
	BoxComponent->AttachToComponent(StaticMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	BoxComponent->SetWorldLocation(StaticMesh->GetSocketLocation(SocketName));
	BoxComponent->SetBoxExtent(FVector(1, 1, 1) * SocketToBoxScale);
	BoxComponent->SetCollisionProfileName(CollisionProfileName);
	BoxComponent->bGenerateOverlapEvents = bGenerateOverlapEvents;
	BoxComponent->bMultiBodyOverlap = true;
}

void FSlicingEditorActionCallbacks::AddHandleComponent(UStaticMeshComponent* StaticMesh)
{
	if (!StaticMesh->DoesSocketExist(USlicingComponent::SocketHandleName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: Socket for the Handle does not exist"));
		return;
	}

	UBoxComponent* HandleComponent =
		NewObject<UBoxComponent>(StaticMesh, USlicingComponent::SocketHandleName);

	FSlicingEditorActionCallbacks::AddBoxComponent(
		StaticMesh,
		HandleComponent,
		USlicingComponent::SocketHandleName,
		FName("BlockAll"),
		false);
}

void FSlicingEditorActionCallbacks::AddBladeComponent(UStaticMeshComponent* StaticMesh)
{
	if (!StaticMesh->DoesSocketExist(USlicingComponent::SocketBladeName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: Socket for the Blade does not exist"));
		return;
	}

	USlicingBladeComponent* BladeComponent =
		NewObject<USlicingBladeComponent>(StaticMesh, USlicingComponent::SocketBladeName);

	FSlicingEditorActionCallbacks::AddBoxComponent(
		StaticMesh,
		BladeComponent,
		USlicingComponent::SocketBladeName,
		FName("OverlapAll"),
		true);
}

void FSlicingEditorActionCallbacks::AddTipComponent(UStaticMeshComponent* StaticMesh)
{
	if (!StaticMesh->DoesSocketExist(USlicingComponent::SocketTipName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: Socket for the Tip does not exist"));
		return;
	}

	USlicingTipComponent* TipComponent =
		NewObject<USlicingTipComponent>(StaticMesh, USlicingComponent::SocketTipName);

	FSlicingEditorActionCallbacks::AddBoxComponent(
		StaticMesh,
		TipComponent,
		USlicingComponent::SocketTipName,
		FName("OverlapAll"),
		true);
}

void FSlicingEditorActionCallbacks::AddDebugVisualComponent(AStaticMeshActor* StaticMesh)
{
	USlicingDebugVisualComponent* DebugComponent = NewObject<USlicingDebugVisualComponent>(StaticMesh);
	StaticMesh->AddInstanceComponent(DebugComponent);

	DebugComponent->RegisterComponent();
}

#undef LOCTEXT_NAMESPACE