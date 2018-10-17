// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "AGraspingHand.h"
#include "UAnimationDataStructure.h"
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
	Loads the data of grasps into the grasping controller
	@param Data - data to load
	*/
	UFUNCTION()
	void SetGraspingData(FHandAnimationData Data);

	/*
	Responsible for updating the movement of all bones
	@param Hand - the AGraspingHand to move
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	*/
	void UpdateGrasp(AGraspingHand* Hand, const float Input);

	// Used to activate TighteningGrasp:
	// While active, after grasping for a certain amount of time the fingers that are not at their desired position get additional force added to them,
	// by icnreasing the target rotation of their bones
	UPROPERTY(EditAnywhere)
	bool bEnableTightening = false;

	// Percentage by which to increase the goal rotation of the fingers by
	UPROPERTY(EditAnywhere)
	float IncreasePercentage = 40;

	// Time to grasp until the target position is altered
	// Set to 0.1 to increase target position as fast as possible
	// Don't set to 0
	UPROPERTY(EditAnywhere)
	float TimeUntilIncrease = 1;

	// This number specifies by how many degree a bones rotation can be of the expected value, before it's target rotation gets increased
	// Set to 0 to always increase target rotation on all bones
	UPROPERTY(EditAnywhere)
	float BoneOrientationTolerance = 0;

private:

	/*
	Calls LerpFingerOrientation on every finger
	@param Target - where calculated positions are saved
	@param Initial - initial hand state if input was 0
	@param Closed - final hand state if input was 1 
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	*/
	void LerpHandOrientation(FHandEpisodeData* Target, FHandEpisodeData Initial, FHandEpisodeData Closed, const float Input);

	/*
	Gets the initial and final position of a gasping process.
	Depending on how much the grasping button is pushed a position inbetween these two is calculated. 
	@param Type - type of finger to calculate for
	@param Target - where calculated positions are saved
	@param Initial - initial hand state if input was 0
	@param Closed - final hand state if input was 1
	@param Input - number from 0 - 1 that indicates how far the grasping trigger is being pushed down
	*/
	void LerpFingerOrientation(ETypeOfFinger Type,FHandEpisodeData* Target, FHandEpisodeData Initial, FHandEpisodeData Closed, const float Input);

	/*
	Calls DriveToFingerOrientationTarget for every finger.
	@param Target - the target position that has been calculated by lerp
	@param Hand - the AGraspingHand to move
	*/
	void DriveToHandOrientationTarget(FHandEpisodeData* Target, AGraspingHand* Hand);

	/*
	Tell all the constraints to move to the target position
	@param Type - type of finger to move
	@param Target - the target position that has been calculated by lerp
	@param Hand - the AGraspingHand to move
	*/
	void DriveToFingerOrientationTarget(ETypeOfFinger Type, FHandEpisodeData* Target, AGraspingHand* Hand);

	/*
	Finds out which constraint belongs to which bone and sets them up  so we cann move them
	@param BoneName - name of bone
	@param Hand - AGraspingHand with the skeleton that has to be searched for the constraint
	*/
	FConstraintInstance* BoneNameToConstraint(FString BoneName, AGraspingHand* Hand);

	/*
	Stops the grasping process and resets the booleans that were changed
	@param Hand - AGraspingHand that should stop grasping
	*/
	void StopGrasping(AGraspingHand* Hand);

	// If hand is grasping
	bool bIsGrasping;

	// Current grasp loaded into hand
	FHandAnimationData GraspingData;

	// If bone constraints are set up the right way
	bool bBonesConstraintsSetUp;

	// Spring value to be used for constraints
	// Makes angular motors more powerfull depending on how much the grasping button is pushed down
	float Spring;

	/*
	Responsible for updating the movement of all bones, when TighteningGrasp is active
	@param Hand - the AGraspingHand to move
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	*/
	void UpdateGraspTighten(AGraspingHand* Hand, const float Input);

	// Set to true when timer for TighteningGrasp was set
	bool bTighteningInit;

	// Set to true when hand is currently being tightened
	bool bCurrentlyTightening;

	// Saves the hand that has to be tightened, since the timer can't give a parameter to the function 
	AGraspingHand* HandToTighten;

	// Finds the fingers, that aren't at their desired position and tightens them 
	void TightenGrip();

	/*
	Used to check if the finger is at the desired position
	@param Type - finger type to check
	*/
	bool CalculateIfBlocked(ETypeOfFinger Type);

	// Saves the the target rotations for the tightened grasp
	FHandEpisodeData* TightenTarget;

	// Calculates the rotations for the tightened grasp
	void CreateNewTightenTarget();

	// When changing grasp type while grasping, the new grasp isn't applied immediately
	// Instead it is saved in this variable and applied once the user stops grasping 
	FHandAnimationData GraspQueue;

	// Set to true when there is a grasp waiting to be applied
	bool bIsInQueue;
};
