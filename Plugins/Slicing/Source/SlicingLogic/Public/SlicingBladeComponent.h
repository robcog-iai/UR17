// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "SlicingComponent.h"

#include "SlicingBladeComponent.generated.h"

class USlicingTipComponent;
class UPhysicsConstraintComponent;

/**
* The delegates for events regarding the cutting-process
*/
DECLARE_MULTICAST_DELEGATE_FourParams(	FBeginSlicingSignature, UObject* /*CuttingAgent*/,
																UObject* /*CuttingObject*/,
																UObject* /*CutObject*/, 
																float /*Time*/);

DECLARE_MULTICAST_DELEGATE_ThreeParams(	FEndSlicingOnFailSignature,	UObject* /*CuttingAgent*/,
																	UObject* /*CutObject*/, 
																	float /*Time*/);

DECLARE_MULTICAST_DELEGATE_FourParams(FEndSlicingOnSuccessSignature,	UObject* /*CuttingAgent*/,
																		UObject* /*CutObject*/,
																		UObject* /*outputsCreated*/,
																		float /*Time*/);

DECLARE_MULTICAST_DELEGATE_TwoParams(FObjectDestruction, UObject* /*ObjectActedOn*/, float /*Time*/);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FObjectCreation,		UObject* /*TransformedObject*/,
															UObject* /*NewSlice*/,
															float /*Time*/);

UCLASS()
class SLICINGLOGIC_API USlicingBladeComponent: public USlicingComponent
{
	GENERATED_BODY()

public:
	FBeginSlicingSignature OnBeginSlicing;
	FEndSlicingOnFailSignature OnEndSlicingFail;
	FEndSlicingOnSuccessSignature OnEndSlicingSuccess;
	FObjectDestruction OnObjectDestruction;
	FObjectCreation OnObjectCreation;

public:
	// Sets default values. Called when generated, even in the editor.
	//USlicingBladeComponent();

	/**** The implementation of standard component functions ****/
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	//* Describes whether the cutting object is currently in the process of cutting a cuttable object
	bool bIsCurrentlyCutting = false;

	// The Constraints Component
	UPhysicsConstraintComponent* ConstraintOne;

	//* The tip component that is attached to the same SlicingComponent
	USlicingTipComponent* TipComponent;

	/**** Positional information needed for proper debug-visuals ****/
	FVector RelativeLocationToCutComponent;
	FQuat RelativeRotationToCutComponent;

	/**** Implementation of the overlap events for slicing/aborting the slicing ****/
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/**** The slicing-logic ****/
	void SliceComponent(UPrimitiveComponent* CuttableComponent);
	void ResetResistance();
	void ResetState();
	void SetUpConstrains(UPrimitiveComponent* CuttableComponent);
};