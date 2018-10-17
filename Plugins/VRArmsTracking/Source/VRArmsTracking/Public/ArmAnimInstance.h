// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "ArmAnimPawn.h"
#include "VRArmsTracking.h"
#include "ArmAnimInstance.generated.h"

//Declare a delegate for an collision event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionDelegate, FHitResult, CollisionObject);
/**
 * This Class calculate the positions for the head and hands. 
	You need also a blueprint with anim graph to use it
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class UArmAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	//The Pawn who own this class
	AArmAnimPawn * OwningPawn;

	//The Head Transform for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FTransform HeadWorldTransform;

	//The Left Hand World location for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector LeftHandWorldLocation;

	//The right Hand World location for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector RightHandWorldLocation;

	//The Left Hand World rotation for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator LeftHandWorldRotation;

	//The right Hand World rotation for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator RightHandWorldRotation;

	//Select if RayTrace should draw a line
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bDrawDebugLine = false;

	//The Delegater. It will be called when we get a collision
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
		FCollisionDelegate OnCollisionDelegate;

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	bool bIsCalled = false;


	/*
	Function to check if we hit something (collision)
	@param bIsRightHand, true if we want to check the right hand, false for the left hand
	@param CurrentPawn, The Pawn who own this class
	@param EffectorLocation, the End Location that is also used for IK
	@param DeltaTimeX, the Tick rate
	@return FVector, the EffectorLocation with collision for IK
	*/
	FVector CalculateOffset(bool bIsRightHand, AArmAnimPawn * CurrentPawn, FVector EffectorLocation, float DeltaTimeX);

	/*
	Function to claculate and set the positions/rotations for each arm/hand
	@param bIsRightHand, true if we want to check the right hand, false for the left hand
	@param HandWorldRotation, Rotation for this hand
	@param HandRotationOffset, the Offset for this hand
	@param CurrentMotionController, the motionController for the left or right hand
	@param DeltaTimeX, the tick rate
	@return FVector, the EffectorPosition for IK
	*/
	FVector CalculatePosition(bool bIsRightHand, FRotator HandWorldRotation, FRotator HandRotationOffset, UMotionControllerComponent* CurrentMotionController, float DeltaTimeX);

};
