// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "SlicingComponent.h"

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
	// Enables the slicing itself
	bGenerateOverlapEvents = true;

	// Register the overlap events
	OnComponentBeginOverlap.AddDynamic(this, &USlicingComponent::OnBladeBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USlicingComponent::OnBladeEndOverlap);
}

// Called when the game starts
void USlicingComponent::BeginPlay()
{
	Super::BeginPlay();

	SlicingObject = (UStaticMeshComponent*)(this->GetAttachmentRoot());
	SlicingLogicModule = &FModuleManager::Get().LoadModuleChecked<FSlicingLogicModule>("SlicingLogic");
}

void USlicingComponent::OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// To be overwritten if used
}

void USlicingComponent::OnBladeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// To be overwritten if used
}