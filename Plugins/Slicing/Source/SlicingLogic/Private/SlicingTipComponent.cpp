// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingTipComponent.h"
#include "SlicingBladeComponent.h"

#include "DrawDebugHelpers.h"

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
}

// Called every frame
void USlicingTipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (SlicingLogicModule->bEnableDebugShowComponents)
	{
		USlicingTipComponent::DrawComponent();
	}
}


void USlicingTipComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip begins overlap"));
	CutComponent = OtherComp;

	if (BladeComponent->CutComponent != NULL && OtherComp == BladeComponent->CutComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip enters the same object as the blade is inside of"));
		
		BladeComponent->bIsCurrentlyCutting = false;
	}
}

void USlicingTipComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip ends overlap"));
	CutComponent = NULL;
	
	if (BladeComponent->CutComponent != NULL && OtherComp == BladeComponent->CutComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip exits the same object as the blade is inside of"));
	}
}

// Draws the TipComponent box
void USlicingTipComponent::DrawComponent()
{
	DrawDebugBox(GetWorld(), GetComponentLocation(), GetScaledBoxExtent(), GetComponentRotation().Quaternion(),
		FColor::Green, false, 0.01f);
}