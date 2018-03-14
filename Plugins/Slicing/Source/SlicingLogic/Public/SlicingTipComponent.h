// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "SlicingComponent.h"

#include "SlicingTipComponent.generated.h"

UCLASS()
class SLICINGLOGIC_API USlicingTipComponent: public USlicingComponent
{
	GENERATED_BODY()

public:
	/**** Implementation of the overlap events to detect early abortion of the slicing ****/
	UFUNCTION()
	void OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnBladeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

};