// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "SlicingComponent.h"

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

#include "Components/BoxComponent.h"

#include "SlicingBladeComponent.generated.h"

UCLASS()
class SLICINGLOGIC_API USlicingBladeComponent: public USlicingComponent
{
	GENERATED_BODY()

public:
	// Sets default values. Called when generated, even in the editor.
	//USlicingBladeComponent();

	/**** The implementation of standard component functions ****/
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//* Describes whether the cutting object is currently in the process of cutting a cuttable object
	bool bIsCurrentlyCutting = false;
	//* If pulled out, the slicing will be aborted
	bool bPulledOutCuttingObject = false;

	//* The object that is currently being cut, but did not go through the slicing process yet
	UProceduralMeshComponent* CutComponent;

	/**** Positional information needed for proper debug-visuals ****/
	FVector RelativeLocationToCutComponent;
	FQuat RelativeRotationToCutComponent;

	/**** Implementation of the overlap events for slicing/aborting the slicing ****/
	UFUNCTION()
	void OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnBladeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	/**** The visual-debugging functions ****/
	void DrawSlicingComponents();
	void DrawSlicingPlane();
	void DrawCuttingEntrancePoint();
	void DrawCuttingExitPoint();
	void DrawCuttingTrajectory();

	void SliceComponent(UPrimitiveComponent* CuttableComponent);
};