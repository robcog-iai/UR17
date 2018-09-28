// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingTipComponent.h"
#include "SlicingBladeComponent.h"
#include "SlicingHelper.h"

// Called when the game starts
void USlicingTipComponent::BeginPlay()
{
	Super::BeginPlay();

	// Check for the blade component to know which object is being cut
	BladeComponent = FSlicingHelper::GetSlicingComponent<USlicingBladeComponent>(SlicingObject);

	// Register the overlap events
	OnComponentBeginOverlap.AddDynamic(this, &USlicingTipComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USlicingTipComponent::OnEndOverlap);

	SetGenerateOverlapEvents(true);
}

void USlicingTipComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappedComponent = OtherComp;

	if (BladeComponent->CutComponent != NULL && OtherComp == BladeComponent->CutComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip enters the same object as the blade is inside of"));
		
		CutComponent = OtherComp;
		bEnteredCurrentlyCutObject = true;
	}
}

void USlicingTipComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedComponent = NULL;
	
	if (OtherComp == CutComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip exits the same object as the blade is inside of"));

		CutComponent = NULL;
		bEnteredCurrentlyCutObject = false;
	}
}