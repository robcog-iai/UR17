// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingTipComponent.h"
#include "SlicingBladeComponent.h"

// Called when the game starts
void USlicingTipComponent::BeginPlay()
{
	Super::BeginPlay();

	// Check for the blade component to know which object is being cut
	TArray<USceneComponent*> BladeComponents;
	SlicingObject->GetChildrenComponents(true, BladeComponents);
	for (USceneComponent* Component : BladeComponents)
	{
		if (Component->GetClass()->IsChildOf(USlicingBladeComponent::StaticClass()))
		{
			// Only one blade should exist
			BladeComponent = (USlicingBladeComponent*)Component;
		}
	}

	// Register the overlap events
	OnComponentBeginOverlap.AddDynamic(this, &USlicingTipComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USlicingTipComponent::OnEndOverlap);

	bGenerateOverlapEvents = true;
	UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip gets instantiated"));
}

void USlicingTipComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip begins overlap"));
	
	if (BladeComponent->CutComponent != NULL && OtherComp == BladeComponent->CutComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip sets boolean to true"));
		
		bPulledOutCuttingObject = true;
	}
}

void USlicingTipComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip ends overlap"));
	
	if (BladeComponent->CutComponent != NULL && OtherComp == BladeComponent->CutComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip sets boolean to false"));
		
		bPulledOutCuttingObject = false;
	}
}