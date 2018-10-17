// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingComponent.h"

#include "DrawDebugHelpers.h"

#include "Components/StaticMeshComponent.h"

// Setting the text for the static names used in the editor
const FName USlicingComponent::SocketHandleName = "SlicingHandle";
const FName USlicingComponent::SocketBladeName = "SlicingBlade";
const FName USlicingComponent::SocketTipName = "SlicingTip";
const FName USlicingComponent::TagCuttable = "Cuttable";

USlicingComponent::USlicingComponent()
{
	// Enables the usage of the TickComponent function
	PrimaryComponentTick.bCanEverTick = true;
	// Enables the usage of the InitializeComponent function
	bWantsInitializeComponent = true;
}

// Called when the game starts
void USlicingComponent::BeginPlay()
{
	Super::BeginPlay();

	SlicingObject = (UStaticMeshComponent*)(this->GetAttachmentRoot());
}