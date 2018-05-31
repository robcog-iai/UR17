// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "SlicingLogicModule.h"

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "SlicingDebugVisualComponent.generated.h"

class AStaticMeshActor;
class USlicingBladeComponent;
class USlicingTipComponent;
class USlicingComponent;

UCLASS()
class SLICINGLOGIC_API USlicingDebugVisualComponent: public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values. Called when generated, even in the editor.
	USlicingDebugVisualComponent();

	/**** The implementation of standard component functions ****/
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//* Needed for the debug option booleans
	FSlicingLogicModule* SlicingLogicModule;

	//* The static mesh actor this component is attached to
	AStaticMeshActor* SlicingObject;
	//* The blade component that is attached to the same SlicingObject
	USlicingBladeComponent* BladeComponent;
	//* The tip component that is attached to the same SlicingObject
	USlicingTipComponent* TipComponent;

private:
	void DrawSlicingPlane();
	void DrawCuttingEntrancePoint();
	void DrawCuttingExitPoint();
	void DrawCuttingTrajectory();
	void DrawComponents();
	void DrawComponent(USlicingComponent* SlicingComponent);
};