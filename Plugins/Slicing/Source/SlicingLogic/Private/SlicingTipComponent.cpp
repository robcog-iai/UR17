// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "SlicingTipComponent.h"

void USlicingTipComponent::OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// To be overwritten if used
}

void USlicingTipComponent::OnBladeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// To be overwritten if used
}