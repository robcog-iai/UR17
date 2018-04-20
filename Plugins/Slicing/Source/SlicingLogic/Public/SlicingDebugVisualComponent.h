// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "Engine.h"

#include "SlicingDebugVisualComponent.generated.h"

class USlicingBladeComponent;
class USlicingTipComponent;

UCLASS()
class SLICINGLOGIC_API USlicingDebugVisualComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**** The implementation of standard component functions ****/
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//* The blade component that is attached to the same SlicingComponent
	USlicingBladeComponent* BladeComponent;
	//* The tip component that is attached to the same SlicingComponent
	USlicingTipComponent* TipComponent;

private:
	void DrawSlicingPlane();
	void DrawCuttingEntrancePoint();
	void DrawCuttingExitPoint();
	void DrawCuttingTrajectory();
};