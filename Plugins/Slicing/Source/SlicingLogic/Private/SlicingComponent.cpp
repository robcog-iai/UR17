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

	// Register the overlap events
	//OnComponentBeginOverlap.AddDynamic(this, &USlicingComponent::OnBeginOverlap);
	//OnComponentEndOverlap.AddDynamic(this, &USlicingComponent::OnEndOverlap);
}

// Called when the game starts
void USlicingComponent::BeginPlay()
{
	Super::BeginPlay();

	SlicingLogicModule = &FModuleManager::Get().LoadModuleChecked<FSlicingLogicModule>("SlicingLogic");
	SlicingObject = (UStaticMeshComponent*)(this->GetAttachmentRoot());
}

//void USlicingComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	// To be overwritten if used
//}
//
//void USlicingComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	// To be overwritten if used
//}