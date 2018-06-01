// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingComponent.h"

#include "DrawDebugHelpers.h"

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

	SlicingLogicModule = &FModuleManager::Get().LoadModuleChecked<FSlicingLogicModule>("SlicingLogic");
	SlicingObject = (UStaticMeshComponent*)(this->GetAttachmentRoot());
}

// Called every frame
void USlicingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SlicingLogicModule->bEnableDebugShowComponents)
	{
		USlicingComponent::DrawComponent();
	}
}

// Draws the SlicingComponent box
void USlicingComponent::DrawComponent()
{
	DrawDebugBox(GetWorld(), GetComponentLocation(), GetScaledBoxExtent(), GetComponentRotation().Quaternion(),
		FColor::Green, false, 0.01f);
}