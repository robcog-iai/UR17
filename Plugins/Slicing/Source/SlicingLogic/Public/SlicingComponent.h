// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "SlicingLogicModule.h"

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

#include "Components/BoxComponent.h"

#include "SlicingComponent.generated.h"

UCLASS()
class SLICINGLOGIC_API USlicingComponent: public UBoxComponent
{
	GENERATED_BODY()

public:
	/**** The names of the sockets that are also needed by the editor ****/
	static const FName SocketHandleName;
	static const FName SocketBladeName;
	static const FName SocketCuttingExitpointName;
	//* The tag name needed to recognize objects in the world that are cuttable by the Slicing plugin
	static const FName TagCuttable;

public:
	// Sets default values. Called when generated, even in the editor.
	USlicingComponent();

	/**** The implementation of standard component functions ****/
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//* Describes whether the cutting object is currently in the process of cutting a cuttable object
	bool bIsCurrentlyCutting = false;
	//* If pulled out, the slicing will be aborted
	bool bPulledOutCuttingObject = false;
	
	//* The component of the object, the SlicingComponent is attached to
	UStaticMeshComponent* SlicingObject;
	//* The object that is currently being cut, but did not go through the slicing process yet
	UProceduralMeshComponent* CutComponent;
	//* Needed for the debug option booleans
	FSlicingLogicModule* SlicingLogicModule;

	/**** Positional information needed for proper debug-visuals ****/
	FVector RelativeLocationToCutComponent;
	FQuat RelativeRotationToCutComponent;

	/**** Implementation of the overlap events for slicing/aborting the slicing ****/
	UFUNCTION()
	void OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBladeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/**** The visual-debugging functions ****/
	void DrawSlicingComponents();
	void DrawSlicingPlane();
	void DrawCuttingEntrancePoint();
	void DrawCuttingExitPoint();
	void DrawCuttingTrajectory();

	void SliceComponent(UPrimitiveComponent* CuttableComponent);
};