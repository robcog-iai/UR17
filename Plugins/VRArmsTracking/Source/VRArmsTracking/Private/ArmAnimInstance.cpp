// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "ArmAnimInstance.h"
#include "Runtime/AnimGraphRuntime/Public/BoneControllers/AnimNode_Fabrik.h"
#include "Runtime/AnimGraphRuntime/Public/BoneControllers/AnimNode_TwoBoneIK.h"
#include "Runtime/AnimGraphRuntime/Public/BoneControllers/AnimNode_ModifyBone.h"
#include "Runtime/Engine/Classes/Engine/Engine.h "
#include "HeadMountedDisplayFunctionLibrary.h"

void UArmAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//Cache the owning pawn for use in Tick. Cast to own Pawn class
	OwningPawn = Cast<AArmAnimPawn>(TryGetPawnOwner());

	//Default Values for the head and hands
	HeadWorldTransform = FTransform(FRotator(0.0f,0.0f,90.0f),FVector(0.0f,0.0f,160.0f), FVector(1.0f,1.0f,1.0f));
	LeftHandWorldLocation = FVector(40.0f,20.0f,100.0f);
	RightHandWorldLocation = FVector(-40.0f, 20.0f, 100.0f);
    //moves the player to the cameras world position
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
}

void UArmAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	//Check it Pawn == null
	if (!OwningPawn)
	{
		return;
	}
	//Get current Positions from the Motioncontrollers and the HMD
	HeadWorldTransform = FTransform(OwningPawn->Camera->GetComponentRotation(), OwningPawn->Camera->GetComponentLocation(), FVector(1.0f,1.0f,1.0f));
	LeftHandWorldRotation = OwningPawn->MotionControllerLeft->GetComponentRotation();
	RightHandWorldRotation = OwningPawn->MotionControllerRight->GetComponentRotation();

	//Calculate the new Effector location for the left hand
	FVector SocketLocation = OwningPawn->Mesh->GetSocketLocation("hand_lSocket");
	FVector HandLocation = OwningPawn->Mesh->GetSocketLocation("hand_l");
	FVector MotionController = OwningPawn->MotionControllerLeft->GetComponentLocation();
	//FVector Result = (MotionController - (SocketLocation - HandLocation));
	LeftHandWorldLocation = MotionController;

	//Calculate the new Effector location for the right hand
	SocketLocation = OwningPawn->Mesh->GetSocketLocation("hand_rSocket");
	HandLocation = OwningPawn->Mesh->GetSocketLocation("hand_r");
	MotionController = OwningPawn->MotionControllerRight->GetComponentLocation();
	//Result = (MotionController - (SocketLocation - HandLocation));
	RightHandWorldLocation = MotionController;

}
