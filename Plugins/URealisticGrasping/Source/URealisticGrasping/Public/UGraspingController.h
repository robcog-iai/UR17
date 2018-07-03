// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AGraspingHand.h"
#include "UFinger.h"
#include "UGraspingController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UREALISTICGRASPING_API UGraspingController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGraspingController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	Responsible for updating the movement of all bones
	@param Hand - the AGraspingHand to move
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	@param bIsRightHand - if hand to change is right or left hand
	*/
	void UpdateGrasp(AGraspingHand* Hand, const float Input, bool bIsRightHand);

	/*
	Calls LerpFingerOrientation on every finger
	@param Target - where calculated positions are saved
	@param Initial - initial hand state if input was 0
	@param Closed - final hand state if input was 1 
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	*/
	void LerpHandOrientation(FFingersData* Target, FFingersData Initial, FFingersData Closed, const float Input);

	/*
	Gets the initial and final position of a gasping process.
	Depending on how much the grasping button is pushed a position inbetween these two is calculated. 
	@param Type - type of finger to calculate for
	@param Target - where calculated positions are saved
	@param Initial - initial hand state if input was 0
	@param Closed - final hand state if input was 1
	@param Input - number from 0 - 1 that indicates how far the grasping trigger is being pushed down
	*/
	void LerpFingerOrientation(ETypeOfFinger Type,FFingersData* Target, FFingersData Initial, FFingersData Closed, const float Input);

	/*
	Calls DriveToFingerOrientationTarget for every finger.
	@param Target - the target position that has been calculated by lerp
	@param Hand - the AGraspingHand to move
	*/
	void DriveToHandOrientationTarget(FFingersData* Target, AGraspingHand* Hand);

	/*
	Tell all the constraints to move to the target position
	@param Type - type of finger to move
	@param Target - the target position that has been calculated by lerp
	@param Hand - the AGraspingHand to move
	*/
	void DriveToFingerOrientationTarget(ETypeOfFinger Type, FFingersData* Target, AGraspingHand* Hand);

	/*
	Finds out which constraint belongs to which bone and sets them up  so we cann move them
	@param BoneName - name of bone
	@param Hand - AGraspingHand with the skeleton taht has to be searched for the constraint
	*/
	FConstraintInstance* BoneNameToConstraint(FString BoneName, AGraspingHand* Hand);

	/*
	Loads the data of grasps into the grasping controller
	@param Data - data to load
	@param bIsRightHand - if data is meant for right hand or left hand
	*/
	UFUNCTION()
	void SetGraspingData(FHandAnimationData Data, bool bIsRightHand);

	//if right hand is grasping
	bool bIsGraspingR = false;

	//if left hand is grasping
	bool bIsGraspingL = false;

	//current grasp loaded into right hand
	FHandAnimationData GraspingDataR;

	//current grasp loaded into left hand
	FHandAnimationData GraspingDataL;

	//if bone constraints are set up the right way
	bool BonesConstraintsSetUp = false;

	//spring value to be used for constraints
	//makes angular motors more powerfull depending on how much the grasping button is pushed down
	float Spring;
	
};
